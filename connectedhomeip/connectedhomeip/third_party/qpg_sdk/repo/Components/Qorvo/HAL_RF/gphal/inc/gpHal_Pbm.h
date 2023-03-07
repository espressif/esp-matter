/*
 * Copyright (c) 2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
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

#ifndef _HAL_GP_PBM_H_
#define _HAL_GP_PBM_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode_Pbm.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/** @file gpHal_Pbm.h
 *  @brief This file contains all the functions needed for PBM functionality.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief Number of PBMs */
#ifndef GPHAL_NUMBER_OF_PBMS_USED
#define GPHAL_NUMBER_OF_PBMS_USED       GPHAL_MM_PBM_NR_OF
#endif

#define GPHAL_PBM_MAX_SIZE              GPHAL_MM_PBM_MAX_SIZE

#define GP_PBM_INVALID_HANDLE           0xFF

#define GPHAL_FRAMEPENDING_NOT_SET      0x00
#define GPHAL_FRAMEPENDING_SET          0x01
#define GPHAL_FRAMEPENDING_UNKNOWN      0xFF

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** range check for offsets into a pbm frame */
#define GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset) GP_HAL_PBM_OFFSET_VALID(offset)

/** range check for offsets into a pbm frame */
//
/** @brief Macro for calculation of PBM buffer address.
 *
 *  This macro calculates the base address of the PBM buffer defined by the parameter address.
 *  This macro should be called before accessing any byte of the PBM buffer.
 *
 *  @param  PBMentry The index of the PBM buffer to tranmsit
 *  @return The base address of the PBM buffer.
 */
#define GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS(entry) (GP_HAL_PBM_ENTRY2ADDR(entry)+ GPHAL_REGISTER_PBM_FORMAT_T_FRAME_0)

/** @brief Macro for writing data to a PBM buffer.
 *
 *  This macro writes data to a PBM buffer defined by the parameter address.
 *  This macro should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer to the data that will be written.
 *  @param length  The number of bytes that will be written.
 *  @param offset  The offset from the base address of the PBM where the data should be written to.
 */
#define GP_HAL_WRITE_DATA_IN_PBM(address,pData,length,offset) do {      \
        UInt16 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l) );                \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l + (length) - 1) ); \
        GP_HAL_WRITE_BYTE_STREAM(( (address) + offset_l ), (pData), (length)); \
    } while (false)

/** @brief Macro for writing one byte to a PBM buffer.
 *
 *  This macro writes one byte to a PBM buffer defined by the parameter address.
 *  This macro should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param byte    The value that will be written.
 *  @param offset  The offset from the base address of the PBM where the byte should be written to.
 */
#define GP_HAL_WRITE_BYTE_IN_PBM(address,byte,offset) do {                    \
        UInt16 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l));                \
        GP_HAL_WRITE_REG((address) + offset_l, byte); \
    } while (false)

/** @brief Macro for reading data from a PBM buffer.
 *
 *  This macro reads data from a PBM buffer defined by the parameter address.
 *  This macro should be used in order to read back the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be read from.  This value should be otained
 *                 from an interrupt callback function
 *  @param pData   The pointer where the read data will be written to.
 *  @param length  The number of bytes that will be read.
 *  @param offset  The offset from the base address of the PBM where the data is read from.
 */
#define GP_HAL_READ_DATA_IN_PBM(address,pData,length,offset)  do {             \
        UInt16 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l) );                \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l + (length) - 1) ); \
        GP_HAL_READ_BYTE_STREAM(( (address) + offset_l ) , (pData) , (length) ); \
    } while (false)

/** @brief Macro for reading one byte from a PBM buffer.
 *
 *  This macro reads data from a PBM buffer defined by the parameter address.
 *  This macro should be used in order to read back one byte of the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the byte should be read from.  This value should be obtained
 *                 from an interrupt callback function
 *  @param offset  The offset from the base address of the PBM where the byte is read from.
 *  @return        The read value.
 */
#define GP_HAL_READ_BYTE_IN_PBM(address,offset)  GP_HAL_READ_REG((address)+ (offset))


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_Pbm.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/** @brief Returns a free handle.
 *
 *  This function allocate a free PBM buffer and returns its handle.
 *  Returns 0xFF if no free buffer is available;
*/
GP_API UInt8 gpHal_GetHandle(UInt16 size);

/** @brief Releases the given handle
 *
 *  This function releases the PBM buffer associated with the handle.
 *  @param handle  A valid handle which will be released.
*/
GP_API void gpHal_FreeHandle(UInt8 handle);

