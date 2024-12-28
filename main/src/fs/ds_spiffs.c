#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_err.h"

#include "ds_spiffs.h"

static const char *TAG = "Spiffs";

esp_vfs_spiffs_conf_t conf = {
.base_path = "/spiffs", 
.partition_label = NULL,
.max_files = 5, 
.format_if_mount_failed = true,
};

esp_err_t init_spiffs( void )
{
    ESP_LOGI( TAG, "Initializing SPIFFS" );

    esp_err_t ret = esp_vfs_spiffs_register( &conf );
    if ( ret == ESP_OK )
    {
        ESP_LOGI( TAG, "Successed to mount and format filesystem" );
    }
    else if ( ret == ESP_ERR_NOT_FOUND )
    {
        ESP_LOGE( TAG, "Failed to find SPIFFS partition" );
    }
    else
    {
        ESP_LOGE( TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name( ret ) );
    }
    return ESP_FAIL;    

    size_t total = 0, used = 0;
    ret = esp_spiffs_info( NULL, &total, &used );
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name( ret ) );
    }
    ESP_LOGI( TAG, "Partition size: total: %d, used: %d", total, used );
    
    return ESP_OK;
}

void ds_spiffs_deinit( void )
{
    esp_vfs_spiffs_unregister( conf.partition_label );
    ESP_LOGI(TAG, "SPIFFS unmounted");
}

void ds_spiffs_test( void )
{
    ESP_LOGI( TAG, "Opening file" );
    FILE *f = fopen( "/spiffs/hello.txt", "w" );
    if ( f == NULL )
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf( f, "Hello World!\n" );
    fclose( f );
    ESP_LOGI( TAG, "File written" );

    struct stat st;
    if ( stat( "/spiffs/foo.txt", &st ) == 0 )
    {
        unlink( "/spiffs/foo.txt" );
    }
    ESP_LOGI(TAG, "Renaming file");
    if ( rename( "/spiffs/hello.txt", "/spiffs/foo.txt" ) != 0 )
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
    
    ESP_LOGI( TAG, "Reading file" );
    f = fopen( "/spiffs/foo.txt", "r" );
    if ( f == NULL ) 
    {
        ESP_LOGE( TAG, "Failed to open file for reading" );
        return;
    }
    char line[ 64 ];
    fgets( line, sizeof( line ), f );
    fclose( f );

    char *pos = strchr( line, '\n' );
    if ( pos )
    {
        *pos = '\0';
    }
    ESP_LOGI( TAG, "Read from file: '%s'", line );
}