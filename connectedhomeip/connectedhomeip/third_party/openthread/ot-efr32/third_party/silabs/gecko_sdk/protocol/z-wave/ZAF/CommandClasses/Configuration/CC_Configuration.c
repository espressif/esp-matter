 /***************************************************************************//**
 * @file CC_Configuration.c
 * @brief CC_Configuration.c
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
// -----------------------------------------------------------------------------
//                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <CC_Configuration.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_Common_interface.h>
#include <stddef.h>
#include <ZAF_tx_mutex.h>

// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------
#define SLI_CC_CONFIGURATION_MAX_STR_LENGTH (256)
// -----------------------------------------------------------------------------
//              Static Function Declarations
// -----------------------------------------------------------------------------

/**
 * Handler for Configuration CC set command
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_set(  RECEIVE_OPTIONS_TYPE_EX const * pRxOpt,
                                  ZW_APPLICATION_TX_BUFFER const * pCmd,
                                  const uint8_t cmdLength);

/**
 * Handler for Configuration CC get command
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_get(  RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                  ZW_APPLICATION_TX_BUFFER const * pCmd,
                                  const uint8_t cmdLength);

/**
 * Handler for Configuration CC get info command
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_info( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                  ZW_APPLICATION_TX_BUFFER const * pCmd,
                                  const uint8_t cmdLength);

/**
 * Handler for Configuration CC get name command
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_name_get( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                      ZW_APPLICATION_TX_BUFFER const * pCmd,
                                      const uint8_t cmdLength);

/**
 * Handler for Configuration CC get property
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_properties_get( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                            ZW_APPLICATION_TX_BUFFER const * pCmd,
                                            const uint8_t cmdLength);

/**
 * Handler for Configuration CC get bulk
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_bulk_get( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                      ZW_APPLICATION_TX_BUFFER const * pCmd,
                                      const uint8_t cmdLength);

/**
 * Handler for Configuration CC set bulk
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_bulk_set( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                      ZW_APPLICATION_TX_BUFFER const * pCmd,
                                      const uint8_t cmdLength);

/**
 * Handler for sending bulk report response
 * @param[in] pRxOpt Receive options.
 * @param[in] parameter_offset offset from the first parameter's number
 * @param[in] stop number where the bulk report should end
 * @param[in] size of the configuration values
 * @param[in] flag_handshake if true the function call is a result of handshake request
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_send_bulk_report( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                              const uint16_t parameter_offset,
                                              const uint16_t stop_number,
                                              const uint8_t  size,
                                              const bool flag_handshake);

/**
 * Handler for resetting to the default state
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Payload including command class.
 * @param[in] cmdLength Length of the received command.
 * @return Result of command parsing.
*/
static received_frame_status_t
cc_configuration_command_default_reset( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                           ZW_APPLICATION_TX_BUFFER const * pCmd,
                                           const uint8_t cmdLength);

/**
 * Copy value to raw frame
 * @param[out] pFrame the raw frame where the data will be copied to
 * @param[in] parameter_buffer reference to the parameter
 * @param[in] pField which will be copied from
 * @return Result of the conversion which is a recevived_frame_status_t type
*/
static void
cc_configuration_copyToFrame(  cc_config_parameter_value_t* pFrame,
                                  const cc_config_parameter_buffer_t* parameter_buffer,
                                  const cc_config_parameter_value_t* pField);

/**
 * Converts recevived_frame_status_t type to EZAF_EnqueueStatus_t
 * @param[in] enqueue_status EZAF_EnqueueStatus_t to convert from
 * @return Result of the conversion which is a recevived_frame_status_t type
*/
static received_frame_status_t
cc_configuration_convert_enquestatus_to_framestatus(EZAF_EnqueueStatus_t enqueue_status);

/**
 * Resets a configuration value back to default value
 * @param[in] parameter_number configuration's number to reset
 * @return Result of the reset true in case of success, else false
*/
static bool
cc_configuration_reset_to_default_value(uint16_t parameter_number);

/**
 * Sets a configuration's value to a new one
 * @param[in] parameter_number configuration's number to change
 * @param[in] new_value pointer to a new configuration value
 * @param[in] size size of the new configuration value
 * @return Result of the setting, NOT_SUPPORTED, FAIL, or OK
*/
static cc_config_configuration_set_return_value
cc_configuration_set(uint16_t parameter_number,
                        cc_config_parameter_value_t* new_value,
                        cc_config_parameter_size_t size);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for signed integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_signed_int(cc_config_parameter_buffer_t const* parameter_buffer,
                                           cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for signed integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_signed_int_8bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                 cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for signed integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_signed_int_16bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                 cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for signed integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_signed_int_32bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                 cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for unsigned integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_unsigned_int(cc_config_parameter_buffer_t const* parameter_buffer,
                                             cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for unsigned integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_unsigned_int_8bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                                   cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for unsigned integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_unsigned_int_16bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                                   cc_config_parameter_value_t * pNewValue);

/**
 * Checks if a parameter is between the min and max values, it not it minimize or maximize.
 * This function is for unsigned integers
 * @param[in] parameter_buffer configuration's parameter which has the limits
 * @param[out] pNewValue the new value to be limited
 * @return Result of the limiting true in case of success, else false
*/
static bool 
cc_configuration_limit_value_unsigned_int_32bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                                   cc_config_parameter_value_t * pNewValue);
/**
 * Finds the configuration with the lowest number in the pool
 * @param[out] parameter_buffer a pointer to a cc_config_parameter_t pointer to pass back the reference of the first configuration
 * @return Result of the look up true in case of success, else false
*/
static bool
cc_configuration_get_first_parameter(cc_config_parameter_buffer_t* parameter_buffer);


/**
 * Calculates how many more frames are needed to send the whole dara
 * @param[in] data_length  length of the data to be sent
 * @param[in] payload_limit maximum payload length
 * @return number of maximum reports to follow
*/
static uint8_t
cc_configuration_calc_reports_to_follow(size_t data_length, size_t payload_limit);

/**
 * Calculates the total length of the report and how many parameters will be included
 * @param[in] start_parameter_buffer The first parameter which will be included in the report
 * @param[in] required_parameter_num Max number of required parameters
 * @param[out] sum_report_size Total length of the report in bytes
 * @param[out] continous_parameter_count Number of the parameters included
 * @return number of maximum reports to follow
*/
static bool
cc_configuration_command_send_bulk_report_calc_report_size(const cc_config_parameter_buffer_t* start_parameter_buffer,
                                                              const uint16_t required_parameter_num,
                                                              uint16_t* sum_report_size,
                                                              uint16_t* continous_parameter_count );
