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

// 出界保护 0=正常  1=出界
uint8 Check_LoseTrack               (void);
// 起止点(斑马线)识别
uint8 Check_Zebra                   (void);
// 赛道识别初始化
void  TrackRecognition_Init         (void);
// 赛道识别进程
void  TrackRecognition_Process      (void);
// 绘制中线、边线(图像显示需要单独调用)
void  TrackRecognition_DrawOverlay  (uint16 y_offset);
// 获取当前二值化阈值
uint8 TrackRecognition_GetThreshold (void);


#endif
