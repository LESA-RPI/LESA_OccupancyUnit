/*This program is a combination of 2 seperate example provided by expressif
First the ESP connects to campus wifi, then send a HTTP GET request to the php file in the server in marks office
From there the PHP file (sendToDatabase.php) takes the data from the URL and converts it into a MySQL query
a visual diagram is provided in working/Engineering Analysis/BLE mesh/BLE mesh diagrams.pptx*/

// Header file for WPA2 Wi-Fi
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"


// Header file for http client
#include "esp_tls.h"
#include "esp_http_client.h"

// Configuration for WPA2 Wi-Fi
#define EXAMPLE_WIFI_SSID "eduroam"
#define EXAMPLE_EAP_METHOD CONFIG_EXAMPLE_EAP_METHOD

#define EXAMPLE_EAP_ID CONFIG_EXAMPLE_EAP_ID
#define EXAMPLE_EAP_USERNAME CONFIG_EXAMPLE_EAP_USERNAME
#define EXAMPLE_EAP_PASSWORD CONFIG_EXAMPLE_EAP_PASSWORD

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? 
const int CONNECTED_BIT = BIT0;

static const char *TAG = "example";
static const char *TAG2 = "http";*/

//event handler from wifi example
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
//function connects ESP to campus wifi, is a cleaned up verison from example on the Rep with the commented section removed
static void initialise_wifi(void);

//event handler from http example
esp_err_t _http_event_handler(esp_http_client_event_t *evt);

static void http_rest_with_url(char* query); // str parameter removed, will have to add back later

void http_test_task(void *pvParameters); //added from http example, done this way to properly allocate stack size in main task