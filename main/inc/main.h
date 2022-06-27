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

void Initialization();
void switcher_Resume();
void lightMusicMode_play();
void lightMusicMode_pause();
void backgroundLightMode_play(void);
void backgroundLightMode_pause(void);
void GetTaskState(xTaskHandle taskHandle);

#endif
