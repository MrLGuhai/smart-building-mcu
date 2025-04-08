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
#define MQTT_URI                "tcp://192.168.20.226:1883"
#define MQTT_USERNAME           "admin"
#define MQTT_PASSWORD           "admin"
#define MQTT_SUBTOPIC           "device/control"
#define MQTT_PUBTOPIC           "sensor/data"

#define MQTT_SENSOR_PUBTOPIC           "sensor/data"
#define MQTT_DEVICE_SUBTOPIC           "device/control"
#define MQTT_DEVICE_PUBTOPIC           "device/control/response"
#define MQTT_THRESHOLD_SUBTOPIC           "threshold/set"
#define MQTT_THRESHOLD_PUBTOPIC           "threshold/set/response"

#define MQTT_WILLMSG            "Goodbye!"

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data);

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data);

static void mqtt_connect_callback(MQTTClient *c);

static void mqtt_online_callback(MQTTClient *c);

static void mqtt_offline_callback(MQTTClient *c);

int mqtt_start(void);

int mqtt_stop(int argc, char **argv);

//int mqtt_publish(int argc, char **argv);
int mqtt_publish(char *topic , char *msg );

static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data);

int mqtt_subscribe(int argc, char **argv);

int mqtt_unsubscribe(int argc, char **argv);


#endif /* APPLICATIONS_INCLUDES_MY_MQTT_H_ */
