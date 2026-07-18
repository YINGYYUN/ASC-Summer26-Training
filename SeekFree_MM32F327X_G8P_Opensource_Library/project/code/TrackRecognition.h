#ifndef _TrackRecognition_h_
#define _TrackRecognition_h_

#include "zf_common_headfile.h"

// 赛道识别结果结构体
typedef struct
{
    int16 left_boundary[MT9V03X_H];         // 每行的左边界 x 坐标 (-1 表示未找到)
    int16 right_boundary[MT9V03X_H];        // 每行的右边界 x 坐标 (-1 表示未找到)
    int16 center_line[MT9V03X_H];           // 每行的赛道中线 x 坐标
    float  steering_value;                   // 综合转角值（越大表示弯越急）
    uint16 valid_rows;                       // 有效行数
    int16  left_lost_from;                   // 左边界开始丢线的行号（从底部向上首次丢线），-1=未丢线
    int16  right_lost_from;                  // 右边界丢线起始行号，-1=未丢线
    uint16 left_lost_count;                  // 左边界连续丢线行数（从底部向上）
    uint16 right_lost_count;                 // 右边界连续丢线行数
} TrackResult_t;

extern TrackResult_t g_track_result;

// 外部接口函数
void  TrackRecognition_Init         (void);
void  TrackRecognition_Process      (void);
void  TrackRecognition_DrawOverlay  (uint16 y_offset);
uint8 TrackRecognition_GetThreshold (void);

#endif
