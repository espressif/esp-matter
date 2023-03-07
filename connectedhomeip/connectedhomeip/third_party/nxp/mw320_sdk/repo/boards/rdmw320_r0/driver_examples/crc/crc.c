/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "fsl_crc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_CRC CRC

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t err     = 0;
    const uint8_t *dataStr = (const uint8_t *)"123456789";
    uint32_t dataLen = strlen((const char *)dataStr);

    const uint16_t checkCcittCrc16  = 0x2189u;
    const uint16_t checkIbmCrc16    = 0xbb3d;
    const uint16_t checkT10DifCrc16 = 0x39eeu;
    const uint32_t checkIeeeCrc32   = 0x2dfd2d88u;
    const uint16_t checkDnpCrc16    = 0x157du;

    uint16_t check16;
    uint32_t check32;

    /* Init hardware*/
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    CRC_Init(DEMO_CRC);

    PRINTF("CRC Peripheral Driver Example\r\n\r\n");

    PRINTF("Test string: %s\r\n", dataStr);

    /* CRC-16/CCITT */
    CRC_SetMode(DEMO_CRC, kCRC_16BitsCcitt);
    check16 = CRC_Calculate(DEMO_CRC, dataStr, dataLen);

    PRINTF("CRC-16 CCITT: 0x%x\r\n", check16);
    if (check16 != checkCcittCrc16)
    {
        err++;
        PRINTF("...Check fail. Expected: 0x%x\r\n", checkCcittCrc16);
    }

    /* CRC-16/IBM */
    CRC_SetMode(DEMO_CRC, kCRC_16BitsIbm);
    check16 = CRC_Calculate(DEMO_CRC, dataStr, dataLen);

    PRINTF("CRC-16 IBM: 0x%x\r\n", check16);
    if (check16 != checkIbmCrc16)
    {
        err++;
        PRINTF("...Check fail. Expected: 0x%x\r\n", checkIbmCrc16);
    }

    /* CRC-16 T10_DIF  */
    CRC_SetMode(DEMO_CRC, kCRC_16BitsT10Dif);
    check16 = CRC_Calculate(DEMO_CRC, dataStr, dataLen);

    PRINTF("CRC-16 T10_DIF: 0x%x\r\n", check16);
    if (check16 != checkT10DifCrc16)
    {
        err++;
        PRINTF("...Check fail. Expected: 0x%x\r\n", checkT10DifCrc16);
    }

    /*  CRC-32 IEEE */
    CRC_SetMode(DEMO_CRC, kCRC_32BitsIeee);
    check32 = CRC_Calculate(DEMO_CRC, dataStr, dataLen);

    PRINTF("CRC-32 IEEE: 0x%x\r\n", check32);
    if (check32 != checkIeeeCrc32)
    {
        err++;
        PRINTF("...Check fail. Expected: 0x%x\r\n", checkIeeeCrc32);
    }

    /* CRC-16/DNP */
    CRC_SetMode(DEMO_CRC, kCRC_16BitsDnp);
    check16 = CRC_Calculate(DEMO_CRC, dataStr, dataLen);

    PRINTF("CRC-16 DNP: 0x%x\r\n", check16);
    if (check16 != checkDnpCrc16)
    {
        err++;
        PRINTF("...Check fail. Expected: 0x%x\r\n", checkDnpCrc16);
    }

    if (err == 0)
    {
        PRINTF("\r\nCRC example finished successfully.\r\n");
    }
    else
    {
        PRINTF("\r\nCRC example failed.\r\n");
    }

    while (1)
    {
    }
}
