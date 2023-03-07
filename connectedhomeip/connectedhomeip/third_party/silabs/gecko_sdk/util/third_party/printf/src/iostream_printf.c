/***************************************************************************//**
 * @file
 * @brief IO Stream printf implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_iostream.h"
#include "printf.h"
#include "stdarg.h"

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

static void stream_putchar(char character, void* arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * printf to stream implementation
 ******************************************************************************/
int sl_iostream_printf_to_stream(sl_iostream_t *stream,
                                 const char *format,
                                 ...)
{
  va_list va;
  int ret;
  va_start(va, format);
  ret = fctprintf(stream_putchar, stream, format, va);
  va_end(va);
  return ret;
}

/*******************************************************************************
 *************************   PRIVATE CUSTOM FUNCTIONS   ************************
 ******************************************************************************/

/***************************************************************************//**
 * Function used by the printf()function to Output a character to a custom
 * implementation.
 ******************************************************************************/
void _putchar(char character)
{
  sl_iostream_putchar(SL_IOSTREAM_STDOUT, character);
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
/***************************************************************************//**
 * putchar implementation for sl_iostream_printf_to_stream; called by fnctprintf()
 ******************************************************************************/
static void stream_putchar(char character,
                           void *arg)
{
  sl_iostream_t *stream = (sl_iostream_t *)arg;
  sl_iostream_putchar(stream, character);
}
