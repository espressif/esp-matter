/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef RAIL_MUX_RENAME_H
#define RAIL_MUX_RENAME_H

#include PLATFORM_HEADER
#include "sl_rail_mux.h"
#include "coexistence/protocol/ieee802154_uc/coexistence-802154.h"

#define RAIL_ConfigRxOptions   sl_rail_mux_ConfigRxOptions
#define RAIL_SetTaskPriority   sl_rail_mux_SetTaskPriority
#define RAIL_IEEE802154_Config2p4GHzRadio   sl_rail_mux_IEEE802154_Config2p4GHzRadio
#define RAIL_ConfigEvents   sl_rail_mux_ConfigEvents
#define RAIL_Init   sl_rail_mux_Init
#define RAIL_SetPtiProtocol   sl_rail_mux_SetPtiProtocol
#define RAIL_ConfigChannels   sl_rail_mux_ConfigChannels
#define RAIL_IEEE802154_Init   sl_rail_mux_IEEE802154_Init
#define RAIL_ConvertLqi   sl_rail_mux_ConvertLqi
#define RAIL_Calibrate   sl_rail_mux_Calibrate
#define RAIL_IEEE802154_CalibrateIr2p4Ghz   sl_rail_mux_IEEE802154_CalibrateIr2p4Ghz
#define RAIL_GetRadioEntropy   sl_rail_mux_GetRadioEntropy
#define RAIL_IEEE802154_SetShortAddress   sl_rail_mux_IEEE802154_SetShortAddress
#define RAIL_IEEE802154_SetLongAddress   sl_rail_mux_IEEE802154_SetLongAddress
#define RAIL_IEEE802154_SetPanId   sl_rail_mux_IEEE802154_SetPanId
#define RAIL_IEEE802154_SetPanCoordinator   sl_rail_mux_IEEE802154_SetPanCoordinator
#define RAIL_IEEE802154_SetAddresses sl_rail_mux_IEEE802154_SetAddresses
#define RAIL_IsValidChannel   sl_rail_mux_IsValidChannel
#define RAIL_GetRssi   sl_rail_mux_GetRssi
#define RAIL_GetSymbolRate   sl_rail_mux_GetSymbolRate
#define RAIL_GetBitRate   sl_rail_mux_GetBitRate
#define RAIL_CancelTimer   sl_rail_mux_CancelTimer
#define RAIL_GetPendingCal   sl_rail_mux_GetPendingCal
#define RAIL_CalibrateTemp   sl_rail_mux_CalibrateTemp
#define RAIL_GetRxPacketInfo   sl_rail_mux_GetRxPacketInfo
#define RAIL_GetRxPacketDetailsAlt   sl_rail_mux_GetRxPacketDetailsAlt
#define RAIL_GetRxTimeSyncWordEnd   sl_rail_mux_GetRxTimeSyncWordEnd
#define RAIL_PeekRxPacket   sl_rail_mux_PeekRxPacket
#define RAIL_IEEE802154_GetAddress   sl_rail_mux_IEEE802154_GetAddress
#define RAIL_GetRxIncomingPacketInfo   sl_rail_mux_GetRxIncomingPacketInfo
#define RAIL_IEEE802154_SetFramePending   sl_rail_mux_IEEE802154_SetFramePending
#define RAIL_GetSchedulerStatus   sl_rail_mux_GetSchedulerStatus
#define RAIL_IsRxAutoAckPaused   sl_rail_mux_IsRxAutoAckPaused
#define RAIL_GetTxPowerConfig   sl_rail_mux_GetTxPowerConfig
#define RAIL_ConfigTxPower   sl_rail_mux_ConfigTxPower
#define RAIL_SetTxPowerDbm   sl_rail_mux_SetTxPowerDbm
#define RAIL_SetTxPower sl_rail_mux_SetTxPower
#define RAIL_YieldRadio   sl_rail_mux_YieldRadio
#define RAIL_GetRadioState   sl_rail_mux_GetRadioState
#define RAIL_Idle   sl_rail_mux_Idle
#define RAIL_StartRx   sl_rail_mux_StartRx
#define RAIL_SetTxFifo   sl_rail_mux_SetTxFifo
#define RAIL_StartCcaCsmaTx   sl_rail_mux_StartCcaCsmaTx
#define RAIL_StartTx  sl_rail_mux_StartTx
#define RAIL_GetRadioEntropy   sl_rail_mux_GetRadioEntropy
#define RAIL_StartTxStream   sl_rail_mux_StartTxStream
#define RAIL_StopTxStream   sl_rail_mux_StopTxStream
#define RAIL_IEEE802154_SetPromiscuousMode   sl_rail_mux_IEEE802154_SetPromiscuousMode
#define RAIL_PauseRxAutoAck   sl_rail_mux_PauseRxAutoAck
#define RAIL_SetCcaThreshold   sl_rail_mux_SetCcaThreshold

