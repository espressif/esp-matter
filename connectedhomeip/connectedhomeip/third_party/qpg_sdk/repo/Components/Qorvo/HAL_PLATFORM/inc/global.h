/*
 * Copyright (c) 2005-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * global.h
 *   This file contains the global typedefs and defines.
 *
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
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "compiler.h"

/*****************************************************************************
 * Compile time verification
 *****************************************************************************/

/** Check a conditon at compile time.
 * this macro will produce a compile error if the condition x is false
 * the error might be strange however,
 * the error will be that the size of the array is negative, which is illegal in C.
 * You can use it both as a statement and an expression, but not at file level.
 * if used with a condition that is not compile-time-constant, it will pass
 */
#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(x)  ((void) sizeof(int[1-2*((x) ? 0 : 1)]))
#endif

//auxiliary macro
#define GP_COMPILE_TIME_VERIFY_TRUE(R) (!!sizeof(struct { unsigned int verify_error_if_negative_size__: (R) ? 1 : -1; }))

/** Check a conditon at compile time.
 * this macro will produce a compile error if the condition x is false
 * it is an alternative for COMPILE_TIME_ASSERT:
 * it is usable at file level, whereas COMPILE_TIME_ASSERT is not
 * COMPILE_TIME_ASSERT is usable as expression (inside macros that return a value), GP_COMPILE_TIME_VERIFY is not.
 * if used with a condition that is not compile-time-constant, it will fail
 */
#ifndef __cplusplus
#define GP_COMPILE_TIME_VERIFY(R) extern int (* gp_verify_function__ (void)) [GP_COMPILE_TIME_VERIFY_TRUE (R)]
#else
#define GP_COMPILE_TIME_VERIFY(R) typedef char gp_verify_function__  [ (R) ? 1 : -1 ]
#endif

/*****************************************************************************
 * Common data types
 *****************************************************************************/

typedef uint8_t                     Bool;
typedef char                        Char;

typedef uint8_t                     UInt8;
typedef uint16_t                    UInt16;
typedef uint32_t                    UInt24;
typedef uint32_t                    UInt32;
typedef uint_fast8_t                UIntLoop;
typedef uint8_t                     UInt8Buffer;

typedef int8_t                      Int8;
typedef int16_t                     Int16;
typedef int32_t                     Int24;
typedef int32_t                     Int32;
typedef int_fast8_t                 IntLoop;

typedef uint8_t                     Byte;

typedef UInt8                       UQ2_6;
typedef UInt16                      UQ2_14;
typedef UInt16                      UQ8_8;
typedef Int16                       Q8_8;
typedef Int8                        Q1_7;
typedef Int16                       Q1_15;
typedef Int32                       Q1_31;
typedef Int32                       Q24_8; /* 8 decimal places */
typedef Int32                       Q8_24; /* 24 decimal places */

#define MILLIVOLT_TO_UQ2_14(mv) ( (UQ2_14) ((((UInt32)mv)<<14)/1000) )

#ifndef HAVE_NO_64BIT
typedef uint64_t                    UInt64;
typedef int64_t                     Int64;
#endif

GP_COMPILE_TIME_VERIFY(sizeof(Char)==1);
GP_COMPILE_TIME_VERIFY(sizeof(UInt8)==1);
GP_COMPILE_TIME_VERIFY(sizeof(UInt16)==2);
GP_COMPILE_TIME_VERIFY(sizeof(UInt32)==4);

//not all hosts have a UInt64, let's simulate one, including endianness
typedef struct UInt64Struct
{
#ifndef GP_BIG_ENDIAN
    //normal case, little endian
    UInt32 LSB;
    UInt32 MSB;
#else //GP_BIG_ENDIAN
    UInt32 MSB;
    UInt32 LSB;
#endif
} UInt64Struct_t;
typedef UInt64Struct_t MACAddress_t;

#ifdef GP_BIG_ENDIAN
#define MACADDRESS_INITIALIZER(msb, lsb) { (msb), (lsb) }
#else
#define MACADDRESS_INITIALIZER(msb, lsb) { (lsb), (msb) }
#endif

// Bluetooth device address (6 bytes)
typedef PACKED_PRE struct
{
    UInt8 addr[6];
} PACKED_POST BtDeviceAddress_t;

