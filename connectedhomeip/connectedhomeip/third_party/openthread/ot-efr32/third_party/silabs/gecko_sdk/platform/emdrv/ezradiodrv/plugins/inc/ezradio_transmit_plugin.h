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

#ifndef EZRADIO_TRANSMIT_PLUGIN_H_
#define EZRADIO_TRANSMIT_PLUGIN_H_

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
 * @addtogroup ezradiodrv_plugin_transmit EZRADIODRV Transmit Plugin
 * @brief EzRadio Transmit plugin, see @ref ezradiodrv_plugin
 *        for detailed documentation.
 * @{
 ******************************************************************************/

// Transmit plugin related error codes
#define ECODE_EMDRV_EZRADIODRV_TRANSMIT_FAILED          (ECODE_EMDRV_EZRADIODRV_TRANSMIT_PLUGIN_BASE | 0x00000001)     ///< Unable to start transmission.

#if defined(EZRADIO_PLUGIN_TRANSMIT)

/// EzRadio transmit packet length mode
typedef enum EZRADIODRV_TransmitLengthMode{
  ezradiodrvTransmitLenghtDefault               = 0,    ///< Transmit packet length is defined in the generated configuration.
  ezradiodrvTransmitLenghtCustomPacketLen       = 1,    ///< Transmit packet length is defined by the application for field1 only.
  ezradiodrvTransmitLenghtCustomFieldLen        = 2     ///< Transmit packet length is defined by the application for multiple fields.
} EZRADIODRV_TransmitLengthMode_t;

/// EzRadio transmit packet type
typedef enum EZRADIODRV_TransmitPacketType{
  ezradiodrvPacketTypeNormal                    = 0,    ///< Normal packet type.
#if defined(EZRADIO_PLUGIN_AUTO_ACK) && defined(EZRADIO_PLUGIN_RECEIVE)
  ezradiodrvPacketTypeAutoAck                   = 1,    ///< Auto acknowledge packet type.
#endif
} EZRADIODRV_TransmitPacketType_t;

/// EzRadio transmit field length sturcture. Holds length information on all available fields.
/// Data is got from the generated radio configuration header file.
typedef struct EZRADIODRV_FieldLength{
  uint8_t f1;       ///< Field 1 length.
  uint8_t f2;       ///< Field 2 length.
  uint8_t f3;       ///< Field 3 length.
  uint8_t f4;       ///< Field 4 length.
  uint8_t f5;       ///< Field 5 length.
} EZRADIODRV_FieldLength_t;

/// EzRadio transmit packet length configuration structure.
typedef struct EZRADIODRV_PacketLengthConfig{
  EZRADIODRV_TransmitLengthMode_t lenMode;  ///< Length mode.
  uint16_t pktLen;                          ///< Packet length.
  EZRADIODRV_FieldLength_t fieldLen;        ///< Field lengths.
} EZRADIODRV_PacketLengthConfig_t;

/// EzRadio transmit plug-in instance initialization and handler structure.
typedef struct EZRADIODRV_PacketTxHandle{
  EZRADIODRV_Callback_t userCallback;           ///< User callback.
  EZRADIODRV_PacketLengthConfig_t lenConfig;    ///< Packet length configuration.
  uint8_t channel;                              ///< Transmission channel.
  EZRADIODRV_TransmitPacketType_t pktType;      ///< Packet type.
  uint16_t pktId;                               ///< Packet ID.
} EZRADIODRV_PacketTxHandle_t;

Ecode_t ezradioStartTransmitBasic(EZRADIODRV_Handle_t radioHandle, uint16_t packetLength, uint8_t *pioRadioPacket);
Ecode_t ezradioStartTransmitConfigured(EZRADIODRV_Handle_t radioHandle, uint8_t *pioRadioPacket);
Ecode_t ezradioStartTransmitCustom(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_PacketLengthConfig_t pktLengthConf, uint8_t *pioRadioPacket);

Ecode_t ezradioStartTransmitDefault(EZRADIODRV_Handle_t radioHandle, uint8_t *pioRadioPacket);
Ecode_t ezradioStartTransmitSmart(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_PacketLengthConfig_t pktLengthConf, uint8_t *pioRadioPacket);

/// Configuration data for EzRadio transmit plug-in.
#define EZRADIODRV_TRANSMIT_PLUGIN_INIT_DEFAULT                                \
  {                                         /* Packet TX                   */  \
    NULL,                                   /* CallBack                    */  \
    {                                       /* Packet length config        */  \
      ezradiodrvTransmitLenghtDefault,      /* Default length mode         */  \
      0,                                    /* Packet length               */  \
      { 0, 0, 0, 0, 0 }                     /* Field lengths               */  \
    },                                                                         \
    RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER, /* Channel                     */ \
    ezradiodrvPacketTypeNormal,             /* Packet type                 */  \
    0,                                      /* Packet ID                   */  \
  },

#else //#if defined( EZRADIO_PLUGIN_TRANSMIT )
#define EZRADIODRV_TRANSMIT_PLUGIN_INIT_DEFAULT
#endif  //#if defined( EZRADIO_PLUGIN_TRANSMIT )

/** @} (end addtogroup ezradiodrv_plugin_transmit) */
/** @} (end addtogroup ezradiodrv_plugin) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif /* EZRADIO_TRANSMIT_PLUGIN_H_ */
