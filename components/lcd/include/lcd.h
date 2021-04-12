#include "i2c-lcd1602.h"
#include "smbus.h"

#ifndef LCD_H
#define LCD_H
void setup_lcd();
void write_string_position(char text[], int x,int y);
void clear_LCD();
void create_costum_char();
void write_mouth();
void write_eye();
void write_blink();

#endif