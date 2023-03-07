/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

/*
 * ======== GENERATED SECTIONS ========
 *
 *     PROLOGUE
 *     INCLUDES
 *
 *     INTERNAL DEFINITIONS
 *     MODULE-WIDE CONFIGS
 *     VIRTUAL FUNCTIONS
 *     FUNCTION DECLARATIONS
 *     CONVERTORS
 *     SYSTEM FUNCTIONS
 *
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_uia_events_UIAErr__include
#define ti_uia_events_UIAErr__include

#ifndef __nested__
#define __nested__
#define ti_uia_events_UIAErr__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_events_UIAErr___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/events/package/package.defs.h>

#include <xdc/runtime/Diags.h>
#include <ti/uia/events/IUIAEvent.h>
#include <xdc/runtime/Log.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_events_UIAErr_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__diagsEnabled ti_uia_events_UIAErr_Module__diagsEnabled__C;
#ifdef ti_uia_events_UIAErr_Module__diagsEnabled__CR
#define ti_uia_events_UIAErr_Module__diagsEnabled__C (*((CT__ti_uia_events_UIAErr_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__diagsEnabled (ti_uia_events_UIAErr_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_events_UIAErr_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__diagsIncluded ti_uia_events_UIAErr_Module__diagsIncluded__C;
#ifdef ti_uia_events_UIAErr_Module__diagsIncluded__CR
#define ti_uia_events_UIAErr_Module__diagsIncluded__C (*((CT__ti_uia_events_UIAErr_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__diagsIncluded (ti_uia_events_UIAErr_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_events_UIAErr_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__diagsMask ti_uia_events_UIAErr_Module__diagsMask__C;
#ifdef ti_uia_events_UIAErr_Module__diagsMask__CR
#define ti_uia_events_UIAErr_Module__diagsMask__C (*((CT__ti_uia_events_UIAErr_Module__diagsMask*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__diagsMask__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__diagsMask (ti_uia_events_UIAErr_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_events_UIAErr_Module__gateObj;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__gateObj ti_uia_events_UIAErr_Module__gateObj__C;
#ifdef ti_uia_events_UIAErr_Module__gateObj__CR
#define ti_uia_events_UIAErr_Module__gateObj__C (*((CT__ti_uia_events_UIAErr_Module__gateObj*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__gateObj__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__gateObj (ti_uia_events_UIAErr_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_events_UIAErr_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__gatePrms ti_uia_events_UIAErr_Module__gatePrms__C;
#ifdef ti_uia_events_UIAErr_Module__gatePrms__CR
#define ti_uia_events_UIAErr_Module__gatePrms__C (*((CT__ti_uia_events_UIAErr_Module__gatePrms*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__gatePrms__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__gatePrms (ti_uia_events_UIAErr_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_events_UIAErr_Module__id;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__id ti_uia_events_UIAErr_Module__id__C;
#ifdef ti_uia_events_UIAErr_Module__id__CR
#define ti_uia_events_UIAErr_Module__id__C (*((CT__ti_uia_events_UIAErr_Module__id*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__id__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__id (ti_uia_events_UIAErr_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_events_UIAErr_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerDefined ti_uia_events_UIAErr_Module__loggerDefined__C;
#ifdef ti_uia_events_UIAErr_Module__loggerDefined__CR
#define ti_uia_events_UIAErr_Module__loggerDefined__C (*((CT__ti_uia_events_UIAErr_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerDefined__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerDefined (ti_uia_events_UIAErr_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_events_UIAErr_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerObj ti_uia_events_UIAErr_Module__loggerObj__C;
#ifdef ti_uia_events_UIAErr_Module__loggerObj__CR
#define ti_uia_events_UIAErr_Module__loggerObj__C (*((CT__ti_uia_events_UIAErr_Module__loggerObj*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerObj__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerObj (ti_uia_events_UIAErr_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_events_UIAErr_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerFxn0 ti_uia_events_UIAErr_Module__loggerFxn0__C;
#ifdef ti_uia_events_UIAErr_Module__loggerFxn0__CR
#define ti_uia_events_UIAErr_Module__loggerFxn0__C (*((CT__ti_uia_events_UIAErr_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerFxn0 (ti_uia_events_UIAErr_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_events_UIAErr_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerFxn1 ti_uia_events_UIAErr_Module__loggerFxn1__C;
#ifdef ti_uia_events_UIAErr_Module__loggerFxn1__CR
#define ti_uia_events_UIAErr_Module__loggerFxn1__C (*((CT__ti_uia_events_UIAErr_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerFxn1 (ti_uia_events_UIAErr_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_events_UIAErr_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerFxn2 ti_uia_events_UIAErr_Module__loggerFxn2__C;
#ifdef ti_uia_events_UIAErr_Module__loggerFxn2__CR
#define ti_uia_events_UIAErr_Module__loggerFxn2__C (*((CT__ti_uia_events_UIAErr_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerFxn2 (ti_uia_events_UIAErr_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_events_UIAErr_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerFxn4 ti_uia_events_UIAErr_Module__loggerFxn4__C;
#ifdef ti_uia_events_UIAErr_Module__loggerFxn4__CR
#define ti_uia_events_UIAErr_Module__loggerFxn4__C (*((CT__ti_uia_events_UIAErr_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerFxn4 (ti_uia_events_UIAErr_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_events_UIAErr_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Module__loggerFxn8 ti_uia_events_UIAErr_Module__loggerFxn8__C;
#ifdef ti_uia_events_UIAErr_Module__loggerFxn8__CR
#define ti_uia_events_UIAErr_Module__loggerFxn8__C (*((CT__ti_uia_events_UIAErr_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_events_UIAErr_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_events_UIAErr_Module__loggerFxn8 (ti_uia_events_UIAErr_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_events_UIAErr_Object__count;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Object__count ti_uia_events_UIAErr_Object__count__C;
#ifdef ti_uia_events_UIAErr_Object__count__CR
#define ti_uia_events_UIAErr_Object__count__C (*((CT__ti_uia_events_UIAErr_Object__count*)(xdcRomConstPtr + ti_uia_events_UIAErr_Object__count__C_offset)))
#else
#define ti_uia_events_UIAErr_Object__count (ti_uia_events_UIAErr_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_events_UIAErr_Object__heap;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Object__heap ti_uia_events_UIAErr_Object__heap__C;
#ifdef ti_uia_events_UIAErr_Object__heap__CR
#define ti_uia_events_UIAErr_Object__heap__C (*((CT__ti_uia_events_UIAErr_Object__heap*)(xdcRomConstPtr + ti_uia_events_UIAErr_Object__heap__C_offset)))
#else
#define ti_uia_events_UIAErr_Object__heap (ti_uia_events_UIAErr_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_events_UIAErr_Object__sizeof;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Object__sizeof ti_uia_events_UIAErr_Object__sizeof__C;
#ifdef ti_uia_events_UIAErr_Object__sizeof__CR
#define ti_uia_events_UIAErr_Object__sizeof__C (*((CT__ti_uia_events_UIAErr_Object__sizeof*)(xdcRomConstPtr + ti_uia_events_UIAErr_Object__sizeof__C_offset)))
#else
#define ti_uia_events_UIAErr_Object__sizeof (ti_uia_events_UIAErr_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_events_UIAErr_Object__table;
__extern __FAR__ const CT__ti_uia_events_UIAErr_Object__table ti_uia_events_UIAErr_Object__table__C;
#ifdef ti_uia_events_UIAErr_Object__table__CR
#define ti_uia_events_UIAErr_Object__table__C (*((CT__ti_uia_events_UIAErr_Object__table*)(xdcRomConstPtr + ti_uia_events_UIAErr_Object__table__C_offset)))
#else
#define ti_uia_events_UIAErr_Object__table (ti_uia_events_UIAErr_Object__table__C)
#endif

/* error */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_error;
__extern __FAR__ const CT__ti_uia_events_UIAErr_error ti_uia_events_UIAErr_error__C;
#ifdef ti_uia_events_UIAErr_error__CR
#define ti_uia_events_UIAErr_error (*((CT__ti_uia_events_UIAErr_error*)(xdcRomConstPtr + ti_uia_events_UIAErr_error__C_offset)))
#else
#define ti_uia_events_UIAErr_error (ti_uia_events_UIAErr_error__C)
#endif

/* errorWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_errorWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAErr_errorWithStr ti_uia_events_UIAErr_errorWithStr__C;
#ifdef ti_uia_events_UIAErr_errorWithStr__CR
#define ti_uia_events_UIAErr_errorWithStr (*((CT__ti_uia_events_UIAErr_errorWithStr*)(xdcRomConstPtr + ti_uia_events_UIAErr_errorWithStr__C_offset)))
#else
#define ti_uia_events_UIAErr_errorWithStr (ti_uia_events_UIAErr_errorWithStr__C)
#endif

/* hwError */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_hwError;
__extern __FAR__ const CT__ti_uia_events_UIAErr_hwError ti_uia_events_UIAErr_hwError__C;
#ifdef ti_uia_events_UIAErr_hwError__CR
#define ti_uia_events_UIAErr_hwError (*((CT__ti_uia_events_UIAErr_hwError*)(xdcRomConstPtr + ti_uia_events_UIAErr_hwError__C_offset)))
#else
#define ti_uia_events_UIAErr_hwError (ti_uia_events_UIAErr_hwError__C)
#endif

/* hwErrorWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_hwErrorWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAErr_hwErrorWithStr ti_uia_events_UIAErr_hwErrorWithStr__C;
#ifdef ti_uia_events_UIAErr_hwErrorWithStr__CR
#define ti_uia_events_UIAErr_hwErrorWithStr (*((CT__ti_uia_events_UIAErr_hwErrorWithStr*)(xdcRomConstPtr + ti_uia_events_UIAErr_hwErrorWithStr__C_offset)))
#else
#define ti_uia_events_UIAErr_hwErrorWithStr (ti_uia_events_UIAErr_hwErrorWithStr__C)
#endif

/* fatal */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_fatal;
__extern __FAR__ const CT__ti_uia_events_UIAErr_fatal ti_uia_events_UIAErr_fatal__C;
#ifdef ti_uia_events_UIAErr_fatal__CR
#define ti_uia_events_UIAErr_fatal (*((CT__ti_uia_events_UIAErr_fatal*)(xdcRomConstPtr + ti_uia_events_UIAErr_fatal__C_offset)))
#else
#define ti_uia_events_UIAErr_fatal (ti_uia_events_UIAErr_fatal__C)
#endif

/* fatalWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_fatalWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAErr_fatalWithStr ti_uia_events_UIAErr_fatalWithStr__C;
#ifdef ti_uia_events_UIAErr_fatalWithStr__CR
#define ti_uia_events_UIAErr_fatalWithStr (*((CT__ti_uia_events_UIAErr_fatalWithStr*)(xdcRomConstPtr + ti_uia_events_UIAErr_fatalWithStr__C_offset)))
#else
#define ti_uia_events_UIAErr_fatalWithStr (ti_uia_events_UIAErr_fatalWithStr__C)
#endif

/* critical */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_critical;
__extern __FAR__ const CT__ti_uia_events_UIAErr_critical ti_uia_events_UIAErr_critical__C;
#ifdef ti_uia_events_UIAErr_critical__CR
#define ti_uia_events_UIAErr_critical (*((CT__ti_uia_events_UIAErr_critical*)(xdcRomConstPtr + ti_uia_events_UIAErr_critical__C_offset)))
#else
#define ti_uia_events_UIAErr_critical (ti_uia_events_UIAErr_critical__C)
#endif

/* criticalWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_criticalWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAErr_criticalWithStr ti_uia_events_UIAErr_criticalWithStr__C;
#ifdef ti_uia_events_UIAErr_criticalWithStr__CR
#define ti_uia_events_UIAErr_criticalWithStr (*((CT__ti_uia_events_UIAErr_criticalWithStr*)(xdcRomConstPtr + ti_uia_events_UIAErr_criticalWithStr__C_offset)))
#else
#define ti_uia_events_UIAErr_criticalWithStr (ti_uia_events_UIAErr_criticalWithStr__C)
#endif

/* exception */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_exception;
__extern __FAR__ const CT__ti_uia_events_UIAErr_exception ti_uia_events_UIAErr_exception__C;
#ifdef ti_uia_events_UIAErr_exception__CR
#define ti_uia_events_UIAErr_exception (*((CT__ti_uia_events_UIAErr_exception*)(xdcRomConstPtr + ti_uia_events_UIAErr_exception__C_offset)))
#else
#define ti_uia_events_UIAErr_exception (ti_uia_events_UIAErr_exception__C)
#endif

