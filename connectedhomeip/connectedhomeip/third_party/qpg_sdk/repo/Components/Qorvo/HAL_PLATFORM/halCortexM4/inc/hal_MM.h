/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * hal_MM.h
 *
 * This file contains the functions dependent on the choice of MCU : memory mapped registers, Interrupts,...
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_MM_H_
#define _HAL_MM_H_

/**
 * @file hal_MM.h
 * This file contains the functions dependent on the choice of MCU : memory mapped registers, Interrupt handling,...
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpAssert.h"    //Assert in 24-bit write

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifndef GP_COMPONENT_ID
//Only needed for Assert
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL
#define GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UIntPtr gpHal_Address_t;

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/
//------------------------------------------------------------------------------


/** @brief Read and write register access macros.
 *
 *  These are the raw macros for accessing GP chip registers.
 *
 *  Depending on the processor the macros are used on, access to the registers is
 *  direct (embedded processor) or using SPI/I2C (external processor).
 *
*/

#define WBPTR(x) ((volatile UInt8 *) (x))
#define REG(x)   (*WBPTR((UIntPtr) (x)))

#define GP_HAL_READ_REG(Register) REG(Register)
#define GP_HAL_WRITE_REG(Register,Data)  REG(Register) = (Data)
//we use here inline functions as much as possible, because inline functions do type checking.

ALWAYS_INLINE void GP_HAL_READMODIFYWRITE_REG(UIntPtr Register, UInt16 Mask, UInt16 Data)
{
    UInt16 tmp= GP_HAL_READ_REG(Register);
    tmp = (tmp & ~Mask) | (Data & Mask);
    GP_HAL_WRITE_REG(Register, tmp);
}
/*
 * we have optimized code for 16/32/64-bit accessses.
 */
#define GP_HAL_HAVE_READ_TWO_BYTES
#define GP_HAL_HAVE_READ_FOUR_BYTES
//#define GP_HAL_HAVE_READ_EIGHT_BYTES
#define GP_HAL_HAVE_WRITE_TWO_BYTES
#define GP_HAL_HAVE_WRITE_FOUR_BYTES
//#define GP_HAL_HAVE_WRITE_EIGHT_BYTES

ALWAYS_INLINE void GP_HAL_READ_TWO_BYTES(UIntPtr Address, UInt16 * pData)
{
    *pData =  *((volatile UInt16 *) Address);
}
ALWAYS_INLINE void GP_HAL_READ_THREE_BYTES(UIntPtr Address, UInt32 * pData)
{
    union
    {
        struct
        {
            UInt16 lsb;
            UInt16 msb;
        } u16;
        UInt32 u32;
    } u;
    u.u16.lsb =  *((volatile UInt16 *) Address);
    u.u16.msb =  *((volatile UInt8  *) Address + 2);
    *pData = u.u32;
}
ALWAYS_INLINE void GP_HAL_READ_FOUR_BYTES(UIntPtr Address, UInt32 * pData)
{
    *pData =  *((volatile UInt32 *) Address);
}
ALWAYS_INLINE void GP_HAL_READ_EIGHT_BYTES(UIntPtr Address, UInt64Struct_t * pData)
{
    // not implemented
}

ALWAYS_INLINE void GP_HAL_WRITE_TWO_BYTES(UIntPtr Address, const UInt16 * pData)
{
    *((volatile UInt16 *) Address) = *pData;
}
ALWAYS_INLINE void GP_HAL_WRITE_THREE_BYTES(UIntPtr Address, const UInt32 * pData)
{
    union
    {
        struct
        {
            UInt16 lsb;
            UInt16 msb;
        } s16;
        UInt32 u32;
    } u;
    u.u32=*pData;
    *((volatile UInt16 *) Address)   = u.s16.lsb;
    GP_ASSERT_DEV_EXT(u.s16.msb < 0x100);
    *((volatile UInt8 *)  Address+2) = u.s16.msb;
}
ALWAYS_INLINE void GP_HAL_WRITE_FOUR_BYTES(UIntPtr Address, const UInt32 * pData)
{
    *((volatile UInt32 *) Address) = *pData;
}

