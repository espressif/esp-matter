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

#ifndef ti_uia_runtime_LoggerSM__include
#define ti_uia_runtime_LoggerSM__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_LoggerSM__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_LoggerSM___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/ITimestampClient.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/ILogger.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/IFilterLogger.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* VERSION */
#define ti_uia_runtime_LoggerSM_VERSION (1)

/* Module_State */
typedef xdc_Char __T1_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer;
typedef xdc_Char *ARRAY1_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer;
typedef const xdc_Char *CARRAY1_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer;
typedef ARRAY1_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer __TA_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer;

/* SharedObj */
struct ti_uia_runtime_LoggerSM_SharedObj {
    xdc_Bits32 headerTag;
    xdc_Bits32 version;
    xdc_Bits32 numPartitions;
    xdc_Char *endPtr;
    volatile xdc_Char *readPtr;
    xdc_Char *writePtr;
    xdc_Char *buffer;
    xdc_Bits32 bufferSizeMAU;
    xdc_Bits32 droppedEvents;
    xdc_Bits16 moduleId;
    xdc_Bits16 instanceId;
    xdc_Bits16 decode;
    xdc_Bits16 overwrite;
};


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_runtime_LoggerSM_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__diagsEnabled ti_uia_runtime_LoggerSM_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_LoggerSM_Module__diagsEnabled__CR
#define ti_uia_runtime_LoggerSM_Module__diagsEnabled__C (*((CT__ti_uia_runtime_LoggerSM_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__diagsEnabled (ti_uia_runtime_LoggerSM_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_LoggerSM_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__diagsIncluded ti_uia_runtime_LoggerSM_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_LoggerSM_Module__diagsIncluded__CR
#define ti_uia_runtime_LoggerSM_Module__diagsIncluded__C (*((CT__ti_uia_runtime_LoggerSM_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__diagsIncluded (ti_uia_runtime_LoggerSM_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_LoggerSM_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__diagsMask ti_uia_runtime_LoggerSM_Module__diagsMask__C;
#ifdef ti_uia_runtime_LoggerSM_Module__diagsMask__CR
#define ti_uia_runtime_LoggerSM_Module__diagsMask__C (*((CT__ti_uia_runtime_LoggerSM_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__diagsMask (ti_uia_runtime_LoggerSM_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_LoggerSM_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__gateObj ti_uia_runtime_LoggerSM_Module__gateObj__C;
#ifdef ti_uia_runtime_LoggerSM_Module__gateObj__CR
#define ti_uia_runtime_LoggerSM_Module__gateObj__C (*((CT__ti_uia_runtime_LoggerSM_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__gateObj (ti_uia_runtime_LoggerSM_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_LoggerSM_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__gatePrms ti_uia_runtime_LoggerSM_Module__gatePrms__C;
#ifdef ti_uia_runtime_LoggerSM_Module__gatePrms__CR
#define ti_uia_runtime_LoggerSM_Module__gatePrms__C (*((CT__ti_uia_runtime_LoggerSM_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__gatePrms (ti_uia_runtime_LoggerSM_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_LoggerSM_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__id ti_uia_runtime_LoggerSM_Module__id__C;
#ifdef ti_uia_runtime_LoggerSM_Module__id__CR
#define ti_uia_runtime_LoggerSM_Module__id__C (*((CT__ti_uia_runtime_LoggerSM_Module__id*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__id__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__id (ti_uia_runtime_LoggerSM_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_LoggerSM_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerDefined ti_uia_runtime_LoggerSM_Module__loggerDefined__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerDefined__CR
#define ti_uia_runtime_LoggerSM_Module__loggerDefined__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerDefined (ti_uia_runtime_LoggerSM_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_LoggerSM_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerObj ti_uia_runtime_LoggerSM_Module__loggerObj__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerObj__CR
#define ti_uia_runtime_LoggerSM_Module__loggerObj__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerObj (ti_uia_runtime_LoggerSM_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_LoggerSM_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerFxn0 ti_uia_runtime_LoggerSM_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerFxn0__CR
#define ti_uia_runtime_LoggerSM_Module__loggerFxn0__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerFxn0 (ti_uia_runtime_LoggerSM_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_LoggerSM_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerFxn1 ti_uia_runtime_LoggerSM_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerFxn1__CR
#define ti_uia_runtime_LoggerSM_Module__loggerFxn1__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerFxn1 (ti_uia_runtime_LoggerSM_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_LoggerSM_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerFxn2 ti_uia_runtime_LoggerSM_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerFxn2__CR
#define ti_uia_runtime_LoggerSM_Module__loggerFxn2__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerFxn2 (ti_uia_runtime_LoggerSM_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_LoggerSM_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerFxn4 ti_uia_runtime_LoggerSM_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerFxn4__CR
#define ti_uia_runtime_LoggerSM_Module__loggerFxn4__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerFxn4 (ti_uia_runtime_LoggerSM_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_LoggerSM_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Module__loggerFxn8 ti_uia_runtime_LoggerSM_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_LoggerSM_Module__loggerFxn8__CR
#define ti_uia_runtime_LoggerSM_Module__loggerFxn8__C (*((CT__ti_uia_runtime_LoggerSM_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Module__loggerFxn8 (ti_uia_runtime_LoggerSM_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_LoggerSM_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Object__count ti_uia_runtime_LoggerSM_Object__count__C;
#ifdef ti_uia_runtime_LoggerSM_Object__count__CR
#define ti_uia_runtime_LoggerSM_Object__count__C (*((CT__ti_uia_runtime_LoggerSM_Object__count*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Object__count__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Object__count (ti_uia_runtime_LoggerSM_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_LoggerSM_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Object__heap ti_uia_runtime_LoggerSM_Object__heap__C;
#ifdef ti_uia_runtime_LoggerSM_Object__heap__CR
#define ti_uia_runtime_LoggerSM_Object__heap__C (*((CT__ti_uia_runtime_LoggerSM_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Object__heap__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Object__heap (ti_uia_runtime_LoggerSM_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_LoggerSM_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Object__sizeof ti_uia_runtime_LoggerSM_Object__sizeof__C;
#ifdef ti_uia_runtime_LoggerSM_Object__sizeof__CR
#define ti_uia_runtime_LoggerSM_Object__sizeof__C (*((CT__ti_uia_runtime_LoggerSM_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Object__sizeof (ti_uia_runtime_LoggerSM_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_LoggerSM_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_Object__table ti_uia_runtime_LoggerSM_Object__table__C;
#ifdef ti_uia_runtime_LoggerSM_Object__table__CR
#define ti_uia_runtime_LoggerSM_Object__table__C (*((CT__ti_uia_runtime_LoggerSM_Object__table*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Object__table__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_Object__table (ti_uia_runtime_LoggerSM_Object__table__C)
#endif

