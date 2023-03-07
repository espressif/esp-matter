/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/drivers/uart/UARTCC26X2.h>
#include <ti/drivers/GPIO.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/uart.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/aon_ioc.h)

/* Size of the TX and RX FIFOs is 32 items */
#define FIFO_SIZE 32

#define READTIMEDOUT 0x10
#define UART_BE_PE_FE 0x700  /* Break, parity, or framing error */

#define UARTCC26X2_RXERROR (UART_RXERROR_OVERRUN | UART_RXERROR_BREAK | \
                            UART_RXERROR_PARITY | UART_RXERROR_FRAMING)

/* UARTCC26X2 functions */
void         UARTCC26X2_close(UART_Handle handle);
int_fast16_t UARTCC26X2_control(UART_Handle handle, uint_fast16_t cmd,
                               void *arg);
void         UARTCC26X2_init(UART_Handle handle);
UART_Handle  UARTCC26X2_open(UART_Handle handle, UART_Params *params);
int_fast32_t UARTCC26X2_read(UART_Handle handle, void *buffer, size_t size);
int_fast32_t UARTCC26X2_readPolling(UART_Handle handle, void *buf,
                                   size_t size);
int_fast32_t UARTCC26X2_readPollingNotImpl(UART_Handle handle, void *buf,
                                   size_t size);
void         UARTCC26X2_readCancel(UART_Handle handle);
int_fast32_t UARTCC26X2_write(UART_Handle handle, const void *buffer,
                            size_t size);
int_fast32_t UARTCC26X2_writePolling(UART_Handle handle, const void *buf,
                                   size_t size);
int_fast32_t UARTCC26X2_writePollingNotImpl(UART_Handle handle,
                                   const void *buf, size_t size);
void         UARTCC26X2_writeCancel(UART_Handle handle);

/* Static functions */
static void UARTCC26X2_hwiIntFxn(uintptr_t arg);
static void disableRX(UART_Handle handle);
static void enableRX(UART_Handle handle);
static uint_fast16_t getPowerMgrId(uint32_t baseAddr);
static void initHw(UART_Handle handle);
static void finalizeIO(UART_Handle handle);
static void initIO(UART_Handle handle);
static int  postNotifyFxn(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg);
static void readBlockingTimeout(uintptr_t arg);
static void readIsr(UART_Handle handle, uint32_t status);
static void readSemCallback(UART_Handle handle, void *buffer, size_t count);
static int readTaskBlocking(UART_Handle handle);
static int readTaskCallback(UART_Handle handle);
static int ringBufGet(UART_Handle handle, unsigned char *data);
static void swiReadCallback(uintptr_t arg0, uintptr_t arg1);
static void swiWriteCallback(uintptr_t arg0, uintptr_t arg1);
static void writeData(UART_Handle handle);
static void writeSemCallback(UART_Handle handle, void *buffer, size_t count);

/*
 * Function for checking whether flow control is enabled.
 */
static inline bool isFlowControlEnabled(UARTCC26X2_HWAttrs const  *hwAttrs) {
    return ((hwAttrs->flowControl == UARTCC26X2_FLOWCTRL_HARDWARE) &&
            (hwAttrs->ctsPin != GPIO_INVALID_INDEX) && (hwAttrs->rtsPin != GPIO_INVALID_INDEX));
}

/* UART function table for UARTCC26X2 implementation */
const UART_FxnTable UARTCC26X2_fxnTable = {
    UARTCC26X2_close,
    UARTCC26X2_control,
    UARTCC26X2_init,
    UARTCC26X2_open,
    UARTCC26X2_read,
    UARTCC26X2_readPollingNotImpl,
    UARTCC26X2_readCancel,
    UARTCC26X2_write,
    UARTCC26X2_writePollingNotImpl,
    UARTCC26X2_writeCancel
};

static const uint32_t dataLength[] = {
    UART_CONFIG_WLEN_5,     /* UART_LEN_5 */
    UART_CONFIG_WLEN_6,     /* UART_LEN_6 */
    UART_CONFIG_WLEN_7,     /* UART_LEN_7 */
    UART_CONFIG_WLEN_8      /* UART_LEN_8 */
};

static const uint32_t stopBits[] = {
    UART_CONFIG_STOP_ONE,   /* UART_STOP_ONE */
    UART_CONFIG_STOP_TWO    /* UART_STOP_TWO */
};

static const uint32_t parityType[] = {
    UART_CONFIG_PAR_NONE,   /* UART_PAR_NONE */
    UART_CONFIG_PAR_EVEN,   /* UART_PAR_EVEN */
    UART_CONFIG_PAR_ODD,    /* UART_PAR_ODD */
    UART_CONFIG_PAR_ZERO,   /* UART_PAR_ZERO */
    UART_CONFIG_PAR_ONE     /* UART_PAR_ONE */
};

/*
 *  Array for mapping of FIFO threshold defines selected by board files to
 *  TX FIFO threshold define used by driverlib FIFO threshold defines
 *  (enum UARTCC26X2_FifoThreshold) must be used as the array index.
 *  Index 0 handles backward compatibility with legacy board files that
 *  don't select any FIFO thresholds.
 */
static const uint8_t txFifoThreshold[6] = {
    UART_FIFO_TX1_8, /* UARTCC26X2_FIFO_THRESHOLD_DEFAULT */
    UART_FIFO_TX1_8, /* UARTCC26X2_FIFO_THRESHOLD_1_8     */
    UART_FIFO_TX2_8, /* UARTCC26X2_FIFO_THRESHOLD_2_8     */
    UART_FIFO_TX4_8, /* UARTCC26X2_FIFO_THRESHOLD_4_8     */
    UART_FIFO_TX6_8, /* UARTCC26X2_FIFO_THRESHOLD_6_8     */
    UART_FIFO_TX7_8  /* UARTCC26X2_FIFO_THRESHOLD_7_8     */
};

