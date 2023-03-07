/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
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

#ifndef _GPCOM_DEFS_H_
#define _GPCOM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpCom.h"
/****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/



#define GP_COM_BLE_HEADER_LENGTH 1

//Shared for UART and USB
#define GP_COM_PACKET_HEADER_LENGTH 6
#define GP_COM_PACKET_FOOTER_LENGTH 2
#define GP_COM_PACKET_COMMID_LENGTH 4

#define GP_COM_MAXIMAL_COMMAND_PARAMETERS (GP_COM_MAX_PACKET_PAYLOAD_SIZE - sizeof(gpCom_CommandID_t))


#define GP_SHMEM_MAX_PAYLOAD_LENGTH (GP_SHMEM_MSG_HEADER_LENGTHFIELD_MAX-GP_SHMEM_MSG_HEADER_LEN)

//#ifdef GP_BSP_USB_COM
/* USB uses the buffer from uart1 */
//#define GP_COM_NUM_UART 1
//#endif

#ifndef GP_BSP_UART_COM1
//Dummy define for flush function
#define GP_BSP_UART_COM1
#endif //GP_BSP_UART_COM1

#ifndef GP_BSP_UART_COM2
#define GP_COM_NUM_UART 1
#else
#define GP_COM_NUM_UART 2
#endif

#if defined(GP_COM_DIVERSITY_SERIAL_SPI) 
#define GP_COM_NUM_SERIAL (GP_COM_NUM_UART+1)
#else
#define GP_COM_NUM_SERIAL GP_COM_NUM_UART
#endif // GP_COM_DIVERSITY_SERIAL_SPI

#define gpCom_SynProtocol_FrameControl_CommIdIsEmbedded     0x80
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpCom_CommandIDTxOverflowIndication             0x02
typedef UInt8 gpCom_CommandID_t;

typedef struct gpCom_SerialPacket
{
    gpCom_CommandID_t        commandID;
    UInt8                    commandParameters[GP_COM_MAXIMAL_COMMAND_PARAMETERS];
} gpCom_SerialPacket_t;

//////////////////
// TxOverflowIndication
//////////////////

typedef struct gpCom_TxOverflowIndicationCommandParameters
{
    UInt16  numberOfMissedMessages;
}gpCom_TxOverflowIndicationCommandParameters_t;

#define gpCom_ProtocolPacketHeader     0
#define gpCom_ProtocolPacketCommId     1
#define gpCom_ProtocolPacketData       2
#define gpCom_ProtocolPacketFooter     3
typedef UInt8 gpCom_ProtocolPacket_t;

typedef struct gpCom_PacketHeader_s {
    UInt8  length;
    UInt8  moduleID;
    UInt16 crc;
} gpCom_PacketHeader_t;

typedef struct gpCom_Packet_s {
    gpCom_CommunicationId_t commId;
    UInt16 length;
    UInt8  moduleID;
    UInt8  packet[GP_COM_MAX_PACKET_PAYLOAD_SIZE];
} gpCom_Packet_t;

typedef struct {
    gpCom_Packet_t*         pPacket;
    UInt16                  crc;
    gpCom_ProtocolPacket_t  partOfPacket; //Header/data/footer
    UInt16                  counter;
    UInt16                  length;
    UInt8                   moduleID;
    gpCom_CommunicationId_t commId;
    gpCom_Protocol_t        Com_protocol;
} gpCom_ProtocolState_t;


extern gpCom_cbActivateTx_t gpCom_ActivateTxCb[];
extern UInt8 nbrOfgpCom_ActivateTxCbs;
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

//Packet memory handlers
gpCom_Packet_t* Com_GetFreePacket(void);
void            Com_FreePacket(gpCom_Packet_t* pPacket);
//Queue handler
Bool Com_AddPendingPacket(gpCom_Packet_t* pPacket);
void Com_Execute(gpCom_Packet_t * pPacket);

