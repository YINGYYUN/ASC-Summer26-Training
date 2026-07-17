/*******************************************************************************
 * 赛道识别模块
 * 大津法（OTSU）二值化 + 最长白列种子 + 八邻域边界追踪
 *
 * 坐标系：image[0] = 远处（图像顶部），image[119] = 近处（图像底部）
 * 流程：Otsu阈值 → 最长白列找种子 → 底部白→黑黑扫边界 → 八邻域向上追边
 *******************************************************************************/

#include "TrackRecognition.h"

// ============================================================
// 可调参数
// ============================================================

#define ROW_STEP              (2    )   // 搜索行步长：每 N 行搜一次边界
#define IMG_W                 (MT9V03X_W)  // 188
#define IMG_H                 (MT9V03X_H)  // 120
#define IMG_CENTER            (IMG_W / 2)  // 94

// image[0] 为远处（图像顶部），image[IMG_H-1] 为近处（图像底部）
#define BOTTOM_ROW            (IMG_H - 1)  // 119，最近行

#define OTSU_COL_SAMPLE_STEP  (5)          // 最长白列搜索步长

// ============================================================
// 全局变量
// ============================================================

TrackResult_t g_track_result;

// ============================================================
// 内部状态变量
// ============================================================

static int16  s_prev_left[IMG_H];           // 上一帧左边界
static int16  s_prev_right[IMG_H];          // 上一帧右边界
static uint8  s_frame_count = 0;
static uint8  s_otsu_threshold = 128;        // 大津法阈值（EMA 平滑）
static uint8  s_otsu_last_valid = 128;        // 上一帧合法阈值（有效性过滤用）

// ============================================================
// 大津法求阈值（快速版，全图 1/4 采样）
// ============================================================
static uint8 otsu_find_threshold(void)
{
#define GrayScale 256
    int Pixel_Max = 0;
    int Pixel_Min = 255;
    static int pixelCount[GrayScale];        // 静态避免栈溢出
    int pixelSum = IMG_W * IMG_H / 4;           // 隔行列采样，像素总数 ≈ 1/4

    int i, j;
    uint8 threshold = 0;

    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
    }

    uint32 gray_sum = 0;
    // 隔行列采样建直方图
    for (i = 0; i < IMG_H; i += 2)
    {
        for (j = 0; j < IMG_W; j += 2)
        {
            uint8 p = mt9v03x_image[i][j];
            pixelCount[p]++;
            gray_sum += p;
            if (p > Pixel_Max) Pixel_Max = p;
            if (p < Pixel_Min) Pixel_Min = p;
        }
    }

    if (Pixel_Max <= Pixel_Min) return s_otsu_last_valid;

    // 类间方差迭代
    float w0 = 0, w1, u0tmp = 0, u0, u1, deltaTmp, deltaMax = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {
        float pj = (float)pixelCount[j] / (float)pixelSum;
        w0    += pj;
        u0tmp += j * pj;

        if (w0 == 0 || w0 >= 1.0f) continue;

        w1 = 1.0f - w0;
        if (w1 == 0) continue;

        u0 = u0tmp / w0;
        u1 = ((float)gray_sum / pixelSum - u0tmp) / w1;
        deltaTmp = w0 * w1 * (u0 - u1) * (u0 - u1);

        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = (uint8)j;
        }
        else if (deltaTmp < deltaMax)
        {
            break;  // 方差已过峰值，提前终止
        }
    }

    // 有效性过滤：合法范围 90~130，否则沿用上一帧
    if (threshold > 90 && threshold < 130)
        s_otsu_last_valid = threshold;
    else
        threshold = s_otsu_last_valid;

    return threshold;
}

// ============================================================
// 最长白列（基于 Otsu 阈值）
// 从底部(119)向上搜索，在中心 1/2 列宽范围找白色段最长的列
// ============================================================
static int16 otsu_longest_white_col(void)
{
    int16 longest_col  = IMG_CENTER;
    int16 longest_dist = 0;           // 白色段长度（行数）
    uint8 th = s_otsu_threshold;

    int16 col_start = IMG_W / 4;
    int16 col_end   = IMG_W * 3 / 4;

    for (int16 col = col_start; col <= col_end; col += OTSU_COL_SAMPLE_STEP)
    {
        for (int16 row = BOTTOM_ROW; row >= 0; row--)
        {
            if (mt9v03x_image[row][col] <= th)   // 碰到黑点
            {
                int16 dist = BOTTOM_ROW - row;    // 从底部到黑点的距离
                if (dist > longest_dist)
                {
                    longest_dist = dist;
                    longest_col  = col;
                }
                break;
            }
        }
    }
    return longest_col;
}

