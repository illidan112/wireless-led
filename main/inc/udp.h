
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

esp_err_t udpClient_open(void);
esp_err_t udpClient_close(void);

void udpClient_START();
void udpClient_STOP();

#endif
