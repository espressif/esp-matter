/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <esp_log.h>

#include <app_driver.h>
#include <board.h>
#include <led_driver.h>

typedef struct driver_src {
    char name[SRC_MAX_NAMELEN];
    struct app_driver_param_callback callbacks;
    struct driver_src *next;
} driver_src_t;

static const char *TAG = "app_driver";
static driver_src_t *s_driver_src = NULL;

esp_err_t app_driver_init()
{
    return board_init();
}

esp_err_t app_driver_register_src(const char *name, app_driver_param_callback_t *callbacks)
{
    driver_src_t *new_src = NULL;

    if (name == NULL || callbacks == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    new_src = (driver_src_t *)malloc(sizeof(driver_src_t));
    if (new_src == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for driver_src_t");
        return ESP_ERR_NO_MEM;
    }

    memset(new_src, 0, sizeof(driver_src_t));
    strncpy(new_src->name, name, strnlen(name, SRC_MAX_NAMELEN));
    memcpy(&new_src->callbacks, callbacks, sizeof(app_driver_param_callback_t));

    new_src->next = s_driver_src;
    s_driver_src = new_src;

    return ESP_OK;
}

esp_err_t app_driver_update_and_report_power(bool power, const char *src)
{
    esp_err_t ret;
    driver_src_t *cur_src = s_driver_src;

    /* Update */
    ret = led_driver_set_power(power);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Report to other sources */
    ESP_LOGI(TAG, "Power (OnOff) set to: %d", power);
    while (cur_src) {
        if (strncmp(cur_src->name, src, strnlen(src, SRC_MAX_NAMELEN)) != 0 &&
                cur_src->callbacks.update_power != NULL) {
            cur_src->callbacks.update_power(power);
        }
        cur_src = cur_src->next;
    }
    return ret;
}

esp_err_t app_driver_update_and_report_brightness(uint8_t brightness, const char *src)
{
    esp_err_t ret;
    driver_src_t *cur_src = s_driver_src;

    /* Update */
    ret = led_driver_set_brightness(brightness);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Report to other sources */
    ESP_LOGI(TAG, "Brightness set to: %d percent", brightness * 100 / 254);
    while (cur_src) {
        if (strncmp(cur_src->name, src, strnlen(src, SRC_MAX_NAMELEN)) != 0 &&
                cur_src->callbacks.update_brightness != NULL) {
            cur_src->callbacks.update_brightness(brightness);
        }
        cur_src = cur_src->next;
    }
    return ret;
}

esp_err_t app_driver_update_and_report_hue(uint16_t hue, const char *src)
{
    esp_err_t ret;
    driver_src_t *cur_src = s_driver_src;
    
    /* Update */
    ret = led_driver_set_hue(hue);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Report to other sources */
    ESP_LOGI(TAG, "Hue set to: %d degree", hue);
    while (cur_src) {
        if (strncmp(cur_src->name, src, strnlen(src, SRC_MAX_NAMELEN)) != 0 &&
                cur_src->callbacks.update_hue != NULL) {
            cur_src->callbacks.update_hue(hue);
        }
        cur_src = cur_src->next;
    }
    return ret;
}

esp_err_t app_driver_update_and_report_saturation(uint8_t saturation, const char *src)
{
    esp_err_t ret;
    driver_src_t *cur_src = s_driver_src;

    /* Update */
    ret = led_driver_set_saturation(saturation);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Report to other sources */
    ESP_LOGI(TAG, "Saturation set to: %d percent", saturation);
    while (cur_src) {
        if (strncmp(cur_src->name, src, strnlen(src, SRC_MAX_NAMELEN)) != 0 &&
                cur_src->callbacks.update_saturation != NULL) {
            cur_src->callbacks.update_saturation(saturation);
        }
        cur_src = cur_src->next;
    }
    return ret;
}

esp_err_t app_driver_update_and_report_temperature(uint32_t temperature, const char *src)
{
    esp_err_t ret;
    driver_src_t *cur_src = s_driver_src;

    /* Update */
    ret = led_driver_set_temperature(temperature);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Report to other sources */
    ESP_LOGI(TAG, "Color Temperature set to: %d kelvin", temperature);
    while (cur_src) {
        if (strncmp(cur_src->name, src, strnlen(src, SRC_MAX_NAMELEN)) != 0 &&
                cur_src->callbacks.update_temperature != NULL) {
            cur_src->callbacks.update_temperature(temperature);
        }
        cur_src = cur_src->next;
    }   
    return ret;
}

bool app_driver_get_power()
{
    return led_driver_get_power();
}

uint8_t app_driver_get_brightness()
{
    return led_driver_get_brightness();
}

uint16_t app_driver_get_hue()
{
    return led_driver_get_hue();
}

uint8_t app_driver_get_saturation()
{
    return led_driver_get_saturation();
}

uint32_t app_driver_get_temperature()
{
    return led_driver_get_temperature();
}
