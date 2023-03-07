/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpCom.h
 *
 * This file contains the API definition of the serial communication module.
 * It implements the GreenPeak serial protocol.
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

#ifndef _GPCOM_H_
#define _GPCOM_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpCom_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include <gpModule.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Check for multiple com usage
#ifdef GP_DIVERSITY_COM_UART
#if defined(GP_COM_DIVERSITY_SERIAL_SPI)
#ifndef GP_COM_DIVERSITY_MULTIPLE_COM
#define GP_COM_DIVERSITY_MULTIPLE_COM
#endif //ifndef GP_COM_DIVERSITY_MULTIPLE_COM
#endif
#endif

#define GP_COM_COMM_ID_HW       (0x80000000)
#define GP_COM_COMM_ID_HW_1     (0x10000)
#define GP_COM_COMM_ID_HW_2     (0x20000)
#define GP_COM_COMM_ID_HW_3     (0x40000)
#define GP_COM_COMM_ID_HW_4     (0x80000)
#define GP_COM_COMM_ID_BLE_COMMAND  GP_COM_COMM_ID_HW_1
#define GP_COM_COMM_ID_BLE_DATA     GP_COM_COMM_ID_HW_2
#define GP_COM_COMM_ID_BLE_ISODATA  GP_COM_COMM_ID_HW_3
#define GP_COM_COMM_ID_BLE_EVENT    GP_COM_COMM_ID_HW_4
#define GP_COM_COMM_ID_USB     (GP_COM_COMM_ID_HW | 0x4000)

#define GP_COM_COMM_ID_UART1_PROPER     (GP_COM_COMM_ID_HW | 0x01)
#define GP_COM_COMM_ID_UART1            GP_COM_COMM_ID_UART1_PROPER
#define GP_COM_COMM_ID_UART2    (GP_COM_COMM_ID_HW | 0x02)
#define GP_COM_COMM_ID_UART3    (GP_COM_COMM_ID_HW | 0x04)
#define GP_COM_COMM_ID_BLE      (GP_COM_COMM_ID_HW_1 | GP_COM_COMM_ID_HW_2 | GP_COM_COMM_ID_HW_3 | GP_COM_COMM_ID_HW_4)
#define GP_COM_COMM_ID_STMUSB         (GP_COM_COMM_ID_UART1)
#define GP_COM_COMM_ID_SHMEM          (0x40000000)
#define GP_COM_COMM_ID_IOCTL_USER1    (GP_COM_COMM_ID_HW | 0x0040)
#define GP_COM_COMM_ID_IOCTL_USER2    (GP_COM_COMM_ID_HW | 0x0080)
#define GP_COM_COMM_ID_IOCTL_USER3    (GP_COM_COMM_ID_HW | 0x0010)
#define GP_COM_COMM_ID_IOCTL_USER4    (GP_COM_COMM_ID_HW | 0x0020)
#define GP_COM_COMM_ID_IOCTL_USER_MASK (GP_COM_COMM_ID_IOCTL_USER1 | GP_COM_COMM_ID_IOCTL_USER2 | GP_COM_COMM_ID_IOCTL_USER3 | GP_COM_COMM_ID_IOCTL_USER4)
#define GP_COM_COMM_ID_IOCTL_KERN1    (GP_COM_COMM_ID_HW | 0x0100)
#define GP_COM_COMM_ID_IOCTL_KERN2    (GP_COM_COMM_ID_HW | 0x0200)
#ifndef GP_COM_COMM_ID_IOCTL_USER
#define GP_COM_COMM_ID_IOCTL_USER GP_COM_COMM_ID_IOCTL_USER1
#endif
#ifndef GP_COM_COMM_ID_IOCTL_KERN
#define GP_COM_COMM_ID_IOCTL_KERN GP_COM_COMM_ID_IOCTL_KERN1
#endif
#define GP_COM_COMM_ID_SOCKET   0xFFFFFFFF /* used for the server (local client) conection if it exists; else the first client (local server) connection */
#define GP_COM_COMM_ID_SOCKET2  0xFFFFFFFE /* for the next client (local server) connection */
#define GP_COM_COMM_ID_RF      (GP_COM_COMM_ID_HW | 0x1000)

#define GP_COM_COMM_ID_SPI     (GP_COM_COMM_ID_HW | 0x2000)

#define GP_COM_COMM_ID_DIGSIM     (GP_COM_COMM_ID_HW | 0x5555)

#define GP_COMM_ID_CARRIED_BY(system,id)  ((system & id) == id)

//Define default COM ID

#ifdef GP_COM_DIVERSITY_MULTIPLE_COM
#ifndef GP_COM_DEFAULT_COMMUNICATION_ID
#if defined(GP_DIVERSITY_JUMPTABLES)
#define GP_COM_DEFAULT_COMMUNICATION_ID  GP_COM_COMM_ID_UART1
#else
#error supply default comm ID
#endif // defined(GP_DIVERSITY_JUMPTABLES)
#endif // GP_COM_DEFAULT_COMMUNICATION_ID
#else
//Default case
#ifndef GP_DIVERSITY_COM_UART
#define GP_DIVERSITY_COM_UART
#endif //GP_DIVERSITY_COM_UART
#define GP_COM_DEFAULT_COMMUNICATION_ID  GP_COM_COMM_ID_UART1
#endif //GP_COM_DIVERSITY_MULTIPLE_COM


