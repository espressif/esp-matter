#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
/******************************************************************************

 @file  npi_tl_uart.c

 @brief NPI Transport Layer Module for UART

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

// ****************************************************************************
// includes
// ****************************************************************************
#include <string.h>
#include <xdc/std.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "icall.h"
#include <ti_drivers_config.h>
#include "hal_types.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include "inc/npi_config.h"
#include "inc/npi_tl_uart.h"
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//*****************************************************************************
// globals
//*****************************************************************************
//! \brief UART Handle for UART Driver
static UART_Handle uartHandle;

//! \brief UART ISR Rx Buffer
static Char isrRxBuf[UART_ISR_BUF_SIZE];

//! \brief NPI TL call back function for the end of a UART transaction
static npiCB_t npiTransmitCB = NULL;

#if (NPI_FLOW_CTRL == 1)
//! \brief Flag signalling receive in progress
static uint8 RxActive = FALSE;

//! \brief Flag signalling transmit in progress
static uint8 TxActive = FALSE;

//! \brief Value of MRDY NPI TL pin
static uint8 mrdy_flag = 1;
#endif // NPI_FLOW_CTRL = 1

//! \brief Pointer to NPI TL TX Buffer
static Char* TransportRxBuf;

//! \brief Length of bytes received
static uint16 TransportRxLen = 0;

//! \brief Pointer to NPI TL RX Buffer
static Char* TransportTxBuf;

//! \brief Length of bytes to send from NPI TL Tx Buffer
static uint16 TransportTxLen = 0;

//! \brief UART Object. Initialized in board specific files
extern UARTCC26XX_Object uartCC26XXObjects[];

//*****************************************************************************
// function prototypes
//*****************************************************************************

//! \brief UART ISR function. Invoked upon specific threshold of UART RX FIFO size
static uint16 NPITLUART_readIsrBuf(size_t size);

//! \brief UART Callback invoked after UART write completion
static void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size);

//! \brief UART Callback invoked after readsize has been read or timeout
static void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size);

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device.
//!
//! \param[in]  tRxBuf - pointer to NPI TL Tx Buffer
//! \param[in]  tTxBuf - pointer to NPI TL Rx Buffer
//! \param[in]  npiCBack - NPI TL call back function to be invoked at the end of
//!             a UART transaction
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_initializeTransport(Char *tRxBuf, Char *tTxBuf, npiCB_t npiCBack)
{
    UART_Params params;

    TransportRxBuf = tRxBuf;
    TransportTxBuf = tTxBuf;
    npiTransmitCB = npiCBack;

    // Initialize the UART driver
    UART_init();

    // Configure UART parameters.
    UART_Params_init(&params);
    params.baudRate = NPI_UART_BR;
    params.readDataMode = UART_DATA_BINARY;
    params.writeDataMode = UART_DATA_BINARY;
    params.dataLength = UART_LEN_8;
    params.stopBits = UART_STOP_ONE;
    params.readMode = UART_MODE_CALLBACK;
    params.writeMode = UART_MODE_CALLBACK;
    params.readEcho = UART_ECHO_OFF;

    params.readCallback = NPITLUART_readCallBack;
    params.writeCallback = NPITLUART_writeCallBack;

    // Open / power on the UART.
    uartHandle = UART_open(CONFIG_DISPLAY_UART, &params);
    if (uartHandle == NULL)
    {
      // An error occured, or indexed UART peripheral is already opened
      HAL_ASSERT( HAL_ASSERT_CAUSE_UNEXPECTED_ERROR );
    }

    //Enable Partial Reads on all subsequent UART_read()
    UART_control(uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE,  NULL);


#if (NPI_FLOW_CTRL == 0)
    // This call will start repeated Uart Reads when Power Savings is disabled
    NPITLUART_readTransport();
#endif // NPI_FLOW_CTRL = 0

    return;
}

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine stops any pending reads
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_stopTransfer(void)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    mrdy_flag = 1;

    // If we have no bytes in FIFO yet we must assume there was nothing to read
    // or that the FIFO has already been read for this UART_read()
    // In either case UART_readCancel will call the read CB function and it will
    // invoke npiTransmitCB with the appropriate number of bytes read
    if (!UARTCharsAvail(((UARTCC26XX_HWAttrsV2 const *)(uartHandle->hwAttrs))->baseAddr))
    {
        RxActive = FALSE;
        UART_readCancel(uartHandle);
    }

    ICall_leaveCriticalSection(key);
    return;
}
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine is called from the application context when MRDY is
//!             de-asserted
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_handleMrdyEvent(void)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    mrdy_flag = 0;

    // If we haven't already begun reading, now is the time before Master
    //    potentially starts to send data
    // The !TxActive condition is because we will call UART_npiRead() prior to setting
    // TxActive true. There is the possibility that MRDY gets set high which
    // clears RxActive prior to us getting to this event. This will cause us to
    // read twice per transaction which will cause the transaction to never
    // complete
    if ( !RxActive && !TxActive )
    {
        NPITLUART_readTransport();
    }

    // If we have something to write, then the Master has signalled it is ready
    //    to receive. Time to write.
    if ( TxActive )
    {
        // Check to see if transport is successful. If not, reset TxLen to allow
        // another write to be processed
        if ( UART_write(uartHandle, TransportTxBuf, TransportTxLen) == UART_ERROR )
        {
          TxActive = FALSE;
          TransportTxLen = 0;
        }
    }

    ICall_leaveCriticalSection(key);

    return;
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
static void NPITLUART_writeCallBack(UART_Handle handle, void *ptr, size_t size)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

#if (NPI_FLOW_CTRL == 1)
    if ( !RxActive )
    {
        UART_readCancel(uartHandle);
        if ( npiTransmitCB )
        {
            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }

    TxActive = FALSE;
#else
    if ( npiTransmitCB )
    {
        npiTransmitCB(0,TransportTxLen);
    }
#endif // NPI_FLOW_CTRL = 1

    ICall_leaveCriticalSection(key);
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
static void NPITLUART_readCallBack(UART_Handle handle, void *ptr, size_t size)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    if (size)
    {
        if (size != NPITLUART_readIsrBuf(size))
        {
            // Buffer overflow imminent. Cancel read and pass to higher layers
            // for handling
#if (NPI_FLOW_CTRL == 1)
            RxActive = FALSE;
#endif // NPI_FLOW_CTRL = 1
            if ( npiTransmitCB )
            {
                npiTransmitCB(NPI_TL_BUF_SIZE,TransportTxLen);
            }
        }
    }

#if (NPI_FLOW_CTRL == 1)
    // Read has been cancelled by transport layer, or bus timeout and no bytes in FIFO
    //    - do not invoke another read
    if ( !UARTCharsAvail(((UARTCC26XX_HWAttrsV2 const *)(uartHandle->hwAttrs))->baseAddr) &&
            mrdy_flag )
    {
        RxActive = FALSE;

        // If TX has also completed then we are safe to issue call back
        if ( !TxActive && npiTransmitCB )
        {
            npiTransmitCB(TransportRxLen,TransportTxLen);
        }
    }
    else
    {
        UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
    }
#else
    if ( npiTransmitCB )
    {
        npiTransmitCB(size,0);
    }
    TransportRxLen = 0;
    UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);
#endif // NPI_FLOW_CTRL = 1

    ICall_leaveCriticalSection(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer based on len,
//!             and places it into the buffer.
//!
//! \param[in]  size - amount of bytes in UART ISR Rx Buffer
//!
//! \return     uint16 - number of bytes read from transport
// -----------------------------------------------------------------------------
static uint16 NPITLUART_readIsrBuf(size_t size)
{
    uint8_t i = 0;

    // Copy the UART buffer to the application buffer
    // Do not allow overflow of buffer. Instead pass up to NPI module and allow
    // it to handle
    for (; (i < size) && (TransportRxLen < NPI_TL_BUF_SIZE); i++)
    {
        TransportRxBuf[TransportRxLen++] = isrRxBuf[i];
        isrRxBuf[i] = 0;
    }

    return i;
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the UART
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLUART_readTransport(void)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

#if (NPI_FLOW_CTRL == 1)
    RxActive = TRUE;
#endif // NPI_FLOW_CTRL = 1

    TransportRxLen = 0;
    UART_read(uartHandle, &isrRxBuf[0], UART_ISR_BUF_SIZE);

    ICall_leaveCriticalSection(key);
}


// -----------------------------------------------------------------------------
//! \brief      This routine writes copies buffer addr to the transport layer.
//!
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint8 - number of bytes written to transport
// -----------------------------------------------------------------------------
uint16 NPITLUART_writeTransport(uint16 len)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    TransportTxLen = len;

#if (NPI_FLOW_CTRL == 1)
    TxActive = TRUE;

    // Start reading prior to impending write transaction
    // We can only call UART_write() once MRDY has been signaled from Master
    // device
    NPITLUART_readTransport();
#else
    // Check to see if transport is successful. If not, reset TxLen to allow
    // another write to be processed
    if(UART_write(uartHandle, TransportTxBuf, TransportTxLen) == UART_ERROR )
    {
      TransportTxLen = 0;
    }
#endif // NPI_FLOW_CTRL = 1
    ICall_leaveCriticalSection(key);

    return TransportTxLen;
}
#endif// HCI_TL_FULL || PTM_MODE
