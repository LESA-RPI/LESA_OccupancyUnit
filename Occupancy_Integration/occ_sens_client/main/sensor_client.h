/* main.c - Application main entry point */

/*
 * Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"

#include "ble_mesh_example_init.h"
#include "board_client.h"

// #define TAG "BLE MESH"

// #define CID_ESP             0x02E5

// #define PROV_OWN_ADDR       0x0001

// #define MSG_SEND_TTL        3
// #define MSG_SEND_REL        false
// #define MSG_TIMEOUT         0
// #define MSG_ROLE            ROLE_PROVISIONER

// #define COMP_DATA_PAGE_0    0x00

// #define APP_KEY_IDX         0x0000
// #define APP_KEY_OCTET       0x12

// #define COMP_DATA_1_OCTET(msg, offset)      (msg[offset])
// #define COMP_DATA_2_OCTET(msg, offset)      (msg[offset + 1] << 8 | msg[offset])

// static uint8_t  dev_uuid[ESP_BLE_MESH_OCTET16_LEN];
// static uint16_t server_address = ESP_BLE_MESH_ADDR_UNASSIGNED;
// static uint16_t sensor_prop_id;

// static struct esp_ble_mesh_key {
//     uint16_t net_idx;
//     uint16_t app_idx;
//     uint8_t  app_key[ESP_BLE_MESH_OCTET16_LEN];
// } prov_key;

// static esp_ble_mesh_cfg_srv_t config_server = {
//     .beacon = ESP_BLE_MESH_BEACON_DISABLED,
// #if defined(CONFIG_BLE_MESH_FRIEND)
//     .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
// #else
//     .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
// #endif
//     .default_ttl = 7,
//     /* 3 transmissions with 20ms interval */
//     .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
//     .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
// };

// static esp_ble_mesh_client_t config_client;
// static esp_ble_mesh_client_t sensor_client;

// static esp_ble_mesh_model_t root_models[] = {
//     ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
//     ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
//     ESP_BLE_MESH_MODEL_SENSOR_CLI(NULL, &sensor_client),
// };

// static esp_ble_mesh_elem_t elements[] = {
//     ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
// };

// static esp_ble_mesh_comp_t composition = {
//     .cid = CID_ESP,
//     .elements = elements,
//     .element_count = ARRAY_SIZE(elements),
// };

// static esp_ble_mesh_prov_t provision = {
//     .prov_uuid          = dev_uuid,
//     .prov_unicast_addr  = PROV_OWN_ADDR,
//     .prov_start_address = 0x0005,
// };

static void example_ble_mesh_set_msg_common(esp_ble_mesh_client_common_param_t *common,
                                            esp_ble_mesh_node_t *node,
                                            esp_ble_mesh_model_t *model, uint32_t opcode);

static esp_err_t prov_complete(uint16_t node_index, const esp_ble_mesh_octet16_t uuid,
                               uint16_t primary_addr, uint8_t element_num, uint16_t net_idx);

static void recv_unprov_adv_pkt(uint8_t dev_uuid[ESP_BLE_MESH_OCTET16_LEN], uint8_t addr[BD_ADDR_LEN],
                                esp_ble_mesh_addr_type_t addr_type, uint16_t oob_info,
                                uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer);

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param);

static void example_ble_mesh_parse_node_comp_data(const uint8_t *data, uint16_t length);

static void example_ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                              esp_ble_mesh_cfg_client_cb_param_t *param);

void example_ble_mesh_send_sensor_message(uint32_t opcode);

static void example_ble_mesh_sensor_timeout(uint32_t opcode);

// this is where the "Boot" button printout is called
static void example_ble_mesh_sensor_client_cb(esp_ble_mesh_sensor_client_cb_event_t event,
                                              esp_ble_mesh_sensor_client_cb_param_t *param);

static esp_err_t ble_mesh_init(void);

// get the ble byte stream to be sent over wifi, done by index due to array return issues
void occ_sens_client_ble_msg_get(uint8_t* array);

void occ_sens_ble_init(void);