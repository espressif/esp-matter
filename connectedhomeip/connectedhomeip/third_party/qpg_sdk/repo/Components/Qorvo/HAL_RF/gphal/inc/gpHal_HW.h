/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 * gpHal_HW.h
 *
 * This file switches between the HW access modes (SPI, I2C, ...)
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 *
 */

#ifndef _HAL_GP_HW_H_
#define _HAL_GP_HW_H_

/**
 * @file gpHal_HW.h
 * @brief This file switches between the HW access modes (SPI, I2C, ...)
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpAssert.h"

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** @brief A macro that is used to print log messages.
 *
 * A macro that is used to print log messages inside of the GPHAL.  This macro is
 * inserted in the GPHAL where the log messages are useful.  By default,
 * this macro is empty and it is up to the GPHAL user to implement it.
 *
 *  @param fmt The format string of the print message.
 *  @param ... List of parameters for the print message.  The first parameter of this list should be
 *             the length of the parameters in bytes.
 */

#ifndef HAVE_NO_VARIABLE_LENGTH_MACRO
#define GP_LOG(fmt, ...)  do {} while (false)
#else
#define GP_LOG
#endif //HAVE_NO_VARIABLE_LENGTH_MACRO


#ifdef GP_DIVERSITY_GPHAL_INTERN
// FIXME: SW-3937
#if defined(GP_COMP_CHIPEMU)
#elif defined(GP_DIVERSITY_GPHAL_K8E) 
#  include "hal_MM.h"
#else
 #  include "gpHal_HW_MM.h"
#endif
#else // so: not GP_DIVERSITY_GPHAL_INTERN
#  include "gpHal_HW_MSI.h"
#endif // GP_DIVERSITY_GPHAL_INTERN

