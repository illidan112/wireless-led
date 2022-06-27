
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
#include "main.h"

#define TAG     "MAIN"
#define DEBUG   true

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

    Initialization();

    switcher_Resume();

}

void GetTaskState(xTaskHandle taskHandle){
    char* taskName;
    taskName =  pcTaskGetName(taskHandle);
    eTaskState state;
    if(DEBUG){
        state = eTaskGetState(taskHandle);
        switch (state)
        {
            case eRunning:
                ESP_LOGW(TAG,"Task %s RUNNING", taskName);
                break;
            case eReady:
                ESP_LOGW(TAG,"Task %s READY", taskName);
                break;
            case eBlocked:
                ESP_LOGW(TAG,"Task %s BLOCKED",taskName);
                break;
            case eSuspended:
                ESP_LOGW(TAG,"Task %s SUSPENDED", taskName);
                break;
            case eDeleted:
                ESP_LOGW(TAG,"Task %s DELETED", taskName);
                break;
            case eInvalid:
                ESP_LOGW(TAG,"Task %s INVALID", taskName);
                break;
            default:
                ESP_LOGW(TAG,"Unknown status");
                break;
        }
    }

}


void Initialization(){

    if (wifi_init_sta(NETWORK_CORE) != ESP_OK) {             //Connecting to wifi station
        ESP_LOGE(TAG, "WI-FI conneting failed");
        esp_restart();
    }

    if (tcpServer_create(NETWORK_CORE) != ESP_OK){         //Start TCP server for the first time
        ESP_LOGE(TAG, "TCP crearing failed");
        esp_restart();
    }

    if (udpServer_create(NETWORK_CORE) != ESP_OK){
        ESP_LOGE(TAG, "UDP crearing failed");
        esp_restart();
    }

    if (LED_init(LED_CORE) != ESP_OK){
        ESP_LOGE(TAG, "LED initialization failed");
        esp_restart();
    }
    ESP_LOGI(TAG, "Initialization was successful");
}

void switcher_Resume(){
    tcpServer_Resume();
}

void lightMusicMode_play(){
    lightMusic_Resume();
    udpServer_Resume();
}

void lightMusicMode_pause(){
    lightMusic_Suspend();
    udpServer_Suspend();
}

void backgroundLightMode_play(){
    backgroundLight_Resume();
}

void backgroundLightMode_pause(){
    backgroundLight_Suspend();
}
