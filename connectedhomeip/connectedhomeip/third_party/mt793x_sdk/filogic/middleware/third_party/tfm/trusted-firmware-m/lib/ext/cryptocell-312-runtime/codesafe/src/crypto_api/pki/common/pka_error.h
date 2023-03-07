/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef PKA_ERROR_H
#define PKA_ERROR_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* base address -  0x00F10B00 */

#define PKA_HW_VERSION_NOT_CORRECT_ERROR   (PKA_MODULE_ERROR_BASE + 0x0UL)
#define PKA_HW_PIC_DECRYPED_FAILURE_ERROR  (PKA_MODULE_ERROR_BASE + 0x1UL)
#define PKA_KG_UNSUPPORTED_KEY_SIZE        (PKA_MODULE_ERROR_BASE + 0x2UL)
#define PKA_KEY_ILLEGAL_SIZE_ERROR     (PKA_MODULE_ERROR_BASE + 0x3UL)

#define PKA_CONVERT_PRIV_KEY_TO_CRT_NOT_SUPPORTED       (PKA_MODULE_ERROR_BASE + 0x10UL)
#define PKA_CONVERT_PRIV_KEY_TO_CRT_FACTORS_NOT_FOUND   (PKA_MODULE_ERROR_BASE + 0x11UL)
#define PKA_CRT_WITH_512_BITS_KEY_ERROR         (PKA_MODULE_ERROR_BASE + 0x12UL)

/* Error definitions for PKA using */
#define PKA_ILLEGAL_PTR_ERROR                       (PKA_MODULE_ERROR_BASE + 0x20UL)
#define PKA_ENTRIES_COUNT_ERROR                     (PKA_MODULE_ERROR_BASE + 0x21UL)
#define PKA_REGISTER_SIZES_ERROR                    (PKA_MODULE_ERROR_BASE + 0x22UL)
#define PKA_SET_MAP_MODE_ERROR                      (PKA_MODULE_ERROR_BASE + 0x23UL)
#define PKA_ILLEGAL_OPERAND_LEN_ERROR               (PKA_MODULE_ERROR_BASE + 0x24UL)
#define PKA_ILLEGAL_OPERAND_ERROR                   (PKA_MODULE_ERROR_BASE + 0x25UL)
#define PKA_ILLEGAL_OPCODE_ERROR                    (PKA_MODULE_ERROR_BASE + 0x26UL)
#define PKA_NOT_ENOUGH_MEMORY_ERROR                 (PKA_MODULE_ERROR_BASE + 0x27UL)
#define PKA_ILLEGAL_OPERAND_TYPE_ERROR              (PKA_MODULE_ERROR_BASE + 0x28UL)
#define PKA_INVERSION_NOT_EXISTS_ERROR              (PKA_MODULE_ERROR_BASE + 0x29UL)
#define PKA_NOT_ENOUGH_TEMP_REGS_ERROR              (PKA_MODULE_ERROR_BASE + 0x2AUL)
#define PKA_OPERATION_NOT_SUPPORTED_ERROR           (PKA_MODULE_ERROR_BASE + 0x2BUL)
#define PKA_MOD_EVEN_USE_OTHER_FUNC_ERROR           (PKA_MODULE_ERROR_BASE + 0x2CUL)
#define PKA_DIVIDER_IS_NULL_ERROR                   (PKA_MODULE_ERROR_BASE + 0x2EUL)
#define PKA_MODULUS_IS_NULL_ERROR                   (PKA_MODULE_ERROR_BASE + 0x2FUL)
#define PKA_DATA_SIZE_ERROR                         (PKA_MODULE_ERROR_BASE + 0x30UL)
#define PKA_OPERATION_SIZE_ERROR                    (PKA_MODULE_ERROR_BASE + 0x31UL)

#define PKA_ILLEGAL_PKA_ADDRESS_ERROR               (PKA_MODULE_ERROR_BASE + 0x32UL)
#define PKA_ILLEGAL_PKA_VIRT_PTR_ERROR              (PKA_MODULE_ERROR_BASE + 0x33UL)
#define PKA_NAF_KEY_SIZE_ERROR                      (PKA_MODULE_ERROR_BASE + 0x34UL)
#define PKA_NAF_BUFFER_SIZE_ERROR                   (PKA_MODULE_ERROR_BASE + 0x35UL)
#define PKA_NAF_KEY_ERROR                           (PKA_MODULE_ERROR_BASE + 0x36UL)
#define PKA_NAF_INTERNAL_ERROR                      (PKA_MODULE_ERROR_BASE + 0x37UL)
#define PKA_REGS_COUNT_ERROR                        (PKA_MODULE_ERROR_BASE + 0x38UL)
#define PKA_INTERNAL_ERROR                          (PKA_MODULE_ERROR_BASE + 0x39UL)
#define PKA_FATAL_ERR_STATE_ERROR                   (PKA_MODULE_ERROR_BASE + 0x3AUL)

/* modular functions errors */
#define PKA_MOD_SQUARE_ROOT_NOT_EXIST_ERROR         (PKA_MODULE_ERROR_BASE + 0x40UL)



#ifdef __cplusplus
}
#endif

#endif

