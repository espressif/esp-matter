/*******************************************************************************
* @file  rsi_ble_gatt_apis.c
* @brief
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
#include "rsi_driver.h"
#ifdef RSI_BLE_ENABLE

#include "rsi_ble.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_config.h"
#include "rsi_bt_common_apis.h"

// GATT Client Functions
/**********************************************************************
 * In all GAT client APIs, all actions (get/set/write) are referred to
 * the GATT server running on the remote / connected device.
 *********************************************************************/

/** @addtogroup BT-LOW-ENERGY4
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_profiles_async(uint8_t *dev_addr,
 *                                                uint16_t start_handle,
 *                                                uint16_t end_handle,
 *                                                rsi_ble_resp_profiles_list_t *p_prof_list)
 * @brief      Get the supported profiles / services of the connected
 *             remote device asynchronously. \ref rsi_ble_on_event_profiles_list_t callback function will be
 *             called after the profiles list event is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_profiles_list_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_profile_list 	- profiles/services information will be filled in this structure after retrieving from the remote device,
				  please refer rsi_ble_resp_profiles_list_s structure for more info.\n
 * 		@note	p_prof_list structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_profiles_async(uint8_t *dev_addr,
                                   uint16_t start_handle,
                                   uint16_t end_handle,
                                   rsi_ble_resp_profiles_list_t *p_prof_list)
{
  rsi_ble_req_profiles_list_t req_profiles;
  memset(&req_profiles, 0, sizeof(req_profiles));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_profiles.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_profiles.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_profiles.start_handle = start_handle;
  req_profiles.end_handle   = end_handle;

  SL_PRINTF(SL_RSI_BLE_GET_PROFILES_HANDLE_ASYNC, BLE, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_PROFILES_ASYNC, &req_profiles, p_prof_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_profile_async(uint8_t *dev_addr, uuid_t profile_uuid,
 *                                               profile_descriptors_t *p_profile)
 * @brief      Get the specific profile / service of the connected remote device.
 *             \ref rsi_ble_one_event_profile_by_uuid_t callback function is called after
 *             the service characteristics response is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_one_event_profile_by_uuid_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  profile_uuid 	- services/profiles which are searched using profile_uuid \n
 * @param[out] p_profile 	- profile / service information filled in this structure after retrieving from the remote device,
				  please refer profile_descriptor_s structure for more info. \n
 * 		@note	p_profile structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */
