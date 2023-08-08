/* board.c - Board-specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "iot_button.h"
#include "esp_ble_mesh_sensor_model_api.h"

#define TAG "BOARD"

#define BUTTON_IO_NUM           0
#define BUTTON_ACTIVE_LEVEL     0

// which demo should we use?
//#define ORGDEMO // original Espressif Demo
#define DATADEMO // demo which only prints sensor data output

extern void example_ble_mesh_send_sensor_message(uint32_t opcode);

// as originally programmed, cycles through state gets
// code pertaining to cadence, descriptor, settings, series largely removed as not immediately usefu;
// may be reimplemented later if use case is apparent
#ifdef ORGDEMO
static uint32_t send_opcode[] = {
    [0] = ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET,
    [1] = ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET,
    [2] = ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET,
    [3] = ESP_BLE_MESH_MODEL_OP_SENSOR_GET,
    [4] = ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET,
};
static uint8_t press_count;

static void button_tap_cb(void* arg)
{
    example_ble_mesh_send_sensor_message(send_opcode[press_count++]);
    press_count = press_count % ARRAY_SIZE(send_opcode);
}
#endif

// only needs to get data state
#ifdef DATADEMO
static uint32_t send_opcode = ESP_BLE_MESH_MODEL_OP_SENSOR_GET;

static void button_tap_cb(void* arg)
{
    example_ble_mesh_send_sensor_message(send_opcode);
}
#endif

static void board_button_init(void)
{
    button_handle_t btn_handle = iot_button_create(BUTTON_IO_NUM, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, button_tap_cb, "RELEASE");
    }
}

void board_init(void)
{
    board_button_init();
}
