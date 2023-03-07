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
 *     CREATE ARGS
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

#ifndef ti_sysbios_xdcruntime_ThreadSupport__include
#define ti_sysbios_xdcruntime_ThreadSupport__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_xdcruntime_ThreadSupport__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_xdcruntime_ThreadSupport___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/xdcruntime/package/package.defs.h>

#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/knl/IThreadSupport.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Priority */
typedef xdc_runtime_knl_IThreadSupport_Priority ti_sysbios_xdcruntime_ThreadSupport_Priority;

/* INVALID_OS_PRIORITY */
#define ti_sysbios_xdcruntime_ThreadSupport_INVALID_OS_PRIORITY (0)

/* GETPRI_FAILED */
#define ti_sysbios_xdcruntime_ThreadSupport_GETPRI_FAILED (-2)

/* CompStatus */
typedef xdc_runtime_knl_IThreadSupport_CompStatus ti_sysbios_xdcruntime_ThreadSupport_CompStatus;

/* RunFxn */
typedef xdc_runtime_knl_IThreadSupport_RunFxn ti_sysbios_xdcruntime_ThreadSupport_RunFxn;

/* Stat */
typedef xdc_runtime_knl_IThreadSupport_Stat ti_sysbios_xdcruntime_ThreadSupport_Stat;

/* Priority_INVALID */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_INVALID xdc_runtime_knl_IThreadSupport_Priority_INVALID

/* Priority_LOWEST */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_LOWEST xdc_runtime_knl_IThreadSupport_Priority_LOWEST

/* Priority_BELOW_NORMAL */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_BELOW_NORMAL xdc_runtime_knl_IThreadSupport_Priority_BELOW_NORMAL

/* Priority_NORMAL */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_NORMAL xdc_runtime_knl_IThreadSupport_Priority_NORMAL

/* Priority_ABOVE_NORMAL */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_ABOVE_NORMAL xdc_runtime_knl_IThreadSupport_Priority_ABOVE_NORMAL

/* Priority_HIGHEST */
#define ti_sysbios_xdcruntime_ThreadSupport_Priority_HIGHEST xdc_runtime_knl_IThreadSupport_Priority_HIGHEST

/* CompStatus_ERROR */
#define ti_sysbios_xdcruntime_ThreadSupport_CompStatus_ERROR xdc_runtime_knl_IThreadSupport_CompStatus_ERROR

/* CompStatus_LOWER */
#define ti_sysbios_xdcruntime_ThreadSupport_CompStatus_LOWER xdc_runtime_knl_IThreadSupport_CompStatus_LOWER

/* CompStatus_EQUAL */
#define ti_sysbios_xdcruntime_ThreadSupport_CompStatus_EQUAL xdc_runtime_knl_IThreadSupport_CompStatus_EQUAL

/* CompStatus_HIGHER */
#define ti_sysbios_xdcruntime_ThreadSupport_CompStatus_HIGHER xdc_runtime_knl_IThreadSupport_CompStatus_HIGHER


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_xdcruntime_ThreadSupport_Args__create {
    xdc_runtime_knl_IThreadSupport_RunFxn fxn;
} ti_sysbios_xdcruntime_ThreadSupport_Args__create;


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* PRI_FAILURE */
#define ti_sysbios_xdcruntime_ThreadSupport_PRI_FAILURE (1)

