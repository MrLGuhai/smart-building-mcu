#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include <my_mqtt.h>
#include <time.h>
#include <my_multiThread.h>

/* define MQTT client context */
static MQTTClient client;
static int is_started = 0;
static char *UUID = NULL; // 用于保存服务器下发命令时携带的UUID，并在响应函数中将其回传

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
    // 订阅相关主题并绑定回调函数
    mqtt_sub_init();
    // 发起同步请求，与数据库中的阈值、外设状态进行数据同步
    Publish_Sync_Request();
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

int mqtt_start(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

    if (is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* generate the random client ID */
        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

//        /* set subscribe table and event callback */
//        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
//        client.messageHandlers[0].callback = mqtt_sub_callback;
//        client.messageHandlers[0].qos = QOS1;
//
//        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;


    }

    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;

    return 0;
}

int mqtt_stop(int argc, char **argv)
{
    if (argc != 1)
    {
        rt_kprintf("mqtt_stop    --stop mqtt worker thread and free mqtt client object.\n");
    }

    is_started = 0;

    return paho_mqtt_stop(&client);
}

int mqtt_publish(char *topic , char *msg )
{
    return paho_mqtt_publish(&client, QOS1, topic, msg);
}

int mqtt_subscribe(char *topic , subscribe_cb callback)
{
    if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }
    return paho_mqtt_subscribe(&client, QOS1, topic, callback);
}

static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt new subscribe callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

int mqtt_unsubscribe(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("mqtt_unsubscribe [topic]  --send an mqtt unsubscribe packet and wait for suback before returning.\n");
        return -1;
    }
	
	if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_unsubscribe(&client, argv[1]);
}

// 设备上报属性JSON格式
//{
//    "type": "all",          // 请求类型: all-全部数据, threshold-仅阈值, device-仅设备状态
//    "timestamp": "2024-03-20T10:30:00"  // 请求时间
//}
static rt_err_t mqtt_get_sync_datas(char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish environment datas failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddStringToObject(root, "type", "all");
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish environment datas failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload environment datas failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}


rt_err_t Publish_Sync_Request(void)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_sync_datas(&send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_SYNC_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("sync request publish failed (%d)!", result);
        goto __exit;
    }
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }

    return result;
}

