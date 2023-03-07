/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpCom.c
 *
 * This file contains the implementation of the serial communication module.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_COM

#include "hal.h"
#include "gpUtils.h"
#include "gpCom.h"
#include "gpCom_defs.h"

#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COM_TX_BUFFER_ACTIVATE_SIZE          min(128,(ComTxBufferSize >> 1)) //Activate faster in larger buffers

#define GP_COM_IS_STANDARD_LOGGING_DATA_WAITING(uart)     (gpCom_ReadPtr[uart]      != gpCom_WritePtr[uart])

//GP_COM_MAX_TX_BUFFER_SIZE can be set in hal/bsp if you don't have a lot of ram
#ifndef GP_COM_MAX_TX_BUFFER_SIZE
#define GP_COM_MAX_TX_BUFFER_SIZE 1536
#endif

#ifndef GP_COM_TX_BUFFER_SIZE
#define GP_COM_TX_BUFFER_SIZE GP_COM_MAX_TX_BUFFER_SIZE
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static UInt16     gpCom_WritePtr[GP_COM_NUM_SERIAL];// needs volatile ?
static UInt16     gpCom_ReadPtr[GP_COM_NUM_SERIAL];

//RX only variables
gpCom_ProtocolState_t   gpComUart_RxState[GP_COM_NUM_SERIAL];

static UInt16 gpComUart_DiscardTxCounter[GP_COM_NUM_SERIAL];
static Bool   gpComUart_DiscardTxHappening[GP_COM_NUM_SERIAL];

Bool gpComSerial_Initialized = false;

HAL_CRITICAL_SECTION_DEF(Com_SerialBufferMutex)
/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/


//Define Tx data buffer here
static UInt8 Com_TxBuf[GP_COM_NUM_SERIAL][GP_COM_TX_BUFFER_SIZE] GP_EXTRAM_SECTION_ATTR;

#define ComTxBufferSize GP_COM_TX_BUFFER_SIZE
#define gpCom_Buf(i)    (Com_TxBuf[i])

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// Static functions
static Bool Com_WriteBlock(UInt16 length , UInt16 *sizeAvailable , UInt16 *sizeBlock , UInt8* pData, UInt8 uart,UInt16 *writeIdx);
static void Com_ActivateTxBuffer(UInt8 uart);
static void Com_CalculateSizes(UInt16* sizeAvailable, UInt16* sizeBlock, UInt8 uart);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
static UInt8 Com_CommIdToUartIndex(gpCom_CommunicationId_t commId)
{
    UInt8 uart = 0xFF; /* provide silly default to keep the compiler happy */
#ifdef GP_COM_DIVERSITY_SERIAL_SPI
    if(GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_SPI))
    {
        return (GP_COM_NUM_SERIAL-1);
    }
#endif
    /* mask away the BLE HW types */
    commId = commId & ~(GP_COM_COMM_ID_BLE);

#ifdef GP_COM_DIVERSITY_MULTIPLE_COM
    /* auto-selection */
#if defined(GP_DIVERSITY_COM_UART) 
    if(GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_UART1)
    || GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_UART2)
    )
    {
        uart = GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_UART1) ? 0 : 1;
    }
#endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
#else // GP_COM_DIVERSITY_MULTIPLE_COM
    #if   defined(GP_DIVERSITY_COM_UART) 
    uart = (commId == GP_COM_COMM_ID_UART1) ? 0 : 1;
    #endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
#endif

    GP_ASSERT_DEV_INT(uart != 0xFF);
    return uart;
}

