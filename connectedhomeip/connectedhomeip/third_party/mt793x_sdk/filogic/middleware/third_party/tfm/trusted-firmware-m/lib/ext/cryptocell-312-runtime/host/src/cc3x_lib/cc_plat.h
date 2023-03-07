/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  CC_PLAT_H
#define  CC_PLAT_H

#define NULL_SRAM_ADDR ((CCSramAddr_t)0xFFFFFFFF)

#define _WriteWordsToSram(addr, data, size) \
do { \
    uint32_t ii; \
    volatile uint32_t dummy; \
    CC_HAL_WRITE_REGISTER( CC_REG_OFFSET (HOST_RGF,SRAM_ADDR), (addr)); \
    for( ii = 0 ; ii < size/sizeof(uint32_t) ; ii++ ) { \
           CC_HAL_WRITE_REGISTER( CC_REG_OFFSET (HOST_RGF,SRAM_DATA), SWAP_TO_LE(((uint32_t *)data)[ii])); \
           do { \
             dummy = CC_HAL_READ_REGISTER( CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
           }while(!(dummy & 0x1)); \
    } \
}while(0)

#define _ReadWordsFromSram( addr , data , size ) \
do { \
    uint32_t ii; \
    volatile uint32_t dummy; \
    CC_HAL_WRITE_REGISTER( CC_REG_OFFSET (HOST_RGF,SRAM_ADDR) ,(addr) ); \
    dummy = CC_HAL_READ_REGISTER( CC_REG_OFFSET (HOST_RGF,SRAM_DATA)); \
    for( ii = 0 ; ii < size/sizeof(uint32_t) ; ii++ ) { \
        do { \
            dummy = CC_HAL_READ_REGISTER( CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
        }while(!(dummy & 0x1)); \
        dummy = CC_HAL_READ_REGISTER( CC_REG_OFFSET (HOST_RGF,SRAM_DATA));\
        ((uint32_t*)data)[ii] = SWAP_TO_LE(dummy); \
    } \
    do { \
        dummy = CC_HAL_READ_REGISTER( CC_REG_OFFSET (HOST_RGF, SRAM_DATA_READY)); \
    }while(!(dummy & 0x1)); \
}while(0)

#define CLEAR_TRNG_SRC()

#endif
