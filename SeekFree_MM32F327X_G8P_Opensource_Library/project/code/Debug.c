/*******************************************************************************
调试
*******************************************************************************/


#include "zf_common_headfile.h"

/**********************************************************/
/*[S] 界面样式 [S]----------------------------------------*/
/**********************************************************/

// [二级界面]Debug模式界面
void Debug_Page_Menu_UI(uint8_t Page)
{
	switch(Page)
	{
		// 第一页
		case 1:
			ips200_show_string(8  ,0  , "[Debug]");
			ips200_show_string(0  ,16 , "==============================");
			ips200_show_string(10 ,32 , "UART");            // CH-04蓝牙 / 串口
			ips200_show_string(10 ,48 , "Motor");           // 驱动+编码器
			ips200_show_string(10 ,64 , "Motor_PID");       // 速度环
            ips200_show_string(10 ,80 , "MT9V03x");         // 总钻风图像显示
            ips200_show_string(10 ,96 , "MT9-Track");       // 赛道识别
            ips200_show_string(10 ,112, "IMU");             // IMU963RA调试
            ips200_show_string(10 ,128, "Remote-Crtl");     // 遥控 (使用蓝牙+江协的蓝牙助手)
		
			break;
	}
}

// [三级界面]UART调试界面       
// CH-04蓝牙 / 串口
void Debug_UART_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-UART");
    ips200_show_string(0  ,16 , "==============================");
    ips200_show_string(0  ,32 , "  [Press CONFIRM to send TX]");
    ips200_show_string(0  ,48 , "TX:");
    // TX溢出字符会切割到这一行显示
    ips200_show_string(0  ,80 , "RX:");
    // RX溢出字符会切割到这一行显示
}

// [三级界面]Motor调试界面      
// 驱动+编码器
void Debug_Motor_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-Motor");
    ips200_show_string(0  ,16 , "==============================");
	// 空行
    ips200_show_string(10 ,48 , "PWM 1:");
	ips200_show_string(10 ,64 , "PWM 2:");
	// 空行
	ips200_show_string(0  ,96 , "ENC 1: NAN");
	ips200_show_string(0  ,112, "ENC 2: NAN");
    ips200_show_string(0  ,128, "SUM 1: NAN");
	ips200_show_string(0  ,144, "SUM 2: NAN");
}

// [三级界面]Motor_PID调试界面   
// 速度环
void Debug_Motor_PID_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-Motor-PID");
    ips200_show_string(0  ,16 , "==============================");
	// 空行
    ips200_show_string(10 ,48 , "TAR 1:");
	ips200_show_string(10 ,64 , "TAR 2:");
	// 空行
	ips200_show_string(0  ,96 , "ENC 1:");
	ips200_show_string(0  ,112, "ENC 2:");
	ips200_show_string(0  ,128, "PWM 1:");
	ips200_show_string(0  ,144, "PWM 2:");
}

// [三级界面]MT9V03x调试界面    
// 总钻风图像显示
void Debug_MT9V03x_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-MT9V03x");
    ips200_show_string(0  ,16 , "==============================");
    // 图像显示空间
    // 图像显示空间
    // 图像显示空间
    ips200_show_string(0  ,160, "Show(us):");
}

// [三级界面]MT9-Track调试界面    
// 赛道识别
void Debug_MT9_Track_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-MT9-Track");
    ips200_show_string(0  ,16 , "==============================");
    // 图像显示空间
    // 图像显示空间
    // 图像显示空间
    ips200_show_string(0  ,160, "Track(us):");
    ips200_show_string(0  ,176, "Show(us):NaN");
    ips200_show_string(0  ,192, "Steer:");
    ips200_show_string(0  ,208, "Lose?:");
    ips200_show_string(0  ,224, "Zebra:");
}

