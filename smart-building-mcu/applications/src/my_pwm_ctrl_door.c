/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-12     Mr-L       the first version
 */
#include <my_pwm_ctrl_door.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <stdbool.h>

#include <my_data_upload.h>

struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */

uint8_t angle = 0;
rt_uint32_t period, pulse;

void my_pwm_init(void)
{
    rt_err_t result = RT_NULL;
    period = 20*1000000;    /* 周期为20ms，单位为纳秒ns */
    pulse = 0.5*1000000;            /* PWM脉冲宽度值，单位为纳秒ns */
    /* 查找设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }

    /* 设置PWM周期和脉冲宽度默认值 */
    result = rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    if(result != RT_EOK){
        rt_kprintf("pwm set failed!\n");
    }
    /* 使能设备 */
    result = rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    if(result != RT_EOK){
        rt_kprintf("pwm set failed!\n");
    }
    rt_kprintf("PWM Init success!\n");
}

void my_pwm_set_angle(uint8_t angles)
{
    uint32_t pulses = 0;
    rt_err_t result = RT_NULL;
    if(angles <= 0){
        angles = 5;
    }
    if(angles >= 180){
        angles = 170;
    }
    // 根据舵机的角度与占空比的转化公式进行转换
    pulses = (uint32_t)(1000000*(0.5 + angles*(2.5-0.5)/180.0));
    /* 设置PWM周期和脉冲宽度默认值 */
    result = rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulses);
//    rt_kprintf("pwm set angle result is %d\n",result);
}

void my_pwm_angle_change(bool direction, uint8_t angles)
{
    if(direction){    // 开门，角度+
        if(angle < 180){
            angle += angles;
        }
    }else if(!direction){ // 关门，角度-
        if(angle > 0 ){
            angle -= angles;
        }
    }
    Set_Devpro_DoorState(direction);
    my_pwm_set_angle(angle);
}

