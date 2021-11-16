

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

static const char *TAG = "MAIN";

/* Function responsible for configuring and starting the http_webserver service.
 * See http_server.c for implementation */
extern void http_server_start(void);

/* Function responsible for configuring and starting the wi-fi module.
 * See wi-fi.c for implementation */
extern esp_err_t wifi_init_sta(void);


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
        ESP_LOGI(TAG, "Connection failed, not starting esp_local_ctrl service");
    }



}