// ============================================================
// 方向向量数组（8 邻域，顺时针）
//  image row=0 在顶部（远处），row=IMG_H-1 在底部（近处）
//  因此"向上"是 row 减小，s_dy 上 = -1
//      0:上(-1) 1:右上(-1) 2:右(0)  3:右下(+1)
//      4:下(+1) 5:左下(+1) 6:左(0)  7:左上(-1)
// ============================================================
static const int8 s_dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
static const int8 s_dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};

// ============================================================
// 底部行：从种子向外扫描，白→黑黑才判定为边界
// ============================================================
static void otsu_bottom_boundary(int16 seed, int16 *p_left, int16 *p_right)
{
    int16 left  = 0;
    int16 right = IMG_W - 1;
    uint8 th = s_otsu_threshold;

    for (int16 col = seed; col >= 2; col--)
    {
        if (mt9v03x_image[BOTTOM_ROW][col - 1] <= th
            && mt9v03x_image[BOTTOM_ROW][col - 2] <= th)
        {
            left = col;          // 最后一个白点 = 左边界
            break;
        }
    }

    for (int16 col = seed; col < IMG_W - 2; col++)
    {
        if (mt9v03x_image[BOTTOM_ROW][col + 1] <= th
            && mt9v03x_image[BOTTOM_ROW][col + 2] <= th)
        {
            right = col;         // 最后一个白点 = 右边界
            break;
        }
    }

    *p_left  = left;
    *p_right = right;
}

// ============================================================
// 八邻域边界追踪：左边界顺时针搜索
// 从上一步方向逆时针 2 步开始，顺时针旋转搜下一个白点（左侧有黑）
// ============================================================
static uint8 trace_left_boundary(int16 *row, int16 *col, int16 *dir)
{
    uint8 th = s_otsu_threshold;
    int16 start = (*dir + 6) % 8;

    for (int16 k = 0; k < 8; k++)
    {
        int16 d  = (start + k) % 8;
        int16 nr = *row + s_dy[d];
        int16 nc = *col + s_dx[d];

        if (nr < 0 || nr >= IMG_H || nc < 1 || nc >= IMG_W) continue;

        if (nc > 0
            && mt9v03x_image[nr][nc] > th
            && mt9v03x_image[nr][nc - 1] <= th)
        {
            *row = nr;
            *col = nc;
            *dir = d;
            return 1;
        }
    }
    return 0;
}

// ============================================================
// 八邻域边界追踪：右边界逆时针搜索
// ============================================================
static uint8 trace_right_boundary(int16 *row, int16 *col, int16 *dir)
{
    uint8 th = s_otsu_threshold;
    int16 start = (*dir + 2) % 8;

    for (int16 k = 0; k < 8; k++)
    {
        int16 d  = (start + 8 - k) % 8;
        int16 nr = *row + s_dy[d];
        int16 nc = *col + s_dx[d];

        if (nr < 0 || nr >= IMG_H || nc < 0 || nc >= IMG_W - 1) continue;

        if (nc < IMG_W - 1
            && mt9v03x_image[nr][nc] > th
            && mt9v03x_image[nr][nc + 1] <= th)
        {
            *row = nr;
            *col = nc;
            *dir = d;
            return 1;
        }
    }
    return 0;
}