/* TASK_FAILURE */
#define ti_sysbios_xdcruntime_ThreadSupport_TASK_FAILURE (2)


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled (ti_sysbios_xdcruntime_ThreadSupport_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded (ti_sysbios_xdcruntime_ThreadSupport_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask (ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj (ti_sysbios_xdcruntime_ThreadSupport_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms (ti_sysbios_xdcruntime_ThreadSupport_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_xdcruntime_ThreadSupport_Module__id;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__id ti_sysbios_xdcruntime_ThreadSupport_Module__id__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__id__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__id__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__id*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__id__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__id (ti_sysbios_xdcruntime_ThreadSupport_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0 ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0 (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1 ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1 (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2 ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2 (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4 ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4 (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8 ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8 (ti_sysbios_xdcruntime_ThreadSupport_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_xdcruntime_ThreadSupport_Object__count;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Object__count ti_sysbios_xdcruntime_ThreadSupport_Object__count__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Object__count__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Object__count__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Object__count*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Object__count__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Object__count (ti_sysbios_xdcruntime_ThreadSupport_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_xdcruntime_ThreadSupport_Object__heap;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Object__heap ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Object__heap__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Object__heap*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Object__heap (ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof (ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_xdcruntime_ThreadSupport_Object__table;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_Object__table ti_sysbios_xdcruntime_ThreadSupport_Object__table__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_Object__table__CR
#define ti_sysbios_xdcruntime_ThreadSupport_Object__table__C (*((CT__ti_sysbios_xdcruntime_ThreadSupport_Object__table*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Object__table__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Object__table (ti_sysbios_xdcruntime_ThreadSupport_Object__table__C)
#endif

/* E_priority */
typedef xdc_runtime_Error_Id CT__ti_sysbios_xdcruntime_ThreadSupport_E_priority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_E_priority ti_sysbios_xdcruntime_ThreadSupport_E_priority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_E_priority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_E_priority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_E_priority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_E_priority__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_E_priority (ti_sysbios_xdcruntime_ThreadSupport_E_priority__C)
#endif

/* L_start */
typedef xdc_runtime_Log_Event CT__ti_sysbios_xdcruntime_ThreadSupport_L_start;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_L_start ti_sysbios_xdcruntime_ThreadSupport_L_start__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_L_start__CR
#define ti_sysbios_xdcruntime_ThreadSupport_L_start (*((CT__ti_sysbios_xdcruntime_ThreadSupport_L_start*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_L_start__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_L_start (ti_sysbios_xdcruntime_ThreadSupport_L_start__C)
#endif

/* L_finish */
typedef xdc_runtime_Log_Event CT__ti_sysbios_xdcruntime_ThreadSupport_L_finish;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_L_finish ti_sysbios_xdcruntime_ThreadSupport_L_finish__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_L_finish__CR
#define ti_sysbios_xdcruntime_ThreadSupport_L_finish (*((CT__ti_sysbios_xdcruntime_ThreadSupport_L_finish*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_L_finish__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_L_finish (ti_sysbios_xdcruntime_ThreadSupport_L_finish__C)
#endif

/* L_join */
typedef xdc_runtime_Log_Event CT__ti_sysbios_xdcruntime_ThreadSupport_L_join;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_L_join ti_sysbios_xdcruntime_ThreadSupport_L_join__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_L_join__CR
#define ti_sysbios_xdcruntime_ThreadSupport_L_join (*((CT__ti_sysbios_xdcruntime_ThreadSupport_L_join*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_L_join__C_offset)))
#else
#define ti_sysbios_xdcruntime_ThreadSupport_L_join (ti_sysbios_xdcruntime_ThreadSupport_L_join__C)
#endif

/* lowestPriority */
typedef xdc_UInt CT__ti_sysbios_xdcruntime_ThreadSupport_lowestPriority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_lowestPriority ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_lowestPriority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_lowestPriority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__C_offset)))
#else
#ifdef ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__D
#define ti_sysbios_xdcruntime_ThreadSupport_lowestPriority (ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__D)
#else
#define ti_sysbios_xdcruntime_ThreadSupport_lowestPriority (ti_sysbios_xdcruntime_ThreadSupport_lowestPriority__C)
#endif
#endif

/* belowNormalPriority */
typedef xdc_UInt CT__ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__C_offset)))
#else
#ifdef ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__D
#define ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority (ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__D)
#else
#define ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority (ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority__C)
#endif
#endif

/* normalPriority */
typedef xdc_UInt CT__ti_sysbios_xdcruntime_ThreadSupport_normalPriority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_normalPriority ti_sysbios_xdcruntime_ThreadSupport_normalPriority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_normalPriority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_normalPriority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_normalPriority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_normalPriority__C_offset)))
#else
#ifdef ti_sysbios_xdcruntime_ThreadSupport_normalPriority__D
#define ti_sysbios_xdcruntime_ThreadSupport_normalPriority (ti_sysbios_xdcruntime_ThreadSupport_normalPriority__D)
#else
#define ti_sysbios_xdcruntime_ThreadSupport_normalPriority (ti_sysbios_xdcruntime_ThreadSupport_normalPriority__C)
#endif
#endif

/* aboveNormalPriority */
typedef xdc_UInt CT__ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__C_offset)))
#else
#ifdef ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__D
#define ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority (ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__D)
#else
#define ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority (ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority__C)
#endif
#endif

/* highestPriority */
typedef xdc_UInt CT__ti_sysbios_xdcruntime_ThreadSupport_highestPriority;
__extern __FAR__ const CT__ti_sysbios_xdcruntime_ThreadSupport_highestPriority ti_sysbios_xdcruntime_ThreadSupport_highestPriority__C;
#ifdef ti_sysbios_xdcruntime_ThreadSupport_highestPriority__CR
#define ti_sysbios_xdcruntime_ThreadSupport_highestPriority (*((CT__ti_sysbios_xdcruntime_ThreadSupport_highestPriority*)(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_highestPriority__C_offset)))
#else
#ifdef ti_sysbios_xdcruntime_ThreadSupport_highestPriority__D
#define ti_sysbios_xdcruntime_ThreadSupport_highestPriority (ti_sysbios_xdcruntime_ThreadSupport_highestPriority__D)
#else
#define ti_sysbios_xdcruntime_ThreadSupport_highestPriority (ti_sysbios_xdcruntime_ThreadSupport_highestPriority__C)
#endif
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_xdcruntime_ThreadSupport_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_IArg arg;
    xdc_runtime_knl_IThreadSupport_Priority priority;
    xdc_Int osPriority;
    xdc_SizeT stackSize;
    xdc_Ptr tls;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_xdcruntime_ThreadSupport_Struct {
    const ti_sysbios_xdcruntime_ThreadSupport_Fxns__ *__fxns;
    ti_sysbios_knl_Task_Handle f0;
    xdc_Ptr f1;
    ti_sysbios_xdcruntime_ThreadSupport_RunFxn f2;
    xdc_IArg f3;
    ti_sysbios_knl_Semaphore_Struct f4;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_xdcruntime_ThreadSupport_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_knl_IThreadSupport_Handle (*self)(xdc_runtime_Error_Block* eb);
    xdc_Bool (*start)(xdc_runtime_Error_Block* eb);
    xdc_Bool (*yield)(xdc_runtime_Error_Block* eb);
    xdc_Int (*compareOsPriorities)(xdc_Int p1, xdc_Int p2, xdc_runtime_Error_Block* eb);
    xdc_Bool (*sleep)(xdc_UInt timeout, xdc_runtime_Error_Block* eb);
    xdc_Bool (*join)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_Error_Block* eb);
    xdc_runtime_knl_IThreadSupport_Priority (*getPriority)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_Error_Block* eb);
    xdc_Bool (*setPriority)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_knl_IThreadSupport_Priority newPri, xdc_runtime_Error_Block* eb);
    xdc_Int (*getOsPriority)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_Error_Block* eb);
    xdc_Bool (*setOsPriority)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_Int newPri, xdc_runtime_Error_Block* eb);
    xdc_Ptr (*getOsHandle)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst);
    xdc_Ptr (*getTls)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst);
    xdc_Void (*setTls)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_Ptr tls);
    xdc_Bool (*stat)(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_knl_IThreadSupport_Stat* buf, xdc_runtime_Error_Block* eb);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_xdcruntime_ThreadSupport_Fxns__ ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__C;
