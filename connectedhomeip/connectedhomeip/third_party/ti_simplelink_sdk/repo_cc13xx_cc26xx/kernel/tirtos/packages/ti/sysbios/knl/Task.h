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

#ifndef ti_sysbios_knl_Task__include
#define ti_sysbios_knl_Task__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_knl_Task__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_knl_Task___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/knl/Task__prologue.h>
#include <ti/sysbios/knl/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/knl/Queue.h>
#include <xdc/runtime/IModule.h>
#include <ti/sysbios/interfaces/ITaskSupport.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/package/Task_SupportProxy.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_knl_Task_FuncPtr)(xdc_UArg arg1, xdc_UArg arg2);

/* AllBlockedFuncPtr */
typedef xdc_Void (*ti_sysbios_knl_Task_AllBlockedFuncPtr)(xdc_Void );

/* ModStateCheckValueFuncPtr */
typedef xdc_UInt32 (*ti_sysbios_knl_Task_ModStateCheckValueFuncPtr)(ti_sysbios_knl_Task_Module_State* arg1);

/* ModStateCheckFuncPtr */
typedef xdc_Int (*ti_sysbios_knl_Task_ModStateCheckFuncPtr)(ti_sysbios_knl_Task_Module_State* arg1, xdc_UInt32 arg2);

/* ObjectCheckValueFuncPtr */
typedef xdc_UInt32 (*ti_sysbios_knl_Task_ObjectCheckValueFuncPtr)(ti_sysbios_knl_Task_Handle arg1);

/* ObjectCheckFuncPtr */
typedef xdc_Int (*ti_sysbios_knl_Task_ObjectCheckFuncPtr)(ti_sysbios_knl_Task_Handle arg1, xdc_UInt32 arg2);

/* Mode */
enum ti_sysbios_knl_Task_Mode {
    ti_sysbios_knl_Task_Mode_RUNNING,
    ti_sysbios_knl_Task_Mode_READY,
    ti_sysbios_knl_Task_Mode_BLOCKED,
    ti_sysbios_knl_Task_Mode_TERMINATED,
    ti_sysbios_knl_Task_Mode_INACTIVE
};
typedef enum ti_sysbios_knl_Task_Mode ti_sysbios_knl_Task_Mode;

/* Stat */
struct ti_sysbios_knl_Task_Stat {
    xdc_Int priority;
    xdc_Ptr stack;
    xdc_SizeT stackSize;
    xdc_runtime_IHeap_Handle stackHeap;
    xdc_Ptr env;
    ti_sysbios_knl_Task_Mode mode;
    xdc_Ptr sp;
    xdc_SizeT used;
};

/* HookSet */
struct ti_sysbios_knl_Task_HookSet {
    xdc_Void (*registerFxn)(xdc_Int arg1);
    xdc_Void (*createFxn)(ti_sysbios_knl_Task_Handle arg1, xdc_runtime_Error_Block* arg2);
    xdc_Void (*readyFxn)(ti_sysbios_knl_Task_Handle arg1);
    xdc_Void (*switchFxn)(ti_sysbios_knl_Task_Handle arg1, ti_sysbios_knl_Task_Handle arg2);
    xdc_Void (*exitFxn)(ti_sysbios_knl_Task_Handle arg1);
    xdc_Void (*deleteFxn)(ti_sysbios_knl_Task_Handle arg1);
};

/* AFFINITY_NONE */
#define ti_sysbios_knl_Task_AFFINITY_NONE (~(0))


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_knl_Task_Args__create {
    ti_sysbios_knl_Task_FuncPtr fxn;
} ti_sysbios_knl_Task_Args__create;


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* PendElem */
struct ti_sysbios_knl_Task_PendElem {
    ti_sysbios_knl_Queue_Elem qElem;
    ti_sysbios_knl_Task_Handle task;
    ti_sysbios_knl_Clock_Handle clock;
};

/* Instance_State */
typedef xdc_Char __T1_ti_sysbios_knl_Task_Instance_State__stack;
typedef xdc_Char *ARRAY1_ti_sysbios_knl_Task_Instance_State__stack;
typedef const xdc_Char *CARRAY1_ti_sysbios_knl_Task_Instance_State__stack;
typedef ARRAY1_ti_sysbios_knl_Task_Instance_State__stack __TA_ti_sysbios_knl_Task_Instance_State__stack;
typedef xdc_Ptr __T1_ti_sysbios_knl_Task_Instance_State__hookEnv;
typedef xdc_Ptr *ARRAY1_ti_sysbios_knl_Task_Instance_State__hookEnv;
typedef const xdc_Ptr *CARRAY1_ti_sysbios_knl_Task_Instance_State__hookEnv;
typedef ARRAY1_ti_sysbios_knl_Task_Instance_State__hookEnv __TA_ti_sysbios_knl_Task_Instance_State__hookEnv;

/* Module_State */
typedef ti_sysbios_knl_Queue_Object __T1_ti_sysbios_knl_Task_Module_State__readyQ;
typedef ti_sysbios_knl_Queue_Object *ARRAY1_ti_sysbios_knl_Task_Module_State__readyQ;
typedef const ti_sysbios_knl_Queue_Object *CARRAY1_ti_sysbios_knl_Task_Module_State__readyQ;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__readyQ __TA_ti_sysbios_knl_Task_Module_State__readyQ;
typedef volatile xdc_UInt __T1_ti_sysbios_knl_Task_Module_State__smpCurSet;
typedef volatile xdc_UInt *ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurSet;
typedef const volatile xdc_UInt *CARRAY1_ti_sysbios_knl_Task_Module_State__smpCurSet;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurSet __TA_ti_sysbios_knl_Task_Module_State__smpCurSet;
typedef volatile xdc_UInt __T1_ti_sysbios_knl_Task_Module_State__smpCurMask;
typedef volatile xdc_UInt *ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurMask;
typedef const volatile xdc_UInt *CARRAY1_ti_sysbios_knl_Task_Module_State__smpCurMask;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurMask __TA_ti_sysbios_knl_Task_Module_State__smpCurMask;
typedef ti_sysbios_knl_Task_Handle __T1_ti_sysbios_knl_Task_Module_State__smpCurTask;
typedef ti_sysbios_knl_Task_Handle *ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurTask;
typedef const ti_sysbios_knl_Task_Handle *CARRAY1_ti_sysbios_knl_Task_Module_State__smpCurTask;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__smpCurTask __TA_ti_sysbios_knl_Task_Module_State__smpCurTask;
typedef ti_sysbios_knl_Queue_Handle __T1_ti_sysbios_knl_Task_Module_State__smpReadyQ;
typedef ti_sysbios_knl_Queue_Handle *ARRAY1_ti_sysbios_knl_Task_Module_State__smpReadyQ;
typedef const ti_sysbios_knl_Queue_Handle *CARRAY1_ti_sysbios_knl_Task_Module_State__smpReadyQ;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__smpReadyQ __TA_ti_sysbios_knl_Task_Module_State__smpReadyQ;
typedef ti_sysbios_knl_Task_Handle __T1_ti_sysbios_knl_Task_Module_State__idleTask;
typedef ti_sysbios_knl_Task_Handle *ARRAY1_ti_sysbios_knl_Task_Module_State__idleTask;
typedef const ti_sysbios_knl_Task_Handle *CARRAY1_ti_sysbios_knl_Task_Module_State__idleTask;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__idleTask __TA_ti_sysbios_knl_Task_Module_State__idleTask;
typedef ti_sysbios_knl_Task_Handle __T1_ti_sysbios_knl_Task_Module_State__constructedTasks;
typedef ti_sysbios_knl_Task_Handle *ARRAY1_ti_sysbios_knl_Task_Module_State__constructedTasks;
typedef const ti_sysbios_knl_Task_Handle *CARRAY1_ti_sysbios_knl_Task_Module_State__constructedTasks;
typedef ARRAY1_ti_sysbios_knl_Task_Module_State__constructedTasks __TA_ti_sysbios_knl_Task_Module_State__constructedTasks;

/* RunQEntry */
struct ti_sysbios_knl_Task_RunQEntry {
    ti_sysbios_knl_Queue_Elem elem;
    xdc_UInt coreId;
    xdc_Int priority;
};

