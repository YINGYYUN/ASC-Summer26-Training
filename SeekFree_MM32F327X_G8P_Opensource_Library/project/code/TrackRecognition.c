/*******************************************************************************
 * 赛道识别模块
 * 八邻域：追踪黑像素的黑→白跳变，多候选取最高，防卡死
 * 坐标系：image[0]=远处(顶)，image[119]=近处(底)
 * 流程：画黑框 → 固定阈值/OTSU可切换 → 最长白列种子 → 底部边界 → 八邻域追黑边 → 边线提取
 *******************************************************************************/

#include "TrackRecognition.h"

// ============================================================
// 可调参数
// ============================================================

#define IMG_W                 (MT9V03X_W)
#define IMG_H                 (MT9V03X_H)
#define IMG_CENTER            (IMG_W / 2)
#define BOTTOM_ROW            (IMG_H - 1)
#define OTSU_COL_SAMPLE_STEP  (5)
#define HALF_WIDTH_FALLBACK   90          // 单边丢线时赛道半宽估计
#define DRAW_YS_MAX           (271)        // 屏幕最大 Y 坐标（320-48-1 留底）

// ============================================================
// 全局 & 内部状态
// ============================================================

TrackResult_t g_track_result;

static uint8  s_frame_count = 0;
static uint8  s_otsu_threshold = 128;
static uint8  s_otsu_last_valid = 128;

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
// OTSU 阈值
// ============================================================

#define THRESHOLD_MODE_FIXED   1    // 1:固定阈值  0:OTSU 自适应
#define FIXED_THRESHOLD        240
#define GrayScale 256

static uint8 otsu_find_threshold(void)
{
#if THRESHOLD_MODE_FIXED
    return (uint8)FIXED_THRESHOLD;
#else
    int Pixel_Max = 0, Pixel_Min = 255;
    static int pixelCount[GrayScale];
    int pixelSum = IMG_W * IMG_H / 4;
    int i, j;
    uint8 threshold = 0;

    for (i = 0; i < GrayScale; i++) pixelCount[i] = 0;

    uint32 gray_sum = 0;
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
#endif
}

// ============================================================
// 最长白列
// ============================================================
static int16 otsu_longest_white_col(void)
{
    int16 longest_col = IMG_CENTER, longest_dist = 0;
    uint8 th = s_otsu_threshold;
    for (int16 col = IMG_W / 4; col <= IMG_W * 3 / 4; col += OTSU_COL_SAMPLE_STEP)
    {
        for (int16 row = BOTTOM_ROW; row >= 0; row--)
        {
            if (mt9v03x_image[row][col] <= th)
            {
                int16 dist = BOTTOM_ROW - row;
                if (dist > longest_dist) { longest_dist = dist; longest_col = col; }
                break;
            }
        }
    }
    return longest_col;
}

// ============================================================
// 方向表：左顺时针 / 右逆时针，均从"下"开始
// 0:下 1:左下 2:左 3:左上 4:上 5:右上 6:右 7:右下
// ============================================================
static const int8 L_DX[8] = { 0, -1, -1, -1,  0,  1,  1,  1 };
static const int8 L_DY[8] = { 1,  1,  0, -1, -1, -1,  0,  1 };

static const int8 R_DX[8] = { 0,  1,  1,  1,  0, -1, -1, -1 };
static const int8 R_DY[8] = { 1,  1,  0, -1, -1, -1,  0,  1 };

// ============================================================
// 底部边界（白→黑黑）
// ============================================================
static void otsu_bottom_boundary(int16 seed, int16 *p_left, int16 *p_right)
{
    int16 left = 0, right = IMG_W - 1;
    uint8 th = s_otsu_threshold;
    for (int16 col = seed; col >= 2; col--)
    {
        if (mt9v03x_image[BOTTOM_ROW][col - 1] <= th
            && mt9v03x_image[BOTTOM_ROW][col - 2] <= th)
        { left = col; break; }
    }
    for (int16 col = seed; col < IMG_W - 2; col++)
    {
        if (mt9v03x_image[BOTTOM_ROW][col + 1] <= th
            && mt9v03x_image[BOTTOM_ROW][col + 2] <= th)
        { right = col; break; }
    }
    *p_left = left; *p_right = right;
}

