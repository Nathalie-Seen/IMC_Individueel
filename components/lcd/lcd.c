#include <stdio.h>
#include "i2c-lcd1602.h"
#include "smbus.h"

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

char clear;
i2c_lcd1602_info_t *lcd_info;

void create_costum_char();

//initialize the 12c master
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

void setup_lcd()
{
    //initialize the i2c master
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = 0x27;

    //initialize the smbus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // set the info of the used lcd
    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);
    i2c_lcd1602_set_backlight(lcd_info, true);

    create_costum_char();
}

void write_char(int collum, int row, char character){
    //set the cursor on the right cell
    i2c_lcd1602_move_cursor(lcd_info, collum, row);
    //write the given char
    i2c_lcd1602_write_char(lcd_info, character);
}

//clears the whole LCD
void clear_LCD(){
        i2c_lcd1602_clear(lcd_info); 
}

//clears the given cell
void clear_cell(int collum, int row){
    write_char(collum, row, clear);
}


void create_costum_char(){
    //creates a costum char where nothing is active
    const unsigned char clear_a[8] = {
	0b00000,0b00000,0b00000,0b00000,
	0b00000,0b00000,0b00000,0b00000
    };
    i2c_lcd1602_define_char(lcd_info, 0, clear_a);

    //creates a costum char where the top half of the cell active is
    const unsigned char top_half_a[8] = {
	0b11111,0b11111,0b11111,0b11111,
	0b00000,0b00000,0b00000,0b00000
    };
    i2c_lcd1602_define_char(lcd_info, 1, top_half_a);

    //creates a costum char where the bottom half of the cell active is
    const unsigned char bottom_half_a[8] = {
	0b00000,0b00000,0b00000,0b00000,
	0b11111,0b11111,0b11111,0b11111
    };
    i2c_lcd1602_define_char(lcd_info, 2, bottom_half_a);
    
    //set the char clear to the costom char with index 0
    clear = I2C_LCD1602_INDEX_CUSTOM_0;
}

