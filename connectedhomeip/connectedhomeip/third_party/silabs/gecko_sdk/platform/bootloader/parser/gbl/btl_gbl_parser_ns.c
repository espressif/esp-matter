/***************************************************************************//**
 * @file
 * @brief Non-secure GBL image parser library.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
// Includes

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include "parser/gbl/btl_gbl_parser_ns.h"

// -----------------------------------------------------------------------------
// NSC functions

extern int32_t parser_nsc_init(uint8_t flags);
extern int32_t parser_nsc_parse(uint8_t buffer[],
                                size_t length,
                                void *prop);

// -----------------------------------------------------------------------------
// NS functions

int32_t parser_init(uint8_t flags)
{
  return parser_nsc_init(flags);
}

int32_t parser_parse(uint8_t buffer[],
                     size_t length,
                     ImageProperties_t *prop)
{
  return parser_nsc_parse(buffer, length, prop);
}