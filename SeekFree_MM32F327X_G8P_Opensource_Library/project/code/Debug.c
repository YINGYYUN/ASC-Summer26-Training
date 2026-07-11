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
			ips200_show_string(10 ,32 , "UART");
		
			break;
	}
}

// [三级界面]UART调试界面 (串口/蓝牙)
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
/**********************************************************/
/*----------------------------------------[E] 界面样式 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 菜单逻辑 [S]----------------------------------------*/
/**********************************************************/

// 相关函数提前声明
int Debug_UART         (void);


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
            if (Debug_Page_flag < 1)Debug_Page_flag = 1;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN); 
            key_pressed = 1;
            Debug_Page_flag ++;
            if (Debug_Page_flag > 1)Debug_Page_flag = 1;
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

        

        /* 显示更新*/
        if (key_pressed)
        {
            switch (Debug_Page_flag)
            {
                case 1:
                    ips200_clear();
                    Debug_Page_Menu_UI(1);
                    ips200_show_string(0  ,32 , ">");

                    break;
                case 2:
                    ips200_clear();
                    Debug_Page_Menu_UI(1);
                    ips200_show_string(0  ,48 , ">");

                    break;
                case 3:
                    ips200_clear();
                    Debug_Page_Menu_UI(1);
                    ips200_show_string(0  ,64 , ">");

                    break;
                case 4:
                    ips200_clear();
                    Debug_Page_Menu_UI(1);
                    ips200_show_string(0  ,80 , ">");

                    break;
                case 5:
                    ips200_clear();
                    Debug_Page_Menu_UI(1);
                    ips200_show_string(0  ,96 , ">");

                    break;
            }
        }
    }
}

/**********************************************************/
/*----------------------------------------[E] 菜单逻辑 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 调试逻辑 [S]----------------------------------------*/
/**********************************************************/

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
/**********************************************************/
/*----------------------------------------[E] 调试逻辑 [E]*/
/**********************************************************/