int32_t rsi_ble_get_profile_async(uint8_t *dev_addr, uuid_t profile_uuid, profile_descriptors_t *p_profile)
{

  SL_PRINTF(SL_RSI_BLE_GET_PROFILES_UUID_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_profile_t req_profile;
  memset(&req_profile, 0, sizeof(req_profile));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_profile.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_profile.dev_addr, (int8_t *)dev_addr, 6);
#endif
  memcpy(&req_profile.profile_uuid, &profile_uuid, sizeof(uuid_t));

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_PROFILE_ASYNC, &req_profile, p_profile);
}
/*==============================================*/
/**
 * @fn         rint32_t rsi_ble_get_char_services_async(uint8_t *dev_addr,
 *                                                     uint16_t start_handle,
 *                                                     uint16_t end_handle,
 *                                                     rsi_ble_resp_char_services_t *p_char_serv_list)
 * @brief      Get the service characteristics of the connected / remote device.
 *             \ref rsi_ble_on_event_read_by_char_services_t callback function is called after
 *             the included service characteristics response is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_by_char_services_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_char_services_list - service characteristics details are filled in this structure, please refer rsi_ble_resp_char_serv_s structure for more info. \n
 * 		@note	p_char_services_list structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_char_services_async(uint8_t *dev_addr,
                                        uint16_t start_handle,
                                        uint16_t end_handle,
                                        rsi_ble_resp_char_services_t *p_char_serv_list)
{

  SL_PRINTF(SL_RSI_BLE_CHAR_SERVICES_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_char_services_t req_char_services;
  memset(&req_char_services, 0, sizeof(req_char_services));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_char_services.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_char_services.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_char_services.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_char_services.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_CHARSERVICES_ASYNC, &req_char_services, p_char_serv_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_inc_services_async(uint8_t *dev_addr,
 *                                                    uint16_t start_handle,
 *                                                    uint16_t end_handle,
 *                                                    rsi_ble_resp_inc_services_t *p_inc_serv_list)
 * @brief      Get the supported include services of the connected / remote device.
 * 			       \ref rsi_ble_on_event_read_by_inc_services_t callback function is called after
 * 			       the service characteristics response is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_by_inc_services_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_inc_serv_list 	- include service characteristics details are filled in this structure, please refer rsi_ble_resp_inc_serv structure for more info.\n
 * 		@note	p_inc_serv_list structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_inc_services_async(uint8_t *dev_addr,
                                       uint16_t start_handle,
                                       uint16_t end_handle,
                                       rsi_ble_resp_inc_services_t *p_inc_serv_list)
{

  SL_PRINTF(SL_RSI_BLE_GET_INC_SERVICES_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_inc_services_t req_inc_services;
  memset(&req_inc_services, 0, sizeof(req_inc_services));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_inc_services.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_inc_services.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_inc_services.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_inc_services.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_INCLUDESERVICES_ASYNC, &req_inc_services, p_inc_serv_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_char_value_by_uuid_async(uint8_t *dev_addr,
 *                                                          uint16_t start_handle,
 *                                                          uint16_t end_handle,
 *                                                          uuid_t char_uuid,
 *                                                          rsi_ble_resp_att_value_t *p_char_val)
 * @brief      Get the characteristic value by UUID (char_uuid).
 * 			       \ref rsi_ble_on_event_read_att_value_t callback function is called
 *			       after the attribute value is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_att_value_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[in]  char_uuid 	- UUID of the characteristic
 * @param[out] p_char_value 	- characteristic value is filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.\n
 * 		@note	p_char_value structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_char_value_by_uuid_async(uint8_t *dev_addr,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             uuid_t char_uuid,
                                             rsi_ble_resp_att_value_t *p_char_val)
{

  SL_PRINTF(SL_RSI_BLE_GET_CHAR_VALUE_BY_UUID_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_char_val_by_uuid_t req_char_val;
  memset(&req_char_val, 0, sizeof(req_char_val));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_char_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_char_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_char_val.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_char_val.end_handle, end_handle);
  memcpy(&req_char_val.char_uuid, &char_uuid, sizeof(uuid_t));

  return rsi_bt_driver_send_cmd(RSI_BLE_READCHARVALUEBYUUID_ASYNC, &req_char_val, p_char_val);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_att_descriptors_async(uint8_t *dev_addr,
 *                                                       uint16_t start_handle,
 *                                                       uint16_t end_handle,
 *                                                       rsi_ble_resp_att_descs_t *p_att_desc)
 * @brief      Get the characteristic descriptors list from the remote device. \ref rsi_ble_on_gatt_desc_val_event_t
 *  	         callback function is called after the attribute descriptors response is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_gatt_desc_val_event_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  start_handle - start handle (index) of the remote device's service records
 * @param[in]  end_handle - end handle (index) of the remote device's service records
 * @param[out] p_att_desc - pointer to characteristic descriptor structure, Please refer rsi_ble_resp_att_descs_s strcuture for more info.\n
 * 		@note	p_att_desc structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_att_descriptors_async(uint8_t *dev_addr,
                                          uint16_t start_handle,
                                          uint16_t end_handle,
                                          rsi_ble_resp_att_descs_t *p_att_desc)
{

  SL_PRINTF(SL_RSI_BLE_GET_ATT_DESCRIPTORS_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_att_descs_t req_att_desc;
  memset(&req_att_desc, 0, sizeof(req_att_desc));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_desc.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_desc.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_att_desc.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_att_desc.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_ATTRIBUTE_ASYNC, &req_att_desc, p_att_desc);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_att_value_async(uint8_t *dev_addr, uint16_t handle,
 *                                                 rsi_ble_resp_att_value_t *p_att_val)
 * @brief      Get the attribute with a handle.
 * 			       \ref rsi_ble_on_event_read_resp_t callback function is called upon receiving the attribute value. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- handle value of the attribute
 * @param[out] p_att_val - attribute value is filled in this structure, Please refer rsi_ble_resp_att_value_s structure for more info.\n
 * 		@note	p_att_val structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_att_value_async(uint8_t *dev_addr, uint16_t handle, rsi_ble_resp_att_value_t *p_att_val)
{

  SL_PRINTF(SL_RSI_BLE_GET_ATT_VALUE_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_att_value_t req_att_val;
  memset(&req_att_val, 0, sizeof(req_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_att_val.handle, handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_DESCRIPTORVALUE_ASYNC, &req_att_val, p_att_val);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_multiple_att_values_async(uint8_t *dev_addr,
 *                                                           uint8_t num_of_handlers,
 *                                                           uint16_t *handles,
 *                                                           rsi_ble_resp_att_value_t *p_att_vals)
 * @brief      Get the multiple attribute values by using multiple handles.
 *             \ref rsi_ble_on_event_read_resp_t callback function is called after the attribute value is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  num_of_handlers 	- number of handles in the list
 * @param[in]  handles 		- list of attribute handles
 * @param[out] p_att_vals 	- attribute values filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.\n
 * 		@note	p_att_vals structure should be passed as NULL because nothing will be filled in this structure \n

 * @return     0		-	Success \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_multiple_att_values_async(uint8_t *dev_addr,
                                              uint8_t num_of_handlers,
                                              uint16_t *handles,
                                              rsi_ble_resp_att_value_t *p_att_vals)
{

  SL_PRINTF(SL_RSI_BLE_GET_MULTIPLE_ATT_VALUES_ASYNC, BLE, LOG_INFO, "NUMBER_OF_HANDLERS: %1x", num_of_handlers);
  rsi_ble_req_multi_att_values_t req_att_vals;
  memset(&req_att_vals, 0, sizeof(req_att_vals));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_vals.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_vals.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_att_vals.num_of_handles = RSI_MIN(num_of_handlers, RSI_BLE_MAX_REQ_LIST);
  for (ix = 0; ix < req_att_vals.num_of_handles; ix++) {
    req_att_vals.handles[ix] = handles[ix];
  }

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_MULTIPLEVALUES_ASYNC, &req_att_vals, p_att_vals);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_long_att_value_async(uint8_t *dev_addr,
 *                                                      uint16_t handle,
 *                                                      uint16_t offset,
 *                                                      rsi_ble_resp_att_value_t *p_att_vals)
 * @brief      Get the long attribute value by using handle and offset.
 *             \ref rsi_ble_on_event_read_resp_t callback function is called after the attribute value is received. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_read_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  offset 	- offset within the attribute value
 * @param[out] p_att_vals - attribute value filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.\n
 * 		@note	p_att_vals structure should be passed as NULL because nothing will be filled in this structure \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_long_att_value_async(uint8_t *dev_addr,
                                         uint16_t handle,
                                         uint16_t offset,
                                         rsi_ble_resp_att_value_t *p_att_vals)
{

  SL_PRINTF(SL_RSI_BLE_GET_LONG_ATT_VALUE_ASYNC, BLE, LOG_INFO, "HANDLE: %2x, OFFSET: %2x", handle, offset);
  rsi_ble_req_long_att_value_t req_long_att_val;
  memset(&req_long_att_val, 0, sizeof(req_long_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_long_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_long_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_long_att_val.handle = handle;
  req_long_att_val.offset = offset;

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_LONGDESCVALUES_ASYNC, &req_long_att_val, p_att_vals);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_att_value_async(uint8_t *dev_addr, uint16_t handle,
 *                                                 uint8_t data_len, uint8_t *p_data)
 * @brief      Set the attribute value of the remote device. \ref rsi_ble_on_event_write_resp_t
 *   		       callback function is called after the attribute set action is completed. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_write_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute value handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_set_att_value_async(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_SET_ATT_VALUE_ASYNC, BLE, LOG_INFO, "HANDLE: %2x, DATA_LEN: %1x", handle, data_len);
  rsi_ble_set_att_value_t set_att_val;
  memset(&set_att_val, 0, sizeof(set_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)set_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(set_att_val.handle, handle);
  set_att_val.length = RSI_MIN(sizeof(set_att_val.att_value), data_len);
  memcpy(set_att_val.att_value, p_data, set_att_val.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_DESCVALUE_ASYNC, &set_att_val, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_prepare_write_async(uint8_t *dev_addr,
 *                                                 uint16_t handle,
 *                                                 uint16_t offset,
 *                                                 uint8_t data_len,
 *                                                 uint8_t *p_data)
 * @brief      Prepare the attribute value. \ref rsi_ble_on_event_prepare_write_resp_t
 * 			       callback function is called after the prepare attribute write action is completed. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_prepare_write_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  offset 	- attribute value offset
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_prepare_write_async(uint8_t *dev_addr,
                                    uint16_t handle,
                                    uint16_t offset,
                                    uint8_t data_len,
                                    uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_PREPARE_WRITE_ASYNC,
            BLE,
            LOG_INFO,
            "HANDLE: %2x, OFFSET: %2x, DATA_LEN: %1x",
            handle,
            offset,
            data_len);
  rsi_ble_req_prepare_write_t req_prepare_write;
  memset(&req_prepare_write, 0, sizeof(req_prepare_write));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_prepare_write.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_prepare_write.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_prepare_write.handle, handle);
  rsi_uint16_to_2bytes(req_prepare_write.offset, offset);
  req_prepare_write.length = RSI_MIN(sizeof(req_prepare_write.att_value), data_len);
  memcpy(req_prepare_write.att_value, p_data, req_prepare_write.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_PREPAREWRITE_ASYNC, &req_prepare_write, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_execute_write_async(uint8_t *dev_addr, uint8_t exe_flag)
 * @brief      Execute the prepared attribute values. \ref rsi_ble_on_event_write_resp_t
 * 			       callback function is called after the execute attribute write action is completed. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_write_resp_t or \ref rsi_ble_on_gatt_error_resp_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  exe_flag - execute flag to write, possible values mentioned below\n
			0 - BLE_ATT_EXECUTE_WRITE_CANCEL \n
		 	1 - BLE_ATT_EXECUTE_PENDING_WRITES_IMMEDIATELY \n

 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D05  -  BLE Socket not available \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_execute_write_async(uint8_t *dev_addr, uint8_t exe_flag)
{
  SL_PRINTF(SL_RSI_BLE_EXECUTE_WRITE_ASYNC, BLE, LOG_INFO);
  rsi_ble_req_execute_write_t req_exe_write;
  memset(&req_exe_write, 0, sizeof(req_exe_write));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_exe_write.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_exe_write.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_exe_write.flag = exe_flag;

  return rsi_bt_driver_send_cmd(RSI_BLE_EXECUTE_LONGDESCWRITE_ASYNC, &req_exe_write, NULL);
}

/** @} */