GP_COMPILE_TIME_VERIFY(sizeof(BtDeviceAddress_t) == 6);


//Define UIntPtr and UIntPtr_P in compiler.h

#ifndef __cplusplus
#ifndef false
#define false   ((Bool) 0)
#endif
#ifndef true
#define true    ((Bool) !false)
#endif
#endif

#include <stddef.h>

#define GP_API extern

typedef void (* void_func) (void);

/*****************************************************************************
 * Helper macros
 *****************************************************************************/
#if !defined(__KERNEL__) && !defined(__cplusplus)
#ifndef min
#define min(a,b)            ((b) < (a) ? (b) : (a))
#endif
#ifndef max
#define max(a,b)            ((a) < (b) ? (b) : (a))
#endif
#ifndef clamp
#define clamp(val,a,b)           (min(max(val, a), b))
#endif
#endif // __KERNEL__ && !__cplusplus

#ifndef dist
#define dist(a,b)           ((a) < (b) ? ((b) - (a)) : ((a) - (b)))
#endif
#ifndef div_ceil
#define div_ceil(nr,div)    ( nr>=0 ? (((nr)+((div)-1))/(div)) : (((nr)+(0      ))/(div)) )
#endif
#ifndef div_round
#define div_round(nr,div)   ( nr>=0 ? (((nr)+((div)/2))/(div)) : (((nr)-((div)/2))/(div)) )
#endif
#ifndef div_floor
#define div_floor(nr,div)   ( nr>=0 ? (((nr)+(0      ))/(div)) : (((nr)-((div)-1))/(div)) )
#endif
#ifndef udiv_ceil
#define udiv_ceil(nr,div)   (((nr)+((div)-1))/(div))
#endif
#ifndef udiv_round
#define udiv_round(nr,div)  (((nr)+((div)/2))/(div))
#endif
#ifndef udiv_floor
#define udiv_floor(nr,div)  (((nr)+(0      ))/(div))
#endif

// Useful stuff
#define BM(n) (1 << (n))
#define BM64(n) (1ULL << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))

#define BIT_SET(p,b)    do{  p|=BM(b);   }while(false)
#define BIT_CLR(p,b)    do{  p&=~BM(b);  }while(false)
#define BIT_TST(p,b)    (!!(p & BM(b)))
#define BIT_SET64(p,b)    do{  p|=BM64(b);   }while(false)
#define BIT_CLR64(p,b)    do{  p&=~BM64(b);  }while(false)
#define BIT_TST64(p,b)    (!!(p & BM64(b)))

#define number_of_elements(arr) (sizeof (arr) / sizeof (*(arr)))

#define MS_TO_US(msec)         ((UInt32)(msec)*1000UL)
#define US_TO_MS(usec)         (UInt16)(usec/1000UL)
#define ABS(a)              (((a) < 0) ? (-(a)) : (a))
#define BITMASK(bitpos_start, bitpos_end)       ((1 << ((bitpos_end) + 1)) - (1 << (bitpos_start)))
/*****************************************************************************
 * Fixed point
 * conversion macro's
 *****************************************************************************/

#define UQ_PRECISION_DECR8(v)    ((UInt8)(((v)+0x80)>>8))
#define UQ_PRECISION_INCR8(v)    (((UInt16)((v)))<<8)

#define Q_PRECISION_DECR8(v)    (((v) < 0) ? (Int8)(((v)-0x80)>>8) : (Int8)(((v)+0x80)>>8))
#define Q_PRECISION_INCR8(v)    (((Int16)(v))<<8)

/*****************************************************************************
 * Endianness
 * you should define GP_BIG_ENDIAN in compiler.h if it's a big endian uc
 * nothing to define for little endian
 *****************************************************************************/

