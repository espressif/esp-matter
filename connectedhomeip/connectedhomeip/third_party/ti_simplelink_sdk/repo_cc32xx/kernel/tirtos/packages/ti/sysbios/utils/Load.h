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
 *     FUNCTION DECLARATIONS
 *     SYSTEM FUNCTIONS
 *
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_utils_Load__include
#define ti_sysbios_utils_Load__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_utils_Load__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_utils_Load___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/utils/package/package.defs.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/interfaces/IHwi.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_utils_Load_FuncPtr)(void);

/* Stat */
struct ti_sysbios_utils_Load_Stat {
    xdc_UInt32 threadTime;
    xdc_UInt32 totalTime;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* HookContext */
struct ti_sysbios_utils_Load_HookContext {
    ti_sysbios_knl_Queue_Elem qElem;
    xdc_UInt32 totalTimeElapsed;
    xdc_UInt32 totalTime;
    xdc_UInt32 nextTotalTime;
    xdc_UInt32 timeOfLastUpdate;
    xdc_Ptr threadHandle;
};

/* Module_State */
typedef xdc_UInt32 __T1_ti_sysbios_utils_Load_Module_State__taskStartTime;
typedef xdc_UInt32 *ARRAY1_ti_sysbios_utils_Load_Module_State__taskStartTime;
typedef const xdc_UInt32 *CARRAY1_ti_sysbios_utils_Load_Module_State__taskStartTime;
typedef ARRAY1_ti_sysbios_utils_Load_Module_State__taskStartTime __TA_ti_sysbios_utils_Load_Module_State__taskStartTime;
typedef ti_sysbios_knl_Task_Handle __T1_ti_sysbios_utils_Load_Module_State__runningTask;
typedef ti_sysbios_knl_Task_Handle *ARRAY1_ti_sysbios_utils_Load_Module_State__runningTask;
typedef const ti_sysbios_knl_Task_Handle *CARRAY1_ti_sysbios_utils_Load_Module_State__runningTask;
typedef ARRAY1_ti_sysbios_utils_Load_Module_State__runningTask __TA_ti_sysbios_utils_Load_Module_State__runningTask;
typedef ti_sysbios_utils_Load_HookContext __T1_ti_sysbios_utils_Load_Module_State__taskEnv;
typedef ti_sysbios_utils_Load_HookContext *ARRAY1_ti_sysbios_utils_Load_Module_State__taskEnv;
typedef const ti_sysbios_utils_Load_HookContext *CARRAY1_ti_sysbios_utils_Load_Module_State__taskEnv;
typedef ARRAY1_ti_sysbios_utils_Load_Module_State__taskEnv __TA_ti_sysbios_utils_Load_Module_State__taskEnv;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_utils_Load_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__diagsEnabled ti_sysbios_utils_Load_Module__diagsEnabled__C;
#ifdef ti_sysbios_utils_Load_Module__diagsEnabled__CR
#define ti_sysbios_utils_Load_Module__diagsEnabled__C (*((CT__ti_sysbios_utils_Load_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__diagsEnabled (ti_sysbios_utils_Load_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_utils_Load_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__diagsIncluded ti_sysbios_utils_Load_Module__diagsIncluded__C;
#ifdef ti_sysbios_utils_Load_Module__diagsIncluded__CR
#define ti_sysbios_utils_Load_Module__diagsIncluded__C (*((CT__ti_sysbios_utils_Load_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__diagsIncluded (ti_sysbios_utils_Load_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_utils_Load_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__diagsMask ti_sysbios_utils_Load_Module__diagsMask__C;
#ifdef ti_sysbios_utils_Load_Module__diagsMask__CR
#define ti_sysbios_utils_Load_Module__diagsMask__C (*((CT__ti_sysbios_utils_Load_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__diagsMask (ti_sysbios_utils_Load_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_utils_Load_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__gateObj ti_sysbios_utils_Load_Module__gateObj__C;
#ifdef ti_sysbios_utils_Load_Module__gateObj__CR
#define ti_sysbios_utils_Load_Module__gateObj__C (*((CT__ti_sysbios_utils_Load_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__gateObj__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__gateObj (ti_sysbios_utils_Load_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_utils_Load_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__gatePrms ti_sysbios_utils_Load_Module__gatePrms__C;
#ifdef ti_sysbios_utils_Load_Module__gatePrms__CR
#define ti_sysbios_utils_Load_Module__gatePrms__C (*((CT__ti_sysbios_utils_Load_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__gatePrms (ti_sysbios_utils_Load_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_utils_Load_Module__id;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__id ti_sysbios_utils_Load_Module__id__C;
#ifdef ti_sysbios_utils_Load_Module__id__CR
#define ti_sysbios_utils_Load_Module__id__C (*((CT__ti_sysbios_utils_Load_Module__id*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__id__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__id (ti_sysbios_utils_Load_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_utils_Load_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerDefined ti_sysbios_utils_Load_Module__loggerDefined__C;
#ifdef ti_sysbios_utils_Load_Module__loggerDefined__CR
#define ti_sysbios_utils_Load_Module__loggerDefined__C (*((CT__ti_sysbios_utils_Load_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerDefined (ti_sysbios_utils_Load_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_utils_Load_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerObj ti_sysbios_utils_Load_Module__loggerObj__C;
#ifdef ti_sysbios_utils_Load_Module__loggerObj__CR
#define ti_sysbios_utils_Load_Module__loggerObj__C (*((CT__ti_sysbios_utils_Load_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerObj (ti_sysbios_utils_Load_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_utils_Load_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn0 ti_sysbios_utils_Load_Module__loggerFxn0__C;
#ifdef ti_sysbios_utils_Load_Module__loggerFxn0__CR
#define ti_sysbios_utils_Load_Module__loggerFxn0__C (*((CT__ti_sysbios_utils_Load_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerFxn0 (ti_sysbios_utils_Load_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_utils_Load_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn1 ti_sysbios_utils_Load_Module__loggerFxn1__C;
#ifdef ti_sysbios_utils_Load_Module__loggerFxn1__CR
#define ti_sysbios_utils_Load_Module__loggerFxn1__C (*((CT__ti_sysbios_utils_Load_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerFxn1 (ti_sysbios_utils_Load_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_utils_Load_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn2 ti_sysbios_utils_Load_Module__loggerFxn2__C;
#ifdef ti_sysbios_utils_Load_Module__loggerFxn2__CR
#define ti_sysbios_utils_Load_Module__loggerFxn2__C (*((CT__ti_sysbios_utils_Load_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerFxn2 (ti_sysbios_utils_Load_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_utils_Load_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn4 ti_sysbios_utils_Load_Module__loggerFxn4__C;
#ifdef ti_sysbios_utils_Load_Module__loggerFxn4__CR
#define ti_sysbios_utils_Load_Module__loggerFxn4__C (*((CT__ti_sysbios_utils_Load_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerFxn4 (ti_sysbios_utils_Load_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_utils_Load_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn8 ti_sysbios_utils_Load_Module__loggerFxn8__C;
#ifdef ti_sysbios_utils_Load_Module__loggerFxn8__CR
#define ti_sysbios_utils_Load_Module__loggerFxn8__C (*((CT__ti_sysbios_utils_Load_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_utils_Load_Module__loggerFxn8 (ti_sysbios_utils_Load_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_utils_Load_Object__count;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Object__count ti_sysbios_utils_Load_Object__count__C;
#ifdef ti_sysbios_utils_Load_Object__count__CR
#define ti_sysbios_utils_Load_Object__count__C (*((CT__ti_sysbios_utils_Load_Object__count*)(xdcRomConstPtr + ti_sysbios_utils_Load_Object__count__C_offset)))
#else
#define ti_sysbios_utils_Load_Object__count (ti_sysbios_utils_Load_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_utils_Load_Object__heap;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Object__heap ti_sysbios_utils_Load_Object__heap__C;
#ifdef ti_sysbios_utils_Load_Object__heap__CR
#define ti_sysbios_utils_Load_Object__heap__C (*((CT__ti_sysbios_utils_Load_Object__heap*)(xdcRomConstPtr + ti_sysbios_utils_Load_Object__heap__C_offset)))
#else
#define ti_sysbios_utils_Load_Object__heap (ti_sysbios_utils_Load_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_utils_Load_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Object__sizeof ti_sysbios_utils_Load_Object__sizeof__C;
#ifdef ti_sysbios_utils_Load_Object__sizeof__CR
#define ti_sysbios_utils_Load_Object__sizeof__C (*((CT__ti_sysbios_utils_Load_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_utils_Load_Object__sizeof__C_offset)))
#else
#define ti_sysbios_utils_Load_Object__sizeof (ti_sysbios_utils_Load_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_utils_Load_Object__table;
__extern __FAR__ const CT__ti_sysbios_utils_Load_Object__table ti_sysbios_utils_Load_Object__table__C;
#ifdef ti_sysbios_utils_Load_Object__table__CR
#define ti_sysbios_utils_Load_Object__table__C (*((CT__ti_sysbios_utils_Load_Object__table*)(xdcRomConstPtr + ti_sysbios_utils_Load_Object__table__C_offset)))
#else
#define ti_sysbios_utils_Load_Object__table (ti_sysbios_utils_Load_Object__table__C)
#endif

/* LS_cpuLoad */
typedef xdc_runtime_Log_Event CT__ti_sysbios_utils_Load_LS_cpuLoad;
__extern __FAR__ const CT__ti_sysbios_utils_Load_LS_cpuLoad ti_sysbios_utils_Load_LS_cpuLoad__C;
#ifdef ti_sysbios_utils_Load_LS_cpuLoad__CR
#define ti_sysbios_utils_Load_LS_cpuLoad (*((CT__ti_sysbios_utils_Load_LS_cpuLoad*)(xdcRomConstPtr + ti_sysbios_utils_Load_LS_cpuLoad__C_offset)))
#else
#define ti_sysbios_utils_Load_LS_cpuLoad (ti_sysbios_utils_Load_LS_cpuLoad__C)
#endif

/* LS_hwiLoad */
typedef xdc_runtime_Log_Event CT__ti_sysbios_utils_Load_LS_hwiLoad;
__extern __FAR__ const CT__ti_sysbios_utils_Load_LS_hwiLoad ti_sysbios_utils_Load_LS_hwiLoad__C;
#ifdef ti_sysbios_utils_Load_LS_hwiLoad__CR
#define ti_sysbios_utils_Load_LS_hwiLoad (*((CT__ti_sysbios_utils_Load_LS_hwiLoad*)(xdcRomConstPtr + ti_sysbios_utils_Load_LS_hwiLoad__C_offset)))
#else
#define ti_sysbios_utils_Load_LS_hwiLoad (ti_sysbios_utils_Load_LS_hwiLoad__C)
#endif

/* LS_swiLoad */
typedef xdc_runtime_Log_Event CT__ti_sysbios_utils_Load_LS_swiLoad;
__extern __FAR__ const CT__ti_sysbios_utils_Load_LS_swiLoad ti_sysbios_utils_Load_LS_swiLoad__C;
#ifdef ti_sysbios_utils_Load_LS_swiLoad__CR
#define ti_sysbios_utils_Load_LS_swiLoad (*((CT__ti_sysbios_utils_Load_LS_swiLoad*)(xdcRomConstPtr + ti_sysbios_utils_Load_LS_swiLoad__C_offset)))
#else
#define ti_sysbios_utils_Load_LS_swiLoad (ti_sysbios_utils_Load_LS_swiLoad__C)
#endif

/* LS_taskLoad */
typedef xdc_runtime_Log_Event CT__ti_sysbios_utils_Load_LS_taskLoad;
__extern __FAR__ const CT__ti_sysbios_utils_Load_LS_taskLoad ti_sysbios_utils_Load_LS_taskLoad__C;
#ifdef ti_sysbios_utils_Load_LS_taskLoad__CR
#define ti_sysbios_utils_Load_LS_taskLoad (*((CT__ti_sysbios_utils_Load_LS_taskLoad*)(xdcRomConstPtr + ti_sysbios_utils_Load_LS_taskLoad__C_offset)))
#else
#define ti_sysbios_utils_Load_LS_taskLoad (ti_sysbios_utils_Load_LS_taskLoad__C)
#endif

/* postUpdate */
typedef ti_sysbios_utils_Load_FuncPtr CT__ti_sysbios_utils_Load_postUpdate;
__extern __FAR__ const CT__ti_sysbios_utils_Load_postUpdate ti_sysbios_utils_Load_postUpdate__C;
#ifdef ti_sysbios_utils_Load_postUpdate__CR
#define ti_sysbios_utils_Load_postUpdate (*((CT__ti_sysbios_utils_Load_postUpdate*)(xdcRomConstPtr + ti_sysbios_utils_Load_postUpdate__C_offset)))
#else
#define ti_sysbios_utils_Load_postUpdate (ti_sysbios_utils_Load_postUpdate__C)
#endif

/* updateInIdle */
typedef xdc_Bool CT__ti_sysbios_utils_Load_updateInIdle;
__extern __FAR__ const CT__ti_sysbios_utils_Load_updateInIdle ti_sysbios_utils_Load_updateInIdle__C;
#ifdef ti_sysbios_utils_Load_updateInIdle__CR
#define ti_sysbios_utils_Load_updateInIdle (*((CT__ti_sysbios_utils_Load_updateInIdle*)(xdcRomConstPtr + ti_sysbios_utils_Load_updateInIdle__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_updateInIdle__D
#define ti_sysbios_utils_Load_updateInIdle (ti_sysbios_utils_Load_updateInIdle__D)
#else
#define ti_sysbios_utils_Load_updateInIdle (ti_sysbios_utils_Load_updateInIdle__C)
#endif
#endif

/* windowInMs */
typedef xdc_UInt CT__ti_sysbios_utils_Load_windowInMs;
__extern __FAR__ const CT__ti_sysbios_utils_Load_windowInMs ti_sysbios_utils_Load_windowInMs__C;
#ifdef ti_sysbios_utils_Load_windowInMs__CR
#define ti_sysbios_utils_Load_windowInMs (*((CT__ti_sysbios_utils_Load_windowInMs*)(xdcRomConstPtr + ti_sysbios_utils_Load_windowInMs__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_windowInMs__D
#define ti_sysbios_utils_Load_windowInMs (ti_sysbios_utils_Load_windowInMs__D)
#else
#define ti_sysbios_utils_Load_windowInMs (ti_sysbios_utils_Load_windowInMs__C)
#endif
#endif

/* hwiEnabled */
typedef xdc_Bool CT__ti_sysbios_utils_Load_hwiEnabled;
__extern __FAR__ const CT__ti_sysbios_utils_Load_hwiEnabled ti_sysbios_utils_Load_hwiEnabled__C;
#ifdef ti_sysbios_utils_Load_hwiEnabled__CR
#define ti_sysbios_utils_Load_hwiEnabled (*((CT__ti_sysbios_utils_Load_hwiEnabled*)(xdcRomConstPtr + ti_sysbios_utils_Load_hwiEnabled__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_hwiEnabled__D
#define ti_sysbios_utils_Load_hwiEnabled (ti_sysbios_utils_Load_hwiEnabled__D)
#else
#define ti_sysbios_utils_Load_hwiEnabled (ti_sysbios_utils_Load_hwiEnabled__C)
#endif
#endif

/* swiEnabled */
typedef xdc_Bool CT__ti_sysbios_utils_Load_swiEnabled;
__extern __FAR__ const CT__ti_sysbios_utils_Load_swiEnabled ti_sysbios_utils_Load_swiEnabled__C;
#ifdef ti_sysbios_utils_Load_swiEnabled__CR
#define ti_sysbios_utils_Load_swiEnabled (*((CT__ti_sysbios_utils_Load_swiEnabled*)(xdcRomConstPtr + ti_sysbios_utils_Load_swiEnabled__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_swiEnabled__D
#define ti_sysbios_utils_Load_swiEnabled (ti_sysbios_utils_Load_swiEnabled__D)
#else
#define ti_sysbios_utils_Load_swiEnabled (ti_sysbios_utils_Load_swiEnabled__C)
#endif
#endif

/* taskEnabled */
typedef xdc_Bool CT__ti_sysbios_utils_Load_taskEnabled;
__extern __FAR__ const CT__ti_sysbios_utils_Load_taskEnabled ti_sysbios_utils_Load_taskEnabled__C;
#ifdef ti_sysbios_utils_Load_taskEnabled__CR
#define ti_sysbios_utils_Load_taskEnabled (*((CT__ti_sysbios_utils_Load_taskEnabled*)(xdcRomConstPtr + ti_sysbios_utils_Load_taskEnabled__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_taskEnabled__D
#define ti_sysbios_utils_Load_taskEnabled (ti_sysbios_utils_Load_taskEnabled__D)
#else
#define ti_sysbios_utils_Load_taskEnabled (ti_sysbios_utils_Load_taskEnabled__C)
#endif
#endif

/* autoAddTasks */
typedef xdc_Bool CT__ti_sysbios_utils_Load_autoAddTasks;
__extern __FAR__ const CT__ti_sysbios_utils_Load_autoAddTasks ti_sysbios_utils_Load_autoAddTasks__C;
#ifdef ti_sysbios_utils_Load_autoAddTasks__CR
#define ti_sysbios_utils_Load_autoAddTasks (*((CT__ti_sysbios_utils_Load_autoAddTasks*)(xdcRomConstPtr + ti_sysbios_utils_Load_autoAddTasks__C_offset)))
#else
#ifdef ti_sysbios_utils_Load_autoAddTasks__D
#define ti_sysbios_utils_Load_autoAddTasks (ti_sysbios_utils_Load_autoAddTasks__D)
#else
#define ti_sysbios_utils_Load_autoAddTasks (ti_sysbios_utils_Load_autoAddTasks__C)
#endif
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_utils_Load_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_utils_Load_Module__startupDone__S, "ti_sysbios_utils_Load_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_utils_Load_Module__startupDone__S( void);

/* getTaskLoad__E */
#define ti_sysbios_utils_Load_getTaskLoad ti_sysbios_utils_Load_getTaskLoad__E
xdc__CODESECT(ti_sysbios_utils_Load_getTaskLoad__E, "ti_sysbios_utils_Load_getTaskLoad")
__extern xdc_Bool ti_sysbios_utils_Load_getTaskLoad__E( ti_sysbios_knl_Task_Handle task, ti_sysbios_utils_Load_Stat *stat);

/* update__E */
#define ti_sysbios_utils_Load_update ti_sysbios_utils_Load_update__E
xdc__CODESECT(ti_sysbios_utils_Load_update__E, "ti_sysbios_utils_Load_update")
__extern xdc_Void ti_sysbios_utils_Load_update__E( void);

/* updateCPULoad__E */
#define ti_sysbios_utils_Load_updateCPULoad ti_sysbios_utils_Load_updateCPULoad__E
xdc__CODESECT(ti_sysbios_utils_Load_updateCPULoad__E, "ti_sysbios_utils_Load_updateCPULoad")
__extern xdc_Void ti_sysbios_utils_Load_updateCPULoad__E( void);

/* updateLoads__E */
#define ti_sysbios_utils_Load_updateLoads ti_sysbios_utils_Load_updateLoads__E
xdc__CODESECT(ti_sysbios_utils_Load_updateLoads__E, "ti_sysbios_utils_Load_updateLoads")
__extern xdc_Void ti_sysbios_utils_Load_updateLoads__E( void);

/* updateContextsAndPost__E */
#define ti_sysbios_utils_Load_updateContextsAndPost ti_sysbios_utils_Load_updateContextsAndPost__E
xdc__CODESECT(ti_sysbios_utils_Load_updateContextsAndPost__E, "ti_sysbios_utils_Load_updateContextsAndPost")
__extern xdc_Void ti_sysbios_utils_Load_updateContextsAndPost__E( void);

/* updateCurrentThreadTime__E */
#define ti_sysbios_utils_Load_updateCurrentThreadTime ti_sysbios_utils_Load_updateCurrentThreadTime__E
xdc__CODESECT(ti_sysbios_utils_Load_updateCurrentThreadTime__E, "ti_sysbios_utils_Load_updateCurrentThreadTime")
__extern xdc_Void ti_sysbios_utils_Load_updateCurrentThreadTime__E( void);

/* updateThreadContexts__E */
#define ti_sysbios_utils_Load_updateThreadContexts ti_sysbios_utils_Load_updateThreadContexts__E
xdc__CODESECT(ti_sysbios_utils_Load_updateThreadContexts__E, "ti_sysbios_utils_Load_updateThreadContexts")
__extern xdc_Void ti_sysbios_utils_Load_updateThreadContexts__E( void);

/* reset__E */
#define ti_sysbios_utils_Load_reset ti_sysbios_utils_Load_reset__E
xdc__CODESECT(ti_sysbios_utils_Load_reset__E, "ti_sysbios_utils_Load_reset")
__extern xdc_Void ti_sysbios_utils_Load_reset__E( void);

/* getGlobalSwiLoad__E */
#define ti_sysbios_utils_Load_getGlobalSwiLoad ti_sysbios_utils_Load_getGlobalSwiLoad__E
xdc__CODESECT(ti_sysbios_utils_Load_getGlobalSwiLoad__E, "ti_sysbios_utils_Load_getGlobalSwiLoad")
__extern xdc_Bool ti_sysbios_utils_Load_getGlobalSwiLoad__E( ti_sysbios_utils_Load_Stat *stat);

/* getGlobalHwiLoad__E */
#define ti_sysbios_utils_Load_getGlobalHwiLoad ti_sysbios_utils_Load_getGlobalHwiLoad__E
xdc__CODESECT(ti_sysbios_utils_Load_getGlobalHwiLoad__E, "ti_sysbios_utils_Load_getGlobalHwiLoad")
__extern xdc_Bool ti_sysbios_utils_Load_getGlobalHwiLoad__E( ti_sysbios_utils_Load_Stat *stat);

/* getCPULoad__E */
#define ti_sysbios_utils_Load_getCPULoad ti_sysbios_utils_Load_getCPULoad__E
xdc__CODESECT(ti_sysbios_utils_Load_getCPULoad__E, "ti_sysbios_utils_Load_getCPULoad")
__extern xdc_UInt32 ti_sysbios_utils_Load_getCPULoad__E( void);

/* calculateLoad__E */
#define ti_sysbios_utils_Load_calculateLoad ti_sysbios_utils_Load_calculateLoad__E
xdc__CODESECT(ti_sysbios_utils_Load_calculateLoad__E, "ti_sysbios_utils_Load_calculateLoad")
__extern xdc_UInt32 ti_sysbios_utils_Load_calculateLoad__E( ti_sysbios_utils_Load_Stat *stat);

/* setMinIdle__E */
#define ti_sysbios_utils_Load_setMinIdle ti_sysbios_utils_Load_setMinIdle__E
xdc__CODESECT(ti_sysbios_utils_Load_setMinIdle__E, "ti_sysbios_utils_Load_setMinIdle")
__extern xdc_UInt32 ti_sysbios_utils_Load_setMinIdle__E( xdc_UInt32 newMinIdleTime);

/* addTask__E */
#define ti_sysbios_utils_Load_addTask ti_sysbios_utils_Load_addTask__E
xdc__CODESECT(ti_sysbios_utils_Load_addTask__E, "ti_sysbios_utils_Load_addTask")
__extern xdc_Void ti_sysbios_utils_Load_addTask__E( ti_sysbios_knl_Task_Handle task, ti_sysbios_utils_Load_HookContext *env);

/* removeTask__E */
#define ti_sysbios_utils_Load_removeTask ti_sysbios_utils_Load_removeTask__E
xdc__CODESECT(ti_sysbios_utils_Load_removeTask__E, "ti_sysbios_utils_Load_removeTask")
__extern xdc_Bool ti_sysbios_utils_Load_removeTask__E( ti_sysbios_knl_Task_Handle task);

/* idleFxn__E */
#define ti_sysbios_utils_Load_idleFxn ti_sysbios_utils_Load_idleFxn__E
xdc__CODESECT(ti_sysbios_utils_Load_idleFxn__E, "ti_sysbios_utils_Load_idleFxn")
__extern xdc_Void ti_sysbios_utils_Load_idleFxn__E( void);

/* idleFxnPwr__E */
#define ti_sysbios_utils_Load_idleFxnPwr ti_sysbios_utils_Load_idleFxnPwr__E
xdc__CODESECT(ti_sysbios_utils_Load_idleFxnPwr__E, "ti_sysbios_utils_Load_idleFxnPwr")
__extern xdc_Void ti_sysbios_utils_Load_idleFxnPwr__E( void);

/* startup__E */
#define ti_sysbios_utils_Load_startup ti_sysbios_utils_Load_startup__E
xdc__CODESECT(ti_sysbios_utils_Load_startup__E, "ti_sysbios_utils_Load_startup")
__extern xdc_Void ti_sysbios_utils_Load_startup__E( void);

/* taskCreateHook__E */
#define ti_sysbios_utils_Load_taskCreateHook ti_sysbios_utils_Load_taskCreateHook__E
xdc__CODESECT(ti_sysbios_utils_Load_taskCreateHook__E, "ti_sysbios_utils_Load_taskCreateHook")
__extern xdc_Void ti_sysbios_utils_Load_taskCreateHook__E( ti_sysbios_knl_Task_Handle task, xdc_runtime_Error_Block *eb);

/* taskDeleteHook__E */
#define ti_sysbios_utils_Load_taskDeleteHook ti_sysbios_utils_Load_taskDeleteHook__E
xdc__CODESECT(ti_sysbios_utils_Load_taskDeleteHook__E, "ti_sysbios_utils_Load_taskDeleteHook")
__extern xdc_Void ti_sysbios_utils_Load_taskDeleteHook__E( ti_sysbios_knl_Task_Handle task);

/* taskSwitchHook__E */
#define ti_sysbios_utils_Load_taskSwitchHook ti_sysbios_utils_Load_taskSwitchHook__E
xdc__CODESECT(ti_sysbios_utils_Load_taskSwitchHook__E, "ti_sysbios_utils_Load_taskSwitchHook")
__extern xdc_Void ti_sysbios_utils_Load_taskSwitchHook__E( ti_sysbios_knl_Task_Handle curTask, ti_sysbios_knl_Task_Handle nextTask);

/* swiBeginHook__E */
#define ti_sysbios_utils_Load_swiBeginHook ti_sysbios_utils_Load_swiBeginHook__E
xdc__CODESECT(ti_sysbios_utils_Load_swiBeginHook__E, "ti_sysbios_utils_Load_swiBeginHook")
__extern xdc_Void ti_sysbios_utils_Load_swiBeginHook__E( ti_sysbios_knl_Swi_Handle swi);

/* swiEndHook__E */
#define ti_sysbios_utils_Load_swiEndHook ti_sysbios_utils_Load_swiEndHook__E
xdc__CODESECT(ti_sysbios_utils_Load_swiEndHook__E, "ti_sysbios_utils_Load_swiEndHook")
__extern xdc_Void ti_sysbios_utils_Load_swiEndHook__E( ti_sysbios_knl_Swi_Handle swi);

/* hwiBeginHook__E */
#define ti_sysbios_utils_Load_hwiBeginHook ti_sysbios_utils_Load_hwiBeginHook__E
xdc__CODESECT(ti_sysbios_utils_Load_hwiBeginHook__E, "ti_sysbios_utils_Load_hwiBeginHook")
__extern xdc_Void ti_sysbios_utils_Load_hwiBeginHook__E( ti_sysbios_interfaces_IHwi_Handle hwi);

/* hwiEndHook__E */
#define ti_sysbios_utils_Load_hwiEndHook ti_sysbios_utils_Load_hwiEndHook__E
xdc__CODESECT(ti_sysbios_utils_Load_hwiEndHook__E, "ti_sysbios_utils_Load_hwiEndHook")
__extern xdc_Void ti_sysbios_utils_Load_hwiEndHook__E( ti_sysbios_interfaces_IHwi_Handle hwi);

/* taskRegHook__E */
#define ti_sysbios_utils_Load_taskRegHook ti_sysbios_utils_Load_taskRegHook__E
xdc__CODESECT(ti_sysbios_utils_Load_taskRegHook__E, "ti_sysbios_utils_Load_taskRegHook")
__extern xdc_Void ti_sysbios_utils_Load_taskRegHook__E( xdc_Int id);

/* logLoads__I */
#define ti_sysbios_utils_Load_logLoads ti_sysbios_utils_Load_logLoads__I
xdc__CODESECT(ti_sysbios_utils_Load_logLoads__I, "ti_sysbios_utils_Load_logLoads")
__extern xdc_Void ti_sysbios_utils_Load_logLoads__I( void);

/* logCPULoad__I */
#define ti_sysbios_utils_Load_logCPULoad ti_sysbios_utils_Load_logCPULoad__I
xdc__CODESECT(ti_sysbios_utils_Load_logCPULoad__I, "ti_sysbios_utils_Load_logCPULoad")
__extern xdc_Void ti_sysbios_utils_Load_logCPULoad__I( void);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_utils_Load_Module_startupDone() ti_sysbios_utils_Load_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_utils_Load_Object_heap() ti_sysbios_utils_Load_Object__heap__C

/* Module_heap */
#define ti_sysbios_utils_Load_Module_heap() ti_sysbios_utils_Load_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_utils_Load_Module__id ti_sysbios_utils_Load_Module_id(void);
static inline CT__ti_sysbios_utils_Load_Module__id ti_sysbios_utils_Load_Module_id( void ) 
{
    return ti_sysbios_utils_Load_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_utils_Load_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_utils_Load_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_utils_Load_Module__diagsMask__C != (CT__ti_sysbios_utils_Load_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_utils_Load_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_utils_Load_Module_getMask(void)
{
    return (ti_sysbios_utils_Load_Module__diagsMask__C != (CT__ti_sysbios_utils_Load_Module__diagsMask)NULL) ? *ti_sysbios_utils_Load_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_utils_Load_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_utils_Load_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_utils_Load_Module__diagsMask__C != (CT__ti_sysbios_utils_Load_Module__diagsMask)NULL) {
        *ti_sysbios_utils_Load_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_utils_Load__top__
#undef __nested__
#endif

#endif /* ti_sysbios_utils_Load__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_utils_Load__internalaccess))

#ifndef ti_sysbios_utils_Load__include_state
#define ti_sysbios_utils_Load__include_state

/* Module_State */
struct ti_sysbios_utils_Load_Module_State {
    xdc_Int taskHId;
    __TA_ti_sysbios_utils_Load_Module_State__taskStartTime taskStartTime;
    xdc_UInt32 timeElapsed;
    __TA_ti_sysbios_utils_Load_Module_State__runningTask runningTask;
    xdc_Bool firstSwitchDone;
    xdc_UInt32 swiStartTime;
    ti_sysbios_utils_Load_HookContext swiEnv;
    __TA_ti_sysbios_utils_Load_Module_State__taskEnv taskEnv;
    xdc_UInt32 swiCnt;
    xdc_UInt32 hwiStartTime;
    ti_sysbios_utils_Load_HookContext hwiEnv;
    xdc_UInt32 hwiCnt;
    xdc_UInt32 timeSlotCnt;
    xdc_UInt32 minLoop;
    xdc_UInt32 minIdle;
    xdc_UInt32 t0;
    xdc_UInt32 idleCnt;
    xdc_UInt32 cpuLoad;
    xdc_UInt32 taskEnvLen;
    xdc_UInt32 taskNum;
    xdc_Bool powerEnabled;
    xdc_UInt32 idleStartTime;
    xdc_UInt32 busyStartTime;
    xdc_UInt32 busyTime;
    char dummy;
};

/* Module__state__V */
#ifndef ti_sysbios_utils_Load_Module__state__VR
extern struct ti_sysbios_utils_Load_Module_State__ ti_sysbios_utils_Load_Module__state__V;
#else
#define ti_sysbios_utils_Load_Module__state__V (*((struct ti_sysbios_utils_Load_Module_State__*)(xdcRomStatePtr + ti_sysbios_utils_Load_Module__state__V_offset)))
#endif

/* Module_State_taskList */
#ifndef ti_sysbios_utils_Load_Module_State_taskList__OR
__extern __FAR__ const xdc_SizeT ti_sysbios_utils_Load_Module_State_taskList__O;
#endif
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_utils_Load_Module_State_taskList(void);
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_utils_Load_Module_State_taskList(void)
{
    return (ti_sysbios_knl_Queue_Handle)(((char*)&ti_sysbios_utils_Load_Module__state__V) + ti_sysbios_utils_Load_Module_State_taskList__O);
}

#endif /* ti_sysbios_utils_Load__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_utils_Load__nolocalnames)

#ifndef ti_sysbios_utils_Load__localnames__done
#define ti_sysbios_utils_Load__localnames__done

/* module prefix */
#define Load_FuncPtr ti_sysbios_utils_Load_FuncPtr
#define Load_Stat ti_sysbios_utils_Load_Stat
#define Load_HookContext ti_sysbios_utils_Load_HookContext
#define Load_Module_State ti_sysbios_utils_Load_Module_State
#define Load_LS_cpuLoad ti_sysbios_utils_Load_LS_cpuLoad
#define Load_LS_hwiLoad ti_sysbios_utils_Load_LS_hwiLoad
#define Load_LS_swiLoad ti_sysbios_utils_Load_LS_swiLoad
#define Load_LS_taskLoad ti_sysbios_utils_Load_LS_taskLoad
#define Load_postUpdate ti_sysbios_utils_Load_postUpdate
#define Load_updateInIdle ti_sysbios_utils_Load_updateInIdle
#define Load_windowInMs ti_sysbios_utils_Load_windowInMs
#define Load_hwiEnabled ti_sysbios_utils_Load_hwiEnabled
#define Load_swiEnabled ti_sysbios_utils_Load_swiEnabled
#define Load_taskEnabled ti_sysbios_utils_Load_taskEnabled
#define Load_autoAddTasks ti_sysbios_utils_Load_autoAddTasks
#define Load_Module_State_taskList ti_sysbios_utils_Load_Module_State_taskList
#define Load_getTaskLoad ti_sysbios_utils_Load_getTaskLoad
#define Load_update ti_sysbios_utils_Load_update
#define Load_updateCPULoad ti_sysbios_utils_Load_updateCPULoad
#define Load_updateLoads ti_sysbios_utils_Load_updateLoads
#define Load_updateContextsAndPost ti_sysbios_utils_Load_updateContextsAndPost
#define Load_updateCurrentThreadTime ti_sysbios_utils_Load_updateCurrentThreadTime
#define Load_updateThreadContexts ti_sysbios_utils_Load_updateThreadContexts
#define Load_reset ti_sysbios_utils_Load_reset
#define Load_getGlobalSwiLoad ti_sysbios_utils_Load_getGlobalSwiLoad
#define Load_getGlobalHwiLoad ti_sysbios_utils_Load_getGlobalHwiLoad
#define Load_getCPULoad ti_sysbios_utils_Load_getCPULoad
#define Load_calculateLoad ti_sysbios_utils_Load_calculateLoad
#define Load_setMinIdle ti_sysbios_utils_Load_setMinIdle
#define Load_addTask ti_sysbios_utils_Load_addTask
#define Load_removeTask ti_sysbios_utils_Load_removeTask
#define Load_idleFxn ti_sysbios_utils_Load_idleFxn
#define Load_idleFxnPwr ti_sysbios_utils_Load_idleFxnPwr
#define Load_startup ti_sysbios_utils_Load_startup
#define Load_taskCreateHook ti_sysbios_utils_Load_taskCreateHook
#define Load_taskDeleteHook ti_sysbios_utils_Load_taskDeleteHook
#define Load_taskSwitchHook ti_sysbios_utils_Load_taskSwitchHook
#define Load_swiBeginHook ti_sysbios_utils_Load_swiBeginHook
#define Load_swiEndHook ti_sysbios_utils_Load_swiEndHook
#define Load_hwiBeginHook ti_sysbios_utils_Load_hwiBeginHook
#define Load_hwiEndHook ti_sysbios_utils_Load_hwiEndHook
#define Load_taskRegHook ti_sysbios_utils_Load_taskRegHook
#define Load_Module_name ti_sysbios_utils_Load_Module_name
#define Load_Module_id ti_sysbios_utils_Load_Module_id
#define Load_Module_startup ti_sysbios_utils_Load_Module_startup
#define Load_Module_startupDone ti_sysbios_utils_Load_Module_startupDone
#define Load_Module_hasMask ti_sysbios_utils_Load_Module_hasMask
#define Load_Module_getMask ti_sysbios_utils_Load_Module_getMask
#define Load_Module_setMask ti_sysbios_utils_Load_Module_setMask
#define Load_Object_heap ti_sysbios_utils_Load_Object_heap
#define Load_Module_heap ti_sysbios_utils_Load_Module_heap

#endif /* ti_sysbios_utils_Load__localnames__done */
#endif