/* Module_StateSmp */
typedef volatile ti_sysbios_knl_Task_RunQEntry __T1_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ;
typedef volatile ti_sysbios_knl_Task_RunQEntry *ARRAY1_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ;
typedef const volatile ti_sysbios_knl_Task_RunQEntry *CARRAY1_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ;
typedef ARRAY1_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ __TA_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ;
struct ti_sysbios_knl_Task_Module_StateSmp {
    ti_sysbios_knl_Queue_Object *sortedRunQ;
    __TA_ti_sysbios_knl_Task_Module_StateSmp__smpRunQ smpRunQ;
};


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_knl_Task_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__diagsEnabled ti_sysbios_knl_Task_Module__diagsEnabled__C;
#ifdef ti_sysbios_knl_Task_Module__diagsEnabled__CR
#define ti_sysbios_knl_Task_Module__diagsEnabled__C (*((CT__ti_sysbios_knl_Task_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__diagsEnabled (ti_sysbios_knl_Task_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_knl_Task_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__diagsIncluded ti_sysbios_knl_Task_Module__diagsIncluded__C;
#ifdef ti_sysbios_knl_Task_Module__diagsIncluded__CR
#define ti_sysbios_knl_Task_Module__diagsIncluded__C (*((CT__ti_sysbios_knl_Task_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__diagsIncluded (ti_sysbios_knl_Task_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_knl_Task_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__diagsMask ti_sysbios_knl_Task_Module__diagsMask__C;
#ifdef ti_sysbios_knl_Task_Module__diagsMask__CR
#define ti_sysbios_knl_Task_Module__diagsMask__C (*((CT__ti_sysbios_knl_Task_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__diagsMask (ti_sysbios_knl_Task_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_knl_Task_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__gateObj ti_sysbios_knl_Task_Module__gateObj__C;
#ifdef ti_sysbios_knl_Task_Module__gateObj__CR
#define ti_sysbios_knl_Task_Module__gateObj__C (*((CT__ti_sysbios_knl_Task_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__gateObj__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__gateObj (ti_sysbios_knl_Task_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_knl_Task_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__gatePrms ti_sysbios_knl_Task_Module__gatePrms__C;
#ifdef ti_sysbios_knl_Task_Module__gatePrms__CR
#define ti_sysbios_knl_Task_Module__gatePrms__C (*((CT__ti_sysbios_knl_Task_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__gatePrms (ti_sysbios_knl_Task_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_knl_Task_Module__id;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__id ti_sysbios_knl_Task_Module__id__C;
#ifdef ti_sysbios_knl_Task_Module__id__CR
#define ti_sysbios_knl_Task_Module__id__C (*((CT__ti_sysbios_knl_Task_Module__id*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__id__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__id (ti_sysbios_knl_Task_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_knl_Task_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerDefined ti_sysbios_knl_Task_Module__loggerDefined__C;
#ifdef ti_sysbios_knl_Task_Module__loggerDefined__CR
#define ti_sysbios_knl_Task_Module__loggerDefined__C (*((CT__ti_sysbios_knl_Task_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerDefined (ti_sysbios_knl_Task_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_knl_Task_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerObj ti_sysbios_knl_Task_Module__loggerObj__C;
#ifdef ti_sysbios_knl_Task_Module__loggerObj__CR
#define ti_sysbios_knl_Task_Module__loggerObj__C (*((CT__ti_sysbios_knl_Task_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerObj (ti_sysbios_knl_Task_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_knl_Task_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn0 ti_sysbios_knl_Task_Module__loggerFxn0__C;
#ifdef ti_sysbios_knl_Task_Module__loggerFxn0__CR
#define ti_sysbios_knl_Task_Module__loggerFxn0__C (*((CT__ti_sysbios_knl_Task_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerFxn0 (ti_sysbios_knl_Task_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_knl_Task_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn1 ti_sysbios_knl_Task_Module__loggerFxn1__C;
#ifdef ti_sysbios_knl_Task_Module__loggerFxn1__CR
#define ti_sysbios_knl_Task_Module__loggerFxn1__C (*((CT__ti_sysbios_knl_Task_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerFxn1 (ti_sysbios_knl_Task_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_knl_Task_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn2 ti_sysbios_knl_Task_Module__loggerFxn2__C;
#ifdef ti_sysbios_knl_Task_Module__loggerFxn2__CR
#define ti_sysbios_knl_Task_Module__loggerFxn2__C (*((CT__ti_sysbios_knl_Task_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerFxn2 (ti_sysbios_knl_Task_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_knl_Task_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn4 ti_sysbios_knl_Task_Module__loggerFxn4__C;
#ifdef ti_sysbios_knl_Task_Module__loggerFxn4__CR
#define ti_sysbios_knl_Task_Module__loggerFxn4__C (*((CT__ti_sysbios_knl_Task_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerFxn4 (ti_sysbios_knl_Task_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_knl_Task_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn8 ti_sysbios_knl_Task_Module__loggerFxn8__C;
#ifdef ti_sysbios_knl_Task_Module__loggerFxn8__CR
#define ti_sysbios_knl_Task_Module__loggerFxn8__C (*((CT__ti_sysbios_knl_Task_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_knl_Task_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_knl_Task_Module__loggerFxn8 (ti_sysbios_knl_Task_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_knl_Task_Object__count;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Object__count ti_sysbios_knl_Task_Object__count__C;
#ifdef ti_sysbios_knl_Task_Object__count__CR
#define ti_sysbios_knl_Task_Object__count__C (*((CT__ti_sysbios_knl_Task_Object__count*)(xdcRomConstPtr + ti_sysbios_knl_Task_Object__count__C_offset)))
#else
#define ti_sysbios_knl_Task_Object__count (ti_sysbios_knl_Task_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_knl_Task_Object__heap;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Object__heap ti_sysbios_knl_Task_Object__heap__C;
#ifdef ti_sysbios_knl_Task_Object__heap__CR
#define ti_sysbios_knl_Task_Object__heap__C (*((CT__ti_sysbios_knl_Task_Object__heap*)(xdcRomConstPtr + ti_sysbios_knl_Task_Object__heap__C_offset)))
#else
#define ti_sysbios_knl_Task_Object__heap (ti_sysbios_knl_Task_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_knl_Task_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Object__sizeof ti_sysbios_knl_Task_Object__sizeof__C;
#ifdef ti_sysbios_knl_Task_Object__sizeof__CR
#define ti_sysbios_knl_Task_Object__sizeof__C (*((CT__ti_sysbios_knl_Task_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_knl_Task_Object__sizeof__C_offset)))
#else
#define ti_sysbios_knl_Task_Object__sizeof (ti_sysbios_knl_Task_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_knl_Task_Object__table;
__extern __FAR__ const CT__ti_sysbios_knl_Task_Object__table ti_sysbios_knl_Task_Object__table__C;
#ifdef ti_sysbios_knl_Task_Object__table__CR
#define ti_sysbios_knl_Task_Object__table__C (*((CT__ti_sysbios_knl_Task_Object__table*)(xdcRomConstPtr + ti_sysbios_knl_Task_Object__table__C_offset)))
#else
#define ti_sysbios_knl_Task_Object__table (ti_sysbios_knl_Task_Object__table__C)
#endif

/* LM_switch */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_switch;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_switch ti_sysbios_knl_Task_LM_switch__C;
#ifdef ti_sysbios_knl_Task_LM_switch__CR
#define ti_sysbios_knl_Task_LM_switch (*((CT__ti_sysbios_knl_Task_LM_switch*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_switch__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_switch (ti_sysbios_knl_Task_LM_switch__C)
#endif

/* LM_sleep */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_sleep;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_sleep ti_sysbios_knl_Task_LM_sleep__C;
#ifdef ti_sysbios_knl_Task_LM_sleep__CR
#define ti_sysbios_knl_Task_LM_sleep (*((CT__ti_sysbios_knl_Task_LM_sleep*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_sleep__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_sleep (ti_sysbios_knl_Task_LM_sleep__C)
#endif

/* LD_ready */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LD_ready;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LD_ready ti_sysbios_knl_Task_LD_ready__C;
#ifdef ti_sysbios_knl_Task_LD_ready__CR
#define ti_sysbios_knl_Task_LD_ready (*((CT__ti_sysbios_knl_Task_LD_ready*)(xdcRomConstPtr + ti_sysbios_knl_Task_LD_ready__C_offset)))
#else
#define ti_sysbios_knl_Task_LD_ready (ti_sysbios_knl_Task_LD_ready__C)
#endif

/* LD_block */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LD_block;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LD_block ti_sysbios_knl_Task_LD_block__C;
#ifdef ti_sysbios_knl_Task_LD_block__CR
#define ti_sysbios_knl_Task_LD_block (*((CT__ti_sysbios_knl_Task_LD_block*)(xdcRomConstPtr + ti_sysbios_knl_Task_LD_block__C_offset)))
#else
#define ti_sysbios_knl_Task_LD_block (ti_sysbios_knl_Task_LD_block__C)
#endif

/* LM_yield */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_yield;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_yield ti_sysbios_knl_Task_LM_yield__C;
#ifdef ti_sysbios_knl_Task_LM_yield__CR
#define ti_sysbios_knl_Task_LM_yield (*((CT__ti_sysbios_knl_Task_LM_yield*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_yield__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_yield (ti_sysbios_knl_Task_LM_yield__C)
#endif

/* LM_setPri */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_setPri;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_setPri ti_sysbios_knl_Task_LM_setPri__C;
#ifdef ti_sysbios_knl_Task_LM_setPri__CR
#define ti_sysbios_knl_Task_LM_setPri (*((CT__ti_sysbios_knl_Task_LM_setPri*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_setPri__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_setPri (ti_sysbios_knl_Task_LM_setPri__C)
#endif

/* LD_exit */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LD_exit;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LD_exit ti_sysbios_knl_Task_LD_exit__C;
#ifdef ti_sysbios_knl_Task_LD_exit__CR
#define ti_sysbios_knl_Task_LD_exit (*((CT__ti_sysbios_knl_Task_LD_exit*)(xdcRomConstPtr + ti_sysbios_knl_Task_LD_exit__C_offset)))
#else
#define ti_sysbios_knl_Task_LD_exit (ti_sysbios_knl_Task_LD_exit__C)
#endif

/* LM_setAffinity */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_setAffinity;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_setAffinity ti_sysbios_knl_Task_LM_setAffinity__C;
#ifdef ti_sysbios_knl_Task_LM_setAffinity__CR
#define ti_sysbios_knl_Task_LM_setAffinity (*((CT__ti_sysbios_knl_Task_LM_setAffinity*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_setAffinity__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_setAffinity (ti_sysbios_knl_Task_LM_setAffinity__C)
#endif

/* LM_schedule */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_schedule;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_schedule ti_sysbios_knl_Task_LM_schedule__C;
#ifdef ti_sysbios_knl_Task_LM_schedule__CR
#define ti_sysbios_knl_Task_LM_schedule (*((CT__ti_sysbios_knl_Task_LM_schedule*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_schedule__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_schedule (ti_sysbios_knl_Task_LM_schedule__C)
#endif

/* LM_noWork */
typedef xdc_runtime_Log_Event CT__ti_sysbios_knl_Task_LM_noWork;
__extern __FAR__ const CT__ti_sysbios_knl_Task_LM_noWork ti_sysbios_knl_Task_LM_noWork__C;
#ifdef ti_sysbios_knl_Task_LM_noWork__CR
#define ti_sysbios_knl_Task_LM_noWork (*((CT__ti_sysbios_knl_Task_LM_noWork*)(xdcRomConstPtr + ti_sysbios_knl_Task_LM_noWork__C_offset)))
#else
#define ti_sysbios_knl_Task_LM_noWork (ti_sysbios_knl_Task_LM_noWork__C)
#endif

/* E_stackOverflow */
typedef xdc_runtime_Error_Id CT__ti_sysbios_knl_Task_E_stackOverflow;
__extern __FAR__ const CT__ti_sysbios_knl_Task_E_stackOverflow ti_sysbios_knl_Task_E_stackOverflow__C;
#ifdef ti_sysbios_knl_Task_E_stackOverflow__CR
#define ti_sysbios_knl_Task_E_stackOverflow (*((CT__ti_sysbios_knl_Task_E_stackOverflow*)(xdcRomConstPtr + ti_sysbios_knl_Task_E_stackOverflow__C_offset)))
#else
#define ti_sysbios_knl_Task_E_stackOverflow (ti_sysbios_knl_Task_E_stackOverflow__C)
#endif