#ifndef GP_HAVE_SWAP_UINT16
#define GP_SWAP_UINT16_CONVERSION(Data) ((Data) & 0xFF) << 8  |  (((Data) >> 8) & 0xFF)
#endif
#ifndef GP_HAVE_SWAP_UINT32
#define GP_SWAP_UINT32_CONVERSION(Data) ((Data) & 0xFF) << 24 | ((((Data) >> 8) & 0xFF) << 16) | ((((Data) >> 16) & 0xFF) << 8) | ((((Data) >> 24) & 0xFF))
#endif
#ifndef GP_HAVE_SWAP_UINT16
#define GP_SWAP_UINT16(pData) (*(UInt16*)pData) = GP_SWAP_UINT16_CONVERSION(*(UInt16*)(pData))
#endif
#ifndef GP_HAVE_SWAP_UINT32
#define GP_SWAP_UINT32(pData) (*(UInt32*)pData) = GP_SWAP_UINT32_CONVERSION(*(UInt32*)(pData))
#endif
#ifndef GP_HAVE_SWAP_UINT64
#define GP_SWAP_UINT64(pData)                           \
do {                                                    \
    UInt8 tmp,i;                                        \
    for (i=0; i < 4; i++)                               \
    {                                                   \
        tmp = ((UInt8*)(pData))[i];                     \
        ((UInt8*)(pData))[i] = ((UInt8*)(pData))[7-i];  \
        ((UInt8*)(pData))[7-i] = tmp;                   \
    }                                                   \
 } while(false)
 #endif

#define HOST_TO_LITTLE_UINT8(pData)
#ifdef GP_BIG_ENDIAN
#define HOST_TO_LITTLE_UINT16_CONVERSION(Data) GP_SWAP_UINT16_CONVERSION(Data)
#define HOST_TO_LITTLE_UINT32_CONVERSION(Data) GP_SWAP_UINT32_CONVERSION(Data)
#define HOST_TO_LITTLE_UINT16(pData)           GP_SWAP_UINT16(pData)
#define HOST_TO_LITTLE_UINT32(pData)           GP_SWAP_UINT32(pData)
#define HOST_TO_LITTLE_UINT64(pData)           GP_SWAP_UINT64(pData)
#define HOST_TO_LITTLE_ENUM(pData)   /* enumeration is 1 byte wide */
#else
#define HOST_TO_LITTLE_UINT16_CONVERSION(Data) (Data)
#define HOST_TO_LITTLE_UINT32_CONVERSION(Data) (Data)
#define HOST_TO_LITTLE_UINT16(pData)
#define HOST_TO_LITTLE_UINT32(pData)
#define HOST_TO_LITTLE_UINT64(pData)
#define HOST_TO_LITTLE_ENUM(pData)
#endif

#define HOST_TO_BIG_UINT8(pData)
#ifdef GP_BIG_ENDIAN
#define HOST_TO_BIG_UINT16(pData)
#define HOST_TO_BIG_UINT32(pData)
#define HOST_TO_BIG_UINT64(pData)
#define HOST_TO_BIG_ENUM(pData)
#else
#define HOST_TO_BIG_UINT16(pData) GP_SWAP_UINT16(pData)
#define HOST_TO_BIG_UINT32(pData) GP_SWAP_UINT32(pData)
#define HOST_TO_BIG_UINT64(pData) GP_SWAP_UINT64(pData)
#define HOST_TO_BIG_ENUM(pData)   /* enumeration is 1 byte wide */
#endif

//Inverse macros - equal to conversion macros
#define BIG_TO_HOST_UINT8(pData)
#define BIG_TO_HOST_UINT16(pData)       HOST_TO_BIG_UINT16(pData)
#define BIG_TO_HOST_UINT32(pData)       HOST_TO_BIG_UINT32(pData)
#define BIG_TO_HOST_UINT64(pData)       HOST_TO_BIG_UINT64(pData)
#define BIG_TO_HOST_ENUM(pData)         HOST_TO_BIG_ENUM(pData)

#define LITTLE_TO_HOST_UINT8(pData)
#define LITTLE_TO_HOST_UINT16(pData)    HOST_TO_LITTLE_UINT16(pData)
#define LITTLE_TO_HOST_UINT32(pData)    HOST_TO_LITTLE_UINT32(pData)
#define LITTLE_TO_HOST_UINT64(pData)    HOST_TO_LITTLE_UINT64(pData)
#define LITTLE_TO_HOST_ENUM(pData)      HOST_TO_LITTLE_ENUM(pData)

// To RF = To little endian
#define HOST_TO_RF_UINT8(pData)
#define HOST_TO_RF_UINT16_CONVERSION(Data)  HOST_TO_LITTLE_UINT16_CONVERSION(Data)
#define HOST_TO_RF_UINT32_CONVERSION(Data)  HOST_TO_LITTLE_UINT32_CONVERSION(Data)
#define HOST_TO_RF_UINT16(pData)            HOST_TO_LITTLE_UINT16(pData)
#define HOST_TO_RF_UINT32(pData)            HOST_TO_LITTLE_UINT32(pData)
#define HOST_TO_RF_UINT64(pData)            HOST_TO_LITTLE_UINT64(pData)
#define HOST_TO_RF_ENUM(pData)              HOST_TO_LITTLE_ENUM(pData)

