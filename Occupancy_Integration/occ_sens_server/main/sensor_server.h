/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* 
 * NOTE: this is modified example code from ESP-IDF: C:\Users\username\esp\esp-idf\examples\bluetooth\esp_ble_mesh\ble_mesh_sensor_model
 * This code was last modified by the S23 ToF Occupancy Sensor Enhancements Team for RPI's LESA.
 * 
 * NOTE: the client device is the provisioner/gateway node, the server is the unprovisioned device(s)
 * 
 * Current State: Commented, added demo states to begin DATADEMO code.
 */


// include necessary libraries
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"

#include "ble_mesh_example_init.h"
#include "board_server.h"

// this is where data is initialized
static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index);

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param);

static void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
                                              esp_ble_mesh_cfg_server_cb_param_t *param);

// The following commented out functions were removed from the code as they werent used. 
// They could be useful in the future, and their implementations are found in
// \working\Software\BLE_Mesh\ble_example\sensor_server\main\main.c

//static void example_ble_mesh_send_sensor_descriptor_status(esp_ble_mesh_sensor_server_cb_param_t *param);
//static void example_ble_mesh_send_sensor_cadence_status(esp_ble_mesh_sensor_server_cb_param_t *param);
//static void example_ble_mesh_send_sensor_settings_status(esp_ble_mesh_sensor_server_cb_param_t *param);
//static void example_ble_mesh_send_sensor_setting_status(esp_ble_mesh_sensor_server_cb_param_t *param);
//static void example_ble_mesh_send_sensor_column_status(esp_ble_mesh_sensor_server_cb_param_t *param);
//static void example_ble_mesh_send_sensor_series_status(esp_ble_mesh_sensor_server_cb_param_t *param);


static uint16_t example_ble_mesh_get_sensor_data(esp_ble_mesh_sensor_state_t *state, uint8_t *data);

static void example_ble_mesh_send_sensor_status(esp_ble_mesh_sensor_server_cb_param_t *param);

static void example_ble_mesh_sensor_server_cb(esp_ble_mesh_sensor_server_cb_event_t event,
                                              esp_ble_mesh_sensor_server_cb_param_t *param);

static esp_err_t ble_mesh_init(void);

// set the ble byte stream to be sent
void occ_sens_server_ble_msg_set(uint8_t byteStream[19]);

// initialize NVS, board, BT, BLE Mesh
void occ_sens_server_ble_init(void);
