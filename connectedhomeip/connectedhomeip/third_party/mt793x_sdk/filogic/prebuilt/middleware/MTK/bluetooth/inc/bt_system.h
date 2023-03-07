/* Copyright Statement:
 *
 * (C) 2005-2020  MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
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
/* MediaTek restricted information */

#ifndef __BT_SYSTEM_H__
#define __BT_SYSTEM_H__

#include "bt_platform.h"
#include "bt_type.h"
#include "bt_system_internal.h"

BT_EXTERN_C_BEGIN

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothCommon Common
 * @{
 * @addtogroup BluetoothCommonSYS_MEM Memory management
 * @{
 * This section defines the macros and API prototypes related to memory configuration. Apply memory configuration after the system boots up.
 *
 * @section memory_conifguration_usage How to use this module
 *
 * - Call the function #bt_memory_init_packet() to assign buffers for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_TX_BUFFER buffer to send commands and application data to the controller with a recommended size from 256 to 512 bytes.
 *   The other is the #BT_MEMORY_RX_BUFFER buffer to receive events and application data from the controller with a recommended size from 512 to 1024 bytes.
 * - Call the function #bt_memory_init_control_block() to assign buffers with fixed block size for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_CONTROL_BLOCK_TIMER buffer for the software timer usage in the Bluetooth stack.
 *   The other is the #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION buffer to store connection information in the stack.
 *
 *  - Sample code:
 *      @code
 *          #define BT_CONNECTION_MAX   5
 *          #define BT_TIMER_NUM 10
 *          #define BT_TX_BUF_SIZE 512
 *          #define BT_RX_BUF_SIZE 1024
 *          #define BT_TIMER_BUF_SIZE (BT_TIMER_NUM * BT_CONTROL_BLOCK_SIZE_OF_TIMER) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_TIMER.
 *          #define BT_CONNECTION_BUF_SIZE (BT_CONNECTION_MAX* BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION.
 *          static char timer_cb_buf[BT_TIMER_BUF_SIZE];
 *          static char connection_cb_buf[BT_CONNECTION_BUF_SIZE];
 *          static char tx_buf[BT_TX_BUF_SIZE];
 *          static char rx_buf[BT_RX_BUF_SIZE];
 *          bt_bd_addr_t local_public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}; // Initialize the public_addr according to your requirement based on application.
 *
 *          // Call memory initialization and task creation in the bt_create_task() only once.
 *          static void bt_create_task(void)
 *          {
 *               //Configurate the radio transmission power level for bt and le connections. the transmission power settings is not mandatory.
 *               bt_config_tx_power_t tx_config_info = {.bt_init_tx_power_level = 5,
 *                                                      .tx_power_level_offset = 0,
 *                                                      .fixed_tx_power_enable = 0,
 *                                                      .fixed_tx_power_level = 0,
 *                                                      .bt_max_tx_power_level = 6,
 *                                                      .le_init_tx_power_level = 6};
 *               bt_config_tx_power_level(&tx_config_info);
 *
 *               //Configurate the memory blocks.
 *               bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
 *               bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION, connection_cb_buf,
 *                                 BT_CONNECTION_BUF_SIZE);
 *
 *               // Create a Bluetooth task. The Bluetooth will power on automatically after the task is created.
 *               if (pdPASS != xTaskCreate(bt_task, "bt_task", 2048, (void *)local_public_addr, 5, NULL)) {
 *                   printf("Cannot create a bt_task.");
 *               }
 *          }
 *      @endcode
 *
 */


/**
 * @defgroup BluetoothCommonhbif_memory_define Define
 * @{
 */


