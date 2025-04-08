#include <my_hwtimer.h>

int my_hwtimer_max_time=5000, my_hwtimer_min_time=125, my_hwtimer_temp_time;  //以ms为单位，用于记录最大超时时间和最小超时时间，以及临时变量

rt_err_t ret = RT_EOK;
rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */
rt_hwtimer_mode_t mode;         /* 定时器模式 */

/* 定时器超时回调函数 */
rt_err_t my_timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_pin_write(LED0_PIN, 1-rt_pin_read(LED0_PIN));//翻转LED0
    rt_pin_write(BEEP_PIN, 1-rt_pin_read(BEEP_PIN));//翻转BEEP
    return 0;
}

int my_hwtimer_openTimer(void)
{
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, my_timeout_cb);

    /* 设置模式为周期性定时器 */
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    /* 设置定时器超时值为0.1s并启动定时器 */
    timeout_s.sec = 0;      /* 秒 */
    timeout_s.usec = 100*1000;     /* 微秒 */

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }
//    rt_kprintf("Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);
    return ret;
}

int my_hwtimer_closeTimer(void)
{
    /* 确保定时器存在 */
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    ret = rt_device_close(hw_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("close %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    return ret;
}

int my_hwtimer_addTime(int ms)
{
    /* 确保定时器存在 */
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    rt_kprintf("old s=%d old us=%d\n",timeout_s.sec,timeout_s.usec);

    my_hwtimer_temp_time=timeout_s.sec*1000+timeout_s.usec/1000;     //将当前定时器的时间转换为ms

    if(my_hwtimer_temp_time+ms>=my_hwtimer_max_time){   //相减后超出阈值上限
        timeout_s.sec=(int)my_hwtimer_max_time/1000;
        timeout_s.usec=(int)my_hwtimer_max_time%1000*1000;
    }else{
        my_hwtimer_temp_time+=ms;
        timeout_s.sec=(int)my_hwtimer_temp_time/1000;
        timeout_s.usec=(int)my_hwtimer_temp_time%1000*1000;
    }

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    rt_kprintf("AddTime: snew s=%d new us=%d\n",timeout_s.sec,timeout_s.usec);

    return ret;
}

int my_hwtimer_subTime(int ms)
{
    /* 确保定时器存在 */
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    rt_kprintf("old s=%d old us=%d\n",timeout_s.sec,timeout_s.usec);

    my_hwtimer_temp_time=timeout_s.sec*1000+timeout_s.usec/1000;     //将当前定时器的时间转换为ms

    if(my_hwtimer_temp_time-ms<=my_hwtimer_min_time){   //相减后超出阈值下限
        timeout_s.sec=(int)my_hwtimer_min_time/1000;
        timeout_s.usec=(int)my_hwtimer_min_time%1000*1000;
    }else{
        my_hwtimer_temp_time-=ms;
        timeout_s.sec=(int)my_hwtimer_temp_time/1000;
        timeout_s.usec=(int)my_hwtimer_temp_time%1000*1000;
    }

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    rt_kprintf("SubTime: new s=%d new us=%d\n",timeout_s.sec,timeout_s.usec);

    return ret;
}

 /* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(my_hwtimer_openTimer, my hwtimer openTimer);

