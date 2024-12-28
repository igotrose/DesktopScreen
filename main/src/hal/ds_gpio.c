#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "ds_gpio.h"
// #include "ds_ft6336.h"
#include "ds_system_data.h"

// T_RST
#define GPIO_OUTPUT_IO_0 5
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_IO_0))
// T_INT
#define GPIO_INPUT_IO_0 4
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0))
// IO0
#define ESP_INTR_FLAG_DEFAULT 0

// CS
#define SCREEN_GPIO_OUTPUT_CS 27
#define SCREEN_GPIO_OUTPUT_CS_SEL ((1ULL << SCREEN_GPIO_OUTPUT_CS))
// DC
#define SCREEN_GPIO_OUTPUT_DC 14
#define SCREEN_GPIO_OUTPUT_DC_SEL ((1ULL << SCREEN_GPIO_OUTPUT_DC))
// RES
#define SCREEN_GPIO_OUTPUT_RES 12
#define SCREEN_GPIO_OUTPUT_RES_SEL ((1ULL << SCREEN_GPIO_OUTPUT_RES))
// BUSY
#define SCREEN_GPIO_INPUT_BUSY 13
#define SCREEN_GPIO_INTPUT_BUSY_SEL ((1ULL << SCREEN_GPIO_INPUT_BUSY))

static QueueHandle_t gpio_event_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    while (1)
    {
        if (xQueueReceive(gpio_event_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
            // set_tp_wackup_timeleft(60);
            // if (gpio_get_level(io_num) == 0)
            // {
            //     // TODO START COUNT
            //     reset_tp_action_manage();
            // }
            // else
            // {
            //     // TODO STOP COUNT
            //     check_tp_action();
            // }
        }
    }
}

void ds_touch_gpio_init(void)
{
    static bool has_init_isr = false;
    gpio_config_t io_conf;

    // T_RST
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    // T_INT
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    if (has_init_isr == false)
    {
        gpio_event_queue = xQueueCreate(10, sizeof(uint32_t));
        xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0);
    }
    has_init_isr = true;
}

void ds_touch_gpio_isr_remove(void)
{
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
}

void ds_touch_gpio_isr_add(void)
{
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0);
}

void ds_screen_gpio_init(void)
{
    gpio_config_t io_conf;
    // CS
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_CS_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    // DC
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_DC_SEL;
    gpio_config(&io_conf);
    // RES
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_RES_SEL;
    gpio_config(&io_conf);
    // BUSY
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = SCREEN_GPIO_INTPUT_BUSY_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}

void ds_gpio_init(void)
{
    ds_touch_gpio_init();
}

void ds_gpio_set_screen_cs(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_CS, level);
}

void ds_gpio_set_screen_dc(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_DC, level);
}
void ds_gpio_set_screen_rst(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_RES, level);
}

int ds_gpio_get_screen_busy(void)
{
    return gpio_get_level(SCREEN_GPIO_INPUT_BUSY);
}

void ds_gpio_set_touch_rst(uint32_t level)
{
    gpio_set_level(GPIO_OUTPUT_IO_0, level);
}
