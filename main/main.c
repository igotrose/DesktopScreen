#include <stdio.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "ds_timer.h"
#include "ds_spiffs.h"
#include "ds_nvs.h"
#include "ds_gpio.h"
#include "ds_ft6336.h"

static const char *TAG = "ESP32"; 

void app_main(void) 
{
    ESP_LOGI( TAG, "version 06\n" );
    ESP_LOGI( TAG, "Start...\n" );
    
    ds_timer_init();

    init_spiffs();
    ds_spiffs_test();

    ds_nvs_init();

    ds_nvs_save_wifi_info("igotu", "aaggdd@147963");
    if (ds_nvs_read_wifi_info() == NVS_WIFO_INFO_HAS_SAVE)
    {
        printf("read wifi info succeed\r\n");
    }

    ds_gpio_init();
    init_ft6336();
    TP_POSITION_T position;
    
    while (1) 
    {
        get_ft6336_touch_sta(&position);
        ESP_LOGI(TAG, "TP X: %d, Y: %d\n", position.x, position.y);
        ESP_LOGI( TAG, "system running...\n" );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) ); 
    }
 
    ESP_LOGI( TAG, "End...\n" );
}
