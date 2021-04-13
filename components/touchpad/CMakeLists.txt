idf_component_register(SRCS "touchpad.c"
                       INCLUDE_DIRS "include"
                       REQUIRES lcd_face lcd esp_peripherals input_key_service)