/***************************************************************************//**
 * @file
 * @brief File System Utility Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_UTILS_PRIV_H_
#define  FS_UTILS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <common/include/rtos_path.h>

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       FILE SYSTEM COUNTER MACRO'S
 *
 * Description : Increment file system counter(s).
 *
 * Argument(s) : Various file system counter variable(s) & values.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) These macro's are INTERNAL file system suite macro's & SHOULD NOT be called by
 *                   application function(s).
 *******************************************************************************************************/

//                                                                 ---------------- GENERIC CTR MACRO'S ---------------
#define  FS_CTR_INC(ctr)                                     do { (ctr)++; } while (0)

#define  FS_CTR_INC_LARGE(ctr_hi, ctr_lo) \
  do {                                    \
    (ctr_lo)++;                           \
    if ((ctr_lo) == 0u) {                 \
      (ctr_hi)++;                         \
    }                                     \
  } while (0)

#define  FS_CTR_ADD(ctr, val)                                do { (ctr) += (val); } while (0)

#define  FS_ERR_CHK_EMPTY_RTN_ARG

#define  FS_ERR_CHK_RTNEXPR(call, callOnError, retExpr) \
  call;                                                 \
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {     \
    callOnError;                                        \
    return retExpr;                                     \
  }

#define  FS_ERR_CHK_RTN(call, callOnError, retVal)       FS_ERR_CHK_RTNEXPR(call, callOnError, (retVal))

#define  FS_ERR_CHK(call, callOnError)                   FS_ERR_CHK_RTNEXPR(call, callOnError, FS_ERR_CHK_EMPTY_RTN_ARG)

/********************************************************************************************************
 *                                   POWER-2 MULTIPLY/DIVIDE MACRO'S
 *
 * Note(s) : (1) Multiplications & divisions by powers of 2 are common within the file system suite.
 *               If the power-of-2 multiplication or divisor is a constant, a compiler can optimize the
 *               calculation (typically encoding it as a logical shift).  However, many of the powers-of-2
 *               multiplicands & divisors are known only at run-time, so the integer multiplications &
 *               divisions lose important information that could have been used for optimization.
 *
 *               Multiplications & divisions by powers of 2 within this file system suite are performed
 *               with macros 'FS_UTIL_MULT_PWR2' & 'FS_UTIL_DIV_PWR2', using left & right shifts.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_UTIL_MULT_PWR2()
 *
 * @brief    Multiple integer by a power of 2.
 *
 * @param    nbr     First multiplicand.
 *
 * @param    pwr     Power of second multiplicand.
 *
 * @return   Product = nbr * 2^pwr.
 *
 * @note     (1) As stated in ISO/IEC 9899:TCP 6.5.8(4) :
 *
 *                   "The result of E1 << E2 is E1 left-shifted E2 bit-positions; vacated bits are
 *                   filled with zeros.  If E1 has an unsigned type, the value of the result is E1 x
 *                   2^E2, reduces modulo one more than the maximum value representable in the result
 *                   type."
 *
 *               Even with conforming compilers, this macro MAY ONLY be used with unsigned operands.
 *               Results with signed operands are undefined.
 *
 * @note     (2) With a non-conforming compiler, this macro should be commented out & redefined.
 *******************************************************************************************************/

#define  FS_UTIL_MULT_PWR2(nbr, pwr)               ((nbr) << (pwr))

#define  FS_UTIL_PWR2(pwr)                          FS_UTIL_MULT_PWR2(1u, (pwr))

/****************************************************************************************************//**
 *                                           FS_UTIL_DIV_PWR2()
 *
 * @brief    Divide integer by a power of 2.
 *
 * @param    nbr     Dividend.
 *
 * @param    pwr     Power of divisor.
 *
 * @return   Quotient = nbr / 2^pwr.
 *
 * @note     (1) As stated in ISO/IEC 9899:TCP 6.5.8(5) :
 *
 *                   "The result of E1 >> E2 is E1 right-shifted E2 bit-positions.  If E1 has an
 *                   unsigned type ..., the value of the result is the integral part of the quotient
 *                   of E1 / 2^E2."
 *
 *               Even with conforming compilers, this macro MAY ONLY be used with unsigned operands.
 *               Results with signed operands are undefined.
 *
 * @note     (2) With a non-conforming compiler, this macro should be commented out & redefined.
 *******************************************************************************************************/

