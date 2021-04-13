#include <string.h>
#include <sys/time.h>
#include "lcd.h"
#include "i2c-lcd1602.h"
#include "smbus.h"
#include "esp_sntp.h"
#include "generic.h"


#define TOP_ROW_MOUTH           2
#define BOTTOM_ROW_MOUTH        3
#define TOP_ROW_EYE             0
#define BOTTOM_ROW_EYE          1

char bottom_half, top_half, full;
int mouth_columns[8] = {6,7,8,9,10,11,12,13};
int left_eye_columns[3] = {3,4,5};
int right_eye_columns[3] = {14,15,16};
int can_blink;

enum emotion{Happy,Neutral,Angry,Sad};
enum emotion feeling;
TimerHandle_t timer_blink;

void set_emotion(enum emotion emot){
    feeling = emot;
}
void write_mouth(){
    size_t amount_mouth_columns = sizeof(mouth_columns)/sizeof(mouth_columns[0]);

    for(int i = 0; i<amount_mouth_columns; ++i){
            clear_cell(mouth_columns[i],BOTTOM_ROW_MOUTH);
            clear_cell(mouth_columns[i],TOP_ROW_MOUTH);
        }


    switch(feeling){
        case Neutral:
        case Happy:
        for(int i = 1; i<amount_mouth_columns-1; ++i){
            write_char(mouth_columns[i],BOTTOM_ROW_MOUTH,full);
        }

        write_char(mouth_columns[0],TOP_ROW_MOUTH,bottom_half);
        write_char(mouth_columns[0],BOTTOM_ROW_MOUTH,top_half);

        write_char(mouth_columns[7],BOTTOM_ROW_MOUTH,top_half);
        write_char(mouth_columns[7],TOP_ROW_MOUTH,bottom_half);
        break;
        case Angry:
        
        case Sad:
        for(int i = 1; i<amount_mouth_columns-1; ++i){
            write_char(mouth_columns[i],TOP_ROW_MOUTH,full);
        }

        write_char(mouth_columns[0],TOP_ROW_MOUTH,bottom_half);
        write_char(mouth_columns[0],BOTTOM_ROW_MOUTH,top_half);

        write_char(mouth_columns[7],BOTTOM_ROW_MOUTH,top_half);
        write_char(mouth_columns[7],TOP_ROW_MOUTH,bottom_half);
        break;
    }
}
void clear_eye_sides(){
    clear_cell(left_eye_columns[0],BOTTOM_ROW_EYE);
    clear_cell(left_eye_columns[2],BOTTOM_ROW_EYE);
    clear_cell(right_eye_columns[0],BOTTOM_ROW_EYE);
    clear_cell(right_eye_columns[2],BOTTOM_ROW_EYE);
    clear_cell(left_eye_columns[0],TOP_ROW_EYE);
    clear_cell(left_eye_columns[2],TOP_ROW_EYE);
    clear_cell(right_eye_columns[0],TOP_ROW_EYE);
    clear_cell(right_eye_columns[2],TOP_ROW_EYE);
}

void write_eye(){
    write_char(left_eye_columns[1],TOP_ROW_EYE,full);
    write_char(left_eye_columns[1],BOTTOM_ROW_EYE,full);
    
    write_char(right_eye_columns[1],TOP_ROW_EYE,full);
    write_char(right_eye_columns[1],BOTTOM_ROW_EYE,full);

    clear_eye_sides();
   
    switch(feeling){
        case Neutral:
        break;
        case Happy:
        write_char(left_eye_columns[0],BOTTOM_ROW_EYE,bottom_half);
        write_char(left_eye_columns[2],BOTTOM_ROW_EYE,bottom_half);
        write_char(right_eye_columns[0],BOTTOM_ROW_EYE,bottom_half);
        write_char(right_eye_columns[2],BOTTOM_ROW_EYE,bottom_half);
        break;
        case Angry:
        write_char(left_eye_columns[0],TOP_ROW_EYE,top_half);
        write_char(left_eye_columns[2],TOP_ROW_EYE,bottom_half);
        write_char(right_eye_columns[0],TOP_ROW_EYE,bottom_half);
        write_char(right_eye_columns[2],TOP_ROW_EYE,top_half);
        break;
        case Sad:
        write_char(left_eye_columns[0],TOP_ROW_EYE,bottom_half);
        write_char(left_eye_columns[2],TOP_ROW_EYE,top_half);
        write_char(right_eye_columns[0],TOP_ROW_EYE,top_half);
        write_char(right_eye_columns[2],TOP_ROW_EYE,bottom_half);
        break;
    }
    
}

void write_blink(){
    clear_cell(4,TOP_ROW_EYE);
    write_char(3,BOTTOM_ROW_EYE,full);
    write_char(5,BOTTOM_ROW_EYE,full);

    clear_cell(15,TOP_ROW_EYE);
    write_char(14,BOTTOM_ROW_EYE,full);
    write_char(16,BOTTOM_ROW_EYE,full);
}

//call this method after 10 seconds and sets the eyes on blink mode
void timer_blink_callback( TimerHandle_t xTimer ){ 
	size_t timeSize = 10;
	for (int i = 0; i < timeSize; i++) {
        if(can_blink){
		write_blink();
        }
	}
    //open the eyes again
    write_eye();
}

void clear_face(){
    clear_LCD();
    can_blink = 0;
}

void initialize_face(){
    
    full = I2C_LCD1602_CHARACTER_BLOCK;
    top_half = I2C_LCD1602_CHARACTER_CUSTOM_1;
    bottom_half = I2C_LCD1602_CHARACTER_CUSTOM_2;

    can_blink = 0;
    feeling = Neutral;

    int id = 1;
    //set a timer for 10 seconds to make it look like a blink
	timer_blink = xTimerCreate("Blink", pdMS_TO_TICKS(10000), pdTRUE, ( void * )id, &timer_blink_callback);
	if( xTimerStart(timer_blink, 10 ) != pdPASS ) {
		ESP_LOGE("lcd_face", "Cannot start 10 second timer");
    }
}

void set_face(){
    write_mouth();
    write_eye();
    can_blink = 1;
}

