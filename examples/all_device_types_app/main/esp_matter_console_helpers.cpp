/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "esp_matter.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "driver/usb_serial_jtag.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "hal/uart_types.h"
#include "driver/uart.h"
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include "esp_vfs_usb_serial_jtag.h"
#include "driver/usb_serial_jtag.h"
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include <app_priv.h>
#include <helpers.h>
#include <device_types.h>
#include <app/clusters/fan-control-server/fan-control-delegate.h>
#include <app/clusters/fan-control-server/fan-control-server.h>

#ifdef CONFIG_OPENTHREAD_BORDER_ROUTER
#include <platform/KvsPersistentStorageDelegate.h>
#include <platform/OpenThread/GenericThreadBorderRouterDelegate.h>
using chip::app::Clusters::ThreadBorderRouterManagement::GenericOpenThreadBorderRouterDelegate;
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER


using namespace esp_matter;

extern uint16_t app_endpoint_id;

static const char *TAG = "esp_matter_console_helpers";

extern SemaphoreHandle_t semaphoreHandle;

#define PROMPT_STR CONFIG_IDF_TARGET

#define SPEED_MAX_SET 100
#define ROCK_SET 0
#define ROCK_SUPPORT_SET 7
#define AIRFLOW_DIRECTION_SET 0
#define WIND_SET  0
#define WIND_SUPPORT_SET 7

#if CONFIG_STORE_HISTORY

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

static void initialize_console(void)
{
    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);
#if CONFIG_ESP_CONSOLE_UART_DEFAULT
    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
            .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
#if SOC_UART_SUPPORT_REF_TICK
        .source_clk = UART_SCLK_REF_TICK,
#elif SOC_UART_SUPPORT_XTAL_CLK
        .source_clk = UART_SCLK_XTAL,
#endif
    };
#if (CONFIG_ESP_CONSOLE_UART_NUM == 0)
    uart_port_t uart_port = UART_NUM_0;
#elif (CONFIG_ESP_CONSOLE_UART_NUM == 1)
    uart_port_t uart_port = UART_NUM_1;
#endif
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(uart_port, 256, 0, 0, NULL, 0) );
    ESP_ERROR_CHECK( uart_param_config(uart_port, &uart_config) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    fcntl(fileno(stdout), F_SETFL, O_NONBLOCK);
    fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);

    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .tx_buffer_size = 256,
        .rx_buffer_size = 256,
    };
    usb_serial_jtag_driver_install(&usb_serial_jtag_config);
    esp_vfs_usb_serial_jtag_use_driver();
    esp_vfs_dev_uart_register();
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

    /* Initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_length = 256,
            .max_cmdline_args = 8,
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);

    /* Set command maximum length */
    linenoiseSetMaxLineLen(console_config.max_cmdline_length);

    /* Don't return empty lines */
    linenoiseAllowEmpty(false);

#if CONFIG_STORE_HISTORY
    /* Load command history from filesystem */
    linenoiseHistoryLoad(HISTORY_PATH);
#endif
}