/* -------------- The buffer size of a single control block. --------------------- */
#define BT_CONTROL_BLOCK_SIZE_OF_TIMER            (24)     /**< The size of a control block for Timer. */
//#ifndef __BT_GATTS_EH__ /* MTK_COMMON default support GATTS_EN, so we need +4 bytes size for bt_att_t struct, which is included by bt_connect_t */
//#define BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION    (108)    /**< The size of a control block for Bluetooth LE connection. */
//#else
#define BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION    (108 + 4)    /**< The size of a control block for Bluetooth LE connection. */
//#endif
#define BT_CONTROL_BLOCK_SIZE_OF_EDR_CONNECTION   (112)     /**< The size of a control block for BR/EDR connection. */
#define BT_CONTROL_BLOCK_SIZE_OF_RFCOMM           (52)     /**< The size of a control block for RFCOMM. */
#define BT_CONTROL_BLOCK_SIZE_OF_HFP              (76)     /**< The size of a control block for HFP. */
#define BT_CONTROL_BLOCK_SIZE_OF_AVRCP            (72)     /**< The size of a control block for AVRCP. */
#define BT_CONTROL_BLOCK_SIZE_OF_A2DP_SEP         (16)     /**< The size of a control block for A2DP_SEP. */
#define BT_CONTROL_BLOCK_SIZE_OF_A2DP             (156)    /**< The size of a control block for A2DP. */
#define BT_CONTROL_BLOCK_SIZE_OF_PBAPC            (184)    /**< The size of a control block for PBAPC. */
#define BT_CONTROL_BLOCK_SIZE_OF_SPP              (44)     /**< The size of a control block for SPP. */
#define BT_CONTROL_BLOCK_SIZE_OF_AWS              (104)    /**< The size of a control block for AWS. */
#define BT_CONTROL_BLOCK_SIZE_OF_HID              (108)    /**< The size of a control block for HID. */
#define BT_CONTROL_BLOCK_SIZE_OF_GATT             (40)     /**< The size of a control block for GATT. */
#define BT_CONTROL_BLOCK_SIZE_OF_AWS_MCE          (102)    /**< The size of a control block for AWS_MCE. */
#define BT_CONTROL_BLOCK_SIZE_OF_HSP              (52)     /**< The size of a control block for GATT. */
#define BT_CONTROL_BLOCK_SIZE_OF_AVRCP_EX         (168)    /**< The size of a control block for AVRCP BROWSE CHANNEL. */
#define BT_CONTROL_BLOCK_SIZE_OF_AIRUPDATE        (100)    /**< The size of a control block for AIRUPDATE. */
#define BT_CONTROL_BLOCK_SIZE_OF_LE_ADV_SET       (16)     /**<The size of a control block size for le adv set. */
#define BT_CONTROL_BLOCK_SIZE_OF_CTP              (54)     /**< The size of a control block size for CTP. */
#define BT_CONTROL_BLOCK_SIZE_OF_L2CAP            (36)     /**< The size of a control block size for L2CAP basic mode. */
#define BT_CONTROL_BLOCK_SIZE_OF_LE_L2CAP_FIX_CHANNEL       (10)     /**< The size of a control block size for LE_L2CAP_FIX_CHANNEL. */
#define BT_CONTROL_BLOCK_SIZE_OF_BREDR_L2CAP_FIX_CHANNEL    (10)     /**< The size of a control block size for BREDR_L2CAP_FIX_CHANNEL. */
#ifdef BT_LE_AUDIO_ENABLE
#define BT_CONTROL_BLOCK_SIZE_OF_LE_CIS_CONNECTION   (8)      /**< The size of a control block size for CIS connection. */
#endif
#define BT_CONTROL_BLOCK_SIZE_OF_MAPC             (160)     /**< one control block size for BT_MEMORY_CONTROL_BLOCK_MAPC. */
#define BT_CONTROL_BLOCK_SIZE_OF_LE_OTP                     (56)     /**< The size of a control block size for LE OTP. */

#define BT_CONTROL_BLOCK_SIZE_OF_LE_ULL              (8)    /**< The size of a control block size for LE ULL. */

#ifdef BT_EATT_ENABLE
#define BT_CONTROL_BLOCK_SIZE_OF_EATT             (100)    /**< The size of a control block size for EATT. */
#endif