/*
 *  Array for mapping of FIFO threshold defines selected by board files to
 *  RX FIFO threshold define used by driverlib FIFO threshold defines
 *  (enum UARTCC26X2_FifoThreshold) must be used as the array index.
 *  Index 0 handles backward compatibility with legacy board files that
 *  don't select any FIFO thresholds.
 */
static const uint8_t rxFifoThreshold[6] = {
    UART_FIFO_RX4_8, /* UARTCC26X2_FIFO_THRESHOLD_DEFAULT */
    UART_FIFO_RX1_8, /* UARTCC26X2_FIFO_THRESHOLD_1_8     */
    UART_FIFO_RX2_8, /* UARTCC26X2_FIFO_THRESHOLD_2_8     */
    UART_FIFO_RX4_8, /* UARTCC26X2_FIFO_THRESHOLD_4_8     */
    UART_FIFO_RX6_8, /* UARTCC26X2_FIFO_THRESHOLD_6_8     */
    UART_FIFO_RX7_8  /* UARTCC26X2_FIFO_THRESHOLD_7_8     */
};

/*
 *  Array for mapping of FIFO threshold defines selected by board files to
 *  number of bytes in the RX FIFO threshold.
 */
static const uint8_t rxFifoBytes[6] = {
    16,   /* UARTCC26X2_FIFO_THRESHOLD_DEFAULT */
     4,   /* UARTCC26X2_FIFO_THRESHOLD_1_8     */
     8,   /* UARTCC26X2_FIFO_THRESHOLD_2_8     */
    16,   /* UARTCC26X2_FIFO_THRESHOLD_4_8     */
    24,   /* UARTCC26X2_FIFO_THRESHOLD_6_8     */
    28    /* UARTCC26X2_FIFO_THRESHOLD_7_8     */
};

/*
 *  ======== UARTCC26X2_close ========
 */
void UARTCC26X2_close(UART_Handle handle)
{
    UARTCC26X2_Object            *object = handle->object;
    UARTCC26X2_HWAttrs const     *hwAttrs = handle->hwAttrs;

    /* Disable UART and interrupts. */
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX | UART_INT_RX |
            UART_INT_RT | UART_INT_OE | UART_INT_BE | UART_INT_PE |
            UART_INT_FE | UART_INT_CTS);

    /* Set to false to allow UARTCC26X2_readCancel() to release constraint */
    object->state.ctrlRxEnabled = false;

    object->state.opened = false;

    /* Cancel any possible ongoing reads/writes */
    UARTCC26X2_writeCancel(handle);
    UARTCC26X2_readCancel(handle);

    /*
     *  Disable the UART.  Do not call driverlib function
     *  UARTDisable() since it polls for BUSY bit to clear
     *  before disabling the UART FIFO and module.
     */
    /* Disable UART FIFO */
    HWREG(hwAttrs->baseAddr + UART_O_LCRH) &= ~(UART_LCRH_FEN);
    /* Disable UART module */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_UARTEN | UART_CTL_TXE |
                                      UART_CTL_RXE);

    HwiP_destruct(&(object->hwi));
    if (object->state.writeMode == UART_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->writeSem));
    }
    if (object->state.readMode == UART_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->readSem));
        ClockP_destruct(&(object->timeoutClk));
    }
    SwiP_destruct(&(object->readSwi));
    SwiP_destruct(&(object->writeSwi));

    /* Deallocate pins */
    finalizeIO(handle);

    /* Unregister power notification objects */
    Power_unregisterNotify(&object->postNotify);

    /* Release power dependency - i.e. potentially power down serial domain. */
    Power_releaseDependency(object->powerMgrId);

    DebugP_log1("UART:(%p) closed", hwAttrs->baseAddr);
}


/*
 *  ======== UARTCC26X2_control ========
 */
int_fast16_t UARTCC26X2_control(UART_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    UARTCC26X2_Object            *object = handle->object;
    UARTCC26X2_HWAttrs const     *hwAttrs = handle->hwAttrs;
    unsigned char                 data;
    int                           bufferCount;
    uintptr_t                     key;

    bufferCount = RingBuf_peek(&object->ringBuffer, &data);

    switch (cmd) {
        /* Common UART CMDs */
        case (UART_CMD_PEEK):
            *(int *)arg = (bufferCount) ? data : UART_ERROR;
            DebugP_log2("UART:(%p) UART_CMD_PEEK: %d", hwAttrs->baseAddr,
                *(uintptr_t*)arg);
            return (UART_STATUS_SUCCESS);

        case (UART_CMD_ISAVAILABLE):
            *(bool *)arg = (bufferCount != 0);
            DebugP_log2("UART:(%p) UART_CMD_ISAVAILABLE: %d",
                    hwAttrs->baseAddr, *(uintptr_t*)arg);
            return (UART_STATUS_SUCCESS);

        case (UART_CMD_GETRXCOUNT):
            *(int *)arg = bufferCount;
            DebugP_log2("UART:(%p) UART_CMD_GETRXCOUNT: %d", hwAttrs->baseAddr,
                    *(uintptr_t*)arg);
            return (UART_STATUS_SUCCESS);

        case (UART_CMD_RXENABLE):
            object->state.ctrlRxEnabled = true;
            enableRX(handle);
            return (UART_STATUS_SUCCESS);

        case (UART_CMD_RXDISABLE):
            object->state.ctrlRxEnabled = false;
            disableRX(handle);
            return (UART_STATUS_SUCCESS);

        /* Specific UART CMDs */
        case UARTCC26X2_CMD_RETURN_PARTIAL_ENABLE:
            /* Enable RETURN_PARTIAL */
            object->readRetPartial = true;
            return (UART_STATUS_SUCCESS);

        case UARTCC26X2_CMD_RETURN_PARTIAL_DISABLE:
            /* Disable RETURN_PARTIAL */
            object->readRetPartial = false;
            return (UART_STATUS_SUCCESS);

        case UARTCC26X2_CMD_RX_FIFO_FLUSH:
            /* Flush RX FIFO */
            /* Disable interrupts to avoid reading data while changing state */
            key = HwiP_disable();

            /* Read RX FIFO until empty */
            while (((int32_t)UARTCharGetNonBlocking(hwAttrs->baseAddr)) != -1);

            /* Reset RingBuf */
            object->ringBuffer.count = 0;
            object->ringBuffer.head = object->ringBuffer.length - 1;
            object->ringBuffer.tail = 0;

            /* Set size = 0 to prevent reading and restore interrupts. */
            object->readSize = 0;
            HwiP_restore(key);

            return (UART_STATUS_SUCCESS);

        default:
            return (UART_STATUS_UNDEFINEDCMD);
    }
}

