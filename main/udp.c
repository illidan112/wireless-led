/*
 *  Created on: 23 11 2021
 *      Author: illidan
 */

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
//#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "udp.h"
#include "main.h"
#include "led.h"

#define PORT CONFIG_UDP_PORT

xTaskHandle xUdpServerHandle = NULL;

static const char *UDP = "UDP";
static int Socket = 0;

static void udpServerTask(void *pvParameters)
{
    uint8_t rx_buffer[dataLength + 1];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {

        if (addr_family == AF_INET) {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(PORT);
            ip_protocol = IPPROTO_IP;
        } else{
            ESP_LOGE(UDP, "Unable to create socket");
            break;
        }

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(UDP, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(UDP, "Socket №%d created", sock);

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(UDP, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(UDP, "Socket bound, port %d", PORT);
        Socket = sock;

        portBASE_TYPE xStatus;
        while (1) {
            //ESP_LOGW(UDP,"CoreID: %d", xPortGetCoreID());
            //ESP_LOGI(UDP, "Waiting for data");
            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(UDP, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                //ESP_LOGI(UDP, "Received %d bytes from %s:", len, addr_str);
                //ESP_LOGI(UDP, "%s", rx_buffer);

                xStatus = xQueueSendToBack( xLightDataQueue, rx_buffer, 0 );
                if( xStatus != pdPASS ){
                    ESP_LOGW(UDP, "Could not send to the queue." );
                    }

                // int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                // if (err < 0) {
                //     ESP_LOGE(UDP, "Error occurred during sending: errno %d", errno);
                //     break;
                // }
            }
        }

        if (sock != -1) {
            ESP_LOGE(UDP, "Shutting down socket and restarting...");
            if( shutdown(sock, 0)!= 0){
                ESP_LOGE(UDP, "Shutting down socket ERROR");
            }
            if( close(sock)!= 0){
                ESP_LOGE(UDP, "Closing socket ERROR");
            }
        }
    }
    vTaskDelete(NULL);
}

esp_err_t udpServer_close(void){

    ESP_LOGI(UDP, "Shutting down socket");

    if( shutdown(Socket, 0)!= 0){
        ESP_LOGE(UDP, "Shutting down socket ERROR");
        //return ESP_FAIL;
        }
    if( close(Socket)!= 0){
        ESP_LOGE(UDP, "Closing socket ERROR");
        //return ESP_FAIL;
        }

    vTaskDelete(xUdpServerHandle);
    xUdpServerHandle = NULL;
    ESP_LOGI(UDP, "Delete udpServerTask");

    return ESP_OK;
}

esp_err_t udpServer_open(void){

    if(xUdpServerHandle == NULL){

        portBASE_TYPE xStatus;

        xStatus = xTaskCreatePinnedToCore(udpServerTask, "udp_server", 4096, (void*)AF_INET, 2, &xUdpServerHandle, 0);
        if( xStatus != pdPASS || xUdpServerHandle == NULL  ){
            ESP_LOGE(UDP, "udpServerTask create error");
            return ESP_FAIL;
        }else{
                vTaskSuspend(xUdpServerHandle);
                GetTaskState(xUdpServerHandle);
            }
        return ESP_OK;
    }else{
        ESP_LOGE(UDP, "Almost exist");
        return ESP_FAIL;
    }

}

void udpServer_Resume(){
    vTaskResume(xUdpServerHandle);
    GetTaskState(xUdpServerHandle);
}

void udpServer_Suspend(){
    vTaskSuspend(xUdpServerHandle);
    GetTaskState(xUdpServerHandle);
}
