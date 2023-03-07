/***************************************************************************//**
 * @file
 * @brief This file contains the plug-in manager for the EZRadio and
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EZRADIO_PLUGIN_MANAGER_H_
#define EZRADIO_PLUGIN_MANAGER_H_

#include "ecode.h"

// Include the generated EzRadio application configuration file.
#include "app-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#define RADIO_CHIP_FAMILY_EZRADIO       1u
#define RADIO_CHIP_FAMILY_EZRADIOPRO    2u

#define FALSE 0u
#define TRUE 1u

/// @endcond

/* Question: How to use my own radio configuration header file?
 *
 * Answer (Simplicity Studio): Simply open the bundled radio configurator
 * located in the RadioConfig source group. Whenever a new header file
 * is generated the project will reconfigure itself to use the generated
 * instead of the default.
 * If one is to go back to use the default radio header file, the
 * RADIO_USE_GENERATED_CONFIGURATION macro should be removed (or set to 0)
 * under the 'Path and Symbols' menu in the 'Properties' of the project.
 *
 * Answer (Other development tool: Add the following definition to your project
 * in order to use your own generated radio configuration header file, instead
 * of the kit default one.
 * #define RADIO_USE_GENERATED_CONFIGURATION
 */

#if defined(RADIO_USE_GENERATED_CONFIGURATION) && (RADIO_USE_GENERATED_CONFIGURATION == 1)
/* Include the generated radio configuration */
#include "radio-config-wds-gen.h"
#else
/* Include the default radio configuration for the board */
#if   defined(SL_WSTK6200A) || defined(SL_WSTK6220A)
#include "radio-config-wds-gen_R60-868MHz-13dBm.h"
#elif defined(SL_WSTK6240A)
#include "radio-config-wds-gen_R68-868MHz-16dBm.h"
#elif defined(SL_WSTK6201A) || defined(SL_WSTK6221A) || defined(SL_WSTK6241A)
#include "radio-config-wds-gen_R55-434MHz-10dBm.h"
#elif defined(SL_WSTK6202A) || defined(SL_WSTK6222A)
#include "radio-config-wds-gen_R63-915MHz-20dBm.h"
#elif defined(SL_WSTK6242A)
#include "radio-config-wds-gen_R68-915MHz-20dBm.h"
#elif defined(SL_WSTK6223A) || defined(SL_WSTK6243A)
#include "radio-config-wds-gen_R63-490MHz-20dBm.h"
#elif defined(SL_WSTK6224A) || defined(SL_WSTK6244A)
#include "radio-config-wds-gen_R63-169MHz-20dBm.h"
#else
#error No radio configuration is defined! Create your own radio configuration or define your kit properly!
#endif
#endif

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_plugin EZRADIODRV Plugin System
 * @brief EzRadio Plugin System Layer, see @ref ezradiodrv_plugin
 *        for detailed documentation.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_plugin_manager EZRADIODRV Plugin Manager
 * @brief EzRadio Plugin manager, see @ref ezradiodrv_plugin
 *        for detailed documentation.
 * @{
 ******************************************************************************/

// Error code definitions of EZRadioDrv
#define ECODE_EMDRV_EZRADIODRV_OK                       (ECODE_OK)                                     ///< Success return value.
#define ECODE_EMDRV_EZRADIODRV_ILLEGAL_HANDLE           (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000001)     ///< Illegal SPI handle.

#define ECODE_EMDRV_EZRADIODRV_TRANSMIT_PLUGIN_BASE             (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000100)     ///< Transmit plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_RECEIVE_PLUGIN_BASE              (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000200)     ///< Receive plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_CRC_ERROR_PLUGIN_BASE            (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000300)     ///< CRC error plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_AUTO_ACK_PLUGIN_BASE             (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000400)     ///< Receive plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_UNMOD_CARRIER_PLUGIN_BASE        (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000500)     ///< Receive plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_PN9_PLUGIN_BASE                  (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000600)     ///< Receive plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_DIRECT_TRANSMIT_PLUGIN_BASE      (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000700)     ///< Transmit plugin base error code.
#define ECODE_EMDRV_EZRADIODRV_DIRECT_RECEIVE_PLUGIN_BASE       (ECODE_EMDRV_EZRADIODRV_BASE | 0x00000800)     ///< Receive plugin base error code.

#if !defined(RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY)
#error RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY is not defined!
#elif (!(RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO) && !(RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO) )
#error RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY is not defined properly!
#endif

#if defined(EZRADIO_PLUGIN_TRANSMIT) || defined(EZRADIO_PLUGIN_RECEIVE)
// Radio packet field length array definition for various radio chip families
#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#define RADIO_CONFIG_DATA_FIELD_LENGTH \
  {                                    \
    RADIO_CONFIG_DATA_FIELD1_LENGTH,   \
    0u,                                \
    0u,                                \
    0u,                                \
    0u,                                \
  }
#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#define RADIO_CONFIG_DATA_FIELD_LENGTH \
  {                                    \
    RADIO_CONFIG_DATA_FIELD1_LENGTH,   \
    RADIO_CONFIG_DATA_FIELD2_LENGTH,   \
    RADIO_CONFIG_DATA_FIELD3_LENGTH,   \
    RADIO_CONFIG_DATA_FIELD4_LENGTH,   \
    RADIO_CONFIG_DATA_FIELD5_LENGTH    \
  }
#endif

// Max radio packet length definition for various radio chip families
#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#define RADIO_CONFIG_DATA_MAX_PACKET_LENGTH     RADIO_CONFIG_DATA_FIELD1_LENGTH
#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#define RADIO_CONFIG_DATA_MAX_PACKET_LENGTH \
  RADIO_CONFIG_DATA_FIELD1_LENGTH           \
  + RADIO_CONFIG_DATA_FIELD2_LENGTH         \
  + RADIO_CONFIG_DATA_FIELD3_LENGTH         \
  + RADIO_CONFIG_DATA_FIELD4_LENGTH         \
  + RADIO_CONFIG_DATA_FIELD5_LENGTH
#endif
#endif //#if ( defined( EZRADIO_PLUGIN_TRANSMIT ) || defined( EZRADIO_PLUGIN_RECEIVE ) )

// Various build time error messages, to make sure, that all necessary definitions are defined properly.
#if (defined(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE) && (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE  > 0) )

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#if (!defined(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD) || !(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  == 1))
#error RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  is not defined properly!
#endif
#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#if (!defined(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD) || (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  < 2) || (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  > 5) )
#error RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  is not defined properly!
#endif
#endif

#if !defined(RADIO_CONFIGURATION_DATA_PH_FIELD_SPLIT) || (RADIO_CONFIGURATION_DATA_PH_FIELD_SPLIT == FALSE)
#error EzRadio plugin manager can only support variable packet configuration for split packets curently!
#endif

#if defined(RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO) && (RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO > 0)

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#if !defined(RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH) || !(RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  == 1)
#error RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  is not defined properly!
#endif
#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#if !defined(RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH) || (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  < 1) || (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  > 4)
#error RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  is not defined properly!
#endif
#endif

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#if !defined(RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE) || !(RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE  == 1)
#error RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE  is not defined properly!
#endif
#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#if !defined(RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE) || (RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE  < 1) || (RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE  > 2)
#error RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE  is not defined properly!
#endif
#endif

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#define EZRADIO_LENGTH_WORD_START     0u

#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#if (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  == 1)
#define EZRADIO_LENGTH_WORD_START     0u

#elif (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  == 2)
#define EZRADIO_LENGTH_WORD_START     RADIO_CONFIG_DATA_FIELD1_LENGTH

#elif  (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  == 3)
#define EZRADIO_LENGTH_WORD_START     (RADIO_CONFIG_DATA_FIELD1_LENGTH + RADIO_CONFIG_DATA_FIELD2_LENGTH)

#elif  (RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH  == 4)
#define EZRADIO_LENGTH_WORD_START     (RADIO_CONFIG_DATA_FIELD1_LENGTH + RADIO_CONFIG_DATA_FIELD2_LENGTH + RADIO_CONFIG_DATA_FIELD3_LENGTH)

#endif // RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH
#endif //#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)

#else //#if defined(RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO) && (RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO > 0)
#undef  RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE
#define RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE     0u
#endif //#if defined(RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO) && (RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO > 0)

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIO)
#define EZRADIO_VARIABLE_DATA_START     RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE

#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)
#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  == 2)
#define EZRADIO_VARIABLE_DATA_START     RADIO_CONFIG_DATA_FIELD1_LENGTH

#elif  (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  == 3)
#define EZRADIO_VARIABLE_DATA_START     (RADIO_CONFIG_DATA_FIELD1_LENGTH + RADIO_CONFIG_DATA_FIELD2_LENGTH)

#elif  (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  == 4)
#define EZRADIO_VARIABLE_DATA_START     (RADIO_CONFIG_DATA_FIELD1_LENGTH + RADIO_CONFIG_DATA_FIELD2_LENGTH + RADIO_CONFIG_DATA_FIELD3_LENGTH)

#elif  (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD  == 5)
#define EZRADIO_VARIABLE_DATA_START     (RADIO_CONFIG_DATA_FIELD1_LENGTH + RADIO_CONFIG_DATA_FIELD2_LENGTH + RADIO_CONFIG_DATA_FIELD3_LENGTH + RADIO_CONFIG_DATA_FIELD4_LENGTH)

#endif // RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD
#endif //#elif (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)

#else //#if defined(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE ) && (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE  > 0)
/* Note: Neither EZRADIO_LENGTH_WORD_START or EZRADIO_VARIABLE_DATA_START is defined, so application knows
 *       that incoming data starts at the really beginning of the packet.  */
