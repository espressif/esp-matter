/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __NVDM_H__
#define __NVDM_H__

#ifdef MTK_NVDM_ENABLE

/**
 * @addtogroup NVDM
 * @{
 * This section introduces the NVDM APIs including terms and acronyms,
 * supported features, software architecture, details on how to use the NVDM, enums, structures, typedefs and functions.
 *
 * @section NVDM_Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                |
 * |----------------------|----------------------------------|
 * |\b NVDM             | Non-volatile Data Management is a middleware used to store user data. |
 *
 * @section NVDM_Features_Chapter Supported features
 *
 * - \b Support \b retaining \b data \b after \b power \b is \b off. \n
 *   NVDM stores user data in a flash memory, so the data is preserved even after the power is off. \n
 * - \b Support \b grouping \b data \b items. \n
 *   The NVDM stores data items based on group names.
 *   The user can define different data items with different group names.
 *   This enables the data item classification and keeps them in order.
 *
 * @section NVDM_APIs_Usage_Chapter How to use the NVDM APIs
 *
 * - To use the NVDM services follow the steps, as shown below. \n
 *  - Step 1. Call #nvdm_init() to initialize the NVDM module.
 *  - Step 2. Call #nvdm_write_data_item() to write or update data item to the flash.
 *  - Step 3. Call #nvdm_read_data_item() to read data item from the flash.
 *  - Step 4. Call #nvdm_query_begin() to start the query request.
 *  - Step 5. Call #nvdm_query_next_group_name() to query the next available data item group.
 *  - Step 6. Call #nvdm_query_next_data_item_name() to query the next available data item from the group queried in Step 5.
 *  - Step 7. Call #nvdm_query_end() to end the query.
 *  - Step 8. Call #nvdm_delete_data_item() to delete a specific data item in flash.
 *  - sample code:
 *    @code
 *       #define STA_IPADDR_DEFAULT_VALUE "192.168.0.1"
 *
 *       void nvdm_application(void)
 *       {
 *           nvdm_status_t status;
 *           uint32_t size;
 *           uint8_t buffer[16];
 *           char group_name[64];
 *           char data_item_name[64];
 *
 *           status = nvdm_init();
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           status = nvdm_write_data_item("STA", "IpAddr", DATA_ITEM_TYPE_STRING, (uint8_t *)STA_IPADDR_DEFAULT_VALUE, sizeof(STA_IPADDR_DEFAULT_VALUE));
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           size = sizeof(buffer);
 *           status = nvdm_read_data_item("STA", "IpAddr", buffer, &size);
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           status = nvdm_query_begin();
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *           status = nvdm_query_next_group_name(group_name);
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *           status = nvdm_query_next_data_item_name(data_item_name);
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *           status = nvdm_query_end();
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           status = nvdm_delete_data_item(group_name, data_item_name);
 *           if (status != NVDM_STATUS_OK) {
 *               //Error handler;
 *           }
 *       }
 *    @endcode
 *
 * @section NVDM_Region_Adjust_Chapter How to adjust the NVDM region
 *
 * - There is a special flash region assigned to the NVDM to store user defined data.
 * For more information on the NVDM memory layout, please refer to the Memory Layout Developer's Guide.
 * - To modify start address and size of the NVDM region, apply the following steps.
 *  - Step 1. Follow chapters "Memory layout adjustment with a linker script" and
 *     "Memory layout adjustment with a scatter file" in Memory Layout Developer's Guide
 *     to modify the start address and size of the NVDM.
 *  - Step 2. Modify flash_map.h or memory_map.h header files under /project/\<board\>/apps/\<project\>/inc/ according to the adjustments in Step 1.
 *   - For 7687 and 7697, user needs to modify flash_map.h under /project/\<board\>/apps/\<project\>/inc/.
 *      Modify the CM4_FLASH_USR_CONF_ADDR to adjust the start address of the NVDM region.
 *      Modify the FLASH_USR_CONF_SIZE to adjust the size of the NVDM region.
 *   - For other chips, modify the memory_map.h header file under /project/\<board\>/apps/\<project\>/inc/.
 *      Modify the ROM_NVDM_BASE to adjust the start address of the NVDM region.
 *      Modify the ROM_NVDM_LENGTH to adjust the size of the NVDM region.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup NVDM_enum Enum
  * @{
  */

/** @brief This enum defines the display type of a data item. */
typedef enum {
    NVDM_DATA_ITEM_TYPE_RAW_DATA = 0x01,   /**< Defines the display type with raw data. */
    NVDM_DATA_ITEM_TYPE_STRING = 0x02,     /**< Defines the display type with string. */
} nvdm_data_item_type_t;