/* filterByLevel */
typedef xdc_Bool CT__ti_uia_runtime_LoggerSM_filterByLevel;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_filterByLevel ti_uia_runtime_LoggerSM_filterByLevel__C;
#ifdef ti_uia_runtime_LoggerSM_filterByLevel__CR
#define ti_uia_runtime_LoggerSM_filterByLevel (*((CT__ti_uia_runtime_LoggerSM_filterByLevel*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_filterByLevel__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_filterByLevel__D
#define ti_uia_runtime_LoggerSM_filterByLevel (ti_uia_runtime_LoggerSM_filterByLevel__D)
#else
#define ti_uia_runtime_LoggerSM_filterByLevel (ti_uia_runtime_LoggerSM_filterByLevel__C)
#endif
#endif

/* E_badLevel */
typedef xdc_runtime_Error_Id CT__ti_uia_runtime_LoggerSM_E_badLevel;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_E_badLevel ti_uia_runtime_LoggerSM_E_badLevel__C;
#ifdef ti_uia_runtime_LoggerSM_E_badLevel__CR
#define ti_uia_runtime_LoggerSM_E_badLevel (*((CT__ti_uia_runtime_LoggerSM_E_badLevel*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_E_badLevel__C_offset)))
#else
#define ti_uia_runtime_LoggerSM_E_badLevel (ti_uia_runtime_LoggerSM_E_badLevel__C)
#endif

/* isTimestampEnabled */
typedef xdc_Bool CT__ti_uia_runtime_LoggerSM_isTimestampEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_isTimestampEnabled ti_uia_runtime_LoggerSM_isTimestampEnabled__C;
#ifdef ti_uia_runtime_LoggerSM_isTimestampEnabled__CR
#define ti_uia_runtime_LoggerSM_isTimestampEnabled (*((CT__ti_uia_runtime_LoggerSM_isTimestampEnabled*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_isTimestampEnabled__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_isTimestampEnabled__D
#define ti_uia_runtime_LoggerSM_isTimestampEnabled (ti_uia_runtime_LoggerSM_isTimestampEnabled__D)
#else
#define ti_uia_runtime_LoggerSM_isTimestampEnabled (ti_uia_runtime_LoggerSM_isTimestampEnabled__C)
#endif
#endif

/* decode */
typedef xdc_Bool CT__ti_uia_runtime_LoggerSM_decode;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_decode ti_uia_runtime_LoggerSM_decode__C;
#ifdef ti_uia_runtime_LoggerSM_decode__CR
#define ti_uia_runtime_LoggerSM_decode (*((CT__ti_uia_runtime_LoggerSM_decode*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_decode__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_decode__D
#define ti_uia_runtime_LoggerSM_decode (ti_uia_runtime_LoggerSM_decode__D)
#else
#define ti_uia_runtime_LoggerSM_decode (ti_uia_runtime_LoggerSM_decode__C)
#endif
#endif

