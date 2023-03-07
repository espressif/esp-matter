/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_ssp_dmac.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_SSP                       SSP2
#define DEMO_DMAC                      DMAC
#define DEMO_DMAC_CHANNEL_SSP_TX       kDMAC_Channel0
#define DEMO_DMAC_CHANNEL_SSP_RX       kDMAC_Channel1
#define DEMO_SSP_TX_DMA_REQUEST_SOURCE kDMAC_PeriphNum_SSP2_Tx
#define DEMO_SSP_RX_DMA_REQUEST_SOURCE kDMAC_PeriphNum_SSP2_Rx

#define DEMO_SSP_I2S_BIT_WIDTH    kSSP_DataSize16Bit
#define DEMO_SSP_I2S_FRAME_LENGTH 16
#define DEMO_SSP_I2S_SLOT         2U
#define DEMO_SSP_I2S_TX_SLOT_MASK (2U | 1U)
#define DEMO_SSP_I2S_RX_SLOT_MASK (2U | 1U)

#define DEMO_BUFFER_LENGTH 256

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Whether the SW button is pressed */
ssp_dmac_handle_t sspDmaSendHandle;
ssp_dmac_handle_t sspDmaReceiveHandle;
dmac_handle_t dmaSendHandle;
dmac_handle_t dmaReceiveHandle;
uint8_t g_sendBuffer[DEMO_BUFFER_LENGTH]    = {0U};
uint8_t g_receiveBuffer[DEMO_BUFFER_LENGTH] = {0U};
volatile bool isRxDone                      = false;
volatile bool isTxDone                      = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static clock_aupll_config_t s_audioPll = {
    .aupllSrc       = kCLOCK_AuPllSrcRC32M,
    .refDiv         = 0xA,
    .fbDiv          = 0x26,
    .icp            = 0,
    .postDivPattern = 1,
    .postDivModulo  = 1,
    .enaDither      = true,
    .enaVcoClkX2    = true,
    .fract          = 0x0AAAA,
};

void ssp_send_callback(ssp_dmac_handle_t *handle, status_t transferStatus, void *userData)
{
    if (transferStatus == kSSP_TransferStatusTxIdle)
    {
        isTxDone = true;
    }
}

void ssp_receive_callback(ssp_dmac_handle_t *handle, status_t transferStatus, void *userData)
{
    if (transferStatus == kSSP_TransferStatusRxIdle)
    {
        isRxDone = true;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    ssp_config_t sspConfig;
    ssp_transfer_t sspSendTransfer;
    ssp_transfer_t sspReceiveTransfer;
    ssp_psp_mode_config_t sspPSPConfig;
    dmac_config_t dmaConfig;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PMU->WLAN_CTRL |= PMU_WLAN_CTRL_REFCLK_AUD_REQ_MASK;
    while ((PMU->WLAN_CTRL & PMU_WLAN_CTRL_REFCLK_AUD_RDY_MASK) == 0U)
    {
    }

    CLOCK_InitAuPll(&s_audioPll);

    CLOCK_AttachClk(kAUPLL_to_SSP2);

    CLOCK_SetClkDiv(kCLOCK_DivSsp2, 16);

    for (uint32_t i = 0U; i < DEMO_BUFFER_LENGTH; i++)
    {
        g_sendBuffer[i] = i;
    }

    PRINTF("SSP i2s dma loopback driver example\r\n");

    DMAC_GetDefaultConfig(&dmaConfig);
    dmaConfig.peripheralNumber[DEMO_DMAC_CHANNEL_SSP_TX] = DEMO_SSP_TX_DMA_REQUEST_SOURCE;
    dmaConfig.peripheralNumber[DEMO_DMAC_CHANNEL_SSP_RX] = DEMO_SSP_RX_DMA_REQUEST_SOURCE;
    DMAC_Init(DEMO_DMAC, &dmaConfig);
    DMAC_TransferCreateHandle(DEMO_DMAC, &dmaSendHandle, DEMO_DMAC_CHANNEL_SSP_TX);
    DMAC_TransferCreateHandle(DEMO_DMAC, &dmaReceiveHandle, DEMO_DMAC_CHANNEL_SSP_RX);

    SSP_GetDefaultPSPModeConfig(&sspPSPConfig);
    sspPSPConfig.dataWidth        = DEMO_SSP_I2S_BIT_WIDTH;
    sspPSPConfig.frameLength      = DEMO_SSP_I2S_FRAME_LENGTH;
    sspPSPConfig.frameSlotsNumber = DEMO_SSP_I2S_SLOT;
    sspPSPConfig.txActiveSlotMask = DEMO_SSP_I2S_TX_SLOT_MASK;
    sspPSPConfig.rxActiveSlotMask = DEMO_SSP_I2S_RX_SLOT_MASK;

    SSP_GetDefaultConfig(&sspConfig);
    sspConfig.format            = kSSP_TransceiverFormatPSP;
    sspConfig.transceiverConfig = &sspPSPConfig;

    SSP_Init(DEMO_SSP, &sspConfig);

    SSP_TransferSendCreateHandleDMAC(DEMO_SSP, &sspDmaSendHandle, ssp_send_callback, NULL, &dmaSendHandle);
    SSP_TransferReceiveCreateHandleDMAC(DEMO_SSP, &sspDmaReceiveHandle, ssp_receive_callback, NULL, &dmaReceiveHandle);

    sspSendTransfer.dataBuffer    = g_sendBuffer;
    sspSendTransfer.dataSize      = DEMO_BUFFER_LENGTH;
    sspReceiveTransfer.dataBuffer = g_receiveBuffer;
    sspReceiveTransfer.dataSize   = DEMO_BUFFER_LENGTH;

    SSP_TransferSendDMAC(&sspDmaSendHandle, &sspSendTransfer);
    SSP_TransferReceiveDMAC(&sspDmaReceiveHandle, &sspReceiveTransfer);

    while ((isRxDone == false) || (isTxDone == false))
    {
    }

    if (memcmp(g_sendBuffer, g_receiveBuffer, DEMO_BUFFER_LENGTH))
    {
        PRINTF("SSP i2s dma loopback failed\r\n");
    }
    else
    {
        PRINTF("SSP i2s dma loopback success\r\n");
    }

    while (1)
    {
    }
}
