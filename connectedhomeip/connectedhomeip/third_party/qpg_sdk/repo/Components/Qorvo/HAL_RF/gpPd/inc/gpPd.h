/*
 * Copyright (c) 2011-2016, GreenPeak Technologies
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

/** @file "gpPd.h"
 *
 *  Packet Descriptor Implementation
 *
 *  Declarations of the public functions of gpPd.
 *
 *  The Packet Descriptor is a handle to packet memory for RF transmission and reception.
 *  It holds, next to the data, additional attributes around the received packet.
 *  The handle is used across RF API's in the Qorvo stack.
 *
 *  For a TX flow, following steps need to be followed:
 *  - Get a Pd with gpPd_GetPd() (Support for 802.15.4) or gpPd_GetCustomPd() (Support for 802.15.4 and BLE packets)
 *  - Fill in data to be sent with the gpPd_Write*() functions.
 *  - Use the handled in the relevant DataRequest API.
 *  - Free the handle in the DataConfirm of the RF TX request
 *
 *  For a RX flow:
 *  - The stack will have allocated a Pd handle with all data and attributes filled in.
 *  - It will be supplied through indication APIs to upper layers
 *  - That layer can read out data with gpPd_Read*() functions and fetch extra data through gpPd_Get*()
 *  - After using the Pd, the higher layer needs to free the Pd handle
*/

#ifndef _GPPD_H_
#define _GPPD_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpPd_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#ifdef GP_COMP_GPHAL
// inclusion for number of pbms
#include "gpHal_Pbm.h"
#endif //GP_COMP_GPHAL

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#if defined (GP_DIVERSITY_PD_USE_PBM_VARIANT)
#ifndef GP_PD_NR_OF_HANDLES
#define GP_PD_NR_OF_HANDLES         GPHAL_NUMBER_OF_PBMS_USED
#endif
#endif //(GP_DIVERSITY_PD_USE_PBM_VARIANT)

/** @brief Handle value indicating an invalid handle.
  * Note!
  * Use gpPd_CheckPdValid() instead of a direct comparison
  * to catch out of range or not in use handles
*/
#define GP_PD_INVALID_HANDLE           0xFF

/** @brief Buffer size for 802.15.4 packets */
#define GP_PD_BUFFER_SIZE_ZIGBEE            128
/** @brief Buffer size for BLE packets */
#define GP_PD_BUFFER_SIZE_BLE               256

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @brief Reference handle to a Packet Descriptor. To be used across RF APIs. */
typedef UInt8 gpPd_Handle_t;
/** @brief Length of data in a Packet Descriptor. */
typedef UInt16 gpPd_Length_t;
/** @brief Offset in the data buffer of a Packet Descriptor. */
typedef UInt16 gpPd_Offset_t;
/** @brief Time in us of reception or transmission of the packet. */
typedef UInt32 gpPd_TimeStamp_t;
/** @brief RSSI (in dBm) of the received packet. */
typedef Int8 gpPd_Rssi_t;
/** @brief Link Quality indication of the received packet. */
typedef UInt8 gpPd_Lqi_t;

/** @name gpPd_Result_t */
//@{
/** @brief Given handle is valid. Only allocated handles will be flagged valid. */
#define gpPd_ResultValidHandle        0x0
/** @brief Given handle is not valid. Can be out of range. */
#define gpPd_ResultInvalidHandle      0x1
/** @brief Given handle is valid but not in use. Handle can be wrong or no longe in use. */
#define gpPd_ResultNotInUse           0x2
/** @typedef gpPd_Result_t
 *  @brief The gpPd_Result_t type defines the result of handle check for validity.
*/
typedef UInt8 gpPd_Result_t;
//@}