/* overwrite */
typedef xdc_Bool CT__ti_uia_runtime_LoggerSM_overwrite;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_overwrite ti_uia_runtime_LoggerSM_overwrite__C;
#ifdef ti_uia_runtime_LoggerSM_overwrite__CR
#define ti_uia_runtime_LoggerSM_overwrite (*((CT__ti_uia_runtime_LoggerSM_overwrite*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_overwrite__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_overwrite__D
#define ti_uia_runtime_LoggerSM_overwrite (ti_uia_runtime_LoggerSM_overwrite__D)
#else
#define ti_uia_runtime_LoggerSM_overwrite (ti_uia_runtime_LoggerSM_overwrite__C)
#endif
#endif

/* level1Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_runtime_LoggerSM_level1Mask;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_level1Mask ti_uia_runtime_LoggerSM_level1Mask__C;
#ifdef ti_uia_runtime_LoggerSM_level1Mask__CR
#define ti_uia_runtime_LoggerSM_level1Mask (*((CT__ti_uia_runtime_LoggerSM_level1Mask*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_level1Mask__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_level1Mask__D
#define ti_uia_runtime_LoggerSM_level1Mask (ti_uia_runtime_LoggerSM_level1Mask__D)
#else
#define ti_uia_runtime_LoggerSM_level1Mask (ti_uia_runtime_LoggerSM_level1Mask__C)
#endif
#endif

/* level2Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_runtime_LoggerSM_level2Mask;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_level2Mask ti_uia_runtime_LoggerSM_level2Mask__C;
#ifdef ti_uia_runtime_LoggerSM_level2Mask__CR
#define ti_uia_runtime_LoggerSM_level2Mask (*((CT__ti_uia_runtime_LoggerSM_level2Mask*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_level2Mask__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_level2Mask__D
#define ti_uia_runtime_LoggerSM_level2Mask (ti_uia_runtime_LoggerSM_level2Mask__D)
#else
#define ti_uia_runtime_LoggerSM_level2Mask (ti_uia_runtime_LoggerSM_level2Mask__C)
#endif
#endif

/* level3Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_runtime_LoggerSM_level3Mask;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_level3Mask ti_uia_runtime_LoggerSM_level3Mask__C;
#ifdef ti_uia_runtime_LoggerSM_level3Mask__CR
#define ti_uia_runtime_LoggerSM_level3Mask (*((CT__ti_uia_runtime_LoggerSM_level3Mask*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_level3Mask__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_level3Mask__D
#define ti_uia_runtime_LoggerSM_level3Mask (ti_uia_runtime_LoggerSM_level3Mask__D)
#else
#define ti_uia_runtime_LoggerSM_level3Mask (ti_uia_runtime_LoggerSM_level3Mask__C)
#endif
#endif

/* level4Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_runtime_LoggerSM_level4Mask;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_level4Mask ti_uia_runtime_LoggerSM_level4Mask__C;
#ifdef ti_uia_runtime_LoggerSM_level4Mask__CR
#define ti_uia_runtime_LoggerSM_level4Mask (*((CT__ti_uia_runtime_LoggerSM_level4Mask*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_level4Mask__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_level4Mask__D
#define ti_uia_runtime_LoggerSM_level4Mask (ti_uia_runtime_LoggerSM_level4Mask__D)
#else
#define ti_uia_runtime_LoggerSM_level4Mask (ti_uia_runtime_LoggerSM_level4Mask__C)
#endif
#endif

/* numPartitions */
typedef xdc_Int CT__ti_uia_runtime_LoggerSM_numPartitions;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_numPartitions ti_uia_runtime_LoggerSM_numPartitions__C;
#ifdef ti_uia_runtime_LoggerSM_numPartitions__CR
#define ti_uia_runtime_LoggerSM_numPartitions (*((CT__ti_uia_runtime_LoggerSM_numPartitions*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_numPartitions__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_numPartitions__D
#define ti_uia_runtime_LoggerSM_numPartitions (ti_uia_runtime_LoggerSM_numPartitions__D)
#else
#define ti_uia_runtime_LoggerSM_numPartitions (ti_uia_runtime_LoggerSM_numPartitions__C)
#endif
#endif

