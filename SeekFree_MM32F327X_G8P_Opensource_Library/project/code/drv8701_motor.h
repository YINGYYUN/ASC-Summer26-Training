/*******************************************************************************
DRV8701 电机驱动
*******************************************************************************/


#ifndef __DRV8701_MOTOR_H__
#define __DRV8701_MOTOR_H__


#include "zf_common_headfile.h"

// 引脚配置
#define MOTOR_1_DIR_PIN             ( A2 )			
#define MOTOR_1_PWM_CHANNEL         TIM5_PWM_CH4_A3		
#define MOTOR_2_DIR_PIN             ( A0 )
#define MOTOR_2_PWM_CHANNEL         TIM5_PWM_CH2_A1

// PWM 频率 10kHz
#define MOTOR_PWM_FREQ              ( 10000 )


// 电机驱动引脚初始化
void Motor_init          (void);
// 设置duty,范围-10000~10000
void Motor_Set      (uint8 motor, int16 duty);


#endif
