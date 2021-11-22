/*
 *  Created on: 22 11 2021
 *      Author: illidan
 */

#include "led.h"

#include <stdio.h>
#include <esp_log.h>
#include <led_strip.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>


#define LED_TYPE LED_STRIP_WS2812
#define LED_GPIO 5
#define LED_STRIP_LEN 8

static const char *LED = "LED";
static portBASE_TYPE xStatus;

static const rgb_t colors[] = {
    { .r = 0x0f, .g = 0x0f, .b = 0x0f },
    { .r = 0x00, .g = 0x00, .b = 0x2f },
    { .r = 0x00, .g = 0x2f, .b = 0x00 },
    { .r = 0x2f, .g = 0x00, .b = 0x00 },
    { .r = 0x00, .g = 0x00, .b = 0x00 },
};

led_strip_t strip = {
    .type = LED_TYPE,
    .length = LED_STRIP_LEN,
    .gpio = LED_GPIO,
    .buf = NULL,
};

#define COLORS_TOTAL (sizeof(colors) / sizeof(rgb_t))

void xLightMusic(void *pvParameters)
{

    char ReceivedData[4];
    const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

    size_t c = 0;
    while (1){

    if( uxQueueMessagesWaiting( xLightDataQueue ) != 0 ){
       //Печать "Очередь должна была быть пустой!"
       ESP_LOGW(LED, "Queue should have been empty!");
    }

       xStatus = xQueueReceive( xLightDataQueue, &ReceivedData, xTicksToWait );

    if( xStatus == pdPASS ){
       /* Данные успешно приняты из очереди, печать принятого значения. */
       ESP_LOGI(LED,"RECEIVED DATA: %d", ReceivedData[0]);
       ESP_ERROR_CHECK(led_strip_fill(&strip, 0, strip.length, colors[c]));
       ESP_ERROR_CHECK(led_strip_flush(&strip));
    }else{
        /* Данные не были приняты из очереди даже после ожидания 100 мс.
            Вызов vTaskSuspend(); */
        ESP_LOGE(LED, "Call vTaskSuspend(NULL), because could not receive from the queue.");
        vTaskSuspend(NULL);
    }

        if (++c >= COLORS_TOTAL)
            c = 0;
    }
}


esp_err_t led_init()
{

    led_strip_install();
    ESP_ERROR_CHECK(led_strip_init(&strip));


    xTaskCreate(xLightMusic, "LightMusic", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    xLightDataQueue = xQueueCreate( 5, sizeof( char ) * 4 );
    if( xLightDataQueue == NULL ){
        ESP_LOGE(LED, "Queue create error");
        return ESP_FAIL;
        }

    xStatus = xTaskCreate(xLightMusic, "LightMusic", configMINIMAL_STACK_SIZE * 5, NULL, 2, &xLightMusicHandle);
    if( xStatus != pdPASS ){
        ESP_LOGE(LED, "Task create error");
        return ESP_FAIL;
    }else{
            ESP_LOGI(LED, "xLightMusicHandle in SUSPEND");
            vTaskSuspend(xLightMusicHandle);
        }

    // ESP_LOGI(LED, "Start Scheduler");
    // vTaskStartScheduler();

    return ESP_OK;

}
