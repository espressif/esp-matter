/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#include <stddef.h>

typedef esp_err_t (*rainmaker_controller_mqtt_params_callback_t)(const char *node_id, const char *payload,
                                                                 size_t payload_len);

esp_err_t rainmaker_controller_subscribe_params_topic(const char *topic,
                                                      rainmaker_controller_mqtt_params_callback_t callback);

esp_err_t rainmaker_controller_unsubscribe_params_topic(const char *topic);
