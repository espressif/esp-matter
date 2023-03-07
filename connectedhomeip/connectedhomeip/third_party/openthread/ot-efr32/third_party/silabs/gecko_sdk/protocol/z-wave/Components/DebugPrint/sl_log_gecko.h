/***************************************************************
 * This file is used to alter the behavior of sl_log.h module.
 *
 * sl_log.h is a configurable logger module for embedded Linux
 * implementations. It prints for each line, the module tag,
 * line number, function name, log level and tag level, as
 * viewed in the code.
 *
 * @copyright 2022 Silicon Laboratories Inc.
 **************************************************************/

#ifndef __SL_LOG_GECKO_H__
#define __SL_LOG_GECKO_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                              INCLUDE FILES
 ****************************************************************************/

/*****************************************************************************
 *                             CONFIGURATIONS
 ****************************************************************************/

/**
 * Set to 1 to disable all unify DEBUGPRINT centrally from this place.
 * Set to 0 to allow each unify module to control DebugPrint.h.
 */
#define DEBUGPRINT_UNIFY_FORCE_DISABLE      1

/*****************************************************************************
 *                                 MACROS
 ****************************************************************************/

#if (DEBUGPRINT_UNIFY_FORCE_DISABLE == 1)
 #ifdef DEBUGPRINT
  #undef DEBUGPRINT
 #endif  // DEBUGPRINT
#else  // (DEBUGPRINT_UNIFY_FORCE_DISABLE == 0)
 #ifndef DEBUGPRINT
  #define DEBUGPRINT                   // Comment out to only enable debugprint in file where DEBUGPRINT is defined.
 #endif  // Unify logger force global enable.
#endif  // (DEBUGPRINT_UNIFY_FORCE_DISABLE == X)

#include "DebugPrint.h"

// Logging macros for calling sl_log with levels
#define sl_log_debug(tag, fmtstr, ...)      DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")
#define sl_log_info(tag, fmtstr, ...)       DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")
#define sl_log_warning(tag, fmtstr, ...)    DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")
#define sl_log_error(tag, fmtstr, ...)      DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")
#define sl_log_critical(tag, fmtstr, ...)   DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")
#define sl_log(tag, level, fmtstr, ...)     DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(fmtstr, ##__VA_ARGS__); DPRINT("\n")

// PRINTF is used in some UIC files.
#ifdef PRINTF
 #undef PRINTF
 #define PRINTF(...)                         DPRINT(tag); DPRINT(" (UNIFY) "); DPRINTF(__VA_ARGS__); DPRINT("\n")
#endif

/*****************************************************************************
 *                             API FUNCTIONS
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  // __SL_LOG_GECKO_H__
