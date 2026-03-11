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
#include "display_functions.c" 

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

// this file can call any function of any file expept main.
// rx_buffer[0] contain data type, rx_buffer[1] contain data lenght rx_buffer[len-1] end confirmation 0xff

#define HANDSHAKE_OK 0XFF
#define HANDSHAKE_FAILED 0X00

//macro to identify received data for sheduler devices
#define RECEIVED_SCHEDULER 1

//macro to identify data for run control remote
#define RECEIVED_C_REMOTE 3

//macro to define data for setup time
#define RECEIVED_TIME 7

//macro to define data for schedule charger time
#define RECEIVED_CHARGER_TIME 15

//macro to identify request present esp32 configuration feed back
#define RECEIVED_REPORT_REQ 35

/*

From nan_app.c :
* NAN States 
#define NAN_STARTED_BIT     BIT0
#define NAN_STOPPED_BIT     BIT1

 NAN Events 
#define NDP_INDICATION      BIT2
#define NDP_ACCEPTED        BIT3
#define NDP_TERMINATED      BIT4
#define NDP_REJECTED        BIT5

 Macros
#define MACADDR_LEN             6
#define MACADDR_EQUAL(a1, a2)   (memcmp(a1, a2, MACADDR_LEN))
#define MACADDR_COPY(dst, src)  (memcpy(dst, src, MACADDR_LEN))
#define NAN_DW_INTVL_MS         524      NAN DW interval (512 TU's ~= 524 mSec) 
#define NAN_NDP_RESP_TIMEOUT_DW 8
#define NAN_NDP_RESP_TIMEOUT    NAN_NDP_RESP_TIMEOUT_DW*NAN_DW_INTVL_MS
#define NAN_NDP_TERM_TIMEOUT    2*NAN_DW_INTVL_MS  NDP Termination Timeout - 2 DW

From esp_nan.h:
#define NDP_STATUS_ACCEPTED     1
#define NDP_STATUS_REJECTED     2


*/



static int BUFFER_SIZE= 128;


static char CONFIG_ESP_WIFI_NAN_MATCHING_FILTER[7] ={'1','2','3','4','5','6','\n'};

static char MAC_REMOTE[ETH_ALEN]; 
 
static EventGroupHandle_t nan_event_group;

static const char *TAG = "publisher";

static int NAN_RECEIVE = BIT0;

uint8_t g_peer_inst_id;

static TaskHandle_t xtaskHandleSocket = NULL; 

static TaskHandle_t	xdiscovery_task = NULL;

static TaskHandle_t xserver_task = NULL;

void load_settings_from_nvs() {
    nvs_handle_t my_handle;
    if (nvs_open("storage", NVS_READONLY, &my_handle) == ESP_OK) {
        size_t mac_size = 6;
        size_t filter_size = 7;

        // If these fail, they keep the hardcoded default values
        nvs_get_blob(my_handle, "peer_mac", MAC_REMOTE, &mac_size);
        nvs_get_blob(my_handle, "match_filter", CONFIG_ESP_WIFI_NAN_MATCHING_FILTER, &filter_size);
        
        nvs_close(my_handle);
        ESP_LOGI(TAG, "NVS: Restored MAC and Filter");
    }
}

void save_settings_to_nvs() {
    nvs_handle_t my_handle;
    if (nvs_open("storage", NVS_READWRITE, &my_handle) == ESP_OK) {
        nvs_set_blob(my_handle, "peer_mac", MAC_REMOTE, 6);
        nvs_set_blob(my_handle, "match_filter", CONFIG_ESP_WIFI_NAN_MATCHING_FILTER, 7);
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}

void save_buffer_size_to_nvs() {
    nvs_handle_t buffer;
    if (nvs_open("storage", NVS_READWRITE, &buffer) == ESP_OK) {
        nvs_set_i32(buffer, "buf_size", BUFFER_SIZE); 
        nvs_commit(buffer);
        nvs_close(buffer);
    }
}

void load_buffer_size_from_nvs() {
    nvs_handle_t buffer;
    if (nvs_open("storage", NVS_READONLY, &buffer) == ESP_OK) {
        size_t buffer_size = 4;
        // If these fail, they keep the hardcoded default values
        nvs_get_i32(buffer, "buffer_size", &BUFFER_SIZE);
        
        nvs_close(buffer_size);
        ESP_LOGI(TAG, "NVS: RESTORED BUFFERSIZE");
    }
}

static void nan_receive_event_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data){
    wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)event_data;
    g_peer_inst_id = evt->peer_inst_id;
    
    memcpy(MAC_REMOTE, evt->peer_if_mac, ETH_ALEN);
    
    save_settings_to_nvs();
    
    if (evt->ssi_len) {
        ESP_LOGI(TAG, "Received payload from Peer "MACSTR" [Peer Service id - %d] - ", MAC2STR(evt->peer_if_mac), evt->peer_inst_id);
    }
    xEventGroupSetBits(nan_event_group, NAN_RECEIVE);
}

