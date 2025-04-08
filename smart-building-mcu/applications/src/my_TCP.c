#include <my_TCP.h>
#include <my_adc.h>
#include <my_file.h>
#include <my_dht11.h>

int sock;
char *recv_data;                                /* 接收数据 */

const char my_led0_Comm_ON[] = "LED0 ON";         /* LED0开启命令 */
const char my_led0_Comm_OFF[] = "LED0 OFF";       /* LED0关闭命令 */
const char my_led1_Comm_ON[] = "LED1 ON";         /* LED1开启命令 */
const char my_led1_Comm_OFF[] = "LED1 OFF";       /* LED1关闭命令 */

const char my_led0_send_ON[] = "LED0 Turn On";    /* LED0打开反馈消息 */
const char my_led0_send_OFF[] = "LED0 Turn Off";  /* LED0关闭反馈消息 */
const char my_led1_send_ON[] = "LED1 Turn On";    /* LED1打开反馈消息 */
const char my_led1_send_OFF[] = "LED1 Turn Off";  /* LED1关闭反馈消息 */

const char my_DHT11data_Comm_Load[]  = "DHT11data Load";   /* 从文件中加载DHT11数据并发送TCP服务器*/
const char my_Lightdata_Comm_Load[]  = "Lightdata Load";   /* 从文件中加载光敏传感器数据并发送TCP服务器*/

const char my_temp_set_limit_Comm[] = "Set templimit";  /* 设置温度阈值命令 */
const char my_temp_feedback_on[]  = "WARNING!!! Abnormal temperature!";  /* 温度异常警告消息 */
const char my_temp_feedback_off[] = "Temperature return to normal! Automatically turn off the beep!";  /* 温度回归正常，自动关闭蜂鸣器 */

const char my_light_set_limit_Comm[] = "Set lightlimit";  /* 设置光敏阈值命令 */
const char my_light_feedback_on[]  = "Light too dark! Automatically turn on the emergency light!";  /* 光线过暗，自动开启应急灯反馈消息 */
const char my_light_feedback_off[] = "Light return to normal! Automatically turn off the emergency light!";/* 光线回归正常，自动关闭应急灯 */

const char DHT11_file_position_reset_Comm[] = "DHT11 file position reset";  /* 重置DHT11data.txt文件的读取位置为文件开头命令 */
const char DHT11_file_position_reset_feedback[] = "DHT11 file position reset success";  /* 重置DHT11data.txt文件的读取位置成功反馈消息 */
const char Light_file_position_reset_Comm[] = "Light file position reset";  /* 重置DHT11data.txt文件的读取位置为文件开头命令 */
const char Light_file_position_reset_feedback[] = "Light file position reset success";  /* 重置Lightdata.txt文件的读取位置成功反馈消息 */

int my_send_temp_feedback(uint8_t flag)
{
    int ret;
    if(flag==0){
        ret = send(sock, my_temp_feedback_off, strlen(my_temp_feedback_off), 0);
    }else if(flag==1){
        ret = send(sock, my_temp_feedback_on, strlen(my_temp_feedback_on), 0);
    }

    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n Msg send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n Msg Send warning,send function return 0.\r\n");
    }
    return ret;
}

int my_send_light_feedback(uint8_t flag)
{
    int ret;
    if(flag==0){
        ret = send(sock, my_light_feedback_off, strlen(my_light_feedback_off), 0);
    }else if(flag==1){
        ret = send(sock, my_light_feedback_on, strlen(my_light_feedback_on), 0);
    }

    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n Msg send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n Msg Send warning,send function return 0.\r\n");
    }
    return ret;
}

int my_send_fp_reset_feedback(uint8_t flag)
{
    int ret;
    if(flag==0){
        ret = send(sock, DHT11_file_position_reset_feedback, strlen(DHT11_file_position_reset_feedback), 0);
    }else if(flag==1){
        ret = send(sock, Light_file_position_reset_feedback, strlen(Light_file_position_reset_feedback), 0);
    }

    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n Msg send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n Msg Send warning,send function return 0.\r\n");
    }
    return ret;
}

