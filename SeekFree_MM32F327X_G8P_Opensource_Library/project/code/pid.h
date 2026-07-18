/*******************************************************************************
PID
*******************************************************************************/


#ifndef __PID_H__
#define __PID_H__


#include "zf_common_headfile.h"


/**********************************************************/
/*[S] 基础函数 [S]----------------------------------------*/
/**********************************************************/

// 位置式PID结构体
typedef struct {
	float Target;			// 目标
	float Actual;			// 这次实际
	// float Actual1;			// 上次实际（微分先行使用）
	float Out;				// 输出
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;			// 此次误差
	float Error1;			// 上次误差
	float ErrorInt; 		// 误差累积（积分）
	
	float ErrorIntMax;		// 积分最大值
	float ErrorIntMin;		// 积分最小值
	
	float OutMax;			// 输出最大值
	float OutMin;			// 输出最小值
	
	float IntSepThresh;  	// 积分分离阈值（误差大于阈值 积分清零）
	
} PID_POS_t;

// 增量式PID结构体
typedef struct {
	float Target;			// 目标
	float Actual;			// 这次实际
	float Out;				// 输出
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;			// 此次误差
	float Error1;			// 上次误差
	float Error2;			// 上上次误差
	float ErrorInt; 		// 误差累积（积分）
	
	float OutMax;			// 输出最大值
	float OutMin;			// 输出最小值
	
	float OutDeltaMax;		// 单次增量变化上限（=0 时不限制，>0 时每次 Out 变化不超过此值）
} PID_INC_t;


// 转向控制结构体（非线性PID + 陀螺仪前馈）
// 公式: Out = Error*KP + Error*|Error|*KP2 + (Error-LastError)*KD + Gyro*GKD
typedef struct {
	float Target;			// 目标
	float Actual;			// 当前实际（图像偏差）
	float Gyro;				// 陀螺仪数值
	float Out;				// 输出（转角值）
	
	float KP;				// 线性比例系数
	float KP2;				// 非线性比例系数（平方项系数）
	float KD;				// 微分系数
	float GKD;				// 陀螺仪系数
	
	float Error0;			// 此次误差
	float Error1;			// 上次误差
	
	float OutMax;			// 输出最大值
	float OutMin;			// 输出最小值
} STEER_CTRL_t;


// 位置式PID重置中间量
void	 PID_POS_Init		(PID_POS_t *p);
// 位置式PID计算
void 	PID_POS_Update		(PID_POS_t *p);
// 增量式PID重置中间量
void	 PID_INC_Init		(PID_INC_t *p);
// 增量式PID计算
void 	PID_INC_Update		(PID_INC_t *p);
// 转向控制重置中间量
void	 STEER_CTRL_Init	(STEER_CTRL_t *p);
// 转向控制计算
void	 STEER_CTRL_Update	(STEER_CTRL_t *p);

/**********************************************************/
/*----------------------------------------[E] 基础函数 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] PID实例 [S]----------------------------------------*/
/**********************************************************/

// 位置式


// 增量式
extern PID_INC_t Motor_1_PID;// 电机1 PID参数
extern PID_INC_t Motor_2_PID;// 电机2 PID参数

// 转向控制
extern STEER_CTRL_t Steer_Ctrl_PPDD;

void PID_ALL_Init(void);

/**********************************************************/
/*----------------------------------------[E] PID实例 [E]*/
/**********************************************************/


#endif
