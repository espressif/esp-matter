/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_spi.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/udma.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/prcm.h)
#include DeviceFamily_constructPath(driverlib/rom.h)

#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26X4DMA.h>

#define MAX_DMA_TRANSFER_AMOUNT     (1024)

#define SPI_DATASIZE_4                  (4)
#define SPI_DATASIZE_8                  (8)
#define SPI_DATASIZE_16                 (16)
#define SPI_DATASIZE_32                 (32)
#define PARAMS_DATASIZE_MIN         SPI_DATASIZE_4
#define PARAMS_DATASIZE_MAX         SPI_DATASIZE_32

/* API Function Prototypes */
void SPICC26X4DMA_close(SPI_Handle handle);
int_fast16_t SPICC26X4DMA_control(SPI_Handle handle,
                                  uint_fast16_t cmd,
                                  void *arg);
void SPICC26X4DMA_init(SPI_Handle handle);
SPI_Handle SPICC26X4DMA_open(SPI_Handle handle, SPI_Params *params);
static void SPICC26X4DMA_swiFxn (uintptr_t arg0, uintptr_t arg1);
bool SPICC26X4DMA_transfer(SPI_Handle handle, SPI_Transaction *transaction);
void SPICC26X4DMA_transferCancel(SPI_Handle handle);

/* Local Function Prototypes */
static void blockingTransferCallback(SPI_Handle handle,
                                     SPI_Transaction *msg);
static void configNextTransfer(SPICC26X4DMA_Object *object,
                               SPICC26X4DMA_HWAttrs const *hwAttrs);
static void csnCallback(uint_least8_t);
static void flushFifos(SPICC26X4DMA_HWAttrs const *hwAttrs);
static inline uint32_t getDmaChannelNumber(uint32_t x);
static bool initHw(SPI_Handle handle);
static void initIO(SPI_Handle handle);
static void finalizeIO(SPI_Handle handle);
static inline void primeTransfer(SPICC26X4DMA_Object *object,
                                 SPICC26X4DMA_HWAttrs const *hwAttrs);
static inline void releaseConstraint(uint32_t txBufAddr);
static inline void setConstraint(uint32_t txBufAddr);
static inline void spiPollingTransfer(SPICC26X4DMA_Object *object,
                                      SPICC26X4DMA_HWAttrs const *hwAttrs,
                                      SPI_Transaction *transaction);
static int spiPostNotify(unsigned int eventType,
                         uintptr_t eventArg,
                         uintptr_t clientArg);
static inline bool spiBusy(SPICC26X4DMA_Object *object,
                           SPICC26X4DMA_HWAttrs const *hwAttrs);
static inline void disableSPI(uint32_t baseAddr);
static inline void enableSPI(uint32_t baseAddr);
static inline void enableInterrupt(uint32_t baseAddr, uint32_t irqs);
static inline void disableInterrupt(uint32_t baseAddr, uint32_t irqs);
static inline void clearInterrupt(uint32_t baseAddr, uint32_t irqs);
static void enableDMA(uint32_t baseAddr, uint32_t dmaFlags);
static void disableDMA(uint32_t baseAddr, uint32_t dmaFlags);
static inline uint32_t getInterruptStatus(uint32_t baseAddr, bool masked);
static bool configSPI(uint32_t baseAddr, uint32_t freq, uint32_t format,
                          uint32_t mode, uint32_t bitRate, uint32_t dataSize);
static int32_t dataPutNonBlocking(uint32_t baseAddr, uint32_t frame);
static void dataGet(uint32_t baseAddr, uint32_t *frame);
static bool isSPIbusy(uint32_t baseAddr);


/* RX FIFO over flowed, data was lost */
#define SPI_INT_RXOF        (SPI_MIS_RXFIFO_OVF_SET)
/* RX FIFO trigger level was passed */
#define SPI_INT_RXFF        (SPI_MIS_RX_SET)
/* TX FIFO trigger level was passed */
#define SPI_INT_TXFF        (SPI_MIS_TX_SET)
/* Transfer complete, HW is idle */
#define SPI_INT_IDLE        (SPI_MIS_IDLE_SET)
/* Subset of SPI interrupts used by this driver */
#define SPI_INT_SUBSET      (SPI_INT_TXFF | SPI_INT_RXFF | SPI_INT_RXOF | \
                             SPI_INT_IDLE)
/* All SPI interrupts */
#define SPI_INT_ALL         (SPI_INT_SUBSET | \
                             SPI_MIS_TXEMPTY_SET | \
                             SPI_MIS_PER_SET | \
                             SPI_MIS_RTOUT_SET | \
                             SPI_MIS_DMA_DONE_RX_SET | \
                             SPI_MIS_DMA_DONE_TX_SET)

/* SPI function table for SPICC26X4DMA implementation */
const SPI_FxnTable SPICC26X4DMA_fxnTable = {
    SPICC26X4DMA_close,
    SPICC26X4DMA_control,
    SPICC26X4DMA_init,
    SPICC26X4DMA_open,
    SPICC26X4DMA_transfer,
    SPICC26X4DMA_transferCancel
};

/* Mapping SPI mode from generic driver to CC26X4 */
static const uint32_t mode[] = {
    /* SPI_MASTER */
    SPI_CTL1_MS_ENABLE,
    /* SPI_SLAVE */
    SPI_CTL1_MS_DISABLE,
};

/* Mapping SPI frame format from generic driver to CC26XX driverlib */
static const uint32_t frameFormat[] = {
    /* SPI_POL0_PHA0 */
    (SPI_CTL0_FRF_MOTOROLA_3WIRE |
     SPI_CTL0_SPO_LOW |
     SPI_CTL0_SPH_FIRST),
    /* SPI_POL0_PHA1 */
    (SPI_CTL0_FRF_MOTOROLA_3WIRE |
     SPI_CTL0_SPO_LOW |
     SPI_CTL0_SPH_SECOND),
    /* SPI_POL1_PHA0 */
    (SPI_CTL0_FRF_MOTOROLA_3WIRE |
     SPI_CTL0_SPO_HIGH |
     SPI_CTL0_SPH_FIRST),
    /* SPI_POL1_PHA1 */
    (SPI_CTL0_FRF_MOTOROLA_3WIRE |
     SPI_CTL0_SPO_HIGH |
     SPI_CTL0_SPH_SECOND),
    /* SPI_TI */
    (SPI_CTL0_FRF_TI_SYNC),
    /* SPI_MW */
    (SPI_CTL0_FRF_MIRCOWIRE),
};

/*
 * These lookup tables are used to configure the DMA channels for the
 * appropriate (8bit/16bit/32bit) transfer sizes.
 */
static const uint32_t dmaTxConfig[] = {
    UDMA_MODE_PINGPONG | UDMA_SIZE_8  | UDMA_SRC_INC_8  | UDMA_DST_INC_NONE |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_32 | UDMA_SRC_INC_32 | UDMA_DST_INC_NONE |
    UDMA_ARB_4
};

static const uint32_t dmaRxConfig[] = {
    UDMA_MODE_PINGPONG | UDMA_SIZE_8  | UDMA_SRC_INC_NONE | UDMA_DST_INC_8  |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_32 |
    UDMA_ARB_4
};

static const uint32_t dmaNullConfig[] = {
    UDMA_MODE_PINGPONG | UDMA_SIZE_8  | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE |
    UDMA_ARB_4,
    UDMA_MODE_PINGPONG | UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE |
    UDMA_ARB_4
};