/* uncaughtException */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_uncaughtException;
__extern __FAR__ const CT__ti_uia_events_UIAErr_uncaughtException ti_uia_events_UIAErr_uncaughtException__C;
#ifdef ti_uia_events_UIAErr_uncaughtException__CR
#define ti_uia_events_UIAErr_uncaughtException (*((CT__ti_uia_events_UIAErr_uncaughtException*)(xdcRomConstPtr + ti_uia_events_UIAErr_uncaughtException__C_offset)))
#else
#define ti_uia_events_UIAErr_uncaughtException (ti_uia_events_UIAErr_uncaughtException__C)
#endif

/* nullPointerException */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_nullPointerException;
__extern __FAR__ const CT__ti_uia_events_UIAErr_nullPointerException ti_uia_events_UIAErr_nullPointerException__C;
#ifdef ti_uia_events_UIAErr_nullPointerException__CR
#define ti_uia_events_UIAErr_nullPointerException (*((CT__ti_uia_events_UIAErr_nullPointerException*)(xdcRomConstPtr + ti_uia_events_UIAErr_nullPointerException__C_offset)))
#else
#define ti_uia_events_UIAErr_nullPointerException (ti_uia_events_UIAErr_nullPointerException__C)
#endif

/* unexpectedInterrupt */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_unexpectedInterrupt;
__extern __FAR__ const CT__ti_uia_events_UIAErr_unexpectedInterrupt ti_uia_events_UIAErr_unexpectedInterrupt__C;
#ifdef ti_uia_events_UIAErr_unexpectedInterrupt__CR
#define ti_uia_events_UIAErr_unexpectedInterrupt (*((CT__ti_uia_events_UIAErr_unexpectedInterrupt*)(xdcRomConstPtr + ti_uia_events_UIAErr_unexpectedInterrupt__C_offset)))
#else
#define ti_uia_events_UIAErr_unexpectedInterrupt (ti_uia_events_UIAErr_unexpectedInterrupt__C)
#endif

/* memoryAccessFault */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_memoryAccessFault;
__extern __FAR__ const CT__ti_uia_events_UIAErr_memoryAccessFault ti_uia_events_UIAErr_memoryAccessFault__C;
#ifdef ti_uia_events_UIAErr_memoryAccessFault__CR
#define ti_uia_events_UIAErr_memoryAccessFault (*((CT__ti_uia_events_UIAErr_memoryAccessFault*)(xdcRomConstPtr + ti_uia_events_UIAErr_memoryAccessFault__C_offset)))
#else
#define ti_uia_events_UIAErr_memoryAccessFault (ti_uia_events_UIAErr_memoryAccessFault__C)
#endif

/* securityException */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_securityException;
__extern __FAR__ const CT__ti_uia_events_UIAErr_securityException ti_uia_events_UIAErr_securityException__C;
#ifdef ti_uia_events_UIAErr_securityException__CR
#define ti_uia_events_UIAErr_securityException (*((CT__ti_uia_events_UIAErr_securityException*)(xdcRomConstPtr + ti_uia_events_UIAErr_securityException__C_offset)))
#else
#define ti_uia_events_UIAErr_securityException (ti_uia_events_UIAErr_securityException__C)
#endif

