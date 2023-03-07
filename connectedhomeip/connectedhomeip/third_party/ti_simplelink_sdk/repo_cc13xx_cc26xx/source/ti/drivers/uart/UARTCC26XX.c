/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/drivers/GPIO.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/uart.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/aon_ioc.h)

/* UARTCC26XX functions */
void         UARTCC26XX_close(UART_Handle handle);
int_fast16_t UARTCC26XX_control(UART_Handle handle, uint_fast16_t cmd,
                               void *arg);
void         UARTCC26XX_init(UART_Handle handle);
UART_Handle  UARTCC26XX_open(UART_Handle handle, UART_Params *params);
int_fast32_t UARTCC26XX_read(UART_Handle handle, void *buffer, size_t size);
int_fast32_t UARTCC26XX_readPolling(UART_Handle handle, void *buf,
                                   size_t size);
void         UARTCC26XX_readCancel(UART_Handle handle);
int_fast32_t UARTCC26XX_write(UART_Handle handle, const void *buffer,
                            size_t size);
int_fast32_t UARTCC26XX_writePolling(UART_Handle handle, const void *buf,
                                   size_t size);
void         UARTCC26XX_writeCancel(UART_Handle handle);

/* UARTCC26XX internal functions */
static void UARTCC26XX_initHw(UART_Handle handle);
static void UARTCC26XX_initIO(UART_Handle handle);
static void UARTCC26XX_finalizeIO(UART_Handle handle);
static int uartPostNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg);

/* UART function table for UARTCC26XX implementation */
const UART_FxnTable UARTCC26XX_fxnTable = {
    UARTCC26XX_close,
    UARTCC26XX_control,
    UARTCC26XX_init,
    UARTCC26XX_open,
    UARTCC26XX_read,
    UARTCC26XX_readPolling,
    UARTCC26XX_readCancel,
    UARTCC26XX_write,
    UARTCC26XX_writePolling,
    UARTCC26XX_writeCancel
};

