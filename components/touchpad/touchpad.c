/**
 * this project is based of the initialization and activation of the touchpad.
 * i didnt make most of the method initialize_touchpad, i wrote the code after  the line of initialize_face();
 * the method input_key_service_cb also isnt selfmade, but the timer and the actions in the switch case are.
 * */

#include <string.h>
#include <sys/time.h>
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "input_key_service.h"
#include "lcd_face.h"
#include "generic.h"

#define TAG "touchpad"

TimerHandle_t timer_neutral;

//set emotion to neutral and set the face accordingly
void timer_neutral_callback( TimerHandle_t xTimer ){ 
	size_t timeSize = 10;
	for (int i = 0; i < timeSize; i++) {
       set_emotion(Neutral);
	}
    set_face();
}

static esp_err_t input_key_service_cb(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
    audio_board_handle_t board_handle = (audio_board_handle_t) ctx;
    int player_volume;
    audio_hal_get_volume(board_handle->audio_hal, &player_volume);
    if (evt->type == INPUT_KEY_SERVICE_ACTION_CLICK_RELEASE) {

        //resets timer when a button is pressed
        if(xTimerReset(timer_neutral, 10) != pdPASS){
            ESP_LOGE(TAG, "Cannot reset 25 second timer");
        }

        switch ((int)evt->data) {
            case INPUT_KEY_USER_ID_PLAY:
			{
                set_emotion(Happy);
                break;
			}
            case INPUT_KEY_USER_ID_SET:
			{
                set_emotion(Angry);
                break;
			}
            case INPUT_KEY_USER_ID_VOLUP:
			{
                set_emotion(Sad);
                break;
			}
            case INPUT_KEY_USER_ID_VOLDOWN:
			{
                //this touchpad doesnt work
                break;
			}
        }
    }
    //always set the face
    set_face();
    return ESP_OK;
}

void initialize_touchpad(){
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    audio_board_key_init(set);
    audio_board_sdcard_init(set,SD_MODE_1_LINE);

    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    input_key_service_info_t input_key_info[] = INPUT_KEY_DEFAULT_INFO();
    input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
    input_cfg.handle = set;
    periph_service_handle_t input_ser = input_key_service_create(&input_cfg);
    input_key_service_add_key(input_ser, input_key_info, INPUT_KEY_NUM);
    periph_service_set_callback(input_ser, input_key_service_cb, (void *)board_handle);

    initialize_face();
    
    int id = 0;
    //set timer for 25 seconds and call timer_neutral_caalback
    timer_neutral = xTimerCreate("Neutral", pdMS_TO_TICKS(25000), pdTRUE, ( void * )id, &timer_neutral_callback);
    if( xTimerStart(timer_neutral, 10 ) != pdPASS ) {
		ESP_LOGE(TAG, "Cannot start 25 second timer");
    }

}