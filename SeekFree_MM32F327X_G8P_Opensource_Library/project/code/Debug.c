/*******************************************************************************
调试
*******************************************************************************/


#include "zf_common_headfile.h"
#include "TrackRecognition.h"
#include "zf_device_ips200pro.h"

extern uint16 page_debug;      // 在 Menu.c 中创建
extern uint16 page_uart;
extern uint16 page_motor;
extern uint16 page_motor_pid;
extern uint16 page_mt9v03x;
extern uint16 page_mt9_track;

// ---- Pro 组件 ID（Debug 页面）----
static uint16 lb_dbg_ui;
static uint8  dbg_ui_created = 0;

// 子界面标签 ID
static uint16 lb_sub_ui;        // 静态文字
static uint16 lb_sub_val[6];    // 动态数值
static uint16 lb_sub_img;       // 图像组件
static uint8  sub_created = 0;  // 是否已创建
static uint16 sub_last_page = 0;// 上次创建时所属页面

// 进入子页面，跨页面自动重建组件
static void SubPage_Enter(uint16 page, const char *label_text)
{
    ips200pro_page_switch(page, PAGE_ANIM_OFF);

    // 不同页面或首次 → 重建所有标签
    if (page != sub_last_page)
    {
        sub_created = 0;
        sub_last_page = page;
    }

    if (!sub_created)
    {
        lb_sub_ui = ips200pro_label_create(0, 0, 240, 160);
        ips200pro_set_font(lb_sub_ui, FONT_SIZE_14);
        ips200pro_label_mode(lb_sub_ui, LABEL_CLIP);
        ips200pro_label_show_string(lb_sub_ui, label_text);

        for (int i = 0; i < 6; i++)
        {
            lb_sub_val[i] = ips200pro_label_create(58, 48 + i * 16, 80, 20);
            ips200pro_set_font(lb_sub_val[i], FONT_SIZE_14);
            ips200pro_label_mode(lb_sub_val[i], LABEL_CLIP);
        }
        sub_created = 1;
    }
}

// 进入带图像组件的子页面
static void SubPage_EnterImg(uint16 page, const char *label_text, uint16 img_w, uint16 img_h)
{
    SubPage_Enter(page, label_text);

    if (!lb_sub_img)
    {
        lb_sub_img = ips200pro_image_create(0, 30, img_w, img_h);
    }
}

// 刷新数值
static void SubPage_UpdateVal(uint8 idx, int32_t val)
{
    ips200pro_label_printf(lb_sub_val[idx], "%d", val);
}

// 刷新图像
static void SubPage_UpdateImg(const void *img, uint16 w, uint16 h, ips200pro_image_type_enum type, uint8 th)
{
    ips200pro_image_display(lb_sub_img, img, w, h, type, th);
}


/**********************************************************/
/*[S] 界面样式 [S]----------------------------------------*/
/**********************************************************/

// [二级界面]Debug模式界面 — IPS200 Pro 版（光标嵌入文字）
static void Debug_Page_Menu_UI_Pro(uint8_t flag)
{
    ips200pro_page_switch(page_debug, PAGE_ANIM_OFF);

    if (!dbg_ui_created)
    {
        lb_dbg_ui = ips200pro_label_create(0, 0, 240, 130);
        ips200pro_set_font(lb_dbg_ui, FONT_SIZE_14);
        ips200pro_label_mode(lb_dbg_ui, LABEL_CLIP);
        dbg_ui_created = 1;
    }

    // 手动拼接字符串，避免 printf 多行兼容问题
    char buf[128];
    snprintf(buf, sizeof(buf),
        "  [Debug]\n"
        "======================\n"
        "%c UART\n"
        "%c Motor\n"
        "%c Motor_PID\n"
        "%c MT9V03x\n"
        "%c MT9-Track",
        flag == 1 ? '>' : ' ',
        flag == 2 ? '>' : ' ',
        flag == 3 ? '>' : ' ',
        flag == 4 ? '>' : ' ',
        flag == 5 ? '>' : ' ');
    ips200pro_label_show_string(lb_dbg_ui, buf);
}

// ---- 三级界面 UI 全部转为 Pro 版本 ----
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


