/*
 *  Created on: 16 11 2021
 *      Author: illidan
 */
#ifndef _WIFI_H_
#define _WIFI_H_

#include "esp_err.h"
#include "main.h"

/* Function responsible for configuring and starting the wi-fi module.
 * See wi_fi.c for implementation */
esp_err_t wifi_init_sta(core_ID id);

#endif