// [三级界面]IMU963RA调试界面
void Debug_IMU_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-IMU");
    ips200_show_string(0  ,16 , "==============================");
    ips200_show_string(0  ,32 , "Cali:[GYRO]IDLE  [MAGN]IDLE");
    // 空行
    ips200_show_string(0  ,64 , "ax:NaN");
    ips200_show_string(0  ,80 , "ay:NaN");
    ips200_show_string(0  ,96 , "az:NaN");
    ips200_show_string(0  ,112, "gx:NaN");
    ips200_show_string(0  ,128, "gy:NaN");
    ips200_show_string(0  ,144, "gz:NaN");
    // ips200_show_string(0  ,160, "mx:NaN");
    // ips200_show_string(0  ,176, "my:NaN");
    // ips200_show_string(0  ,192, "mz:NaN");
    // 空行
    ips200_show_string(0  ,224, "Rol:NaN");
    ips200_show_string(0  ,240, "Yaw:NaN");
    ips200_show_string(0  ,256, "Pih:NaN");
}

void Debug_Remote_Crtl_UI(void)
{
    ips200_show_string(8  ,0  , "[DEBUG]-Remote_Crtl");
    ips200_show_string(0  ,16 , "==============================");
    ips200_show_string(0  ,48 , "LH:");
    ips200_show_string(72 ,48 , "LV:");
    ips200_show_string(0  ,64 , "RH:");
    ips200_show_string(72 ,64 , "RV:");
    ips200_show_string(0  ,80 , "TAR L:");
    ips200_show_string(104,80 , "R:");
    ips200_show_string(0  ,96 , "ENC L:");
    ips200_show_string(104,96 , "R:");
}
// [三级界面]遥控界面
/**********************************************************/
/*----------------------------------------[E] 界面样式 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 菜单逻辑 [S]----------------------------------------*/
/**********************************************************/

// 相关函数提前声明
int Debug_UART         	(void);
int Debug_Motor         (void);
int Debug_Motor_PID     (void);
int Debug_MT9V03x       (void);
int Debug_MT9_Track     (void);
int Debug_IMU           (void);
int Debug_Remote_Crtl   (void);

// [二级界面]Debug模式界面
int Debug_Page_Menu(void)
{
    // Debug模式选项 标志位
    uint8_t Debug_Page_flag = 1;

    Debug_Page_Menu_UI(1);
    ips200_show_string(0  ,32 , ">");

    while(1)
    {
        // 存储确认键被按下时Debug_Page_flag的值的临时变量，默认为无效值0
		uint8_t Debug_Page_flag_temp = 0;
		// 上/下按键是否被按下过
		uint8_t key_pressed = 0;


        /* 按键处理*/
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);
            key_pressed = 1;
            Debug_Page_flag --;
            if (Debug_Page_flag < 1)Debug_Page_flag = 7;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN); 
            key_pressed = 1;
            Debug_Page_flag ++;
            if (Debug_Page_flag > 7)Debug_Page_flag = 1;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            Debug_Page_flag_temp = Debug_Page_flag;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))    
        {
			key_clear_state(KEY_BACK);
            // 返回上一级界面
            return 0;   
        }


        /* 模式跳转*/
        if (Debug_Page_flag_temp == 1)
        {
            ips200_clear();
            Debug_UART();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,32 , ">");
        }
		else if (Debug_Page_flag_temp == 2)
        {
            ips200_clear();
            Debug_Motor();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,48 , ">");
        }
        else if (Debug_Page_flag_temp == 3)
        {
            ips200_clear();
            Debug_Motor_PID();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,64 , ">");
        }
		else if (Debug_Page_flag_temp == 4)
        {
            ips200_clear();
            Debug_MT9V03x();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,80 , ">");
        }
        else if (Debug_Page_flag_temp == 5)
        {
            ips200_clear();
            Debug_MT9_Track();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,96 , ">");
        }
        else if (Debug_Page_flag_temp == 6)
        {
            ips200_clear();
            Debug_IMU();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,112, ">");
        }
        else if (Debug_Page_flag_temp == 7)
        {
            ips200_clear();
            Debug_Remote_Crtl();
            
            // 从子界面返回后
            ips200_clear();
            Debug_Page_Menu_UI(1);
            ips200_show_string(0  ,112, ">");
        }

        
        /* 显示更新*/
        if (key_pressed)
        {
			// 清理光标
			ips200_show_string(0  ,32 , " ");
			ips200_show_string(0  ,48 , " ");
			ips200_show_string(0  ,64 , " ");
			ips200_show_string(0  ,80 , " ");
			ips200_show_string(0  ,96 , " ");
            ips200_show_string(0  ,112, " ");
            ips200_show_string(0  ,128, " ");
			// 显示光标
			ips200_show_string(0  ,16 + 16*Debug_Page_flag , ">");
        }
    }
}