/*
 *  ======== SPICC26X4DMA_close ========
 */
void SPICC26X4DMA_close(SPI_Handle handle)
{
    SPICC26X4DMA_Object          *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Disable the SPI */
    disableSPI(hwAttrs->baseAddr);

    HwiP_destruct(&(object->hwi));

    UDMACC26XX_close(object->udmaHandle);

    SwiP_destruct(&(object->swi));

    if (object->transferMode == SPI_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->transferComplete));
    }

    finalizeIO(handle);

    Power_releaseDependency(hwAttrs->powerMngrId);

    Power_unregisterNotify(&object->spiPostObj);

    object->isOpen = false;
}

/*!
 *  ======== SPICC26X4DMA_control ========
 *  @brief  Function for setting control parameters of the SPI driver
 *          after it has been opened.
 *
 *  @pre    SPICC26X4DMA_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle A SPI handle returned from SPICC26X4DMA_open()
 *
 *  @param  cmd  The command to execute, supported commands are:
 *  | Command                                   | Description                  |
 *  |-------------------------------------------|------------------------------|
 *  | ::SPICC26X4DMA_CMD_RETURN_PARTIAL_ENABLE  | Enable RETURN_PARTIAL        |
 *  | ::SPICC26X4DMA_CMD_RETURN_PARTIAL_DISABLE | Disable RETURN_PARTIAL       |
 *  | ::SPICC26X4DMA_CMD_SET_CSN_PIN            | Re-configure chip select pin |
 *  | ::SPICC26X4DMA_CMD_SET_MANUAL             | Enable manual start mode     |
 *  | ::SPICC26X4DMA_CMD_CLR_MANUAL             | Disable manual start mode    |
 *  | ::SPICC26X4DMA_CMD_MANUAL_START           | Perform a manual start       |
 *
 *  @param  *arg  Pointer to command arguments.
 *
 *  @return ::SPI_STATUS_SUCCESS if success, or error code if error.
 */
int_fast16_t SPICC26X4DMA_control(SPI_Handle handle, uint_fast16_t cmd, void *arg)
{
    SPICC26X4DMA_Object         *object = handle->object;
    SPICC26X4DMA_HWAttrs const  *hwAttrs = handle->hwAttrs;
    uint_least8_t                pinIndex;

    /* Initialize return value */
    int ret = SPI_STATUS_ERROR;

    /* Perform command */
    switch(cmd) {
        case SPICC26X4DMA_CMD_RETURN_PARTIAL_ENABLE:
            /* Enable RETURN_PARTIAL if slave mode is enabled */

            if(object->mode == SPI_SLAVE){
                object->returnPartial = SPICC26X4DMA_retPartEnabledIntNotSet;
                ret = SPI_STATUS_SUCCESS;
            }
            else{
                /* Partial return not available in master mode. */
                ret = SPI_STATUS_ERROR;
            }
            break;

        case SPICC26X4DMA_CMD_RETURN_PARTIAL_DISABLE:
            GPIO_setInterruptConfig(object->csnPin, GPIO_CFG_INT_DISABLE);
            object->returnPartial = SPICC26X4DMA_retPartDisabled;
            ret = SPI_STATUS_SUCCESS;
            break;

        case SPICC26X4DMA_CMD_SET_CSN_PIN:
            pinIndex = *((uint_least8_t*) arg);

            if (pinIndex == GPIO_INVALID_INDEX) {
                /* If trying to disable the CS pin, forward to the CLEAR_CSN command and break */
                ret = SPICC26X4DMA_control(handle, SPICC26X4DMA_CMD_CLEAR_CSN_PIN, NULL);
                break;
            }

            /* Reset the previous CS pin and configure the new one */
            GPIO_resetConfig(object->csnPin);
            object->csnPin = pinIndex;
            GPIO_setMux(object->csnPin, hwAttrs->csnPinMux);

            ret = SPI_STATUS_SUCCESS;

        case SPICC26X4DMA_CMD_CLEAR_CSN_PIN:
            /* If the CS pin is assigned, reset and unassign it */
            if (object->csnPin != GPIO_INVALID_INDEX) {
                GPIO_resetConfig(object->csnPin);
                object->csnPin = GPIO_INVALID_INDEX;
            }
            ret = SPI_STATUS_SUCCESS;
            break;

        case SPICC26X4DMA_CMD_SET_MANUAL:
            /* If a transaction is queued, do not modify */
            if (object->headPtr == NULL) {
                object->manualStart = true;
                ret = SPI_STATUS_SUCCESS;
            }
            break;
        case SPICC26X4DMA_CMD_CLR_MANUAL:
            /* If a transaction is queued, do not modify */
            if (object->headPtr == NULL) {
                object->manualStart = false;
                ret = SPI_STATUS_SUCCESS;
            }
            break;
        case SPICC26X4DMA_CMD_MANUAL_START:
            if (object->headPtr != NULL &&
                object->manualStart) {
                enableDMA(hwAttrs->baseAddr, SPI_DMACR_TXDMAE | SPI_DMACR_RXDMAE);
                UDMACC26XX_channelEnable(object->udmaHandle,
                                         hwAttrs->rxChannelBitMask |
                                         hwAttrs->txChannelBitMask);
                enableSPI(hwAttrs->baseAddr);
                ret = SPI_STATUS_SUCCESS;
            }
            break;
        default:
            /* This command is not defined */
            ret = SPI_STATUS_UNDEFINEDCMD;
            break;
    }

    return (ret);
}

/*
 *  ======== SPICC26X4DMA_hwiFxn ========
 */