/**
* @brief     Bluetooth memory control block types.
*/
typedef enum {
    BT_MEMORY_CONTROL_BLOCK_TIMER = 0x00,              /**< Timer. */
    BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION = 0x01,      /**< Bluetooth LE connection. */
    BT_MEMORY_CONTROL_BLOCK_EDR_CONNECTION = 0x02,     /**< BR/EDR connection. */
    BT_MEMORY_CONTROL_BLOCK_RFCOMM = 0x03,             /**< RFCOMM. */
    BT_MEMORY_CONTROL_BLOCK_AVRCP = 0x04,              /**< AVRCP control channel. */
    BT_MEMORY_CONTROL_BLOCK_PBAPC = 0x05,              /**< PBAPC. */
    BT_MEMORY_CONTROL_BLOCK_A2DP_SEP = 0x06,           /**< A2DP SEP. */
    BT_MEMORY_CONTROL_BLOCK_A2DP = 0x07,               /**< A2DP. */
    BT_MEMORY_CONTROL_BLOCK_HFP = 0x08,                /**< HFP. */
    BT_MEMORY_CONTROL_BLOCK_SPP = 0x09,                /**< SPP. */
    BT_MEMORY_CONTROL_BLOCK_AWS = 0x0A,                /**< AWS. */
    BT_MEMORY_CONTROL_BLOCK_GATT = 0x0B,               /**< GATT. */
    BT_MEMORY_CONTROL_BLOCK_HID = 0x0C,                /**< HID. */
    BT_MEMORY_CONTROL_BLOCK_AWS_MCE = 0x0D,            /**< AWS_MCE. */
    BT_MEMORY_CONTROL_BLOCK_HSP = 0x0E,                /**< HSP. */
    BT_MEMORY_CONTROL_BLOCK_AVRCP_EX = 0x0F,           /**< AVRCP control channel and browse channel. */
    BT_MEMORY_CONTROL_BLOCK_AIRUPDATE = 0x10,          /**< AIRUPDATE. */
    BT_MEMORY_CONTROL_BLOCK_LE_ADV_SET = 0x11,         /**< ADV set. */
    BT_MEMORY_CONTROL_BLOCK_CTP = 0x12,                /**< CTP. */
    BT_MEMORY_CONTROL_BLOCK_L2CAP = 0x13,              /**< L2CAP basic mode. */
    BT_MEMORY_CONTROL_BLOCK_LE_L2CAP_FIX_CHANNEL = 0x14,       /**< L2CAP fix channel. */
    BT_MEMORY_CONTROL_BLOCK_BREDR_L2CAP_FIX_CHANNEL = 0x15,    /**< L2CAP fix channel. */
    BT_MEMORY_CONTROL_BLOCK_MAPC = 0x16,               /**< MAP. */
#ifdef BT_LE_AUDIO_ENABLE
    BT_MEMORY_CONTROL_BLOCK_LE_CIS_CONNECTION = 0x17,  /**< Bluetooth LE CIS connection. */
#endif
    BT_MEMORY_CONTROL_BLOCK_LE_OTP = 0x18,             /**< LE OTP. */
    BT_MEMORY_CONTROL_BLOCK_LE_ULL = 0x19,             /**< LE ULL. */
#ifdef BT_EATT_ENABLE
    BT_MEMORY_CONTROL_BLOCK_EATT = 0x20,               /**< EATT. */
#endif
    BT_MEMORY_CONTROL_BLOCK_NUM                        /**< The total number of memory control block types. */

} bt_memory_control_block_t;

/**
 * @brief    Bluetooth memory buffer types for the Memory Management module.
 * @{
 */
typedef enum {
    BT_MEMORY_TX_BUFFER = 0,     /**< TX packet buffer.*/
    BT_MEMORY_RX_BUFFER          /**< RX packet buffer.*/
} bt_memory_packet_t;

#define BT_MEMORY_TX_BUFFER_AVAILABLE_IND     (BT_MODULE_MM | 0x00010000)     /**< A buffer availability indication with #bt_memory_tx_buffer_available_ind_t. */
#define BT_MEMORY_FREE_GARBAGE_IND            (BT_MODULE_MM | 0x00010001)     /**< Ask the application to free garbage memory with NULL payload. */

/**
 * @}
 */
/**
 * @}
 */

/**
* @defgroup Bluetoothhbif_memory_struct Struct
* @{
*/

/**
*  @brief TX buffer availability indication.
*/
typedef struct {
    uint32_t      size; /**< The size of available TX buffer in bytes. */
} bt_memory_tx_buffer_available_ind_t;

/**
* @}
*/
/**
 * @brief     This function initializes a TX/RX packet buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_TX_BUFFER or #BT_MEMORY_RX_BUFFER.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer in bytes.
 * @return    void.
 */
void  bt_memory_init_packet(bt_memory_packet_t type, char *buf, uint32_t size);

/**
 * @brief     This function obtain the peek size of type buffer.
 * @param[in] type is #BT_MEMORY_TX_BUFFER or #BT_MEMORY_RX_BUFFER.
 * @return    The buffer peek value.
 */
//uint32_t bt_memory_get_packet_peek(bt_memory_packet_t type);

/**
 * @brief     This function obtain the all buffer size of packet type.
 * @param[in] type is #BT_MEMORY_TX_BUFFER or #BT_MEMORY_RX_BUFFER.
 * @return    The all buffer size.
 */
uint32_t bt_memory_get_all_buffer_size(bt_memory_packet_t type);

/**
 * @brief     This function initializes a control block buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_CONTROL_BLOCK_TIMER or #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer, in bytes.
 * @return    void.
 */
void  bt_memory_init_control_block(bt_memory_control_block_t type, char *buf, uint32_t size);

/**
 * @}
 */

