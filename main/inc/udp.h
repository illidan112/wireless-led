
/*
 *  Created on: 23 11 2021
 *      Author: illidan
 */
#ifndef _UDP_H_
#define _UDP_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "esp_err.h"

extern xTaskHandle xUdpServerHandle;

esp_err_t udpClient_open(void);

#endif