/** @name gpPd_Result_t */
//@{
/** @brief Use a packet buffer with a size suitable for 802.15.4/ZigBee. */
#define gpPd_BufferTypeZigBee       0x00
/** @brief Use a packet buffer with a size suitable for BLE. */
#define gpPd_BufferTypeBle          0x01
/** @brief Invalid buffer type - used as check. */
#define gpPd_BufferTypeInvalid                                       0x02
/** @typedef gpPd_BufferType_t
 *  @brief The gpPd_BufferType_t type defines the type of Packet Descriptor the use.
*/
typedef UInt8 gpPd_BufferType_t;
//@}

/** @struct gpPd_Loh_t
 *  @brief Structure grouping information about a part of data to be read out or written.
*/
typedef struct gpPd_Loh_s {
    gpPd_Length_t   length;
    gpPd_Offset_t   offset;
    gpPd_Handle_t   handle;
} gpPd_Loh_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpPd_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/** @brief Initialize gpPd component. This will init all internal data structures */
void gpPd_Init(void);

/** @brief De-initializes gpPd component. */
void gpPd_DeInit(void);

//Descriptor handling
/** @brief  Return a handle to use for packet construction and transmission
 *          This function will return a gpPd_BufferTypeZigBee.
 *          See gpPd_GetCustomPd() for other buffer types (BLE).
 *
 *  @return pdHandle    Handle as reference for use with the packet memory througout other APIs.
 *                      GP_PD_INVALID_HANDLE will be returned if no handles are available.
*/
gpPd_Handle_t gpPd_GetPd(void);

/** @brief  Return a handle to use for a specific packet buffer type.
 *
 *  @param  type        Buffer type requested. Possible types listed in gpPd_BufferType_t.
 *  @param  size        Buffer size needed for the requested packet. Size must align with the request type (ZB/BLE/BLE XL).
 *
 *  @return pdHandle    Handle as reference for use with the packet memory througout other APIs.
 *                      Will return GP_PD_INVALID_HANDLE in the following cases:
 *                      - no handles are available.
 *                      - no buffers with the requested size are available
 *                      - requested size is not valid for the selected type.
*/
gpPd_Handle_t gpPd_GetCustomPd(gpPd_BufferType_t type, UInt16 size);

/** @brief Release a Pd handle and it's associated memory after use.
 *
 *  Pd handle can be freed, allowing it and his associated memory to be re-used by the application (Tx) or HW (Rx).
 *  It is expected the handles are not kept for long durations, not to block the ability to send or receive new packets.
 *  The recommentation is the copy all needed information in a short lifespan.
 *  Note that accessing the handle after freeing is an illegal operation.
 *
 *  @param pdHandle  Handle referring to Tx or Rx buffer used.
*/
void gpPd_FreePd(gpPd_Handle_t pdHandle);

/** @brief Copy all contents of a Packet Descriptor to a new one.
 *
 *  This function can be used when distributing a packet over multiple upper layers.
 *
 *  @param pdHandle Handle to copy all information from
 *
 *  @return newHandle New handle to copied content.
 *                    Will return GP_PD_INVALID_HANDLE in the following case:
 *                      - no handles are available with the same type
*/
gpPd_Handle_t gpPd_CopyPd(gpPd_Handle_t pdHandle);

/** @brief Lookup pd handle by PBM.
 *
 *
 *  @param pdHandle  Handle referring to Tx or Rx buffer used.
*/
gpPd_Handle_t gpPd_GetPdFromPBM(UInt8 pbmHandle);

/** @brief Check if the Pd is valid.
 *  To be used to check a fetched Pd.
 *  This function is advised to be used opposed to comparing against GP_PD_INVALID_HANDLE as more checks are performed.
 *
 *  @param pdHandle  Handle fetched or received.
 *  @return result   Can return the following:
                     - gpPd_ResultValidHandle if it's in range of possible handle values.
                     - gpPd_ResultInvalidHandle if the handle is outside the possible range.
                     - gpPd_ResultNotInUse if the handle is not considered claimed (fi when feeding an unfetched but valid handle value).
*/
gpPd_Result_t gpPd_CheckPdValid(gpPd_Handle_t pdHandle);