static void Com_ActivateTxBuffer(UInt8 uart)
{
    gpCom_CommunicationId_t commId = (uart == 0 ?
        GP_COM_COMM_ID_UART1
        : GP_COM_COMM_ID_UART2
        );
    gpComUart_DiscardTxHappening[uart] = false;

#ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
    if (nbrOfgpCom_ActivateTxCbs)
    {
        UInt16 retryPackets = 0;
        /* This callback will try to resend 'retryPackets' amount of packets.*/
        retryPackets = Com_Call_ActivateTxCb(gpComUart_DiscardTxCounter[uart], commId);
        GP_ASSERT_DEV_EXT(gpComUart_DiscardTxCounter[uart] >= retryPackets);

        /* Since 'retryPackets' will be retried we don't report them as overflows.
         * !!! In fact, sending an overflow indication is a bug (SDB004-436) that
         *     can cause the buffer to fill up with overflow messages:
         *
         * 1) APP  | Buffer full (room too small for big message)
         * 2) UART | reads small amount of bytes from buffer
         * 3) UART | Trigger Com_ActivateTxBuffer (this schedules a retry, see (5) )
         * 4) UART | Append overflow message to UART buffer
         * 5) APP  | retry of large packet (still no room); goto step (2)
         *
         * DANGER: if 'gpCom_ActivateTxCb' reports the wrong amount of retryPackets
         *         this problem can still occur.
         *
         * We could try to remember the size of the largest packet and use this as the
         * minimum size before a ActivateTxBuffer is triggered but what guarantee do we
         * have that the caller won't try to send an even bigger packet next time?
         *
         * The only secure thing I can think of is an ASSERT that checks that the maximum packet
         * size that can be sent must be <= (GP_COM_TX_BUFFER_ACTIVATE_SIZE+txOverflowIndicationLength).
         */
        gpComUart_DiscardTxCounter[uart] -= retryPackets;
    }
#endif //ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK

    if ((gpComUart_DiscardTxCounter[uart]))
    {
        // Remove the following lines, when the TcpListener can interpret the TxOverflowIndication message
        // The printf could in this case be put in the else case of the new serial protocol
        UInt8 txOverflowIndicationLength = sizeof(gpCom_CommandID_t)+sizeof(gpCom_TxOverflowIndicationCommandParameters_t);
        UInt8 txOverflowIndication[sizeof(gpCom_CommandID_t)+sizeof(gpCom_TxOverflowIndicationCommandParameters_t)];

        gpCom_SerialPacket_t* pSerialPacket = (gpCom_SerialPacket_t*)(txOverflowIndication);
        pSerialPacket->commandID            = gpCom_CommandIDTxOverflowIndication;

        HOST_TO_LITTLE_UINT16(&gpComUart_DiscardTxCounter[uart]);
        MEMCPY(pSerialPacket->commandParameters,&gpComUart_DiscardTxCounter[uart],2);
        LITTLE_TO_HOST_UINT16(&gpComUart_DiscardTxCounter[uart]);

        gpCom_DataRequest(GP_COMPONENT_ID, txOverflowIndicationLength, txOverflowIndication, commId);

        //Reset overflow counter
        gpComUart_DiscardTxCounter[uart] = 0;
    }
}

// Com_Serial_Transmit_GetSpaceAvailable
static void Com_CalculateSizes(UInt16* sizeAvailable, UInt16* sizeBlock, UInt8 uart)
{
    //Calculate available size

    //Available size will always be 1 less than the total buffer size.
    //No distinction can be made between completely full or empty (indexes ==)

    // This function is called from a context where we want to add new data to the buffer
    // This function MUST be called with interrupts disabled.

#define TO_TRANSMIT_POINTER gpCom_ReadPtr

    if (TO_TRANSMIT_POINTER[uart] <= gpCom_WritePtr[uart])
    {
        *sizeAvailable = ComTxBufferSize - 1 - gpCom_WritePtr[uart] + TO_TRANSMIT_POINTER[uart];
        if(sizeBlock != NULL)
            *sizeBlock = ComTxBufferSize - gpCom_WritePtr[uart];
    }
    else
    {
        *sizeAvailable = TO_TRANSMIT_POINTER[uart] - gpCom_WritePtr[uart] - 1;
        if(sizeBlock != NULL)
            *sizeBlock = *sizeAvailable;
    }
#undef TO_TRANSMIT_POINTER
}

