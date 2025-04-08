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

void my_auto_control_entry(void *parameter);

int my_auto_control_init(void);

#endif /* APPLICATIONS_INCLUDES_MY_AUTO_CONTROL_H_ */
