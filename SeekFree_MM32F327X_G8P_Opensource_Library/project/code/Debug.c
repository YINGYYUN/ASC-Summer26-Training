/*******************************************************************************
调试
*******************************************************************************/


#include "zf_common_headfile.h"
#include "TrackRecognition.h"

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
	ips200_show_string(0  ,96 , "ENC 1:");
	ips200_show_string(0  ,112, "ENC 2:");
    ips200_show_string(0  ,128, "SUM 1:");
	ips200_show_string(0  ,144, "SUM 2:");
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
    ips200_show_string(0  ,176, "Show(us):");
    ips200_show_string(0  ,192, "Steer:");
}
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
            if (Debug_Page_flag < 1)Debug_Page_flag = 5;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN); 
            key_pressed = 1;
            Debug_Page_flag ++;
            if (Debug_Page_flag > 5)Debug_Page_flag = 1;
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

        
        /* 显示更新*/
        if (key_pressed)
        {
			// 清理光标
			ips200_show_string(0  ,32 , " ");
			ips200_show_string(0  ,48 , " ");
			ips200_show_string(0  ,64 , " ");
			ips200_show_string(0  ,80 , " ");
			ips200_show_string(0  ,96 , " ");
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
    Motor_Set(1,0);
    Motor_Set(2,0);

    // 编码器相关,为方便调用元素数量为3
    int32_t enc_cur[3] = {0};
    int32_t enc_sum[3] = {0};
    ENC1_CLEAR();
    ENC2_CLEAR();

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

            Motor_Set(1,0);
            Motor_Set(2,0);
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
                
                /* 电机编码器读取 */
                if (Time_Count2 >= 2)// 10ms * 2 周期
                {
                    Time_Count2 = 0;
                    
                    enc_cur[1] = ENC1_GET();
                    enc_cur[2] = ENC2_GET();
                    ENC1_CLEAR();
                    ENC2_CLEAR();
                    enc_sum[1] += enc_cur[1];
                    enc_sum[2] += enc_cur[2];
                    ips200_printf(58 ,96 , "%d  ", enc_cur[1]);
                    ips200_printf(58 ,112, "%d  ", enc_cur[2]);
                    ips200_printf(58 ,128, "%d  ", enc_sum[1]);
                    ips200_printf(58 ,144, "%d  ", enc_sum[2]);
                }
            }
        }
            
            
        /* 电机编码器读取 */
        if (Time_Count2 >= 2)// 10ms * 2 周期
        {
            Time_Count2 = 0;
            
            enc_cur[1] = ENC1_GET();
            enc_cur[2] = ENC2_GET();
            ENC1_CLEAR();
            ENC2_CLEAR();
            enc_sum[1] += enc_cur[1];
            enc_sum[2] += enc_cur[2];
            ips200_printf(58 ,96 , "%d  ", enc_cur[1]);
            ips200_printf(58 ,112, "%d  ", enc_cur[2]);
            ips200_printf(58 ,128, "%d  ", enc_sum[1]);
            ips200_printf(58 ,144, "%d  ", enc_sum[2]);
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
    // 重置中间量
    PID_INC_Init(&Motor_1_PID);    
    PID_INC_Init(&Motor_2_PID);

    // // 暂时方法,有其他替代方法时，需要注意此处的直接赋值
    // Motor_1_PID.Kp = 25;
    // Motor_1_PID.Ki = 2;
    // Motor_1_PID.Kd = 8;

    // Motor_2_PID.Kp = 25;
    // Motor_2_PID.Ki = 2;
    // Motor_2_PID.Kd = 8;

    // 编码器相关,为方便调用元素数量为3
    int16_t enc_cur[3] = {0};
    ENC1_CLEAR();
    ENC2_CLEAR();

    // 电机速度重置
    Motor_Set(1,0);
    Motor_Set(2,0);

    // 参考计时值重置
    Time_Count1 = 0;
    Time_Count2 = 0;

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

            // 各项重置
            PID_INC_Init(&Motor_1_PID);    
            PID_INC_Init(&Motor_2_PID);
            Motor_Set(1,0);
            Motor_Set(2,0);
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
                    enc_tar[Debug_M_P_f] += 50;
                    if (enc_tar[Debug_M_P_f] > 1000)enc_tar[Debug_M_P_f] = 1000;
                    ips200_printf(58 ,32 + 16*Debug_M_P_f, "%d  ", enc_tar[Debug_M_P_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
                {
                    key_clear_state(KEY_DOWN);
                    enc_tar[Debug_M_P_f] -= 50;
                    if (enc_tar[Debug_M_P_f] < -1000)enc_tar[Debug_M_P_f] = -1000;
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

                
                /* 电机编码器读取 */
                if (Time_Count1 >= 2)// 10ms * 2 周期
                {
                    Time_Count1 = 0;
                    
                    enc_cur[1] = ENC1_GET();
                    enc_cur[2] = ENC2_GET();
                    ENC1_CLEAR();
                    ENC2_CLEAR();

                    // PID计算
                    Motor_1_PID.Actual = enc_cur[1];
                    Motor_1_PID.Target = enc_tar[1];
                    PID_INC_Update(&Motor_1_PID);
                    Motor_Set(1, (int16_t)Motor_1_PID.Out);
                    
                    Motor_2_PID.Actual = enc_cur[2];
                    Motor_2_PID.Target = enc_tar[2];
                    PID_INC_Update(&Motor_2_PID);
                    Motor_Set(2, (int16_t)Motor_2_PID.Out);
                }


                /* 数据显示 */
                if (Time_Count2 >= 10)// 10ms * 10 周期
                {
                    Time_Count2 = 0;

                    ips200_printf(58 ,96 , "%d  ", enc_cur[1]);
                    ips200_printf(58 ,112, "%d  ", enc_cur[2]);
                    ips200_printf(58 ,128, "%d  ", (int16_t)Motor_1_PID.Out);
                    ips200_printf(58 ,144, "%d  ", (int16_t)Motor_2_PID.Out);
                }
            }
        }
        
            
        /* 电机编码器读取 */
        if (Time_Count1 >= 2)// 10ms * 2 = 20ms 周期
        {
            Time_Count1 = 0;
            
            enc_cur[1] = ENC1_GET();
            enc_cur[2] = ENC2_GET();
            ENC1_CLEAR();
            ENC2_CLEAR();

            // PID计算
            Motor_1_PID.Actual = enc_cur[1];
            Motor_1_PID.Target = enc_tar[1];
            PID_INC_Update(&Motor_1_PID);
            Motor_Set(1, (int16_t)Motor_1_PID.Out);
            
            Motor_2_PID.Actual = enc_cur[2];
            Motor_2_PID.Target = enc_tar[2];
            PID_INC_Update(&Motor_2_PID);
            Motor_Set(2, (int16_t)Motor_2_PID.Out);
        }


        /* 数据显示 */
        if (Time_Count2 > 10)// 10 * 10 ms周期
        {
            Time_Count2 = 0;

            ips200_printf(58 ,96 , "%d  ", enc_cur[1]);
            ips200_printf(58 ,112, "%d  ", enc_cur[2]);
            ips200_printf(58 ,128, "%d  ", (int16_t)Motor_1_PID.Out);
            ips200_printf(58 ,144, "%d  ", (int16_t)Motor_2_PID.Out);
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

        
        if (Time_Count1 >= 10)// 10ms * 10 周期
        {
            Time_Count1 = 0;
        }

        
        if (Time_Count2 >= 10)// 10ms * 10 周期
        {
            Time_Count2 = 0;

            // ---- 图像数据读取 + 赛道识别 ----
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;

                // 计时1：测量赛道识别处理耗时
                timer_init(TIM_2, TIMER_US);
                timer_start(TIM_2);

                // 执行赛道识别
                TrackRecognition_Process();

                uint16 track_us = timer_get(TIM_2);
                timer_stop(TIM_2);

                // 计时2：测量图像显示耗时
                timer_start(TIM_2);

                // 图像从 y=32 开始，避开顶部标题行
                // 传入当前 Otsu 阈值显示二值化图像，与算法看到的完全一致
                ips200_show_gray_image(0, 32,
                    mt9v03x_image[0], MT9V03X_W, MT9V03X_H,
                    MT9V03X_W, MT9V03X_H, TrackRecognition_GetThreshold());

                // 叠加赛道边界/中线（对齐图像偏移）
                TrackRecognition_DrawOverlay(32);

                uint16 show_us = timer_get(TIM_2);
                timer_stop(TIM_2);

                // 在图像下方显示耗时和转角
                ips200_show_uint(80, 160, track_us, 6);
                ips200_show_uint(72, 176, show_us, 6);
                ips200_printf(48, 192, "%2.2f", g_track_result.steering_value);
            }
        }
    }
}
/**********************************************************/
/*----------------------------------------[E] 调试逻辑 [E]*/
/**********************************************************/
