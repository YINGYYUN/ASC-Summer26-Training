/*******************************************************************************
链式通用菜单框架
参考:【手把手教菜单_框架搭建】 https://www.bilibili.com/video/BV1LCh9zrEMp/?share_source=copy_web&vd_source=befff269c2e0ef1583e2528659770521
*******************************************************************************/


#include "zf_common_headfile.h"


Menu_Item head = {
    .name           = "MENU",
    .data           = NULL,
    .kind           = MENU_Folder,

    .sons           = 0,
    .No             = 0,
    .select         = false,

    .father         = NULL,
    .first_son      = NULL,
    .next_brother   = NULL,
    .last_brother   = NULL,
};
Menu_Item *key;

/**********************************************************/
/*[S] 节点创建函数 [S]--------------------------------------*/
/**********************************************************/

#define MENU_MAX_SIZE           (64)

static Menu_Item menu_item_arr[MENU_MAX_SIZE];
static uint8_t menu_arr_index = 0;

// 初始化节点成员
static void Create_Menu_Item(Menu_Item *father, Menu_Item *me, const char name[], void *data, MENU_KIND kind)
{
    if (father->kind != MENU_Folder) return;

    me->name = name;
    me->data = data;
    me->kind = kind;

    me->sons = 0;
    me->select = false;

    me->father = father;
    me->first_son = NULL;       // 第一个子节点先设为空
    me->next_brother = NULL;
    me->last_brother = NULL;

    // 开始判定连接
    if (father->sons == 0)
    {
        father->first_son = me;
    }
    else
    {
        Menu_Item *p = father->first_son;
        while(p->next_brother != NULL)
            p = p->next_brother;
        p->next_brother = me;
        me->last_brother = p;
    }
    me->No = father->sons;
    father->sons++;
}

// 创建文件夹
void Create_Menu_Folder(Menu_Item *father, Menu_Item *me, const char name[])
{
    Create_Menu_Item(father, me, name, NULL, MENU_Folder);
}

// 创建文件
void Create_Menu_Number(Menu_Item *father, Menu_Item *me, const char name[], void *data, MENU_KIND kind)
{
    Create_Menu_Item(father, me, name, data, kind);
}

// 动态创建文件夹
Menu_Item* DynamicCreate_Menu_Folder(Menu_Item *father, const char name[])
{
    // 分配内存池
    Menu_Item *me = &menu_item_arr[menu_arr_index++];
    Create_Menu_Item(father, me, name, NULL, MENU_Folder);

    return me;
}

// 动态创建文件
void DynamicCreate_Menu_Number(Menu_Item *father, const char name[], void *data, MENU_KIND kind)
{
    // 分配内存池
    Menu_Item *me = &menu_item_arr[menu_arr_index++];
    Create_Menu_Item(father, me, name, data, kind);
}
/**********************************************************/
/*--------------------------------------[E] 节点创建函数 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 显示调用函数 [S]--------------------------------------*/
/**********************************************************/

// 显示光标
void show_key(void)
{
    Menu_Item *h = key->father;
    Menu_Item *s = h->first_son;

    for(int i = 0;i < h->sons; i++)
    {
        if (s == key)
        {
            ips200_show_string(0  , i*16, ">");
        }
        else
        {
            ips200_show_string(0  , i*16, " ");
        }
        s = s->next_brother;
    }   

}

// 显示存储的参数
void show_number(void)
{
    Menu_Item *h = key->father;
    Menu_Item *s = h->first_son;

    for(int i = 0;i < h->sons; i++)
    {
        if (s->select)
            ips200_show_string(142 , i*16, "[");
        else
            ips200_show_string(142 , i*16, " ");

        switch (s->kind)
        {
            case int32_Box:
                ips200_printf(150 , i*16, "%05d", *(int32_t *)s->data);
                break;

            case float_Box:
                ips200_printf(150 , i*16, "%3.2f", *(float *)s->data);
                break;

            case bool_Box:
                if(*(bool *)s->data == true)
                    ips200_printf(150 , i*16, "Y");
                else
                    ips200_printf(150 , i*16, "N");
                break;

            default:
                break;
        }
        s = s->next_brother;
    }
}

void menu_show(void)
{
    Menu_Item *h = key->father;
    Menu_Item *s = h->first_son;

    for(int i = 0;i < h->sons; i++)
    {
        ips200_show_string(10 , i*16, s->name);
        s = s->next_brother;
    }

    show_number();
    show_key();
}
/**********************************************************/
/*--------------------------------------[E] 显示调用函数 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 按键一次封装 [S]--------------------------------------*/
/**********************************************************/

// 光标上移
// 需要在按键判定中调用
void key_up(void)
{
    if (key->last_brother != NULL)
        key = key->last_brother;
}

// 光标下移
// 需要在按键判定中调用
void key_down(void)
{
    if (key->next_brother != NULL)
        key = key->next_brother;
}

// 进入菜单
// 需要在按键判定中调用
void key_enter(void)
{
    if (key->sons > 0)
    {
        key = key->first_son;
        ips200_clear();
    } 
}

// 退出菜单(回到上一级)
// 需要在按键判定中调用
void key_quit(void)
{
    if (key->father != NULL && key->father->father != NULL)
    {
        key = key->father;
        ips200_clear();
    }
}

// 选中/取消选中
void key_select(void)
{
    if (key->kind != MENU_Folder && key->kind != bool_Box)
        key->select = !key->select;
}

// 参数加
void key_plus(void)
{
    switch (key->kind)
    {
        case int32_Box:
            *(int32_t *)key->data += 1;
            break;

        default:
            break;
    }
}

// 参数减
void key_sub(void)
{
    switch (key->kind)
    {
        case int32_Box:
            *(int32_t *)key->data -= 1;
            break;

        default:
            break;
    }
}
/**********************************************************/
/*--------------------------------------[E] 按键一次封装 [E]*/
/**********************************************************/


/**********************************************************/
/*[S] 按键二次封装 [S]--------------------------------------*/
/**********************************************************/

void key_1(void)
{
    if (key->select == false)
        // 光标上移
        key_up();
    else
        // 参数加
        key_plus();
}

void key_2(void)
{
    if (key->select == false)
        // 光标下移
        key_down();
    else
        // 参数减
        key_sub();
}

void key_3(void)
{
    if (key->kind == MENU_Folder)
        key_enter();
    else
        key_select();
}
/**********************************************************/
/*--------------------------------------[E] 按键二次封装 [E]*/
/**********************************************************/




// 调用测试

Menu_Item menu6;
Menu_Item menu7;
Menu_Item menu8;

int test = 10;
float test1 = 2.2;
bool test2 = false;

// 初始化菜单(连接菜单节点)
void menu_init(void)
{
    Menu_Item *folder_1 = DynamicCreate_Menu_Folder(&head, "folder1");
    DynamicCreate_Menu_Folder(&head, "folder2");
    DynamicCreate_Menu_Folder(&head, "folder3");
    DynamicCreate_Menu_Folder(&head, "folder4");

    DynamicCreate_Menu_Number(folder_1, "aaa", &test, int32_Box);
    DynamicCreate_Menu_Number(folder_1, "bbb", &test1, float_Box);
    DynamicCreate_Menu_Number(folder_1, "ccc", &test2, bool_Box);

    key = head.first_son; // 初始化光标key的位置
}