// [二级界面]Debug模式界面 — IPS200 Pro 版
int Debug_Page_Menu(void)
{
    // Debug模式选项 标志位
    uint8_t Debug_Page_flag = 1;

    Debug_Page_Menu_UI_Pro(1);

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
            Debug_UART();
            Debug_Page_Menu_UI_Pro(1);
        }
		else if (Debug_Page_flag_temp == 2)
        {
            Debug_Motor();
            Debug_Page_Menu_UI_Pro(2);
        }
        else if (Debug_Page_flag_temp == 3)
        {
            Debug_Motor_PID();
            Debug_Page_Menu_UI_Pro(3);
        }
		else if (Debug_Page_flag_temp == 4)
        {
            Debug_MT9V03x();
            Debug_Page_Menu_UI_Pro(4);
        }
        else if (Debug_Page_flag_temp == 5)
        {
            Debug_MT9_Track();
            Debug_Page_Menu_UI_Pro(5);
        }

        
        /* 光标更新 */
        if (key_pressed)
        {
            Debug_Page_Menu_UI_Pro(Debug_Page_flag);
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
// [三级界面]串口/蓝牙调试 — Pro 版
int Debug_UART(void)
{
    SubPage_Enter(page_uart,
        "  [DEBUG]-UART\n"
        "======================\n"
        "  [Press CONFIRM to TX]\n"
        "TX:\n"
        "\n"
        "RX:");

    while(1)
    {
        key_clear_state(KEY_UP);
        key_clear_state(KEY_DOWN);
        if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            uart_write_string(UART_6, "TEST_TXT\n");
            ips200pro_label_show_string(lb_sub_val[0], "TEST_TXT");
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            return 0;
        }

        char *frame = uart_query_frame(UART_6);
        if (frame)
        {
            ips200pro_label_show_string(lb_sub_val[1], frame);
        }
    }
}