/*
 *  ======== UARTCC26X2_hwiIntFxn ========
 *  Hwi function that processes UART interrupts.
 */
static void UARTCC26X2_hwiIntFxn(uintptr_t arg)
{
    uint32_t                    status;
    UARTCC26X2_HWAttrs const    *hwAttrs = ((UART_Handle)arg)->hwAttrs;

    /* Clear interrupts */
    status = UARTIntStatus(hwAttrs->baseAddr, true);
    UARTIntClear(hwAttrs->baseAddr, status);

    if (status & (UART_INT_RX | UART_INT_RT | UART_INT_OE | UART_INT_BE |
            UART_INT_PE | UART_INT_FE)) {
        readIsr((UART_Handle)arg, status);
    }

    if (status & (UART_INT_TX | UART_INT_EOT)) {
        writeData((UART_Handle)arg);
    }
}

/*
 *  ======== UARTCC26X2_init ========
 */
void UARTCC26X2_init(UART_Handle handle)
{
}

/*
 *  ======== UARTCC26X2_open ========
 */
UART_Handle UARTCC26X2_open(UART_Handle handle, UART_Params *params)
{
    uintptr_t                    key;
    UARTCC26X2_Object           *object = handle->object;
    UARTCC26X2_HWAttrs const    *hwAttrs = handle->hwAttrs;
    /* Use union to save on stack allocation */
    union {
        HwiP_Params     hwiParams;
        ClockP_Params   clkParams;
        SwiP_Params     swiParams;
    } paramsUnion;


    /* Check for callback when in UART_MODE_CALLBACK */
    DebugP_assert((params->readMode != UART_MODE_CALLBACK) ||
                  (params->readCallback != NULL));
    DebugP_assert((params->writeMode != UART_MODE_CALLBACK) ||
                  (params->writeCallback != NULL));

    key = HwiP_disable();

    if (object->state.opened == true) {
        HwiP_restore(key);

        DebugP_log1("UART:(%p) already in use.", hwAttrs->baseAddr);
        return (NULL);
    }
    object->state.opened = true;

    HwiP_restore(key);

    object->state.readMode       = params->readMode;
    object->state.writeMode      = params->writeMode;
    object->state.readReturnMode = params->readReturnMode;
    object->state.readDataMode   = params->readDataMode;
    object->state.writeDataMode  = params->writeDataMode;
    object->state.readEcho       = params->readEcho;
    object->readTimeout          = params->readTimeout;
    object->writeTimeout         = params->writeTimeout;
    object->readCallback         = params->readCallback;
    object->writeCallback        = params->writeCallback;
    object->baudRate             = params->baudRate;
    object->stopBits             = params->stopBits;
    object->dataLength           = params->dataLength;
    object->parityType           = params->parityType;

    /* Set UART transaction variables to defaults. */
    object->writeBuf             = NULL;
    object->readBuf              = NULL;
    object->writeCount           = 0;
    object->readCount            = 0;
    object->writeSize            = 0;
    object->readSize             = 0;
    object->status               = 0;
    object->readRetPartial       = false;
    object->state.rxEnabled      = false;
    object->state.ctrlRxEnabled  = false;
    object->state.txEnabled      = false;
    object->state.drainByISR     = false;

    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&object->ringBuffer, hwAttrs->ringBufPtr,
            hwAttrs->ringBufSize);

    /* Get the Power resource Id from the base address */
    object->powerMgrId = getPowerMgrId(hwAttrs->baseAddr);
    if (object->powerMgrId >= PowerCC26X2_NUMRESOURCES) {
        DebugP_log1("UART:(%p) Failed to determine Power resource id",
                hwAttrs->baseAddr);
        return (NULL);
    }

    /* Register power dependency - i.e. power up and enable clock for UART. */
    Power_setDependency(object->powerMgrId);

    UARTDisable(hwAttrs->baseAddr);

    /* Configure IOs, make sure it was successful */
    initIO(handle);

    /* Initialize the UART hardware module */
    initHw(handle);

    /* Register notification function */
    Power_registerNotify(&object->postNotify, PowerCC26XX_AWAKE_STANDBY,
            postNotifyFxn, (uintptr_t)handle);

    /* Create Hwi object for this UART peripheral. */
    HwiP_Params_init(&(paramsUnion.hwiParams));
    paramsUnion.hwiParams.arg = (uintptr_t)handle;
    paramsUnion.hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi), hwAttrs->intNum, UARTCC26X2_hwiIntFxn,
                  &(paramsUnion.hwiParams));

    SwiP_Params_init(&(paramsUnion.swiParams));
    paramsUnion.swiParams.arg0 = (uintptr_t)handle;
    paramsUnion.swiParams.priority = hwAttrs->swiPriority;
    SwiP_construct(&(object->readSwi), swiReadCallback, &(paramsUnion.swiParams));

    SwiP_construct(&(object->writeSwi), swiWriteCallback, &(paramsUnion.swiParams));

    /* If read mode is blocking create a semaphore and set callback. */
    if (object->state.readMode == UART_MODE_BLOCKING) {
        /* Timeout clock for reads */
        ClockP_Params_init(&(paramsUnion.clkParams));
        paramsUnion.clkParams.arg = (uintptr_t)handle;

        ClockP_construct(&(object->timeoutClk),
                (ClockP_Fxn)&readBlockingTimeout,  0 /* timeout */,
                &(paramsUnion.clkParams));

        SemaphoreP_constructBinary(&(object->readSem), 0);
        object->readCallback = &readSemCallback;
    }

    /* If write mode is blocking create a semaphore and set callback. */
    if (object->state.writeMode == UART_MODE_BLOCKING) {
        SemaphoreP_constructBinary(&(object->writeSem), 0);
        object->writeCallback = &writeSemCallback;
    }

    /* UART opened successfully */
    DebugP_log1("UART:(%p) opened", hwAttrs->baseAddr);

    /* Return the handle */
    return (handle);
}