#else
#define ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_xdcruntime_ThreadSupport_Module_startup( state ) (-1)

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Instance_init__E, "ti_sysbios_xdcruntime_ThreadSupport_Instance_init")
__extern xdc_Int ti_sysbios_xdcruntime_ThreadSupport_Instance_init__E(ti_sysbios_xdcruntime_ThreadSupport_Object *obj, xdc_runtime_knl_IThreadSupport_RunFxn fxn, const ti_sysbios_xdcruntime_ThreadSupport_Params *prms, xdc_runtime_Error_Block *eb);

/* Instance_finalize__E */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Instance_finalize__E, "ti_sysbios_xdcruntime_ThreadSupport_Instance_finalize")
__extern void ti_sysbios_xdcruntime_ThreadSupport_Instance_finalize__E(ti_sysbios_xdcruntime_ThreadSupport_Object *obj, int ec);

/* create */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_create, "ti_sysbios_xdcruntime_ThreadSupport_create")
__extern ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_create( xdc_runtime_knl_IThreadSupport_RunFxn fxn, const ti_sysbios_xdcruntime_ThreadSupport_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_construct, "ti_sysbios_xdcruntime_ThreadSupport_construct")
__extern void ti_sysbios_xdcruntime_ThreadSupport_construct(ti_sysbios_xdcruntime_ThreadSupport_Struct *obj, xdc_runtime_knl_IThreadSupport_RunFxn fxn, const ti_sysbios_xdcruntime_ThreadSupport_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_delete, "ti_sysbios_xdcruntime_ThreadSupport_delete")
__extern void ti_sysbios_xdcruntime_ThreadSupport_delete(ti_sysbios_xdcruntime_ThreadSupport_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_destruct, "ti_sysbios_xdcruntime_ThreadSupport_destruct")
__extern void ti_sysbios_xdcruntime_ThreadSupport_destruct(ti_sysbios_xdcruntime_ThreadSupport_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Handle__label__S, "ti_sysbios_xdcruntime_ThreadSupport_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_xdcruntime_ThreadSupport_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Module__startupDone__S, "ti_sysbios_xdcruntime_ThreadSupport_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Object__create__S, "ti_sysbios_xdcruntime_ThreadSupport_Object__create__S")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Object__delete__S, "ti_sysbios_xdcruntime_ThreadSupport_Object__delete__S")
__extern xdc_Void ti_sysbios_xdcruntime_ThreadSupport_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Object__get__S, "ti_sysbios_xdcruntime_ThreadSupport_Object__get__S")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Object__first__S, "ti_sysbios_xdcruntime_ThreadSupport_Object__first__S")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Object__next__S, "ti_sysbios_xdcruntime_ThreadSupport_Object__next__S")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_Params__init__S, "ti_sysbios_xdcruntime_ThreadSupport_Params__init__S")
__extern xdc_Void ti_sysbios_xdcruntime_ThreadSupport_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* self__E */
#define ti_sysbios_xdcruntime_ThreadSupport_self ti_sysbios_xdcruntime_ThreadSupport_self__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_self__E, "ti_sysbios_xdcruntime_ThreadSupport_self")
__extern xdc_runtime_knl_IThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_self__E( xdc_runtime_Error_Block *eb);

