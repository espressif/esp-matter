/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
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



#ifndef _GPHAL_MAC_H_
#define _GPHAL_MAC_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode_MAC.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/** @file gpHal_MAC.h
 *  @brief This file contains all the functions needed for MAC functionality.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"
#include "gpHal_Phy.h"
#include "gpPd.h"
#include "gpPad.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief Default value of the time to live setting of a PBM entry. */
#define GPHAL_TTL_START_VALUE           20

/** @brief Maximum amount of channels used to do multi channel retries */
#define GP_HAL_MULTICHANNEL_MAX_CHANNELS 3

/** @brief Maximum number of slots (simultaneously active RX channels) */
#define GP_HAL_MAX_NUM_OF_SLOTS         6

/** @brief  Define to ignore channel used in the multiChannel options struct */
#define GP_HAL_MULTICHANNEL_INVALID_CHANNEL 0xFF

/** @brief  Offset in IEEE packet to check if Ack Request is required */
#define GPHAL_ACK_REQ_LSB                  5

// the result from formula 14 from the IEEE-802.15.4 spec is 31776 us ==> 0x7C2 symbols
#define GPHAL_POLL_REQ_MAX_WAIT_TIME    0x7C2

#define GPHAL_MAX_15_4_FCS_LENGTH               2

#define GPHAL_MAX_15_4_PACKET_LENGTH            127UL

/** @brief Maximum length of a payload that can be written into a PBM entry. */
#define GPHAL_MAX_15_4_PACKET_LENGTH_NO_FCS     (GPHAL_MAX_15_4_PACKET_LENGTH - GPHAL_MAX_15_4_FCS_LENGTH)


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
//------------------------------------------------------------------------------
// ENUMERATIONS
//------------------------------------------------------------------------------



//@{
/** @name gpHal_MacScenario_t */
// MacScenarios
#define gpHal_MacDefault               0x0
/** gpHal_MacSPollReq Mac Scenario to send a Poll Req*/
#define gpHal_MacPollReq               0x1
/** gpHal_MacTimedTx Put frame on timed TX queue; will be sent at next event gpHal_EventTypeTXPacket */
#define gpHal_MacTimedTx               0x2
/** gpHal_MacManualCrc Mac scenario to manually set  a CRC checksum (a corrupt one if applicable for the test) */
#define gpHal_MacManualCrc             0x3
/** gpHal_MacManualCrc Mac scenario to manually set  a CRC checksum (a corrupt one if applicable for the test), and force the retries tp 0 */
#define gpHal_MacManualCrc_NoRetries   0x4
/** @typedef gpHal_MacScenario_t
 *  @brief The gpHal_MacScenario_t type defines the Mac Scenario as defined in the databook.
*/
typedef UInt8 gpHal_MacScenario_t;
//@}




/** @name gpHal_SourceIdentifier_t */
//@{
/** @brief Identifier for first Pan (pan 0) */
#define gpHal_SourceIdentifier_0    0x0
/** @brief Identifier for second Pan (pan 1) */
#define gpHal_SourceIdentifier_1    0x1
/** @brief Identifier for third Pan (pan 2) */
#define gpHal_SourceIdentifier_2    0x2
/** @brief Identifier for invalid value */
#define gpHal_SourceIdentifier_Inv    0xFF
/** @typedef gpHal_SourceIdentifier_t
 *  @brief A source identifier refers to a group of settings (address, PAN, channel).
 *
 *  The number of supported source identifiers depends on the device type and is specified as GP_HAL_NUMBER_OF_RX_SRCIDS.
 */
typedef UInt8 gpHal_SourceIdentifier_t;
//@}

