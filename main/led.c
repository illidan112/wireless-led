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
#define LED_STRIP_LEN 34
#define TAG     "LED"
#define COLORS_TOTAL (sizeof(colors) / sizeof(rgb_t))

xQueueHandle  xLightDataQueue = NULL;
xTaskHandle xLightMusicHandle = NULL;

static portBASE_TYPE xStatus;
const uint8_t dataLength = 2;
static uint8_t VUlength = 0;
static uint8_t VUpixel = 0;
static uint8_t VUcolorStep = 0;
void VUmetr(uint8_t l_level ,uint8_t r_level);

static const rgb_t colors[] = {
    { .r = 0x0f, .g = 0x0f, .b = 0x0f },
    { .r = 0x00, .g = 0x00, .b = 0x2f },
    { .r = 0x00, .g = 0x2f, .b = 0x00 },
    { .r = 0x2f, .g = 0x00, .b = 0x00 },
    { .r = 0x00, .g = 0x00, .b = 0x00 },
};

static rgb_t red = { .r = 0x2f, .g = 0x00, .b = 0x00 };
static rgb_t green = { .r = 0x00, .g = 0x2f, .b = 0x00 };
static rgb_t empty = { .r = 0x00, .g = 0x00, .b = 0x00 };


led_strip_t strip = {
    .type = LED_TYPE,
    .length = LED_STRIP_LEN,
    .gpio = LED_GPIO,
    .buf = NULL,
    .brightness = 255,
};


void xLightMusic(void *pvParameters)
{
    ESP_LOGI(TAG, "xLightMusic start");
    uint8_t ReceivedData[dataLength];
    const portTickType xTicksToWait = 1000 / portTICK_RATE_MS;

    if(LED_STRIP_LEN < sizeof(uint8_t)){
        VUlength = LED_STRIP_LEN / 2;
        VUpixel = 1;
        VUcolorStep = 120 / VUlength;


    }else{
        VUlength = LED_STRIP_LEN / 2;
        VUpixel = LED_STRIP_LEN / sizeof(uint8_t);
    }

    while (1){

        if( uxQueueMessagesWaiting( xLightDataQueue ) != 0 ){
        //Печать "Очередь должна была быть пустой!"
        ESP_LOGW(TAG, "Queue should have been empty!");
        }

        xStatus = xQueueReceive( xLightDataQueue, &ReceivedData, xTicksToWait );

        if( xStatus == pdPASS ){
            // ESP_LOGI(TAG,"DATA RECEIVED");
            // for (uint8_t i = 0; i < dataLength; i++){
            //     printf(" %d", ReceivedData[i]);
            // }
            // printf("\n");
            // changeColor();

            VUmetr(ReceivedData[0],ReceivedData[1]);

            //ESP_LOGW(TAG,"CoreID: %d", xPortGetCoreID());
        }else{
            ESP_LOGE(TAG, "Failed to receive from the queue, because waiting time is over");
            //closeLightMusicMode();
        }

    }
}

void lightDataQueue_close(){
    ESP_LOGI(TAG, "Delete LightDataQueue");
    vQueueDelete(xLightDataQueue);
    xLightDataQueue = NULL;
}

esp_err_t lightDataQueue_open(){
    if(xLightDataQueue == NULL){
        xLightDataQueue = xQueueCreate( 3, sizeof( uint8_t[dataLength] ) );
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

        xStatus = xTaskCreatePinnedToCore(xLightMusic, "LightMusic", configMINIMAL_STACK_SIZE * 5, NULL, 3, &xLightMusicHandle, 1);
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

void lightMusic_Resume(){

    vTaskResume(xLightMusicHandle);
    GetTaskState(xLightMusicHandle);
}

void lightMusic_Suspend(){

    vTaskSuspend(xLightMusicHandle);
    GetTaskState(xLightMusicHandle);
}

void strip_init(){
    ESP_LOGI(TAG,"WS2812 strip initialization");
    led_strip_install();
    ESP_ERROR_CHECK(led_strip_init(&strip));
}

static size_t c = 0;

void changeColor(){
    ESP_ERROR_CHECK(led_strip_fill(&strip, 0, strip.length, colors[c]));
    ESP_ERROR_CHECK(led_strip_flush(&strip));
    ESP_LOGI(TAG, "color number: %d", c);

    if (++c >= COLORS_TOTAL){
        c = 0;
    }
}

void VUmetr(uint8_t l_level ,uint8_t r_level){
    uint8_t maxLevel = l_level / VUlength;
    rgb_t rgb_color;

    ESP_ERROR_CHECK(led_strip_fill(&strip, 0, strip.length, empty));
    ESP_ERROR_CHECK(led_strip_flush(&strip));


    //level channel
    for(int i = 0; i < maxLevel; i++){

        hsv_t hsv_color = {
                .hue = 120 - (VUcolorStep * i+1),
                .sat = 255,
                .val = 255,
            };
        rgb_color = hsv2rgb_rainbow(hsv_color);
        ESP_ERROR_CHECK(led_strip_set_pixels(&strip, VUlength - i, 1, &rgb_color));
    }
    //right channel
    maxLevel = r_level / VUlength;
    for(int i = 1; i <= maxLevel; i++){
        hsv_t hsv_color = {
                .hue = 120 - (VUcolorStep * i),
                .sat = 255,
                .val = 255,
            };
        rgb_color = hsv2rgb_rainbow(hsv_color);
        ESP_ERROR_CHECK(led_strip_set_pixels(&strip, VUlength + i, 1, &rgb_color));
    }

    ESP_ERROR_CHECK(led_strip_flush(&strip));

}