// Com_Serial_Transmit_GetSpaceUsed
UInt16 Com_CalculateSizeOfNewData(gpCom_CommunicationId_t commId, UInt16* sizeContinuous)
{
    UInt16 sizeOfNewData;
    UInt8 uart;
    uart = Com_CommIdToUartIndex(commId);

    // This function is called when we are transferring buffered data into an interface.
    // it needs to return the amount of bytes which has not been read yet.

    // For block transfers we don't want to handle the buffer wrap-round
    // so if the new data wraps, then we handle that in seperate parts:
    // i.e. we calculate the size up to the end of the buffer only.

    if (gpCom_ReadPtr[uart] <= gpCom_WritePtr[uart])
    {
        sizeOfNewData = gpCom_WritePtr[uart] - gpCom_ReadPtr[uart];
        if (sizeContinuous)
        {
            *sizeContinuous = sizeOfNewData;
        }
    }
    else
    {
        sizeOfNewData = ComTxBufferSize - gpCom_ReadPtr[uart] + gpCom_WritePtr[uart];
        if (sizeContinuous)
        {
            (*sizeContinuous)= ComTxBufferSize - gpCom_ReadPtr[uart];
        }
    }
    return sizeOfNewData;
}

// Com_Serial_Transmit_WriteBlock
static Bool Com_WriteBlock(UInt16 length , UInt16 *sizeAvailable , UInt16 *sizeBlock , UInt8* pData, UInt8 uart, UInt16 *writeIdx)
{
    GP_UTILS_STACK_TRACK();
    if (length > *sizeAvailable)
    {
        //Overrun detected
        return true;
    }
    else
    {
        if (length > *sizeBlock)
        {
            UInt16 length_2block;

            // Copy in 2 blocks
            MEMCPY(&(gpCom_Buf(uart)[*writeIdx]), pData, *sizeBlock);
            length_2block = length - *sizeBlock;

            //Remaining piece from start of buffer
            MEMCPY(&(gpCom_Buf(uart)[0]), &pData[*sizeBlock], length_2block);
            *writeIdx = length_2block;

            *sizeAvailable -= *sizeBlock + length_2block;
            *sizeBlock      = *sizeAvailable;
        }
        else
        {
            // Copy in 1 block
            MEMCPY(&(gpCom_Buf(uart)[*writeIdx]), pData, length);
            *writeIdx += length;
            GP_ASSERT_DEV_INT((*writeIdx <= ComTxBufferSize)); //Should be checked before
            if (*writeIdx >= ComTxBufferSize) *writeIdx = 0;

            *sizeAvailable -= length;
            *sizeBlock     -= length;
        }
    }
    return false;
}


void Com_ParseProtocol(Int16 rxbyte, gpCom_CommunicationId_t comm_id)
{
    gpCom_ProtocolState_t* state = &gpComUart_RxState[Com_CommIdToUartIndex(comm_id)];
    state->commId = comm_id;
    switch(state->Com_protocol)
    {
#ifdef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
        case gpCom_ProtocolNoSynNoCrc:
        {
            if(ComNoSynNoCrcProtocol_ParseByte(rxbyte, state) == gpCom_ProtocolDone)
            {
                state->Com_protocol = gpCom_ProtocolInvalid;
            }
            break;
        }
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
        case gpCom_ProtocolInvalid: // fall through
        default:
        {
            gpCom_ProtocolStatus_t status = gpCom_ProtocolError;
#ifdef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
            status = ComNoSynNoCrcProtocol_ParseByte(rxbyte, state);
            if (status == gpCom_ProtocolContinue)
            {
                state->Com_protocol = gpCom_ProtocolNoSynNoCrc;
                break;
            }
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
            if(status == gpCom_ProtocolDone)
            {
                state->Com_protocol = gpCom_ProtocolInvalid;
                break;
            }
            else
            {
#if !defined(GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC) 
                #error "No gpCom serial parser specified, please add"
#endif
            }
            break;
        }
    }
}


void Com_TriggerTx(gpCom_CommunicationId_t commId)
{
#if defined(GP_COM_DIVERSITY_UNLOCK_TX_AFTER_RX)
    if (gpCom_TxLocked == true)
    {
        return;
    }
#endif

#ifdef GP_COM_DIVERSITY_MULTIPLE_COM
    /* auto-selection */
    if(GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_SPI))
    {
    }
    else
    {
        #if defined(GP_DIVERSITY_COM_UART) 
        UInt8  uart;
        uart = Com_CommIdToUartIndex(commId);
        ComUart_TriggerTx(uart);
        #endif //GP_DIVERSITY_COM_UART
    }
