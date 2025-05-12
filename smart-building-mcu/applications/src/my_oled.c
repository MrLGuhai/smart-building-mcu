#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"

void my_oled_test(void)
{
    char message[] = "My OLED Test!";

    ssd1306_Init();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2,0);
    ssd1306_WriteString("Testing...", Font_11x18, White);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString(message, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void my_oled_init(void)
{
    ssd1306_Init();
    ssd1306_SetCursor(2,0);
    ssd1306_WriteString("Sensor_Data", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void my_clean_line(uint8_t y)
{
    ssd1306_SetCursor(0, y);
    ssd1306_WriteString("                    ", Font_7x10, White);
    //ssd1306_UpdateScreen();
}

void my_oled_print_led(const char *message)
{
    ssd1306_SetCursor(2, 60);
    ssd1306_WriteString(message, Font_7x10, White);
    //ssd1306_UpdateScreen();
}

void my_oled_print_dht(uint8_t temp,uint8_t humi)
{
    const char str_data[25];
    sprintf(str_data,"temp:%dC humi:%d",temp,humi);
    sprintf(str_data,"%s%s",str_data,"%\n");
    //rt_kprintf(str_data);
    my_clean_line(30);
    ssd1306_SetCursor(2, 30);
    ssd1306_WriteString(str_data, Font_7x10, White);
    //ssd1306_UpdateScreen();
}

void my_oled_print_light(rt_uint32_t value)
{
    const char str_data[50]={""};
    sprintf(str_data,"light:%dL\n",value);
    //sprintf(str_data,"%s%s",str_data,"%\n");
    //rt_kprintf(str_data);
//    my_clean_line(50);
    ssd1306_SetCursor(2, 50);
    ssd1306_WriteString(str_data, Font_6x8, White);
//    ssd1306_UpdateScreen();
}

void my_oled_print_mq2(rt_uint32_t value)
{
    const char str_data[25]={""};
    sprintf(str_data,"mq2:%dppm\n",value);
    //sprintf(str_data,"%s%s",str_data,"%\n");
    //rt_kprintf(str_data);
//    my_clean_line(50);
    ssd1306_SetCursor(70, 50);
    ssd1306_WriteString(str_data, Font_6x8, White);
    ssd1306_UpdateScreen();
}
