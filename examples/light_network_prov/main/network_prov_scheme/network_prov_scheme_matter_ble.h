/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <protocomm.h>
#include <protocomm_ble.h>
#include <stdint.h>
#include <esp_err.h>

#include <network_provisioning/manager.h>

extern const network_prov_scheme_t network_prov_scheme_matter_ble;

#define NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM {      \
    .event_cb  = network_prov_scheme_ble_event_cb_free_btdm,   \
    .user_data = NULL                                       \
}

void network_prov_scheme_matter_ble_event_cb_free_btdm(void *user_data, network_prov_cb_event_t event, void *event_data);

esp_err_t network_prov_scheme_matter_ble_set_service_uuid(uint8_t *uuid, size_t uuid_size);

esp_err_t network_prov_scheme_matter_ble_set_mfg_data(uint8_t *mfg_data, size_t mfg_data_len);
