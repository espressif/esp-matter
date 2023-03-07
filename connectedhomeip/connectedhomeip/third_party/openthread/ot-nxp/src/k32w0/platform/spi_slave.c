/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for SPI communication.
 *
 */

#include <assert.h>

#include <utils/code_utils.h>
#include <openthread/platform/spi-slave.h>

#include "fsl_dma.h"
#include "fsl_gpio.h"
#include "fsl_spi.h"
#include "fsl_spi_dma.h"
#include "openthread-system.h"
#include "common/logging.hpp"

#define SPI_SLAVE_RX_CHANNEL 10
#define SPI_SLAVE_TX_CHANNEL 11
#define SPI_RX_RING_BUFFER 1023
#define SPI_INT_PIN 19U

typedef enum
{
    SPI_TRANSACTION_DONE,
    SPI_TRANSACTION_IN_PROGRESS,
} eSpiTransactionState;

/**
 *  SPI Slave transaction variables.
 */
static void *                                    sContext          = NULL;
static uint8_t *                                 sOutputBuf        = NULL;
static uint16_t                                  sOutputBufLen     = 0;
static uint8_t *                                 sInputBuf         = NULL;
static uint16_t                                  sInputBufLen      = 0;
static otPlatSpiSlaveTransactionProcessCallback  sProcessCallback  = NULL;
static otPlatSpiSlaveTransactionCompleteCallback sCompleteCallback = NULL;
static bool                                      firstBoot         = true;

static dma_handle_t         slaveTxHandle;
static dma_handle_t         slaveRxHandle;
static dma_handle_t *       txHandle; /*!< DMA handler for SPI send */
static dma_handle_t *       rxHandle; /*!< DMA handler for SPI receive */
static eSpiTransactionState transactionState          = SPI_TRANSACTION_DONE;
static uint32_t             nbBusyStateDuringSpiTrans = 0;

#if defined(__ICCARM__)
#pragma data_alignment              = 16
dma_descriptor_t g_descriptorPtr[1] = {0};
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
__attribute__((aligned(16))) dma_descriptor_t g_descriptorPtr[1] = {0};
#elif defined(__GNUC__)
__attribute__((aligned(16))) dma_descriptor_t g_descriptorPtr[1] = {0};
#endif

static uint8_t  spiRxBuffer[SPI_RX_RING_BUFFER];
static uint32_t rxIn                 = 0;
static uint32_t RemainingBytesGlobal = SPI_RX_RING_BUFFER;

static bool spiCheckRxSate(void)
{
    uint32_t regPrimask     = 0U;
    uint32_t RemainingBytes = 0;

    regPrimask     = DisableGlobalIRQ();
    RemainingBytes = DMA_GetRemainingBytes(DMA0, SPI_SLAVE_RX_CHANNEL);
    EnableGlobalIRQ(regPrimask);

    if (RemainingBytesGlobal != RemainingBytes)
    {
        transactionState     = SPI_TRANSACTION_IN_PROGRESS;
        RemainingBytesGlobal = RemainingBytes;
    }
    else
    {
        transactionState = SPI_TRANSACTION_DONE;
    }
}

static void prepareTxBuffers(uint8_t *aOutputBuf, uint16_t aOutputBufLen, bool aRequestTransactionFlag)
{
    spi_transfer_t        xfer       = {0};
    dma_transfer_config_t xferConfig = {0};
    spi_config_t *        spi_config_p;

    /* Empty FIFOs */
    SPI1->FIFOCFG |= SPI_FIFOCFG_EMPTYTX_MASK;
    spi_config_p = (spi_config_t *)SPI_GetConfig(SPI1);
    DMA_PrepareTransfer(&xferConfig, aOutputBuf, ((void *)((uint32_t)&SPI1->FIFOWR)),
                        ((spi_config_p->dataWidth > kSPI_Data8Bits) ? (sizeof(uint16_t)) : (sizeof(uint8_t))),
                        aOutputBufLen, kDMA_MemoryToPeripheral, NULL);
    DMA_SubmitTransfer(&slaveTxHandle, &xferConfig);
    DMA_StartTransfer(&slaveTxHandle);

    // otDumpDebg(OT_LOG_REGION_PLATFORM, "TX", sOutputBuf, aOutputBufLen);

    if (aRequestTransactionFlag)
    {
        GPIO_PortSet(GPIO, 0, 1U << SPI_INT_PIN);
        GPIO_PortClear(GPIO, 0, 1U << SPI_INT_PIN);
    }
}

