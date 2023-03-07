/******************************************************************************

 @file  npi_tl.c

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

//*****************************************************************************
// Includes
//*****************************************************************************
#include <string.h>
#include <xdc/std.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "hal_types.h"

#include "npi_tl.h"
#include "npi_data.h"
#include "npi_util.h"

//*****************************************************************************
// Defines
//*****************************************************************************

#if defined(NPI_USE_SPI)
#include "inc/npi_tl_spi.h"
#elif defined(NPI_USE_UART)
#include "inc/npi_tl_uart.h"
#else
#error "Must define an underlying serial bus for NPI"
#endif //NPI_USE_SPI

#if (NPI_FLOW_CTRL == 1)
// Indexes for pin configurations in PIN_Config array
#define REM_RDY_PIN_IDX      0
#define LOC_RDY_PIN_IDX      1

#define LocRDY_ENABLE()      PIN_setOutputValue(hNpiHandshakePins, locRdyPIN, 0)
#define LocRDY_DISABLE()     PIN_setOutputValue(hNpiHandshakePins, locRdyPIN, 1)
#else
#define LocRDY_ENABLE()
#define LocRDY_DISABLE()
#endif // NPI_FLOW_CTRL = 1

//*****************************************************************************
// Typedefs
//*****************************************************************************

//*****************************************************************************
// Globals
//*****************************************************************************

//! \brief Flag for low power mode
static volatile bool npiPMSetConstraint = FALSE;

//! \brief Flag for ongoing NPI TX
static volatile bool npiTxActive = FALSE;
static volatile bool npiRxActive = FALSE;

//! \brief The packet that was being sent when MRDY HWI negedge was received
static volatile uint32_t mrdyPktStamp = 0;

//! \brief Packets transmitted counter
static uint32_t txPktCount = 0;

//! \brief NPI Transport Layer receive buffer
uint8_t *npiRxBuf;

//! \brief Index to last byte written into NPI Transport Layer receive buffer
static uint16_t npiRxBufTail = 0;

//! \brief Index to first byte to read from NPI Transport Layer receive buffer
static uint16_t npiRxBufHead = 0;

//! \brief NPI Transport Layer transmit buffer
uint8_t *npiTxBuf;

//! \brief Number of bytes in NPI Transport Layer transmit buffer
static uint16_t npiTxBufLen = 0;

//! \brief Size of allocated Tx and Rx buffers
uint16_t npiBufSize = 0;

npiTLCallBacks taskCBs;

#if (NPI_FLOW_CTRL == 1)
//! \brief PIN Config for Mrdy and Srdy signals without PIN IDs
static PIN_Config npiHandshakePinsCfg[] =
{
    PIN_GPIO_OUTPUT_DIS | PIN_INPUT_EN | PIN_PULLUP,
    PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint32_t remRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);
static uint32_t locRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);

//! \brief PIN State for remRdy and locRdy signals
static PIN_State npiHandshakePins;

//! \brief PIN Handles for remRdy and locRdy signals
static PIN_Handle hNpiHandshakePins;

//! \brief No way to detect whether positive or negative edge with PIN Driver
//!        Use a flag to keep track of state
static uint8_t remRdy_state;
#endif // NPI_FLOW_CTRL = 1

//*****************************************************************************
// function prototypes
//*****************************************************************************

//! \brief Call back function provided to underlying serial interface to be
//         invoked upon the completion of a transmission
static void NPITL_transmissionCallBack(uint16_t Rxlen, uint16_t Txlen);

#if (NPI_FLOW_CTRL == 1)
//! \brief HWI interrupt function for remRdy
static void NPITL_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId);

//! \brief This routine is used to set constraints on power manager
static void NPITL_setPM(void);

//! \brief This routine is used to release constraints on power manager
static void NPITL_relPM(void);
#endif // NPI_FLOW_CTRL = 1

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer and opens the port
//!             of the device. Note that based on project defines, either the
//!             UART, or SPI driver can be used.
//!
//! \param[in]  params - Transport Layer parameters
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_openTL(NPITL_Params *params)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    // Set NPI Task Call backs
    memcpy(&taskCBs, &params->npiCallBacks, sizeof(params->npiCallBacks));

    // Allocate memory for Transport Layer Tx/Rx buffers
    npiBufSize = params->npiTLBufSize;
    npiRxBuf = NPIUtil_malloc(params->npiTLBufSize);
    memset(npiRxBuf, 0, npiBufSize);
    npiTxBuf = NPIUtil_malloc(params->npiTLBufSize);
    memset(npiTxBuf, 0, npiBufSize);

    // This will be updated to be able to select SPI/UART TL at runtime
    // Now only compile time with the NPI_USE_[UART,SPI] flag
#if defined(NPI_USE_UART)
    transportOpen(params->portBoardID,
                  &params->portParams.uartParams,
                  NPITL_transmissionCallBack);
#elif defined(NPI_USE_SPI)
    transportOpen(params->portBoardID,
                  &params->portParams.spiParams,
                  NPITL_transmissionCallBack);
#endif //NPI_USE_UART

#if (NPI_FLOW_CTRL == 1)
    // Assign PIN IDs to remRdy and locRrdy
#ifdef NPI_MASTER
    remRdyPIN = (params->srdyPinID & IOC_IOID_MASK);
    locRdyPIN = (params->mrdyPinID & IOC_IOID_MASK);
#else
    remRdyPIN = (params->mrdyPinID & IOC_IOID_MASK);
    locRdyPIN = (params->srdyPinID & IOC_IOID_MASK);
#endif //NPI_MASTER

    // Add PIN IDs to PIN Configuration
    npiHandshakePinsCfg[REM_RDY_PIN_IDX] |= remRdyPIN;
    npiHandshakePinsCfg[LOC_RDY_PIN_IDX] |= locRdyPIN;

    // Initialize LOCRDY/REMRDY. Enable int after callback registered
    hNpiHandshakePins = PIN_open(&npiHandshakePins, npiHandshakePinsCfg);
    PIN_registerIntCb(hNpiHandshakePins, NPITL_remRdyPINHwiFxn);
    PIN_setConfig(hNpiHandshakePins,
                  PIN_BM_IRQ,
                  remRdyPIN | PIN_IRQ_BOTHEDGES);

    // Enable wakeup
    PIN_setConfig(hNpiHandshakePins,
                  PINCC26XX_BM_WAKEUP,
                  remRdyPIN | PINCC26XX_WAKEUP_NEGEDGE);

    remRdy_state = PIN_getInputValue(remRdyPIN);

    // If MRDY is already low then we must initiate a read because there was
    // a prior MRDY negedge that was missed
    if (!remRdy_state)
    {
        NPITL_setPM();
        if (taskCBs.remRdyCB)
        {
            transportRemRdyEvent();
            LocRDY_ENABLE();
        }
    }
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 0)
    // This call will start repeated Uart Reads when Power Savings is disabled
    transportRead();
#endif // NPI_FLOW_CTRL = 0

    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine closes the transport layer
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_closeTL(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    // Clear NPI Task Call backs
    memset(&taskCBs, 0, sizeof(taskCBs));

    // Free Transport Layer RX/TX buffers
    npiBufSize = 0;
    NPIUtil_free(npiRxBuf);
    NPIUtil_free(npiTxBuf);

    // Close Transport Layer
    transportClose();

#if (NPI_FLOW_CTRL == 1)
    // Clear mrdy and srdy PIN IDs
    remRdyPIN = (IOID_UNUSED & IOC_IOID_MASK); // Set to 0x000000FF
    locRdyPIN = (IOID_UNUSED & IOC_IOID_MASK); // Set to 0x000000FF

    // Clear PIN IDs from PIN Configuration
    npiHandshakePinsCfg[REM_RDY_PIN_IDX] &= ~remRdyPIN;
    npiHandshakePinsCfg[LOC_RDY_PIN_IDX] &= ~locRdyPIN;

    // Close PIN Handle
    PIN_close(hNpiHandshakePins);

    // Release Power Management
    NPITL_relPM();
#endif // NPI_FLOW_CTRL = 1

    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine returns the state of transmission on NPI
//!
//! \return     bool - state of NPI transmission - 1 - active, 0 - not active
// -----------------------------------------------------------------------------
bool NPITL_checkNpiBusy(void)
{
#if (NPI_FLOW_CTRL == 1)
#ifdef NPI_MASTER
    return !PIN_getOutputValue(locRdyPIN) || npiRxActive;
#else
    return !PIN_getOutputValue(locRdyPIN);
#endif //NPI_MASTER
#else
    return npiTxActive;
#endif // NPI_FLOW_CTRL = 1
}

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine is used to set constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_setPM(void)
{
    if (npiPMSetConstraint)
    {
        return;
    }

    // set constraints for Standby and idle mode
    Power_setConstraint(PowerCC26XX_SB_DISALLOW);
    Power_setConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
    npiPMSetConstraint = TRUE;
}
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine is used to release constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_relPM(void)
{
    if (!npiPMSetConstraint)
    {
        return;
    }

    // release constraints for Standby and idle mode
    Power_releaseConstraint(PowerCC26XX_SB_DISALLOW);
    Power_releaseConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
    npiPMSetConstraint = FALSE;
}
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This routine is used to handle an MRDY transition from a task
//!             context. Certain operations such as UART_read() cannot be
//!             performed from a HWI context
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_handleRemRdyEvent(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();

    // Check to make sure this event is not occurring during the next packet
    // transmission
    if (PIN_getInputValue(remRdyPIN) == 0 ||
        (npiTxActive && mrdyPktStamp == txPktCount))
    {
        transportRemRdyEvent();
        npiRxActive = TRUE;
        LocRDY_ENABLE();
    }

    NPIUtil_ExitCS(key);
}
#endif // NPI_FLOW_CTRL = 1

#if (NPI_FLOW_CTRL == 1)
// -----------------------------------------------------------------------------
//! \brief      This is a HWI function handler for the MRDY pin. Some MRDY
//!             functionality can execute from this HWI context. Others
//!             must be executed from task context hence the taskCBs.remRdyCB()
//!
//! \param[in]  hPin - PIN Handle
//! \param[in]  pinId - ID of pin that triggered HWI
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId)
{
    // The pin driver does not currently support returning whether the int
    // was neg or pos edge so we must use a variable to keep track of state.
    remRdy_state ^= 1;

    if (remRdy_state == 0)
    {
        mrdyPktStamp = txPktCount;
        NPITL_setPM();
    }
    else
    {
        transportStopTransfer();
        npiRxActive = FALSE;
    }

    // Signal to registered task that Rem Ready signal has changed state
    if (taskCBs.remRdyCB)
    {
        taskCBs.remRdyCB(remRdy_state);
    }

    // Check the physical state of the pin to see if it matches the variable
    // state. If not trigger another task call back
    if (remRdy_state != PIN_getInputValue(remRdyPIN))
    {
        remRdy_state = PIN_getInputValue(remRdyPIN);

        if (taskCBs.remRdyCB)
        {
            taskCBs.remRdyCB(remRdy_state);
        }
    }
}
#endif // NPI_FLOW_CTRL = 1

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on the completion of one transmission
//!             to/from the host MCU. Any bytes receives will be [0,Rxlen) in
//!             npiRxBuf.
//!             If bytes were receives or transmitted, this function notifies
//!             the NPI task via registered call backs
//!
//! \param[in]  Rxlen   - length of the data received
//! \param[in]  Txlen   - length of the data transferred
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_transmissionCallBack(uint16_t Rxlen, uint16_t Txlen)
{
    npiRxBufHead = 0;
    npiRxBufTail = Rxlen;

    // Only set TX Active flag false if a tx was taking place
    if (Txlen > 0)
    {
      npiTxActive = FALSE;
    }

    // If Task is registered, invoke transaction complete callback
    if (taskCBs.transCompleteCB)
    {
        taskCBs.transCompleteCB(Rxlen, Txlen);
    }

#if (NPI_FLOW_CTRL == 1)
    NPITL_relPM();
    LocRDY_DISABLE();
#endif // NPI_FLOW_CTRL = 1
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer based on len,
//!             and places it into the buffer.
//!
//! \param[in]  buf - Pointer to buffer to place read data.
//! \param[in]  len - Number of bytes to read.
//!
//! \return     uint16_t - the number of bytes read from transport
// -----------------------------------------------------------------------------
uint16_t NPITL_readTL(uint8_t *buf, uint16_t len)
{
    // Only copy the lowest number between len and bytes remaining in buffer
    len = (len > NPITL_getRxBufLen()) ? NPITL_getRxBufLen() : len;

    memcpy(buf, &npiRxBuf[npiRxBufHead], len);
    npiRxBufHead += len;

    return len;
}

// -----------------------------------------------------------------------------
//! \brief      This routine writes data from the buffer to the transport layer.
//!
//! \param[in]  buf - Pointer to buffer to write data from.
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint16_t - NPI error code value
// -----------------------------------------------------------------------------
uint8_t NPITL_writeTL(uint8_t *buf, uint16_t len)
{
#if (NPI_FLOW_CTRL == 1)
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
#endif // NPI_FLOW_CTRL = 1

    // Check to make sure NPI is not currently in a transaction
    if (NPITL_checkNpiBusy())
    {
#if (NPI_FLOW_CTRL == 1)
        NPIUtil_ExitCS(key);
#endif // NPI_FLOW_CTRL = 1

        return NPI_BUSY;
    }

    // Check to make sure that write size is not greater than what is
    // allowed
    if (len > npiBufSize)
    {
#if (NPI_FLOW_CTRL == 1)
        NPIUtil_ExitCS(key);
#endif // NPI_FLOW_CTRL = 1

        return NPI_TX_MSG_OVERSIZE;
    }

    // Copy into the second byte of npiTxBuf. This will save Serial Port
    // Specific TL code from having to shift one byte later on for SOF.
    memcpy(&npiTxBuf[1], buf, len);
    npiTxBufLen = len;
    npiTxActive = TRUE;
    txPktCount++;

    transportWrite(npiTxBufLen);

#if (NPI_FLOW_CTRL == 1)
    LocRDY_ENABLE();
    NPIUtil_ExitCS(key);
#endif // NPI_FLOW_CTRL = 1

    return NPI_SUCCESS;
}

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
void NPITL_writeBypassSafeTL(uint8_t *buf, uint16_t len)
{
#if (NPI_FLOW_CTRL == 1)
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
#endif // NPI_FLOW_CTRL = 1

    // Copy into the second byte of npiTxBuf. This will save Serial Port
    // Specific TL code from having to shift one byte later on for SOF.
    memcpy(&npiTxBuf[1], buf, len);
    npiTxBufLen = len;
    npiTxActive = TRUE;
    txPktCount++;

    transportWrite(npiTxBufLen);

#if (NPI_FLOW_CTRL == 1)
    LocRDY_ENABLE();
    NPIUtil_ExitCS(key);
#endif // NPI_FLOW_CTRL = 1
}

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size receive buffer.
//!
//! \return     uint16_t - max size of the receive buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxRxBufSize(void)
{
    return(npiBufSize);
}

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size transmit buffer.
//!
//! \return     uint16_t - max size of the transmit buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxTxBufSize(void)
{
    return(npiBufSize);
}

// -----------------------------------------------------------------------------
//! \brief      Returns number of bytes that are unread in RxBuf
//!
//! \return     uint16_t - number of unread bytes
// -----------------------------------------------------------------------------
uint16_t NPITL_getRxBufLen(void)
{
    return ((npiRxBufTail - npiRxBufHead) + npiBufSize) % npiBufSize;
}