/* E_spOutOfBounds */
typedef xdc_runtime_Error_Id CT__ti_sysbios_knl_Task_E_spOutOfBounds;
__extern __FAR__ const CT__ti_sysbios_knl_Task_E_spOutOfBounds ti_sysbios_knl_Task_E_spOutOfBounds__C;
#ifdef ti_sysbios_knl_Task_E_spOutOfBounds__CR
#define ti_sysbios_knl_Task_E_spOutOfBounds (*((CT__ti_sysbios_knl_Task_E_spOutOfBounds*)(xdcRomConstPtr + ti_sysbios_knl_Task_E_spOutOfBounds__C_offset)))
#else
#define ti_sysbios_knl_Task_E_spOutOfBounds (ti_sysbios_knl_Task_E_spOutOfBounds__C)
#endif

/* E_deleteNotAllowed */
typedef xdc_runtime_Error_Id CT__ti_sysbios_knl_Task_E_deleteNotAllowed;
__extern __FAR__ const CT__ti_sysbios_knl_Task_E_deleteNotAllowed ti_sysbios_knl_Task_E_deleteNotAllowed__C;
#ifdef ti_sysbios_knl_Task_E_deleteNotAllowed__CR
#define ti_sysbios_knl_Task_E_deleteNotAllowed (*((CT__ti_sysbios_knl_Task_E_deleteNotAllowed*)(xdcRomConstPtr + ti_sysbios_knl_Task_E_deleteNotAllowed__C_offset)))
#else
#define ti_sysbios_knl_Task_E_deleteNotAllowed (ti_sysbios_knl_Task_E_deleteNotAllowed__C)
#endif

/* E_moduleStateCheckFailed */
typedef xdc_runtime_Error_Id CT__ti_sysbios_knl_Task_E_moduleStateCheckFailed;
__extern __FAR__ const CT__ti_sysbios_knl_Task_E_moduleStateCheckFailed ti_sysbios_knl_Task_E_moduleStateCheckFailed__C;
#ifdef ti_sysbios_knl_Task_E_moduleStateCheckFailed__CR
#define ti_sysbios_knl_Task_E_moduleStateCheckFailed (*((CT__ti_sysbios_knl_Task_E_moduleStateCheckFailed*)(xdcRomConstPtr + ti_sysbios_knl_Task_E_moduleStateCheckFailed__C_offset)))
#else
#define ti_sysbios_knl_Task_E_moduleStateCheckFailed (ti_sysbios_knl_Task_E_moduleStateCheckFailed__C)
#endif

/* E_objectCheckFailed */
typedef xdc_runtime_Error_Id CT__ti_sysbios_knl_Task_E_objectCheckFailed;
__extern __FAR__ const CT__ti_sysbios_knl_Task_E_objectCheckFailed ti_sysbios_knl_Task_E_objectCheckFailed__C;
#ifdef ti_sysbios_knl_Task_E_objectCheckFailed__CR
#define ti_sysbios_knl_Task_E_objectCheckFailed (*((CT__ti_sysbios_knl_Task_E_objectCheckFailed*)(xdcRomConstPtr + ti_sysbios_knl_Task_E_objectCheckFailed__C_offset)))
#else
#define ti_sysbios_knl_Task_E_objectCheckFailed (ti_sysbios_knl_Task_E_objectCheckFailed__C)
#endif

/* A_badThreadType */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_badThreadType;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_badThreadType ti_sysbios_knl_Task_A_badThreadType__C;
#ifdef ti_sysbios_knl_Task_A_badThreadType__CR
#define ti_sysbios_knl_Task_A_badThreadType (*((CT__ti_sysbios_knl_Task_A_badThreadType*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_badThreadType__C_offset)))
#else
#define ti_sysbios_knl_Task_A_badThreadType (ti_sysbios_knl_Task_A_badThreadType__C)
#endif

/* A_badTaskState */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_badTaskState;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_badTaskState ti_sysbios_knl_Task_A_badTaskState__C;
#ifdef ti_sysbios_knl_Task_A_badTaskState__CR
#define ti_sysbios_knl_Task_A_badTaskState (*((CT__ti_sysbios_knl_Task_A_badTaskState*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_badTaskState__C_offset)))
#else
#define ti_sysbios_knl_Task_A_badTaskState (ti_sysbios_knl_Task_A_badTaskState__C)
#endif

/* A_noPendElem */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_noPendElem;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_noPendElem ti_sysbios_knl_Task_A_noPendElem__C;
#ifdef ti_sysbios_knl_Task_A_noPendElem__CR
#define ti_sysbios_knl_Task_A_noPendElem (*((CT__ti_sysbios_knl_Task_A_noPendElem*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_noPendElem__C_offset)))
#else
#define ti_sysbios_knl_Task_A_noPendElem (ti_sysbios_knl_Task_A_noPendElem__C)
#endif

/* A_taskDisabled */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_taskDisabled;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_taskDisabled ti_sysbios_knl_Task_A_taskDisabled__C;
#ifdef ti_sysbios_knl_Task_A_taskDisabled__CR
#define ti_sysbios_knl_Task_A_taskDisabled (*((CT__ti_sysbios_knl_Task_A_taskDisabled*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_taskDisabled__C_offset)))
#else
#define ti_sysbios_knl_Task_A_taskDisabled (ti_sysbios_knl_Task_A_taskDisabled__C)
#endif

/* A_badPriority */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_badPriority;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_badPriority ti_sysbios_knl_Task_A_badPriority__C;
#ifdef ti_sysbios_knl_Task_A_badPriority__CR
#define ti_sysbios_knl_Task_A_badPriority (*((CT__ti_sysbios_knl_Task_A_badPriority*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_badPriority__C_offset)))
#else
#define ti_sysbios_knl_Task_A_badPriority (ti_sysbios_knl_Task_A_badPriority__C)
#endif

/* A_badTimeout */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_badTimeout;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_badTimeout ti_sysbios_knl_Task_A_badTimeout__C;
#ifdef ti_sysbios_knl_Task_A_badTimeout__CR
#define ti_sysbios_knl_Task_A_badTimeout (*((CT__ti_sysbios_knl_Task_A_badTimeout*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_badTimeout__C_offset)))
#else
#define ti_sysbios_knl_Task_A_badTimeout (ti_sysbios_knl_Task_A_badTimeout__C)
#endif

/* A_badAffinity */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_badAffinity;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_badAffinity ti_sysbios_knl_Task_A_badAffinity__C;
#ifdef ti_sysbios_knl_Task_A_badAffinity__CR
#define ti_sysbios_knl_Task_A_badAffinity (*((CT__ti_sysbios_knl_Task_A_badAffinity*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_badAffinity__C_offset)))
#else
#define ti_sysbios_knl_Task_A_badAffinity (ti_sysbios_knl_Task_A_badAffinity__C)
#endif

/* A_sleepTaskDisabled */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_sleepTaskDisabled;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_sleepTaskDisabled ti_sysbios_knl_Task_A_sleepTaskDisabled__C;
#ifdef ti_sysbios_knl_Task_A_sleepTaskDisabled__CR
#define ti_sysbios_knl_Task_A_sleepTaskDisabled (*((CT__ti_sysbios_knl_Task_A_sleepTaskDisabled*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_sleepTaskDisabled__C_offset)))
#else
#define ti_sysbios_knl_Task_A_sleepTaskDisabled (ti_sysbios_knl_Task_A_sleepTaskDisabled__C)
#endif

/* A_invalidCoreId */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_knl_Task_A_invalidCoreId;
__extern __FAR__ const CT__ti_sysbios_knl_Task_A_invalidCoreId ti_sysbios_knl_Task_A_invalidCoreId__C;
#ifdef ti_sysbios_knl_Task_A_invalidCoreId__CR
#define ti_sysbios_knl_Task_A_invalidCoreId (*((CT__ti_sysbios_knl_Task_A_invalidCoreId*)(xdcRomConstPtr + ti_sysbios_knl_Task_A_invalidCoreId__C_offset)))
#else
#define ti_sysbios_knl_Task_A_invalidCoreId (ti_sysbios_knl_Task_A_invalidCoreId__C)
#endif

/* numPriorities */
typedef xdc_UInt CT__ti_sysbios_knl_Task_numPriorities;
__extern __FAR__ const CT__ti_sysbios_knl_Task_numPriorities ti_sysbios_knl_Task_numPriorities__C;
#ifdef ti_sysbios_knl_Task_numPriorities__CR
#define ti_sysbios_knl_Task_numPriorities (*((CT__ti_sysbios_knl_Task_numPriorities*)(xdcRomConstPtr + ti_sysbios_knl_Task_numPriorities__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_numPriorities__D
#define ti_sysbios_knl_Task_numPriorities (ti_sysbios_knl_Task_numPriorities__D)
#else
#define ti_sysbios_knl_Task_numPriorities (ti_sysbios_knl_Task_numPriorities__C)
#endif
#endif

/* defaultStackSize */
typedef xdc_SizeT CT__ti_sysbios_knl_Task_defaultStackSize;
__extern __FAR__ const CT__ti_sysbios_knl_Task_defaultStackSize ti_sysbios_knl_Task_defaultStackSize__C;
#ifdef ti_sysbios_knl_Task_defaultStackSize__CR
#define ti_sysbios_knl_Task_defaultStackSize (*((CT__ti_sysbios_knl_Task_defaultStackSize*)(xdcRomConstPtr + ti_sysbios_knl_Task_defaultStackSize__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_defaultStackSize__D
#define ti_sysbios_knl_Task_defaultStackSize (ti_sysbios_knl_Task_defaultStackSize__D)
#else
#define ti_sysbios_knl_Task_defaultStackSize (ti_sysbios_knl_Task_defaultStackSize__C)
#endif
#endif

