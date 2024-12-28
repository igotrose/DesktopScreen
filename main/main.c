#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "LogDemo"; 

uint32_t custom_timestamp(void) 
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

int custom_log_handler(const char *fmt, va_list args) 
{
    return vprintf(fmt, args);
}

void app_main(void) {
    esp_log_set_vprintf(custom_log_handler);

    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGE(TAG, "this is error log");
    ESP_LOGW(TAG, "this is warning log");
    ESP_LOGI(TAG, "this is info log");
    ESP_LOGD(TAG, "this is debug log");
    ESP_LOGV(TAG, "this is verbose log");

    for (int i = 0; i < 5; i++) 
    {
        ESP_LOGI(TAG, "count %d", i);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }

    ESP_LOGI(TAG, "Log Demo End");
}