#if defined(GP_COMP_COM)
#define GP_COM_DATA_REQUEST(length,pData, commId)   gpCom_DataRequest(GP_MODULE_ID, (length), (pData), commId)
#if defined(GP_DIVERSITY_COM_UART) 
#define GP_COM_GET_FREE_BUFFER_SPACE(commId)        gpCom_GetFreeBufferSpace(GP_MODULE_ID, commId)
#endif
#define GP_COM_REGISTER_MODULE(handleCallback)      gpCom_RegisterModule(GP_MODULE_ID, (handleCallback))
#define GP_COM_REGISTER_ACTIVATE_TX_CB(activateCallback)      gpCom_RegisterActivateTxCb(GP_MODULE_ID, activateCallback)
#else
#define GP_COM_DATA_REQUEST(length,pData, commId)             do {} while (false)
#define GP_COM_REGISTER_MODULE(handleCallback)                do {} while (false)
#define GP_COM_REGISTER_ACTIVATE_TX_CB(activateCallback)      do {} while (false)
#endif

#ifndef GP_COM_MAX_PACKET_PAYLOAD_SIZE
#define GP_COM_MAX_PACKET_PAYLOAD_SIZE   127
#endif //GP_COM_MAX_PACKET_PAYLOAD_SIZE

//Length expressed in 12-bits - maximum 4096-1 bytes available
#define GP_COM_MAX_PACKET_PAYLOAD_SIZE_PROTOCOL 4095
#if GP_COM_MAX_PACKET_PAYLOAD_SIZE > GP_COM_MAX_PACKET_PAYLOAD_SIZE_PROTOCOL
#error Length maximum 12-bits = 4096-1 bytes
#endif

#define GP_COM_IOCTL_KERNEL_NR_OF_CLIENTS (4)

#ifndef GP_COM_NR_OF_ACTIVATETXCB
#define GP_COM_NR_OF_ACTIVATETXCB (2)
#endif
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt32 gpCom_CommunicationId_t;

typedef void (*gpCom_HandleCallback_t) ( UInt16 length , UInt8* pData, gpCom_CommunicationId_t commId);
typedef UInt16 (*gpCom_cbActivateTx_t) ( UInt16 overFlowCounter, gpCom_CommunicationId_t commId);

#define gpCom_ProtocolSyn        1
#define gpCom_ProtocolBle        2
#define gpCom_ProtocolNoSynNoCrc 3
#define gpCom_ProtocolInvalid   0xff
typedef UInt8 gpCom_Protocol_t;
/*****************************************************************************
 *                    External Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpCom_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpCom_Init(void);

/**
* @brief Send data over a specified communication channel
*
* @param moduleID   SW component ID that will handle the data
* @param length     Length of the data
* @param pData      Pointer to the data to send
* @param commId     Communication channel to use for transmission (if available). Can be UART/USB/...
* @return status    Returns True if request was executed.
*/
Bool gpCom_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId);

/**
* @brief Send data over a specified communication channel and wait for a specific ACK packet
*
* @param moduleID   SW component ID that will handle the data
* @param length     Length of the data
* @param pData      Pointer to the data to send
* @param commId     Communication channel to use for transmission (if available). Can be UART/USB/...
* @param reqAcked   Return pointer in whick ack state is returned.
* @param timeout    Time to wait for ack (in us)
* @param ackId      Specific command ID to wait for in ACK packet. Tied to API serialization.
* @return status    Returns True if request was executed.
*/
Bool gpCom_DataRequestAndWaitForAck(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId, Bool *reqAcked, UInt32 timeout, UInt8 ackId);
UInt16 gpCom_GetFreeBufferSpace(UInt8 moduleID, gpCom_CommunicationId_t commId);
Bool gpCom_RegisterModule(UInt8 moduleID, gpCom_HandleCallback_t handleCallback);
Bool gpCom_DeRegisterModule(UInt8 moduleID);
Bool gpCom_RegisterActivateTxCb(UInt8 moduleID, gpCom_cbActivateTx_t cb);
Bool gpCom_IsReceivedPacketPending(void);
void gpCom_HandleRx(void);
void gpCom_HandleRxBlocking(Bool blockingWait, UInt8 cmdId);

// Common functions
Bool gpCom_GetTXEnable(void);
void gpCom_HandleTx(void);
void gpCom_Flush(void);
void gpCom_DeInit(void);
Bool gpCom_TXDataPending(void);
gpCom_CommunicationId_t gpCom_cbSelectCommId(void);
gpCom_CommunicationId_t gpCom_cbSelectBleCommId(void);





#ifdef GP_DIVERSITY_LINUXKERNEL
int gpCom_InitKernel(void);
void  gpCom_DeInitKernel(void);
#endif //ifdef GP_DIVERSITY_LINUXKERNEL


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)



#endif // _GPCOM_H_