/* defaultStackHeap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_knl_Task_defaultStackHeap;
__extern __FAR__ const CT__ti_sysbios_knl_Task_defaultStackHeap ti_sysbios_knl_Task_defaultStackHeap__C;
#ifdef ti_sysbios_knl_Task_defaultStackHeap__CR
#define ti_sysbios_knl_Task_defaultStackHeap (*((CT__ti_sysbios_knl_Task_defaultStackHeap*)(xdcRomConstPtr + ti_sysbios_knl_Task_defaultStackHeap__C_offset)))
#else
#define ti_sysbios_knl_Task_defaultStackHeap (ti_sysbios_knl_Task_defaultStackHeap__C)
#endif

/* allBlockedFunc */
typedef ti_sysbios_knl_Task_AllBlockedFuncPtr CT__ti_sysbios_knl_Task_allBlockedFunc;
__extern __FAR__ const CT__ti_sysbios_knl_Task_allBlockedFunc ti_sysbios_knl_Task_allBlockedFunc__C;
#ifdef ti_sysbios_knl_Task_allBlockedFunc__CR
#define ti_sysbios_knl_Task_allBlockedFunc (*((CT__ti_sysbios_knl_Task_allBlockedFunc*)(xdcRomConstPtr + ti_sysbios_knl_Task_allBlockedFunc__C_offset)))
#else
#define ti_sysbios_knl_Task_allBlockedFunc (ti_sysbios_knl_Task_allBlockedFunc__C)
#endif

/* initStackFlag */
typedef xdc_Bool CT__ti_sysbios_knl_Task_initStackFlag;
__extern __FAR__ const CT__ti_sysbios_knl_Task_initStackFlag ti_sysbios_knl_Task_initStackFlag__C;
#ifdef ti_sysbios_knl_Task_initStackFlag__CR
#define ti_sysbios_knl_Task_initStackFlag (*((CT__ti_sysbios_knl_Task_initStackFlag*)(xdcRomConstPtr + ti_sysbios_knl_Task_initStackFlag__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_initStackFlag__D
#define ti_sysbios_knl_Task_initStackFlag (ti_sysbios_knl_Task_initStackFlag__D)
#else
#define ti_sysbios_knl_Task_initStackFlag (ti_sysbios_knl_Task_initStackFlag__C)
#endif
#endif

/* checkStackFlag */
typedef xdc_Bool CT__ti_sysbios_knl_Task_checkStackFlag;
__extern __FAR__ const CT__ti_sysbios_knl_Task_checkStackFlag ti_sysbios_knl_Task_checkStackFlag__C;
#ifdef ti_sysbios_knl_Task_checkStackFlag__CR
#define ti_sysbios_knl_Task_checkStackFlag (*((CT__ti_sysbios_knl_Task_checkStackFlag*)(xdcRomConstPtr + ti_sysbios_knl_Task_checkStackFlag__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_checkStackFlag__D
#define ti_sysbios_knl_Task_checkStackFlag (ti_sysbios_knl_Task_checkStackFlag__D)
#else
#define ti_sysbios_knl_Task_checkStackFlag (ti_sysbios_knl_Task_checkStackFlag__C)
#endif
#endif

/* deleteTerminatedTasks */
typedef xdc_Bool CT__ti_sysbios_knl_Task_deleteTerminatedTasks;
__extern __FAR__ const CT__ti_sysbios_knl_Task_deleteTerminatedTasks ti_sysbios_knl_Task_deleteTerminatedTasks__C;
#ifdef ti_sysbios_knl_Task_deleteTerminatedTasks__CR
#define ti_sysbios_knl_Task_deleteTerminatedTasks (*((CT__ti_sysbios_knl_Task_deleteTerminatedTasks*)(xdcRomConstPtr + ti_sysbios_knl_Task_deleteTerminatedTasks__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_deleteTerminatedTasks__D
#define ti_sysbios_knl_Task_deleteTerminatedTasks (ti_sysbios_knl_Task_deleteTerminatedTasks__D)
#else
#define ti_sysbios_knl_Task_deleteTerminatedTasks (ti_sysbios_knl_Task_deleteTerminatedTasks__C)
#endif
#endif

/* hooks */
typedef ti_sysbios_knl_Task_HookSet __T1_ti_sysbios_knl_Task_hooks;
typedef struct { int length; ti_sysbios_knl_Task_HookSet *elem; } ARRAY1_ti_sysbios_knl_Task_hooks;
typedef struct { int length; ti_sysbios_knl_Task_HookSet const *elem; } CARRAY1_ti_sysbios_knl_Task_hooks;
typedef CARRAY1_ti_sysbios_knl_Task_hooks __TA_ti_sysbios_knl_Task_hooks;
typedef CARRAY1_ti_sysbios_knl_Task_hooks CT__ti_sysbios_knl_Task_hooks;
__extern __FAR__ const CT__ti_sysbios_knl_Task_hooks ti_sysbios_knl_Task_hooks__C;
#ifdef ti_sysbios_knl_Task_hooks__CR
#define ti_sysbios_knl_Task_hooks (*((CT__ti_sysbios_knl_Task_hooks*)(xdcRomConstPtr + ti_sysbios_knl_Task_hooks__C_offset)))
#else
#define ti_sysbios_knl_Task_hooks (ti_sysbios_knl_Task_hooks__C)
#endif

/* moduleStateCheckFxn */
typedef ti_sysbios_knl_Task_ModStateCheckFuncPtr CT__ti_sysbios_knl_Task_moduleStateCheckFxn;
__extern __FAR__ const CT__ti_sysbios_knl_Task_moduleStateCheckFxn ti_sysbios_knl_Task_moduleStateCheckFxn__C;
#ifdef ti_sysbios_knl_Task_moduleStateCheckFxn__CR
#define ti_sysbios_knl_Task_moduleStateCheckFxn (*((CT__ti_sysbios_knl_Task_moduleStateCheckFxn*)(xdcRomConstPtr + ti_sysbios_knl_Task_moduleStateCheckFxn__C_offset)))
#else
#define ti_sysbios_knl_Task_moduleStateCheckFxn (ti_sysbios_knl_Task_moduleStateCheckFxn__C)
#endif

/* moduleStateCheckValueFxn */
typedef ti_sysbios_knl_Task_ModStateCheckValueFuncPtr CT__ti_sysbios_knl_Task_moduleStateCheckValueFxn;
__extern __FAR__ const CT__ti_sysbios_knl_Task_moduleStateCheckValueFxn ti_sysbios_knl_Task_moduleStateCheckValueFxn__C;
#ifdef ti_sysbios_knl_Task_moduleStateCheckValueFxn__CR
#define ti_sysbios_knl_Task_moduleStateCheckValueFxn (*((CT__ti_sysbios_knl_Task_moduleStateCheckValueFxn*)(xdcRomConstPtr + ti_sysbios_knl_Task_moduleStateCheckValueFxn__C_offset)))
#else
#define ti_sysbios_knl_Task_moduleStateCheckValueFxn (ti_sysbios_knl_Task_moduleStateCheckValueFxn__C)
#endif

/* moduleStateCheckFlag */
typedef xdc_Bool CT__ti_sysbios_knl_Task_moduleStateCheckFlag;
__extern __FAR__ const CT__ti_sysbios_knl_Task_moduleStateCheckFlag ti_sysbios_knl_Task_moduleStateCheckFlag__C;
#ifdef ti_sysbios_knl_Task_moduleStateCheckFlag__CR
#define ti_sysbios_knl_Task_moduleStateCheckFlag (*((CT__ti_sysbios_knl_Task_moduleStateCheckFlag*)(xdcRomConstPtr + ti_sysbios_knl_Task_moduleStateCheckFlag__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_moduleStateCheckFlag__D
#define ti_sysbios_knl_Task_moduleStateCheckFlag (ti_sysbios_knl_Task_moduleStateCheckFlag__D)
#else
#define ti_sysbios_knl_Task_moduleStateCheckFlag (ti_sysbios_knl_Task_moduleStateCheckFlag__C)
#endif
#endif

/* objectCheckFxn */
typedef ti_sysbios_knl_Task_ObjectCheckFuncPtr CT__ti_sysbios_knl_Task_objectCheckFxn;
__extern __FAR__ const CT__ti_sysbios_knl_Task_objectCheckFxn ti_sysbios_knl_Task_objectCheckFxn__C;
#ifdef ti_sysbios_knl_Task_objectCheckFxn__CR
#define ti_sysbios_knl_Task_objectCheckFxn (*((CT__ti_sysbios_knl_Task_objectCheckFxn*)(xdcRomConstPtr + ti_sysbios_knl_Task_objectCheckFxn__C_offset)))
#else
#define ti_sysbios_knl_Task_objectCheckFxn (ti_sysbios_knl_Task_objectCheckFxn__C)
#endif

/* objectCheckValueFxn */
typedef ti_sysbios_knl_Task_ObjectCheckValueFuncPtr CT__ti_sysbios_knl_Task_objectCheckValueFxn;
__extern __FAR__ const CT__ti_sysbios_knl_Task_objectCheckValueFxn ti_sysbios_knl_Task_objectCheckValueFxn__C;
#ifdef ti_sysbios_knl_Task_objectCheckValueFxn__CR
#define ti_sysbios_knl_Task_objectCheckValueFxn (*((CT__ti_sysbios_knl_Task_objectCheckValueFxn*)(xdcRomConstPtr + ti_sysbios_knl_Task_objectCheckValueFxn__C_offset)))
#else
#define ti_sysbios_knl_Task_objectCheckValueFxn (ti_sysbios_knl_Task_objectCheckValueFxn__C)
#endif

/* objectCheckFlag */
typedef xdc_Bool CT__ti_sysbios_knl_Task_objectCheckFlag;
__extern __FAR__ const CT__ti_sysbios_knl_Task_objectCheckFlag ti_sysbios_knl_Task_objectCheckFlag__C;
#ifdef ti_sysbios_knl_Task_objectCheckFlag__CR
#define ti_sysbios_knl_Task_objectCheckFlag (*((CT__ti_sysbios_knl_Task_objectCheckFlag*)(xdcRomConstPtr + ti_sysbios_knl_Task_objectCheckFlag__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_objectCheckFlag__D
#define ti_sysbios_knl_Task_objectCheckFlag (ti_sysbios_knl_Task_objectCheckFlag__D)
#else
#define ti_sysbios_knl_Task_objectCheckFlag (ti_sysbios_knl_Task_objectCheckFlag__C)
#endif
#endif