/* sharedMemorySize */
typedef xdc_SizeT CT__ti_uia_runtime_LoggerSM_sharedMemorySize;
__extern __FAR__ const CT__ti_uia_runtime_LoggerSM_sharedMemorySize ti_uia_runtime_LoggerSM_sharedMemorySize__C;
#ifdef ti_uia_runtime_LoggerSM_sharedMemorySize__CR
#define ti_uia_runtime_LoggerSM_sharedMemorySize (*((CT__ti_uia_runtime_LoggerSM_sharedMemorySize*)(xdcRomConstPtr + ti_uia_runtime_LoggerSM_sharedMemorySize__C_offset)))
#else
#ifdef ti_uia_runtime_LoggerSM_sharedMemorySize__D
#define ti_uia_runtime_LoggerSM_sharedMemorySize (ti_uia_runtime_LoggerSM_sharedMemorySize__D)
#else
#define ti_uia_runtime_LoggerSM_sharedMemorySize (ti_uia_runtime_LoggerSM_sharedMemorySize__C)
#endif
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_runtime_LoggerSM_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_uia_runtime_LoggerSM_Struct {
    const ti_uia_runtime_LoggerSM_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_LoggerSM_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(ti_uia_runtime_LoggerSM_Handle inst);
    xdc_Bool (*disable)(ti_uia_runtime_LoggerSM_Handle inst);
    xdc_Void (*write0)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
    xdc_Void (*write1)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
    xdc_Void (*write2)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
    xdc_Void (*write4)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
    xdc_Void (*write8)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
    xdc_Void (*setFilterLevel)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
    xdc_runtime_Diags_Mask (*getFilterLevel)(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Diags_EventLevel level);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_LoggerSM_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_runtime_LoggerSM_Fxns__ ti_uia_runtime_LoggerSM_Module__FXNS__C;
#else
#define ti_uia_runtime_LoggerSM_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_runtime_LoggerSM_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_LoggerSM_Module_startup ti_uia_runtime_LoggerSM_Module_startup__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_Module_startup__E, "ti_uia_runtime_LoggerSM_Module_startup")
__extern xdc_Int ti_uia_runtime_LoggerSM_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_uia_runtime_LoggerSM_Module_startup__F, "ti_uia_runtime_LoggerSM_Module_startup")
__extern xdc_Int ti_uia_runtime_LoggerSM_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Instance_init__E, "ti_uia_runtime_LoggerSM_Instance_init")
__extern xdc_Void ti_uia_runtime_LoggerSM_Instance_init__E(ti_uia_runtime_LoggerSM_Object *obj, const ti_uia_runtime_LoggerSM_Params *prms);

/* create */
xdc__CODESECT(ti_uia_runtime_LoggerSM_create, "ti_uia_runtime_LoggerSM_create")
__extern ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_create( const ti_uia_runtime_LoggerSM_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_uia_runtime_LoggerSM_construct, "ti_uia_runtime_LoggerSM_construct")
__extern void ti_uia_runtime_LoggerSM_construct(ti_uia_runtime_LoggerSM_Struct *obj, const ti_uia_runtime_LoggerSM_Params *prms);

/* delete */
xdc__CODESECT(ti_uia_runtime_LoggerSM_delete, "ti_uia_runtime_LoggerSM_delete")
__extern void ti_uia_runtime_LoggerSM_delete(ti_uia_runtime_LoggerSM_Handle *instp);

/* destruct */
xdc__CODESECT(ti_uia_runtime_LoggerSM_destruct, "ti_uia_runtime_LoggerSM_destruct")
__extern void ti_uia_runtime_LoggerSM_destruct(ti_uia_runtime_LoggerSM_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Handle__label__S, "ti_uia_runtime_LoggerSM_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_runtime_LoggerSM_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Module__startupDone__S, "ti_uia_runtime_LoggerSM_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_LoggerSM_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Object__create__S, "ti_uia_runtime_LoggerSM_Object__create__S")
__extern xdc_Ptr ti_uia_runtime_LoggerSM_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Object__delete__S, "ti_uia_runtime_LoggerSM_Object__delete__S")
__extern xdc_Void ti_uia_runtime_LoggerSM_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Object__get__S, "ti_uia_runtime_LoggerSM_Object__get__S")
__extern xdc_Ptr ti_uia_runtime_LoggerSM_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Object__first__S, "ti_uia_runtime_LoggerSM_Object__first__S")
__extern xdc_Ptr ti_uia_runtime_LoggerSM_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Object__next__S, "ti_uia_runtime_LoggerSM_Object__next__S")
__extern xdc_Ptr ti_uia_runtime_LoggerSM_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_runtime_LoggerSM_Params__init__S, "ti_uia_runtime_LoggerSM_Params__init__S")
__extern xdc_Void ti_uia_runtime_LoggerSM_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* setPartitionId__E */
#define ti_uia_runtime_LoggerSM_setPartitionId ti_uia_runtime_LoggerSM_setPartitionId__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_setPartitionId__E, "ti_uia_runtime_LoggerSM_setPartitionId")
__extern xdc_Void ti_uia_runtime_LoggerSM_setPartitionId__E( xdc_Int partitionId);

