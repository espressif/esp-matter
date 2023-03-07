/***************************************************************************//**
 * @file
 * @brief The code to implement a simple and parsable response print format.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "response_print.h"

// -----------------------------------------------------------------------------
// Configuration Macros
// -----------------------------------------------------------------------------
// Pull in correct config file:
#ifdef RESPONSE_PRINT_USE_LOCAL_CONFIG_HEADER
  #include "response_print_config.h" // component-level config file (new method)
#else // !defined(RESPONSE_PRINT_USE_LOCAL_CONFIG_HEADER)
  #ifdef CONFIGURATION_HEADER
    #include CONFIGURATION_HEADER // application-level config file (old method)
  #endif

  #ifdef MAX_FORMAT_STRING_SIZE
    #define RESPONSE_PRINT_FORMAT_STR_SIZE_MAX MAX_FORMAT_STRING_SIZE
  #else
    #define RESPONSE_PRINT_FORMAT_STR_SIZE_MAX 256U
  #endif
#endif // defined(RESPONSE_PRINT_USE_LOCAL_CONFIG_HEADER)

#define TAG_VALUE_OVERHEAD 3  // '{', '}', and '\0'

#define RESPONSE_PRINT_RETURN_IF_DISABLED \
  do {                                    \
    if (!responsePrintEnabled) {          \
      return true;                        \
    }                                     \
  } while (false)

// -----------------------------------------------------------------------------
// Structures and Types
// -----------------------------------------------------------------------------
/**
 * @enum StripMode_t
 * This enum allows you to specify a mode to determine whether the tag or value
 * format string are stripped when processing.
 */
typedef enum StripMode {
  STRIP_NONE,  /**< Do not strip either tag or value format strings */
  STRIP_TAG,   /**< Strip the 'tag:' protion of the string */
  STRIP_VALUE, /**< Strip the ':valueFormat' portion of the string */
} StripMode_t;

// -----------------------------------------------------------------------------
// Static Function Prototypes
// -----------------------------------------------------------------------------
static int32_t appendToFormatString(char *dest,
                                    char *src,
                                    uint32_t size,
                                    uint32_t capacity,
                                    StripMode_t stripMode);

static int responsePrintInternal(StripMode_t stripMode,
                                 char *formatString,
                                 va_list args,
                                 bool finalize);

// -----------------------------------------------------------------------------
// Static Variables
// -----------------------------------------------------------------------------
static volatile bool responsePrintEnabled = true;

// -----------------------------------------------------------------------------
// Response Print Private Functions
// -----------------------------------------------------------------------------

/**
 * Append the given tag:valueFormat pair to the format string while validating
 * it and adding any necessary framing characters. It can optionally strip off
 * either the tag or value from the string
 * @param dest String to add the output to
 * @param src The string to take the tag:valueFormat from
 * @param size The number of characters to read from src
 * @param capacity The space remaining in dest
 * @param stripMode Determines whether to strip the tag or valueFormat
 * components from the string.
 * @return Returns the number of characters written on success and something
 * less than 0 on failure.
 */
static int32_t appendToFormatString(char *dest,
                                    char *src,
                                    uint32_t size,
                                    uint32_t capacity,
                                    StripMode_t stripMode)
{
  char *delim;
  int32_t offset = 0;

  // Make sure there's enough room for this formatter and any overhead
  if (capacity < (size + TAG_VALUE_OVERHEAD)) {
    return -11;
  }

  // If the first byte of the format string is \n, insert that before '{'
  while ((size > 0) && (*src == '\n')) {
    dest[offset++] = *src++;
    size--;
  }

  // Always insert the leading '{'
  dest[offset++] = '{';

  // Copy the data from the format string into our temporary space
  memcpy(dest + offset, src, size);
  offset += size;

  // Insert the trailing '}' and null terminator
  dest[offset++] = '}';
  dest[offset]   = '\0'; // Don't count the null terminator in the size

  // Make sure that the format string is of the form tag:valueFormat
  delim = strstr(dest, ":");
  if (delim == NULL && stripMode == STRIP_NONE) {
    // We don't have a delimiter but one is expected
    return -12;
  } else if (delim == NULL) {
    // There is only one tag/value piece and we only need one so return success
    return offset;
  }

  // Make sure there isn't a second ':' character
  if (strstr(delim + 1, ":") != NULL) {
    return -13;
  }

  // Remove either the tag or value if we're supposed to
  if (stripMode == STRIP_TAG) {
    offset = offset - (delim - dest);
    // Copy the value string over the tag (including the terminating '\0')
    memmove(dest + 1, delim + 1, offset);
  } else if (stripMode == STRIP_VALUE) {
    *delim = '}'; // Just terminate the destination string early
    *(delim + 1) = '\0';
    offset = delim - dest + 1;
  }

  return offset;
}

/**
 * Take in a response print format string and convert it to the appropriate
 * printf-style string. Then pass this string to vprintf() along with any
 * supplied arguments for display.
 * @param stripMode Specifies whether the tag or value format strings should be
 * stripped from the formatString if present.
 * @param formatString This is the format string to process.
 * @param args The va_list object representing the arguments passed on the stack
 * for this print command.
 * @param finalize Whether or not to print the closing '}'
 * @return
 */

static char buffer[RESPONSE_PRINT_FORMAT_STR_SIZE_MAX];

