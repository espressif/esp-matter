
#ifndef APP_DQSMART_H
#define APP_DQSMART_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include <app_priv.h>


#define LED_CHECK_RESET_FACTORY 1
#define LED_COMMISSING 2
#define LED_COMMISSINGCOMPLETE 3


led_driver_handle_t _handle = NULL;

static const char *TAG_ = "app_dqsmart";
extern DQSmartBulb dqsmart_bulb;

esp_timer_handle_t periodic_timer_reset_factory;
esp_timer_handle_t periodic_timer_led;

static void periodic_timer_callback(void* arg);
static void periodic_timer_led_callback(void* arg);

namespace DQSTIMER
{
    void create_timer_reset_factory()
    {
        const esp_timer_create_args_t periodic_timer_reset_factory_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
        };
        ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_reset_factory_args, &periodic_timer_reset_factory));
    }

    void start_timer_reset_factory(uint32_t microseconds)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_reset_factory, microseconds));
    }

    void stop_timer_reset_factory()
    {
         ESP_ERROR_CHECK(esp_timer_stop(periodic_timer_reset_factory));
        // app_driver_set_level_led_status(1);
    }

    void create_timer_led()
    {
        const esp_timer_create_args_t periodic_timer_led_args = {
            .callback = &periodic_timer_led_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic_led"
        };
        ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_led_args, &periodic_timer_led));
    }

    void start_timer_led(uint32_t microseconds)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_led, microseconds));
    }

    void stop_timer_led()
    {
        if(periodic_timer_led != NULL)
        ESP_ERROR_CHECK(esp_timer_stop(periodic_timer_led));
    }

}

static void periodic_timer_callback(void* arg)
{
     ESP_LOGI(TAG_,"CALLBACK restart counter from NVS ... ");
    int64_t time_since_boot = esp_timer_get_time();
    esp_err_t err = ESP_OK;
    dqsmart_bulb.counter_reset_factory = 0;
    err = nvs_set_i32(dqsmart_bulb.my_handle, "restart_counter", dqsmart_bulb.counter_reset_factory);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(dqsmart_bulb.my_handle);
    DQSTIMER::stop_timer_reset_factory();
    
}

static void periodic_timer_led_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    esp_err_t err = ESP_OK;
    // Close
    dqsmart_bulb.mSetStateCommissing();

}

#endif