/**
 * @addtogroup BluetoothCommonPanic Panic
 * @{
 * This section defines the event, structure and types for Bluetooth panic feature.
 *
 * @section bt_panic_usage How to use this module
 *
 *   The panic event is triggered when Bluetooth host or controller has asserted or crashed. Once the panic event is received, the Bluetooth has to reboot.
 *  - Sample code:
 *      @code
 *          // The application reboots the Bluetooth, once Bluetooth panic event is received.
 *          bool is_panic = false;
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_PANIC:
 *                      is_panic = true;
 *                      if (((bt_panic_t *)buff)->source == BT_PANIC_SOURCE_HOST) {
 *                          // Host is the source of panic event.
 *                      } else {
 *                          // Controller is the source of panic event.
 *                      }
 *
 *                      if (bt_power_off()) {
 *                          // The Bluetooth powered off successfully.
 *                      } else {
 *                          // The Bluetooth power off has failed.
 *                      }
 *                      break;
 *                  case BT_POWER_OFF_CNF:
 *                      {
 *                          if (is_panic == true) {
 *                              if(bt_power_on(NULL,NULL)) {
 *                                  // The Bluetooth powered off successfully.
 *                              } else {
 *                                  // The Bluetooth power off has failed.
 *                              }
 *                          }
 *                      }
 *                      break;
 *                  case BT_POWER_ON_CNF:
 *                      {
 *                          if (is_panic == true) {
 *                              is_panic = false;
 *                          }
 *                      }
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

/**
 * @defgroup Bluetoothbt_panic_define Define
 * @{
 * Define Bluetooth panic event and source types.
 */

/**
 * @brief    Bluetooth panic event.
 */
#define BT_PANIC                            (BT_MODULE_SYSTEM | 0x0003)    /**< Bluetooth host or controller asserted or crashed, the application has to reboot the Bluetooth.*/

/**
 * @defgroup Bluetoothbt_fw_assert_define Define
 * @{
 * Define Bluetooth fw assert event and source types.
 */

/**
 * @brief    Bluetooth fw assert event.
 */
#define BT_PANIC_COREDUMP_DONE                      (BT_MODULE_SYSTEM | 0x000F)

/**
 * @brief    Bluetooth panic source type.
 */
