/*******************************************************************************
参数 Flash 存储 — 实现
*******************************************************************************/

#include "Param_Storage.h"

// 需要引用实际 PID 结构体以完成同步
extern PID_INC_t Motor_1_PID;   // 电机1 PID (左轮)
extern PID_INC_t Motor_2_PID;   // 电机2 PID (右轮)


// 默认参数值(首次使用或恢复出厂设置时使用)
static const float DEFAULT_PARAMS[PARAM_COUNT] = {
    // Motor_1_PID
    25.0f, 2.0f, 8.0f,       // Kp, Ki, Kd
    // Motor_2_PID
    25.0f, 2.0f, 8.0f,       // Kp, Ki, Kd
    // 预留(默认 0)
    0.0f, 0.0f, 0.0f, 0.0f,
};

// 参数缓存区(菜单直接修改此数组, Flash 读写也通过此数组)
float param_cache[PARAM_COUNT];


//------------------- 内部辅助函数 -------------------

static void load_default(void)
{
    for (uint8_t i = 0; i < PARAM_COUNT; i++)
        param_cache[i] = DEFAULT_PARAMS[i];
}

static void copy_cache_to_flash_buffer(void)
{
    for (uint8_t i = 0; i < PARAM_COUNT; i++)
        flash_union_buffer[i].float_type = param_cache[i];
}

static void copy_flash_buffer_to_cache(void)
{
    for (uint8_t i = 0; i < PARAM_COUNT; i++)
        param_cache[i] = flash_union_buffer[i].float_type;
}

// 校验参数是否合法(异常值/全零 Kp 视为无效)
static uint8_t param_cache_is_valid(void)
{
    uint8_t kp_nonzero = 0;

    for (uint8_t i = 0; i < PARAM_COUNT; i++)
    {
        // 范围检查
        if (param_cache[i] > 10000.0f || param_cache[i] < -10000.0f)
            return 0;

        // 计数非零 Kp(任意一个 Kp 非零即认为有效)
        if (i == MOTOR1_KP_IDX || i == MOTOR2_KP_IDX)
        {
            if (param_cache[i] > 0.0001f || param_cache[i] < -0.0001f)
                kp_nonzero = 1;
        }
    }

    return kp_nonzero;
}


//------------------- 对外函数 -------------------

// 初始化参数系统(上电调用一次)
void Param_Init(void)
{
    if (flash_check(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE))
    {
        // Flash 有数据 → 读取到缓冲区
        flash_read_page_to_buffer(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE);
        copy_flash_buffer_to_cache();

        // 校验失败 → 回退默认值并修复 Flash
        if (!param_cache_is_valid())
        {
            load_default();
            copy_cache_to_flash_buffer();
            flash_write_page_from_buffer(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE);
        }
    }
    else
    {
        // 首次使用 → 写入默认值
        load_default();
        copy_cache_to_flash_buffer();
        flash_write_page_from_buffer(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE);
    }

    // 将参数推送到 PID 结构体
    Flash_SyncTo_Param();
}

// 保存参数到 Flash(退出参数页面时调用)
void Param_Save(void)
{
    copy_cache_to_flash_buffer();
    flash_write_page_from_buffer(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE);
}

// 擦除 Flash(下次启动恢复默认)
void Param_Erase(void)
{
    flash_erase_page(PARAM_FLASH_SECTION, PARAM_FLASH_PAGE);
}

// 将缓存区值同步到实际 PID 结构体
void Flash_SyncTo_Param(void)
{
    Motor_1_PID.Kp = MOTOR1_KP;
    Motor_1_PID.Ki = MOTOR1_KI;
    Motor_1_PID.Kd = MOTOR1_KD;

    Motor_2_PID.Kp = MOTOR2_KP;
    Motor_2_PID.Ki = MOTOR2_KI;
    Motor_2_PID.Kd = MOTOR2_KD;
}
