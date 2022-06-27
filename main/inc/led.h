
/*
 *  Created on: 22 11 2021
 *      Author: illidan
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "main.h"

#ifndef _LED_H_
#define _LED_H_

extern xQueueHandle xLightDataQueue;
extern xTaskHandle xLightMusicHandle;
extern xTaskHandle xBackgroundLightHandle;
extern const uint8_t dataLength;

//esp_err_t lightMusicMode_create();
//esp_err_t lightDataQueue_create();
//void lightMusicMode_delete();
//void lightDataQueue_delete();
esp_err_t LED_init(core_ID);
// void xRainbowLoop(void *pvParameters);
// void xRainbowFade(void *pvParameters);

// void changeColor();

void lightMusic_Resume();
void lightMusic_Suspend();
void backgroundLight_Resume();
void backgroundLight_Suspend();

#endif