/** @brief This function sets the rx mode configuration for the MAC part of the radio.
 *
 *  This function sets the rx mode configuration for the MAC part of the radio.
 *
 *  @param enableMultiStandard         Allows concurrent listening on ZigBee and BLE channels (not compatible with the other two options).
                                       This option is also known as ConcurrentConnect&trade;. Note that this is not available on some older products.
 *
 *  @param enableMultiChannel          Allows listening to multiple ZigBee channels simultaneously (not compatible with the other two options)
 *
 *  @param enableHighSensitivity       Allows for higher sensitivity ZigBee reception (not compatible with the other two options)
 *
 *  @return gpHal_Result_t
 *  Possible results are :
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidParameter      (invalid combination was selected)
 */
gpHal_Result_t gpHal_SetMacRxMode(Bool enableMultiStandard, Bool enableMultiChannel, Bool enableHighSensitivity);

/** @brief This function gets the rx mode configuration for the MAC part of the radio.
 *
 *  This function gets the rx mode configuration for the MAC part of the radio.
 *
 *  @param enableMultiStandard         Pointer to Bool indicating: concurrent listening on ZigBee and BLE channels (not compatible with the other two options)
 *
 *  @param enableMultiChannel          Pointer to Bool indicating: listening to multiple ZigBee channels simultaniously (not compatible with the other two options)
 *
 *  @param enableHighSensitivity       Pointer to Bool indicating: for higher sensitivity ZigBee reception (not compatible with the other two options)
 *
 *  @return void
 */
void gpHal_GetMacRxMode(Bool* enableMultiStandard, Bool* enableMultiChannel, Bool* enableHighSensitivity);

//@{
/** @struct gpHal_DataReqOptions_t
 *  @param gpHal_MacScenario_t
 *  @brief These options dictate the way a data packet should be transmitted
*/
typedef struct gpHal_DataReqOptions_s{
    gpHal_MacScenario_t             macScenario;
    gpHal_SourceIdentifier_t        srcId;
}gpHal_DataReqOptions_t;
//@}



/** @typedef gpHal_DataIndicationCallback_t
 *  @brief   The gpHal_DataIndicationCallback_t callback type definition defines the callback prototype of the DataIndication.*/
typedef void (*gpHal_DataIndicationCallback_t)(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);

/** @typedef gpHal_SnifferDataIndicationCallback_t
 *  @brief   The gpHal_SnifferDataIndicationCallback_t callback type definition defines the callback prototype of the SnifferDataIndication.*/
typedef void (*gpHal_SnifferDataIndicationCallback_t)(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);

/** @typedef gpHal_DataConfirmCallback_t
 *  @brief   The gpHal_DataConfirmCallback_t callback type definition defines the callback prototype of the DataConfirm.*/
typedef void (*gpHal_DataConfirmCallback_t)(UInt8 status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);

/** @typedef gpHal_EDConfirmCallback_t
 *  @brief   The gpHal_EDConfirmCallback_t callback typedef defines the callback prototype of the EDConfirm.
 *
 *  The parameter protoED isn't the real energy level.  The real energy level needs to be calculated with the function gpHal_CalculateED().
 */
typedef void (*gpHal_EDConfirmCallback_t)(UInt16 channelMask, UInt8 *protoED);

/** @typedef gpHal_BusyTXCallback_t
 *  @brief   The gpHal_BusyTXCallback_t callback type definition defines the callback prototype of the BusyTX interrupt. */
typedef void (*gpHal_BusyTXCallback_t)(void);

/** @typedef gpHal_EmptyQueueCallback_t
 *  @brief   The gpHal_EmptyQueueCallback_t callback type definition defines the callback prototype of the EmptyQueue interrupt. */
typedef void (*gpHal_EmptyQueueCallback_t)(void);

/** @typedef gpHal_CmdDataReqCallback_t
 *  @brief   The gpHal_CmdDataReqCallback_t callback type definition defines the callback prototype of the Cmd Data Req interrupt. */
typedef void (*gpHal_CmdDataReqCallback_t)(void);

typedef void (*gpHal_MacFrameQueued_t)(void);
typedef void (*gpHal_MacFrameUnqueued_t)(void);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
// see gpHal_DEFS.h
// MAC public functions

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_MAC.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

