/***************************************************************************//**
 * @file
 * @brief Common - Bitmaps
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/collections/bitmap_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Bitmap_BitIsSet()
 *
 * @brief    Determines if specified bit of bitmap is set.
 *
 * @param    p_bitmap    Pointer to bitmap.
 *
 * @param    offset_bit  Offset of bit in bitmap to test.
 *
 * @return   DEF_YES, if bit is set;
 *           DEF_NO , otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN Bitmap_BitIsSet(CPU_INT08U *p_bitmap,
                            CPU_SIZE_T offset_bit)
{
  CPU_SIZE_T  offset_octet;
  CPU_DATA    offset_bit_in_octet;
  CPU_INT08U  bit_mask;
  CPU_BOOLEAN bit_set;

  offset_octet = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
  offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

  bit_mask = DEF_BIT(offset_bit_in_octet);
  bit_set = DEF_BIT_IS_SET(p_bitmap[offset_octet], bit_mask);

  return (bit_set);
}

/****************************************************************************************************//**
 *                                               Bitmap_BitSet()
 *
 * @brief    Set specified bit in bitmap.
 *
 * @param    p_bitmap    Pointer to bitmap.
 *
 * @param    offset_bit  Offset of bit in bitmap to test.
 *******************************************************************************************************/
void Bitmap_BitSet(CPU_INT08U *p_bitmap,
                   CPU_SIZE_T offset_bit)
{
  CPU_SIZE_T offset_octet;
  CPU_DATA   offset_bit_in_octet;
  CPU_INT08U bit_mask;

  offset_octet = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
  offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

  bit_mask = DEF_BIT(offset_bit_in_octet);
  DEF_BIT_SET(p_bitmap[offset_octet], bit_mask);
}

/****************************************************************************************************//**
 *                                               Bitmap_BitClr()
 *
 * @brief    Clear specified bit in bitmap.
 *
 * @param    p_bitmap    Pointer to bitmap.
 *
 * @param    offset_bit  Offset of bit in bitmap to test.
 *******************************************************************************************************/
void Bitmap_BitClr(CPU_INT08U *p_bitmap,
                   CPU_SIZE_T offset_bit)
{
  CPU_SIZE_T offset_octet;
  CPU_DATA   offset_bit_in_octet;
  CPU_INT08U bit_mask;

  offset_octet = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
  offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

  bit_mask = DEF_BIT(offset_bit_in_octet);
  DEF_BIT_CLR_08(p_bitmap[offset_octet], bit_mask);
}