ALWAYS_INLINE void GP_HAL_WRITE_EIGHT_BYTES(UIntPtr Address, const UInt64Struct_t * pData)
{
    // not implemented
}


/*
 * code to handle block read/writes.
 * unfortunately we can't use memcpy for READ/WRITEREGS, since this has non-volatile pointers and can be optimised away.
 * we define below a few inline functions to do the actual blkcp
 *
 * the actual macros for READ/GP_HAL_WRITE_BYTE_STREAM try to avoid blkcp and use simple load/stores as much as possible.
 *
 */

ALWAYS_INLINE void BLOCKREAD(UIntPtr Address, void * pBuffer, UInt16 Length)
{
    MEMCPY(pBuffer, (void *) Address, Length);
}

ALWAYS_INLINE void BLOCKWRITE(UIntPtr Address, const void * pBuffer, UInt16 Length)
{
    MEMCPY((void *) Address, pBuffer, Length);
}

//helper macro
#define WRITE_N_BYTES(Address, pData, type)                             \
    * ((volatile type *) (Address)) = *((type *) (pData))


// if i put this in an inline function, __builtin_constant_p always return false, so we have to use a macro here.
// no exact type checking, to avoid warnings for not-taken branches, and because pBuffer migth be a pointer to unsigned char or so.
//using __builting_choose_expr did not work, since it is for expressions, not statements.
#if 1
#define GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, Length)         \
    do                                                      \
    {                                                       \
        BLOCKWRITE(Address, pBuffer, Length);           \
    } while (false)
#else
#define GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, Length)         \
    do                                                      \
    {                                                       \
        if (__builtin_constant_p(Length))                   \
        {                                                   \
            if (Length == 2)                                \
            {                                               \
                WRITE_N_BYTES(Address, pBuffer, UInt16);    \
            }                                               \
            else if (Length == 4)                           \
            {                                               \
                WRITE_N_BYTES(Address, pBuffer, UInt32);    \
            }                                               \
            else                                            \
            {                                               \
                BLOCKWRITE(Address, pBuffer, Length);       \
            }                                               \
        }                                                   \
        else                                                \
        {                                                   \
            BLOCKWRITE(Address, pBuffer, Length);           \
        }                                                   \
    } while (false)
#endif

//helper macro
#define READ_N_BYTES(Address, pData, type) \
    *((type *) (pData)) = * ((volatile type *) (Address))

//if the size is known and "natural", use optimised code, otherwise the blockread code.
#if 1
#define GP_HAL_READ_BYTE_STREAM(Address, pBuffer, Length)      \
    do                                                  \
    {                                                   \
        BLOCKREAD(Address, pBuffer, Length);        \
    } while (false)
#else
#define GP_HAL_READ_BYTE_STREAM(Address, pBuffer, Length)      \
    do                                                  \
    {                                                   \
        if (__builtin_constant_p(Length))               \
        {                                               \
            if (Length == 2)                            \
            {                                           \
                READ_N_BYTES(Address, pBuffer, UInt16); \
            }                                           \
            else if (Length == 4)                       \
            {                                           \
                READ_N_BYTES(Address, pBuffer, UInt32); \
            }                                           \
            else                                        \
            {                                           \
                BLOCKREAD(Address, pBuffer, Length);    \
            }                                           \
        }                                               \
        else                                            \
        {                                               \
            BLOCKREAD(Address, pBuffer, Length);        \
        }                                               \
    } while (false)
#endif

#ifdef GP_COMPONENT_ID_DEFINED_IN_HEADER
#undef GP_COMPONENT_ID
#undef GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID_DEFINED_IN_HEADER

#endif  /* _HAL_MM_H_ */