//MAC-SEC interrupts
/**
 * @brief Enables the interrupt line of the MAC and SEC operations.
 *
 * This function sets the interrupt mask of the PIO block.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnablePrimitiveCallbackInterrupt(enable)      GP_HAL_ENABLE_PIO_INT(enable)
/**
 * @brief Enables the interrupt line of the Empty Queue interrupt.
 *
 * This function sets the interrupt mask of the Empty Queue interrupt.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnableEmptyQueueCallbackInterrupt(enable)     GP_HAL_ENABLE_EMPTY_QUEUE_CALLBACK_INTERRUPT(enable)

/**
 * @brief Enables the interrupt line of the BusyTX interrupt.
 *
 * This function sets the interrupt mask of the BusyTX interrupt.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnableBusyTXCallbackInterrupt(enable)     GP_HAL_ENABLE_BUSY_TX_CALLBACK_INTERRUPT(enable)
/**
 * @brief Registers the callback for a DataConfirm.
 *
 * This function registers the callback for a DataConfirm. The callback will be executed on a DataConfirm interrupt.
 * This DataConfirm will be given after a DataRequest is finished.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterDataConfirmCallback(gpHal_DataConfirmCallback_t callback);

/**
 * @brief Registers the callback for a DataIndication.
 *
 * This function registers the DataIndication callback. The callback will be executed on a DataIndication interrupt
 * This DataIndication will be given if a packet is received.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterDataIndicationCallback(gpHal_DataIndicationCallback_t callback);


/**
 * @brief Registers the callback for a EDConfirm.
 *
 * This function registers the EDConfirm callback. The callback will be executed on a EDConfirm interrupt
 * This EDConfirm will be given after a EDRequest is finished
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterEDConfirmCallback(gpHal_EDConfirmCallback_t callback);

/**
 * @brief Registers the callback for a BusyTX interrupt.
 *
 * This function registers the BusyTX callback. The callback will be executed on a BusyTX interrupt, i.e.
 * is triggered when the MAC receives a TX trigger while he is already transmitting another packet.
 * The BusyTX interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterBusyTXCallback(gpHal_BusyTXCallback_t callback);

/**
 * @brief Registers the callback for Empty Queue interrupt.
 *
 * This function registers the EmptyQueue callback.  The callback will be executed on an Empty Queue interrupt, i.e.
 * is triggered when a TX trigger is given to the MAC when no packet is pending in the TX queue.
 * The Empty Queue interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterEmptyQueueCallback(gpHal_EmptyQueueCallback_t callback);

/**
 * @brief Registers the callback for the reception of a Cmd Data Req interrupt.
 *
 * This function registers the CmdDataReq callback.
 * This function determines whether the Frame Pending bit in an Ack needs to be set.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterCmdDataReqConfirmCallback(gpHal_CmdDataReqCallback_t callback);

/** @brief Start a data transmission.
 *
 *  Performs a DataRequest(according to the IEEE802.15.4 specification).
 *  The DataConfirm function can be registered as a callback using gpHal_RegisterDataConfirmCallback().
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy              (no packet buffer available)
 *          - gpHal_ResultInvalidParameter
 *
 *  @param dataReqOptions       csma, multiChannelOptions, macScenario
 *  @param pdLoh                The packet descriptor structure that contains length, offset and unique handle of the packet content.
*/
GP_API gpHal_Result_t gpHal_DataRequest(gpHal_DataReqOptions_t *dataReqOptions, gpPad_Handle_t padHandle, gpPd_Loh_t pdLoh);

/** @brief Returns the listening channel currently used
 *
 *  @param srcId The source identifier.
*/
GP_API UInt8 gpHal_GetRxChannel(gpHal_SourceIdentifier_t srcId);

/** @brief This function returns the last 802.15.4 channel used to transmit on.
 *
 *  This function returns the last 802.15.4 channel used to transmit on.
 *
 *  @param  PBMentry   The last transmitted PBM
 *  @return channel    The last 802.15.4 channel the radio transmitted on
 *
 */