#define BT_PANIC_SOURCE_HOST                0                              /**< Bluetooth host has asserted or crashed. */
#define BT_PANIC_SOURCE_CONTROLLER          1                              /**< Bluetooth controller has asserted or crashed. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_panic_struct Struct
 * @{
 * Define the Bluetooth panic structure.
 */

/**
 *  @brief the Bluetooth panic event parameter.
 */

typedef struct {
    uint8_t source; /**< The source for the panic, such as #BT_PANIC_SOURCE_HOST or #BT_PANIC_SOURCE_CONTROLLER. */
} bt_panic_t;

/**
 * @}
 * @}
 */

/**
 * @addtogroup BluetoothCommonSYS_POWER Power management
 * @{
 * This section defines the confirmation types and APIs to switch the power on and off.
 *
 * @section bt_power_management_usage How to use this module
 *
 *   The application layer can power on the Bluetooth if it is powered off and a stack memory is assigned.
 *  - Sample code:
 *      @code
 *          // Power on the Bluetooth from the application.
 *          void user_application_power_on_function()
 *          {
 *              // Initialize the public_addr and the random_addr according to the application requirements.
 *              bt_bd_addr_t public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
 *              bt_bd_addr_t random_addr = {0xc0, 0x22, 0x33, 0x44, 0x55, 0xc6};
 *
 *              bt_power_on(&public_addr, &random_addr);
 *          }
 *
 *          // The application receives the power on confirmation event in the function #bt_app_event_callback() after calling #bt_power_on().
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_POWER_ON_CNF:
 *                      // Add the implementation flow after the Bluetooth is powered on.
 *                      break;
 *                  case BT_GAP_LE_CONNECT_IND:
 *                      // Connection is established.
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

/**
 * @defgroup Bluetoothhbif_power_define Define
 * @{
 * Define the power on or off events.
 */

#define BT_POWER_ON_CNF                     (BT_MODULE_SYSTEM | 0x0001)    /**< Power on confirmation event with #bt_power_on_cnf_t. */
#define BT_POWER_OFF_CNF                    (BT_MODULE_SYSTEM | 0x0002)    /**< Power off confirmation event with NULL payload. */

/**
 * @brief    Bluetooth initialize feature mask.
 */
#define BT_INIT_FEATURE_MASK_BT_5_0                 0x0001  /**< Indicate the hummingbird can use the bluetooth Spec 5.0 feature. */
#define BT_INIT_FEATURE_MASK_DISABLE_SLEEP_MODE     0x0002  /**< Indicate the bt can't enter sleep mode. */
#define BT_INIT_FEATURE_MASK_DISABLE_SNIFF_MODE     0x0004  /**< Indicate the BR/EDR can't enter sniff mode. */
#define BT_INIT_FEATURE_MASK_DISABLE_SNIFF_SUB_MODE 0x0008  /**< Indicate the BR/EDR can't enter sniff subrating mode. */
#define BT_INIT_FEATURE_MASK_DISABLE_3M             0x0010  /**< Indicate the BR/EDR can't use 3M packet type. */
#define BT_INIT_FEATURE_MASK_SECURITY_CONNECTION    0x0020  /**< Indicate the security connection feature enable.*/

typedef uint16_t bt_init_feature_mask_t; /**<Define the init feature mask type. */


/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_power_struct Struct
 * @{
 * Define power on event parameter structure.
 */

/**
 *  @brief Power on confirmation structure.
 */
typedef struct {
    const bt_bd_addr_t      local_public_addr; /**< The local public address of the device. */
} bt_power_on_cnf_t;

/**
 * @brief This structure defines the detail information about config tx power level.
 */
typedef struct {
    uint8_t    bt_init_tx_power_level;   /**< Initial level of the BT connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    tx_power_level_offset;    /**< Initial/fixed level offset below the bt/le connection radio transmission power. The range is between 0 and 3. The larger the value, the weaker the TX power. Only available in AB155x.*/
    uint8_t    fixed_tx_power_enable;    /**< Level of bt/le connection radio transmission power is fixed to the #fixed_tx_power_level when enable is 1. Disable this funtion when enable is 0. Only available in AB155x.*/
    uint8_t    fixed_tx_power_level;     /**< Fixed level of bt/le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x.*/
    uint8_t    le_init_tx_power_level;   /**< Initial level of le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    bt_max_tx_power_level;    /**< Maximum level of bt connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
} bt_config_tx_power_t;

/**
 * @brief This structure defines the detail information about config tx power level.
 */
typedef struct {
    uint8_t    bt_init_tx_power_level;   /**< Initial level of the BT connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    reserved;                 /**<Reserved.*/
    uint8_t    fixed_tx_power_enable;    /**< Level of bt/le connection radio transmission power is fixed to the #fixed_tx_power_level when enable is 1. Disable this funtion when enable is 0. Only available in AB155x.*/
    uint8_t    fixed_tx_power_level;     /**< Fixed level of bt/le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x.*/
    uint8_t    le_init_tx_power_level;   /**< Initial level of le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    bt_max_tx_power_level;    /**< Maximum level of bt connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    bdr_ble_tx_power_level_offset; /**< BDR/BLE tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    bdr_ble_fine_tx_power_level_offset;/**< BDR/BLE fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_tx_power_level_offset; /**< BLE tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_fine_tx_power_level_offset; /**< BLE fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
} bt_config_tx_power_ext_t;

/**
 * @brief This structure defines the detail information about config tx power gain.
 */
typedef struct {
    uint16_t dpc_value;      /**< modem digtal power control value */
    uint16_t rfcr;           /**< tx power RF circuit register */
} bt_config_tx_power_gain_t;

/**
 * @}
 */

/**
* @brief    Bluetooth reset types.
* @{
*/
typedef enum {
    BT_POWER_RESET_SW = 1,     /**< Bluetooth software power reset. */
    BT_POWER_RESET_HW          /**< Bluetooth hardware power reset. */
} bt_power_reset_t;
/**
 * @}
 */

/**
 * @addtogroup Bluetoothhbif_tx_power_config Tx power config
 * @{
 * This section defines the usage of how to config tx power associate with tx power version.
 *
 * @section bt_tx_power_config_usage How to use this module
 *
 * The application layer must provide a function for Bluetooth to obtain tx power config parameters, so that Bluetooth can config bt tx power level when Bluetooth power on.
 * - Sample code:
 *    @code
 *     // Application layer provide a function for Bluetooth to obtain tx power config parameters.
 *     void user_application_get_tx_power_config_function(bt_tx_power_config_version_t version, void *tx_power_info)
 *     {
 *         // Fill the tx_power_info according to #bt_tx_power_config_version_t version.
 *         switch (version) {
 *             case BT_TX_POWER_CONFIG_VERSION1: {
 *                 bt_config_tx_power_version1_t user_tx_power_config = {
 *                     .bdr_init_tx_power_level = 7,
 *                     .reserved = 0,
 *                     .fixed_tx_power_enable = 0,
 *                     .fixed_tx_power_level = 0,
 *                     .le_init_tx_power_level = 5,
 *                     .bt_max_tx_power_level = 7,
 *                     .bdr_tx_power_level_offset = 1,
 *                     .bdr_fine_tx_power_level_offset = 0,
 *                     .edr_tx_power_level_offset = 1,
 *                     .edr_fine_tx_power_level_offset = 0,
 *                     .ble_tx_power_level_offset = 1,
 *                     .ble_fine_tx_power_level_offset = 0,
 *                     .edr_init_tx_power_level = 7
 *                  };
 *                  memcpy(tx_power_info, &user_tx_power_config, sizeof(bt_config_tx_power_version1_t));
 *                  break;
 *              }
 *              default: {
 *                  break;
 *              }
 *          }
 *     }
 *    @endcode
 *
 */

/**
 * @defgroup Bluetoothhbif_tx_power_config_struct Struct
 * @{
 * Define tx power config version structure.
 */

/**
 * @brief    Bluetooth tx power config version.
 */
#define BT_TX_POWER_CONFIG_VERSION1         0x0001     /**< Indicate the tx power config version associate with #bt_config_tx_power_version1_t. */

typedef uint16_t bt_tx_power_config_version_t; /**< Define the tx power config version type. */

/**
 * @brief This structure defines the detail information about config tx power level version1.
 */
typedef struct {
    uint8_t    bdr_init_tx_power_level;       /**< Initial level of the bdr connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power.*/
    uint8_t    reserved;                      /**< Reserved.*/
    uint8_t    fixed_tx_power_enable;         /**< Level of bdr/edr/ble connection radio transmission power is fixed to the #fixed_tx_power_level when enable is 1. Disable this funtion when enable is 0. Only available in AB155x.*/
    uint8_t    fixed_tx_power_level;          /**< Fixed level of bdr/edr/ble connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x.*/
    uint8_t    le_init_tx_power_level;        /**< Initial level of ble connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power.*/
    uint8_t    bt_max_tx_power_level;         /**< Maximum level of bdr/edr/ble connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power.*/
    uint8_t    bdr_tx_power_level_offset;     /**< BDR tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    bdr_fine_tx_power_level_offset;/**< BDR fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_tx_power_level_offset;     /**< EDR tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_fine_tx_power_level_offset;/**< EDR fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    ble_tx_power_level_offset;     /**< BLE tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    ble_fine_tx_power_level_offset;/**< BLE fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_init_tx_power_level;       /**< Initial level of the edr connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x.*/
} bt_config_tx_power_version1_t;

/**
 * @}
 */

/**
* @brief    Bluetooth device test mode change events define.
* @{
*/
#define BT_TEST_MODE_NONE_IND                   (BT_MODULE_SYSTEM | 0x0010)    /**< Buletooth device enter normal mode event with NULL payload. */
#define BT_TEST_MODE_DUT_MIX_ENABLE_IND         (BT_MODULE_SYSTEM | 0x0011)    /**< Buletooth device DUT mixed mode enable event with NULL payload. */
#define BT_TEST_MODE_DUT_ONLY_ENABLE_IND        (BT_MODULE_SYSTEM | 0x0012)    /**< Buletooth device DUT only mode enable event with NULL payload. */
#define BT_TEST_MODE_DUT_ACTIVE_IND             (BT_MODULE_SYSTEM | 0x0013)    /**< Buletooth device DUT active event with NULL payload. */
#define BT_TEST_MODE_RELAY_ENABLE_IND           (BT_MODULE_SYSTEM | 0x0014)    /**< Buletooth device relay mode enable event with NULL payload. */
#define BT_TEST_MODE_COMMAND_ENABLE_IND         (BT_MODULE_SYSTEM | 0x0015)    /**< Buletooth device command mode enable event with NULL payload. */
#define BT_TEST_MODE_DUT_INACTIVE_IND           (BT_MODULE_SYSTEM | 0x0016)    /**< Buletooth device DUT inactive event with NULL payload. */
/**
 * @}
 */

/**
 * @brief     This function powers on the Bluetooth. The application layer receives #BT_POWER_ON_CNF with #bt_power_on_cnf_t after the Bluetooth is powered on.
              Make sure the memory is initialized, the Bluetooth task is created and Bluetooth is powered on before calling any other Bluetooth APIs.
 * @param[in] public_addr   is a pointer to a given public address (6 bytes).
 *                          If public_addr is NULL, the controller will generate a public address and provide it to the user using #BT_POWER_ON_CNF with #bt_power_on_cnf_t.
 *                          If public_addr is not NULL, the address in the #BT_POWER_ON_CNF is same as public_addr.
 *                          The given public address should not be [00-00-00-00-00-00].
 * @param[in] random_addr   is a pointer to a given random address(6 bytes).
 * @return                  #BT_STATUS_SUCCESS, the power on request has been sent successfully.
 *                          #BT_STATUS_FAIL, the operation fails due to the same operation is ongoing.
 *                          #BT_STATUS_OUT_OF_MEMORY, the operation fails due to out of memory.
 */
bt_status_t bt_power_on(bt_bd_addr_ptr_t public_addr, bt_bd_addr_ptr_t random_addr);

/**
 * @brief     This function powers off the Bluetooth. The SDK will not send disconnect requests to the existing connections during the power off operation.
 *            The application layer will receive #BT_POWER_OFF_CNF after the Bluetooth is powered off.
 * @return    #BT_STATUS_SUCCESS, the power off request has been sent successfully.
 *            #BT_STATUS_FAIL, the operation fails due to the same operation is ongoing.
 *            #BT_STATUS_OUT_OF_MEMORY, the operation fails due to out of memory.
 */
bt_status_t bt_power_off(void);

/**
 * @brief     This function powers reset the Bluetooth. The application layer will receive #BT_POWER_OFF_CNF after the Bluetooth is powered off
 *             and receives #BT_POWER_ON_CNF with #bt_power_on_cnf_t after the Bluetooth is powered on.
 *            Make sure the bluetooth had powerd on.
 * @param[in] type   is the type of bluetooth reset, #BT_POWER_RESET_SW is used to reset the bluetooth controller software.
 *                          #BT_POWER_RESET_HW is used to reset the bluetooth controller hardware.
 * @return                  #BT_STATUS_SUCCESS, the power on request has been sent successfully.
 *                          #BT_STATUS_FAIL, the operation fails due to the same operation is ongoing or bluetooth is not powerd on.
 *                          #BT_STATUS_OUT_OF_MEMORY, the operation fails due to out of memory.
 */
bt_status_t bt_power_reset(bt_power_reset_t type);

/**
 * @brief     This function is used to set the local public address when bt is in idle state.
 * @param[in] public_addr   is a pointer to a given public address (6 bytes), it can't be NULL.
 *                   It will be taken effect on next bt power on if the current bt state is not bt power on.
 * @return    #BT_STATUS_SUCCESS, the operation success.
 *            #BT_STATUS_FAIL, the operation fails. the operation fails due to the same operation is ongoing.
 */
bt_status_t bt_set_local_public_address(bt_bd_addr_ptr_t public_addr);

/**
 * @brief     This function is used to config the tx power gain table of both BDR and EDR. This function must be invoked before bt power on, or it will take no effection on bt tx power gain.
 * @param[in] bdr_table   is a pointer to a given BDR power gain table(total 32 element),it can't be NULL.
 * @param[in] edr_table   is a pointer to a given EDR power gain table(total 32 element),it can't be NULL.
 * @return    #BT_STATUS_SUCCESS, the operation success.
 *            #BT_STATUS_FAIL, the operation fails. because the Bluetooth is not in powerd off state or parameters bdr_tx_pwr_gain_table or edr_tx_pwr_gain_table is NULL.
 */
bt_status_t bt_config_tx_power_gain_table(const bt_config_tx_power_gain_t *bdr_table, const bt_config_tx_power_gain_t *edr_table);

/**
 * @brief     This function configurates the radio transmission power level for bt and le connections. The transmission power settings is not mandatory.
              User can configurate the radio transmission power before power on Bluetooth, and the configuration will not be reset until chip reboot.
 * @param[in] tx_power_info         is a pointer to a structure that specifies the parameters for radio transmission power level.
 * @return    #BT_STATUS_SUCCESS, the operation success.
 *            #BT_STATUS_FAIL, the operation fails. because the Bluetooth is power on or bt_init_tx_power_level is bigger than bt_max_tx_power_level.
 */
bt_status_t bt_config_tx_power_level(const bt_config_tx_power_t *tx_power_info);

/**
 * @brief     This function configurates the radio transmission power level for bt and le connections. The transmission power settings is not mandatory.
              User can configurate the radio transmission power before power on Bluetooth, and the configuration will not be reset until chip reboot.
              User could only select one of API to set tx power between bt_config_tx_power_level and bt_config_tx_power_level_ext
 * @param[in] tx_power_info         is a pointer to a structure that specifies the parameters for radio transmission power level.
 * @return    #BT_STATUS_SUCCESS, the operation success.
 *            #BT_STATUS_FAIL, the operation fails. because the Bluetooth is power on or bt_init_tx_power_level is bigger than bt_max_tx_power_level.
 */
bt_status_t bt_config_tx_power_level_ext(const bt_config_tx_power_ext_t *tx_power_info);

/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf);

/**
 * @brief   This function is for memory allocation from Rx buffer by application.
 * @param[in] buffer_size   is the allocation size requested.
 * @return            The point to Rx buffer allocated.
 */
void *bt_alloc_rx_buffer_from_external(uint32_t buffer_size);

/**
 * @brief   This function is used to send the vendor command directly to the BT controller, which can only used for the BT test or a special case in middleware layer,
            the application shall not use it in normal cases. There is no other event report for this vendor command; You must check the return value.
 * @param[in] opcode       is the operation code of the vendor command.
 * @param[in] param        is the param data of the vendor command.
 * @param[in] param_length is the length of the command param.
 * @return    #BT_STATUS_SUCCESS, the vendor command was successfully sent.
 *            #BT_STATUS_FAIL, sending the vendor command failed because BT is not powered on or ready to power off.
 */
bt_status_t bt_hci_send_vendor_cmd(uint16_t opcode, uint8_t *param, uint8_t param_length);

//__MTK_COMMON__
/**
 * @brief                  HCI command callback function prototype
 * @param[in] is_timeout   True : Sending HCI vendor command error; False : HCI vendor command is sent successfully.
 * @param[in] timer_id     Timer ID. No need to handle this argument in application.
 * @param[in] data         User callback data inputted when invoking bt_hci_send_vendor_cmd_ex(...);
 * @param[in] arg          Bypass function argument for internal use. No need to handle this argument in application.
 * @return                 #BT_STATUS_SUCCESS, It should always return BT_STATUS_SUCCESS in callback function.
 */
typedef bt_status_t (*bt_vendor_cmd_cbk) (uint32_t is_timeout, uint32_t timer_id, uint32_t data, const void *arg);

typedef bt_vendor_cmd_cbk bt_vendor_cmd_cbk_t;

/**
 * @brief                   This function is used to send vendor command directly to the BT controller and receive HCI vendor command result by application.
 * @param[in] opcode        HCI vendor command opcode.
 * @param[in] param         HCI vendor command parameter.
 * @param[in] param_length  HCI vendor command parameter length.
 * @return                  #BT_STATUS_SUCCESS, the vendor command is successfully sent.
 *                          #BT_STATUS_FAIL, sending the vendor command failed because BT is not powered on.
 */
bt_status_t bt_hci_send_vendor_cmd_ex(uint16_t opcode, uint8_t *param, uint8_t param_length,
                                              uint32_t callback_data, const bt_vendor_cmd_cbk_t callback);
//(end)__MTK_COMMON__

#ifdef __BT_GATTS_EH__
/**
 * @brief   This function is for memory allocation from TX buffer by application.
 * @param[in] buffer_size   is the allocation size requested.
 * @return            The point to TX buffer allocated.
 */
void *bt_alloc_tx_buffer_from_external(uint32_t buffer_size);
#endif

/**
 * @brief    Bluetooth clock types for common modules.
 * @{
 */
typedef struct {
    uint32_t nclk;          /**< The Bluetooth clock unit is 312.5 microseconds.*/
    uint16_t nclk_intra;    /**< The Bluetooth clock is shown in microseconds.*/
} bt_clock_t;
/**
 * @}
 */


/**
* @brief               This function gets the current Bluetooth clock value.
* @param[in] gap_handle is the gap conneciton handle for the current link.
* @param[out] bt_clock  is the bluetooth clock.
* @return              #BT_STATUS_SUCCESS, getting the Bluetooth clock value was successful.
*                      #BT_STATUS_FAIL, failed to get the Bluetooth clock value. The reason
*                      could be that the gap handle is invalied or the buffer pointer for the Bluetooth clock is NULL.
*/
bt_status_t bt_get_bt_clock(uint32_t gap_handle, bt_clock_t *bt_clock);



/**
 * @brief   This function is implemented by application to indicate preference featrure mask. Provide a user-defined callback.
 * @return            The feature mask of application preference.
 */
bt_init_feature_mask_t bt_get_feature_mask_configuration(void);

/** @}
 *  @}
 *  @}
 *  @}
 */

BT_EXTERN_C_END

#endif