/*
 *  ======== UARTCC26X2_read ========
 */
int_fast32_t UARTCC26X2_read(UART_Handle handle, void *buffer, size_t size)
{
    uintptr_t                 key;
    UARTCC26X2_Object        *object = handle->object;
    int32_t                   bytesRead;

    key = HwiP_disable();

    if (!object->state.opened ||
            ((object->state.readMode == UART_MODE_CALLBACK) &&
                    object->readSize)) {
        HwiP_restore(key);
        return (UART_ERROR);
    }

    /* Save the data to be read and restore interrupts. */
    object->readBuf = buffer;
    object->readSize = size;
    object->readCount = size;
    object->status = 0;  /* Clear read timeout or other errors */

    HwiP_restore(key);

    enableRX(handle);

    if (object->state.readMode == UART_MODE_CALLBACK) {
        /* Return value of readTaskCallback() should be 0 */
        bytesRead = readTaskCallback(handle);
    }
    else {
        bytesRead = readTaskBlocking(handle);
        /*
         *  Set the readCount to 0 so as not to trigger a read timeout
         *  interrupt in case more data comes in.
         */
        object->readCount = 0;
    }

    return (bytesRead);
}

/*
 *  ======== UARTCC26X2_readCancel ========
 */
void UARTCC26X2_readCancel(UART_Handle handle)
{
    uintptr_t             key;
    UARTCC26X2_Object    *object = handle->object;

    disableRX(handle);

    if ((object->state.readMode != UART_MODE_CALLBACK) ||
        (object->readSize == 0)) {
        return;
    }

    key = HwiP_disable();

    object->state.drainByISR = false;
    /*
     * Indicate that what we've currently received is what we asked for so that
     * the existing logic handles the completion.
     */
    object->readSize -= object->readCount;
    object->readCount = 0;

    HwiP_restore(key);

    /*
     *  Trigger the RX callback function, even if no data is in the
     *  buffer.  The ISR might not have been triggered yet because the
     *  FIFO trigger level has not been reached.
     */
    SwiP_post(&(object->readSwi));
}

/*
 *  ======== UARTCC26X2_readPolling ========
 */
int_fast32_t UARTCC26X2_readPolling(UART_Handle handle, void *buf, size_t size)
{
    int32_t                      count = 0;
    UARTCC26X2_Object           *object = handle->object;
    UARTCC26X2_HWAttrs const    *hwAttrs = handle->hwAttrs;
    unsigned char               *buffer = (unsigned char *)buf;
    uintptr_t                    key;

    /* Read characters. */
    while (size) {
        /* Grab data from the RingBuf before getting it from the RX data reg */
        key = HwiP_disable();
        UARTIntDisable(hwAttrs->baseAddr, UART_INT_RX | UART_INT_RT);
        HwiP_restore(key);

        if (RingBuf_get(&object->ringBuffer, buffer) == -1) {
            *buffer = UARTCharGet(hwAttrs->baseAddr);
        }

        key = HwiP_disable();
        if (object->state.rxEnabled) {
            UARTIntEnable(hwAttrs->baseAddr, UART_INT_RX | UART_INT_RT);
        }
        HwiP_restore(key);

        DebugP_log2("UART:(%p) Read character 0x%x", hwAttrs->baseAddr,
            *buffer);
        count++;
        size--;

        if (object->state.readDataMode == UART_DATA_TEXT && *buffer == '\r') {
            /* Echo character if enabled. */
            if (object->state.readEcho) {
                UARTCharPut(hwAttrs->baseAddr, '\r');
            }
            *buffer = '\n';
        }

        /* Echo character if enabled. */
        if (object->state.readDataMode == UART_DATA_TEXT &&
                object->state.readEcho) {
            UARTCharPut(hwAttrs->baseAddr, *buffer);
        }

        /* If read return mode is newline, finish if a newline was received. */
        if (object->state.readDataMode == UART_DATA_TEXT &&
                object->state.readReturnMode == UART_RETURN_NEWLINE &&
                *buffer == '\n') {
            return (count);
        }

        buffer++;
    }

    DebugP_log2("UART:(%p) Read polling finished, %d bytes read",
        hwAttrs->baseAddr, count);

    return (count);
}

/*
 *  ======== UARTCC26X2_readPollingNotImpl ========
 */
int_fast32_t UARTCC26X2_readPollingNotImpl(UART_Handle handle, void *buf,
        size_t size)
{
    /* Not supported */
    return (UART_ERROR);
}

/*
 *  ======== UARTCC26X2_write ========
 */
