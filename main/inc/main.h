/*
 *  Created on: 25 11 2021
 *      Author: illidan
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void GetTaskState(xTaskHandle taskHandle);
void closeLightMusicMode();
esp_err_t openLightMusicMode();

#endif