static int responsePrintInternal(StripMode_t stripMode,
                                 char *formatString,
                                 va_list args,
                                 bool finalize)
{
  char *end = formatString, *start = formatString;
  uint32_t offset = 0;
  int rval = 0;

  // Take the input string and convert it into valid response format
  while (end != NULL) {
    uint32_t size;

    // Get the next token in the input buffer to find the tag name
    end = strpbrk(start, ",");

    // If we didn't find an end token then just process the rest of the string
    if (end != NULL) {
      size = end - start;
    } else {
      size = strlen(start);
    }

    // Convert and validate the given format string
    rval = appendToFormatString(buffer + offset,
                                start,
                                size,
                                (RESPONSE_PRINT_FORMAT_STR_SIZE_MAX - offset),
                                stripMode);
    if (rval < 0) {
      break;
    }
    // Move past all the currently written characters
    offset += rval;

    // Advance the start pointer
    start = end + 1;
  }
  // Print out the parsed format buffer
  vprintf(buffer, args);

  // Print out the error code if there is one
  if (rval < 0) {
    printf(" {internal_error:%d}", -rval);
  } else {
    rval = 0;
  }
  if (finalize) {
    printf("}\n");
  }

  return rval;
}

// -----------------------------------------------------------------------------
// Response Print Public Functions
// -----------------------------------------------------------------------------

void responsePrintEnable(bool enable)
{
  responsePrintEnabled = enable;
}

bool responsePrintHeader(char *command, char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);
  printf("#");  // Header strings start with a '#'
  if (!responsePrintStart(command)) {
    va_end(ap);
    return false;
  }
  if (responsePrintInternal(STRIP_VALUE, formatString, ap, true) != 0) {
    va_end(ap);
    return false;
  }
  va_end(ap);

  return true;
}

bool responsePrintMulti(char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);
  if (!responsePrintStart(NULL)) {
    va_end(ap);
    return false;
  }
  if (responsePrintInternal(STRIP_TAG, formatString, ap, true) != 0) {
    va_end(ap);
    return false;
  }
  va_end(ap);

  return true;
}

bool responsePrint(char *command, char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);
  if (!responsePrintStart(command)) {
    va_end(ap);
    return false;
  }
  if (responsePrintInternal(STRIP_NONE, formatString, ap, true) != 0) {
    va_end(ap);
    return false;
  }
  va_end(ap);

  return true;
}

bool responsePrintStart(char *command)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  // Print the start of command standard formatting
  printf("{");
  if (command != NULL) {
    printf("{(%s)}", command);
  }
  return true;
}

bool responsePrintContinue(char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);
  if (responsePrintInternal(STRIP_NONE, formatString, ap, false) != 0) {
    va_end(ap);
    return false;
  }
  va_end(ap);

  return true;
}

bool responsePrintEnd(char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);
  if (responsePrintInternal(STRIP_NONE, formatString, ap, true) != 0) {
    va_end(ap);
    return false;
  }
  va_end(ap);

  return true;
}

bool responsePrintError(char *command, uint8_t code, char *formatString, ...)
{
  RESPONSE_PRINT_RETURN_IF_DISABLED;
  va_list ap;

  va_start(ap, formatString);

  // Print the command name
  printf("{");
  if (command != NULL) {
    printf("{(%s)}", command);
  }

  // Print the formatted error string.
  // @todo: Add validation of the formatString
  printf("{error:");
  vprintf(formatString, ap);
  printf("}");

  // Print the error code if it was specified
  printf("{errorCode:%d}", code);

  // Terminate the response
  printf("}\n");

  va_end(ap);

  return true;
}

int sprintfFloat(char *buffer, int8_t len, float f, uint8_t precision)
{
  int8_t isNegative = (f < 0) ? 1 : 0;

  // Buffer needs to be big enough to hold sign (if negative), 1 integral digit,
  // precision fractional digits, decimal point (if precision > 0), and \0.
  if (buffer == NULL || len < (isNegative + 1 + precision + (precision > 0) + 1)) {
    return 0;
  }

  int8_t writeIndex = len - 1;
  buffer[writeIndex] = '\0';

  for (uint8_t exp = 0; exp < precision; exp++) {
    f *= 10;
  }

  int a;
  if (isNegative != 0) {
    a = -(int)(f - 0.5); // Round toward negative infinity
  } else {
    a = (int)(f + 0.5); // Round toward positive infinity
  }
  if (a < 0) { // Sign changed, float too large!
    return 0;
  }

  buffer[writeIndex--] = '\0'; // terminate string

  int8_t digit;
  do {
    digit = a % 10;
    a = a / 10;
    buffer[writeIndex--] = '0' + digit;
    if (precision && len == writeIndex + 2 + precision) {
      buffer[writeIndex--] = '.';
    }
  } while ((a != 0 || (precision && writeIndex >= (len - precision - 3)))
           && writeIndex >= isNegative);
  if (a != 0) {
    return 0; // Number too large to represent in buffer!
  }
  if (isNegative != 0) {
    buffer[writeIndex--] = '-';
  }

  // shift up
  if (writeIndex != -1 ) {
    memmove(buffer, &buffer[writeIndex + 1], len - writeIndex - 1);
  }
  return len - writeIndex - 1;
}