//Conversion from RF to host = Conversion from host to RF
#define RF_TO_HOST_UINT8(pData)
#define RF_TO_HOST_UINT16(pData) HOST_TO_RF_UINT16(pData)
#define RF_TO_HOST_UINT32(pData) HOST_TO_RF_UINT32(pData)
#define RF_TO_HOST_UINT64(pData) HOST_TO_RF_UINT64(pData)
#define RF_TO_HOST_ENUM(pData)   HOST_TO_RF_ENUM(pData)


/*****************************************************************************
 * String functions.
 * you need to define these if the clib version is not suitable
 * see c library for prototypes.
 * a reason to define these macros is if the C compiler has "original" ideas about problematic casts and issues warnings for perfectly legal code.
 *****************************************************************************/
#ifndef MEMCPY
#define MEMCPY(dst, src, len)   memcpy(dst, src, len)
#endif
#ifndef MEMCMP
#define MEMCMP(p1, p2, len)     memcmp(p1, p2, len)
#endif
#ifndef MEMSET
#define MEMSET(dst, value, len) memset (dst, value, len)
#endif
#ifndef STRNCPY
#define STRNCPY(dst, src, len)  strncpy (dst, src, len)
#endif
#ifndef STRNCMP
#define STRNCMP(p1, p2, len)    strncmp(p1, p2, len)
#endif
#ifndef STRLEN
#define STRLEN(s)               strlen(s)
#endif

#define MEMCPY_INPLACE(dest, source, length)                         \
{                                                                    \
    UInt16 index;                                                    \
    if(dest > source) /* shift to the right */                       \
    {                                                                \
        for(index = length - 1; index <=0 ; index--)                 \
        {                                                            \
            (((UInt8*)dest)[index]) = (((UInt8*)source)[index]);     \
        }                                                            \
    }                                                                \
    else if(source > dest) /* shift to the left */                   \
    {                                                                \
        for(index = 0; index < length; index++)                      \
        {                                                            \
            (((UInt8*)dest)[index]) = (((UInt8*)source)[index]);     \
        }                                                            \
    }                                                                \
}

/*
 * const data in flash
 *
 * this define should be set in compiler.h, if the flash/rom/otp is in a different address space.
 * in that case all the macros below should also be defined
 * the documentation can be found below
 */
#ifndef GP_FLASH_PTR_IS_NO_ORDINARY_PTR

/*
 * a variable in flash is declared as
 * const mytype ROM myvar FLASH_PROGMEM;
 * most compilers that have special flash pointers need only to define one of these two keywords, the other can be left empty.
 */
#define ROM
#define FLASH_PROGMEM

/*
 * Keyword to indicate a symbol is weak
*/
#ifndef WEAK
#define WEAK
#endif

/**
 * read one byte from flash. Argument is flash address *
 */
#ifndef PGM_READ_BYTE
#define PGM_READ_BYTE(address)        (*(address))
#endif //PGM_READ_BYTE
/**
 * read 2 bytes from flash. Argument is a flash address *
 */
#ifndef PGM_READ_WORD
#define PGM_READ_WORD(address)        (*(address))
#endif //PGM_READ_WORD

/** functions that work with a flash string as second argument instead of a normal string */
#define MEMCPY_P(ptr,pprogmem,size)   MEMCPY(ptr,(const void *)(pprogmem), size)
#define MEMCMP_P(ptr,pprogmem,size)   MEMCMP(ptr,(const void *)(pprogmem), size)
#define STRNCPY_P(pchar,pprogmem,len) STRNCPY(pchar,(const void *)(pprogmem), len)
#define STRNCMP_P(ptr,pprogmem,size)  STRNCMP(ptr,(const void *)(pprogmem),size)
#define STRLEN_P(pprogmem)            STRLEN((const void *)(pprogmem))

/*
 * the argument s is a literal string.
 * example of usage: printf(GEN_STR2FLASH("iteration %u"), n);
 * the macro should make sure the literal string ends up in flash
 * this is rarely needed (atmel only)
 */
