/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_FILE_H_
#define APPLICATIONS_INCLUDES_MY_FILE_H_

#include "drv_spi.h"
#include "spi_flash_sfud.h"
#include "dfs_fs.h"

#define DHT11_ARRAY_SIZE 25  // 每次读取25组温湿度数据
#define LIGHT_ARRAY_SIZE 25  // 每次读取25个光敏数据



static int rt_hw_spi_flash_init(void);

int dfs_mount_init(void);

// 写入温湿度数据
void write_dht11_data(rt_uint8_t temp, rt_uint8_t humi);
// 读取温湿度数据并通过TCP发送
void read_and_send_dht11_data(void);

// 重置读取位置（如果需要从头开始读）
void reset_dht11_read_position(void);

void set_dht11_read_position_end(void);

// 写入光敏数据
void write_light_data(rt_uint32_t light_value);

// 读取光敏数据并通过TCP发送
void read_and_send_light_data(void);

// 重置读取位置（如果需要从头开始读）
void reset_light_read_position(void);

//设置追加位置为文件末尾，用于每次运行程序时从文件尾进行读写
void set_light_read_position_end(void);

#endif /* APPLICATIONS_INCLUDES_MY_FILE_H_ */