// ============================================================
// 左边界八邻域一步（追黑像素，黑→白跳变，取最高）
// ============================================================
static uint8 trace_left_boundary(int16 *row, int16 *col)
{
    uint8 th = s_otsu_threshold;
    int16 best_r = 999, best_c = -1;

    for (int16 i = 0; i < 8; i++)
    {
        int16 nr  = *row + L_DY[i];
        int16 nc  = *col + L_DX[i];
        int16 nr1 = *row + L_DY[(i + 1) & 7];
        int16 nc1 = *col + L_DX[(i + 1) & 7];

        if (nr < 0 || nr >= IMG_H || nc < 0 || nc >= IMG_W) continue;
        if (nr1 < 0 || nr1 >= IMG_H || nc1 < 0 || nc1 >= IMG_W) continue;

        if (mt9v03x_image[nr][nc] <= th && mt9v03x_image[nr1][nc1] > th)
        {
            if (nr < best_r) { best_r = nr; best_c = nc; }
        }
    }
    if (best_c < 0) return 0;
    *row = best_r; *col = best_c;
    return 1;
}

// ============================================================
// 右边界八邻域一步（追黑像素，黑→白跳变，取最高）
// ============================================================
static uint8 trace_right_boundary(int16 *row, int16 *col)
{
    uint8 th = s_otsu_threshold;
    int16 best_r = 999, best_c = -1;

    for (int16 i = 0; i < 8; i++)
    {
        int16 nr  = *row + R_DY[i];
        int16 nc  = *col + R_DX[i];
        int16 nr1 = *row + R_DY[(i + 1) & 7];
        int16 nc1 = *col + R_DX[(i + 1) & 7];

        if (nr < 0 || nr >= IMG_H || nc < 0 || nc >= IMG_W) continue;
        if (nr1 < 0 || nr1 >= IMG_H || nc1 < 0 || nc1 >= IMG_W) continue;

        if (mt9v03x_image[nr][nc] <= th && mt9v03x_image[nr1][nc1] > th)
        {
            if (nr < best_r) { best_r = nr; best_c = nc; }
        }
    }
    if (best_c < 0) return 0;
    *row = best_r; *col = best_c;
    return 1;
}

// ============================================================
// 边界搜索主入口
// ============================================================
static void search_boundaries(void)
{
    draw_image_black_border();

    // 重置
    g_track_result.valid_rows = 0;
    for (int16 i = 0; i < IMG_H; i++)
    {
        g_track_result.left_boundary[i]  = -1;
        g_track_result.right_boundary[i] = -1;
        g_track_result.center_line[i]    = IMG_CENTER;
    }

    // 阈值
#if THRESHOLD_MODE_FIXED
    s_otsu_threshold = (uint8)FIXED_THRESHOLD;
#else
    uint8 raw_th = otsu_find_threshold();
    s_otsu_threshold = (uint8)(((uint16)s_otsu_threshold * 7 + (uint16)raw_th * 3) / 10);
#endif

    // 最长白列种子
    int16 seed = otsu_longest_white_col();

    // 底部边界（白像素位置）
    int16 bot_l, bot_r;
    otsu_bottom_boundary(seed, &bot_l, &bot_r);

    // ---- 左边界追踪（从白边左侧的黑像素出发） ----
    {
        int16 lr = BOTTOM_ROW, lc = bot_l - 1;            // 黑像素起点
        int16 lr_p1 = -1, lc_p1 = -1, lr_p2 = -1, lc_p2 = -1;  // 防卡死
        int16 safety = IMG_H * 3;

        while (lr > 0 && safety-- > 0)
        {
            if (!trace_left_boundary(&lr, &lc)) break;

            // 黑→白跳变，白像素 = lc + 1
            int16 wx = lc + 1;
            if (wx >= 0 && wx < IMG_W)
                g_track_result.left_boundary[lr] = wx;

            // 防卡死：同一坐标连续3次则退出
            if (lr == lr_p1 && lc == lc_p1 && lr == lr_p2 && lc == lc_p2) break;
            lr_p2 = lr_p1; lc_p2 = lc_p1;
            lr_p1 = lr;    lc_p1 = lc;
        }
    }

    // ---- 右边界追踪（从白边右侧的黑像素出发） ----
    {
        int16 rr = BOTTOM_ROW, rc = bot_r + 1;
        int16 rr_p1 = -1, rc_p1 = -1, rr_p2 = -1, rc_p2 = -1;
        int16 safety = IMG_H * 3;

        while (rr > 0 && safety-- > 0)
        {
            if (!trace_right_boundary(&rr, &rc)) break;

            int16 wx = rc - 1;
            if (wx >= 0 && wx < IMG_W)
                g_track_result.right_boundary[rr] = wx;

            if (rr == rr_p1 && rc == rc_p1 && rr == rr_p2 && rc == rc_p2) break;
            rr_p2 = rr_p1; rc_p2 = rc_p1;
            rr_p1 = rr;    rc_p1 = rc;
        }
    }

    // ---- 中线 & 丢线统计 ----
    g_track_result.left_lost_from  = -1;
    g_track_result.right_lost_from = -1;

    for (int16 row = BOTTOM_ROW; row >= 0; row--)
    {
        int16 l = g_track_result.left_boundary[row];
        int16 r = g_track_result.right_boundary[row];

        if (g_track_result.left_lost_from  < 0 && l < 0) g_track_result.left_lost_from  = row;
        if (g_track_result.right_lost_from < 0 && r < 0) g_track_result.right_lost_from = row;

        if (l >= 0 && r >= 0)
        { g_track_result.center_line[row] = (l + r) / 2; g_track_result.valid_rows++; }
        else if (l >= 0)
        { g_track_result.center_line[row] = l + HALF_WIDTH_FALLBACK; }
        else if (r >= 0)
        { g_track_result.center_line[row] = r - HALF_WIDTH_FALLBACK; }
    }

    g_track_result.left_lost_count  = (g_track_result.left_lost_from  >= 0)
        ? (uint16)(BOTTOM_ROW - g_track_result.left_lost_from)  : 0;
    g_track_result.right_lost_count = (g_track_result.right_lost_from >= 0)
        ? (uint16)(BOTTOM_ROW - g_track_result.right_lost_from) : 0;
}