static const uint32_t dataLength[] = {
    UART_CONFIG_WLEN_5, /* UART_LEN_5 */
    UART_CONFIG_WLEN_6, /* UART_LEN_6 */
    UART_CONFIG_WLEN_7, /* UART_LEN_7 */
    UART_CONFIG_WLEN_8  /* UART_LEN_8 */
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

/* Array for mapping of FIFO threshold defines selected by board files to:   */
/* - TX FIFO threshold define used by driverlib                              */
/* FIFO threshold defines (enum UARTCC26XX_FifoThreshold) must be used as    */
/* array index.                                                              */
/* Index 0 handles backward compatibility with legacy board files that don't */
/* select any FIFO thresholds.                                               */
static const uint8_t txFifoThreshold[6] = {
    UART_FIFO_TX1_8, /* UARTCC26XX_FIFO_THRESHOLD_DEFAULT */
    UART_FIFO_TX1_8, /* UARTCC26XX_FIFO_THRESHOLD_1_8     */
    UART_FIFO_TX2_8, /* UARTCC26XX_FIFO_THRESHOLD_2_8     */
    UART_FIFO_TX4_8, /* UARTCC26XX_FIFO_THRESHOLD_4_8     */
    UART_FIFO_TX6_8, /* UARTCC26XX_FIFO_THRESHOLD_6_8     */
    UART_FIFO_TX7_8  /* UARTCC26XX_FIFO_THRESHOLD_7_8     */
};

/* Array for mapping of FIFO threshold defines selected by board files to:   */
/* - RX FIFO threshold define used by driverlib                              */
/* FIFO threshold defines (enum UARTCC26XX_FifoThreshold) must be used as    */
/* array index.                                                              */
/* Index 0 handles backward compatibility with legacy board files that don't   */
/* select any FIFO thresholds.                                               */
static const uint8_t rxFifoThreshold[6] = {
    UART_FIFO_RX4_8, /* UARTCC26XX_FIFO_THRESHOLD_DEFAULT */
    UART_FIFO_RX1_8, /* UARTCC26XX_FIFO_THRESHOLD_1_8     */
    UART_FIFO_RX2_8, /* UARTCC26XX_FIFO_THRESHOLD_2_8     */
    UART_FIFO_RX4_8, /* UARTCC26XX_FIFO_THRESHOLD_4_8     */
    UART_FIFO_RX6_8, /* UARTCC26XX_FIFO_THRESHOLD_6_8     */
    UART_FIFO_RX7_8  /* UARTCC26XX_FIFO_THRESHOLD_7_8     */
};

/* Array for mapping of FIFO threshold defines selected by board files to: */
/* - Number of TX FIFO bytes                                               */
/* FIFO threshold defines (enum UARTCC26XX_FifoThreshold) must be used as  */
/* array index.                                                            */
/* Index 0 handles backward compatibility with legacy board files that     */
/* don't select any FIFO thresholds.                                       */
static const uint8_t txFifoBytes[6] = {
     4,   /* UARTCC26XX_FIFO_THRESHOLD_DEFAULT */
     4,   /* UARTCC26XX_FIFO_THRESHOLD_1_8     */
     8,   /* UARTCC26XX_FIFO_THRESHOLD_2_8     */
    16,   /* UARTCC26XX_FIFO_THRESHOLD_4_8     */
    24,   /* UARTCC26XX_FIFO_THRESHOLD_6_8     */
    28    /* UARTCC26XX_FIFO_THRESHOLD_7_8     */
};

/* Array for mapping of FIFO threshold defines selected by board files to: */
/* - Number of RX FIFO bytes                                               */
/* FIFO threshold defines (enum UARTCC26XX_FifoThreshold) must be used as  */
/* array index.                                                            */
/* Index 0 handles backward compatibility with legacy board files that     */
/* don't select any FIFO thresholds.                                       */
static const uint8_t rxFifoBytes[6] = {
    16,   /* UARTCC26XX_FIFO_THRESHOLD_DEFAULT */
     4,   /* UARTCC26XX_FIFO_THRESHOLD_1_8     */
     8,   /* UARTCC26XX_FIFO_THRESHOLD_2_8     */
    16,   /* UARTCC26XX_FIFO_THRESHOLD_4_8     */
    24,   /* UARTCC26XX_FIFO_THRESHOLD_6_8     */
    28    /* UARTCC26XX_FIFO_THRESHOLD_7_8     */
};

/*
 *  ================================= Macro ====================================
 *  TODO: Move me
 */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define UNIT_DIV_ROUNDUP(x,d) ((x + ((d) - 1)) / (d))

#define READ_DONE  0x1  /* Mask to trigger Swi on a read complete */
#define WRITE_DONE 0x2  /* Mask to trigger Swi on a write complete */

/*
 * Function for checking whether flow control is enabled.
 */
static inline bool isFlowControlEnabled(UARTCC26XX_HWAttrsV2 const  *hwAttrs) {
    return ((hwAttrs->ctsPin != GPIO_INVALID_INDEX) && (hwAttrs->rtsPin != GPIO_INVALID_INDEX));
}

/*
 * Ensure safe setting of the standby disallow constraint.
 */
static inline void threadSafeStdbyDisSet(volatile bool *pConstraint) {
    unsigned int  key;

    /* Disable interrupts */
    key = HwiP_disable();

    /* Only act if the current constraint is not previously set */
    if (!*pConstraint) {
        /* Set constraints to guarantee operation */
        Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
        *pConstraint = true;
    }

    /* Re-enable interrupts */
    HwiP_restore(key);
}

/*
 * Ensure safe releasing of the standby disallow constraint.
 */
static inline void threadSafeStdbyDisRelease(volatile bool *pConstraint) {
    unsigned int  key;

    /* Disable interrupts */
    key = HwiP_disable();

    if (*pConstraint) {
        /* release constraint since operation is done */
        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
        *pConstraint = false;
    }

    /* Re-enable interrupts */
    HwiP_restore(key);
}

/*
 *  ======== writeSemCallback ========
 *  Simple callback to post a semaphore for the blocking mode.
 */
static void writeSemCallback(UART_Handle handle, void *buffer, size_t count)
{
    UARTCC26XX_Object *object = handle->object;

    DebugP_log1("UART:(%p) posting write semaphore",
                ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr);

    SemaphoreP_post(&(object->writeSem));
}

/*
 *  ======== readSemCallback ========
 *  Simple callback to post a semaphore for the blocking mode.
 */
static void readSemCallback(UART_Handle handle, void *buffer, size_t count)
{
    UARTCC26XX_Object *object = handle->object;

    DebugP_log1("UART:(%p) posting read semaphore",
                ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr);

    SemaphoreP_post(&(object->readSem));
}

/*
 *  ======== writeData ========
 *  Write and process data to the UART.
 */
static int32_t writeData(UART_Handle handle, int32_t size)
{
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Send characters until FIFO is full or done. */
    while (size) {
        /* Send the next character and increment counts. */
        if (!UARTCharPutNonBlocking(hwAttrs->baseAddr, *(unsigned char *)(object->writeBuf))) {
            /* Character was not sent */
            break;
        }
        DebugP_log2("UART:(%p) Wrote character 0x%x",
                   hwAttrs->baseAddr, *(unsigned char *)object->writeBuf);
        object->writeBuf = (unsigned char *)object->writeBuf + 1;
        size--;
        object->writeCount++;
    }
    return (size);
}

/*
 *  ======== readData ========
 *  Read and process data from the UART.
 *  @param(size) number of bytes to be read
 */
static int32_t readData(UART_Handle handle, int32_t size)
{
    int32_t                         readIn;
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Receive chars until empty or done. */
    while (size && (readIn = (int32_t)UARTCharGetNonBlocking(hwAttrs->baseAddr)) != -1) {

        DebugP_log2("UART:(%p) Read character 0x%x",
                                 hwAttrs->baseAddr, (uint8_t)readIn);

        /* Update status. */
        *((unsigned char *)object->readBuf) = (uint8_t)readIn;
        object->readBuf = (unsigned char *)object->readBuf + 1;
        object->readCount++;
        size--;
    }
    return (size);
}

/*
 *  ======== readData2RingBuf ========
 *  Read and process data from the UART to RingBuf.
 *  @param(size) number of bytes to be read
 */
static int32_t readData2RingBuf(UART_Handle handle, int32_t size)
{
    int32_t                         readIn;
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const      *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Fill up RingBuf */
    while (size && (readIn = (int32_t)UARTCharGetNonBlocking(hwAttrs->baseAddr)) != -1) {

        DebugP_log2("UART:(%p) Read character 0x%x",
                                 hwAttrs->baseAddr, (uint8_t)readIn);

        size--;
        RingBuf_put(&object->ringBuffer, (unsigned char)readIn);
    }

    return (size);
}


/*
 *  ======== startTxFifoEmptyClk ========
 *  Last write to TX FIFO is done, but not shifted out yet. Start a clock
 *  which will trigger when the TX FIFO should be empty.
 *
 *  @param  handle           The UART_Handle for ongoing write.
 *  @param  numOfDataInFifo  The number of data present in FIFO after last write
 */
static void startTxFifoEmptyClk(UART_Handle handle, unsigned int numOfDataInFifo)
{
    UARTCC26XX_Object           *object;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Ensure that the clock is stopped so we can set a new timeout */
    ClockP_stop((ClockP_Handle) &(object->txFifoEmptyClk));

    /* No more to write, but data is not shifted out properly yet.
     *   1. Compute appropriate wait time for FIFO to empty out
     *       - 1 bit for start bit
     *       - 5+(object->dataLength) for total data length
     *       - +1 to "map" from stopBits to actual number of bits
     *       - 1000000 so we get 1 us resolution
     *       - 100 (100us) for margin
     */
    unsigned int writeTimeoutUs = (numOfDataInFifo*(1+5+(object->dataLength)+(object->stopBits+1))*1000000)/object->baudRate + 100;
    /*   2. Configure clock object to trigger when FIFO is empty
     *       - +1 in case clock module due to tick in less than one ClockP_getSystemTickPeriod()
     *       - UNIT_DIV_ROUNDUP to avoid fractional part being truncated during division
     */

    ClockP_setTimeout((ClockP_Handle) &(object->txFifoEmptyClk),
                     (1 + UNIT_DIV_ROUNDUP(writeTimeoutUs, ClockP_getSystemTickPeriod())));
    ClockP_start((ClockP_Handle) &(object->txFifoEmptyClk));
}

/*
 *  ======== writeFinishedDoCallback ========
 *  Write finished - make callback
 *
 *  This function is called when the txFifoEmptyClk times out. The TX FIFO
 *  should now be empty and all bytes have been transmitted. The TX will be
 *  turned off, TX interrupt is disabled and standby is allowed again.
 *
 *  @param(handle)         The UART_Handle for ongoing write.
 */
static void writeFinishedDoCallback(UART_Handle handle)
{
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /*   Function verifies that the FIFO is empty via BUSY flag */
    /*   If not yet ready start the periodic timer and wait another period*/
    /*   Return.. */
    if(UARTBusy(hwAttrs->baseAddr)){
        /* The UART is still busy.
         * Wait 500 us before checking again or 1 tick period if the
         * ClockP_getSystemTickPeriod() is larger than 500 us.
         */
        ClockP_setTimeout((ClockP_Handle) &(object->txFifoEmptyClk), MAX((500/ClockP_getSystemTickPeriod()),1));
        ClockP_start((ClockP_Handle) &(object->txFifoEmptyClk));
        return;
    }

    SwiP_or(&(object->swi), WRITE_DONE);
}

/*
 *  ======== writeTxFifoFlush ========
 *  Write cancelled or timed out, the TX FIFO must be flushed out.
 *
 *  This function is called either from writeCancel or when a blocking write
 *  has timed out. The HW does not support a simple API for flushing the TX FIFO
 *  so a workaround is done in SW.
 *
 *  @pre The TX FIFO empty clock must have been started in blocking mode.
 *
 *  @param object         Pointer to UART object
 *  @param hwAttrs        Pointer to UART hwAttrs
 */
static void writeTxFifoFlush(UARTCC26XX_Object  *object, UARTCC26XX_HWAttrsV2 const  *hwAttrs)
{
    unsigned int key;

    /*It is not possible to flush the TX FIFO with simple write to HW, doing workaround:
     * 0. Disable TX interrupt
     */
    key = HwiP_disable();
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX);
    HwiP_restore(key);

    /* 1. Re-mux TX as a GPIO and ensure it will stay high */
    GPIO_setConfig(hwAttrs->txPin, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH);
    /* 2. Disconnect cts */
    GPIO_setConfig(hwAttrs->ctsPin, GPIO_CFG_INPUT);

    /* 3. Wait for TX FIFO to become empty.
     *    CALLBACK: Idle until the TX FIFO is empty, i.e. no longer busy.
     *    BLOCKING: Periodically check if TX is busy emptying the FIFO.
     *              Must be handled at TX FIFO empty clock timeout:
     *                - the timeout/finish function must check the status
     */
    if(object->writeMode == UART_MODE_CALLBACK) {
        /* Wait until the TX FIFO is empty. CALLBACK mode can be used from
         * hwi/swi context, so we cannot use semaphore..
         */
        while(UARTBusy(hwAttrs->baseAddr));
    } else { /* i.e. UART_MODE_BLOCKING */
        /* Pend on semaphore again..(this time forever since we are flushing
         * TX FIFO and nothing should be able to stop it..
         */
        SemaphoreP_pend(&(object->writeSem), SemaphoreP_WAIT_FOREVER);
    }
    /* 5. Revert to active pins before returning */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
    DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1)
    GPIO_setMux(hwAttrs->txPin, (hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_TX : IOC_PORT_MCU_UART1_TX));
    if(isFlowControlEnabled(hwAttrs)) {
        GPIO_setMux(hwAttrs->ctsPin, (hwAttrs->baseAddr == UART0_BASE ? IOC_PORT_MCU_UART0_CTS : IOC_PORT_MCU_UART1_CTS));
    }