/* start__E */
#define ti_sysbios_xdcruntime_ThreadSupport_start ti_sysbios_xdcruntime_ThreadSupport_start__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_start__E, "ti_sysbios_xdcruntime_ThreadSupport_start")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_start__E( xdc_runtime_Error_Block *eb);

/* yield__E */
#define ti_sysbios_xdcruntime_ThreadSupport_yield ti_sysbios_xdcruntime_ThreadSupport_yield__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_yield__E, "ti_sysbios_xdcruntime_ThreadSupport_yield")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_yield__E( xdc_runtime_Error_Block *eb);

/* compareOsPriorities__E */
#define ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities__E, "ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities")
__extern xdc_Int ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities__E( xdc_Int p1, xdc_Int p2, xdc_runtime_Error_Block *eb);

/* sleep__E */
#define ti_sysbios_xdcruntime_ThreadSupport_sleep ti_sysbios_xdcruntime_ThreadSupport_sleep__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_sleep__E, "ti_sysbios_xdcruntime_ThreadSupport_sleep")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_sleep__E( xdc_UInt timeout, xdc_runtime_Error_Block *eb);

/* join__E */
#define ti_sysbios_xdcruntime_ThreadSupport_join ti_sysbios_xdcruntime_ThreadSupport_join__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_join__E, "ti_sysbios_xdcruntime_ThreadSupport_join")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_join__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_runtime_Error_Block *eb);

/* getPriority__E */
#define ti_sysbios_xdcruntime_ThreadSupport_getPriority ti_sysbios_xdcruntime_ThreadSupport_getPriority__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_getPriority__E, "ti_sysbios_xdcruntime_ThreadSupport_getPriority")
__extern xdc_runtime_knl_IThreadSupport_Priority ti_sysbios_xdcruntime_ThreadSupport_getPriority__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_runtime_Error_Block *eb);

/* setPriority__E */
#define ti_sysbios_xdcruntime_ThreadSupport_setPriority ti_sysbios_xdcruntime_ThreadSupport_setPriority__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_setPriority__E, "ti_sysbios_xdcruntime_ThreadSupport_setPriority")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_setPriority__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_runtime_knl_IThreadSupport_Priority newPri, xdc_runtime_Error_Block *eb);

/* getOsPriority__E */
#define ti_sysbios_xdcruntime_ThreadSupport_getOsPriority ti_sysbios_xdcruntime_ThreadSupport_getOsPriority__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_getOsPriority__E, "ti_sysbios_xdcruntime_ThreadSupport_getOsPriority")
__extern xdc_Int ti_sysbios_xdcruntime_ThreadSupport_getOsPriority__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_runtime_Error_Block *eb);

/* setOsPriority__E */
#define ti_sysbios_xdcruntime_ThreadSupport_setOsPriority ti_sysbios_xdcruntime_ThreadSupport_setOsPriority__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_setOsPriority__E, "ti_sysbios_xdcruntime_ThreadSupport_setOsPriority")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_setOsPriority__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_Int newPri, xdc_runtime_Error_Block *eb);

/* getOsHandle__E */
#define ti_sysbios_xdcruntime_ThreadSupport_getOsHandle ti_sysbios_xdcruntime_ThreadSupport_getOsHandle__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_getOsHandle__E, "ti_sysbios_xdcruntime_ThreadSupport_getOsHandle")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_getOsHandle__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp);

