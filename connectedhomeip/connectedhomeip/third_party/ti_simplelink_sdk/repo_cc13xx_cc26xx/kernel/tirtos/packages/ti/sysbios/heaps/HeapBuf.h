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
 *     PER-INSTANCE TYPES
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

#ifndef ti_sysbios_heaps_HeapBuf__include
#define ti_sysbios_heaps_HeapBuf__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_heaps_HeapBuf__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_heaps_HeapBuf___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/heaps/package/package.defs.h>

#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Queue.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* ExtendedStats */
struct ti_sysbios_heaps_HeapBuf_ExtendedStats {
    xdc_UInt maxAllocatedBlocks;
    xdc_UInt numAllocatedBlocks;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Instance_State */
typedef xdc_Char __T1_ti_sysbios_heaps_HeapBuf_Instance_State__buf;
typedef xdc_Char *ARRAY1_ti_sysbios_heaps_HeapBuf_Instance_State__buf;
typedef const xdc_Char *CARRAY1_ti_sysbios_heaps_HeapBuf_Instance_State__buf;
typedef ARRAY1_ti_sysbios_heaps_HeapBuf_Instance_State__buf __TA_ti_sysbios_heaps_HeapBuf_Instance_State__buf;

/* Module_State */
typedef ti_sysbios_heaps_HeapBuf_Handle __T1_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps;
typedef ti_sysbios_heaps_HeapBuf_Handle *ARRAY1_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps;
typedef const ti_sysbios_heaps_HeapBuf_Handle *CARRAY1_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps;
typedef ARRAY1_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps __TA_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_heaps_HeapBuf_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsEnabled ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__CR
#define ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__diagsEnabled (ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_heaps_HeapBuf_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsIncluded ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__CR
#define ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__diagsIncluded (ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask ti_sysbios_heaps_HeapBuf_Module__diagsMask__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__diagsMask__CR
#define ti_sysbios_heaps_HeapBuf_Module__diagsMask__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__diagsMask (ti_sysbios_heaps_HeapBuf_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_heaps_HeapBuf_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__gateObj ti_sysbios_heaps_HeapBuf_Module__gateObj__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__gateObj__CR
#define ti_sysbios_heaps_HeapBuf_Module__gateObj__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__gateObj__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__gateObj (ti_sysbios_heaps_HeapBuf_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_heaps_HeapBuf_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__gatePrms ti_sysbios_heaps_HeapBuf_Module__gatePrms__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__gatePrms__CR
#define ti_sysbios_heaps_HeapBuf_Module__gatePrms__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__gatePrms (ti_sysbios_heaps_HeapBuf_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_heaps_HeapBuf_Module__id;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__id ti_sysbios_heaps_HeapBuf_Module__id__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__id__CR
#define ti_sysbios_heaps_HeapBuf_Module__id__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__id*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__id__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__id (ti_sysbios_heaps_HeapBuf_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_heaps_HeapBuf_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerDefined ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerDefined__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerDefined (ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_heaps_HeapBuf_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerObj ti_sysbios_heaps_HeapBuf_Module__loggerObj__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerObj__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerObj__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerObj (ti_sysbios_heaps_HeapBuf_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn0 ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn0 (ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn1 ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn1 (ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn2 ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn2 (ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn4 ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn4 (ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn8 ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C;
#ifdef ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__CR
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C (*((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Module__loggerFxn8 (ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_heaps_HeapBuf_Object__count;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__count ti_sysbios_heaps_HeapBuf_Object__count__C;
#ifdef ti_sysbios_heaps_HeapBuf_Object__count__CR
#define ti_sysbios_heaps_HeapBuf_Object__count__C (*((CT__ti_sysbios_heaps_HeapBuf_Object__count*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Object__count__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Object__count (ti_sysbios_heaps_HeapBuf_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_heaps_HeapBuf_Object__heap;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__heap ti_sysbios_heaps_HeapBuf_Object__heap__C;
#ifdef ti_sysbios_heaps_HeapBuf_Object__heap__CR
#define ti_sysbios_heaps_HeapBuf_Object__heap__C (*((CT__ti_sysbios_heaps_HeapBuf_Object__heap*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Object__heap__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Object__heap (ti_sysbios_heaps_HeapBuf_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_heaps_HeapBuf_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__sizeof ti_sysbios_heaps_HeapBuf_Object__sizeof__C;
#ifdef ti_sysbios_heaps_HeapBuf_Object__sizeof__CR
#define ti_sysbios_heaps_HeapBuf_Object__sizeof__C (*((CT__ti_sysbios_heaps_HeapBuf_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Object__sizeof__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Object__sizeof (ti_sysbios_heaps_HeapBuf_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_heaps_HeapBuf_Object__table;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__table ti_sysbios_heaps_HeapBuf_Object__table__C;
#ifdef ti_sysbios_heaps_HeapBuf_Object__table__CR
#define ti_sysbios_heaps_HeapBuf_Object__table__C (*((CT__ti_sysbios_heaps_HeapBuf_Object__table*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Object__table__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_Object__table (ti_sysbios_heaps_HeapBuf_Object__table__C)
#endif

/* A_nullBuf */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_nullBuf;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_nullBuf ti_sysbios_heaps_HeapBuf_A_nullBuf__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_nullBuf__CR
#define ti_sysbios_heaps_HeapBuf_A_nullBuf (*((CT__ti_sysbios_heaps_HeapBuf_A_nullBuf*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_nullBuf__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_nullBuf (ti_sysbios_heaps_HeapBuf_A_nullBuf__C)
#endif

/* A_bufAlign */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_bufAlign;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_bufAlign ti_sysbios_heaps_HeapBuf_A_bufAlign__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_bufAlign__CR
#define ti_sysbios_heaps_HeapBuf_A_bufAlign (*((CT__ti_sysbios_heaps_HeapBuf_A_bufAlign*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_bufAlign__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_bufAlign (ti_sysbios_heaps_HeapBuf_A_bufAlign__C)
#endif

/* A_invalidAlign */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_invalidAlign;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidAlign ti_sysbios_heaps_HeapBuf_A_invalidAlign__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_invalidAlign__CR
#define ti_sysbios_heaps_HeapBuf_A_invalidAlign (*((CT__ti_sysbios_heaps_HeapBuf_A_invalidAlign*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_invalidAlign__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_invalidAlign (ti_sysbios_heaps_HeapBuf_A_invalidAlign__C)
#endif

/* A_invalidRequestedAlign */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign__CR
#define ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign (*((CT__ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign (ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign__C)
#endif

/* A_invalidBlockSize */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_invalidBlockSize;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidBlockSize ti_sysbios_heaps_HeapBuf_A_invalidBlockSize__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_invalidBlockSize__CR
#define ti_sysbios_heaps_HeapBuf_A_invalidBlockSize (*((CT__ti_sysbios_heaps_HeapBuf_A_invalidBlockSize*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_invalidBlockSize__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_invalidBlockSize (ti_sysbios_heaps_HeapBuf_A_invalidBlockSize__C)
#endif

/* A_zeroBlocks */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_zeroBlocks;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_zeroBlocks ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_zeroBlocks__CR
#define ti_sysbios_heaps_HeapBuf_A_zeroBlocks (*((CT__ti_sysbios_heaps_HeapBuf_A_zeroBlocks*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_zeroBlocks (ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C)
#endif

/* A_zeroBufSize */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_zeroBufSize;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_zeroBufSize ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_zeroBufSize__CR
#define ti_sysbios_heaps_HeapBuf_A_zeroBufSize (*((CT__ti_sysbios_heaps_HeapBuf_A_zeroBufSize*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_zeroBufSize (ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C)
#endif

/* A_invalidBufSize */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_invalidBufSize;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidBufSize ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_invalidBufSize__CR
#define ti_sysbios_heaps_HeapBuf_A_invalidBufSize (*((CT__ti_sysbios_heaps_HeapBuf_A_invalidBufSize*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_invalidBufSize (ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C)
#endif

/* A_noBlocksToFree */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_noBlocksToFree;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_noBlocksToFree ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__CR
#define ti_sysbios_heaps_HeapBuf_A_noBlocksToFree (*((CT__ti_sysbios_heaps_HeapBuf_A_noBlocksToFree*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_noBlocksToFree (ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C)
#endif

/* A_invalidFree */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_heaps_HeapBuf_A_invalidFree;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidFree ti_sysbios_heaps_HeapBuf_A_invalidFree__C;
#ifdef ti_sysbios_heaps_HeapBuf_A_invalidFree__CR
#define ti_sysbios_heaps_HeapBuf_A_invalidFree (*((CT__ti_sysbios_heaps_HeapBuf_A_invalidFree*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_A_invalidFree__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_A_invalidFree (ti_sysbios_heaps_HeapBuf_A_invalidFree__C)
#endif

/* E_size */
typedef xdc_runtime_Error_Id CT__ti_sysbios_heaps_HeapBuf_E_size;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_E_size ti_sysbios_heaps_HeapBuf_E_size__C;
#ifdef ti_sysbios_heaps_HeapBuf_E_size__CR
#define ti_sysbios_heaps_HeapBuf_E_size (*((CT__ti_sysbios_heaps_HeapBuf_E_size*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_E_size__C_offset)))
#else
#define ti_sysbios_heaps_HeapBuf_E_size (ti_sysbios_heaps_HeapBuf_E_size__C)
#endif

/* trackMaxAllocs */
typedef xdc_Bool CT__ti_sysbios_heaps_HeapBuf_trackMaxAllocs;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_trackMaxAllocs ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C;
#ifdef ti_sysbios_heaps_HeapBuf_trackMaxAllocs__CR
#define ti_sysbios_heaps_HeapBuf_trackMaxAllocs (*((CT__ti_sysbios_heaps_HeapBuf_trackMaxAllocs*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C_offset)))
#else
#ifdef ti_sysbios_heaps_HeapBuf_trackMaxAllocs__D
#define ti_sysbios_heaps_HeapBuf_trackMaxAllocs (ti_sysbios_heaps_HeapBuf_trackMaxAllocs__D)
#else
#define ti_sysbios_heaps_HeapBuf_trackMaxAllocs (ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C)
#endif
#endif

/* numConstructedHeaps */
typedef xdc_UInt CT__ti_sysbios_heaps_HeapBuf_numConstructedHeaps;
__extern __FAR__ const CT__ti_sysbios_heaps_HeapBuf_numConstructedHeaps ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C;
#ifdef ti_sysbios_heaps_HeapBuf_numConstructedHeaps__CR
#define ti_sysbios_heaps_HeapBuf_numConstructedHeaps (*((CT__ti_sysbios_heaps_HeapBuf_numConstructedHeaps*)(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C_offset)))
#else
#ifdef ti_sysbios_heaps_HeapBuf_numConstructedHeaps__D
#define ti_sysbios_heaps_HeapBuf_numConstructedHeaps (ti_sysbios_heaps_HeapBuf_numConstructedHeaps__D)
#else
#define ti_sysbios_heaps_HeapBuf_numConstructedHeaps (ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C)
#endif
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_heaps_HeapBuf_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_SizeT align;
    xdc_UInt numBlocks;
    xdc_SizeT blockSize;
    xdc_runtime_Memory_Size bufSize;
    xdc_Ptr buf;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_heaps_HeapBuf_Struct {
    const ti_sysbios_heaps_HeapBuf_Fxns__ *__fxns;
    xdc_SizeT f0;
    xdc_SizeT f1;
    xdc_UInt f2;
    xdc_runtime_Memory_Size f3;
    __TA_ti_sysbios_heaps_HeapBuf_Instance_State__buf f4;
    xdc_UInt f5;
    xdc_UInt f6;
    ti_sysbios_knl_Queue_Struct f7;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_heaps_HeapBuf_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Ptr (*alloc)(ti_sysbios_heaps_HeapBuf_Handle inst, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb);
    xdc_Void (*free)(ti_sysbios_heaps_HeapBuf_Handle inst, xdc_Ptr block, xdc_SizeT size);
    xdc_Bool (*isBlocking)(ti_sysbios_heaps_HeapBuf_Handle inst);
    xdc_Void (*getStats)(ti_sysbios_heaps_HeapBuf_Handle inst, xdc_runtime_Memory_Stats* stats);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_heaps_HeapBuf_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_heaps_HeapBuf_Fxns__ ti_sysbios_heaps_HeapBuf_Module__FXNS__C;
#else
#define ti_sysbios_heaps_HeapBuf_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_heaps_HeapBuf_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_heaps_HeapBuf_Module_startup ti_sysbios_heaps_HeapBuf_Module_startup__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Module_startup__E, "ti_sysbios_heaps_HeapBuf_Module_startup")
__extern xdc_Int ti_sysbios_heaps_HeapBuf_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Module_startup__F, "ti_sysbios_heaps_HeapBuf_Module_startup")
__extern xdc_Int ti_sysbios_heaps_HeapBuf_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Instance_init__E, "ti_sysbios_heaps_HeapBuf_Instance_init")
__extern xdc_Int ti_sysbios_heaps_HeapBuf_Instance_init__E(ti_sysbios_heaps_HeapBuf_Object *obj, const ti_sysbios_heaps_HeapBuf_Params *prms, xdc_runtime_Error_Block *eb);

/* Instance_finalize__E */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Instance_finalize__E, "ti_sysbios_heaps_HeapBuf_Instance_finalize")
__extern void ti_sysbios_heaps_HeapBuf_Instance_finalize__E(ti_sysbios_heaps_HeapBuf_Object *obj, int ec);

/* create */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_create, "ti_sysbios_heaps_HeapBuf_create")
__extern ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_create( const ti_sysbios_heaps_HeapBuf_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_construct, "ti_sysbios_heaps_HeapBuf_construct")
__extern void ti_sysbios_heaps_HeapBuf_construct(ti_sysbios_heaps_HeapBuf_Struct *obj, const ti_sysbios_heaps_HeapBuf_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_delete, "ti_sysbios_heaps_HeapBuf_delete")
__extern void ti_sysbios_heaps_HeapBuf_delete(ti_sysbios_heaps_HeapBuf_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_destruct, "ti_sysbios_heaps_HeapBuf_destruct")
__extern void ti_sysbios_heaps_HeapBuf_destruct(ti_sysbios_heaps_HeapBuf_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Handle__label__S, "ti_sysbios_heaps_HeapBuf_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_heaps_HeapBuf_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Module__startupDone__S, "ti_sysbios_heaps_HeapBuf_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_heaps_HeapBuf_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Object__create__S, "ti_sysbios_heaps_HeapBuf_Object__create__S")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Object__delete__S, "ti_sysbios_heaps_HeapBuf_Object__delete__S")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Object__get__S, "ti_sysbios_heaps_HeapBuf_Object__get__S")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Object__first__S, "ti_sysbios_heaps_HeapBuf_Object__first__S")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Object__next__S, "ti_sysbios_heaps_HeapBuf_Object__next__S")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_Params__init__S, "ti_sysbios_heaps_HeapBuf_Params__init__S")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* free__E */
#define ti_sysbios_heaps_HeapBuf_free ti_sysbios_heaps_HeapBuf_free__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_free__E, "ti_sysbios_heaps_HeapBuf_free")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_free__E( ti_sysbios_heaps_HeapBuf_Handle instp, xdc_Ptr block, xdc_SizeT size);