/** @addtogroup BT-LOW-ENERGY3
* @{
*/
/*==============================================*/
/**
 * @fn         nt32_t rsi_ble_get_profiles(uint8_t *dev_addr,
 *                                         uint16_t start_handle,
 *                                         uint16_t end_handle,
 *                                         rsi_ble_resp_profiles_list_t *p_prof_list)
 * @brief      Get the supported profiles / services of the connected
 * 			       remote device. \ref rsi_ble_on_profiles_list_resp_t callback
 *  		       function will be called after the profiles list response is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_profiles_list_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address. 
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_profile_list 	- profiles/services information will be filled in this structure after retrieving from the remote device,
				  please refer rsi_ble_resp_profiles_list_s structure for more info.\n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_profiles(uint8_t *dev_addr,
                             uint16_t start_handle,
                             uint16_t end_handle,
                             rsi_ble_resp_profiles_list_t *p_prof_list)
{

  SL_PRINTF(SL_RSI_BLE_GET_PROFILES, BLE, LOG_INFO);
  rsi_ble_req_profiles_list_t req_profiles;
  memset(&req_profiles, 0, sizeof(req_profiles));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_profiles.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_profiles.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_profiles.start_handle = start_handle;
  req_profiles.end_handle   = end_handle;

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_PROFILES, &req_profiles, p_prof_list);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_profile(uint8_t *dev_addr, uuid_t profile_uuid,
 *                                         profile_descriptors_t *p_profile)
 * @brief      Get the specific profile / service of the connected remote device.
 * 			       \ref rsi_ble_on_profile_resp_t callback function is called after the service
 * 			       characteristics response is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_profile_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  profile_uuid 	- services/profiles which are searched using profile_uuid \n
 * @param[out] p_profile 	- profile / service information filled in this structure after retrieving from the remote device,
				  please refer profile_descriptor_s structure for more info. \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_profile(uint8_t *dev_addr, uuid_t profile_uuid, profile_descriptors_t *p_profile)
{

  SL_PRINTF(SL_RSI_BLE_GET_PROFILE, BLE, LOG_INFO);
  rsi_ble_req_profile_t req_profile;
  memset(&req_profile, 0, sizeof(req_profile));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_profile.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_profile.dev_addr, (int8_t *)dev_addr, 6);
#endif
  memcpy(&req_profile.profile_uuid, &profile_uuid, sizeof(uuid_t));

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_PROFILE, &req_profile, p_profile);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_char_services(uint8_t *dev_addr,
 *                                               uint16_t start_handle,
 *                                               uint16_t end_handle,
 *                                               rsi_ble_resp_char_services_t *p_char_serv_list)
 * @brief      Get the service characteristic services of the connected / remote device.
 * 			       \ref rsi_ble_on_char_services_resp_t callback function is called after the characteristic service
 * 			            response is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_char_services_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_char_services_list - service characteristics details are filled in this structure, please refer rsi_ble_resp_char_serv_s structure for more info. \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_char_services(uint8_t *dev_addr,
                                  uint16_t start_handle,
                                  uint16_t end_handle,
                                  rsi_ble_resp_char_services_t *p_char_serv_list)
{

  SL_PRINTF(SL_RSI_BLE_GET_CHAR_SERVICES, BLE, LOG_INFO);
  rsi_ble_req_char_services_t req_char_services;
  memset(&req_char_services, 0, sizeof(req_char_services));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_char_services.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_char_services.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_char_services.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_char_services.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_CHAR_SERVICES, &req_char_services, p_char_serv_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_inc_services(uint8_t *dev_addr,
 *                                             uint16_t start_handle,
 *                                             uint16_t end_handle,
 *                                             rsi_ble_resp_inc_services_t *p_inc_serv_list)
 * @brief      Get the supported include services of the connected / remote device.
 * 			       \ref rsi_ble_on_inc_services_resp_t callback function is called after
 * 			       the include service response is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_inc_services_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[out] p_inc_serv_list 	- include service characteristics details are filled in this structure, please refer rsi_ble_resp_inc_serv structure for more info.\n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_inc_services(uint8_t *dev_addr,
                                 uint16_t start_handle,
                                 uint16_t end_handle,
                                 rsi_ble_resp_inc_services_t *p_inc_serv_list)
{

  SL_PRINTF(SL_RSI_BLE_GET_INC_SERVICES, BLE, LOG_INFO);
  rsi_ble_req_inc_services_t req_inc_services;
  memset(&req_inc_services, 0, sizeof(req_inc_services));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_inc_services.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_inc_services.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_inc_services.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_inc_services.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_INC_SERVICES, &req_inc_services, p_inc_serv_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_char_value_by_uuid(uint8_t *dev_addr,
 *                                                    uint16_t start_handle,
 *                                                    uint16_t end_handle,
 *                                                    uuid_t char_uuid,
 *                                                    rsi_ble_resp_att_value_t *p_char_val)
 * @brief      Get the characteristic value by UUID (char_uuid).
 * 		    	   \ref rsi_ble_on_read_resp_t callback function is called after the attribute value is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_read_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
  * @param[in]  dev_addr 	- remote device address
 * @param[in]  start_handle 	- start handle (index) of the remote device's service records
 * @param[in]  end_handle 	- end handle (index) of the remote device's service records
 * @param[in]  char_uuid 	- UUID of the characteristic
 * @param[out] p_char_value 	- characteristic value is filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.\n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             If the return value is less than 0 \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_char_value_by_uuid(uint8_t *dev_addr,
                                       uint16_t start_handle,
                                       uint16_t end_handle,
                                       uuid_t char_uuid,
                                       rsi_ble_resp_att_value_t *p_char_val)
{

  SL_PRINTF(SL_RSI_BLE_GET_CHAR_VALUE_BY_UUID, BLE, LOG_INFO);
  rsi_ble_req_char_val_by_uuid_t req_char_val;
  memset(&req_char_val, 0, sizeof(req_char_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_char_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_char_val.dev_addr, (int8_t *)dev_addr, 6);
#endif

  rsi_uint16_to_2bytes(req_char_val.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_char_val.end_handle, end_handle);
  memcpy(&req_char_val.char_uuid, &char_uuid, sizeof(uuid_t));

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_READ_BY_UUID, &req_char_val, p_char_val);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_att_descriptors(uint8_t *dev_addr,
 *                                                 uint16_t start_handle,
 *                                                 uint16_t end_handle,
 *                                                 rsi_ble_resp_att_descs_t *p_att_desc)
 * @brief      Get the characteristic descriptors list from the remote device.
 * 			       \ref rsi_ble_on_att_desc_resp_t callback function is called after
 * 			       the attribute descriptors response is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_att_desc_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  start_handle - start handle (index) of the remote device's service records
 * @param[in]  end_handle - end handle (index) of the remote device's service records
 * @param[out] p_att_desc - pointer to characteristic descriptor structure, Please refer rsi_ble_resp_att_descs_s strcuture for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_att_descriptors(uint8_t *dev_addr,
                                    uint16_t start_handle,
                                    uint16_t end_handle,
                                    rsi_ble_resp_att_descs_t *p_att_desc)
{

  SL_PRINTF(SL_RSI_BLE_GET_ATT_DESCRIPTORS, BLE, LOG_INFO);
  rsi_ble_req_att_descs_t req_att_desc;
  memset(&req_att_desc, 0, sizeof(req_att_desc));

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_desc.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_desc.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_att_desc.start_handle, start_handle);
  rsi_uint16_to_2bytes(req_att_desc.end_handle, end_handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_DESC, &req_att_desc, p_att_desc);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_att_value(uint8_t *dev_addr, uint16_t handle, rsi_ble_resp_att_value_t *p_att_val)
 * @brief      Get the attribute by handle.
 * 			       \ref rsi_ble_on_read_resp_t callback function is called upon receiving the attribute value. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_read_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr	 - remote device address
 * @param[in]  handle	 - handle value of the attribute
 * @param[out] p_att_val - attribute value is filled in this structure, Please refer rsi_ble_resp_att_value_s structure for more info.\n
 * @return     0  		-  Success \n
 *             Non-Zero Value - Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_att_value(uint8_t *dev_addr, uint16_t handle, rsi_ble_resp_att_value_t *p_att_val)
{

  SL_PRINTF(SL_RSI_BLE_GET_ATT_VALUE, BLE, LOG_INFO);
  rsi_ble_req_att_value_t req_att_val;
  memset(&req_att_val, 0, sizeof(req_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_att_val.handle, handle);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_READ_VAL, &req_att_val, p_att_val);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_multiple_att_values(uint8_t *dev_addr,
 *                                                     uint8_t num_of_handlers,
 *                                                     uint16_t *handles,
 *                                                     rsi_ble_resp_att_value_t *p_att_vals)
 * @brief      Get the multiple attribute values by using multiple handles.
 * 			       \ref rsi_ble_on_read_resp_t callback function is called after the attribute value is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_read_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device address
 * @param[in]  num_of_handlers 	- number of handles in the list
 * @param[in]  handles 		- list of attribute handles
 * @param[out] p_att_vals 	- attribute values filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_multiple_att_values(uint8_t *dev_addr,
                                        uint8_t num_of_handlers,
                                        uint16_t *handles,
                                        rsi_ble_resp_att_value_t *p_att_vals)
{
  SL_PRINTF(SL_RSI_BLE_GET_MULTIPLE_ATT_VALUES, BLE, LOG_INFO, "NUMBER_OF_HANDLERS: %1x", num_of_handlers);
  rsi_ble_req_multi_att_values_t req_att_vals;
  memset(&req_att_vals, 0, sizeof(req_att_vals));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_att_vals.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_att_vals.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_att_vals.num_of_handles = RSI_MIN(num_of_handlers, RSI_BLE_MAX_REQ_LIST);
  for (ix = 0; ix < req_att_vals.num_of_handles; ix++) {
    req_att_vals.handles[ix] = handles[ix];
  }

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_MULTIPLE_READ, &req_att_vals, p_att_vals);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_long_att_value(uint8_t *dev_addr,
 *                                                uint16_t handle,
 *                                                uint16_t offset,
 *                                                rsi_ble_resp_att_value_t *p_att_vals)
 * @brief      Get the long attribute value by using handle and offset.
 * 			       \ref rsi_ble_on_read_resp_t callback function is called after the attribute value is received. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_read_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  offset 	- offset within the attribute value
 * @param[out] p_att_vals - attribute value filled in this structure, please refer rsi_ble_resp_att_value_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_get_long_att_value(uint8_t *dev_addr,
                                   uint16_t handle,
                                   uint16_t offset,
                                   rsi_ble_resp_att_value_t *p_att_vals)
{

  SL_PRINTF(SL_RSI_BLE_GET_LONG_ATT_VALUE, BLE, LOG_INFO, "HANDLES: %2x, OFFSET: %2x", handle, offset);
  rsi_ble_req_long_att_value_t req_long_att_val;
  memset(&req_long_att_val, 0, sizeof(req_long_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_long_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_long_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_long_att_val.handle = handle;
  req_long_att_val.offset = offset;

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_LONG_READ, &req_long_att_val, p_att_vals);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_att_value(uint8_t *dev_addr, uint16_t handle,
 *                                           uint8_t data_len, uint8_t *p_data)
 * @brief      Set the attribute value of the remote device.
 * 			       \ref rsi_ble_on_write_resp_t callback function is called if the attribute set action is completed. This is a non-blocking API,
 *                 Still user need to wait untill the callback \ref rsi_ble_on_write_resp_t is received from the device,
 *                 to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute value handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_set_att_value(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_SET_ATT_VALUE, BLE, LOG_INFO);
  rsi_ble_set_att_value_t set_att_val;
  memset(&set_att_val, 0, sizeof(set_att_val));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_att_val.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)set_att_val.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(set_att_val.handle, handle);
  set_att_val.length = RSI_MIN(sizeof(set_att_val.att_value), data_len);
  memcpy(set_att_val.att_value, p_data, set_att_val.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_WRITE, &set_att_val, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_att_cmd(uint8_t *dev_addr, uint16_t handle,
 *                                         uint8_t data_len, uint8_t *p_data)
 * @brief      Set the attribute value without waiting for an ACK from the remote device. This is a Blocking API.
 *             If the API returns RSI_ERROR_BLE_DEV_BUF_FULL  (-31) error then wait untill the \ref rsi_ble_on_le_more_data_req_t event gets received from the module.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute value handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4E60  -  Invalid Handle range \n
 *             0x4E62  -  Invalid Parameters \n
 *             0x4D04  -  BLE not connected \n
 *             0x4D05  -  BLE Socket not available \n
 *             0x4E65  -  Invalid Attribute Length When Small Buffer Mode is Configured \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_set_att_cmd(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_SET_ATT_COMMAND, BLE, LOG_INFO, "HANDLE: %2x, DATA_LEN: %1x", handle, data_len);
  rsi_ble_set_att_cmd_t set_att_cmd;
  memset(&set_att_cmd, 0, sizeof(set_att_cmd));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_att_cmd.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)set_att_cmd.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(set_att_cmd.handle, handle);
  set_att_cmd.length = RSI_MIN(sizeof(set_att_cmd.att_value), data_len);
  memcpy(set_att_cmd.att_value, p_data, set_att_cmd.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_WRITE_NO_ACK, &set_att_cmd, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_long_att_value(uint8_t *dev_addr,
 *                                             uint16_t handle,
 *                                             uint16_t offset,
 *                                             uint8_t data_len,
 *                                             uint8_t *p_data)
 * @brief      Set the long attribute value of the remote device. \ref rsi_ble_on_write_resp_t
 *             callback function is called after the attribute set action is completed. This is a non-blocking API,
 *             Still user need to wait untill the callback \ref rsi_ble_on_write_resp_t is received from the device,
 *             to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  offset 	- attribute value offset
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_set_long_att_value(uint8_t *dev_addr,
                                   uint16_t handle,
                                   uint16_t offset,
                                   uint8_t data_len,
                                   uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_SET_LONG_ATT_VALUE,
            BLE,
            LOG_INFO,
            "HANDLE: %2x, DATA_LEN: %1x, OFFSET: %2x",
            handle,
            data_len,
            offset);
  rsi_ble_set_long_att_value_t set_long_att;
  memset(&set_long_att, 0, sizeof(set_long_att));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_long_att.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)set_long_att.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(set_long_att.handle, handle);
  rsi_uint16_to_2bytes(set_long_att.offset, offset);
  set_long_att.length = RSI_MIN(sizeof(set_long_att.att_value), data_len);
  memcpy(set_long_att.att_value, p_data, set_long_att.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_LONG_WRITE, &set_long_att, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_prepare_write(uint8_t *dev_addr, uint16_t handle, uint16_t offset,
 *                                           uint8_t data_len, uint8_t *p_data)
 * @brief      Prepare the attribute value. \ref rsi_ble_on_write_resp_t
 * 			       callback function is called after the prepare attribute write action is completed. This is a non-blocking API,
 *             Still user need to wait untill the callback \ref rsi_ble_on_write_resp_t is received from the device,
 *             to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  offset 	- attribute value offset
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_prepare_write(uint8_t *dev_addr, uint16_t handle, uint16_t offset, uint8_t data_len, uint8_t *p_data)
{
  SL_PRINTF(SL_RSI_BLE_PREPARE_WRITE,
            BLE,
            LOG_INFO,
            "HANDLE: %2x, DATA_LEN: %1x, OFFSET: %2x",
            handle,
            data_len,
            offset);
  rsi_ble_req_prepare_write_t req_prepare_write;
  memset(&req_prepare_write, 0, sizeof(req_prepare_write));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_prepare_write.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_prepare_write.dev_addr, (int8_t *)dev_addr, 6);
#endif
  rsi_uint16_to_2bytes(req_prepare_write.handle, handle);
  rsi_uint16_to_2bytes(req_prepare_write.offset, offset);
  req_prepare_write.length = RSI_MIN(sizeof(req_prepare_write.att_value), data_len);
  memcpy(req_prepare_write.att_value, p_data, req_prepare_write.length);

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_PREPARE_WRITE, &req_prepare_write, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_execute_write(uint8_t *dev_addr, uint8_t exe_flag)
 * @brief      Execute the prepared attribute values. \ref rsi_ble_on_write_resp_t
 * 			   callback function is called after the execute attribute write action is completed. This is a non-blocking API,
 *             Still user need to wait untill the callback \ref rsi_ble_on_write_resp_t is received from the device,
 *             to initiate further attribute related transactions on this remote device address.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  exe_flag - execute flag to write, possible values mentioned below\n
			0 - BLE_ATT_EXECUTE_WRITE_CANCEL \n
		 	1 - BLE_ATT_EXECUTE_PENDING_WRITES_IMMEDIATELY \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_execute_write(uint8_t *dev_addr, uint8_t exe_flag)
{

  SL_PRINTF(SL_RSI_BLE_EXECUTABLE_WRITE, BLE, LOG_INFO);
  rsi_ble_req_execute_write_t req_exe_write;
  memset(&req_exe_write, 0, sizeof(req_exe_write));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(req_exe_write.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)req_exe_write.dev_addr, (int8_t *)dev_addr, 6);
#endif
  req_exe_write.flag = exe_flag;

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_EXECUTE_WRITE, &req_exe_write, NULL);
}

/** @} */