#define GEN_STR2FLASH(s)              (s)
#endif //GP_FLASH_PTR_IS_NO_ORDINARY_PTR

typedef const Char ROM gp_flash_char FLASH_PROGMEM;
#define FLASH_STRING gp_flash_char *

/*****************************************************************************
 * Preprocesser macros
 *****************************************************************************/

//Convert defines into strings
#define XSTRINGIFY(s) STRINGIFY(s)
#define STRINGIFY(s) #s

#if defined(__GNUC__)
//if GNU preprocessor is used, we do have variable length macros, so override compiler.h
#undef HAVE_NO_VARIABLE_LENGTH_MACRO
#endif //__GNUC__

#ifndef LINKER_SECTION
#define LINKER_SECTION(name)
#endif //LINKER_SECTION

#ifndef GP_HAVE_EXTRAM_SECTION
#define GP_EXTRAM_SECTION_ATTR
#endif //GP_HAVE_EXTRAM_SECTION

#define STATIC              static
#define STATIC_VAR          static
#define STATIC_VAR_CONST    static const

/** use this macro to get rid of warnings about unused variables/arguments*/
#define NOT_USED(x) ((void) (x))

/** Parameter is passed as an argument but it is not used in function. */
#define IGNORE_PARAM    (0xFF)

#ifdef GP_DIVERSITY_JUMPTABLES

/* to be able to remove the static for functions that need to be patchable */
#define STATIC_FUNC

#ifdef GP_DIVERSITY_ROM_CODE
/* these macros will be generated in the ROM code to enable calling via jumptables */
#define JUMPTABLE_FLASHTYPEDEF(rtype, func, ...)    rtype JUMPTABLE_FLASH(func)(__VA_ARGS__)
#define JUMPTABLE_ROMDEF(rtype, func, ...)          rtype func(__VA_ARGS__) LINKER_SECTION(".rom")
#define JUMPTABLE_ROMIMPL(func)                     rom_impl_##func
#define JUMPTABLE_FLASH(func)                       flash_jump_##func
#undef  STATIC_VAR
#define STATIC_VAR "this should generate an error: you shouldn't use STATIC_VAR in ROM code."
#endif //GP_DIVERSITY_ROM_CODE

#else // def GP_DIVERSITY_JUMPTABLES

/* when the ROM code is compiled in legacy builds */
#define STATIC_FUNC                           static

#endif // def GP_DIVERSITY_JUMPTABLES

/*****************************************************************************
 * Base type marshalling macro's
 *****************************************************************************/

#ifdef GP_BIG_ENDIAN
#define generic_api2buf(pDest, pSource, length, pIndex, byteSize) do { \
    UIntLoop __i; \
    const UInt8* __pSource = (const UInt8*)(pSource); \
    for (__i = 0; __i < length; __i++) \
    { \
        if (byteSize == 2) \
        { \
            UInt16 __v; \
            MEMCPY(&__v, __pSource, 2); \
            HOST_TO_LITTLE_UINT16(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 2); \
        } \
        else if (byteSize == 4) \
        { \
            UInt32 __v; \
            MEMCPY(&__v, __pSource, 4); \
            HOST_TO_LITTLE_UINT32(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 4); \
        } \
        else if (byteSize == 8) \
        { \
            UInt64Struct_t __v; \
            MEMCPY(&__v, __pSource, 8); \
            HOST_TO_LITTLE_UINT64(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 8); \
        } \
        *(pIndex) += byteSize; \
        __pSource += byteSize; \
    } \
} while(false)
#define generic_api2buf_1(pDest, pSource, pIndex, byteSize) do { \
        if (byteSize == 2) \
        { \
            UInt16 __v; \
            MEMCPY(&__v, __pSource, 2); \
            HOST_TO_LITTLE_UINT16(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 2); \
        } \
        else if (byteSize == 4) \
        { \
            UInt32 __v; \
            MEMCPY(&__v, __pSource, 4); \
            HOST_TO_LITTLE_UINT32(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 4); \
        } \
        else if (byteSize == 8) \
        { \
            UInt64Struct_t __v; \
            MEMCPY(&__v, __pSource, 8); \
            HOST_TO_LITTLE_UINT64(&__v); \
            MEMCPY(&pDest[*(pIndex)], &__v, 8); \
        } \
        *(pIndex) += byteSize; \
    } while(false)
