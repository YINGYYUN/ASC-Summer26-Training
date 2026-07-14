/*******************************************************************************
链式通用菜单框架
参考:【手把手教菜单_框架搭建】 https://www.bilibili.com/video/BV1LCh9zrEMp/?share_source=copy_web&vd_source=befff269c2e0ef1583e2528659770521
*******************************************************************************/


#include "zf_common_headfile.h"


// 初始化节点成员
static void Create_Menu_Item(Menu_Item *father, Menu_Item *me, const char name[], void *data, MENU_KIND kind)
{
    if (father->kind != MENU_Folder) return;

    me->name = name;
    me->sons = 0;

    me->father = father;
    me->first_son = NULL;       // 第一个子节点先设为空
    me->next_brother = NULL;
    me->last_brother = NULL;

    me->data = data;
    me->kind = kind;

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


// 调用测试
Menu_Item head;
Menu_Item menu1;
Menu_Item menu2;
Menu_Item menu3;
Menu_Item menu4;
Menu_Item menu5;

Menu_Item menu6;
Menu_Item menu7;
Menu_Item menu8;

int test = 10;
float test1 = 2.2;
bool test2 = false;

void menu_init(void)
{
    Create_Menu_Folder(&head, &menu1, "hello1");
    Create_Menu_Folder(&head, &menu2, "hello2");
    Create_Menu_Folder(&head, &menu3, "hello3");
    Create_Menu_Number(&head, &menu6, "aaa", &test, int32_Box);
    Create_Menu_Number(&head, &menu7, "bbb", &test1, float_Box);
    Create_Menu_Number(&head, &menu8, "ccc", &test2, bool_Box);

    Create_Menu_Folder(&menu1, &menu4, "hello1");
    Create_Menu_Folder(&menu1, &menu5, "hello2");
}

void show_number(void)
{
    Menu_Item *h = &head;
    Menu_Item *s = head.first_son;

    for(int i = 0;i < h->sons; i++)
    {
        switch (s->kind)
        {
            case int32_Box:
                ips200_printf(80, i*16, "%05d", *(int32_t *)s->data);
                break;

            case float_Box:
                ips200_printf(80, i*16, "%3.2f", *(float *)s->data);
                break;

            case bool_Box:
                if(*(bool *)s->data == true)
                    ips200_printf(80, i*16, "Y");
                else
                    ips200_printf(80, i*16, "N");
                break;

            default:
                break;
        }
        s = s->next_brother;
    }
}

void menu_show(void)
{
    Menu_Item *h = &head;
    Menu_Item *s = head.first_son;

    for(int i = 0;i < h->sons; i++)
    {
        ips200_show_string(0, i*16, s->name);
        s = s->next_brother;
    }

    s = h->first_son;
    s = s->first_son;
    h = h->first_son;

    for(int i = 0;i < h->sons; i++)
    {
        ips200_show_string(0, i*16, s->name);
        s = s->next_brother;
    }

    show_number();
}