// ============================================================
// 转角计算 & 保存 & 绘制
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

static void draw_row_overlay(int16 row, uint16 y_offset)
{
    int16 left  = g_track_result.left_boundary[row];
    int16 right = g_track_result.right_boundary[row];
    int16 center = g_track_result.center_line[row];
    uint16 ys = (uint16)row + y_offset;
    uint8 draw_second_row = (ys + 1 < DRAW_YS_MAX);

    if (left >= 0 && left + 2 < IMG_W)
    {
        ips200_draw_point(left,     ys, RGB565_RED);
        ips200_draw_point(left + 1, ys, RGB565_RED);
        ips200_draw_point(left + 2, ys, RGB565_RED);
        if (draw_second_row)
        {
            ips200_draw_point(left,     ys + 1, RGB565_RED);
            ips200_draw_point(left + 1, ys + 1, RGB565_RED);
            ips200_draw_point(left + 2, ys + 1, RGB565_RED);
        }
    }
    if (right >= 0 && right + 2 < IMG_W)
    {
        ips200_draw_point(right,     ys, RGB565_BLUE);
        ips200_draw_point(right + 1, ys, RGB565_BLUE);
        ips200_draw_point(right + 2, ys, RGB565_BLUE);
        if (draw_second_row)
        {
            ips200_draw_point(right,     ys + 1, RGB565_BLUE);
            ips200_draw_point(right + 1, ys + 1, RGB565_BLUE);
            ips200_draw_point(right + 2, ys + 1, RGB565_BLUE);
        }
    }
    if (center >= 1 && center + 1 < IMG_W && (left >= 0 || right >= 0))
    {
        ips200_draw_point(center - 1, ys, RGB565_GREEN);
        ips200_draw_point(center,     ys, RGB565_GREEN);
        ips200_draw_point(center + 1, ys, RGB565_GREEN);
    }
}

// ============================================================
// 外部接口
// ============================================================

void TrackRecognition_Init(void)
{
    s_otsu_threshold = 128; s_otsu_last_valid = 128; s_frame_count = 0;
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
    search_boundaries();
    calc_steering_value();
    s_frame_count++;
}

uint8 TrackRecognition_GetThreshold(void)
{ return s_otsu_threshold; }

void TrackRecognition_DrawOverlay(uint16 y_offset)
{
    for (int16 row = BOTTOM_ROW; row >= 0; row--)
        draw_row_overlay(row, y_offset);
}
