#include <my_file.h>
#include <my_TCP.h>

// 全局变量，记录上次读取的位置
static long dht11_last_position = 0;
static long light_last_position = 0;

uint8_t fp_auto_reset_flag = 1;

static int rt_hw_spi_flash_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    rt_hw_spi_device_attach("spi1", "spi10", GPIOB, GPIO_PIN_14);// spi10 表示挂载在 spi1总线上的 0号设备,PB14是片选，这一步就可以将从设备挂在到总线中。
    if (RT_NULL == rt_sfud_flash_probe("W25Q128", "spi10"))  //注册块设备，这一步可以将外部                                flash抽象为系统的块设备
    {
        return -RT_ERROR;
    };

    return RT_EOK;
}
/* 导出到自动初始化 */
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);

//int dfs_mount_init(void)    //断电保存
//{
//     rt_device_t dev;
//        dev = rt_device_find("W25Q128");
//        if(dev != RT_NULL)
//        {
//         if(dfs_mount("W25Q128", "/", "elm", 0, 0) == 0)
//             {
//                rt_kprintf("spi_flash mount to spi!\n");
//             }
//         else
//             {
//                rt_kprintf("spi_flash mount to spi failed!\n");
//                dfs_mkfs("elm","W25Q128");
//                if(dfs_mount("W25Q128", "/","elm",0,0) == 0)  //注册块设备，这一步可以将外部flash抽象为系统的块设备
//                   {
//
//                       return -RT_ERROR;
//                   }
//            }
//        }
//    //每一次运行时记录两个文件的末尾位置作为当前的起始位置
//    set_dht11_read_position_end();
//    set_light_read_position_end();
//    return RT_EOK;
//}

int dfs_mount_init(void)      //断电丢失
{

    dfs_mkfs("elm","W25Q128");
    if(dfs_mount("W25Q128", "/","elm",0,0) == 0)  //注册块设备，这一步可以将外部flash抽象为系统的块设备
    {

        return -RT_ERROR;

    }

    return RT_EOK;
}


/* 导出到自动初始化 */
INIT_COMPONENT_EXPORT(dfs_mount_init);



// 写入温湿度数据
void write_dht11_data(rt_uint8_t temp, rt_uint8_t humi) {
    FILE* fp = fopen("DHT11data.txt", "a");  // 以文本追加模式打开
    if (fp) {
        fprintf(fp, "%d %d\n", temp, humi);  // 以空格分隔的文本形式写入
        fclose(fp);
    }
}

// 读取温湿度数据并通过TCP发送
void read_and_send_dht11_data(void) {
    FILE* fp = fopen("DHT11data.txt", "r");
    if (!fp) {
        rt_kprintf("open DHT11data.txt filed!\n");
        return;
    }
    // 移动文件指针到上次读取的位置
    fseek(fp, dht11_last_position, SEEK_SET);

    char line[10];  // 足够存储两个2位数字和空格
    char *dht11data_str[DHT11_ARRAY_SIZE*2+1];  //存储分割后的字符串
    uint8_t dht11data_index=0; //分割字符串数组索引

    uint8_t temp, humi;
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < DHT11_ARRAY_SIZE) {
        //获取第一个字串
        dht11data_str[dht11data_index]=strtok(line," ");  //以字符“ ”进行分割
        while(dht11data_str[dht11data_index]!=NULL){
            dht11data_str[++dht11data_index]=strtok(NULL," ");  //以字符“ ”进行分割
        }
        temp=strtoul(dht11data_str[0], 0, 10);
        humi=strtoul(dht11data_str[1], 0, 10);
        my_send_DHT11data(temp,humi,1);
        count++;
    }
    // 更新上次读取的位置
    dht11_last_position = ftell(fp);
    fclose(fp);
}

// 重置读取位置（如果需要从头开始读）
void reset_dht11_read_position(void)
{
    dht11_last_position = 0;
    my_send_fp_reset_feedback(0);
}

void set_dht11_read_position_end(void)
{
    FILE* fp = fopen("DHT11data.txt", "r");
    if (!fp) {
        rt_kprintf("open DHT11data.txt filed!\n");
        return;
    }
    // 使用fseek函数定位文件指针到文件末尾
    fseek(fp, 0, SEEK_END);
    // 使用ftell函数获取文件指针的当前位置
    dht11_last_position = ftell(fp);
    fclose(fp);
}

// 写入光敏数据
void write_light_data(rt_uint32_t light_value) {
    FILE* fp = fopen("Lightdata.txt", "a");  // 以文本追加模式打开
    if (fp) {
        fprintf(fp, "%d\n", light_value);  // 以文本形式写入
        fclose(fp);
    }
}

// 读取光敏数据并通过TCP发送
void read_and_send_light_data(void) {
    FILE* fp = fopen("Lightdata.txt", "r");
    if (!fp) return;
    // 移动文件指针到上次读取的位置
    fseek(fp, light_last_position, SEEK_SET);
    char line[10];  // 足够存储一个4位数字
    rt_uint32_t light_value;
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < LIGHT_ARRAY_SIZE) {
        if (sscanf(line, "%d", &light_value) == 1) {
            // 通过TCP发送数据，这里需要您的TCP发送函数
            my_send_Lightdata(light_value,1);
            count++;
        }
    }
    // 更新上次读取的位置
    light_last_position = ftell(fp);
    fclose(fp);
}
// 重置读取位置（如果需要从头开始读）
void reset_light_read_position(void)
{
    light_last_position = 0;
    my_send_fp_reset_feedback(1);
}
//设置追加位置为文件末尾，用于每次运行程序时从文件尾进行读写
void set_light_read_position_end(void)
{
    FILE* fp = fopen("Lightdata.txt", "r");
    if (!fp) {
        rt_kprintf("open Lightdata.txt filed!\n");
        return;
    }
    // 使用fseek函数定位文件指针到文件末尾
    fseek(fp, 0, SEEK_END);
    // 使用ftell函数获取文件指针的当前位置
    light_last_position = ftell(fp);
    fclose(fp);
}