void Com_InitRx(void);
void Com_ConnectionClose(gpCom_CommunicationId_t commId);
void Com_DeInitRx(void);

//Specific parsing
#define gpCom_ProtocolDone       0
#define gpCom_ProtocolContinue   1
#define gpCom_ProtocolError      2
typedef UInt8 gpCom_ProtocolStatus_t;

#ifdef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
gpCom_ProtocolStatus_t ComNoSynNoCrcProtocol_ParseBuffer(UInt8 *rxbuf, UInt16 size, gpCom_ProtocolState_t* state);
gpCom_ProtocolStatus_t ComNoSynNoCrcProtocol_ParseByte(UInt8 rxbyte, gpCom_ProtocolState_t* state);
#endif
//Specific parsing
gpCom_ProtocolStatus_t ComSynProtocol_ParseBuffer(UInt8 *rxbuf, UInt16 size, gpCom_ProtocolState_t* state);
gpCom_ProtocolStatus_t ComSynProtocol_ParseByte(UInt8 rxbyte, gpCom_ProtocolState_t* state);

extern gpCom_ProtocolState_t   gpComUart_RxState[];

void Com_ParseProtocol(Int16 rxbyte, gpCom_CommunicationId_t comm_id);
void ComSerial_ParseBuffer(UInt8 *buffer, UInt16 size, gpCom_CommunicationId_t comm_id);

#if defined(GP_DIVERSITY_COM_UART) 
#define Com_FlushRxSerial() ComSerial_FlushRx();
#else
#define Com_FlushRxSerial()
#endif

#define Com_FlushRxIoctl()

#define Com_FlushRx() do { Com_FlushRxSerial() Com_FlushRxIoctl() } while(false)

#if defined( GP_DIVERSITY_COM_UART ) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
//UART functions
extern UInt16 gpComSerial_GetPacketSize(gpCom_CommunicationId_t commId, UInt16 payloadSize);
extern void gpComSerial_Init(void);
extern Bool gpComSerial_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId);
extern UInt16 gpComSerial_GetFreeSpace(gpCom_CommunicationId_t commId);
extern Bool gpComSerial_GetTXEnable(void);
extern void gpComSerial_HandleTx(void);
extern void gpComSerial_Flush(void);
extern void gpComSerial_DeInit(void);
extern Bool gpComSerial_TXDataPending(void);
extern void ComSerial_FlushRx(void);
Bool Com_IsDataWaiting(gpCom_CommunicationId_t commId);
UInt8 Com_GetData(gpCom_CommunicationId_t commId);
#endif //defined( GP_DIVERSITY_COM_UART )  || defined( GP_DIVERSITY_COM_USB)

#if defined( GP_DIVERSITY_COM_UART ) 
void gpComUart_Init(void);
void gpComUart_DeInit(void);
void gpComUart_Flush(void);
void ComUart_FlushRx(void);
void ComUart_TriggerTx(UInt8 uart);
#endif //defined( GP_DIVERSITY_COM_UART )

UInt16 Com_CalculateSizeOfNewData(gpCom_CommunicationId_t commId, UInt16* sizeContinuous);

void Com_SetDataReadPointer(gpCom_CommunicationId_t commId, UInt16 readPtr);






UInt16 Com_Call_ActivateTxCb(UInt16 overFlowCounter, gpCom_CommunicationId_t commId);

void Com_cbPacketReceived(gpCom_Packet_t* pPacket);

#if defined(GP_COM_DIVERSITY_UNLOCK_TX_AFTER_RX)
extern Bool gpCom_TxLocked;
#endif //#if defined(GP_COM_DIVERSITY_UNLOCK_TX_AFTER_RX)

void Com_TriggerTx(gpCom_CommunicationId_t commId);


extern Bool   gpCom_Initialized;
#ifdef GP_COM_DIVERSITY_SERIAL
extern Bool gpComSerial_Initialized;
#endif

#endif // _GPCOM_DEFS_H_

