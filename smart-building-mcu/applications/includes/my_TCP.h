/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_TCP_H_
#define APPLICATIONS_INCLUDES_MY_TCP_H_

#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>
#include <string.h>
#include <finsh.h>

#include <rtdevice.h>
#include <board.h>

#define BUFSZ   2048             //接收缓冲区大小

#define LED0_PIN    GET_PIN(F, 9)
#define LED1_PIN    GET_PIN(F, 10)

int my_send_temp_feedback(uint8_t flag);

int my_send_light_feedback(uint8_t flag);

int my_send_fp_reset_feedback(uint8_t flag);

int my_send_msg(const char *send_data,int size,int flag);

int my_send_DHT11data(uint8_t data1,uint8_t data2,int flag);

int my_send_Lightdata(rt_uint32_t value,int flag);

void my_control_led(rt_base_t led_pin,int led_state);

void my_tcp_control(int argc, char **argv);


#endif /* APPLICATIONS_INCLUDES_MY_TCP_H_ */
