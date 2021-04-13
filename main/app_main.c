#include <stdio.h>
#include "lcd_face.h"
#include "lcd.h"
#include "touchpad.h"

void app_main(void)
{
    initialize_touchpad();
    setup_lcd();
    set_face();
}