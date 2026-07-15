/*******************************************************************************
DRV8701 电机驱动
*******************************************************************************/


#include "zf_common_headfile.h"

// 电机驱动引脚初始化
void Motor_init (void)
{
    gpio_init(MOTOR_1_DIR_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_2_DIR_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pwm_init(MOTOR_1_PWM_CHANNEL, MOTOR_PWM_FREQ, 0);
    pwm_init(MOTOR_2_PWM_CHANNEL, MOTOR_PWM_FREQ, 0);
}

//----------------------------------------------------------
// 函数简介     设置电机速度
// 使用示例     Motor_Set(1, 2500);
// 参数说明     motor范围：1-2
// 参数说明     duty范围：-10000~10000（注：duty本意是没有负数的）
//----------------------------------------------------------
void Motor_Set (uint8 motor, int16 duty)
{
    gpio_pin_enum    dir_pin;// 方向引脚
    pwm_channel_enum pwm_ch;// PWM引脚

    if(motor == 1) // 电机1
    {
        dir_pin = MOTOR_1_DIR_PIN;
        pwm_ch  = MOTOR_1_PWM_CHANNEL;
    }
    else // 电机2
    {
        dir_pin = MOTOR_2_DIR_PIN;
        pwm_ch  = MOTOR_2_PWM_CHANNEL;
    }

    if(duty > 0)
    {
        gpio_high(dir_pin);
    }
    else
    {
        gpio_low(dir_pin);
        duty = -duty;
    }

    if(duty > PWM_DUTY_MAX) // 限幅
    {
        duty = PWM_DUTY_MAX;
    }

    pwm_set_duty(pwm_ch, (uint16)duty);
}
