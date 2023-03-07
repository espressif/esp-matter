/***************************************************************************//**
 * @file
 * @brief EzRadio PN9 plug-in managed by the plug-in manager if enabled.
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

#ifndef EZRADIO_PN9_PLUGIN_H_
#define EZRADIO_PN9_PLUGIN_H_

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
 * @addtogroup ezradiodrv_plugin_pn9 EZRADIODRV PN9 Plugin
 * @brief EzRadio Pseudo random transmit plugin, see @ref ezradiodrv_plugin
 *        for detailed documentation.
 * @{
 ******************************************************************************/

#if defined(EZRADIO_PLUGIN_PN9)

/// EzRadio PN9 plug-in instance initialization and handler structure.
typedef struct EZRADIODRV_Pn9Handle{
  uint8_t channel;      ///< PN9 transmit channel.
} EZRADIODRV_Pn9Handle_t;

Ecode_t ezradioStartPn9(EZRADIODRV_Handle_t radioHandle);
Ecode_t ezradioStopPn9(void);

/// Configuration data for EzRadio PN9 plug-in.
#define EZRADIODRV_PN9_PLUGIN_INIT_DEFAULT                             \
  {                             /* PN9                              */ \
    0,                          /* Channel                          */ \
  },

#else //#if defined( EZRADIO_PLUGIN_PN9 )
#define EZRADIODRV_PN9_PLUGIN_INIT_DEFAULT
#endif  //#if defined( EZRADIO_PLUGIN_PN9 )

/** @} (end addtogroup ezradiodrv_plugin_pn9) */
/** @} (end addtogroup ezradiodrv_plugin) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif /* EZRADIO_PN9_PLUGIN_H_ */
