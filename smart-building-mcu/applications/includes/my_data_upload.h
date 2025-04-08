/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_DATA_UPLOAD_H_
#define APPLICATIONS_INCLUDES_MY_DATA_UPLOAD_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <rtthread.h>
#include <rtdbg.h>

typedef struct Dev_Propeties{
    bool Beep_State;        // 警报器当前状态
    bool Relay_State;       // 继电器当前状态
    bool LED0_State;        // 警示灯当前状态
    bool LED1_State;        // 补光灯当前状态
    bool Door_State;        // 应急逃生门当前状态
    bool DHT11_Error_State; // DHT11温湿度传感器故障标识
    bool Light_Error_State; // 光敏传感器故障标识
    int temp;               // 温度
    int Temp_Limit;         // 温度阈值
    int humi;               // 湿度
    int Humi_Limit;         // 湿度阈值
    int light;              // 光照强度
    int Light_Up_Limit;     // 光强上限，超出上限关闭补光灯
    int Light_Down_Limit;   // 光强下限，超出下限开启补光灯
}Dev_properties_t;

void my_data_upload_entry(void *parameter);

int my_data_upload_init(void);

bool Get_Devpro_BeepState(void);
void Set_Devpro_BeepState(bool state);

bool Get_Devpro_RelayState(void);
void Set_Devpro_RelayState(bool state);

bool Get_Devpro_LED0State(void);
void Set_Devpro_LED0State(bool state);

bool Get_Devpro_LED1State(void);
void Set_Devpro_LED1State(bool state);

bool Get_Devpro_DoorState(void);
void Set_Devpro_DoorState(bool state);

bool Get_Devpro_DHT11_Error_State(void);
void Set_Devpro_DHT11_Error_State(bool state);

bool Get_Devpro_Light_Error_State(void);
void Set_Devpro_Light_Error_State(bool state);

int  Get_Devpro_temp(void);
void Set_Devpro_temp(int temp);

int  Get_Devpro_Temp_Limit(void);
void Set_Devpro_Temp_Limit(int Temp_Limit);

int  Get_Devpro_humi(void);
void Set_Devpro_humi(int humi);

int  Get_Devpro_Humi_Limit(void);
void Set_Devpro_Humi_Limit(int Humi_Limit);

int  Get_Devpro_light(void);
void Set_Devpro_light(int light);

int  Get_Devpro_Light_Up_Limit(void);
void Set_Devpro_Light_Up_Limit(int Light_Up_Limit);

int  Get_Devpro_Light_Down_Limit(void);
void Set_Devpro_Light_Down_Limit(int Light_Down_Limit);

#endif /* APPLICATIONS_INCLUDES_MY_DATA_UPLOAD_H_ */
