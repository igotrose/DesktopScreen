#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "esp_log.h"

#include "ds_timer.h"
// #include "ds_system_data.h"
// #include "ds_ft6336.h"
// #include "ds_ui_timepage.h"
// #include "ds_ui_tomatopage.h"

#define TIMER_DIVIER 16
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIER / 1000)
#define TIMER_INTERVAL0_SEC (10)
#define TEST_WITH_RELOAD 1

typedef struct
{
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
    uint64_t timer_second_count_isr;

} timer_event_t;

timer_event_t g_timer_event;

QueueHandle_t timer_queue;
// QueueHandle_t  ui_update_timer_queue;

static const char *TAG = "TIMER & QUEUE"; 


void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = ( int ) para;

    timer_event_t event;
    timer_group_clr_intr_status_in_isr( TIMER_GROUP_0, TIMER_0 );  

    timer_group_enable_alarm_in_isr( TIMER_GROUP_0, timer_idx );
    xQueueSendFromISR( timer_queue, &event, NULL );
    // xQueueSendFromISR(ui_update_timer_queue, &event, NULL);
}

static void tg0_timer_init( int timer_idx, bool auto_reload, double timer_interval_sec )
{
    timer_config_t config = {
        .divider = TIMER_DIVIER, 
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    };
    
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    timer_set_counter_value( TIMER_GROUP_0, timer_idx, 0x00000000ULL );
    timer_set_alarm_value( TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE );
    timer_enable_intr( TIMER_GROUP_0, timer_idx );
    timer_isr_register( TIMER_GROUP_0, timer_idx, timer_group0_isr, 
                        ( void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL );

    timer_start( TIMER_GROUP_0, timer_idx );
}

static void timer_event_task( void * pvParameters )
{
    while (1)
    {
        timer_event_t event;
        xQueueReceive(timer_queue, &event, portMAX_DELAY);
        g_timer_event.timer_minute_count ++;
        if (g_timer_event.timer_minute_count >= 6000)
        {
            g_timer_event.timer_minute_count = 0;
        }
        g_timer_event.timer_second_count++;
        if (g_timer_event.timer_second_count >= 100)
        {
            g_timer_event.timer_second_count = 0;
            ESP_LOGI(TAG, "timer running\n");
        }
    }
    
}

// static void ui_timer_update_task( void * pvParameters )
// {
    
// }

void ds_timer_init(void)
{
    g_timer_event.timer_minute_count = 0;
    g_timer_event.timer_second_count = 0;
    g_timer_event.timer_second_count_isr = 0;

    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    // ui_update_timer_queue = xQueueCreate( 10, sizeof( timer_event_t ) );

    tg0_timer_init(TIMER_0, TEST_WITH_RELOAD, TIMER_INTERVAL0_SEC);

    xTaskCreate(timer_event_task, "timer_event_task", 2048, NULL, 5, NULL);
    // xTaskCreate(ui_timer_update_task, "ui_timer_update_task", 2048, NULL, 5, NULL);
}
