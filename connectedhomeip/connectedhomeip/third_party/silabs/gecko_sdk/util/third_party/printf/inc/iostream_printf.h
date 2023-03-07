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

#ifndef IOSTREAM_PRINTF_H
#define IOSTREAM_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup service
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup IOSTREAM_PRINTF
 * @{
 ******************************************************************************/

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Print a formated string on stream.
 *
 * @param[in] stream  IO Stream to be used:
 *                      SL_IOSTREAM_STDOUT;           Default output stream will be used.
 *                      SL_IOSTREAM_STDERR;           Default error output stream will be used.
 *                      Pointer to specific stream;   Specific stream will be used.
 *
 *
 * @param[in] format  String that contains the text to be written.
 *
 * @param[in] ...     Additional arguments.
 *
 * @return  The number of characters that are written into the array, not counting
 *          the terminating null character.
 ******************************************************************************/

int sl_iostream_printf_to_stream(sl_iostream_t *stream,
                                 const char *format,
                                 ...);

/** @} (end addtogroup IOSTREAM_PRINTF) */
/** @} (end addtogroup service) */

#ifdef __cplusplus
}
#endif

#endif /* IOSTREAM_PRINTF_H */