/** only defined for internal usage*/
#define GP_HAL_IS_ONLY_PROP_IN_REG(prop) (prop##_MASK == prop##_REG_BITS_IN_USE)
#define GP_HAL_CHECK_ONLY_PROP_IN_REG(prop) COMPILE_TIME_ASSERT(GP_HAL_IS_ONLY_PROP_IN_REG(prop))
#define GP_HAL_CHECK_PROP_MATCHES_REG(prop,reg) COMPILE_TIME_ASSERT(prop##_REGISTER == reg)
#define GP_HAL_PROPOFFSET2REG(Offset, Property) (((UInt16) (Offset)) + Property##_REGISTER)
#define GP_HAL_CHECK_ONLY_PROPS_IN_REG(prop1, prop2)                           \
    do {                                                                \
        COMPILE_TIME_ASSERT(prop1##_REGISTER == prop2##_REGISTER);      \
        COMPILE_TIME_ASSERT((prop1##_MASK | prop2##_MASK) == prop1##_REG_BITS_IN_USE); \
    } while (0)

//actual check should be: if arrays on stack with run-time size is allowed
#ifdef __GNUC__
//compile time assert on non-const will succeed, we can use it always
/**part of implementation, don't use*/
#define GP_ASSERT_EARLY(cond)                   \
    do {                                        \
        COMPILE_TIME_ASSERT(cond);              \
        GP_ASSERT_DEV_INT(cond);                        \
    } while (false)
#else
//compile time assert on non-const will give syntax error, don't use
/**part of implementation, don't use*/
#define GP_ASSERT_EARLY(cond) GP_ASSERT_DEV_INT(cond)
#endif
/** part of implementation, don't use */
#define GP_HAL_VAL2PLACEDVAL(Value, Property) (((UInt8) (Value)) << Property##_LSB)
/** part of implementation, don't use! Use GP_HAL_WRITE_PROPTO */
#define GP_HAL_BASE_WRITE_PROPTO(Byte, Property, Value)                 \
    do {                                                                \
        COMPILE_TIME_ASSERT(Property##_WRITABLE==1);                    \
        /* check if the value fits in the property*/                    \
        GP_ASSERT_EARLY(((Value) & (Property##_MASK >> Property##_LSB)) == (Value)); \
        /* the if clause produces better code than the else on the xap, */ \
        /* even for mask 0xFF: no (value and 0xFF)*/                    \
        if (GP_HAL_IS_ONLY_PROP_IN_REG(Property))                       \
        {                                                               \
            (Byte) = GP_HAL_VAL2PLACEDVAL(Value, Property);             \
        }                                                               \
        else                                                            \
        {                                                               \
            (Byte) = ((Byte) & ~(Property##_MASK))                      \
                | (GP_HAL_VAL2PLACEDVAL(Value, Property) & (Property##_MASK)); \
        }                                                               \
    } while (0)

/** @brief A macro to modify a property in a cached version of a GP chip register.
 *
 * A macro to modify a property in a cached version of a GP chip register.
 * This cached memory has to written to the real register to have any effect.
 * Use this to change several properties in the same register in an efficient manner.
 *
 *  @param Byte the cached version register in memory.
 *  @param Property Property name (only use with regprop definition).
 *  @param Value    Value to be set.
 *  @param Reg      The GP chip register name that the Byte parameter represents.
 */
#define GP_HAL_WRITE_PROPTO(Byte, Reg, Property, Value)     \
    do {                                                    \
        GP_HAL_CHECK_PROP_MATCHES_REG(Property, Reg);       \
        GP_HAL_BASE_WRITE_PROPTO(Byte, Property, Value);    \
    } while (0)

/** internal usage, main code in auxiliary macro to make property substitution work as intended */
#define GP_HAL_WRITE_PROP_OFFSET_AUX(Offset, Property, Value)           \
    do {                                                                \
        COMPILE_TIME_ASSERT(Property##_WRITABLE==1);                    \
        /* check if the value fits in the property*/                    \
        GP_ASSERT_EARLY(((Value) & (Property##_MASK >> Property##_LSB)) == (Value)); \
        if (Property##_READABLE && !(GP_HAL_IS_ONLY_PROP_IN_REG(Property))) \
        {                                                                   \
            GP_HAL_READMODIFYWRITE_REG( ((UInt16) (Offset)) + Property##_REGISTER,  Property##_MASK, GP_HAL_VAL2PLACEDVAL(Value, Property) );\
        }                                                               \
        else                                                            \
        {  /* not readable/write to whole register*/   \
            GP_HAL_WRITE_REG( ((UInt16) (Offset)) + Property##_REGISTER, GP_HAL_VAL2PLACEDVAL(Value, Property));    \
        }                                                               \
    } while (0)

/** @brief A macro to write a specific field (=property)  in a register,
 * for a property that is offset based.
 *
 * A macro to write a specific field (= property) in a register,
 * for a property that is offset based.
 * Register definitions are needed to preprocess these instructions.
 * The register must be relative to a base address: for use with PBM, ES and other repeating structures.
 *
 *  @param Offset Base address of the entry to which the property applies.
 *  @param Property Property name (only use with regprop definition).
 *  @param Value    Value to be set.
*/
#define GP_HAL_WRITE_PROP_OFFSET(Offset, Property, Value) GP_HAL_WRITE_PROP_OFFSET_AUX(Offset, Property, Value)

/** @brief A macro to write a specific field (= property) in a register .
 *
 *  A macro to write a specific field (= property) in a register. Register definitions are needed to preprocess these instructions .
 *
 *  @param Property Property name (only use with regprop definition).
 *  @param Value    Value to be set.
*/
#define GP_HAL_WRITE_PROP(Property, Value) GP_HAL_WRITE_PROP_OFFSET(0, Property, Value)


/** internal*/
#define GP_HAL_UNSAFE_WRITE_PROP(Property, Value) GP_HAL_WRITE_PROP_OFFSET_AUX(0, Property, Value)

/* use this to force low-level signals */
#define GP_HAL_TDC_ENABLE(Property, Value)                      \
    do                                                          \
    {                                                           \
        GP_HAL_WRITE_PROP(Property, Value);                     \
        GP_HAL_UNSAFE_WRITE_PROP(Property##_TDC_ENABLE, true);  \
    } while (false)


/** part of implementation, don't use */
#define GP_HAL_PLACEDVAL2VAL(Byte, Prop)                        \
    ( GP_HAL_IS_ONLY_PROP_IN_REG(Prop)                          \
      ? (((UInt8) (Byte)) >> Prop##_LSB)                        \
      : ((((UInt8) (Byte)) & Prop##_MASK) >> Prop##_LSB)        \
      )

#define GP_HAL_BASE_READ_PROPFROM(Byte, Property)                       \
    ( COMPILE_TIME_ASSERT(Property##_READABLE),                         \
      (UInt8) GP_HAL_PLACEDVAL2VAL(Byte, Property)                      \
      )

/** @brief Read a property from a byte buffer.
 * A macro to read a specific bit field (=property) of a byte buffer.
 *
 * @param Byte   The byte buffer.
 * @param Reg    The register of the property to access.  This parameter is used in order to check
 *               the consistency between the byte buffer and the accessed property.
 * @param Property Property name (only use with regprop definition).
*/
#define GP_HAL_READ_PROPFROM(Byte, Reg, Property)          \
    ( GP_HAL_CHECK_PROP_MATCHES_REG(Property, Reg), \
      GP_HAL_BASE_READ_PROPFROM(Byte, Property)     \
      )


/** main code in auxiliary macro to make property substitution work as intended */
#define GP_HAL_READ_PROP_OFFSET_AUX(Offset, Property) \
    GP_HAL_BASE_READ_PROPFROM(GP_HAL_READ_REG(((UInt16) (Offset)) + Property##_REGISTER), Property)

/** @brief A macro to read a specific field (= property) of a register,
 * for a property that is offset based
 *
 * A macro to read a specific field (= property) of a register,
 * for a property that is offset based.
 * Register definitions are needed to preprocess these instructions.
 *
 * @param Offset Base address of the entry to which the property applies.
 * @param Property Property name (only use with regprop definition).
*/
#define GP_HAL_READ_PROP_OFFSET(Offset, Property) GP_HAL_READ_PROP_OFFSET_AUX(Offset, Property)

/** @brief A macro to read a specific field (= property) of a register.
 *
 *  A macro to read a specific field (= property) of a register. Register definitions are needed to preprocess these instructions.
 *
 *  @param Property Property name (only use with regprop definition).
*/
#define GP_HAL_READ_PROP(Property) GP_HAL_READ_PROP_OFFSET(0, Property)


/** @brief A macro to modify some bits of a specific field (= property) in a register .
 *
 *  A macro to modify some bits in a specific field (= property) in a register. Register definitions are needed to preprocess these instructions .
 *
 *  @param Prop    Property name (only use with regprop definition).
 *  @param Mask    The read-modify-write mask.
 *  @param Data    The Value to write.
*/
#define GP_HAL_READMODIFYWRITE_PROP(Prop, Mask, Data)   do                 \
{                                                                   \
    UInt8 newData = (GP_HAL_READ_PROP(Prop) & ~(Mask)) | ((Data) & (Mask));    \
    GP_HAL_WRITE_PROP(Prop, newData);                                          \
} while (false)

/* These macros read or write 2,4 or 8 bytes. (no endiannness conversion)
 * if a particular implementation has efficient methods to access two/four/eight bytes, it can provide it's own implementations.
 * (macro or inline). It should however set the various HAVE_ macros.
 * for ease of use we define here default implementations.
 */
#ifndef GP_HAL_HAVE_READ_TWO_BYTES
#define GP_HAL_READ_TWO_BYTES(Address, pBuffer) GP_HAL_READ_BYTE_STREAM(Address, pBuffer, 2)
#endif
#ifndef GP_HAL_HAVE_READ_FOUR_BYTES
#define GP_HAL_READ_FOUR_BYTES(Address, pBuffer) GP_HAL_READ_BYTE_STREAM(Address, pBuffer, 4)
#endif
#ifndef GP_HAL_HAVE_READ_EIGHT_BYTES
#define GP_HAL_READ_EIGHT_BYTES(Address, pBuffer) GP_HAL_READ_BYTE_STREAM(Address, pBuffer, 8)
#endif
#ifndef GP_HAL_HAVE_WRITE_TWO_BYTES
#define GP_HAL_WRITE_TWO_BYTES(Address, pBuffer) GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, 2)
#endif
#define GP_HAL_WRITE_THREE_BYTES(Address, pBuffer) GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, 3)
#ifndef GP_HAL_HAVE_WRITE_FOUR_BYTES
#define GP_HAL_WRITE_FOUR_BYTES(Address, pBuffer) GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, 4)
#endif
#ifndef GP_HAL_HAVE_WRITE_EIGHT_BYTES
#define GP_HAL_WRITE_EIGHT_BYTES(Address, pBuffer) GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, 8)
#endif

/** @brief A macro to read a 16 bit value.
 *
 *  A macro to read a 16 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to read
 *  @param pBuffer    The buffer where the data is returned.
*/
#define GP_HAL_READ_REGS16(Address, pBuffer)         do { GP_HAL_READ_TWO_BYTES(Address, pBuffer); RF_TO_HOST_UINT16(pBuffer); } while(false)

/** @brief A macro to read a 32 bit value.
 *
 *  A macro to read a 32 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to read
 *  @param pBuffer    The buffer where the data is returned.
*/
#define GP_HAL_READ_REGS32(Address, pBuffer)         do { GP_HAL_READ_FOUR_BYTES(Address, pBuffer); RF_TO_HOST_UINT32(pBuffer); } while(false)

/** @brief A macro to read a 64 bit value.
 *
 *  A macro to read a 64 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to read
 *  @param pBuffer    The buffer where the data is returned.
*/
#define GP_HAL_READ_REGS64(Address, pBuffer)         do { GP_HAL_READ_EIGHT_BYTES(Address, pBuffer); RF_TO_HOST_UINT64(pBuffer); } while(false)

/** @brief A macro to write a 16 bit value.
 *
 *  A macro to write a 16 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to write
 *  @param pBuffer    The buffer where the data to write is read from.
*/
#define GP_HAL_WRITE_REGS16(Address,pBuffer)         do { HOST_TO_RF_UINT16(pBuffer); GP_HAL_WRITE_TWO_BYTES(Address, pBuffer); RF_TO_HOST_UINT16(pBuffer); } while(false)

/** @brief A macro to write a 24 bit value.
 *
 *  A macro to write a 24 bit value.  The macro takes care of the endianness of the host processor. The argument should be a 32-bit value.
 *
 *  @param Address    The address of the value to write
 *  @param pBuffer    The buffer where the data to write is read from.
*/
#define GP_HAL_WRITE_REGS24(Address,pBuffer)         do { HOST_TO_RF_UINT32(pBuffer); GP_HAL_WRITE_THREE_BYTES(Address, pBuffer); RF_TO_HOST_UINT32(pBuffer); } while(false)

/** @brief A macro to write a 32 bit value.
 *
 *  A macro to write a 32 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to write
 *  @param pBuffer    The buffer where the data to write is read from.
*/
#define GP_HAL_WRITE_REGS32(Address,pBuffer)         do { HOST_TO_RF_UINT32(pBuffer); GP_HAL_WRITE_FOUR_BYTES(Address, pBuffer); RF_TO_HOST_UINT32(pBuffer); } while(false)

/** @brief A macro to write a 64 bit value.
 *
 *  A macro to write a 64 bit value.  The macro takes care of the endianness of the host processor.
 *
 *  @param Address    The address of the value to write
 *  @param pBuffer    The buffer where the data to write is read from.
*/
#define GP_HAL_WRITE_REGS64(Address,pBuffer)         do { HOST_TO_RF_UINT64(pBuffer); GP_HAL_WRITE_EIGHT_BYTES(Address, pBuffer); RF_TO_HOST_UINT64(pBuffer); } while(false)

//------------------------------------------------------------------------------
//  INTERRUPT CONTROL COMMANDS
//------------------------------------------------------------------------------

//Turn off/on all interrupt sources of the micro processor
/** @brief Enable the interrupts on the micro processor*/
#define ENABLE_GP_GLOBAL_INT()      HAL_ENABLE_GLOBAL_INT()
/** @brief Disables the interrupts on the micro processor */
#define DISABLE_GP_GLOBAL_INT()     HAL_DISABLE_GLOBAL_INT()

#endif //_HAL_GP_HW_H_