int my_send_msg(const char *send_data,int size,int flag)
{
    int ret;
    ret = send(sock, send_data, size, flag);
    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n Msg send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n Msg Send warning,send function return 0.\r\n");
    }
    return ret;
}

int my_send_DHT11data(uint8_t data1,uint8_t data2,int flag)
{
    int ret;
    const char send_data[15];
    sprintf(send_data,"temp:%dC humi:%d",data1,data2);
    sprintf(send_data,"%s%s",send_data,"%\n");
    ret = send(sock, send_data, strlen(send_data), flag);
    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n DHT11 send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n DHT11 Send warning,send function return 0.\r\n");
    }
    return ret;
}

int my_send_Lightdata(rt_uint32_t value,int flag)
{
    int ret;
    const char send_data[15];
    sprintf(send_data,"light:%dL\n",value);
    ret = send(sock, send_data, strlen(send_data), flag);
    if (ret < 0)
    {
        /* 发送失败，关闭这个连接 */
        closesocket(sock);
        rt_kprintf("\n Light send error,close the socket.\r\n");
        rt_free(recv_data);
    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        rt_kprintf("\n Light Send warning,send function return 0.\r\n");
    }
    return ret;
}

void my_control_led(rt_base_t led_pin,int led_state)
{
    rt_pin_write(led_pin, led_state);  //根据参数点亮或熄灭对应的LED
}

