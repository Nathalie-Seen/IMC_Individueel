#include "i2c-lcd1602.h"
#include "smbus.h"

#ifndef LCD_H
#define LCD_H
void setup_lcd();
void clear_LCD();
void clear_cell(int collum, int row);
void write_char(int collum, int row, char character);

#endif