/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_ssp.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_SSP SSP2

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
ssp_handle_t sspSendHandle;
ssp_handle_t sspReceiveHandle;
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

void ssp_send_callback(ssp_handle_t *handle, status_t transferStatus, void *userData)
{
    if (transferStatus == kSSP_TransferStatusTxIdle)
    {
        isTxDone = true;
    }
}

void ssp_receive_callback(ssp_handle_t *handle, status_t transferStatus, void *userData)
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

    PRINTF("SSP i2s interrupt loopback Driver example\r\n");

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

    SSP_TransferSendCreateHandle(DEMO_SSP, &sspSendHandle, ssp_send_callback, NULL);
    SSP_TransferReceiveCreateHandle(DEMO_SSP, &sspReceiveHandle, ssp_receive_callback, NULL);

    sspSendTransfer.dataBuffer    = g_sendBuffer;
    sspSendTransfer.dataSize      = DEMO_BUFFER_LENGTH;
    sspReceiveTransfer.dataBuffer = g_receiveBuffer;
    sspReceiveTransfer.dataSize   = DEMO_BUFFER_LENGTH;

    SSP_TransferSendNonBlocking(&sspSendHandle, &sspSendTransfer);
    SSP_TransferReceiveNonBlocking(&sspReceiveHandle, &sspReceiveTransfer);

    while ((isRxDone == false) || (isTxDone == false))
    {
    }

    if (memcmp(g_sendBuffer, g_receiveBuffer, DEMO_BUFFER_LENGTH))
    {
        PRINTF("SSP i2s interrupt loopback failed\r\n");
    }
    else
    {
        PRINTF("SSP i2s interrupt loopback success\r\n");
    }

    while (1)
    {
    }
}
