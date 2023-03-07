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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/source/shared/fs_utils_priv.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH        (FS, CORE, UTIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSUtil_Log2()
 *
 * @brief    Calculate ceiling of base-2 logarithm of integer.
 *
 * @param    val     Integer value (must be greater than zero).
 *
 * @return   Logarithm of value.
 *
 * @note     (1) This function is an INTERNAL file system suite function & MUST NOT be called by
 *               application function(s).
 *******************************************************************************************************/
CPU_INT08U FSUtil_Log2(CPU_INT32U val)
{
  CPU_SIZE_T log_val;
  CPU_SIZE_T val_cmp;

  RTOS_ASSERT_DBG(val != 0u, RTOS_ERR_INVALID_ARG, 0u);

  val_cmp = 1u;
  log_val = 0u;
  while (val_cmp < val) {
    val_cmp *= 2u;
    log_val += 1u;
  }

  return (log_val);
}

/****************************************************************************************************//**
 *                                           FSUtil_Log2Floor()
 *
 * @brief    Calculate floor of base-2 logarithm of integer.
 *
 * @param    val     Integer value (must be greater than zero).
 *
 * @return   Logarithm of value.
 *
 * @note     (1) This function is an INTERNAL file system suite function & MUST NOT be called by
 *               application function(s).
 *******************************************************************************************************/
CPU_INT08U FSUtil_Log2Floor(CPU_INT32U val)
{
  CPU_SIZE_T log_val;
  CPU_SIZE_T val_cmp;

  RTOS_ASSERT_DBG(val != 0u, RTOS_ERR_INVALID_ARG, 0u);

  val_cmp = 1u;
  log_val = 0u;
  while (val_cmp <= val) {
    val_cmp *= 2u;
    log_val += 1u;
  }

  return (log_val - 1u);
}

/****************************************************************************************************//**
 *                                           FSUtil_ValPack32()
 *
 * @brief    Packs a specified number of least significant bits of a 32-bit value to a specified octet
 *           and bit position within an octet array, in little-endian order.
 *
 * @param    p_dest          Pointer to destination octet array.
 *
 * @param    p_offset_octet  Pointer to octet offset into 'p_dest'. This function adjusts the pointee
 *                           to the new octet offset within octet array.
 *
 * @param    p_offset_bit    Pointer to bit offset into initial 'p_dest[*p_offset_octet]'. This function
 *                           ajusts the pointee to the new bit offset within octet.
 *
 * @param    val             Value to pack into 'p_dest' array.
 *
 * @param    nbr_bits        Number of least-significants bits of 'val' to pack into 'p_dest'.
 *******************************************************************************************************/
void FSUtil_ValPack32(CPU_INT08U *p_dest,
                      CPU_SIZE_T *p_offset_octet,
                      CPU_DATA   *p_offset_bit,
                      CPU_INT32U val,
                      CPU_DATA   nbr_bits)
{
  CPU_INT32U val_32_rem;
  CPU_DATA   nbr_bits_rem;
  CPU_DATA   nbr_bits_partial;
  CPU_INT08U val_08;
  CPU_INT08U val_08_mask;
  CPU_INT08U dest_08_mask;
  CPU_INT08U dest_08_mask_lsb;

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG(p_dest != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_offset_octet != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_offset_bit != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(nbr_bits <= sizeof(val) * DEF_OCTET_NBR_BITS, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG(*p_offset_bit < DEF_OCTET_NBR_BITS, RTOS_ERR_INVALID_ARG,; );

  nbr_bits_rem = nbr_bits;
  val_32_rem = val;

  //                                                               ------------ PACK LEADING PARTIAL OCTET ------------
  if (*p_offset_bit > 0) {
    //                                                             Calc nbr bits to pack in initial array octet.
    nbr_bits_partial = DEF_OCTET_NBR_BITS - *p_offset_bit;
    if (nbr_bits_partial > nbr_bits_rem) {
      nbr_bits_partial = nbr_bits_rem;
    }

    val_08_mask = DEF_BIT_FIELD_08(nbr_bits_partial, 0u);       // Calc  mask to apply on 'val_32_rem'.
    val_08 = val_32_rem & val_08_mask;                          // Apply mask.
    val_08 <<= *p_offset_bit;                                   // Shift according to bit offset in leading octet.

    //                                                             Calc mask for kept non-val bits from leading octet.
    dest_08_mask_lsb = *p_offset_bit + nbr_bits_partial;
    dest_08_mask = DEF_BIT_FIELD_08(DEF_OCTET_NBR_BITS - dest_08_mask_lsb, dest_08_mask_lsb);
    dest_08_mask |= DEF_BIT_FIELD_08(*p_offset_bit, 0u);

    p_dest[*p_offset_octet] &= dest_08_mask;                    // Keep      non-val bits from leading array octet.
    p_dest[*p_offset_octet] |= val_08;                          // Merge leading val bits into leading array octet.

    //                                                             Update bit/octet offsets.
    *p_offset_bit += nbr_bits_partial;
    if (*p_offset_bit >= DEF_OCTET_NBR_BITS) {                  // If bit offset > octet nbr bits, ...
      *p_offset_bit = 0u;                                       // ... zero bit offset (offset <= DEF_OCTET_NBR_BITS)
      (*p_offset_octet)++;                                      // ... and inc octet offset.
    }

    //                                                             Update rem'ing val/nbr bits.
    val_32_rem >>= nbr_bits_partial;
    nbr_bits_rem -= nbr_bits_partial;
  }

  //                                                               ---------------- PACK FULL OCTET(S) ----------------
  while (nbr_bits_rem >= DEF_OCTET_NBR_BITS) {
    val_08 = (CPU_INT08U)val_32_rem & DEF_OCTET_MASK;
    p_dest[*p_offset_octet] = val_08;                           // Merge full-octet val bits into array octet.
    (*p_offset_octet)++;                                        // Update octet offset.

    //                                                             Update rem'ing val/nbr bits.
    val_32_rem >>= DEF_OCTET_NBR_BITS;
    nbr_bits_rem -= DEF_OCTET_NBR_BITS;
  }

  //                                                               ----------- PACK TRAILING PARTIAL OCTET ------------
  if (nbr_bits_rem > 0) {
    val_08_mask = DEF_BIT_FIELD_08(nbr_bits_rem, 0u);
    val_08 = (CPU_INT08U)val_32_rem & val_08_mask;              // Mask trailing val bits for merge.

    dest_08_mask = DEF_BIT_FIELD_08(DEF_OCTET_NBR_BITS - nbr_bits_rem,
                                    nbr_bits_rem);

    p_dest[*p_offset_octet] &= dest_08_mask;                    // Keep non-val bits of         trailing array octet.
    p_dest[*p_offset_octet] |= val_08;                          // Merge trailing val bits into trailing array octet.

    *p_offset_bit += nbr_bits_rem;                              // Update/rtn final bit offset.
  }
}

/****************************************************************************************************//**
 *                                           FSUtil_ValUnpack32()
 *
 * @brief    Unpacks a specified number of least-significant bits from a specified octet and bit
 *           position within an octet array, in little-endian order to a 32-bit value.
 *
 * @param    p_src           Pointer to source octet array.
 *
 * @param    p_offset_octet  Pointer to octet offset into 'p_src'. This function adjusts the pointee
 *                           to the new position within octet array.
 *
 * @param    p_offset_bit    Pointer to bit offset into initial 'p_src[*p_offset_octet]'. This function
 *                           ajusts the pointee to the new position within octet array.
 *
 * @param    nbr_bits        Number of least-significants bits to unpack from 'p_src' into value.
 *
 * @return   Unpacked 32-bit value, if no errors;
 *           DEF_INT_32U_MAX_VAL,   otherwise.
 *******************************************************************************************************/
CPU_INT32U FSUtil_ValUnpack32(CPU_INT08U *p_src,
                              CPU_SIZE_T *p_offset_octet,
                              CPU_DATA   *p_offset_bit,
                              CPU_DATA   nbr_bits)
{
  CPU_INT32U val_32;
  CPU_DATA   nbr_bits_partial;
  CPU_DATA   nbr_bits_rem;
  CPU_INT08U val_08;
  CPU_INT08U val_08_mask;

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG(p_src != DEF_NULL, RTOS_ERR_NULL_PTR, DEF_INT_32U_MAX_VAL);
  RTOS_ASSERT_DBG(p_offset_octet != DEF_NULL, RTOS_ERR_NULL_PTR, DEF_INT_32U_MAX_VAL);
  RTOS_ASSERT_DBG(p_offset_bit != DEF_NULL, RTOS_ERR_NULL_PTR, DEF_INT_32U_MAX_VAL);
  RTOS_ASSERT_DBG(nbr_bits <= sizeof(val_32) * DEF_OCTET_NBR_BITS, RTOS_ERR_INVALID_ARG, DEF_INT_32U_MAX_VAL);
  RTOS_ASSERT_DBG(*p_offset_bit < DEF_OCTET_NBR_BITS, RTOS_ERR_INVALID_ARG, DEF_INT_32U_MAX_VAL);

  nbr_bits_rem = nbr_bits;
  val_32 = 0u;
  //                                                               ----------- UNPACK LEADING PARTIAL OCTET -----------
  if (*p_offset_bit > 0) {
    //                                                             Calc nbr of bits to unpack from first initial octet.
    nbr_bits_partial = DEF_OCTET_NBR_BITS - *p_offset_bit;
    if (nbr_bits_partial > nbr_bits) {
      nbr_bits_partial = nbr_bits;
    }

    val_08_mask = DEF_BIT_FIELD_08(nbr_bits_partial, *p_offset_bit);
    val_08 = p_src[*p_offset_octet];
    val_08 &= val_08_mask;                                      // Keep val leading bits.
    val_08 >>= *p_offset_bit;                                   // Shift bit offset to least sig of val.

    val_32 |= (CPU_INT32U)val_08;                               // Merge leading val bits from leading array octet.

    //                                                             Update bit/octet offsets.
    *p_offset_bit += nbr_bits_partial;
    if (*p_offset_bit >= DEF_OCTET_NBR_BITS) {                  // If bit offset > octet nbr bits, ...
      *p_offset_bit = 0u;                                       // ... zero bit offset (offset <= DEF_OCTET_NBR_BITS)
      (*p_offset_octet)++;                                      // ... and inc octet offset.
    }

    nbr_bits_rem -= nbr_bits_partial;                           // Update rem'ing nbr bits.
  }

  //                                                               -------------- UNPACK FULL OCTET(S) ----------------
  while (nbr_bits_rem >= DEF_OCTET_NBR_BITS) {
    val_08 = p_src[*p_offset_octet];
    //                                                             Merge full-octet val bits into array octet.
    val_32 |= (CPU_INT08U)(val_08 << (nbr_bits - nbr_bits_rem));

    (*p_offset_octet)++;                                        // Update octet offset.

    nbr_bits_rem -= DEF_OCTET_NBR_BITS;                         // Update rem'ing nbr bits.
  }

  //                                                               ----------- UNPACK FINAL TRAILING OCTET ------------
  if (nbr_bits_rem > 0) {
    val_08_mask = DEF_BIT_FIELD_08(nbr_bits_rem, 0u);
    val_08 = p_src[*p_offset_octet];
    val_08 &= val_08_mask;                                      // Keep val trailing bits.

    //                                                             Merge trailing val bits from trailing array octet.
    val_32 |= (CPU_INT08U)(val_08 << (nbr_bits - nbr_bits_rem));

    *p_offset_bit += nbr_bits_rem;                              // Update bit offset.
  }

  return (val_32);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