/** @addtogroup BT-LOW-ENERGY5
* @{
*/

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_add_service(uuid_t service_uuid, rsi_ble_resp_add_serv_t *p_resp_serv)
 * @brief      Add a new service to the local GATT Server. This is a Blocking API.
 * @pre        Call \ref rsi_wireless_init() before calling this API. 
 * @param[in]  service_uuid 	- new service UUID value, please refer uuid_s structure for more info.
 * @param[out] p_resp_serv 	- new service handler filled in this structure, please refer rsi_ble_resp_add_serv_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4046  -  Invalid Arguments \n
 *             0x4D08  -  Profile record full \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_add_service(uuid_t service_uuid, rsi_ble_resp_add_serv_t *p_resp_serv)
{

  SL_PRINTF(SL_RSI_BLE_ADD_SERVICE, BLE, LOG_INFO);
  rsi_ble_req_add_serv_t new_service;
  memset(&new_service, 0, sizeof(new_service));

  memcpy(&new_service.service_uuid, &service_uuid, sizeof(uuid_t));

  return rsi_bt_driver_send_cmd(RSI_BLE_ADD_SERVICE, &new_service, p_resp_serv);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_add_attribute(rsi_ble_req_add_att_t *p_attribute)
 * @brief      Add a new attribute to a specific service. This is a Blocking API.
 * @pre        Call \ref rsi_wireless_init() before calling this API. 
 * @param[in]  p_attribute - add a new attribute to the service, please refer rsi_ble_req_add_att_s structure for more info. \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4046  -  Invalid Arguments \n
 *             0x4D09  -  Attribute record full  \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */

int32_t rsi_ble_add_attribute(rsi_ble_req_add_att_t *p_attribute)
{

  SL_PRINTF(SL_RSI_BLE_ADD_ATTRIBUTE, BLE, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BLE_ADD_ATTRIBUTE, p_attribute, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_local_att_value(uint16_t handle, uint16_t data_len,
 *                                                 uint8_t *p_data)
 * @brief      Change the local attribute value. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  handle 	- attribute value handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4046  -  Invalid Arguments \n
 *             0x4D06  -  Attribute record not found \n
 *             0x4E60  -  Invalid Handle Range \n
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 * @note      This API can only be used if the service is maintained inside the firmware. \n
 *            The services which are maintained by firmware must follow the below rules. \n
 *            Rule 1: The attribute_data_size is less than 20 bytes during the service_creation \n
 *            Rule 2: while creating the service, don't use the RSI_BLE_ATT_MAINTAIN_IN_HOST bit \n
 *                    in the RSI_BLE_ATT_CONFIG_BITMAP macro. \n
 *            Rule 3: The data_len must be less than or equal to the dat_length mentioned while \n
 *                    creating the service/attribute \n
 *		      Rule 4: If the services are maintained in the Application/Host,\n
 *		      then need to use \ref rsi_ble_notify_value() API to send the notifications to the remote devices.\n
 */

int32_t rsi_ble_set_local_att_value(uint16_t handle, uint16_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_SET_LOCAL_ATT_VALUE, BLE, LOG_INFO, "HANDLE: %2x", handle);
  rsi_ble_set_local_att_value_t rec_data = { 0 };

  rec_data.handle   = handle;
  rec_data.data_len = RSI_MIN(data_len, sizeof(rec_data.data));
  memcpy(rec_data.data, p_data, rec_data.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_LOCAL_ATT_VALUE, &rec_data, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_set_wo_resp_notify_buf_info(uint8_t *dev_addr, uint8_t buf_mode,
 *                                                         uint8_t buf_cnt)
 * @brief      Configure the buf mode for Notify and WO response commands for the remote device. This is a Blocking API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  buf_mode - buffer mode configuration\n
			 0 - BLE_SMALL_BUFF_MODE \n
			 1 - BLE_BIG_BUFF_MODE \n
 * @param[in]  buf_count - no of buffers to be configured \n
			only value 1 and 2 are supporetd in BLE_SMALL_BUFF_MODE  \n

			in BLE_BIG_BUFF_MODE, buffers allocated based on the below notations.
			intial available_buf_cnt = RSI_BLE_NUM_CONN_EVENTS,
			a) When connection 1 is formed, the possible range of buffers is (available_buf_cnt - remaining possible no.connections)
			b) After allocating X buffers using \ref rsi_ble_set_wo_resp_notify_buf_info to the 1st connection remaiining available_buf_cnt = (available_buf_cnt - X ) \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4046  -  Invalid Arguments \n
 *             0x4D05  -  BLE socket not available \n 
 *             0x4D06  -  Attribute record not found \n
 *             0x4E60  -  Invalid Handle Range \n
 *             0x4E63  -  BLE Buffer Count Exceeded \n
 *             0x4E64  -  BLE Buffer already in use  \n 
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *
 */
int32_t rsi_ble_set_wo_resp_notify_buf_info(uint8_t *dev_addr, uint8_t buf_mode, uint8_t buf_cnt)
{

  SL_PRINTF(SL_RSI_BLE_SET_WO_RESP_NOTIFY_BUF_INFO, BLE, LOG_INFO, "BUF_MODE: %1x, BUF_COUNT: %1x", buf_mode, buf_cnt);
  rsi_ble_set_wo_resp_notify_buf_info_t buf_info = { 0 };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(buf_info.dev_addr, dev_addr);
#else
  memcpy(buf_info.dev_addr, dev_addr, RSI_DEV_ADDR_LEN);
#endif
  buf_info.buf_mode  = buf_mode;
  buf_info.buf_count = buf_cnt;

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_SET_WWO_RESP_NOTIFY_BUF_INFO, &buf_info, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_notify_value(uint8_t *dev_addr, uint16_t handle,
 *                                          uint16_t data_len, uint8_t *p_data)
 * @brief      Notify the local value to the remote device. This is a Blocking API.
 *             If the API returns RSI_ERROR_BLE_DEV_BUF_FULL  (-31) error then wait untill the \ref rsi_ble_on_le_more_data_req_t event gets received from the module.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- local attribute handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4046  -  Invalid Arguments \n
 *             0x4A0D  -  Invalid attribute value length \n
 *             0x4D05  -  BLE socket not available \n 
 *             0x4D06  -  Attribute record not found \n
 *             0x4E60  -  Invalid Handle Range \n
 *             0x4E65  -  Invalid Attribute Length When Small Buffer Mode is Configured  \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 *
 * @note	If the services are maintained in the Application/Host,\n
 * 			then need to use \ref rsi_ble_notify_value() API instead of using \ref rsi_ble_set_local_att_value() API\n
 *			to send the notifications to the remote devices.
 */
int32_t rsi_ble_notify_value(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_NOTIFY_VALUE_TRIGGER, BLE, LOG_INFO, "HANDLE: %2x", handle);
  rsi_ble_notify_att_value_t rec_data;
  memset(&rec_data, 0, sizeof(rec_data));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(rec_data.dev_addr, dev_addr);
#else
  memcpy(rec_data.dev_addr, dev_addr, 6);
#endif

  rec_data.handle   = handle;
  rec_data.data_len = RSI_MIN(data_len, sizeof(rec_data.data));
  memcpy(rec_data.data, p_data, rec_data.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_NOTIFY, &rec_data, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_indicate_value(uint8_t *dev_addr, uint16_t handle,
 *                                            uint16_t data_len, uint8_t *p_data)
 * @brief      Indicate the local value to the remote device. This is a blocking API and can unblock the application
 *             on the reception of the callback functions either \ref rsi_ble_on_event_indicate_confirmation_t.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- local attribute handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D05  -  BLE socket not available \n 
 *             0x4E60  -  Invalid Handle Range \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 *
 */

int32_t rsi_ble_indicate_value(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_INDICATE_VOLUME_TRIGGER, BLE, LOG_INFO);
  rsi_ble_notify_att_value_t rec_data;
  memset(&rec_data, 0, sizeof(rec_data));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(rec_data.dev_addr, dev_addr);
#else
  memcpy(rec_data.dev_addr, dev_addr, 6);
#endif

  rec_data.handle   = handle;
  rec_data.data_len = RSI_MIN(data_len, sizeof(rec_data.data));
  memcpy(rec_data.data, p_data, rec_data.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_INDICATE, &rec_data, NULL);
}
/** @} */
/** @addtogroup BT-LOW-ENERGY4
 * * @{ */
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_indicate_value_sync(uint8_t *dev_addr, uint16_t handle,
 *                                            uint16_t data_len, uint8_t *p_data)
 * @brief      Indicate the local value to the remote device. This is a blocking API. \n
 *             This will not send any confirmation event to the application instead  \n
 *             send the status as success on receiving confirmation from remote side.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- local attribute handle
 * @param[in]  data_len - attribute value length
 * @param[in]  p_data 	- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D05  -  BLE socket not available \n 
 *             0x4E60  -  Invalid Handle Range \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 *
 */

int32_t rsi_ble_indicate_value_sync(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_INDICATE_VALUE_SYNC, BLE, LOG_INFO);
  rsi_ble_notify_att_value_t rec_data;
  memset(&rec_data, 0, sizeof(rec_data));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(rec_data.dev_addr, dev_addr);
#else
  memcpy(rec_data.dev_addr, dev_addr, 6);
#endif

  rec_data.handle   = handle;
  rec_data.data_len = RSI_MIN(data_len, sizeof(rec_data.data));
  memcpy(rec_data.data, p_data, rec_data.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_INDICATE_SYNC, &rec_data, NULL);
}
/** @} */

/** @addtogroup BT-LOW-ENERGY4
* @{
*/
/*==============================================*/
/**
 * @fn        int32_t rsi_ble_indicate_confirm(uint8_t *dev_addr)
 * @brief     Send indicate confirmation to the remote device. This is a blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in] dev_addr - remote device address 
 * @return    0			-	Success \n
 *            Non-Zero Value	-	Failure
 *            0x4D05  -  BLE socket not available \n 
 * @note      Refer Error Codes section for above error codes \ref error-codes \n 
 *
 */
int32_t rsi_ble_indicate_confirm(uint8_t *dev_addr)
{

  SL_PRINTF(SL_RSI_BLE_INDICATE_CONFIRM, BLE, LOG_INFO);
  rsi_ble_indicate_confirm_t rec_data = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(rec_data.dev_addr, dev_addr);
#else
  memcpy(rec_data.dev_addr, dev_addr, 6);
#endif

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_INDICATE_CONFIRMATION, &rec_data, NULL);
}
/** @} */

/** @addtogroup BT-LOW-ENERGY5
* @{
*/

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_get_local_att_value(uint16_t handle,
 *                                                 rsi_ble_resp_local_att_value_t *p_resp_local_att_val)
 * @brief      Get the local attribute value. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API. 
 * @param[in]  handle 			- local attribute handle
 * @param[out] p_resp_local_att_val 	- local attribute value filled in this structure, plase refer rsi_ble_resp_local_att_value_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4046  -  Invalid Arguments \n 
 *             0x4D06  -  Attribute record not found \n 
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 * @note      This API can only be used if the service is maintained inside the firmware. The services which are maintained by firmware must
 * follow the below rules.\n
 * Rule 1: The attribute_data_size is less than 20 bytes during the service_creation \n
 * Rule 2: While creating the service, don't use the RSI_BLE_ATT_MAINTAIN_IN_HOST bit in the RSI_BLE_ATT_CONFIG_BITMAP macro.
 *
 */
int32_t rsi_ble_get_local_att_value(uint16_t handle, rsi_ble_resp_local_att_value_t *p_resp_local_att_val)
{

  SL_PRINTF(SL_RSI_BLE_GET_LOCAL_ATT_VALUE, BLE, LOG_INFO);
  rsi_ble_get_local_att_value_t local_read_val = { 0 };

  local_read_val.handle = handle;

  return rsi_bt_driver_send_cmd(RSI_BLE_GET_LOCAL_ATT_VALUE, &local_read_val, p_resp_local_att_val);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_gatt_read_response(uint8_t *dev_addr,
 *                                                uint8_t read_type,
 *                                                uint16_t handle,
 *                                                uint16_t offset,
 *                                                uint16_t length,
 *                                                uint8_t *p_data)
 * @brief      Send the response for the read request received from the remote device. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr 	- remote device Address
 * @param[in]  read_type 	- read value type \n
				0 - Read response \n
				1 - Read blob response
 * @param[in]  handle 		- attribute value handle
 * @param[in]  offset 		- attribute value offset
 * @param[in]  length 		- attribute value length
 * @param[in]  p_data 		- attribute value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D04  -  BLE not connected \n 
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_gatt_read_response(uint8_t *dev_addr,
                                   uint8_t read_type,
                                   uint16_t handle,
                                   uint16_t offset,
                                   uint16_t length,
                                   uint8_t *p_data)
{

  SL_PRINTF(SL_RSI_BLE_GATT_READ_RESPONSE, BLE, LOG_INFO);
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(offset);
  UNUSED_PARAMETER(handle);
  rsi_ble_gatt_read_response_t local_read_blob_resp;
  memset(&local_read_blob_resp, 0, sizeof(local_read_blob_resp));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(local_read_blob_resp.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)local_read_blob_resp.dev_addr, (int8_t *)dev_addr, 6);
#endif
  local_read_blob_resp.type     = read_type;
  local_read_blob_resp.data_len = RSI_MIN(length, sizeof(local_read_blob_resp.data));
  memcpy(local_read_blob_resp.data, p_data, local_read_blob_resp.data_len); //local_read_blob_resp.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_READ_RESP, &local_read_blob_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_remove_gatt_service(uint32_t service_handler)
 * @brief      Remove the GATT service record. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  service_handle - GATT service record handle
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D0A  -  BLE profile not found (profile handler invalid) \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_remove_gatt_service(uint32_t service_handler)
{

  SL_PRINTF(SL_RSI_BLE_REMOVE_GATT_SERVICE, BLE, LOG_INFO, "SERVICE_HANDLER: %4x", service_handler);
  rsi_ble_gatt_remove_serv_t rem_serv;

  rem_serv.serv_hndler = service_handler;

  return rsi_bt_driver_send_cmd(RSI_BLE_RSP_REMOVE_SERVICE, &rem_serv, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_remove_gatt_attibute(uint32_t service_handler, uint16_t att_hndl)
 * @brief      Remove the GATT attribute record. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  service_handle - GATT service record handle
 * @param[in]  att_hndl - attribute handle
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4D06  -  Attribute record not found  \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_remove_gatt_attibute(uint32_t service_handler, uint16_t att_hndl)
{

  SL_PRINTF(SL_RSI_BLE_REMOVE_GATT_ATTRIBUTE,
            BLE,
            LOG_INFO,
            "SERVICE_HANDLER: %4x, ATT_HANDLE: %2x",
            service_handler,
            att_hndl);
  rsi_ble_gatt_remove_att_t rem_att;

  rem_att.serv_hndler = service_handler;
  rem_att.att_hndl    = att_hndl;

  return rsi_bt_driver_send_cmd(RSI_BLE_RSP_REMOVE_ATTRIBUTE, &rem_att, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_att_error_response(uint8_t *dev_addr, uint16_t handle,
 *												  uint8_t opcode, uint8_t err)
 * @brief      Send attribute error response for any of the att request. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  handle 	- attribute handle
 * @param[in]  opcode 	- error response opcode
 * @param[in]  error_code - specific error related Gatt
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4D04  -  BLE not Connected  \n
 *             0x4E62  -  Invalid Parameters \n 
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_att_error_response(uint8_t *dev_addr, uint16_t handle, uint8_t opcode, uint8_t err)
{

  SL_PRINTF(SL_RSI_BLE_ATT_ERROR_RESPONSE, BLE, LOG_INFO, "HANDLE: %2x", handle);
  rsi_ble_att_error_response_t att_error = { 0 };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_error.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)att_error.dev_addr, (int8_t *)dev_addr, 6);
#endif

  att_error.att_handle = handle;
  att_error.req_opcode = opcode;
  att_error.err_code   = err;

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_ATT_ERROR, &att_error, NULL);
}
/** @} */

/** @addtogroup BT-LOW-ENERGY3
* @{
*/

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_mtu_exchange_event(uint8_t *dev_addr, uint8_t mtu_size)
 * @brief      Initiates the MTU exchange request with the remote device.  \n
 *             This is a Blocking API and will recive a callback event \ref rsi_ble_on_mtu_event_t as the response for this API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address 
 * @param[in]  mtu_size - requested MTU value
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4D04  -  BLE not Connected  \n
 *             0x4E62  -  Invalid Parameters \n 
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_mtu_exchange_event(uint8_t *dev_addr, uint8_t mtu_size)
{

  SL_PRINTF(SL_RSI_BLE_MTU_EXCHANGE_EVENT, BLE, LOG_INFO);
  rsi_ble_mtu_exchange_t mtu_req;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(mtu_req.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)mtu_req.dev_addr, (int8_t *)dev_addr, 6);
#endif
  mtu_req.req_mtu_size = mtu_size;

  return rsi_bt_driver_send_cmd(RSI_BLE_MTU_EXCHANGE_REQUEST, &mtu_req, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ble_mtu_exchange_resp(uint8_t *dev_addr, uint8_t mtu_size)
 * @brief      This function (Exchange MTU Response) is sent in reply to a received Exchange MTU Request. 
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - Remote Device Address
 * @param[in]  mtu_size - requested MTU value
 * @return     0		-	Success \n
 *             0x4D0C   -   When RSI_BLE_MTU_EXCHANGE_FROM_HOST BIT is not SET.
 *             0x4D05   -   BLE Socket Not Available.
 *             Non-Zero Value	-	Failure
 *             Refer Error Codes section for above error codes \ref error-codes
 *
 */
int32_t rsi_ble_mtu_exchange_resp(uint8_t *dev_addr, uint8_t mtu_size)
{
  rsi_ble_mtu_exchange_resp_t mtu_resp;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(mtu_resp.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)mtu_resp.dev_addr, (int8_t *)dev_addr, 6);
#endif
  mtu_resp.req_mtu_size = mtu_size;

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_MTU_EXCHANGE_RESP, &mtu_resp, NULL);
}

/** @} */

/** @addtogroup BT-LOW-ENERGY5
* @{
*/

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_gatt_write_response(uint8_t *dev_addr, uint8_t type)
 * @brief      Send the response to the write request received from the remote device. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address
 * @param[in]  type 	- response type \n
			0 - write response, \n
			1 - execute write response.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4046  -  Invalid Arguments  \n 
 *             0x4D04  -  BLE not Connected  \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_gatt_write_response(uint8_t *dev_addr, uint8_t type)
{

  SL_PRINTF(SL_RSI_BLE_GATT_WRITE_RESPONSE, BLE, LOG_INFO);
  rsi_ble_gatt_write_response_t local_write_resp;
  memset(&local_write_resp, 0, sizeof(local_write_resp));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(local_write_resp.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)local_write_resp.dev_addr, (int8_t *)dev_addr, 6);
#endif
  local_write_resp.type = type;

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_WRITE_RESP, &local_write_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_ble_gatt_prepare_write_response(uint8_t *dev_addr,
 *                                                         uint16_t handle,
 *                                                         uint16_t offset,
 *                                                         uint16_t length,
 *                                                         uint8_t *data)
 * @brief      Send the response for the prepare write requests received from the remote device. This is a Blocking API.
 * @pre        \ref rsi_ble_connect() API needs to be called before this API.
 * @param[in]  dev_addr - remote device address 
 * @param[in]  handle 	- attribute value handle
 * @param[in]  offset 	- attribute value offset
 * @param[in]  data_len - attribute value length
 * @param[in]  data 	- attribite value 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 *             0x4046  -  Invalid Arguments  \n 
 *             0x4D04  -  BLE not Connected  \n
 * @note       Refer Error Codes section for above error codes \ref error-codes \n 
 */
int32_t rsi_ble_gatt_prepare_write_response(uint8_t *dev_addr,
                                            uint16_t handle,
                                            uint16_t offset,
                                            uint16_t length,
                                            uint8_t *data)
{

  SL_PRINTF(SL_RSI_BLE_GATT_PREPARE_WRITE_RESPONSE,
            BLE,
            LOG_INFO,
            "HANDLE: %2x, LENGTH: %2x, OFFSET: %2x",
            handle,
            length,
            offset);
  rsi_ble_gatt_prepare_write_response_t local_prepare_write_resp;
  memset(&local_prepare_write_resp, 0, sizeof(local_prepare_write_resp));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(local_prepare_write_resp.dev_addr, dev_addr);
#else
  memcpy((uint8_t *)local_prepare_write_resp.dev_addr, (int8_t *)dev_addr, 6);
#endif
  local_prepare_write_resp.handle   = handle;
  local_prepare_write_resp.offset   = offset;
  local_prepare_write_resp.data_len = length;

  memcpy(local_prepare_write_resp.data, data, length);

  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_PREPARE_WRITE_RESP, &local_prepare_write_resp, NULL);
}

#endif
/** @} */
