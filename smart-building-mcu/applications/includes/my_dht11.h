/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_DHT11_H_
#define APPLICATIONS_INCLUDES_MY_DHT11_H_

#include <rtthread.h>
#include <rtdevice.h>
#include "sensor.h"
#include "sensor_dallas_dht11.h"
#include "drv_common.h"

/* Modify this pin according to the actual wiring situation */
#define DHT11_DATA_PIN    GET_PIN(G, 9)

void change_dht11_hard_error_state(void);

void set_beep_state(uint8_t state);

void set_temp_limit(uint8_t limit);

void my_read_temp_entry(void *parameter);

int my_dht11_read_temp_sample(void);

int my_rt_hw_dht11_port(void);

 void show_Data(void);

 uint8_t my_dht11_get_temp(void);

 uint8_t my_dht11_get_humi(void);

#endif /* APPLICATIONS_INCLUDES_MY_DHT11_H_ */