/**********************************************************/
/*----------------------------------------[E] 菜单逻辑 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 调试逻辑 [S]----------------------------------------*/
/**********************************************************/

//  #   #   ###   ####   #####  
//  #   #  #   #  #   #    #    
//  #   #  #####  ####     #    
//  #   #  #   #  #  #     #    
//   ###   #   #  #   #    #    
//
// 显示一行文本，不足用空格填充，确保覆盖旧内容
static void Debug_UART_ShowLine (uint16 x, uint16 y, uint8 width, const char *str)
{
    char line[31];
    uint8_t i, j = 0;

    // 只复制可打印字符，跳过控制字符，防止字库数组负索引
    for(i = 0; '\0' != str[i] && j < width; i++)
    {
        if(str[i] >= 32 && str[i] <= 126)
            line[j++] = str[i];
    }
    memset(&line[j], ' ', width - j);                                           // 剩余填充空格
    line[width] = '\0';
    ips200_show_string(x, y, line);
}

// [三级界面]串口/蓝牙调试(接收封装包体)
int Debug_UART(void)
{
    Debug_UART_UI();

    char tx_buf[58];                                                            // TX 缓冲区 27+30+1=58
    char rx_disp[58];                                                           // 用于显示的 RX 内容
    uint8_t tx_dirty = 0;
    uint8_t rx_dirty = 0;

    tx_buf[0] = '\0';
    rx_disp[0] = '\0';

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    while(1)
    {
        /* 按键处理 */
        key_clear_state(KEY_UP);                                                // 仅消费标志位
        key_clear_state(KEY_DOWN);                                              // 仅消费标志位
        if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            uart_write_string(UART_6, "TEST_TXT\n");
            strcpy(tx_buf, "TEST_TXT");
            tx_dirty = 1;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            // 返回上一级界面
            return 0;
        }


        /* 接收处理 — 使用 zf_driver_uart 帧解析 */
        {
            char *frame = uart_query_frame(UART_6);
            if(frame)
            {
                strcpy(rx_disp, frame);
                rx_dirty = 1;
            }
        }


        /* 显示更新 - TX */
        if(tx_dirty)
        {
            tx_dirty = 0;
            uint8_t len = strlen(tx_buf);
            Debug_UART_ShowLine(24, 48, 27, tx_buf);                            // TX 第一行 "TX:"后面
            if(len > 27)
                Debug_UART_ShowLine(0, 64, 30, tx_buf + 27);                    // TX 溢出到第二行
            else
                ips200_show_string(0, 64, "                              ");     // 清除溢出行
        }


        /* 显示更新 - RX */
        if(rx_dirty)
        {
            rx_dirty = 0;
            uint8_t len = strlen(rx_disp);
            Debug_UART_ShowLine(24, 80, 27, rx_disp);                           // RX 第一行 "RX:"后面
            if(len > 27)
                Debug_UART_ShowLine(0, 96, 30, rx_disp + 27);                   // RX 溢出到第二行
            else
                ips200_show_string(0, 96, "                              ");
        }
    }
}


