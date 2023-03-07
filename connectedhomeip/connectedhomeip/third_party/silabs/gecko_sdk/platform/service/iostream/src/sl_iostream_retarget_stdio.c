/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting for all supported toolchains.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_iostream.h"
#include "sl_status.h"

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)

#include <sys/stat.h>
#include <stddef.h>
#include "em_device.h"
#include "sl_assert.h"

int _close(int file);
void _exit(int status);
int _fstat(int file, struct stat *st);
int _getpid(void);
int _isatty(int file);
int _kill(int pid, int sig);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _write(int file, const char *ptr, int len);

/**************************************************************************//**
 * Close a file.
 *
 * @param[in] file  File you want to close.
 *
 * @return  Returns 0 when the file is closed.
 *****************************************************************************/
int _close(int file)
{
  (void) file;
  return 0;
}

/**************************************************************************//**
 * Exit the program.
 *
 * @param[in] status The value to return to the parent process as the
 *            exit status (not used).
 *****************************************************************************/
void _exit(int status)
{
  (void) status;
  while (1) {
  }                 // Hang here forever...
}

/**************************************************************************//**
 * Status of an open file.
 *
 * @param[in] file  Check status for this file.
 *
 * @param[in] st    Status information.
 *
 * @return  Returns 0 when st_mode is set to character special.
 *****************************************************************************/
int _fstat(int file, struct stat *st)
{
  (void) file;
  st->st_mode = S_IFCHR;
  return 0;
}

/**************************************************************************//**
 * Get process ID.
 *
 * @return  Return 1 when not implemented.
 *****************************************************************************/
int _getpid(void)
{
  return 1;
}

/**************************************************************************//**
 * Query whether output stream is a terminal.
 *
 * @param[in] file  Descriptor for the file.
 *
 * @return  Returns 1 when query is done.
 *****************************************************************************/
int _isatty(int file)
{
  (void) file;
  return 1;
}

/**************************************************************************//**
 * Send signal to process.
 *
 * @param[in] pid Process id (not used).
 *
 * @param[in] sig Signal to send (not used).
 *****************************************************************************/
int _kill(int pid, int sig)
{
  (void)pid;
  (void)sig;
  return -1;
}

/**************************************************************************//**
 * Set position in a file.
 *
 * @param[in] file  Descriptor for the file.
 *
 * @param[in] ptr   Poiter to the argument offset.
 *
 * @param[in] dir   Directory whence.
 *
 * @return  Returns 0 when position is set.
 *****************************************************************************/
int _lseek(int file, int ptr, int dir)
{
  (void) file;
  (void) ptr;
  (void) dir;
  return 0;
}

/**************************************************************************//**
 * Read from a file.
 *
 * @param[in] file  Descriptor for the file you want to read from.
 *
 * @param[in] ptr   Pointer to the chacaters that are beeing read.
 *
 * @param[in] len   Number of characters to be read.
 *
 * @return  Number of characters that have been read.
 *****************************************************************************/
int _read(int file, char *ptr, int len)
{
  size_t bytes_read = 0;

  (void)file;
  sl_iostream_read(SL_IOSTREAM_STDIN, ptr, (size_t)len, &bytes_read);

  if (bytes_read == 0) {
    return -1;
  }

  return (int)bytes_read;
}

/**************************************************************************//**
 * Write to a file.
 *
 * @param[in] file  Descriptor for the file you want to write to.
 *
 * @param[in] ptr   Pointer to the text you want to write
 *
 * @param[in] len   Number of characters to be written.
 *
 * @return  Number of characters that have been written.
 *****************************************************************************/
int _write(int file, const char *ptr, int len)
{
  sl_status_t status;

  (void)file;
  status = sl_iostream_write(SL_IOSTREAM_STDOUT, ptr, (size_t)len);
  EFM_ASSERT(status == SL_STATUS_OK);

  return len;
}

#endif /* !defined( __CROSSWORKS_ARM ) && defined( __GNUC__ ) */

#if defined(__ICCARM__)
/*******************
 *
 * Copyright 1998-2003 IAR Systems.  All rights reserved.
 *
 * $Revision: 38614 $
 *
 * This is a template implementation of the "__write" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__write" function should output "size" number of bytes from
 * "buffer" in some application-specific way.  It should return the
 * number of characters written, or _LLIO_ERROR on failure.
 *
 * If "buffer" is zero then __write should perform flushing of
 * internal buffers, if any.  In this case "handle" can be -1 to
 * indicate that all handles should be flushed.
 *
 * The template implementation below assumes that the application
 * provides the function "MyLowLevelPutchar".  It should return the
 * character written, or -1 on failure.
 *
 ********************/

#include <yfuns.h>
#include <stdint.h>
#include "sl_common.h"

_STD_BEGIN

/**************************************************************************//**
 * Transmit buffer to IOStream
 *
 * @param buffer  Array of characters to send
 *
 * @param nbytes  Number of bytes to transmit
 *
 * @return Number of bytes sent
 *****************************************************************************/
