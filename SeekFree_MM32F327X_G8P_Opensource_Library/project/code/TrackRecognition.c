/*******************************************************************************
 * 赛道识别模块
 * 基于对比度算法（差比和）的智能车赛道边界搜索与中线提取
 *
 * 算法参考：逐飞科技推文 — 摄像头寻迹算法
 * 核心原理：abs(a-b)*100/(a+b) 计算对比度，避免二值化光线敏感问题
 *******************************************************************************/

#include "TrackRecognition.h"

// ============================================================
// 可调参数
// ============================================================

#define RATIO_THRESHOLD       (18   )   // 差比和阈值：大于此值认为是黑白跳变 范围建议 15-25
#define GAP                   (5    )   // 比较点间距：隔 N 个像素做差比和，避免渐变边界漏检
#define ROW_STEP              (2    )   // 搜索行步长：每 N 行搜一次边界，减少计算量
#define COL_SAMPLE_STEP       (5    )   // 最长白列搜索的列采样步长
#define EDGE_TRACK_OFFSET     (10   )   // 边缘跟踪时在上次边界±此范围内搜索

#define IMG_W                 (MT9V03X_W)  // 188
#define IMG_H                 (MT9V03X_H)  // 120
#define IMG_CENTER            (IMG_W / 2)  // 图像中心列 = 94

#define BOTTOM_ROW            0             // image[0][x] 为图像最近行（底部）

// ============================================================
// 全局变量
// ============================================================

TrackResult_t g_track_result;

// ============================================================
// 内部状态变量
// ============================================================

static uint8  s_ref_white = 128;            // 参考白点灰度值
static int16  s_prev_left[IMG_H];           // 上一帧左边界（边缘跟踪用）
static int16  s_prev_right[IMG_H];          // 上一帧右边界
static uint8  s_frame_count = 0;

// ============================================================
// 工具函数
// ============================================================

// 差比和计算：abs(a-b)*100/(a+b)，返回 0-100
static int16 calc_ratio(uint8 a, uint8 b)
{
    int16 sum = (int16)a + b;
    if (sum == 0) return 0;
    int16 diff = (a > b) ? (a - b) : (b - a);
    return (diff * 100) / sum;
}

// 判断是否接近参考白点（容差范围内）
static uint8 is_white(uint8 pixel)
{
    return (pixel >= s_ref_white - 25);
}

// ============================================================
// 查找参考白点
// 在图像底部中心区域采样，取较亮的像素平均作为白点参考
// 使用 EMA 平滑避免光照突变导致参考值剧烈抖动
// ============================================================
static void find_ref_white(void)
{
    uint32 sum = 0;
    uint16 count = 0;
    int16  start_col = IMG_CENTER - 30;
    int16  end_col   = IMG_CENTER + 30;

    // 约束范围
    if (start_col < 0)  start_col = 0;
    if (end_col >= IMG_W) end_col = IMG_W - 1;

    // 采样靠近底部的行（row 0~9），过滤掉过暗像素避免干扰
    for (int16 row = BOTTOM_ROW; row < BOTTOM_ROW + 10 && row < IMG_H; row++)
    {
        for (int16 col = start_col; col <= end_col; col += 3)
        {
            uint8 pixel = mt9v03x_image[row][col];
            if (pixel > 60)                     // 排除明显的黑点
            {
                sum += pixel;
                count++;
            }
        }
    }

    if (count > 20)
    {
        uint8 new_white = (uint8)(sum / count);
        // EMA 平滑：30% 新值 + 70% 旧值，避免单帧异常引起突变
        s_ref_white = (uint8)(((uint16)s_ref_white * 7 + (uint16)new_white * 3) / 10);
    }
}

