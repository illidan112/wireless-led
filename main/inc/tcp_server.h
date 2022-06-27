/*
 *  Created on: 19 06 2022
 *      Author: illidan
 */

#ifndef _TCP_H_
#define _TCP_H_

#include "main.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern xTaskHandle xTCPServerHandle;

/* Function responsible for configuring and starting the tcp_server service.
 * See tcp_server.c for implementation */
esp_err_t tcpServer_create(core_ID id);

void tcpServer_Resume();

#endif