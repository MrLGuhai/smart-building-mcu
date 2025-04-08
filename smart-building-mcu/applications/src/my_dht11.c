#include <my_dht11.h>
#include <my_multiThread.h>
#include <my_TCP.h>
#include <my_file.h>
#include <my_oled.h>
#include <my_data_upload.h>
#include <my_mqtt.h>
#include <string.h>

uint8_t temp,humi;
uint8_t temp_limit=50;
uint8_t beep_state=0;
uint8_t dht11_hard_error_state=0;   //DHT11硬件故障标识

void change_dht11_hard_error_state(void)
{
    if(dht11_hard_error_state>=0){
        if(dht11_hard_error_state==0){  //故障标识原先为0，外部中断介入，说明发生故障，应当关闭蜂鸣器
            beep_close();
        }
        dht11_hard_error_state=1-dht11_hard_error_state;
    }
}

void set_beep_state(uint8_t state)
{
    beep_state=state;
}

void set_temp_limit(uint8_t limit)
{
    temp_limit=limit;
    rt_kprintf("the new temp_limit is %d\n",temp_limit);
}


void my_read_temp_entry(void *parameter)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
    rt_size_t res;
    rt_uint8_t get_data_freq = 1; /* 1Hz */

    dev = rt_device_find("temp_dht11");     // 在设备列表中找指定名称的设备，设备名称在rt_hw_sensor_register函数中进行了补充，为传感器类型+设备名称
    if (dev == RT_NULL)
    {
        return;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)     // 以读写的方式打开I/0设备
    {
        rt_kprintf("open device failed!\n");
        return;
    }

    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));   // 设置数据的输出速率为1HZ

    while (1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);      //读取温湿度传感器数据

        if (res != 1)
        {
            rt_kprintf("read data failed! result is %d\n", res);
            rt_device_close(dev);
            return;
        }
        else
        {
            if (sensor_data.data.temp >= 0)     //判断数据是否合法
            {
                temp = (sensor_data.data.temp & 0xffff) >> 0;      // get temp
                humi = (sensor_data.data.temp & 0xffff0000) >> 16; // get humi
                //将温湿度数据写入Flash中并显示到OLED上
                show_Data();

                const char str_data[25];
                sprintf(str_data,"temp:%dC humi:%d",temp,humi);
                sprintf(str_data,"%s%s",str_data,"%\n");
                mqtt_publish(MQTT_PUBTOPIC, str_data);

                // 将温湿度数据写入设备属性结构体中
                Set_Devpro_temp(temp);
                Set_Devpro_humi(humi);

//                if(dht11_hard_error_state==0){  //硬件没故障，数据才有效
//                    //判断数据是否超出阈值
//                    if(beep_state==0){      //蜂鸣器未开启，需要判断是否超出阈值
//                        if(temp>temp_limit){
////                            my_send_temp_feedback(1);   //向TCP服务器发送反馈信息
//                            beep_open();    //开启蜂鸣器
//                        }
//                    }else if(beep_state==1){     //蜂鸣器已经开启，需要判断数据是否回到正常阈值下
//                        if( temp<temp_limit){
////                            my_send_temp_feedback(0);   //向TCP服务器发送反馈信息
//                            beep_close();   //关闭蜂鸣器
//                        }
//                    }
//                }
            }
        }

        rt_thread_delay(1000);
    }
}

int my_dht11_read_temp_sample(void)
{
    rt_thread_t dht11_thread;

    dht11_thread = rt_thread_create("my_dht_tem",
                                     my_read_temp_entry,
                                     RT_NULL,
                                     2048,
                                     RT_THREAD_PRIORITY_MAX / 2,
                                     20);
    if (dht11_thread != RT_NULL)
    {
        rt_thread_startup(dht11_thread);
        rt_kprintf("dht11 thread created success\n");
    }else{
        rt_kprintf("dht11 thread created fail\n");
    }

    return RT_EOK;
}
//INIT_APP_EXPORT(my_dht11_read_temp_sample);

int my_rt_hw_dht11_port(void)
{
    struct rt_sensor_config cfg;

    cfg.intf.user_data = (void *)DHT11_DATA_PIN;    // 将DHT11的引脚进行换算后得出其内存地址，将内存地址作为I2C的从机地址
    // 对sensor设备模型进行初始化，底层调用rt_hw_sensor_register函数按照DHT11传感器的相关信息进行注册，
    // 然后rt_hw_sensor_register才调用rt_device_register函数创建RT-Thread中的I/O设备模型，成功后就可以调用RT-Thread中I/0设备相关的接口函数直接访问传感器
    rt_hw_dht11_init("dht11", &cfg);

    return RT_EOK;
}
//INIT_COMPONENT_EXPORT(my_rt_hw_dht11_port);

 void show_Data(void)
 {
     //注意,如果要使用采集数据并发送至TCP服务器，必须在上电运行时加一个延时等待TCP连接成功
     //my_send_DHT11data(temp, humi, 0);   //通过TCP发送到服务器
//     write_dht11_data(temp, humi);          //将数据写入Flash
     my_oled_print_dht(temp, humi);      //显示到OLED上
 }

 uint8_t my_dht11_get_temp(void)
 {
     return temp;
 }

 uint8_t my_dht11_get_humi(void)
 {
     return humi;
 }
