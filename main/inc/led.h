
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
extern const uint8_t dataLength;

esp_err_t lightmusic_open();
esp_err_t lightDataQueue_open();
void lightmusic_close();
void lightDataQueue_close();

void strip_init();
void changeColor();

void lightMusic_Resume();
void lightMusic_Suspend();

#endif