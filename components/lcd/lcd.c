/**
 * @file lcd.c
 * @brief LCD driver
 */
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32/rom/uart.h"
#include "string.h"

#include "lcd.h"
#include "i2c-lcd1602.h"
#include "smbus.h"

#include "i2c-lcd1602.h"
#include "lcd.h"
#include "smbus.h"
#include "esp_sntp.h"



#define TAG "lcd"
#undef USE_STDIN

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                    
#define I2C_MASTER_RX_BUF_LEN    0                     
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        18
#define I2C_MASTER_SCL_IO        23
#define LCD_NUM_ROWS			 4
#define LCD_NUM_COLUMNS			 40
#define LCD_NUM_VIS_COLUMNS		 20

#define TOP_ROW_MOUTH           2
#define BOTTOM_ROW_MOUTH        3
#define TOP_ROW_EYE             0
#define BOTTOM_ROW_EYE          1

i2c_lcd1602_info_t *lcd_info;
char bottom_half;
char top_half;
char clear;
char full;

static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE; 
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

static uint8_t _wait_for_user(void)
{
    uint8_t c = 0;
#ifdef USE_STDIN
    while (!c)
    {
       STATUS s = uart_rx_one_char(&c);
       if (s == OK) {
          printf("%c", c);
       }
    }
#else
    vTaskDelay(1000 / portTICK_RATE_MS);
#endif
    return c;
}

//Setup for LCD
void setup_lcd()
{
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = 0x27;

    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);
    i2c_lcd1602_set_backlight(lcd_info, true);
}

// from here on is the code for the individual assignment
void write_char(int collum, int row, char character){
    i2c_lcd1602_move_cursor(lcd_info, collum, row);
    i2c_lcd1602_write_char(lcd_info, character);
}

//Method to clear the LCD
void clear_LCD(){
        i2c_lcd1602_clear(lcd_info); 
}
//clear a cell at the given place
void clear_cell(int collum, int row){
    write_char(collum, row, clear);
}


void create_costum_char(){
    const unsigned char top_half_a[8] = {
	0b11111,0b11111,0b11111,0b11111,
	0b00000,0b00000,0b00000,0b00000
    };
   const unsigned char bottom_half_a[8] = {
	0b00000,0b00000,0b00000,0b00000,
	0b11111,0b11111,0b11111,0b11111
    };
 const unsigned char clear_a[8] = {
	0b00000,0b00000,0b00000,0b00000,
	0b00000,0b00000,0b00000,0b00000
    };
    i2c_lcd1602_define_char(lcd_info, 0, clear_a);
    i2c_lcd1602_define_char(lcd_info, 1, top_half_a);
    i2c_lcd1602_define_char(lcd_info, 2, bottom_half_a);

    clear = I2C_LCD1602_INDEX_CUSTOM_0;
    top_half = I2C_LCD1602_CHARACTER_CUSTOM_1;
    bottom_half = I2C_LCD1602_CHARACTER_CUSTOM_2;
    full = I2C_LCD1602_CHARACTER_BLOCK;

    
}

void write_mouth(){
    _wait_for_user();
    i2c_lcd1602_home(lcd_info);
    write_char(6,TOP_ROW_MOUTH,bottom_half);
    write_char(6,BOTTOM_ROW_MOUTH,top_half);
    write_char(7,BOTTOM_ROW_MOUTH,full);
    write_char(8,BOTTOM_ROW_MOUTH,full);
    write_char(9,BOTTOM_ROW_MOUTH,full);
    write_char(10,BOTTOM_ROW_MOUTH,full);
    write_char(11,BOTTOM_ROW_MOUTH,full);
    write_char(12,BOTTOM_ROW_MOUTH,full);
    write_char(13,BOTTOM_ROW_MOUTH,top_half);
    write_char(13,TOP_ROW_MOUTH,bottom_half);
}

void write_eye(){
    write_char(4,TOP_ROW_EYE,full);
    write_char(4,BOTTOM_ROW_EYE,full);
    clear_cell(3,BOTTOM_ROW_EYE);
    clear_cell(5,BOTTOM_ROW_EYE);

    write_char(15,TOP_ROW_EYE,full);
    write_char(15,BOTTOM_ROW_EYE,full);
    clear_cell(14,BOTTOM_ROW_EYE);
    clear_cell(16,BOTTOM_ROW_EYE);
}

void write_blink(){
    clear_cell(4,TOP_ROW_EYE);
    write_char(3,BOTTOM_ROW_EYE,full);
    write_char(5,BOTTOM_ROW_EYE,full);

    clear_cell(15,TOP_ROW_EYE);
    write_char(14,BOTTOM_ROW_EYE,full);
    write_char(16,BOTTOM_ROW_EYE,full);
}

