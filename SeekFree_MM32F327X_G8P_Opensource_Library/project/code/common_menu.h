/*******************************************************************************
链式通用菜单框架
参考:【手把手教菜单_框架搭建】 https://www.bilibili.com/video/BV1LCh9zrEMp/?share_source=copy_web&vd_source=befff269c2e0ef1583e2528659770521
*******************************************************************************/


#ifndef __COMMON_MENU_H__
#define __COMMON_MENU_H__


#include "zf_common_headfile.h"

typedef enum MENU_KIND {
    MENU_Folder = 0,
    int32_Box,
    uint32_Box,
    int16_Box,
    uint16_Box,
    int8_Box,
    uint8_Box,
    float_Box,
    bool_Box,
}MENU_KIND;

typedef struct Menu_Item {
    const char *name;
    void *data;                         // 指向存放的变量
    MENU_KIND kind;                     // 记录节点属性

    uint8_t sons;                       // 记录子节点数量
    uint8_t No;                         // 记录当前是父节点的第几个成员
    bool select;                        // 是否被选中的标志位(参数修改的功能)

    struct Menu_Item *father;           // 父节点
    struct Menu_Item *first_son;        // 第一个子节点
    struct Menu_Item *next_brother;     // 下一个兄弟节点
    struct Menu_Item *last_brother;     // 上一个兄弟节点
}Menu_Item;

void        Create_Menu_Folder          (Menu_Item *father, Menu_Item *me, const char name[]);
void        Create_Menu_Number          (Menu_Item *father, Menu_Item *me, const char name[], void *data, MENU_KIND kind);
Menu_Item*  DynamicCreate_Menu_Folder   (Menu_Item *father, const char name[]);
void        DynamicCreate_Menu_Number   (Menu_Item *father, const char name[], void *data, MENU_KIND kind);
void        menu_init                   (void);
void        menu_show                   (void);
void        key_1                       (void);
void        key_2                       (void);
void        key_3                       (void);
void        key_quit                    (void);


#endif
