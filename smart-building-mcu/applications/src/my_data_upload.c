/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-06     Mr-L       the first version
 */

#include <my_Data_upload.h>
#include <my_adc.h>
#include <my_dht11.h>
#include <my_mqtt.h>

//Dev_properties_t my_dev_properties = {false,false,false,false,false,false,false,0,50,0,60,0,1000,500};
Devices_t my_devices = {false,false,false,false,false,true,true};
Environment_t my_environment = {0,0,0};
Thresholds_t my_thresholds = {50,60,2000,1250};

void my_data_upload_entry(void *parameter)
{
    rt_err_t result = RT_EOK;

    while (1)
    {
//        // 调用API获取最新的传感器数据
//        my_dev_properties.temp = my_dht11_get_temp();
//        my_dev_properties.humi = my_dht11_get_humi();
//        my_dev_properties.light = my_adc_get_light();
//        // 根据数据确定外设的状态

        // 将传感器数据上传OneNET
//        result = onenet_mqtt_upload_devprops(&my_dev_properties);

        // 发布环境数据
        result = Publish_Environment_Data(&my_environment);


        if(result < 0){
            LOG_E("device properties datas upload failed (%d)!", result);
        }
        rt_thread_delay(2000);
    }
}

int my_data_upload_init(void)
{
    rt_thread_t data_upload_thread;

    data_upload_thread = rt_thread_create("my_data_upload",
                                     my_data_upload_entry,
                                     RT_NULL,
                                     2048,
                                     RT_THREAD_PRIORITY_MAX / 2,
                                     20);
    if (data_upload_thread != RT_NULL)
    {
        rt_thread_startup(data_upload_thread);
        rt_kprintf("data upload thread created success\n");
    }else{
        rt_kprintf("data upload thread created fail\n");
    }

    return RT_EOK;
}
//INIT_APP_EXPORT(my_data_upload_init);

Devices_t * Get_Device_Properties(void)
{
    return &my_devices;
}

Environment_t * Get_Environment_Properties(void)
{
    return &my_environment;
}

Thresholds_t * Get_Thresholds_Properties(void)
{
    return &my_thresholds;
}


bool Get_Devpro_BeepState(void)
{
    return my_devices.Beep_State;
}
void Set_Devpro_BeepState(bool state)
{
    my_devices.Beep_State = state;
}

bool Get_Devpro_RelayState(void)
{
    return my_devices.Relay_State;
}
void Set_Devpro_RelayState(bool state)
{
    my_devices.Relay_State = state;
}

bool Get_Devpro_LED0State(void)
{
    return my_devices.LED0_State;
}
void Set_Devpro_LED0State(bool state)
{
    my_devices.LED0_State = state;
}

bool Get_Devpro_LED1State(void)
{
    return my_devices.LED1_State;
}
void Set_Devpro_LED1State(bool state)
{
    my_devices.LED1_State = state;
}

bool Get_Devpro_DoorState(void)
{
    return my_devices.Door_State;
}
void Set_Devpro_DoorState(bool state)
{
    my_devices.Door_State = state;
}

bool Get_Devpro_DHT11_Sensor_State(void)
{
    return my_devices.DHT11_Sensor_State;
}
void Set_Devpro_DHT11_Sensor_State(bool state)
{
    my_devices.DHT11_Sensor_State = state;
}

bool Get_Devpro_Light_Sensor_State(void)
{
    return my_devices.Light_Sensor_State;
}
void Set_Devpro_Light_Sensor_State(bool state)
{
    my_devices.Light_Sensor_State = state;
}

int  Get_Devpro_temp(void)
{
    return my_environment.temp;
}
void Set_Devpro_temp(int temp)
{
    my_environment.temp = temp;
}

int  Get_Devpro_Temp_Limit(void)
{
    return my_thresholds.Temp_Limit;
}
void Set_Devpro_Temp_Limit(int Temp_Limit)
{
    my_thresholds.Temp_Limit = Temp_Limit;
}

int  Get_Devpro_humi(void)
{
    return my_environment.humi;
}
void Set_Devpro_humi(int humi)
{
    my_environment.humi = humi;
}

int  Get_Devpro_Humi_Limit(void)
{
    return my_thresholds.Humi_Limit;
}
void Set_Devpro_Humi_Limit(int Humi_Limit)
{
    my_thresholds.Humi_Limit = Humi_Limit;
}

int  Get_Devpro_light(void)
{
    return my_environment.light;
}
void Set_Devpro_light(int light)
{
    my_environment.light = light;
}

int  Get_Devpro_Light_Up_Limit(void)
{
    return my_thresholds.Light_Up_Limit;
}
void Set_Devpro_Light_Up_Limit(int Light_Up_Limit)
{
    my_thresholds.Light_Up_Limit = Light_Up_Limit;
}

int  Get_Devpro_Light_Down_Limit(void)
{
    return my_thresholds.Light_Down_Limit;
}
void Set_Devpro_Light_Down_Limit(int Light_Down_Limit)
{
    my_thresholds.Light_Down_Limit = Light_Down_Limit;
}

void show_data_upload_dev_data(void)
{
    rt_kprintf("data upload dht11 sensor state is %d\n",my_devices.DHT11_Sensor_State);
    rt_kprintf("data upload light sensor state is %d\n",my_devices.Light_Sensor_State);
    rt_kprintf("data upload temp limit is %d    humi limit is %d\n",my_thresholds.Temp_Limit,my_thresholds.Humi_Limit);
    rt_kprintf("data upload light up limit is %d   down limit is %d\n",my_thresholds.Light_Up_Limit,my_thresholds.Light_Down_Limit);

}
MSH_CMD_EXPORT(show_data_upload_dev_data,show dev data);
