 /***************************************************************************//**
 * @file CC_Configuration.h
 * @brief CC_Configuration.h
 * @copyright 2020 Silicon Laboratories Inc.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef ZAF_COMMANDCLASSES_CONFIGURATION_CC_CONFIGURATION_H_
#define ZAF_COMMANDCLASSES_CONFIGURATION_CC_CONFIGURATION_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ZAF_types.h>
#include <ZW_classcmd.h>
#include <zpal_nvm.h>
#include "CC_Configuration_Configuration.h"

/**
 * Defines the size options of a value.
 */
typedef enum
{
  CC_CONFIG_PARAMETER_SIZE_8_BIT            = 1,   //!< CC_CONFIG_PARAMETER_SIZE_8_BIT
  CC_CONFIG_PARAMETER_SIZE_16_BIT           = 2,   //!< CC_CONFIG_PARAMETER_SIZE_16_BIT
  CC_CONFIG_PARAMETER_SIZE_32_BIT           = 4,   //!< CC_CONFIG_PARAMETER_SIZE_32_BIT
  CC_CONFIG_PARAMETER_SIZE_NOT_SPECIFIED    = 0xFF,//!< CC_CONFIG_PARAMETER_SIZE_NOT_SPECIFIED
}
cc_config_parameter_size_t;

/**
 * Defines the format options of a value.
 */
typedef enum
{
  CC_CONFIG_PARAMETER_FORMAT_SIGNED_INTEGER   = CONFIGURATION_PROPERTIES_REPORT_FORMAT_SIGNED_INTEGER_V3,  //!< CC_CONFIG_PARAMETER_FORMAT_SIGNED_INTEGER
  CC_CONFIG_PARAMETER_FORMAT_UNSIGNED_INTEGER = CONFIGURATION_PROPERTIES_REPORT_FORMAT_UNSIGNED_INTEGER_V3,//!< CC_CONFIG_PARAMETER_FORMAT_UNSIGNED_INTEGER
  CC_CONFIG_PARAMETER_FORMAT_ENUMERATED       = CONFIGURATION_PROPERTIES_REPORT_FORMAT_ENUMERATED_V3,      //!< CC_CONFIG_PARAMETER_FORMAT_ENUMERATED
  CC_CONFIG_PARAMETER_FORMAT_BIT_FIELD        = CONFIGURATION_PROPERTIES_REPORT_FORMAT_BIT_FIELD_V3,       //!< CC_CONFIG_PARAMETER_FORMAT_BIT_FIELD
}
cc_config_parameter_format_t;

/**
 * Defines the different types that a value can take, e.g. uint8_t, int16_t, etc.
 */
typedef union
{
  // Unsigned integers
  uint32_t as_uint32;
  uint16_t as_uint16;
  uint8_t  as_uint8;
  // Signed integers
  int32_t  as_int32;
  int16_t  as_int16;
  int8_t   as_int8;
  // Array representation
  uint8_t as_uint8_array[4];
}
cc_config_parameter_value_t;

typedef struct _cc_config_parameter_attributes_t{
  char const * const name;                           ///< Human readable name
  char const * const info;                           ///< Human readable info
  const cc_config_parameter_value_t min_value;       ///< Minimum value
  const cc_config_parameter_value_t max_value;       ///< Maximum value
  cc_config_parameter_size_t  size;                  ///< Size of the parameter in bytes
  const cc_config_parameter_format_t format;         ///< Type of the parameter, e.g.: signed, unsigned
  const cc_config_parameter_value_t  default_value;  ///< Default data
  const struct {
    bool altering_capabilities : 1;          ///< 
    bool read_only             : 1;          ///< 
    bool advanced              : 1;          ///< 
  } flags;
} cc_config_parameter_attributes_t;

typedef struct cc_config_parameter_buffer_t cc_config_parameter_buffer_t;

