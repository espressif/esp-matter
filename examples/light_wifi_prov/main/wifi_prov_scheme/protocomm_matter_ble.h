/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_event.h>
#include <protocomm.h>


ESP_EVENT_DECLARE_BASE(PROTOCOMM_TRANSPORT_MATTER_BLE_EVENT);

typedef enum {
    PROTOCOMM_TRANSPORT_MATTER_BLE_CONNECTED,
    PROTOCOMM_TRANSPORT_MATTER_BLE_DISCONNECTED,
} protocomm_transport_matter_ble_event_t;

#define BLE_UUID128_VAL_LENGTH 16

#define MAX_BLE_MANUFACTURER_DATA_LEN 29

typedef struct {
    const char *name;
    uint16_t uuid;
} protocomm_matter_ble_name_uuid_t;

typedef struct {
    uint16_t evt_type;
    uint16_t conn_handle;
    union {
        int conn_status;
        int disconnect_reason;
    };
} protocomm_matter_ble_event_t;

typedef struct protocomm_matter_ble_config {
    char device_name[30];
    uint8_t service_uuid[BLE_UUID128_VAL_LENGTH];
    uint8_t *manufacturer_data;
    ssize_t manufacturer_data_len;
    ssize_t name_uuid_array_len;
    protocomm_matter_ble_name_uuid_t *name_uuid_array;
    unsigned ble_bonding:1;
    unsigned ble_sm_sc:1;
    unsigned ble_link_encryption:1;
} protocomm_matter_ble_config_t;

esp_err_t protocomm_matter_ble_start(protocomm_t *pc, const protocomm_matter_ble_config_t *config);

esp_err_t protocomm_matter_ble_stop(protocomm_t *pc);

int start_secondary_ble_adv();
