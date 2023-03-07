/***************************************************************************//**
 * @file
 * @brief Sensor client module
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_dcd.h"

#include "app_assert.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_sensor_client_config.h"
#include "sl_btmesh_sensor_client.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup SensorClient
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SensorClient
 * @{
 ******************************************************************************/

/// Number of supported properties
#define PROPERTIES_NUMBER       3
/// Parameter ignored for publishing
#define IGNORED                 0
/// No flags used for message
#define NO_FLAGS                0
/// The size of descriptor is 8 bytes
#define SIZE_OF_DESCRIPTOR      8
/// Size of property ID in bytes
#define PROPERTY_ID_SIZE        2
/// Size of property header in bytes
#define PROPERTY_HEADER_SIZE    3
/// Sensor index value for not registered devices
#define SENSOR_INDEX_NOT_FOUND  0xFF

// Address zero is used in sensor client commands to indicate that
// the message should be published
static const uint16_t PUBLISH_ADDRESS = 0x0000;

typedef struct {
  uint16_t address_table[SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL];
  uint8_t count;
} mesh_registered_device_properties_address_t;

static bool mesh_address_already_exists(mesh_registered_device_properties_address_t* property,
                                        uint16_t address);
static uint8_t mesh_get_sensor_index(mesh_registered_device_properties_address_t* property,
                                     uint16_t address);
static void mesh_sensor_client_init(void);

static mesh_registered_device_properties_address_t registered_devices = {
  .count = 0,
};

static mesh_device_properties_t registering_property = DEVICE_PROPERTY_INVALID;

// -----------------------------------------------------------------------------
// Sensor Model Callbacks

SL_WEAK void sl_btmesh_sensor_client_on_discovery_started(uint16_t property_id)
{
  (void)property_id;
}

SL_WEAK void sl_btmesh_sensor_client_on_new_device_found(uint16_t property_id,
                                                         uint16_t address)
{
  (void)property_id;
  (void)address;
}

SL_WEAK void sl_btmesh_sensor_client_on_new_temperature_data(uint8_t sensor_idx,
                                                             uint16_t address,
                                                             sl_btmesh_sensor_client_data_status_t status,
                                                             temperature_8_t temperature)
{
  (void) sensor_idx;
  (void) address;
  (void) status;
  (void) temperature;
}

SL_WEAK void sl_btmesh_sensor_client_on_new_people_count_data(uint8_t sensor_idx,
                                                              uint16_t address,
                                                              sl_btmesh_sensor_client_data_status_t status,
                                                              count16_t people_count)
{
  (void) sensor_idx;
  (void) address;
  (void) status;
  (void) people_count;
}

SL_WEAK void sl_btmesh_sensor_client_on_new_illuminance_data(uint8_t sensor_idx,
                                                             uint16_t address,
                                                             sl_btmesh_sensor_client_data_status_t status,
                                                             illuminance_t illuminance)
{
  (void) sensor_idx;
  (void) address;
  (void) status;
  (void) illuminance;
}

/*******************************************************************************
 * Publishing of sensor client get descriptor request for currently displayed
 * property id. It also resets the registered devices counter.
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_update_registered_devices(mesh_device_properties_t property)
{
  sl_status_t sc;
  registered_devices.count = 0;
  memset(registered_devices.address_table,
         0,
         sizeof(registered_devices.address_table));
  registering_property = property;

  sl_btmesh_sensor_client_on_discovery_started(property);

  sc = sl_btmesh_sensor_client_get_descriptor(PUBLISH_ADDRESS,
                                              BTMESH_SENSOR_CLIENT_MAIN,
                                              IGNORED,
                                              NO_FLAGS,
                                              property);
  if (SL_STATUS_OK == sc) {
    log_info(SL_BTMESH_SENSOR_CLIENT_LOGGING_START_REGISTERING_DEVICES_CFG_VAL, property);
  } else {
    log_btmesh_status_f(sc,
                        SL_BTMESH_SENSOR_CLIENT_LOGGING_REGISTERING_DEVICES_FAILED_CFG_VAL,
                        property);
  }
  return sc;
}

/***************************************************************************//**
 * Handling of sensor client descriptor status event.
 *
 * @param[in] evt  Pointer to sensor client descriptor status event.
 ******************************************************************************/
