/***************************************************************************//**
 * @file
 * @brief Common - Custom Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup LIB_UTILS LIB Utilities API
 * @ingroup  LIB
 * @brief    LIB Utilities API
 *
 * @addtogroup LIB_UTILS
 * @{
 ********************************************************************************************************
 * @note     (1) This file is intended to regroup LIB capabilities that depends on CPU elements.
 *               'lib_def.h' should be used if CPU is not needed.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _LIB_UTILS_H_
#define  _LIB_UTILS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <cpu/include/cpu_def.h>

#include  <common/include/rtos_path.h>
#include  <cpu_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               BIT MACRO'S
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   DEF_BIT()
 *
 * @brief    Create bit mask with single, specified bit set.
 *
 * @param    bit     Bit number of bit to set.
 *
 * @return   Bit mask with single, specified bit set.
 *
 * @note     (1) 'bit' SHOULD be a non-negative integer.
 *
 * @note     (2) 'bit' values that overflow the target CPU &/or compiler environment (e.g. negative
 *               or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
 *******************************************************************************************************/

#define  DEF_BIT(bit)                                                   (1u << (bit))

/****************************************************************************************************//**
 *                                               DEF_BITxx()
 *
 * @brief    Create bit mask of specified bit size with single, specified bit set.
 *
 * @param    bit     Bit number of bit to set.
 *
 * @return   Bit mask with single, specified bit set.
 *
 * @note     (1) 'bit' SHOULD be a non-negative integer.
 *
 * @note     (2) 'bit' values that overflow the target CPU &/or compiler environment (e.g. negative
 *               or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
 *               @n
 *               To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
 *               bit constant '1' is cast to specified integer data type size.
 *
 * @note     (3) Ideally, DEF_BITxx() macro's should be named DEF_BIT_xx(); however, these names already
 *               previously-released for bit constant #define's (see 'STANDARD DEFIN   BIT DEFINES').
 *******************************************************************************************************/

#define  DEF_BIT08(bit)                        ((CPU_INT08U)((CPU_INT08U)1u << (bit)))

#define  DEF_BIT16(bit)                        ((CPU_INT16U)((CPU_INT16U)1u << (bit)))

#define  DEF_BIT32(bit)                        ((CPU_INT32U)((CPU_INT32U)1u << (bit)))

#define  DEF_BIT64(bit)                        ((CPU_INT64U)((CPU_INT64U)1u << (bit)))

/****************************************************************************************************//**
 *                                               DEF_BIT_MASK()
 *
 * @brief    Shift a bit mask.
 *
 * @param    bit_mask    Bit mask to shift.
 *
 * @param    bit_shift   Number of bit positions to left-shift bit mask.
 *
 * @return   Shifted bit mask.
 *
 * @note     (1) 'bit_mask'  SHOULD be an unsigned    integer.
 *               'bit_shift' SHOULD be a non-negative integer.
 *
 * @note     (2) 'bit_shift' values that overflow the target CPU &/or compiler environment (e.g. negative
 *               or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
 *******************************************************************************************************/

#define  DEF_BIT_MASK(bit_mask, bit_shift)                                     ((bit_mask) << (bit_shift))

/****************************************************************************************************//**
 *                                               DEF_BIT_MASK_xx()
 *
 * @brief    Shift a bit mask of specified bit size.
 *
 * @param    bit_mask    Bit mask to shift.
 *
 * @param    bit_shift   Number of bit positions to left-shift bit mask.
 *
 * @return   Shifted bit mask.
 *
 * @note     (1) 'bit_mask'  SHOULD be an unsigned    integer.
 *
 * @note     (2) 'bit_shift' SHOULD be a non-negative integer.
 *
 * @note     (3) 'bit_shift' values that overflow the target CPU &/or compiler environment (e.g. negative
 *               or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
 *******************************************************************************************************/

#define  DEF_BIT_MASK_08(bit_mask, bit_shift)         ((CPU_INT08U)((CPU_INT08U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_16(bit_mask, bit_shift)         ((CPU_INT16U)((CPU_INT16U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_32(bit_mask, bit_shift)         ((CPU_INT32U)((CPU_INT32U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_64(bit_mask, bit_shift)         ((CPU_INT64U)((CPU_INT64U)(bit_mask) << (bit_shift)))