/* getTls__E */
#define ti_sysbios_xdcruntime_ThreadSupport_getTls ti_sysbios_xdcruntime_ThreadSupport_getTls__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_getTls__E, "ti_sysbios_xdcruntime_ThreadSupport_getTls")
__extern xdc_Ptr ti_sysbios_xdcruntime_ThreadSupport_getTls__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp);

/* setTls__E */
#define ti_sysbios_xdcruntime_ThreadSupport_setTls ti_sysbios_xdcruntime_ThreadSupport_setTls__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_setTls__E, "ti_sysbios_xdcruntime_ThreadSupport_setTls")
__extern xdc_Void ti_sysbios_xdcruntime_ThreadSupport_setTls__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_Ptr tls);

/* stat__E */
#define ti_sysbios_xdcruntime_ThreadSupport_stat ti_sysbios_xdcruntime_ThreadSupport_stat__E
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_stat__E, "ti_sysbios_xdcruntime_ThreadSupport_stat")
__extern xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_stat__E( ti_sysbios_xdcruntime_ThreadSupport_Handle instp, xdc_runtime_knl_IThreadSupport_Stat *buf, xdc_runtime_Error_Block *eb);

/* runStub__I */
#define ti_sysbios_xdcruntime_ThreadSupport_runStub ti_sysbios_xdcruntime_ThreadSupport_runStub__I
xdc__CODESECT(ti_sysbios_xdcruntime_ThreadSupport_runStub__I, "ti_sysbios_xdcruntime_ThreadSupport_runStub")
__extern xdc_Void ti_sysbios_xdcruntime_ThreadSupport_runStub__I( xdc_UArg arg1, xdc_UArg arg2);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_knl_IThreadSupport_Module ti_sysbios_xdcruntime_ThreadSupport_Module_upCast(void);
static inline xdc_runtime_knl_IThreadSupport_Module ti_sysbios_xdcruntime_ThreadSupport_Module_upCast(void)
{
    return (xdc_runtime_knl_IThreadSupport_Module)&ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__C;
}

/* Module_to_xdc_runtime_knl_IThreadSupport */
#define ti_sysbios_xdcruntime_ThreadSupport_Module_to_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_knl_IThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Handle_upCast(ti_sysbios_xdcruntime_ThreadSupport_Handle i);
static inline xdc_runtime_knl_IThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Handle_upCast(ti_sysbios_xdcruntime_ThreadSupport_Handle i)
{
    return (xdc_runtime_knl_IThreadSupport_Handle)i;
}

/* Handle_to_xdc_runtime_knl_IThreadSupport */
#define ti_sysbios_xdcruntime_ThreadSupport_Handle_to_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Handle_downCast(xdc_runtime_knl_IThreadSupport_Handle i);
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Handle_downCast(xdc_runtime_knl_IThreadSupport_Handle i)
{
    xdc_runtime_knl_IThreadSupport_Handle i2 = (xdc_runtime_knl_IThreadSupport_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_sysbios_xdcruntime_ThreadSupport_Module__FXNS__C) ? (ti_sysbios_xdcruntime_ThreadSupport_Handle)i : (ti_sysbios_xdcruntime_ThreadSupport_Handle)NULL;
}

/* Handle_from_xdc_runtime_knl_IThreadSupport */
#define ti_sysbios_xdcruntime_ThreadSupport_Handle_from_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_xdcruntime_ThreadSupport_Module_startupDone() ti_sysbios_xdcruntime_ThreadSupport_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_xdcruntime_ThreadSupport_Object_heap() ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C