#else
#define generic_api2buf(pDest, pSource, length, pIndex, byteSize) do { \
    UIntLoop __i;\
    const UInt8* __pSource = (const UInt8*)(pSource); \
    for (__i = 0; __i < length; __i++) \
    {\
        MEMCPY(&pDest[*pIndex], __pSource, byteSize);\
        *(pIndex) += byteSize;\
        __pSource += byteSize;\
    }\
} while(false)
#define generic_api2buf_1(pDest, pSource, pIndex, byteSize) do { \
    MEMCPY(&pDest[*pIndex], (const UInt8*)(pSource), byteSize);\
    *(pIndex) += byteSize;\
} while(false)
#endif

#define UInt8_api2buf(pDest, pSource, length, pIndex) do { \
    if(__builtin_constant_p(length) && (length == 1)) { \
        (pDest)[*(pIndex)] = *(pSource); \
    } \
    else\
    { \
        MEMCPY(&(pDest)[*(pIndex)], (pSource), (length)); \
    } \
    *(pIndex)+=(length); \
} while(false)
#define UInt8_api2buf_1(pDest, pSource, pIndex) do { \
    (pDest)[*(pIndex)] = *(pSource); \
    *(pIndex)+=1; \
} while(false)

#define void_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define Bool_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define bool_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define char_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define Byte_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define uint8_t_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define void_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define Bool_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define bool_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define char_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define Byte_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define uint8_t_api2buf_1(pDest, pSource, pIndex) UInt8_api2buf_1(pDest, pSource, pIndex)

#define UInt16_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 2)
#define uint16_t_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 2)
#define UInt32_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 4)
#define uint32_t_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 4)
#define UInt16_api2buf_1(pDest, pSource, pIndex)  generic_api2buf_1(pDest, pSource, pIndex, 2)
#define UInt32_api2buf_1(pDest, pSource, pIndex)  generic_api2buf_1(pDest, pSource, pIndex, 4)
#define uint16_t_api2buf_1(pDest, pSource, pIndex)  generic_api2buf_1(pDest, pSource, pIndex, 2)
#define uint32_t_api2buf_1(pDest, pSource, pIndex)  generic_api2buf_1(pDest, pSource, pIndex, 4)

#define Int8_api2buf(pDest, pSource, length, pIndex)    UInt8_api2buf(pDest, pSource, length, pIndex)
#define int8_t_api2buf(pDest, pSource, length, pIndex)  UInt8_api2buf(pDest, pSource, length, pIndex)
#define Int16_api2buf(pDest, pSource, length, pIndex)   UInt16_api2buf(pDest, pSource, length, pIndex)
#define Int32_api2buf(pDest, pSource, length, pIndex)   UInt32_api2buf(pDest, pSource, length, pIndex)
#define Int8_api2buf_1(pDest, pSource, pIndex)    UInt8_api2buf_1(pDest, pSource, pIndex)
#define int8_t_api2buf_1(pDest, pSource, pIndex)  UInt8_api2buf_1(pDest, pSource, pIndex)
#define Int16_api2buf_1(pDest, pSource, pIndex)   UInt16_api2buf_1(pDest, pSource, pIndex)
#define Int32_api2buf_1(pDest, pSource, pIndex)   UInt32_api2buf_1(pDest, pSource, pIndex)

#ifndef HAVE_NO_64BIT
#define UInt64_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 8)
#define uint64_t_api2buf(pDest, pSource, pIndex)        UInt64_api2buf(pDest, pSource, length, pIndex)
#define Int64_api2buf(pDest, pSource, length, pIndex)   UInt64_api2buf(pDest, pSource, length, pIndex)
#define UInt64_api2buf_1(pDest, pSource, pIndex)    generic_api2buf_1(pDest, pSource, pIndex, 8)
#define Int64_api2buf_1(pDest, pSource, pIndex)     UInt64_api2buf_1(pDest, pSource, pIndex)
#define uint64_t_api2buf_1(pDest, pSource, pIndex)  UInt64_api2buf_1(pDest, pSource, pIndex)
#endif