/* divisionByZero */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_divisionByZero;
__extern __FAR__ const CT__ti_uia_events_UIAErr_divisionByZero ti_uia_events_UIAErr_divisionByZero__C;
#ifdef ti_uia_events_UIAErr_divisionByZero__CR
#define ti_uia_events_UIAErr_divisionByZero (*((CT__ti_uia_events_UIAErr_divisionByZero*)(xdcRomConstPtr + ti_uia_events_UIAErr_divisionByZero__C_offset)))
#else
#define ti_uia_events_UIAErr_divisionByZero (ti_uia_events_UIAErr_divisionByZero__C)
#endif

/* overflowException */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_overflowException;
__extern __FAR__ const CT__ti_uia_events_UIAErr_overflowException ti_uia_events_UIAErr_overflowException__C;
#ifdef ti_uia_events_UIAErr_overflowException__CR
#define ti_uia_events_UIAErr_overflowException (*((CT__ti_uia_events_UIAErr_overflowException*)(xdcRomConstPtr + ti_uia_events_UIAErr_overflowException__C_offset)))
#else
#define ti_uia_events_UIAErr_overflowException (ti_uia_events_UIAErr_overflowException__C)
#endif

/* indexOutOfRange */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_indexOutOfRange;
__extern __FAR__ const CT__ti_uia_events_UIAErr_indexOutOfRange ti_uia_events_UIAErr_indexOutOfRange__C;
#ifdef ti_uia_events_UIAErr_indexOutOfRange__CR
#define ti_uia_events_UIAErr_indexOutOfRange (*((CT__ti_uia_events_UIAErr_indexOutOfRange*)(xdcRomConstPtr + ti_uia_events_UIAErr_indexOutOfRange__C_offset)))
#else
#define ti_uia_events_UIAErr_indexOutOfRange (ti_uia_events_UIAErr_indexOutOfRange__C)
#endif

/* notImplemented */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_notImplemented;
__extern __FAR__ const CT__ti_uia_events_UIAErr_notImplemented ti_uia_events_UIAErr_notImplemented__C;
#ifdef ti_uia_events_UIAErr_notImplemented__CR
#define ti_uia_events_UIAErr_notImplemented (*((CT__ti_uia_events_UIAErr_notImplemented*)(xdcRomConstPtr + ti_uia_events_UIAErr_notImplemented__C_offset)))
#else
#define ti_uia_events_UIAErr_notImplemented (ti_uia_events_UIAErr_notImplemented__C)
#endif

/* stackOverflow */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_stackOverflow;
__extern __FAR__ const CT__ti_uia_events_UIAErr_stackOverflow ti_uia_events_UIAErr_stackOverflow__C;
#ifdef ti_uia_events_UIAErr_stackOverflow__CR
#define ti_uia_events_UIAErr_stackOverflow (*((CT__ti_uia_events_UIAErr_stackOverflow*)(xdcRomConstPtr + ti_uia_events_UIAErr_stackOverflow__C_offset)))
#else
#define ti_uia_events_UIAErr_stackOverflow (ti_uia_events_UIAErr_stackOverflow__C)
#endif

/* illegalInstruction */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_illegalInstruction;
__extern __FAR__ const CT__ti_uia_events_UIAErr_illegalInstruction ti_uia_events_UIAErr_illegalInstruction__C;
#ifdef ti_uia_events_UIAErr_illegalInstruction__CR
#define ti_uia_events_UIAErr_illegalInstruction (*((CT__ti_uia_events_UIAErr_illegalInstruction*)(xdcRomConstPtr + ti_uia_events_UIAErr_illegalInstruction__C_offset)))
#else
#define ti_uia_events_UIAErr_illegalInstruction (ti_uia_events_UIAErr_illegalInstruction__C)
#endif

