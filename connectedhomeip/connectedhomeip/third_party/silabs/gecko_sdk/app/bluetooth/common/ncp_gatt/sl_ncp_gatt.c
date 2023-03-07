/***************************************************************************//**
 * @file
 * @brief Composes the GATT database using the Dynamic GATT API.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app_assert.h"
#include "sl_bt_api.h"
#include "gatt_db.h"
#include "sl_ncp_gatt.h"

// UUID lengths.
#define UUID_16_LEN   2
#define UUID_128_LEN  16

// Bitmask indicating a 128-bit UUID handle.
#define UUID_128_HANDLE                           0x8000
// Check for 16-bit UUID value.
#define IS_UUID_16(uuid)                          (((uuid) & UUID_128_HANDLE) == 0)

// 16-bit UUIDs allocated for GATT Declarations.
#define PRIMARY_SERVICE_UUID                      0x2800
#define SECONDARY_SERVICE_UUID                    0x2801
#define INCLUDE_UUID                              0x2802
#define CHARACTERISTIC_UUID                       0x2803
#define CLIENT_CHARACTERISTIC_CONFIGURATION_UUID  0x2902

// GATT database datatypes.
#define GATTDB_DATATYPE_CONSTANT_VALUE            0x00
#define GATTDB_DATATYPE_FIXED_LENGTH_VALUE        0x01
#define GATTDB_DATATYPE_VARIABLE_LENGTH_VALUE     0x02
#define GATTDB_DATATYPE_USER_VALUE                0x07

// GATT database permission flags.
#define GATTDB_PERMISSION_READ                    0x0001
#define GATTDB_PERMISSION_WRITE                   0x0002
#define GATTDB_PERMISSION_ENCRYPTED_READ          0x0010
#define GATTDB_PERMISSION_AUTHENTICATED_READ      0x0020
#define GATTDB_PERMISSION_BONDED_READ             0x0040
#define GATTDB_PERMISSION_ENCRYPTED_WRITE         0x0100
#define GATTDB_PERMISSION_AUTHENTICATED_WRITE     0x0200
#define GATTDB_PERMISSION_BONDED_WRITE            0x0400
#define GATTDB_PERMISSION_DISCOVERABLE            0x0800
#define GATTDB_PERMISSION_ENCRYPTED_NOTIFY        0x1000
#define GATTDB_PERMISSION_AUTHENTICATED_NOTIFY    0x2000
#define GATTDB_PERMISSION_BONDED_NOTIFY           0x4000
#define GATTDB_PERMISSION_ADVERTISE               0x8000

// Flag indicating that the Client Characteristic Configuration descriptor
// should be created automatically.
#define GATTDB_AUTO_CCCD                          0x00

// Flag indicating that a service should not be advertised.
#define GATTDB_NON_ADVERTISED_SERVICE             0x00

// Structure for attribute values.
typedef struct {
  uint16_t max_len;
  uint16_t len;
  const uint8_t *data;
} attribute_value_t;

// Initialize GATT database dynamically.
static void dynamic_gattdb_init(void);

// Select the security related information from the permission property of an
// attribute and map it according to sl_bt_gattdb_security_requirements macros.
static uint16_t map_security(uint16_t permissions);

// Convert the datatype property of an attribute according to the
// sl_bt_gattdb_value_type_t type.
static sl_bt_gattdb_value_type_t map_value_type(uint8_t datatype);

// Convert the permissions property of an attribute according to the
// sl_bt_gattdb_descriptor_properties macros.
static uint16_t map_descriptor_property(uint16_t permissions);

// Get the value and its related length informations of an attribute.
static attribute_value_t get_attribute_value(const sli_bt_gattdb_attribute_t *attribute);

// Get the index of the first attrubute to be processed.
static uint16_t get_start_index(void);

// Bluetooth stack event handler.
void sl_ncp_gatt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      dynamic_gattdb_init();
      break;

    default:
      break;
  }
}

/**************************************************************************//**
 * Initialize GATT database dynamically.
 *****************************************************************************/