//	#   #   ###   #####   ###   ####   
//  ## ##  #   #    #    #   #  #   #  
//  # # #  #   #    #    #   #  ####   
//  #   #  #   #    #    #   #  #  #   
//  #   #   ###     #     ###   #   #  
//
// [三级界面]电机调试
int Debug_Motor (void)
{
    // 电机驱动相关,为方便调用元素数量为3
    int16_t pwm[3] = {0};
    Motor_ALL_Zero();

    Speed_PID_Crtl_Enable = 0;

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    Debug_Motor_UI();
    ips200_show_string(0 ,48 , ">");
    ips200_printf(58 ,48 , "%d  ", pwm[1]);
    ips200_printf(58 ,64 , "%d  ", pwm[2]);

    // 电机调试界面光标 标志位
    // 正常的命名为Debug_Motor_flag，此处进行简化
    uint8_t Debug_M_f = 1;

    while(1)
    {
        // 存储确认键被按下时Debug_M_f的值的临时变量，默认为无效值0
        uint8_t Debug_M_f_temp = 0;
        // 上/下按键是否被按下过
        uint8_t key_pressed = 0;

        /* 按键处理 */
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);
            key_pressed = 1;
            Debug_M_f --;
            if (Debug_M_f < 1){Debug_M_f = 2;}
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN);
            key_pressed = 1;
            Debug_M_f ++;
            if (Debug_M_f > 2){Debug_M_f = 1;}      
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
           Debug_M_f_temp = Debug_M_f;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            Motor_ALL_Zero();
            // 返回上一级界面
            return 0;
        }
            
            
        /* 参数设置 */
        if (Debug_M_f_temp == 1 || Debug_M_f_temp == 2)
        {
            ips200_show_string(0 ,32 + 16*Debug_M_f_temp , "=");
            
            // 电机手动设置
            while(1)
            {
                /* 按键解析 */
                if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
                {
                    key_clear_state(KEY_UP);
                    pwm[Debug_M_f] += 100;
                    if (pwm[Debug_M_f] > 10000)pwm[Debug_M_f] = 10000;
                    Motor_Set(Debug_M_f, pwm[Debug_M_f]);
                    ips200_printf(58 ,32 + 16*Debug_M_f, "%d  ", pwm[Debug_M_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
                {
                    key_clear_state(KEY_DOWN);
                    pwm[Debug_M_f] -= 100;
                    if (pwm[Debug_M_f] < -10000)pwm[Debug_M_f] = -10000;
                    Motor_Set(Debug_M_f, pwm[Debug_M_f]);
                    ips200_printf(58 ,32 + 16*Debug_M_f, "%d  ", pwm[Debug_M_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM) || 
                        KEY_SHORT_PRESS == key_get_state(KEY_BACK))
                {
                    key_clear_state(KEY_CONFIRM);
                    key_clear_state(KEY_BACK);
					ips200_show_string(0 ,32 + 16*Debug_M_f_temp , ">");
                    
                    break;  // 退出修改模式
                }
                
                /* 显示更新 */
                if (Time_Count2 >= 10)// 10ms * 10 周期
                {
                    Time_Count2 = 0;
                    
                    ips200_printf(56 ,96 , "%d  ", ENC_left_CNT * 10);
                    ips200_printf(56 ,112, "%d  ", ENC_right_CNT * 10);
                    // ips200_printf(56 ,128, "%d  ", 0);
                    // ips200_printf(56 ,144, "%d  ", 0);
                }
            }
        }
            
            
        /* 显示更新 */
        if (Time_Count2 >= 10)// 10ms * 10n周期
        {
            Time_Count2 = 0;
            
            ips200_printf(56 ,96 , "%d  ", ENC_left_CNT * 10);
            ips200_printf(56 ,112, "%d  ", ENC_right_CNT * 10);
            // ips200_printf(56 ,128, "%d  ", 0);
            // ips200_printf(56 ,144, "%d  ", 0);
        }
            
            
        /* 光标更新 */
        if (key_pressed)
        {
            // 清理光标
            ips200_show_string(0 ,48 , " ");
            ips200_show_string(0 ,64 , " ");
            // 显示光标
            ips200_show_string(0 ,32 + 16*Debug_M_f  , ">");
        }
    }
}


//	#   #   ###   #####   ###   ####          ####   #####  ####   
//  ## ##  #   #    #    #   #  #   #         #   #    #    #   #  
//  # # #  #   #    #    #   #  ####    ###   ####     #    #   #  
//  #   #  #   #    #    #   #  #  #          #        #    #   #  
//  #   #   ###     #     ###   #   #         #      #####  ####   
//
// [三级界面]电机调试
int Debug_Motor_PID (void)
{
    // PID期望值相关,为方便调用元素数量为3
    int16_t enc_tar[3] = {0};
    // 重置PID中间量
    PID_ALL_Init();
    // 电机速度重置
    Motor_ALL_Zero();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    Speed_PID_Crtl_Enable = 1;

	Debug_Motor_PID_UI();
	ips200_show_string(0 ,48 , ">");
    ips200_printf(58 ,48 , "%d  ", enc_tar[1]);
    ips200_printf(58 ,64 , "%d  ", enc_tar[2]);
    
    // 电机调试界面光标 标志位
    // 正常的命名为Debug_Motor_PID_flag，此处进行简化
    uint8_t Debug_M_P_f = 1;

    while(1)
    {

        // 存储确认键被按下时Debug_M_f的值的临时变量，默认为无效值0
        uint8_t Debug_M_P_f_temp = 0;
        // 上/下按键是否被按下过
        uint8_t key_pressed = 0;

        /* 按键处理 */
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);
            key_pressed = 1;
            Debug_M_P_f --;
            if (Debug_M_P_f < 1){Debug_M_P_f = 2;}
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN);
            key_pressed = 1;
            Debug_M_P_f ++;
            if (Debug_M_P_f > 2){Debug_M_P_f = 1;}      
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            Debug_M_P_f_temp = Debug_M_P_f;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            Speed_PID_Crtl_Enable = 0;
            // 重置PID中间量
            PID_ALL_Init();
            // 电机速度重置
            Motor_ALL_Zero();
            // 返回上一级界面
            return 0;
        }

        
        /* 参数设置 */
        if (Debug_M_P_f_temp == 1 || Debug_M_P_f_temp == 2)
        {
            ips200_show_string(0 ,32 + 16*Debug_M_P_f_temp , "=");
            
            // 电机手动设置
            while(1)
            {
                /* 按键解析 */
                if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
                {
                    key_clear_state(KEY_UP);
                    enc_tar[Debug_M_P_f] += 25;
                    if (enc_tar[Debug_M_P_f] > 800)enc_tar[Debug_M_P_f] = 800;
                    Motor_1_PID.Target = enc_tar[1];
                    Motor_2_PID.Target = enc_tar[2];
                    ips200_printf(58 ,32 + 16*Debug_M_P_f, "%d  ", enc_tar[Debug_M_P_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
                {
                    key_clear_state(KEY_DOWN);
                    enc_tar[Debug_M_P_f] -= 25;
                    if (enc_tar[Debug_M_P_f] < -800)enc_tar[Debug_M_P_f] = -800;
                    Motor_1_PID.Target = enc_tar[1];
                    Motor_2_PID.Target = enc_tar[2];
                    ips200_printf(58 ,32 + 16*Debug_M_P_f, "%d  ", enc_tar[Debug_M_P_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM) || 
                        KEY_SHORT_PRESS == key_get_state(KEY_BACK))
                {
                    key_clear_state(KEY_CONFIRM);
                    key_clear_state(KEY_BACK);
                    ips200_show_string(0 ,32 + 16*Debug_M_P_f_temp , ">");
                    
                    break;  // 退出修改模式
                }


                /* 数据显示 */
                if (Time_Count2 >= 2)// 10ms * 2 周期
                {
                    Time_Count2 = 0;

                    ips200_printf(56 ,96 , "%d  ", (int16_t)Motor_1_PID.Actual);
                    ips200_printf(56 ,112, "%d  ", (int16_t)Motor_2_PID.Actual);
                    ips200_printf(56 ,128, "%d  ", (int16_t)Motor_1_PID.Out);
                    ips200_printf(56 ,144, "%d  ", (int16_t)Motor_2_PID.Out);
					printf("%d,%d,%d\n", (int16_t)Motor_1_PID.Actual, (int16_t)Motor_1_PID.Target, (int16_t)Motor_1_PID.Out);
                }
            }
        }
        

        /* 数据显示 */
        if (Time_Count2 >= 2)// 10 * 2 ms周期
        {
            Time_Count2 = 0;

            ips200_printf(56 ,96 , "%d  ", (int16_t)Motor_1_PID.Actual);
            ips200_printf(56 ,112, "%d  ", (int16_t)Motor_2_PID.Actual);
            ips200_printf(56 ,128, "%d  ", (int16_t)Motor_1_PID.Out);
            ips200_printf(56 ,144, "%d  ", (int16_t)Motor_2_PID.Out);
			printf("%d,%d,%d\n", (int16_t)Motor_1_PID.Actual, (int16_t)Motor_1_PID.Target, (int16_t)Motor_1_PID.Out);
        }
        
        
        /* 光标更新 */
        if (key_pressed)
        {
            // 清理光标
            ips200_show_string(0 ,48 , " ");
            ips200_show_string(0 ,64 , " ");
            // 显示光标
            ips200_show_string(0 ,32 + 16*Debug_M_P_f  , ">");
        }
    }
}


//  #   #  #####  #####  #   #  #####  #####  #   #  
//  ## ##    #    #   #  #   #  #   #      #   # #   
//  # # #    #    #####  #   #  #   #  #####    #    
//  #   #    #        #   # #   #   #      #   # #   
//  #   #    #    #####    #    #####  #####  #   #  
//
// [三级界面]总钻风调试
int Debug_MT9V03x (void)
{
    // 大概率会被覆盖显示,作为保留项目
    Debug_MT9V03x_UI();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    while(1)
    {
        /* 按键处理 */
        key_clear_state(KEY_UP); // 仅消费标志位
        key_clear_state(KEY_DOWN); // 仅消费标志位
        key_clear_state(KEY_CONFIRM); // 仅消费标志位
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            ips200_clear();
            // 返回上一级界面
            return 0;
        }


        /* 总钻风显示 */
        if (Time_Count2 >= 10)// 10ms * 10 周期
        {
            Time_Count2 = 0;

            // 检查新帧
            if(mt9v03x_finish_flag)                              
            {
                mt9v03x_finish_flag = 0;

                // 计时：测量图像传输耗时
                timer_init(TIM_2, TIMER_US);
                timer_start(TIM_2);

                // 图像从 y=32 开始，避开顶部标题行
                ips200_show_gray_image(0, 32,
                    mt9v03x_image[0], MT9V03X_W, MT9V03X_H,
                    MT9V03X_W, MT9V03X_H, 0);

                uint16 show_us = timer_get(TIM_2);
                timer_stop(TIM_2);

                // 在图像下方显示耗时
                ips200_show_uint(72, 160, show_us, 6);
            }
        }
    }
}


//  #   #  #####  #####         #####  ####    ###    ####  #   #  
//  ## ##    #    #   #           #    #   #  #   #  #      #  #   
//  # # #    #    #####   ###     #    ####   #####  #      ###    
//  #   #    #        #           #    #  #   #   #  #      #  #   
//  #   #    #    #####           #    #   #  #   #   ####  #   #  
//
// [三级界面]赛道识别调试
int Debug_MT9_Track     (void)
{
    // 大概率会被覆盖显示,作为保留项目
    Debug_MT9_Track_UI();

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    uint16 g_track_us = 0;
    uint8_t lose_track_flag = 0;
    uint8_t zebra_flag = 0;

    // 识别结果初始化
    TrackRecognition_Init();

    while(1)
    {
        /* 按键处理 */
        key_clear_state(KEY_UP);
        key_clear_state(KEY_DOWN);
        key_clear_state(KEY_CONFIRM);
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            ips200_clear();
            return 0;
        }


        if (Time_Count1 >= 1)// 10ms * 1 处理周期
        {
            Time_Count1 = 0;

            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;
                // 计时开始
                timer_init(TIM_2, TIMER_US);
                timer_start(TIM_2);

                lose_track_flag = Check_LoseTrack();
                TrackRecognition_Process();
                zebra_flag = Check_Zebra();

                // 计时结束
                g_track_us = timer_get(TIM_2);
                timer_stop(TIM_2);
            }
        }


        if (Time_Count2 >= 25)// 10ms * 25 显示周期
        {
            Time_Count2 = 0;

            ips200_show_gray_image(0, 32,
                mt9v03x_image[0], MT9V03X_W, MT9V03X_H,
                MT9V03X_W, MT9V03X_H, TrackRecognition_GetThreshold());
            TrackRecognition_DrawOverlay(32);
            // 显示耗时
            ips200_show_uint(80, 160, g_track_us, 6);
            // 显示偏差值
            ips200_printf(48, 192, "%2.2f", g_track_result.steering_value);
            // 显示是否丢线
            if (lose_track_flag)
            {
                ips200_show_string(48 ,208, "Y");
            }
            else
            {
                ips200_show_string(48 ,208, "N");
            }
            // 显示是否寻找到斑马线
            // 实际上由于显示频率有限制，并不好捕捉zebra_flag == 1
            if (zebra_flag == 1)
            {
                ips200_show_string(48 ,224, "Y1");
            }
            else if (zebra_flag == 2)
            {
                ips200_show_string(48 ,224, "2");
            }
            else
            {
                ips200_show_string(48 ,224, "N ");
            }
        }
    }
}