// 后端服务器下发的命令格式
//{
//    "threshold": {          // 阈值数据(当type为all或threshold时包含)
//        "temperature": 26,    // 温度阈值
//        "humidity": 65,       // 湿度阈值
//        "lightUpper": 1000,     // 光照上限
//        "lightLower": 500       // 光照下限
//    },
//    "device": {             // 设备状态数据(当type为all或device时包含)
//        "warningLight": true,   // 警示灯状态
//        "fillLight": true,      // 补光灯状态
//        "exhaustFan": true,     // 排气扇状态
//        "alarm": false,         // 警报器状态
//        "emergencyDoor": false  // 应急逃生门状态
//        "dht11Status": false,       // DHT11传感器状态
//        "lightSensorStatus": false  // 光照传感器状态
//    },
//    "timestamp": "2024-03-20T10:30:00"  // 响应时间
//}
static void mqtt_sync_callback(MQTTClient *c, MessageData *msg_data)
{

//    LOG_D("Enter mqtt_usr_callback!");
    // 为字符串的结尾添加结束标志
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
//    rt_kprintf("Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    // %.*s 用于以指定的长度输出字符串，两个参数分别为：要输出的字符个数、要输出的字符串的起始位置
//    rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);

    cJSON *root = NULL;
    cJSON *threshold = NULL;
    cJSON *device = NULL;
    cJSON *node = NULL;
    root = cJSON_Parse((char *)msg_data->message->payload);
    threshold = cJSON_GetObjectItem(root,"threshold");
    device = cJSON_GetObjectItem(root,"device");
    // 解析阈值
    if(threshold){
        node = cJSON_GetObjectItem(threshold,"temperature");
        if(node){
            Set_Devpro_Temp_Limit(node->valueint);
            LOG_D(" Sync: Temp Limit is %d",node->valueint);
        }
        node = cJSON_GetObjectItem(threshold,"humidity");
        if(node){
            Set_Devpro_Humi_Limit(node->valueint);
            LOG_D(" Sync: Humi Limit is %d",node->valueint);
        }
        node = cJSON_GetObjectItem(threshold,"lightUpper");
        if(node){
            Set_Devpro_Light_Up_Limit(node->valueint);
            LOG_D(" Sync: Light Up Limit is %d",node->valueint);
        }
        node = cJSON_GetObjectItem(threshold,"lightLower");
        if(node){
            Set_Devpro_Light_Down_Limit(node->valueint);
            LOG_D(" Sync: Light Down Limit is %d",node->valueint);
        }
    }
    // 解析外设状态
    if(device){
        node = cJSON_GetObjectItem(device,"warningLight");
        if(node){
            LOG_D("change warningLight to %d",node->valueint);
            Set_Devpro_LED0State(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(device,"fillLight");
        if(node){
            LOG_D("change fillLight to %d",node->valueint);
            emergency_light_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(device,"exhaustFan");
        if(node){
            LOG_D("change exhaustFan to %d",node->valueint);
            relay_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(device,"alarm");
        if(node){
            LOG_D("change alarm to %d",node->valueint);
            beep_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(device,"emergencyDoor");
        if(node){
            LOG_D("change emergencyDoor to %d",node->valueint);
            my_pwm_angle_change(node->type == cJSON_True,90);
        }
        node = cJSON_GetObjectItem(device,"dht11Status");
        if(node){
            Set_Devpro_DHT11_Sensor_State(node->type == cJSON_True);
            LOG_D(" Sync: DHT11 Sensor State is %d",node->valueint);
        }
        node = cJSON_GetObjectItem(device,"lightSensorStatus");
        if(node){
            Set_Devpro_Light_Sensor_State(node->type == cJSON_True);
            LOG_D(" Sync: Light Sensor State is %d",node->valueint);
        }
    }
    // 释放空间
    if (root)
    {
        cJSON_Delete(root);
    }
}

// 设备上报属性JSON格式
//{
//    "temperature": 25,    // 温度值，单位：℃
//    "humidity": 60,       // 湿度值，单位：%
//    "light": 800,           // 光照值，单位：lux
//    "smoke": 200,           // 烟雾浓度，单位：ppm
//    "timestamp": "2024-03-20T10:30:00"  // 数据采集时间
//}
/*
 *  功能：将数据组装成JSON格式
 *  参数：要上报的数据值、存储JSON字符串的缓冲区
 * 返回值：操作成功与否
 */
static rt_err_t mqtt_get_environment_datas(const Environment_t *envi_data, char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish environment datas failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddNumberToObject(root, "temperature", envi_data->temp);
    cJSON_AddNumberToObject(root, "humidity", envi_data->humi);
    cJSON_AddNumberToObject(root, "light", envi_data->light);
    cJSON_AddNumberToObject(root, "smoke", envi_data->smoke);
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish environment datas failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload environment datas failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}


rt_err_t Publish_Environment_Data(const Environment_t *envi_data)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_environment_datas(envi_data, &send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_SENSOR_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("environment publish failed (%d)!", result);
        goto __exit;
    }
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }

    return result;
}

// 设备上报属性JSON格式
//{
//    "warningLight": true,   // 警示灯状态
//    "fillLight": true,      // 补光灯状态
//    "exhaustFan": true,     // 排气扇状态
//    "alarm": false,         // 警报器状态
//    "emergencyDoor": false, // 应急逃生门状态
//    "dht11Status": false,       // DHT11传感器状态
//    "lightSensorStatus": false  // 光照传感器状态
//    "timestamp": "2024-03-20T10:30:00"  // 状态更新时间
//}
static rt_err_t mqtt_get_device_status(const Devices_t *dev_data, char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish device status failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddNumberToObject(root, "warningLight", dev_data->LED0_State);
    cJSON_AddNumberToObject(root, "fillLight", dev_data->LED1_State);
    cJSON_AddNumberToObject(root, "exhaustFan", dev_data->Relay_State);
    cJSON_AddNumberToObject(root, "alarm", dev_data->Beep_State);
    cJSON_AddNumberToObject(root, "emergencyDoor", dev_data->Door_State);
    cJSON_AddNumberToObject(root, "dht11Status", dev_data->DHT11_Sensor_State);
    cJSON_AddNumberToObject(root, "lightSensorStatus", dev_data->Light_Sensor_State);
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish device status failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload device status failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}


rt_err_t Publish_Device_Status(const Devices_t *dev_data)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_device_status(dev_data, &send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_DEVICE_STATUS_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("device status publish failed (%d)!", result);
        goto __exit;
    }
    rt_kprintf("device status publish success!\n");
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }

    return result;
}


// 设备上报属性JSON格式
//{
//    "commandId": "uuid",    // 控制命令的唯一标识
//    "success": true,        // 是否执行成功
//    "message": "执行成功",   // 执行结果描述
//    "timestamp": "2024-03-20T10:30:00"  // 响应时间
//}
static rt_err_t mqtt_get_dev_response(char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish device response failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddStringToObject(root, "commandId", UUID);
    cJSON_AddBoolToObject(root, "success", cJSON_True);
    cJSON_AddStringToObject(root, "message", "执行成功");
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish device response failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload device response failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}

rt_err_t Publish_Device_Response(void)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_dev_response(&send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_DEVICE_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("device response publish failed (%d)!", result);
        goto __exit;
    }
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }
    if(UUID){
        rt_free(UUID);
    }

    return result;
}