/** @brief Query the calibrated RSSI value from the specified pbm entry.
 *
 *  This function returns the calibrated RSSI from the specified pbm entry.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API Int8 gpHal_GetRSSI(UInt8 PBMhandle);

/** @brief Query the calibrated LQI value from the specified pbm entry.
 *
 *  This function returns the calibrated LQI from the specified pbm entry.
 *  LQI is a value from 0 to 0xFF with 0 the lowest value and 0xFF the highest value.
 *  LQI is only based on signal strength not on correlation.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt8 gpHal_GetLQI(UInt8 PBMhandle);

/** @brief Query the timestamp of a received packet from the specified pbm entry.
 *
 * This function returns the timestamp of a received packet for a specified pbm entry.
 * The timestamp is taken at the beginning of the frame (preamble).
 * @param PBMentry  the pbm entry containing the requested data.
 * @param timeStamp the pointer to which the timestamp will be returned
*/
GP_API void gpHal_GetRxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp);

/** @brief Get the DF (AOA/AOD) Samples buffer associated with this PBM
 *
 *  This function returns the DF samples from the specified pbm entry.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt16* gpHal_GetPhaseSamples(UInt8 PBMhandle);

/** @brief Get the Rx Channel on which the packet was received
 *
 *  This function returns the Rx Channel from the specified pbm entry.
 *  @param  PBMentry  the pbm entry containing the requested data.
 *  @return rxChannel Channel on which the packet was received.
*/
GP_API UInt8 gpHal_GetRxedChannel(UInt8 PBMentry);

/** @brief Get the framecontrol for the Tx Ack after Rx Indication
 *
 *  This function returns the value of the framecontrol field on a Tx Ack.
 *  @param  PBMentry  the pbm entry containing the requested data.
 *  @return framecontrol The framecontrol field used in the Ack frame.
*/
GP_API UInt16 gpHal_GetFrameControlFromTxAckAfterRx(UInt8 PBMentry);

/** @brief Get the frequency offset (in Hz) with which the packet was received - used for the DF (AOA/AOD)feature
 *
 *  This function returns the frequency offset from the specified pbm entry.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API Int32 gpHal_GetRxedFreqOffset(UInt8 PBMentry);

/** @brief Get the (internal) antenna with which the packet was received - used for the DF (AOA/AOD)feature
 *
 *  This function returns the Rx antenna (internal antenna id) from the specified pbm entry.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt16 gpHal_GetRxedAntenna(UInt8 PBMentry);

/** @brief Query the timestamp of a transmitted packet from the specified pbm entry.
 *
 * This function returns the timestamp of a transmitted packet for a specified pbm entry.
 * The timestamp is taken at the beginning of the frame (preamble).
 * @param PBMentry  the pbm entry containing the requested data.
 * @param timeStamp the pointer to which the timestamp will be returned
*/
GP_API void gpHal_GetTxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp);

/** @brief Query the LQI value of the ACK packet related to the specified TX pbm entry.
 *
 * This function returns the LQI of the received ACK packet, the passed pbm entry needs to be for a TX packet.
 * @param PBMentry the TX pbm entry containing the requested data.
 * @note if transmission is not in ACKED MODE or the ACK is not received, 0 is returned
*/
GP_API UInt8 gpHal_GetTxAckLQI(UInt8 PBMentry);

/** @brief Query the CCA counter of a transmitted packet from the specified pbm entry.
 *
 * This function returns the number of CCA backoffs that were done for transmitting the packet.
 * If csma-cca is disabled, 0 is returned.
 * @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt8 gpHal_GetTxCCACntr(UInt8 PBMentry);

/** @brief Query the retry counter of a transmitted packet from the specified pbm entry.
 *
 * This function returns the retry counter of a transmitted packet for a specified pbm entry.
 * @param PBMentry  the pbm entry containing the requested data.
 * @note retry counter 0 means - first time succesfully transmitted
*/
GP_API UInt8 gpHal_GetTxRetryCntr(UInt8 PBMhandle);

/** @brief Query the framepending bit from the ACK of a transmitted packet from the specified pbm entry.
 *
 * This function returns the framepending bit from the ACK of a transmitted packet for a specified pbm entry.
 * @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt8 gpHal_GetFramePendingFromTxPbm(UInt8 PBMentry);

/** @brief  Returns a bool indicating the reception of an enhanced ack after a transmission of a packet.
 *
 *  @param  PBMentry     The pbm entry associated with the transmitted packet.
 *  @return enhancedAck  True if the an enhanced ack was received.
*/
GP_API Bool gpHal_GetRxEnhancedAckFromTxPbm(UInt8 PBMentry);

/** @brief Get the framecounter for the Tx Ack after Rx Indication
 *
 *  This function returns the value of the framecounter on a Tx Ack.
 *  @param  PBMentry     The pbm entry containing the requested data.
 *  @return frameCounter The framecounter field used in the Ack frame.
*/
GP_API UInt32 gpHal_GetFrameCounterFromTxAckAfterRx(UInt8 PBMentry);

/** @brief Get the keyid field for the Tx Ack after Rx Indication
 *
 *  This function returns the value of the keyid field on a Tx Ack.
 *  @param  PBMentry  The pbm entry containing the requested data.
 *  @return keyId     The keyid field used in the Ack frame.
*/
GP_API UInt8 gpHal_GetKeyIdFromTxAckAfterRx(UInt8 PBMentry);