/****************************************************************************************************//**
 *                                               DEF_BIT_FIELD()
 *
 * @brief    Create & shift a contiguous bit field.
 *
 * @param    bit_field   Number of contiguous bits to set in the bit field.
 *
 * @param    bit_shift   Number of bit positions   to left-shift bit field.
 *
 * @return   Shifted bit field.
 *
 * @note     (1) 'bit_field' & 'bit_shift' SHOULD be non-negative integers.
 *
 * @note     (2) 'bit_field'/'bit_shift' values that overflow the target CPU &/or compiler
 *               environment (e.g. negative or greater-than-CPU-data-size values) MAY generate
 *               compiler warnings &/or errors.
 *           - (a) To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
 *                   bit constant '1' is suffixed with 'L'ong integer modifier.
 *                   @n
 *                   This may still be insufficient for CPUs &/or compilers that support 'long long'
 *                   integer data types, in which case 'LL' integer modifier should be suffixed.
 *                   However, since almost all 16- & 32-bit CPUs & compilers support 'long' integer
 *                   data types but many may NOT support 'long long' integer data types, only 'long'
 *                   integer data types & modifiers are supported.
 *******************************************************************************************************/

#define  DEF_BIT_FIELD(bit_field, bit_shift)                                 ((((bit_field) >= DEF_INT_CPU_NBR_BITS) ? (DEF_INT_CPU_U_MAX_VAL) \
                                                                               : (DEF_BIT(bit_field) - 1uL))                                   \
                                                                              << (bit_shift))

/****************************************************************************************************//**
 *                                           DEF_BIT_FIELD_xx()
 *
 * @brief    Create & shift a contiguous bit field of specified bit size.
 *
 * @param    bit_field   Number of contiguous bits to set in the bit field.
 *
 * @param    bit_shift   Number of bit positions   to left-shift bit field.
 *
 * @return   Shifted bit field.
 *
 * @note     (1) 'bit_field' & 'bit_shift' SHOULD be non-negative integers.
 *
 * @note     (2) 'bit_field'/'bit_shift' values that overflow the target CPU &/or compiler
 *               environment (e.g. negative or greater-than-CPU-data-size values) MAY generate
 *               compiler warnings &/or errors.
 *               @n
 *               To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
 *               bit constant '1' is cast to specified integer data type size.
 *******************************************************************************************************/

#define  DEF_BIT_FIELD_08(bit_field, bit_shift)     ((CPU_INT08U)((((CPU_INT08U)(bit_field) >= (CPU_INT08U)DEF_INT_08_NBR_BITS) ? (CPU_INT08U)(DEF_INT_08U_MAX_VAL) \
                                                                   : (CPU_INT08U)(DEF_BIT08(bit_field) - (CPU_INT08U)1u))                                           \
                                                                  << (bit_shift)))

#define  DEF_BIT_FIELD_16(bit_field, bit_shift)     ((CPU_INT16U)((((CPU_INT16U)(bit_field) >= (CPU_INT16U)DEF_INT_16_NBR_BITS) ? (CPU_INT16U)(DEF_INT_16U_MAX_VAL) \
                                                                   : (CPU_INT16U)(DEF_BIT16(bit_field) - (CPU_INT16U)1u))                                           \
                                                                  << (bit_shift)))

#define  DEF_BIT_FIELD_32(bit_field, bit_shift)     ((CPU_INT32U)((((CPU_INT32U)(bit_field) >= (CPU_INT32U)DEF_INT_32_NBR_BITS) ? (CPU_INT32U)(DEF_INT_32U_MAX_VAL) \
                                                                   : (CPU_INT32U)(DEF_BIT32(bit_field) - (CPU_INT32U)1u))                                           \
                                                                  << (bit_shift)))

#define  DEF_BIT_FIELD_64(bit_field, bit_shift)     ((CPU_INT64U)((((CPU_INT64U)(bit_field) >= (CPU_INT64U)DEF_INT_64_NBR_BITS) ? (CPU_INT64U)(DEF_INT_64U_MAX_VAL) \
                                                                   : (CPU_INT64U)(DEF_BIT64(bit_field) - (CPU_INT64U)1u))                                           \
                                                                  << (bit_shift)))

/****************************************************************************************************//**
 *                                               DEF_BIT_SET()
 *
 * @brief    Set specified bit(s) in a value.
 *
 * @param    val     Value to modify by setting specified bit(s).
 *
 * @param    mask    Mask of bits to set.
 *
 * @return   Modified value with specified bit(s) set.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define  DEF_BIT_SET(val, mask)                        ((val) = ((val) | (mask)))

/****************************************************************************************************//**
 *                                               DEF_BIT_SET_xx()
 *
 * @brief    Set specified bit(s) in a value of specified bit size.
 *
 * @param    val     Value to modify by setting specified bit(s).
 *
 * @param    mask    Mask of bits to set.
 *
 * @return   Modified value with specified bit(s) set.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) These macros are deprecated and should be replaced by the DEF_BIT_SET macro.
 *******************************************************************************************************/

