/*
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */



#ifndef _GPHAL_PHY_H_
#define _GPHAL_PHY_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/** @brief Default Tx Antenna selection
 *
 *  This macros selects the antenna configuration.  By default (macro GP_HAL_DIVERSITY_SINGLE_ANTENNA
 *  not defined), both antennas are used with antenna diversity.
 *  When the macro GP_HAL_DIVERSITY_SINGLE_ANTENNA is defined
 *  through the command line option of the compiler, the single antenna is selected.
*/
#if defined(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#define GP_HAL_TX_ANTENNA  GP_HAL_DIVERSITY_SINGLE_ANTENNA
#else
#define GP_HAL_TX_ANTENNA 0
#endif
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

//Antenna selection options
/** @name gpHal_AntennaSelection_t */
//@{
/** @brief Force antenna 0 (termed RF Port 1 or Ant1 in datasheet) to be used*/
#define gpHal_AntennaSelection_Ant0     0x0
/** @brief Force antenna 1 (termed RF Port 2 or Ant2 in datasheet) to be used*/
#define gpHal_AntennaSelection_Ant1     0x1
/** @brief Automatic antenna seletion based on BBP-RX for RX and on MAC settings for TX */
#define gpHal_AntennaSelection_Auto     0x2
/** @brief Not possible to dedect which antenna is selected (error condition) */
#define gpHal_AntennaSelection_Unknown  0x3
/** @typedef gpHal_AntennaSelection_t
 *  @brief The gpHal_AntennaSelection_t type defines the antenna selection mode.
*/
typedef UInt8 gpHal_AntennaSelection_t;
//@}

/** @name gpHal_FirFilter_t */
//@{
/** @brief None means: no filtering, i.e. FIR is a dirac impulse. */
#define gpHal_FirFilter_None                            0x00
/** @brief applies FIR 25 filter values. */
#define gpHal_FirFilter_FIR25                           0x01
/** @typedef gpHal_FirFilter_t
 *  @brief The gpHal_FirFilter_t type defines the FIR filter type that will be used by the radio.
 */
typedef UInt8 gpHal_FirFilter_t;
//@}

/** @name gpHal_ContinuousWaveMode_t */
//@{
/** @brief Unmodulated Continuous Wave Mode */
#define gpHal_CW_Unmodulated              'U' /*0x55*/
/** @brief Unmodulated (ble channelindex + unmodulated ) */
#define gpHal_CW_Ble_UnModulated          'D' /*0x44*/
/** @brief Modulated Continuous Wave Mode */
#define gpHal_CW_Modulated                'M' /*0x4D*/
/** @brief Modulated BLE (ble channelindex + blemode) */
#define gpHal_CW_Ble_Modulated            'B' /*0x42*/
/** @brief High Data Rate Modulated BLE (ble channelindex + blemode) */
#define gpHal_CW_Ble_HDRModulated         'H' /*0x48*/
/** @brief Continuous Wave Mode is off */
#define gpHal_CW_Off                      'O' /*0x4F*/
/** @typedef gpHal_ContinuousWaveMode_t
 *  @brief The gpHal_ContinuousWaveMode_t type defines the different continuous wave modes.
*/
typedef UInt8 gpHal_ContinuousWaveMode_t;
//@}

/** @name gpHal_TxPower_t*/
//@{
/** @brief Default transmit power configured by gpHal_SetDefaultTransmitPowers. */
#define gpHal_MinTransmitPower         0x7d
#define gpHal_MaxTransmitPower         0x7e
#define gpHal_DefaultTransmitPower     0x7f
/** @typedef gpHal_TxPower_t
 *  @brief The gpHal_TxPower_t type defines the transmit power in dBm or the default, min or max define.
*/
typedef Int8 gpHal_TxPower_t;


/** @name gpHal_CCAMode_t*/
//@{
/** @brief CCA based on Energy Detect will be performed. */
#define gpHal_CCAModeEnergy                 0x1
/** @brief CCA based on detection of modulated carrier will be performed. */
#define gpHal_CCAModeModulatedCarrier       0x2
/** @brief CCA based on an AND of Energy and Modulated Carrier Detect will be performed. */
#define gpHal_CCAModeEnergyAndModulated     0x3
/** @typedef gpHal_CCAMode_t
 *  @brief The gpHal_CCAMode_t type defines the CCA measurement method.
*/
typedef UInt8 gpHal_CCAMode_t;
//@}

/** @name gpHal_CollisionAvoidanceMode_t */
//@{
/** @brief No CCA nor CSMA will be performed. */
#define gpHal_CollisionAvoidanceModeNoCCA   0
/** @brief Only CCA will be performed. */
#define gpHal_CollisionAvoidanceModeCCA     1
/** @brief CSMA will be performed. */
#define gpHal_CollisionAvoidanceModeCSMA    2
/** @typedef gpHal_CollisionAvoidanceMode_t
 *  @brief The gpHal_CollisionAvoidanceMode_t type defines the CSMA/CA method in TX.
*/
typedef UInt8 gpHal_CollisionAvoidanceMode_t;
//@}

// RxMode selection options
/** @name gpHal_RxMode_t */
//@{
/** @brief Mode Normal to be used*/
#define gpHal_RxModeNormal               0x1
#define gpHal_RxModeHighSensitivity      0x1 /* new name for the normal mode */
/** @brief Mode Low Power Listening to be used*/
#define gpHal_RxModeLowPower             0x2
/** @brief Mode Multi Channel Listening to be used*/
#define gpHal_RxModeMultiChannel         0x4
/** @brief Mode Multi Standard Listening to be used*/
#define gpHal_RxModeMultiStandard        0x5
/** @typedef gpHal_RxMode_t
    @brief The gpHal_RxMode_t type defines the receiver mode selection.
*/
typedef UInt8 gpHal_RxMode_t;
//@}

