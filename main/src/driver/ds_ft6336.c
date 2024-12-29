
#include <string.h>
#include <stdio.h>

#include "esp_types.h"
#include "esp_log.h"

#include "ds_ft6336.h"
#include "ds_iic.h"
#include "ds_gpio.h"
// #include "ds_screen.h"
#include "ds_system_data.h"

static const char *TAG = "FT6336";

const uint16_t FT6236_TPX_TBL[5] =
    {
        FT_TP1_REG,
        FT_TP2_REG,
        FT_TP3_REG,
        FT_TP4_REG,
        FT_TP5_REG
    };

TouchPoint_T gTPS;

static void scan_ft6336(void)
{
    uint8_t i = 0;
    uint8_t sta = 0;
    uint8_t buf[4] = {0};
    uint8_t gestid = 0;
    i2c_master_read_slave(FT_REG_NUM_FINGER, &sta, 1);
    gTPS.touch_count = sta;
    i2c_master_read_slave(0x01, &gestid, 1);
    if (sta & 0x0f)
    {
        gTPS.touch_sta = ~(0xFF << (sta & 0x0F));
        for (i = 0; i < 2; i++)
        {
            if (gTPS.touch_sta & (1 << i))
            {
                i2c_master_read_slave(FT6236_TPX_TBL[i], buf, 4);
                gTPS.x[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                gTPS.y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
            }
        }
        gTPS.touch_sta |= TP_PRES_DOWN;
    }
    else
    {
        if (gTPS.touch_sta & TP_PRES_DOWN)
        {
            gTPS.touch_sta &= ~0x80;
        }
        else
        {
            gTPS.x[0] = 0;
            gTPS.y[0] = 0;
            gTPS.touch_sta &= 0xe0;
        }
    }
}

static void count_position_ft6336(TP_POSITION_T *position)
{
    switch (gTPS.touch_count)
    {
    case 1:
        ESP_LOGI(TAG, "x=%d y=%d\n", gTPS.x[0], gTPS.y[0]);
        if ((gTPS.x[0] != 0) && (gTPS.y[0] != 0) && (gTPS.x[0] < 200) && (gTPS.y[0] < 200)) // 软件滤掉无效操作
        {
            gTPS.x[0] = gTPS.x[0] * 152 / 200;
            gTPS.y[0] = gTPS.y[0] * 152 / 200;
            position->status = 1;
            position->x = gTPS.x[0];
            position->y = gTPS.y[0];
            return;
        }
        break;
    case 2:
        if ((gTPS.x[0] != 0) && (gTPS.y[0] != 0) && (gTPS.x[1] != 0) && (gTPS.y[1] != 0) && (gTPS.x[0] < 200) && (gTPS.y[0] < 200) && (gTPS.x[1] < 200) && (gTPS.y[1] < 200)) // 软件滤掉无效操作
        {
            // To 152x152
            gTPS.x[0] = gTPS.x[0] * 152 / 200;
            gTPS.y[0] = gTPS.y[0] * 152 / 200;
            gTPS.x[1] = gTPS.x[1] * 152 / 200;
            gTPS.y[1] = gTPS.y[1] * 152 / 200;
        }
        break;
    default:
        break;
    }
    for (int i = 0; i < 2; i++)
    {
        gTPS.x[i] = 0;
        gTPS.y[i] = 0;
    }
    position->status = 0;
    position->x = gTPS.x[0];
    position->y = gTPS.y[0];
}

void get_ft6336_touch_sta(TP_POSITION_T *position)
{
    scan_ft6336();
    count_position_ft6336(position);
}

void init_ft6336(void)
{
    uint8_t w_data, r_data = 0;
    memset(&gTPS, 0, sizeof(TouchPoint_T));

    ds_touch_gpio_init();

    ds_gpio_set_touch_rst(GPIO_RST_LOW);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    ds_gpio_set_touch_rst(GPIO_RST_HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    i2c_master_init();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    w_data = 0; // set the operation mode to normal
    i2c_master_write_slave(FT_DEVIDE_MODE, &w_data, 1);
    w_data = 22; // set touch valid value 22 the smaller the more sensitive
    i2c_master_write_slave(FT_ID_G_THGROUP, &w_data, 1);
    i2c_master_read_slave(FT_ID_G_THGROUP, &r_data, 1);
    ESP_LOGI(TAG, "init THGROUP = %d \n", r_data);
    w_data = 14;
    i2c_master_write_slave(FT_ID_G_PERIODACTIVE, &w_data, 1);
    i2c_master_read_slave(FT_ID_G_PERIODACTIVE, &r_data, 1);
    ESP_LOGI(TAG, "init PERIODACTIVE = %d \n", r_data);
    w_data = 0; // interrupt generation mode sustained level
    i2c_master_write_slave(FT_ID_G_MODE, &w_data, 1);
    i2c_master_read_slave(FT_ID_G_MODE, &r_data, 1);
    ESP_LOGI(TAG, "init G_MODE = %d \n", r_data);
}