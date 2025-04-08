/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-27     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <board.h>

#include <my_multiThread.h>
#include <my_oled.h>
#include <my_adc.h>
#include <my_dht11.h>
#include <my_mqtt.h>
#include <my_data_upload.h>
#include <my_auto_control.h>
#include <my_pwm_ctrl_door.h>


#define LED0_PIN    GET_PIN(F, 9)

int main(void)
{
    my_GPIO_Init();     //初始化GPIO引脚
    my_oled_init();     //初始化OLED
    my_light_adc();     //开启光敏传感器ADC线程采集数据
    my_rt_hw_dht11_port();          // 初始化DHT11模块
    my_dht11_read_temp_sample();    // 开启采集温湿度数据线程
    my_pwm_init();      // 初始化PWM模块

    // 连接OneNET云平台
//    onenet_mqtt_init();
    // 连接本地搭建的MQTT Broker代理
    mqtt_start();

    rt_thread_mdelay(4000);
    // 数据上传OneNET云平台线程初始化
    my_data_upload_init();

    // 自动监控线程初始化
    my_auto_control_init();

    while(1){
//        rt_pin_write(LED0_PIN, 1-rt_pin_read(LED0_PIN));//翻转LED0
//        LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