/** @brief Check if the Pd is valid.
 *
 *  @param  pdHandle Handle referring to buffer.
 *  @return type     Buffer type of gpPd_BufferType_t pertaining to the given handle.
*/
gpPd_BufferType_t gpPd_GetPdType(gpPd_Handle_t pdHandle);

//Data handling
/** @brief  Read one byte from a Pd buffer.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @param  offset      Offset to location in buffer to read from.
 *  @return byte        Byte read out @ offset.
*/
UInt8 gpPd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset);

/** @brief  Write one byte to a Pd buffer.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @param  offset      Offset to location in buffer to write to.
 *  @param  byte        Byte to write @ offset.
*/
void gpPd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte);

/** @brief  Read a bytestream from a Pd buffer.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @param  offset      Offset to location in buffer to start read from.
 *  @param  length      Length to read.
 *  @return pData       Pointer to buffer for bytes read out @ offset.
*/
void gpPd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);

/** @brief  Write a bytestream to a Pd buffer.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @param  offset      Offset to location in buffer to start write operation.
 *  @param  length      Length to write.
 *  @return pData       Pointer to buffer of bytes written @ offset.
*/
void gpPd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);

//Data handling with update of pdLoh
/** @brief  Append a bytestream to a Pd buffer and update the pdLoh structure.
 *
 *  @param  pPdLoh    Pointer to Length,Offset,Handle associated with the packet buffer.
 *  @param  length    Length to write.
 *  @param  pData     Pointer to buffer of bytes written @ Offset (in pdLoh structure).
*/
void gpPd_AppendWithUpdate(gpPd_Loh_t* pPdLoh, UInt8 length, UInt8 const* pData);

/** @brief  Prepend a bytestream to a Pd buffer and update the pdLoh structure.
 *          Typically used to add header information in front of payload bytes of higher layers.
 *
 *  @param  pPdLoh    Pointer to Length,Offset,Handle associated with the packet buffer.
 *  @param  length    Length to write.
 *  @param  pData     Pointer to buffer of bytes written @ Offset (in pdLoh structure).
*/
void gpPd_PrependWithUpdate(gpPd_Loh_t* pPdLoh, UInt8 length, UInt8 const* pData);

/** @brief  Read a bytestream from a Pd buffer and update the pdLoh structure.
 *          Typically used to scroll through bytes in a Pd.
 *
 *  @param  pPdLoh    Pointer to Length,Offset,Handle associated with the packet buffer.
 *  @param  length    Length to read.
 *  @param  pData     Pointer to buffer for bytes read out @ Offset (in pdLoh structure).
*/
void gpPd_ReadWithUpdate(gpPd_Loh_t* pPdLoh, UInt8 length, UInt8* pData);

//Properties handling
//Rx
/** @brief  Return the Relative Signal Strength Indication (RSSI) of the received packet.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return rssi        RSSI value in dBm.
*/
gpPd_Rssi_t gpPd_GetRssi(gpPd_Handle_t pdHandle);

/** @brief  Return the Link Quality Indication (LQI) of the received packet.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return lqi         LQI value (as specified in 802.15.4 PHY specification).
*/
gpPd_Lqi_t gpPd_GetLqi(gpPd_Handle_t pdHandle);

/** @brief  Return the timestamp of the received packet.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return timestamp   Timestamp in us, taken from the timebase of the OS (hal timing).
*/
gpPd_TimeStamp_t gpPd_GetRxTimestamp(gpPd_Handle_t pdHandle);

/** @brief  Return the timestamp of the received packet taken from the chip HW timer.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return timestamp   Timestamp in us, taken from the timer of the chip HW (gpHal timing).
 *                      The timestamp will be equivalent to gpPd_GetRxTimestamp() when running fully embedded (no transceiver setup).
*/
gpPd_TimeStamp_t gpPd_GetRxTimestampChip(gpPd_Handle_t pdHandle);

/** @brief  Return the channel on which the packet was received.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return rxChannel   Channel on which the packet was received.
 *
*/
UInt8 gpPd_GetRxChannel(gpPd_Handle_t pdHandle);

