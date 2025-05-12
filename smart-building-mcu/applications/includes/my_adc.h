/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_ADC_H_
#define APPLICATIONS_INCLUDES_MY_ADC_H_

#include <rtthread.h>
#include <rtdbg.h>
#include <rtdevice.h>
#include <board.h>

#define ADC_LIGHT_DEV_NAME        "adc3"      /* ADC 设备名称 */
#define ADC_LIGHT_DEV_CHANNEL     5           /* ADC 通道 */
#define ADC_MQ2_DEV_NAME        "adc2"      /* ADC 设备名称 */
#define ADC_MQ2_DEV_CHANNEL     4           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

#define THREAD_STACK_SIZE       2048
#define THREAD_PRIORITY         15
#define THREAD_TIMESLICE        5

void change_light_hard_error_state(void);

void set_emergency_light_state(uint8_t state);

void set_light_limit(rt_uint32_t limit);

int my_light_ADC_Init(void);

//光敏电阻采集数据线程
static void my_light_entry(void *param);

int my_light_adc(void);

rt_uint32_t my_adc_get_light(void);


int my_mq2_ADC_Init(void);

//MQ2采集数据线程
static void my_mq2_entry(void *param);

int my_mq2_adc(void);

rt_uint32_t my_adc_get_mq2(void);

#endif /* APPLICATIONS_INCLUDES_MY_ADC_H_ */