/* setSharedMemory__E */
#define ti_uia_runtime_LoggerSM_setSharedMemory ti_uia_runtime_LoggerSM_setSharedMemory__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_setSharedMemory__E, "ti_uia_runtime_LoggerSM_setSharedMemory")
__extern xdc_Bool ti_uia_runtime_LoggerSM_setSharedMemory__E( xdc_Ptr sharedMemory, xdc_Bits32 sharedMemorySize);

/* enable__E */
#define ti_uia_runtime_LoggerSM_enable ti_uia_runtime_LoggerSM_enable__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_enable__E, "ti_uia_runtime_LoggerSM_enable")
__extern xdc_Bool ti_uia_runtime_LoggerSM_enable__E( ti_uia_runtime_LoggerSM_Handle instp);

/* disable__E */
#define ti_uia_runtime_LoggerSM_disable ti_uia_runtime_LoggerSM_disable__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_disable__E, "ti_uia_runtime_LoggerSM_disable")
__extern xdc_Bool ti_uia_runtime_LoggerSM_disable__E( ti_uia_runtime_LoggerSM_Handle instp);

/* write0__E */
#define ti_uia_runtime_LoggerSM_write0 ti_uia_runtime_LoggerSM_write0__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_write0__E, "ti_uia_runtime_LoggerSM_write0")
__extern xdc_Void ti_uia_runtime_LoggerSM_write0__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);

/* write1__E */
#define ti_uia_runtime_LoggerSM_write1 ti_uia_runtime_LoggerSM_write1__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_write1__E, "ti_uia_runtime_LoggerSM_write1")
__extern xdc_Void ti_uia_runtime_LoggerSM_write1__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);

/* write2__E */
#define ti_uia_runtime_LoggerSM_write2 ti_uia_runtime_LoggerSM_write2__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_write2__E, "ti_uia_runtime_LoggerSM_write2")
__extern xdc_Void ti_uia_runtime_LoggerSM_write2__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);

/* write4__E */
#define ti_uia_runtime_LoggerSM_write4 ti_uia_runtime_LoggerSM_write4__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_write4__E, "ti_uia_runtime_LoggerSM_write4")
__extern xdc_Void ti_uia_runtime_LoggerSM_write4__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);

/* write8__E */
#define ti_uia_runtime_LoggerSM_write8 ti_uia_runtime_LoggerSM_write8__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_write8__E, "ti_uia_runtime_LoggerSM_write8")
__extern xdc_Void ti_uia_runtime_LoggerSM_write8__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);

/* setFilterLevel__E */
#define ti_uia_runtime_LoggerSM_setFilterLevel ti_uia_runtime_LoggerSM_setFilterLevel__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_setFilterLevel__E, "ti_uia_runtime_LoggerSM_setFilterLevel")
__extern xdc_Void ti_uia_runtime_LoggerSM_setFilterLevel__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);

/* getFilterLevel__E */
#define ti_uia_runtime_LoggerSM_getFilterLevel ti_uia_runtime_LoggerSM_getFilterLevel__E
xdc__CODESECT(ti_uia_runtime_LoggerSM_getFilterLevel__E, "ti_uia_runtime_LoggerSM_getFilterLevel")
__extern xdc_runtime_Diags_Mask ti_uia_runtime_LoggerSM_getFilterLevel__E( ti_uia_runtime_LoggerSM_Handle instp, xdc_runtime_Diags_EventLevel level);

