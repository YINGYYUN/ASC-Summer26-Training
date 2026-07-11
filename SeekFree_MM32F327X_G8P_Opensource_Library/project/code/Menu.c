/*******************************************************************************
主菜单
*******************************************************************************/


#include "zf_common_headfile.h"

//----------------------------------------------------------
// 函数简介     外设初始化
// 使用示例     Peripheral_Init();
// 备注信息     只在"main.c"中调用一次
//----------------------------------------------------------
void Peripheral_Init(void)
{
    // IPS200 初始化 (SPI 接口串口两寸屏)
	ips200_init(IPS200_TYPE_SPI);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);       // 白色字，黑色背景
    ips200_clear();                                     // 清屏为黑色
	
	// 按键初始化(10ms扫描周期)
    key_init(10);

	
}


/**********************************************************/
/*[S] 界面样式 [S]----------------------------------------*/
/**********************************************************/

// [一级界面]主菜单界面
void Menu_UI(uint8_t Page)
{
	switch(Page)
	{		
		// 第一页
		case 1:
		{
            ips200_show_string(8  ,0  , "[Menu]");
            ips200_show_string(0  ,16 , "==============================");
            ips200_show_string(10 ,32 , "Process");
            ips200_show_string(10 ,48 , "Debug");

			break;
		}
	}
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
    ips200_clear();
    Menu_UI(1);
    ips200_show_string(0  ,32 , ">");

    while(1)
    {
        // 存储确认键被按下时menuflag的值的临时变量，默认为无效值0
		uint8_t menu_flag_temp = 0;
		// 上/下按键是否被按下过
		uint8_t key_pressed = 0;

        /* 按键处理*/
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
        // 仅消费标志位
        key_clear_state(KEY_BACK); 


        /* 模式跳转*/
        if (menu_flag_temp == 1)
        {
            ips200_clear();


            // 从Process模式返回，显示主菜单界面
            ips200_clear();
            Menu_UI(1);
            ips200_show_string(0  ,32 , ">");
        }
        else if (menu_flag_temp == 2)
        {
            ips200_clear();
            Debug_Page_Menu();

            // 从Debug模式返回，显示主菜单界面
            ips200_clear();
            Menu_UI(1);
            ips200_show_string(0  ,48 , ">");
        }


        /* 显示更新*/
        if (key_pressed)
        {
			// 首先清除其他光标
			ips200_show_string(0  ,32 , " ");
			ips200_show_string(0  ,48 , " ");
			
            switch (menu_flag)
            {
                // 在Process模式的光标
                case 1:
                    ips200_show_string(0  ,32 , ">");
                    break;

                // 在Debug模式的光标
                case 2:
                    ips200_show_string(0  ,48 , ">");
                    break;
            }
        }
    }
}
/**********************************************************/
/*----------------------------------------[E] 菜单逻辑 [E]*/
/**********************************************************/