/** @brief  Return the buffer of BLE phase samples of the received packet.
 *
 *  @param  pdHandle    Handle associated with the packet buffer.
 *  @return pSamples    Pointer to the received phase samples.
 *                      NULL if not available and valid untill free of the pdHandle.
*/
UInt16* gpPd_GetPhaseSamplesBuffer(gpPd_Handle_t pdHandle);

//TxConfirm
/** @brief  Return the timestamp of a transmitted packet.
 *
 *  @param  pdHandle    Handle associated with the packet.
 *  @return timestamp   Timestamp in us at which the packet was sent, taken from the timebase of the OS (hal timing).
*/
gpPd_TimeStamp_t gpPd_GetTxTimestamp(gpPd_Handle_t pdHandle);

/** @brief  Return the timestamp of a transmitted packet.
 *
 *  @param  pdHandle    Handle associated with the packet.
 *  @return channel     The last channel at which the packet was sent
*/
UInt8 gpPd_GetTxChannel(gpPd_Handle_t pdHandle);

/** @brief  Return the Link Quality Indication (LQI) of the ACK for the transmitted packet.
 *
 *  @param  pdHandle    Handle associated with the packet.
 *  @return lqi         LQI value of the received ACK - only valid when there was an ACK present.
*/
gpPd_Lqi_t gpPd_GetTxAckLqi(gpPd_Handle_t pdHandle);

/** @brief  Return the amount of CCA retries of the transmitted packet
 *
 *  @param  pdHandle    Handle associated with the packet.
 *  @return retries     Amount of CCA retries performed.
*/
UInt8 gpPd_GetTxCCACntr(gpPd_Handle_t pdHandle);

/** @brief  Return the amount of MAC retries of the transmitted packet
 *
 *  @param  pdHandle    Handle associated with the packet.
 *  @return retries     Amount of MAC retries. 0 if the first attempt succeeded.
*/
UInt8 gpPd_GetTxRetryCntr(gpPd_Handle_t pdHandle);

/** @brief  Return the framepending bit from the ACK of a transmitted packet.
 *
 *  @param  pdHandle     Handle associated with the packet.
 *  @return framePending True if the frame pending bit was set in the associated ACK.
*/
UInt8 gpPd_GetFramePendingAfterTx(gpPd_Handle_t pdHandle);

/** @brief  Returns a bool indicating the reception of an enhanced ack after a transmission of a packet.
 *
 *  @param  pdHandle     Handle associated with the transmitted packet.
 *  @return enhancedAck  True if the an enhanced ack was received.
*/
Bool gpPd_GetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle);

/** @brief  Return the framecontrol field from the ACK transmitted after receiving a frame.
 *
 *  @param  pdHandle     Handle associated with the rx packet.
 *  @return frameControl The framecontrol field as it was was set in the tx ACK.
*/
UInt16 gpPd_GetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle);

/** @brief Get the framecounter for the ACK transmitted after receiving a frame.
 *
 *  This function returns the value of the framecounter on a Tx Ack.
 *  @param  pdHandle     Handle associated with the rx packet.
 *  @return frameCounter The framecounter field used in the Ack frame.
*/
UInt32 gpPd_GetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle);

/** @brief Get the keyid field for the ACK transmitted after receiving a frame.
 *
 *  This function returns the value of the keyid field on a Tx Ack.
 *  @param  pdHandle     Handle associated with the rx packet.
 *  @return keyId        The keyid field used in the Ack frame.
*/
UInt8 gpPd_GetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle);

//Internal Helper functions

/******************************
 * Rx Flow fill-in
 ******************************/
/** @brief Function to fill in RSSI value during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle     Handle associated with the rx packet.
 *  @param  rssi         RSSI value (in dBm)
*/
void gpPd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi);

/** @brief Function to fill in LQI value during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  lqi      LQI value.
*/
void gpPd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi);

/** @brief Function to fill in timestamp value during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  timestamp Time of reception (in us) of the associated packet.
*/
void gpPd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp);

