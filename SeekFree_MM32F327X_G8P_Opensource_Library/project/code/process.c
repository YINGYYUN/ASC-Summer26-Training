/*******************************************************************************
主程序
*******************************************************************************/


#include "zf_common_headfile.h"
#include <math.h>

// 小车运行状态
typedef enum
{
    Car_Stop        = 0, // 停车状态
    Car_Running     = 1, // 运行状态
    Car_IDLE        = 2, // 空闲状态(和停车状态区分)
} Car_MAIN_State;
// 默认停车
static Car_MAIN_State car_state = Car_IDLE;

int main_process(void)
{
    // 重置PID中间量
    PID_ALL_Init();
    // 电机速度重置
    Motor_ALL_Zero();

    Speed_PID_Crtl_Enable = 0;

    // 识别结果初始化
    TrackRecognition_Init();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    int16_t Speed_base = 0;
    int16_t Tar_Left   = 0;
    int16_t Tar_Right  = 0;

    uint8_t Zebra_Zone_Count = 0;

    ips200_show_string(8  ,0  , "[Process]");
    ips200_show_string(0  ,16 , "==============================");
    
    ips200_show_string(0  ,192, "State:IDLE");

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
            // 发车
            car_state = Car_Running;
            ips200_show_string(48 ,192, "Run~");
            Speed_PID_Crtl_Enable = 1;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            car_state = Car_IDLE;
            // 电机速度重置
            Motor_ALL_Zero();
            // 重置PID中间量
            PID_ALL_Init();
            ips200_clear();
            
            // 返回上一级界面
            return 0;
        }


        if (car_state == Car_Stop)
        {
            Speed_PID_Crtl_Enable = 0;
            // 电机速度重置
            Motor_ALL_Zero();
            // 重置PID中间量
            PID_ALL_Init();
            // 识别结果初始化
            TrackRecognition_Init();
            // 斑马线计数重置
            Zebra_Zone_Count = 0;
            // 切换到空闲状态，防止反复触发停车相关代码
            car_state = Car_IDLE;
            // 加快流程,只会在屏幕指示IDLE状态,STOP状态跳过
            ips200_show_string(48 ,192, "IDLE");
        }


        if (Time_Count1 >= 1)// 10ms * 1 = 10ms 控制周期
        {
            Time_Count1 = 0;
            
            // 出界判定 + 赛道识别
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;
                if (Check_LoseTrack() == 1)
                {
                    // 停车
                    car_state = Car_Stop;
                }
                TrackRecognition_Process();
                if (Check_Zebra() == 1)
                {
                    Zebra_Zone_Count ++;
                    
                    // 如果经过了 n 次斑马线
                    if (Zebra_Zone_Count >= 2)
                    {
                        // 停车
                        car_state = Car_Stop;
                    }
                }
            }

            if (car_state == Car_Running)
            {
                imu963ra_get_gyro();

                // 速度分级：赛道偏差小 → 高速，偏差大 → 低速
                if (fabs(g_track_result.steering_value) < 15.0f)
                    Speed_base = 1400;
                else
                    Speed_base = 1200;

				Steer_Ctrl_PPDD.Target = 0;
				Steer_Ctrl_PPDD.Actual = g_track_result.steering_value;
                
                Steer_Ctrl_PPDD.Gyro = (imu963ra_gyro_z + 7)/20*20;
                STEER_CTRL_Update(&Steer_Ctrl_PPDD);

                Tar_Left  = Speed_base - (int16_t)Steer_Ctrl_PPDD.Out;
                Tar_Right = Speed_base + (int16_t)Steer_Ctrl_PPDD.Out;
                
                // Steer_PID.Target = 0;
                // Steer_PID.Actual = g_track_result.steering_value * 10.0f;
                // PID_POS_Update(&Steer_PID);
                // GZ_PID.Target = Steer_PID.Out;
                // GZ_PID.Actual = imu963ra_gyro_transition((imu963ra_gyro_z + 7)/20*20);
                // PID_POS_Update(&GZ_PID);

                // Tar_Left = Speed_base - (int16_t)GZ_PID.Out;
                // Tar_Right = Speed_base + (int16_t)GZ_PID.Out;

                // if (8000 <= Tar_Left)  { Tar_Left  =  8000; }
                // if (Tar_Left <= -8000) { Tar_Left  = -8000; }
                // if (8000 <= Tar_Right) { Tar_Right =  8000; }
                // if (Tar_Right <= -8000){ Tar_Right = -8000; }
                // Motor_Set(1, Tar_Left);
                // Motor_Set(2, Tar_Right);

                Motor_1_PID.Target = Tar_Left;
                Motor_2_PID.Target = Tar_Right;
            }
        }


        if (Time_Count2 >= 25)// 10ms * 25 显示周期
        {
            Time_Count2 = 0;
            // 图像不显示

            // 调试：观察赛道偏差和基础速度
            ips200_show_float(0, 160, g_track_result.steering_value, 6, 2);
            ips200_show_uint (120, 160, (uint16)Speed_base, 4);
            // ips200_show_float(0, 176, Steer_Ctrl_PPDD.Out, 6, 2);
            ips200_show_float(0, 176, Steer_PID.Out, 6, 2);
            ips200_show_float(120, 176, GZ_PID.Out, 6, 2);
        }
    }
}