#define  DEF_BIT_SET_08(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_16(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_32(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_64(val, mask)                     DEF_BIT_SET((val), (mask))

/****************************************************************************************************//**
 *                                               DEF_BIT_CLR_xx()
 *
 * @brief    Clear specified bit(s) in a value of specified bit size.
 *
 * @param    val     Value to modify by clearing specified bit(s).
 *
 * @param    mask    Mask of bits to clear.
 *
 * @return   Modified value with specified bit(s) clear.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) These macros are deprecated and should be replaced by the DEF_BIT_CLR macro.
 *******************************************************************************************************/

#define  DEF_BIT_CLR_08(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_16(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_32(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_64(val, mask)                     DEF_BIT_CLR((val), (mask))

/****************************************************************************************************//**
 *                                               DEF_BIT_CLR()
 *
 * @brief    Clear specified bit(s) in a value.
 *
 * @param    val     Value to modify by clearing specified bit(s).
 *
 * @param    mask    Mask of bits to clear.
 *
 * @return   Modified value with specified bit(s) clear.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2)  'mask' SHOULD be cast with the same data type than 'val'.
 *******************************************************************************************************/

#define  DEF_BIT_CLR(val, mask)                 ((val) = ((val) & (~(mask))))

/****************************************************************************************************//**
 *                                               DEF_BIT_TOGGLE()
 *
 * @brief    Toggles specified bit(s) in a value.
 *
 * @param    val     Value to modify by toggling specified bit(s).
 *
 * @param    mask    Mask of bits to toggle.
 *
 * @return   Modified value with specified bit(s) toggled.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define  DEF_BIT_TOGGLE(val, mask)                      ((val) ^= (mask))

/****************************************************************************************************//**
 *                                           DEF_BIT_FIELD_RD()
 *
 * @brief    Reads a 'val' field, masked and shifted, given by mask 'field_mask'.
 *
 * @param    val         Value to read from.
 *
 * @param    field_mask  Mask of field to read. See note #1, #2 and #3.
 *
 * @return   Field value, masked and right-shifted to bit position 0.
 *
 * @note     (1) 'field_mask' argument must NOT be 0.
 *
 * @note     (2) 'field_mask' argument must contain a mask with contiguous set bits.
 *
 * @note     (3) 'val' & 'field_mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define  DEF_BIT_FIELD_RD(val, field_mask)              (((val) & (field_mask)) / ((field_mask) & ((~(field_mask)) + 1u)))

/****************************************************************************************************//**
 *                                           DEF_BIT_FIELD_ENC()
 *
 * @brief    Encodes given 'field_val' at position given by mask 'field_mask'.
 *
 * @param    field_val   Value to encode.
 *
 * @param    field_mask  Mask of field to read. See note #1 and #2.
 *
 * @return   Field value, masked and left-shifted to field position.
 *
 * @note     (1) 'field_mask' argument must contain a mask with contiguous set bits.
 *
 * @note     (2) 'field_val' & 'field_mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define  DEF_BIT_FIELD_ENC(field_val, field_mask)       (((field_val) * ((field_mask) & ((~(field_mask)) + 1u))) & (field_mask))

/****************************************************************************************************//**
 *                                           DEF_BIT_FIELD_WR()
 *
 * @brief    Writes 'field_val' field at position given by mask 'field_mask' in variable 'var'.
 *
 * @param    var         Variable to write field to. See note #2.
 *
 * @param    field_val   Desired value for field. See note #2.
 *
 * @param    field_mask  Mask of field to write to. See note #1 and #2.
 *
 * @note     (1) 'field_mask' argument must contain a mask with contiguous set bits.
 *
 * @note     (2) 'var', 'field_val' & 'field_mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define  DEF_BIT_FIELD_WR(var, field_val, field_mask)   (var) = (((var) & ~(field_mask)) | DEF_BIT_FIELD_ENC((field_val), (field_mask)))

/****************************************************************************************************//**
 *                                               DEF_BIT_IS_SET()
 *
 * @brief    Determine if specified bit(s) in a value are set.
 *
 * @param    val     Value to check for specified bit(s) set.
 *
 * @param    mask    Mask of bits to check if set (see Note #2).
 *
 * @return   DEF_YES, if ALL specified bit(s) are     set in value.
 *
 *           DEF_NO,  if ALL specified bit(s) are NOT set in value.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
 *******************************************************************************************************/