#define UInt64Struct_t_api2buf(pDest, pSource, length, pIndex)  generic_api2buf(pDest, pSource, length, pIndex, 8)
#define MACAddress_t_api2buf(pDest, pSource, length, pIndex)    generic_api2buf(pDest, pSource, length, pIndex, 8)
#define BtDeviceAddress_t_api2buf(pDest, pSource, length, pIndex) generic_api2buf(pDest, pSource, length, pIndex, 6)
#define UInt64Struct_t_api2buf_1(pDest, pSource, pIndex)  generic_api2buf_1(pDest, pSource, pIndex, 8)
#define MACAddress_t_api2buf_1(pDest, pSource, pIndex)    generic_api2buf_1(pDest, pSource, pIndex, 8)
#define BtDeviceAddress_t_api2buf_1(pDest, pSource, pIndex) generic_api2buf_1(pDest, pSource, pIndex, 6)


#define generic_buf2api(pDest, pSource, length, pIndex, byteSize) do { \
    UIntLoop __i; \
    UInt8* __pDest = (UInt8*)(pDest); \
    for (__i = 0; __i < length; __i++) \
    { \
        MEMCPY(__pDest, &(pSource)[*(pIndex)], byteSize); \
        if(byteSize == 2) \
        { \
            LITTLE_TO_HOST_UINT16(__pDest); \
        } \
        else if(byteSize == 4) \
        { \
            LITTLE_TO_HOST_UINT32(__pDest); \
        } \
        else if(byteSize == 8) \
        { \
            LITTLE_TO_HOST_UINT64(__pDest); \
        } \
        *(pIndex) += byteSize; \
        __pDest += byteSize; \
    } \
} while(false)
#define generic_buf2api_1(pDest, pSource, pIndex, byteSize) do { \
    UInt8* __pDest = (UInt8*)(pDest); \
    MEMCPY(__pDest, &(pSource)[*(pIndex)], byteSize); \
    if(byteSize == 2) \
    { \
        LITTLE_TO_HOST_UINT16(__pDest); \
    } \
    else if(byteSize == 4) \
    { \
        LITTLE_TO_HOST_UINT32(__pDest); \
    } \
    else if(byteSize == 8) \
    { \
        LITTLE_TO_HOST_UINT64(__pDest); \
    } \
    *(pIndex) += byteSize; \
} while(false)

#define UInt8_buf2api(pDest, pSource, length, pIndex) do { \
    if(__builtin_constant_p(length) && (length == 1)) { \
       *(pDest) = (pSource)[*(pIndex)]; \
    }\
    else\
    { \
        MEMCPY((pDest), &(pSource)[*(pIndex)], (length)); \
    }\
    *(pIndex)+=(length); \
} while(false)
#define UInt8_buf2api_1(pDest, pSource, pIndex) do { \
   *(pDest) = (pSource)[*(pIndex)]; \
   *(pIndex)+=1; \
} while(false)

#define void_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define Bool_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define bool_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define char_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define Byte_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define uint8_t_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define void_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define Bool_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define bool_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define char_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define Byte_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define uint8_t_buf2api_1(pDest, pSource, pIndex) UInt8_buf2api_1(pDest, pSource, pIndex)

#define UInt16_buf2api(pDest, pSource, length, pIndex)  generic_buf2api(pDest, pSource, length, pIndex, 2)
#define uint16_t_buf2api(pDest, pSource, length, pIndex)  generic_buf2api(pDest, pSource, length, pIndex, 2)
#define UInt32_buf2api(pDest, pSource, length, pIndex)  generic_buf2api(pDest, pSource, length, pIndex, 4)
#define uint32_t_buf2api(pDest, pSource, length, pIndex)  generic_buf2api(pDest, pSource, length, pIndex, 4)
#define UInt16_buf2api_1(pDest, pSource, pIndex)  generic_buf2api_1(pDest, pSource, pIndex, 2)
#define uint16_t_buf2api_1(pDest, pSource, pIndex)  generic_buf2api_1(pDest, pSource, pIndex, 2)
#define UInt32_buf2api_1(pDest, pSource, pIndex)  generic_buf2api_1(pDest, pSource, pIndex, 4)
#define uint32_t_buf2api_1(pDest, pSource, pIndex)  generic_buf2api_1(pDest, pSource, pIndex, 4)

