/*******************************************************************************
 * 赛道识别模块 — 逐行扫描版
 *
 * 坐标系：image[0]=远处(顶)，image[119]=近处(底)
 *******************************************************************************/

#include "TrackRecognition.h"

// ============================================================
// 可调参数
// ============================================================

#define IMG_W                 (MT9V03X_W)
#define IMG_H                 (MT9V03X_H)
#define IMG_CENTER            (IMG_W / 2)
#define BOTTOM_ROW            (IMG_H - 1)

#define THRESHOLD_MODE_FIXED  1   // 1:固定阈值  0:OTSU 自适应
#define FIXED_THRESHOLD       240

// ============================================================
// 全局 & 内部状态
// ============================================================

TrackResult_t g_track_result;

// 二值化判定阈值
static uint8  s_otsu_threshold = 240;

// ============================================================
// 黑框
// ============================================================
static void draw_image_black_border(void)
{
    uint8 i;
    for (i = 0; i < IMG_H; i++)
    {
        mt9v03x_image[i][0]           = 0;
        mt9v03x_image[i][1]           = 0;
        mt9v03x_image[i][IMG_W - 1]   = 0;
        mt9v03x_image[i][IMG_W - 2]   = 0;
    }
    for (i = 0; i < IMG_W; i++)
    {
        mt9v03x_image[0][i] = 0;
        mt9v03x_image[1][i] = 0;
    }
}

// ============================================================
// 阈值（固定 / OTSU 可切换）
// ============================================================
#if !THRESHOLD_MODE_FIXED

#define GrayScale 256

static uint8 s_otsu_last_valid = 128;

static uint8 otsu_find_threshold(void)
{
    int Pixel_Max = 0, Pixel_Min = 255;
    static int pixelCount[GrayScale];
    int pixelSum = IMG_W * IMG_H / 4;
    int i, j; uint8 threshold = 0;

    for (i = 0; i < GrayScale; i++) pixelCount[i] = 0;

    uint32 gray_sum = 0;
    for (i = 0; i < IMG_H; i += 2)
        for (j = 0; j < IMG_W; j += 2)
        {
            uint8 p = mt9v03x_image[i][j];
            pixelCount[p]++; gray_sum += p;
            if (p > Pixel_Max) Pixel_Max = p;
            if (p < Pixel_Min) Pixel_Min = p;
        }

    if (Pixel_Max <= Pixel_Min) return s_otsu_last_valid;

    float w0 = 0, w1, u0tmp = 0, u0, u1, deltaTmp, deltaMax = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {
        float pj = (float)pixelCount[j] / (float)pixelSum;
        w0 += pj; u0tmp += j * pj;
        if (w0 == 0 || w0 >= 1.0f) continue;
        w1 = 1.0f - w0;
        if (w1 == 0) continue;
        u0 = u0tmp / w0;
        u1 = ((float)gray_sum / pixelSum - u0tmp) / w1;
        deltaTmp = w0 * w1 * (u0 - u1) * (u0 - u1);
        if (deltaTmp > deltaMax) { deltaMax = deltaTmp; threshold = (uint8)j; }
        else if (deltaTmp < deltaMax) break;
    }
    if (threshold > 90 && threshold < 250) s_otsu_last_valid = threshold;
    else threshold = s_otsu_last_valid;
    return threshold;
}

#endif // !THRESHOLD_MODE_FIXED

// ============================================================
// 出界判定
// 扫描底行中心是否全黑
// ============================================================

// 底部黑点比例检测 — 只扫描中心区域
// 参数对应：3/8*W=70, 5/8*W=117, 底部5行=第115~119行
#define LOSE_TRACK_COL_LEFT   		70
#define LOSE_TRACK_COL_RIGHT  		117
#define LOSE_TRACK_ROW_START  		115           // 从第115行开始(近处)
#define LOSE_TRACK_FRAME_CNT    	3           // 连续 CNT 帧触发才判定出界

#define LOSE_TRACK_CHECK_TOTAL      ((BOTTOM_ROW - LOSE_TRACK_ROW_START + 1)  * (LOSE_TRACK_COL_RIGHT - LOSE_TRACK_COL_LEFT + 1)) 