/* numConstructedTasks */
typedef xdc_UInt CT__ti_sysbios_knl_Task_numConstructedTasks;
__extern __FAR__ const CT__ti_sysbios_knl_Task_numConstructedTasks ti_sysbios_knl_Task_numConstructedTasks__C;
#ifdef ti_sysbios_knl_Task_numConstructedTasks__CR
#define ti_sysbios_knl_Task_numConstructedTasks (*((CT__ti_sysbios_knl_Task_numConstructedTasks*)(xdcRomConstPtr + ti_sysbios_knl_Task_numConstructedTasks__C_offset)))
#else
#ifdef ti_sysbios_knl_Task_numConstructedTasks__D
#define ti_sysbios_knl_Task_numConstructedTasks (ti_sysbios_knl_Task_numConstructedTasks__D)
#else
#define ti_sysbios_knl_Task_numConstructedTasks (ti_sysbios_knl_Task_numConstructedTasks__C)
#endif
#endif

/* startupHookFunc */
typedef xdc_Void (*CT__ti_sysbios_knl_Task_startupHookFunc)(xdc_Void );
__extern __FAR__ const CT__ti_sysbios_knl_Task_startupHookFunc ti_sysbios_knl_Task_startupHookFunc__C;
#ifdef ti_sysbios_knl_Task_startupHookFunc__CR
#define ti_sysbios_knl_Task_startupHookFunc (*((CT__ti_sysbios_knl_Task_startupHookFunc*)(xdcRomConstPtr + ti_sysbios_knl_Task_startupHookFunc__C_offset)))
#else
#define ti_sysbios_knl_Task_startupHookFunc (ti_sysbios_knl_Task_startupHookFunc__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_knl_Task_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_UArg arg0;
    xdc_UArg arg1;
    xdc_Int priority;
    xdc_Ptr stack;
    xdc_SizeT stackSize;
    xdc_runtime_IHeap_Handle stackHeap;
    xdc_Ptr env;
    xdc_Bool vitalTaskFlag;
    xdc_UInt affinity;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_knl_Task_Struct {
    ti_sysbios_knl_Queue_Elem f0;
    volatile xdc_Int f1;
    xdc_UInt f2;
    xdc_Ptr f3;
    ti_sysbios_knl_Task_Mode f4;
    ti_sysbios_knl_Task_PendElem *f5;
    xdc_SizeT f6;
    __TA_ti_sysbios_knl_Task_Instance_State__stack f7;
    xdc_runtime_IHeap_Handle f8;
    ti_sysbios_knl_Task_FuncPtr f9;
    xdc_UArg f10;
    xdc_UArg f11;
    xdc_Ptr f12;
    __TA_ti_sysbios_knl_Task_Instance_State__hookEnv f13;
    xdc_Bool f14;
    ti_sysbios_knl_Queue_Handle f15;
    xdc_UInt f16;
    xdc_UInt f17;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_knl_Task_Module_startup ti_sysbios_knl_Task_Module_startup__E
xdc__CODESECT(ti_sysbios_knl_Task_Module_startup__E, "ti_sysbios_knl_Task_Module_startup")
__extern xdc_Int ti_sysbios_knl_Task_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_knl_Task_Module_startup__F, "ti_sysbios_knl_Task_Module_startup")
__extern xdc_Int ti_sysbios_knl_Task_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_knl_Task_Instance_init__E, "ti_sysbios_knl_Task_Instance_init")
__extern xdc_Int ti_sysbios_knl_Task_Instance_init__E(ti_sysbios_knl_Task_Object *obj, ti_sysbios_knl_Task_FuncPtr fxn, const ti_sysbios_knl_Task_Params *prms, xdc_runtime_Error_Block *eb);

/* Instance_finalize__E */
xdc__CODESECT(ti_sysbios_knl_Task_Instance_finalize__E, "ti_sysbios_knl_Task_Instance_finalize")
__extern void ti_sysbios_knl_Task_Instance_finalize__E(ti_sysbios_knl_Task_Object *obj, int ec);

/* create */
xdc__CODESECT(ti_sysbios_knl_Task_create, "ti_sysbios_knl_Task_create")
__extern ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_create( ti_sysbios_knl_Task_FuncPtr fxn, const ti_sysbios_knl_Task_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_knl_Task_construct, "ti_sysbios_knl_Task_construct")
__extern void ti_sysbios_knl_Task_construct(ti_sysbios_knl_Task_Struct *obj, ti_sysbios_knl_Task_FuncPtr fxn, const ti_sysbios_knl_Task_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_knl_Task_delete, "ti_sysbios_knl_Task_delete")
__extern void ti_sysbios_knl_Task_delete(ti_sysbios_knl_Task_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_knl_Task_destruct, "ti_sysbios_knl_Task_destruct")
__extern void ti_sysbios_knl_Task_destruct(ti_sysbios_knl_Task_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_knl_Task_Handle__label__S, "ti_sysbios_knl_Task_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_knl_Task_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_knl_Task_Module__startupDone__S, "ti_sysbios_knl_Task_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_knl_Task_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_knl_Task_Object__create__S, "ti_sysbios_knl_Task_Object__create__S")
__extern xdc_Ptr ti_sysbios_knl_Task_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_knl_Task_Object__delete__S, "ti_sysbios_knl_Task_Object__delete__S")
__extern xdc_Void ti_sysbios_knl_Task_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_knl_Task_Object__get__S, "ti_sysbios_knl_Task_Object__get__S")
__extern xdc_Ptr ti_sysbios_knl_Task_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_knl_Task_Object__first__S, "ti_sysbios_knl_Task_Object__first__S")
__extern xdc_Ptr ti_sysbios_knl_Task_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_knl_Task_Object__next__S, "ti_sysbios_knl_Task_Object__next__S")
__extern xdc_Ptr ti_sysbios_knl_Task_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_knl_Task_Params__init__S, "ti_sysbios_knl_Task_Params__init__S")
__extern xdc_Void ti_sysbios_knl_Task_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* startup__E */
#define ti_sysbios_knl_Task_startup ti_sysbios_knl_Task_startup__E
xdc__CODESECT(ti_sysbios_knl_Task_startup__E, "ti_sysbios_knl_Task_startup")
__extern xdc_Void ti_sysbios_knl_Task_startup__E( void);

/* enabled__E */
#define ti_sysbios_knl_Task_enabled ti_sysbios_knl_Task_enabled__E
xdc__CODESECT(ti_sysbios_knl_Task_enabled__E, "ti_sysbios_knl_Task_enabled")
__extern xdc_Bool ti_sysbios_knl_Task_enabled__E( void);

/* unlockSched__E */
#define ti_sysbios_knl_Task_unlockSched ti_sysbios_knl_Task_unlockSched__E
xdc__CODESECT(ti_sysbios_knl_Task_unlockSched__E, "ti_sysbios_knl_Task_unlockSched")
__extern xdc_Void ti_sysbios_knl_Task_unlockSched__E( void);

/* disable__E */
#define ti_sysbios_knl_Task_disable ti_sysbios_knl_Task_disable__E
xdc__CODESECT(ti_sysbios_knl_Task_disable__E, "ti_sysbios_knl_Task_disable")
__extern xdc_UInt ti_sysbios_knl_Task_disable__E( void);

/* enable__E */
#define ti_sysbios_knl_Task_enable ti_sysbios_knl_Task_enable__E
xdc__CODESECT(ti_sysbios_knl_Task_enable__E, "ti_sysbios_knl_Task_enable")
__extern xdc_Void ti_sysbios_knl_Task_enable__E( void);

/* restore__E */
#define ti_sysbios_knl_Task_restore ti_sysbios_knl_Task_restore__E
xdc__CODESECT(ti_sysbios_knl_Task_restore__E, "ti_sysbios_knl_Task_restore")
__extern xdc_Void ti_sysbios_knl_Task_restore__E( xdc_UInt key);

/* restoreHwi__E */
#define ti_sysbios_knl_Task_restoreHwi ti_sysbios_knl_Task_restoreHwi__E
xdc__CODESECT(ti_sysbios_knl_Task_restoreHwi__E, "ti_sysbios_knl_Task_restoreHwi")
__extern xdc_Void ti_sysbios_knl_Task_restoreHwi__E( xdc_UInt key);

/* self__E */
#define ti_sysbios_knl_Task_self ti_sysbios_knl_Task_self__E
xdc__CODESECT(ti_sysbios_knl_Task_self__E, "ti_sysbios_knl_Task_self")
__extern ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_self__E( void);

/* checkStacks__E */
#define ti_sysbios_knl_Task_checkStacks ti_sysbios_knl_Task_checkStacks__E
xdc__CODESECT(ti_sysbios_knl_Task_checkStacks__E, "ti_sysbios_knl_Task_checkStacks")
__extern xdc_Void ti_sysbios_knl_Task_checkStacks__E( ti_sysbios_knl_Task_Handle oldTask, ti_sysbios_knl_Task_Handle newTask);

/* exit__E */
#define ti_sysbios_knl_Task_exit ti_sysbios_knl_Task_exit__E
xdc__CODESECT(ti_sysbios_knl_Task_exit__E, "ti_sysbios_knl_Task_exit")
__extern xdc_Void ti_sysbios_knl_Task_exit__E( void);

/* sleep__E */
#define ti_sysbios_knl_Task_sleep ti_sysbios_knl_Task_sleep__E
xdc__CODESECT(ti_sysbios_knl_Task_sleep__E, "ti_sysbios_knl_Task_sleep")
__extern xdc_Void ti_sysbios_knl_Task_sleep__E( xdc_UInt32 nticks);

/* yield__E */
#define ti_sysbios_knl_Task_yield ti_sysbios_knl_Task_yield__E
xdc__CODESECT(ti_sysbios_knl_Task_yield__E, "ti_sysbios_knl_Task_yield")
__extern xdc_Void ti_sysbios_knl_Task_yield__E( void);

/* getIdleTask__E */
#define ti_sysbios_knl_Task_getIdleTask ti_sysbios_knl_Task_getIdleTask__E
xdc__CODESECT(ti_sysbios_knl_Task_getIdleTask__E, "ti_sysbios_knl_Task_getIdleTask")
__extern ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_getIdleTask__E( void);