/** @brief Function to fill in channel on which packet was received during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  rxChannel Channel on which packet was received.
 *          802.15.4 or BLE channel range will be filled in according to the type of Pd.
*/
void gpPd_SetRxChannel(gpPd_Handle_t pdHandle, UInt8 rxChannel);

/** @brief Function to fill in FrameControl of Ack sent during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  frameControl 802.15.4 frame control field of the ACK sent out in response of packet.
*/
void gpPd_SetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt16 frameControl);

/** @brief Function to fill in FrameCounter of Ack sent during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  frameCounter 802.15.4 security frame counter field of the ACK sent out in response of packet.
*/
void gpPd_SetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt32 frameCounter);

/** @brief Function to fill in KeyId for Ack sent during processing of an incoming packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle Handle associated with the rx packet.
 *  @param  keyId 802.15.4 security key Id used for the ACK sent out in response of packet.
*/
void gpPd_SetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt8 keyId);

/******************************
 * Tx Flow fill-in
 ******************************/
/** @brief Function to fill in timestamp during processing of an confirm of a sent packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle     Handle associated with the Tx packet.
 *  @param  timestamp    Time of Tx (in us) of the associated packet.
*/
void gpPd_SetTxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp);

/** @brief Function to fill in the retry count during processing of an confirm of a sent packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle     Handle associated with the Tx packet.
 *  @param  txRetryCntr  Retry count of the transmit.
*/
void gpPd_SetTxRetryCntr(gpPd_Handle_t pdHandle, UInt8 txRetryCntr);

/** @brief Function to fill in the frame pending bit of the ack during processing of an confirm of a sent packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle     Handle associated with the Tx packet.
 *  @param  framePending set to @True if the frame pending bit was set in the associated
 *          ACK.Time of Tx (in us) of the associated packet.
*/
void gpPd_SetFramePendingAfterTx(gpPd_Handle_t pdHandle, UInt8 framePending);

/** @brief Function to fill in the ack type during processing of an confirm of a sent packet.
 *
 *  Only to be used by stack when processing incoming packet.
 *  @param  pdHandle     Handle associated with the Tx packet.
 *  @param  enhancedAck  set to @True if the transmitted frame was acked with an enhanced ack
*/
void gpPd_SetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle, Bool enhancedAck);

/******************************
 * Internal stack calls
 ******************************/
//Data
/** @brief Function to prepare a Packet Descriptor for transmission.
 *
 *  Only to be used by stack when processing a packet transmission.
 *  @param  p_PdLoh Pointer to relevant data.
 *  @return pdHandle Allocated handle for data to be sent.
*/
UInt8 gpPd_DataRequest(gpPd_Loh_t* p_PdLoh);

/** @brief Function to prepare a Packet Descriptor after a Tx
 *
 *  Only to be used by stack when processing a packet Tx confirm.
 *  Will translate HW buffers into a Packet Descriptor
 *
 *  @param pbmHandle HW buffer handle
 *  @param pbmOffset Offset of the data in the buffer
 *  @param pbmLength Length of data
 *  @param[out] p_PdLoh Pointer to output processed Packet Descriptor
*/
void gpPd_cbDataConfirm(UInt8 pbmHandle, UInt16 pbmOffset, UInt16 pbmLength, gpPd_Loh_t* p_PdLoh);

/** @brief Function to prepare a Packet Descriptor after a received packet
 *
 *  Only to be used by stack when processing a packet reception.
 *  Will translate HW buffers into a Packet Descriptor
 *
 *  @param pbmHandle HW buffer handle
 *  @param pbmOffset Offset of the data in the buffer
 *  @param pbmLength Length of data
 *  @param[out] p_PdLoh Pointer to output processed Packet Descriptor
 *  @param type Type of received data (802.15.4/BLE)
*/
void gpPd_DataIndication(UInt8 pbmHandle, UInt16 pbmOffset, UInt16 pbmLength, gpPd_Loh_t* p_PdLoh, gpPd_BufferType_t type);

