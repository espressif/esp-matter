/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <array>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_client.h>
#include <esp_matter_console.h>

#include <app_priv.h>

#include <device.h>
#include <common_macros.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

static CHIP_ERROR format_command_fields(chip::ClusterId cluster_id, chip::CommandId command_id,
                                        int parameter_count, char **parameters,
                                        char *output, size_t output_size, bool &has_fields)
{
    VerifyOrReturnError(parameter_count >= 0 && parameter_count <= 10, CHIP_ERROR_INVALID_ARGUMENT);
    for (int index = 0; index < parameter_count; ++index) {
        VerifyOrReturnError(parameters[index][0] == '0' && parameters[index][1] == 'x' &&
                            strlen(parameters[index] + 2) <= 10,
                            CHIP_ERROR_INVALID_ARGUMENT);
    }

    has_fields = false;
    if (cluster_id == OnOff::Id) {
        return CHIP_NO_ERROR;
    }
    VerifyOrReturnError(cluster_id == Identify::Id && command_id == Identify::Commands::Identify::Id,
                        CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    VerifyOrReturnError(parameter_count == 1, CHIP_ERROR_INVALID_ARGUMENT);
    snprintf(output, output_size, "{\"0:OBJ\":{\"0:U16\": %ld}}", strtoul(parameters[0] + 2, nullptr, 16));
    has_fields = true;
    return CHIP_NO_ERROR;
}

// Four caller-owned operations preserve rapid edge-triggered button actions.
static std::array<client::invoke_command::interaction, 4> invoke_actions;
static client::binding::dispatcher bound_actions;

static CHIP_ERROR start_invoke(chip::ScopedNodeId target,
                               const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    for (auto &action : invoke_actions) {
        if (!action.active()) {
            return action.start(target, request, nullptr);
        }
    }
    return CHIP_ERROR_BUSY;
}

static CHIP_ERROR start_group_invoke(chip::GroupId group_id, chip::FabricIndex fabric_index,
                                     const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    for (auto &action : invoke_actions) {
        if (!action.active()) {
            return action.start(group_id, fabric_index, request, nullptr);
        }
    }
    return CHIP_ERROR_BUSY;
}

static CHIP_ERROR start_bound_invoke(chip::EndpointId local_endpoint,
                                     const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    return bound_actions.dispatch(local_endpoint, request, nullptr);
}

static CHIP_ERROR start_bound_invoke(chip::EndpointId local_endpoint, chip::ClusterId cluster_id,
                                     chip::CommandId command_id, int parameter_count, char **parameters)
{
    char json[32] = {};
    bool has_fields;
    ReturnErrorOnFailure(format_command_fields(cluster_id, command_id, parameter_count, parameters,
                                               json, sizeof(json), has_fields));
    client::json_encodable fields(json);
    client::invoke_command::request_t request = {
        .cluster_id = cluster_id,
        .command_id = command_id,
        .fields = has_fields ? &fields : nullptr,
    };
    return start_bound_invoke(local_endpoint, request);
}

CHIP_ERROR app_driver_bound_on_off(chip::EndpointId local_endpoint, bool on)
{
    return start_bound_invoke(local_endpoint, OnOff::Id,
                              on ? OnOff::Commands::On::Id : OnOff::Commands::Off::Id, 0, nullptr);
}

#if CONFIG_ENABLE_CHIP_SHELL
static esp_err_t app_driver_bound_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Bound commands:\n"
               "\thelp: Print help\n"
               "\tinvoke: <local_endpoint_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp bound invoke 0x0001 0x0008 0x0000 0x50 0x0 0x1 0x1.\n");
    } else if (argc >= 4 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0) {
        chip::EndpointId local_endpoint_id = strtoul((const char *)&argv[1][2], nullptr, 16);
        chip::ClusterId cluster_id = strtoul((const char *)&argv[2][2], nullptr, 16);
        chip::CommandId command_id = strtoul((const char *)&argv[3][2], nullptr, 16);

        CHIP_ERROR err = start_bound_invoke(local_endpoint_id, cluster_id, command_id,
                                            argc - 4, argv + 4);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Bound Invoke submission failed: %s", chip::ErrorStr(err));
            return ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t app_driver_client_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Client commands:\n"
               "\thelp: Print help\n"
               "\tinvoke: <fabric_index> <remote_node_id> <remote_endpoint_id> <cluster_id> <command_id>  parameters ... \n"
               "\t\tExample: matter esp client invoke 0x0001 0xBC5C01 0x0001 0x0008 0x0000 0x50 0x0 0x1 0x1.\n"
               "\tinvoke-group: <fabric_index> <group_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp client invoke-group 0x0001 0x257 0x0008 0x0000 0x50 0x0 0x1 0x1.\n");
        return ESP_OK;
    }

    const bool is_unicast = argc >= 6 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0;
    const bool is_group = argc >= 5 && strncmp(argv[0], "invoke-group", sizeof("invoke-group")) == 0;
    if (!is_unicast && !is_group) {
        ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
        return ESP_ERR_INVALID_ARG;
    }

    const int first_parameter = is_unicast ? 6 : 5;
    const chip::FabricIndex fabric_index = strtoul(argv[1] + 2, nullptr, 16);
    const chip::EndpointId endpoint_id = is_unicast ? strtoul(argv[3] + 2, nullptr, 16) : chip::kInvalidEndpointId;
    const chip::ClusterId cluster_id = strtoul(argv[is_unicast ? 4 : 3] + 2, nullptr, 16);
    const chip::CommandId command_id = strtoul(argv[is_unicast ? 5 : 4] + 2, nullptr, 16);
    char json[32] = {};
    bool has_fields;
    CHIP_ERROR err = format_command_fields(cluster_id, command_id,
                                           argc - first_parameter, argv + first_parameter,
                                           json, sizeof(json), has_fields);
    if (err != CHIP_NO_ERROR) {
        return ESP_ERR_INVALID_ARG;
    }
    client::json_encodable fields(json);
    client::invoke_command::request_t request = {
        .endpoint_id = endpoint_id,
        .cluster_id = cluster_id,
        .command_id = command_id,
        .fields = has_fields ? &fields : nullptr,
    };
    if (is_unicast) {
        const chip::NodeId node_id = strtoull(argv[2] + 2, nullptr, 16);
        err = start_invoke(chip::ScopedNodeId(node_id, fabric_index), request);
    } else {
        const chip::GroupId group_id = static_cast<chip::GroupId>(strtoul(argv[2] + 2, nullptr, 16));
        err = start_group_invoke(group_id, fabric_index, request);
    }
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Invoke submission failed: %s", chip::ErrorStr(err));
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void app_driver_register_commands()
{
    /* Add console command for bound devices */
    static const esp_matter::console::command_t bound_command = {
        .name = "bound",
        .description = "This can be used to simulate on-device control for bound devices."
        "Usage: matter esp bound <bound_command>. "
        "Bound commands: help, invoke",
        .handler = app_driver_bound_console_handler,
    };
    esp_matter::console::add_commands(&bound_command, 1);

    /* Add console command for client to control non-bound devices */
    static const esp_matter::console::command_t client_command = {
        .name = "client",
        .description = "This can be used to simulate on-device control for client devices."
        "Usage: matter esp client <client_command>. "
        "Client commands: help, invoke",
        .handler = app_driver_client_console_handler,
    };
    esp_matter::console::add_commands(&client_command, 1);
}
#endif // CONFIG_ENABLE_CHIP_SHELL

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    return led_driver_set_power(handle, val->val.b);
}

