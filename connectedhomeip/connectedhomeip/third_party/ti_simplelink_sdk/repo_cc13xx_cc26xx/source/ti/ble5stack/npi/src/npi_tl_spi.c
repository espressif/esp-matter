/******************************************************************************

 @file  npi_tl_spi.c

 @brief NPI Transport Layer Module for SPI

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
#include <ti_drivers_config.h>
#include "hal_types.h"
#include "icall.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include "inc/npi_config.h"
#include "inc/npi_tl.h"
#include "inc/npi_tl_spi.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

// ****************************************************************************
// defines
// ****************************************************************************

#define SPI_TX_FIELD_LEN                        NPI_SPI_HDR_LEN
#define SPI_TX_HDR_LEN                          3
#define SPI_TX_ZERO_PAD_INDEX                   0
#define SPI_TX_SOF_INDEX                        1
#define SPI_TX_LEN_INDEX                        2

#define SPI_RX_FIELD_LEN                        3
#define SPI_RX_HDR_LEN                          2
#define SPI_RX_SOF_INDEX                        0
#define SPI_RX_LEN_INDEX                        1

#define SPI_SOF                                 0xFE

// ****************************************************************************
// typedefs
// ****************************************************************************

//*****************************************************************************
// globals
//*****************************************************************************

//! \brief Handle for SPI object used by SPI driver
static SPI_Handle spiHandle;

//! \brief Structure that defines parameters of one SPI transaction
static SPI_Transaction spiTransaction;

//! \brief NPI TL call back function for the end of a SPI transaction
static npiCB_t npiTransmitCB = NULL;

//! \brief Pointer to NPI TL RX Buffer
static Char* TransportRxBuf;

//! \brief Pointer to NPI TL RX Buffer
static Char* TransportTxBuf;

//! \brief Length of bytes to send from NPI TL Tx Buffer
static uint16 TransportTxBufLen = 0;

//! \brief Flag signalling receive in progress
static uint8 RxActive = FALSE;

//*****************************************************************************
// function prototypes
//*****************************************************************************

//! \brief Call back function invoked at the end of a SPI transaction
static void NPITLSPI_CallBack(SPI_Handle handle, SPI_Transaction  *objTransaction);

//! \brief Calculate FCS field of SPI Transaction frame
static uint8_t NPITLSPI_calcFCS(Char *msg_ptr, uint8 len);

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device.
//!
//! \param[in]  tRxBuf - pointer to NPI TL Tx Buffer
//! \param[in]  tTxBuf - pointer to NPI TL Rx Buffer
//! \param[in]  npiCBack - NPI TL call back function to be invoked at the end of
//!             a SPI transaction
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_initializeTransport(Char *tRxBuf, Char *tTxBuf, npiCB_t npiCBack)
{
    SPI_Params spiParams;

    TransportRxBuf = tRxBuf;
    TransportTxBuf = tTxBuf;
    npiTransmitCB = npiCBack;

    // Initialize the SPI driver
    SPI_init();

    // Configure SPI parameters
    SPI_Params_init(&spiParams);

    // Slave mode
    spiParams.mode = SPI_SLAVE;
    spiParams.bitRate = SPI_SLAVE_BAUD_RATE;
    spiParams.frameFormat = SPI_POL1_PHA1;
    spiParams.transferMode = SPI_MODE_CALLBACK;
    spiParams.transferCallbackFxn = NPITLSPI_CallBack;

    // Attempt to open SPI
    spiHandle = SPI_open(NPI_SPI_CONFIG, &spiParams);
    if (spiHandle == NULL)
    {
      // An error occured, or peripheral is already opened
      HAL_ASSERT( HAL_ASSERT_CAUSE_UNEXPECTED_ERROR );
    }

    return;
}

// -----------------------------------------------------------------------------
//! \brief      This routine stops any pending reads
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_stopTransfer()
{
    SPI_transferCancel(spiHandle);
    return;
}

// -----------------------------------------------------------------------------
//! \brief      This routine is called from the application context when MRDY is
//!             de-asserted
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_handleMrdyEvent()
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    // If we have not already set up a write then we must be reading
    // during this transaction. There is a possibility that after
    // bidirectional transaction there is an extra MRDY event. This event
    // could cause a double read (which would clear the RX Buffer) so this
    // check ignores the MRDY event if Rx is already in progress
    if (!TransportTxBufLen && !RxActive)
    {
      NPITLSPI_readTransport();
    }

    ICall_leaveCriticalSection(key);
    return;
}

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on transmission completion
//!
//! \param[in]  handle - handle to the SPI port
//! \param[in]  objTransaction    - handle for SPI transmission
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITLSPI_CallBack(SPI_Handle handle, SPI_Transaction  *objTransaction)
{
    uint8 i;
    uint16 readLen = 0;
    uint16 storeTxLen = TransportTxBufLen;

    // Check if a valid packet was found
    // SOF:
    if ( TransportRxBuf[SPI_RX_SOF_INDEX] == SPI_SOF &&
            objTransaction->count)
    {
        // Length:
        readLen = TransportRxBuf[SPI_RX_LEN_INDEX];

        // FCS:
        if ( TransportRxBuf[readLen + SPI_RX_HDR_LEN] ==
                NPITLSPI_calcFCS(&TransportRxBuf[SPI_RX_LEN_INDEX],readLen + 1) )
        {
            // Message is valid. Shift bytes to remove header
            for( i = 0 ; i < readLen ; i++)
            {
                TransportRxBuf[i] = TransportRxBuf[i + SPI_RX_HDR_LEN];
            }
        }
        else
        {
            // Invalid FCS. Discard message
            readLen = 0;
        }
    }

     //All bytes in TxBuf must be sent by this point
    TransportTxBufLen = 0;
    RxActive = FALSE;

    if ( npiTransmitCB )
    {
        npiTransmitCB(readLen,storeTxLen);
    }


}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer
//!             and places it into the buffer.
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_readTransport()
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    RxActive = TRUE;
    TransportTxBufLen = 0;

    // Clear DMA Rx buffer and clear extra Tx buffer bytes to ensure clean buffer
    //    for next RX/TX
    memset(TransportRxBuf, 0, NPI_TL_BUF_SIZE);
    memset(&TransportTxBuf[TransportTxBufLen], 0, NPI_TL_BUF_SIZE - TransportTxBufLen);

    // set up the SPI Transaction
    spiTransaction.txBuf = TransportTxBuf;
    spiTransaction.rxBuf = TransportRxBuf;
    spiTransaction.count = NPI_TL_BUF_SIZE;
    SPI_transfer(spiHandle, &spiTransaction);

    ICall_leaveCriticalSection(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine initializes and begins a SPI transaction
//!
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint8 - number of bytes written to transport
// -----------------------------------------------------------------------------
uint16 NPITLSPI_writeTransport(uint16 len)
{
    int16 i = 0;
    ICall_CSState key;

    key = ICall_enterCriticalSection();

    TransportTxBufLen = len + SPI_TX_FIELD_LEN;

    // Shift TX message two bytes to give room for SPI header
    for( i = ( TransportTxBufLen - 1 ) ; i >= 0 ; i-- )
    {
      TransportTxBuf[i + SPI_TX_HDR_LEN] = TransportTxBuf[i];
    }

    // Add header (including a zero padding byte required by the SPI driver)
    TransportTxBuf[SPI_TX_ZERO_PAD_INDEX] = 0x00;
    TransportTxBuf[SPI_TX_SOF_INDEX] = SPI_SOF;
    TransportTxBuf[SPI_TX_LEN_INDEX] = len;

    // Calculate and append FCS at end of Frame
    TransportTxBuf[TransportTxBufLen - 1] =
        NPITLSPI_calcFCS(&TransportTxBuf[SPI_TX_LEN_INDEX],len + 1);

    // Clear DMA Rx buffer and clear extra Tx buffer bytes to ensure clean buffer
    //    for next RX/TX
    memset(TransportRxBuf, 0, NPI_TL_BUF_SIZE);
    memset(&TransportTxBuf[TransportTxBufLen], 0, NPI_TL_BUF_SIZE - TransportTxBufLen);

    // set up the SPI Transaction
    spiTransaction.count = NPI_TL_BUF_SIZE;
    spiTransaction.txBuf = TransportTxBuf;
    spiTransaction.rxBuf = TransportRxBuf;

    // Check to see if transport is successful. If not, reset TxBufLen to allow
    // another write to be processed
    if( ! SPI_transfer(spiHandle, &spiTransaction) )
    {
      TransportTxBufLen = 0;
    }

    ICall_leaveCriticalSection(key);

    return TransportTxBufLen;
}

// ----------------------------------------------------------------------------
//! \brief      Calculate the FCS of a message buffer by XOR'ing each uint8_t.
//!             Remember to NOT include SOP and FCS fields, so start at the CMD
//!             field.
//!
//! \param[in]  msg_ptr   message pointer
//! \param[in]  len       length (in uint8_ts) of message
//!
//! \return     uint8_t
// ----------------------------------------------------------------------------
uint8_t NPITLSPI_calcFCS(Char *msg_ptr, uint8 len)
{
    uint8_t x;
    uint8_t xorResult;

    xorResult = 0;

    for (x = 0; x < len; x++, msg_ptr++)
    {
        xorResult = xorResult ^ *msg_ptr;
    }

    return (xorResult);
}