// ============================================================
// 查找最长白列
// 仅在图像中心 1/2 区域采样（车始终在赛道中，边缘无需搜索）
// 用差比和从下往上找第一个黑点
// 找到黑点后判断白色长度 = 黑点所在行 - BOTTOM_ROW
// 返回白色最长的那一列
// ============================================================
static int16 find_longest_white_col(void)
{
    int16 longest_col  = IMG_CENTER;
    int16 longest_dist = 0;
    uint8 search_a = s_ref_white;             // 参考白点作为 a

    // 最长白线搜索起止位置（图像中心 1/2 宽度区域）
    int16 col_start = IMG_W / 4;              // 左 1/4 处
    int16 col_end   = IMG_W * 3 / 4;          // 右 3/4 处

    for (int16 col = col_start; col <= col_end; col += COL_SAMPLE_STEP)
    {
        for (int16 row = BOTTOM_ROW; row < IMG_H; row++)
        {
            uint8 pixel_b = mt9v03x_image[row][col];
            int16 ratio = calc_ratio(search_a, pixel_b);

            if (ratio > RATIO_THRESHOLD)      // 找到黑点
            {
                int16 dist = row - BOTTOM_ROW;
                if (dist > longest_dist)
                {
                    longest_dist = dist;
                    longest_col  = col;
                }
                break;
            }

            // 底部前几行未必是白，跳过
            if (row > IMG_H - 5) break;
        }
    }
    return longest_col;
}

// ============================================================
// 在指定行搜索左/右边界（使用差比和 + 间隔比较）
// row: 当前行
// start_col: 搜索起点
// direction: -1 向左搜索，+1 向右搜索
// 返回边界列坐标，-1 表示未找到
// ============================================================
static int16 search_boundary(int16 row, int16 start_col, int8 direction)
{
    int16 a_idx = start_col;
    int16 b_idx = start_col + direction * GAP;

    // 循环直到找到边界或到达图像边缘
    while (1)
    {
        // 越界检查
        if (b_idx < 0 || b_idx >= IMG_W)
        {
            // 最后一次机会：检查边缘点本身
            b_idx = (direction < 0) ? 0 : (IMG_W - 1);
            if (b_idx == a_idx) break;

            uint8 pixel_a = mt9v03x_image[row][a_idx];
            uint8 pixel_b = mt9v03x_image[row][b_idx];
            if (calc_ratio(pixel_a, pixel_b) > RATIO_THRESHOLD)
            {
                return b_idx;
            }
            break;
        }

        uint8 pixel_a = mt9v03x_image[row][a_idx];
        uint8 pixel_b = mt9v03x_image[row][b_idx];

        if (calc_ratio(pixel_a, pixel_b) > RATIO_THRESHOLD)
        {
            return b_idx;                     // 找到边界
        }

        // 步进：a = b, b = b + GAP*direction
        a_idx = b_idx;
        b_idx += direction * GAP;
    }

    return -1;                                // 未找到边界
}

// ============================================================
// 查找白带区间
// 扫描中心 1/2 宽度区域，找出所有"整列全白"（底部到顶部均为白）的列
// 返回最大连续区间作为白带左右边缘
// 若无整列全白的列，band_left == band_right == IMG_CENTER（退化回单起点）
// ============================================================
static void find_white_band(int16 *band_left, int16 *band_right)
{
    int16 col_start = IMG_W / 4;
    int16 col_end   = IMG_W * 3 / 4;
    int16 best_l    = IMG_CENTER;
    int16 best_r    = IMG_CENTER;
    int16 best_len  = 0;

    int16 cur_l     = -1;
    int16 cur_len   = 0;

    for (int16 col = col_start; col <= col_end; col += COL_SAMPLE_STEP)
    {
        uint8 is_all_white = 1;

        // 检查该列从底部到顶部是否全白
        for (int16 row = BOTTOM_ROW; row < IMG_H; row += ROW_STEP)
        {
            if (!is_white(mt9v03x_image[row][col]))
            {
                is_all_white = 0;
                break;
            }
        }

        if (is_all_white)
        {
            if (cur_l < 0)
            {
                cur_l   = col;              // 新区间起点
                cur_len = 1;
            }
            else
            {
                cur_len++;                  // 延续当前区间
            }
        }
        else
        {
            if (cur_len > best_len)
            {
                best_l   = cur_l;
                best_r   = col - COL_SAMPLE_STEP;  // 上一个采样列
                best_len = cur_len;
            }
            cur_l   = -1;
            cur_len = 0;
        }
    }

    // 收尾：处理最后一个区间
    if (cur_len > best_len)
    {
        best_l   = cur_l;
        best_r   = cur_l + (int16)(cur_len - 1) * COL_SAMPLE_STEP;
        best_len = cur_len;
    }

    *band_left  = best_l;
    *band_right = best_r;
}

