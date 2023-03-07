/***************************************************************************//**
 * @file
 * @brief EzRadio transmit plug-in managed by the plug-in manager if enabled.
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

#ifndef EZRADIO_RECEIVE_PLUGIN_H_
#define EZRADIO_RECEIVE_PLUGIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_plugin
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_plugin_receive EZRADIODRV Receive Plugin
 * @brief EzRadio Receive plugin, see @ref ezradiodrv_plugin
 *        for detailed documentation.
 * @{
 ******************************************************************************/

#if defined(EZRADIO_PLUGIN_RECEIVE)

/// EzRadio receive plug-in instance initialization and handler structure.
typedef struct EZRADIODRV_PacketRxHandle{
  EZRADIODRV_Callback_t userCallback;   ///< User callback.
  uint8_t channel;                      ///< Reception channel.
  uint8_t * pktBuf;                     ///< Pointer to the receive buffer.
} EZRADIODRV_PacketRxHandle_t;

Ecode_t ezradioStartRx(EZRADIODRV_Handle_t radioHandle);

/// Configuration data for EzRadio receive plug-in.
#define EZRADIODRV_RECEIVE_PLUGIN_INIT_DEFAULT                               \
  {                                           /* Packet RX                */ \
    NULL,                                     /* CallBack                 */ \
    RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,  /* Channel                  */ \
    NULL,                                     /* Packet buffer            */ \
  },

#else //#if defined( EZRADIO_PLUGIN_RECEIVE )

#define EZRADIODRV_RECEIVE_PLUGIN_INIT_DEFAULT

#endif //#if defined( EZRADIO_PLUGIN_RECEIVE )

/** @} (end addtogroup ezradiodrv_plugin_receive) */
/** @} (end addtogroup ezradiodrv_plugin) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif /* EZRADIO_RECEIVE_PLUGIN_H_ */
