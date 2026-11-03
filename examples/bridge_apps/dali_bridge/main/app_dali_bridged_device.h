/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app_bridged_device.h>

typedef struct dali_device_addr {
    uint8_t short_addr; /**< DALI short address (0–63) */
    uint8_t color_caps; /**< IEC 62386-209 colour type features; 0 for non-DT8 gear */
} dali_device_addr_t;

class app_dali_bridged_device_t : public app_bridged_device_t {
public:
    esp_err_t set_dev_addr(const void *addr_ctx) override;

    bool check_dev_addr(const void *addr_ctx) override;

    esp_err_t delete_dev_addr() override;

    esp_err_t store_dev_addr() override;

    esp_err_t restore_dev_addr() override;

    esp_err_t erase_dev_addr() override;
};