// ============================================================
// 边界搜索入口
// band_left/band_right: 白带左右边缘，相等时退化为单一搜索起点
// 对每一行使用边缘跟踪策略搜索左右边界
// ============================================================
static void search_all_boundaries(int16 band_left, int16 band_right)
{
    int16 prev_left  = band_left;
    int16 prev_right = band_right;

    for (int16 row = BOTTOM_ROW; row < IMG_H; row += ROW_STEP)
    {
        g_track_result.left_boundary[row]  = -1;
        g_track_result.right_boundary[row] = -1;

        // 边缘跟踪：从上一次边界附近开始搜索
        if (s_frame_count > 0 && s_prev_left[row] > 0)
        {
            int16 search_start = s_prev_left[row] + EDGE_TRACK_OFFSET;
            if (search_start >= IMG_W) search_start = IMG_W - 2;
            prev_left = search_start;
        }
        else if (s_frame_count > 0 && s_prev_right[row] > 0 && prev_left < 0)
        {
            // 用右边界估算左边界
            prev_left = s_prev_right[row] - 120;
            if (prev_left < 0) prev_left = 5;
        }

        // 搜索左边界
        if (prev_left >= 0 && prev_left < IMG_W)
        {
            int16 left = search_boundary(row, prev_left, -1);
            if (left >= 0)
            {
                g_track_result.left_boundary[row] = left;
                prev_left = left;
            }
        }

        // 边缘跟踪：从上一次右边界附近开始
        if (s_frame_count > 0 && s_prev_right[row] > 0)
        {
            int16 search_start = s_prev_right[row] - EDGE_TRACK_OFFSET;
            if (search_start < 0) search_start = 2;
            prev_right = search_start;
        }
        else if (s_frame_count > 0 && s_prev_left[row] > 0 && prev_right < 0)
        {
            prev_right = s_prev_left[row] + 120;
            if (prev_right >= IMG_W) prev_right = IMG_W - 5;
        }

        // 搜索右边界
        if (prev_right >= 0 && prev_right < IMG_W)
        {
            int16 right = search_boundary(row, prev_right, 1);
            if (right >= 0 && right < IMG_W)
            {
                g_track_result.right_boundary[row] = right;
                prev_right = right;
            }
        }

        // 若左右都有效则更新中线；若只有一边有效，用另一边估算
        if (g_track_result.left_boundary[row] >= 0
            && g_track_result.right_boundary[row] >= 0)
        {
            g_track_result.center_line[row] =
                (g_track_result.left_boundary[row] + g_track_result.right_boundary[row]) / 2;
            g_track_result.valid_rows++;
        }
        else if (g_track_result.left_boundary[row] >= 0)
        {
            g_track_result.center_line[row] = g_track_result.left_boundary[row] + 90;
        }
        else if (g_track_result.right_boundary[row] >= 0)
        {
            g_track_result.center_line[row] = g_track_result.right_boundary[row] - 90;
        }
    }
}

