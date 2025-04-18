/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_MULTITHREAD_H_
#define APPLICATIONS_INCLUDES_MY_MULTITHREAD_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdbool.h>

#define LED0_PIN    GET_PIN(F, 9)
#define LED1_PIN    GET_PIN(F, 10)
#define KEY0_PIN    GET_PIN(E, 4)
#define KEY1_PIN    GET_PIN(E, 3)
#define BEEP_PIN    GET_PIN(F, 8)
//#define PWM4_PIN    GET_PIN(D, 12)
#define Relay_PIN    GET_PIN(D, 4)

//static uint8_t KEY0_State=0;      //记录KEY0按键的状态

//void beep_open(void);
//void beep_close(void);
void beep_change(bool state);
void relay_change(bool state);
void emergency_light_open(void);
void emergency_light_close(void);
void emergency_light_change(bool state);

/* 自己定义KEY0的回调函数，相当于中断服务函数 */
void IQR_HANDALE_KEY0(void *args);;

/* 自己定义KEY1的回调函数，相当于中断服务函数 */
void IQR_HANDALE_KEY1(void *args);

void my_GPIO_Init(void);

#endif /* APPLICATIONS_INCLUDES_MY_MULTITHREAD_H_ */