// 返回值: 0=正常  1=出界
uint8 Check_LoseTrack(void)
{
    uint32 black_cnt = 0;
    static uint8 lose_cnt = 0;

    for (int16 row = LOSE_TRACK_ROW_START; row <= BOTTOM_ROW; row++)
    {
        for (int16 col = LOSE_TRACK_COL_LEFT; col <= LOSE_TRACK_COL_RIGHT; col++)
        {
            if (mt9v03x_image[row][col] <= TrackRecognition_GetThreshold())
                black_cnt++;
        }
    }

    float ratio = (float)black_cnt / (float)LOSE_TRACK_CHECK_TOTAL;

    if (ratio > 0.80f)
    {
        if (++lose_cnt >= LOSE_TRACK_FRAME_CNT)
            return 1;
    }
    else
    {
        lose_cnt = 0;
    }

    return 0;
}

// ============================================================
// 斑马线判定
// 采样,统计黑白跳变的次数
// ============================================================

// 斑马线检测 — 状态机 + 底行 WBB 模式计数，防止同一片斑马线被多次统计
// 返回: 0=无斑马线  1=首次遇到(本次斑马线的第一帧)  2=仍在斑马线中
#define ZEBRA_CHECK_ROWS       3      // 检测底部 3 行   
#define ZEBRA_GUARD_OFFSET     30     // 距底 30 行处须双边存在


#define ZEBRA_STATE_IDLE      0
#define ZEBRA_STATE_ENTER     1
#define ZEBRA_STATE_INSIDE    2

uint8 Check_Zebra(void)
{
    uint8 threshold = TrackRecognition_GetThreshold();

    // ---- 基本条件：距底 30 行处双边存在 ----
    int16 guard_row = BOTTOM_ROW - ZEBRA_GUARD_OFFSET;  // row 89
    if (!(g_track_result.left_boundary[guard_row] >= 0
       && g_track_result.right_boundary[guard_row] >= 0
       && g_track_result.left_boundary[guard_row] < g_track_result.right_boundary[guard_row]))
    {
        return 0;
    }

    // ---- WBB 模式检测：底部 3 行 ----
    uint8 zebra_now = 0;

    for (uint8 i = 0; i < ZEBRA_CHECK_ROWS; i++)
    {
        int16 row = BOTTOM_ROW - i;     // 行 119, 118, 117
        uint8 wbb_count = 0;

        for (int16 col = 0; col < IMG_W - 2; col++)
        {
            // 白黑黑模式
            if (mt9v03x_image[row][col] > threshold
             && mt9v03x_image[row][col + 1] <= threshold
             && mt9v03x_image[row][col + 2] <= threshold)
            {
                wbb_count++;
            }
        }
        // 单行 WBB 模式 ≥ 4 → 斑马线
        if (wbb_count >= 4)
        { zebra_now = 1; break; }
    }

    // ---- 状态机 ----
    static uint8 s_zebra_state = ZEBRA_STATE_IDLE;

    switch (s_zebra_state)
    {
        case ZEBRA_STATE_IDLE:
            if (zebra_now)
            { s_zebra_state = ZEBRA_STATE_ENTER; return 1; }
            return 0;

        case ZEBRA_STATE_ENTER:
            if (zebra_now)
            { s_zebra_state = ZEBRA_STATE_INSIDE; return 2; }
            s_zebra_state = ZEBRA_STATE_IDLE;
            return 0;

        case ZEBRA_STATE_INSIDE:
            if (zebra_now) return 2;
            s_zebra_state = ZEBRA_STATE_IDLE;
            return 0;

        default:
            s_zebra_state = ZEBRA_STATE_IDLE;
            return 0;
    }
}

// ============================================================
// 最长白列（从底部向上，中心区域找白色段最长的列）
// ============================================================
#define OTSU_COL_SAMPLE_STEP  5
#define HALF_WIDTH_FALLBACK   40   // 赛道半宽 (全宽=80)
#define DRAW_YS_MAX           271
#define DRAW_WIDTH            1    // 边界绘制宽度 1~3