/* filterOutEvent__I */
#define ti_uia_runtime_LoggerSM_filterOutEvent ti_uia_runtime_LoggerSM_filterOutEvent__I
xdc__CODESECT(ti_uia_runtime_LoggerSM_filterOutEvent__I, "ti_uia_runtime_LoggerSM_filterOutEvent")
__extern xdc_Bool ti_uia_runtime_LoggerSM_filterOutEvent__I( xdc_runtime_Diags_Mask mask);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IFilterLogger_Module ti_uia_runtime_LoggerSM_Module_upCast(void);
static inline xdc_runtime_IFilterLogger_Module ti_uia_runtime_LoggerSM_Module_upCast(void)
{
    return (xdc_runtime_IFilterLogger_Module)&ti_uia_runtime_LoggerSM_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_LoggerSM_Module_to_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IFilterLogger_Handle ti_uia_runtime_LoggerSM_Handle_upCast(ti_uia_runtime_LoggerSM_Handle i);
static inline xdc_runtime_IFilterLogger_Handle ti_uia_runtime_LoggerSM_Handle_upCast(ti_uia_runtime_LoggerSM_Handle i)
{
    return (xdc_runtime_IFilterLogger_Handle)i;
}

/* Handle_to_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_LoggerSM_Handle_to_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Handle_upCast

/* Handle_downCast */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Handle_downCast(xdc_runtime_IFilterLogger_Handle i);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Handle_downCast(xdc_runtime_IFilterLogger_Handle i)
{
    xdc_runtime_IFilterLogger_Handle i2 = (xdc_runtime_IFilterLogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_runtime_LoggerSM_Module__FXNS__C) ? (ti_uia_runtime_LoggerSM_Handle)i : (ti_uia_runtime_LoggerSM_Handle)NULL;
}

/* Handle_from_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_LoggerSM_Handle_from_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Handle_downCast

/* Module_upCast2 */
static inline xdc_runtime_ILogger_Module ti_uia_runtime_LoggerSM_Module_upCast2(void);
static inline xdc_runtime_ILogger_Module ti_uia_runtime_LoggerSM_Module_upCast2(void)
{
    return (xdc_runtime_ILogger_Module)&ti_uia_runtime_LoggerSM_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_uia_runtime_LoggerSM_Module_to_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Module_upCast2

/* Handle_upCast2 */
static inline xdc_runtime_ILogger_Handle ti_uia_runtime_LoggerSM_Handle_upCast2(ti_uia_runtime_LoggerSM_Handle i);
static inline xdc_runtime_ILogger_Handle ti_uia_runtime_LoggerSM_Handle_upCast2(ti_uia_runtime_LoggerSM_Handle i)
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_uia_runtime_LoggerSM_Handle_to_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Handle_upCast2

/* Handle_downCast2 */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Handle_downCast2(xdc_runtime_ILogger_Handle i);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Handle_downCast2(xdc_runtime_ILogger_Handle i)
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_runtime_LoggerSM_Module__FXNS__C) ? (ti_uia_runtime_LoggerSM_Handle)i : (ti_uia_runtime_LoggerSM_Handle)NULL;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_uia_runtime_LoggerSM_Handle_from_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Handle_downCast2


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_LoggerSM_Module_startupDone() ti_uia_runtime_LoggerSM_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_LoggerSM_Object_heap() ti_uia_runtime_LoggerSM_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_LoggerSM_Module_heap() ti_uia_runtime_LoggerSM_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_LoggerSM_Module__id ti_uia_runtime_LoggerSM_Module_id(void);
static inline CT__ti_uia_runtime_LoggerSM_Module__id ti_uia_runtime_LoggerSM_Module_id( void ) 
{
    return ti_uia_runtime_LoggerSM_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_LoggerSM_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_LoggerSM_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_LoggerSM_Module__diagsMask__C != (CT__ti_uia_runtime_LoggerSM_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_LoggerSM_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_LoggerSM_Module_getMask(void)
{
    return (ti_uia_runtime_LoggerSM_Module__diagsMask__C != (CT__ti_uia_runtime_LoggerSM_Module__diagsMask)NULL) ? *ti_uia_runtime_LoggerSM_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_LoggerSM_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_LoggerSM_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_LoggerSM_Module__diagsMask__C != (CT__ti_uia_runtime_LoggerSM_Module__diagsMask)NULL) {
        *ti_uia_runtime_LoggerSM_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_uia_runtime_LoggerSM_Params_init(ti_uia_runtime_LoggerSM_Params *prms);
static inline void ti_uia_runtime_LoggerSM_Params_init( ti_uia_runtime_LoggerSM_Params *prms ) 
{
    if (prms != NULL) {
        ti_uia_runtime_LoggerSM_Params__init__S(prms, NULL, sizeof(ti_uia_runtime_LoggerSM_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_uia_runtime_LoggerSM_Params_copy(ti_uia_runtime_LoggerSM_Params *dst, const ti_uia_runtime_LoggerSM_Params *src);
static inline void ti_uia_runtime_LoggerSM_Params_copy(ti_uia_runtime_LoggerSM_Params *dst, const ti_uia_runtime_LoggerSM_Params *src) 
{
    if (dst != NULL) {
        ti_uia_runtime_LoggerSM_Params__init__S(dst, (const void *)src, sizeof(ti_uia_runtime_LoggerSM_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_uia_runtime_LoggerSM_Object_count() ti_uia_runtime_LoggerSM_Object__count__C

/* Object_sizeof */
#define ti_uia_runtime_LoggerSM_Object_sizeof() ti_uia_runtime_LoggerSM_Object__sizeof__C

/* Object_get */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_get(ti_uia_runtime_LoggerSM_Object *oarr, int i);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_get(ti_uia_runtime_LoggerSM_Object *oarr, int i) 
{
    return (ti_uia_runtime_LoggerSM_Handle)ti_uia_runtime_LoggerSM_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_first(void);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_first(void)
{
    return (ti_uia_runtime_LoggerSM_Handle)ti_uia_runtime_LoggerSM_Object__first__S();
}

/* Object_next */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_next(ti_uia_runtime_LoggerSM_Object *obj);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_Object_next(ti_uia_runtime_LoggerSM_Object *obj)
{
    return (ti_uia_runtime_LoggerSM_Handle)ti_uia_runtime_LoggerSM_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_runtime_LoggerSM_Handle_label(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_runtime_LoggerSM_Handle_label(ti_uia_runtime_LoggerSM_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_uia_runtime_LoggerSM_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_uia_runtime_LoggerSM_Handle_name(ti_uia_runtime_LoggerSM_Handle inst);
static inline xdc_String ti_uia_runtime_LoggerSM_Handle_name(ti_uia_runtime_LoggerSM_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_uia_runtime_LoggerSM_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_handle(ti_uia_runtime_LoggerSM_Struct *str);
static inline ti_uia_runtime_LoggerSM_Handle ti_uia_runtime_LoggerSM_handle(ti_uia_runtime_LoggerSM_Struct *str)
{
    return (ti_uia_runtime_LoggerSM_Handle)str;
}

/* struct */
static inline ti_uia_runtime_LoggerSM_Struct *ti_uia_runtime_LoggerSM_struct(ti_uia_runtime_LoggerSM_Handle inst);
static inline ti_uia_runtime_LoggerSM_Struct *ti_uia_runtime_LoggerSM_struct(ti_uia_runtime_LoggerSM_Handle inst)
{
    return (ti_uia_runtime_LoggerSM_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_LoggerSM__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_LoggerSM__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_LoggerSM__internalaccess))

#ifndef ti_uia_runtime_LoggerSM__include_state
#define ti_uia_runtime_LoggerSM__include_state

/* Module_State */
struct ti_uia_runtime_LoggerSM_Module_State {
    xdc_Int partitionId;
    xdc_runtime_Diags_Mask level1;
    xdc_runtime_Diags_Mask level2;
    xdc_runtime_Diags_Mask level3;
    ti_uia_runtime_LoggerSM_SharedObj *sharedObj;
    __TA_ti_uia_runtime_LoggerSM_Module_State__sharedBuffer sharedBuffer;
    xdc_Bits16 serial;
    xdc_Bool enabled;
};

/* Module__state__V */
#ifndef ti_uia_runtime_LoggerSM_Module__state__VR
extern struct ti_uia_runtime_LoggerSM_Module_State__ ti_uia_runtime_LoggerSM_Module__state__V;
#else
#define ti_uia_runtime_LoggerSM_Module__state__V (*((struct ti_uia_runtime_LoggerSM_Module_State__*)(xdcRomStatePtr + ti_uia_runtime_LoggerSM_Module__state__V_offset)))
#endif

/* Object */
struct ti_uia_runtime_LoggerSM_Object {
    const ti_uia_runtime_LoggerSM_Fxns__ *__fxns;
};

#endif /* ti_uia_runtime_LoggerSM__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_LoggerSM__nolocalnames)

#ifndef ti_uia_runtime_LoggerSM__localnames__done
#define ti_uia_runtime_LoggerSM__localnames__done

/* module prefix */
#define LoggerSM_Instance ti_uia_runtime_LoggerSM_Instance
#define LoggerSM_Handle ti_uia_runtime_LoggerSM_Handle
#define LoggerSM_Module ti_uia_runtime_LoggerSM_Module
#define LoggerSM_Object ti_uia_runtime_LoggerSM_Object
#define LoggerSM_Struct ti_uia_runtime_LoggerSM_Struct
#define LoggerSM_VERSION ti_uia_runtime_LoggerSM_VERSION
#define LoggerSM_Module_State ti_uia_runtime_LoggerSM_Module_State
#define LoggerSM_SharedObj ti_uia_runtime_LoggerSM_SharedObj
#define LoggerSM_Instance_State ti_uia_runtime_LoggerSM_Instance_State
#define LoggerSM_filterByLevel ti_uia_runtime_LoggerSM_filterByLevel
#define LoggerSM_E_badLevel ti_uia_runtime_LoggerSM_E_badLevel
#define LoggerSM_isTimestampEnabled ti_uia_runtime_LoggerSM_isTimestampEnabled
#define LoggerSM_decode ti_uia_runtime_LoggerSM_decode
#define LoggerSM_overwrite ti_uia_runtime_LoggerSM_overwrite
#define LoggerSM_level1Mask ti_uia_runtime_LoggerSM_level1Mask
#define LoggerSM_level2Mask ti_uia_runtime_LoggerSM_level2Mask
#define LoggerSM_level3Mask ti_uia_runtime_LoggerSM_level3Mask
#define LoggerSM_level4Mask ti_uia_runtime_LoggerSM_level4Mask
#define LoggerSM_numPartitions ti_uia_runtime_LoggerSM_numPartitions
#define LoggerSM_sharedMemorySize ti_uia_runtime_LoggerSM_sharedMemorySize
#define LoggerSM_Params ti_uia_runtime_LoggerSM_Params
#define LoggerSM_setPartitionId ti_uia_runtime_LoggerSM_setPartitionId
#define LoggerSM_setSharedMemory ti_uia_runtime_LoggerSM_setSharedMemory
#define LoggerSM_enable ti_uia_runtime_LoggerSM_enable
#define LoggerSM_disable ti_uia_runtime_LoggerSM_disable
#define LoggerSM_write0 ti_uia_runtime_LoggerSM_write0
#define LoggerSM_write1 ti_uia_runtime_LoggerSM_write1
#define LoggerSM_write2 ti_uia_runtime_LoggerSM_write2
#define LoggerSM_write4 ti_uia_runtime_LoggerSM_write4
#define LoggerSM_write8 ti_uia_runtime_LoggerSM_write8
#define LoggerSM_setFilterLevel ti_uia_runtime_LoggerSM_setFilterLevel
#define LoggerSM_getFilterLevel ti_uia_runtime_LoggerSM_getFilterLevel
#define LoggerSM_Module_name ti_uia_runtime_LoggerSM_Module_name
#define LoggerSM_Module_id ti_uia_runtime_LoggerSM_Module_id
#define LoggerSM_Module_startup ti_uia_runtime_LoggerSM_Module_startup
#define LoggerSM_Module_startupDone ti_uia_runtime_LoggerSM_Module_startupDone
#define LoggerSM_Module_hasMask ti_uia_runtime_LoggerSM_Module_hasMask
#define LoggerSM_Module_getMask ti_uia_runtime_LoggerSM_Module_getMask
#define LoggerSM_Module_setMask ti_uia_runtime_LoggerSM_Module_setMask
#define LoggerSM_Object_heap ti_uia_runtime_LoggerSM_Object_heap
#define LoggerSM_Module_heap ti_uia_runtime_LoggerSM_Module_heap
#define LoggerSM_construct ti_uia_runtime_LoggerSM_construct
#define LoggerSM_create ti_uia_runtime_LoggerSM_create
#define LoggerSM_handle ti_uia_runtime_LoggerSM_handle
#define LoggerSM_struct ti_uia_runtime_LoggerSM_struct
#define LoggerSM_Handle_label ti_uia_runtime_LoggerSM_Handle_label
#define LoggerSM_Handle_name ti_uia_runtime_LoggerSM_Handle_name
#define LoggerSM_Instance_init ti_uia_runtime_LoggerSM_Instance_init
#define LoggerSM_Object_count ti_uia_runtime_LoggerSM_Object_count
#define LoggerSM_Object_get ti_uia_runtime_LoggerSM_Object_get
#define LoggerSM_Object_first ti_uia_runtime_LoggerSM_Object_first
#define LoggerSM_Object_next ti_uia_runtime_LoggerSM_Object_next
#define LoggerSM_Object_sizeof ti_uia_runtime_LoggerSM_Object_sizeof
#define LoggerSM_Params_copy ti_uia_runtime_LoggerSM_Params_copy
#define LoggerSM_Params_init ti_uia_runtime_LoggerSM_Params_init
#define LoggerSM_delete ti_uia_runtime_LoggerSM_delete
#define LoggerSM_destruct ti_uia_runtime_LoggerSM_destruct
#define LoggerSM_Module_upCast ti_uia_runtime_LoggerSM_Module_upCast
#define LoggerSM_Module_to_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Module_to_xdc_runtime_IFilterLogger
#define LoggerSM_Handle_upCast ti_uia_runtime_LoggerSM_Handle_upCast
#define LoggerSM_Handle_to_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Handle_to_xdc_runtime_IFilterLogger
#define LoggerSM_Handle_downCast ti_uia_runtime_LoggerSM_Handle_downCast
#define LoggerSM_Handle_from_xdc_runtime_IFilterLogger ti_uia_runtime_LoggerSM_Handle_from_xdc_runtime_IFilterLogger
#define LoggerSM_Module_upCast2 ti_uia_runtime_LoggerSM_Module_upCast2
#define LoggerSM_Module_to_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Module_to_xdc_runtime_ILogger
#define LoggerSM_Handle_upCast2 ti_uia_runtime_LoggerSM_Handle_upCast2
#define LoggerSM_Handle_to_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Handle_to_xdc_runtime_ILogger
#define LoggerSM_Handle_downCast2 ti_uia_runtime_LoggerSM_Handle_downCast2
#define LoggerSM_Handle_from_xdc_runtime_ILogger ti_uia_runtime_LoggerSM_Handle_from_xdc_runtime_ILogger

#endif /* ti_uia_runtime_LoggerSM__localnames__done */
#endif