static void handle_sensor_client_descriptor_status(
  sl_btmesh_evt_sensor_client_descriptor_status_t *evt)
{
  sensor_descriptor_t descriptor;
  if (evt->descriptors.len >= SIZE_OF_DESCRIPTOR) {
    mesh_lib_sensor_descriptors_from_buf(&descriptor,
                                         evt->descriptors.data,
                                         SIZE_OF_DESCRIPTOR);
    uint8_t number_of_devices = registered_devices.count;
    if (descriptor.property_id == registering_property
        && number_of_devices < SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL
        && !mesh_address_already_exists(&registered_devices,
                                        evt->server_address)) {
      registered_devices.address_table[number_of_devices] = evt->server_address;
      registered_devices.count = number_of_devices + 1;
      sl_btmesh_sensor_client_on_new_device_found(descriptor.property_id,
                                                  evt->server_address);
    }
  }
}

/*******************************************************************************
 * Publishing of sensor client get request for currently displayed property id.
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_sensor_data(mesh_device_properties_t property)
{
  sl_status_t sc;

  sc = sl_btmesh_sensor_client_get(PUBLISH_ADDRESS,
                                   BTMESH_SENSOR_CLIENT_MAIN,
                                   IGNORED,
                                   NO_FLAGS,
                                   property);

  if (SL_STATUS_OK == sc) {
    log_info(SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_CFG_VAL, property);
  } else {
    log_btmesh_status_f(sc,
                        SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_FAIL_CFG_VAL,
                        property);
  }
  return sc;
}

/***************************************************************************//**
 * Handling of sensor client status event.
 *
 * @param[in] evt  Pointer to sensor client status event.
 ******************************************************************************/
static void handle_sensor_client_status(sl_btmesh_evt_sensor_client_status_t *evt)
{
  uint8_t *sensor_data = evt->sensor_data.data;
  uint8_t data_len = evt->sensor_data.len;
  uint8_t pos = 0;
  while (pos < data_len) {
    if (data_len - pos > PROPERTY_ID_SIZE) {
      mesh_device_properties_t property_id = (mesh_device_properties_t)(sensor_data[pos]
                                                                        + (sensor_data[pos + 1] << 8));
      uint8_t property_len = sensor_data[pos + PROPERTY_ID_SIZE];
      uint8_t *property_data = NULL;

      if (mesh_address_already_exists(&registered_devices, evt->server_address)) {
        sl_btmesh_sensor_client_data_status_t status;
        uint16_t address;
        uint8_t sensor_idx;

        if (property_len && (data_len - pos > PROPERTY_HEADER_SIZE)) {
          property_data = &sensor_data[pos + PROPERTY_HEADER_SIZE];
        }

        address = evt->server_address;
        sensor_idx = mesh_get_sensor_index(&registered_devices, address);
        status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;

        switch (property_id) {
          case PEOPLE_COUNT:
          {
            count16_t people_count = SL_BTMESH_SENSOR_CLIENT_PEOPLE_COUNT_UNKNOWN;

            if (property_len == 2) {
              mesh_device_property_t new_property = mesh_sensor_data_from_buf(PEOPLE_COUNT,
                                                                              property_data);
              people_count = new_property.count16;

              if (people_count == SL_BTMESH_SENSOR_CLIENT_PEOPLE_COUNT_UNKNOWN) {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN;
              } else {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_VALID;
              }
            } else {
              status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;
            }

            sl_btmesh_sensor_client_on_new_people_count_data(sensor_idx,
                                                             address,
                                                             status,
                                                             people_count);
            break;
          }

          case PRESENT_AMBIENT_TEMPERATURE:
          {
            temperature_8_t temperature = SL_BTMESH_SENSOR_CLIENT_TEMPERATURE_UNKNOWN;

            if (property_len == 1) {
              mesh_device_property_t new_property = mesh_sensor_data_from_buf(PRESENT_AMBIENT_TEMPERATURE,
                                                                              property_data);
              temperature = new_property.temperature_8;

              if (temperature == SL_BTMESH_SENSOR_CLIENT_TEMPERATURE_UNKNOWN) {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN;
              } else {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_VALID;
              }
            } else {
              status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;
            }

            sl_btmesh_sensor_client_on_new_temperature_data(sensor_idx,
                                                            address,
                                                            status,
                                                            temperature);
            break;
          }

          case PRESENT_AMBIENT_LIGHT_LEVEL:
          {
            illuminance_t illuminance = SL_BTMESH_SENSOR_CLIENT_ILLUMINANCE_UNKNOWN;

            if (property_len == 3) {
              mesh_device_property_t new_property = mesh_sensor_data_from_buf(PRESENT_AMBIENT_LIGHT_LEVEL,
                                                                              property_data);
              illuminance = new_property.illuminance;

              if (illuminance == SL_BTMESH_SENSOR_CLIENT_ILLUMINANCE_UNKNOWN) {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN;
              } else {
                status = SL_BTMESH_SENSOR_CLIENT_DATA_VALID;
              }
            } else {
              status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;
            }

            sl_btmesh_sensor_client_on_new_illuminance_data(sensor_idx,
                                                            address,
                                                            status,
                                                            illuminance);
            break;
          }

          default:
            log(SL_BTMESH_SENSOR_CLIENT_LOGGING_UNSUPPORTED_PROPERTY_CFG_VAL, property_id);
            break;
        }
      }
      pos += PROPERTY_HEADER_SIZE + property_len;
    } else {
      pos = data_len;
    }
  }
}

