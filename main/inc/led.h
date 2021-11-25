
/*
 *  Created on: 22 11 2021
 *      Author: illidan
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#ifndef _LED_H_
#define _LED_H_

extern xQueueHandle xLightDataQueue;
extern xTaskHandle xLightMusicHandle;

esp_err_t lightmusic_start();

#endif