static void SPICC26X4DMA_hwiFxn (uintptr_t arg)
{
    uint32_t                      freeChannel;
    uint32_t                      intStatus;
    SPI_Transaction              *completedList;
    size_t                       *transferSize;
    volatile tDMAControlTable    *rxDmaTableEntry;
    volatile tDMAControlTable    *txDmaTableEntry;
    SPICC26X4DMA_Object          *object = ((SPI_Handle) arg)->object;
    SPICC26X4DMA_HWAttrs const   *hwAttrs = ((SPI_Handle) arg)->hwAttrs;
    uint8_t                       i;

    intStatus = getInterruptStatus(hwAttrs->baseAddr, true);
    clearInterrupt(hwAttrs->baseAddr, intStatus);

    if (intStatus & SPI_MIS_RXFIFO_OVF_SET) {
        if (object->headPtr != NULL) {
            /*
             * RX overrun during a transfer; mark the current transfer
             * as failed & cancel all remaining transfers.
             */
            object->headPtr->status = SPI_TRANSFER_FAILED;

            SPICC26X4DMA_transferCancel((SPI_Handle) arg);
        }
        else {
            disableSPI(hwAttrs->baseAddr);

            /* Disable DMA and clear DMA interrupts */
            disableDMA(hwAttrs->baseAddr, SPI_DMACR_TXDMAE | SPI_DMACR_RXDMAE);
            UDMACC26XX_channelDisable(object->udmaHandle,
                                      hwAttrs->rxChannelBitMask |
                                      hwAttrs->txChannelBitMask);
            UDMACC26XX_clearInterrupt(object->udmaHandle,
                                      hwAttrs->rxChannelBitMask |
                                      hwAttrs->txChannelBitMask);
            disableInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);
            clearInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);

            /* Clear out the FIFO by resetting SPI module and re-initting */
            flushFifos(hwAttrs);
        }
    }
    else {
        UDMACC26XX_clearInterrupt(object->udmaHandle,
                                  hwAttrs->rxChannelBitMask |
                                  hwAttrs->txChannelBitMask);

        /*
         * We check both channels for completion; this is done in case the
         * second channel finishes while we are still configuring the first.
         */
        for (i = 0; i < 2; i++) {
            if (object->headPtr == NULL){
                /* When i was 0, we finished the last transaction */
                break;
            }

            if (object->activeChannel == UDMA_PRI_SELECT) {
                transferSize = &object->priTransferSize;

                rxDmaTableEntry = hwAttrs->dmaRxTableEntryPri;
                txDmaTableEntry = hwAttrs->dmaTxTableEntryPri;
            }
            else {
                transferSize = &object->altTransferSize;

                rxDmaTableEntry = hwAttrs->dmaRxTableEntryAlt;
                txDmaTableEntry = hwAttrs->dmaTxTableEntryAlt;
            }

            /*
             * The SPI TX FIFO continuously requests the DMA to fill it if there
             * is space available.  If there are no more frames to put in the
             * FIFO we run into a situation where DMA TX will cause undesired
             * interrupts.  To prevent many undesired interrupts disable DMA_TX
             * if there are no more frames to load into the FIFO & there are no
             * pending queued transactions.
             */
            if (UDMACC26XX_channelDone(object->udmaHandle, hwAttrs->txChannelBitMask) &&
               (txDmaTableEntry->ui32Control & UDMA_MODE_M) == UDMA_MODE_STOP &&
                object->framesQueued == object->headPtr->count &&
                object->headPtr->nextPtr == NULL){
                    disableDMA(hwAttrs->baseAddr, SPI_DMACR_TXDMAE);
                    UDMACC26XX_clearInterrupt(object->udmaHandle,
                                              hwAttrs->txChannelBitMask);
            }

            if((rxDmaTableEntry->ui32Control & UDMA_MODE_M) == UDMA_MODE_STOP &&
                *transferSize != 0) {
                object->framesTransferred += *transferSize;
                freeChannel = object->activeChannel;
                object->activeChannel = (freeChannel == UDMA_PRI_SELECT) ?
                    UDMA_ALT_SELECT : UDMA_PRI_SELECT;

                /*
                 * Set the channel's transfer size to 0; 0 lets
                 * configNextTransfer() know that there is a free channel.
                 */
                *transferSize = 0;

                if ((object->framesQueued) < (object->headPtr->count) ||
                    (object->framesTransferred) < (object->headPtr->count)) {
                    /*
                     * In this case we need to reconfigure the channel to
                     * continue transferring frames. configNextTransfer() will
                     * continue queuing frames for the current transfer or
                     * start the following transaction if necessary.
                     */
                    configNextTransfer(object, hwAttrs);

                    if (object->manualStart &&
                        UDMACC26XX_channelDone(object->udmaHandle, hwAttrs->txChannelBitMask)) {
                        /* Ping pong flow was broken, restart */
                        UDMACC26XX_channelEnable(object->udmaHandle,
                                                 hwAttrs->txChannelBitMask);
                    }
                }
                else {
                    /*
                     * All data has been transferred for the current
                     * transaction. Set status & move the transaction to
                     * object->completedList. This is required because
                     * object->headPtr is moved to the following transaction.
                     * Also, transaction callbacks are executed in the driver
                     * SWI which will be posted later.
                     */
                    object->headPtr->status = SPI_TRANSFER_COMPLETED;

                    if (object->completedTransfers == NULL) {
                        /* List is empty; just add the transaction */
                        object->completedTransfers = object->headPtr;
                        completedList = object->completedTransfers;
                    }
                    else {
                        /* Traverse to the last element */
                        completedList = object->completedTransfers;
                        while (completedList->nextPtr != NULL) {
                            completedList = completedList->nextPtr;
                        }

                        /* Store the completed transaction at end of list */
                        completedList->nextPtr = object->headPtr;

                        /*
                         * Make sure we are pointing to the end of the list;
                         * we need to clear references in completed transfer
                         * after we move object->headPtr forward.
                         */
                        completedList = completedList->nextPtr;
                    }

                    /* Move the object->headPtr to the next transaction */
                    object->headPtr = object->headPtr->nextPtr;

                    /* Clear references in completed transfer */
                    completedList->nextPtr = NULL;

                    /* Update object variables for the following transfer. */
                    object->framesQueued =
                        (object->activeChannel == UDMA_PRI_SELECT) ?
                        object->priTransferSize : object->altTransferSize;
                    object->framesTransferred = 0;

                    if (object->headPtr != NULL) {
                        /* Reconfigure channel for following transaction */
                        configNextTransfer(object, hwAttrs);

                        if (object->manualStart &&
                            UDMACC26XX_channelDone(object->udmaHandle, hwAttrs->txChannelBitMask)) {
                            /* Ping pong flow was broken, restart */
                            UDMACC26XX_channelEnable(object->udmaHandle,
                                                     hwAttrs->txChannelBitMask);
                        }
                    }
                    else {
                        /* No more queued transfers; disable DMA & SPI */
                        disableDMA(hwAttrs->baseAddr,
                                      SPI_DMACR_TXDMAE |
                                      SPI_DMACR_RXDMAE);

                        /*
                         * For this driver implementation the peripheral is kept
                         * active until either a FIFO-overrun occurs or
                         * SPI_transferCancel() is executed.
                         */
                    }

                    /* Post driver SWI to execute transaction callbacks */
                    SwiP_post(&(object->swi));
                }
            }
        }
    }
}

/*
 *  ======== SPICC26X4DMA_init ========
 */
void SPICC26X4DMA_init(SPI_Handle handle)
{
    ((SPICC26X4DMA_Object *) handle->object)->isOpen = false;
}

/*
 *  ======== SPICC26X4DMA_open ========
 */