#else
    GPIO_setMux(hwAttrs->txPin, IOC_PORT_MCU_UART0_TX);
    if(isFlowControlEnabled(hwAttrs)) {
        GPIO_setMux(hwAttrs->ctsPin, IOC_PORT_MCU_UART0_CTS);
    }
#endif
}

/*
 *  ======== UARTCC26XX_hwiIntFxn ========
 *  Hwi function that processes UART interrupts.
 *
 *  Six UART interrupts are enabled when configured for RX: Receive timeout,
 *  receive FIFO at configured threshold (4/8 full by default) and all four
 *  error interrupts.
 *
 *  One interrupt is enabled when configured for TX: Transmit FIFO at configured
 *  threshold (7/8 empty by default).
 *
 *  The RX and TX can operate independently of each other.
 *
 *  When the read or write is finished they will
 *  post the semaphore or make the callback and log the transfer.
 *
 *  @param  arg         The UART_Handle for this Hwi.
 */
void UARTCC26XX_hwiIntFxn(uintptr_t arg)
{
    unsigned long               intStatus;
    unsigned long               errStatus = UART_OK;
    UARTCC26XX_Object           *object;
    UARTCC26XX_HWAttrsV2 const *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = ((UART_Handle)arg)->object;
    hwAttrs = ((UART_Handle)arg)->hwAttrs;

    /* Clear interrupts */
    intStatus = UARTIntStatus(hwAttrs->baseAddr, true);
    UARTIntClear(hwAttrs->baseAddr, intStatus);

    DebugP_log2("UART:(%p) Interrupt with mask 0x%x",
                             hwAttrs->baseAddr, intStatus);

    /* Record readSize */
    int32_t readSize = object->readSize;
    int32_t readSize_ringBuf = object->ringBuffer.length - object->ringBuffer.count;

    /* Basic error handling */
    if (intStatus & (UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE)) {

        /* If overrun, the error bit is set immediately */
        if (intStatus & UART_INT_OE) {
            /* check receive status register */
            errStatus = UARTRxErrorGet(hwAttrs->baseAddr);
            UARTRxErrorClear(hwAttrs->baseAddr);
            /* read out min of readsize and fifo size */
            int32_t bytesToRead = MIN(UARTCC26XX_FIFO_SIZE, readSize);
            readData((UART_Handle)arg, bytesToRead);
            /* read the rest into RingBuf */
            bytesToRead = MIN((UARTCC26XX_FIFO_SIZE - bytesToRead), readSize_ringBuf);
            readData2RingBuf((UART_Handle)arg, bytesToRead);
        }
        else {
            /*
             * else, for break, framing and par. error bits are at error index
             * (we would like to keep the "surviving" bits)
             */
            /* Only if UART_read is active */
            while(object->readSize > 0) {
                /* read one data */
                readData((UART_Handle)arg, 1);
                /* check receive status register if byte is OK */
                errStatus = UARTRxErrorGet(hwAttrs->baseAddr);
                UARTRxErrorClear(hwAttrs->baseAddr);
                if(errStatus != 0) {
                    /*
                     * last read was not including data, reset data vars
                     * (i.e. upd readBuf pointer, decr readCount)
                     */
                    object->readBuf = (unsigned char *)object->readBuf - 1;
                    object->readCount--;
                    /* the FIFO index with error is reached, stop reading */
                    break;
                }
                else {
                    /* current read was not the problem update readSize */
                    object->readSize--;
                }
            }

            /* If we managed to read out all bytes when checking for errors, make sure to post the Swi
             * and read remaining data into the ring buffer */
            if (!object->readSize) {
                /* Read succeeded. */
                SwiP_or(&(object->swi), READ_DONE);
                /* Read any remaining data in the FIFO into the ring buffer. */
                readData2RingBuf((UART_Handle)arg, readSize_ringBuf);
            }
        }

        /* Report current error status */
        object->status = (UART_Status)errStatus;

        /* Break and clean up any ongoing transaction */
        UARTCC26XX_readCancel((UART_Handle)arg);

        /* Notify application of error */
        if (hwAttrs->errorFxn) {
            hwAttrs->errorFxn((UART_Handle)arg, errStatus);
        }
    }
    else {
        /* Receive Timeout */
        if(intStatus & UART_INT_RT) {
            /* if UART_read is active */
            if (readSize) {
                /* Read data from FIFO. */
                object->readSize = readData((UART_Handle)arg, readSize);

                /* If return partial is set */
                if(object->readRetPartial) {
                    /* Partial read accepted, read succeeded by def, set
                     * readSize to allow callback to trigger new _read()
                     */
                    object->readSize = 0;
                    /* Read succeeded */
                    SwiP_or(&(object->swi), READ_DONE);
                }
                /* else - return when all bytes have arrived */
                else {
                    /* If all bytes are read */
                    if(!object->readSize) {
                        /* Read succeeded */
                        SwiP_or(&(object->swi), READ_DONE);
                    }
                }
            }
            else {
                /* not during UART_read, put in RingBuf */
                readData2RingBuf((UART_Handle)arg, readSize_ringBuf);
            }
        }
        else {
            /* RX interrupt, since CTS is handled by HW */
            if (intStatus & UART_INT_RX) {
                if (readSize) {
                    /* if UART_read is active */

                    /* Read whatever is less of:
                     *   - FIFO_THR bytes - 1 (leave one byte to trigger RT timeout)
                     *   - object->readSize (never read more than requested from application)
                     * since RT timeout will only trigger if FIFO is not empty.
                     */
                    size_t readFifoThresholdBytes = rxFifoBytes[hwAttrs->rxIntFifoThr];
                    size_t bytesToRead = MIN(readFifoThresholdBytes - 1, object->readSize);

                    /* Do read, all bytes we request to read, are present in FIFO */
                    readData((UART_Handle)arg, bytesToRead);

                    /* Decrement object->readSize with the actual number of bytes read
                     *  There will always be at least bytesToRead number of bytes in FIFO
                     */
                    object->readSize -= bytesToRead;

                    /* If all bytes are read */
                    if (!object->readSize) {
                        /* Read succeeded. */
                        SwiP_or(&(object->swi), READ_DONE);

                        /* Read any remaining data in the FIFO into the ring buffer. */
                        readData2RingBuf((UART_Handle)arg, readSize_ringBuf);
                    }
                }
                else {
                    /* not during UART_read, put all in RingBuf */
                    readData2RingBuf((UART_Handle)arg, readSize_ringBuf);
                }
            }
        }
    }

    /* Write if there are characters to be written. */
    if ((intStatus & UART_INT_TX) && object->writeSize) {
        /* Using writtenLast=writeSize before last write
         *(since writeSize=0 when the last finishes)
         */
        uint32_t writtenLast = object->writeSize;
        object->writeSize = writeData((UART_Handle)arg, object->writeSize);
        if(!object->writeSize) {
            /* No more to write, but data is not shifted out properly yet.
             * Start TX FIFO Empty clock, which will trigger the txFifoEmptyClk
             * function.
             * The number of bytes left in the TX FIFO when the TX FIFO threshold
             * interrupt occurs are added to writtenLast.
             */
             startTxFifoEmptyClk((UART_Handle)arg, (writtenLast + txFifoBytes[hwAttrs->txIntFifoThr]));
        }
    }
}