static void nan_ndp_indication_event_handler(void *arg, esp_event_base_t event_base,
                                             int32_t event_id, void *event_data)
{
    if (event_data == NULL) {
        return;
    }
    wifi_event_ndp_indication_t *evt = (wifi_event_ndp_indication_t *)event_data;

    wifi_nan_datapath_resp_t ndp_resp = {0};
    ndp_resp.accept = true; /* Accept incoming datapath request */
    ndp_resp.ndp_id = evt->ndp_id;
    memcpy(ndp_resp.peer_mac, evt->peer_nmi, ETH_ALEN);

    esp_wifi_nan_datapath_resp(&ndp_resp);

}

static void nan_ndp_confirm_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data) {
    wifi_event_ndp_confirm_t *evt = (wifi_event_ndp_confirm_t *)data;
    
    if (evt->status == NDP_STATUS_ACCEPTED) {
        ESP_LOGI(TAG, "NDP Data Link Confirmed. Notifying Socket Task...");
        if (xserver_task != NULL) {
            xTaskNotifyGive(xserver_task); // Unblocks the socket accept() loop
        }
    }
}

uint8_t wifi_nan_publish(void){
   nan_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NAN_RECEIVE,
                    &nan_receive_event_handler,
                    NULL,
                    &instance_any_id));

    /* Start NAN Discovery */
    wifi_nan_config_t nan_cfg = WIFI_NAN_CONFIG_DEFAULT(); //wifi_nan_sync_config_t and WIFI_NAN_SYNC_CONFIG_DEFAULT() this mention in example does not exist

    esp_netif_create_default_wifi_nan();
    esp_wifi_nan_start(&nan_cfg);

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NDP_INDICATION,
                    &nan_ndp_indication_event_handler,
                    NULL,
                    &instance_any_id));
                    
                    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NDP_CONFIRM, &nan_ndp_confirm_event_handler, NULL, NULL));

    /* Publish a service */
    uint8_t pub_id;
    wifi_nan_publish_cfg_t publish_cfg = {
        .service_name = "ControlRemote",
        .type = NAN_PUBLISH_UNSOLICITED,
		//.ndp_resp_needed=1,
        /* 0 - All incoming NDP requests will be internally accepted,
           1 - All incoming NDP requests raise NDP_INDICATION event and require esp_wifi_nan_datapath_resp to accept or reject. */   		
    };
    
    memcpy(publish_cfg.matching_filter, CONFIG_ESP_WIFI_NAN_MATCHING_FILTER, 7);
    
    // npd response needed was included into this function and removed from config as shown esp_nan.h example looks like it is wrong
    // esp_wifi_types_generic.h esp-idf-5.5,1 does not includes the .ndp_resp_needed 
    
    pub_id = esp_wifi_nan_publish_service(&publish_cfg, true); 
    if (pub_id == 0) {
        return 0;
    }
    return pub_id;
}