// ============================================================
// 计算转角值
// 将每行中线与图像中心的偏差累加，弯道越大偏差累计越大
// 靠近底部的行权重更高（近处更重要）
// ============================================================
static void calc_steering_value(void)
{
    if (g_track_result.valid_rows == 0)
    {
        g_track_result.steering_value = 0.0f;
        return;
    }

    float total_deviation = 0.0f;
    float total_weight    = 0.0f;

    for (int16 row = BOTTOM_ROW; row < IMG_H; row += ROW_STEP)
    {
        if (g_track_result.left_boundary[row] >= 0
            && g_track_result.right_boundary[row] >= 0)
        {
            int16 deviation = (int16)g_track_result.center_line[row] - IMG_CENTER;
            float weight = 1.0f + (float)(IMG_H - row) / (float)IMG_H * 2.0f;
            // weight: 底部权重大（近处更重要），顶部权重小
            total_deviation += deviation * weight;
            total_weight     += weight;
        }
    }

    if (total_weight > 0.0f)
    {
        g_track_result.steering_value = total_deviation / total_weight;
    }
    else
    {
        g_track_result.steering_value = 0.0f;
    }
}

// ============================================================
// 保存当前帧边界供下一帧边缘跟踪
// ============================================================
static void save_boundary_for_next_frame(void)
{
    for (int16 row = BOTTOM_ROW; row < IMG_H; row += ROW_STEP)
    {
        s_prev_left[row]  = g_track_result.left_boundary[row];
        s_prev_right[row] = g_track_result.right_boundary[row];
    }
}

// ============================================================
// 外部接口
// ============================================================

// 初始化赛道识别模块
void TrackRecognition_Init(void)
{
    s_ref_white   = 128;
    s_frame_count = 0;

    for (int16 i = 0; i < IMG_H; i++)
    {
        s_prev_left[i]        = -1;
        s_prev_right[i]       = -1;
        g_track_result.left_boundary[i]  = -1;
        g_track_result.right_boundary[i] = -1;
        g_track_result.center_line[i]    = IMG_CENTER;
    }
    g_track_result.steering_value = 0.0f;
    g_track_result.valid_rows     = 0;
}

// 赛道识别主处理
// 每次新帧到来时调用一次
void TrackRecognition_Process(void)
{
    // 步骤 1：查找参考白点
    find_ref_white();

    // 步骤 2：查找白带区间（整列全白的连续区域）
    int16 band_left, band_right;
    find_white_band(&band_left, &band_right);

    // 若无整列全白列，退化回最长白列作为单一搜索起点
    if (band_left == band_right)
    {
        band_left = band_right = find_longest_white_col();
    }

    // 步骤 3：搜索全部行的左右边界
    g_track_result.valid_rows = 0;
    search_all_boundaries(band_left, band_right);

    // 步骤 4：计算转角值
    calc_steering_value();

    // 步骤 5：保存本帧结果供下一帧边缘跟踪
    save_boundary_for_next_frame();

    s_frame_count++;
}

// ============================================================
// 在屏幕上叠加绘制赛道边界和中线（供调试显示）
// 左边界红色、右边界蓝色、中线绿色
// 注意：此函数必须在 ips200_displayimage03x 之后调用
// ============================================================
void TrackRecognition_DrawOverlay(void)
{
    for (int16 row = BOTTOM_ROW; row < IMG_H; row += ROW_STEP)
    {
        int16 left  = g_track_result.left_boundary[row];
        int16 right = g_track_result.right_boundary[row];
        int16 center = g_track_result.center_line[row];

        // 画左边界（红色，2像素宽）
        if (left >= 0 && left < IMG_W)
        {
            ips200_draw_point(left,     row, RGB565_RED);
            ips200_draw_point(left + 1, row, RGB565_RED);
        }

        // 画右边界（蓝色，2像素宽）
        if (right >= 0 && right < IMG_W)
        {
            ips200_draw_point(right,     row, RGB565_BLUE);
            ips200_draw_point(right + 1, row, RGB565_BLUE);
        }

        // 画中线（绿色，仅左右边界都有效时）
        if (center >= 0 && center < IMG_W
            && left >= 0 && right >= 0)
        {
            ips200_draw_point(center, row, RGB565_GREEN);
        }
    }
}