#define  DEF_BIT_IS_SET(val, mask)                    (((((val) & (mask)) == (mask)) \
                                                        && ((mask) != 0u))    ? (DEF_YES) : (DEF_NO))

/****************************************************************************************************//**
 *                                               DEF_BIT_IS_CLR()
 *
 * @brief    Determine if specified bit(s) in a value are clear.
 *
 * @param    val     Value to check for specified bit(s) clear.
 *
 * @param    mask    Mask of bits to check if clear (see Note #2).
 *
 * @return   DEF_YES, if ALL specified bit(s) are     clear in value.
 *
 *           DEF_NO,  if ALL specified bit(s) are NOT clear in value.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
 *******************************************************************************************************/

#define  DEF_BIT_IS_CLR(val, mask)                    (((((val) & (mask)) == 0u) \
                                                        && ((mask) != 0u))  ? (DEF_YES) : (DEF_NO))

/****************************************************************************************************//**
 *                                           DEF_BIT_IS_SET_ANY()
 *
 * @brief    Determine if any specified bit(s) in a value are set.
 *
 * @param    val     Value to check for specified bit(s) set.
 *
 * @param    mask    Mask of bits to check if set (see Note #2).
 *
 * @return   DEF_YES, if ANY specified bit(s) are     set in value.
 *
 *           DEF_NO,  if ALL specified bit(s) are NOT set in value.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
 *******************************************************************************************************/

#define  DEF_BIT_IS_SET_ANY(val, mask)               ((((val) & (mask)) == 0u)     ? (DEF_NO) : (DEF_YES))

/****************************************************************************************************//**
 *                                           DEF_BIT_IS_CLR_ANY()
 *
 * @brief    Determine if any specified bit(s) in a value are clear.
 *
 * @param    val     Value to check for specified bit(s) clear.
 *
 * @param    mask    Mask of bits to check if clear (see Note #2).
 *
 * @return   DEF_YES, if ANY specified bit(s) are     clear in value.
 *
 *           DEF_NO,  if ALL specified bit(s) are NOT clear in value.
 *
 * @note     (1) 'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
 *******************************************************************************************************/

#define  DEF_BIT_IS_CLR_ANY(val, mask)               ((((val) & (mask)) == (mask))  ? (DEF_NO) : (DEF_YES))

/****************************************************************************************************//**
 *                                           DEF_GET_U_MAX_VAL()
 *
 * @brief    Get the maximum unsigned value that can be represented in an unsigned integer variable
 *               of the same data type size as an object.
 *
 * @param    obj     Object or data type to return maximum unsigned value (see Note #1).
 *
 * @return   Maximum unsigned integer value that can be represented by the object, if NO error(s).
 *           0, otherwise.
 *
 * @note     (1) 'obj' SHOULD be an integer object or data type but COULD also be a character or
 *               pointer object or data type.
 *******************************************************************************************************/

#if     (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_08)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL : 0)

#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_16)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL \
                                                 : ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL : 0))

#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_32)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL    \
                                                 : ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL \
                                                    : ((sizeof(obj) == CPU_WORD_SIZE_32) ? DEF_INT_32U_MAX_VAL : 0)))

#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_64)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL       \
                                                 : ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL    \
                                                    : ((sizeof(obj) == CPU_WORD_SIZE_32) ? DEF_INT_32U_MAX_VAL \
                                                       : ((sizeof(obj) == CPU_WORD_SIZE_64) ? DEF_INT_64U_MAX_VAL : 0))))

#else

#error  CPU_CFG_DATA_SIZE_MAX illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif

/****************************************************************************************************//**
 *                                             CONTAINER_OF()
 *
 * @brief    Find pointer to structure type 'parent_type', containing 'p_member'.
 *
 * @param    p_member        Pointer to member of structure of which the pointer to container structure is
 *                           found.
 *
 * @param    parent_type     Name of the parent structure data type.
 *
 * @param    member          Name of the member field, in the parent structure data type.
 *
 * @return   Pointer to structure containing p_member.
 *
 * @note     (1) 'p_member' SHOULD NOT be DEF_NULL.
 *******************************************************************************************************/

#define  CONTAINER_OF(p_member, parent_type, member)  (parent_type *)((CPU_ADDR)(p_member) - ((CPU_ADDR)(&((parent_type *)0)->member)))

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of lib utils module include.