/* entryPointNotFound */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_entryPointNotFound;
__extern __FAR__ const CT__ti_uia_events_UIAErr_entryPointNotFound ti_uia_events_UIAErr_entryPointNotFound__C;
#ifdef ti_uia_events_UIAErr_entryPointNotFound__CR
#define ti_uia_events_UIAErr_entryPointNotFound (*((CT__ti_uia_events_UIAErr_entryPointNotFound*)(xdcRomConstPtr + ti_uia_events_UIAErr_entryPointNotFound__C_offset)))
#else
#define ti_uia_events_UIAErr_entryPointNotFound (ti_uia_events_UIAErr_entryPointNotFound__C)
#endif

/* moduleNotFound */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_moduleNotFound;
__extern __FAR__ const CT__ti_uia_events_UIAErr_moduleNotFound ti_uia_events_UIAErr_moduleNotFound__C;
#ifdef ti_uia_events_UIAErr_moduleNotFound__CR
#define ti_uia_events_UIAErr_moduleNotFound (*((CT__ti_uia_events_UIAErr_moduleNotFound*)(xdcRomConstPtr + ti_uia_events_UIAErr_moduleNotFound__C_offset)))
#else
#define ti_uia_events_UIAErr_moduleNotFound (ti_uia_events_UIAErr_moduleNotFound__C)
#endif

/* floatingPointError */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_floatingPointError;
__extern __FAR__ const CT__ti_uia_events_UIAErr_floatingPointError ti_uia_events_UIAErr_floatingPointError__C;
#ifdef ti_uia_events_UIAErr_floatingPointError__CR
#define ti_uia_events_UIAErr_floatingPointError (*((CT__ti_uia_events_UIAErr_floatingPointError*)(xdcRomConstPtr + ti_uia_events_UIAErr_floatingPointError__C_offset)))
#else
#define ti_uia_events_UIAErr_floatingPointError (ti_uia_events_UIAErr_floatingPointError__C)
#endif

/* invalidParameter */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAErr_invalidParameter;
__extern __FAR__ const CT__ti_uia_events_UIAErr_invalidParameter ti_uia_events_UIAErr_invalidParameter__C;
#ifdef ti_uia_events_UIAErr_invalidParameter__CR
#define ti_uia_events_UIAErr_invalidParameter (*((CT__ti_uia_events_UIAErr_invalidParameter*)(xdcRomConstPtr + ti_uia_events_UIAErr_invalidParameter__C_offset)))
#else
#define ti_uia_events_UIAErr_invalidParameter (ti_uia_events_UIAErr_invalidParameter__C)
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_events_UIAErr_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_events_UIAErr_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_events_UIAErr_Fxns__ ti_uia_events_UIAErr_Module__FXNS__C;
#else
#define ti_uia_events_UIAErr_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_events_UIAErr_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_events_UIAErr_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_events_UIAErr_Module__startupDone__S, "ti_uia_events_UIAErr_Module__startupDone__S")
__extern xdc_Bool ti_uia_events_UIAErr_Module__startupDone__S( void);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAErr_Module_upCast(void);
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAErr_Module_upCast(void)
{
    return (ti_uia_events_IUIAEvent_Module)&ti_uia_events_UIAErr_Module__FXNS__C;
}

/* Module_to_ti_uia_events_IUIAEvent */
#define ti_uia_events_UIAErr_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAErr_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_events_UIAErr_Module_startupDone() ti_uia_events_UIAErr_Module__startupDone__S()

/* Object_heap */
#define ti_uia_events_UIAErr_Object_heap() ti_uia_events_UIAErr_Object__heap__C

/* Module_heap */
#define ti_uia_events_UIAErr_Module_heap() ti_uia_events_UIAErr_Object__heap__C