SPI_Handle SPICC26X4DMA_open(SPI_Handle handle, SPI_Params *params)
{
    union {
        HwiP_Params               hwiParams;
        SwiP_Params               swiParams;
    } paramsUnion;
    uint32_t                      key;
    SPICC26X4DMA_Object          *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;

    key = HwiP_disable();

    /* Failure conditions */
    if (object->isOpen ||
        params->dataSize > PARAMS_DATASIZE_MAX ||
        params->dataSize < PARAMS_DATASIZE_MIN) {
        HwiP_restore(key);

        return (NULL);
    }
    object->isOpen = true;

    HwiP_restore(key);

    DebugP_assert((params->dataSize >= PARAMS_DATASIZE_MIN) && (params->dataSize <= PARAMS_DATASIZE_MAX));

    object->bitRate = params->bitRate;
    object->dataSize = params->dataSize;
    object->mode = params->mode;
    object->transferMode = params->transferMode;
    object->transferTimeout = params->transferTimeout;
    object->returnPartial = SPICC26X4DMA_retPartDisabled;
    object->headPtr = NULL;
    object->tailPtr = NULL;
    object->completedTransfers = NULL;
    object->format = params->frameFormat;
    object->txScratchBuf = hwAttrs->defaultTxBufValue;
    object->busyBit = (params->mode == SPI_MASTER ? SPI_STAT_BUSY : SPI_STAT_TFE);
    object->manualStart = false;

    Power_setDependency(hwAttrs->powerMngrId);

    /*
     * Configure IOs after hardware has been initialized so that IOs aren't
     * toggled unnecessary
     */
    if (!initHw(handle)) {
        /*
         * Trying to use SPI driver when some other driver or application
         * has already allocated these pins, error!
         */
        Power_releaseDependency(hwAttrs->powerMngrId);

        object->isOpen = false;
        return (NULL);
    }

    initIO(handle);

    /* CSN is initialized using hwAttrs, but can be re-configured later */
    object->csnPin = hwAttrs->csnPin;

    HwiP_Params_init(&paramsUnion.hwiParams);
    paramsUnion.hwiParams.arg = (uintptr_t) handle;
    paramsUnion.hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi),
                   (int) hwAttrs->intNum, SPICC26X4DMA_hwiFxn,
                   &paramsUnion.hwiParams);

    SwiP_Params_init(&paramsUnion.swiParams);
    paramsUnion.swiParams.arg0 = (uintptr_t)handle;
    paramsUnion.swiParams.priority = hwAttrs->swiPriority;
    SwiP_construct(&(object->swi),
                   SPICC26X4DMA_swiFxn,
                   &(paramsUnion.swiParams));

    object->udmaHandle = UDMACC26XX_open();

    Power_registerNotify(&object->spiPostObj,
                         PowerCC26XX_AWAKE_STANDBY,
                         (Power_NotifyFxn) spiPostNotify,
                         (uint32_t) handle);

    if (object->transferMode == SPI_MODE_BLOCKING) {
        /*
         * Create a semaphore to block task execution for the duration of the
         * SPI transfer
         */
        SemaphoreP_constructBinary(&(object->transferComplete), 0);
        object->transferCallbackFxn = blockingTransferCallback;
    }
    else {
        DebugP_assert(params->transferCallbackFxn != NULL);
        object->transferCallbackFxn = params->transferCallbackFxn;
    }

    return (handle);
}

/*
 *  ======== SPICC26X4DMA_swiFxn ========
 */
static void SPICC26X4DMA_swiFxn(uintptr_t arg0, uintptr_t arg1) {
    SPI_Transaction     *transaction;
    SPICC26X4DMA_Object *object = ((SPI_Handle) arg0)->object;

    while (object->completedTransfers != NULL) {
        transaction = object->completedTransfers;

        /* Move object->completedTransfers to the next transaction */
        object->completedTransfers = object->completedTransfers->nextPtr;

        transaction->nextPtr = NULL;

        /* Execute callback function for completed transfer */
        object->transferCallbackFxn((SPI_Handle) arg0, transaction);
    }
}

/*
 *  ======== SPICC26X4DMA_transfer ========
 */
bool SPICC26X4DMA_transfer(SPI_Handle handle, SPI_Transaction *transaction)
{
    uint8_t                       alignMask;
    bool                          buffersAligned;
    uintptr_t                     key;
    SPICC26X4DMA_Object          *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (transaction->count == 0) {
        return (false);
    }

    key = HwiP_disable();

    /*
     * Make sure that the buffers are aligned properly.
     * alignMask is used to check if the RX/TX buffers addresses
     * are aligned to the frameSize.
     */
    if (object->dataSize <= SPI_DATASIZE_8) {
        alignMask = 0;
    } else if (object->dataSize <= SPI_DATASIZE_16) {
        alignMask = 1;
    }
    else {
        alignMask = 3;
    }

    buffersAligned = ((((uint32_t) transaction->rxBuf & alignMask) == 0) &&
                     (((uint32_t) transaction->txBuf & alignMask) == 0));

    if (!buffersAligned ||
        (object->headPtr && object->transferMode == SPI_MODE_BLOCKING)) {
        transaction->status = SPI_TRANSFER_FAILED;

        HwiP_restore(key);

        return (false);
    }
    else {
        if (object->headPtr) {
            object->tailPtr->nextPtr = transaction;
            object->tailPtr = transaction;
            object->tailPtr->status = SPI_TRANSFER_QUEUED;
        }
        else {
            object->headPtr = transaction;
            object->tailPtr = transaction;

            object->framesQueued = 0;
            object->framesTransferred = 0;
            object->priTransferSize = 0;
            object->altTransferSize = 0;
            object->tailPtr->status =
                    (object->returnPartial != SPICC26X4DMA_retPartDisabled) ?
                     SPI_TRANSFER_PEND_CSN_ASSERT :
                     SPI_TRANSFER_STARTED;
        }

        object->tailPtr->nextPtr = NULL;
    }

    /* In slave mode, optionally enable callback on CSN de-assert */
    if (object->returnPartial == SPICC26X4DMA_retPartEnabledIntNotSet) {
        object->returnPartial = SPICC26X4DMA_retPartEnabledIntSet;
        GPIO_setInterruptConfig(object->csnPin, GPIO_CFG_IN_INT_BOTH_EDGES);
    }

    /* Set constraints to guarantee transaction */
    setConstraint((uint32_t)transaction->txBuf);

    /*
     * Polling transfer if BLOCKING mode & transaction->count < threshold
     * Slaves not allowed to use polling unless timeout is disabled
     * Polling not allowed with returnPartial mode
     */
    if (object->transferMode == SPI_MODE_BLOCKING &&
        transaction->count < hwAttrs->minDmaTransferSize &&
        object->returnPartial == SPICC26X4DMA_retPartDisabled &&
        (object->mode == SPI_MASTER ||
        object->transferTimeout == SPI_WAIT_FOREVER)) {
        HwiP_restore(key);

        spiPollingTransfer(object, hwAttrs, transaction);

        /* Release constraint since transaction is done */
        releaseConstraint((uint32_t) transaction->txBuf);

        /* Transaction completed; set status & mark SPI ready */
        object->headPtr->status = SPI_TRANSFER_COMPLETED;
        object->headPtr = NULL;
        object->tailPtr = NULL;
    }
    else {
        /*
         * Perform a DMA backed SPI transfer; we need exclusive access while
         * priming the transfer to prevent race conditions with
         * SPICC26X4DMA_transferCancel().
         */

        /* Enable DMA interrupts */
        clearInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);
        enableInterrupt(hwAttrs->baseAddr, SPI_MIS_DMA_DONE_TX_SET | SPI_MIS_DMA_DONE_RX_SET);

        primeTransfer(object, hwAttrs);

        /* Enable the RX overrun interrupt in the SPI module */
        enableInterrupt(hwAttrs->baseAddr, SPI_MIS_RXFIFO_OVF_SET);

        HwiP_restore(key);

        if (object->transferMode == SPI_MODE_BLOCKING) {
            if (SemaphoreP_OK != SemaphoreP_pend(&(object->transferComplete),
                                                 object->transferTimeout)) {
                /* Timeout occurred; cancel the transfer */
                object->headPtr->status = SPI_TRANSFER_FAILED;
                SPICC26X4DMA_transferCancel(handle);

                /*
                 * SPICC26X4DMA_transferCancel() performs callback which posts
                 * transferComplete semaphore. This call consumes this extra
                 * post.
                 */
                SemaphoreP_pend(&(object->transferComplete),
                                SemaphoreP_NO_WAIT);

                return (false);
            }
        }
    }
    return (true);
}

