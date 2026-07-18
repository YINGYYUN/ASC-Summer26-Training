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
// 最长白列（从底部向上，中心区域找白色段最长的列）
// ============================================================
#define OTSU_COL_SAMPLE_STEP  5
#define HALF_WIDTH_FALLBACK   90
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
#define SWEEP_OFFSET  10   // 上行搜索起点向内偏移量

static void sweep_boundaries(void)
{
    uint8 th = s_otsu_threshold;

    g_track_result.valid_rows = 0;

    // 最长白列种子
    int16 seed = otsu_longest_white_col();

    // ---- 底行：从种子向左右扫（白→黑） ----
    int16 l = seed;
    while (l > 2 && mt9v03x_image[BOTTOM_ROW][l - 1] > th) l--;
    int16 r = seed;
    while (r < IMG_W - 3 && mt9v03x_image[BOTTOM_ROW][r + 1] > th) r++;

    g_track_result.left_boundary[BOTTOM_ROW]  = l;
    g_track_result.right_boundary[BOTTOM_ROW] = r;

    if (l < r)
    {
        g_track_result.center_line[BOTTOM_ROW] = (l + r) / 2;
        g_track_result.valid_rows++;
    }

    int16 prev_l = l, prev_r = r;

    // ---- 上行：从上一行边界 + 偏移开始，白黑黑判定 ----
    for (int16 row = BOTTOM_ROW - 1; row >= 0; row--)
    {
        // 左边界：从 prev_l + OFFSET（向内移）向左扫
        l = prev_l + SWEEP_OFFSET;
        if (l > IMG_W - 4) l = IMG_W - 4;
        if (mt9v03x_image[row][l] > th)
        {
            // 起点在白区，向左找白黑黑
            while (l > 2
                && !(mt9v03x_image[row][l - 1] <= th
                  && mt9v03x_image[row][l - 2] <= th))
                l--;
        }
        else
        {
            // 起点在黑区（赛道变窄），从边界向内回找
            l = prev_l;
            while (l > 2 && mt9v03x_image[row][l] <= th) l--;
            if (l > 2)
            {
                while (l > 2
                    && !(mt9v03x_image[row][l - 1] <= th
                      && mt9v03x_image[row][l - 2] <= th))
                    l--;
            }
        }

        // 右边界：从 prev_r - OFFSET（向内移）向右扫
        r = prev_r - SWEEP_OFFSET;
        if (r < 2) r = 2;
        if (mt9v03x_image[row][r] > th)
        {
            // 起点在白区，向右找白黑黑
            while (r < IMG_W - 3
                && !(mt9v03x_image[row][r + 1] <= th
                  && mt9v03x_image[row][r + 2] <= th))
                r++;
        }
        else
        {
            // 起点在黑区（赛道变窄），从边界向内回找
            r = prev_r;
            while (r < IMG_W - 3 && mt9v03x_image[row][r] <= th) r++;
            if (r < IMG_W - 3)
            {
                while (r < IMG_W - 3
                    && !(mt9v03x_image[row][r + 1] <= th
                      && mt9v03x_image[row][r + 2] <= th))
                    r++;
            }
        }

        g_track_result.left_boundary[row]  = l;
        g_track_result.right_boundary[row] = r;

        if (l >= 0 && r >= 0 && l < r)
        {
            g_track_result.center_line[row] = (l + r) / 2;
            g_track_result.valid_rows++;
            prev_l = l;
            prev_r = r;
        }
        else if (l >= 0)
        {
            g_track_result.center_line[row] = l + HALF_WIDTH_FALLBACK;
        }
        else if (r >= 0)
        {
            g_track_result.center_line[row] = r - HALF_WIDTH_FALLBACK;
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
// 转角计算
// ============================================================
static void calc_steering_value(void)
{
    if (g_track_result.valid_rows == 0)
    { g_track_result.steering_value = 0.0f; return; }

    float total_dev = 0.0f, total_w = 0.0f;
    for (int16 row = BOTTOM_ROW; row >= 0; row--)
    {
        if (g_track_result.left_boundary[row] >= 0
            && g_track_result.right_boundary[row] >= 0
            && g_track_result.center_line[row] >= 0
            && g_track_result.center_line[row] < IMG_W)
        {
            int16 dev = (int16)g_track_result.center_line[row] - IMG_CENTER;
            float w = 1.0f + (float)row / (float)IMG_H * 2.0f;
            total_dev += dev * w; total_w += w;
        }
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
#if THRESHOLD_MODE_FIXED
    s_otsu_threshold = FIXED_THRESHOLD;
#else
    s_otsu_threshold = 128;
    s_otsu_last_valid = 128;
#endif

    for (int16 i = 0; i < IMG_H; i++)
    {
        g_track_result.left_boundary[i]  = -1;
        g_track_result.right_boundary[i] = -1;
        g_track_result.center_line[i]    = IMG_CENTER;
    }
    g_track_result.steering_value = 0.0f;
    g_track_result.valid_rows = 0;
    g_track_result.left_lost_from  = -1; g_track_result.right_lost_from = -1;
    g_track_result.left_lost_count = 0;  g_track_result.right_lost_count = 0;
}

void TrackRecognition_Process(void)
{
    draw_image_black_border();

#if THRESHOLD_MODE_FIXED
    s_otsu_threshold = (uint8)FIXED_THRESHOLD;
#else
    uint8 raw_th = otsu_find_threshold();
    s_otsu_threshold = (uint8)(((uint16)s_otsu_threshold * 7 + (uint16)raw_th * 3) / 10);
#endif

    sweep_boundaries();
    calc_steering_value();
}

uint8 TrackRecognition_GetThreshold(void)
{ return s_otsu_threshold; }

void TrackRecognition_DrawOverlay(uint16 y_offset)
{
    for (int16 row = BOTTOM_ROW; row >= 0; row--)
        draw_row_overlay(row, y_offset);
}
