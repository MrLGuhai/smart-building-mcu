#include <my_multiThread.h>
#include <my_data_upload.h>
#include <my_auto_control.h>
#include <stdbool.h>
#include <my_adc.h>
#include <my_hwtimer.h>


//void beep_open(void)
//{
//    my_hwtimer_openTimer();             //红灯闪烁、蜂鸣器响
//    Set_Devpro_BeepState(true);
////    set_beep_state(1);
//}
//
//void beep_close(void)
//{
//    my_hwtimer_closeTimer();
//    rt_pin_write(LED0_PIN, PIN_HIGH);   //红灯灭，因为定时器关闭时红灯可能是亮着的，所以再手动关闭一次
//    rt_pin_write(BEEP_PIN, PIN_LOW);    //关闭蜂鸣器，因为定时器关闭时蜂鸣器可能是开启的，所以再手动关闭一次
//    Set_Devpro_BeepState(false);
////    set_beep_state(0);
//}

void beep_change(bool state)
{
    if(!state){      // 关闭蜂鸣器
//        rt_kprintf("Ready to CLOSE Timer\n");
        my_hwtimer_closeTimer();
        rt_pin_write(LED0_PIN, PIN_HIGH);   //红灯灭，因为定时器关闭时红灯可能是亮着的，所以再手动关闭一次
        rt_pin_write(BEEP_PIN, PIN_LOW);    //关闭蜂鸣器，因为定时器关闭时蜂鸣器可能是开启的，所以再手动关闭一次
    }else if(state){ // 开启蜂鸣器
//        rt_kprintf("Ready to OPEN Timer\n");
        my_hwtimer_openTimer();             //红灯闪烁、蜂鸣器响
    }
    Set_Devpro_LED0State(state);
    Set_Devpro_BeepState(state);
}

void relay_change(bool state)
{
    if(!state){      // 关闭继电器
        rt_pin_write(Relay_PIN, PIN_LOW);    //关闭继电器
    }else if(state){ // 开启继电器
        rt_pin_write(Relay_PIN, PIN_HIGH);    //开启继电器
    }
    Set_Devpro_RelayState(state);
}

void emergency_light_open(void)
{
    rt_pin_write(LED1_PIN, PIN_LOW);   //开启应急灯
    set_emergency_light_state(1); //更新应急灯状态
}

void emergency_light_close(void)
{
    rt_pin_write(LED1_PIN, PIN_HIGH);    //关闭应急灯
    set_emergency_light_state(0); //更新应急灯状态
}

void emergency_light_change(bool state)
{
    if(!state){      // 关闭应急灯
        rt_pin_write(LED1_PIN, PIN_HIGH);    //关闭应急灯
    }else if(state){ // 开启应急灯
        rt_pin_write(LED1_PIN, PIN_LOW);   //开启应急灯
    }
    Set_Devpro_LED1State(state);    //更新应急灯状态
}

/* 自己定义KEY0的回调函数，相当于中断服务函数 */
void IQR_HANDALE_KEY0(void *args)   //关闭蜂鸣器
{
    //rt_kprintf("KEY0 Down! Change BEEP!\n");
//    change_dht11_hard_error_state();    //KEY0按下，翻转DHT11故障标识
    Set_Devpro_DHT11_Sensor_State(!Get_Devpro_DHT11_Sensor_State());    //KEY0按下，翻转DHT11故障标识
}
/* 自己定义KEY1的回调函数，相当于中断服务函数 */
void IQR_HANDALE_KEY1(void *args)
{
    //rt_kprintf("KEY1 Down! Change Emergency light!\n");
//    change_light_hard_error_state();    //KEY1按下，翻转应急灯故障标识
    Set_Devpro_Light_Sensor_State(!Get_Devpro_Light_Sensor_State());    //KEY1按下，翻转应急灯故障标识
}

uint8_t Get_MQ2_D0(void)
{
    return rt_pin_read(MQ2_D0_PIN);
}

void my_GPIO_Init(void)
{
    /* 把LED引脚设置为输出 */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

    /* 先把两个灯关掉 */
    rt_pin_write(LED0_PIN, PIN_HIGH);
    rt_pin_write(LED1_PIN, PIN_HIGH);

    /* 把KEY引脚设置为输入 */
    rt_pin_mode(KEY0_PIN, PIN_MODE_INPUT);
    rt_pin_mode(KEY1_PIN, PIN_MODE_INPUT);

    /* 绑定中断，下降沿模式，回调函数名为IQR_HANDALE_KEY0 */
    rt_pin_attach_irq(KEY0_PIN, PIN_IRQ_MODE_FALLING, IQR_HANDALE_KEY0, RT_NULL);
    /* 绑定中断，下降沿模式，回调函数名为IQR_HANDALE_KEY1 */
    rt_pin_attach_irq(KEY1_PIN, PIN_IRQ_MODE_FALLING, IQR_HANDALE_KEY1, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY0_PIN, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(KEY1_PIN, PIN_IRQ_ENABLE);

    /* 把BEEP引脚设置为输出 */
    rt_pin_mode(BEEP_PIN, PIN_MODE_OUTPUT);
    /* 先关闭蜂鸣器 */
    rt_pin_write(BEEP_PIN, PIN_LOW);

    /* 把Relay引脚设置为输出 */
    rt_pin_mode(Relay_PIN, PIN_MODE_OUTPUT);
    /* 先关闭继电器 */
    rt_pin_write(Relay_PIN, PIN_LOW);

    /* 把MQ2的D0引脚设置为输入 */
    rt_pin_mode(MQ2_D0_PIN, PIN_MODE_INPUT);

}