/**
 * Determine the first parameter number
 *
 * @param[out] parameter_buffer pointer to a parameter buffer which will holds the metadata and current value of a parameter
 * @return same type as cc_configuration_get
 */
static bool
cc_configuration_probe(cc_config_parameter_buffer_t* parameter_buffer);

/**
 * Find the next valid parameter number in case of an invalid  one
 * @param[in] input holds the value of the current (invalid) parameter number
 * @return the next available parameter number, if not exist, the fist available parameter number is given back
 */
static uint16_t
cc_configuration_find_next_valid_parameter_number(uint16_t input);

/**
 * Give back the first available parameter number
 * @return the fist available parameter number is given back
 */
static uint16_t
cc_configuration_get_lowest_parameter_number();

/**
 * Check if the given parameter number is valid
 * @param[in] input holds the value of the current tested parameter number
 * @return true if the parameter number is valid, false anyway
 */
static bool
cc_configuration_check_if_parameter_number_is_valid(uint16_t input);

/**
 * Check, whether the size parameter is valid or not
 * @param[in] size_value the size value that needs to be checked
 * @return true if valid, false anyways
*/
static bool 
cc_configuration_is_valid_size(cc_config_parameter_size_t size_value);

static size_t
cc_configuration_strnlen(const char *str, size_t maxlen);
// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Static Variables
// -----------------------------------------------------------------------------
/**< cc_configuration_t pointer to the meta data of the parameters */
static cc_configuration_t const* configuration_pool;
/**< cc_configuration_interface_t instance which holds the registered io interface references. */
static cc_configuration_io_interface_t const* io_interface;
// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------

void
cc_configuration_set_interface(cc_configuration_io_interface_t const* interface)
{
  io_interface = interface;
}

void
cc_configuration_set_default_configuration(cc_configuration_t const* configuration)
{
  configuration_pool = configuration;
}

bool
cc_configuration_init(cc_configuration_t const* configuration, cc_configuration_io_interface_t const* interface )
{
  bool retval = false;
  bool is_migrated = false;
  cc_config_parameter_buffer_t parameter_buffer;

  if((configuration != NULL) && (interface != NULL))
  {
    cc_configuration_set_interface(interface);
    cc_configuration_set_default_configuration(configuration);

    for(uint16_t loop_cnt = 0; loop_cnt < configuration_pool->numberOfParameters ; loop_cnt++)
    {
      is_migrated = false;

      parameter_buffer.metadata = &configuration_pool->parameters[loop_cnt];

      retval = io_interface->read_handler( parameter_buffer.metadata->file_id,
                                          (uint8_t*)&parameter_buffer.data_buffer,
                                           sizeof(cc_config_parameter_value_t));

      if(retval == false)
      {
        /*Parameter is not stored, let's write it*/
        retval = io_interface->write_handler( parameter_buffer.metadata->file_id,
                                             (const uint8_t*)&configuration_pool->parameters[loop_cnt].attributes.default_value,
                                              sizeof(cc_config_parameter_value_t));
      }
      else if(configuration_pool->parameters[loop_cnt].migration_handler != NULL)
      {
        is_migrated = configuration_pool->parameters[loop_cnt].migration_handler(&parameter_buffer);
      }

      if(is_migrated == true)
      {
        /*Parameter is not stored, let's write it*/
        retval = io_interface->write_handler( parameter_buffer.metadata->file_id,
                                              (const uint8_t*)&parameter_buffer.data_buffer,
                                              sizeof(cc_config_parameter_value_t));
      }
    }
  }

  return retval;
}

static void
cc_configuration_copyToFrame(  cc_config_parameter_value_t* pFrame,
                                  const cc_config_parameter_buffer_t* parameter_buffer,
                                  const cc_config_parameter_value_t* pField)
{
  if( (pFrame != NULL)           &&
      (parameter_buffer != NULL) &&
      (pField != NULL))
  {

    if(NULL != parameter_buffer->metadata)
    {
      int8_t src_ix  = (int8_t)parameter_buffer->metadata->attributes.size - 1;
      int8_t dest_ix = 0;

      while((src_ix >= 0) && (dest_ix < (int8_t)sizeof(pFrame->as_uint8_array)))
      {
        pFrame->as_uint8_array[dest_ix] = pField->as_uint8_array[src_ix];
        src_ix--;
        dest_ix++;
      }
    }
  }
}