void K32WSpiSlaveProcess(void)
{
    uint32_t RemainingBytes          = 0;
    uint32_t i                       = 0;
    uint32_t nbBytesReceived         = 0;
    uint32_t nbNotEmptyBytesReceived = 0;
    uint8_t *pInputBufIterator       = sInputBuf;
    uint32_t savedInputBufLen        = sInputBufLen;
    bool     processCallback         = false;

    if (sCompleteCallback != NULL)
    {
        /* Check if we received enough data in the ring buffer */
        spiCheckRxSate();
        if (transactionState == SPI_TRANSACTION_IN_PROGRESS)
        {
            return;
        }

        RemainingBytes = RemainingBytesGlobal;

        /* When all transfers are completed, XFERCOUNT value changes from 0 to 0x3FF. The last value
         * 0x3FF does not mean there are 1024 transfers left to complete.It means all data transfer
         * has completed.
         */
        if (RemainingBytes == 1024)
        {
            RemainingBytes = 0U;
        }

        nbBytesReceived = sizeof(spiRxBuffer) - RemainingBytes;

        if (nbBytesReceived >= rxIn)
        {
            nbBytesReceived -= rxIn;
        }
        else
        {
            nbBytesReceived += (sizeof(spiRxBuffer) - rxIn);
        }
        nbNotEmptyBytesReceived = nbBytesReceived;

        if (nbBytesReceived >= 0)
        {
            /* Increase rxIn until first byte is != 0 and != 0xFF */
            for (i = 0; i < nbBytesReceived; i++)
            {
                if (spiRxBuffer[rxIn] != 0x00 && spiRxBuffer[rxIn] != 0xFF)
                {
                    // otLogDebgPlat("spiRxBuffer[rxIn] = %d rxIn=%d", spiRxBuffer[rxIn], rxIn);
                    break;
                }
                nbNotEmptyBytesReceived--;
                rxIn++;
                if (rxIn >= sizeof(spiRxBuffer))
                    rxIn = 0;
            }
            if (nbNotEmptyBytesReceived >= 5)
            {
                // otLogDebgPlat("ReBytes = %d", RemainingBytes);
                // otLogDebgPlat("offset = %d", sizeof(spiRxBuffer)-RemainingBytes);
                // otLogDebgPlat("rxIn = %d", rxIn);
                uint8_t  sizeFoundByte3    = 0;
                uint8_t  sizeFoundByte4    = 0;
                uint16_t spiPacketDataSize = 0;

                for (i = 0; i < nbNotEmptyBytesReceived; i++)
                {
                    if (i < savedInputBufLen)
                    {
                        (*pInputBufIterator) = spiRxBuffer[rxIn];
                        pInputBufIterator++;
                    }
                    /* Extract the packet len */
                    if (i == 3)
                    {
                        spiPacketDataSize += spiRxBuffer[rxIn];
                    }
                    else if (i == 4)
                    {
                        spiPacketDataSize += (spiRxBuffer[rxIn] << 8);
                        // otLogDebgPlat("spiPacketDataSize = %d", spiPacketDataSize);
                        /* Did we get enough bytes ? */
                        if (nbNotEmptyBytesReceived < spiPacketDataSize + 5)
                        {
                            /* If not reset rxIn */
                            uint8_t j = 4;
                            while (j != 0)
                            {
                                if (rxIn == 0)
                                {
                                    rxIn = sizeof(spiRxBuffer) - 1;
                                }
                                else
                                {
                                    rxIn--;
                                }
                                j--;
                            }
                            break;
                        }
                    }

                    rxIn++;
                    if (rxIn >= sizeof(spiRxBuffer))
                        rxIn = 0;

                    /* If we received all bytes as indicated in the packet len
                     * => schedule a call to the callback and break */
                    if (i >= 4 && i == spiPacketDataSize + 4)
                    {
                        processCallback = true;
                        break;
                    }
                    /* If the input buffer is full, schedule a call to the callback
                     * but do not break as we need to continue to process the remaining bytes */
                    if (i == savedInputBufLen - 1)
                    {
                        processCallback = true;
                    }
                }
            }
        }
    }
    if (processCallback)
    {
        /* Workaound to avoid to lose an output request.
         *  Such an issue could happen when there are 3 calls to otPlatSpiSlavePrepareTransaction
         *  and that at each calls SPI is busy.
         * This workaound aims to modify the input buffer accept-len to 0, so that the output buffer would be
         * send again.
         */
        if (nbBusyStateDuringSpiTrans >= 3)
        {
            sInputBuf[1] = 0x00;
            sInputBuf[2] = 0x00;
        }
        nbBusyStateDuringSpiTrans = 0;
        // otLogDebgPlat("inpLen = %d transLen=%d", savedInputBufLen, nbNotEmptyBytesReceived);
        // otDumpDebg(OT_LOG_REGION_PLATFORM, "RX", sInputBuf, savedInputBufLen);
        /* Call the callback */
        if (sCompleteCallback(sContext, sOutputBuf, sOutputBufLen, sInputBuf, savedInputBufLen,
                              nbNotEmptyBytesReceived))
        {
            // Perform any further processing if necessary.
            sProcessCallback(sContext);
        }
    }
}

