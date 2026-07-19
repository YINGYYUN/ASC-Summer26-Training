/*******************************************************************************
主程序
*******************************************************************************/


#include "zf_common_headfile.h"
#include <math.h>

int main_process(void)
{
    // 重置PID中间量
    PID_ALL_Init();
    // 电机速度重置
    Motor_ALL_Zero();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    int16_t pwm_base = 0;
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


        if (Time_Count1 >= 1)// 10ms * 1 = 10ms 控制周期
        {
            Time_Count1 = 0;

            // ---- 赛道识别 ----
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;
                TrackRecognition_Process();
            }

            imu963ra_get_gyro();

            // 丢线保护：双侧大范围丢线时只用陀螺仪维持姿态
            if (g_track_result.left_lost_count > 20
                && g_track_result.right_lost_count > 20)
            {
                Steer_Ctrl_PPDD.Target = 0;
                Steer_Ctrl_PPDD.Actual = 0;
            }
            else
            {
                Steer_Ctrl_PPDD.Target = 0;
                Steer_Ctrl_PPDD.Actual = g_track_result.steering_value * 20.0f;
            }
            Steer_Ctrl_PPDD.Gyro = imu963ra_gyro_z;
            STEER_CTRL_Update(&Steer_Ctrl_PPDD);

            // 速度分级：赛道偏差小 → 高速，偏差大 → 低速
            if (fabs(g_track_result.steering_value) < 1.5f)
                pwm_base = 700;
            else
                pwm_base = 650;

            pwm_left  = pwm_base - (int16_t)Steer_Ctrl_PPDD.Out;
            pwm_right = pwm_base + (int16_t)Steer_Ctrl_PPDD.Out;
            if (3000 <= pwm_left)  { pwm_left  =  3000; }
            if (pwm_left <= -3000) { pwm_left  = -3000; }
            if (3000 <= pwm_right) { pwm_right =  3000; }
            if (pwm_right <= -3000){ pwm_right = -3000; }

            Motor_Set(1, pwm_left);
            Motor_Set(2, pwm_right);
        }


        if (Time_Count2 >= 15)// 10ms * 15 = 150ms 显示周期
        {
            Time_Count2 = 0;

            // 不消费 finish_flag，直接显示（可能比控制周期旧一帧，不影响调试）
            ips200_show_gray_image(0, 32,
                mt9v03x_image[0], MT9V03X_W, MT9V03X_H,
                MT9V03X_W, MT9V03X_H, TrackRecognition_GetThreshold());
            TrackRecognition_DrawOverlay(32);

            // 调试：观察赛道偏差和基础速度
            ips200_show_float(0, 160, g_track_result.steering_value, 6, 2);
            ips200_show_uint (120, 160, (uint16)pwm_base, 4);
            ips200_show_float(0, 176, Steer_Ctrl_PPDD.Out, 6, 2);
        }

    }
}