/*
 *  ======== UARTCC26XX_swiIntFxn ========
 *  Swi function that processes UART interrupts.
 *  @param  arg         The UART_Handle for this Hwi.
 */
void UARTCC26XX_swiIntFxn(uintptr_t arg0, uintptr_t arg1)
{
    UARTCC26XX_Object          *object;
    UARTCC26XX_HWAttrsV2 const *hwAttrs;
    uint32_t                    trigger = SwiP_getTrigger();

    /* Get the pointer to the object and hwAttrs */
    object = ((UART_Handle)arg0)->object;
    hwAttrs = (UARTCC26XX_HWAttrsV2 const *)(((UART_Handle)arg0)->hwAttrs);

    if (trigger & READ_DONE) {
        /* Release power constraint. */
        threadSafeStdbyDisRelease(&(object->uartRxPowerConstraint));

        /* Reset the read buffer so we can pass it back */
        object->readBuf = (unsigned char *)object->readBuf - object->readCount;

        /* Do Callback */
        object->readCallback((UART_Handle)arg0, object->readBuf,
                         object->readCount);

        DebugP_log2("UART:(%p) Read finished, %d bytes read",
                hwAttrs->baseAddr, object->readCount);
    }

    if (trigger & WRITE_DONE) {
        /* Release constraint since transaction is done */
        threadSafeStdbyDisRelease(&(object->uartTxPowerConstraint));

        /* Disable TX interrupt */
        UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX);

        /* Disable TX */
        HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);

        /* Reset the write buffer so we can pass it back */
        object->writeBuf = (unsigned char *)object->writeBuf - object->writeCount;

        /* Make callback */
        object->writeCallback((UART_Handle)arg0, (uint8_t*)object->writeBuf,
                              object->writeCount);
        DebugP_log2("UART:(%p) Write finished, %d bytes written",
                hwAttrs->baseAddr, object->writeCount);
    }
}

/*!
 *  @brief UART CC26XX initialization
 *
 *  @pre    Calling context: Hwi, Swi, Task, Main
 *
 *  @param handle  A UART_Handle
 *
 */
void UARTCC26XX_init(UART_Handle handle)
{
    UARTCC26XX_Object           *object;

    /* Get the pointer to the object */
    object = handle->object;
    object->opened = false;
}

/*!
 *  @brief  Function to initialize the CC26XX UART peripheral specified by the
 *          particular handle. The parameter specifies which mode the UART
 *          will operate.
 *
 *  The function will set a dependency on it power domain, i.e. power up the
 *  module and enable the clock. The IOs are allocated. Neither the RX nor TX
 *  will be enabled, and none of the interrupts are enabled.
 *
 *  @pre    UART controller has been initialized
 *          Calling context: Task
 *
 *  @param  handle        A UART_Handle
 *
 *  @param  params        Pointer to a parameter block, if NULL it will use
 *                        default values
 *
 *  @return A UART_Handle on success or a NULL on an error or if it has been
 *          already opened
 *
 *  @sa     UARTCC26XX_close()
 */
