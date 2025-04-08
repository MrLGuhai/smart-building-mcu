/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_OLED_H_
#define APPLICATIONS_INCLUDES_MY_OLED_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"

void my_oled_test(void);

void my_oled_init(void);

void my_clean_line(uint8_t y);

void my_oled_print_led(const char *message);

void my_oled_print_dht(uint8_t temp,uint8_t humi);

void my_oled_print_light(rt_uint32_t value);

#endif /* APPLICATIONS_INCLUDES_MY_OLED_H_ */
