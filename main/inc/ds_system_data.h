#ifndef __DS_SYSTEM_DATA_H_
#define __DS_SYSTEM_DATA_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SETTING_SSID_LEN 32
#define MAX_SETTING_PSW_LEN 64
#define MAX_SETTING_CITY_LEN 15

typedef enum
{
    TP_ACTION_NULL = 0,
    TP_ACTION_SHORT,     // short press
    TP_ACTION_LONG,      // long press
    TP_ACTION_MOVE_LEFT, // move
    TP_ACTION_MOVE_RIGHT,
    TP_ACTION_MOVE_UP,
    TP_ACTION_MOVE_DOWN,
} TP_ACTION_E;

typedef enum
{
    SETTING_NULL = 0,
    SETTING_ING,
    SETTING_WAIT,
    SETTING_FINISH,
} SETTING_STATUS_E;

typedef enum
{
    WIFI_AP_MODE_INIT = 0,
    WIFI_AP_MODE_CONNECT,
    WIFI_AP_MODE_DISCONNECT,
} WIFI_AP_MODE_STATUS_E; // AP mode

typedef enum
{
    WIFI_STA_MODE_INIT = 0,
    WIFI_STA_MODE_CONNECT_SUCCESS,
    WIFI_STA_MODE_CONNECT_FAIL,
    WIFI_STA_MODE_CONNECT_TIMEOUT,
} WIFI_STA_MODE_STATUS_E; // STA mode

typedef struct
{
    int tp_interval_timecount;
    uint8_t status;
    uint16_t tp_start_x;
    uint16_t tp_start_y;
    uint16_t tp_stop_x;
    uint16_t tp_stop_y;
    TP_ACTION_E tp_action;
} TP_ACTION_MANAGE_T; // Touch Action Mananger

typedef struct
{
    // wifi configration
    char setting_ssid[MAX_SETTING_SSID_LEN];
    uint8_t setting_ssid_len;
    char setting_psw[MAX_SETTING_PSW_LEN];
    uint8_t setting_psw_len;
    bool has_wifi_info;

    // network status
    WIFI_AP_MODE_STATUS_E wifi_ap_status;
    WIFI_STA_MODE_STATUS_E wifi_sta_status;
    bool is_ap_sta_open;

    // http request
    bool first_time_httpdata_request;
    bool update_httpdata_request;

    // city
    char setting_city[MAX_SETTING_CITY_LEN];
    uint8_t setting_city_len;

    // tomato clock time
    uint8_t tomato_work_time;
    uint8_t tomato_rest_time;
    uint8_t tomato_time_count;
    uint8_t tomato_left_time;

    // touchpad manage
    TP_ACTION_MANAGE_T tp_action_manage;
    uint32_t tp_wackup_timeleft;

    // system time
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} SYSTRM_DATA_T;

/* SYSTEM DATA */
void ds_system_data_init(void);
SYSTRM_DATA_T get_system_data(void);

/* WIFI INFO MANAGE */
void set_system_data_wifi_info(char *p_ssid, char *p_psw);
void print_system_data_wifi_info(void);
bool has_wifi_info(void);

/* NETWORK STATUS MANAGE */
WIFI_STA_MODE_STATUS_E get_wifi_sta_status();
void set_wifi_sta_status(WIFI_STA_MODE_STATUS_E status);
WIFI_AP_MODE_STATUS_E get_wifi_ap_status();
void set_wifi_ap_status(WIFI_AP_MODE_STATUS_E status);
bool get_is_ap_sta_open();
void set_is_ap_sta_open(bool isopen);

/* HTTP REQUEST */
bool has_first_time_httpdata_request();
void set_has_first_time_httpdata_request(void);
bool has_update_httpdata_request();
void set_update_httpdata_request(bool update);

/* SYSTEM SETTING */
void set_system_data_city_info(char *p_city);
void set_system_data_tomato_info(char *work_time, char *rest_time, char *count_time);

/* TP ACTION MANAGE */
void clear_tp_action(void);
uint8_t get_tp_action_status(void);
TP_ACTION_E get_tp_action(void);
void check_tp_action(void);
void set_tp_action_manage_stop_point(uint16_t x, uint16_t y);
void set_tp_action_manage_start_point(uint16_t x, uint16_t y);
void count_tp_action_manage_time(void);
void reset_tp_action_manage(void);

void set_tp_wackup_timeleft(uint32_t timeleft);
void count_tp_wackup_timeleft();
uint32_t get_tp_wackup_timeleft();

/* SYSTEM TIME */
void update_system_time(uint8_t hour, uint8_t minute, uint8_t second);
void update_system_time_second(void);
void update_system_time_minute(void);

#endif

