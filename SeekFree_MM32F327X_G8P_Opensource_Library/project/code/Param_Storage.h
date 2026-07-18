/*******************************************************************************
参数 Flash 存储
将可调参数持久化到 Flash，上电自动加载并同步到实际应用参数。
宏定义映射 — 每个参数单独编号，不强制 形同Kp/Ki/Kd 整齐排列。
*******************************************************************************/


#ifndef __PARAM_STORAGE_H__
#define __PARAM_STORAGE_H__


#include "zf_common_headfile.h"

// Flash 存储位置(扇区 63 页 3, 256KB Flash 最后 4KB)
#define PARAM_FLASH_SECTION     (63)
#define PARAM_FLASH_PAGE        (3)

// 参数总数
#define PARAM_COUNT             (10)
// 参数缓冲区(Flash 读写的唯一载体)
extern float param_cache[PARAM_COUNT];

// 参数索引定义(可自由扩展)
// Motor_1_PID (0-2)  电机1 速度环
#define MOTOR1_KP_IDX       0
#define MOTOR1_KI_IDX       1
#define MOTOR1_KD_IDX       2

// Motor_2_PID (3-5)  电机2 速度环
#define MOTOR2_KP_IDX       3
#define MOTOR2_KI_IDX       4
#define MOTOR2_KD_IDX       5

// Steer_Ctrl_PPDD 转向控制
#define STEER_KP_IDX        6
#define STEER_KP2_IDX       7
#define STEER_KD_IDX        8
#define STEER_GKD_IDX       9


// 便捷访问宏
#define MOTOR1_KP   param_cache[MOTOR1_KP_IDX]
#define MOTOR1_KI   param_cache[MOTOR1_KI_IDX]
#define MOTOR1_KD   param_cache[MOTOR1_KD_IDX]

#define MOTOR2_KP   param_cache[MOTOR2_KP_IDX]
#define MOTOR2_KI   param_cache[MOTOR2_KI_IDX]
#define MOTOR2_KD   param_cache[MOTOR2_KD_IDX]

#define STEER_KP    param_cache[STEER_KP_IDX]
#define STEER_KP2   param_cache[STEER_KP2_IDX]
#define STEER_KD    param_cache[STEER_KD_IDX]
#define STEER_GKD   param_cache[STEER_GKD_IDX]


void    Param_Init          (void);     // 初始化(加载或设默认值)
void    Param_Save          (void);     // 保存当前参数到 Flash
void    Param_Erase         (void);     // 擦除 Flash(下次启动恢复默认)
void    Flash_SyncTo_Param  (void);     // 将缓存区的值推送到实际参数


#endif
