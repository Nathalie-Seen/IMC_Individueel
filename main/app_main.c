#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "generic.h"
#include "lcd.h"


TimerHandle_t timer_blink;

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
		write_blink();
        write_eye();
	}
}

void app_main(void)
{
    setup_lcd();
    create_costum_char();
    write_mouth();
    write_eye();
    //write_string_position("test",0,0);
    int id = 1;
	timer_blink = xTimerCreate("Blink", pdMS_TO_TICKS(10000), pdTRUE, ( void * )id, &timer_blink_callback);
	if( xTimerStart(timer_blink, 10 ) != pdPASS ) {
		ESP_LOGE("main", "Cannot start 1 second timer");
    }
}