int_fast32_t UARTCC26X2_write(UART_Handle handle, const void *buffer,
        size_t size)
{
    UARTCC26X2_Object           *object = handle->object;
    UARTCC26X2_HWAttrs const    *hwAttrs = handle->hwAttrs;
    uintptr_t                    key;

    if (size == 0) {
        return (0);
    }

    key = HwiP_disable();

    /*
     *  Make sure any previous write has fininshed.  If TX is still
     *  enabled, then writeSwi has not yet been posted.
     */
    if (!object->state.opened || object->state.txEnabled) {
        HwiP_restore(key);
        DebugP_log1("UART:(%p) Could not write data, uart closed or in use.",
            hwAttrs->baseAddr);

        return (UART_ERROR);
    }

    /* Save the data to be written and restore interrupts. */
    object->writeBuf = buffer;
    object->writeSize = size;
    object->writeCount = size;

    object->state.txEnabled = true;

    /* Set constraints to guarantee transaction */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /* Enable TX */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_TXE;

    DebugP_log1("UART:(%p) UART_write set write power constraint",
            hwAttrs->baseAddr);

    HwiP_restore(key);

    if (!(UARTIntStatus(hwAttrs->baseAddr, false) & UART_INT_TX)) {
        /*
         *  Start the transfer going if the raw interrupt status TX bit
         *  is 0.  This will cause the ISR to fire when we enable
         *  UART_INT_TX.  If the RIS TX bit is not cleared, we don't
         *  need to call writeData(), since the ISR will fire once we
         *  enable the interrupt, causing the transfer to start.
         */
        writeData(handle);
    }
    if (object->writeCount) {
        key = HwiP_disable();
        UARTIntEnable(hwAttrs->baseAddr, UART_INT_TX);
        HwiP_restore(key);
    }

    /* If writeMode is blocking, block and get the state. */
    if (object->state.writeMode == UART_MODE_BLOCKING) {
        /* Pend on semaphore and wait for Hwi to finish. */
        if (SemaphoreP_OK != SemaphoreP_pend(&(object->writeSem),
                    object->writeTimeout)) {
            /* Semaphore timed out, make the write empty and log the write. */
            key = HwiP_disable();
            UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX | UART_INT_EOT);
            UARTIntClear(hwAttrs->baseAddr, UART_INT_TX | UART_INT_EOT);

            if (object->state.txEnabled) {

                /* Disable TX */
                HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);

                Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
                object->state.txEnabled = false;
            }
            HwiP_restore(key);

            DebugP_log2("UART:(%p) Write timed out, %d bytes written",
                hwAttrs->baseAddr, object->writeCount);
        }
        return (object->writeSize - object->writeCount);
    }

    return (0);
}

/*
 *  ======== UARTCC26X2_writeCancel ========
 */
void UARTCC26X2_writeCancel(UART_Handle handle)
{
    uintptr_t                     key;
    UARTCC26X2_Object            *object = handle->object;
    UARTCC26X2_HWAttrs const     *hwAttrs = handle->hwAttrs;

    key = HwiP_disable();

    /* Return if there is no write. */
    if (!object->state.txEnabled) {
        HwiP_restore(key);
        return;
    }

    UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX | UART_INT_EOT);
    UARTIntClear(hwAttrs->baseAddr, UART_INT_TX | UART_INT_EOT);

    /* Release constraint since transaction is done */
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
    object->state.txEnabled = false;

    /* Disable TX */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);

    HwiP_restore(key);

    object->writeCallback(handle, (void *)object->writeBuf,
        object->writeSize - object->writeCount);

    DebugP_log2("UART:(%p) Write canceled, %d bytes written",
            hwAttrs->baseAddr, object->writeSize - object->writeCount);
}

/*
 *  ======== UARTCC26X2_writePolling ========
 */
int_fast32_t UARTCC26X2_writePolling(UART_Handle handle, const void *buf,
        size_t size)
{
    int32_t                    count = 0;
    UARTCC26X2_Object         *object = handle->object;
    UARTCC26X2_HWAttrs const  *hwAttrs = handle->hwAttrs;
    unsigned char             *buffer = (unsigned char *)buf;
    uintptr_t                  key;

    /* Enable TX */
    key = HwiP_disable();
    HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_TXE;
    HwiP_restore(key);

    /* Write characters. */
    while (size) {
        if (object->state.writeDataMode == UART_DATA_TEXT && *buffer == '\n') {
            UARTCharPut(hwAttrs->baseAddr, '\r');
            count++;
        }
        UARTCharPut(hwAttrs->baseAddr, *buffer);

        DebugP_log2("UART:(%p) Wrote character 0x%x", hwAttrs->baseAddr,
            *buffer);
        buffer++;
        count++;
        size--;
    }

    while (UARTBusy(hwAttrs->baseAddr)) {
        ;
    }

    /* Disable TX */
    key = HwiP_disable();
    HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);
    HwiP_restore(key);

    DebugP_log2("UART:(%p) Write polling finished, %d bytes written",
        hwAttrs->baseAddr, count);

    return (count);
}

/*
 *  ======== UARTCC26X2_writePollingNotImpl ========
 */
int_fast32_t UARTCC26X2_writePollingNotImpl(UART_Handle handle,
        const void *buf, size_t size)
{
    /* Not supported */
    return (UART_ERROR);
}

/*
 *  ======== disableRX ========
 */
static void disableRX(UART_Handle handle)
{
    UARTCC26X2_Object         *object = handle->object;
    UARTCC26X2_HWAttrs const  *hwAttrs = handle->hwAttrs;
    uintptr_t                  key;

    if (!object->state.ctrlRxEnabled) {
        key = HwiP_disable();
        if (object->state.rxEnabled) {
            UARTIntDisable(hwAttrs->baseAddr, UART_INT_RX | UART_INT_RT |
                    UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE);
            /* Disable RX */
            HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_RXE);

            object->state.rxEnabled = false;

            Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
        }
        HwiP_restore(key);
    }
}