/*
 *  ======== SPICC26X4DMA_transferCancel ========
 */
void SPICC26X4DMA_transferCancel(SPI_Handle handle) {
    uintptr_t                    key;
    uint32_t                     temp;
    SPI_Transaction              *tempPtr;
    SPICC26X4DMA_Object          *object = handle->object;
    SPICC26X4DMA_HWAttrs const   *hwAttrs = handle->hwAttrs;

    /*
     * Acquire exclusive access to the driver. Required to prevent race
     * conditions if preempted by code trying to configure another transfer.
     */
    key = HwiP_disable();

    if (object->headPtr == NULL) {

        /*
         * Disable the SPI peripheral in case the peripherals finite state
         * machine is in a bad state. Calling SPI_transfer() will re-enable
         * the peripheral.
         */
        disableSPI(hwAttrs->baseAddr);
        HwiP_restore(key);

        return;
    }

    /*
     * There are 2 use cases in which to call transferCancel():
     *   1.  The driver is in CALLBACK mode.
     *   2.  The driver is in BLOCKING mode & there has been a transfer timeout.
     */
    if (object->transferMode != SPI_MODE_BLOCKING ||
        object->headPtr->status == SPI_TRANSFER_FAILED ||
        object->headPtr->status == SPI_TRANSFER_CSN_DEASSERT) {

        /* Prevent interrupt while canceling the transfer */
        HwiP_disableInterrupt(hwAttrs->intNum);

        /*
         * Disable the TX DMA channel first to stop feeding more frames to
         * the FIFO.  Next, wait until the TX FIFO is empty (all frames in
         * FIFO have been sent).  RX DMA channel is disabled later to allow
         * the DMA to move all frames already in FIFO to memory.
         */
        UDMACC26XX_channelDisable(object->udmaHandle,
                                  hwAttrs->txChannelBitMask);

        if (object->mode == SPI_MASTER) {
            /*
             * Wait until the TX FIFO is empty; this is to make sure the
             * chip select is deasserted before disabling the SPI.
             */
            while (isSPIbusy(hwAttrs->baseAddr)) {}
        }

        disableSPI(hwAttrs->baseAddr);

        /* Now disable the RX, DMA & interrupts */
        UDMACC26XX_channelDisable(object->udmaHandle,
                                  hwAttrs->rxChannelBitMask);
        disableDMA(hwAttrs->baseAddr,
                      SPI_DMACR_TXDMAE | SPI_DMACR_RXDMAE);
        UDMACC26XX_clearInterrupt(object->udmaHandle,
                                  hwAttrs->rxChannelBitMask |
                                  hwAttrs->txChannelBitMask);
        disableInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);
        clearInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);

        /*
         * Update transaction->count with the amount of frames which have
         * been transferred.
         */
        object->headPtr->count = object->framesTransferred;
        if (object->priTransferSize) {
            temp = UDMACC26XX_GET_TRANSFER_SIZE(hwAttrs->dmaRxTableEntryPri->ui32Control);

            if (temp <= object->priTransferSize) {
                object->headPtr->count += (object->priTransferSize - temp);
             }
        }

        if (object->altTransferSize) {
            temp =
            UDMACC26XX_GET_TRANSFER_SIZE(hwAttrs->dmaRxTableEntryAlt->ui32Control);

            if (temp <= object->altTransferSize) {
                object->headPtr->count += (object->altTransferSize - temp);
            }
        }

        /*
         * Disables peripheral, clears all registers & reinitializes it to
         * parameters used in SPI_open()
         */
        initHw(handle);

        HwiP_clearInterrupt(hwAttrs->intNum);
        HwiP_enableInterrupt(hwAttrs->intNum);

        /*
         * Go through all queued transfers; set status CANCELED (if we did
         * not cancel due to timeout).  The object->headPtr->count is
         * stored/restored temporarily.
         */
        temp = object->headPtr->count;
        tempPtr = object->headPtr;

        while (tempPtr != NULL) {
            if (tempPtr->status != SPI_TRANSFER_FAILED &&
                tempPtr->status != SPI_TRANSFER_CSN_DEASSERT) {
                tempPtr->status = SPI_TRANSFER_CANCELED;
            }

            tempPtr->count = 0;
            tempPtr = tempPtr->nextPtr;
        }
        object->headPtr->count = temp;

        /* Add all cancelled transactions to object->completedTransfers */
        tempPtr = object->completedTransfers;
        if (tempPtr == NULL) {
            /* Empty list; just add all of the cancelled transactions */
            object->completedTransfers = object->headPtr;
        }
        else {
            /* Move through the list until we reach the last element */
            while (tempPtr->nextPtr != NULL) {
                tempPtr = tempPtr->nextPtr;
            }

            /* Add all of the cancelled transactions */
            tempPtr->nextPtr = object->headPtr;
        }

        /* Clear all driver object variables*/
        object->headPtr = NULL;
        object->tailPtr = NULL;
        object->framesQueued = 0;
        object->framesTransferred = 0;
        object->priTransferSize = 0;
        object->altTransferSize = 0;

        HwiP_restore(key);

        /*
         * All transactions have been marked as cancelled & added to
         * object->completedTransfers.  Post the driver SWI to execute
         * callback functions.
         */
        SwiP_post(&(object->swi));

        /* Must return here; do not call HwiP_restore() twice */
        return;
    }

    HwiP_restore(key);
}

/*
 *  ======== blockingTransferCallback ========
 */
static void blockingTransferCallback(SPI_Handle handle, SPI_Transaction *msg)
{
    SPICC26X4DMA_Object *object = handle->object;

    SemaphoreP_post(&(object->transferComplete));
}

/*
 *  ======== configNextTransfer ========
 *  This function must be executed with interrupts disabled.
 */