UART_Handle UARTCC26XX_open(UART_Handle handle, UART_Params *params)
{
    unsigned int                    key;
    /* Use union to save on stack allocation */
    union {
        HwiP_Params                  hwiParams;
        SwiP_Params                  swiParams;
        ClockP_Params                clkParams;
    } paramsUnion;
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if the UART is open. */
    key = HwiP_disable();

    /* Check if the UART is open already with the base addr. */
    if (object->opened == true) {
        HwiP_restore(key);

        DebugP_log1("UART:(%p) already in use.", hwAttrs->baseAddr);

        return (NULL);
    }
    object->opened = true;
    HwiP_restore(key);

    /* Check that a callback is set */
    DebugP_assert(params->readMode != UART_MODE_CALLBACK ||
                  params->readCallback != NULL);
    DebugP_assert(params->writeMode != UART_MODE_CALLBACK ||
                  params->writeCallback != NULL);

    /* Initialize the UART object */
    object->readMode       = params->readMode;
    object->writeMode      = params->writeMode;
    object->readTimeout    = params->readTimeout;
    object->writeTimeout   = params->writeTimeout;
    object->readCallback   = params->readCallback;
    object->writeCallback  = params->writeCallback;
    object->readReturnMode = params->readReturnMode;
    object->readDataMode   = params->readDataMode;
    object->writeDataMode  = params->writeDataMode;
    object->baudRate       = params->baudRate;
    object->dataLength     = params->dataLength;
    object->stopBits       = params->stopBits;
    object->parityType     = params->parityType;

    /* Set UART transaction variables to defaults. */
    object->writeBuf = NULL;
    object->readBuf = NULL;
    object->writeCount = 0;
    object->readCount = 0;
    object->writeSize = 0;
    object->readSize = 0;
    object->writeCR = false;
    object->readRetPartial = false;

    object->uartRxPowerConstraint = false;
    object->uartTxPowerConstraint = false;

    /* Register power dependency - i.e. power up and enable clock for UART. */
    Power_setDependency(hwAttrs->powerMngrId);

    /* Initialize the UART hardware module */
    UARTCC26XX_initHw(handle);

    /* Configure IOs, make sure it was successful */
    UARTCC26XX_initIO(handle);

    /* Create Hwi object for this UART peripheral. */
    HwiP_Params_init(&(paramsUnion.hwiParams));
    paramsUnion.hwiParams.arg = (uintptr_t)handle;
    paramsUnion.hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi), hwAttrs->intNum, UARTCC26XX_hwiIntFxn,
                  &(paramsUnion.hwiParams));

    /* Create Swi object for this UART peripheral */
    SwiP_Params_init(&(paramsUnion.swiParams));
    paramsUnion.swiParams.arg0 = (uintptr_t)handle;
    paramsUnion.swiParams.priority = hwAttrs->swiPriority;
    SwiP_construct(&(object->swi), UARTCC26XX_swiIntFxn, &(paramsUnion.swiParams));

    /* Initialize semaphore */

    /* If write mode is blocking create a semaphore and set callback. */
    if (object->writeMode == UART_MODE_BLOCKING) {
        SemaphoreP_constructBinary(&(object->writeSem), 0);
        object->writeCallback = &writeSemCallback;
    }

    /* If read mode is blocking create a semaphore and set callback. */
    if (object->readMode == UART_MODE_BLOCKING) {
        SemaphoreP_constructBinary(&(object->readSem), 0);
        object->readCallback = &readSemCallback;
    }

    /* Create clock object to be used for write FIFO empty callback */
    ClockP_Params_init(&paramsUnion.clkParams);
    paramsUnion.clkParams.period    = 0;
    paramsUnion.clkParams.startFlag = false;
    paramsUnion.clkParams.arg       = (uintptr_t) handle;
    ClockP_construct(&(object->txFifoEmptyClk),
                    (ClockP_Fxn) &writeFinishedDoCallback,
                    10, &(paramsUnion.clkParams));

    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&object->ringBuffer, hwAttrs->ringBufPtr, hwAttrs->ringBufSize);

    /* Register notification function */
    Power_registerNotify(&object->uartPostObj, PowerCC26XX_AWAKE_STANDBY, (Power_NotifyFxn)uartPostNotify, (uint32_t)handle);

    /* UART opened successfully */
    DebugP_log1("UART:(%p) opened", hwAttrs->baseAddr);

    /* Return the handle */
    return (handle);
}

/*!
 *  @brief  Function to close a given CC26XX UART peripheral specified by the
 *          UART handle.
 *
 *  Will disable the UART, disable all UART interrupts and release the
 *  dependency on the corresponding power domain.
 *
 *  @pre    UARTCC26XX_open() had to be called first.
 *          Calling context: Task
 *
 *  @param  handle  A UART_Handle returned from UART_open()
 *
 *  @sa     UARTCC26XX_open
 */
void UARTCC26XX_close(UART_Handle handle)
{
    UARTCC26XX_Object            *object;
    UARTCC26XX_HWAttrsV2 const   *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable all UART module interrupts. */
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                      UART_INT_FE | UART_INT_RT | UART_INT_TX |
                                      UART_INT_RX | UART_INT_CTS);

    /* Mark the module as available */
    object->opened = false;

    /* Cancel any possible ongoing reads/writes */
    UARTCC26XX_writeCancel(handle);
    UARTCC26XX_readCancel(handle);

    /* Disable UART FIFO */
    HWREG(hwAttrs->baseAddr + UART_O_LCRH) &= ~(UART_LCRH_FEN);
    /* Disable UART module */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_UARTEN | UART_CTL_TXE |
                                      UART_CTL_RXE);

    /* Deallocate pins */
    UARTCC26XX_finalizeIO(handle);

    /* Release power dependency - i.e. potentially power down serial domain. */
    Power_releaseDependency(hwAttrs->powerMngrId);

    /* Destruct the SYS/BIOS objects. */
    HwiP_destruct(&(object->hwi));
    SwiP_destruct(&(object->swi));
    if (object->writeMode == UART_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->writeSem));
    }
    if (object->readMode == UART_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->readSem));
    }
    ClockP_destruct(&(object->txFifoEmptyClk));

    /* Unregister power notification objects */
    Power_unregisterNotify(&object->uartPostObj);

    DebugP_log1("UART:(%p) closed", hwAttrs->baseAddr);
}


/*!
 *  @brief  Function for setting control parameters of the UART
 *          after it has been opened.
 *
 *  @pre    UARTCC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle A UART handle returned from UARTCC26XX_open()
 *
 *  @param  cmd  The command to execute, supported commands are:
 *          | Command                               | Description             |
 *          |-------------------------------------- |-------------------------|
 *          | ::UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE | Enable RETURN_PARTIAL  |
 *          | ::UARTCC26XX_CMD_RETURN_PARTIAL_DISABLE| Disable RETURN_PARTIAL |
 *
 *  @param  *arg  Pointer to command arguments, currently not in use, set to NULL.
 *
 *  @return ::UART_STATUS_SUCCESS if success, or error code if error.
 */
