#include "lcd.h"
#include "i2c-lcd1602.h"
#include "smbus.h"
#include "esp_sntp.h"
#include <string.h>
#include <sys/time.h>
#include "generic.h"


#define TOP_ROW_MOUTH           2
#define BOTTOM_ROW_MOUTH        3
#define TOP_ROW_EYE             0
#define BOTTOM_ROW_EYE          1

char bottom_half, top_half, full;
int can_blink;


TimerHandle_t timer_blink;

void write_mouth(){
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

void timer_blink_callback( TimerHandle_t xTimer ){ 
	// Print current time to the screen
	time_t now;
    struct tm timeinfo;
    time(&now);
	
	char strftime_buf[20];
	localtime_r(&now, &timeinfo);
	sprintf(&strftime_buf[0], "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
	size_t timeSize = strlen(strftime_buf);
	for (int i = 0; i < timeSize; i++) {
        if(can_blink){
		write_blink();
        write_eye();
        }
	}
}

void clear_face(){
    clear_LCD();
    can_blink = 0;
}

void set_initial_face(){
    
    top_half = I2C_LCD1602_CHARACTER_CUSTOM_1;
    bottom_half = I2C_LCD1602_CHARACTER_CUSTOM_2;
    full = I2C_LCD1602_CHARACTER_BLOCK;

    write_mouth();
    write_eye();
    can_blink = 1;

    int id = 1;
	timer_blink = xTimerCreate("Blink", pdMS_TO_TICKS(10000), pdTRUE, ( void * )id, &timer_blink_callback);
	if( xTimerStart(timer_blink, 10 ) != pdPASS ) {
		ESP_LOGE("main", "Cannot start 1 second timer");
    }
}