/* getStats__E */
#define ti_sysbios_heaps_HeapBuf_getStats ti_sysbios_heaps_HeapBuf_getStats__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_getStats__E, "ti_sysbios_heaps_HeapBuf_getStats")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_getStats__E( ti_sysbios_heaps_HeapBuf_Handle instp, xdc_runtime_Memory_Stats *stats);

/* alloc__E */
#define ti_sysbios_heaps_HeapBuf_alloc ti_sysbios_heaps_HeapBuf_alloc__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_alloc__E, "ti_sysbios_heaps_HeapBuf_alloc")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_alloc__E( ti_sysbios_heaps_HeapBuf_Handle instp, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block *eb);

/* isBlocking__E */
#define ti_sysbios_heaps_HeapBuf_isBlocking ti_sysbios_heaps_HeapBuf_isBlocking__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_isBlocking__E, "ti_sysbios_heaps_HeapBuf_isBlocking")
__extern xdc_Bool ti_sysbios_heaps_HeapBuf_isBlocking__E( ti_sysbios_heaps_HeapBuf_Handle instp);

/* getBlockSize__E */
#define ti_sysbios_heaps_HeapBuf_getBlockSize ti_sysbios_heaps_HeapBuf_getBlockSize__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_getBlockSize__E, "ti_sysbios_heaps_HeapBuf_getBlockSize")
__extern xdc_SizeT ti_sysbios_heaps_HeapBuf_getBlockSize__E( ti_sysbios_heaps_HeapBuf_Handle instp);

