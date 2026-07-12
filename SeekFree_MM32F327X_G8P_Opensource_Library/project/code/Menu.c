/*******************************************************************************
主菜单
*******************************************************************************/


#include "zf_common_headfile.h"
#include "zf_device_ips200pro.h"

// ---- Pro 组件 ID ----
static uint16 lb_menu_ui;
static uint8  ui_created = 0;            // 组件是否已创建
       uint16 page_main;                 // 主菜单页面
       uint16 page_debug;                // Debug 页面
       uint16 page_uart;                 // UART 调试页面
       uint16 page_motor;                // Motor 调试页面
       uint16 page_motor_pid;            // Motor_PID 调试页面
       uint16 page_mt9v03x;              // MT9V03x 图像页面
       uint16 page_mt9_track;            // MT9-Track 赛道识别页面

//----------------------------------------------------------
// 函数简介     外设初始化
// 使用示例     Peripheral_Init();
// 备注信息     只在"main.c"中调用一次
//----------------------------------------------------------
void Peripheral_Init(void)
{
    // IPS200 Pro 初始化
    page_main      = ips200pro_init("", IPS200PRO_TITLE_TOP, 0);
    page_debug     = ips200pro_page_create("Debug");
    page_uart      = ips200pro_page_create("UART");
    page_motor     = ips200pro_page_create("Motor");
    page_motor_pid = ips200pro_page_create("MotorPID");
    page_mt9v03x   = ips200pro_page_create("MT9V03x");
    page_mt9_track = ips200pro_page_create("Track");
    
	// 按键初始化(10ms扫描周期)
    key_init(10);

    // CH-04蓝牙 / 串口初始化
    uart_init(UART_6, 9600, UART6_TX_C6, UART6_RX_C7);
    uart_printf(UART_6, "CH-04 ready, baud=%d\r\n", 9600);
	
	// 电机驱动初始化
	Motor_init();
	
	// 编码器1: B4 + B5 → TIM3
	encoder_quad_init(ENCODER_1, ENC_1_P_CH1, ENC_1_P_CH2);
	// 编码器2: B6 + B7 → TIM4
	encoder_quad_init(ENCODER_2, ENC_2_P_CH1, ENC_2_P_CH2);

    // 1. 初始化总钻风摄像头（自动检测 UART/SCCB 通信方式，配置内部寄存器）
    uint8 ret = mt9v03x_init();
}


/**********************************************************/
/*[S] 界面样式 [S]----------------------------------------*/
/**********************************************************/

// [一级界面]主菜单界面 — IPS200 Pro 版（光标嵌入文字）
static void Menu_UI_Pro(uint8_t flag)
{
    ips200pro_page_switch(page_main, PAGE_ANIM_OFF);

    if (!ui_created)
    {
        lb_menu_ui = ips200pro_label_create(0, 0, 240, 64);
        ips200pro_set_font(lb_menu_ui, FONT_SIZE_14);
        ips200pro_label_mode(lb_menu_ui, LABEL_CLIP);
        ui_created = 1;
    }

    // 手动拼接字符串，避免 printf 多行兼容问题
    char buf[64];
    snprintf(buf, sizeof(buf),
        "  [Menu]\n"
        "====================\n"
        "%c Process\n"
        "%c Debug",
        flag == 1 ? '>' : ' ',
        flag == 2 ? '>' : ' ');
    ips200pro_label_show_string(lb_menu_ui, buf);
}
/**********************************************************/
/*----------------------------------------[E] 界面样式 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 菜单逻辑 [S]----------------------------------------*/
/**********************************************************/

// 菜单选项 标志位
uint8_t menu_flag = 1;

void Menu_Show(void)
{
    Menu_UI_Pro(1);

    while(1)
    {
        // 存储确认键被按下时menuflag的值的临时变量，默认为无效值0
		uint8_t menu_flag_temp = 0;
		// 上/下按键是否被按下过
		uint8_t key_pressed = 0;

        
        /* 按键处理 */
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);
            key_pressed = 1;
            menu_flag --;
            if (menu_flag < 1)menu_flag = 2;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN); 
            key_pressed = 1;
            menu_flag ++;
            if (menu_flag > 2)menu_flag = 1;
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            menu_flag_temp = menu_flag;
        }
        key_clear_state(KEY_BACK); // 仅消费标志位


        /* 模式跳转 */
        if (menu_flag_temp == 1)
        {
            // Process 模式（暂无，直接重建 UI）
            Menu_UI_Pro(1);
        }
        else if (menu_flag_temp == 2)
        {
            Debug_Page_Menu();

            // 从Debug模式返回，重建主菜单界面
            Menu_UI_Pro(2);
        }


        /* 光标更新 */
        if (key_pressed)
        {
            Menu_UI_Pro(menu_flag);
        }
    }
}
/**********************************************************/
/*----------------------------------------[E] 菜单逻辑 [E]*/
/**********************************************************/
