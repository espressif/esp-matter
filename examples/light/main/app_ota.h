/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "esp_err.h"

/** Initialize the matter OTA Requestor
 *
 */
void matter_ota_requestor_init(void);

/** Register the matter OTA commands
 *
 */
void esp_matter_console_ota_register_commands(void);