int_fast16_t UARTCC26XX_control(UART_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    /* Locals */
    UARTCC26XX_Object            *object;
    UARTCC26XX_HWAttrsV2 const   *hwAttrs;
    unsigned int                 key;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Initialize return value*/
    int ret = UART_STATUS_UNDEFINEDCMD;
    /* Do command*/
    switch(cmd)
    {
        case UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE:
            /* Enable RETURN_PARTIAL */
            object->readRetPartial = true;
            ret = UART_STATUS_SUCCESS;
        break;

        case UARTCC26XX_CMD_RETURN_PARTIAL_DISABLE:
            /* Disable RETURN_PARTIAL */
            object->readRetPartial = false;
            ret = UART_STATUS_SUCCESS;
        break;

        case UARTCC26XX_CMD_RX_FIFO_FLUSH:
            /* Flush RX FIFO */
            hwAttrs = handle->hwAttrs;

            /* Disable interrupts to avoid reading data while changing state. */
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

            ret = UART_STATUS_SUCCESS;
        break;

        default:
            /* This command is not defined */
            ret = UART_STATUS_UNDEFINEDCMD;
        break;
    }

    /* Return */
    return (ret);
}

/*!
 *  @brief  Function that writes data to a UART
 *
 *  This function initiates an operation to write data to CC26XX UART
 *  controller.
 *
 *  In ::UART_MODE_BLOCKING, UART_write will block task execution until all
 *  the data in buffer has been written.
 *
 *  In ::UART_MODE_CALLBACK, UART_write does not block task execution, but calls a
 *  callback function specified by writeCallback when the data has been written.
 *
 *  When the write function is called, TX is enabled, TX interrupt is enabled,
 *  and standby is not allowed.
 *
 *  @pre    UARTCC26XX_open() has to be called first.
 *          Calling context: Hwi and Swi (only if using ::UART_MODE_CALLBACK), Task
 *
 *  @param  handle      A UART_Handle returned from UARTCC26XX_open()
 *
 *  @param  buffer      A pointer to buffer containing data to be written
 *
 *  @param  size        The number of bytes in buffer that should be written
 *                      onto the UART.
 *
 *  @return Returns the number of bytes that have been written to the UART,
 *          UART_ERROR on an error.
 *
 */
int_fast32_t UARTCC26XX_write(UART_Handle handle, const void *buffer,
        size_t size)
{
    unsigned int                    key;
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Check that there is data to write */
    DebugP_assert(size != 0);

    /* Disable preemption while checking if the UART is in use. */
    key = HwiP_disable();
    /* The UART TX is disabled after a successful write, if it is
     * still active another write is in progress, reject. */
    uint32_t writeActive = HWREG(hwAttrs->baseAddr + UART_O_CTL) & (UART_CTL_TXE);
    if (!object->opened || writeActive) {
        HwiP_restore(key);
        DebugP_log1("UART:(%p) Could not write data, uart closed or in use.",
                    ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr);

        return (UART_ERROR);
    }

    /* Stop the txFifoEmpty clock in case it was running due to a previous write operation */
    ClockP_stop((ClockP_Handle) &(object->txFifoEmptyClk));

    /* Update the status of the UART module */
    object->status = UART_OK;

    /* Save the data to be written and restore interrupts. */
    object->writeBuf = buffer;
    object->writeCount = 0;

    /* Enable TX */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_TXE;

    HwiP_restore(key);

    /* Set constraints to guarantee transaction */
    threadSafeStdbyDisSet(&(object->uartTxPowerConstraint));

    uint32_t writtenLast = size;
    /* Fill up TX FIFO */
    if (!(object->writeSize = writeData(handle, size))) {
        /* No more data to transmit - Write is finished but all bytes
        *  may not have been shifted out. */
        startTxFifoEmptyClk((UART_Handle)handle, writtenLast);

        /* If writeMode is blocking, block and get the status. */
        if (object->writeMode == UART_MODE_BLOCKING) {
            /* Pend on semaphore and wait for Hwi to finish. */
            if (SemaphoreP_OK != SemaphoreP_pend(&(object->writeSem),
                        object->writeTimeout)) {
                /* Reset writeSize */
                object->writeSize = 0;

                /* Set status to TIMED_OUT */
                object->status = UART_TIMED_OUT;

                /* Workaround for flushing the TX FIFO */
                writeTxFifoFlush(object, hwAttrs);

                /* Release constraint */
                threadSafeStdbyDisRelease(&(object->uartTxPowerConstraint));

                DebugP_log2("UART:(%p) Write timed out, %d bytes written",
                           ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr,
                           object->writeCount);

                /* Return UART_ERROR to indicate something went wrong, object->status set to UART_TIMED_OUT*/
                return UART_ERROR;
            }
            return (object->writeCount);
        }
    } else {

        key = HwiP_disable();

        /* Enable TX interrupts */
        UARTIntEnable(hwAttrs->baseAddr, UART_INT_TX);

        HwiP_restore(key);

        /* If writeMode is blocking, block and get the status. */
        if (object->writeMode == UART_MODE_BLOCKING) {
            /* Pend on semaphore and wait for Hwi to finish. */
            if (SemaphoreP_OK != SemaphoreP_pend(&(object->writeSem),
                        object->writeTimeout)) {
                /* Semaphore timed out, make the write empty and log the write. */

                /* Starting a timer to enable the posting of semaphore used in writeTxFifoFlush.
                 * writtenLast in this case is equal to full TX FIFO. This is a conservative number as
                 * some of the data might have been sent.
                 */
                startTxFifoEmptyClk((UART_Handle)handle, writtenLast);

                /* Reset writeSize */
                object->writeSize = 0;

                /* Set status to TIMED_OUT */
                object->status = UART_TIMED_OUT;

                /* Workaround for flushing the TX FIFO */
                writeTxFifoFlush(object, hwAttrs);

                /* Release constraint */
                threadSafeStdbyDisRelease(&(object->uartTxPowerConstraint));

                DebugP_log2("UART:(%p) Write timed out, %d bytes written",
                           ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr,
                           object->writeCount);

                /* Return UART_ERROR to indicate something went wrong (object->status set to UART_TIMED_OUT)*/
                return UART_ERROR;
            }

            /* Return the numbers of samples written */
            return (object->writeCount);
        }
    }
    /* This return will only be active in UART_MODE_CALLBACK mode. */
    return (0);
}

/*!
 *  @brief This function is NOT supported
 *
 *  @pre    UARTCC26XX_open() and has to be called first.
 *          Calling context: Task
 *
 *  @param  handle      The UART_Handle for ongoing write.
 *
 *  @param  buf         A pointer to buffer containing data to be written
 *
 *  @param  size        The number of bytes in buffer that should be written
 *                      onto the UART.
 *
 *  @return Always ::UART_ERROR
 */
int_fast32_t UARTCC26XX_writePolling(UART_Handle handle, const void *buf,
        size_t size)
{
    /* Not supported */
    return (UART_ERROR);
}

/*!
 *  @brief Function that cancel UART write. Will disable TX interrupt, disable
 *         TX and allow standby.
 *
 *  @pre    UARTCC26XX_open() and has to be called first.
 *          Calling context: Task
 *
 *  @param handle         The UART_Handle for ongoing write.
 */