static void SPI_TxDMACallback(dma_handle_t *handle, void *userData, bool transferDone, uint32_t intmode)
{
    // Empty
}

otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback,
                             otPlatSpiSlaveTransactionProcessCallback  aProcessCallback,
                             void *                                    aContext)
{
    otError               result = OT_ERROR_NONE;
    spi_slave_config_t    userConfig;
    dma_transfer_config_t transferConfig;
    uint32_t              tmp = 0;
    spi_config_t *        spi_config_p;

    assert(aCompleteCallback != NULL);
    assert(aProcessCallback != NULL);

    // Check if SPI Slave interface is already enabled.
    otEXPECT_ACTION(sCompleteCallback == NULL, result = OT_ERROR_ALREADY);

    /* DMA0 is used to empty the RX SPI FIFO and to fill the TX SPI FIFO
     * A DMA is used to avoid to have to wait for the processor to process SPI IRQ to empty or to fill RX/TX FIFO.
     * In fact if the processor is busy and cannot empty the RX fifo, an overflow could occur and some bytes could
     * be lost. That is way a DMA is required. A ring buffer mechanism is implemented to be able to store data in
     * RAM. For RX, a DMA descriptor is used to be able to automatically restart from the begging of the RAM buffer
     * (when it is full) without having to wait for the process to reload the DMA. A pulling mechanism is implemeted
     * to check if there is data available in the RX ring buffer.
     */
    DMA_Init(DMA0);

    SPI_SlaveGetDefaultConfig(&userConfig);
    userConfig.sselPol = (spi_spol_t)kSPI_SpolActiveAllLow;
    /*
     * SPI properties (based on openthread SPI recommendations):
     * CS is active low.
     * CLK is active high.
     * Data is valid on leading edge of CLK.
     * Data is sent in multiples of 8-bits (bytes).
     * Bytes are sent most-significant bit first.
     */
    SPI_SlaveInit(SPI1, &userConfig);
    SPI_SetDummyData(SPI1, 0xFF);

    DMA_EnableChannel(DMA0, SPI_SLAVE_TX_CHANNEL);
    DMA_EnableChannel(DMA0, SPI_SLAVE_RX_CHANNEL);
    DMA_SetChannelPriority(DMA0, SPI_SLAVE_TX_CHANNEL, kDMA_ChannelPriority0);
    DMA_SetChannelPriority(DMA0, SPI_SLAVE_RX_CHANNEL, kDMA_ChannelPriority1);
    DMA_CreateHandle(&slaveTxHandle, DMA0, SPI_SLAVE_TX_CHANNEL);
    DMA_CreateHandle(&slaveRxHandle, DMA0, SPI_SLAVE_RX_CHANNEL);

    spi_config_p = (spi_config_t *)SPI_GetConfig(SPI1);

    /* Install callback for Tx dma channel */
    DMA_SetCallback(&slaveTxHandle, SPI_TxDMACallback, NULL);
    // DMA_SetCallback(&slaveRxHandle, SPI_RxDMACallback, NULL);

    SPI_EnableRxDMA(SPI1, true);
    SPI_EnableTxDMA(SPI1, true);

    DMA_PrepareTransfer(&transferConfig, (void *)&SPI1->FIFORD, spiRxBuffer,
                        ((spi_config_p->dataWidth > kSPI_Data8Bits) ? (sizeof(uint16_t)) : (sizeof(uint8_t))),
                        sizeof(spiRxBuffer), kDMA_PeripheralToMemory, &g_descriptorPtr[0]);
    DMA_SubmitTransfer(&slaveRxHandle, &transferConfig);
    transferConfig.xfercfg.intA = true;
    transferConfig.xfercfg.intB = false;
    DMA_CreateDescriptor(&g_descriptorPtr[0], &transferConfig.xfercfg, (void *)&SPI1->FIFORD, spiRxBuffer,
                         &g_descriptorPtr[0]);
    tmp |= (SPI_DEASSERT_ALL & (~SPI_DEASSERTNUM_SSEL(spi_config_p->sselNum)));
    /* set width of data - range asserted at entry */
    tmp |= SPI_FIFOWR_LEN(spi_config_p->dataWidth);

    /* Clear the SPI_FIFOWR_EOT_MASK bit when data is not the last. */
    tmp &= (uint32_t)(~kSPI_FrameAssert);
    *(((uint16_t *)((uint32_t) & (SPI1->FIFOWR))) + 1) = (uint16_t)(tmp >> 16U);

    DMA_StartTransfer(&slaveRxHandle);

    // Set proper callback and context.
    sProcessCallback  = aProcessCallback;
    sCompleteCallback = aCompleteCallback;
    sContext          = aContext;

exit:
    return result;
}