/* Module_heap */
#define ti_sysbios_xdcruntime_ThreadSupport_Module_heap() ti_sysbios_xdcruntime_ThreadSupport_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_xdcruntime_ThreadSupport_Module__id ti_sysbios_xdcruntime_ThreadSupport_Module_id(void);
static inline CT__ti_sysbios_xdcruntime_ThreadSupport_Module__id ti_sysbios_xdcruntime_ThreadSupport_Module_id( void ) 
{
    return ti_sysbios_xdcruntime_ThreadSupport_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_xdcruntime_ThreadSupport_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C != (CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_xdcruntime_ThreadSupport_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_xdcruntime_ThreadSupport_Module_getMask(void)
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C != (CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask)NULL) ? *ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_xdcruntime_ThreadSupport_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_xdcruntime_ThreadSupport_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C != (CT__ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask)NULL) {
        *ti_sysbios_xdcruntime_ThreadSupport_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_xdcruntime_ThreadSupport_Params_init(ti_sysbios_xdcruntime_ThreadSupport_Params *prms);
static inline void ti_sysbios_xdcruntime_ThreadSupport_Params_init( ti_sysbios_xdcruntime_ThreadSupport_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_xdcruntime_ThreadSupport_Params__init__S(prms, NULL, sizeof(ti_sysbios_xdcruntime_ThreadSupport_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_xdcruntime_ThreadSupport_Params_copy(ti_sysbios_xdcruntime_ThreadSupport_Params *dst, const ti_sysbios_xdcruntime_ThreadSupport_Params *src);
static inline void ti_sysbios_xdcruntime_ThreadSupport_Params_copy(ti_sysbios_xdcruntime_ThreadSupport_Params *dst, const ti_sysbios_xdcruntime_ThreadSupport_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_xdcruntime_ThreadSupport_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_xdcruntime_ThreadSupport_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_xdcruntime_ThreadSupport_Object_count() ti_sysbios_xdcruntime_ThreadSupport_Object__count__C

/* Object_sizeof */
#define ti_sysbios_xdcruntime_ThreadSupport_Object_sizeof() ti_sysbios_xdcruntime_ThreadSupport_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_get(ti_sysbios_xdcruntime_ThreadSupport_Object *oarr, int i);
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_get(ti_sysbios_xdcruntime_ThreadSupport_Object *oarr, int i) 
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Handle)ti_sysbios_xdcruntime_ThreadSupport_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_first(void);
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_first(void)
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Handle)ti_sysbios_xdcruntime_ThreadSupport_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_next(ti_sysbios_xdcruntime_ThreadSupport_Object *obj);
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Object_next(ti_sysbios_xdcruntime_ThreadSupport_Object *obj)
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Handle)ti_sysbios_xdcruntime_ThreadSupport_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_xdcruntime_ThreadSupport_Handle_label(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_xdcruntime_ThreadSupport_Handle_label(ti_sysbios_xdcruntime_ThreadSupport_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_xdcruntime_ThreadSupport_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_xdcruntime_ThreadSupport_Handle_name(ti_sysbios_xdcruntime_ThreadSupport_Handle inst);
static inline xdc_String ti_sysbios_xdcruntime_ThreadSupport_Handle_name(ti_sysbios_xdcruntime_ThreadSupport_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_xdcruntime_ThreadSupport_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_handle(ti_sysbios_xdcruntime_ThreadSupport_Struct *str);
static inline ti_sysbios_xdcruntime_ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_handle(ti_sysbios_xdcruntime_ThreadSupport_Struct *str)
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Handle)str;
}

/* struct */
static inline ti_sysbios_xdcruntime_ThreadSupport_Struct *ti_sysbios_xdcruntime_ThreadSupport_struct(ti_sysbios_xdcruntime_ThreadSupport_Handle inst);
static inline ti_sysbios_xdcruntime_ThreadSupport_Struct *ti_sysbios_xdcruntime_ThreadSupport_struct(ti_sysbios_xdcruntime_ThreadSupport_Handle inst)
{
    return (ti_sysbios_xdcruntime_ThreadSupport_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_xdcruntime_ThreadSupport__top__
#undef __nested__
#endif

#endif /* ti_sysbios_xdcruntime_ThreadSupport__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_xdcruntime_ThreadSupport__internalaccess))

#ifndef ti_sysbios_xdcruntime_ThreadSupport__include_state
#define ti_sysbios_xdcruntime_ThreadSupport__include_state

/* Object */
struct ti_sysbios_xdcruntime_ThreadSupport_Object {
    const ti_sysbios_xdcruntime_ThreadSupport_Fxns__ *__fxns;
    ti_sysbios_knl_Task_Handle task;
    xdc_Ptr tls;
    ti_sysbios_xdcruntime_ThreadSupport_RunFxn startFxn;
    xdc_IArg startFxnArg;
    char dummy;
};

/* Instance_State_join_sem */
#ifndef ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem__OR
__extern __FAR__ const xdc_SizeT ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem__O;
#endif
static inline ti_sysbios_knl_Semaphore_Handle ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem(ti_sysbios_xdcruntime_ThreadSupport_Object *obj);
static inline ti_sysbios_knl_Semaphore_Handle ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem(ti_sysbios_xdcruntime_ThreadSupport_Object *obj)
{
    return (ti_sysbios_knl_Semaphore_Handle)(((char*)obj) + ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem__O);
}

#endif /* ti_sysbios_xdcruntime_ThreadSupport__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_xdcruntime_ThreadSupport__nolocalnames)

#ifndef ti_sysbios_xdcruntime_ThreadSupport__localnames__done
#define ti_sysbios_xdcruntime_ThreadSupport__localnames__done

/* module prefix */
#define ThreadSupport_Instance ti_sysbios_xdcruntime_ThreadSupport_Instance
#define ThreadSupport_Handle ti_sysbios_xdcruntime_ThreadSupport_Handle
#define ThreadSupport_Module ti_sysbios_xdcruntime_ThreadSupport_Module
#define ThreadSupport_Object ti_sysbios_xdcruntime_ThreadSupport_Object
#define ThreadSupport_Struct ti_sysbios_xdcruntime_ThreadSupport_Struct
#define ThreadSupport_Priority ti_sysbios_xdcruntime_ThreadSupport_Priority
#define ThreadSupport_INVALID_OS_PRIORITY ti_sysbios_xdcruntime_ThreadSupport_INVALID_OS_PRIORITY
#define ThreadSupport_GETPRI_FAILED ti_sysbios_xdcruntime_ThreadSupport_GETPRI_FAILED
#define ThreadSupport_CompStatus ti_sysbios_xdcruntime_ThreadSupport_CompStatus
#define ThreadSupport_RunFxn ti_sysbios_xdcruntime_ThreadSupport_RunFxn
#define ThreadSupport_Stat ti_sysbios_xdcruntime_ThreadSupport_Stat
#define ThreadSupport_PRI_FAILURE ti_sysbios_xdcruntime_ThreadSupport_PRI_FAILURE
#define ThreadSupport_TASK_FAILURE ti_sysbios_xdcruntime_ThreadSupport_TASK_FAILURE
#define ThreadSupport_Instance_State ti_sysbios_xdcruntime_ThreadSupport_Instance_State
#define ThreadSupport_Priority_INVALID ti_sysbios_xdcruntime_ThreadSupport_Priority_INVALID
#define ThreadSupport_Priority_LOWEST ti_sysbios_xdcruntime_ThreadSupport_Priority_LOWEST
#define ThreadSupport_Priority_BELOW_NORMAL ti_sysbios_xdcruntime_ThreadSupport_Priority_BELOW_NORMAL
#define ThreadSupport_Priority_NORMAL ti_sysbios_xdcruntime_ThreadSupport_Priority_NORMAL
#define ThreadSupport_Priority_ABOVE_NORMAL ti_sysbios_xdcruntime_ThreadSupport_Priority_ABOVE_NORMAL
#define ThreadSupport_Priority_HIGHEST ti_sysbios_xdcruntime_ThreadSupport_Priority_HIGHEST
#define ThreadSupport_CompStatus_ERROR ti_sysbios_xdcruntime_ThreadSupport_CompStatus_ERROR
#define ThreadSupport_CompStatus_LOWER ti_sysbios_xdcruntime_ThreadSupport_CompStatus_LOWER
#define ThreadSupport_CompStatus_EQUAL ti_sysbios_xdcruntime_ThreadSupport_CompStatus_EQUAL
#define ThreadSupport_CompStatus_HIGHER ti_sysbios_xdcruntime_ThreadSupport_CompStatus_HIGHER
#define ThreadSupport_E_priority ti_sysbios_xdcruntime_ThreadSupport_E_priority
#define ThreadSupport_L_start ti_sysbios_xdcruntime_ThreadSupport_L_start
#define ThreadSupport_L_finish ti_sysbios_xdcruntime_ThreadSupport_L_finish
#define ThreadSupport_L_join ti_sysbios_xdcruntime_ThreadSupport_L_join
#define ThreadSupport_lowestPriority ti_sysbios_xdcruntime_ThreadSupport_lowestPriority
#define ThreadSupport_belowNormalPriority ti_sysbios_xdcruntime_ThreadSupport_belowNormalPriority
#define ThreadSupport_normalPriority ti_sysbios_xdcruntime_ThreadSupport_normalPriority
#define ThreadSupport_aboveNormalPriority ti_sysbios_xdcruntime_ThreadSupport_aboveNormalPriority
#define ThreadSupport_highestPriority ti_sysbios_xdcruntime_ThreadSupport_highestPriority
#define ThreadSupport_Instance_State_join_sem ti_sysbios_xdcruntime_ThreadSupport_Instance_State_join_sem
#define ThreadSupport_Params ti_sysbios_xdcruntime_ThreadSupport_Params
#define ThreadSupport_self ti_sysbios_xdcruntime_ThreadSupport_self
#define ThreadSupport_start ti_sysbios_xdcruntime_ThreadSupport_start
#define ThreadSupport_yield ti_sysbios_xdcruntime_ThreadSupport_yield
#define ThreadSupport_compareOsPriorities ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities
#define ThreadSupport_sleep ti_sysbios_xdcruntime_ThreadSupport_sleep
#define ThreadSupport_join ti_sysbios_xdcruntime_ThreadSupport_join
#define ThreadSupport_getPriority ti_sysbios_xdcruntime_ThreadSupport_getPriority
#define ThreadSupport_setPriority ti_sysbios_xdcruntime_ThreadSupport_setPriority
#define ThreadSupport_getOsPriority ti_sysbios_xdcruntime_ThreadSupport_getOsPriority
#define ThreadSupport_setOsPriority ti_sysbios_xdcruntime_ThreadSupport_setOsPriority
#define ThreadSupport_getOsHandle ti_sysbios_xdcruntime_ThreadSupport_getOsHandle
#define ThreadSupport_getTls ti_sysbios_xdcruntime_ThreadSupport_getTls
#define ThreadSupport_setTls ti_sysbios_xdcruntime_ThreadSupport_setTls
#define ThreadSupport_stat ti_sysbios_xdcruntime_ThreadSupport_stat
#define ThreadSupport_Module_name ti_sysbios_xdcruntime_ThreadSupport_Module_name
#define ThreadSupport_Module_id ti_sysbios_xdcruntime_ThreadSupport_Module_id
#define ThreadSupport_Module_startup ti_sysbios_xdcruntime_ThreadSupport_Module_startup
#define ThreadSupport_Module_startupDone ti_sysbios_xdcruntime_ThreadSupport_Module_startupDone
#define ThreadSupport_Module_hasMask ti_sysbios_xdcruntime_ThreadSupport_Module_hasMask
#define ThreadSupport_Module_getMask ti_sysbios_xdcruntime_ThreadSupport_Module_getMask
#define ThreadSupport_Module_setMask ti_sysbios_xdcruntime_ThreadSupport_Module_setMask
#define ThreadSupport_Object_heap ti_sysbios_xdcruntime_ThreadSupport_Object_heap
#define ThreadSupport_Module_heap ti_sysbios_xdcruntime_ThreadSupport_Module_heap
#define ThreadSupport_construct ti_sysbios_xdcruntime_ThreadSupport_construct
#define ThreadSupport_create ti_sysbios_xdcruntime_ThreadSupport_create
#define ThreadSupport_handle ti_sysbios_xdcruntime_ThreadSupport_handle
#define ThreadSupport_struct ti_sysbios_xdcruntime_ThreadSupport_struct
#define ThreadSupport_Handle_label ti_sysbios_xdcruntime_ThreadSupport_Handle_label
#define ThreadSupport_Handle_name ti_sysbios_xdcruntime_ThreadSupport_Handle_name
#define ThreadSupport_Instance_init ti_sysbios_xdcruntime_ThreadSupport_Instance_init
#define ThreadSupport_Object_count ti_sysbios_xdcruntime_ThreadSupport_Object_count
#define ThreadSupport_Object_get ti_sysbios_xdcruntime_ThreadSupport_Object_get
#define ThreadSupport_Object_first ti_sysbios_xdcruntime_ThreadSupport_Object_first
#define ThreadSupport_Object_next ti_sysbios_xdcruntime_ThreadSupport_Object_next
#define ThreadSupport_Object_sizeof ti_sysbios_xdcruntime_ThreadSupport_Object_sizeof
#define ThreadSupport_Params_copy ti_sysbios_xdcruntime_ThreadSupport_Params_copy
#define ThreadSupport_Params_init ti_sysbios_xdcruntime_ThreadSupport_Params_init
#define ThreadSupport_Instance_finalize ti_sysbios_xdcruntime_ThreadSupport_Instance_finalize
#define ThreadSupport_delete ti_sysbios_xdcruntime_ThreadSupport_delete
#define ThreadSupport_destruct ti_sysbios_xdcruntime_ThreadSupport_destruct
#define ThreadSupport_Module_upCast ti_sysbios_xdcruntime_ThreadSupport_Module_upCast
#define ThreadSupport_Module_to_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Module_to_xdc_runtime_knl_IThreadSupport
#define ThreadSupport_Handle_upCast ti_sysbios_xdcruntime_ThreadSupport_Handle_upCast
#define ThreadSupport_Handle_to_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Handle_to_xdc_runtime_knl_IThreadSupport
#define ThreadSupport_Handle_downCast ti_sysbios_xdcruntime_ThreadSupport_Handle_downCast
#define ThreadSupport_Handle_from_xdc_runtime_knl_IThreadSupport ti_sysbios_xdcruntime_ThreadSupport_Handle_from_xdc_runtime_knl_IThreadSupport

#endif /* ti_sysbios_xdcruntime_ThreadSupport__localnames__done */
#endif