//  #####  #   #  #   #  
//    #    ## ##  #   #  
//    #    # # #  #   #  
//    #    #   #  #   #  
//  #####  #   #   ###   
//
// [三级界面]IMU963RA原始数据查看界面
int Debug_IMU (void)
{
    Debug_IMU_UI();

    // 干脆总是存在一个没有校准陀螺仪就校准陀螺仪的设定

    /* 半阻塞式IMU963RA零飘此时请保持静此时请保持静止)*/
	if (IMU_Gyro_Calib_Check(&gyro_cal) != GYRO_CALIB_STATE_DONE)// 如果未校准
	{
        ips200_show_string(40 ,32 , "#GYRO#ING~");
	    IMU_Gyro_Calib_Start(&gyro_cal);
	}
	// 半阻塞式零飘校准
	while(1)
    {
        if (IMU_Gyro_Calib_Check(&gyro_cal) == GYRO_CALIB_STATE_DONE)  // 零飘校准完成
        {
            ips200_show_string(40 ,32 , "#GYRO#DONE");
            break;  // 结束零飘校准
        }
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK)) // 强制零飘校准退出
        {
            key_clear_state(KEY_BACK);
            ips200_show_string(40 ,32 , "#GYRO#STOP");
            break;  // 中止零飘校准
        }
    }

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    while(1)
    {
        /* 按键处理 */
        key_clear_state(KEY_UP); // 仅消费标志位
        key_clear_state(KEY_DOWN); // 仅消费标志位
        if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);

            IMU_Gyro_Calib_Start(&gyro_cal);
            ips200_show_string(40 ,32 , "#GYRO#ING~");
            // 半阻塞式零飘校准
            while(1)
            {
                if (IMU_Gyro_Calib_Check(&gyro_cal) == GYRO_CALIB_STATE_DONE)  // 零飘校准完成
                {
                    ips200_show_string(40 ,32 , "#GYRO#DONE");
                    break;  // 结束零飘校准
                }                                                             
                if (KEY_SHORT_PRESS == key_get_state(KEY_BACK)) // 强制零飘校准退出
                {
                    key_clear_state(KEY_BACK);

                    ips200_show_string(40 ,32 , "#GYRO#STOP");
                    break;  // 中止零飘校准
                }        
            }
            IMU_Reset_Data();
        };
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            ips200_clear();
            // 返回上一级界面
            return 0;
        }


        if (Time_Count1 >= 1)// 10ms * 1 解算周期
        {
            Time_Count1 = 0;

            // 读取 IMU 原始数据
            // imu963ra_get_acc();
            // imu963ra_get_gyro();
            // imu963ra_get_mag();
            IMU_Update_Data();

            // 姿态解算
            IMU_Update_Analysis();
        }


        if (Time_Count2 >= 15)// 10ms * 15 显示周期
        {
            Time_Count2 = 0;

            ips200_printf(24 ,64 , "%d   ", imu963ra_acc_x);
            ips200_printf(24 ,80 , "%d   ", imu963ra_acc_y);
            ips200_printf(24 ,96 , "%d   ", imu963ra_acc_z);
			float gx = 0.0f, gy = 0.0f,gz = 0.0f;
			IMU_Gyro_Apply(&gyro_cal, &gx, &gy, &gz);
            ips200_printf(24 ,112, "%d   ", (int)gx);
            ips200_printf(24 ,128, "%d   ", (int)gy);
            ips200_printf(24 ,144, "%d   ", (int)gz);
            // ips200_printf(24 ,160, "%d   ", imu963ra_mag_x);
            // ips200_printf(24 ,176, "%d   ", imu963ra_mag_y);
            // ips200_printf(24 ,192, "%d   ", imu963ra_mag_z);

            ips200_printf(32 ,256, "%.2f  ", Pitch_Result);
        }
    }
}


