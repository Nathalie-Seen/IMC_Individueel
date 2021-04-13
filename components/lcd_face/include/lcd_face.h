#include "i2c-lcd1602.h"
#include "smbus.h"

#ifndef LCD_FACE_H
#define LCD_FACE_H
void initialize_face();
void clear_face();

void set_face();
void set_emotion();

enum emotion{Happy,Neutral,Angry,Sad};

#endif