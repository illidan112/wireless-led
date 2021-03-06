/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <mdns.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "tcp_server.h"
#include "main.h"

#define PORT                        CONFIG_TCP_PORT
#define KEEPALIVE_IDLE              CONFIG_KEEPALIVE_IDLE
#define KEEPALIVE_INTERVAL          CONFIG_KEEPALIVE_INTERVAL
#define KEEPALIVE_COUNT             CONFIG_KEEPALIVE_COUNT

#define ESP_MDNS_URI                "esp_led"
#define ESP_MDNS_INSTANCE_NAME      "wireless_led"

#define DEFAULT_MODE                22


xTaskHandle xTCPServerHandle = NULL;

static const char *TCP = "TCP";
static const char *MDNS = "MDNS";

typedef enum lightmode{
    LIGHTMUSIC = 11,
    BACKGROUND_LIGHT = 22,
}lightmode;

static void initialise_mdns(void){
    //initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(ESP_MDNS_URI) );
    ESP_LOGI(MDNS, "mdns hostname set to: [%s]", ESP_MDNS_URI);
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set( ESP_MDNS_INSTANCE_NAME) );
}

static void approve_receive(const int sock,char* tx_buffer){


    // send() can return less bytes than supplied length.
    // Walk-around for robust implementation.
    uint8_t len = sizeof(tx_buffer);
    int to_write = len;
    while (to_write > 0) {
        int written = send(sock, tx_buffer + (len - to_write), to_write, 0);
        if (written < 0) {
            ESP_LOGE(TCP, "Error occurred during sending: errno %d", errno);
        }
        to_write -= written;
    }
}

uint8_t receiveCommand(const int sock){

    int len;
    char rx_buffer[128];
    uint8_t cmd_num;

    //receive command
    do {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TCP, "Error occurred during receiving: errno %d", errno);
        } else if (len == 0) {
            ESP_LOGW(TCP, "Connection closed");
        } else {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            ESP_LOGI(TCP, "Received %d bytes: %s", len, rx_buffer);
        }
    } while (len > 0);

   //Convert string to int
    cmd_num = atoi (rx_buffer);
    ESP_LOGI (TCP, "receive command: %d",cmd_num);

    return cmd_num;

}

static esp_err_t modeSwitcher(uint8_t cmd_num){

    switch (cmd_num)
    {
        case LIGHTMUSIC:
            ESP_LOGI(TCP, "LIGHTMUSIC");

            backgroundLightMode_pause();
            lightMusicMode_play();
            //approve_receive(sock, rx_buffer);

            break;

        case BACKGROUND_LIGHT:
            ESP_LOGI(TCP, "BACKGROUND_LIGHT");

            lightMusicMode_pause();
            backgroundLightMode_play();
            //approve_receive(sock, rx_buffer);

            break;
        default:
            ESP_LOGW(TCP, "Unknown command");
            return ESP_FAIL;
            break;
    }
    return ESP_OK;
}

static void tcpServerTask(void *pvParameters)
{
    uint8_t mode_number = 0;
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    int keepAlive = 0; //1 - yes, 0 - no
    // int keepIdle = KEEPALIVE_IDLE;
    // int keepInterval = KEEPALIVE_INTERVAL;
    // int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }
#ifdef CONFIG_EXAMPLE_IPV6
    else if (addr_family == AF_INET6) {
        struct sockaddr_in6 *dest_addr_ip6 = (struct sockaddr_in6 *)&dest_addr;
        bzero(&dest_addr_ip6->sin6_addr.un, sizeof(dest_addr_ip6->sin6_addr.un));
        dest_addr_ip6->sin6_family = AF_INET6;
        dest_addr_ip6->sin6_port = htons(PORT);
        ip_protocol = IPPROTO_IPV6;
    }
#endif

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TCP, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
    // Note that by default IPV6 binds to both protocols, it is must be disabled
    // if both protocols used at the same time (used in CI)
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif

    ESP_LOGI(TCP, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TCP, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TCP, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TCP, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TCP, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    //turning on default mode
    modeSwitcher(DEFAULT_MODE);

    while (1) {

        ESP_LOGI(TCP, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TCP, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        // setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        // setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        // setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
#ifdef CONFIG_EXAMPLE_IPV6
        else if (source_addr.ss_family == PF_INET6) {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
#endif
        ESP_LOGI(TCP, "Socket accepted ip address: %s", addr_str);

        // receive mode number
        mode_number = receiveCommand(sock);

        //switching mode
        modeSwitcher(mode_number);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

esp_err_t tcpServer_create(core_ID id )
{

    initialise_mdns();

    if(xTCPServerHandle == NULL){

        portBASE_TYPE xStatus;

#ifdef CONFIG_IPV4
        xStatus = xTaskCreatePinnedToCore(tcpServerTask, "tcp_server", 4096, (void *)AF_INET, 1, &xTCPServerHandle, id);
#endif
#ifdef CONFIG_IPV6
        xStatus = xTaskCreatePinnedToCore(tcpServerTask, "tcp_server", 4096, (void *)AF_INET6, 1, &xTCPServerHandle, id);
#endif

        if( xStatus != pdPASS || xTCPServerHandle == NULL  ){
            ESP_LOGE(TCP, "TCPServerTask create error");
            return ESP_FAIL;
        }

        vTaskSuspend(xTCPServerHandle);         // tcpServer_create() only for initialization, not for turn on
        GetTaskState(xTCPServerHandle);
        return ESP_OK;
    }else{
        ESP_LOGE(TCP, "Almost exist");
        return ESP_FAIL;
    }

}

void tcpServer_Resume(){
    ESP_LOGI(TCP, " tcpServer_Resume");
    vTaskResume(xTCPServerHandle);
}