static void dynamic_gattdb_init(void)
{
  sl_status_t sc = SL_STATUS_OK;

  // Create a new dynamic GATT Database session.
  uint16_t gattdb_session; // Session handle.
  sc = sl_bt_gattdb_new_session(&gattdb_session);
  app_assert_status(sc);

  // The handle of the last added service.
  uint16_t current_service;
  // The handle of the last added characteristic.
  uint16_t current_characteristic;
  // The handle of the last added descriptor.
  uint16_t current_descriptor;
  // Start index of the attributes
  uint16_t i = get_start_index();

  for (; i < gattdb.attribute_num; i++) {
    if (IS_UUID_16(gattdb.attributes[i].uuid)) {
      switch (gattdb.uuid16[gattdb.attributes[i].uuid]) {
        case PRIMARY_SERVICE_UUID:
          // Add service.
          sc = sl_bt_gattdb_add_service(gattdb_session,
                                        sl_bt_gattdb_primary_service,
                                        gattdb.attributes[i].permissions & GATTDB_PERMISSION_ADVERTISE \
                                        ? SL_BT_GATTDB_ADVERTISED_SERVICE                              \
                                        : GATTDB_NON_ADVERTISED_SERVICE,
                                        gattdb.attributes[i].constdata->len,
                                        gattdb.attributes[i].constdata->data,
                                        &current_service);
          app_assert_status(sc);

          // Check the returned handle.
          app_assert(current_service == gattdb.attributes[i].handle,
                     "Handle mismatch when adding service %d.", i);

          // Start service.
          sc = sl_bt_gattdb_start_service(gattdb_session, current_service);
          app_assert_status(sc);
          break;

        case SECONDARY_SERVICE_UUID:
          // Add service.
          sc = sl_bt_gattdb_add_service(gattdb_session,
                                        sl_bt_gattdb_secondary_service,
                                        gattdb.attributes[i].permissions & GATTDB_PERMISSION_ADVERTISE \
                                        ? SL_BT_GATTDB_ADVERTISED_SERVICE                              \
                                        : GATTDB_NON_ADVERTISED_SERVICE,
                                        gattdb.attributes[i].constdata->len,
                                        gattdb.attributes[i].constdata->data,
                                        &current_service);
          app_assert_status(sc);

          // Check the returned handle.
          app_assert(current_service == gattdb.attributes[i].handle,
                     "Handle mismatch when adding service %d.", i);

          // Start service.
          sc = sl_bt_gattdb_start_service(gattdb_session, current_service);
          app_assert_status(sc);
          break;

        case INCLUDE_UUID: {
          uint16_t handle;
          sc = sl_bt_gattdb_add_included_service(gattdb_session,
                                                 current_service,
                                                 gattdb.attributes[i].service_include.start,
                                                 &handle);
          app_assert_status(sc);

          // Check the returned handle.
          app_assert(handle == gattdb.attributes[i].handle,
                     "Handle mismatch when including service. Returned handle: %d, expected handle: %d.",
                     handle,
                     gattdb.attributes[i].handle);
          break;
        }

        case CHARACTERISTIC_UUID: {
          // Preparing characteristic value.
          attribute_value_t value;
          value = get_attribute_value(&gattdb.attributes[i + 1]);

          // Add characteristic.
          if (IS_UUID_16(gattdb.attributes[i].characteristic.char_uuid)) {
            // 16-bit uuid
            sl_bt_uuid_16_t uuid;
            memcpy(&uuid.data, &gattdb.uuid16[gattdb.attributes[i].characteristic.char_uuid], UUID_16_LEN);
            sc = sl_bt_gattdb_add_uuid16_characteristic(gattdb_session,
                                                        current_service,
                                                        (uint16_t)gattdb.attributes[i].characteristic.properties,
                                                        map_security(gattdb.attributes[i + 1].permissions),
                                                        GATTDB_AUTO_CCCD,
                                                        uuid,
                                                        map_value_type(gattdb.attributes[i + 1].datatype),
                                                        value.max_len,
                                                        value.len,
                                                        value.data,
                                                        &current_characteristic);
            app_assert_status(sc);
          } else {
            // 128-bit uuid
            uuid_128 uuid;
            memcpy(&uuid.data, &gattdb.uuid128[UUID_128_LEN * (gattdb.attributes[i].characteristic.char_uuid & ~(UUID_128_HANDLE))], UUID_128_LEN);
            sc = sl_bt_gattdb_add_uuid128_characteristic(gattdb_session,
                                                         current_service,
                                                         (uint16_t)gattdb.attributes[i].characteristic.properties,
                                                         map_security(gattdb.attributes[i + 1].permissions),
                                                         GATTDB_AUTO_CCCD,
                                                         uuid,
                                                         map_value_type(gattdb.attributes[i + 1].datatype),
                                                         value.max_len,
                                                         value.len,
                                                         value.data,
                                                         &current_characteristic);
            app_assert_status(sc);
          }

          // Check the returned handle.
          app_assert(current_characteristic == gattdb.attributes[i + 1].handle,
                     "Handle mismatch when adding characteristic %d.", i);

          // Start characteristic.
          sc = sl_bt_gattdb_start_characteristic(gattdb_session, current_characteristic);
          app_assert_status(sc);

          i++; // Skip the next attribute since it is processed already.
          break;
        }

        case CLIENT_CHARACTERISTIC_CONFIGURATION_UUID:
          // Client Characteristic Configuration descriptors are created
          // automatically by the stack.
          break;

        default: {
          // Descriptor with 16-bit uuid.
          // Preparing descriptor value.
          attribute_value_t value;
          value = get_attribute_value(&gattdb.attributes[i]);

          // Add descriptor.
          sl_bt_uuid_16_t uuid;
          memcpy(&uuid.data, &gattdb.uuid16[gattdb.attributes[i].uuid], UUID_16_LEN);
          sc = sl_bt_gattdb_add_uuid16_descriptor(gattdb_session,
                                                  current_characteristic,
                                                  map_descriptor_property(gattdb.attributes[i].permissions),
                                                  map_security(gattdb.attributes[i].permissions),
                                                  uuid,
                                                  map_value_type(gattdb.attributes[i].datatype),
                                                  value.max_len,
                                                  value.len,
                                                  value.data,
                                                  &current_descriptor);
          app_assert_status(sc);

          // Check the returned handle.
          app_assert(current_descriptor == gattdb.attributes[i].handle,
                     "Handle mismatch when adding descriptor %d.", i);
          break;
        }
      }
    } else {
      // Descriptor with 128-bit uuid.
      // Preparing descriptor value.
      attribute_value_t value;
      value = get_attribute_value(&gattdb.attributes[i]);

      // Add descriptor.
      uuid_128 uuid;
      memcpy(&uuid.data, &gattdb.uuid128[UUID_128_LEN * (gattdb.attributes[i].uuid & ~(UUID_128_HANDLE))], UUID_128_LEN);
      sc = sl_bt_gattdb_add_uuid128_descriptor(gattdb_session,
                                               current_characteristic,
                                               map_descriptor_property(gattdb.attributes[i].permissions),
                                               map_security(gattdb.attributes[i].permissions),
                                               uuid,
                                               map_value_type(gattdb.attributes[i].datatype),
                                               value.max_len,
                                               value.len,
                                               value.data,
                                               &current_descriptor);
      app_assert_status(sc);

      // Check the returned handle.
      app_assert(current_descriptor == gattdb.attributes[i].handle,
                 "Handle mismatch when adding descriptor %d.", i);
    }
  }

  // Commit changes.
  sc = sl_bt_gattdb_commit(gattdb_session);
  app_assert_status(sc);
}