otError otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf,
                                         uint16_t aOutputBufLen,
                                         uint8_t *aInputBuf,
                                         uint16_t aInputBufLen,
                                         bool     aRequestTransactionFlag)
{
    otError result = OT_ERROR_NONE;

    assert(sCompleteCallback != NULL);

    spiCheckRxSate();

    otEXPECT_ACTION((transactionState != SPI_TRANSACTION_IN_PROGRESS), result = OT_ERROR_BUSY);

    prepareTxBuffers(aOutputBuf, aOutputBufLen, aRequestTransactionFlag);

    if (aOutputBuf != NULL)
    {
        sOutputBuf    = aOutputBuf;
        sOutputBufLen = aOutputBufLen;
    }
    if (aInputBuf != NULL)
    {
        sInputBuf    = aInputBuf;
        sInputBufLen = aInputBufLen;
    }

exit:
    if (result == OT_ERROR_BUSY)
    {
        /* Make sure to always trigger a spi interrupt
         *  if it is the first boot and that the SPI is busy */
        if (firstBoot && aRequestTransactionFlag)
        {
            GPIO_PortSet(GPIO, 0, 1U << SPI_INT_PIN);
            GPIO_PortClear(GPIO, 0, 1U << SPI_INT_PIN);
        }
        nbBusyStateDuringSpiTrans++;
    }
    firstBoot = false;
    // otLogDebgPlat("O=%d I=%d tF=%d St=%d nbBs=%d", aOutputBufLen, aInputBufLen, aRequestTransactionFlag,
    // transactionState, nbBusyStateDuringSpiTrans);
    return result;
}
