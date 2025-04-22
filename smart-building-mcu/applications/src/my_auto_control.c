/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-10     Mr-L       the first version
 */
#include <my_auto_control.h>
#include <my_data_upload.h>
#include <my_multiThread.h>
#include <my_pwm_ctrl_door.h>

Devices_t *device_status = RT_NULL; // 设备状态结构体
Environment_t *environmenr = RT_NULL; // 环境数据结构体
Thresholds_t *thresholds = RT_NULL; // 阈值信息结构体

void my_auto_control_entry(void *parameter)
{
    device_status = Get_Device_Properties(); // 使用指针形式，直接获取同一份设备状态结构体
    environmenr = Get_Environment_Properties(); // 使用指针形式，直接获取同一份环境数据结构体
    thresholds = Get_Thresholds_Properties(); // 使用指针形式，直接获取同一份阈值信息结构体

    while (1)
    {
        // 获取最新的传感器数据和相应的数据阈值

        if(device_status->DHT11_Sensor_State){  // 硬件未故障，数据才有效
            // 判断温度数据是否超出阈值
            if(!device_status->Beep_State && !device_status->Door_State){      //警报器和逃生门未开启，需要判断是否超出阈值
                if(environmenr->temp > thresholds->Temp_Limit){
//                    my_send_temp_feedback(1);   //向TCP服务器发送反馈信息
//                    beep_open();    //开启蜂鸣器
//                    rt_kprintf("ready to change beep open");

                    // 向后端服务器发送告警信息——温度过高
                    Publish_Alarm_Info(ALARM_TEMPERATURE_TOO_HIGH,environmenr->temp,thresholds->Temp_Limit);

                    beep_change(true); //开启蜂鸣器和警示灯
                    my_pwm_angle_change(true,90); // 开启应急逃生门
                    // 控制外设后向服务器发布更新外设状态的消息
                    Publish_Device_Status(device_status);
                }
            }else if(device_status->Beep_State && device_status->Door_State){     //警报器和逃生门已经开启，需要判断数据是否回到正常阈值下
                if( environmenr->temp < thresholds->Temp_Limit){
//                    my_send_temp_feedback(0);   //向TCP服务器发送反馈信息
//                    beep_close();   //关闭蜂鸣器
//                    rt_kprintf("ready to change beep close");

                    // 向后端服务器发送告警信息——温度回归正常
                    Publish_Alarm_Info(ALARM_TEMPERATURE_BACK_NORMAL,environmenr->temp,thresholds->Temp_Limit);

                    beep_change(false); //关闭蜂鸣器和警示灯
                    my_pwm_angle_change(false,90); // 关闭应急逃生门
                    // 控制外设后向服务器发布更新外设状态的消息
                    Publish_Device_Status(device_status);
                }
            }
//             判断湿度数据是否超出阈值
            if(!device_status->Relay_State){      //继电器未开启，需要判断是否超出阈值
                if(environmenr->humi > thresholds->Humi_Limit){
//                    my_send_temp_feedback(1);   //向TCP服务器发送反馈信息
//                    beep_open();    //开启蜂鸣器

                    // 向后端服务器发送告警信息——湿度过高
                    Publish_Alarm_Info(ALARM_HUMIDITY_TOO_HIGH,environmenr->humi,thresholds->Humi_Limit);

                      relay_change(true);   //开启继电器
                      // 控制外设后向服务器发布更新外设状态的消息
                      Publish_Device_Status(device_status);
                }
            }else if(device_status->Relay_State){     //继电器已经开启，需要判断数据是否回到正常阈值下
                if( environmenr->humi < thresholds->Humi_Limit){
//                    my_send_temp_feedback(0);   //向TCP服务器发送反馈信息
//                    beep_close();   //关闭蜂鸣器

                    // 向后端服务器发送告警信息——湿度回归正常
                    Publish_Alarm_Info(ALARM_HUMIDITY_BACK_NORMAL,environmenr->humi,thresholds->Humi_Limit);

                    relay_change(false);    //关闭继电器
                    // 控制外设后向服务器发布更新外设状态的消息
                    Publish_Device_Status(device_status);
                }
            }
        }

        if(device_status->Light_Sensor_State){  // 硬件未故障，数据才有效
            // 判断光照强度数据是否超出阈值
            if(!device_status->LED1_State){   //应急灯未开启，需要判断是否超出阈值
                if( environmenr->light < thresholds->Light_Down_Limit){
//                    my_send_light_feedback(1);   //向TCP服务器发送反馈信息
//                    emergency_light_open(); //开启应急灯

                    // 向后端服务器发送告警信息——光照过弱
                    Publish_Alarm_Info(ALARM_LIGHT_TOO_WEAK,environmenr->light,thresholds->Light_Down_Limit);

                    emergency_light_change(true); //开启应急灯
                    // 控制外设后向服务器发布更新外设状态的消息
                    Publish_Device_Status(device_status);
                }
            }else if(device_status->LED1_State){     //应急灯已经开启，需要判断数据是否回到正常阈值下
                if( environmenr->light > thresholds->Light_Up_Limit){
//                    my_send_light_feedback(0);   //向TCP服务器发送反馈信息
//                    emergency_light_close(); //关闭应急灯

                    // 向后端服务器发送告警信息——光照过强
                    Publish_Alarm_Info(ALARM_LIGHT_TOO_STRONG,environmenr->light,thresholds->Light_Up_Limit);

                    emergency_light_change(false); //关闭应急灯
                    // 控制外设后向服务器发布更新外设状态的消息
                    Publish_Device_Status(device_status);
                }
            }
        }

        rt_thread_delay(2000);
    }
}

int my_auto_control_init(void)
{
    rt_thread_t auto_control_thread;
    auto_control_thread = rt_thread_create("my_auto_control",
                                     my_auto_control_entry,
                                     RT_NULL,
                                     2048,
                                     RT_THREAD_PRIORITY_MAX / 2,
                                     20);
    if (auto_control_thread != RT_NULL)
    {
        rt_thread_startup(auto_control_thread);
        rt_kprintf("auto control thread created success\n");
    }else{
        rt_kprintf("auto control thread created fail\n");
    }
    return RT_EOK;
}

void show_dev_data(void)
{
    rt_kprintf("dht11 sensor state is %d\n",device_status->DHT11_Sensor_State);
    rt_kprintf("light sensor state is %d\n",device_status->Light_Sensor_State);
    rt_kprintf("temp limit is %d    humi limit is %d\n",thresholds->Temp_Limit,thresholds->Humi_Limit);
    rt_kprintf("light up limit is %d   down limit is %d\n",thresholds->Light_Up_Limit,thresholds->Light_Down_Limit);

}
MSH_CMD_EXPORT(show_dev_data,show dev data);