static int16 otsu_longest_white_col(void)
{
    int16 longest_col = IMG_CENTER, longest_dist = 0;
    uint8 th = s_otsu_threshold;
    for (int16 col = IMG_W / 4; col <= IMG_W * 3 / 4; col += OTSU_COL_SAMPLE_STEP)
        for (int16 row = BOTTOM_ROW; row >= 0; row--)
            if (mt9v03x_image[row][col] <= th)
            { int16 d = BOTTOM_ROW - row; if (d > longest_dist) { longest_dist = d; longest_col = col; } break; }
    return longest_col;
}

// ============================================================
// 逐行扫描：底行从种子起扫，上行从上一行边界偏移起扫
// ============================================================
#define SWEEP_OFFSET  8   // 上行搜索起点向内偏移量
#define SWEEP_MAX_ROWS 90   // 只扫近处行数，row 30~119

static void sweep_boundaries(void)
{
    uint8 th = s_otsu_threshold;
    static int16 s_last_valid_width = HALF_WIDTH_FALLBACK * 2;  // 兜底：未记录到有效宽度时使用

    g_track_result.valid_rows = 0;

    int16 sweep_end_row = IMG_H - SWEEP_MAX_ROWS;  // row 30, 只扫到这里
    if (sweep_end_row < 0) sweep_end_row = 0;

    // 最长白列种子
    int16 seed = otsu_longest_white_col();

    // ---- 底行：从种子向左右扫（白→黑） ----
    int16 l = seed;
    while (l > 2 && mt9v03x_image[BOTTOM_ROW][l - 1] > th) l--;
    int16 r = seed;
    while (r < IMG_W - 3 && mt9v03x_image[BOTTOM_ROW][r + 1] > th) r++;

    // 扫到图像边缘视为丢线
    g_track_result.left_boundary[BOTTOM_ROW]  = (l <= 2) ? -1 : l;
    g_track_result.right_boundary[BOTTOM_ROW] = (r >= IMG_W - 3) ? -1 : r;

    if (l >= 0 && r >= 0 && l < r)
    {
        g_track_result.center_line[BOTTOM_ROW] = (l + r) / 2;
        g_track_result.valid_rows++;
        s_last_valid_width = r - l;
    }

    int16 prev_l = l, prev_r = r;
    uint8 lost_consecutive = 0;  // 连续双边都丢的行数
    int16 clean_from = sweep_end_row;  // sweep_end_row 以上行未扫描，需清除上帧残留

    // ---- 上行：从上一行边界 + 偏移开始，白黑黑判定 ----
    for (int16 row = BOTTOM_ROW - 1; row >= sweep_end_row; row--)
    {
        // 左边界：从 prev_l + OFFSET（向内移）向左扫
        l = prev_l + SWEEP_OFFSET;
        if (l > IMG_W - 4) l = IMG_W - 4;
        if (mt9v03x_image[row][l] > th)
        {
            while (l > 2
                && !(mt9v03x_image[row][l - 1] <= th
                  && mt9v03x_image[row][l - 2] <= th))
                l--;
        }
        else
        {
            l = seed;
            while (l > 2
                && !(mt9v03x_image[row][l - 1] <= th
                  && mt9v03x_image[row][l - 2] <= th))
                l--;
        }

        // 右边界：从 prev_r - OFFSET（向内移）向右扫
        r = prev_r - SWEEP_OFFSET;
        if (r < 2) r = 2;
        if (mt9v03x_image[row][r] > th)
        {
            while (r < IMG_W - 3
                && !(mt9v03x_image[row][r + 1] <= th
                  && mt9v03x_image[row][r + 2] <= th))
                r++;
        }
        else
        {
            r = seed;
            while (r < IMG_W - 3
                && !(mt9v03x_image[row][r + 1] <= th
                  && mt9v03x_image[row][r + 2] <= th))
                r++;
        }

        // 扫到图像边缘视为丢线
        if (l <= 2) l = -1;
        if (r >= IMG_W - 3) r = -1;

        g_track_result.left_boundary[row]  = l;
        g_track_result.right_boundary[row] = r;

        if (l >= 0 && r >= 0 && l < r)
        {
            g_track_result.center_line[row] = (l + r) / 2;
            g_track_result.valid_rows++;
            s_last_valid_width = r - l;   // 记住最近有效路宽
            prev_l = l;
            prev_r = r;
        }
        else if (l >= 0)
        {
            g_track_result.center_line[row] = l + s_last_valid_width / 2;
        }
        else if (r >= 0)
        {
            g_track_result.center_line[row] = r - s_last_valid_width / 2;
        }
        else
        {
            // 双边都丢：清除上帧残留的中线，绘制时不显示
            g_track_result.center_line[row] = IMG_CENTER;
        }

        // 双边都丢：先区分是"真黑区"还是"全白无边缘"(如十字)
        // 采样中心区域，存在白像素则不视为黑区
        if (l < 0 && r < 0)
        {
            prev_l = seed;
            prev_r = seed;

            uint8 is_black_zone = 1;  // 默认是黑区
            for (int16 chk = IMG_W / 4; chk <= IMG_W * 3 / 4; chk += OTSU_COL_SAMPLE_STEP)
            {
                if (mt9v03x_image[row][chk] > th)
                { is_black_zone = 0; break; }  // 有白像素 → 不是黑区
            }

            if (is_black_zone)
            {
                if (++lost_consecutive >= 5)   // 连续 5 行真黑区，停止向上扫
                { clean_from = row; break; }
            }
            else
            {
                lost_consecutive = 0;
            }
        }
        else
        {
            lost_consecutive = 0;
        }
    }

    // 清除 break 后未扫描行的上帧残留数据（否则绘制时会显示旧边线）
    if (clean_from > 0)
    {
        for (int16 i = clean_from - 1; i >= 0; i--)
        {
            g_track_result.left_boundary[i]  = -1;
            g_track_result.right_boundary[i] = -1;
            g_track_result.center_line[i]    = IMG_CENTER;
        }
    }

    // 丢线统计
    g_track_result.left_lost_from  = -1;
    g_track_result.right_lost_from = -1;
    for (int16 row = BOTTOM_ROW; row >= 0; row--)
    {
        if (g_track_result.left_lost_from  < 0 && g_track_result.left_boundary[row]  < 0)
            g_track_result.left_lost_from  = row;
        if (g_track_result.right_lost_from < 0 && g_track_result.right_boundary[row] < 0)
            g_track_result.right_lost_from = row;
    }
    g_track_result.left_lost_count  = (g_track_result.left_lost_from  >= 0)
        ? (uint16)(BOTTOM_ROW - g_track_result.left_lost_from)  : 0;
    g_track_result.right_lost_count = (g_track_result.right_lost_from >= 0)
        ? (uint16)(BOTTOM_ROW - g_track_result.right_lost_from) : 0;
}