/* getIdleTaskHandle__E */
#define ti_sysbios_knl_Task_getIdleTaskHandle ti_sysbios_knl_Task_getIdleTaskHandle__E
xdc__CODESECT(ti_sysbios_knl_Task_getIdleTaskHandle__E, "ti_sysbios_knl_Task_getIdleTaskHandle")
__extern ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_getIdleTaskHandle__E( xdc_UInt coreId);

/* startCore__E */
#define ti_sysbios_knl_Task_startCore ti_sysbios_knl_Task_startCore__E
xdc__CODESECT(ti_sysbios_knl_Task_startCore__E, "ti_sysbios_knl_Task_startCore")
__extern xdc_Void ti_sysbios_knl_Task_startCore__E( xdc_UInt coreId);

/* getArg0__E */
#define ti_sysbios_knl_Task_getArg0 ti_sysbios_knl_Task_getArg0__E
xdc__CODESECT(ti_sysbios_knl_Task_getArg0__E, "ti_sysbios_knl_Task_getArg0")
__extern xdc_UArg ti_sysbios_knl_Task_getArg0__E( ti_sysbios_knl_Task_Handle instp);

/* getArg1__E */
#define ti_sysbios_knl_Task_getArg1 ti_sysbios_knl_Task_getArg1__E
xdc__CODESECT(ti_sysbios_knl_Task_getArg1__E, "ti_sysbios_knl_Task_getArg1")
__extern xdc_UArg ti_sysbios_knl_Task_getArg1__E( ti_sysbios_knl_Task_Handle instp);

/* getEnv__E */
#define ti_sysbios_knl_Task_getEnv ti_sysbios_knl_Task_getEnv__E
xdc__CODESECT(ti_sysbios_knl_Task_getEnv__E, "ti_sysbios_knl_Task_getEnv")
__extern xdc_Ptr ti_sysbios_knl_Task_getEnv__E( ti_sysbios_knl_Task_Handle instp);

/* getFunc__E */
#define ti_sysbios_knl_Task_getFunc ti_sysbios_knl_Task_getFunc__E
xdc__CODESECT(ti_sysbios_knl_Task_getFunc__E, "ti_sysbios_knl_Task_getFunc")
__extern ti_sysbios_knl_Task_FuncPtr ti_sysbios_knl_Task_getFunc__E( ti_sysbios_knl_Task_Handle instp, xdc_UArg *arg0, xdc_UArg *arg1);

/* getHookContext__E */
#define ti_sysbios_knl_Task_getHookContext ti_sysbios_knl_Task_getHookContext__E
xdc__CODESECT(ti_sysbios_knl_Task_getHookContext__E, "ti_sysbios_knl_Task_getHookContext")
__extern xdc_Ptr ti_sysbios_knl_Task_getHookContext__E( ti_sysbios_knl_Task_Handle instp, xdc_Int id);

/* getPri__E */
#define ti_sysbios_knl_Task_getPri ti_sysbios_knl_Task_getPri__E
xdc__CODESECT(ti_sysbios_knl_Task_getPri__E, "ti_sysbios_knl_Task_getPri")
__extern xdc_Int ti_sysbios_knl_Task_getPri__E( ti_sysbios_knl_Task_Handle instp);

/* setArg0__E */
#define ti_sysbios_knl_Task_setArg0 ti_sysbios_knl_Task_setArg0__E
xdc__CODESECT(ti_sysbios_knl_Task_setArg0__E, "ti_sysbios_knl_Task_setArg0")
__extern xdc_Void ti_sysbios_knl_Task_setArg0__E( ti_sysbios_knl_Task_Handle instp, xdc_UArg arg);

/* setArg1__E */
#define ti_sysbios_knl_Task_setArg1 ti_sysbios_knl_Task_setArg1__E
xdc__CODESECT(ti_sysbios_knl_Task_setArg1__E, "ti_sysbios_knl_Task_setArg1")
__extern xdc_Void ti_sysbios_knl_Task_setArg1__E( ti_sysbios_knl_Task_Handle instp, xdc_UArg arg);

/* setEnv__E */
#define ti_sysbios_knl_Task_setEnv ti_sysbios_knl_Task_setEnv__E
xdc__CODESECT(ti_sysbios_knl_Task_setEnv__E, "ti_sysbios_knl_Task_setEnv")
__extern xdc_Void ti_sysbios_knl_Task_setEnv__E( ti_sysbios_knl_Task_Handle instp, xdc_Ptr env);

/* setHookContext__E */
#define ti_sysbios_knl_Task_setHookContext ti_sysbios_knl_Task_setHookContext__E
xdc__CODESECT(ti_sysbios_knl_Task_setHookContext__E, "ti_sysbios_knl_Task_setHookContext")
__extern xdc_Void ti_sysbios_knl_Task_setHookContext__E( ti_sysbios_knl_Task_Handle instp, xdc_Int id, xdc_Ptr hookContext);

/* setPri__E */
#define ti_sysbios_knl_Task_setPri ti_sysbios_knl_Task_setPri__E
xdc__CODESECT(ti_sysbios_knl_Task_setPri__E, "ti_sysbios_knl_Task_setPri")
__extern xdc_Int ti_sysbios_knl_Task_setPri__E( ti_sysbios_knl_Task_Handle instp, xdc_Int newpri);

/* stat__E */
#define ti_sysbios_knl_Task_stat ti_sysbios_knl_Task_stat__E
xdc__CODESECT(ti_sysbios_knl_Task_stat__E, "ti_sysbios_knl_Task_stat")
__extern xdc_Void ti_sysbios_knl_Task_stat__E( ti_sysbios_knl_Task_Handle instp, ti_sysbios_knl_Task_Stat *statbuf);

/* getMode__E */
#define ti_sysbios_knl_Task_getMode ti_sysbios_knl_Task_getMode__E
xdc__CODESECT(ti_sysbios_knl_Task_getMode__E, "ti_sysbios_knl_Task_getMode")
__extern ti_sysbios_knl_Task_Mode ti_sysbios_knl_Task_getMode__E( ti_sysbios_knl_Task_Handle instp);

/* setAffinity__E */
#define ti_sysbios_knl_Task_setAffinity ti_sysbios_knl_Task_setAffinity__E
xdc__CODESECT(ti_sysbios_knl_Task_setAffinity__E, "ti_sysbios_knl_Task_setAffinity")
__extern xdc_UInt ti_sysbios_knl_Task_setAffinity__E( ti_sysbios_knl_Task_Handle instp, xdc_UInt coreId);

/* getAffinity__E */
#define ti_sysbios_knl_Task_getAffinity ti_sysbios_knl_Task_getAffinity__E
xdc__CODESECT(ti_sysbios_knl_Task_getAffinity__E, "ti_sysbios_knl_Task_getAffinity")
__extern xdc_UInt ti_sysbios_knl_Task_getAffinity__E( ti_sysbios_knl_Task_Handle instp);

/* block__E */
#define ti_sysbios_knl_Task_block ti_sysbios_knl_Task_block__E
xdc__CODESECT(ti_sysbios_knl_Task_block__E, "ti_sysbios_knl_Task_block")
__extern xdc_Void ti_sysbios_knl_Task_block__E( ti_sysbios_knl_Task_Handle instp);

/* unblock__E */
#define ti_sysbios_knl_Task_unblock ti_sysbios_knl_Task_unblock__E
xdc__CODESECT(ti_sysbios_knl_Task_unblock__E, "ti_sysbios_knl_Task_unblock")
__extern xdc_Void ti_sysbios_knl_Task_unblock__E( ti_sysbios_knl_Task_Handle instp);

/* blockI__E */
#define ti_sysbios_knl_Task_blockI ti_sysbios_knl_Task_blockI__E
xdc__CODESECT(ti_sysbios_knl_Task_blockI__E, "ti_sysbios_knl_Task_blockI")
__extern xdc_Void ti_sysbios_knl_Task_blockI__E( ti_sysbios_knl_Task_Handle instp);

/* unblockI__E */
#define ti_sysbios_knl_Task_unblockI ti_sysbios_knl_Task_unblockI__E
xdc__CODESECT(ti_sysbios_knl_Task_unblockI__E, "ti_sysbios_knl_Task_unblockI")
__extern xdc_Void ti_sysbios_knl_Task_unblockI__E( ti_sysbios_knl_Task_Handle instp, xdc_UInt hwiKey);

/* schedule__I */
#define ti_sysbios_knl_Task_schedule ti_sysbios_knl_Task_schedule__I
xdc__CODESECT(ti_sysbios_knl_Task_schedule__I, "ti_sysbios_knl_Task_schedule")
__extern xdc_Void ti_sysbios_knl_Task_schedule__I( void);

/* enter__I */
#define ti_sysbios_knl_Task_enter ti_sysbios_knl_Task_enter__I
xdc__CODESECT(ti_sysbios_knl_Task_enter__I, "ti_sysbios_knl_Task_enter")
__extern xdc_Void ti_sysbios_knl_Task_enter__I( void);

/* sleepTimeout__I */
#define ti_sysbios_knl_Task_sleepTimeout ti_sysbios_knl_Task_sleepTimeout__I
xdc__CODESECT(ti_sysbios_knl_Task_sleepTimeout__I, "ti_sysbios_knl_Task_sleepTimeout")
__extern xdc_Void ti_sysbios_knl_Task_sleepTimeout__I( xdc_UArg arg);

/* postInit__I */
#define ti_sysbios_knl_Task_postInit ti_sysbios_knl_Task_postInit__I
xdc__CODESECT(ti_sysbios_knl_Task_postInit__I, "ti_sysbios_knl_Task_postInit")
__extern xdc_Int ti_sysbios_knl_Task_postInit__I( ti_sysbios_knl_Task_Object *task, xdc_runtime_Error_Block *eb);

/* allBlockedFunction__I */
#define ti_sysbios_knl_Task_allBlockedFunction ti_sysbios_knl_Task_allBlockedFunction__I
xdc__CODESECT(ti_sysbios_knl_Task_allBlockedFunction__I, "ti_sysbios_knl_Task_allBlockedFunction")
__extern xdc_Void ti_sysbios_knl_Task_allBlockedFunction__I( void);

/* deleteTerminatedTasksFunc__I */
#define ti_sysbios_knl_Task_deleteTerminatedTasksFunc ti_sysbios_knl_Task_deleteTerminatedTasksFunc__I
xdc__CODESECT(ti_sysbios_knl_Task_deleteTerminatedTasksFunc__I, "ti_sysbios_knl_Task_deleteTerminatedTasksFunc")
__extern xdc_Void ti_sysbios_knl_Task_deleteTerminatedTasksFunc__I( void);

