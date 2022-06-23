/*
 *  Created on: 25 11 2021
 *      Author: illidan
 */

#ifndef _MAIN_H_
#define _MAIN_H_

typedef enum core_ID {
    LED_CORE = 0,
    NETWORK_CORE = 1,
} core_ID;


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void switcher_Resume();
void Initialization();
void GetTaskState(xTaskHandle taskHandle);
// void closeLightMusicMode();
// esp_err_t openLightMusicMode();

#endif
