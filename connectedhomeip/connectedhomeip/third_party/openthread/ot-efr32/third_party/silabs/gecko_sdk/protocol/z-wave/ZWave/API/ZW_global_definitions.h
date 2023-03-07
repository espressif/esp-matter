/**
 * @file ZW_global_definitions.h
 *
 * This file is a helper file for including all globally required parameters
 * and definitions for ease of coding.
 * It is defined as a ZWave API module so that it also can be used by the
 * ZAF API if needed.
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef __ZW_GLOBAL_DEFINITIONS_H__
#define __ZW_GLOBAL_DEFINITIONS_H__

/****************************************************************************
 * INCLUDE FILES
 ***************************************************************************/

#include <stdbool.h>
#include <ZW_typedefs.h>

/****************************************************************************
 * CONFIGURATIONS
 ***************************************************************************/

/****************************************************************************
 * DEFINITIONS, TYPEDEF and CONSTANTS
 ***************************************************************************/

#define DO_WHILE(code)        do{code}while(0)

/****************************************************************************
 * MACROS
 ***************************************************************************/

/**
 * This will calculate the distance between two index values in a circular buffer. (FILO)
 *
 * @param tailIndex     The index at which items are taken off the circular buffer.
 *                      (Old items are removed from buffer at this index)
 * @param headIndex     The index at which items are placed on the circular buffer.
 *                      (New items are added to buffer at this index)
 * @param maxValue      The maximum index value, or the size of the buffer minus 1.
 */
#define CIRCULAR_BUFFER_DIFF(tailIndex, headIndex, maxValue) \
  ((headIndex >= tailIndex) ? (headIndex - tailIndex) : ((maxValue - tailIndex + 1) + headIndex))

/****************************************************************************
 * EXTERNAL VARIABLES
 ***************************************************************************/

/****************************************************************************
 * ENUMERATIONS
 ***************************************************************************/

/**
 * Universal return code used by all functions where ever applicable.
 *
 * ATTENTION: Use the enum for catching errors at runtime, since the order
 * of these definitions can change in future versions!!
 */
typedef enum {
  /* Meaning: SUCCESS */
  Code_Success = 0,

  /* Meaning: Default failed code with no helping indication. */
  Code_Fail_Unknown,

  /* Meaning: Failed due to lack of memory. */
  Code_Fail_NoMemory,

  /* Meaning: A set limit is reached for this operation. */
  Code_Fail_LimitReached,

  /* Meaning: A specific entity could not be found. */
  Code_Fail_NotFound,

  /* Meaning: The operation or the entire module is globally
   * suspended and cannot be utilized at this point in time. */
  Code_Fail_Suspended,

  /* Meaning: Not in an appropriate state for this call. */
  Code_Fail_InvalidState,

  /* Meaning: This operation can not be allowed at this point. */
  Code_Fail_InvalidOperation,

  /* Meaning: Wrong/invalid input parameters. */
  Code_Fail_InvalidParameter,

  /* Meaning: A required resource or dependency that this operation relies
   * on was not setup or available. Perhaps wrong initialization order. */
  Code_DependencyUnavailable,

  /* Meaning: A resource needed by this operation or call is busy.
   * Try again later. Potential racing condition or untimely action. */
  Code_Fail_Busy,

  /* Meaning: Failed due to timeout. */
  Code_Fail_Timeout,

  /* Meaning: Error occurred at the driver level. */
  Code_Fail_Driver,

  /* Meaning: This operation is not supported due to limited
   * implementation or need. */
  Code_Fail_NotImplemented,

  /* Meaning: This operation cannot be performed, since it is
   * blocked or not allowed. */
  Code_Fail_NotAllowed,
} ReturnCode_t;

/****************************************************************************
 * STATIC CONTROLLING FUNCTIONS OF DUT
 ***************************************************************************/

/****************************************************************************
 * API FUNCTIONS
 ***************************************************************************/


#endif // __ZW_GLOBAL_DEFINITIONS_H__