/** @brief This enum defines return type of NVDM APIs. */
typedef enum {
    NVDM_STATUS_INVALID_PARAMETER = -5,  /**< The user parameter is invalid. */
    NVDM_STATUS_ITEM_NOT_FOUND = -4,     /**< The data item wasn't found by the NVDM. */
    NVDM_STATUS_INSUFFICIENT_SPACE = -3, /**< No space is available in the flash. */
    NVDM_STATUS_INCORRECT_CHECKSUM = -2, /**< The NVDM found a checksum error when reading the data item. */
    NVDM_STATUS_ERROR = -1,              /**< An unknown error occurred. */
    NVDM_STATUS_OK = 0,                  /**< The operation was successful. */
} nvdm_status_t;

#ifdef SYSTEM_DAEMON_TASK_ENABLE
/** @brief  This defines the callback function prototype.
 *          User should register a callback function when calling #nvdm_write_data_item_non_blocking(),
 *          this function is called after the write operation is done or the error is detected during the write opeartion.
 *          User must check the status to see whether the write operation is successful.
 *          The callback runs in the context of the specific task with very low priority. And the user should not do the time-consuming things .
 *  @param [in] status is the status of write operation.
 *  @param [in] user_data is a user-defined parameter.
 */
typedef void (*nvdm_user_callback_t)(nvdm_status_t status, void *user_data);
#endif

/**
  * @}
  */

/**
 * @brief     This function initializes the NVDM module to enable the NVDM services.
 *            User should not call #nvdm_read_data_item() or #nvdm_write_data_item() before #nvdm_init().
 *            Call #nvdm_init() only once during the initialization.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 */
nvdm_status_t nvdm_init(void);

/**
 * @brief     This function is used to write or update a data item to flash.
 *            If there are existing data items on the flash, the NVDM invalidates old values and replaces with new ones.
 *            No other operation is performed while the data is being written to the flash.
 *            This function returns after the data is completely written to the flash.
 *            User should check the return status to verify whether the data is written to flash successfully.
 * @param[in] type   is the data item type, which defines how the value is displayed.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as "address" of the "Bluetooth".
 * @param[in] buffer   is a pointer to the data item's content.
 * @param[in] size   is the size of the data item's content.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_INSUFFICIENT_SPACE, if the storage space is not enough. \n
 * @note      Call this API in tasks or function callbacks, but not in the interrupt handlers.
 */
nvdm_status_t nvdm_write_data_item(const char *group_name,
                                   const char *data_item_name,
                                   nvdm_data_item_type_t type,
                                   const uint8_t *buffer,
                                   uint32_t size);

#ifdef SYSTEM_DAEMON_TASK_ENABLE
/**
 * @brief     This function is used to write or update a data item to flash with non-blocking method.
 *            If there are existing data items on the flash, the NVDM invalidates old values and replaces with new ones.
 *            No other operation is performed while the data is being written to the flash.
 *            This function returns immediately after the write request is issued or the error is detected.
 *            User's callback is called after the write operation is done or the error is detected during the write opeartion.
 *            User should check both the return status of the API and the status of the callback to verify whether the data is written to flash successfully.
 *            The callback runs in the context of the specific task with very low priority. And the user should not do the time-consuming things .
 * @param[in] type   is the data item type, which defines how the value is displayed.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as "address" of the "Bluetooth".
 * @param[in] buffer   is a pointer to the data item's content.
 * @param[in] size   is the size of the data item's content.
 * @param[in] callback   is the user's callback to be noticed later.
 * @param[in] user_data   is the user's data to be used in the callback.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 * @note      Call this API in tasks or function callbacks, but not in the interrupt handlers.
 */
nvdm_status_t nvdm_write_data_item_non_blocking(const char *group_name,
                                                const char *data_item_name,
                                                nvdm_data_item_type_t type,
                                                const uint8_t *buffer,
                                                uint32_t size,
                                                const nvdm_user_callback_t callback,
                                                const void *user_data);
#endif

/**
 * @brief     This function is used to read the data item from flash.
 *            If there is no such data item, it returns #NVDM_STATUS_ITEM_NOT_FOUND.
 *            User should check the return status to verify whether the data read from the flash is correct.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as an "address" of "Bluetooth".
 * @param[out] buffer   is a pointer to the user buffer, that stores the data item content read from the flash.
 * @param[in,out] size   is the user buffer size when used as an input and is the size of actual data item's content read out when used as an output.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item is not found. \n
 *                #NVDM_STATUS_INCORRECT_CHECKSUM, if the checksum of data item is invalid. \n
 * @note      Call this API in tasks or function callbacks, but not in the interrupt handlers.
 */
