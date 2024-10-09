/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <lib/support/CodeUtils.h>

#include <drivers/pir.h>

#define PIR_SENSOR_PIN (static_cast<gpio_num_t>(CONFIG_PIR_DATA_PIN))

typedef struct {
    pir_sensor_config_t *config;
    bool is_initialized;
} pir_sensor_ctx_t;

static pir_sensor_ctx_t s_ctx;

static void IRAM_ATTR pir_gpio_handler(void *arg)
{
    static bool occupancy = false;
    bool new_occupancy = gpio_get_level(PIR_SENSOR_PIN);

    // we only need to notify application layer if occupancy changed
    if (occupancy != new_occupancy) {
        occupancy = new_occupancy;
        if (s_ctx.config->cb) {
            s_ctx.config->cb(s_ctx.config->endpoint_id, new_occupancy, s_ctx.config->user_data);
        }
    }
}

static void pir_gpio_init(gpio_num_t pin)
{
    gpio_reset_pin(pin);
    gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(pin, pir_gpio_handler, NULL);
}

esp_err_t pir_sensor_init(pir_sensor_config_t *config)
{
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_ctx.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    pir_gpio_init(PIR_SENSOR_PIN);

    s_ctx.config = config;
    return ESP_OK;
}