/***************************************************************************//**
 * Select the security related information from the permission property of an
 * attribute and map it according to sl_bt_gattdb_security_requirements macros.
 *
 * @param[in] permissions. The permissions property of an attribute provided by
 *   gatt_db.c.
 * @return The converted security flags ready to pass to the api.
 ******************************************************************************/
static uint16_t map_security(uint16_t permissions)
{
  uint16_t security = 0x0000;

  security |= (permissions & GATTDB_PERMISSION_ENCRYPTED_READ) ? SL_BT_GATTDB_ENCRYPTED_READ : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_AUTHENTICATED_READ) ? SL_BT_GATTDB_AUTHENTICATED_READ : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_BONDED_READ) ? SL_BT_GATTDB_BONDED_READ : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_ENCRYPTED_WRITE) ? SL_BT_GATTDB_ENCRYPTED_WRITE : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_AUTHENTICATED_WRITE) ? SL_BT_GATTDB_AUTHENTICATED_WRITE : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_BONDED_WRITE) ? SL_BT_GATTDB_BONDED_WRITE : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_ENCRYPTED_NOTIFY) ? SL_BT_GATTDB_ENCRYPTED_NOTIFY : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_AUTHENTICATED_NOTIFY) ? SL_BT_GATTDB_AUTHENTICATED_NOTIFY : 0x0000;
  security |= (permissions & GATTDB_PERMISSION_BONDED_NOTIFY) ? SL_BT_GATTDB_BONDED_NOTIFY : 0x0000;

  return security;
}