/*******************************************************************************
 * Handling of mesh sensor client events.
 * It handles:
 *  - sensor_client_descriptor_status
 *  - sensor_client_status
 *
 * @param[in] evt  Pointer to incoming sensor server event.
 ******************************************************************************/
static void handle_sensor_client_events(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_sensor_client_descriptor_status_id:
      handle_sensor_client_descriptor_status(
        &(evt->data.evt_sensor_client_descriptor_status));
      break;

    case sl_btmesh_evt_sensor_client_status_id:
      handle_sensor_client_status(
        &(evt->data.evt_sensor_client_status));
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Handle Sensor Client events.
 ******************************************************************************/
void sl_btmesh_handle_sensor_client_on_event(sl_btmesh_msg_t *evt)
{
  if (NULL == evt) {
    return;
  }

  // Handle events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        mesh_sensor_client_init();
      }
      break;

    case sl_btmesh_evt_node_provisioned_id:
      mesh_sensor_client_init();
      break;

    case sl_btmesh_evt_sensor_client_descriptor_status_id:
    case sl_btmesh_evt_sensor_client_status_id:
      handle_sensor_client_events(evt);
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Check if the mesh address already exists or not.
 *
 * @param[in] property Pointer to registered devices' properties
 * @param[in] address  Mesh address to check
 *
 * @return             true:  The address exists
 *                     false: The address doesn't exist
 ******************************************************************************/
static bool mesh_address_already_exists(mesh_registered_device_properties_address_t *property,
                                        uint16_t address)
{
  bool address_exists = false;
  if (property != NULL) {
    for (int i = 0; i < SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL; i++) {
      if (address == property->address_table[i]) {
        address_exists = true;
        break;
      }
    }
  }
  return address_exists;
}

/***************************************************************************//**
 * Gets the sensor index.
 *
 * @param[in] property Pointer to registered devices' properties
 * @param[in] address  Mesh address of the sensor
 *
 * @return             Index of the sensor
 ******************************************************************************/
static uint8_t mesh_get_sensor_index(mesh_registered_device_properties_address_t *property,
                                     uint16_t address)
{
  uint8_t sensor_index = SENSOR_INDEX_NOT_FOUND;
  if (property != NULL) {
    for (int i = 0; i < SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL; i++) {
      if (address == property->address_table[i]) {
        sensor_index = i;
        break;
      }
    }
  }
  return sensor_index;
}

/***************************************************************************//**
 * Initializes sensor client component
 ******************************************************************************/
static void mesh_sensor_client_init(void)
{
  sl_status_t sc = sl_btmesh_sensor_client_init();

  app_assert_status_f(sc, "Failed to initialize sensor client\n");
}

/** @} (end addtogroup SensorClient) */
