/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_HWTIMER_H_
#define APPLICATIONS_INCLUDES_MY_HWTIMER_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <hwtimer.h>
#include <board.h>

#define HWTIMER_DEV_NAME   "timer3"     /* 定时器名称 */

#define LED0_PIN    GET_PIN(F, 9)
#define BEEP_PIN    GET_PIN(F, 8)

/* 定时器超时回调函数 */
rt_err_t my_timeout_cb(rt_device_t dev, rt_size_t size);

int my_hwtimer_openTimer(void);

int my_hwtimer_closeTimer(void);

int my_hwtimer_addTime(int ms);

int my_hwtimer_subTime(int ms);

#endif /* APPLICATIONS_INCLUDES_MY_HWTIMER_H_ */