/** @brief Calculate the RSSI from the protoRSSI returned by the data indication handler.
 *
 *  This function calculates the RSSI from the protoRSSI value returned by the data indication handler.
 *  @param protoRSSI Value returned by data indication handler.
*/
GP_API Int8 gpHal_CalculateRSSI(UInt8 protoRSSI);

/** @brief Calculate the proto RSSI from the RSSI.
 *
 *  This function calculates the proto RSSI from the RSSI value.
 *  @param protoRSSI Value returned by data indication handler.
*/
GP_API UInt8 gpHal_CalculateProtoRSSI(Int8 protoRSSI);

/** @brief Calculate the LQI of a received packet based on RSSI.
 *
 *  This function calculates the LQI of a received packet based on the RSSI. The lowest value is 0 which is at -93dBm. The highest value is 0xFF witch is at -20dBm.
 *  @param RSSI Value returned by data indication handler.
*/
GP_API UInt8 gpHal_CalculateLQIfromRSSI(Int8 rssi);

/** @brief Returns the sensitivity level of the receiver.
 *
 *  This function returns the sensitivity level of the receiver in dBm.
 *  @return        The sensitivity level.
*/
GP_API Int8 gpHal_GetSensitivityLevel(void);

/** @brief function for writing data to a PBM buffer on a cyclic way.
 *
 *  This function writes data to a PBM buffer defined by the parameter address. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer to the data that will be written.
 *  @param length  The number of bytes that will be written.
 *  @param offset  The offset from the base address of the PBM where the data should be written to.
 */
GP_API void gpHal_WriteDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Macro for reading data from a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer where the read data will be written to.
 *  @param length  The number of bytes that will be read.
 *  @param offset  The offset from the base address of the PBM where the data is read from.
 */
GP_API void gpHal_ReadDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Functino for writing one byte to a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function writes one byte to a PBM buffer defined by the parameter address.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param byte    The value that will be written.
 *  @param offset  The offset from the base address of the PBM where the byte should be written to.
 */
GP_API void gpHal_WriteByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8 byte);

/** @brief Function for reading one byte from a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back one byte of the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the byte should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param offset  The offset from the base address of the PBM where the byte is read from.
 *  @return        The read value.
 */
GP_API UInt8 gpHal_ReadByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset );


/** @brief function for writing data to a PBM buffer.
 *
 *  This function writes data to a PBM buffer defined by the parameter address. The destination is the data segment of the PBM.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer to the data that will be written.
 *  @param length  The number of bytes that will be written.
 *  @param offset  The offset from the base address of the PBM where the data should be written to.
 */
GP_API void gpHal_WriteDataInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Macro for reading data from a PBM buffer. The destination is the data segment of the PBM.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer where the read data will be written to.
 *  @param length  The number of bytes that will be read.
 *  @param offset  The offset from the base address of the PBM where the data is read from.
 */
GP_API void gpHal_ReadDataInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Functino for writing one byte to a PBM buffer. The destination is the data segment of the PBM.
 *
 *  This function writes one byte to a PBM buffer defined by the parameter address.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param byte    The value that will be written.
 *  @param offset  The offset from the base address of the PBM where the byte should be written to.
 */
GP_API void gpHal_WriteByteInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset, UInt8 byte);

/** @brief Function for reading one byte from a PBM buffer. The destination is the data segment of the PBM.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back one byte of the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the byte should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param offset  The offset from the base address of the PBM where the byte is read from.
 *  @return        The read value.
 */
GP_API UInt8 gpHal_ReadByteInPBM(gpHal_Address_t pbmAddr,UInt16 pbmOffset );

/** @brief Function for duplicating a PBM in a new PBM
 *
 *  @param PBMentryOrig The pbm.which will be copied
 *  @param PBMentryDst  The destination pbm. This pbm will have the same data and options as the original pbm.
 */
void gpHal_MakeBareCopyPBM( UInt8 PBMentryOrig , UInt8 PBMentryDst );

#ifdef GP_COMP_GPHAL_BLE

/** @brief Function for retrieving the event counter when a pbm was received
 *
 *  @param pbmHandle the pbm entry where the CTE needs to be added to
 *  @param pEventCount A pointer to the event counter
 *  @return success when the event counter could be read, a failure otherwise.
 */
UInt8 gpHal_PbmGetEventCounter(UInt8 pbmHandle, UInt16* pEventCount);

/** @brief Function for adding a constant tone extension (CTE) to the PBM
 *
 *  @param pbmHandle the pbm entry where the CTE needs to be added to
 *  @param cteLengthUs The total length of the CTE that needs to be added
 */
void gpHal_PbmSetCteLengthUs(UInt8 pbmHandle, UInt8 cteLengthUs);

/** @brief Function getting BLE RX mode the frame was received
 *
 *  @param pbmHandle
 */
UInt8 gpHal_PbmGetBleRxPhy(UInt8 pbmHandle);
#endif //GP_COMP_GPHAL_BLE

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_HAL_GP_PBM_H_