// 后端服务器下发的命令格式
//{
//    "commandId": "uuid",    // 控制命令的唯一标识
//    "warningLight": true,   // 控制警示灯
//    "fillLight": true,      // 控制补光灯
//    "exhaustFan": true,     // 控制排气扇
//    "alarm": false,         // 控制警报器
//    "emergencyDoor": false, // 控制应急逃生门
//    "timestamp": "2024-03-20T10:30:00"  // 控制命令时间
//}
static void mqtt_dev_control_callback(MQTTClient *c, MessageData *msg_data)
{

//    LOG_D("Enter mqtt_usr_callback!");
    // 为字符串的结尾添加结束标志
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
//    rt_kprintf("Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    // %.*s 用于以指定的长度输出字符串，两个参数分别为：要输出的字符个数、要输出的字符串的起始位置
//    rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);

    cJSON *root = NULL;
    cJSON *node = NULL;
    root = cJSON_Parse((char *)msg_data->message->payload);
    // 解析命令并执行
    if(root){
        node = cJSON_GetObjectItem(root,"commandId");
        if(node != NULL){
            LOG_D("commandId is %s",node->valuestring);
            UUID = strdup(node->valuestring);   // 最好是在响应函数中将UUID申请的空间free掉
        }
        node = cJSON_GetObjectItem(root,"warningLight");
        if(node != NULL){
            LOG_D("change warningLight to %d",node->valueint);
            Set_Devpro_LED0State(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(root,"fillLight");
        if(node != NULL){
            LOG_D("change fillLight to %d",node->valueint);
            emergency_light_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(root,"exhaustFan");
        if(node != NULL){
            LOG_D("change exhaustFan to %d",node->valueint);
            relay_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(root,"alarm");
        if(node != NULL){
            LOG_D("change alarm to %d",node->valueint);
            beep_change(node->type == cJSON_True);
        }
        node = cJSON_GetObjectItem(root,"emergencyDoor");
        if(node != NULL){
            LOG_D("change emergencyDoor to %d",node->valueint);
            my_pwm_angle_change(node->type == cJSON_True,90);
        }
    }

    // 调用发布函数向服务器响应执行结果
    if(Publish_Device_Response() != RT_EOK){
        LOG_E("device response publish failed!");
    }

    // 释放空间
    if (root)
    {
        cJSON_Delete(root);
    }
}


// 设备上报属性JSON格式
//{
//    "commandId": "uuid",    // 控制命令的唯一标识
//    "success": true,        // 是否设置成功
//    "message": "设置成功",  // 设置结果描述
//    "timestamp": "2024-03-20T10:30:00"  // 响应时间
//}
static rt_err_t mqtt_get_thresholds_response(char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish device response failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddStringToObject(root, "commandId", UUID);
    cJSON_AddBoolToObject(root, "success", cJSON_True);
    cJSON_AddStringToObject(root, "message", "设置成功");
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish device response failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload device response failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}

rt_err_t Publish_Thresholds_Response(void)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_thresholds_response(&send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_THRESHOLD_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("thresholds response publish failed (%d)!", result);
        goto __exit;
    }
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }
    if(UUID){
        rt_free(UUID);
    }

    return result;
}

// 后端服务器下发的命令格式
//{
//    "commandId": "uuid",    // 设置命令的唯一标识
//    "temperature": 26,    // 温度阈值
//    "humidity": 65,       // 湿度阈值
//    "lightUpper": 1000,     // 光照上限
//    "lightLower": 500,      // 光照下限
//    "timestamp": "2024-03-20T10:30:00"  // 设置时间
//}
static void mqtt_thresholds_callback(MQTTClient *c, MessageData *msg_data)
{

//    LOG_D("Enter mqtt_usr_callback!");
    // 为字符串的结尾添加结束标志
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
//    rt_kprintf("Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    // %.*s 用于以指定的长度输出字符串，两个参数分别为：要输出的字符个数、要输出的字符串的起始位置
//    rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);

    cJSON *root = NULL;
    cJSON *node = NULL;
    root = cJSON_Parse((char *)msg_data->message->payload);
    // 解析命令并执行
    if(root){
        node = cJSON_GetObjectItem(root,"commandId");
        if(node){
            UUID = strdup(node->valuestring);   // 最好是在响应函数中将UUID申请的空间free掉
        }
        node = cJSON_GetObjectItem(root,"temperature");
        if(node){
            Set_Devpro_Temp_Limit(node->valueint);
        }
        node = cJSON_GetObjectItem(root,"humidity");
        if(node){
            Set_Devpro_Humi_Limit(node->valueint);
        }
        node = cJSON_GetObjectItem(root,"lightUpper");
        if(node){
            Set_Devpro_Light_Up_Limit(node->valueint);
        }
        node = cJSON_GetObjectItem(root,"lightLower");
        if(node){
            Set_Devpro_Light_Down_Limit(node->valueint);
        }
    }

    // 调用发布函数向服务器响应执行结果
    if(Publish_Thresholds_Response() != RT_EOK){
        LOG_E("thresholds response publish failed!");
    }

    // 释放空间
    if (root)
    {
        cJSON_Delete(root);
    }
}

// 设备上报属性JSON格式
//{
//    "alarmType": 1,           // 告警类型：1-温度过高，2-温度回归正常，3-湿度过高，4-湿度回归正常，5-光照过强，6-光照过弱
//    "actualValue": 35,        // 实际值
//    "thresholdValue": 30,     // 阈值
//    "timestamp": "2024-03-20T10:30:00"  // 告警时间
//}
static rt_err_t mqtt_get_alarm_info(uint8_t alarm_type , int environment_data , int thresholds_data, char **out_buff)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;
    time_t now;
    struct tm *timeinfo;
    char *timestamp = RT_NULL;

    RT_ASSERT(out_buff);

    // 构建当前的时间戳
    now = time(RT_NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, strlen(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);

    root = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish alarm info failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddNumberToObject(root, "alarmType", alarm_type);
    cJSON_AddNumberToObject(root, "actualValue", environment_data);
    cJSON_AddNumberToObject(root, "thresholdValue", thresholds_data);
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish alarm info failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    *out_buff = rt_malloc(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("mqtt upload alarm info failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }
    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}


rt_err_t Publish_Alarm_Info( uint8_t alarm_type , int environment_data , int thresholds_data)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;

    result = mqtt_get_alarm_info(alarm_type , environment_data, thresholds_data , &send_buffer);
    if (result < 0)
    {
        goto __exit;
    }
    result = mqtt_publish(MQTT_ALARM_PUBTOPIC, send_buffer);
    if (result < 0)
    {
        LOG_E("alarm info publish failed (%d)!", result);
        goto __exit;
    }
__exit:
    if (send_buffer)
    {
        rt_free(send_buffer);
    }

    return result;
}


// 订阅相关的主题并绑定回调函数
void mqtt_sub_init(void)
{
    mqtt_subscribe(MQTT_SYNC_SUBTOPIC, mqtt_sync_callback);
    mqtt_subscribe(MQTT_DEVICE_SUBTOPIC, mqtt_dev_control_callback);
    mqtt_subscribe(MQTT_THRESHOLD_SUBTOPIC, mqtt_thresholds_callback);
}

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(mqtt_start, startup mqtt client);
MSH_CMD_EXPORT(mqtt_stop, stop mqtt client);
MSH_CMD_EXPORT(mqtt_publish, mqtt publish message to specified topic);
MSH_CMD_EXPORT(mqtt_subscribe,  mqtt subscribe topic);
MSH_CMD_EXPORT(mqtt_unsubscribe, mqtt unsubscribe topic);
#endif /* FINSH_USING_MSH */