/***************************************************************************//**
 * Convert the datatype property of an attribute according to the
 * sl_bt_gattdb_value_type_t type.
 *
 * @param[in] datatype. The datatype property of an attribute provided by
 *   gatt_db.c.
 * @return The converted datatype ready to pass to the api.
 ******************************************************************************/
static sl_bt_gattdb_value_type_t map_value_type(uint8_t datatype)
{
  switch (datatype) {
    case GATTDB_DATATYPE_FIXED_LENGTH_VALUE:
      return sl_bt_gattdb_fixed_length_value;

    case GATTDB_DATATYPE_VARIABLE_LENGTH_VALUE:
      return sl_bt_gattdb_variable_length_value;

    case GATTDB_DATATYPE_USER_VALUE:
      return sl_bt_gattdb_user_managed_value;

    default:
      // For value types not being supported by dynamic GATT.
      return sl_bt_gattdb_fixed_length_value;
  }
}

/***************************************************************************//**
 * Convert the permissions property of an attribute according to the
 * sl_bt_gattdb_descriptor_properties macros.
 *
 * @param[in] permissions. The permissions property of an attribute provided by
 *   gatt_db.c.
 * @return The converted permissions ready to pass to the api.
 ******************************************************************************/
static uint16_t map_descriptor_property(uint16_t permissions)
{
  uint16_t property = 0x0000;

  property |= (permissions & GATTDB_PERMISSION_READ) ? SL_BT_GATTDB_DESCRIPTOR_READ : 0x0000;
  property |= (permissions & GATTDB_PERMISSION_WRITE) ? SL_BT_GATTDB_DESCRIPTOR_WRITE : 0x0000;
  property |= (permissions & GATTDB_PERMISSION_DISCOVERABLE) ? 0x0000 : SL_BT_GATTDB_DESCRIPTOR_LOCAL_ONLY;

  return property;
}

/***************************************************************************//**
 * Get the value and its related length informations of an attribute.
 *
 * @param[in] attribute. An attribute from provided by gatt_db.c.
 * @return The maximum length, length and the actual data of the attribute.
 *   @ref attribute_value_t.
 ******************************************************************************/
static attribute_value_t get_attribute_value(const sli_bt_gattdb_attribute_t *attribute)
{
  attribute_value_t value;

  switch (attribute->datatype) {
    case GATTDB_DATATYPE_CONSTANT_VALUE:
      value.max_len = attribute->constdata->len;
      value.len = attribute->constdata->len;
      value.data = attribute->constdata->data;
      break;

    case GATTDB_DATATYPE_FIXED_LENGTH_VALUE:
      value.max_len = attribute->dynamicdata->max_len;
      value.len = attribute->dynamicdata->max_len;
      value.data = attribute->dynamicdata->data;
      break;

    case GATTDB_DATATYPE_VARIABLE_LENGTH_VALUE:
      value.max_len = attribute->dynamicdata->max_len;
      value.len = attribute->dynamicdata->len;
      value.data = attribute->dynamicdata->data;
      break;

    default:
      // User managed values and other symbols.
      value.max_len = 0;
      value.len = 0;
      value.data = NULL;
      break;
  }

  return value;
}

/***************************************************************************//**
 * Get the index of the first attrubute to be processed.
 ******************************************************************************/
static uint16_t get_start_index(void)
{
  uint16_t idx = 0;
  // Check for Generic Attribute UUID (0x1801)
  if ((gattdb.attributes[idx].constdata->len == 2)
      && (gattdb.attributes[idx].constdata->data[0] == 0x01)
      && (gattdb.attributes[idx].constdata->data[1] == 0x18)) {
    for (idx = 1; idx < gattdb.attribute_num; idx++) {
      // Find the next service declaration
      if (IS_UUID_16(gattdb.attributes[idx].uuid)
          && ((gattdb.uuid16[gattdb.attributes[idx].uuid] == PRIMARY_SERVICE_UUID)
              || (gattdb.uuid16[gattdb.attributes[idx].uuid] == SECONDARY_SERVICE_UUID))) {
        break;
      }
    }
  }
  return idx;
}