static int TxBuf(uint8_t *buffer, int nbytes)
{
  sl_status_t status;

  status = sl_iostream_write(SL_IOSTREAM_STDOUT, buffer, nbytes);
  EFM_ASSERT(status == SL_STATUS_OK);

  return nbytes;
}

/**************************************************************************//**
 * Write to.
 *
 * @param[in] handle  Handle
 *
 * @param[in] buffer  Pointer to the buffer you want to write
 *
 * @param[in] size    Number of characters to be written.
 *
 * @return  Number of characters that have been written.
 *
 * @note  If the __write implementation uses internal buffering, uncomment
 *        the following line to ensure that we are called with "buffer" as 0
 *        (i.e. flush) when the application terminates.
 *****************************************************************************/
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  // Remove the #if #endif pair to enable the implementation

  size_t nChars = 0;

  if (buffer == 0) {
    // This means that we should flush internal buffers.  Since we
    // don't we just return.  (Remember, "handle" == -1 means that all
    // handles should be flushed.)
    return 0;
  }

  // This template only writes to "standard out" and "standard err",
  // for all other file handles it returns failure.
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
    return _LLIO_ERROR;
  }

  if (TxBuf((uint8_t *) buffer, size) != size) {
    return _LLIO_ERROR;
  } else {
    nChars = size;
  }

  return nChars;
}

/**************************************************************************//**
 * Read from.
 *
 * @param[in] handle  Handle
 *
 * @param[in] buffer   Pointer to the characters that have been read.
 *
 * @return  Number of characters that have been read.
 *****************************************************************************/
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  size_t bytes_read = 0;

  // This template only reads from "standard in", for all other file
  // handles it returns failure.
  if (handle != _LLIO_STDIN) {
    return _LLIO_ERROR;
  }

  sl_iostream_read(SL_IOSTREAM_STDIN, buffer, size, &bytes_read);
  if (bytes_read == 0) {
    return -1;
  }

  return bytes_read;
}

_STD_END

#endif /* defined( __ICCARM__ ) */

#if defined(__CROSSWORKS_ARM)

/**************************************************************************//**
 * Write a character.
 *
 * @param[in] ch  character
 *
 * @return  1
 *****************************************************************************/
int __putchar(int ch)
{
  sl_status_t status;

  status = sl_iostream_putchar(SL_IOSTREAM_STDOUT, ch);
  EFM_ASSERT(status == SL_STATUS_OK);

  return(1);
}

/**************************************************************************//**
 * Read a character.
 *
 * @return  Character read
 *****************************************************************************/
int __getchar(void)
{
  int rtn_val = 0;
  char c;
  sl_status_t status;

  status = sl_iostream_getchar(SL_IOSTREAM_STDIN, &c);
  if (status == SL_STATUS_OK) {
    rtn_val = 1;
  } else {
    EFM_ASSERT(status == SL_STATUS_EMPTY);
  }

  return(rtn_val);
}

#endif /* defined( __CROSSWORKS_ARM ) */

#if defined(__CC_ARM)
/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low-level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>

// #pragma import(__use_no_semihosting_swi)

struct __FILE{
  int handle;
};

//Standard output stream
FILE __stdout;

/**************************************************************************//**
 * Writes character to file
 *
 * @param[in] f   File
 *
 * @param[in] ch  Character
 *
 * @return  Written character
 *****************************************************************************/
int fputc(int ch, FILE *f)
{
  sl_status_t status;

  status = sl_iostream_putchar(SL_IOSTREAM_STDOUT, ch);
  EFM_ASSERT(status == SL_STATUS_OK);

  return(1);
}

/**************************************************************************//**
 * Reads character from file
 *
 * @param[in] f File
 *
 * @return  Character
 *****************************************************************************/
int fgetc(FILE *f)
{
  int rtn_val = 0;
  char c;
  sl_status_t status;

  status = sl_iostream_getchar(SL_IOSTREAM_STDIN, &c);
  if (status == SL_STATUS_OK) {
    rtn_val = 1;
  } else {
    EFM_ASSERT(status == SL_STATUS_EMPTY);
  }

  return((int)c);
}

/**************************************************************************//**
 * Tests the error indicator for the stream pointed to by file
 *
 * @param[in] f File
 *
 * @return  Returns non-zero if it is set
 *****************************************************************************/
int ferror(FILE *f)
{
  // Your implementation of ferror
  return EOF;
}

/**************************************************************************//**
 * Writes a character to the console
 *
 * @param[in] ch  Input character
 *****************************************************************************/
void _ttywrch(int ch)
{
  sl_status_t status;

  status = sl_iostream_putchar(SL_IOSTREAM_STDOUT, ch);
  EFM_ASSERT(status == SL_STATUS_OK);
}

/**************************************************************************//**
 * Library exit function. This function is called if stack overflow occurs.
 *
 * @param[in] return_code Return code
 *****************************************************************************/
void _sys_exit(int return_code)
{
  label:  goto label; // endless loop
}
#endif /* defined( __CC_ARM ) */
