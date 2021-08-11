/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define APP_LOG_TAG "rainmaker_light"

#define DEFAULT_POWER false
#define DEFAULT_BRIGHTNESS 100
#define DEFAULT_HUE 0
#define DEFAULT_SATURATION 0

#define HUE_MAX 359
#define HUE_ATTRIBUTE_MAX 254
#define SATURATION_MAX 100
#define SATURATION_ATTRIBUTE_MAX 254
#define BRIGHTNESS_MAX 100
#define BRIGHTNESS_ATTRIBUTE_MAX 254

#define APP_DRIVER_SRC_LOCAL     "local"
#define APP_DRIVER_SRC_MATTER    "matter"
#define APP_DRIVER_SRC_RAINMAKER "rainmaker"

#define REMAP_TO_RANGE(value, from, to)  value * to / from

#ifdef __cplusplus
}
#endif
