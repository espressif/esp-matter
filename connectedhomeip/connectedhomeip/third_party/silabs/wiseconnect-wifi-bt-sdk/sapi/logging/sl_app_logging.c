/*******************************************************************************
 * @file  sl_app_logging.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifdef SAPI_LOGGING_ENABLE
/**
 * Includes
 */
#include "rsi_driver.h"
#include "sl_app_logging.h"

uint8_t *logging_buffer;
uint8_t signal_lost;
uint32_t last_msg_sent_tsf;
const char debug_version[] = SL_LOG_VERSION;

/*==================================================================*/
/**
 * @fn         uint8_t *sl_log_init()
 * @param[in]  uint8_t * buffer, address of the buffer
 * @return     uint32_t length of the buffer consumed
 * @section description
 * This API initializes logging feature
 */
uint32_t sl_log_init(uint8_t *buffer)
{
  // Creates logging mutex
  rsi_mutex_create(&rsi_driver_cb_non_rom->logging_mutex);
  // assign the buffer
  logging_buffer = buffer;

  return LOG_BUFFER_SIZE;
}

/*==================================================================*/
/**
 * @fn         uint8_t *sl_log_deinit()
 * @param[in]  none
 * @return      0 = success 
 *             <0 = failure
 * @section description
 * This API deinitializes logging feature
 */
uint32_t sl_log_deinit()
{
  // Destroys logging mutex
  return rsi_mutex_destroy(&rsi_driver_cb_non_rom->logging_mutex);
}

/*==================================================================*/
/**
 * @fn         uint8_t *sl_get_log_buffer()
 * @param[in]  None
 * @return     uint8_t *pointer to the buffer
 * @section description
 * This API provides logging buffer
 */
uint8_t *sl_get_log_buffer()
{
  return logging_buffer;
}

/*==================================================================*/
/**
 * @fn       uint32_t sl_log_data_into_buf(uint8_t *buf, uint32_t arg_len, uint32_t arg)
 * @param[in]  uint8_t *buf, pointer to the buffer to add data
 * @param[in]  uint32_t arg_len, length of the data argument
 * @param[in]  uint32_t arg, data argument
 * @return     uint32_t len, number of bytes added to the buf
 * @section description
 * This API is used to populate the log data into buffer
 */
uint32_t sl_log_data_into_buf(uint8_t *buf, uint32_t arg_len, uint32_t arg)
{
  uint32_t len;

  if (arg_len == VARIABLE_LEN) {
    // Decoder needs NULL terminator
    len = strlen((char *)arg) + 1;
    strncpy((char *)buf, (char *)arg, LOG_BUFFER_SIZE);
  } else {
    len = (arg_len + 1);
    if (arg_len == DWORD_LEN) {
      *(uint32_t *)buf = (uint32_t)arg;
    } else if (arg_len == WORD_LEN) {
      *(uint16_t *)buf = (uint16_t)arg;
    } else {
      *(uint8_t *)buf = (uint8_t)arg;
    }
  }
  return len;
}

/*==================================================================*/
/**
 * @fn       uint32_t sl_log_debug_header(uint8_t *buf, uint32_t debug_info)
 * @param[in]  uint8_t *buf, pointer to the buffer to add data
 * @param[in]  debug_info_t *debug_info, debug information passed by reference
 * @param[in]  uint32_t arg, data argument
 * @return     uint32_t len, number of bytes added to the buf
 * @section description
 * This API is populates the debug logging header info into the buffer
 */
uint32_t sl_log_debug_header(uint8_t *buf, debug_info_t *debug_info)
{
  log_flags_t *msg_header = NULL;
  log_tsf_t *msg_tsf      = NULL;
  uint32_t current_tsf;
  uint32_t tsf_diff;

  msg_header = (log_flags_t *)buf;

  // signal_lost will be 1, if the previous logging attempt had failed
  msg_header->signal_lost = signal_lost;
  // reset signal_lost as buffer is now available
  signal_lost = 0;

  // check if HDID is required
  if (debug_info->did > MAX_LDID) {
    msg_header->hdid_present = 1;
    buf += sizeof(log_flags_t);
  } else {
    msg_header->hdid_present = 0;
    // HDID is not required, tsf can start a byte earlier
    buf += sizeof(log_flags_t) - 1;
  }
  msg_header->did = debug_info->did;

  // check for TSF
  current_tsf = rsi_hal_gettickcount();
  tsf_diff    = current_tsf - last_msg_sent_tsf;

  // add a new TSF only if the tsf_diff is equal/more than granularity configured
  if (tsf_diff < LOG_TSF_GRANULARITY_TICKS) {
    msg_header->tsf_present = 0;
  } else {
    msg_header->tsf_present = 1;
    msg_tsf                 = (log_tsf_t *)buf;
    if ((tsf_diff > MAX_LTSF) || (last_msg_sent_tsf == 0)) {
      msg_tsf->tsf_format = 1;
      msg_tsf->tsf        = current_tsf;
      buf += sizeof(log_tsf_t);
    } else {
      msg_tsf->tsf_format = 0;
      msg_tsf->tsf        = tsf_diff;
      buf += sizeof(log_tsf_t) - 1;
    }
    last_msg_sent_tsf = current_tsf;
  }

  return ((uint32_t)buf - (uint32_t)msg_header);
}

