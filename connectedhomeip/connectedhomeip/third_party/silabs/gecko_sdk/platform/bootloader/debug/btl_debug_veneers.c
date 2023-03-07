/***************************************************************************//**
 * @file
 * @brief Debug instrumentation veneers.
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

#include "btl_debug.h"

// -----------------------------------------------------------------------------
// NSC functions

#if defined(SL_DEBUG_ASSERT) && (SL_DEBUG_ASSERT == 1)
#if defined(SL_DEBUG_ASSERT_VERBOSE)
__attribute__((cmse_nonsecure_entry))
__attribute__ ((noreturn)) void btl_nsc_assert(const char* file, int line)
{
  btl_nsc_assert(file, line);
}
#else
__attribute__((cmse_nonsecure_entry))
__attribute__ ((noreturn)) void btl_nsc_assert(void)
{
  btl_nsc_assert();
}
#endif
#endif

#if defined(SL_DEBUG_PRINT) && (SL_DEBUG_PRINT == 1)
__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugInit(void)
{
  btl_debugInit();
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteChar(char c)
{
  btl_debugWriteChar(c);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteString(const char * s)
{
  btl_debugWriteString(s);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteLine(const char * s)
{
  btl_debugWriteLine(s);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteCharHex(uint8_t number)
{
  btl_debugWriteCharHex(number);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteShortHex(uint16_t number)
{
  btl_debugWriteShortHex(number);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteWordHex(uint32_t number)
{
  btl_debugWriteWordHex(number);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteInt(int number)
{
  btl_debugWriteInt(number);
}

__attribute__((cmse_nonsecure_entry))
void btl_nsc_debugWriteNewline(void)
{
  btl_debugWriteNewline();
}
#endif