#else // not GP_COM_DIVERSITY_COM_SELECTION_FROM_CONFIG_PINS
    #if defined(GP_DIVERSITY_COM_UART) 
    UInt8  uart;
    uart = Com_CommIdToUartIndex(commId);
    ComUart_TriggerTx(uart);
    #endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
#endif //def GP_COM_DIVERSITY_COM_SELECTION_FROM_CONFIG_PINS

}

Bool Com_IsDataWaiting(gpCom_CommunicationId_t commId)
{
    UInt8 uart;
    uart = Com_CommIdToUartIndex(commId);
#if defined(GP_COM_DIVERSITY_UNLOCK_TX_AFTER_RX)
    if (gpCom_TxLocked == true)
    {
        return false;
    }
#endif
    return (gpCom_ReadPtr[uart] != gpCom_WritePtr[uart]);
}

#if defined(GP_DIVERSITY_COM_UART)
UInt8 Com_GetData(gpCom_CommunicationId_t commId)
{
    UInt8 returnValue;
    UInt8 uart;
    uart = Com_CommIdToUartIndex(commId);

    // Output character
    returnValue = gpCom_Buf(uart)[gpCom_ReadPtr[uart]];

    // Scroll output pointer
    if ((unsigned int) ++gpCom_ReadPtr[uart] >= (unsigned int) ComTxBufferSize) gpCom_ReadPtr[uart] = 0;
    return returnValue;
}
#endif


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/****************************************************************************
 ****************************************************************************
 **                       Initialization                                   **
 ****************************************************************************
 ****************************************************************************/

void gpComSerial_Init(void)
{
    HAL_DISABLE_GLOBAL_INT();

    // init variables
    MEMSET(gpCom_WritePtr, 0, sizeof(gpCom_WritePtr));
    MEMSET(gpCom_ReadPtr, 0, sizeof(gpCom_ReadPtr));

    MEMSET(gpComUart_DiscardTxHappening, 0, sizeof(gpComUart_DiscardTxHappening));
    MEMSET(gpComUart_DiscardTxCounter, 0, sizeof(gpComUart_DiscardTxCounter));


    {
        UInt8 idx;
        for (idx=0; idx < GP_COM_NUM_SERIAL; idx++)
        {
            gpComUart_RxState[idx].Com_protocol = gpCom_ProtocolInvalid;
        }
    }

    /* no auto-selection here */
    /* init inititializes all available interfaces */
    #if defined(GP_DIVERSITY_COM_UART) 
#if defined(GP_COM_DIVERSITY_MULTIPLE_COM) 
    /* no GP_COM_DEFAULT_COMMUNICATION_ID check; the check makes no sense since the other COM diversites fall outside of COM_SERIAL */
#else
    if(GP_COMM_ID_CARRIED_BY(GP_COM_DEFAULT_COMMUNICATION_ID,GP_COM_COMM_ID_UART1) || GP_COMM_ID_CARRIED_BY(GP_COM_DEFAULT_COMMUNICATION_ID,GP_COM_COMM_ID_UART2))
#endif
    {
        gpComUart_Init();
    }

    #endif //GP_DIVERSITY_COM_UART
    HAL_ENABLE_GLOBAL_INT();

    /* USB init requires interrupts to be enabled */
    /* add SPI here */
    HAL_CREATE_MUTEX(&Com_SerialBufferMutex);
    gpComSerial_Initialized = true;
}