void nan_discovery_task(void *pvParameters) {
    uint32_t pub_id;
    
    xTaskNotifyWait(0,0,&pub_id,portMAX_DELAY);
    
    wifi_nan_followup_params_t fup = { .inst_id =(uint8_t) pub_id };

    for(;;) {
        // Wait for the signal from the handler
        EventBits_t bits = xEventGroupWaitBits(nan_event_group, NAN_RECEIVE, pdTRUE, pdFALSE, portMAX_DELAY);
        
        // YES - You MUST copy the data here to update the 'fup' object
        if (bits & NAN_RECEIVE) {
            fup.peer_inst_id = g_peer_inst_id;
            memcpy(fup.peer_mac, MAC_REMOTE, ETH_ALEN);
            
            // Now get the IP and send the 16-byte SSI
            esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_NAN_UNK");
            esp_ip6_addr_t ip6;
            if (esp_netif_get_ip6_linklocal(netif, &ip6) == ESP_OK) {
                fup.ssi = (uint8_t*)ip6.addr; 
                fup.ssi_len = 16;
                esp_wifi_nan_send_message(&fup);
                ESP_LOGI(TAG, "IPv6 Follow-up sent to Android MAC: "MACSTR, MAC2STR(fup.peer_mac));
            }
        }
    }
}

void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
}

void processScheduler(char *rx_buffer[2],int len){
//pending
}

void runDevice(char *rx_buffer[2],int len){
//pending
}

void setupTime(char *rx_buffer[2],int len){
//pending
}

void chargerScheduler(char *rx_buffer[2],int len){
//pending
}

void sendStatus(char *rx_buffer[2],int len){
//pending
}


void wifi_aware_socket_task(void *pvParameters) {
	
	load_buffer_size_from_nvs();
    char rx_buffer[BUFFER_SIZE];
    
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "NDP Ready. Opening Socket...");

        int listen_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
        if (listen_sock < 0) continue;

        // 1. Allow immediate restart if the task cycles
        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in6 dest_addr = {
            .sin6_family = AF_INET6,
            .sin6_port = htons(8080),
            .sin6_addr = IN6ADDR_ANY_INIT, // Listens on NAN Link-Local
        };

        if (bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == 0) {
            listen(listen_sock, 1);
            
            // This blocks and yields CPU to your ADC/PWM tasks
            struct sockaddr_in6 source_addr;
            socklen_t addr_len = sizeof(source_addr);
            int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

            if (sock >= 0) {
                // 2. Receive data from Android
                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                if (len > 0) {
                    ESP_LOGI(TAG, "From Android: %s", rx_buffer);

                    // 3. Send response back hand shake ok with 0xff
 					if(rx_buffer[len]==(char) 0xff){                   
                    	const char msg = (char) HANDSHAKE_OK;
                    	send(sock, &msg, 1, 0);
                    	
                    	char * ptr_rx_buffer = &rx_buffer[2];
                    	
                    	switch(rx_buffer[0]){
                    	
                    		case RECEIVED_SCHEDULER:
                    			processScheduler(&ptr_rx_buffer, len-3);
                    			goto close;
                    		
                    		case RECEIVED_C_REMOTE:
                    			runDevice(&ptr_rx_buffer, len-3);
                    			goto close;
                    	
                    		case RECEIVED_TIME:
                    			setupTime(&ptr_rx_buffer, len-3);
                    			goto close;
                    	
                    		case RECEIVED_CHARGER_TIME:
                    			chargerScheduler(&ptr_rx_buffer, len-3);
                    			goto close;
                    	
                    		case RECEIVED_REPORT_REQ:
                    			sendStatus(&ptr_rx_buffer, len-3);
                    			goto close;
                    	}
                    
                    }
                    else{
                    	const char msg = (char) HANDSHAKE_FAILED;// 00 means resend  
                    	send(sock, &msg, 1, 0);
                    	}
                    
                }
                close:
                close(sock);
            }
        }
        
        close(listen_sock);
        ESP_LOGI(TAG, "Socket cycle finished. Waiting for next notification.");
    }
}

void wifi_aware_publish(void *pvParameters) {

int pub_id;

// this task only runs ones to setup wifi and publish so no loop is needed

	initialise_wifi();

	vTaskDelay(pdMS_TO_TICKS(10));

	pub_id = wifi_nan_publish();
	
	vTaskDelay(pdMS_TO_TICKS(10));
	if (pub_id > 0) { 
    	if (xdiscovery_task != NULL) {
        	xTaskNotify(xdiscovery_task, pub_id, eSetValueWithOverwrite);
    	} else {
        ESP_LOGE("WIFI", "CRITICAL: Discovery Task handle is NULL!");
    	}
    	}else{ ESP_LOGE("WIFI", "NAN Publish failed to return a valid ID!");
    }

 vTaskDelete(NULL);

}