static void configNextTransfer(SPICC26X4DMA_Object *object,
                               SPICC26X4DMA_HWAttrs const *hwAttrs)
{
    size_t                     framesQueued;
    uint32_t                   transferAmt;
    SPI_Transaction           *transaction;
    volatile tDMAControlTable *rxDmaTableEntry;
    volatile tDMAControlTable *txDmaTableEntry;
    uint8_t                    index;
    uint8_t                    optionsIndex;

    /*
     * The DMA options vary according to data frame size; options for 8-bit
     * data (or smaller) are in index 0.  Options for 16-bit frame sizes are
     * in index 1, option for 32-bit frame sizes are in index 2
     */
    if (object->dataSize <= SPI_DATASIZE_8 ) {
        index = 0;
        optionsIndex =1;
    }
    else if (object->dataSize <= SPI_DATASIZE_16 ) {
        index = 1;
        optionsIndex = 2;
    }
    else {
        index = 2;
        optionsIndex = 4;
    }

    /*
     * object->framesQueued keeps track of how many frames (of the current
     * transaction) have been configured for DMA transfer.  If
     * object->framesQueued == transaction->count; all frames have been queued
     * & we should configure the free DMA channel to send the next transaction.
     * When the current transaction has completed; object->framesQueued
     * will be updated (in the ISR) to reflect the amount of frames queued
     * of the following transaction.
     */
    transaction = object->headPtr;
    if (object->framesQueued < transaction->count) {
        framesQueued = object->framesQueued;
    }
    else {
        transaction = object->headPtr->nextPtr;
        if (transaction == NULL) {
            /* There are no queued transactions */
            return;
        }

        framesQueued = 0;
        transaction->status = SPI_TRANSFER_STARTED;
    }

    /*
     * The DMA has a max transfer amount of 1024.  If the transaction is
     * greater; we must transfer it in chunks.  framesQueued keeps track of
     * how much data has been queued for transfer.
     */
    if ((transaction->count - framesQueued) > MAX_DMA_TRANSFER_AMOUNT) {
        transferAmt = MAX_DMA_TRANSFER_AMOUNT;
    }
    else {
        transferAmt = transaction->count - framesQueued;
    }

    /* Determine free channel & mark it as used by setting transfer size */
    if (object->priTransferSize == 0) {
        object->priTransferSize = transferAmt;

        rxDmaTableEntry = hwAttrs->dmaRxTableEntryPri;
        txDmaTableEntry = hwAttrs->dmaTxTableEntryPri;
    }
    else {
        object->altTransferSize = transferAmt;

        rxDmaTableEntry = hwAttrs->dmaRxTableEntryAlt;
        txDmaTableEntry = hwAttrs->dmaTxTableEntryAlt;
    }

    /* Setup the TX transfer buffers & characteristics */
    if (transaction->txBuf) {
        txDmaTableEntry->ui32Control = dmaTxConfig[index];

        /*
         * Add an offset for the amount of data transfered.  The offset is
         * calculated by: object->framesQueued * (optionsIndex).  This
         * accounts for 8 or 16-bit sized transfers.
         */
        txDmaTableEntry->pvSrcEndAddr =
            (void *)((uint32_t) transaction->txBuf +
            (uint32_t) (framesQueued * optionsIndex) +
            (transferAmt << index) - 1);
    }
    else {
        txDmaTableEntry->ui32Control = dmaNullConfig[index];
        txDmaTableEntry->pvSrcEndAddr = (void *) &(object->txScratchBuf);
    }
    txDmaTableEntry->pvDstEndAddr = (void *) (hwAttrs->baseAddr + SPI_O_TXDATA);
    txDmaTableEntry->ui32Control |=
        UDMACC26XX_SET_TRANSFER_SIZE((uint16_t) transferAmt);

    /* Setup the RX transfer buffers & characteristics */
    if (transaction->rxBuf) {
        rxDmaTableEntry->ui32Control = dmaRxConfig[index];

        /*
         * Add an offset for the amount of data transfered.  The offset is
         * calculated by: object->framesQueued * (optionsIndex).  This
         * accounts for 8 or 16-bit sized transfers.
         */

        rxDmaTableEntry->pvDstEndAddr =
            (void *) ((uint32_t) transaction->rxBuf +
            (uint32_t) (framesQueued * optionsIndex) +
            (transferAmt << index) - 1);
    }
    else {
        rxDmaTableEntry->ui32Control = dmaNullConfig[index];
        rxDmaTableEntry->pvDstEndAddr = &object->rxScratchBuf;
    }
    rxDmaTableEntry->pvSrcEndAddr = (void *) (hwAttrs->baseAddr + SPI_O_RXDATA);
    rxDmaTableEntry->ui32Control |=
        UDMACC26XX_SET_TRANSFER_SIZE((uint16_t) transferAmt);

    if (transaction == object->headPtr) {
        /*
         * Only update object->framesQueued if we are configuring a DMA
         * channel for the current transaction.
         */
        object->framesQueued += transferAmt;
    }

    if (!object->manualStart) {
        /* Enable DMA to generate interrupt on SPI peripheral */
        enableDMA(hwAttrs->baseAddr, SPI_DMACR_TXDMAE | SPI_DMACR_RXDMAE);
        UDMACC26XX_channelEnable(object->udmaHandle,
                                 hwAttrs->rxChannelBitMask |
                                 hwAttrs->txChannelBitMask);
    }

    return;
}

/*
 *  ======== csnCallback ========
 *  Slave mode optional callback function for when the CSN is asserted &
 *  deasserted.
 */
static void csnCallback(uint_least8_t index)
{
    uintptr_t key;
    SPI_Handle spiHandle = (SPI_Handle) GPIO_getUserArg(index);
    SPICC26X4DMA_Object *object = spiHandle->object;

    if (!GPIO_read(index))
    {
        /* Start transfer if CSN goes low */
        key = HwiP_disable();
        if (object->headPtr != NULL)
        {
            /* We have data ready; indicate transaction started */
            object->headPtr->status = SPI_TRANSFER_STARTED;
        }
        else
        {
            /* We are not waiting to transfer; disable all interrupts */
            GPIO_setInterruptConfig(object->csnPin, GPIO_CFG_INT_DISABLE);
            object->returnPartial = SPICC26X4DMA_retPartEnabledIntNotSet;
        }
        HwiP_restore(key);
    }
    else
    {
        /* Cancel ongoing transfer if CSN high */
        key = HwiP_disable();

        GPIO_setInterruptConfig(object->csnPin, GPIO_CFG_INT_DISABLE);
        object->returnPartial = SPICC26X4DMA_retPartEnabledIntNotSet;

        /* Indicate why the transaction completed */
        if (object->headPtr != NULL) {
            object->headPtr->status = SPI_TRANSFER_CSN_DEASSERT;
        }

        HwiP_restore(key);

        /* Cancel the current transaction */
        SPICC26X4DMA_transferCancel(spiHandle);
    }
}

/*
 *  ======== flushFifos ========
 */
static void flushFifos(SPICC26X4DMA_HWAttrs const *hwAttrs)
{
    /* Flush RX FIFO */
    while(!(HWREG(hwAttrs->baseAddr + SPI_O_STAT) & SPI_STAT_RFE_EMPTY)) {
        /* Read element from RX FIFO and discard */
        HWREG(hwAttrs->baseAddr + SPI_O_RXDATA);
    }

    /* Flush TX FIFO */
    while(!(HWREG(hwAttrs->baseAddr + SPI_O_STAT) & SPI_STAT_TFE_EMPTY)) {
        /* Read element from TX FIFO and discard */
        HWREG(hwAttrs->baseAddr + SPI_O_TXDATA);
    }

}

/*
 *  ======== getDmaChannelNumber ========
 */
static inline uint32_t getDmaChannelNumber(uint32_t x) {
#if defined(__TI_COMPILER_VERSION__)
    return ((uint32_t) __clz(__rbit(x)));
#elif defined(__GNUC__)
    return ((uint32_t) __builtin_ctz(x));
#elif defined(__IAR_SYSTEMS_ICC__)
    return ((uint32_t) __CLZ(__RBIT(x)));
#else
    #error "Unsupported compiler"
#endif
}

/*
 *  ======== initHw ========
 */
static bool initHw(SPI_Handle handle) {
    ClockP_FreqHz              freq;
    SPICC26X4DMA_Object        *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t                format;

    flushFifos(hwAttrs);

    /* Disable SSI operation */
    disableSPI(hwAttrs->baseAddr);

    /* Disable SPI module interrupts */
    disableInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);
    clearInterrupt(hwAttrs->baseAddr, SPI_INT_ALL);

    /* Get requested format */
    format = frameFormat[object->format];
    if(hwAttrs->csnPin) {
        /* A CS pin was specified, set to 4 wire mode */
        format |= SPI_CTL0_FRF_MOTOROLA_4WIRE;
    }

    /* Set the SPI configuration */
    ClockP_getCpuFreq(&freq);

    if(freq.lo/2 >= object->bitRate) {
        configSPI(hwAttrs->baseAddr, freq.lo, format, mode[object->mode], object->bitRate, object->dataSize);
        return true;
    }
    return false;
}