GP_API UInt8 gpHal_GetLastUsedChannel(UInt8 PBMentry);


/** @brief Configure the default transmit power for each channel
 *
 *  @param pointer to 16 byte array with default transmit power for each IEEE channel (11..26).
 *
*/
GP_API void gpHal_SetDefaultTransmitPowers(gpHal_TxPower_t* pDefaultTransmitPowerTable );

/** @brief Get the current default transmit power for the specified channel.
 */
GP_API gpHal_TxPower_t gpHal_GetDefaultTransmitPower(gpHal_Channel_t channel);

/** @brief Set the CCA Threshold setting */

GP_API void gpHal_SetCCAThreshold(void);





/** @brief Writes data in the specified pbm address*/
#define gpHal_WriteDataInPbm(address,pData,length,offset)             GP_HAL_WRITE_DATA_IN_PBM(address,pData,length,offset)

#define gpHal_CalculateTxPbmDataBufferAddress(pbmEntry)            GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS(pbmEntry)

#define gpHal_CheckPbmValid(pbmEntry)             GP_HAL_CHECK_PBM_VALID(pbmEntry)

/** @brief Sets the Rx packet in packet mode
 *
 *  The function sets the Rx packet in packet mode on or off
 *
*/
#if defined(GP_DIVERSITY_GPHAL_K8E) 
GP_API void gpHal_SetPipMode(Bool pipmode);
#else
#define gpHal_SetPipMode(pipmode)               GP_HAL_SET_PIP_MODE(pipmode)
#endif //defined(GP_DIVERSITY_GPHAL_USE_HAL_FUNCTIONS) || defined (GP_DIVERSITY_GPHAL_K5)

/** @brief Returns the number packet in packet mode */
#define gpHal_GetPipMode()                      GP_HAL_GET_PIP_MODE()


GP_API gpHal_Result_t gpHal_GetRadioState(void);

// MAC ED Request
/** @brief Performs a Energy Detect request according to the IEEE802.15.4 spec
 *
 *  This function triggers an energy detection. The energy value of this function is given in the
 *  EDConfirm callback (to be registered with gpHal_RegisterEDConfirmCallback).
 *
 *  To stop an ongoing ED Request, call this function again with time_us 0 and channelMask 0x0000.
 *  The scan will be aborted and the confirm will be generated with the results up to that point.
 *  Note this stop request will not generate a confirm.
 *
 *  Possible results are :
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy      (no packet buffer available)
 *
 *  @param time_us      Time period to scan on each channel given in the channelMask (in us).
 *  @param channelMask  Mask of channels to scan. LSB bit = channel 11, MSB bit = channel 26.
*/
GP_API gpHal_Result_t gpHal_EDRequest(UInt32 time_us , UInt16 channelMask);

/** @brief Set the PAN ID
 *
 *  This function sets the PAN ID .
 *  Setting the PAN ID of your network enables the automatic filter of
 *  packets not intended for your device.
 *
 *  @param panId The PAN ID of the network.
 *  @param srcId The PAN src, we want to change the ID from.
*/
GP_API void gpHal_SetPanId(UInt16 panId, gpHal_SourceIdentifier_t srcId);

/** @brief Returns the PAN ID stored
 *
 *  @param srcId The src id of the Pan.
 **/
GP_API UInt16 gpHal_GetPanId(gpHal_SourceIdentifier_t srcId);

/** @brief Sets the Short Address.
 *
 *  This functions sets the Short Address.
 *  Setting the Short Address of your device enables the automatic filter of
 *  packets not intended for your device.
 *
 *  @param shortAddress The Short Address of the device.
 *  @param srcId The src id of the pan where we want to change the short address.
*/
GP_API void gpHal_SetShortAddress(UInt16 shortAddress, gpHal_SourceIdentifier_t srcId);