#define  FS_UTIL_DIV_PWR2(nbr, pwr)                 ((nbr) >> (pwr))

#define  FS_UTIL_MODULO_PWR2(nbr, pwr)              ((nbr) & (FS_UTIL_PWR2(pwr) - 1u))

/****************************************************************************************************//**
 *                                               FS_UTIL_IS_PWR2()
 *
 * @brief    Determine whether unsigned integer is a power of 2 or not.
 *
 * @param    nbr     Unsigned integer.
 *
 * @return   DEF_YES, if integer is     a power of 2.
 *           DEF_NO,  if integer is not a power of 2.
 *******************************************************************************************************/

#define  FS_UTIL_IS_PWR2(nbr)                     ((((nbr) != 0u) && (((nbr) & ((nbr) - 1u)) == 0u)) ? DEF_YES : DEF_NO)

/****************************************************************************************************//**
 *                                               FS_UTIL_IS_ODD()
 *
 * @brief    Determine whether unsigned integer is odd.
 *
 * @param    nbr     Unsigned integer.
 *
 * @return   DEF_YES, if integer is odd.
 *           DEF_NO,  if integer is even.
 *******************************************************************************************************/

#define  FS_UTIL_IS_ODD(nbr)                        (DEF_BIT_IS_SET((nbr), DEF_BIT_00))

/****************************************************************************************************//**
 *                                               FS_UTIL_IS_EVEN()
 *
 * @brief    Determine whether unsigned integer is even.
 *
 * @param    nbr     Unsigned integer.
 *
 * @return   DEF_YES, if integer is even.
 *           DEF_NO,  if integer is odd.
 *******************************************************************************************************/

#define  FS_UTIL_IS_EVEN(nbr)                       (DEF_BIT_IS_CLR((nbr), DEF_BIT_00))

/********************************************************************************************************
 *                                   BIT/OCTET MANIPULATION MACRO'S
 *
 * Note(s) : (1) These macros allow to perform mutliple conversions between bits and octets, either at
 *               runtime or compile time.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_UTIL_BIT_NBR_TO_OCTET_NBR()
 *
 * @brief    Convert a number of required bits (generally for a bitmap) into a number of required
 *           octets.
 *
 * @param    bit_nbr     Number of required bits.
 *
 * @return   The lowest number of octets that contains at least bit_nbr bits.
 *******************************************************************************************************/

#define  FS_UTIL_BIT_NBR_TO_OCTET_NBR(bit_nbr)                ( ((bit_nbr) >> DEF_OCTET_TO_BIT_SHIFT) \
                                                                + (((bit_nbr) &  DEF_OCTET_TO_BIT_MASK) == 0u ? 0u : 1u))

/****************************************************************************************************//**
 *                                           FS_UTIL_BITMAP_LOC_GET()
 *
 * @brief    Convert the position of a bit in a bitmap to the equivalent location of an octet in
 *           this bitmap, and the location of the bit in this octet.
 *
 * @param    bit_pos     Position of the bit in the bitmap/array.
 *
 * @param    octet_loc   Location of the octet in the bitmap/array.
 *
 * @param    bit_loc     Location of the bit in the octet.
 *
 * @note     (1) Care must be taken not to use the same variable for 'bit_pos' and 'bit_loc', because
 *               it is modified before being used for the calculation of 'octet_loc'.
 *******************************************************************************************************/

#define  FS_UTIL_BITMAP_LOC_GET(bit_pos, octet_loc, bit_loc) \
  do {                                                       \
    (bit_loc) = (bit_pos)  & DEF_OCTET_TO_BIT_MASK;          \
    (octet_loc) = (bit_pos) >> DEF_OCTET_TO_BIT_SHIFT;       \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT08U FSUtil_Log2(CPU_INT32U val);

CPU_INT08U FSUtil_Log2Floor(CPU_INT32U val);

void FSUtil_ValPack32(CPU_INT08U *p_dest,
                      CPU_SIZE_T *p_offset_octet,
                      CPU_DATA   *p_offset_bit,
                      CPU_INT32U val,
                      CPU_DATA   nbr_bits);

CPU_INT32U FSUtil_ValUnpack32(CPU_INT08U *p_src,
                              CPU_SIZE_T *p_offset_octet,
                              CPU_DATA   *p_offset_bit,
                              CPU_DATA   nbr_bits);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
