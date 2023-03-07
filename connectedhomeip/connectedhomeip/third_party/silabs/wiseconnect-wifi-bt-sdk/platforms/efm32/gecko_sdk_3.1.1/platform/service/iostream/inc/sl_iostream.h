/***************************************************************************//**
 * @file
 * @brief IO Stream.
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

#ifndef SL_IOSTREAM_H
#define SL_IOSTREAM_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_status.h"
#include "sl_enum.h"
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream IO Stream
 * @brief IO Stream
 * @details
 * ## Overview
 *
 *   IO Stream is a platform module software that provides Input/Output functionalities
 *   by creating streams. Streams are abstractions allowing a uniform way to read/write
 *   data regardless of the physical communication interface.
 *
 *   IO Stream offers many interfaces, see submodules for a list of all types available
 *   and their specificities.You can load multiple streams in the project and you can
 *   select the interface that must be used at runtime.Some interface type can also
 *   be instantiated, meaning that you can have multiple instances of an interface
 *   type which will be normally bound to a hardware peripheral.
 *
 * ## Initialization
 *
 *   The IO Stream core doesn't require any initialization. Instead each stream type has
 *   their own initialization and their own configuration. See IO Stream specific type
 *   to know more about how to initialize a stream.
 *
 *   Note that most stream will set itself as the default stream during their initialization.
 *   Thus the initial default stream will be the last stream initialized.
 *
 * ## Default system-wide stream
 *
 *   Multiple streams can be initialized in your application and you can configure a default
 *   stream that must be used when no stream is specified. Also note that the default stream
 *   will be used when calling printf and you can change the default stream at runtime.
 *   The following defines should be used for the default stream:
 *
 *      SL_IOSTREAM_STDIN
 *      SL_IOSTREAM_STDOUT
 *      SL_IOSTREAM_STDERR
 *
 * ## RTOS - Task's default stream
 *
 *   In the case of an RTOS environment, each task can set its own stream. By default, the task
 *   stream will be set to the system_wide default stream. From your task, you can change the
 *   default stream assigned to your task without affecting the other tasks' stream.
 *
 * ## Printf
 *
 *   IO Stream provides third-party printf integrations. It can work with toolchain implementation
 *   or with the tiny printf implementation for embedded system. The printf API doesn't have an
 *   argument for specifying the stream to be used, so IO Stream provides a printf API that takes
 *   a stream as an argument and calls the configured third-party implementation of printf.
 *
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define SL_IOSTREAM_STDIN 0  ///< Default input stream
#define SL_IOSTREAM_STDOUT 0 ///< Default output stream
#define SL_IOSTREAM_STDERR 0 ///< Default error stream
/// @endcond

// -----------------------------------------------------------------------------
// Data Types

/// @brief Struct representing iostream operations.
typedef struct {
  void *context;                                                                                ///< context
  sl_status_t (*write)(void *context, const void *buffer, size_t buffer_length);                ///< write
  sl_status_t (*read)(void *context, void *buffer, size_t buffer_length, size_t *bytes_read);   ///< read
} sl_iostream_t;

/// @brief Enumeration representing the possible types of iostream instances.
SL_ENUM(sl_iostream_type_t){
  SL_IOSTREAM_TYPE_SWO = 0,              ///< SWO Instance
  SL_IOSTREAM_TYPE_RTT = 1,              ///< RTT Instance
  SL_IOSTREAM_TYPE_UART = 2,             ///< USART Instance
  SL_IOSTREAM_TYPE_VUART = 3,            ///< Vuart
  SL_IOSTREAM_TYPE_DEBUG_OUTPUT = 4,     ///< Backchannel output Instance Type
  SL_IOSTREAM_TYPE_LOOPBACK = 5,         ///< Loopback Instance
  SL_IOSTREAM_TYPE_UNDEFINED = 6,        ///< Undefined Instance Type
};

/// @brief Struct representing an IO Stream instance.
typedef struct {
  sl_iostream_t *handle;        ///< iostream instance handle.
  char *name;                   ///< iostream instance name.
  sl_iostream_type_t type;      ///< iostream instance type.
  uint8_t periph_id;            ///< iostream peripheral id.
  sl_status_t (*init)(void);    ///< iostream instance init function.
} sl_iostream_instance_info_t;

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
// Special stream to be used when you want to avoid printing anything
extern sl_iostream_t sl_iostream_null;
/// @endcond

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Set the stream as default IO Stream.
 *
 * @param[in] stream   IO Stream to set as default.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_set_default(sl_iostream_t *stream);

/***************************************************************************//**
 * Get the default IO Stream configured.
 *
 * @return  Status result
 ******************************************************************************/
sl_iostream_t *sl_iostream_get_default(void);

/***************************************************************************//**
 * Configure the systemwide default stream.
 *
 * @param[in] stream  IO Stream to be used.
 *
 * @return  Status result
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
sl_status_t sl_iostream_set_system_default(sl_iostream_t *stream);
#else
#define sl_iostream_set_system_default sl_iostream_set_default
#endif

/***************************************************************************//**
 * Output data on a stream.
 *
 * @param[in] stream          IO Stream to be used.
 *                              SL_IOSTREAM_STDOUT;           Default output stream will be used.
 *                              Pointer to specific stream;   Specific stream will be used.
 *
 * @param[in] buffer          Buffer that contains the data to output.
 *
 * @param[in] buffer_length   Data length contained in the buffer.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_write(sl_iostream_t *stream,
                              const void *buffer,
                              size_t buffer_length);

/***************************************************************************//**
 * Get data from a stream.
 *
 * @param[in]  stream          IO Stream to be used.
 *                               SL_IOSTREAM_STDOUT;           Default output stream will be used.
 *                               Pointer to specific stream;   Specific stream will be used.
 *
 * @param[out] buffer          Buffer that contains the data to output.
 *
 * @param[in]  buffer_length   Data length contained in the buffer.
 *
 * @param[out] bytes_read      Data length copied to the buffer.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_read(sl_iostream_t *stream,
                             void *buffer,
                             size_t buffer_length,
                             size_t *bytes_read);

/***************************************************************************//**
 * Print a character on stream.
 *
 * @param[in] stream  IO Stream to be used:
 *                      SL_IOSTREAM_STDOUT;           Default output stream will be used.
 *                      SL_IOSTREAM_STDERR;           Default error output stream will be used.
 *                      Pointer to specific stream;   Specific stream will be used.
 *
 * @param[in] c       Character to print
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_putchar(sl_iostream_t *stream,
                                char c);

/***************************************************************************//**
 * Print a character on stream.
 *
 * @param[in] stream  IO Stream to be used.
 *                      SL_IOSTREAM_STDIN;            Default input stream will be used.
 *                      Pointer to specific stream;   Specific stream will be used.
 *
 * @param[out] c      Pointer to variable that will receive the character.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_getchar(sl_iostream_t *stream,
                                char *c);

/***************************************************************************//**
 * Print a formated string on stream.
 *
 * @param[in] stream  IO Stream to be used:
 *                      SL_IOSTREAM_STDOUT;           Default output stream will be used.
 *                      SL_IOSTREAM_STDERR;           Default error output stream will be used.
 *                      Pointer to specific stream;   Specific stream will be used.
 *
 * @param[in] format  String that contains the text to be written.
 *
 * @param[in] ...     Additional arguments.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_printf(sl_iostream_t *stream,
                               const char *format,
                               ...);

/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_H
