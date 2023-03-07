/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_crc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mw_crc"

#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Initializes CRC module.
 *
 * Call this function to initialize the CRC module.
 *
 * param base CRC peripheral base address
 */
void CRC_Init(CRC_Type *base)
{
    base->CTRL = 0U;
    base->ICR  = CRC_ICR_CLEAR_MASK;
}

/*!
 * brief Disable the interrupt and clear all the interrupts
 * param base RTC peripheral base address
 */
void CRC_Deinit(CRC_Type *base)
{
    base->CTRL = 0x0;
    base->ICR  = CRC_ICR_CLEAR_MASK;
}

/*!
 * brief Calculate the CRC value for input data block.
 *
 * param  base      CRC peripheral base address.
 * param  dataStr   input data stream.
 * param  dataLen   data length in byte.
 * return CRC calculation result.
 */
uint32_t CRC_Calculate(CRC_Type *base, const uint8_t *dataStr, uint32_t dataLen)
{
    uint32_t cnt;

    uint32_t divisor, tail;

    CRC_Enable(base, true);

    /* set data length */
    base->STREAM_LEN_M1 = dataLen - 1;

    /* Padding first  and copy data in */
    divisor = dataLen / 4;
    tail    = dataLen % 4;

    switch (tail)
    {
        case 1:
            base->STREAM_IN = (((uint32_t)dataStr[0]) << 24);
            break;

        case 2:
            base->STREAM_IN = ((((uint32_t)dataStr[0]) << 16) | (((uint32_t)dataStr[1]) << 24));
            break;

        case 3:
            base->STREAM_IN =
                ((((uint32_t)dataStr[0]) << 8) | (((uint32_t)dataStr[1]) << 16) | (((uint32_t)dataStr[2]) << 24));
            break;

        default:
            break;
    }

    /* compose last data from byte to word and copy data in  */
    for (cnt = 0; cnt < divisor; cnt++)
    {
        base->STREAM_IN =
            (((uint32_t)dataStr[4 * cnt + tail]) | (((uint32_t)dataStr[4 * cnt + tail + 1]) << 8) |
             (((uint32_t)dataStr[4 * cnt + tail + 2]) << 16) | (((uint32_t)dataStr[4 * cnt + tail + 3]) << 24));
    }
    /* wait for the result */
    while (base->IRSR == 0)
        ;

    /* clear interrupt flag */
    CRC_ClearInterruptStatus(base);

    /* return the result */
    return base->RESULT;
}
