
#ifndef APP_DQSMART_H
#define APP_DQSMART_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include <app_priv.h>

led_driver_handle_t _handle = NULL;
bool val =0;
static const char *TAG_ = "app_dqsmart";
esp_timer_handle_t periodic_timer;
static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    val = !val;
    app_driver_set_level_led_status(val);
}

namespace DQSLED
{
    bool commissioning_status = 0;
    void create_timer_led_status(led_driver_handle_t handle)
    {
        _handle = handle;
        const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
        };
        ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    }

    void start_timer_led_status(uint32_t microseconds)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, microseconds));
    }

    void stop_timer_led_status()
    {
         ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
         app_driver_set_level_led_status(1);
    }

    void set_led_status_off()
    {
         app_driver_set_level_led_status(1);
    }

    uint8_t getModelSwitch()
    {
        return (uint8_t)2;
    }
}

#endif