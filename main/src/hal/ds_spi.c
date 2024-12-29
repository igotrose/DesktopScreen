#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "ds_gpio.h"

#define DMA_CHAN 2

#define PIN_NUM_MISO 23
#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK 25
#define PIN_NUM_CS 27

#define PARALLEL_LINES 16

spi_device_handle_t spi;

void spi_send_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    ds_gpio_set_screen_dc(0);
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void *)0;
    ret = spi_device_polling_transmit(spi, &t);
    ds_gpio_set_screen_cs(1);
    assert(ret == ESP_OK);
}

void spi_send_data(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    ds_gpio_set_screen_dc(1);
    ds_gpio_set_screen_cs(0);
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &data;
    t.user = (void *)1;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

void spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    printf("dc callback\n");
    ds_gpio_set_screen_dc(dc);
}

void screen_spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64 * 8,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 15 * 1000 * 1000,
        .mode = 0,
        .queue_size = 7,
    };
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

void screen_spi_test(void)
{
    spi_send_cmd(0x55);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    spi_send_data(0x55);
}