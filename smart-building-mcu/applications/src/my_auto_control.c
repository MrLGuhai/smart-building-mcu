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


void my_auto_control_entry(void *parameter)
{
    int temp,humi,light;
    int Temp_Limit,Humi_Limit,Light_Up_Limit,Light_Down_Limit;
    bool Beep_State,Relay_State,LED0_State,LED1_State,Door_State,DHT11_Error_State,Light_Error_State;
    while (1)
    {
        // 获取最新的传感器数据和相应的数据阈值
        temp = Get_Devpro_temp();
        humi = Get_Devpro_humi();
        light = Get_Devpro_light();
        Temp_Limit = Get_Devpro_Temp_Limit();
        Humi_Limit = Get_Devpro_Humi_Limit();
        Light_Up_Limit = Get_Devpro_Light_Up_Limit();
        Light_Down_Limit = Get_Devpro_Light_Down_Limit();
        Beep_State = Get_Devpro_BeepState();
        Relay_State = Get_Devpro_RelayState();
        LED0_State = Get_Devpro_LED0State();
        LED1_State = Get_Devpro_LED1State();
        Door_State = Get_Devpro_DoorState();
        DHT11_Error_State = Get_Devpro_DHT11_Error_State();
        Light_Error_State = Get_Devpro_Light_Error_State();

        if(!DHT11_Error_State){  // 硬件未故障，数据才有效
            // 判断温度数据是否超出阈值
            if(!Beep_State && !Door_State){      //蜂鸣器未开启，需要判断是否超出阈值
                if(temp>Temp_Limit){
//                    my_send_temp_feedback(1);   //向TCP服务器发送反馈信息
//                    beep_open();    //开启蜂鸣器
                    beep_change(true); //开启蜂鸣器和警示灯
                    my_pwm_angle_change(true,90); // 开启应急逃生门
                }
            }else if(Beep_State && Door_State){     //蜂鸣器已经开启，需要判断数据是否回到正常阈值下
                if( temp<Temp_Limit){
//                    my_send_temp_feedback(0);   //向TCP服务器发送反馈信息
//                    beep_close();   //关闭蜂鸣器
                    beep_change(false); //关闭蜂鸣器和警示灯
                    my_pwm_angle_change(false,90); // 关闭应急逃生门
                }
            }
//             判断湿度数据是否超出阈值
            if(!Relay_State){      //继电器未开启，需要判断是否超出阈值
                if(humi>Humi_Limit){
//                    my_send_temp_feedback(1);   //向TCP服务器发送反馈信息
//                    beep_open();    //开启蜂鸣器
                      relay_change(true);   //开启继电器
                }
            }else if(Relay_State){     //继电器已经开启，需要判断数据是否回到正常阈值下
                if( humi<Humi_Limit){
//                    my_send_temp_feedback(0);   //向TCP服务器发送反馈信息
//                    beep_close();   //关闭蜂鸣器
                    relay_change(false);    //关闭继电器
                }
            }
        }

        if(!Light_Error_State){  // 硬件未故障，数据才有效
            // 判断光照强度数据是否超出阈值
            if(!LED1_State){   //应急灯未开启，需要判断是否超出阈值
                if( light<Light_Down_Limit){
//                    my_send_light_feedback(1);   //向TCP服务器发送反馈信息
//                    emergency_light_open(); //开启应急灯
                    emergency_light_change(true); //开启应急灯
                }
            }else if(LED1_State){     //应急灯已经开启，需要判断数据是否回到正常阈值下
                if( light>Light_Up_Limit){
//                    my_send_light_feedback(0);   //向TCP服务器发送反馈信息
//                    emergency_light_close(); //关闭应急灯
                    emergency_light_change(false); //关闭应急灯
                }
            }
        }

        rt_thread_delay(1000);
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
