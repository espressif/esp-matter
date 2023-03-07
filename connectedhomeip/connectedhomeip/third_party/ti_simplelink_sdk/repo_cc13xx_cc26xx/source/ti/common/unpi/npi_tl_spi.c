/******************************************************************************

 @file  npi_tl_spi.c

 @brief NPI Transport Layer Module for SPI

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
#include "icall.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include "npi_tl.h"
#include "npi_tl_spi.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

//*****************************************************************************
// Defines
//*****************************************************************************

#define SPI_TX_FIELD_LEN                        NPI_SPI_HDR_LEN
#define SPI_TX_HDR_LEN                          3
#define SPI_TX_ZERO_PAD_INDEX                   0
#define SPI_TX_SOF_INDEX                        1
#define SPI_TX_LEN_INDEX                        2

//! \brief NPI SPI Message Indexes and Constants
//
#define NPI_SPI_MSG_LEN_LSB_IDX                     0x01
#define NPI_SPI_MSG_LEN_MSB_IDX                     0x02
#define NPI_SPI_MSG_HDR_LEN                         0x05
#define NPI_SPI_MSG_HDR_NOSOF_LEN                   0x04
#define NPI_SPI_MSG_SOF                             0xFE
#define NPI_SPI_MSG_SOF_IDX                         0x00
#define ZERO_PAD                                    1

//*****************************************************************************
// Typedefs
//*****************************************************************************

//*****************************************************************************
// Globals
//*****************************************************************************

//! \brief Handle for SPI object used by SPI driver
static SPI_Handle spiHandle;

//! \brief Structure that defines parameters of one SPI transaction
static SPI_Transaction spiTransaction;

//! \brief NPI TL call back function for the end of a SPI transaction
static npiCB_t npiTransmitCB = NULL;

//! \brief Length of bytes to send from NPI TL Tx Buffer
static uint16_t tlWriteLen = 0;

//! \brief NPI Transport Layer Buffer variables defined in npi_tl.c
extern uint8_t * npiRxBuf;
extern uint8_t * npiTxBuf;
extern uint16_t npiBufSize;

//! \brief Flag signalling receive in progress
static uint8 RxActive = FALSE;

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

//! \brief Call back function invoked at the end of a SPI transaction
static void NPITLSPI_CallBack(SPI_Handle handle, SPI_Transaction  *objTransaction);

//! \brief Calculate FCS field of SPI Transaction frame
static uint8_t NPITLSPI_calcFCS(uint8_t *buf, uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device.
//!
//! \param[in]  portID      ID value for board specific SPI port
//! \param[in]  portParams  Parameters used to initialize SPI port
//! \param[in]  npiCBack    Transport Layer call back function
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_openTransport(uint8_t portID, SPI_Params *portParams,
                            npiCB_t npiCBack)
{
    npiTransmitCB = npiCBack;

    // Initialize spi driver
    SPI_init();

    // Add call backs SPI parameters.
    portParams->transferMode = SPI_MODE_CALLBACK;
    portParams->transferCallbackFxn = NPITLSPI_CallBack;

    // Attempt to open SPI
    spiHandle = SPI_open(portID, portParams);
}

// -----------------------------------------------------------------------------
//! \brief      This routine closes Transport Layer port
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_closeTransport(void)
{
    SPI_close(spiHandle);
}

// -----------------------------------------------------------------------------
//! \brief      This routine stops any pending reads
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_stopTransfer(void)
{
    SPI_transferCancel(spiHandle);
}

// -----------------------------------------------------------------------------
//! \brief      This routine is called from the application context when REM RDY
//!             is de-asserted
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_handleRemRdyEvent(void)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    // If write has not be set up then a read must occur during this
    // transaction. There is a possibility that after bidirectional
    // transaction there is an extra MRDY event. This event
    // could cause a double read (which would clear the RX Buffer) so this
    // check ignores the MRDY event if Rx is already in progress
    if (!tlWriteLen && !RxActive)
    {
      NPITLSPI_readTransport();
    }

    ICall_leaveCriticalSection(key);
}

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on transmission completion
//!
//! \param[in]  handle - handle to the SPI port
//! \param[in]  objTransaction    - handle for SPI transmission
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITLSPI_CallBack(SPI_Handle handle, SPI_Transaction *objTransaction)
{
    uint16_t i;
    uint16_t readLen = 0;
    uint16_t storeWriteLen;

    // Check if a valid packet was found
    // SOF:
    if (npiRxBuf[NPI_SPI_MSG_SOF_IDX] == NPI_SPI_MSG_SOF &&
            objTransaction->count)
    {
        // Length:
        readLen = npiRxBuf[NPI_SPI_MSG_LEN_LSB_IDX];
        readLen += ((uint16)npiRxBuf[NPI_SPI_MSG_LEN_MSB_IDX] << 8);
        readLen += NPI_SPI_MSG_HDR_NOSOF_LEN; // Include the header w/o SOF

        if ( readLen > npiBufSize)
        {
            // the frame we receive is bigger than our buffer...
            // Discard it...
            readLen = 0;
        }
        else
        {
            // FCS:
            if (npiRxBuf[readLen + 1] ==
                NPITLSPI_calcFCS(&npiRxBuf[NPI_SPI_MSG_LEN_LSB_IDX],readLen))
            {
                // Message is valid. Shift bytes to remove SOF
                for (i = 0 ; i < readLen; i++)
                {
                    npiRxBuf[i] = npiRxBuf[i + 1];
                }
            }
            else
            {
                // Invalid FCS. Discard message
                readLen = 0;
            }
        }
    }

    //All bytes in TxBuf must be sent by this point
    storeWriteLen = tlWriteLen;
    tlWriteLen = 0;
    RxActive = FALSE;

    if (npiTransmitCB)
    {
        npiTransmitCB(readLen,storeWriteLen);
    }
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer
//!             and places it into the buffer.
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITLSPI_readTransport(void)
{
    ICall_CSState key;
    key = ICall_enterCriticalSection();

    tlWriteLen = 0;
    RxActive = TRUE;

    // Clear DMA Rx buffer and clear extra Tx buffer bytes to ensure clean buffer
    //    for next RX/TX
    memset(npiRxBuf, 0, npiBufSize);
    memset(&npiTxBuf[tlWriteLen], 0, npiBufSize - tlWriteLen);

    // set up the SPI Transaction
    spiTransaction.txBuf = npiTxBuf;
    spiTransaction.rxBuf = npiRxBuf;
    spiTransaction.count = npiBufSize;
    SPI_transfer(spiHandle, &spiTransaction);

    ICall_leaveCriticalSection(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine initializes and begins a SPI transaction
//!
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint16_t - number of bytes written to transport
// -----------------------------------------------------------------------------
uint16_t NPITLSPI_writeTransport(uint16_t len)
{
    int16 i = 0;
    ICall_CSState key;

    key = ICall_enterCriticalSection();

    // Shift all bytes in TX Buffer up ZERO_PAD indexes. SPI Driver clips off
    // first byte
    // NPI TL already shifts bytes up 1 index for SOF before calling write()
    for (i = len + 1; i > 0; i--)
    {
        npiTxBuf[i + ZERO_PAD] = npiTxBuf[i];
    }

    // Clear zero pad bytes
    memset(npiTxBuf, 0, ZERO_PAD);

    npiTxBuf[NPI_SPI_MSG_SOF_IDX + ZERO_PAD] = NPI_SPI_MSG_SOF;
    npiTxBuf[len + ZERO_PAD + 1] = NPITLSPI_calcFCS((uint8_t *)&npiTxBuf[2],len);
    tlWriteLen = len + ZERO_PAD + 2; // 2 = SOF + FCS

    // Clear DMA Rx buffer and clear extra Tx buffer bytes to ensure clean buffer
    //    for next RX/TX
    memset(npiRxBuf, 0, npiBufSize);
    memset(&npiTxBuf[tlWriteLen], 0, npiBufSize - tlWriteLen);

    // set up the SPI Transaction
    spiTransaction.count = npiBufSize;
    spiTransaction.txBuf = npiTxBuf;
    spiTransaction.rxBuf = npiRxBuf;

    // Check to see if transport is successful. If not, reset TxBufLen to allow
    // another write to be processed
    if (!SPI_transfer(spiHandle, &spiTransaction))
    {
      tlWriteLen = 0;
    }

    ICall_leaveCriticalSection(key);

    return(len);
}

// ----------------------------------------------------------------------------
//! \brief      Calculate the FCS of a message buffer by XOR'ing each byte.
//!             Remember to NOT include SOP and FCS fields, so start at the CMD
//!             field.
//!
//! \param[in]  msg_ptr   message pointer
//! \param[in]  len       length of message
//!
//! \return     uint8_t   fcs value
// ----------------------------------------------------------------------------
uint8_t NPITLSPI_calcFCS(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t fcs = 0;

    for (i = 0; i < len; i++)
    {
        fcs ^= buf[i];
    }

    return(fcs);
}