/** @brief Returns the ShortAddress
 *
 *  @param srcId The src id of the pan where we want to get the short address.
 */
GP_API UInt16 gpHal_GetShortAddress(gpHal_SourceIdentifier_t srcId);

/** @brief Set the Extended Address
 *
 *  This function sets the Extended Address.
 *  Setting the Extended Address of your device enables the automatic filter of
 *  packets not intended for your device.
 *  @param pExtendedAddress The pointer to the Extended Address of the device.
 *  @param srcId The src id of the extended address.
*/
GP_API void gpHal_SetExtendedAddress(MACAddress_t* pExtendedAddress, gpHal_SourceIdentifier_t srcId);

/** @brief Returns the ExtendedAddress stored
 *
 *  This function returns the extended address stored.
 *  @param pExtendedAddress pointer where the Extended Address is read back to
 *  @param srcId The src id of the extended address.
*/
GP_API void gpHal_GetExtendedAddress(MACAddress_t* pExtendedAddress, gpHal_SourceIdentifier_t srcId);

/** @brief Resets the ExtendedAddress to its factory value
 *
 *  This function resets the extended address to its initial factory value.
 *  @param srcId The src id of the extended address.
*/
GP_API void gpHal_ResetExtendedAddress(gpHal_SourceIdentifier_t srcId);

/** @brief Set the Coordinator Address
 *
 *  Setting the Coordinator Address enables the filtering of
 *  packets not coming from the coordinator.
 *  @param Address The pointer to the Address of the coordinator.
*/
GP_API void gpHal_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress);
GP_API void gpHal_SetCoordShortAddress(UInt16 shortCoordAddress);


/** @brief Set the pan coordinator property
 *
 *  This function sets the pan coordinator property of the device.
 *
 *  @param panCoordinator true if the device is the pan coordinator, false otherwise.
*/
GP_API void gpHal_SetPanCoordinator(Bool panCoordinator);

/** @brief Returns the pan coordinator property of this device */
GP_API Bool gpHal_GetPanCoordinator(void);

/** @brief Enables/Disables Address Recognition.
 *
 *  This function sets the address recognition options.
 *  @param enable           Possible values are :
 *                           - set to true : destination address of a packet will checked against the address (set by gpHal_SetExtendedAddress and gpHal_SetShortAddress) and destination PAN ID of incoming packets.
 *                           - set to false: address recognition disabled.
 *  @param panCoordinator   Possible values are :
 *                           - set to true : The device is a PAN coordinator.  He will accept messages without a destination address.
 *                           - set to false: Normal filtering will be applied according to recognition settings.
*/
GP_API void gpHal_SetAddressRecognition(Bool enable, Bool panCoordinator);

/** @brief Returns the addressRecognition flag. */
#define gpHal_GetAddressRecognition()   GP_HAL_GET_ADDRESS_RECOGNITION()

/** @brief Sets the HW FrameType FilterMask.
 *
 *  This function sets the Frame Type filter mask used by HW
 *
 *  Packets can be filtered out based on their frametype (BCN, DATA, CMD, RSV).
 *  This can be controlled by setting the filter bitmap with this function.
 *  Setting a certain type's mask bit to 1 will filter out the packet with that type.
 *
 *  @param bitmap The FrameType FilterMask.
*/
#define gpHal_SetFrameTypeFilterMask(bitmap)    GP_HAL_SET_FRAME_TYPE_FILTER_MASK((bitmap))

/** @brief Returns the HW FrameType FilterMask.
 *
 *  This function returns the Frame Type filter mask set in HW.
 *  Frametypes which have their mask bit set will be filtered out.
 *
 *  @return bitmap The FrameType FilterMask.
*/
#define gpHal_GetFrameTypeFilterMask()              GP_HAL_GET_FRAME_TYPE_FILTER_MASK()

/** @brief Sets the property for filtering beacons based on src pan.
 *
 *  This function sets the  property for filtering beacons based on src pan.
 *
 *  @param enable If we want to enable beacon filtering on src pan or not.
*/
GP_API void gpHal_SetBeaconSrcPanChecking(Bool enable);


