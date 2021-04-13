#include <string.h>
#include <sys/time.h>

#include "lcd.h"
#include "lcd_face.h"
#include "i2c-lcd1602.h"
#include "smbus.h"
#include "esp_sntp.h"
#include "generic.h"


#define TAG "lcd_face"
//define the rows for the face structures
#define TOP_ROW_EYE             0
#define BOTTOM_ROW_EYE          1
#define TOP_ROW_MOUTH           2
#define BOTTOM_ROW_MOUTH        3


char bottom_half, top_half, full;

//set arrays for the collums of the face structures
int mouth_columns[8] = {6,7,8,9,10,11,12,13};
int left_eye_columns[3] = {3,4,5};
int right_eye_columns[3] = {14,15,16};

int can_blink;

enum emotion feeling;

TimerHandle_t timer_blink;

void set_emotion(enum emotion emot){
    feeling = emot;
}


void write_mouth(){
    size_t amount_mouth_columns = sizeof(mouth_columns)/sizeof(mouth_columns[0]);

    //clear the mouth first before drawing it again
    for(int i = 0; i<amount_mouth_columns; ++i){
            clear_cell(mouth_columns[i],BOTTOM_ROW_MOUTH);
            clear_cell(mouth_columns[i],TOP_ROW_MOUTH);
        }

    switch(feeling){
        case Neutral:
        //mouth of happy and neutral are the same
        case Happy:
        //set a row active for the mouth, 
        //from the 2nd to the 2nd last of the columns from the mouth
        for(int i = 1; i<amount_mouth_columns-1; ++i){
            write_char(mouth_columns[i],BOTTOM_ROW_MOUTH,full);
        }

        //add the corners to the 1st and last plase of the columns from the mouth
        write_char(mouth_columns[0],TOP_ROW_MOUTH,bottom_half);
        write_char(mouth_columns[0],BOTTOM_ROW_MOUTH,top_half);

        write_char(mouth_columns[7],BOTTOM_ROW_MOUTH,top_half);
        write_char(mouth_columns[7],TOP_ROW_MOUTH,bottom_half);
        break;
        case Angry:
        //mouth of angry and sad are the same
        case Sad:
        //set a row active for the mouth, 
        //from the 2nd to the 2nd last of the columns from the mouth
        for(int i = 1; i<amount_mouth_columns-1; ++i){
            write_char(mouth_columns[i],TOP_ROW_MOUTH,full);
        }
        //add the corners to the 1st and last plase of the columns from the mouth
        write_char(mouth_columns[0],TOP_ROW_MOUTH,bottom_half);
        write_char(mouth_columns[0],BOTTOM_ROW_MOUTH,top_half);

        write_char(mouth_columns[7],BOTTOM_ROW_MOUTH,top_half);
        write_char(mouth_columns[7],TOP_ROW_MOUTH,bottom_half);
        break;
    }
}
//clear the first and last column off the eyes
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
    //create basic shape of the eyes
    write_char(left_eye_columns[1],TOP_ROW_EYE,full);
    write_char(left_eye_columns[1],BOTTOM_ROW_EYE,full);
    
    write_char(right_eye_columns[1],TOP_ROW_EYE,full);
    write_char(right_eye_columns[1],BOTTOM_ROW_EYE,full);

    //clear the sides before adding the additions of the eye
    clear_eye_sides();
   
   //add additions to the eye based on the emotion/feeling
    switch(feeling){
        case Neutral:
        //neurtal is already the basic shape
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

//activate the bottom row and deactivate the top row of the eye
//to make it seem like a blink
void write_blink(){
    clear_cell(4,TOP_ROW_EYE);
    write_char(3,BOTTOM_ROW_EYE,full);
    write_char(5,BOTTOM_ROW_EYE,full);

    clear_cell(15,TOP_ROW_EYE);
    write_char(14,BOTTOM_ROW_EYE,full);
    write_char(16,BOTTOM_ROW_EYE,full);
}

//calls blink and after the for calls write eye
void timer_blink_callback( TimerHandle_t xTimer ){ 
	size_t timeSize = 10;
	for (int i = 0; i < timeSize; i++) {
        //checks if the face is able to blink
        if(can_blink){
        //close eye
		write_blink();
        }
	}
    //open the eyes again
    write_eye();
}

//clears lcd and makes the face unable to blink
void clear_face(){
    clear_LCD();
    can_blink = 0;
}

void initialize_face(){
    
    //set the costum chars to better readeble char names
    full = I2C_LCD1602_CHARACTER_BLOCK;
    top_half = I2C_LCD1602_CHARACTER_CUSTOM_1;
    bottom_half = I2C_LCD1602_CHARACTER_CUSTOM_2;

    //set basic feeling to neutral
    feeling = Neutral;

    int id = 1;
    //set a timer for 10 seconds to make it look like a blink
	timer_blink = xTimerCreate("Blink", pdMS_TO_TICKS(10000), pdTRUE, ( void * )id, &timer_blink_callback);
	if( xTimerStart(timer_blink, 10 ) != pdPASS ) {
		ESP_LOGE(TAG, "Cannot start 10 second timer");
    }
}
//sets the eyes and mouth and makes sure the face can blink
void set_face(){
    write_mouth();
    write_eye();
    can_blink = 1;
}