static received_frame_status_t CC_Configuration_handler(RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                                        ZW_APPLICATION_TX_BUFFER *pCmd,
                                                        uint8_t cmdLength)
{
  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_NO_SUPPORT;

  switch (pCmd->ZW_Common.cmd) {
    case CONFIGURATION_INFO_GET_V4:
      frame_status = cc_configuration_command_info(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_GET_V4:
      frame_status = cc_configuration_command_get(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_SET_V4:
      frame_status = cc_configuration_command_set(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_NAME_GET_V4:
      frame_status = cc_configuration_command_name_get(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_PROPERTIES_GET_V4:
      frame_status = cc_configuration_command_properties_get(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_BULK_GET_V4:
      frame_status = cc_configuration_command_bulk_get(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_BULK_SET_V4:
      frame_status = cc_configuration_command_bulk_set(pRxOpt, pCmd, cmdLength);
      break;
    case CONFIGURATION_DEFAULT_RESET_V4:
      frame_status = cc_configuration_command_default_reset(pRxOpt, pCmd, cmdLength);
      break;
    default:
      break;
  }
  return frame_status;
}

// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------
static bool 
cc_configuration_limit_value_signed_int_8bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_int8 > pNewValue->as_int8)
  {
    pNewValue->as_int8 = parameter_buffer->metadata->attributes.min_value.as_int8;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_int8 < pNewValue->as_int8)
  {
    pNewValue->as_int8 = parameter_buffer->metadata->attributes.max_value.as_int8;
    return_value = false;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_signed_int_16bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                 cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_int16 > pNewValue->as_int16)
  {
    pNewValue->as_int16 = parameter_buffer->metadata->attributes.min_value.as_int16;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_int16 < pNewValue->as_int16)
  {
    pNewValue->as_int16 = parameter_buffer->metadata->attributes.max_value.as_int16;
    return_value = false;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_signed_int_32bit(cc_config_parameter_buffer_t const * parameter_buffer,
                                                 cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_int32 > pNewValue->as_int32)
  {
    pNewValue->as_int32 = parameter_buffer->metadata->attributes.min_value.as_int32;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_int32 < pNewValue->as_int32)
  {
    pNewValue->as_int32 = parameter_buffer->metadata->attributes.max_value.as_int32;
    return_value = false;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_signed_int(cc_config_parameter_buffer_t const * parameter_buffer,
                                           cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  switch(parameter_buffer->metadata->attributes.size)
  {
    case CC_CONFIG_PARAMETER_SIZE_8_BIT:
      return_value = cc_configuration_limit_value_signed_int_8bit(parameter_buffer, pNewValue);
      break;
    case CC_CONFIG_PARAMETER_SIZE_16_BIT:
      return_value = cc_configuration_limit_value_signed_int_16bit(parameter_buffer, pNewValue);
      break;
    case CC_CONFIG_PARAMETER_SIZE_32_BIT:
      return_value = cc_configuration_limit_value_signed_int_32bit(parameter_buffer, pNewValue);
      break;
    default:
      break;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_unsigned_int_8bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                             cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_uint8 > pNewValue->as_uint8)
  {
    pNewValue->as_uint8 = parameter_buffer->metadata->attributes.min_value.as_uint8;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_uint8 < pNewValue->as_uint8)
  {
    pNewValue->as_uint8 = parameter_buffer->metadata->attributes.max_value.as_uint8;
    return_value = false;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_unsigned_int_16bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                                   cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_uint16 > pNewValue->as_uint16)
  {
    pNewValue->as_uint16 = parameter_buffer->metadata->attributes.min_value.as_uint16;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_uint16 < pNewValue->as_uint16)
  {
    pNewValue->as_uint16 = parameter_buffer->metadata->attributes.max_value.as_uint16;
    return_value = false;
  }

  return return_value;
}

static bool 
cc_configuration_limit_value_unsigned_int_32bit(cc_config_parameter_buffer_t const* parameter_buffer,
                                                   cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  if( parameter_buffer->metadata->attributes.min_value.as_uint32 > pNewValue->as_uint32)
  {
    pNewValue->as_uint32 = parameter_buffer->metadata->attributes.min_value.as_uint32;
    return_value = false;
  }
  else if( parameter_buffer->metadata->attributes.max_value.as_uint32 < pNewValue->as_uint32)
  {
    pNewValue->as_uint32 = parameter_buffer->metadata->attributes.max_value.as_uint32;
    return_value = false;
  }
  
  return return_value;
}

static bool 
cc_configuration_limit_value_unsigned_int(cc_config_parameter_buffer_t const* parameter_buffer,
                                             cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  switch(parameter_buffer->metadata->attributes.size)
  {
    case CC_CONFIG_PARAMETER_SIZE_8_BIT:
      return_value = cc_configuration_limit_value_unsigned_int_8bit(parameter_buffer, pNewValue);
      break;
    case CC_CONFIG_PARAMETER_SIZE_16_BIT:
      return_value = cc_configuration_limit_value_unsigned_int_16bit(parameter_buffer, pNewValue);
      break;
    case CC_CONFIG_PARAMETER_SIZE_32_BIT:
      return_value = cc_configuration_limit_value_unsigned_int_32bit(parameter_buffer, pNewValue);
      break;
    default:
      break;
  }

  return return_value;
}

bool
cc_configuration_limit_value(cc_config_parameter_buffer_t const* parameter_buffer,
                                cc_config_parameter_value_t * pNewValue)
{
  bool return_value = true;

  switch (parameter_buffer->metadata->attributes.format)
  {
    case CC_CONFIG_PARAMETER_FORMAT_SIGNED_INTEGER:
      return_value = cc_configuration_limit_value_signed_int(parameter_buffer, pNewValue);
      break;
    case CC_CONFIG_PARAMETER_FORMAT_UNSIGNED_INTEGER:
      return_value = cc_configuration_limit_value_unsigned_int(parameter_buffer, pNewValue);
      break;
    default:
      break;
  }

  return return_value;
}

static received_frame_status_t
cc_configuration_command_info(   RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                    const ZW_APPLICATION_TX_BUFFER *pCmd,
                                    const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  EZAF_EnqueueStatus_t enqueue_status = ZAF_ENQUEUE_STATUS_TIMEOUT;
  ZW_CONFIGURATION_PROPERTIES_GET_V4_FRAME const* pCfgNameGetFrame = &pCmd->ZW_ConfigurationPropertiesGetV4Frame;

  cc_config_parameter_buffer_t parameter_buffer;
  const uint16_t payload_limit = ZAF_getAppHandle()->pNetworkInfo->MaxPayloadSize;
  uint16_t parameter_number = (uint16_t)(((uint16_t)pCfgNameGetFrame->parameterNumber1<<8) | pCfgNameGetFrame->parameterNumber2);
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  bool is_io_transaction_success = false;
  uint8_t reports_to_follow_count = 0;
  size_t info_str_length = 0;
  size_t info1_offset_in_struct = offsetof(ZW_CONFIGURATION_INFO_REPORT_4BYTE_V4_FRAME , info1);

  if (true == Check_not_legal_response_job(pRxOpt))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if(io_interface == NULL)
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  is_io_transaction_success = cc_configuration_get(parameter_number, &parameter_buffer);
  memset((void*)&pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame, 0 ,sizeof(pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame));
  pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.cmdClass   = COMMAND_CLASS_CONFIGURATION_V4;
  pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.cmd        = CONFIGURATION_INFO_REPORT_V4;
  pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.parameterNumber1 = pCfgNameGetFrame->parameterNumber1;
  pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.parameterNumber2 = pCfgNameGetFrame->parameterNumber2;
  pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.reportsToFollow  = reports_to_follow_count;
  RxToTxOptions(pRxOpt, &pTxOptionsEx);
  
  if(is_io_transaction_success != false)
  {
    size_t str_pointer = 0;
    info_str_length = cc_configuration_strnlen(parameter_buffer.metadata->attributes.info,
                                                  SLI_CC_CONFIGURATION_MAX_STR_LENGTH);
    reports_to_follow_count = cc_configuration_calc_reports_to_follow(info_str_length, payload_limit);
    pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.reportsToFollow  = reports_to_follow_count;

    while(str_pointer < info_str_length)
    {
      size_t remaining_byte_count = info_str_length - str_pointer;
      size_t byte_count_to_send   = (remaining_byte_count >= payload_limit) ? payload_limit : remaining_byte_count;
      size_t frame_size_bytes     = info1_offset_in_struct /*header size*/
                                    + byte_count_to_send;

      uint8_t* raw_buffer = (uint8_t*)pTxBuf;
      memcpy((void*)&raw_buffer[info1_offset_in_struct],
             (const void*)&parameter_buffer.metadata->attributes.info[str_pointer],
             byte_count_to_send);

      enqueue_status = Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                                  frame_size_bytes,
                                                  pTxOptionsEx,
                                                  NULL);

      if(enqueue_status != ZAF_ENQUEUE_STATUS_SUCCESS)
      {break;}

      str_pointer+=byte_count_to_send;
      pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.reportsToFollow--;
    }
  }
  else
  {
    enqueue_status = Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                                  sizeof(pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame),
                                                  pTxOptionsEx,
                                                  NULL);
  }

  return cc_configuration_convert_enquestatus_to_framestatus(enqueue_status);
}

static received_frame_status_t
cc_configuration_command_name_get( RECEIVE_OPTIONS_TYPE_EX *rxOpt,
                                      const ZW_APPLICATION_TX_BUFFER *pCmd,
                                      const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  EZAF_EnqueueStatus_t enqueue_status = ZAF_ENQUEUE_STATUS_TIMEOUT;
  ZW_CONFIGURATION_NAME_GET_V4_FRAME const* pCfgNameGetFrame = &pCmd->ZW_ConfigurationNameGetV4Frame;

  const uint16_t payload_limit = ZAF_getAppHandle()->pNetworkInfo->MaxPayloadSize;
  uint16_t parameter_number = (uint16_t)(((uint16_t)pCfgNameGetFrame->parameterNumber1<<8) | pCfgNameGetFrame->parameterNumber2);
  cc_config_parameter_buffer_t parameter_buffer;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  bool is_io_transaction_success = false;
  uint8_t reports_to_follow_count = 0;
  size_t name_str_length = 0;

  if (true == Check_not_legal_response_job(rxOpt))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  is_io_transaction_success = cc_configuration_get(parameter_number, &parameter_buffer);
  memset((void*)&pTxBuf->ZW_ConfigurationNameReport4byteV4Frame, 0, sizeof(pTxBuf->ZW_ConfigurationNameReport4byteV4Frame));
  pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.cmdClass = COMMAND_CLASS_CONFIGURATION_V4;
  pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.cmd      = CONFIGURATION_NAME_REPORT_V4;
  pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.parameterNumber1 = pCfgNameGetFrame->parameterNumber1;
  pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.parameterNumber2 = pCfgNameGetFrame->parameterNumber2;
  pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.reportsToFollow  = reports_to_follow_count;
  RxToTxOptions(rxOpt, &pTxOptionsEx);

  if(is_io_transaction_success != false)
  {
    size_t str_pointer = 0;
    size_t name1_offset_in_struct = offsetof(ZW_CONFIGURATION_NAME_REPORT_4BYTE_V4_FRAME , name1);

    name_str_length = cc_configuration_strnlen(parameter_buffer.metadata->attributes.name,
                                                   SLI_CC_CONFIGURATION_MAX_STR_LENGTH);

    reports_to_follow_count = cc_configuration_calc_reports_to_follow(name_str_length, payload_limit);
    pTxBuf->ZW_ConfigurationNameReport4byteV4Frame.reportsToFollow  = reports_to_follow_count;
    
    while(str_pointer < name_str_length)
    {
      size_t remaining_byte_count = name_str_length - str_pointer;
      size_t byte_count_to_send   = (remaining_byte_count >= payload_limit) ? payload_limit : remaining_byte_count;
      size_t frame_size_bytes     = name1_offset_in_struct /*header size*/
                                    + byte_count_to_send;

      uint8_t* raw_buffer = (uint8_t*)pTxBuf;
      memcpy((void*)&raw_buffer[name1_offset_in_struct],
            (const void*)&parameter_buffer.metadata->attributes.name[str_pointer],
            byte_count_to_send);

      enqueue_status = Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                                  frame_size_bytes,
                                                  pTxOptionsEx,
                                                  NULL);

      if(enqueue_status != ZAF_ENQUEUE_STATUS_SUCCESS)
      {break;}

      str_pointer+=byte_count_to_send;
      pTxBuf->ZW_ConfigurationInfoReport4byteV4Frame.reportsToFollow--;
    }
  }
  else
  {
     enqueue_status = Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                                  sizeof(pTxBuf->ZW_ConfigurationNameReport4byteV4Frame),
                                                  pTxOptionsEx,
                                                  NULL);

  }
  

  return cc_configuration_convert_enquestatus_to_framestatus(enqueue_status);
}

static received_frame_status_t
cc_configuration_command_set(  RECEIVE_OPTIONS_TYPE_EX const *rxOpt,
                                  ZW_APPLICATION_TX_BUFFER const *pCmd,
                                  const uint8_t cmdLength)
{
  UNUSED(rxOpt);
  UNUSED(cmdLength);

  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
  ZW_CONFIGURATION_SET_4BYTE_V4_FRAME const* pCfgSetFrame = &pCmd->ZW_ConfigurationSet4byteV4Frame;
  cc_config_parameter_size_t size  =   pCfgSetFrame->level&0x07;

  if((io_interface == NULL) || (false == cc_configuration_is_valid_size(size)))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  bool is_default                  = ((pCfgSetFrame->level&0x80) == 0x80)?true:false;
  cc_config_parameter_value_t new_value;

  if(is_default == false)
  {
    memset(&new_value, 0, sizeof(cc_config_parameter_value_t));
    const cc_config_parameter_value_t* pConfigValue = ( cc_config_parameter_value_t *)&pCmd->ZW_ConfigurationSet4byteV4Frame.configurationValue1;
    uint8_t size_of_configuration_values = (uint8_t)size;

    for(uint8_t index = 0; index < size_of_configuration_values; index++)
    {
      new_value.as_uint8_array[size_of_configuration_values - index - 1] = pConfigValue->as_uint8_array[index];
    }

    cc_config_configuration_set_return_value return_value = cc_configuration_set(pCfgSetFrame->parameterNumber, &new_value, size);
    if(( CC_CONFIG_RETURN_CODE_IO_FAIL == return_value) || (CC_CONFIG_RETURN_CODE_NOT_SUPPORTED == return_value))
    {
      frame_status = RECEIVED_FRAME_STATUS_FAIL;
    }
  }
  else
  {
    if(false == cc_configuration_reset_to_default_value(pCfgSetFrame->parameterNumber))
    {
      frame_status = RECEIVED_FRAME_STATUS_FAIL;
    }
  }
  
  return frame_status;
}

static received_frame_status_t
cc_configuration_command_get(  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
                                  ZW_APPLICATION_TX_BUFFER const * pCmd,
                                  const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  EZAF_EnqueueStatus_t enqueue_status;
  ZW_CONFIGURATION_GET_V4_FRAME const* pCfgGetFrame = &pCmd->ZW_ConfigurationGetV4Frame;
  ZAF_TRANSPORT_TX_BUFFER TxBuf;
  uint16_t parameter_number = pCfgGetFrame->parameterNumber;
  cc_config_parameter_buffer_t parameter_buffer;
  bool is_io_transaction_success = false;
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptions;

  memset(&TxBuf, 0, sizeof(ZAF_TRANSPORT_TX_BUFFER));

  if (true == Check_not_legal_response_job(rxOpt) || (io_interface == NULL))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  is_io_transaction_success = cc_configuration_get(parameter_number, &parameter_buffer);

  // Sonarcube error suppress (Dereference of undefined pointer value)
  if(NULL != parameter_buffer.metadata)
  {
    parameter_number = parameter_buffer.metadata->number;
  }
  

  if(is_io_transaction_success == false)
  {
    /*Unknown parameter number, get the first one*/
    is_io_transaction_success = cc_configuration_get_first_parameter(&parameter_buffer);
  }

  TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame.cmdClass        = COMMAND_CLASS_CONFIGURATION_V4;
  TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame.cmd             = CONFIGURATION_REPORT_V4;
  TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame.parameterNumber = (uint8_t)parameter_number;
  if(NULL != parameter_buffer.metadata)
  {
    TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame.level         = (uint8_t)parameter_buffer.metadata->attributes.size;
  }
  
  cc_configuration_copyToFrame((cc_config_parameter_value_t *)&TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame.configurationValue1,
                                  &parameter_buffer,
                                  &parameter_buffer.data_buffer);

  RxToTxOptions(rxOpt, &pTxOptions);

  enqueue_status = Transport_SendResponseEP((uint8_t*)&TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame,
                                            sizeof(TxBuf.appTxBuf.ZW_ConfigurationReport4byteV4Frame),
                                            pTxOptions,
                                            NULL);

  return cc_configuration_convert_enquestatus_to_framestatus(enqueue_status);
}


static uint16_t
cc_configuration_find_next_valid_parameter_number(uint16_t input)
{

  uint16_t global_difference = 0xFF;
  uint16_t next_number = 0;
  const uint16_t lowest_number = cc_configuration_get_lowest_parameter_number();
  
  for(uint8_t i = 0; i < configuration_pool->numberOfParameters; i++)
  {
    if((configuration_pool->parameters[i].next_number - input) <= global_difference)
    {
      global_difference = configuration_pool->parameters[i].next_number - input;
      next_number = configuration_pool->parameters[i].next_number;
    }
  }
  // return the calculated next number
  return ((next_number == 0) ? lowest_number : next_number);
}

static bool
cc_configuration_check_if_parameter_number_is_valid(uint16_t input)
{

  bool result = false;
  for(uint8_t i = 0; i < configuration_pool->numberOfParameters; i++)
  {
    if(input == configuration_pool->parameters[i].number)
    {
      result = true;
      break;
    }
  }
  return result;
}


static received_frame_status_t
cc_configuration_command_properties_get(  RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                             ZW_APPLICATION_TX_BUFFER const * pCmd,
                                             const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  EZAF_EnqueueStatus_t enqueue_status = ZAF_ENQUEUE_STATUS_TIMEOUT;
  ZW_CONFIGURATION_PROPERTIES_GET_V4_FRAME const* pCfgNameGetFrame = &pCmd->ZW_ConfigurationPropertiesGetV4Frame;
  uint16_t parameter_number = (uint16_t)(((uint16_t)pCfgNameGetFrame->parameterNumber1<<8) | pCfgNameGetFrame->parameterNumber2);
  cc_config_parameter_buffer_t parameter_buffer;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  bool is_valid_parameter_number = cc_configuration_check_if_parameter_number_is_valid(parameter_number);
  if (true == Check_not_legal_response_job(pRxOpt) || (io_interface == NULL))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if(0 == parameter_number)
  {
    cc_configuration_probe(&parameter_buffer);
  }
  else if(true == is_valid_parameter_number)
  {
    cc_configuration_get(parameter_number, &parameter_buffer);
  }

  RxToTxOptions(pRxOpt, &pTxOptionsEx);

  if( false == is_valid_parameter_number)
  {
    uint8_t* raw_buffer = (uint8_t*)pTxBuf;
    uint8_t raw_buffer_index = 0;

    uint16_t next_parameter_number = cc_configuration_find_next_valid_parameter_number(parameter_number);

    raw_buffer[raw_buffer_index++] = COMMAND_CLASS_CONFIGURATION_V4;
    raw_buffer[raw_buffer_index++] = CONFIGURATION_PROPERTIES_REPORT_V4;
    raw_buffer[raw_buffer_index++] = pCfgNameGetFrame->parameterNumber1;
    raw_buffer[raw_buffer_index++] = pCfgNameGetFrame->parameterNumber2;
    raw_buffer[raw_buffer_index++] = 0; // size and format bits are 0 if the parameter number is invalid
    raw_buffer[raw_buffer_index++] = (uint8_t)((next_parameter_number >> 8) & 0xFF);
    raw_buffer[raw_buffer_index++] = (uint8_t)(next_parameter_number  & 0xFF);
    raw_buffer[raw_buffer_index++] = 0; // Properties 2
    
    enqueue_status = Transport_SendResponseEP((uint8_t *)pTxBuf,
                                              raw_buffer_index,
                                              pTxOptionsEx,
                                              NULL);
  }
  else
  {
    memset(&pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame,
          0,
          sizeof(pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame));

    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.cmdClass         = COMMAND_CLASS_CONFIGURATION_V4;
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.cmd              = CONFIGURATION_PROPERTIES_REPORT_V4;
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.parameterNumber1 = pCfgNameGetFrame->parameterNumber1;
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.parameterNumber2 = pCfgNameGetFrame->parameterNumber2;
    
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.properties1 =  (uint8_t)(parameter_buffer.metadata->attributes.format<<3) |
                                                                        (uint8_t)(parameter_buffer.metadata->attributes.size);
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.nextParameterNumber1 = (uint8_t)((parameter_buffer.metadata->next_number >> 8) & 0xFF);
    pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.nextParameterNumber2 = (uint8_t)((parameter_buffer.metadata->next_number) & 0xFF);
  

    cc_configuration_copyToFrame((cc_config_parameter_value_t *)&pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.minValue1,
                                      &parameter_buffer,
                                      &parameter_buffer.metadata->attributes.min_value);
    cc_configuration_copyToFrame((cc_config_parameter_value_t *)&pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.maxValue1,
                                      &parameter_buffer,
                                      &parameter_buffer.metadata->attributes.max_value);
    cc_configuration_copyToFrame((cc_config_parameter_value_t *)&pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame.defaultValue1,
                                      &parameter_buffer,
                                      &parameter_buffer.metadata->attributes.default_value);

    enqueue_status = Transport_SendResponseEP((uint8_t *)pTxBuf,
                                            sizeof(pTxBuf->ZW_ConfigurationPropertiesReport4byteV4Frame),
                                            pTxOptionsEx,
                                            NULL);
  }

  return cc_configuration_convert_enquestatus_to_framestatus(enqueue_status);
}


static received_frame_status_t
cc_configuration_command_bulk_get( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                      ZW_APPLICATION_TX_BUFFER const * pCmd,
                                      const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  ZW_CONFIGURATION_BULK_GET_V4_FRAME const* pCfgBulkGetFrame = &pCmd->ZW_ConfigurationBulkGetV4Frame;

  uint16_t parameter_offset    = (uint16_t)(((uint16_t)pCfgBulkGetFrame->parameterOffset1<<8) |
                                                       pCfgBulkGetFrame->parameterOffset2);
  uint8_t number_of_parameters = pCfgBulkGetFrame->numberOfParameters;

  if (true == Check_not_legal_response_job(pRxOpt))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  return cc_configuration_command_send_bulk_report(pRxOpt,
                                                      parameter_offset,
                                                      parameter_offset + number_of_parameters,
                                                      CC_CONFIG_PARAMETER_SIZE_NOT_SPECIFIED,
                                                      false);
}

static received_frame_status_t
cc_configuration_command_bulk_set( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                      ZW_APPLICATION_TX_BUFFER const * pCmd,
                                      const uint8_t cmdLength)
{
  UNUSED(cmdLength);

  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
  uint8_t const* p_raw_cmd = (uint8_t const*)pCmd;
  bool io_transaction_result = false;
  cc_config_parameter_buffer_t parameter_buffer;
  
  uint8_t cmd_index = 0;
  cmd_index++; /*Command Class*/
  cmd_index++; /*Command */

  /*combine parameter offset 1 and 2*/
  uint16_t parameter_offset = (uint16_t)(((uint16_t)p_raw_cmd[cmd_index]<<8) | p_raw_cmd[cmd_index + 1] );
  cmd_index+=2;
  uint8_t number_of_parameters = p_raw_cmd[cmd_index];   /*Number of parameters*/
  cmd_index++;
  
  bool is_default   = ((p_raw_cmd[cmd_index]&(1<<7)) == 0x80)?true:false; /*Default*/
  bool handshake    = ((p_raw_cmd[cmd_index]&(1<<6)) == 0x40)?true:false; /*Handshake*/
  cc_config_parameter_size_t size = p_raw_cmd[cmd_index]&0x07; /*Size*/
  if(false == cc_configuration_is_valid_size(size))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }
  cmd_index++;

  cc_config_parameter_value_t new_parameter_value;
  uint16_t updated_parameter_counter = 0;
  uint16_t first_parameter_number   = parameter_offset;
  bool status = cc_configuration_get_first_parameter(&parameter_buffer);
  if(status == true)
  {
    first_parameter_number   += parameter_buffer.metadata->number;
  }


  while(updated_parameter_counter < number_of_parameters)
  {
    bool has_to_break = false;
    io_transaction_result = cc_configuration_get(first_parameter_number, &parameter_buffer);

    if(io_transaction_result == false)
    {
      frame_status = RECEIVED_FRAME_STATUS_FAIL;
      has_to_break = true;
    }

    if(has_to_break == true)
    {
      break;
    }

    if(is_default == true)
    {
      cc_configuration_reset_to_default_value(first_parameter_number);
    }
    else
    {
      memset(&new_parameter_value, 0, sizeof(cc_config_parameter_value_t));

      uint8_t size_of_new_parameter_value = sizeof(new_parameter_value.as_int32); //4bytes
      for(uint8_t index = 0; index < size_of_new_parameter_value ; index++)
      {
        new_parameter_value.as_uint8_array[size_of_new_parameter_value - index -1] = p_raw_cmd[cmd_index + index];  
      }

      cc_config_configuration_set_return_value return_value = cc_configuration_set(first_parameter_number, &new_parameter_value, size);
      if(( CC_CONFIG_RETURN_CODE_IO_FAIL == return_value) || (CC_CONFIG_RETURN_CODE_NOT_SUPPORTED == return_value))
      {
        frame_status = RECEIVED_FRAME_STATUS_FAIL;
        has_to_break = true;
      }
      cmd_index += size;
    }

    if(parameter_buffer.metadata->next_number != 0x0000)
    {
      first_parameter_number = parameter_buffer.metadata->next_number;
    }
    else
    {
      //This is the last parameter
      has_to_break = true;
    }
    updated_parameter_counter++;
    
  }

  if(handshake == true)
  {
    frame_status = cc_configuration_command_send_bulk_report(pRxOpt,
                                                                parameter_offset,
                                                                parameter_offset + number_of_parameters,
                                                                (uint8_t)size,
                                                                handshake);
  }

  return frame_status;
}

static received_frame_status_t
cc_configuration_command_send_bulk_report( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                              const uint16_t parameter_offset,
                                              const uint16_t stop_number,
                                              const uint8_t size,
                                              const bool flag_handshake)
{
  EZAF_EnqueueStatus_t enqueue_status = ZAF_ENQUEUE_STATUS_SUCCESS;

  const uint16_t payload_limit = ZAF_getAppHandle()->pNetworkInfo->MaxPayloadSize;
  bool status = true;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  cc_config_parameter_buffer_t parameter_buffer;
  uint8_t* p_payload = &pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.variantgroup1.parameter1;
  uint16_t required_parameter_count = stop_number - parameter_offset;
  uint16_t first_parameter_number   = parameter_offset; 

  if(io_interface == NULL)
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  RxToTxOptions(pRxOpt, &pTxOptionsEx);

  uint16_t sum_size_to_report      = 0;
  uint16_t continous_parater_count = 0;
  uint8_t reports_to_follow_count  = 0;


  status = cc_configuration_get_first_parameter(&parameter_buffer);

  if(status == true)
  {
    first_parameter_number   += parameter_buffer.metadata->number;
    status = cc_configuration_get(first_parameter_number,
                                                        &parameter_buffer);
  }

  if(status == true)
  {
    cc_configuration_command_send_bulk_report_calc_report_size(&parameter_buffer,
                                                                  required_parameter_count,
                                                                  &sum_size_to_report,
                                                                  &continous_parater_count);

    reports_to_follow_count = cc_configuration_calc_reports_to_follow(sum_size_to_report, payload_limit);

    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.cmdClass = COMMAND_CLASS_CONFIGURATION_V4;
    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.cmd      = CONFIGURATION_BULK_REPORT_V4;
    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.parameterOffset1   = (uint8_t)((parameter_offset >> 8)&0xFF);
    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.parameterOffset2   = (uint8_t)(parameter_offset & 0xFF);
    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.reportsToFollow    = reports_to_follow_count;
    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.numberOfParameters = 0;

    if(size == CC_CONFIG_PARAMETER_SIZE_NOT_SPECIFIED)
    {
      pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 = (uint8_t)parameter_buffer.metadata->attributes.size;
    }
    else
    {
      pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 = size;
    }

    if(flag_handshake == true)
    {
      pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 |= (1<<6);/*Handshake*/
    }

    pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 |= (1<<7); /*Default*/

    uint16_t sent_parameter_count = 0;
    size_t  current_payload_size = 0;
    status = true;
    cc_configuration_get(first_parameter_number, &parameter_buffer);

    while(status == true)
    {
      p_payload += current_payload_size;
      cc_configuration_copyToFrame(  (cc_config_parameter_value_t *)p_payload,
                                        &parameter_buffer,
                                        &parameter_buffer.data_buffer);

      if(0 != memcmp( (void*)parameter_buffer.data_buffer.as_uint8_array,
                      (const void*)parameter_buffer.metadata->attributes.default_value.as_uint8_array,
                      sizeof(cc_config_parameter_value_t)))
      {
        /*This configuration is not the default*/
        pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 &= (uint8_t)(~(1<<7));
      }
      pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.numberOfParameters++;

      current_payload_size += parameter_buffer.metadata->attributes.size;
      sent_parameter_count++;

      if((current_payload_size == payload_limit)               ||
         (parameter_buffer.metadata->next_number == 0x0000)    ||
         (sent_parameter_count == continous_parater_count))
      {
        size_t frame_size_bytes = offsetof(ZW_CONFIGURATION_BULK_REPORT_4BYTE_V4_FRAME , variantgroup1) /*header size*/
                                  + current_payload_size;

        enqueue_status = Transport_SendResponseEP((uint8_t *)pTxBuf,
                                                   frame_size_bytes,
                                                   pTxOptionsEx,
                                                   NULL);
        current_payload_size = 0;
        pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.numberOfParameters = 0;
        pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.properties1 |= (1<<7); /*Default*/
        pTxBuf->ZW_ConfigurationBulkReport4byteV4Frame.reportsToFollow--;
      }

      if((sent_parameter_count == continous_parater_count)   ||
         (enqueue_status != ZAF_ENQUEUE_STATUS_SUCCESS))
      {break;}

      status = cc_configuration_get(parameter_buffer.metadata->next_number, &parameter_buffer);
    }
  }
  else
  {
    enqueue_status = ZAF_ENQUEUE_STATUS_TIMEOUT;
  }

  return cc_configuration_convert_enquestatus_to_framestatus(enqueue_status);
}

static received_frame_status_t
cc_configuration_command_default_reset( RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                                           ZW_APPLICATION_TX_BUFFER const * pCmd,
                                           const uint8_t cmdLength)
{

  UNUSED(pRxOpt);
  UNUSED(pCmd);
  UNUSED(cmdLength);
  bool write_success = false;
  for(uint16_t parameter_ix = 0 ; parameter_ix < configuration_pool->numberOfParameters ; parameter_ix++)
  {
    write_success = io_interface->write_handler( configuration_pool->parameters[parameter_ix].file_id,
                                                        (const uint8_t*)&configuration_pool->parameters[parameter_ix].attributes.default_value,
                                                          sizeof(cc_config_parameter_value_t)); 
    if(false == write_success)
    {
      return RECEIVED_FRAME_STATUS_FAIL;
    }

  }
  return RECEIVED_FRAME_STATUS_SUCCESS;
}



static received_frame_status_t
cc_configuration_convert_enquestatus_to_framestatus(EZAF_EnqueueStatus_t enqueue_status)
{
  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_FAIL;

  switch(enqueue_status)
  {
    case ZAF_ENQUEUE_STATUS_SUCCESS:
      frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case ZAF_ENQUEUE_STATUS_TIMEOUT:
      frame_status = RECEIVED_FRAME_STATUS_FAIL;
      break;
    default:
      frame_status = RECEIVED_FRAME_STATUS_FAIL;
      break;
  }

  return frame_status;
}

static cc_config_configuration_set_return_value
cc_configuration_set(uint16_t parameter_number,  cc_config_parameter_value_t* new_value, cc_config_parameter_size_t size)
{
  cc_config_configuration_set_return_value return_value = CC_CONFIG_RETURN_CODE_OK;
  bool io_transaction_result = false;
  bool is_value_in_range = false;
  bool has_to_break = false;
  cc_config_parameter_buffer_t parameter_buffer;

    for(uint16_t parameter_ix = 0 ; parameter_ix < configuration_pool->numberOfParameters ; parameter_ix++)
    {
      if( configuration_pool->parameters[parameter_ix].attributes.flags.read_only == false            && 
          configuration_pool->parameters[parameter_ix].number                     == parameter_number) 
      {
        if(configuration_pool->parameters[parameter_ix].attributes.size != size)
        {
          return_value = CC_CONFIG_RETURN_CODE_NOT_SUPPORTED;
          has_to_break = true;
        }
        if(true == has_to_break)
        {
          break;
        }
        io_transaction_result = io_interface->read_handler( configuration_pool->parameters[parameter_ix].file_id,
                                                            (uint8_t*)&parameter_buffer.data_buffer,
                                                            sizeof(cc_config_parameter_value_t));

        if(io_transaction_result == false)
        {
          return_value = CC_CONFIG_RETURN_CODE_IO_FAIL;
        }
        
        parameter_buffer.metadata = &configuration_pool->parameters[parameter_ix];
        is_value_in_range = cc_configuration_limit_value(&parameter_buffer, new_value);
        if( true == is_value_in_range)
        {
            memcpy( (void*)(uint8_t*)&parameter_buffer.data_buffer.as_uint8_array,
                  (const void*)new_value->as_uint8_array,
                  sizeof(cc_config_parameter_value_t));

            io_transaction_result = io_interface->write_handler( configuration_pool->parameters[parameter_ix].file_id,
                                                            (const uint8_t*)&parameter_buffer.data_buffer,
                                                            sizeof(cc_config_parameter_value_t));
           
          

          
        }
        if(io_transaction_result == false)
        {
          return_value = CC_CONFIG_RETURN_CODE_IO_FAIL;
        }
        has_to_break = true;
      }

    }

    return return_value;
}

bool
cc_configuration_get(uint16_t parameter_number, cc_config_parameter_buffer_t* parameter_buffer)
{
  bool io_transaction_result = false;
  if(parameter_buffer != NULL)
  { 
     for(uint16_t parameter_ix = 0 ; parameter_ix < configuration_pool->numberOfParameters ; parameter_ix++)
     {
       if(configuration_pool->parameters[parameter_ix].number == parameter_number)
       {
         parameter_buffer->metadata = &configuration_pool->parameters[parameter_ix];
         io_transaction_result = io_interface->read_handler( configuration_pool->parameters[parameter_ix].file_id,
                                                             (uint8_t*)&parameter_buffer->data_buffer,
                                                             sizeof(cc_config_parameter_value_t));
         break;
       }
     }
   }
  return io_transaction_result;
}

static bool
cc_configuration_probe(cc_config_parameter_buffer_t* parameter_buffer)
{
  static cc_config_parameter_metadata_t zero_meta;
  static uint16_t lowest_parameter_number;

  lowest_parameter_number = cc_configuration_get_lowest_parameter_number();
  zero_meta.next_number = lowest_parameter_number;
  memset(parameter_buffer, 0, sizeof(cc_config_parameter_buffer_t));
  parameter_buffer->metadata = &zero_meta;
  return true;
}

static bool
cc_configuration_reset_to_default_value(uint16_t parameter_number)
{
  bool io_transaction_result = false;

  for(uint16_t parameter_ix = 0 ; parameter_ix < configuration_pool->numberOfParameters ; parameter_ix++)
  {
    if(configuration_pool->parameters[parameter_ix].number == parameter_number)
    {
      io_transaction_result = io_interface->write_handler( configuration_pool->parameters[parameter_ix].file_id,
                                                          (const uint8_t*)&configuration_pool->parameters[parameter_ix].attributes.default_value,
                                                           sizeof(cc_config_parameter_value_t));
      
      break;
    }
  }
  
  return io_transaction_result;
}

static bool
cc_configuration_get_first_parameter(cc_config_parameter_buffer_t* parameter_buffer)
{
  uint16_t lowest_parameter_number = cc_configuration_get_lowest_parameter_number(parameter_buffer);
  return cc_configuration_get(lowest_parameter_number, parameter_buffer);
}

static uint16_t
cc_configuration_get_lowest_parameter_number()
{
  uint16_t lowest_parameter_number = 0xFFFF;
  /*Find the lowest number to be the first one*/
  for(uint16_t parameter_ix = 0 ; parameter_ix < configuration_pool->numberOfParameters ; parameter_ix++)
  {
    uint16_t current_parameter_number = configuration_pool->parameters[parameter_ix].number;
    if(lowest_parameter_number > current_parameter_number)
    {
      lowest_parameter_number = current_parameter_number;
    }
  }
  return lowest_parameter_number;
}

static uint8_t
cc_configuration_calc_reports_to_follow(size_t data_length, size_t payload_limit)
{
  uint16_t reports_to_follow_count  = (uint16_t)(data_length / payload_limit);
  uint16_t val = ((data_length % payload_limit) != 0)?1:0;
  reports_to_follow_count += val; /*Round up*/
  reports_to_follow_count-=1; /*If the data fits in payload it should be zero*/

  return (uint8_t)reports_to_follow_count;
}
static bool
cc_configuration_command_send_bulk_report_calc_report_size(const cc_config_parameter_buffer_t* start_parameter_buffer,
                                                              const uint16_t required_parameter_num,
                                                              uint16_t* sum_report_size,
                                                              uint16_t* continous_parameter_count )
{
  bool status = false;
  uint16_t report_size     = 0;
  uint16_t parameter_count = 0;
  cc_config_parameter_buffer_t parameter_buffer;

  if((start_parameter_buffer != NULL) && (sum_report_size != NULL) && (continous_parameter_count != NULL))
  {
    status = true;
    memcpy( (void*)&parameter_buffer,
            (const void*)start_parameter_buffer,
            sizeof(cc_config_parameter_buffer_t));

    while(status == true)
    {
      report_size += parameter_buffer.metadata->attributes.size;
      parameter_count++;
      if(parameter_count == required_parameter_num)
      {
        // We are done, found all of the parameters
        break;
      }

      status = cc_configuration_get(parameter_buffer.metadata->next_number, &parameter_buffer);
    }

    *sum_report_size           = report_size;
    *continous_parameter_count = parameter_count;
  }
  return status;
}

static size_t
cc_configuration_strnlen(const char *str, size_t maxlen)
{
  size_t i = 0;
  if(str != NULL)
  {
    for(i = 0; i < maxlen && str[i]; i++);
  }
  return i;
}

static bool 
cc_configuration_is_valid_size(cc_config_parameter_size_t size_value)
{
  //Reffering to SDS13781-15 document, table-32 (page 165)
  bool return_value = false;
  switch(size_value)
  {
    case CC_CONFIG_PARAMETER_SIZE_8_BIT:
      return_value = true;
    break;

    case CC_CONFIG_PARAMETER_SIZE_16_BIT:
      return_value = true;
    break;

    case CC_CONFIG_PARAMETER_SIZE_32_BIT:
      return_value = true;
    break;

    case CC_CONFIG_PARAMETER_SIZE_NOT_SPECIFIED:
      return_value = false;
    break;

    default:
      return_value = false;
    break;
  }
  return return_value;

}

REGISTER_CC(COMMAND_CLASS_CONFIGURATION_V4, CONFIGURATION_VERSION_V4, CC_Configuration_handler);