#define gpHal_AntennaModeDiversity       0x0
#define gpHal_AntennaModeAntenna0        0x1
#define gpHal_AntennaModeAntenna1        0x2
typedef UInt8 gpHal_AntennaMode_t;

typedef UInt8 gpHal_Channel_t;

//Channelslot identifier
/** @name gpHal_ChannelSlotIdentifier_t */
//@{
/** @brief ChannelSlot for first slot */
#define gpHal_ChannelSlotIdentifier_0   0x0
/** @brief ChannelSlot for second slot */
#define gpHal_ChannelSlotIdentifier_1   0x1
/** @brief ChannelSlot for third slot */
#define gpHal_ChannelSlotIdentifier_2   0x2
/** @brief ChannelSlot for fourth slot */
#define gpHal_ChannelSlotIdentifier_3   0x3
/** @brief ChannelSlot for fifth slot */
#define gpHal_ChannelSlotIdentifier_4   0x4
/** @brief ChannelSlot for sixth slot */
#define gpHal_ChannelSlotIdentifier_5   0x5
/** @typedef gpHal_ChannelSlotIdentifier_t
 *  @brief The gpHal_ChannelSlotIdentifier_t type defines the channel slot used
*/
typedef UInt8 gpHal_ChannelSlotIdentifier_t;
//@}

#if defined(GP_DIVERSITY_GPHAL_K8E) 

/** @brief Set RX as antena diversity, so the current load will always be the same, indepent of the current configuration
*/
void gpHal_PrepareRadioForLoad(void);

/** @brief Restore the right configuration to the radio after performing the current measurement.
*/
void gpHal_RestoreRadioFromLoad(void);
#endif
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

/** @brief Set continuous wave mode on/off.
 *
 *  This function sets the continuous wave mode
 *
 *  @param mode  Continuous wave mode can be Unmodulated, Modulated or Off.
 *  @param channel channel on which CW will be generated
 *  @param txpower power at which the CW will be generated (in dB)
 *  @param antenna antenna on which CW will be generated. Auto will default to Ant0 or the antenna selected with GP_HAL_DIVERSITY_SINGLE_ANTENNA diversity
*/
void gpHal_SetContinuousWaveMode( gpHal_ContinuousWaveMode_t mode, UInt8 channel, Int8 txpower, gpHal_AntennaSelection_t antenna);

void gpHal_CalibrateFllChannel(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel);
// Same functionality as gpHal_CalibrateFllChannel, but without claiming the radio (the caller function is responsible for this)
void gpHal_CalibrateFllChannelUnprotected(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel);

/** @brief Claim radio management and wait for it to be granted
*/
void gpHal_ClaimRadioMgmt(void);

/** @brief Release radio management
  */
void gpHal_ReleaseRadioMgmt(void);

UInt8 gpHalPhy_FillInModeSignals(UInt8 txEn, UInt8 rxEn, UInt8 mode);
void gpHalPhy_SetTxAntenna(UInt8 antenna);
UInt8 gpHalPhy_GetTxAntenna(void);

/** @brief RX antenna select
 * intAntSel - Select internal antenna port
 * extAntSel - Select external antenna port
 */
void gpHalPhy_UpdateRxAntennaSelection(UInt8 intAntSel, UInt8 extAntSel);

/** @brief Initialize signal to control an MODE bit for an external antenna switch */
GP_API void gpHalPhy_InitExternalModeSignal(UInt8 mode_signal);


void gpHal_InitPhy(void);

#if defined(GP_DIVERSITY_GPHAL_K8E) 
void gpHal_SetRxLnaAttDuringTimeoutForRssiBasedAgcMode(Bool enable);
#endif // defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)

/** @brief Gets the antenna used to transmit a packet
 *
 *  This function queries the antenna used to transmit a packet
 *  @return the antenna used to transmit a packet, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API gpHal_AntennaSelection_t gpHal_GetTxAntenna(void);

/** @brief Gets the antenna used to receive packets
 *
 *  This function queries the antenna used to receive packets
 *  @return the antenna used to receive packets, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API gpHal_AntennaSelection_t gpHal_GetRxAntenna(void);

/** @brief Sets the antenna used to transmit and receiving packets
 *
 *  This function selects the used antenna used to transmit a packet
 *  Note that this function is obsolete, gpHal_SetRxAntenna should be used instead.
 *  @param antenna The antenna used to transmit and receive packets, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API void gpHal_SetAntenna(gpHal_AntennaSelection_t antenna);

/** @brief Sets the antenna used to receive packets
 *
 *  This function selects the used antenna used to receive a packet
 *  @param antenna The antenna used to receive packets, possible values defined in enumeration gpHal_AntennaSelection_t
 */
GP_API void gpHal_SetRxAntenna(gpHal_AntennaSelection_t antenna);

/** @brief This function returns the antenna which is used by the BLE stack.
 *
 *  @return gpHal_AntennaSelection_t The antenna which is used
 */
GP_API gpHal_AntennaSelection_t gpHal_GetBleAntenna(void);

/** @brief Returns antenna diversity mode
*
*   @return true if antenna diversity is enabled
*/
GP_API Bool gpHal_GetRxAntennaDiversity(void);

/** @brief Sets the FIR filter config
 *
 *  This primitive is for configuring the values for the TX FIR filter.
 *  This affects the ZB transmission.
 *  @param firFilter An enum value that corresponds to one of the predefined sets of FIR filter coefficients.
 */
void gpHal_SetRadioFirFilter(gpHal_FirFilter_t firFilter);

#ifdef __cplusplus
}
#endif

#endif // _GPHAL_PHY_H_