/** @brief Gets the property for filtering beacons based on src pan.
 *
 *  This function gets the  property for filtering beacons based on src pan.
 *
*/
GP_API Bool gpHal_GetBeaconSrcPanChecking(void);

/** @brief Sets the RxOnWhenIdle flag.
 *
 *  This function sets the RxOnWhenIdle flag.
 *  Turns on the receiver when the device is idle.  Switching between
 *  TX and RX is done automatically.
 *
 *  @param srcId Source identifier.
 *  @param flag Possible values are :
 *              - set to true : RxOnWhenIdle is activated and the radio is turned on.
 *              - set to false: RxOnWhenIdle is deactivated.
 *  @param channel channel to enable radio on
*/
GP_API void gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_t srcId, Bool flag, UInt8 channel);
/** @brief Returns the RxOnWhenIdle flag. */
#define gpHal_GetRxOnWhenIdle()                 GP_HAL_GET_RX_ON_WHEN_IDLE()

/** @brief Sets the Auto Acknowledge flag.
 *
 *  This function sets the Auto Acknowledge flag.
 *  All packets addressed to the device (see address recognition) will be automatically acknowledge
 *  (if requested by the MAC headerof the packet).
 *  @param flag
 *              - Set to true : Automatic acknowledgement enabled.
 *              - Set to false: Automatic acknowledgement disabled.
*/
GP_API void gpHal_SetAutoAcknowledge(Bool flag);
/** @brief Returns the AutoAcknowledge flag. */
GP_API Bool gpHal_GetAutoAcknowledge(void);

extern UInt8 gpHal_MacState;

/** @brief Sets the chip to timed MAC mode
 *
 * This function sets the chip to timed MAC mode.
 * When a timed MAC is used all transmission is done using scheduled triggers from the Event Scheduler (ES).
 * This function may only be called once after the initialization of the stack.
 */
GP_API void gpHal_SetTimedMode(Bool timedMode);

/** @brief Calculate the protoRSSI from the protoED returned by the data indication handler.
 *
 *  This function calculates the protoRSSI from the protoED value returned by the data indication handler.
 *  @param protoRSSI Value returned by data indication handler.
*/
GP_API UInt8 gpHal_ConvertProtoEDToProtoRSSI( UInt8 protoED );

/** @brief Calculate the ED value from the protoED returned by the ED scan handler, values are conform the ZIP phy testspec.
 *
 *  This function calculates the ED value from the protoED returned by the ED scan handler. The lowest value is 0, which is at -75dBm, the highest value is 0xFF, which is at -35dBm.
 *  @param protoED  Value returned by ED scan handler.
*/
GP_API UInt8 gpHal_CalculateED(UInt8 protoED);

/** @brief Checks if a NO LOCK was triggerd by the radio.
 *
 *  This function reports if a lock loss was detected by the radio.
 *  @return Result of the check.
*/
GP_API Bool gpHal_CheckNoLock(void);

/** @brief Enables the promiscuous mode.
 *
 *  In promiscuous mode, all packets will be received. In order to enable the receiver the
 *  RxOnWhenIdle flag must be set.
 *
 *  @param flag Possible values are :
 *              - set to true : Promiscuous mode is enabled and the filters disabled.
 *              - set to false: Normal filtering is applied on incoming packets.
*/
GP_API void gpHal_SetPromiscuousMode(Bool flag);

/** @brief Returns promiscuous mode state.
*/
GP_API Bool gpHal_GetPromiscuousMode(void);


/** @brief Set the default ack frame pending bit. */
#if defined(GP_DIVERSITY_GPHAL_K8E) 
GP_API void gpHal_SetFramePendingAckDefault(Bool enable);
#else //GP_DIVERSITY_GPHAL_USE_HAL_FUNCTIONS
#define gpHal_SetFramePendingAckDefault(enable)  GP_HAL_WRITE_PROP(GPHAL_PROP_RIB_FP_ACK_DEFAULT_VALUE , enable );
#endif //GP_DIVERSITY_GPHAL_USE_HAL_FUNCTIONS