/*
*  ======== initIO ========
*  This functions initializes the SPI IOs.
*
*  @pre    Function assumes that the SPI handle is pointing to a hardware
*          module which has already been opened.
*/
static void initIO(SPI_Handle handle)
{
    SPICC26X4DMA_Object        *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (object->mode == SPI_SLAVE) {
        GPIO_setMux(hwAttrs->mosiPin, hwAttrs->rxPinMux);
        GPIO_setMux(hwAttrs->misoPin, hwAttrs->txPinMux);
        GPIO_setMux(hwAttrs->clkPin, hwAttrs->clkPinMux);

        /* Configure CSN callback for optional RETURN_PARTIAL slave mode */
        if (object->csnPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(object->csnPin, hwAttrs->csnPinMux);
            GPIO_setCallback(object->csnPin, csnCallback);
            GPIO_setUserArg(object->csnPin, handle);
        }
    }
    else {
        GPIO_setMux(hwAttrs->mosiPin, hwAttrs->txPinMux);
        GPIO_setMux(hwAttrs->misoPin, hwAttrs->rxPinMux);
        GPIO_setMux(hwAttrs->clkPin, hwAttrs->clkPinMux);

        /* Mux CS unless it is software-controlled */
        if (object->csnPin != GPIO_INVALID_INDEX) {
            GPIO_setMux(object->csnPin, hwAttrs->csnPinMux);
        }
    }
}

/*
*  ======== finalizeIO ========
*  This function releases the SPI IOs.
*/
static void finalizeIO(SPI_Handle handle)
{
    SPICC26X4DMA_Object        *object = handle->object;
    SPICC26X4DMA_HWAttrs const *hwAttrs = handle->hwAttrs;

    GPIO_resetConfig(hwAttrs->mosiPin);
    GPIO_resetConfig(hwAttrs->misoPin);
    GPIO_resetConfig(hwAttrs->clkPin);

    /* We always mux CS in slave mode, but as master it can be SW-controlled (and therefore unmuxed) */
    if (object->mode == SPI_SLAVE || object->csnPin != GPIO_INVALID_INDEX) {
        GPIO_resetConfig(object->csnPin);
    }
}

/*
 *  ======== primeTransfer ========
 *  Function must be executed with interrupts disabled.
 */
static inline void primeTransfer(SPICC26X4DMA_Object *object,
                                 SPICC26X4DMA_HWAttrs const *hwAttrs)
{
    if (object->priTransferSize != 0 && object->altTransferSize != 0) {
        /*
         * Both primary & alternate channels are configured for a transfer.
         * In this case no work is required; the Hwi will configure channels
         * as transfers continue & complete.
         */
    }
    else if (object->priTransferSize == 0 && object->altTransferSize == 0) {
        /*
         * Primary & alternate channels are disabled; no active transfer,
         * configure a new transfer.
         *
         * DMA based transfers use the DMA in ping-pong mode. If the transfer is
         * larger than what the primary channel can handle; alternate channel is
         * configured to continue where the primary channel left off. Channels
         * are continuously reconfigured until the transfer is completed.
         *
         * We disable the alternate channel initially.  This however causes an
         * undesired interrupt to be triggered; so we need to
         * disable/clear/re-enable the interrupt.
         */
        HwiP_disableInterrupt(hwAttrs->intNum);

        /* Set the primary DMA structure as active */
        UDMACC26XX_disableAttribute(object->udmaHandle,
                                    getDmaChannelNumber(hwAttrs->rxChannelBitMask),
                                    UDMA_ATTR_ALTSELECT);
        UDMACC26XX_disableAttribute(object->udmaHandle,
                                    getDmaChannelNumber(hwAttrs->txChannelBitMask),
                                    UDMA_ATTR_ALTSELECT);

        HwiP_clearInterrupt(hwAttrs->intNum);
        HwiP_enableInterrupt(hwAttrs->intNum);

        /* Configure RX & TX DMA transfers */
        configNextTransfer(object, hwAttrs);
        object->activeChannel = UDMA_PRI_SELECT;
        if (object->headPtr->count > MAX_DMA_TRANSFER_AMOUNT) {
            configNextTransfer(object, hwAttrs);
        }

        /* Enable DMA to generate interrupt on SPI peripheral */
        if (!object->manualStart) {
            enableSPI(hwAttrs->baseAddr);
        }
    }
    else {
        /* One of the channels is active; configure the other channel */
        configNextTransfer(object, hwAttrs);
    }
}

/*
 *  ======== releaseConstraint ========
 */
static inline void releaseConstraint(uint32_t txBufAddr)
{
    /* Release need flash if buffer was in flash. */
    if (((txBufAddr & 0xF0000000) == 0x0) && (txBufAddr)) {
        Power_releaseConstraint(PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING);
        Power_releaseConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
    }

    /* Release standby constraint since operation is done. */
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
}

/*
 *  ======== setConstraint ========
 */
static inline void setConstraint(uint32_t txBufAddr)
{
    /*
     * Ensure flash is available if TX buffer is in flash.
     * Flash starts with 0x0..
     */
    if (((txBufAddr & 0xF0000000) == 0x0) && (txBufAddr)) {
        Power_setConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
        Power_setConstraint(PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING);
    }

    /* Set constraints to guarantee operation */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
}

/*
 *  ======== spiPollingTransfer ========
 */
static inline void spiPollingTransfer(SPICC26X4DMA_Object *object,
                                      SPICC26X4DMA_HWAttrs const *hwAttrs,
                                      SPI_Transaction *transaction)
{
    uint8_t   txIncrement, rxIncrement;
    uint32_t  dummyBuffer;
    size_t    rxCount, txCount;
    void     *rxBuf;
    void     *txBuf;
    bool      put;

    /* Only increment src/destination pointers if buffers were provided */
    if (transaction->rxBuf) {
        rxBuf = transaction->rxBuf;
        if(object->dataSize <= SPI_DATASIZE_8) {
            rxIncrement = sizeof(uint8_t);
        }
        else if (object->dataSize <= SPI_DATASIZE_16) {
            rxIncrement = sizeof(uint16_t);
        }
        else {
            rxIncrement = sizeof(uint32_t);
        }
    }
    else {
        rxBuf = &(object->rxScratchBuf);
        rxIncrement = 0;
    }

    if (transaction->txBuf) {
        txBuf = transaction->txBuf;
        if(object->dataSize <= SPI_DATASIZE_8) {
            txIncrement = sizeof(uint8_t);
        }
        else if (object->dataSize <= SPI_DATASIZE_16) {
            txIncrement = sizeof(uint16_t);
        }
        else {
            txIncrement = sizeof(uint32_t);
        }
    }
    else {
        txBuf = (void *) &(object->txScratchBuf);
        txIncrement = 0;
    }

    rxCount = transaction->count;
    txCount = rxCount;

    enableSPI(hwAttrs->baseAddr);

    /* Fill the TX FIFO as much as we can before reading */
    while (rxCount--) {
        if (object->dataSize <= SPI_DATASIZE_8) {
            put = true;
            while(txCount > 0 && put) {
                put = dataPutNonBlocking(hwAttrs->baseAddr, *((uint8_t *) txBuf));
                if (put) {
                    txBuf = (void *) (((uint32_t) txBuf) + txIncrement);
                    txCount--;
                }
            }
            dataGet(hwAttrs->baseAddr, &dummyBuffer);
            *((uint8_t *) rxBuf) = (uint8_t) dummyBuffer;
        }
        else if (object->dataSize <= SPI_DATASIZE_16) {
            put = true;
            while(txCount > 0 && put) {
                put = dataPutNonBlocking(hwAttrs->baseAddr, *((uint16_t *) txBuf));
                if (put) {
                    txBuf = (void *) (((uint32_t) txBuf) + txIncrement);
                    txCount--;
                }
            }
            dataGet(hwAttrs->baseAddr, &dummyBuffer);
            *((uint16_t *) rxBuf) = (uint16_t) dummyBuffer;
        }
        else {
            put = true;
            while(txCount > 0 && put) {
                put = dataPutNonBlocking(hwAttrs->baseAddr, *((uint32_t *) txBuf));
                if (put) {
                    txBuf = (void *) (((uint32_t) txBuf) + txIncrement);
                    txCount--;
                }
            }
            dataGet(hwAttrs->baseAddr, &dummyBuffer);
            *((uint32_t *) rxBuf) = (uint32_t) dummyBuffer;
        }

        /* Update rxBuf position */
        rxBuf = (void *) (((uint32_t) rxBuf) + rxIncrement);
    }

    while (spiBusy(object, hwAttrs)) {}
}