int Debug_Remote_Crtl   (void)
{
    #define REMOTE_BASE_SPEED  800          // 直行灵敏度(速度环读取实际值为编码器脉冲*10/5ms)
    #define REMOTE_STEER_SPEED 300          // 转向灵敏度(速度环读取实际值为编码器脉冲*10/5ms)

    Debug_Remote_Crtl_UI();

    int8_t LH = 0, LV = 0, RH = 0, RV = 0;
    int16_t tar_left = 0, tar_right = 0;

    Motor_ALL_Zero();
    Speed_PID_Crtl_Enable = 1;

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

    while(1)
    {
        /* 按键处理 */
        key_clear_state(KEY_UP);
        key_clear_state(KEY_DOWN);
        key_clear_state(KEY_CONFIRM);
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            // 停止电机，关闭 PID，返回上一级界面
            Speed_PID_Crtl_Enable = 0;
            Motor_ALL_Zero();
            ips200_clear();
            return 0;
        }

        /* 接收处理 — 解析蓝牙助手 joystick 帧 [joystick,LH,LV,RH,RV] */
        {
            char *frame = uart_query_frame(UART_6);
            if(frame)
            {
                char *tag = strtok(frame, ",");
                if(tag && strcmp(tag, "joystick") == 0)
                {
                    LH = (int8_t)atoi(strtok(NULL, ","));
                    LV = (int8_t)atoi(strtok(NULL, ","));
                    RH = (int8_t)atoi(strtok(NULL, ","));
                    RV = (int8_t)atoi(strtok(NULL, ","));

                    // 差速映射：LV=油门(前后) RH=转向(左右)
                    tar_left  = (int16_t)(LV * REMOTE_BASE_SPEED  / 100 + RH * REMOTE_STEER_SPEED / 100);
                    tar_right = (int16_t)(LV * REMOTE_BASE_SPEED  / 100 - RH * REMOTE_STEER_SPEED / 100);

                    // 限幅
                    if(tar_left  >  REMOTE_BASE_SPEED) tar_left  =  REMOTE_BASE_SPEED;
                    if(tar_left  < -REMOTE_BASE_SPEED) tar_left  = -REMOTE_BASE_SPEED;
                    if(tar_right >  REMOTE_BASE_SPEED) tar_right =  REMOTE_BASE_SPEED;
                    if(tar_right < -REMOTE_BASE_SPEED) tar_right = -REMOTE_BASE_SPEED;

                    Motor_1_PID.Target = tar_left;
                    Motor_2_PID.Target = tar_right;

                    Time_Count1 = 0;                                            // 收到有效帧，重置断连计时
                }
            }
        }

        /* 断连保护：0.5s 无有效帧则停车 */
        if (Time_Count1 >= 50)                                                  // 10ms * 50 = 500ms
        {
            Time_Count1 = 50;                                                   // 上限保持，避免溢出回绕
            tar_left  = 0;
            tar_right = 0;
            Motor_1_PID.Target = 0;
            Motor_2_PID.Target = 0;
        }

        /* 显示更新 */
        if (Time_Count2 >= 25)                                                  // 10ms * 25 = 250ms 周期
        {
            Time_Count2 = 0;

            ips200_printf(24 ,48 , "%4d  ", LH);
            ips200_printf(96 ,48 , "%4d  ", LV);
            ips200_printf(24 ,64 , "%4d  ", RH);
            ips200_printf(96 ,64 , "%4d  ", RV);
            ips200_printf(48 ,80 , "%-5d ", tar_left);
            ips200_printf(120,80 , "%-5d ", tar_right);
            ips200_printf(48 ,96 , "%-5d ", (int16_t)Motor_1_PID.Actual);
            ips200_printf(120,96 , "%-5d ", (int16_t)Motor_2_PID.Actual);

            if(Time_Count1 >= 50)
                ips200_show_string(0 ,112, "TIMEOUT - NO SIGNAL");
            else
                ips200_show_string(0 ,112, "                   ");
        }
    }
}
/**********************************************************/
/*----------------------------------------[E] 调试逻辑 [E]*/
/**********************************************************/