//Security
/** @brief Function to prepare a Packet Descriptor for a Security operation
 *
 *  Only to be used by stack when processing a security operation.
 *  Will translate HW buffers into a Packet Descriptor
 *
 *  @param pdHandle Packet Descriptor handle
 *  @param dataOffset Offset of the data in the buffer
 *  @param dataLength Length of data
 *  @param auxOffset Offset of auxiliary security data. Can be part of overal data.
 *  @param auxLength Length of auxiliary security data
 *
 *  @return pbmHandle HW buffer handle
*/
UInt8 gpPd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength);

/** @brief Function to prepare a Packet Descriptor after completing a security operation.
 *
 *  Only to be used by stack when processing a security operation.
 *  Will translate HW buffers into a Packet Descriptor
 *
 *  @param pbmHandle HW buffer handle
 *  @param dataOffset Offset of the data in the buffer
 *  @param dataLength Length of data
 *
 *  @return pdHandle Packet Descriptor handle containing processed security operation
*/
gpPd_Handle_t gpPd_cbSecConfirm(UInt8 pbmHandle, UInt8 dataOffset, UInt8 dataLength);

//Purge
/** @brief Function to prepare a Packet Descriptor for a Purge operation
 *
 *  Only to be used by stack when processing a purge operation.
 *  Will translate HW buffers into a Packet Descriptor
 *
 *  @param pdHandle Packet Descriptor handle
 *
 *  @return pbmHandle HW buffer handle
*/
UInt8 gpPd_PurgeRequest(gpPd_Handle_t pdHandle);

/** @brief Function to prepare a Packet Descriptor after completing a Purge operation.
 *
 *  Only to be used by stack when processing a purge operation.
 *  Will finalize work for the operation.
 *
 *  @param pbmHandle HW buffer handle
*/
void gpPd_cbPurgeConfirm(UInt8 pbmHandle);


#if defined (GP_DIVERSITY_PD_USE_PBM_VARIANT)
#include "gpPd_pbm.h"
#else // defined (GP_DIVERSITY_PD_USE_RAM_VARIANT)
#include "gpPd_ram.h"
#endif //(GP_DIVERSITY_PD_USE_PBM_VARIANT)

/******************************
 * Serialization helper functions
 ******************************/

/** @brief Initialize mapping between incoming and internal handles.
 *
 * When a serialized component uses Pd handles, the ones used at client side
 * are different from the ones managed in the server side.
 * A mapping is kept to track the ones from the client to return correct handles
 * in any confirm.
 *
*/
void gpPd_InitPdHandleMapping(void);

/** @brief Store a pair of mapped and new handle
 *
 *  @param newPdHandle Handle taken at server side to store in mapping
 *  @param pdHandle    Handle coming from client side to store
 *
 *  @return success    Returns true if mapping was succesfull, false if no space was available.
*/
Bool gpPd_StorePdHandle(gpPd_Handle_t newPdHandle, gpPd_Handle_t pdHandle);

/** @brief Retrieve a mapped pdHandle
 *
 *  @param[out] storedPdHandle Pointer to return stored Packet Descriptor handle.
                               Will return GP_INVALID_HANDLE if not found.
 *  @param pdHandle Local handle used to look up external handle.
 *
 *  @return success    Returns true if operation was succesfull, false if invalid could not be set.
*/
Bool gpPd_GetStoredPdHandle(gpPd_Handle_t* storedPdHandle, gpPd_Handle_t pdHandle);

/** @brief Retrieve a mapped pdHandle and remove if necessary
 *
 *  @param newPdHandle Local handle used when storing the external handle.
 *  @param remove Remove the handle from the mapping if true.
 *
 *  @return pdHandle Returns GP_INVALID_HANDLE if not found.
*/
gpPd_Handle_t gpPd_RestorePdHandle(gpPd_Handle_t newPdHandle, Bool remove);

#if defined(GP_COMP_UNIT_TEST)
#include "gpPd_ut.h"
#endif //GP_COMP_UNIT_TEST

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPPD_H_