#define RAIL_SetFreqOffset sl_rail_mux_SetFreqOffset
#define RAIL_SetTimer sl_rail_mux_SetTimer

#define RAIL_IEEE802154_IsEnabled   sl_rail_mux_IEEE802154_IsEnabled
#define RAIL_SetRxTransitions   sl_rail_mux_SetRxTransitions
#define RAIL_ConfigCal   sl_rail_mux_ConfigCal
#define RAIL_EnablePaAutoMode   sl_rail_mux_EnablePaAutoMode
#define RAIL_WriteTxFifo   sl_rail_mux_WriteTxFifo
#define RAIL_ReadRxFifo   sl_rail_mux_ReadRxFifo
#define RAIL_GetRxFifoBytesAvailable   sl_rail_mux_GetRxFifoBytesAvailable
#define RAIL_StartScheduledTx   sl_rail_mux_StartScheduledTx
#define RAIL_StartScheduledCcaCsmaTx sl_rail_mux_StartScheduledCcaCsmaTx
#define RAIL_ScheduleRx sl_rail_mux_RAIL_ScheduleRx
#define RAIL_ConfigSleep   sl_rail_mux_ConfigSleep
#define RAIL_GetRssiOffset sl_rail_mux_GetRssiOffset

#define RAIL_Idle   sl_rail_mux_Idle

#define RAIL_ConfigEvents   sl_rail_mux_ConfigEvents

#define RAIL_StartAverageRssi   sl_rail_mux_StartAverageRssi

#define RAIL_GetBitRate   sl_rail_mux_GetBitRate

#define RAIL_GetTxPowerDbm   sl_rail_mux_GetTxPowerDbm
#define RAIL_GetTxPower   sl_rail_mux_GetTxPower
#define RAIL_GetRxIncomingPacketInfo   sl_rail_mux_GetRxIncomingPacketInfo

#define RAIL_IEEE802154_WriteEnhAck   sl_rail_mux_IEEE802154_WriteEnhAck

#define RAIL_ReleaseRxPacket   sl_rail_mux_ReleaseRxPacket

#define RAIL_HoldRxPacket   sl_rail_mux_HoldRxPacket

#define RAIL_Calibrate   sl_rail_mux_Calibrate

#define RAIL_GetAverageRssi   sl_rail_mux_GetAverageRssi

#define RAIL_GetRxPacketDetailsAlt   sl_rail_mux_GetRxPacketDetailsAlt

#define RAIL_GetRxTimeSyncWordEndAlt   sl_rail_mux_GetRxTimeSyncWordEndAlt

#define RAIL_IEEE802154_GetPtiRadioConfig   sl_rail_mux_IEEE802154_GetPtiRadioConfig

#define RAIL_IEEE802154_EnableEarlyFramePending   sl_rail_mux_IEEE802154_EnableEarlyFramePending

#define RAIL_SetTxFifoThreshold     sl_rail_mux_SetTxFifoThreshold

#define RAIL_SetNextTxRepeat sl_rail_mux_SetNextTxRepeat

#define RAIL_GetChannel sl_rail_mux_GetChannel

#define ATOMIC_SECTION(RETURN_TYPE, yourcode, ...) \
  ({                                               \
    CORE_DECLARE_IRQ_STATE;                        \
    RETURN_TYPE result;                            \
    CORE_ENTER_ATOMIC();                           \
    {                                              \
      result = yourcode(__VA_ARGS__);              \
    }                                              \
    CORE_EXIT_ATOMIC();                            \
    result;                                        \
  })

#define sl_rail_util_coex_set_bool(...)         ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_bool, __VA_ARGS__)
#define sl_rail_util_coex_get_options(...)      ATOMIC_SECTION(sl_rail_util_coex_options_t, sl_rail_util_coex_get_options, __VA_ARGS__)
#define sl_rail_util_coex_set_options(...)      ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_options, __VA_ARGS__)
#define sl_rail_util_coex_set_enable(...)       ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_enable, __VA_ARGS__)
#define sl_rail_util_coex_set_tx_request(...)   ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_tx_request, __VA_ARGS__)
#define sl_rail_util_coex_set_rx_request(...)   ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_rx_request, __VA_ARGS__)
#define sl_rail_util_coex_set_request_pwm(...)  ATOMIC_SECTION(sl_status_t, sl_rail_util_coex_set_request_pwm, __VA_ARGS__)

#endif //RAIL_MUX_RENAME_H