static esp_err_t app_driver_light_set_brightness(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    return led_driver_set_brightness(handle, value);
}

static esp_err_t app_driver_light_set_hue(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
    return led_driver_set_hue(handle, value);
}

static esp_err_t app_driver_light_set_saturation(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
    return led_driver_set_saturation(handle, value);
}

static esp_err_t app_driver_light_set_temperature(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
    return led_driver_set_temperature(handle, value);
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val;
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_driver_handle_t handle = (led_driver_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = app_driver_light_set_power(handle, val);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                err = app_driver_light_set_brightness(handle, val);
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                err = app_driver_light_set_hue(handle, val);
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                err = app_driver_light_set_saturation(handle, val);
            } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
                err = app_driver_light_set_temperature(handle, val);
            }
        }
    }
    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_driver_handle_t handle = (led_driver_handle_t)priv_data;
    esp_matter_attr_val_t val;

    /* Setting brightness */
    attribute_t *attribute = attribute::get(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_brightness(handle, &val);

    /* Setting color */
    attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorMode::Id);
    attribute::get_val(attribute, &val);
    if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kCurrentHueAndCurrentSaturation) {
        /* Setting hue */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_hue(handle, &val);
        /* Setting saturation */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_saturation(handle, &val);
    } else if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kColorTemperature) {
        /* Setting temperature */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id);
        attribute::get_val(attribute, &val);
        err |= app_driver_light_set_temperature(handle, &val);
    } else {
        ESP_LOGE(TAG, "Color mode not supported");
    }

    /* Setting power */
    attribute = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_power(handle, &val);

    return err;
}

app_driver_handle_t app_driver_light_init()
{
    /* Initialize led */
    led_driver_config_t config = led_driver_get_config();
    led_driver_handle_t handle = led_driver_init(&config);
    return (app_driver_handle_t)handle;
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    button_handle_t handle = NULL;
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = button_driver_get_config();

    if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create button device");
        return NULL;
    }

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL, app_driver_button_toggle_cb, NULL);

#if CONFIG_ENABLE_CHIP_SHELL
    app_driver_register_commands();
#endif // CONFIG_ENABLE_CHIP_SHELL

    return (app_driver_handle_t)handle;
}
