/*******************************************************************************
主程序
*******************************************************************************/


#include "zf_common_headfile.h"

int main_process(void)
{
    // 重置PID中间量
    PID_ALL_Init();
    // 电机速度重置
    Motor_ALL_Zero();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    // 基础的PWM占空比
    int16_t pwm_base = 1000;
    int16_t pwm_left  = 0;
    int16_t pwm_right = 0;

    while(1)
    {
        /* 按键处理 */
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);

        }
        if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN);
        }
        if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
        }

        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            ips200_clear();
            // 重置PID中间量
            PID_ALL_Init();
            // 电机速度重置
            Motor_ALL_Zero();

            // 返回上一级界面
            return 0;
        }


        if (Time_Count1 >= 10)// 10ms * 5 周期
        {
            Time_Count1 = 0;

            // ---- 图像数据读取 + 赛道识别 ----
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;

                // 执行赛道识别
                TrackRecognition_Process();
            }
            imu963ra_get_gyro();

            Steer_Ctrl_PPDD.Target = 0;              // 目标：中线
            Steer_Ctrl_PPDD.Actual = g_track_result.steering_value;
            Steer_Ctrl_PPDD.Gyro   = imu963ra_gyro_z; 
            STEER_CTRL_Update(&Steer_Ctrl_PPDD);

            pwm_left  = pwm_base + (int16_t)Steer_Ctrl_PPDD.Out;
            pwm_right = pwm_base - (int16_t)Steer_Ctrl_PPDD.Out;
            if (2000 <= pwm_left){pwm_left = 2000;}
            if (pwm_left <= -2000){pwm_left = -2000;}
            if (2000 <= pwm_right){pwm_right = 2000;}
            if (pwm_right <= -2000){pwm_right = -2000;}

            Motor_Set(1 ,pwm_left);
            Motor_Set(2 ,pwm_right);
        }

        
        if (Time_Count2 >= 10)// 10ms * 10 周期
        {
            Time_Count2 = 0;

        }

    }
}
