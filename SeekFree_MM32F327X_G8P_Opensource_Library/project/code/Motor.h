/*******************************************************************************
电机相关
*******************************************************************************/


#ifndef __MOTOR_H__
#define __MOTOR_H__


#include "zf_common_headfile.h"


/**********************************************************/
/*[S] 电机驱动 [S]----------------------------------------*/
/**********************************************************/

// 电机驱动(DRV8701)引脚配置	
#define MOTOR_1_DIR_PIN             ( A0 )
#define MOTOR_1_PWM_CHANNEL         TIM5_PWM_CH2_A1
#define MOTOR_2_DIR_PIN             ( A2 )			
#define MOTOR_2_PWM_CHANNEL         TIM5_PWM_CH4_A3	

// PWM 频率 10kHz
#define MOTOR_PWM_FREQ              ( 10000 )

// 电机驱动引脚初始化
void    Motor_init                  (void);
// 设置duty,范围-10000~10000
void    Motor_Set                   (uint8 motor, int16 duty);
// 电机速度归零
void    Motor_ALL_Zero              (void);
/**********************************************************/
/*----------------------------------------[E] 电机驱动 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 编码器 [S]------------------------------------------*/
/**********************************************************/
// 编码器(正交)通道定义二次宏定义
// 1 (左)
#define ENCODER_1 				TIM3_ENCODER
#define ENC_1_P_CH1				TIM3_ENCODER_CH1_B4
#define ENC_1_P_CH2				TIM3_ENCODER_CH2_B5

// 2 (右)
#define ENCODER_2 				TIM4_ENCODER
#define ENC_2_P_CH1				TIM4_ENCODER_CH1_B6
#define ENC_2_P_CH2				TIM4_ENCODER_CH2_B7
// 编码器(正交)调用二次宏定义
#define ENC1_GET()              (encoder_get_count(ENCODER_1))
#define ENC1_CLEAR()            encoder_clear_count(ENCODER_1)

#define ENC2_GET()              (-encoder_get_count(ENCODER_2))
#define ENC2_CLEAR()            encoder_clear_count(ENCODER_2)

// 左右编码器计数值全局变量
extern int ENC_left_CNT;
extern int ENC_right_CNT;
// // 左右编码器累加值全局变量
// 似乎不方便做防溢出？
// extern int ENC_left_SUM;
// extern int ENC_right_SUM;
/**********************************************************/
/*------------------------------------------[E] 编码器 [E]*/
/**********************************************************/


#endif
