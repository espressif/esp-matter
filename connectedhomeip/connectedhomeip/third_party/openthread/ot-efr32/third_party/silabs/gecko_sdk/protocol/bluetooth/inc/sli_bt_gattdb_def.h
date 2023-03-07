/***************************************************************************//**
 * @brief Bluetooth GATT database structure definition
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SLI_BT_GATT_DEF_H
#define SLI_BT_GATT_DEF_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint16_t len;  //size of buffer
  uint8_t data[];
} sli_bt_gattdb_value_t;

typedef struct {
  uint8_t properties;   //Characteristic properties, same as in characteristic descriptor gatt_char_prop
  uint16_t max_len;  //Maximum length of data in buffer
  uint16_t len;  //current size of data in buffer
  uint8_t data[];  //size is max_len
} sli_bt_gattdb_attribute_chrvalue_t;

typedef struct {
  uint8_t flags;   //client characteristic flags allowed 1-notification, 2-indication)
  uint8_t clientconfig_index;   // index to client config.
} sli_bt_gattdb_attribute_config_t;

typedef struct {
  uint8_t properties;   //Characteristic properties, same as in characteristic descriptor gatt_char_prop
  uint16_t char_uuid;   // characteristic UUID handle
} sli_bt_gattdb_attribute_characteristic_t;

typedef struct {
  uint16_t start;    //Included Service Attribute Handle
  uint16_t end;    //End Group Handle
} sli_bt_gattdb_attribute_service_include_t;

typedef struct {
  uint16_t handle;
  uint16_t uuid;
  uint16_t permissions;   //gatt_attribute_permission
  uint16_t caps;   // Capability bit flags
  uint8_t datatype;   //Just use uint8_t Do not use enum type, may cause compatibility troubles
  uint8_t state;
  union {
    const sli_bt_gattdb_value_t *constdata;     //generic constant data
    sli_bt_gattdb_attribute_chrvalue_t *dynamicdata;     //Modifiable data
    sli_bt_gattdb_attribute_config_t configdata;
    sli_bt_gattdb_attribute_characteristic_t characteristic;
    sli_bt_gattdb_attribute_service_include_t service_include;
  };
} sli_bt_gattdb_attribute_t;

struct sli_bt_gattdb_s {
  const sli_bt_gattdb_attribute_t  *attributes;
  uint16_t                          attribute_table_size;
  uint16_t                          attribute_num;
  const uint16_t                   *uuid16;
  uint16_t                          uuid16_table_size;
  uint16_t                          uuid16_num;
  const uint8_t                    *uuid128;
  uint16_t                          uuid128_table_size;
  uint16_t                          uuid128_num;
  uint8_t                           num_ccfg;
  uint16_t                          caps_mask;
  uint16_t                          enabled_caps;
};

typedef struct sli_bt_gattdb_s sli_bt_gattdb_t;

extern const sli_bt_gattdb_t *static_gattdb;

/**
 * @addtogroup dynamic_gatt_config
 *
 * Dynamic GATT database configuration.
 */

/**
 * Flag indicating GATT caching should be enabled. When enabled, a Generic
 * Attribute Profile Service will be created in database if one doesn't exist.
 **/
#define SLI_BT_GATTDB_CONFIG_FLAG_ENABLE_GATT_CACHING    (0x01)

/**
 * Flag indicating the static database should be included if one exists.
 */
#define SLI_BT_GATTDB_CONFIG_FLAG_INCLUDE_STATIC_DB      (0X02)

typedef struct {
  uint32_t flags;
} sli_bt_gattdb_config_t;

/** @} (end addtogroup dynamic_gatt_config) */

#endif
