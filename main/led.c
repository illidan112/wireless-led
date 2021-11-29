/*
 *  Created on: 22 11 2021
 *      Author: illidan
 */

#include "led.h"
#include "main.h"

#include <stdio.h>
#include <esp_log.h>
#include <led_strip.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
//#include "FreeRTOSConfig.h"


#define LED_TYPE LED_STRIP_WS2812
#define LED_GPIO 5
#define LED_STRIP_LEN 8
#define TAG     "LED"

xQueueHandle  xLightDataQueue = NULL;
xTaskHandle xLightMusicHandle = NULL;

//static const char *LED = "LED";
static portBASE_TYPE xStatus;

// static const rgb_t colors[] = {
//     { .r = 0x0f, .g = 0x0f, .b = 0x0f },
//     { .r = 0x00, .g = 0x00, .b = 0x2f },
//     { .r = 0x00, .g = 0x2f, .b = 0x00 },
//     { .r = 0x2f, .g = 0x00, .b = 0x00 },
//     { .r = 0x00, .g = 0x00, .b = 0x00 },
// };

// led_strip_t strip = {
//     .type = LED_TYPE,
//     .length = LED_STRIP_LEN,
//     .gpio = LED_GPIO,
//     .buf = NULL,
// };

//#define COLORS_TOTAL (sizeof(colors) / sizeof(rgb_t))

void xLightMusic(void *pvParameters)
{
    ESP_LOGI(TAG, "xLightMusic start");
    char ReceivedData[16];
    const portTickType xTicksToWait = 1000 / portTICK_RATE_MS;

    size_t c = 0;

    xTaskHandle xH = xTaskGetCurrentTaskHandle();

    if(xH == xLightMusicHandle){
        ESP_LOGW(TAG, "xLightMusicHandle true");
    }else{
        ESP_LOGW(TAG, "xLightMusicHandle false");
    }

    while (1){
        ESP_LOGI(TAG, "xLightMusic WHILE(1)");

    if( uxQueueMessagesWaiting( xLightDataQueue ) != 0 ){
       //Печать "Очередь должна была быть пустой!"
       ESP_LOGW(TAG, "Queue should have been empty!");
    }

       xStatus = xQueueReceive( xLightDataQueue, &ReceivedData, xTicksToWait );

    if( xStatus == pdPASS ){
       /* Данные успешно приняты из очереди, печать принятого значения. */
        ESP_LOGI(TAG,"RECEIVED DATA: %d", ReceivedData[6]);
        // ESP_ERROR_CHECK(led_strip_fill(&strip, 0, strip.length, colors[c]));
        // ESP_ERROR_CHECK(led_strip_flush(&strip));
        // if (++c >= COLORS_TOTAL)
        //     c = 0;
    }else{
        /* Данные не были приняты из очереди даже после ожидания 1000 мс.
            Вызов vTaskSuspend(); */
        ESP_LOGE(TAG, "Call closeLightMusicMode(), because could not receive from the queue.");
        //closeLightMusicMode();
    }

    }
}

void lightDataQueue_close(){
    ESP_LOGW(TAG, "Delete LightDataQueue");
    vQueueDelete(xLightDataQueue);
    xLightDataQueue = NULL;
}

esp_err_t lightDataQueue_open(){
    if(xLightDataQueue == NULL){
        xLightDataQueue = xQueueCreate( 3, sizeof( uint8_t[16] ) );
            if( xLightDataQueue == NULL ){
                ESP_LOGE(TAG, "Queue create error");
                return ESP_FAIL;
                }
    }else{
        ESP_LOGE(TAG, "Almost exist");
        return ESP_FAIL;
    }

    return ESP_OK;
}


void lightmusic_close(){

    ESP_LOGW(TAG, "Delete LightMusicTask");
    vTaskDelete(xLightMusicHandle);
    xLightMusicHandle = NULL;
}

esp_err_t lightmusic_open()
{
    if(xLightMusicHandle == NULL){

        xStatus = xTaskCreate(xLightMusic, "LightMusic", configMINIMAL_STACK_SIZE * 5, NULL, 3, &xLightMusicHandle);
        if( xStatus != pdPASS || xLightMusicHandle==NULL ){
            ESP_LOGE(TAG, "Task create error");
            return ESP_FAIL;
        }
        else{
                vTaskSuspend(xLightMusicHandle);
                GetTaskState(xLightMusicHandle);
            }

        return ESP_OK;
    } else{
        ESP_LOGE(TAG, "Almost exist");
        return ESP_FAIL;
    }

}

void lightmusic_START(){

    vTaskResume(xLightMusicHandle);
    GetTaskState(xLightMusicHandle);
}

void lightmusic_STOP(){

    vTaskSuspend(xLightMusicHandle);
    GetTaskState(xLightMusicHandle);
}
