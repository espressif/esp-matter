/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

/** @file "gpHci.h"
 *
 *  Host Controller Interface
 *
 *  Declarations of the public functions and enumerations of gpHci.
*/

#ifndef _GPHCI_H_
#define _GPHCI_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_types.h"

/* <CodeGenerator Placeholder> AdditionalIncludes */
#include "gpHci_Includes.h"
/* </CodeGenerator Placeholder> AdditionalIncludes */

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpHci_PacketType_t */
//@{
#define gpHci_CommandPacket                                    0x01
#define gpHci_DataPacket                                       0x02
#define gpHci_SyncDataPacket                                   0x03
#define gpHci_EventPacket                                      0x04
#define gpHci_IsoDataPacket                                    0x05
typedef UInt8                             gpHci_PacketType_t;
//@}

/** @enum gpHci_VendorResult_t */
//@{
/** @brief Vendor specific Error Codes */
#define gpHci_OutOfSync                                        0x01
#define gpHci_NoBuffers                                        0x02
#define gpHci_BufferOverflow                                   0x03
typedef UInt8                             gpHci_VendorResult_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_HCI_NR_OF_PHYS */
#define GP_HCI_NR_OF_PHYS                            3
/* <CodeGenerator Placeholder> AdditionalMacroDefinitions */
/** @macro GP_HCI_TO_HOST_MAX_PACKET_SIZE */
/** @brief TODO: max size should be calculated from indications by generate script! */
#define GP_HCI_TO_HOST_MAX_PACKET_SIZE               (4+GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX)
/* </CodeGenerator Placeholder> AdditionalMacroDefinitions */
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @struct gpHci_CommandPacket_t */
typedef struct {
    /** @brief OCF 0-9 OGF 10-15 */
    gpHci_CommandOpCode_t          OpCode;
    UInt8                          totalLength;
    UInt8*                         pData;
} gpHci_CommandPacket_t;

/** @struct gpHci_DataPacket_t */
typedef struct {
    /** @brief PacketBoundary  12-13  Broadcast 14-15 */
    UInt16                         handle;
    UInt16                         totalLength;
    UInt8*                         pData;
} gpHci_DataPacket_t;

/** @struct gpHci_SyncDataPacket_t */
typedef struct {
    /** @brief PacketStatus  12-13  Reserved 14-15 */
    UInt16                         handle;
    UInt8                          totalLength;
    UInt8*                         pData;
} gpHci_SyncDataPacket_t;

/** @struct gpHci_IsoDataPacket_t */
typedef struct {
    /** @brief ConnHandle 0-11  PacketBoundary_Flag 12-13  TimeStamp_Flag 14  Reserved 15 */
    UInt16                         handle;
    /** @brief Data Load Length  0-13  Reserved 14-15 */
    UInt16                         isoDataLoadLength;
    UInt8*                         pisoDataLoad;
} gpHci_IsoDataPacket_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
void gpHci_Init(void);

void gpHci_CompReset(void);

void gpHci_processCommand(UInt16 opCode, UInt8 totalLength, UInt8* pData);

void gpHci_processData(UInt16 handle, UInt16 totalLength, UInt8* pData);


Bool gpHci_commandsEnabled(void);

void gpHci_stopCommands(void);

void gpHci_resumeCommands(void);

Bool gpHci_StoreCrossOverCommand(gpHci_CommandOpCode_t opCode, UInt8 totalLength, UInt8* pData);

Bool gpHci_ExecuteCrossOverCommand(void);

//Indications
Bool gpHci_ConnectionCompleteEvent(UInt8 eventCode, gpHci_ConnectionCompleteParams_t* connectionComplete);

Bool gpHci_DisconnectionCompleteEvent(UInt8 eventCode, gpHci_DisconnectCompleteParams_t* disconnectComplete);

Bool gpHci_EncryptionChangeEvent(UInt8 eventCode, gpHci_EncryptionChangeParams_t* encryptionChange);

Bool gpHci_ReadRemoteVersionCompleteEvent(UInt8 eventCode, gpHci_ReadRemoteVersionInfoComplete_t* versionInfo);

Bool gpHci_CommandCompleteEvent(UInt8 eventCode, gpHci_CommandCompleteParams_t* commandCompleteParams);

#if defined(GP_HCI_DIVERSITY_HOST_SERVER)
Bool WcBleHost_gpHci_CommandCompleteHandler(gpHci_EventCode_t eventCode, gpHci_CommandCompleteParams_t* commandCompleteParams);
#endif //defined(GP_HCI_DIVERSITY_HOST_SERVER)

Bool gpHci_CommandStatusEvent(gpHci_EventCode_t eventCode, gpHci_CommandStatusParams_t* commandStatusParams);

Bool gpHci_HardwareErrorEvent(UInt8 eventCode, UInt8 hwcode);

Bool gpHci_NumberOfCompletedPacketsEvent(UInt8 eventCode, gpHci_NumberOfCompletedPackets_t* numberOfCompletedPackets);

Bool gpHci_DataBufferOverflowEvent(UInt8 eventCode, gpHci_LinkType_t linktype);

Bool gpHci_EncryptionKeyRefreshCompleteEvent(UInt8 eventCode, gpHci_EncryptionKeyRefreshComplete_t* keyRefresh);


Bool gpHci_AuthenticationPayloadTOEvent(UInt8 eventCode, gpHci_AuthenticatedPayloadToExpired_t* authenticatedPayloadToExpired);

Bool gpHci_SendHciDataFrameToHost(UInt16 connAndBoundary, UInt16 dataLength, UInt8* pData);

Bool gpHci_VsdSinkRxIndication(UInt8 eventCode, gpHci_VsdSinkRxIndication_t* vsdSinkRxIndication);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPHCI_H_