/* getAlign__E */
#define ti_sysbios_heaps_HeapBuf_getAlign ti_sysbios_heaps_HeapBuf_getAlign__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_getAlign__E, "ti_sysbios_heaps_HeapBuf_getAlign")
__extern xdc_SizeT ti_sysbios_heaps_HeapBuf_getAlign__E( ti_sysbios_heaps_HeapBuf_Handle instp);

/* getEndAddr__E */
#define ti_sysbios_heaps_HeapBuf_getEndAddr ti_sysbios_heaps_HeapBuf_getEndAddr__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_getEndAddr__E, "ti_sysbios_heaps_HeapBuf_getEndAddr")
__extern xdc_Ptr ti_sysbios_heaps_HeapBuf_getEndAddr__E( ti_sysbios_heaps_HeapBuf_Handle instp);

/* getExtendedStats__E */
#define ti_sysbios_heaps_HeapBuf_getExtendedStats ti_sysbios_heaps_HeapBuf_getExtendedStats__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_getExtendedStats__E, "ti_sysbios_heaps_HeapBuf_getExtendedStats")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_getExtendedStats__E( ti_sysbios_heaps_HeapBuf_Handle instp, ti_sysbios_heaps_HeapBuf_ExtendedStats *stats);

/* mergeHeapBufs__E */
#define ti_sysbios_heaps_HeapBuf_mergeHeapBufs ti_sysbios_heaps_HeapBuf_mergeHeapBufs__E
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_mergeHeapBufs__E, "ti_sysbios_heaps_HeapBuf_mergeHeapBufs")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_mergeHeapBufs__E( ti_sysbios_heaps_HeapBuf_Handle instp, ti_sysbios_heaps_HeapBuf_Handle heapBuf2);

