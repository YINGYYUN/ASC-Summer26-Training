#ifndef _TrackRecognition_h_
#define _TrackRecognition_h_

#include "zf_common_headfile.h"

// 赛道识别结果结构体
typedef struct
{
    int16 left_boundary[MT9V03X_H];
    int16 right_boundary[MT9V03X_H];
    int16 center_line[MT9V03X_H];
    float  steering_value;
    uint16 valid_rows;
    int16  left_lost_from;
    int16  right_lost_from;
    uint16 left_lost_count;
    uint16 right_lost_count;
} TrackResult_t;

extern TrackResult_t g_track_result;

void  TrackRecognition_Init         (void);
void  TrackRecognition_Process      (void);
void  TrackRecognition_DrawOverlay  (uint16 y_offset);
uint8 TrackRecognition_GetThreshold (void);

#endif