void gpComSerial_DeInit(void)
{
    HAL_DISABLE_GLOBAL_INT();
    /* no auto-selection here */
    /* deinit de-inititializes all available interfaces */
#if defined(GP_DIVERSITY_COM_UART) 
#if defined(GP_COM_DIVERSITY_MULTIPLE_COM) 
    /* no GP_COM_DEFAULT_COMMUNICATION_ID check; the check makes no sense since the other COM diversites fall outside of COM_SERIAL */
#else
    if(GP_COMM_ID_CARRIED_BY(GP_COM_DEFAULT_COMMUNICATION_ID,GP_COM_COMM_ID_UART1) || GP_COMM_ID_CARRIED_BY(GP_COM_DEFAULT_COMMUNICATION_ID,GP_COM_COMM_ID_UART2))
#endif
    {
        gpComUart_DeInit();
    }
#endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
    gpComSerial_Initialized = false;

    HAL_ENABLE_GLOBAL_INT();
    if(HAL_VALID_MUTEX(Com_SerialBufferMutex))
    {
        HAL_DESTROY_MUTEX(&Com_SerialBufferMutex);
    }
}

UInt16 gpComSerial_GetFreeSpace(gpCom_CommunicationId_t commId)
{
    UInt16 freeSpace;
    UInt8  uart;
    UInt16 sizeAvailable, sizeBlock;
    UInt16 overhead = gpComSerial_GetPacketSize(commId, 0);

    if (!gpCom_GetTXEnable())
        return 0;

    uart = Com_CommIdToUartIndex(commId);
    if(uart >= GP_COM_NUM_SERIAL)
    {
        return 0;
    }
    if (gpComUart_DiscardTxHappening[uart])
    {
        return 0;
    }

    HAL_ACQUIRE_MUTEX(Com_SerialBufferMutex);

    //Calculate available sizes
    Com_CalculateSizes(&sizeAvailable,&sizeBlock, uart);

    freeSpace = overhead < sizeAvailable ? sizeAvailable - overhead : 0;

    HAL_RELEASE_MUTEX(Com_SerialBufferMutex);

    return freeSpace;
}

Bool gpComSerial_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId)
{
#define GP_COM_DIVERSITY_SYN_SEQNUM
    UInt8  uart;
    Bool   overrun     = false;
    UInt16 sizeAvailable, sizeBlock;
#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    UInt16 checksumNew;
    UInt8  header[GP_COM_PACKET_HEADER_LENGTH];
#if defined(GP_COM_DIVERSITY_SYN_SEQNUM)
    static UInt8 Com_SynSeqNum = 0;
#endif
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    UInt16 writeIdx;
    if (!gpCom_GetTXEnable())
        return false;

    uart = Com_CommIdToUartIndex(commId);
    if(uart >= GP_COM_NUM_SERIAL)
    {
        return false;
    }
#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    header[0] = 'S';
    header[1] = 'Y';
    header[2] = 'N';
    header[3] = length & 0xFF;
    header[4] =(length >> 8) & 0x0F;
#if defined(GP_COM_DIVERSITY_SYN_SEQNUM)
    header[4] |= (Com_SynSeqNum << 4);
    Com_SynSeqNum  = ((Com_SynSeqNum+1) % 8);
#endif
    header[5] = moduleID;

    //calculate CRC before AtomicOn, since it takes a long time...
    checksumNew = 0;
    gpUtils_CalculatePartialCrc(&checksumNew,  header, GP_COM_PACKET_HEADER_LENGTH);

    gpUtils_CalculatePartialCrc(&checksumNew,  pData, length);

#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC

    HAL_ACQUIRE_MUTEX(Com_SerialBufferMutex);

    //Calculate available sizes
    Com_CalculateSizes(&sizeAvailable,&sizeBlock, uart);
    if (gpComSerial_GetPacketSize(commId, length) > sizeAvailable)
    {
        gpComUart_DiscardTxHappening[uart] = true;
    }

    if (gpComUart_DiscardTxHappening[uart])
    {
        gpComUart_DiscardTxCounter[uart]++;

        HAL_RELEASE_MUTEX(Com_SerialBufferMutex);

        return false;
    }
    writeIdx = gpCom_WritePtr[uart];
#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    // Write new serial protocol header
    overrun = Com_WriteBlock(GP_COM_PACKET_HEADER_LENGTH, &sizeAvailable, &sizeBlock, header, uart,&writeIdx);
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    // Write data
    if (!overrun)
    {
        overrun = Com_WriteBlock(length, &sizeAvailable, &sizeBlock, pData, uart,&writeIdx);
    }

#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    // Write data
    if (!overrun)
    {
      UInt8 footer[GP_COM_PACKET_FOOTER_LENGTH];
        footer[0] = (UInt8)((checksumNew) >> 0);
        footer[1] = (UInt8)((checksumNew) >> 8);
        // Write additional new serial protocol header
        overrun = Com_WriteBlock(GP_COM_PACKET_FOOTER_LENGTH, &sizeAvailable, &sizeBlock, footer, uart,&writeIdx);
    }
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    if (!overrun)
    {
        /* To avoid the need for locking between an iothread and the current thread, */
        /* only change the tx pointer once (and also only if message does fit) */
        gpCom_WritePtr[uart] = writeIdx;
    }
    HAL_RELEASE_MUTEX(Com_SerialBufferMutex);

    HAL_ACQUIRE_MUTEX(Com_SerialBufferMutex);

    GP_ASSERT_DEV_INT(!overrun);

    // Enable Tx interrupt immediately for continuous logging
    Com_TriggerTx(commId);

    HAL_RELEASE_MUTEX(Com_SerialBufferMutex);

    return !overrun;
}

