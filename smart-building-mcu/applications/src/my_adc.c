#include <my_adc.h>
#include <my_multiThread.h>
#include <my_TCP.h>
#include <my_file.h>
#include <my_oled.h>
#include <my_data_upload.h>

static rt_uint8_t thread1_stack[2048];             //线程栈1
static struct rt_thread static_thread1;           //线程控制块1

rt_adc_device_t adc_light_dev;    //光敏的ADC设备
rt_adc_device_t adc_mq2_dev;    //MQ2的ADC设备
rt_uint32_t light_value, light_vol;     //光敏的数据值、电压值
rt_uint32_t mq2_value;     //MQ2的数据值
rt_uint32_t light_limit=2500;   //光敏阈值(越暗值越大)
bool mq2_state=false;   //MQ2的D0引脚(0表示未超出阈值，1表示超出阈值)
uint8_t emergency_light_state=0;  //记录应急灯的状态，初始关闭则为0
uint8_t light_hard_error_state=0;   //光敏传感器硬件故障标识

void change_light_hard_error_state(void)
{
    if(light_hard_error_state>=0){
        if(light_hard_error_state==0){  //故障标识原先为0，外部中断介入，说明发生故障，应当关闭应急灯
            emergency_light_close();
        }
        light_hard_error_state=1-light_hard_error_state;
    }
}

void set_emergency_light_state(uint8_t state)
{
    emergency_light_state=state;
}

void set_light_limit(rt_uint32_t limit)
{
    light_limit=limit;
    rt_kprintf("the new light_limit is %d\n",light_limit);
}

int my_light_ADC_Init(void)
{
    adc_light_dev = (rt_adc_device_t)rt_device_find(ADC_LIGHT_DEV_NAME);  /* 查找设备 */
    if (adc_light_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_LIGHT_DEV_NAME);
        return RT_ERROR;
    }
    else {
        rt_kprintf("find %s success!\n",ADC_LIGHT_DEV_NAME);
    }

    rt_adc_enable(adc_light_dev, ADC_LIGHT_DEV_CHANNEL);/* 使能设备 */
    return RT_EOK;
}

//光敏电阻采集数据线程
static void my_light_entry(void *param)
{
    while (1)
    {
        /* 读取采样值 */
        light_value = rt_adc_read(adc_light_dev, ADC_LIGHT_DEV_CHANNEL);
        //rt_kprintf("light_entry: the value is :%d \n", value);

        /* 转换为对应电压值 */
//        vol = value * REFER_VOLTAGE / CONVERT_BITS;
        //rt_kprintf("light_entry: the voltage is :%d.%02d \n", vol / 100, vol % 100);

        light_value = 4000 - light_value;  // 将采集到的光敏值进行转换，与明暗度一致，越亮光线值越高，越暗光线值越暗
        //my_send_Lightdata(value, 0);    //通过TCP发送到服务器
//        write_light_data(value);        //写入Flash中
        my_oled_print_light(light_value);     //显示到OLED上

        // 将光照强度数据写入到设备属性结构体中
        Set_Devpro_light(light_value);

//        if(light_hard_error_state==0){  //硬件没故障，数据才有效
//            /* 判断数据是否超出阈值 */
//            if(emergency_light_state==0){   //应急灯未开启，需要判断是否超出阈值
//                if( value>light_limit){
////                    my_send_light_feedback(1);   //向TCP服务器发送反馈信息
//                    emergency_light_open(); //开启应急灯
//                }
//            }else if(emergency_light_state==1){     //应急灯已经开启，需要判断数据是否回到正常阈值下
//                if( value<light_limit){
////                    my_send_light_feedback(0);   //向TCP服务器发送反馈信息
//                    emergency_light_close(); //关闭应急灯
//                }
//            }
//        }
        rt_thread_mdelay(1000);
    }
}

int my_light_adc(void)
{
    if(RT_ERROR==my_light_ADC_Init()){   //初始化ADC
        rt_kprintf("Light ADC INIT Failed!\n");
        return RT_ERROR;
    }
    static rt_thread_t my_light_thread = RT_NULL;
    my_light_thread = rt_thread_create("my_light_th",    //名称
                                 my_light_entry,   //线程代码
                                 RT_NULL,         //参数
                                 THREAD_STACK_SIZE,            //栈大小
                                 THREAD_PRIORITY,              //优先级
                                 THREAD_TIMESLICE);             //时间片
    if (my_light_thread != RT_NULL)
        rt_thread_startup(my_light_thread);             //线程进入就绪态
    else
    {
        rt_kprintf("my_light_thread create failure\n");
        rt_kprintf("create thread by static\n");
        rt_thread_init(&static_thread1,     //线程handle
                "my_light_th",           //线程名称
                my_light_entry,              //线程入口函数
                RT_NULL,                   //线程入口参数
                &thread1_stack[0],         //线程栈地址
                sizeof(thread1_stack),     //线程栈大小
                THREAD_PRIORITY,                        //线程优先级
                THREAD_TIMESLICE);                        //线程时间片
        rt_thread_startup(&static_thread1);                      //线程进入就绪态
    }
    return RT_EOK;
}

rt_uint32_t my_adc_get_light(void)
{
    return light_value;
}


int my_mq2_ADC_Init(void)
{
    adc_mq2_dev = (rt_adc_device_t)rt_device_find(ADC_MQ2_DEV_NAME);  /* 查找设备 */
    if (adc_mq2_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_MQ2_DEV_NAME);
        return RT_ERROR;
    }
    else {
        rt_kprintf("find %s success!\n",ADC_MQ2_DEV_NAME);
    }

    rt_adc_enable(adc_mq2_dev, ADC_MQ2_DEV_CHANNEL);/* 使能设备 */
    return RT_EOK;
}

//光敏电阻采集数据线程
static void my_mq2_entry(void *param)
{
    while (1)
    {
        /* 读取采样值 */
        mq2_value = rt_adc_read(adc_mq2_dev, ADC_MQ2_DEV_CHANNEL);
        //rt_kprintf("mq2_entry: the mq2_value is :%d \n", mq2_value);

        my_oled_print_mq2(mq2_value);     //显示到OLED上

        // 将光照强度数据写入到设备属性结构体中
        Set_Devpro_smoke(mq2_value);

        rt_thread_mdelay(1000);
    }
}

int my_mq2_adc(void)
{
    if(RT_ERROR==my_mq2_ADC_Init()){   //初始化ADC
        rt_kprintf("MQ2 ADC INIT Failed!\n");
        return RT_ERROR;
    }
    static rt_thread_t my_mq2_thread = RT_NULL;
    my_mq2_thread = rt_thread_create("my_mq2_th",    //名称
                                 my_mq2_entry,   //线程代码
                                 RT_NULL,         //参数
                                 THREAD_STACK_SIZE,            //栈大小
                                 THREAD_PRIORITY,              //优先级
                                 THREAD_TIMESLICE);             //时间片
    if (my_mq2_thread != RT_NULL)
        rt_thread_startup(my_mq2_thread);             //线程进入就绪态
    else
    {
        rt_kprintf("MQ2 Thread create failed!\n");
    }
    return RT_EOK;
}

rt_uint32_t my_adc_get_mq2(void)
{
    return mq2_value;
}
