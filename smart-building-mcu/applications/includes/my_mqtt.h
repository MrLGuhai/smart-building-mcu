/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-03     Mr-L       the first version
 */
#ifndef APPLICATIONS_INCLUDES_MY_MQTT_H_
#define APPLICATIONS_INCLUDES_MY_MQTT_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "paho_mqtt.h"
#include <my_data_upload.h>
#include <cJSON_util.h>

/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */
#define MQTT_URI                "tcp://192.168.170.226:1883"
#define MQTT_USERNAME           "admin"
#define MQTT_PASSWORD           "admin"
//#define MQTT_SUBTOPIC           "device/control"
#define MQTT_PUBTOPIC           "sys/info"

#define MQTT_SYNC_PUBTOPIC             "sync/request"   // 用于向服务器请求同步数据库中的阈值和外设状态
#define MQTT_SYNC_SUBTOPIC             "sync/response"  // 用于接收服务器发送的同步数据

#define MQTT_SENSOR_PUBTOPIC           "sensor/data"    // 上传采集的环境数据

#define MQTT_DEVICE_STATUS_PUBTOPIC    "device/status"  // 用于在自动控制外设时向服务器发布外设状态更新信息
#define MQTT_DEVICE_SUBTOPIC           "device/control" // 接收用户远程操控外设的命令
#define MQTT_DEVICE_PUBTOPIC           "device/control/response" // 用于响应用户的远程操控外设是否成功

#define MQTT_THRESHOLD_SUBTOPIC           "threshold/set" // 接收用户设置的环境阈值
#define MQTT_THRESHOLD_PUBTOPIC           "threshold/set/response"  // 用于响应用户的阈值设置是否成功

#define MQTT_WILLMSG            "Goodbye!"

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data);

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data);

static void mqtt_connect_callback(MQTTClient *c);

static void mqtt_online_callback(MQTTClient *c);

static void mqtt_offline_callback(MQTTClient *c);

int mqtt_start(void);

int mqtt_stop(int argc, char **argv);

int mqtt_publish(char *topic , char *msg );

int mqtt_subscribe(char *topic , subscribe_cb callback);

static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data);


int mqtt_unsubscribe(int argc, char **argv);

rt_err_t Publish_Sync_Request(void);

rt_err_t Publish_Environment_Data(const Environment_t *envi_data);

rt_err_t Publish_Device_Status(const Devices_t *dev_data);

void mqtt_sub_init(void);

#endif /* APPLICATIONS_INCLUDES_MY_MQTT_H_ */
