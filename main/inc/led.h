
/*
 *  Created on: 22 11 2021
 *      Author: illidan
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

xQueueHandle xLightDataQueue;
xTaskHandle xLightMusicHandle;

esp_err_t led_init();