/*==================================================================*/
/**
 * @fn         void sl_log_send_message(uint32_t diagnostic_info, uint8_t num_args, uint32 *args)
 * @param[in]  uint32_t diagnostic info, debug information passed as value
 * @param[in]  uint8_t num_args, number of valid arguments sent by caller
 * @param[in]  uint8_t *args, pointer to array of data
 * @param[out] None
 * @return     None
 * @section description
 * This API is used to log diagnostic information without any arguments
 */
void sl_log_send_message(uint32_t diagnostic_info, uint8_t num_args, uint32_t *args)
{
  uint8_t *buf       = NULL;
  uint8_t *buf_start = NULL;
  uint8_t arg_len[MAX_NUM_ARGS];
  debug_info_t *debug_info = (debug_info_t *)&diagnostic_info;

  // log msg_headers with debug level within user configured level
  if (debug_info->level <= CONFIG_DEBUG_LEVEL) {
    rsi_mutex_lock(&rsi_driver_cb_non_rom->logging_mutex);

    buf_start = buf = sl_get_log_buffer();
    if (buf == NULL) {
      signal_lost = 1;
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->logging_mutex);
      return;
    }
    arg_len[0] = debug_info->arg1_len;
    arg_len[1] = debug_info->arg2_len;
    arg_len[2] = debug_info->arg3_len;

    buf += sl_log_debug_header(buf, debug_info);

    for (uint32_t i = 0; (i < num_args) && (i < MAX_NUM_ARGS); i++) {
      buf += sl_log_data_into_buf(buf, arg_len[i], args[i]);
    }

    sl_hal_send_log(buf_start, (uint32_t)buf - (uint32_t)buf_start);
    rsi_mutex_unlock(&rsi_driver_cb_non_rom->logging_mutex);
  }
}
/*==================================================================*/
/**
 * @fn         void sl_log_no_args(uint32_t debug_info)
 * @param[in]  uint32_t debug_info, debug information passed as value
 * @param[out] None
 * @return     None
 * @section description
 * This API is used to log diagnostic information without any arguments
 */
void sl_log_no_args(uint32_t diagnostic_info)
{
  sl_log_send_message(diagnostic_info, 0, NULL);
}
/*==================================================================*/
/**
 * @fn         void sl_log_1_args(uint32_t debug_info, uint32_t arg1)
 * @param[in]  uint32_t debug_info, debug information passed as value
 * @param[in]  uint32_t arg1, argument passed - could be integer or address
 * @param[out] None
 * @return     None
 * @section description
 * This API is used to log diagnostic information containing 1 argument
 */
void sl_log_1_args(uint32_t diagnostic_info, uint32_t arg1)
{
  sl_log_send_message(diagnostic_info, 1, &arg1);
}
/*==================================================================*/
/**
 * @fn         void sl_log_2_args(uint32_t debug_info, uint32_t arg1, uint32_t arg2)
 * @param[in]  uint32_t debug_info, debug information passed as value
 * @param[in]  uint32_t arg1, argument passed - could be integer or address
 * @param[in]  uint32_t arg2, argument passed - could be integer or address
 * @param[out] None
 * @return     None
 * @section description
 * This API is used to log diagnostic information containing 2 arguments
 */
void sl_log_2_args(uint32_t diagnostic_info, uint32_t arg1, uint32_t arg2)
{
  uint32_t args[2];

  args[0] = arg1;
  args[1] = arg2;

  sl_log_send_message(diagnostic_info, 2, args);
}
/*==================================================================*/
/**
 * @fn         void sl_log_3_args(uint32_t debug_info, uint32_t arg1, uint32_t arg2, uint32_t arg3)
 * @param[in]  uint32_t debug_info, debug information passed as value
 * @param[in]  uint32_t arg1, argument passed - could be integer or address
 * @param[in]  uint32_t arg2, argument passed - could be integer or address
 * @param[in]  uint32_t arg3, argument passed - could be integer or address
 * @param[out] None
 * @return     None
 * @section description
 * This API is used to log diagnostic information containing 3 arguments
 */
void sl_log_3_args(uint32_t diagnostic_info, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
  uint32_t args[3];

  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;

  sl_log_send_message(diagnostic_info, 3, args);
}
#endif