typedef struct {
  const uint16_t number;                       ///< Unique id number of the parameter
  uint16_t next_number;                        ///< Next parameter number
  const zpal_nvm_object_key_t file_id;              ///< ID of the configuration file
  cc_config_parameter_attributes_t attributes; ///< Attributes of the parameters
  bool (*migration_handler)(cc_config_parameter_buffer_t* parameter_buffer); ///< Function pointer to handle the migration between versions
} cc_config_parameter_metadata_t;

struct cc_config_parameter_buffer_t {
  const cc_config_parameter_metadata_t* metadata;
  cc_config_parameter_value_t     data_buffer;
};

/**
 * Return value of the configuration set process
 */
typedef enum
{
    CC_CONFIG_RETURN_CODE_NOT_SUPPORTED,
    CC_CONFIG_RETURN_CODE_IO_FAIL,
    CC_CONFIG_RETURN_CODE_OK
} 
cc_config_configuration_set_return_value;

/**
 * Defines all data related to the Configuration CC.
 */
typedef struct
{
  uint16_t numberOfParameters;
  const cc_config_parameter_metadata_t* parameters;
} cc_configuration_t;

/**
 * 
 * Holds a write and a read interface defined by the user. 
 */ 
typedef struct {
  bool (*write_handler)(zpal_nvm_object_key_t file_id, uint8_t const* data, size_t size); ///< Write function which writes data with a specified size to non volatile memory
  bool (*read_handler)(zpal_nvm_object_key_t file_id, uint8_t *data, size_t size);        ///< Read function which reads data with specified size from non volatile memory
} cc_configuration_io_interface_t;

/**
 * Defines a handle type for the Configuration CC.
 */
typedef void * cc_configuration_handle_t;

/**
 * Sets a configuration parameter with the given number to the given value.
 * * Upon invocation CC_Configuration_Set_handler will be called given that the number and value is
 * valid.
 * @param[in] handle CC Configuration handle.
 * @param[in] number The number of the parameter to be set.
 * @param[in] value The value that the parameter must be set to.
 * @return Returns true if the value was set, false otherwise.
 */
bool CC_Configuration_SetValue(cc_configuration_handle_t handle,
                               uint16_t number,
                               cc_config_parameter_value_t value);

/**
 * Initialize the configuration Command Class
 *
 * @param[in] default_configuration pointer to a configuration structure which holds the factory default values
 * @param[in] interface pointer which holds the read and write functions to manipulate non volatile memory
 * @return Returns true if successfuly inited the command class, false otherwise.
 */
bool
cc_configuration_init(cc_configuration_t const* default_configuration, cc_configuration_io_interface_t const* interface );

/**
 * Initialize the configuration Command Class
 *
 * @param[in] parameter_number number of the parameter.
 * @param[out] parameter_buffer pointer to a parameter buffer which will holds the metadata and current value of a parameter
 * @return Returns true if successfuly found and filled parameter buffer, false otherwise
 */
bool
cc_configuration_get(uint16_t parameter_number, cc_config_parameter_buffer_t* parameter_buffer);

/**
 * Sets new interface set to manipulate non volatile memory to store and read configuration values
 *
 * @param[in]  interface pointer to the new interface set
 */
void
cc_configuration_set_interface(cc_configuration_io_interface_t const* interface);

/**
 * Sets default configuration in the Command Class
 *
 * @param[in]  configuration pointer to a configuration set
 */
void
cc_configuration_set_default_configuration(cc_configuration_t const* configuration);

/**
 * Limit a value by the parameter's limits
 *
 * @param[in]  parameter_buffer pointer to the parameter buffer which holds the limit values
 * @param[out]  pNewValue the new value to be limited
 * @return Returns true if successfuly limiterd, false otherwise
 */
bool
cc_configuration_limit_value(cc_config_parameter_buffer_t const* parameter_buffer,
                                cc_config_parameter_value_t * pNewValue);

#endif /* ZAF_COMMANDCLASSES_CONFIGURATION_CC_CONFIGURATION_H_ */