#endif //#if defined(RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE ) && (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE  > 0)

/// Interface for EZRADIODRV_HandleData structure defined below.
struct EZRADIODRV_HandleData;

/// Interface for EZRADIODRV_HandleData structure type defined below.
typedef struct EZRADIODRV_HandleData EZRADIODRV_HandleData_t;

/// EZRadio driver instance handle.
typedef EZRADIODRV_HandleData_t * EZRADIODRV_Handle_t;

/// EZradio reply union handle.
typedef ezradio_cmd_reply_t * EZRADIODRV_ReplyHandle_t;

/***************************************************************************//**
 * @brief
 *  EZRADIODRV feature callback function interface.
 *
 * @param[in] handle EZRadioDrv instance handler.
 * @param[in] status Callback status.
 *
 * @return None
 ******************************************************************************/
typedef void (*EZRADIODRV_Callback_t)(EZRADIODRV_Handle_t handle,
                                      Ecode_t status);

// Inclusion of all the underlying EzRadio plug-in headers.
#include "ezradio_transmit_plugin.h"
#include "ezradio_receive_plugin.h"
#include "ezradio_crcerror_plugin.h"
#include "ezradio_auto_ack_plugin.h"
#include "ezradio_unmod_carrier_plugin.h"
#include "ezradio_pn9_plugin.h"
#include "ezradio_direct_transmit_plugin.h"
#include "ezradio_direct_receive_plugin.h"

/// EzRadio driver instance initialization and handler structure.
/// This data structure contains a number of EzRadio driver configuration
/// options. Handler to this structure is passed to @ref ezradioInit() when
/// initializing an EzRadio driver instance. Some common initialization data
/// sets are predefined in @ref EZRADIODRV_INIT_DEFAULT.
/// Note: All the referenced plug-in instances has there own handler sturcture,
/// that is referenced from this structure.
struct EZRADIODRV_HandleData{
#if defined(EZRADIO_PLUGIN_TRANSMIT)
  EZRADIODRV_PacketTxHandle_t        packetTx;       ///< Packet transmission plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_RECEIVE)
  EZRADIODRV_PacketRxHandle_t        packetRx;       ///< Packet reception plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_CRC_ERROR)
  EZRADIODRV_PacketCrcErrorHandle_t  packetCrcError; ///< Packet reception with CRC error plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_AUTO_ACK) && defined(EZRADIO_PLUGIN_TRANSMIT) && defined(EZRADIO_PLUGIN_RECEIVE)
  EZRADIODRV_AutoAckHandle_t         autoAck;        ///< Auto-acknowledge plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_UNMOD_CARRIER)
  EZRADIODRV_UnmodCarrierHandle_t    unmodCarrier;   ///< Unmodulated carrier plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_PN9)
  EZRADIODRV_Pn9Handle_t             pn9;            ///< PN9 plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_DIRECT_TRANSMIT)
  EZRADIODRV_DirectTxHandle_t        directTx;       ///< Direct transmission plug-in handler.
#endif

#if defined(EZRADIO_PLUGIN_DIRECT_RECEIVE)
  EZRADIODRV_DirectRxHandle_t        directRx;       ///< Direct reception plug-in handler.
#endif
};

/// Configuration data for EzRadio plugin manager.
#define EZRADIODRV_INIT_DEFAULT                                                 \
  {                                                                             \
    EZRADIODRV_TRANSMIT_PLUGIN_INIT_DEFAULT      /* Tx plugin init */           \
    EZRADIODRV_RECEIVE_PLUGIN_INIT_DEFAULT       /* Rx plugin init */           \
    EZRADIODRV_CRC_ERROR_PLUGIN_INIT_DEFAULT     /* CRC error plugin init */    \
    EZRADIODRV_AUTO_ACK_PLUGIN_INIT_DEFAULT      /* Auto-ack plugin init */     \
    EZRADIODRV_UNMOD_CARRIER_PLUGIN_INIT_DEFAULT /* CW plugin init */           \
    EZRADIODRV_PN9_PLUGIN_INIT_DEFAULT           /* PN9 plugin init */          \
    EZRADIODRV_DIRECT_TRANSMIT_PLUGIN_INIT_DEFAULT /* Direct Tx plugin init */  \
      EZRADIODRV_DIRECT_RECEIVE_PLUGIN_INIT_DEFAULT /* Direct Rx plugin init */ \
  }

void ezradioInit(EZRADIODRV_Handle_t handle);
Ecode_t ezradioPluginManager(EZRADIODRV_Handle_t handle);
void ezradioResetTRxFifo(void);

/** @} (end addtogroup ezradiodrv_plugin_manager) */
/** @} (end addtogroup ezradiodrv_plugin) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif /* EZRADIO_PLUGIN_MANAGER_H_ */