nvdm_status_t nvdm_read_data_item(const char *group_name,
                                  const char *data_item_name,
                                  uint8_t *buffer,
                                  uint32_t *size);

/**
 * @brief     This function is used to read the data item from flash.
 *            If there is no such data item, it returns #NVDM_STATUS_ITEM_NOT_FOUND.
 *            User should check the return status to verify whether the data read from the flash is correct.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as an "address" of "Bluetooth".
 * @param[out] type   is the data item type, which defines how the value is displayed.
 * @param[out] buffer   is a pointer to the user buffer, that stores the data item content read from the flash.
 * @param[in,out] size   is the user buffer size when used as an input and is the size of actual data item's content read out when used as an output.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item is not found. \n
 *                #NVDM_STATUS_INCORRECT_CHECKSUM, if the checksum of data item is invalid. \n
 * @note      Call this API in tasks or function callbacks, but not in the interrupt handlers.
 */
nvdm_status_t nvdm_read_data_item_with_type(const char *group_name,
                                            const char *data_item_name,
                                            nvdm_data_item_type_t *type,
                                            uint8_t *buffer,
                                            uint32_t *size);

/**
 * @brief     This function deletes a single data item stored in the NVDM region.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as "address" of the "Bluetooth".
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item is not found. \n
 */
nvdm_status_t nvdm_delete_data_item(const char *group_name, const char *data_item_name);

/**
 * @brief     This function is used to delete all data items in the group.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 */
nvdm_status_t nvdm_delete_group(const char *group_name);

/**
 * @brief     This function is used to delete all data items stored in the NVDM region.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 * @note
 *                It's dangerous to call this API, as this will remove everything in NVDM region.
 */
nvdm_status_t nvdm_delete_all(void);

/**
 * @brief     This function marks the start of the query.
 *                User should call this API before #nvdm_query_next_group_name() and
 *                #nvdm_query_next_data_item_name() to query the naming information of data items.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 */
nvdm_status_t nvdm_query_begin(void);

/**
 * @brief     This function marks the end of the query.
 *                User should call this API after #nvdm_query_next_group_name() and
 *                #nvdm_query_next_data_item_name() to query the naming information of data items.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 */
nvdm_status_t nvdm_query_end(void);

/**
 * @brief     This function is used to enumerate all available group names.
 *                The next available group name is returned each time user calls this API.
 *                If the user calls the API after all available group names are returned,
 *                the return value will be #NVDM_STATUS_ITEM_NOT_FOUND.
 * @param[out] group_name   return the name of the next group available.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 */
nvdm_status_t nvdm_query_next_group_name(char *group_name);

/**
 * @brief     This function is used to enumerate all available data item names
 *                from the group returned by #nvdm_query_next_group_name().
 *                The next available data item name is returned each time user calls this API.
 *                If the user calls the API after all available data item names are returned,
 *                the return value will be #NVDM_STATUS_ITEM_NOT_FOUND.
 * @param[out] data_item_name   return the name of next data item belonging the specifc group.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 */
nvdm_status_t nvdm_query_next_data_item_name(char *data_item_name);

/**
 * @brief     This function is used to query data item size.
 *            If there is no such data item, it returns #NVDM_STATUS_ITEM_NOT_FOUND.
 *            User should check the return status to verify whether the size of data item is retrieved successfully.
 * @param[in] group_name   is the group name of the data item, such as "Bluetooth".
 * @param[in] data_item_name   is the name of the data item, such as an "address" of "Bluetooth".
 * @param[in,out] size   is the user buffer size when used as an input and is the size of actual data item's content read out when used as an output.
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 */
nvdm_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size);

/**
 * @brief    This function is used to get item count by group name
 *           It will parse all items, and compare which one is equal to group name gived
 * @param[int] group_name    group name what you want to parse
 * @return
 *           uint32_t, the item count of group name
 */
uint32_t nvdm_get_item_count_by_group(const char *group_name);

/**
 * @brief    This function is used to get item name and values of group name what you give
 *           It will parse all items, and compare which one is equal group namd,
 *           then memcpy item name and value to the params's buffer
 * @param[in] group name    the group name which you want to parse
 * @param[in] items         the item name buffer which should be return
 * @param[in] item_len      the max item name length
 * @param[in] vals          the item value buffer which should be return
 * @param[in] val_len       the max item value length
 * @return
 *                #NVDM_STATUS_OK, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR, if an unknown error occurred. \n
 *                #NVDM_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND, if the data item group is not found. \n
 */
nvdm_status_t nvdm_get_all_items_by_group(const char *group_name, char *items, uint16_t item_len, char *vals, uint16_t val_len);

#ifdef __cplusplus
}
#endif

/**
* @}
*/

#endif

#endif /* __NVDM_H__ */

