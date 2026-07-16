/*******************************************************************************
可更改参数菜单
参数缓存区(param_cache) → Flash 持久化, 通过 Param_Storage 宏访问
*******************************************************************************/


#include "zf_common_headfile.h"
#include "Param_Storage.h"


// 构建参数页面
static void param_page_init(void)
{
    menu_reset();       // 清空旧菜单

    // ---------- 电机1 PID ----------
    Menu_Item *folder_m1 = DynamicCreate_Menu_Folder(&head, "Motor1_PID");
    DynamicCreate_Menu_LimitNumber(folder_m1, "Kp", &MOTOR1_KP, float_Box, 0, 60);
    DynamicCreate_Menu_LimitNumber(folder_m1, "Ki", &MOTOR1_KI, float_Box, 0, 10);
    DynamicCreate_Menu_LimitNumber(folder_m1, "Kd", &MOTOR1_KD, float_Box, 0, 20);

    // ---------- 电机2 PID ----------
    Menu_Item *folder_m2 = DynamicCreate_Menu_Folder(&head, "Motor2_PID");
    DynamicCreate_Menu_LimitNumber(folder_m2, "Kp", &MOTOR2_KP, float_Box, 0, 60);
    DynamicCreate_Menu_LimitNumber(folder_m2, "Ki", &MOTOR2_KI, float_Box, 0, 10);
    DynamicCreate_Menu_LimitNumber(folder_m2, "Kd", &MOTOR2_KD, float_Box, 0, 20);

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
            menu_show();
        }
        // 短按: 下键
        else if (KEY_SHORT_PRESS == key_get_state(KEY_DOWN))
        {
            key_clear_state(KEY_DOWN);
            key_down_btn();
            menu_show();
        }
        // 短按: 确认键 → 进入文件夹 / 选中参数 / 翻转 bool
        else if (KEY_SHORT_PRESS == key_get_state(KEY_CONFIRM))
        {
            key_clear_state(KEY_CONFIRM);
            key_enter_btn();
            menu_show();
        }
        // 短按: 返回键
        else if (KEY_SHORT_PRESS == key_get_state(KEY_BACK))
        {
            key_clear_state(KEY_BACK);

            if (key->select == true)
            {
                // 正在修改参数 → 切换步进值
                key_quit_btn();
                menu_show();
            }
            else if (key->father->father == NULL)
            {
                // 未选中 且 已在根层 → 保存并退出
                Param_Save();           // 持久化到 Flash
                Flash_SyncTo_Param();   // 同步到使用中的参数(比如PID)
                return 0;
            }
            else
            {
                // 未选中 且在子文件夹内 → 回退到上一级
                key_quit_btn();
                menu_show();
            }
        }
    }
}
