
/*
 *  Created on: 23 11 2021
 *      Author: illidan
 */
#ifndef _UDP_H_
#define _UDP_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern xTaskHandle xUdpServerHandle;

esp_err_t udpServer_open(void);
esp_err_t udpServer_close(void);

void udpServer_Resume();
void udpServer_Suspend();

#endif