/* Module_id */
static inline CT__ti_uia_events_UIAErr_Module__id ti_uia_events_UIAErr_Module_id(void);
static inline CT__ti_uia_events_UIAErr_Module__id ti_uia_events_UIAErr_Module_id( void ) 
{
    return ti_uia_events_UIAErr_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_events_UIAErr_Module_hasMask(void);
static inline xdc_Bool ti_uia_events_UIAErr_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_events_UIAErr_Module__diagsMask__C != (CT__ti_uia_events_UIAErr_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_events_UIAErr_Module_getMask(void);
static inline xdc_Bits16 ti_uia_events_UIAErr_Module_getMask(void)
{
    return (ti_uia_events_UIAErr_Module__diagsMask__C != (CT__ti_uia_events_UIAErr_Module__diagsMask)NULL) ? *ti_uia_events_UIAErr_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_events_UIAErr_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_events_UIAErr_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_events_UIAErr_Module__diagsMask__C != (CT__ti_uia_events_UIAErr_Module__diagsMask)NULL) {
        *ti_uia_events_UIAErr_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_events_UIAErr__top__
#undef __nested__
#endif

#endif /* ti_uia_events_UIAErr__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_events_UIAErr__internalaccess))

#ifndef ti_uia_events_UIAErr__include_state
#define ti_uia_events_UIAErr__include_state


#endif /* ti_uia_events_UIAErr__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_events_UIAErr__nolocalnames)

#ifndef ti_uia_events_UIAErr__localnames__done
#define ti_uia_events_UIAErr__localnames__done

/* module prefix */
#define UIAErr_error ti_uia_events_UIAErr_error
#define UIAErr_errorWithStr ti_uia_events_UIAErr_errorWithStr
#define UIAErr_hwError ti_uia_events_UIAErr_hwError
#define UIAErr_hwErrorWithStr ti_uia_events_UIAErr_hwErrorWithStr
#define UIAErr_fatal ti_uia_events_UIAErr_fatal
#define UIAErr_fatalWithStr ti_uia_events_UIAErr_fatalWithStr
#define UIAErr_critical ti_uia_events_UIAErr_critical
#define UIAErr_criticalWithStr ti_uia_events_UIAErr_criticalWithStr
#define UIAErr_exception ti_uia_events_UIAErr_exception
#define UIAErr_uncaughtException ti_uia_events_UIAErr_uncaughtException
#define UIAErr_nullPointerException ti_uia_events_UIAErr_nullPointerException
#define UIAErr_unexpectedInterrupt ti_uia_events_UIAErr_unexpectedInterrupt
#define UIAErr_memoryAccessFault ti_uia_events_UIAErr_memoryAccessFault
#define UIAErr_securityException ti_uia_events_UIAErr_securityException
#define UIAErr_divisionByZero ti_uia_events_UIAErr_divisionByZero
#define UIAErr_overflowException ti_uia_events_UIAErr_overflowException
#define UIAErr_indexOutOfRange ti_uia_events_UIAErr_indexOutOfRange
#define UIAErr_notImplemented ti_uia_events_UIAErr_notImplemented
#define UIAErr_stackOverflow ti_uia_events_UIAErr_stackOverflow
#define UIAErr_illegalInstruction ti_uia_events_UIAErr_illegalInstruction
#define UIAErr_entryPointNotFound ti_uia_events_UIAErr_entryPointNotFound
#define UIAErr_moduleNotFound ti_uia_events_UIAErr_moduleNotFound
#define UIAErr_floatingPointError ti_uia_events_UIAErr_floatingPointError
#define UIAErr_invalidParameter ti_uia_events_UIAErr_invalidParameter
#define UIAErr_Module_name ti_uia_events_UIAErr_Module_name
#define UIAErr_Module_id ti_uia_events_UIAErr_Module_id
#define UIAErr_Module_startup ti_uia_events_UIAErr_Module_startup
#define UIAErr_Module_startupDone ti_uia_events_UIAErr_Module_startupDone
#define UIAErr_Module_hasMask ti_uia_events_UIAErr_Module_hasMask
#define UIAErr_Module_getMask ti_uia_events_UIAErr_Module_getMask
#define UIAErr_Module_setMask ti_uia_events_UIAErr_Module_setMask
#define UIAErr_Object_heap ti_uia_events_UIAErr_Object_heap
#define UIAErr_Module_heap ti_uia_events_UIAErr_Module_heap
#define UIAErr_Module_upCast ti_uia_events_UIAErr_Module_upCast
#define UIAErr_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAErr_Module_to_ti_uia_events_IUIAEvent

#endif /* ti_uia_events_UIAErr__localnames__done */
#endif
