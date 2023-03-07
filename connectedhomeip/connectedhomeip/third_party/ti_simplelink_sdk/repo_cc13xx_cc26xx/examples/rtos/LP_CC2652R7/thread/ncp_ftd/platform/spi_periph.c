/******************************************************************************

 @file spi_periph.c

 @brief TIRTOS platform specific uart functions for OpenThread

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
 
#include <openthread/config.h>
#include <config/openthread-core-default-config.h>
#include <ncp/ncp_config.h>

#include <stddef.h>
#include <string.h>

#include <utils/code_utils.h>
#include <openthread/platform/spi-slave.h>
#include <openthread/platform/logging.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26X2DMA.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>

#include "ti_drivers_config.h"
#include "system.h"

#define PLATFORM_SPI_DATA_SIZE          8
#define PLATFORM_SPI_FREQ               1000000
#define PLATFORM_SPI_MODE               SPI_SLAVE
#define PLATFORM_SPI_TRANSFER_MODE      SPI_MODE_CALLBACK
#define PLATFORM_SPI_FRAME_FORMAT       SPI_POL0_PHA1
#define PLATFORM_SPI_MAX_TRANSACTIONS   3
#define PLATFORM_SPI_FIRST_TRANSACTION  1
#define PLATFORM_SPI_MID_TRANSACTION    2
#define PLATFORM_SPI_LAST_TRANSACTION   3
#define PLATFORM_SPI_GET_CRC(X)         (X[0] & 0x40)
#define PLATFORM_SPI_SET_CRC(X)         (X[0] |= 0x40)
#define PLATFORM_SPI_UNSET_CRC(X)       (X[0] &= ~0x40)
#define PLATFORM_SPI_GET_CCF(X)         (X[0] & 0x20)
#define PLATFORM_SPI_SET_CCF(X)         (X[0] = X[0] | 0x20)
#define PLATFORM_SPI_GET_LEN(X)         (X[3] | (X[4] << 8))

otPlatSpiSlaveTransactionCompleteCallback sCompleteCallback = NULL;
otPlatSpiSlaveTransactionProcessCallback sProcessCallback  = NULL;
void *sContext         = NULL;

SPI_Handle sSpiHandle;
SPI_Transaction sSpiTransaction[PLATFORM_SPI_MAX_TRANSACTIONS];
uint32_t spiTransactions = 0;
uint32_t spiCallbacks = 0;
uint32_t spiCrcErrors = 0;
size_t spiTxSize = 0;
size_t spiRxSize = 0;

#ifdef PLATFORM_SPI_CRC_SUPPORT
/**
 * Function to calculater CRC.
 *
 */
static uint16_t otPlatSpiFcs(uint8_t *buffer)
{
    static const uint16_t fcsTable[256] =
    {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
    };

    uint16_t fcs = 0xFFFF;
    uint32_t length = PLATFORM_SPI_GET_LEN(buffer) + 5;
    uint16_t i;

    if (length > OPENTHREAD_CONFIG_NCP_SPI_BUFFER_SIZE - 2)
    {
        return 0xFFFF;
    }

    for (i = 0; i < length; i++)
    {
        fcs = (fcs >>8) ^ fcsTable[(fcs ^ buffer[i]) & 0xFF];
    }
    fcs ^= 0xFFFF;

    return fcs;
}
#endif

/**
 * This method performs spi driver processing.
 *
 */
void platformSpiProcess(void)
{
    if (sProcessCallback != NULL)
    {
        sProcessCallback(sContext);
    }
}

/**
 * Callback for when the SPI driver finishes transaction.
 */
void platSpiCallback(SPI_Handle handle, SPI_Transaction *transaction)
{
    uint32_t i;
    uint32_t nTransactions;
    bool transferComplete;

    if (sCompleteCallback != NULL && transaction->arg == (void *)PLATFORM_SPI_LAST_TRANSACTION)
    {
        uint8_t  *aOutputBuf         = sSpiTransaction[0].txBuf;
        uint16_t  aOutputBufLen      = spiTxSize;
        uint8_t  *aInputBuf          = sSpiTransaction[0].rxBuf;
        uint16_t  aInputBufLen       = spiRxSize;
        uint16_t  aTransactionLength = 0;
#ifdef PLATFORM_SPI_CRC_SUPPORT
        uint16_t fcs;
        uint16_t fcsRx;
        uint16_t offset;
#endif

        transferComplete = false;
        nTransactions = ((uint32_t)transaction - (uint32_t)&sSpiTransaction[0])
                       / sizeof(SPI_Transaction) + 1;
        for (i = 0; !transferComplete && (i < nTransactions); i++)
        {
            aTransactionLength += sSpiTransaction[i].count;
            if (sSpiTransaction[i].status == SPI_TRANSFER_CSN_DEASSERT)
            {
                transferComplete = true;
            }
        }

#ifdef PLATFORM_SPI_CRC_SUPPORT
    /* check RX crc, if bad, aTransactionLength = 0 */
    if (transferComplete && PLATFORM_SPI_GET_CRC(aInputBuf))
    {
        fcs = otPlatSpiFcs(aInputBuf);
        if (fcs != 0xFFFF)
        {
            offset = PLATFORM_SPI_GET_LEN(aInputBuf) + 5;
            fcsRx = aInputBuf[offset] | (aInputBuf[offset + 1] << 8);
            if (fcs != fcsRx)
            {
                aTransactionLength = 0;
                spiCrcErrors++;
            }
        }
        else
        {
            aTransactionLength = 0;
            spiCrcErrors++;
        }
    }
#endif

        if (transferComplete && sCompleteCallback(sContext, aOutputBuf, aOutputBufLen, aInputBuf,
                              aInputBufLen, aTransactionLength))
        {
            platformSpiSignal();
        }
        spiCallbacks++;

    }
}