/*
 *  ======== enableRX ========
 */
static void enableRX(UART_Handle handle)
{
    UARTCC26X2_Object         *object = handle->object;
    UARTCC26X2_HWAttrs const  *hwAttrs = handle->hwAttrs;
    uintptr_t                  key;

    key = HwiP_disable();
    if (!object->state.rxEnabled) {
        /* Set constraint for sleep to guarantee transaction */
        Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

        /* Enable RX and receive interrupts */
        HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_RXE;
        UARTIntEnable(hwAttrs->baseAddr, UART_INT_RX | UART_INT_RT |
                UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE);

        object->state.rxEnabled = true;
    }
    HwiP_restore(key);
}

/*
 *  ======== getPowerMgrId ========
 */
static uint_fast16_t getPowerMgrId(uint32_t baseAddr)
{
    switch (baseAddr) {
        case UART0_BASE:
            return (PowerCC26XX_PERIPH_UART0);
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
        case UART1_BASE:
            return (PowerCC26X2_PERIPH_UART1);
#endif
        default:
            return ((uint_fast16_t)(~0U));
    }
}

/*
 *  ======== initHw ========
 */
static void initHw(UART_Handle handle)
{
    ClockP_FreqHz                 freq;
    UARTCC26X2_Object            *object = handle->object;
    UARTCC26X2_HWAttrs const     *hwAttrs = handle->hwAttrs;

    /*
     *  Configure frame format and baudrate.  UARTConfigSetExpClk() disables
     *  the UART and does not re-enable it, so call this function first.
     */
    ClockP_getCpuFreq(&freq);
    UARTConfigSetExpClk(hwAttrs->baseAddr, freq.lo, object->baudRate,
                        dataLength[object->dataLength] |
                        stopBits[object->stopBits] |
                        parityType[object->parityType]);

    /* Clear all UART interrupts */
    UARTIntClear(hwAttrs->baseAddr, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                    UART_INT_FE | UART_INT_RT | UART_INT_TX |
                                    UART_INT_RX | UART_INT_CTS);

    /* Set TX interrupt FIFO level and RX interrupt FIFO level */
    UARTFIFOLevelSet(hwAttrs->baseAddr, txFifoThreshold[hwAttrs->txIntFifoThr],
                                        rxFifoThreshold[hwAttrs->rxIntFifoThr]);

    /* If Flow Control is enabled, configure hardware flow control */
    if (isFlowControlEnabled(hwAttrs)) {
        UARTHwFlowControlEnable(hwAttrs->baseAddr);
    }
    else {
        UARTHwFlowControlDisable(hwAttrs->baseAddr);
    }

    /* Enable UART FIFOs */
    HWREG(hwAttrs->baseAddr + UART_O_LCRH) |= UART_LCRH_FEN;

    /* Enable the UART module */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_UARTEN;

    if (object->state.ctrlRxEnabled) {
        /* Enable RX */
        enableRX(handle);
    }
}

/*
 *  ======== initIO ========
 */
static void initIO(UART_Handle handle)
{
    UARTCC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (hwAttrs->txPin != GPIO_INVALID_INDEX) {
        GPIO_setMux(hwAttrs->txPin, hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_TX : IOC_PORT_MCU_UART1_TX);
    }
    if (hwAttrs->rxPin != GPIO_INVALID_INDEX) {
        GPIO_setMux(hwAttrs->rxPin, hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_RX : IOC_PORT_MCU_UART1_RX);
    }
    if (isFlowControlEnabled(hwAttrs))
    {
        if (hwAttrs->ctsPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(
                hwAttrs->ctsPin, hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_CTS : IOC_PORT_MCU_UART1_CTS
            );
        }
        if (hwAttrs->rtsPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(
                hwAttrs->rtsPin, hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_RTS : IOC_PORT_MCU_UART1_RTS
            );
        }
    }
}

static void finalizeIO(UART_Handle handle)
{
    UARTCC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (hwAttrs->txPin != GPIO_INVALID_INDEX) {
        GPIO_resetConfig(hwAttrs->txPin);
    }
    if (hwAttrs->rxPin != GPIO_INVALID_INDEX) {
        GPIO_resetConfig(hwAttrs->rxPin);
    }

    if (isFlowControlEnabled(hwAttrs))
    {
        if (hwAttrs->ctsPin != GPIO_INVALID_INDEX) {
            GPIO_resetConfig(hwAttrs->ctsPin);
        }
        if (hwAttrs->rtsPin != GPIO_INVALID_INDEX) {
            GPIO_resetConfig(hwAttrs->rtsPin);
        }
    }
}

/*
 *  ======== postNotifyFxn ========
 *  Called by Power module when waking up from LPDS.
 */
static int postNotifyFxn(unsigned int eventType, uintptr_t eventArg,
        uintptr_t clientArg)
{
    /* Reconfigure the hardware if returning from sleep */
    if (eventType == PowerCC26XX_AWAKE_STANDBY) {
        initHw((UART_Handle) clientArg);
    }

    return (Power_NOTIFYDONE);
}

/*
 *  ======== readBlockingTimeout ========
 */
static void readBlockingTimeout(uintptr_t arg)
{
    UARTCC26X2_Object *object = ((UART_Handle)arg)->object;
    object->state.bufTimeout = true;
    SemaphoreP_post(&(object->readSem));
}

/*
 *  ======== readIsr ========
 */
