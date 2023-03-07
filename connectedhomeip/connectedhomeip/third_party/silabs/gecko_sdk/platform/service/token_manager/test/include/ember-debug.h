/***************************************************************************//**
 * @brief See @ref debug for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __EMBER_DEBUG_H__
#define __EMBER_DEBUG_H__

// We currently don't have debug support in Connect.

// Define the values for DEBUG_LEVEL
#define NO_DEBUG    0
#define BASIC_DEBUG 1
#define FULL_DEBUG  2

#define emberDebugAssert(filename, linenumber) do {} while (FALSE)
#define emberDebugMemoryDump(start, end) do {} while (FALSE)
#define emberDebugBinaryPrintf(formatstring, ...) do {} while (FALSE)
#define emDebugSendVuartMessage(buff, len) do {} while (FALSE)
#define emberDebugError(code) do {} while (FALSE)
#define emberDebugReportOff() (FALSE)
#define emberDebugReportRestore(state) do {} while (FALSE)
#define emberDebugPrintf(...) do {} while (FALSE)

#define emLog(type, ...)
#define emLogLine(type, ...)
#define emLogBytes(type, format, bytes, count, ...)

#endif // __EMBER_DEBUG_H__