/**
 * Function to enable SPI driver
 */
otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback,
                             otPlatSpiSlaveTransactionProcessCallback aProcessCallback, void *aContext)
{
    SPI_Params spiParams;
    bool transferOk;
    otError retval = OT_ERROR_NONE;

    otEXPECT_ACTION(sSpiHandle == NULL, retval = OT_ERROR_ALREADY);

    /* Initialize SPI handle in periperal mode */
    SPI_Params_init(&spiParams);
    spiParams.transferMode        = PLATFORM_SPI_TRANSFER_MODE;
    spiParams.transferCallbackFxn = platSpiCallback;
    spiParams.mode                = PLATFORM_SPI_MODE;
    spiParams.dataSize            = PLATFORM_SPI_DATA_SIZE;
    spiParams.bitRate             = PLATFORM_SPI_FREQ;
    spiParams.frameFormat         = PLATFORM_SPI_FRAME_FORMAT;

    sSpiHandle = SPI_open(CONFIG_SPI_1, &spiParams);

    otEXPECT_ACTION(sSpiHandle != NULL, retval = OT_ERROR_FAILED);

    SPI_control(sSpiHandle, SPICC26X2DMA_CMD_RETURN_PARTIAL_ENABLE, NULL);

    sCompleteCallback = aCompleteCallback;
    sProcessCallback  = aProcessCallback;
    sContext          = aContext;

    memset(&sSpiTransaction[0], 0, PLATFORM_SPI_MAX_TRANSACTIONS * sizeof(SPI_Transaction));
    
    sSpiTransaction[0].count  = UINT32_MAX;
    sSpiTransaction[0].txBuf  = NULL;
    sSpiTransaction[0].rxBuf  = NULL;
    sSpiTransaction[0].arg    = (void *) PLATFORM_SPI_LAST_TRANSACTION;
    spiTxSize = 0;
    spiRxSize = 0;

    transferOk = SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[0]);
    if (!transferOk)
    {
#if 0 // TIDRIVERS-1816 issue      
        SPI_transferCancel(sSpiHandle);
#endif        
        retval = OT_ERROR_FAILED;
    }
exit:
    return retval;
}

/**
 * Function to disable SPI driver
 */
void otPlatSpiSlaveDisable(void)
{
    if (sSpiHandle != NULL)
    {
        SPI_transferCancel(sSpiHandle);
        SPI_close(sSpiHandle);

        sCompleteCallback = NULL;
        sProcessCallback  = NULL;
        sContext          = NULL;
    }
}

/**
 * Function to run SPI transaction
 */