// ============================================================
// 边界搜索主入口
// ============================================================
static void search_boundaries(void)
{
    // 0) 重置全部行数据，防止上一帧残留
    g_track_result.valid_rows = 0;
    for (int16 i = 0; i < IMG_H; i++)
    {
        g_track_result.left_boundary[i]  = -1;
        g_track_result.right_boundary[i] = -1;
        g_track_result.center_line[i]    = IMG_CENTER;
    }

    // 1) 大津法阈值（EMA 平滑）
    uint8 raw_th = otsu_find_threshold();
    s_otsu_threshold = (uint8)(((uint16)s_otsu_threshold * 7 + (uint16)raw_th * 3) / 10);

    // 2) 最长白列找种子（从底部向上）
    int16 seed = otsu_longest_white_col();

    // 3) 底部行向外扫描确切边界（白→黑黑）
    int16 bot_l, bot_r;
    otsu_bottom_boundary(seed, &bot_l, &bot_r);

    g_track_result.left_boundary[BOTTOM_ROW]  = bot_l;
    g_track_result.right_boundary[BOTTOM_ROW] = bot_r;
    if (bot_l < bot_r)
    {
        g_track_result.center_line[BOTTOM_ROW] = (bot_l + bot_r) / 2;
        g_track_result.valid_rows++;
    }

    // 4) 八邻域向上追踪：从底部(119)追到顶部(0)
    // 向上 = row 减小，while 用 > 而非 <
    int16 lr = BOTTOM_ROW, lc = bot_l, ld = 6;   // 左边界初始方向 = 6 (左)
    int16 rr = BOTTOM_ROW, rc = bot_r, rd = 2;   // 右边界初始方向 = 2 (右)

    for (int16 row = BOTTOM_ROW - 1; row >= 0; row -= ROW_STEP)
    {
        g_track_result.left_boundary[row]  = -1;
        g_track_result.right_boundary[row] = -1;

        // 左边界追踪（向上：row 减小，lr > target）
        {
            int16 safety = 120;  // 安全上限 = IMG_H，防止死循环
            while (lr > row && safety-- > 0)
            {
                if (!trace_left_boundary(&lr, &lc, &ld)) { lc = -1; break; }
            }
            if (safety <= 0) lc = -1;
        }
        if (lr > row) lc = -1;
        g_track_result.left_boundary[row] = lc;

        // 右边界追踪
        {
            int16 safety = 120;
            while (rr > row && safety-- > 0)
            {
                if (!trace_right_boundary(&rr, &rc, &rd)) { rc = -1; break; }
            }
            if (safety <= 0) rc = -1;
        }
        if (rr > row) rc = -1;
        g_track_result.right_boundary[row] = rc;

        // 中线计算
        int16 l = g_track_result.left_boundary[row];
        int16 r = g_track_result.right_boundary[row];

        if (l >= 0 && r >= 0)
        {
            g_track_result.center_line[row] = (l + r) / 2;
            g_track_result.valid_rows++;
        }
        else if (l >= 0)
        {
            g_track_result.center_line[row] = l + 90;
        }
        else if (r >= 0)
        {
            g_track_result.center_line[row] = r - 90;
        }
    }
}

// ============================================================
// 计算转角值
// BOTTOM_ROW=119 是最近行（权重最高），row=0 是最远行
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

    for (int16 row = BOTTOM_ROW; row >= 0; row -= ROW_STEP)
    {
        if (g_track_result.left_boundary[row] >= 0
            && g_track_result.right_boundary[row] >= 0
            && g_track_result.center_line[row] >= 0
            && g_track_result.center_line[row] < IMG_W)
        {
            int16 deviation = (int16)g_track_result.center_line[row] - IMG_CENTER;
            // 底部行(row大)权重高，顶部行(row小)权重低
            float weight = 1.0f + (float)(row) / (float)IMG_H * 2.0f;
            total_deviation += deviation * weight;
            total_weight     += weight;
        }
    }

    g_track_result.steering_value = (total_weight > 0.0f)
        ? (total_deviation / total_weight) : 0.0f;
}

// ============================================================
// 保存当前帧边界
// ============================================================
static void save_boundary_for_next_frame(void)
{
    for (int16 row = BOTTOM_ROW; row >= 0; row -= ROW_STEP)
    {
        s_prev_left[row]  = g_track_result.left_boundary[row];
        s_prev_right[row] = g_track_result.right_boundary[row];
    }
}

// ============================================================
// 外部接口
// ============================================================

void TrackRecognition_Init(void)
{
    s_otsu_threshold   = 128;
    s_otsu_last_valid  = 128;
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

void TrackRecognition_Process(void)
{
    // search_boundaries 内部已重置 valid_rows 和边界数组
    search_boundaries();

    calc_steering_value();
    save_boundary_for_next_frame();
    s_frame_count++;
}

static void draw_row_overlay(int16 row, uint16 y_offset)
{
    int16 left  = g_track_result.left_boundary[row];
    int16 right = g_track_result.right_boundary[row];
    int16 center = g_track_result.center_line[row];
    uint16 y_screen = (uint16)row + y_offset;

    if (left >= 0 && left + 1 < IMG_W)
    {
        ips200_draw_point(left,     y_screen, RGB565_RED);
        ips200_draw_point(left + 1, y_screen, RGB565_RED);
    }

    if (right >= 0 && right + 1 < IMG_W)
    {
        ips200_draw_point(right,     y_screen, RGB565_BLUE);
        ips200_draw_point(right + 1, y_screen, RGB565_BLUE);
    }

    if (center >= 0 && center < IMG_W
        && left >= 0 && right >= 0)
    {
        ips200_draw_point(center, y_screen, RGB565_GREEN);
    }
}

void TrackRecognition_DrawOverlay(uint16 y_offset)
{
    // image[0] 显示在屏幕 y=0+y_offset，image[119] 在 y=119+y_offset
    // 遍历顺序与 search_boundaries 填充行一致：
    // BOTTOM_ROW(119) + BOTTOM_ROW-1 向下以 ROW_STEP 递减

    draw_row_overlay(BOTTOM_ROW, y_offset);

    for (int16 row = BOTTOM_ROW - 1; row >= 0; row -= ROW_STEP)
    {
        draw_row_overlay(row, y_offset);
    }
}
