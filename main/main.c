
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

#include "tcp_server.h"
#include "wi_fi.h"
#include "led.h"
#include "udp.h"

#define TAG "MAIN"
#define DEBUG false

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


    if (!(wifi_init_sta() == ESP_OK)) {             //Connecting to wifi station
        ESP_LOGE(TAG, "WI-FI conneting failed");
    } else {
        if (!(tcpServer_open() == ESP_OK)){         //Start the TCP server for the first time
            ESP_LOGE(TAG, "TCP crearing failed");
        } else {
            //strip_init();
        }
    }
}

void GetTaskState(xTaskHandle taskHandle){
#ifdef DEBUG
    char* taskName;
    taskName =  pcTaskGetName(taskHandle);
    eTaskState state = eTaskGetState(taskHandle);

   switch (state)
   {
        case eRunning:
            ESP_LOGD(TAG,"Task %s RUNNING", taskName);
            break;
        case eReady:
            ESP_LOGD(TAG,"Task %s READY", taskName);
            break;
        case eBlocked:
            ESP_LOGD(TAG,"Task %s BLOCKED",taskName);
            break;
        case eSuspended:
            ESP_LOGD(TAG,"Task %s SUSPENDED", taskName);
            break;
        case eDeleted:
            ESP_LOGD(TAG,"Task %s DELETED", taskName);
            break;
        case eInvalid:
            ESP_LOGD(TAG,"Task %s INVALID", taskName);
            break;
        default:
            ESP_LOGD(TAG,"Unknown status");
            break;
   }
#endif
}

void closeLightMusicMode(){

    ESP_LOGI(TAG, "Close LightMusic Mode");
    if(xLightMusicHandle!= NULL){
        lightmusic_close();
    }
    if(xUdpServerHandle != NULL){
        ESP_ERROR_CHECK_WITHOUT_ABORT(udpServer_close());
    }
    if(xLightDataQueue != NULL){
        lightDataQueue_close();
    }
}

esp_err_t openLightMusicMode(){
    ESP_LOGI(TAG, "Opening LightMusic Mode");
    // if(lightDataQueue_open() != ESP_OK || udpServer_open() != ESP_OK || lightmusic_open() != ESP_OK){
    //     ESP_LOGE(TAG, "Open LightMusic Mode ERROR");
    //     return ESP_FAIL;
    // }else {
    //     return ESP_OK;
    // }

    if(lightDataQueue_open() == ESP_OK){
        if(udpServer_open() == ESP_OK){
            if(lightmusic_open() == ESP_OK){
                return ESP_OK;
            }else{
                udpServer_close();
                lightDataQueue_close();
                return ESP_FAIL;
            }
        }else{
            lightDataQueue_close();
            return ESP_FAIL;
        }
    }else {
        return ESP_FAIL;
    }

}