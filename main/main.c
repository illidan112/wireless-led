
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "sdkconfig.h"
#include <string.h>

#include "http_server.h"
#include "wi_fi.h"
#include "led.h"
#include "udp.h"

#define TAG "MAIN"

void app_main(void){

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (wifi_init_sta() == ESP_OK) {
        /* Start the server for the first time */
        ESP_LOGI(TAG, "Start http server");
        http_server_start();
    } else {
        ESP_LOGI(TAG, "Connection failed");
    }

}

void GetTaskState(xTaskHandle taskHandle){
    static const char* DEBUG = "DEBUG";
    char* taskName;
    taskName =  pcTaskGetName(taskHandle);
    eTaskState state = eTaskGetState(taskHandle);


   switch (state)
   {
        case eRunning:
            ESP_LOGI(DEBUG,"Task %s RUNNING", taskName);
            break;
        case eReady:
            ESP_LOGI(DEBUG,"Task %s READY", taskName);
            break;
        case eBlocked:
            ESP_LOGI(DEBUG,"Task %s BLOCKED",taskName);
            break;
        case eSuspended:
            ESP_LOGI(DEBUG,"Task %s SUSPENDED", taskName);
            break;
        case eDeleted:
            ESP_LOGI(DEBUG,"Task %s DELETED", taskName);
            break;
        case eInvalid:
            ESP_LOGI(DEBUG,"Task %s INVALID", taskName);
            break;
        default:
            ESP_LOGI(DEBUG,"Unknown status");
            break;
   }
}

void closeLightMusicMode(){
    ESP_LOGI(TAG, "Close LightMusic Mode");

    lightmusic_close();
    ESP_ERROR_CHECK_WITHOUT_ABORT(udpClient_close());
    lightDataQueue_close();
}

esp_err_t openLightMusicMode(){
    ESP_LOGI(TAG, "Open LightMusic Mode");
    if(lightDataQueue_open() != ESP_OK || udpClient_open() != ESP_OK || lightmusic_open() != ESP_OK){
        ESP_LOGE(TAG, "Open LightMusic Mode ERROR");
        return ESP_FAIL;
    }else {
        return ESP_OK;
    }

}