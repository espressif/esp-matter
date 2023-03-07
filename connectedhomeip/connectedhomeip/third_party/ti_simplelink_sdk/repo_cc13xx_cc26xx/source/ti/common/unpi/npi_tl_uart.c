/******************************************************************************

 @file  npi_tl_uart.c

 @brief NPI Transport Layer Module for UART

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2021-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

//*****************************************************************************
// Includes
//*****************************************************************************
#include <string.h>
#include <xdc/std.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "hal_types.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include "npi_data.h"
#include "npi_util.h"
#include "npi_tl_uart.h"
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

//*****************************************************************************
// Defines
//*****************************************************************************

//! \brief NPI UART Message Indexes and Constants
//
#define NPI_UART_MSG_NON_PAYLOAD_LEN                 0x05
#define NPI_UART_MSG_HDR_LEN                         0x04
#define NPI_UART_MSG_SOF                             0xFE
#define NPI_UART_MSG_SOF_IDX                         0x00

//*****************************************************************************
// Typedefs
//*****************************************************************************

//*****************************************************************************
// Globals
//*****************************************************************************
//! \brief UART Handle for UART Driver
static UART_Handle uartHandle;

//! \brief UART ISR Rx Buffer
static uint8_t isrRxBuf[UART_ISR_BUF_SIZE];

//! \brief NPI TL call back function for the end of a UART transaction
static npiCB_t npiTransmitCB = NULL;

#if (NPI_FLOW_CTRL == 1)
//! \brief Flag signalling receive in progress
static uint8_t RxActive = FALSE;

//! \brief Flag signalling transmit in progress
static uint8_t TxActive = FALSE;

//! \brief Value of REM RDY NPI TL pin
static uint8_t remRdy_flag = 1;
#endif // NPI_FLOW_CTRL = 1

//! \brief Length of bytes received
static uint16_t TransportRxLen = 0;

//! \brief Length of bytes to send from NPI TL Tx Buffer
static uint16_t TransportTxLen = 0;

//! \brief UART Object. Initialized in board specific files
extern UARTCC26XX_Object uartCC26XXObjects[];

//! \brief NPI Transport Layer Buffer variables defined in npi_tl.c
extern uint8_t *npiRxBuf;
extern uint8_t *npiTxBuf;
extern uint16_t npiBufSize;

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

//! \brief UART ISR function. Invoked upon specific threshold of UART RX FIFO size
static uint16_t NPITLUART_readIsrBuf(size_t size);

//! \brief UART Callback invoked after UART write completion
static void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size);

//! \brief UART Callback invoked after readsize has been read or timeout
static void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size);

//! \brief Check for whether a complete and valid packet has been received
static uint8_t NPITLUART_validPacketFound(void);

//! \brief      Calculate FCS over the given length of buf
static uint8_t NPITLUART_calcFCS(uint8_t *buf, uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device.
//!
//! \param[in]  portID      ID value for board specific UART port
//! \param[in]  portParams  Parameters used to initialize UART port
//! \param[in]  npiCBack    Transport Layer call back function
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_openTransport(uint8_t portID, UART_Params *portParams,
                             npiCB_t npiCBack)
{
    npiTransmitCB = npiCBack;

    // Initialize the UART driver
    UART_init();

    // Add call backs UART parameters.
    portParams->readCallback = NPITLUART_readCallBack;
    portParams->writeCallback = NPITLUART_writeCallBack;

    // Open / power on the UART.
    uartHandle = UART_open(portID, portParams);
    //Enable Partial Reads on all subsequent UART_read()
    UART_control(uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE,  NULL);
}

// -----------------------------------------------------------------------------
//! \brief      This routine closes Transport Layer port
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_closeTransport(void)
{
    UART_close(uartHandle);
}

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine stops any pending reads
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_stopTransfer(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    remRdy_flag = 1;

    // If we have no bytes in FIFO yet we must assume there was nothing to read
    // or that the FIFO has already been read for this UART_read()
    // In either case UART_readCancel will call the read CB function and it will
    // invoke npiTransmitCB with the appropriate number of bytes read
    if (!UARTCharsAvail(((UARTCC26XX_HWAttrsV2 const *)(uartHandle->hwAttrs))->baseAddr))
    {
        RxActive = FALSE;
        UART_readCancel(uartHandle);
    }

    NPIUtil_ExitCS(key);
}
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine is called from the application context when REM RDY
//!             is de-asserted
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_handleRemRdyEvent(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    remRdy_flag = 0;

    // If we haven't already begun reading, now is the time before Master
    //    potentially starts to send data
    // The !TxActive condition is because we will call UART_npiRead() prior to setting
    // TxActive true. There is the possibility that REM RDY gets set high which
    // clears RxActive prior to us getting to this event. This will cause us to
    // read twice per transaction which will cause the transaction to never
    // complete
    if (!RxActive && !TxActive)
    {
        NPITLUART_readTransport();
    }

    // If we have something to write, then the Master has signalled it is ready
    //    to receive. Time to write.
    if (TxActive)
    {
        // Check to see if transport is successful. If not, reset TxLen to allow
        // another write to be processed
        if (UART_write(uartHandle, npiTxBuf, TransportTxLen) == UART_ERROR)
        {
          TxActive = FALSE;
          TransportTxLen = 0;
        }
    }

    NPIUtil_ExitCS(key);
}
#endif // NPI_FLOW_CTRL = 1

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on Write completion
//!
//! \param[in]  handle - handle to the UART port
//! \param[in]  ptr    - pointer to data to be transmitted
//! \param[in]  size   - size of the data
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

#if (NPI_FLOW_CTRL == 1)
    if (!RxActive)
    {
        UART_readCancel(uartHandle);

        if (npiTransmitCB)
        {
            if (NPITLUART_validPacketFound() == NPI_SUCCESS)
            {
              // Decrement as to not include trailing FCS byte
              TransportRxLen--;
            }
            else
            {
              // Did not receive valid packet so denote RX length as zero in CB
              TransportRxLen = 0;
            }

            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }

    TxActive = FALSE;

#else
    if (npiTransmitCB)
    {
        npiTransmitCB(0,TransportTxLen);
    }
#endif // NPI_FLOW_CTRL = 1

    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on Read completion of readSize/receive
//!             timeout
//!
//! \param[in]  handle - handle to the UART port
//! \param[in]  ptr    - pointer to buffer to read data into
//! \param[in]  size   - size of the data
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size)
{
#if (NPI_FLOW_CTRL == 0)
    uint16_t packetSize;
    uint16_t sofIndex;
#endif // NPI_FLOW_CTRL = 0
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    if (size)
    {
        if (size != NPITLUART_readIsrBuf(size))
        {
            // Buffer overflow imminent. Cancel read and pass to higher layers
            // for handling
#if (NPI_FLOW_CTRL == 1)
            RxActive = FALSE;
#endif // NPI_FLOW_CTRL = 1
        }
    }
#if (NPI_FLOW_CTRL == 1)
    // Read has been cancelled by transport layer, or bus timeout and no bytes in FIFO
    //    - do not invoke another read
    if (!UARTCharsAvail(((UARTCC26XX_HWAttrsV2 const *)(uartHandle->hwAttrs))->baseAddr) &&
            remRdy_flag)
    {
        RxActive = FALSE;

        // If TX has also completed then we are safe to issue call back
        if (!TxActive && npiTransmitCB)
        {
            if (NPITLUART_validPacketFound() == NPI_SUCCESS)
            {
                // Decrement as to not include trailing FCS byte
                TransportRxLen--;
            }
            else
            {
                // Did not receive valid packet so denote RX length as zero in CB
                TransportRxLen = 0;
            }

            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }
    else
    {
        UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
    }
#else
    while (NPITLUART_validPacketFound() == NPI_SUCCESS &&
          npiTransmitCB)
    {
        // The Rx Buffer can contain more than the one valid packet
        // Must only return the number of bytes for the first valid packet and
        // shift the remaining bytes to the beginning of the RX buffer after the
        // CB has completed

        // SOF has already been removed from npiRxBuf here. It is removed
        // when bytes are copied from the ISR Rx buffer to npiRxBuf
        packetSize = (uint16) npiRxBuf[0];
        packetSize += ((uint16) npiRxBuf[1]) << 8;
        packetSize += NPI_UART_MSG_HDR_LEN;

        npiTransmitCB(packetSize,0);

        // Must look for SOF of next packet, first eligible byte is after the
        // FCS of the valid packet
        sofIndex = packetSize + 1;
        while(sofIndex < TransportRxLen)
        {

            if (npiRxBuf[sofIndex] == NPI_UART_MSG_SOF)
            {
                break;
            }

            sofIndex++;
        }

        // New RX len does not include bytes prior to and include the
        // next found SOF byte. If no next SOF byte then len is 0
        TransportRxLen = (TransportRxLen == sofIndex) ? 0 :
                              TransportRxLen - sofIndex - 1;

        // Copy all bytes following next found SOF to beginning of the RX buf
        // and check again for another valid packet
        memcpy(npiRxBuf,&npiRxBuf[sofIndex + 1],TransportRxLen);
    }

    UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
#endif // NPI_FLOW_CTRL = 1

    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer based on len,
//!             and places it into the buffer.
//!
//! \param[in]  size - amount of bytes in UART ISR Rx Buffer
//!
//! \return     uint16_t - number of bytes read from transport
// -----------------------------------------------------------------------------
uint16_t NPITLUART_readIsrBuf(size_t size)
{
    // Check to see if there is enough room in the npiRxBuf to move all
    // bytes from the ISR buf. If not, move as much as possible. A return value
    // not equal to size will notify of a lack of buffer space
    size = ((TransportRxLen + size) > npiBufSize)?
            npiBufSize - TransportRxLen : size;

    // Check if this is the first byte in the message. If it is then
    // make sure it is equal to SOF but do not write it into the
    // Transport RX Buf. If it is not SOF then ignore all bytes in ISR
    if (TransportRxLen == 0 && isrRxBuf[0] == NPI_UART_MSG_SOF)
    {
        // Start the copying of the message but skip SOF byte
        memcpy(&npiRxBuf[TransportRxLen],&isrRxBuf[1],size-1);
        TransportRxLen += (size - 1);
    }
    else if (TransportRxLen != 0)
    {
        // This is not the first ISR RX of the message, copy all bytes
        memcpy(&npiRxBuf[TransportRxLen],&isrRxBuf[0],size);
        TransportRxLen += size;
    }

    // Clear ISR Buffer
    memset(isrRxBuf,0,size);

    return(size);
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the UART
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_readTransport(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

#if (NPI_FLOW_CTRL == 1)
    RxActive = TRUE;
#endif // NPI_FLOW_CTRL = 1

    TransportRxLen = 0;
    UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);

    NPIUtil_ExitCS(key);
}


// -----------------------------------------------------------------------------
//! \brief      This routine writes copies buffer addr to the transport layer.
//!
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint16_t - number of bytes written to transport
// -----------------------------------------------------------------------------
uint16_t NPITLUART_writeTransport(uint16_t len)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    npiTxBuf[NPI_UART_MSG_SOF_IDX] = NPI_UART_MSG_SOF;
    npiTxBuf[len + 1] = NPITLUART_calcFCS((uint8_t *)&npiTxBuf[1],len);
    TransportTxLen = len + 2;

#if (NPI_FLOW_CTRL == 1)
    TxActive = TRUE;

    // Start reading prior to impending write transaction
    // We can only call UART_write() once REM RDY has been signaled from Master
    // device
    NPITLUART_readTransport();
#else
    // Check to see if transport is successful. If not, reset TxLen to allow
    // another write to be processed
    if(UART_write(uartHandle, npiTxBuf, TransportTxLen) == UART_ERROR)
    {
      TransportTxLen = NPI_BUSY;
    }
#endif // NPI_FLOW_CTRL = 1

    NPIUtil_ExitCS(key);

    return(len);
}

// -----------------------------------------------------------------------------
//! \brief      Check for whether a complete and valid packet has been received.
//!             If packet incomplete do nothing. If packet is invalid, flush
//!             the Rx buffer
//!
//! \return     uint8_t - NPI_SUCCESS if Success, NPI_INCOMPLETE_PACKET if not
//!                     enough bytes received, NPI_INVALID_PACKET if incorrect
//!                     format or FCS
// -----------------------------------------------------------------------------
uint8_t NPITLUART_validPacketFound(void)
{
    uint16_t payloadLen;
    uint8_t fcs;

    // SOF has already been removed from npiRxBuf here. It is removed
    // when bytes are copied from the ISR Rx buffer to npiRxBuf
    payloadLen = (uint16) npiRxBuf[0];
    payloadLen += ((uint16) npiRxBuf[1]) << 8;

    if ((payloadLen + NPI_UART_MSG_NON_PAYLOAD_LEN) > npiBufSize)
    {
      // Bad Frame Length, impossible to receive it entierely.
      // Frame might be corrupted.
      // Flush RX buffer before returning error.
      TransportRxLen = 0;
      return(NPI_INVALID_PKT);
    }

    // Check to make sure we have received all bytes of this message
    if (TransportRxLen < (payloadLen + NPI_UART_MSG_NON_PAYLOAD_LEN))
    {
        return(NPI_INCOMPLETE_PKT);
    }

    // Calculate FCS of this message
    fcs = NPITLUART_calcFCS((uint8_t *)npiRxBuf, payloadLen + NPI_UART_MSG_HDR_LEN);

    if (fcs != npiRxBuf[payloadLen + NPI_UART_MSG_HDR_LEN])
    {
        // Invalid FCS, Flush RX buffer before returning error
        TransportRxLen = 0;

        return(NPI_INVALID_PKT);
    }

    return(NPI_SUCCESS);
}

// -----------------------------------------------------------------------------
//! \brief      Calculate FCS over the given length of buf
//!
//! \param[in]  buf - Pointer to first byte to use for FCS
//!             len - Number of bytes to calculate FCS over.
//!
//! \return     uint8_t - FCS value
// -----------------------------------------------------------------------------
uint8_t NPITLUART_calcFCS(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t fcs = 0;

    for (i = 0; i < len; i++)
    {
        fcs ^= buf[i];
    }

    return(fcs);
}