static void readIsr(UART_Handle handle, uint32_t status)
{
    UARTCC26X2_Object           *object = handle->object;
    UARTCC26X2_HWAttrs const    *hwAttrs = handle->hwAttrs;
    int                          readIn;
    int                          maxBytesToRead = FIFO_SIZE;
    size_t                       rxFifoThresholdBytes;
    int                          bytesRead;
    uint32_t                     errStatus = 0;

    if (status & UART_INT_OE) {
        /* Fifo overrun error - will read all data in the fifo */
        errStatus = UARTRxErrorGet(hwAttrs->baseAddr);
        UARTRxErrorClear(hwAttrs->baseAddr);
        object->status = errStatus;
    }
    else if (status & (UART_INT_RT)) {
        object->status = READTIMEDOUT;
    }
    else if (object->readRetPartial && (status & UART_INT_RX)) {
        rxFifoThresholdBytes = rxFifoBytes[hwAttrs->rxIntFifoThr];
        if (object->readCount > rxFifoThresholdBytes) {
            /* Will leave one byte in the FIFO to trigger the RT interrupt */
            maxBytesToRead = rxFifoThresholdBytes - 1;
        }
    }

    bytesRead = 0;

    while (UARTCharsAvail(hwAttrs->baseAddr)) {
        /*
         *  If the Ring buffer is full, leave the data in the FIFO.
         *  This will allow flow control to work, if it is enabled.
         */
        if (RingBuf_isFull(&object->ringBuffer)) {
            break;
        }

        readIn = UARTCharGetNonBlocking(hwAttrs->baseAddr);
        if (readIn & UART_BE_PE_FE) {
            errStatus = UARTRxErrorGet(hwAttrs->baseAddr);
            UARTRxErrorClear(hwAttrs->baseAddr);
            object->status = errStatus;
            break;
        }

        bytesRead++;

        if ((object->state.readDataMode == UART_DATA_TEXT) && readIn == '\r') {
            /* Echo character if enabled. */
            if (object->state.readEcho) {
                UARTCharPut(hwAttrs->baseAddr, '\r');
            }
            readIn = '\n';
        }
        RingBuf_put(&object->ringBuffer, (unsigned char)readIn);

        if ((object->state.readDataMode == UART_DATA_TEXT) &&
                (object->state.readEcho)) {
            UARTCharPut(hwAttrs->baseAddr, (unsigned char)readIn);
        }

        if (bytesRead >= maxBytesToRead) {
            break;
        }
    }

    if ((object->state.readMode == UART_MODE_BLOCKING) && ((bytesRead > 0) ||
            (errStatus != 0))) {
        /* object->state.callCallback set in readTaskBlocking() */
        if (object->state.callCallback) {
            object->state.callCallback = false;
            object->readCallback(handle, NULL, 0);
        }
    }

    /*
     * Check and see if a UART_read in callback mode told use to continue
     * servicing the user buffer...
     */
    if (object->state.drainByISR) {
        /* In CALLBACK mode */
        readTaskCallback(handle);
    }

    if (errStatus) {
        UARTCC26X2_readCancel(handle);
        if (hwAttrs->errorFxn) {
            hwAttrs->errorFxn(handle, errStatus);
        }
    }
}

/*
 *  ======== readSemCallback ========
 *  Simple callback to post a semaphore for the blocking mode.
 */
static void readSemCallback(UART_Handle handle, void *buffer, size_t count)
{
    UARTCC26X2_Object *object = handle->object;

    SemaphoreP_post(&(object->readSem));
}

/*
 *  ======== readTaskBlocking ========
 */
static int readTaskBlocking(UART_Handle handle)
{
    unsigned char              readIn;
    uintptr_t                  key;
    UARTCC26X2_Object         *object = handle->object;
    unsigned char             *buffer = object->readBuf;

    object->state.bufTimeout = false;
    object->state.callCallback = false;

    /*
     * It is possible for the object->timeoutClk and the callback function to
     * have posted the object->readSem Semaphore from the previous UART_read
     * call (if the code below didn't get to stop the clock object in time).
     * To clear this, we simply do a NO_WAIT pend on (binary) object->readSem
     * so that it resets the Semaphore count.
     */
    SemaphoreP_pend(&(object->readSem), SemaphoreP_NO_WAIT);

    if ((object->readTimeout != 0) &&
            (object->readTimeout != UART_WAIT_FOREVER)) {
        ClockP_setTimeout(&(object->timeoutClk), object->readTimeout);
        ClockP_start(&(object->timeoutClk));
    }

    while (object->readCount) {
        key = HwiP_disable();

        if (ringBufGet(handle, &readIn) < 0) {
            if (object->readRetPartial) {
                if (object->status == READTIMEDOUT) {
                    object->status = 0;
                    HwiP_restore(key);
                    break;
                }
            }

            if (object->state.bufTimeout || (object->status != 0)) {
                /* Timed out or RX error waiting for read to complete */
                HwiP_restore(key);
                disableRX(handle);
                break;
            }

            object->state.callCallback = true;
            HwiP_restore(key);

            if (object->readTimeout == 0) {
                break;
            }

            SemaphoreP_pend(&(object->readSem), SemaphoreP_WAIT_FOREVER);
        }
        else {
            /* Got something from the ring buffer */
            object->readCount--;
            HwiP_restore(key);

            DebugP_log2("UART:(%p) read '0x%02x'",
                    ((UARTCC26X2_HWAttrs const *)(handle->hwAttrs))->baseAddr,
                    (unsigned char)readIn);

            *buffer = readIn;
            buffer++;

            if (object->state.readDataMode == UART_DATA_TEXT &&
                    object->state.readReturnMode == UART_RETURN_NEWLINE &&
                    readIn == '\n') {
                break;
            }
        }
    }

    ClockP_stop(&(object->timeoutClk));
    return (object->readSize - object->readCount);
}

/*
 *  ======== readTaskCallback ========
 *  This function is called the first time by the UART_read task and tries to
 *  get all the data it can get from the ringBuffer. If it finished, it will
 *  perform the user supplied callback. If it didn't finish, the ISR must
 *  handle the remaining data. By setting the drainByISR flag, the UART_read
 *  function handed over the responsibility to get the remaining data to the
 *  ISR.
 */