void UARTCC26XX_writeCancel(UART_Handle handle)
{
    unsigned int                    key;
    UARTCC26XX_Object               *object;
    UARTCC26XX_HWAttrsV2 const     *hwAttrs;

    /* Get the pointer to the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable interrupts to avoid writing data while changing state. */
    key = HwiP_disable();

    /* Return if there is nothing to write and TX FIFO is empty. */
    if (!object->writeSize &&
        !UARTBusy(hwAttrs->baseAddr) &&
        !(HWREG(hwAttrs->baseAddr + UART_O_CTL) & (UART_CTL_TXE))) {
        HwiP_restore(key);
        return;
    }

    /* Stop the clock in case we have finished shifting out the
     * tx bytes but the tx FIFO empty clock has not timed out yet.
     * Otherwise the clock will timeout and behave as though the
     * operation were not canceled resulting in two callbacks for
     * the same operation.
     */
    ClockP_stop((ClockP_Handle) &(object->txFifoEmptyClk));

    /* Set size = 0 to prevent writing and restore interrupts. */
    object->writeSize = 0;
    HwiP_restore(key);

    /* If flow control is enabled, a workaround for flushing the fifo is needed..*/
    if (isFlowControlEnabled(hwAttrs)) {
        writeTxFifoFlush(object, hwAttrs);
    }

    key = HwiP_disable();

    /* Disable TX interrupt */
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_TX);

    /* Disable UART TX */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) &= ~(UART_CTL_TXE);

    HwiP_restore(key);

    /* Release constraint since transaction is done */
    threadSafeStdbyDisRelease(&(object->uartTxPowerConstraint));

    /* Reset the write buffer so we can pass it back */
    object->writeBuf = (unsigned char *)object->writeBuf - object->writeCount;
    object->writeCallback(handle, (uint8_t*)object->writeBuf,
                          object->writeCount);

    DebugP_log2("UART:(%p) Write canceled, "
                            "%d bytes written",
             ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr,
               object->writeCount);
}

/*!
 *  @brief  Function for reading from UART interface.
 *
 *  The function will enable the RX, enable all RX interrupts and disallow
 *  chip from going into standby.
 *
 *  @pre    UARTCC26XX_open() has to be called first.
 *          Calling context: Hwi and Swi (only if using ::UART_MODE_CALLBACK), Task
 *
 *  @param  handle A UART handle returned from UARTCC26XX_open()
 *
 *  @param  *buffer  Pointer to read buffer
 *
*  @param  size  Number of bytes to read. If ::UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE
 *                has been set, the read will
 *                return if the reception is inactive for a 32-bit period
 *                (i.e. before all bytes are received).
 *
 *  @return Number of samples read
 *
 *  @sa     UARTCC26XX_open(), UARTCC26XX_readCancel()
 */
int_fast32_t UARTCC26XX_read(UART_Handle handle, void *buffer, size_t size)
{
    unsigned char                    readIn;
    UARTCC26XX_Object                *object;
    UARTCC26XX_HWAttrsV2 const       *hwAttrs;
    size_t                           objectReadSize;
    unsigned int                     key;

    /* Get the pointer to the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    if (!object->opened) {
        DebugP_log1("UART:(%p) not opened.", hwAttrs->baseAddr);
        return (UART_ERROR);
    }

    if (object->readSize) {
        /* Previous read is not done, return */
        DebugP_log1("UART:(%p) Could not read data, uart in use.",
             ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr);

        return (UART_ERROR);
    }

    /* Set readSize */
    objectReadSize = size;

    /* Update the status of the UART module */
    object->status = UART_OK;

    /* Save the data to be read and restore interrupts. */
    object->readBuf = buffer;
    object->readCount = 0;

    /* Read from RingBuf as much as is available */
    while (objectReadSize) {
        int rdCount;

        /* Disable interrupts while reading from the ring buffer */
        key = HwiP_disable();
        rdCount = RingBuf_get(&object->ringBuffer, &readIn);
        if (rdCount < 0) {
            /* RingBuf is empty, need to read from FIFO */
            object->readSize = objectReadSize;

            /* Enable RX */
            HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_RXE;

            /* Enable RX interrupts */
            UARTIntEnable(hwAttrs->baseAddr, UART_INT_RX | UART_INT_RT |
                    UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE);

            HwiP_restore(key);

            /* Set constraint for sleep to guarantee transaction */
            threadSafeStdbyDisSet(&(object->uartRxPowerConstraint));

            /* If readMode is blocking, block and get the status. */
            if (object->readMode == UART_MODE_BLOCKING) {
                /*
                 *  Check for return partial case before waiting on the
                 *  semaphore.  It's possible that no more data will come
                 *  in to post the semaphore or cause a read timeout.
                 */
                key = HwiP_disable();

                if ((object->readRetPartial) && (object->readCount)
                        && (object->readSize)) {
                    /* Return partial enabled and some data has been read */
                    /* Reset readSize to allow next UART_read() */
                    object->readSize = 0;
                    HwiP_restore(key);

                    /* Release the constraint */
                    threadSafeStdbyDisRelease(&(object->uartRxPowerConstraint));

                    /* return the number of data read */
                    return (object->readCount);
                }
                HwiP_restore(key);

                /* Pend on semaphore and wait for Hwi to finish. */
                if (SemaphoreP_OK != SemaphoreP_pend(&(object->readSem),
                            object->readTimeout)) {

                    /*
                     *  If the ISR posts the semaphore here, the count will be
                     *  wrong, so we'll pend again with 0 timeout after setting
                     *  the read size to 0
                     */
                    /* Semaphore timed out, make the read empty and log the read. */
                    object->readSize = 0;

                    if (SemaphoreP_OK != SemaphoreP_pend(&(object->readSem),
                                SemaphoreP_NO_WAIT)) {
                        /*
                         *  Release constraint since transaction timed out,
                         *  allowed to enter standby
                         */
                        threadSafeStdbyDisRelease(&(object->uartRxPowerConstraint));

                        /* Reset the read buffer so we can pass it back */
                        object->readBuf = (unsigned char *)object->readBuf -
                                object->readCount;

                        /* Set status to TIMED_OUT */
                        object->status = UART_TIMED_OUT;

                        DebugP_log2("UART:(%p) Read timed out, %d bytes read",
                                ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr,
                                object->readCount);
                    }
                }

                /* return the number of data read */
                return (object->readCount);
            }

            /*
             *  Post Swi for return partial case.  Disable interrupts to
             *  ensure that the ISR does not finish the read and also post
             *  the Swi before we set object->readSize to 0.  Check
             *  object->readSize in case the ISR already posted the Swi.
             */
            key = HwiP_disable();

            /* readMode is callback */
            if ((object->readRetPartial) && (object->readCount)
                    && (object->readSize)) {
                /* Return partial enabled and some data has been read */
                /* Reset readSize to allow next UART_read() */
                object->readSize = 0;
                HwiP_restore(key);

                /* Read succeeded */
                SwiP_or(&(object->swi), READ_DONE);
            }
            else {
                HwiP_restore(key);
            }

            return (0);
        }

        HwiP_restore(key);

        /* save data to receive buffer */
        *((unsigned char *)object->readBuf) = (uint8_t)readIn;
        object->readBuf = (unsigned char *)object->readBuf + 1;
        object->readCount++;
        objectReadSize--;
    }

    /* Read succeeded. */
    /* Reset the read buffer so we can pass it back */
    object->readBuf = (unsigned char *)object->readBuf - object->readCount;

    /* Do Callback */
    if (object->readMode == UART_MODE_CALLBACK) {
        object->readCallback(handle, object->readBuf, object->readCount);
    }

    /* return the number of data read */
    return (object->readCount);
}