// ============================================================
// 转角计算（加权平均中线偏差，单位：像素）
// 只使用图像近处 90 行（row 30~119），远处噪声大不参与计算
// steering_value = Σ(每行中线偏离图像中心距离 × 权重) / Σ权重
// 结果：0=直道  >0=右弯  <0=左弯  典型值 0~40
// ============================================================
#define STEER_NEAR_ROWS  90   // 只取近处行数
// 权重：近处(底) → 远处(行30)，本次调为远重近轻以提前转向
#define STEER_W_NEAR   2.0f    // 起点(最底)权重
#define STEER_W_FAR    2.8f    // 远端(row30)权重

static void calc_steering_value(void)
{
    int16 end_row = IMG_H - STEER_NEAR_ROWS;  // 只算 row >= end_row 的行
    if (end_row < 0) end_row = 0;

    // 找到"近处范围内"底部第一个存在左或右边界的行作为起点
    int16 start_row = -1;
    for (int16 row = BOTTOM_ROW; row >= end_row; row--)
    {
        if (g_track_result.left_boundary[row] >= 0
            || g_track_result.right_boundary[row] >= 0)
        {
            start_row = row;
            break;
        }
    }

    // 有效性检验
    if (start_row < 0 || start_row <= end_row)
    {
        g_track_result.steering_value = 0.0f;
        return;
    }

    // 从近处向远处递增权重
    float step = (STEER_W_FAR - STEER_W_NEAR) / (float)(start_row - end_row);
    float w = STEER_W_NEAR;
    float total_dev = 0.0f, total_w = 0.0f;

    for (int16 row = start_row; row >= end_row; row--)
    {
        if (g_track_result.center_line[row] >= 0
            && g_track_result.center_line[row] < IMG_W
            && (g_track_result.left_boundary[row] >= 0
                || g_track_result.right_boundary[row] >= 0))
        {
            int16 dev = (int16)g_track_result.center_line[row] - IMG_CENTER;
            total_dev += dev * w;
            total_w   += w;
        }
        w += step; // 下一行(更远处)权重递增
        if (w > STEER_W_FAR) w = STEER_W_FAR;
    }
    g_track_result.steering_value = (total_w > 0.0f) ? (total_dev / total_w) : 0.0f;
}