/* processVitalTaskFlag__I */
#define ti_sysbios_knl_Task_processVitalTaskFlag ti_sysbios_knl_Task_processVitalTaskFlag__I
xdc__CODESECT(ti_sysbios_knl_Task_processVitalTaskFlag__I, "ti_sysbios_knl_Task_processVitalTaskFlag")
__extern xdc_Void ti_sysbios_knl_Task_processVitalTaskFlag__I( ti_sysbios_knl_Task_Object *task);

/* moduleStateCheck__I */
#define ti_sysbios_knl_Task_moduleStateCheck ti_sysbios_knl_Task_moduleStateCheck__I
xdc__CODESECT(ti_sysbios_knl_Task_moduleStateCheck__I, "ti_sysbios_knl_Task_moduleStateCheck")
__extern xdc_Int ti_sysbios_knl_Task_moduleStateCheck__I( ti_sysbios_knl_Task_Module_State *moduleState, xdc_UInt32 checkValue);

/* getModuleStateCheckValue__I */
#define ti_sysbios_knl_Task_getModuleStateCheckValue ti_sysbios_knl_Task_getModuleStateCheckValue__I
xdc__CODESECT(ti_sysbios_knl_Task_getModuleStateCheckValue__I, "ti_sysbios_knl_Task_getModuleStateCheckValue")
__extern xdc_UInt32 ti_sysbios_knl_Task_getModuleStateCheckValue__I( ti_sysbios_knl_Task_Module_State *moduleState);

/* objectCheck__I */
#define ti_sysbios_knl_Task_objectCheck ti_sysbios_knl_Task_objectCheck__I
xdc__CODESECT(ti_sysbios_knl_Task_objectCheck__I, "ti_sysbios_knl_Task_objectCheck")
__extern xdc_Int ti_sysbios_knl_Task_objectCheck__I( ti_sysbios_knl_Task_Handle handle, xdc_UInt32 checkValue);

/* getObjectCheckValue__I */
#define ti_sysbios_knl_Task_getObjectCheckValue ti_sysbios_knl_Task_getObjectCheckValue__I
xdc__CODESECT(ti_sysbios_knl_Task_getObjectCheckValue__I, "ti_sysbios_knl_Task_getObjectCheckValue")
__extern xdc_UInt32 ti_sysbios_knl_Task_getObjectCheckValue__I( ti_sysbios_knl_Task_Handle handle);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_knl_Task_Module_startupDone() ti_sysbios_knl_Task_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_knl_Task_Object_heap() ti_sysbios_knl_Task_Object__heap__C

