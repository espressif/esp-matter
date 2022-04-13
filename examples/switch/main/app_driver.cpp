/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <light_driver.h>

#include <app_priv.h>

using chip::kInvalidClusterId;
static constexpr chip::CommandId kInvalidCommandId = 0xFFFF'FFFF;

using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern int switch_endpoint_id;
static int g_cluster_id = kInvalidClusterId;
static int g_command_id = kInvalidCommandId;

static esp_err_t app_driver_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Driver commands:\n"
               "\thelp: Print help\n"
               "\tsend_bind: <endpoint_id> <cluster_id> <command_id>. "
               "Example: matter esp driver send_bind 0x0001 0x0006 0x0002.\n"
               "\tsend: <fabric_index> <remote_node_id> <remote_endpoint_id> <cluster_id> <command_id>. "
               "Example: matter esp driver send 0x0001 0xBC5C01 0x0001 0x0006 0x0002.\n");
    } else if (argc == 4 && strncmp(argv[0], "send_bind", sizeof("send_bind")) == 0) {
        int endpoint_id = strtol((const char *)&argv[1][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[2][2], NULL, 16);
        int command_id = strtol((const char *)&argv[3][2], NULL, 16);

        g_cluster_id = cluster_id;
        g_command_id = command_id;
        client::cluster_update(endpoint_id, cluster_id);
    } else if (argc == 6 && strncmp(argv[0], "send", sizeof("send")) == 0) {
        int fabric_index = strtol((const char *)&argv[1][2], NULL, 16);
        int node_id = strtol((const char *)&argv[2][2], NULL, 16);
        int remote_endpoint_id = strtol((const char *)&argv[3][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[4][2], NULL, 16);
        int command_id = strtol((const char *)&argv[5][2], NULL, 16);

        g_cluster_id = cluster_id;
        g_command_id = command_id;
        client::connect(fabric_index, node_id, remote_endpoint_id);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static void app_driver_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "driver",
        .description = "This can be used to simulate on-device control. Usage: matter esp driver <driver_command>. "
                       "Driver commands: help, send, send_bind",
        .handler = app_driver_console_handler,
    };
    esp_matter_console_add_command(&command);
}

void app_driver_client_command_callback(client::peer_device_t *peer_device, int remote_endpoint_id, void *priv_data)
{
    /** TODO: Find a better way to get the cluster_id and command_id.
    Once done, move the console commands to esp_matter_client. */
    if (g_cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
        if (g_command_id == ZCL_OFF_COMMAND_ID) {
            on_off::command::send_off(peer_device, remote_endpoint_id);
        } else if (g_command_id == ZCL_ON_COMMAND_ID) {
            on_off::command::send_on(peer_device, remote_endpoint_id);
        } else if (g_command_id == ZCL_TOGGLE_COMMAND_ID) {
            on_off::command::send_toggle(peer_device, remote_endpoint_id);
        }
    }
}

esp_err_t app_driver_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val)
{
    /* Nothing to do here */
    return ESP_OK;
}

static esp_err_t app_driver_attribute_set_defaults()
{
    /* Get the default value (current value) from esp_matter and update the app_driver */
    esp_err_t err = ESP_OK;
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get_first(node);
    while (endpoint) {
        int endpoint_id = endpoint::get_id(endpoint);
        cluster_t *cluster = cluster::get_first(endpoint);
        while (cluster) {
            int cluster_id = cluster::get_id(cluster);
            attribute_t *attribute = attribute::get_first(cluster);
            while (attribute) {
                int attribute_id = attribute::get_id(attribute);
                esp_matter_attr_val_t val = esp_matter_invalid(NULL);
                err |= attribute::get_val(attribute, &val);
                err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, &val);
                attribute = attribute::get_next(attribute);
            }
            cluster = cluster::get_next(cluster);
        }
        endpoint = endpoint::get_next(endpoint);
    }
    return err;
}

esp_err_t app_driver_init()
{
    ESP_LOGI(TAG, "Initialising driver");
    // device_init();
    app_driver_attribute_set_defaults();
    app_driver_register_commands();
    client::set_command_callback(app_driver_client_command_callback, NULL);
    return ESP_OK;
}
