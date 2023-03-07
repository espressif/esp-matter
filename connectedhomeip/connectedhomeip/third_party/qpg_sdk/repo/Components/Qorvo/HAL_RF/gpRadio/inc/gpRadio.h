/*
 * Copyright (c) 2020, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 */

#ifndef _GPRADIO_H_
#define _GPRADIO_H_

/**
 * @file gpRadio.h
 *
 *
 * @defgroup INIT INIT primitives
 * This module groups the primitives for initialisation.
 *
 * @defgroup CONFIG CONFIG primitives
 * This module groups the primitives for configuring the radio modes
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

//Antenna selection options
/** @name gpRadio_AntennaSelection_t */
//@{
/** @brief Force antenna 0 (termed RF Port 1 or Ant1 in datasheet) to be used*/
#define gpRadio_AntennaSelection_PortRF1  0x0
/** @brief Force antenna 1 (termed RF Port 2 or Ant2 in datasheet) to be used*/
#define gpRadio_AntennaSelection_PortRF2  0x1
/** @brief Automatic antenna seletion based on BBP-RX for RX and on MAC settings for TX */
#define gpRadio_AntennaSelection_Auto     0x2
/** @brief Not possible to dedect which antenna is selected (error condition) */
#define gpRadio_AntennaSelection_Unknown  0xFF
/** @typedef gpRadio_AntennaSelection_t
 *  @brief The gpRadio_AntennaSelection_t type defines the antenna selection mode.
*/
typedef UInt8 gpRadio_AntennaSelection_t;
//@}

/** @name gpRadio_Status_t */
//@{
/** @brief The requested operation was completed successfully. */
#define gpRadio_StatusSuccess                             0x00
/** @brief Requested language is not supported. */
#define gpRadio_StatusNotImplemented                      0x01
/** @brief Invalid parameter before enabling currentRX mode. */
#define gpRadio_StatusInvalidParameter                    0x02
/** @brief Unspecified error condition triggered. */
#define gpRadio_StatusError                               0xFF
/** @typedef gpRadio_Status_t
 *  @brief Return status enumeration.
 */
typedef UInt8                             gpRadio_Status_t;
//@}

/** @name gpRadio_FirFilter_t */
//@{
/** @brief None means: no filtering, i.e. FIR is a dirac impulse. */
#define gpRadio_FirFilter_None                            0x00
/** @brief applies FIR 25 filter values. */
#define gpRadio_FirFilter_FIR25                           0x01
/** @typedef gpRadio_FirFilter_t
 *  @brief The gpRadio_FirFilter_t type defines the FIR filter type that will be used by the radio.
 */
typedef UInt8 gpRadio_FirFilter_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup INIT
 *
 *  @brief Initialisation method
 *
 *  This primitive is for initialisation and typically called from \a gpBaseComps_StackInit().
 *
 */
void gpRadio_Init(void);

/** @ingroup CONFIG
 *
 *  @brief RxMode config
 *
 *  This primitive is for configuring the options for the Rx Mode.
 *
 */
gpRadio_Status_t gpRadio_SetRxMode(Bool enableMultiStandard, Bool enableMultiChannel, Bool enableHighSensitivity);

/** @ingroup CONFIG
 *
 *  @brief RxMode config
 *
 *  This primitive is for getting the options for the Rx Mode.
 *
 *  @param enableMultiStandard         Allows concurrent listening on ZigBee and BLE channels (not compatible with the other two options).
                                       This option is also known as ConcurrentConnect&trade;. Note that this is not available on some older products.
 *
 *  @param enableMultiChannel          Allows listening to multiple ZigBee channels simultaneously (not compatible with the other two options)
 *
 *  @param enableHighSensitivity       Allows for higher sensitivity ZigBee reception (not compatible with the other two options)
 *
 */
gpRadio_Status_t gpRadio_GetRxMode(Bool* enableMultiStandard, Bool* enableMultiChannel, Bool* enableHighSensitivity);

/** @ingroup CONFIG
 *
 *  @brief Antenna config
 *
 *  This primitive is for configuring the rx antenna selection..
 *  This affects this ZB and BLE Rx antenna, but in the current implementation also the BLE TX antenna
 *  since it is always the same as the BLE Rx antenna.
 *
 */
gpRadio_Status_t gpRadio_SetRxAntenna(gpRadio_AntennaSelection_t rxAntenna);

/** @ingroup CONFIG
 *
 *  @brief Antenna config
 *
 *  This primitive is for getting the rx antenna selection..
 *
 */
gpRadio_AntennaSelection_t gpRadio_GetRxAntenna(void);

/** @ingroup CONFIG
 *
 *  @brief FIR filter config
 *
 *  This primitive is for configuring the values for the TX FIR filter.
 *  This affects the ZB transmission.
 *  It is adviced to change this setting at startup, or when there is no radio traffic.
 *  In case the setting would be changed during a frame transmission, it will result
 *  in a corrupted frame.
 *
 */
gpRadio_Status_t gpRadio_SetRadioFirFilter(gpRadio_FirFilter_t firFilter);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPRADIO_H_

