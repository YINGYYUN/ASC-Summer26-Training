/*******************************************************************************
宏定义文件
对于部分配置进行二次宏定义，方便阅读代码和集中更改
*******************************************************************************/

#ifndef __DEFINE_H__
#define __DEFINE_H__


// 编码器(正交)通道定义二次宏定义
#define ENCODER_1 				TIM3_ENCODER
#define ENC_1_P_CH1				TIM3_ENCODER_CH1_B4
#define ENC_1_P_CH2				TIM3_ENCODER_CH2_B5

#define ENCODER_2 				TIM4_ENCODER
#define ENC_2_P_CH1				TIM4_ENCODER_CH1_B6
#define ENC_2_P_CH2				TIM4_ENCODER_CH2_B7
// 编码器(正交)调用二次宏定义
#define ENC1_GET()              encoder_get_count(ENCODER_1)
#define ENC1_CLEAR()            encoder_clear_count(ENCODER_1)

#define ENC2_GET()              encoder_get_count(ENCODER_2)
#define ENC2_CLEAR()            encoder_clear_count(ENCODER_2)

#endif