namespace esp_matter {

namespace data_model {

int create(uint8_t device_type_index)
{
    esp_err_t err = ESP_OK;
    esp_matter::node_t *node = node::get();
    esp_matter::endpoint_t *endpoint = NULL;
    switch(device_type_index) {
        case ESP_MATTER_ON_OFF_LIGHT: {
            esp_matter::endpoint::on_off_light::config_t light_config;
            endpoint = esp_matter::endpoint::on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DIMMABLE_LIGHT: {
            esp_matter::endpoint::dimmable_light::config_t dimmable_light_config;
            endpoint = esp_matter::endpoint::dimmable_light::create(node, &dimmable_light_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_COLOR_TEMP_LIGHT: {
            esp_matter::endpoint::color_temperature_light::config_t color_temperature_light_config;
            endpoint = esp_matter::endpoint::color_temperature_light::create(node, &color_temperature_light_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_EXTENDED_COLOR_LIGHT: {
            esp_matter::endpoint::extended_color_light::config_t extended_color_light_config;
            endpoint = esp_matter::endpoint::extended_color_light::create(node, &extended_color_light_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_ON_OFF_SWITCH: {
            esp_matter::endpoint::on_off_switch::config_t on_off_switch_config;
            endpoint = esp_matter::endpoint::on_off_switch::create(node, &on_off_switch_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DIMMER_SWITCH: {
            esp_matter::endpoint::dimmer_switch::config_t dimmer_switch_config;
            endpoint = esp_matter::endpoint::dimmer_switch::create(node, &dimmer_switch_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_COLOR_DIMMER_SWITCH: {
            esp_matter::endpoint::color_dimmer_switch::config_t color_dimmer_switch_config;
            endpoint = esp_matter::endpoint::color_dimmer_switch::create(node, &color_dimmer_switch_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_GENERIC_SWITCH: {
            esp_matter::endpoint::generic_switch::config_t generic_switch_config;
            endpoint = esp_matter::endpoint::generic_switch::create(node, &generic_switch_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_ON_OFF_PLUGIN_UNIT: {
            esp_matter::endpoint::on_off_plugin_unit::config_t on_off_plugin_unit_config;
            endpoint = esp_matter::endpoint::on_off_plugin_unit::create(node, &on_off_plugin_unit_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DIMMABLE_PLUGIN_UNIT: {
            esp_matter::endpoint::dimmable_plugin_unit::config_t dimmable_plugin_unit_config;
            endpoint = esp_matter::endpoint::dimmable_plugin_unit::create(node, &dimmable_plugin_unit_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_FAN: {
            esp_matter::endpoint::fan::config_t fan_config;
            #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
            static FanDelegateImpl fan_delegate(app_endpoint_id);
            fan_config.fan_control.delegate = &fan_delegate;
            #endif
            endpoint = esp_matter::endpoint::fan::create(node, &fan_config, ENDPOINT_FLAG_NONE, NULL);
            cluster_t *cluster = cluster::get(endpoint, chip::app::Clusters::FanControl::Id);
            cluster::fan_control::feature::multi_speed::config_t multispeed_config;
            cluster::fan_control::feature::rocking::config_t rock_config;
            cluster::fan_control::feature::airflow_direction::config_t airflow_direction_config;
            cluster::fan_control::feature::wind::config_t wind_config;

            multispeed_config.speed_max = SPEED_MAX_SET;
            cluster::fan_control::feature::multi_speed::add(cluster, &multispeed_config);

            rock_config.rock_setting = ROCK_SET;
            rock_config.rock_support = ROCK_SUPPORT_SET;
            cluster::fan_control::feature::rocking::add(cluster, &rock_config);

            airflow_direction_config.airflow_direction = AIRFLOW_DIRECTION_SET;
            cluster::fan_control::feature::airflow_direction::add(cluster, &airflow_direction_config);

            wind_config.wind_setting = WIND_SET;
            wind_config.wind_support = WIND_SUPPORT_SET;
            cluster::fan_control::feature::wind::add(cluster, &wind_config);

            cluster::fan_control::feature::step::add(cluster);

            app_endpoint_id = endpoint::get_id(endpoint);
            break;
        }
        case ESP_MATTER_THERMOSTAT: {
            esp_matter::endpoint::thermostat::config_t thermostat_config;
            endpoint = esp_matter::endpoint::thermostat::create(node, &thermostat_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_AGGREGATOR: {
            esp_matter::endpoint::aggregator::config_t aggregator_config;
	        endpoint = esp_matter::endpoint::aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_BRIDGED_NODE: {
            esp_matter::endpoint::bridged_node::config_t bridged_node_config;
            endpoint = esp_matter::endpoint::bridged_node::create(node, &bridged_node_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_CONTROL_BRIDGE: {
            esp_matter::endpoint::control_bridge::config_t control_bridge_config;
	        endpoint = esp_matter::endpoint::control_bridge::create(node, &control_bridge_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DOOR_LOCK: {
            esp_matter::endpoint::door_lock::config_t door_lock_config;
            endpoint = esp_matter::endpoint::door_lock::create(node, &door_lock_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_WINDOW_COVERING_DEVICE: {
            esp_matter::endpoint::window_covering_device::config_t window_covering_device_config;
            endpoint = esp_matter::endpoint::window_covering_device::create(node, &window_covering_device_config, ENDPOINT_FLAG_NONE, NULL);
            cluster_t *cluster = cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);
            cluster::window_covering::feature::lift::config_t lift;
            cluster::window_covering::feature::position_aware_lift::config_t position_aware_lift;
            cluster::window_covering::feature::absolute_position::config_t absolute_position;

            nullable<uint8_t> percentage = nullable<uint8_t>(0);
            nullable<uint16_t> percentage_100ths = nullable<uint16_t>(0);
            position_aware_lift.current_position_lift_percentage = percentage;
            position_aware_lift.target_position_lift_percent_100ths = percentage_100ths;
            position_aware_lift.current_position_lift_percent_100ths = percentage_100ths;

            cluster::window_covering::feature::lift::add(cluster, &lift);
            cluster::window_covering::feature::position_aware_lift::add(cluster, &position_aware_lift);
            cluster::window_covering::feature::absolute_position::add(cluster, &absolute_position);
            break;
        }
        case ESP_MATTER_TEMP_SENSOR: {
            esp_matter::endpoint::temperature_sensor::config_t temperature_sensor_config;
            endpoint = esp_matter::endpoint::temperature_sensor::create(node, &temperature_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_HUMIDITY_SENSOR: {
            esp_matter::endpoint::humidity_sensor::config_t humidity_sensor_config;
            endpoint = esp_matter::endpoint::humidity_sensor::create(node, &humidity_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_OCCUPANCY_SENSOR: {
            esp_matter::endpoint::occupancy_sensor::config_t occupancy_sensor_config;
            occupancy_sensor_config.occupancy_sensing.features = cluster::occupancy_sensing::feature::other::get_id();
            endpoint = esp_matter::endpoint::occupancy_sensor::create(node, &occupancy_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_CONTACT_SENSOR: {
            esp_matter::endpoint::contact_sensor::config_t contact_sensor_config;
            endpoint = esp_matter::endpoint::contact_sensor::create(node, &contact_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_LIGHT_SENSOR: {
            esp_matter::endpoint::light_sensor::config_t light_sensor_config;
            endpoint = esp_matter::endpoint::light_sensor::create(node, &light_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_PRESSURE_SENSOR: {
            esp_matter::endpoint::pressure_sensor::config_t pressure_sensor_config;
            endpoint = esp_matter::endpoint::pressure_sensor::create(node, &pressure_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_FLOW_SENSOR: {
            esp_matter::endpoint::flow_sensor::config_t flow_sensor_config;
            endpoint = esp_matter::endpoint::flow_sensor::create(node, &flow_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_PUMP: {
            esp_matter::endpoint::pump::config_t pump_config;
            pump_config.pump_configuration_and_control.features = cluster::pump_configuration_and_control::feature::constant_pressure::get_id();
            endpoint = esp_matter::endpoint::pump::create(node, &pump_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_MODE_SELECT_DEVICE: {
            esp_matter::endpoint::mode_select_device::config_t mode_select_device_config;
            endpoint = esp_matter::endpoint::mode_select_device::create(node, &mode_select_device_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_RAC: {
            esp_matter::endpoint::room_air_conditioner::config_t room_air_conditioner_config;
            endpoint = esp_matter::endpoint::room_air_conditioner::create(node, &room_air_conditioner_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_TEMP_CTRL_CABINET: {
            esp_matter::endpoint::temperature_controlled_cabinet::config_t temperature_controlled_cabinet_config;
            endpoint = esp_matter::endpoint::temperature_controlled_cabinet::create(node, &temperature_controlled_cabinet_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_REFRIGERATOR: {
            esp_matter::endpoint::refrigerator::config_t refrigerator_config;
            endpoint = esp_matter::endpoint::refrigerator::create(node, &refrigerator_config, ENDPOINT_FLAG_NONE, NULL);

            esp_matter::endpoint::temperature_controlled_cabinet::config_t temperature_controlled_cabinet_config;
            esp_matter::endpoint_t *tcc_endpoint = esp_matter::endpoint::temperature_controlled_cabinet::create(node, &temperature_controlled_cabinet_config, ENDPOINT_FLAG_NONE, NULL);

            if (!tcc_endpoint) {
                ESP_LOGE(TAG, "Matter create endpoint failed");
                return 1;
            }

            esp_matter::cluster_t *cluster = esp_matter::cluster::get(tcc_endpoint, chip::app::Clusters::TemperatureControl::Id);
            cluster::temperature_control::feature::temperature_number::config_t temperature_number_config;
            cluster::temperature_control::feature::temperature_number::add(cluster, &temperature_number_config);
            break;
        }
        case ESP_MATTER_OVEN: {
            esp_matter::endpoint::oven::config_t oven_config;
            endpoint = esp_matter::endpoint::oven::create(node, &oven_config, ENDPOINT_FLAG_NONE, NULL);

            esp_matter::endpoint::temperature_controlled_cabinet::config_t temperature_controlled_cabinet_config;
            esp_matter::endpoint_t *tcc_endpoint = esp_matter::endpoint::temperature_controlled_cabinet::create(node, &temperature_controlled_cabinet_config, ENDPOINT_FLAG_NONE, NULL);

            if (!tcc_endpoint) {
                ESP_LOGE(TAG, "Matter create endpoint failed");
                return 1;
            }

            esp_matter::cluster_t *cluster = esp_matter::cluster::get(tcc_endpoint, chip::app::Clusters::TemperatureControl::Id);
            cluster::temperature_control::feature::temperature_number::config_t temperature_number_config;
            cluster::temperature_control::feature::temperature_number::add(cluster, &temperature_number_config);
            break;
        }
        case ESP_MATTER_AIR_PURIFIER: {
            esp_matter::endpoint::air_purifier::config_t air_purifier_config;
            endpoint = esp_matter::endpoint::air_purifier::create(node, &air_purifier_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_AIR_QUALITY_SENSOR: {
            esp_matter::endpoint::air_quality_sensor::config_t air_quality_sensor_config;
            endpoint = esp_matter::endpoint::air_quality_sensor::create(node, &air_quality_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_ROBOTIC_VACUUM_CLEANER: {
            esp_matter::endpoint::robotic_vacuum_cleaner::config_t robotic_vacuum_cleaner_config;
            endpoint = esp_matter::endpoint::robotic_vacuum_cleaner::create(node, &robotic_vacuum_cleaner_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_LAUNDRY_WASHER: {
            esp_matter::endpoint::laundry_washer::config_t laundry_washer_config;
            endpoint = esp_matter::endpoint::laundry_washer::create(node, &laundry_washer_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DISH_WASHER: {
            esp_matter::endpoint::dish_washer::config_t dish_washer_config;
            endpoint = esp_matter::endpoint::dish_washer::create(node, &dish_washer_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_SMOKE_CO_ALARM: {
            esp_matter::endpoint::smoke_co_alarm::config_t smoke_co_alarm_config;
            endpoint = esp_matter::endpoint::smoke_co_alarm::create(node, &smoke_co_alarm_config, ENDPOINT_FLAG_NONE, NULL);

            esp_matter::endpoint::power_source_device::config_t power_source_config;
            esp_matter::endpoint_t *ps_endpoint = esp_matter::endpoint::power_source_device::create(node, &power_source_config, ENDPOINT_FLAG_NONE, NULL);

            if (!ps_endpoint) {
                ESP_LOGE(TAG, "Matter create endpoint failed");
                return 1;
            }
            break;
        }
        case ESP_MATTER_WATER_LEAK_DETECTOR: {
            esp_matter::endpoint::water_leak_detector::config_t water_leak_detector_config;
            endpoint = esp_matter::endpoint::water_leak_detector::create(node, &water_leak_detector_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_WATER_FREEZE_DETECTOR: {
            esp_matter::endpoint::water_freeze_detector::config_t water_freeze_detector_config;
            endpoint = esp_matter::endpoint::water_freeze_detector::create(node, &water_freeze_detector_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_POWER_SOURCE: {
            esp_matter::endpoint::power_source_device::config_t power_source_device_config;
            endpoint = esp_matter::endpoint::power_source_device::create(node, &power_source_device_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_RAIN_SENSOR: {
            esp_matter::endpoint::rain_sensor::config_t rain_sensor_config;
            endpoint = esp_matter::endpoint::rain_sensor::create(node, &rain_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_ELECTRICAL_SENSOR: {
            esp_matter::endpoint::electrical_sensor::config_t electrical_sensor_config;
            endpoint = esp_matter::endpoint::electrical_sensor::create(node, &electrical_sensor_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_COOKTOP: {
            esp_matter::endpoint::cooktop::config_t cooktop_config;
            endpoint = esp_matter::endpoint::cooktop::create(node, &cooktop_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_ENERGY_EVSE: {
            esp_matter::endpoint::energy_evse::config_t energy_evse_config;
            endpoint = esp_matter::endpoint::energy_evse::create(node, &energy_evse_config, ENDPOINT_FLAG_NONE, NULL);

            esp_matter::endpoint::power_source_device::config_t power_source_config;
            esp_matter::endpoint_t *ps_endpoint = esp_matter::endpoint::power_source_device::create(node, &power_source_config, ENDPOINT_FLAG_NONE, NULL);
            esp_matter::endpoint::electrical_sensor::config_t electrical_sensor_config;
            esp_matter::endpoint::electrical_sensor::add(ps_endpoint, &electrical_sensor_config);

            if (!ps_endpoint) {
                ESP_LOGE(TAG, "Matter create endpoint failed");
                return 1;
            }
            break;
        }
        case ESP_MATTER_MICROWAVE_OVEN: {
            esp_matter::endpoint::microwave_oven::config_t microwave_oven_config;
            endpoint = esp_matter::endpoint::microwave_oven::create(node, &microwave_oven_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_EXTRACTOR_HOOD: {
            esp_matter::endpoint::extractor_hood::config_t extractor_hood_config;
            endpoint = esp_matter::endpoint::extractor_hood::create(node, &extractor_hood_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_LAUNDRY_DRYER: {
            esp_matter::endpoint::laundry_dryer::config_t laundry_dryer_config;
            endpoint = esp_matter::endpoint::laundry_dryer::create(node, &laundry_dryer_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_WATER_VALVE: {
            esp_matter::endpoint::water_valve::config_t water_valve_config;
            endpoint = esp_matter::endpoint::water_valve::create(node, &water_valve_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_DEVICE_ENERGY_MANAGEMENT: {
            esp_matter::endpoint::device_energy_management::config_t device_energy_management_config;
            endpoint = esp_matter::endpoint::device_energy_management::create(node, &device_energy_management_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_PUMP_CONTROLLER: {
            esp_matter::endpoint::pump_controller::config_t pump_controller_config;
            endpoint = esp_matter::endpoint::pump_controller::create(node, &pump_controller_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
#ifdef CONFIG_OPENTHREAD_BORDER_ROUTER
        case ESP_MATTER_THREAD_BORDER_ROUTER: {
            static chip::KvsPersistentStorageDelegate tbr_storage_delegate;
            chip::DeviceLayer::PersistedStorage::KeyValueStoreManager & kvsManager = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr();
            tbr_storage_delegate.Init(&kvsManager);
            GenericOpenThreadBorderRouterDelegate *delegate = chip::Platform::New<GenericOpenThreadBorderRouterDelegate>(&tbr_storage_delegate);
            if (!delegate) {
                ESP_LOGE(TAG, "Failed to create thread_border_router delegate");
                return 1;
            }
            char threadBRName[] = "Espressif-ThreadBR";
            delegate->SetThreadBorderRouterName(chip::CharSpan(threadBRName));
            esp_matter::endpoint::thread_border_router::config_t tbr_config;
            tbr_config.thread_border_router_management.delegate = delegate;
            endpoint = esp_matter::endpoint::thread_border_router::create(node, &tbr_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
#endif
        case ESP_MATTER_MOUNTED_ON_OFF_CONTROL: {
            esp_matter::endpoint::mounted_on_off_control::config_t mounted_on_off_control_config;
            endpoint = esp_matter::endpoint::mounted_on_off_control::create(node, &mounted_on_off_control_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_MOUNTED_DIMMABLE_LOAD_CONTROL: {
            esp_matter::endpoint::mounted_dimmable_load_control::config_t mounted_dimmable_load_control_config;
            endpoint = esp_matter::endpoint::mounted_dimmable_load_control::create(node, &mounted_dimmable_load_control_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_WATER_HEATER: {
            esp_matter::endpoint::water_heater::config_t water_heater_config;
            endpoint = esp_matter::endpoint::water_heater::create(node, &water_heater_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_SOLAR_POWER: {
            esp_matter::endpoint::solar_power::config_t solar_power_config;
            endpoint = esp_matter::endpoint::solar_power::create(node, &solar_power_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_BATTERY_STORAGE: {
            esp_matter::endpoint::battery_storage::config_t battery_storage_config;
            endpoint = esp_matter::endpoint::battery_storage::create(node, &battery_storage_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        case ESP_MATTER_HEAT_PUMP: {
            esp_matter::endpoint::heat_pump::config_t heat_pump_config;
            endpoint = esp_matter::endpoint::heat_pump::create(node, &heat_pump_config, ENDPOINT_FLAG_NONE, NULL);
            break;
        }
        default: {
            ESP_LOGE(TAG, "Please input a valid device type");
            break;
        }
    }

    if (!endpoint) {
        ESP_LOGE(TAG, "Matter create endpoint failed");
        return 1;
    } else {
        ESP_LOGI(TAG, "%s created with endpoint_id %d", device_type_list[device_type_index - 1].device_name, endpoint::get_id(endpoint));
        if(esp_matter::nvs_helpers::get_device_type_from_nvs(&device_type_index) != ESP_OK) {
            err = esp_matter::nvs_helpers::set_device_type_in_nvs(device_type_index);
            if(err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to store device type in nvs");
    	        return 1;
            }
        }

        if(semaphoreHandle) {
            xSemaphoreGive(semaphoreHandle);
        }
    }

    return 0;
}

} /* namespace data_model */
} /* namespace esp_matter */

namespace example {
namespace console {

/** Arguments used by 'create' function */
 static struct {
     struct arg_str *device_type;
     struct arg_end *end;
 } create_device_args;

static int create(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &create_device_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, create_device_args.end, argv[0]);
        ESP_LOGI(TAG, "Please use: create --device_type=device_type where device type can be anyone from the below list:");
        for(int traverse=0; traverse < ESP_MATTER_DEVICE_TYPE_MAX - 1 ; traverse++) {
            ESP_LOGI("", "%s", device_type_list[traverse].device_name);
        }
        return 1;
    }

    uint8_t index = 0;
    std::string str = create_device_args.device_type->sval[0];
    for(int traverse=0; traverse < ESP_MATTER_DEVICE_TYPE_MAX - 1 ; traverse++) {
        if(device_type_list[traverse].device_name == str) {
            index = device_type_list[traverse].device_id;
       	    break;
        }
    }

    return esp_matter::data_model::create(index);
}

esp_err_t register_create_device_commands()
{
    create_device_args.device_type = arg_str0(NULL, "device_type", "<device_type>", "Device type name");
    create_device_args.end = arg_end(2);

    const esp_console_cmd_t create_cmd = {
        .command = "create",
        .help = "Create device type",
        .hint = NULL,
        .func = &create,
        .argtable = &create_device_args
    };

    return esp_console_cmd_register(&create_cmd);

}

void init(void)
{
#if CONFIG_STORE_HISTORY
    initialize_filesystem();
    ESP_LOGI(TAG, "Command history enabled");
#endif

    initialize_console();

    register_create_device_commands();

    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    const char* prompt = LOG_COLOR_I PROMPT_STR "> " LOG_RESET_COLOR;

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = PROMPT_STR "> ";
#endif //CONFIG_LOG_COLORS
    }


    /* Main loop */
    while(true) {
        /* Get a line using linenoise.
         * The line is returned when ENTER is pressed.
         */
        char* line = linenoise(prompt);


        if (line == NULL) { /* Break on EOF or error */
            continue;
        }
        /* Add the command to the history if not empty*/
        if (strlen(line) > 0) {
            linenoiseHistoryAdd(line);
#if CONFIG_STORE_HISTORY
            /* Save command history to filesystem */
            linenoiseHistorySave(HISTORY_PATH);
#endif
        }

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        } else if (err == ESP_OK && ret == ESP_OK) {
            break;
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }
}

void deinit(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
    esp_console_deinit();
}
} // namespace console
} // namespace example
