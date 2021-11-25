
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

static const char *TAG = "MAIN";

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

    //ESP_ERROR_CHECK(led_init());
    //ESP_ERROR_CHECK(udpClient_init());
}

// void GetTaskState(xTaskHandle taskHandle){
//     char taskName[64];
//     eTaskState state = eTaskGetState(taskHandle);
//     taskName =  pcTaskGetName(taskHandle);

//    switch (state)
//    {
//         case eRunning:
//             ESP_LOGI("Task RUNNING");
//             break;
//         case eReady:
//             ESP_LOGI("Task READY");
//             break;
//         case eBlocked:
//             ESP_LOGI("Task BLOCKED");
//             break;
//         case eSuspended:
//             ESP_LOGI("Task  SUSPENDED");
//             break;
//         case eDeleted:
//             ESP_LOGI("Task  DELETED");
//             break;
//         case eInvalid:
//             ESP_LOGI("Task  INVALID");
//             break;
//         default:
//             ESP_LOGI("Unknown status");
//             break;
//    }
// }
