/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef COMPUTE_HPP
#define COMPUTE_HPP

#include <cstdlib>

using namespace std;

/* Arguably at least, this LFSR-based hashing code is run more commonly on the
   target itself -- included in the generated code -- than it is run here.
   However, it's available here too, such as to parallel-calculate expected hash
   values. */

class crc32
{
public:
    void seed_lfsr (uint32_t init_value);
    /* lfsr_1b() performs one shift of the LFSR, factoring in a single bit of info,
       that single bit must be in the low-order bit of the parameter. */
    uint32_t lfsr_1b (uint32_t a_bit);
    // crc() has two overloadings, calculating the CRC for byte or word quantities:
    uint32_t crc (uint8_t a_byte);
    uint32_t crc (uint16_t a_halfword);
    uint32_t crc (uint32_t a_word);
    crc32 (void);
private:
    const uint32_t polynomial = 0xb4bcd35c;
    uint32_t shift_reg;
};

#endif /* COMPUTE_HPP */