static int readTaskCallback(UART_Handle handle)
{
    unsigned int               key;
    UARTCC26X2_Object         *object = handle->object;
    unsigned char              readIn;
    unsigned char             *bufferEnd;
    bool                       makeCallback = false;

    object->state.drainByISR = false;
    bufferEnd = (unsigned char*) object->readBuf + object->readSize;

    while (object->readCount) {
        if (ringBufGet(handle, &readIn) < 0) {
            break;
        }

        DebugP_log2("UART:(%p) read '0x%02x'",
            ((UARTCC26X2_HWAttrs const *)(handle->hwAttrs))->baseAddr,
            (unsigned char)readIn);

        *(unsigned char *) (bufferEnd - object->readCount *
            sizeof(unsigned char)) = readIn;

        key = HwiP_disable();

        object->readCount--;

        HwiP_restore(key);

        if ((object->state.readDataMode == UART_DATA_TEXT) &&
            (object->state.readReturnMode == UART_RETURN_NEWLINE) &&
            (readIn == '\n')) {
            makeCallback = true;
            break;
        }
    }

    if ((object->status & UARTCC26X2_RXERROR) && (object->readCount != 0)) {
        /* An error occurred, readIsr() will cancel the read */
    }
    else if ((object->readRetPartial && (object->status == READTIMEDOUT)) ||
            (object->readCount == 0) || makeCallback) {
        SwiP_post(&(object->readSwi));
    }
    else {
        object->state.drainByISR = true;
    }

    return (0);
}

/*
 *  ======== ringBufGet ========
 */
static int ringBufGet(UART_Handle handle, unsigned char *data)
{
    UARTCC26X2_Object      *object = handle->object;
    UARTCC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t               key;
    int32_t                 readIn;
    int                     count;

    key = HwiP_disable();

    if (RingBuf_isFull(&object->ringBuffer)) {
        count = RingBuf_get(&object->ringBuffer, data);

        readIn = UARTCharGetNonBlocking(hwAttrs->baseAddr);
        if (readIn != -1) {
            RingBuf_put(&object->ringBuffer, (unsigned char)readIn);
            count++;
        }
        HwiP_restore(key);
    }
    else {
        count = RingBuf_get(&object->ringBuffer, data);
        HwiP_restore(key);
    }

    return (count);
}

/*
 *  ======== swiReadCallback ========
 */
static void swiReadCallback(uintptr_t arg0, uintptr_t arg1)
{
    UARTCC26X2_Object *object = (UARTCC26X2_Object *)(((UART_Handle)arg0)->object);
    uintptr_t          key;
    size_t             count;

    key = HwiP_disable();

    count = object->readSize - object->readCount;
    object->readSize = 0;

    HwiP_restore(key);

    object->readCallback((UART_Handle)arg0, object->readBuf, count);
}

/*
 *  ======== swiWriteCallback ========
 */
static void swiWriteCallback(uintptr_t arg0, uintptr_t arg1)
{
    UARTCC26X2_Object  *object = (UARTCC26X2_Object *)(((UART_Handle)arg0)->object);
    UARTCC26X2_HWAttrs *hwAttrs = (UARTCC26X2_HWAttrs *)(((UART_Handle)arg0)->hwAttrs);
    uintptr_t           key;

    key = HwiP_disable();
    if (object->state.txEnabled) {
        /* Disable TX */
        HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);

        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
        object->state.txEnabled = false;
    }
    HwiP_restore(key);

    object->writeCallback((UART_Handle)arg0, (void *)object->writeBuf,
            object->writeSize);
}

/*
 *  ======== writeData ========
 */
static void writeData(UART_Handle handle)
{
    UARTCC26X2_Object           *object = handle->object;
    UARTCC26X2_HWAttrs const    *hwAttrs = handle->hwAttrs;
    unsigned char               *writeOffset;
    uintptr_t                    key;

    writeOffset = (unsigned char *)object->writeBuf +
            object->writeSize * sizeof(unsigned char);

    while (object->writeCount) {
        if (!UARTCharPutNonBlocking(hwAttrs->baseAddr,
                *(writeOffset - object->writeCount))) {
            /* TX FIFO is FULL */
            break;
        }
        if ((object->state.writeDataMode == UART_DATA_TEXT) &&
            (*(writeOffset - object->writeCount) == '\n')) {
            UARTCharPut(hwAttrs->baseAddr, '\r');
        }
        object->writeCount--;
    }

    if (!object->writeCount) {
        key = HwiP_disable();
        UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX);
        HwiP_restore(key);

        UARTIntClear(hwAttrs->baseAddr, UART_INT_TX);

        /*
         *  Set TX interrupt for end of transmission.
         *  The EOTRIS bit will be set only when all the data
         *  (including stop bits) have left the serializer.
         */
        key = HwiP_disable();
        UARTIntEnable(hwAttrs->baseAddr, UART_INT_EOT);
        HwiP_restore(key);

        if (!UARTBusy(hwAttrs->baseAddr)) {
            key = HwiP_disable();
            UARTIntDisable(hwAttrs->baseAddr, UART_INT_EOT);
            HwiP_restore(key);
            UARTIntClear(hwAttrs->baseAddr, UART_INT_EOT);

            SwiP_post(&(object->writeSwi));
        }

        DebugP_log2("UART:(%p) Write finished, %d bytes written",
            hwAttrs->baseAddr, object->writeSize - object->writeCount);
    }
}

/*
 *  ======== writeSemCallback ========
 *  Simple callback to post a semaphore for the blocking mode.
 */
static void writeSemCallback(UART_Handle handle, void *buffer, size_t count)
{
    UARTCC26X2_Object *object = handle->object;

    SemaphoreP_post(&(object->writeSem));
}