otError otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf, uint16_t aOutputBufLen, uint8_t *aInputBuf,
                                         uint16_t aInputBufLen, bool aRequestTransactionFlag)
{
    uint8_t spiIndex;
    otError retval = OT_ERROR_NONE;
#ifdef PLATFORM_SPI_CRC_SUPPORT
    uint16_t fcs;
    uint16_t offset;
#endif
    
    otEXPECT_ACTION(sSpiTransaction[0].status != SPI_TRANSFER_STARTED, retval = OT_ERROR_BUSY);
    otEXPECT_ACTION(!(sSpiTransaction[0].status != SPI_TRANSFER_PEND_CSN_ASSERT
                   && sSpiTransaction[1].status == SPI_TRANSFER_STARTED),
                    retval = OT_ERROR_BUSY);
    otEXPECT_ACTION(!(sSpiTransaction[0].status != SPI_TRANSFER_PEND_CSN_ASSERT
                   && sSpiTransaction[2].status == SPI_TRANSFER_STARTED),
                    retval = OT_ERROR_BUSY);

    if ((sSpiTransaction[0].status == SPI_TRANSFER_PEND_CSN_ASSERT)
     || (sSpiTransaction[0].status == SPI_TRANSFER_QUEUED))
    {
#if 0 // TIDRIVERS-1816 issue
        SPI_transferCancel(sSpiHandle);
#else
        return OT_ERROR_BUSY;
#endif
    }

    /* NULL means same as previous */
    if(aInputBuf == NULL)
    {
        aInputBuf = sSpiTransaction[0].rxBuf;
        aInputBufLen = spiRxSize;
    }
    spiTransactions++;
    spiTxSize = aOutputBufLen;
    spiRxSize = aInputBufLen;
    spiIndex = 0;

#ifdef PLATFORM_SPI_CRC_SUPPORT
    /* add CRC to TX frame */
    PLATFORM_SPI_SET_CRC(aOutputBuf);
    fcs = otPlatSpiFcs(aOutputBuf);
    if (fcs != 0xFFFF)
    {
        offset = PLATFORM_SPI_GET_LEN(aOutputBuf) + 5;
        aOutputBuf[offset] = fcs & 0xFF;
        aOutputBuf[offset + 1] = (fcs >> 8) & 0xFF;
        aOutputBufLen = (aOutputBufLen + 2 > OPENTHREAD_CONFIG_NCP_SPI_BUFFER_SIZE)
                       ? OPENTHREAD_CONFIG_NCP_SPI_BUFFER_SIZE : aOutputBufLen + 2;
        aInputBufLen = (aInputBufLen + 2 > OPENTHREAD_CONFIG_NCP_SPI_BUFFER_SIZE)
                      ? OPENTHREAD_CONFIG_NCP_SPI_BUFFER_SIZE : aInputBufLen + 2;
    }
    else
    {
        PLATFORM_SPI_UNSET_CRC(aOutputBuf);
    }
#endif

    memset(&sSpiTransaction[0], 0, PLATFORM_SPI_MAX_TRANSACTIONS * sizeof(SPI_Transaction));
    
    if (aInputBufLen && aOutputBufLen)
    {
        sSpiTransaction[0].count  = aOutputBufLen > aInputBufLen ? aInputBufLen : aOutputBufLen;
        sSpiTransaction[0].txBuf  = aOutputBuf;
        sSpiTransaction[0].rxBuf  = aInputBuf;
        sSpiTransaction[0].arg    = (void *) PLATFORM_SPI_FIRST_TRANSACTION;
        otEXPECT_ACTION(SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[0]),
                                     retval = OT_ERROR_FAILED);
        spiIndex++;

        if (aOutputBufLen != aInputBufLen)
        {
            sSpiTransaction[spiIndex].count  = aOutputBufLen > aInputBufLen ?
                                              (aOutputBufLen - aInputBufLen) : (aInputBufLen - aOutputBufLen);
            sSpiTransaction[spiIndex].txBuf  = aOutputBufLen > aInputBufLen ?
                                               aOutputBuf + aInputBufLen : NULL;
            sSpiTransaction[spiIndex].rxBuf  = aOutputBufLen > aInputBufLen ?
                                               NULL : aInputBuf + aOutputBufLen;
            sSpiTransaction[spiIndex].arg    = (void *) PLATFORM_SPI_MID_TRANSACTION;
            otEXPECT_ACTION(SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[spiIndex]),
                                         retval = OT_ERROR_FAILED);
            spiIndex++;
        }
    }
    else if (aInputBufLen)
    {
        sSpiTransaction[0].count  = aInputBufLen;
        sSpiTransaction[0].txBuf  = NULL;
        sSpiTransaction[0].rxBuf  = aInputBuf;
        sSpiTransaction[0].arg    = (void *) PLATFORM_SPI_FIRST_TRANSACTION;
        otEXPECT_ACTION(SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[0]),
                        retval = OT_ERROR_FAILED);
        spiIndex++;
    }
    else if (aOutputBufLen)
    {
        sSpiTransaction[0].count  = aOutputBufLen;
        sSpiTransaction[0].txBuf  = aOutputBuf;
        sSpiTransaction[0].rxBuf  = NULL;
        sSpiTransaction[0].arg    = (void *) PLATFORM_SPI_FIRST_TRANSACTION;
        otEXPECT_ACTION(SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[0]),
                        retval = OT_ERROR_FAILED);
        spiIndex++;
    }
    else
    {
        /* do nothing */
    }
    sSpiTransaction[spiIndex].count  = UINT32_MAX;
    sSpiTransaction[spiIndex].txBuf  = NULL;
    sSpiTransaction[spiIndex].rxBuf  = NULL;
    sSpiTransaction[spiIndex].arg    = (void *) PLATFORM_SPI_LAST_TRANSACTION;
    otEXPECT_ACTION(SPI_transfer(sSpiHandle, (SPI_Transaction *)&sSpiTransaction[spiIndex]),
                                 retval = OT_ERROR_FAILED);

exit:
    if (retval == OT_ERROR_FAILED)
    {
#if 0 // TIDRIVERS-1816 issue      
        SPI_transferCancel(sSpiHandle);
#endif 
    }
    
    return retval;
}