/* Module_heap */
#define ti_sysbios_knl_Task_Module_heap() ti_sysbios_knl_Task_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_knl_Task_Module__id ti_sysbios_knl_Task_Module_id(void);
static inline CT__ti_sysbios_knl_Task_Module__id ti_sysbios_knl_Task_Module_id( void ) 
{
    return ti_sysbios_knl_Task_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_knl_Task_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_knl_Task_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_knl_Task_Module__diagsMask__C != (CT__ti_sysbios_knl_Task_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_knl_Task_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_knl_Task_Module_getMask(void)
{
    return (ti_sysbios_knl_Task_Module__diagsMask__C != (CT__ti_sysbios_knl_Task_Module__diagsMask)NULL) ? *ti_sysbios_knl_Task_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_knl_Task_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_knl_Task_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_knl_Task_Module__diagsMask__C != (CT__ti_sysbios_knl_Task_Module__diagsMask)NULL) {
        *ti_sysbios_knl_Task_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_knl_Task_Params_init(ti_sysbios_knl_Task_Params *prms);
static inline void ti_sysbios_knl_Task_Params_init( ti_sysbios_knl_Task_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_knl_Task_Params__init__S(prms, NULL, sizeof(ti_sysbios_knl_Task_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_knl_Task_Params_copy(ti_sysbios_knl_Task_Params *dst, const ti_sysbios_knl_Task_Params *src);
static inline void ti_sysbios_knl_Task_Params_copy(ti_sysbios_knl_Task_Params *dst, const ti_sysbios_knl_Task_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_knl_Task_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_knl_Task_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_knl_Task_Object_count() ti_sysbios_knl_Task_Object__count__C

/* Object_sizeof */
#define ti_sysbios_knl_Task_Object_sizeof() ti_sysbios_knl_Task_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_get(ti_sysbios_knl_Task_Object *oarr, int i);
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_get(ti_sysbios_knl_Task_Object *oarr, int i) 
{
    return (ti_sysbios_knl_Task_Handle)ti_sysbios_knl_Task_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_first(void);
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_first(void)
{
    return (ti_sysbios_knl_Task_Handle)ti_sysbios_knl_Task_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_next(ti_sysbios_knl_Task_Object *obj);
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_Object_next(ti_sysbios_knl_Task_Object *obj)
{
    return (ti_sysbios_knl_Task_Handle)ti_sysbios_knl_Task_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_knl_Task_Handle_label(ti_sysbios_knl_Task_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_knl_Task_Handle_label(ti_sysbios_knl_Task_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_knl_Task_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_knl_Task_Handle_name(ti_sysbios_knl_Task_Handle inst);
static inline xdc_String ti_sysbios_knl_Task_Handle_name(ti_sysbios_knl_Task_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_knl_Task_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_handle(ti_sysbios_knl_Task_Struct *str);
static inline ti_sysbios_knl_Task_Handle ti_sysbios_knl_Task_handle(ti_sysbios_knl_Task_Struct *str)
{
    return (ti_sysbios_knl_Task_Handle)str;
}

/* struct */
static inline ti_sysbios_knl_Task_Struct *ti_sysbios_knl_Task_struct(ti_sysbios_knl_Task_Handle inst);
static inline ti_sysbios_knl_Task_Struct *ti_sysbios_knl_Task_struct(ti_sysbios_knl_Task_Handle inst)
{
    return (ti_sysbios_knl_Task_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/sysbios/knl/Task__epilogue.h>

#ifdef ti_sysbios_knl_Task__top__
#undef __nested__
#endif

#endif /* ti_sysbios_knl_Task__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_knl_Task__internalaccess))

#ifndef ti_sysbios_knl_Task__include_state
#define ti_sysbios_knl_Task__include_state

/* Module_State */
struct ti_sysbios_knl_Task_Module_State {
    volatile xdc_Bool locked;
    volatile xdc_UInt curSet;
    volatile xdc_Bool workFlag;
    xdc_UInt vitalTasks;
    ti_sysbios_knl_Task_Handle curTask;
    ti_sysbios_knl_Queue_Handle curQ;
    __TA_ti_sysbios_knl_Task_Module_State__readyQ readyQ;
    __TA_ti_sysbios_knl_Task_Module_State__smpCurSet smpCurSet;
    __TA_ti_sysbios_knl_Task_Module_State__smpCurMask smpCurMask;
    __TA_ti_sysbios_knl_Task_Module_State__smpCurTask smpCurTask;
    __TA_ti_sysbios_knl_Task_Module_State__smpReadyQ smpReadyQ;
    __TA_ti_sysbios_knl_Task_Module_State__idleTask idleTask;
    __TA_ti_sysbios_knl_Task_Module_State__constructedTasks constructedTasks;
    char dummy;
};

/* Module__state__V */
#ifndef ti_sysbios_knl_Task_Module__state__VR
extern struct ti_sysbios_knl_Task_Module_State__ ti_sysbios_knl_Task_Module__state__V;
#else
#define ti_sysbios_knl_Task_Module__state__V (*((struct ti_sysbios_knl_Task_Module_State__*)(xdcRomStatePtr + ti_sysbios_knl_Task_Module__state__V_offset)))
#endif

/* Object */
struct ti_sysbios_knl_Task_Object {
    ti_sysbios_knl_Queue_Elem qElem;
    volatile xdc_Int priority;
    xdc_UInt mask;
    xdc_Ptr context;
    ti_sysbios_knl_Task_Mode mode;
    ti_sysbios_knl_Task_PendElem *pendElem;
    xdc_SizeT stackSize;
    __TA_ti_sysbios_knl_Task_Instance_State__stack stack;
    xdc_runtime_IHeap_Handle stackHeap;
    ti_sysbios_knl_Task_FuncPtr fxn;
    xdc_UArg arg0;
    xdc_UArg arg1;
    xdc_Ptr env;
    __TA_ti_sysbios_knl_Task_Instance_State__hookEnv hookEnv;
    xdc_Bool vitalTaskFlag;
    ti_sysbios_knl_Queue_Handle readyQ;
    xdc_UInt curCoreId;
    xdc_UInt affinity;
};

/* Module_State_inactiveQ */
#ifndef ti_sysbios_knl_Task_Module_State_inactiveQ__OR
__extern __FAR__ const xdc_SizeT ti_sysbios_knl_Task_Module_State_inactiveQ__O;
#endif
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_knl_Task_Module_State_inactiveQ(void);
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_knl_Task_Module_State_inactiveQ(void)
{
    return (ti_sysbios_knl_Queue_Handle)(((char*)&ti_sysbios_knl_Task_Module__state__V) + ti_sysbios_knl_Task_Module_State_inactiveQ__O);
}

/* Module_State_terminatedQ */
#ifndef ti_sysbios_knl_Task_Module_State_terminatedQ__OR
__extern __FAR__ const xdc_SizeT ti_sysbios_knl_Task_Module_State_terminatedQ__O;
#endif
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_knl_Task_Module_State_terminatedQ(void);
static inline ti_sysbios_knl_Queue_Handle ti_sysbios_knl_Task_Module_State_terminatedQ(void)
{
    return (ti_sysbios_knl_Queue_Handle)(((char*)&ti_sysbios_knl_Task_Module__state__V) + ti_sysbios_knl_Task_Module_State_terminatedQ__O);
}

#endif /* ti_sysbios_knl_Task__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_knl_Task__nolocalnames)

#ifndef ti_sysbios_knl_Task__localnames__done
#define ti_sysbios_knl_Task__localnames__done

/* module prefix */
#define Task_Instance ti_sysbios_knl_Task_Instance
#define Task_Handle ti_sysbios_knl_Task_Handle
#define Task_Module ti_sysbios_knl_Task_Module
#define Task_Object ti_sysbios_knl_Task_Object
#define Task_Struct ti_sysbios_knl_Task_Struct
#define Task_FuncPtr ti_sysbios_knl_Task_FuncPtr
#define Task_AllBlockedFuncPtr ti_sysbios_knl_Task_AllBlockedFuncPtr
#define Task_ModStateCheckValueFuncPtr ti_sysbios_knl_Task_ModStateCheckValueFuncPtr
#define Task_ModStateCheckFuncPtr ti_sysbios_knl_Task_ModStateCheckFuncPtr
#define Task_ObjectCheckValueFuncPtr ti_sysbios_knl_Task_ObjectCheckValueFuncPtr
#define Task_ObjectCheckFuncPtr ti_sysbios_knl_Task_ObjectCheckFuncPtr
#define Task_Mode ti_sysbios_knl_Task_Mode
#define Task_Stat ti_sysbios_knl_Task_Stat
#define Task_HookSet ti_sysbios_knl_Task_HookSet
#define Task_AFFINITY_NONE ti_sysbios_knl_Task_AFFINITY_NONE
#define Task_PendElem ti_sysbios_knl_Task_PendElem
#define Task_Instance_State ti_sysbios_knl_Task_Instance_State
#define Task_Module_State ti_sysbios_knl_Task_Module_State
#define Task_RunQEntry ti_sysbios_knl_Task_RunQEntry
#define Task_Module_StateSmp ti_sysbios_knl_Task_Module_StateSmp
#define Task_Mode_RUNNING ti_sysbios_knl_Task_Mode_RUNNING
#define Task_Mode_READY ti_sysbios_knl_Task_Mode_READY
#define Task_Mode_BLOCKED ti_sysbios_knl_Task_Mode_BLOCKED
#define Task_Mode_TERMINATED ti_sysbios_knl_Task_Mode_TERMINATED
#define Task_Mode_INACTIVE ti_sysbios_knl_Task_Mode_INACTIVE
#define Task_LM_switch ti_sysbios_knl_Task_LM_switch
#define Task_LM_sleep ti_sysbios_knl_Task_LM_sleep
#define Task_LD_ready ti_sysbios_knl_Task_LD_ready
#define Task_LD_block ti_sysbios_knl_Task_LD_block
#define Task_LM_yield ti_sysbios_knl_Task_LM_yield
#define Task_LM_setPri ti_sysbios_knl_Task_LM_setPri
#define Task_LD_exit ti_sysbios_knl_Task_LD_exit
#define Task_LM_setAffinity ti_sysbios_knl_Task_LM_setAffinity
#define Task_LM_schedule ti_sysbios_knl_Task_LM_schedule
#define Task_LM_noWork ti_sysbios_knl_Task_LM_noWork
#define Task_E_stackOverflow ti_sysbios_knl_Task_E_stackOverflow
#define Task_E_spOutOfBounds ti_sysbios_knl_Task_E_spOutOfBounds
#define Task_E_deleteNotAllowed ti_sysbios_knl_Task_E_deleteNotAllowed
#define Task_E_moduleStateCheckFailed ti_sysbios_knl_Task_E_moduleStateCheckFailed
#define Task_E_objectCheckFailed ti_sysbios_knl_Task_E_objectCheckFailed
#define Task_A_badThreadType ti_sysbios_knl_Task_A_badThreadType
#define Task_A_badTaskState ti_sysbios_knl_Task_A_badTaskState
#define Task_A_noPendElem ti_sysbios_knl_Task_A_noPendElem
#define Task_A_taskDisabled ti_sysbios_knl_Task_A_taskDisabled
#define Task_A_badPriority ti_sysbios_knl_Task_A_badPriority
#define Task_A_badTimeout ti_sysbios_knl_Task_A_badTimeout
#define Task_A_badAffinity ti_sysbios_knl_Task_A_badAffinity
#define Task_A_sleepTaskDisabled ti_sysbios_knl_Task_A_sleepTaskDisabled
#define Task_A_invalidCoreId ti_sysbios_knl_Task_A_invalidCoreId
#define Task_numPriorities ti_sysbios_knl_Task_numPriorities
#define Task_defaultStackSize ti_sysbios_knl_Task_defaultStackSize
#define Task_defaultStackHeap ti_sysbios_knl_Task_defaultStackHeap
#define Task_allBlockedFunc ti_sysbios_knl_Task_allBlockedFunc
#define Task_initStackFlag ti_sysbios_knl_Task_initStackFlag
#define Task_checkStackFlag ti_sysbios_knl_Task_checkStackFlag
#define Task_deleteTerminatedTasks ti_sysbios_knl_Task_deleteTerminatedTasks
#define Task_hooks ti_sysbios_knl_Task_hooks
#define Task_moduleStateCheckFxn ti_sysbios_knl_Task_moduleStateCheckFxn
#define Task_moduleStateCheckValueFxn ti_sysbios_knl_Task_moduleStateCheckValueFxn
#define Task_moduleStateCheckFlag ti_sysbios_knl_Task_moduleStateCheckFlag
#define Task_objectCheckFxn ti_sysbios_knl_Task_objectCheckFxn
#define Task_objectCheckValueFxn ti_sysbios_knl_Task_objectCheckValueFxn
#define Task_objectCheckFlag ti_sysbios_knl_Task_objectCheckFlag
#define Task_numConstructedTasks ti_sysbios_knl_Task_numConstructedTasks
#define Task_startupHookFunc ti_sysbios_knl_Task_startupHookFunc
#define Task_Module_State_inactiveQ ti_sysbios_knl_Task_Module_State_inactiveQ
#define Task_Module_State_terminatedQ ti_sysbios_knl_Task_Module_State_terminatedQ
#define Task_Params ti_sysbios_knl_Task_Params
#define Task_startup ti_sysbios_knl_Task_startup
#define Task_enabled ti_sysbios_knl_Task_enabled
#define Task_unlockSched ti_sysbios_knl_Task_unlockSched
#define Task_disable ti_sysbios_knl_Task_disable
#define Task_enable ti_sysbios_knl_Task_enable
#define Task_restore ti_sysbios_knl_Task_restore
#define Task_restoreHwi ti_sysbios_knl_Task_restoreHwi
#define Task_self ti_sysbios_knl_Task_self
#define Task_selfMacro ti_sysbios_knl_Task_selfMacro
#define Task_checkStacks ti_sysbios_knl_Task_checkStacks
#define Task_exit ti_sysbios_knl_Task_exit
#define Task_sleep ti_sysbios_knl_Task_sleep
#define Task_yield ti_sysbios_knl_Task_yield
#define Task_getIdleTask ti_sysbios_knl_Task_getIdleTask
#define Task_getIdleTaskHandle ti_sysbios_knl_Task_getIdleTaskHandle
#define Task_startCore ti_sysbios_knl_Task_startCore
#define Task_getArg0 ti_sysbios_knl_Task_getArg0
#define Task_getArg1 ti_sysbios_knl_Task_getArg1
#define Task_getEnv ti_sysbios_knl_Task_getEnv
#define Task_getFunc ti_sysbios_knl_Task_getFunc
#define Task_getHookContext ti_sysbios_knl_Task_getHookContext
#define Task_getPri ti_sysbios_knl_Task_getPri
#define Task_setArg0 ti_sysbios_knl_Task_setArg0
#define Task_setArg1 ti_sysbios_knl_Task_setArg1
#define Task_setEnv ti_sysbios_knl_Task_setEnv
#define Task_setHookContext ti_sysbios_knl_Task_setHookContext
#define Task_setPri ti_sysbios_knl_Task_setPri
#define Task_stat ti_sysbios_knl_Task_stat
#define Task_getMode ti_sysbios_knl_Task_getMode
#define Task_setAffinity ti_sysbios_knl_Task_setAffinity
#define Task_getAffinity ti_sysbios_knl_Task_getAffinity
#define Task_block ti_sysbios_knl_Task_block
#define Task_unblock ti_sysbios_knl_Task_unblock
#define Task_blockI ti_sysbios_knl_Task_blockI
#define Task_unblockI ti_sysbios_knl_Task_unblockI
#define Task_Module_name ti_sysbios_knl_Task_Module_name
#define Task_Module_id ti_sysbios_knl_Task_Module_id
#define Task_Module_startup ti_sysbios_knl_Task_Module_startup
#define Task_Module_startupDone ti_sysbios_knl_Task_Module_startupDone
#define Task_Module_hasMask ti_sysbios_knl_Task_Module_hasMask
#define Task_Module_getMask ti_sysbios_knl_Task_Module_getMask
#define Task_Module_setMask ti_sysbios_knl_Task_Module_setMask
#define Task_Object_heap ti_sysbios_knl_Task_Object_heap
#define Task_Module_heap ti_sysbios_knl_Task_Module_heap
#define Task_construct ti_sysbios_knl_Task_construct
#define Task_create ti_sysbios_knl_Task_create
#define Task_handle ti_sysbios_knl_Task_handle
#define Task_struct ti_sysbios_knl_Task_struct
#define Task_Handle_label ti_sysbios_knl_Task_Handle_label
#define Task_Handle_name ti_sysbios_knl_Task_Handle_name
#define Task_Instance_init ti_sysbios_knl_Task_Instance_init
#define Task_Object_count ti_sysbios_knl_Task_Object_count
#define Task_Object_get ti_sysbios_knl_Task_Object_get
#define Task_Object_first ti_sysbios_knl_Task_Object_first
#define Task_Object_next ti_sysbios_knl_Task_Object_next
#define Task_Object_sizeof ti_sysbios_knl_Task_Object_sizeof
#define Task_Params_copy ti_sysbios_knl_Task_Params_copy
#define Task_Params_init ti_sysbios_knl_Task_Params_init
#define Task_Instance_finalize ti_sysbios_knl_Task_Instance_finalize
#define Task_delete ti_sysbios_knl_Task_delete
#define Task_destruct ti_sysbios_knl_Task_destruct

/* proxies */
#include <ti/sysbios/knl/package/Task_SupportProxy.h>

#endif /* ti_sysbios_knl_Task__localnames__done */
#endif
