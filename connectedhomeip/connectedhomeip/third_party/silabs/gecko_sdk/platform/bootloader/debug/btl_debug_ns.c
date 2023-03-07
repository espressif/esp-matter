/***************************************************************************//**
 * @file
 * @brief Non-secure debug instrumentation.
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

// -----------------------------------------------------------------------------
// NSC functions

#if defined(SL_DEBUG_ASSERT) && (SL_DEBUG_ASSERT == 1)
#if defined(SL_DEBUG_ASSERT_VERBOSE)
extern __attribute__ ((noreturn)) void btl_nsc_assert(const char* file, int line);
#else
extern __attribute__ ((noreturn)) void btl_nsc_assert(void);
#endif
#endif

#if defined(SL_DEBUG_PRINT) && (SL_DEBUG_PRINT == 1)
extern void btl_nsc_debugInit(void);
extern void btl_nsc_debugWriteChar(char c);
extern void btl_debugWriteString(const char * s);
extern void btl_debugWriteLine(const char * s);
extern void btl_debugWriteCharHex(uint8_t number);
extern void btl_debugWriteShortHex(uint16_t number);
extern void btl_debugWriteWordHex(uint32_t number);
extern void btl_debugWriteInt(int number);
extern void btl_debugWriteNewline(void);
#endif

// -----------------------------------------------------------------------------
// NS functions

#if defined(SL_DEBUG_ASSERT) && (SL_DEBUG_ASSERT == 1)
#if defined(SL_DEBUG_ASSERT_VERBOSE)
void btl_assert(const char* file, int line)
{
  btl_nsc_assert(file, line);
}
#else
__attribute__ ((noreturn)) void btl_assert(void)
{
  btl_nsc_assert();
}
#endif
#endif

#if defined(SL_DEBUG_PRINT) && (SL_DEBUG_PRINT == 1)
void btl_debugInit(void)
{
  btl_nsc_debugInit();
}

void btl_debugWriteChar(char c)
{
  btl_nsc_debugWriteChar(c);
}

void btl_debugWriteString(const char *s)
{
  btl_nsc_debugWriteString(s);
}

void btl_debugWriteLine(const char *s)
{
  btl_nsc_debugWriteLine(s);
}

void btl_debugWriteCharHex(uint8_t number)
{
  btl_nsc_debugWriteCharHex(number);
}

void btl_debugWriteShortHex(uint16_t number)
{
  btl_nsc_debugWriteShortHex(number);
}

void btl_debugWriteWordHex(uint32_t number)
{
  btl_nsc_debugWriteWordHex(number);
}

void btl_debugWriteInt(int number)
{
  btl_nsc_debugWriteInt(number);
}

void btl_debugWriteNewline(void)
{
  btl_nsc_debugWriteNewline();
}
#endif
