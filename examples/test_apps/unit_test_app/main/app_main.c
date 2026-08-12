/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "unity.h"
#include "esp_log.h"

static const char *TAG = "UT";

void app_main(void)
{
    ESP_LOGI(TAG, "esp-matter unit test app");
    unity_run_menu();
}