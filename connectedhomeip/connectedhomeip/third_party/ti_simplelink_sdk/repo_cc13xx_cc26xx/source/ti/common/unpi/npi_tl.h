/******************************************************************************

 @file  npi_tl.h

 @brief NPI Transport Layer API

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

#ifndef NPI_TL_H
#define NPI_TL_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Includes
//*****************************************************************************

#include "inc/npi_data.h"
#include "hal_types.h"

//*****************************************************************************
// Defines
//*****************************************************************************

#if defined(NPI_USE_UART)
#define transportOpen NPITLUART_openTransport
#define transportClose NPITLUART_closeTransport
#define transportRead NPITLUART_readTransport
#define transportWrite NPITLUART_writeTransport
#define transportStopTransfer NPITLUART_stopTransfer
#define transportRemRdyEvent NPITLUART_handleRemRdyEvent
#elif defined(NPI_USE_SPI)
#define transportOpen NPITLSPI_openTransport
#define transportClose NPITLSPI_closeTransport
#define transportRead NPITLSPI_readTransport
#define transportWrite NPITLSPI_writeTransport
#define transportStopTransfer NPITLSPI_stopTransfer
#define transportRemRdyEvent NPITLSPI_handleRemRdyEvent
#endif //NPI_USE_UART

//*****************************************************************************
// Typedefs
//*****************************************************************************

//! \brief      Typedef for call back function mechanism to notify NPI Task that
//!             an NPI transaction has occurred
typedef void (*npiRtosCB_t)(uint16_t sizeRx, uint16_t sizeTx);

//! \brief      Typedef for call back function mechanism to notify NPI Task that
//!             an Remote Ready edge has occurred
typedef void (*npiMrdyRtosCB_t)(uint8_t state);

//! \brief      Struct for transport layer call backs
typedef struct
{
  npiMrdyRtosCB_t remRdyCB;
  npiRtosCB_t     transCompleteCB;
} npiTLCallBacks;

typedef struct
{
  uint16_t              npiTLBufSize;   //!< Buffer size of Tx/Rx Transport layer buffers
  uint32_t              mrdyPinID;      //!< Pin ID Mrdy (only with Power Saving enabled)
  uint32_t              srdyPinID;      //!< Pin ID Srdy (only with Power Saving enabled)
  uint8_t               portType;       //!< NPI_SERIAL_TYPE_[UART,SPI]
  uint8_t               portBoardID;    //!< Board ID for HW, i.e. CC2650_UART0
  npiInterfaceParams    portParams;     //!< Params to initialize NPI port
  npiTLCallBacks        npiCallBacks;   //!< Call backs to NPI Task
} NPITL_Params;

//*****************************************************************************
// Globals
//*****************************************************************************

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device. Note that based on project defines, either the
//!             UART, or SPI driver can be used.
//!
//! \param[in]  params - Transport Layer parameters
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_openTL(NPITL_Params *params);

// -----------------------------------------------------------------------------
//! \brief      This routine closes the transport layer
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_closeTL(void);

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer based on len,
//!             and places it into the buffer.
//!
//! \param[out] buf - Pointer to buffer to place read data.
//! \param[in]  len - Number of bytes to read.
//!
//! \return     uint16_t - the number of bytes read from transport
// -----------------------------------------------------------------------------
uint16_t NPITL_readTL(uint8_t *buf, uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      This routine writes data from the buffer to the transport layer.
//!
//! \param[in]  buf - Pointer to buffer to write data from.
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint16_t - NPI Error Code value
// -----------------------------------------------------------------------------
uint8_t NPITL_writeTL(uint8_t *buf, uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      This routine writes data from the buffer to the transport layer
//!             and bypasses flow control and len check. Used for emergency
//!             assert transmission if device is failing.
//!
//! \param[in]  buf - Pointer to buffer to write data from.
//! \param[in]  len - Number of bytes to write. Must be lest the txBuff size
//!
//! \return     None
// -----------------------------------------------------------------------------
void NPITL_writeBypassSafeTL(uint8_t *buf, uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      This routine is used to handle an Rem RDY edge from the app
//!             context. Certain operations such as UART_read() cannot be
//!             performed from the actual hwi handler
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_handleRemRdyEvent(void);

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size receive buffer.
//!
//! \return     uint16_t - max size of the receive buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxRxBufSize(void);

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size transmit buffer.
//!
//! \return     uint16_t - max size of the transmit buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxTxBufSize(void);

// -----------------------------------------------------------------------------
//! \brief      Returns number of bytes that are unread in RxBuf
//!
//! \return     uint16_t - number of unread bytes
// -----------------------------------------------------------------------------
uint16_t NPITL_getRxBufLen(void);

// -----------------------------------------------------------------------------
//! \brief      This routine returns the state of transmission on NPI
//!
//! \return     bool - state of NPI transmission - 1 - active, 0 - not active
// -----------------------------------------------------------------------------
bool NPITL_checkNpiBusy(void);

/*******************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif /* NPI_TL_H */
