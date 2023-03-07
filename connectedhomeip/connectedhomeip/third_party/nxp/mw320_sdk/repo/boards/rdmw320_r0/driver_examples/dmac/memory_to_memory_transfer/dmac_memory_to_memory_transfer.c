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
dmac_handle_t g_DMAC_Handle;
volatile bool g_Transfer_Done = false;

AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_srcAddr[BUFF_LENGTH], 4U)  = {0x01, 0x02, 0x03, 0x04};
AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_destAddr[BUFF_LENGTH], 4U) = {0x00, 0x00, 0x00, 0x00};
/*******************************************************************************
 * Code
 ******************************************************************************/

/* User callback function for DMAC transfer. */
void DMAC_Callback(dmac_handle_t *handle, void *userData, uint32_t status)
{
    g_Transfer_Done = true;
}

/* Main function. Excuate DMAC transfer with transactional APIs. */
int main(void)
{
    uint32_t i = 0;
    dmac_channel_transfer_config_t dmacTransferConfig;
    dmac_config_t dmacConfig;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    /* Print source buffer */
    PRINTF("DMAC memory to memory transfer example begin.\r\n\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", g_destAddr[i]);
    }
    /* Configure DMAC one shot transfer */
    DMAC_GetDefaultConfig(&dmacConfig);
    DMAC_GetChannelDefaultTransferConfig(&dmacTransferConfig, g_srcAddr, g_destAddr, kDMAC_ChannelTransferWidth32Bits,
                                         kDMAC_ChannelBurstLength1DataWidth, sizeof(g_srcAddr[0]) * BUFF_LENGTH,
                                         kDMAC_ChannelTransferMemoryToMemory);
    dmacTransferConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
    DMAC_Init(EXAMPLE_DMAC, &dmacConfig);
    DMAC_TransferCreateHandle(EXAMPLE_DMAC, &g_DMAC_Handle, DEMO_DMAC_CHANNEL);
    DMAC_TransferSetCallback(&g_DMAC_Handle, DMAC_Callback, NULL);
    DMAC_TransferSubmit(&g_DMAC_Handle, &dmacTransferConfig);
    DMAC_TransferStart(&g_DMAC_Handle);
    /* Wait for DMAC transfer finish */
    while (g_Transfer_Done != true)
    {
    }
    /* Print destination buffer */
    PRINTF("\r\n\r\nDMAC memory to memory transfer example finish.\r\n\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", g_destAddr[i]);
    }
    while (1)
    {
    }
}