void my_tcp_control(int argc, char **argv)
{
    int ret;
    struct hostent *host;
    int bytes_received;
    struct sockaddr_in server_addr;
    const char *url;
    int port;

    char temp_recv_comm[50];
    char *strtok_str[5];  //存储分割后的字符串
    uint8_t strtok_index=0; //分割字符串数组索引
    int new_temp_limit,new_light_limit; //用于接收新的温度阈值、新的光敏阈值
    if (argc < 3)
    {
        rt_kprintf("Usage: tcpclient URL PORT\n");
        rt_kprintf("Like: tcpclient 192.168.31.116 9011\n");
        return ;
    }
    url = argv[1];
    port = strtoul(argv[2], 0, 10);
    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(url);
    /* 分配用于存放接收数据的缓冲 */
    recv_data = rt_malloc(BUFSZ);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory tcp\n");
        return;
    }
    /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建socket失败 */
        rt_kprintf("Socket error\n");
        /* 释放接收缓冲 */
        rt_free(recv_data);
        return;
    }
    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    /* 连接到服务端 */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 连接失败 */
        rt_kprintf("Connect fail!\n");
        closesocket(sock);
        /*释放接收缓冲 */
        rt_free(recv_data);
        return;
    }
    while (1)
    {
        /* 从sock连接中接收最大BUFSZ - 1字节数据 */
        bytes_received = recv(sock, recv_data, BUFSZ - 1, 0);
        if (bytes_received < 0)
        {
            /* 接收失败，关闭这个连接 */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");
            /* 释放接收缓冲 */
            rt_free(recv_data);
            break;
        }
        else if (bytes_received == 0)
        {
            /* 默认 recv 为阻塞模式，此时收到0认为连接出错，关闭这个连接 */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");
            /* 释放接收缓冲 */
            rt_free(recv_data);
            break;
        }
        /* 有接收到数据，把末端清零 */
        recv_data[bytes_received] = '\0';
        if (strncmp(recv_data, "q", 1) == 0 || strncmp(recv_data, "Q", 1) == 0)
        {
            /* 如果是首字母是q或Q，关闭这个连接 */
            closesocket(sock);
            rt_kprintf("\n got a 'q' or 'Q',close the socket.\r\n");
            /* 释放接收缓冲 */
            rt_free(recv_data);
            break;
        }else if (strcmp(recv_data,my_led0_Comm_ON) == 0) {
            rt_kprintf("\n Receive LED0 ON Comm \n");
            my_control_led(LED0_PIN,PIN_LOW);      //点亮LED0
            ret=my_send_msg( my_led0_send_ON, strlen(my_led0_send_ON), 0);  //向TCP服务器发送反馈消息
            if(ret < 0)
                break;
        }else if (strcmp(recv_data,my_led0_Comm_OFF) == 0) {
            rt_kprintf("\n Receive LED0 OFF Comm \n");
            my_control_led(LED0_PIN,PIN_HIGH);      //熄灭LED0
            ret = my_send_msg( my_led0_send_OFF, strlen(my_led0_send_OFF), 0);  //向TCP服务器发送反馈消息
            if(ret < 0)
                break;
        }else if (strcmp(recv_data,my_led1_Comm_ON) == 0) {
            rt_kprintf("\n Receive LED1 ON Comm \n");
            my_control_led(LED1_PIN,PIN_LOW);      //点亮LED1
            ret=my_send_msg( my_led1_send_ON, strlen(my_led1_send_ON), 0);  //向TCP服务器发送反馈消息
            if(ret < 0)
                break;
        }else if (strcmp(recv_data,my_led1_Comm_OFF) == 0) {
            rt_kprintf("\n Receive LED1 OFF Comm \n");
            my_control_led(LED1_PIN,PIN_HIGH);      //熄灭LED0
            ret = my_send_msg(my_led1_send_OFF, strlen(my_led1_send_OFF), 0);   //向TCP服务器发送反馈消息
            if(ret < 0)
                break;
        }else if (strcmp(recv_data,my_DHT11data_Comm_Load) == 0) {  //从Flash中读取DHT11数据
            rt_kprintf("\n Receive DHT11data Load Comm \n");
            read_and_send_dht11_data();
        }else if (strcmp(recv_data,my_Lightdata_Comm_Load) == 0) {  //从Flash中读取光敏数据
            rt_kprintf("\n Receive Lightdata Load Comm \n");
            read_and_send_light_data();
        }else if (strcmp(recv_data,DHT11_file_position_reset_Comm) == 0) {  //重置DHT11data.txt的文件指针为0
            rt_kprintf("\n Receive DHT11 file position reset Comm \n");
            reset_dht11_read_position();
        }else if (strcmp(recv_data,Light_file_position_reset_Comm) == 0) {  //重置Lightdata.txt的文件指针为0
            rt_kprintf("\n Receive Light file position reset Comm \n");
            reset_light_read_position();
        }
        else    //不是常规类型命令，拆分后再次进行比较
        {
            strcpy(temp_recv_comm,recv_data);   //保存当前接收到的数据
            strtok_index=0; //索引清零
            //获取第一个字串
            strtok_str[strtok_index]=strtok(temp_recv_comm,":");  //以字符“:”进行分割
            while(strtok_str[strtok_index]!=NULL){
                //rt_kprintf("strtok_str[%d] is %s\n",strtok_index,strtok_str[strtok_index]);
                strtok_str[++strtok_index]=strtok(NULL,":");  //以字符“:”进行分割
            }
            if (strcmp(strtok_str[0],my_temp_set_limit_Comm) == 0){
                new_temp_limit=strtoul(strtok_str[1], 0, 10);
                set_temp_limit(new_temp_limit);
            }else if (strcmp(strtok_str[0],my_light_set_limit_Comm) == 0){
                new_light_limit=strtoul(strtok_str[1], 0, 10);
                set_light_limit(new_light_limit);
            }
            else{
                /* 在控制终端显示收到的数据 */
                rt_kprintf("\nReceived data = %s \n", recv_data);
            }
        }
    }
    return;
}

MSH_CMD_EXPORT(my_tcp_control, my tcp control);