// ============================================================
// 绘制单行边线
// ============================================================
static void draw_row_overlay(int16 row, uint16 y_offset)
{
    int16 left  = g_track_result.left_boundary[row];
    int16 right = g_track_result.right_boundary[row];
    int16 center = g_track_result.center_line[row];
    uint16 ys = (uint16)row + y_offset;
    uint8 draw2 = (ys + 1 < DRAW_YS_MAX);

#if DRAW_WIDTH == 1
    if (left >= 0 && left < IMG_W)
        ips200_draw_point(left, ys, RGB565_RED);
    if (right >= 0 && right < IMG_W)
        ips200_draw_point(right, ys, RGB565_BLUE);
    if (center >= 0 && center < IMG_W && (left >= 0 || right >= 0))
        ips200_draw_point(center, ys, RGB565_GREEN);
#else
    if (left >= 0 && left + DRAW_WIDTH - 1 < IMG_W)
    {
        for (int16 w = 0; w < DRAW_WIDTH; w++)
        {
            ips200_draw_point(left + w, ys, RGB565_RED);
            if (draw2) ips200_draw_point(left + w, ys + 1, RGB565_RED);
        }
    }
    if (right >= 0 && right + DRAW_WIDTH - 1 < IMG_W)
    {
        for (int16 w = 0; w < DRAW_WIDTH; w++)
        {
            ips200_draw_point(right + w, ys, RGB565_BLUE);
            if (draw2) ips200_draw_point(right + w, ys + 1, RGB565_BLUE);
        }
    }
    if (center >= DRAW_WIDTH / 2 && center + DRAW_WIDTH / 2 < IMG_W
        && (left >= 0 || right >= 0))
    {
        for (int16 w = -DRAW_WIDTH / 2; w <= DRAW_WIDTH / 2; w++)
            ips200_draw_point(center + w, ys, RGB565_GREEN);
    }
#endif
}

// ============================================================
// 外部接口
// ============================================================

void TrackRecognition_Init(void)
{
    // 初始化二值化阈值
#if THRESHOLD_MODE_FIXED
    s_otsu_threshold = FIXED_THRESHOLD;
#else
    s_otsu_threshold = 220;
    s_otsu_last_valid = 220;
#endif

    // 重置赛道识别结果
    TrackResult_t *p = &g_track_result;
    for (int16 i = 0; i < IMG_H; i++)
    {
        p->left_boundary[i]  = -1;
        p->right_boundary[i] = -1;
        p->center_line[i]    = IMG_CENTER;
    }
    p->steering_value = 0.0f;
    p->valid_rows     = 0;
    p->left_lost_from  = -1; p->right_lost_from  = -1;
    p->left_lost_count = 0;  p->right_lost_count = 0;
}

void TrackRecognition_Process(void)
{
    // 绘制黑框
    draw_image_black_border();

    // 获取当前二值化阈值
#if THRESHOLD_MODE_FIXED
    s_otsu_threshold = (uint8)FIXED_THRESHOLD;
#else
    uint8 raw_th = otsu_find_threshold();
    s_otsu_threshold = (uint8)(((uint16)s_otsu_threshold * 7 + (uint16)raw_th * 3) / 10);
#endif
    // 扫边线
    sweep_boundaries();
    // 计算转角
    calc_steering_value();
}

// 获取当前二值化阈值
uint8 TrackRecognition_GetThreshold(void)
{ return s_otsu_threshold; }

// 绘制中线、边线(图像显示需要单独调用)
void TrackRecognition_DrawOverlay(uint16 y_offset)
{
    for (int16 row = BOTTOM_ROW; row >= 0; row--)
        draw_row_overlay(row, y_offset);
}