/* postInit__I */
#define ti_sysbios_heaps_HeapBuf_postInit ti_sysbios_heaps_HeapBuf_postInit__I
xdc__CODESECT(ti_sysbios_heaps_HeapBuf_postInit__I, "ti_sysbios_heaps_HeapBuf_postInit")
__extern xdc_Void ti_sysbios_heaps_HeapBuf_postInit__I( ti_sysbios_heaps_HeapBuf_Object *heap);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IHeap_Module ti_sysbios_heaps_HeapBuf_Module_upCast(void);
static inline xdc_runtime_IHeap_Module ti_sysbios_heaps_HeapBuf_Module_upCast(void)
{
    return (xdc_runtime_IHeap_Module)&ti_sysbios_heaps_HeapBuf_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IHeap */
#define ti_sysbios_heaps_HeapBuf_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IHeap_Handle ti_sysbios_heaps_HeapBuf_Handle_upCast(ti_sysbios_heaps_HeapBuf_Handle i);
static inline xdc_runtime_IHeap_Handle ti_sysbios_heaps_HeapBuf_Handle_upCast(ti_sysbios_heaps_HeapBuf_Handle i)
{
    return (xdc_runtime_IHeap_Handle)i;
}

/* Handle_to_xdc_runtime_IHeap */
#define ti_sysbios_heaps_HeapBuf_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Handle_downCast(xdc_runtime_IHeap_Handle i);
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Handle_downCast(xdc_runtime_IHeap_Handle i)
{
    xdc_runtime_IHeap_Handle i2 = (xdc_runtime_IHeap_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_sysbios_heaps_HeapBuf_Module__FXNS__C) ? (ti_sysbios_heaps_HeapBuf_Handle)i : (ti_sysbios_heaps_HeapBuf_Handle)NULL;
}

/* Handle_from_xdc_runtime_IHeap */
#define ti_sysbios_heaps_HeapBuf_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_heaps_HeapBuf_Module_startupDone() ti_sysbios_heaps_HeapBuf_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_heaps_HeapBuf_Object_heap() ti_sysbios_heaps_HeapBuf_Object__heap__C

/* Module_heap */
#define ti_sysbios_heaps_HeapBuf_Module_heap() ti_sysbios_heaps_HeapBuf_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_heaps_HeapBuf_Module__id ti_sysbios_heaps_HeapBuf_Module_id(void);
static inline CT__ti_sysbios_heaps_HeapBuf_Module__id ti_sysbios_heaps_HeapBuf_Module_id( void ) 
{
    return ti_sysbios_heaps_HeapBuf_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_heaps_HeapBuf_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_heaps_HeapBuf_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_heaps_HeapBuf_Module__diagsMask__C != (CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_heaps_HeapBuf_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_heaps_HeapBuf_Module_getMask(void)
{
    return (ti_sysbios_heaps_HeapBuf_Module__diagsMask__C != (CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask)NULL) ? *ti_sysbios_heaps_HeapBuf_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_heaps_HeapBuf_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_heaps_HeapBuf_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_heaps_HeapBuf_Module__diagsMask__C != (CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask)NULL) {
        *ti_sysbios_heaps_HeapBuf_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_heaps_HeapBuf_Params_init(ti_sysbios_heaps_HeapBuf_Params *prms);
static inline void ti_sysbios_heaps_HeapBuf_Params_init( ti_sysbios_heaps_HeapBuf_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_heaps_HeapBuf_Params__init__S(prms, NULL, sizeof(ti_sysbios_heaps_HeapBuf_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_heaps_HeapBuf_Params_copy(ti_sysbios_heaps_HeapBuf_Params *dst, const ti_sysbios_heaps_HeapBuf_Params *src);
static inline void ti_sysbios_heaps_HeapBuf_Params_copy(ti_sysbios_heaps_HeapBuf_Params *dst, const ti_sysbios_heaps_HeapBuf_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_heaps_HeapBuf_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_heaps_HeapBuf_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_heaps_HeapBuf_Object_count() ti_sysbios_heaps_HeapBuf_Object__count__C

/* Object_sizeof */
#define ti_sysbios_heaps_HeapBuf_Object_sizeof() ti_sysbios_heaps_HeapBuf_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_get(ti_sysbios_heaps_HeapBuf_Object *oarr, int i);
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_get(ti_sysbios_heaps_HeapBuf_Object *oarr, int i) 
{
    return (ti_sysbios_heaps_HeapBuf_Handle)ti_sysbios_heaps_HeapBuf_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_first(void);
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_first(void)
{
    return (ti_sysbios_heaps_HeapBuf_Handle)ti_sysbios_heaps_HeapBuf_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_next(ti_sysbios_heaps_HeapBuf_Object *obj);
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Object_next(ti_sysbios_heaps_HeapBuf_Object *obj)
{
    return (ti_sysbios_heaps_HeapBuf_Handle)ti_sysbios_heaps_HeapBuf_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_heaps_HeapBuf_Handle_label(ti_sysbios_heaps_HeapBuf_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_heaps_HeapBuf_Handle_label(ti_sysbios_heaps_HeapBuf_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_heaps_HeapBuf_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_heaps_HeapBuf_Handle_name(ti_sysbios_heaps_HeapBuf_Handle inst);
static inline xdc_String ti_sysbios_heaps_HeapBuf_Handle_name(ti_sysbios_heaps_HeapBuf_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_heaps_HeapBuf_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_handle(ti_sysbios_heaps_HeapBuf_Struct *str);
static inline ti_sysbios_heaps_HeapBuf_Handle ti_sysbios_heaps_HeapBuf_handle(ti_sysbios_heaps_HeapBuf_Struct *str)
{
    return (ti_sysbios_heaps_HeapBuf_Handle)str;
}

/* struct */
static inline ti_sysbios_heaps_HeapBuf_Struct *ti_sysbios_heaps_HeapBuf_struct(ti_sysbios_heaps_HeapBuf_Handle inst);
static inline ti_sysbios_heaps_HeapBuf_Struct *ti_sysbios_heaps_HeapBuf_struct(ti_sysbios_heaps_HeapBuf_Handle inst)
{
    return (ti_sysbios_heaps_HeapBuf_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_heaps_HeapBuf__top__
#undef __nested__
#endif

#endif /* ti_sysbios_heaps_HeapBuf__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_heaps_HeapBuf__internalaccess))

#ifndef ti_sysbios_heaps_HeapBuf__include_state
#define ti_sysbios_heaps_HeapBuf__include_state

/* Module_State */
struct ti_sysbios_heaps_HeapBuf_Module_State {
    __TA_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps constructedHeaps;
};

/* Module__state__V */
#ifndef ti_sysbios_heaps_HeapBuf_Module__state__VR
extern struct ti_sysbios_heaps_HeapBuf_Module_State__ ti_sysbios_heaps_HeapBuf_Module__state__V;
#else
#define ti_sysbios_heaps_HeapBuf_Module__state__V (*((struct ti_sysbios_heaps_HeapBuf_Module_State__*)(xdcRomStatePtr + ti_sysbios_heaps_HeapBuf_Module__state__V_offset)))
#endif

/* Object */
struct ti_sysbios_heaps_HeapBuf_Object {
    const ti_sysbios_heaps_HeapBuf_Fxns__ *__fxns;
    xdc_SizeT blockSize;
    xdc_SizeT align;
    xdc_UInt numBlocks;
    xdc_runtime_Memory_Size bufSize;
    __TA_ti_sysbios_heaps_HeapBuf_Instance_State__buf buf;
    xdc_UInt numFreeBlocks;
    xdc_UInt minFreeBlocks;
    char dummy;
};

/* Instance_State_freeList */
#ifndef ti_sysbios_heaps_HeapBuf_Instance_State_freeList__OR
__extern __FAR__ const xdc_SizeT ti_sysbios_heaps_HeapBuf_Instance_State_freeList__O;
#endif
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_heaps_HeapBuf_Instance_State_freeList(ti_sysbios_heaps_HeapBuf_Object *obj);
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_heaps_HeapBuf_Instance_State_freeList(ti_sysbios_heaps_HeapBuf_Object *obj)
{
    return (ti_sysbios_knl_Queue_Handle)(((char*)obj) + ti_sysbios_heaps_HeapBuf_Instance_State_freeList__O);
}

#endif /* ti_sysbios_heaps_HeapBuf__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_heaps_HeapBuf__nolocalnames)

#ifndef ti_sysbios_heaps_HeapBuf__localnames__done
#define ti_sysbios_heaps_HeapBuf__localnames__done

/* module prefix */
#define HeapBuf_Instance ti_sysbios_heaps_HeapBuf_Instance
#define HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Handle
#define HeapBuf_Module ti_sysbios_heaps_HeapBuf_Module
#define HeapBuf_Object ti_sysbios_heaps_HeapBuf_Object
#define HeapBuf_Struct ti_sysbios_heaps_HeapBuf_Struct
#define HeapBuf_ExtendedStats ti_sysbios_heaps_HeapBuf_ExtendedStats
#define HeapBuf_Instance_State ti_sysbios_heaps_HeapBuf_Instance_State
#define HeapBuf_Module_State ti_sysbios_heaps_HeapBuf_Module_State
#define HeapBuf_A_nullBuf ti_sysbios_heaps_HeapBuf_A_nullBuf
#define HeapBuf_A_bufAlign ti_sysbios_heaps_HeapBuf_A_bufAlign
#define HeapBuf_A_invalidAlign ti_sysbios_heaps_HeapBuf_A_invalidAlign
#define HeapBuf_A_invalidRequestedAlign ti_sysbios_heaps_HeapBuf_A_invalidRequestedAlign
#define HeapBuf_A_invalidBlockSize ti_sysbios_heaps_HeapBuf_A_invalidBlockSize
#define HeapBuf_A_zeroBlocks ti_sysbios_heaps_HeapBuf_A_zeroBlocks
#define HeapBuf_A_zeroBufSize ti_sysbios_heaps_HeapBuf_A_zeroBufSize
#define HeapBuf_A_invalidBufSize ti_sysbios_heaps_HeapBuf_A_invalidBufSize
#define HeapBuf_A_noBlocksToFree ti_sysbios_heaps_HeapBuf_A_noBlocksToFree
#define HeapBuf_A_invalidFree ti_sysbios_heaps_HeapBuf_A_invalidFree
#define HeapBuf_E_size ti_sysbios_heaps_HeapBuf_E_size
#define HeapBuf_trackMaxAllocs ti_sysbios_heaps_HeapBuf_trackMaxAllocs
#define HeapBuf_numConstructedHeaps ti_sysbios_heaps_HeapBuf_numConstructedHeaps
#define HeapBuf_Instance_State_freeList ti_sysbios_heaps_HeapBuf_Instance_State_freeList
#define HeapBuf_Params ti_sysbios_heaps_HeapBuf_Params
#define HeapBuf_free ti_sysbios_heaps_HeapBuf_free
#define HeapBuf_getStats ti_sysbios_heaps_HeapBuf_getStats
#define HeapBuf_alloc ti_sysbios_heaps_HeapBuf_alloc
#define HeapBuf_isBlocking ti_sysbios_heaps_HeapBuf_isBlocking
#define HeapBuf_getBlockSize ti_sysbios_heaps_HeapBuf_getBlockSize
#define HeapBuf_getAlign ti_sysbios_heaps_HeapBuf_getAlign
#define HeapBuf_getEndAddr ti_sysbios_heaps_HeapBuf_getEndAddr
#define HeapBuf_getExtendedStats ti_sysbios_heaps_HeapBuf_getExtendedStats
#define HeapBuf_mergeHeapBufs ti_sysbios_heaps_HeapBuf_mergeHeapBufs
#define HeapBuf_Module_name ti_sysbios_heaps_HeapBuf_Module_name
#define HeapBuf_Module_id ti_sysbios_heaps_HeapBuf_Module_id
#define HeapBuf_Module_startup ti_sysbios_heaps_HeapBuf_Module_startup
#define HeapBuf_Module_startupDone ti_sysbios_heaps_HeapBuf_Module_startupDone
#define HeapBuf_Module_hasMask ti_sysbios_heaps_HeapBuf_Module_hasMask
#define HeapBuf_Module_getMask ti_sysbios_heaps_HeapBuf_Module_getMask
#define HeapBuf_Module_setMask ti_sysbios_heaps_HeapBuf_Module_setMask
#define HeapBuf_Object_heap ti_sysbios_heaps_HeapBuf_Object_heap
#define HeapBuf_Module_heap ti_sysbios_heaps_HeapBuf_Module_heap
#define HeapBuf_construct ti_sysbios_heaps_HeapBuf_construct
#define HeapBuf_create ti_sysbios_heaps_HeapBuf_create
#define HeapBuf_handle ti_sysbios_heaps_HeapBuf_handle
#define HeapBuf_struct ti_sysbios_heaps_HeapBuf_struct
#define HeapBuf_Handle_label ti_sysbios_heaps_HeapBuf_Handle_label
#define HeapBuf_Handle_name ti_sysbios_heaps_HeapBuf_Handle_name
#define HeapBuf_Instance_init ti_sysbios_heaps_HeapBuf_Instance_init
#define HeapBuf_Object_count ti_sysbios_heaps_HeapBuf_Object_count
#define HeapBuf_Object_get ti_sysbios_heaps_HeapBuf_Object_get
#define HeapBuf_Object_first ti_sysbios_heaps_HeapBuf_Object_first
#define HeapBuf_Object_next ti_sysbios_heaps_HeapBuf_Object_next
#define HeapBuf_Object_sizeof ti_sysbios_heaps_HeapBuf_Object_sizeof
#define HeapBuf_Params_copy ti_sysbios_heaps_HeapBuf_Params_copy
#define HeapBuf_Params_init ti_sysbios_heaps_HeapBuf_Params_init
#define HeapBuf_Instance_finalize ti_sysbios_heaps_HeapBuf_Instance_finalize
#define HeapBuf_delete ti_sysbios_heaps_HeapBuf_delete
#define HeapBuf_destruct ti_sysbios_heaps_HeapBuf_destruct
#define HeapBuf_Module_upCast ti_sysbios_heaps_HeapBuf_Module_upCast
#define HeapBuf_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Module_to_xdc_runtime_IHeap
#define HeapBuf_Handle_upCast ti_sysbios_heaps_HeapBuf_Handle_upCast
#define HeapBuf_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_to_xdc_runtime_IHeap
#define HeapBuf_Handle_downCast ti_sysbios_heaps_HeapBuf_Handle_downCast
#define HeapBuf_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_from_xdc_runtime_IHeap

#endif /* ti_sysbios_heaps_HeapBuf__localnames__done */
#endif
