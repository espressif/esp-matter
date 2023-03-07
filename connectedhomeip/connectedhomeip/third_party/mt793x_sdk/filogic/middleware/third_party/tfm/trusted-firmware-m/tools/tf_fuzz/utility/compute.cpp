/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <cstdint>  // for uint32_t
#include "compute.hpp"


using namespace std;

/**********************************************************************************
   Methods of class crc32 follow:
**********************************************************************************/

crc32::crc32 (void)
{
    shift_reg = 0x55555555;  // just give it some default value
}

void crc32::seed_lfsr (uint32_t init_value)
{
    shift_reg = init_value;
}

/* lfsr_1b() performs one shift of the LFSR, factoring in a single bit of info,
   that single bit must be in the low-order bit of the parameter.  It returns
   the LFSR value, which may be ignored. */
uint32_t crc32::lfsr_1b (uint32_t a_bit)
{
    bool odd;

    odd = ((shift_reg ^ a_bit) & 1) == 1;
    shift_reg >>= 1;
    if (odd) {
        shift_reg ^= polynomial;
    }
    if (shift_reg == 0) {
        // Theoretically should never happen, but precaution...
        seed_lfsr (0x55555555);
    }
    return shift_reg;
}

uint32_t crc32::crc (uint8_t a_byte)
{
    for (int i = 0;  i < 8;  i++) {
        lfsr_1b ((uint32_t) a_byte);
        a_byte >>= 1;
    }
    return shift_reg;
}

uint32_t crc32::crc (uint16_t a_halfword)
{
    for (int i = 0;  i < 16;  i++) {
        lfsr_1b ((uint32_t) a_halfword);
        a_halfword >>= 1;
    }
    return shift_reg;
}

uint32_t crc32::crc (uint32_t a_word)
{
    for (int i = 0;  i < 32;  i++) {
        lfsr_1b ((uint32_t) a_word);
        a_word >>= 1;
    }
    return shift_reg;
}

/**********************************************************************************
   End of methods of class crc32.
**********************************************************************************/
