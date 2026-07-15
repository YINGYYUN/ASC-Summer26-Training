/*******************************************************************************
可更改参数菜单
部分参数在对应的模块文件中定义, 此处通过 extern 引用
*******************************************************************************/


#include "zf_common_headfile.h"


//从模块文件中引用(示例)
// 目前并非实际应用参数

// Speed_PID 参数
static float speed_kp = 1.0f;
static float speed_ki = 0.1f;
static float speed_kd = 0.01f;

//Turn_PID 参数
static float turn_kp  = 2.0f;
static float turn_ki  = 0.2f;
static float turn_kd  = 0.02f;

//Other 杂项
static int32_t  other_aaa  = 100;
static float    other_bbb  = 3.14f;
static bool     other_ccc  = true;


// 构建参数页面
static void param_page_init(void)
{
    menu_reset();       // 清空旧菜单

    // ---------- Speed_PID 文件夹 ----------
    Menu_Item *folder_speed = DynamicCreate_Menu_Folder(&head, "Speed_PID");
    DynamicCreate_Menu_LimitNumber(folder_speed, "S_Kp", &speed_kp, float_Box, 0, 100);
    DynamicCreate_Menu_LimitNumber(folder_speed, "S_Ki", &speed_ki, float_Box, 0, 10);
    DynamicCreate_Menu_LimitNumber(folder_speed, "S_Kd", &speed_kd, float_Box, 0, 1);

    // ---------- Turn_PID 文件夹 ----------
    Menu_Item *folder_turn = DynamicCreate_Menu_Folder(&head, "Turn_PID");
    DynamicCreate_Menu_LimitNumber(folder_turn, "T_Kp", &turn_kp, float_Box, 0, 100);
    DynamicCreate_Menu_LimitNumber(folder_turn, "T_Ki", &turn_ki, float_Box, 0, 10);
    DynamicCreate_Menu_LimitNumber(folder_turn, "T_Kd", &turn_kd, float_Box, 0, 1);

    // ---------- Other 文件夹 ----------
    Menu_Item *folder_other = DynamicCreate_Menu_Folder(&head, "Other");
    DynamicCreate_Menu_Number(folder_other, "aaa", &other_aaa, int32_Box);
    DynamicCreate_Menu_Number(folder_other, "bbb", &other_bbb, float_Box);
    DynamicCreate_Menu_Number(folder_other, "ccc", &other_ccc, bool_Box);

    key = head.first_son;   // 光标移到第一项
}


// 参数设置页面主入口
int Param_Page_Menu(void)
{
    param_page_init();          // 构建菜单
    menu_show_all();            // 清屏 + 刷新

    while(1)
    {
        // 短按: 上键
        if (KEY_SHORT_PRESS == key_get_state(KEY_UP))
        {
            key_clear_state(KEY_UP);
            key_up_btn();
        }
        // 短按: 下键
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN);
            key_down_btn();
        }
        // 短按: 确认键 → 进入文件夹 / 选中参数 / 翻转 bool
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            key_enter_btn();
        }
        // 短按: 返回键
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            if (key->select == true)
            {
                // 正在修改参数 → 切换步进值
                key_quit_btn();
            }
            else if (key->father->father == NULL)
            {
                // 未选中 且 已在根层 → 退出参数页面
                return 0;
            }
            else
            {
                // 未选中 且在子文件夹内 → 回退到上一级
                key_quit_btn();
            }
        }

        menu_show();            // 刷新显示
    }
}