#define Int8_buf2api(pDest, pSource, length, pIndex)    UInt8_buf2api(pDest, pSource, length, pIndex)
#define int8_t_buf2api(pDest, pSource, length, pIndex)  UInt8_buf2api(pDest, pSource, length, pIndex)
#define Int16_buf2api(pDest, pSource, length, pIndex)   UInt16_buf2api(pDest, pSource, length, pIndex)
#define Int32_buf2api(pDest, pSource, length, pIndex)   UInt32_buf2api(pDest, pSource, length, pIndex)
#define Int8_buf2api_1(pDest, pSource, pIndex)    UInt8_buf2api_1(pDest, pSource, pIndex)
#define int8_t_buf2api_1(pDest, pSource, pIndex)  UInt8_buf2api_1(pDest, pSource, pIndex)
#define Int16_buf2api_1(pDest, pSource, pIndex)   UInt16_buf2api_1(pDest, pSource, pIndex)
#define Int32_buf2api_1(pDest, pSource, pIndex)   UInt32_buf2api_1(pDest, pSource, pIndex)
#define int_buf2api(pDest, pSource, length, pIndex)   generic_buf2api(pDest, pSource, length, pIndex, sizeof(int))
#define int_buf2api_1(pDest, pSource, pIndex)         generic_buf2api_1(pDest, pSource, pIndex, sizeof(int))

#ifndef HAVE_NO_64BIT
#define UInt64_buf2api(pDest, pSource, length, pIndex)  generic_buf2api(pDest, pSource, length, pIndex, 8)
#define uint64_t_buf2api(pDest, pSource, length, pIndex) UInt64_buf2api(pDest, pSource, length, pIndex)
#define Int64_buf2api(pDest, pSource, length, pIndex)    UInt64_buf2api(pDest, pSource, length, pIndex)
#define UInt64_buf2api_1(pDest, pSource, pIndex)    generic_buf2api_1(pDest, pSource, pIndex, 8)
#define Int64_buf2api_1(pDest, pSource, pIndex)     UInt64_buf2api_1(pDest, pSource, pIndex)
#define uint64_t_buf2api_1(pDest, pSource, pIndex)  UInt64_buf2api_1(pDest, pSource, pIndex)
#endif

#define UInt64Struct_t_buf2api(pDest, pSource, length, pIndex) generic_buf2api(pDest, pSource, length, pIndex, 8)
#define MACAddress_t_buf2api(pDest, pSource, length, pIndex)   generic_buf2api(pDest, pSource, length, pIndex, 8)
#define BtDeviceAddress_t_buf2api(pDest, pSource, length, pIndex)   generic_buf2api(pDest, pSource, length, pIndex, 6)
#define UInt64Struct_t_buf2api_1(pDest, pSource, pIndex) generic_buf2api_1(pDest, pSource, pIndex, 8)
#define MACAddress_t_buf2api_1(pDest, pSource, pIndex)   generic_buf2api_1(pDest, pSource, pIndex, 8)
#define BtDeviceAddress_t_buf2api_1(pDest, pSource, pIndex)   generic_buf2api_1(pDest, pSource, pIndex, 6)


/** @enum gpMarshall_AckStatus_t */
//@{
/** @brief Command processed correctly */
#define gpMarshall_AckStatusSuccess                                       0x0
/** @brief Command not known by device */
#define gpMarshall_AckStatusUnknownCommand                                0x1
/** @brief Command unsupported */
#define gpMarshall_AckStatusUnsupportedCommand                            0x2
/** @brief Command parameter length incorrect */
#define gpMarshall_AckStatusWrongParameterLength                          0x3
/** @brief Command execution failure */
#define gpMarshall_AckStatusExecutionFailed                               0x4
/** @brief Device not able to process command at this time */
#define gpMarshall_AckStatusBusy                                          0x5
/** @brief The connected PC client is not the expected client. Last connected client takes ownership of the board, rendering previously connected client invalid */
#define gpMarshall_AckStatusClientIDMismatch                              0x6
/** @typedef AckStatus_t
    @brief Serial status reported on communication
*/
typedef UInt8                             gpMarshall_AckStatus_t;
//@}

/*****************************************************************************
 * Main function return
 *****************************************************************************/
#define MAIN_FUNCTION_RETURN_TYPE int
#define MAIN_FUNCTION_RETURN_VALUE (0)

#endif  // _GLOBAL_H_
