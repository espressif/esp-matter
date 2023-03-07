/***************************************************************************//**
 * @file
 * @brief Routines to help print out command responses in a standard format.
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
/***************************************************************************//**
 * There are two response otuput formats: single and multi line. Both of these
 * follow the following formatting rules.
 * - Start and end with curly braces { }
 * - List the command name, enclosed in parentheses ( )
 * - Contain any number of tag/value pairs enclosed in curly braces {}
 * - May contain other text and whitespace outside of command names and
 *   tag/value pairs that is ignored by any parser
 * - Carriage returns and line feeds are treated as whitespace by any parser
 *
 * - Single Response: Used when there is only a single response to a command.
 *     - There is a single start/end curly brace wrapper
 *     - tag/value pairs are wrapped in a single set of curly braces, separated
 *       by a colon {tag:value}.
 * - Multi Response: Used when a command may have multiple responses. For example,
 *   when reading a block of memory, or receiving multiple packets.
 *     - Response starts with a header, delimited by a hash # at the start of
 *       the header line.
 *     - Header includes the command name, followed by any tags individually
 *       wrapped with curly braces { }
 *     - Following the header, any number of responses can be provided
 *     - Data lines do not contain the command name or tags, only the values
 *       that correspond to the tags in the order described in the header
 *     - Multi-response output is terminated by the re-appearance of the command
 *       prompt >
 *
 * Single Response Example:
 * @code
 * {{(getchannel)} {channel:18}}
 *
 * Multi Response Example:
 * @code
 * #{{(rx)} {length} {payload} {lqi} {rssi} {gain} {framestatus}}
 * {{12} {000102030405060708090A0B0C0D0E0F1011} {02} {9E} {00} {4000}}
 * {{12} {000102030405060708090A0B0C0D0E0F1011} {02} {9E} {00} {4000}}
 * {{12} {000102030405060708090A0B0C0D0E0F1011} {02} {9E} {00} {4000}}
 * {{12} {000102030405060708090A0B0C0D0E0F1011} {02} {9E} {00} {4000}}
 * {{12} {000102030405060708090A0B0C0D0E0F1011} {02} {9E} {00} {4000}}
 *
 ******************************************************************************/

#ifndef RESPONSE_PRINT_H__
#define RESPONSE_PRINT_H__

// Get the standard include types
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
// Response Print Functions
// -----------------------------------------------------------------------------

/**
 * Function that can enable or disable all future responsePrint... API calls.
 * Generally useful in testing modes where the application cannot afford
 * the time penalty associated with printing. By default, printing is enabled.
 * @param enable Enable (true) or disable (false) responsePrint... prints.
 */
void responsePrintEnable(bool enable);

/**
 * Print the header for a multi response print. The format string may contain
 * either both the tag and valueFormat component or just the tag component since
 * any valueFormat component will be ignored.
 * @param command The name of the command that's being executed
 * @param formatString The format string to be used for this response print. It
 * should be tag:valueFormat pairs separated by ',' characters. The tag should
 * just be the name string for this value and valueFormat should be a valid
 * printf formatter describing how to print the argument for this entry.
 * @param ... The values to be printed based on the given formats.
 * @return Returns true on success and false on failure.
 */
bool responsePrintHeader(char *command, char *formatString, ...);

/**
 * Print one response as part of a multi response output. The format string may
 * contain either both tag and valueFormat components or just the valueFormat
 * component since the tags are stripped from multi response outputs.
 * @param formatString The format string to be used for this response print. It
 * should be tag:valueFormat pairs separated by ',' characters. The tag should
 * just be the name string for this value and valueFormat should be a valid
 * printf formatter describing how to print the argument for this entry.
 * @param ... The values to be printed based on the given formats.
 * @return Returns true on success and false on failure.
 */
bool responsePrintMulti(char *formatString, ...);

/**
 * Print a single response. The format string must include both tag and
 * valueFormat sections.
 * @param command The name of the command that's being executed.
 * @param formatString The format string to be used for this response print. It
 * should be tag:valueFormat pairs separated by ',' characters. The tag should
 * just be the name string for this value and valueFormat should be a valid
 * printf formatter describing how to print the argument for this entry.
 * @param ... The values to be printed based on the given formats.
 * @return Returns true on success and false on failure.
 */
bool responsePrint(char *command, char *formatString, ...);

/**
 * Begin to print a single response. A call to responsePrintStart followed by a
 * call to responsePrintEnd is equivalent to a call to responsePrint with the
 * same arguments. Separating the calls allows inserting calls to
 * responsePrintContinue to extend the response.
 * @param command The name of the command that's being executed.
 * @return Returns true on success and false on failure.
 */
bool responsePrintStart(char *command);

/**
 * Continue to print a single response. A call to responsePrintStart followed by
 * a call to responsePrintEnd is equivalent to a call to responsePrint with the
 * same arguments. Separating the calls allows inserting calls to
 * responsePrintContinue to extend the response.
 * @param formatString The format string to be used for this response print. It
 * should be tag:valueFormat pairs separated by ',' characters. The tag should
 * just be the name string for this value and valueFormat should be a valid
 * printf formatter describing how to print the argument for this entry.
 * @param ... The values to be printed based on the given formats.
 * @return Returns true on success and false on failure.
 */
bool responsePrintContinue(char *formatString, ...);

/**
 * Finish printing a single response. A call to responsePrintStart followed by
 * a call to responsePrintEnd is equivalent to a call to responsePrint with the
 * same arguments. Separating the calls allows inserting calls to
 * responsePrintContinue to extend the response.
 * @param formatString The format string to be used for this response print. It
 * should be tag:valueFormat pairs separated by ',' characters. The tag should
 * just be the name string for this value and valueFormat should be a valid
 * printf formatter describing how to print the argument for this entry.
 * @param ... The values to be printed based on the given formats.
 * @return Returns true on success and false on failure.
 */
bool responsePrintEnd(char *formatString, ...);

/**
 * Print an error message for this command. An error will use the standard
 * response format but will only have a single tag:value pair. The tag will
 * be 'error' and the value is the string that the user wanted to return.
 * @param command The name of the command that's being executed.
 * @param code A numeric code to identify this error.
 * @param formatString The format string to use for the error message.
 * @param ... The values to be printed based on the given format.
 * @return Returns true on success and false on failure.
 */
bool responsePrintError(char *command, uint8_t code, char *formatString, ...);

/*
 * Write float formatted output to sized buffer.
 *
 * The output buffer is zero'ed out. The floating point number is written with
 * specified precision and a null terminator at the end.
 *
 * @param  buffer    The output buffer
 * @param  len       The length of the output buffer
 * @param  f         Number to be printed
 * @param  precision The precision of printed float
 * @return           Number of characters printed.
 */
int sprintfFloat(char *buffer, int8_t len, float f, uint8_t precision);

#endif // RESPONSE_PRINT_H__
