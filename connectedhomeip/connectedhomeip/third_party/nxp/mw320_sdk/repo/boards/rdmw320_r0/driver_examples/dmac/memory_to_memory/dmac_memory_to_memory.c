/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_dmac.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DMAC      DMAC
#define DEMO_DMAC_CHANNEL kDMAC_Channel0

#define BUFF_LENGTH 4U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool g_Transfer_Done = false;

AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_srcAddr[BUFF_LENGTH], 4U)  = {0x01, 0x02, 0x03, 0x04};
AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_destAddr[BUFF_LENGTH], 4U) = {0x00, 0x00, 0x00, 0x00};
/*******************************************************************************
 * Code
 ******************************************************************************/

void DMAC_IRQHandler(void)
{
    if ((DMAC_GetChannelInterruptStatus(EXAMPLE_DMAC, DEMO_DMAC_CHANNEL) &
         (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete) != 0U)
    {
        g_Transfer_Done = true;
        DMAC_ClearChannelInterruptStatus(EXAMPLE_DMAC, DEMO_DMAC_CHANNEL,
                                         (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete |
                                             (uint32_t)kDMAC_ChannelInterruptBlockTransferComplete);
    }
}

/* Main function. */
int main(void)
{
    uint32_t i = 0;
    dmac_channel_transfer_config_t dmacTransferConfig;
    dmac_config_t dmacConfig;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    /* Print source buffer */
    PRINTF("DMAC memory to memory example begin.\r\n\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", g_destAddr[i]);
    }
    /* Enable interrupt */
    (void)EnableIRQ(DMAC_IRQn);
    /* Configure DMAC one shot transfer */
    DMAC_GetChannelDefaultTransferConfig(&dmacTransferConfig, g_srcAddr, g_destAddr, kDMAC_ChannelTransferWidth32Bits,
                                         kDMAC_ChannelBurstLength1DataWidth, sizeof(g_srcAddr[0]) * BUFF_LENGTH,
                                         kDMAC_ChannelTransferMemoryToMemory);
    dmacTransferConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
    DMAC_GetDefaultConfig(&dmacConfig);
    dmacConfig.channelTransferConfig[DEMO_DMAC_CHANNEL] = &dmacTransferConfig;
    DMAC_Init(EXAMPLE_DMAC, &dmacConfig);
    DMAC_StartChannel(EXAMPLE_DMAC, DEMO_DMAC_CHANNEL);
    /* Wait for DMAC transfer finish */
    while (g_Transfer_Done != true)
    {
    }
    /* Print destination buffer */
    PRINTF("\r\n\r\nDMAC memory to memory example finish.\r\n\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", g_destAddr[i]);
    }
    while (1)
    {
    }
}