/** @brief Returns the default ack frame pending bit. */
#if defined(GP_DIVERSITY_GPHAL_K8E) 
GP_API Bool gpHal_GetFramePendingAckDefault(void);
#else //GP_DIVERSITY_GPHAL_USE_HAL_FUNCTIONS
#define gpHal_GetFramePendingAckDefault()   GP_HAL_GET_ACK_DATA_PENDING()
#endif //GP_DIVERSITY_GPHAL_USE_HAL_FUNCTIONS


/** @brief Get the last used chip transmit power
*/
GP_API gpHal_TxPower_t gpHal_GetLastUsedTxPower(void);

/** @brief Reset the history of the Tx power compensation.
*/


#if defined(GP_DIVERSITY_GPHAL_K8E) 
// Tmp, see SW-5181 for cleanup
GP_API void gpHal_RegisterMacFrameQueuedCallback(gpHal_MacFrameQueued_t callback);
GP_API void gpHal_RegisterMacFrameUnqueuedCallback(gpHal_MacFrameUnqueued_t callback);
GP_API Bool gpHal_IsMacQueueEmpty(void);
#endif

/** @brief This function gets the number of channels which can be used by different Zigbee/MAC stacks simultaniously.
 *
 *  This function gets the number of channels on which different Zigbee/MAC stacks can listen simultaniously
 *
 *  @return UInt8                   The number of indexes on which a seperate rx channel can be configured.
 *
 */
UInt8 gpHal_GetAvailableSrcIds(void);

#if defined(GP_DIVERSITY_OOB)
/** @brief This function set channel frequency for OOB.
 *
 *  This function set channel frequency, it is for supporting OOB
 *
 *  @param frequency                channel frequency (in mHz)
 *
 */
GP_API  void gpHal_SetChannelFrequency_OOB(UInt16 frequency);

/** @brief This function cancel channel frequency for OOB.
 *
 *  This function cancel the channel frequency to be used for OOB
 *
 */
GP_API  void gpHal_CancelChannelFrequency_OOB(void);

/** @brief This function cancel channel frequency for OOB.
 *
 *  This function restore the channel frequency to be used for OOB
 *
 */
GP_API  void gpHal_RestoreChannelFrequency_OOB(void);
#endif //GP_DIVERSITY_OOB


#if defined(GP_DIVERSITY_JUMPTABLES)
GP_API Int8 gpHalPhy_GetMinTransmitPower(void);
GP_API Int8 gpHalPhy_GetMaxTransmitPower(void);
#endif //defined(GP_DIVERSITY_JUMPTABLES)

void gpHal_MacSetMaxTransferTime(UInt32 MacMaxTransferTime);


/** @brief This function enables the Enhanced FramePending behavior.
 *
 *  This function enables the Enhanced FramePending behavior
 *
 *  @param srcId            The source identifier for which the Enhanced FramePending behavior needs to be configured.
 *  @param enable           Enable or disable the Enhanced FramePending behavior
 *
 */
GP_API void gpHal_MacEnableEnhancedFramePending(gpHal_SourceIdentifier_t srcId, Bool enable);

/** @brief This function configures the insertion of a VS IE header in the Enh Ack frames containing probing data.
 *
 *  This function configures the insertion of a VS IE header in the Enh Ack frames containing probing data.
 *
 *  @param vsIeLen          The length of the VS IE, including IE header which needs to be added to the EnhAck frame.
 *  @param pVsIeData        A Pointer to the VS IE data, including IE header which needs to be added to the EnhAck frame.
 *
 */
GP_API void gpHal_SetEnhAckVSIE(UInt8 vsIeLen, UInt8* pVsIeData);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif  /* _GPHAL_MAC_H_ */
