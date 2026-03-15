#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_nan.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "socket.h" 


#define START_BYTE 0x02  // ASCII STX
#define END_BYTE   0x03  // ASCII ETX
#define ACK_CHAR   'K'   // "OK" confirmation

#include "esp_nan.h"
#include "lwip/sockets.h"

#define PORT 8080
#define STX 0x02
#define ETX 0x03
#define ACK 0x06

void wifi_aware_publisher_task(void *pvParameters) {
    // 1. Publish the service so Android can find it
    wifi_nan_publish_cfg_t publish_cfg = {
        .service_name = "ControlRemote",
        .match_filter = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37}, // "1234567"
        .type = NAN_PUBLISH_TYPE_UNSOLICITED,
    };
    esp_wifi_nan_publish_start(&publish_cfg);

    // 2. Setup Socket Server
    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in dest_addr = {
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    listen(listen_sock, 1);

    while (1) {
        struct sockaddr_in source_addr;
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

        if (sock >= 0) {
            // Send the scheduler data to the Android Subscriber
            uint8_t tx_buffer[502]; 
            // ... Fill buffer with [STX][Count][Data...][ETX]
            send(sock, tx_buffer, sizeof(tx_buffer), 0);

            // Wait for 0x06 ACK from Android
            uint8_t response;
            if (recv(sock, &response, 1, 0) > 0 && response == ACK) {
                printf("Android received the data! Sync complete.\n");
            }
            close(sock);
        }
    }
}
