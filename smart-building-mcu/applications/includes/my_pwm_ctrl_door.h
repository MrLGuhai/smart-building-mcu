/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-12     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_PWM_CTRL_DOOR_H_
#define APPLICATIONS_INCLUDES_MY_PWM_CTRL_DOOR_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <rtthread.h>
#include <rtdbg.h>

#define PWM_DEV_NAME        "pwm4"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     1       /* PWM通道 */

void my_pwm_init(void);
void my_pwm_set_angle(uint8_t angles);
void my_pwm_angle_change(bool direction, uint8_t angles);

#endif /* APPLICATIONS_INCLUDES_MY_PWM_CTRL_DOOR_H_ */