void gpComSerial_HandleTx(void)
{
    UIntLoop i;


    //Tx re-activation after overflow
    for(i = 0; i < GP_COM_NUM_SERIAL; i++)
    {
        if (gpComUart_DiscardTxHappening[i])
        {
            UInt16 sizeAvailable;

            HAL_ACQUIRE_MUTEX(Com_SerialBufferMutex);
            Com_CalculateSizes(&sizeAvailable, NULL, i);
            HAL_RELEASE_MUTEX(Com_SerialBufferMutex);

            if (sizeAvailable > GP_COM_TX_BUFFER_ACTIVATE_SIZE)
            {
                // Activate back the tx buffer, when sufficient empty space available
                Com_ActivateTxBuffer(i);
            }
        }
    }
}

Bool gpComSerial_GetTXEnable( void )
{
    Bool txEnabled = false;
#if !defined(GP_DIVERSITY_LINUXKERNEL)
    #if defined(GP_DIVERSITY_COM_UART) 
    txEnabled |= HAL_UART_COM_TX_ENABLED();
    #endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
#else
    txEnabled=true;
#endif //if !defined(GP_DIVERSITY_LINUXKERNEL)
#if defined(GP_COM_DIVERSITY_SERIAL_SPI)
    txEnabled |= true;
#endif
    return gpComSerial_Initialized && txEnabled;
}

Bool gpComSerial_TXDataPending(void)
{
    Bool pending = false;
    UIntLoop i;

    if (!gpComSerial_GetTXEnable())
    {
        return false;
    }

    for(i = 0; i < GP_COM_NUM_SERIAL; i++)
    {
        if(GP_COM_IS_STANDARD_LOGGING_DATA_WAITING(i))
        {
            pending = true;
            break;
        }
    }

    return pending;
}

void gpComSerial_Flush(void)
{
    /* we only flush the default comm id */
    /* You can't have UART and USB active at the same time */
    /* as they are using the same gpCom buffers */
    #if defined(GP_DIVERSITY_COM_UART) 
    if((GP_COM_DEFAULT_COMMUNICATION_ID == GP_COM_COMM_ID_UART1) || (GP_COM_DEFAULT_COMMUNICATION_ID == GP_COM_COMM_ID_UART2))
    {
        gpComUart_Flush();
    }
    #endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
    gpComSerial_HandleTx();
}

void ComSerial_FlushRx(void)
{
    /* no auto-selection here */
    #if defined(GP_DIVERSITY_COM_UART) 

    ComUart_FlushRx();
    #endif //GP_DIVERSITY_COM_UART
    /* add SPI here */
}


UInt16 gpComSerial_GetPacketSize(gpCom_CommunicationId_t commId, UInt16 payloadSize)
{
#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    UInt16 r = GP_COM_PACKET_HEADER_LENGTH + GP_COM_PACKET_FOOTER_LENGTH;
    return r + payloadSize;
#else
    return payloadSize;
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
}