//	#   #   ###   #####   ###   ####   
//  ## ##  #   #    #    #   #  #   #  
//  # # #  #   #    #    #   #  ####   
//  #   #  #   #    #    #   #  #  #   
//  #   #   ###     #     ###   #   #  
//
// [三级界面]电机调试 — Pro 版
int Debug_Motor (void)
{
    int16_t pwm[3] = {0};
    Motor_Set(1,0);
    Motor_Set(2,0);

    int32_t enc_cur[3] = {0};
    int32_t enc_sum[3] = {0};
    ENC1_CLEAR();
    ENC2_CLEAR();
    Time_Count1 = 0;
    Time_Count2 = 0;

    SubPage_Enter(page_motor,
        "  [DEBUG]-Motor\n"
        "======================\n"
        "\n"
        "PWM 1:\n"
        "PWM 2:\n"
        "\n"
        "ENC 1:\n"
        "ENC 2:\n"
        "SUM 1:\n"
        "SUM 2:");

    SubPage_UpdateVal(0, pwm[1]);
    SubPage_UpdateVal(1, pwm[2]);
    SubPage_UpdateVal(2, enc_cur[1]);
    SubPage_UpdateVal(3, enc_cur[2]);
    SubPage_UpdateVal(4, enc_sum[1]);
    SubPage_UpdateVal(5, enc_sum[2]);

    uint8_t Debug_M_f = 1;

    while(1)
    {
        uint8_t Debug_M_f_temp = 0;
        uint8_t key_pressed = 0;

        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
            { key_clear_state(KEY_UP); key_pressed = 1; Debug_M_f --; if (Debug_M_f < 1)Debug_M_f = 2; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
            { key_clear_state(KEY_DOWN); key_pressed = 1; Debug_M_f ++; if (Debug_M_f > 2)Debug_M_f = 1; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
            { key_clear_state(KEY_CONFIRM); Debug_M_f_temp = Debug_M_f; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            Motor_Set(1,0);
            Motor_Set(2,0);
            return 0;
        }

        if (Debug_M_f_temp == 1 || Debug_M_f_temp == 2)
        {
            while(1)
            {
                if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
                {
                    key_clear_state(KEY_UP);
                    pwm[Debug_M_f] += 100;
                    if (pwm[Debug_M_f] > 10000) pwm[Debug_M_f] = 10000;
                    Motor_Set(Debug_M_f, pwm[Debug_M_f]);
                    SubPage_UpdateVal(Debug_M_f - 1, pwm[Debug_M_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
                {
                    key_clear_state(KEY_DOWN);
                    pwm[Debug_M_f] -= 100;
                    if (pwm[Debug_M_f] < -10000) pwm[Debug_M_f] = -10000;
                    Motor_Set(Debug_M_f, pwm[Debug_M_f]);
                    SubPage_UpdateVal(Debug_M_f - 1, pwm[Debug_M_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM)
                      || KEY_SHORT_PRESS == key_get_state(KEY_BACK))
                {
                    key_clear_state(KEY_CONFIRM);
                    key_clear_state(KEY_BACK);
                    break;
                }

                if (Time_Count2 > 5)
                {
                    Time_Count2 = 0;
                    enc_cur[1] = ENC1_GET(); enc_cur[2] = ENC2_GET();
                    ENC1_CLEAR(); ENC2_CLEAR();
                    enc_sum[1] += enc_cur[1]; enc_sum[2] += enc_cur[2];
                    SubPage_UpdateVal(2, enc_cur[1]);
                    SubPage_UpdateVal(3, enc_cur[2]);
                    SubPage_UpdateVal(4, enc_sum[1]);
                    SubPage_UpdateVal(5, enc_sum[2]);
                }
            }
        }

        if (Time_Count2 > 5)
        {
            Time_Count2 = 0;
            enc_cur[1] = ENC1_GET(); enc_cur[2] = ENC2_GET();
            ENC1_CLEAR(); ENC2_CLEAR();
            enc_sum[1] += enc_cur[1]; enc_sum[2] += enc_cur[2];
            SubPage_UpdateVal(2, enc_cur[1]);
            SubPage_UpdateVal(3, enc_cur[2]);
            SubPage_UpdateVal(4, enc_sum[1]);
            SubPage_UpdateVal(5, enc_sum[2]);
        }
    }
}


//	#   #   ###   #####   ###   ####          ####   #####  ####   
//  ## ##  #   #    #    #   #  #   #         #   #    #    #   #  
//  # # #  #   #    #    #   #  ####    ###   ####     #    #   #  
//  #   #  #   #    #    #   #  #  #          #        #    #   #  
//  #   #   ###     #     ###   #   #         #      #####  ####   
//
// [三级界面]电机 PID 调试 — Pro 版
int Debug_Motor_PID (void)
{
    int16_t enc_tar[3] = {0};
    PID_INC_Init(&Motor_1_PID);
    PID_INC_Init(&Motor_2_PID);
    Motor_1_PID.Kp = 25; Motor_1_PID.Ki = 2; Motor_1_PID.Kd = 8;
    Motor_2_PID.Kp = 25; Motor_2_PID.Ki = 2; Motor_2_PID.Kd = 8;

    int16_t enc_cur[3] = {0};
    ENC1_CLEAR(); ENC2_CLEAR();
    Motor_Set(1,0); Motor_Set(2,0);
    Time_Count1 = 0; Time_Count2 = 0;

    SubPage_Enter(page_motor_pid,
        "  [DEBUG]-Motor-PID\n"
        "======================\n"
        "\n"
        "TAR 1:\n"
        "TAR 2:\n"
        "\n"
        "ENC 1:\n"
        "ENC 2:\n"
        "PWM 1:\n"
        "PWM 2:");

    SubPage_UpdateVal(0, enc_tar[1]);
    SubPage_UpdateVal(1, enc_tar[2]);
    SubPage_UpdateVal(2, enc_cur[1]);
    SubPage_UpdateVal(3, enc_cur[2]);
    SubPage_UpdateVal(4, (int32_t)Motor_1_PID.Out);
    SubPage_UpdateVal(5, (int32_t)Motor_2_PID.Out);

    uint8_t Debug_M_P_f = 1;

    while(1)
    {
        uint8_t Debug_M_P_f_temp = 0;
        uint8_t key_pressed = 0;

        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
            { key_clear_state(KEY_UP); key_pressed = 1; Debug_M_P_f --; if (Debug_M_P_f < 1)Debug_M_P_f = 2; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
            { key_clear_state(KEY_DOWN); key_pressed = 1; Debug_M_P_f ++; if (Debug_M_P_f > 2)Debug_M_P_f = 1; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
            { key_clear_state(KEY_CONFIRM); Debug_M_P_f_temp = Debug_M_P_f; }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            PID_INC_Init(&Motor_1_PID); PID_INC_Init(&Motor_2_PID);
            Motor_Set(1,0); Motor_Set(2,0);
            return 0;
        }

        if (Debug_M_P_f_temp == 1 || Debug_M_P_f_temp == 2)
        {
            while(1)
            {
                if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
                {
                    key_clear_state(KEY_UP);
                    enc_tar[Debug_M_P_f] += 20;
                    if (enc_tar[Debug_M_P_f] > 1000) enc_tar[Debug_M_P_f] = 1000;
                    SubPage_UpdateVal(Debug_M_P_f - 1, enc_tar[Debug_M_P_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
                {
                    key_clear_state(KEY_DOWN);
                    enc_tar[Debug_M_P_f] -= 20;
                    if (enc_tar[Debug_M_P_f] < -1000) enc_tar[Debug_M_P_f] = -1000;
                    SubPage_UpdateVal(Debug_M_P_f - 1, enc_tar[Debug_M_P_f]);
                }
                else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM)
                      || KEY_SHORT_PRESS == key_get_state(KEY_BACK))
                {
                    key_clear_state(KEY_CONFIRM);
                    key_clear_state(KEY_BACK);
                    break;
                }

                if (Time_Count1 > 5)
                {
                    Time_Count1 = 0;
                    enc_cur[1] = ENC1_GET(); enc_cur[2] = ENC2_GET();
                    ENC1_CLEAR(); ENC2_CLEAR();
                    Motor_1_PID.Actual = enc_cur[1]; Motor_1_PID.Target = enc_tar[1];
                    PID_INC_Update(&Motor_1_PID); Motor_Set(1, (int16_t)Motor_1_PID.Out);
                    Motor_2_PID.Actual = enc_cur[2]; Motor_2_PID.Target = enc_tar[2];
                    PID_INC_Update(&Motor_2_PID); Motor_Set(2, (int16_t)Motor_2_PID.Out);
                }
                if (Time_Count2 > 10)
                {
                    Time_Count2 = 0;
                    SubPage_UpdateVal(2, enc_cur[1]);
                    SubPage_UpdateVal(3, enc_cur[2]);
                    SubPage_UpdateVal(4, (int32_t)Motor_1_PID.Out);
                    SubPage_UpdateVal(5, (int32_t)Motor_2_PID.Out);
                }
            }
        }

        if (Time_Count1 > 5)
        {
            Time_Count1 = 0;
            enc_cur[1] = ENC1_GET(); enc_cur[2] = ENC2_GET();
            ENC1_CLEAR(); ENC2_CLEAR();
            Motor_1_PID.Actual = enc_cur[1]; Motor_1_PID.Target = enc_tar[1];
            PID_INC_Update(&Motor_1_PID); Motor_Set(1, (int16_t)Motor_1_PID.Out);
            Motor_2_PID.Actual = enc_cur[2]; Motor_2_PID.Target = enc_tar[2];
            PID_INC_Update(&Motor_2_PID); Motor_Set(2, (int16_t)Motor_2_PID.Out);
        }
        if (Time_Count2 > 10)
        {
            Time_Count2 = 0;
            SubPage_UpdateVal(2, enc_cur[1]);
            SubPage_UpdateVal(3, enc_cur[2]);
            SubPage_UpdateVal(4, (int32_t)Motor_1_PID.Out);
            SubPage_UpdateVal(5, (int32_t)Motor_2_PID.Out);
        }
    }
}


//  #   #  #####  #####  #   #  #####  #####  #   #  
//  ## ##    #    #   #  #   #  #   #      #   # #   
//  # # #    #    #####  #   #  #   #  #####    #    
//  #   #    #        #   # #   #   #      #   # #   
//  #   #    #    #####    #    #####  #####  #   #  
//
// [三级界面]总钻风调试 — Pro 版
int Debug_MT9V03x (void)
{
    SubPage_EnterImg(page_mt9v03x,
        "  [DEBUG]-MT9V03x\n"
        "======================",
        MT9V03X_W, MT9V03X_H);

    while(1)
    {
        key_clear_state(KEY_UP);
        key_clear_state(KEY_DOWN);
        key_clear_state(KEY_CONFIRM);
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            return 0;
        }

        if (Time_Count2 > 10)
        {
            Time_Count2 = 0;
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;
                SubPage_UpdateImg(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, IMAGE_GRAYSCALE, 0);
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
// [三级界面]赛道识别调试 — Pro 版
int Debug_MT9_Track(void)
{
    SubPage_EnterImg(page_mt9_track,
        "  [DEBUG]-MT9-Track\n"
        "======================",
        MT9V03X_W, MT9V03X_H);

    while(1)
    {
        key_clear_state(KEY_UP);
        key_clear_state(KEY_DOWN);
        key_clear_state(KEY_CONFIRM);
        if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);
            return 0;
        }

        if (Time_Count1 > 10)
        {
            Time_Count1 = 0;
        }

        if (Time_Count2 > 10)
        {
            Time_Count2 = 0;
            if(mt9v03x_finish_flag)
            {
                mt9v03x_finish_flag = 0;
                TrackRecognition_Process();
                TrackRecognition_DrawOverlay();
                SubPage_UpdateImg(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, IMAGE_GRAYSCALE, 0);
            }
        }
    }
}

/**********************************************************/
/*----------------------------------------[E] 调试逻辑 [E]*/
/**********************************************************/