/*
 *  ======== UARTCC26XX_readPolling ========
 */
int_fast32_t UARTCC26XX_readPolling(UART_Handle handle, void *buf, size_t size)
{
    /* Not supported */
    return (UART_ERROR);
}

/*!
 *  @brief Function that cancel UART read. Will disable all RX interrupt,
 *         disable
 *         RX and allow standby. Should also be called after a succeeding UART
 *         read if no more bytes are expected and standby is wanted.
 *
 *  @pre    UARTCC26XX_open() has to be called first.
 *          Calling context: Task
 *
 *  @param handle         The UART_Handle for ongoing write.
 */
void UARTCC26XX_readCancel(UART_Handle handle)
{
    unsigned int                 key;
    UARTCC26XX_Object            *object;
    UARTCC26XX_HWAttrsV2 const  *hwAttrs;

    /* Get the pointer to the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable interrupts to avoid reading data while changing state. */
    key = HwiP_disable();

    /* Disable RX interrupts */
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                      UART_INT_FE | UART_INT_RT | UART_INT_RX);

    /* Release constraint since transaction is done */
    threadSafeStdbyDisRelease(&(object->uartRxPowerConstraint));

    /* Return if there is no read. */
    if (!object->readSize) {
        HwiP_restore(key);
        return;
    }

    /* Set size = 0 to prevent reading. */
    object->readSize = 0;

    /* Reset the read buffer so we can pass it back */
    object->readBuf = (unsigned char *)object->readBuf - object->readCount;

    /* Restore interrupts */
    HwiP_restore(key);

    object->readCallback(handle, object->readBuf, object->readCount);

    DebugP_log2("UART:(%p) Read canceled, "
                            "%d bytes read",
             ((UARTCC26XX_HWAttrsV2 const *)(handle->hwAttrs))->baseAddr,
               object->readCount);
}

/*
 *  ======== UARTCC26XX_initHW ========
 *  This functions initializes the UART hardware module.
 *
 *  @pre    Function assumes that the UART handle is pointing to a hardware
 *          module which has already been opened.
 */
static void UARTCC26XX_initHw(UART_Handle handle) {
    UARTCC26XX_Object *object;
    UARTCC26XX_HWAttrsV2 const *hwAttrs;
    ClockP_FreqHz freq;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable UART function. */
    UARTDisable(hwAttrs->baseAddr);

    /* Disable all UART module interrupts. */
    UARTIntDisable(hwAttrs->baseAddr, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                      UART_INT_FE | UART_INT_RT | UART_INT_TX |
                                      UART_INT_RX | UART_INT_CTS);

    /* Clear all UART interrupts */
    UARTIntClear(hwAttrs->baseAddr, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                    UART_INT_FE | UART_INT_RT | UART_INT_TX |
                                    UART_INT_RX | UART_INT_CTS);

    /* Set TX interrupt FIFO level and RX interrupt FIFO level */
    UARTFIFOLevelSet(hwAttrs->baseAddr, txFifoThreshold[hwAttrs->txIntFifoThr],
                                        rxFifoThreshold[hwAttrs->rxIntFifoThr]);

    /* Configure frame format and baudrate */
    ClockP_getCpuFreq(&freq);
    UARTConfigSetExpClk(hwAttrs->baseAddr,
                        freq.lo,
                        object->baudRate,
                       (dataLength[object->dataLength] |
                        stopBits[object->stopBits] |
                        parityType[object->parityType]));

    DebugP_log3("UART:(%p) CPU freq: %d; UART baudrate to %d",
                                hwAttrs->baseAddr,
                                freq.lo,
                                object->baudRate);

    /* Enable UART FIFOs */
    HWREG(hwAttrs->baseAddr + UART_O_LCRH) |= UART_LCRH_FEN;

    /* Enable the UART module */
    HWREG(hwAttrs->baseAddr + UART_O_CTL) |= UART_CTL_UARTEN;

    /* If Flow Control is enabled, configure hardware controlled flow control */
    if(isFlowControlEnabled(hwAttrs)) {
        HWREG(hwAttrs->baseAddr + UART_O_CTL) |= (UART_CTL_CTSEN | UART_CTL_RTSEN);
    }
}

/*
 *  ======== UARTCC26XX_initIO ========
 *  This functions initializes the UART IOs.
 *
 *  @pre    Function assumes that the UART handle is pointing to a hardware
 *          module which has already been opened.
 */
static void UARTCC26XX_initIO(UART_Handle handle) {
    /* Locals */
    UARTCC26XX_HWAttrsV2 const *hwAttrs = handle->hwAttrs;

    /* Set IO muxing for the UART pins */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
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
#else
    if (hwAttrs->txPin != GPIO_INVALID_INDEX) {
        GPIO_setMux(hwAttrs->txPin, IOC_PORT_MCU_UART0_TX);
    }
    if (hwAttrs->rxPin != GPIO_INVALID_INDEX) {
        GPIO_setMux(hwAttrs->rxPin, IOC_PORT_MCU_UART0_RX);
    }
    if (isFlowControlEnabled(hwAttrs)) {
        if (hwAttrs->ctsPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(hwAttrs->ctsPin, IOC_PORT_MCU_UART0_CTS);
        }
        if (hwAttrs->rtsPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(hwAttrs->rtsPin, IOC_PORT_MCU_UART0_RTS);
        }
    }
#endif
}

static void UARTCC26XX_finalizeIO(UART_Handle handle)
{
    UARTCC26XX_HWAttrsV2 const *hwAttrs = handle->hwAttrs;

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
 *  ======== uartPostNotify ========
 *  This functions is called to notify the UART driver of an ongoing transition
 *  out of sleep mode.
 *
 *  @pre    Function assumes that the UART handle (clientArg) is pointing to a
 *          hardware module which has already been opened.
 */
static int uartPostNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg)
{
    /* Reconfigure the hardware if returning from sleep */
    if (eventType == PowerCC26XX_AWAKE_STANDBY) {
        UARTCC26XX_initHw((UART_Handle) clientArg);
    }
    return Power_NOTIFYDONE;
}
