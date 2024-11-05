/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <stdint.h>
#include <string.h>

#include <esp_log.h>

#include <esp_matter_attribute_utils.h>

/**
 * @brief
 *
 * @param espnow_macaddr
 *
 * @return void
 */
esp_err_t espnow_bridge_match_bridged_switch(uint8_t espnow_macaddr[6], uint16_t espnow_initiator_attr, uint32_t matter_device_type_id);

/**
 * @brief
 *
 * @param espnow_macaddr
 *
 * @return void
 */
esp_err_t espnow_bridge_remove_bridged_switch(uint8_t espnow_macaddr[6]);
