/**
* @file
* DebugPrint.
*
* Usage:
* Call DebugPrint or DebugPrintF depending on your needs.
* Define DEBUGPRINT to actually print, if not defined the debug
* printing code and variables only used for debug printing should
* be optimized away by the compiler.
*
* Define NO_DEBUGPRINT to turn off all debug printing everywhere,
* even where DEBUGPRINT has been defined.
* NO_DEBUGPRINT is usually defined globally (by e.g. release build
* scripts) while DEBUGPRINT can be defined on a module/file basis.
*
* NOTE that variables only used for debug should be marked with
* as so with he UNUSED macro.
*              
* Note that implementation of dprintf is a simplified version that
* does not support floats and doubles etc.
*
* @copyright 2018 Silicon Laboratories Inc.
*/


#ifndef _DEBUGPRINT_H_
#define _DEBUGPRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

void DebugPrintf(const char* pFormat, ...);
void DebugPrint(const char* pString);

#ifdef __cplusplus
}
#endif

/*
 * Use TOSTRING() when printing #defines as strings.
 * (STRINGIFY is simply a helper in this two step procedure)
 */
#define STRINGIFY(s)            #s
#define TOSTRING(s)             STRINGIFY(s)

/**
 * Suitable for writing runtime unit-tests and additional outputs after calculations that otherwise are redundant
 * in release.
 */
#if !defined(NO_DEBUGPRINT) && defined(DEBUGPRINT)
#define DEBUG_CODE(code)        do {code} while(0)
#else
#define DEBUG_CODE(code)        do {} while(0)
#endif

/**
 * Debug-level log.
 */
#if !defined(NO_DEBUGPRINT) && defined(DEBUGPRINT)
#define DPRINTF(PFORMAT, ...)   ( DebugPrintf( (PFORMAT), ## __VA_ARGS__ ) )
#define DPRINT(PSTRING)         ( DebugPrint( (PSTRING) ) )
#else
// Do While 0 is to require the ; at the end, compiler will optimize it away.
// Its intentional that there is no code using the variables passed to DPRINT / DPRINTF
// and thus avoiding issues when they become unused when debug is not defined.
// Variables only used for debugging should be marked with the UNUSED(MyDebugVar) macro
// to avoid warning and to clearly show that the variable is only for debug.
// Issues has also been observed with GCC, which will give warnings iwhen variables only used
// in code that is optimized away is struct members etc. (e.g. MyStruct->myMember)
#define DPRINTF(PFORMAT, ...)   do {} while(0)
#define DPRINT(PSTRING)         do {} while(0)
#endif

#endif	// _DEBUGPRINT_H_

