/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <device.h>

#include <app_priv.h>
#include <app_reset.h>

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    button_config_t config = button_driver_get_config();
    button_handle_t handle = iot_button_create(&config);

    return (app_driver_handle_t)handle;
}