/*
 *  ======== spiPostNotify ========
 */
static int spiPostNotify(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg)
{
    initHw((SPI_Handle) clientArg);

    return (Power_NOTIFYDONE);
}

/*
 *  ======== spiBusy ========
 *  HW is busy when in master mode and BSY bit is set, or when in slave mode
 *  and TFE bit is not set.
 */
static inline bool spiBusy(SPICC26X4DMA_Object *object,
                           SPICC26X4DMA_HWAttrs const *hwAttrs)
{
    bool registerBit = (bool)(HWREG(hwAttrs->baseAddr + SPI_O_STAT) & (object->busyBit));
    if (object->busyBit == SPI_STAT_BUSY_ACTIVE){
        return(registerBit);
    }
    else
    {
        return(!registerBit);
    }
}

/*
 *  ======== disableSPI ========
 *  Disables the SPI peripheral
 */
static inline void disableSPI(uint32_t baseAddr)
{
    HWREG(baseAddr + SPI_O_CTL1) &= ~SPI_CTL1_ENABLE_ENABLE;
}

/*
 *  ======== enableSPI ========
 *  Enables the SPI peripheral
 */
static inline void enableSPI(uint32_t baseAddr)
{
    HWREG(baseAddr + SPI_O_CTL1) |= SPI_CTL1_ENABLE_ENABLE;
}

/*
 *  ======== getInterruptStatus ========
 *  Return the masked interrupt status
 */
static inline uint32_t getInterruptStatus(uint32_t baseAddr, bool masked)
{
    /* Return either the interrupt status or the raw interrupt status as
    requested. */
    if(masked)
    {
        return(HWREG(baseAddr + SPI_O_MIS));
    }
    else
    {
        return(HWREG(baseAddr + SPI_O_RIS));
    }
}

/*
 *  ======== enableInterrupt ========
 *  Enables the SPI peripheral interrupt
 */
static inline void enableInterrupt(uint32_t baseAddr, uint32_t irqs)
{
    HWREG(baseAddr + SPI_O_IMASK) |= irqs;
}

/*
 *  ======== disableInterrupt ========
 *  Disables the SPI peripheral interrupt
 */
static inline void disableInterrupt(uint32_t baseAddr, uint32_t irqs)
{
    HWREG(baseAddr + SPI_O_IMASK) &= ~irqs;
}

/*
 *  ======== clearInterrupt ========
 *  Clears the specified SPI peripheral interrupts
 */
static inline void clearInterrupt(uint32_t baseAddr, uint32_t irqs)
{
    HWREG(baseAddr + SPI_O_ICLR) = irqs;
}

/*
 *  ======== enableDMA ========
 *  Enable the SPI DMA peripheral
 */
static void enableDMA(uint32_t baseAddr, uint32_t dmaFlags)
{
    /* Set the requested bits in the SPI DMA control register. */
    HWREG(baseAddr + SPI_O_DMACR) |= dmaFlags;
}

static void disableDMA(uint32_t baseAddr, uint32_t dmaFlags)
{
    /* Clear the requested bits in the SPI DMA control register. */
    HWREG(baseAddr + SPI_O_DMACR) &= ~dmaFlags;
}


/*
 *  ======== configSPI ========
 *  Configures the peripheral settings
 */
static bool configSPI(uint32_t baseAddr,
                      uint32_t freq,
                      uint32_t format,
                      uint32_t mode,
                      uint32_t bitRate,
                      uint32_t dataSize)
{
    uint16_t scr;
    uint32_t ratio;
    /* Get existing settings */
    uint32_t reg0 = HWREG(baseAddr + SPI_O_CTL0);
    /* Create mask for settings to modify */
    uint32_t mask0 = (SPI_CTL0_DSS_M | SPI_CTL0_FRF_M | SPI_CTL0_SPO_M | SPI_CTL0_SPH_M );

    /* Convert and mask data size to HW register format */
    dataSize = (SPI_CTL0_DSS_M & (dataSize - 1));
    /* Apply updated register */
    HWREG(baseAddr + SPI_O_CTL0) = (reg0 & ~mask0) | format | dataSize;

    /* Set master/slave mode, MSB first */
    HWREG(baseAddr + SPI_O_CTL1) = mode | SPI_CTL1_MSB_ENABLE;

    /* Calculate clock control variable */
    ratio = freq/(2*bitRate);
    if(ratio > 0 && ratio <= SPI_CLKCTL_SCR_MAXIMUM) {
        scr = (uint16_t)(ratio - 1);
    }
    else {
        scr = 0;
    }
    /* Set clock divider */
    HWREG(baseAddr + SPI_O_CLKCTL) = (~SPI_CLKCTL_SCR_M & HWREG(baseAddr + SPI_O_CLKCTL)) | scr;

    return(true);
}

static int32_t dataPutNonBlocking(uint32_t baseAddr, uint32_t frame)
{
    /* Check for space to write. */
    if(HWREG(baseAddr + SPI_O_STAT) & SPI_STAT_TNF_NOT_FULL) {
        /* Write the data to the SPI TX FIFO. */
        HWREG(baseAddr + SPI_O_TXDATA) = frame;
        return(1);
    }
    else {
        return(0);
    }
}

static void dataGet(uint32_t baseAddr, uint32_t *frame)
{
    /* Wait until there is data to be read. */
    while(!(HWREG(baseAddr + SPI_O_STAT) & SPI_STAT_RFE_EMPTY))
    {
    }

    /* Read data from SPI RX FIFO. */
    *frame = HWREG(baseAddr + SPI_O_RXDATA);
}

static bool isSPIbusy(uint32_t baseAddr)
{
    /* Determine if the SPI is busy. */
    return((HWREG(baseAddr + SPI_O_STAT) & SPI_STAT_BUSY) ? true : false);
}
