/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-10     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_AUTO_CONTROL_H_
#define APPLICATIONS_INCLUDES_MY_AUTO_CONTROL_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <rtthread.h>
#include <rtdbg.h>

typedef enum {
    ALARM_TEMPERATURE_TOO_HIGH = 1,
    ALARM_TEMPERATURE_BACK_NORMAL = 2,
    ALARM_HUMIDITY_TOO_HIGH = 3,
    ALARM_HUMIDITY_BACK_NORMAL = 4,
    ALARM_LIGHT_TOO_STRONG = 5,
    ALARM_LIGHT_TOO_WEAK = 6
} AlarmType;

void my_auto_control_entry(void *parameter);

int my_auto_control_init(void);

#endif /* APPLICATIONS_INCLUDES_MY_AUTO_CONTROL_H_ */
