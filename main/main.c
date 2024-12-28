#include <stdio.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "ds_timer.h"
#include "ds_spiffs.h"

static const char *TAG = "ESP32"; 

void app_main(void) 
{
    ESP_LOGI( TAG, "version 04\n" );
    ESP_LOGI( TAG, "Start...\n" );
    
    ds_timer_init();
    init_spiffs();

    ds_spiffs_test();
    ds_spiffs_deinit();

    while (1) 
    {
        ESP_LOGI( TAG, "system running...\n" );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) ); 
    }
 
    ESP_LOGI( TAG, "End...\n" );
}
