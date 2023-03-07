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

#ifndef ti_sysbios_hal_TimerNull__include
#define ti_sysbios_hal_TimerNull__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_hal_TimerNull__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_hal_TimerNull___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/hal/package/package.defs.h>

#include <ti/sysbios/interfaces/ITimer.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef ti_sysbios_interfaces_ITimer_FuncPtr ti_sysbios_hal_TimerNull_FuncPtr;

/* ANY */
#define ti_sysbios_hal_TimerNull_ANY (~0)

/* StartMode */
typedef ti_sysbios_interfaces_ITimer_StartMode ti_sysbios_hal_TimerNull_StartMode;

/* RunMode */
typedef ti_sysbios_interfaces_ITimer_RunMode ti_sysbios_hal_TimerNull_RunMode;

/* Status */
typedef ti_sysbios_interfaces_ITimer_Status ti_sysbios_hal_TimerNull_Status;

/* PeriodType */
typedef ti_sysbios_interfaces_ITimer_PeriodType ti_sysbios_hal_TimerNull_PeriodType;

/* StartMode_AUTO */
#define ti_sysbios_hal_TimerNull_StartMode_AUTO ti_sysbios_interfaces_ITimer_StartMode_AUTO

/* StartMode_USER */
#define ti_sysbios_hal_TimerNull_StartMode_USER ti_sysbios_interfaces_ITimer_StartMode_USER

/* RunMode_CONTINUOUS */
#define ti_sysbios_hal_TimerNull_RunMode_CONTINUOUS ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS

/* RunMode_ONESHOT */
#define ti_sysbios_hal_TimerNull_RunMode_ONESHOT ti_sysbios_interfaces_ITimer_RunMode_ONESHOT

/* RunMode_DYNAMIC */
#define ti_sysbios_hal_TimerNull_RunMode_DYNAMIC ti_sysbios_interfaces_ITimer_RunMode_DYNAMIC

/* Status_INUSE */
#define ti_sysbios_hal_TimerNull_Status_INUSE ti_sysbios_interfaces_ITimer_Status_INUSE

/* Status_FREE */
#define ti_sysbios_hal_TimerNull_Status_FREE ti_sysbios_interfaces_ITimer_Status_FREE

/* PeriodType_MICROSECS */
#define ti_sysbios_hal_TimerNull_PeriodType_MICROSECS ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS

/* PeriodType_COUNTS */
#define ti_sysbios_hal_TimerNull_PeriodType_COUNTS ti_sysbios_interfaces_ITimer_PeriodType_COUNTS


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_hal_TimerNull_Args__create {
    xdc_Int id;
    ti_sysbios_interfaces_ITimer_FuncPtr tickFxn;
} ti_sysbios_hal_TimerNull_Args__create;


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_hal_TimerNull_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__diagsEnabled ti_sysbios_hal_TimerNull_Module__diagsEnabled__C;
#ifdef ti_sysbios_hal_TimerNull_Module__diagsEnabled__CR
#define ti_sysbios_hal_TimerNull_Module__diagsEnabled__C (*((CT__ti_sysbios_hal_TimerNull_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__diagsEnabled (ti_sysbios_hal_TimerNull_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_hal_TimerNull_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__diagsIncluded ti_sysbios_hal_TimerNull_Module__diagsIncluded__C;
#ifdef ti_sysbios_hal_TimerNull_Module__diagsIncluded__CR
#define ti_sysbios_hal_TimerNull_Module__diagsIncluded__C (*((CT__ti_sysbios_hal_TimerNull_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__diagsIncluded (ti_sysbios_hal_TimerNull_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_hal_TimerNull_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__diagsMask ti_sysbios_hal_TimerNull_Module__diagsMask__C;
#ifdef ti_sysbios_hal_TimerNull_Module__diagsMask__CR
#define ti_sysbios_hal_TimerNull_Module__diagsMask__C (*((CT__ti_sysbios_hal_TimerNull_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__diagsMask (ti_sysbios_hal_TimerNull_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_hal_TimerNull_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__gateObj ti_sysbios_hal_TimerNull_Module__gateObj__C;
#ifdef ti_sysbios_hal_TimerNull_Module__gateObj__CR
#define ti_sysbios_hal_TimerNull_Module__gateObj__C (*((CT__ti_sysbios_hal_TimerNull_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__gateObj__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__gateObj (ti_sysbios_hal_TimerNull_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_hal_TimerNull_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__gatePrms ti_sysbios_hal_TimerNull_Module__gatePrms__C;
#ifdef ti_sysbios_hal_TimerNull_Module__gatePrms__CR
#define ti_sysbios_hal_TimerNull_Module__gatePrms__C (*((CT__ti_sysbios_hal_TimerNull_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__gatePrms (ti_sysbios_hal_TimerNull_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_hal_TimerNull_Module__id;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__id ti_sysbios_hal_TimerNull_Module__id__C;
#ifdef ti_sysbios_hal_TimerNull_Module__id__CR
#define ti_sysbios_hal_TimerNull_Module__id__C (*((CT__ti_sysbios_hal_TimerNull_Module__id*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__id__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__id (ti_sysbios_hal_TimerNull_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_hal_TimerNull_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerDefined ti_sysbios_hal_TimerNull_Module__loggerDefined__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerDefined__CR
#define ti_sysbios_hal_TimerNull_Module__loggerDefined__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerDefined (ti_sysbios_hal_TimerNull_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_hal_TimerNull_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerObj ti_sysbios_hal_TimerNull_Module__loggerObj__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerObj__CR
#define ti_sysbios_hal_TimerNull_Module__loggerObj__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerObj (ti_sysbios_hal_TimerNull_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_hal_TimerNull_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerFxn0 ti_sysbios_hal_TimerNull_Module__loggerFxn0__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerFxn0__CR
#define ti_sysbios_hal_TimerNull_Module__loggerFxn0__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerFxn0 (ti_sysbios_hal_TimerNull_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_hal_TimerNull_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerFxn1 ti_sysbios_hal_TimerNull_Module__loggerFxn1__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerFxn1__CR
#define ti_sysbios_hal_TimerNull_Module__loggerFxn1__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerFxn1 (ti_sysbios_hal_TimerNull_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_hal_TimerNull_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerFxn2 ti_sysbios_hal_TimerNull_Module__loggerFxn2__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerFxn2__CR
#define ti_sysbios_hal_TimerNull_Module__loggerFxn2__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerFxn2 (ti_sysbios_hal_TimerNull_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_hal_TimerNull_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerFxn4 ti_sysbios_hal_TimerNull_Module__loggerFxn4__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerFxn4__CR
#define ti_sysbios_hal_TimerNull_Module__loggerFxn4__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerFxn4 (ti_sysbios_hal_TimerNull_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_hal_TimerNull_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Module__loggerFxn8 ti_sysbios_hal_TimerNull_Module__loggerFxn8__C;
#ifdef ti_sysbios_hal_TimerNull_Module__loggerFxn8__CR
#define ti_sysbios_hal_TimerNull_Module__loggerFxn8__C (*((CT__ti_sysbios_hal_TimerNull_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Module__loggerFxn8 (ti_sysbios_hal_TimerNull_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_hal_TimerNull_Object__count;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Object__count ti_sysbios_hal_TimerNull_Object__count__C;
#ifdef ti_sysbios_hal_TimerNull_Object__count__CR
#define ti_sysbios_hal_TimerNull_Object__count__C (*((CT__ti_sysbios_hal_TimerNull_Object__count*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Object__count__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Object__count (ti_sysbios_hal_TimerNull_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_hal_TimerNull_Object__heap;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Object__heap ti_sysbios_hal_TimerNull_Object__heap__C;
#ifdef ti_sysbios_hal_TimerNull_Object__heap__CR
#define ti_sysbios_hal_TimerNull_Object__heap__C (*((CT__ti_sysbios_hal_TimerNull_Object__heap*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Object__heap__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Object__heap (ti_sysbios_hal_TimerNull_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_hal_TimerNull_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Object__sizeof ti_sysbios_hal_TimerNull_Object__sizeof__C;
#ifdef ti_sysbios_hal_TimerNull_Object__sizeof__CR
#define ti_sysbios_hal_TimerNull_Object__sizeof__C (*((CT__ti_sysbios_hal_TimerNull_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Object__sizeof__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Object__sizeof (ti_sysbios_hal_TimerNull_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_hal_TimerNull_Object__table;
__extern __FAR__ const CT__ti_sysbios_hal_TimerNull_Object__table ti_sysbios_hal_TimerNull_Object__table__C;
#ifdef ti_sysbios_hal_TimerNull_Object__table__CR
#define ti_sysbios_hal_TimerNull_Object__table__C (*((CT__ti_sysbios_hal_TimerNull_Object__table*)(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Object__table__C_offset)))
#else
#define ti_sysbios_hal_TimerNull_Object__table (ti_sysbios_hal_TimerNull_Object__table__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_hal_TimerNull_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    ti_sysbios_interfaces_ITimer_RunMode runMode;
    ti_sysbios_interfaces_ITimer_StartMode startMode;
    xdc_UArg arg;
    xdc_UInt32 period;
    ti_sysbios_interfaces_ITimer_PeriodType periodType;
    xdc_runtime_Types_FreqHz extFreq;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_hal_TimerNull_Struct {
    const ti_sysbios_hal_TimerNull_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_hal_TimerNull_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_UInt (*getNumTimers)(void);
    ti_sysbios_interfaces_ITimer_Status (*getStatus)(xdc_UInt id);
    xdc_Void (*startup)(void);
    xdc_UInt32 (*getMaxTicks)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_Void (*setNextTick)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UInt32 ticks);
    xdc_Void (*start)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_Void (*stop)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_Void (*setPeriod)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UInt32 period);
    xdc_Bool (*setPeriodMicroSecs)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UInt32 microsecs);
    xdc_UInt32 (*getPeriod)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_UInt32 (*getCount)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_Void (*getFreq)(ti_sysbios_hal_TimerNull_Handle inst, xdc_runtime_Types_FreqHz* freq);
    ti_sysbios_interfaces_ITimer_FuncPtr (*getFunc)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UArg* arg);
    xdc_Void (*setFunc)(ti_sysbios_hal_TimerNull_Handle inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg);
    xdc_Void (*trigger)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UInt32 cycles);
    xdc_UInt32 (*getExpiredCounts)(ti_sysbios_hal_TimerNull_Handle inst);
    xdc_UInt32 (*getExpiredTicks)(ti_sysbios_hal_TimerNull_Handle inst, xdc_UInt32 tickPeriod);
    xdc_UInt32 (*getCurrentTick)(ti_sysbios_hal_TimerNull_Handle inst, xdc_Bool save);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_hal_TimerNull_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_hal_TimerNull_Fxns__ ti_sysbios_hal_TimerNull_Module__FXNS__C;
#else
#define ti_sysbios_hal_TimerNull_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_hal_TimerNull_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_hal_TimerNull_Module_startup( state ) (-1)

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Instance_init__E, "ti_sysbios_hal_TimerNull_Instance_init")
__extern xdc_Int ti_sysbios_hal_TimerNull_Instance_init__E(ti_sysbios_hal_TimerNull_Object *obj, xdc_Int id, ti_sysbios_interfaces_ITimer_FuncPtr tickFxn, const ti_sysbios_hal_TimerNull_Params *prms, xdc_runtime_Error_Block *eb);

/* Instance_finalize__E */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Instance_finalize__E, "ti_sysbios_hal_TimerNull_Instance_finalize")
__extern void ti_sysbios_hal_TimerNull_Instance_finalize__E(ti_sysbios_hal_TimerNull_Object *obj, int ec);

/* create */
xdc__CODESECT(ti_sysbios_hal_TimerNull_create, "ti_sysbios_hal_TimerNull_create")
__extern ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_create( xdc_Int id, ti_sysbios_interfaces_ITimer_FuncPtr tickFxn, const ti_sysbios_hal_TimerNull_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_hal_TimerNull_construct, "ti_sysbios_hal_TimerNull_construct")
__extern void ti_sysbios_hal_TimerNull_construct(ti_sysbios_hal_TimerNull_Struct *obj, xdc_Int id, ti_sysbios_interfaces_ITimer_FuncPtr tickFxn, const ti_sysbios_hal_TimerNull_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_hal_TimerNull_delete, "ti_sysbios_hal_TimerNull_delete")
__extern void ti_sysbios_hal_TimerNull_delete(ti_sysbios_hal_TimerNull_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_hal_TimerNull_destruct, "ti_sysbios_hal_TimerNull_destruct")
__extern void ti_sysbios_hal_TimerNull_destruct(ti_sysbios_hal_TimerNull_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Handle__label__S, "ti_sysbios_hal_TimerNull_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_hal_TimerNull_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Module__startupDone__S, "ti_sysbios_hal_TimerNull_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_hal_TimerNull_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Object__create__S, "ti_sysbios_hal_TimerNull_Object__create__S")
__extern xdc_Ptr ti_sysbios_hal_TimerNull_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Object__delete__S, "ti_sysbios_hal_TimerNull_Object__delete__S")
__extern xdc_Void ti_sysbios_hal_TimerNull_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Object__get__S, "ti_sysbios_hal_TimerNull_Object__get__S")
__extern xdc_Ptr ti_sysbios_hal_TimerNull_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Object__first__S, "ti_sysbios_hal_TimerNull_Object__first__S")
__extern xdc_Ptr ti_sysbios_hal_TimerNull_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Object__next__S, "ti_sysbios_hal_TimerNull_Object__next__S")
__extern xdc_Ptr ti_sysbios_hal_TimerNull_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_hal_TimerNull_Params__init__S, "ti_sysbios_hal_TimerNull_Params__init__S")
__extern xdc_Void ti_sysbios_hal_TimerNull_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* getNumTimers__E */
#define ti_sysbios_hal_TimerNull_getNumTimers ti_sysbios_hal_TimerNull_getNumTimers__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getNumTimers__E, "ti_sysbios_hal_TimerNull_getNumTimers")
__extern xdc_UInt ti_sysbios_hal_TimerNull_getNumTimers__E( void);

/* getStatus__E */
#define ti_sysbios_hal_TimerNull_getStatus ti_sysbios_hal_TimerNull_getStatus__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getStatus__E, "ti_sysbios_hal_TimerNull_getStatus")
__extern ti_sysbios_interfaces_ITimer_Status ti_sysbios_hal_TimerNull_getStatus__E( xdc_UInt id);

/* startup__E */
#define ti_sysbios_hal_TimerNull_startup ti_sysbios_hal_TimerNull_startup__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_startup__E, "ti_sysbios_hal_TimerNull_startup")
__extern xdc_Void ti_sysbios_hal_TimerNull_startup__E( void);

/* getMaxTicks__E */
#define ti_sysbios_hal_TimerNull_getMaxTicks ti_sysbios_hal_TimerNull_getMaxTicks__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getMaxTicks__E, "ti_sysbios_hal_TimerNull_getMaxTicks")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getMaxTicks__E( ti_sysbios_hal_TimerNull_Handle instp);

/* setNextTick__E */
#define ti_sysbios_hal_TimerNull_setNextTick ti_sysbios_hal_TimerNull_setNextTick__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_setNextTick__E, "ti_sysbios_hal_TimerNull_setNextTick")
__extern xdc_Void ti_sysbios_hal_TimerNull_setNextTick__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UInt32 ticks);

/* start__E */
#define ti_sysbios_hal_TimerNull_start ti_sysbios_hal_TimerNull_start__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_start__E, "ti_sysbios_hal_TimerNull_start")
__extern xdc_Void ti_sysbios_hal_TimerNull_start__E( ti_sysbios_hal_TimerNull_Handle instp);

/* stop__E */
#define ti_sysbios_hal_TimerNull_stop ti_sysbios_hal_TimerNull_stop__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_stop__E, "ti_sysbios_hal_TimerNull_stop")
__extern xdc_Void ti_sysbios_hal_TimerNull_stop__E( ti_sysbios_hal_TimerNull_Handle instp);

/* setPeriod__E */
#define ti_sysbios_hal_TimerNull_setPeriod ti_sysbios_hal_TimerNull_setPeriod__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_setPeriod__E, "ti_sysbios_hal_TimerNull_setPeriod")
__extern xdc_Void ti_sysbios_hal_TimerNull_setPeriod__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UInt32 period);

/* setPeriodMicroSecs__E */
#define ti_sysbios_hal_TimerNull_setPeriodMicroSecs ti_sysbios_hal_TimerNull_setPeriodMicroSecs__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_setPeriodMicroSecs__E, "ti_sysbios_hal_TimerNull_setPeriodMicroSecs")
__extern xdc_Bool ti_sysbios_hal_TimerNull_setPeriodMicroSecs__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UInt32 microsecs);

/* getPeriod__E */
#define ti_sysbios_hal_TimerNull_getPeriod ti_sysbios_hal_TimerNull_getPeriod__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getPeriod__E, "ti_sysbios_hal_TimerNull_getPeriod")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getPeriod__E( ti_sysbios_hal_TimerNull_Handle instp);

/* getCount__E */
#define ti_sysbios_hal_TimerNull_getCount ti_sysbios_hal_TimerNull_getCount__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getCount__E, "ti_sysbios_hal_TimerNull_getCount")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getCount__E( ti_sysbios_hal_TimerNull_Handle instp);

/* getFreq__E */
#define ti_sysbios_hal_TimerNull_getFreq ti_sysbios_hal_TimerNull_getFreq__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getFreq__E, "ti_sysbios_hal_TimerNull_getFreq")
__extern xdc_Void ti_sysbios_hal_TimerNull_getFreq__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_runtime_Types_FreqHz *freq);

/* getFunc__E */
#define ti_sysbios_hal_TimerNull_getFunc ti_sysbios_hal_TimerNull_getFunc__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getFunc__E, "ti_sysbios_hal_TimerNull_getFunc")
__extern ti_sysbios_interfaces_ITimer_FuncPtr ti_sysbios_hal_TimerNull_getFunc__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UArg *arg);

/* setFunc__E */
#define ti_sysbios_hal_TimerNull_setFunc ti_sysbios_hal_TimerNull_setFunc__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_setFunc__E, "ti_sysbios_hal_TimerNull_setFunc")
__extern xdc_Void ti_sysbios_hal_TimerNull_setFunc__E( ti_sysbios_hal_TimerNull_Handle instp, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg);

/* trigger__E */
#define ti_sysbios_hal_TimerNull_trigger ti_sysbios_hal_TimerNull_trigger__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_trigger__E, "ti_sysbios_hal_TimerNull_trigger")
__extern xdc_Void ti_sysbios_hal_TimerNull_trigger__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UInt32 cycles);

/* getExpiredCounts__E */
#define ti_sysbios_hal_TimerNull_getExpiredCounts ti_sysbios_hal_TimerNull_getExpiredCounts__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getExpiredCounts__E, "ti_sysbios_hal_TimerNull_getExpiredCounts")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getExpiredCounts__E( ti_sysbios_hal_TimerNull_Handle instp);

/* getExpiredTicks__E */
#define ti_sysbios_hal_TimerNull_getExpiredTicks ti_sysbios_hal_TimerNull_getExpiredTicks__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getExpiredTicks__E, "ti_sysbios_hal_TimerNull_getExpiredTicks")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getExpiredTicks__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_UInt32 tickPeriod);

/* getCurrentTick__E */
#define ti_sysbios_hal_TimerNull_getCurrentTick ti_sysbios_hal_TimerNull_getCurrentTick__E
xdc__CODESECT(ti_sysbios_hal_TimerNull_getCurrentTick__E, "ti_sysbios_hal_TimerNull_getCurrentTick")
__extern xdc_UInt32 ti_sysbios_hal_TimerNull_getCurrentTick__E( ti_sysbios_hal_TimerNull_Handle instp, xdc_Bool save);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_sysbios_interfaces_ITimer_Module ti_sysbios_hal_TimerNull_Module_upCast(void);
static inline ti_sysbios_interfaces_ITimer_Module ti_sysbios_hal_TimerNull_Module_upCast(void)
{
    return (ti_sysbios_interfaces_ITimer_Module)&ti_sysbios_hal_TimerNull_Module__FXNS__C;
}

/* Module_to_ti_sysbios_interfaces_ITimer */
#define ti_sysbios_hal_TimerNull_Module_to_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Module_upCast

/* Handle_upCast */
static inline ti_sysbios_interfaces_ITimer_Handle ti_sysbios_hal_TimerNull_Handle_upCast(ti_sysbios_hal_TimerNull_Handle i);
static inline ti_sysbios_interfaces_ITimer_Handle ti_sysbios_hal_TimerNull_Handle_upCast(ti_sysbios_hal_TimerNull_Handle i)
{
    return (ti_sysbios_interfaces_ITimer_Handle)i;
}

/* Handle_to_ti_sysbios_interfaces_ITimer */
#define ti_sysbios_hal_TimerNull_Handle_to_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Handle_downCast(ti_sysbios_interfaces_ITimer_Handle i);
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Handle_downCast(ti_sysbios_interfaces_ITimer_Handle i)
{
    ti_sysbios_interfaces_ITimer_Handle i2 = (ti_sysbios_interfaces_ITimer_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_sysbios_hal_TimerNull_Module__FXNS__C) ? (ti_sysbios_hal_TimerNull_Handle)i : (ti_sysbios_hal_TimerNull_Handle)NULL;
}

/* Handle_from_ti_sysbios_interfaces_ITimer */
#define ti_sysbios_hal_TimerNull_Handle_from_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_hal_TimerNull_Module_startupDone() ti_sysbios_hal_TimerNull_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_hal_TimerNull_Object_heap() ti_sysbios_hal_TimerNull_Object__heap__C

/* Module_heap */
#define ti_sysbios_hal_TimerNull_Module_heap() ti_sysbios_hal_TimerNull_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_hal_TimerNull_Module__id ti_sysbios_hal_TimerNull_Module_id(void);
static inline CT__ti_sysbios_hal_TimerNull_Module__id ti_sysbios_hal_TimerNull_Module_id( void ) 
{
    return ti_sysbios_hal_TimerNull_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_hal_TimerNull_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_hal_TimerNull_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_hal_TimerNull_Module__diagsMask__C != (CT__ti_sysbios_hal_TimerNull_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_hal_TimerNull_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_hal_TimerNull_Module_getMask(void)
{
    return (ti_sysbios_hal_TimerNull_Module__diagsMask__C != (CT__ti_sysbios_hal_TimerNull_Module__diagsMask)NULL) ? *ti_sysbios_hal_TimerNull_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_hal_TimerNull_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_hal_TimerNull_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_hal_TimerNull_Module__diagsMask__C != (CT__ti_sysbios_hal_TimerNull_Module__diagsMask)NULL) {
        *ti_sysbios_hal_TimerNull_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_hal_TimerNull_Params_init(ti_sysbios_hal_TimerNull_Params *prms);
static inline void ti_sysbios_hal_TimerNull_Params_init( ti_sysbios_hal_TimerNull_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_hal_TimerNull_Params__init__S(prms, NULL, sizeof(ti_sysbios_hal_TimerNull_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_hal_TimerNull_Params_copy(ti_sysbios_hal_TimerNull_Params *dst, const ti_sysbios_hal_TimerNull_Params *src);
static inline void ti_sysbios_hal_TimerNull_Params_copy(ti_sysbios_hal_TimerNull_Params *dst, const ti_sysbios_hal_TimerNull_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_hal_TimerNull_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_hal_TimerNull_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_hal_TimerNull_Object_count() ti_sysbios_hal_TimerNull_Object__count__C

/* Object_sizeof */
#define ti_sysbios_hal_TimerNull_Object_sizeof() ti_sysbios_hal_TimerNull_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_get(ti_sysbios_hal_TimerNull_Object *oarr, int i);
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_get(ti_sysbios_hal_TimerNull_Object *oarr, int i) 
{
    return (ti_sysbios_hal_TimerNull_Handle)ti_sysbios_hal_TimerNull_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_first(void);
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_first(void)
{
    return (ti_sysbios_hal_TimerNull_Handle)ti_sysbios_hal_TimerNull_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_next(ti_sysbios_hal_TimerNull_Object *obj);
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_Object_next(ti_sysbios_hal_TimerNull_Object *obj)
{
    return (ti_sysbios_hal_TimerNull_Handle)ti_sysbios_hal_TimerNull_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_hal_TimerNull_Handle_label(ti_sysbios_hal_TimerNull_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_hal_TimerNull_Handle_label(ti_sysbios_hal_TimerNull_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_hal_TimerNull_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_hal_TimerNull_Handle_name(ti_sysbios_hal_TimerNull_Handle inst);
static inline xdc_String ti_sysbios_hal_TimerNull_Handle_name(ti_sysbios_hal_TimerNull_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_hal_TimerNull_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_handle(ti_sysbios_hal_TimerNull_Struct *str);
static inline ti_sysbios_hal_TimerNull_Handle ti_sysbios_hal_TimerNull_handle(ti_sysbios_hal_TimerNull_Struct *str)
{
    return (ti_sysbios_hal_TimerNull_Handle)str;
}

/* struct */
static inline ti_sysbios_hal_TimerNull_Struct *ti_sysbios_hal_TimerNull_struct(ti_sysbios_hal_TimerNull_Handle inst);
static inline ti_sysbios_hal_TimerNull_Struct *ti_sysbios_hal_TimerNull_struct(ti_sysbios_hal_TimerNull_Handle inst)
{
    return (ti_sysbios_hal_TimerNull_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_hal_TimerNull__top__
#undef __nested__
#endif

#endif /* ti_sysbios_hal_TimerNull__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_hal_TimerNull__internalaccess))

#ifndef ti_sysbios_hal_TimerNull__include_state
#define ti_sysbios_hal_TimerNull__include_state

/* Object */
struct ti_sysbios_hal_TimerNull_Object {
    const ti_sysbios_hal_TimerNull_Fxns__ *__fxns;
};

#endif /* ti_sysbios_hal_TimerNull__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_hal_TimerNull__nolocalnames)

#ifndef ti_sysbios_hal_TimerNull__localnames__done
#define ti_sysbios_hal_TimerNull__localnames__done

/* module prefix */
#define TimerNull_Instance ti_sysbios_hal_TimerNull_Instance
#define TimerNull_Handle ti_sysbios_hal_TimerNull_Handle
#define TimerNull_Module ti_sysbios_hal_TimerNull_Module
#define TimerNull_Object ti_sysbios_hal_TimerNull_Object
#define TimerNull_Struct ti_sysbios_hal_TimerNull_Struct
#define TimerNull_FuncPtr ti_sysbios_hal_TimerNull_FuncPtr
#define TimerNull_ANY ti_sysbios_hal_TimerNull_ANY
#define TimerNull_StartMode ti_sysbios_hal_TimerNull_StartMode
#define TimerNull_RunMode ti_sysbios_hal_TimerNull_RunMode
#define TimerNull_Status ti_sysbios_hal_TimerNull_Status
#define TimerNull_PeriodType ti_sysbios_hal_TimerNull_PeriodType
#define TimerNull_Instance_State ti_sysbios_hal_TimerNull_Instance_State
#define TimerNull_StartMode_AUTO ti_sysbios_hal_TimerNull_StartMode_AUTO
#define TimerNull_StartMode_USER ti_sysbios_hal_TimerNull_StartMode_USER
#define TimerNull_RunMode_CONTINUOUS ti_sysbios_hal_TimerNull_RunMode_CONTINUOUS
#define TimerNull_RunMode_ONESHOT ti_sysbios_hal_TimerNull_RunMode_ONESHOT
#define TimerNull_RunMode_DYNAMIC ti_sysbios_hal_TimerNull_RunMode_DYNAMIC
#define TimerNull_Status_INUSE ti_sysbios_hal_TimerNull_Status_INUSE
#define TimerNull_Status_FREE ti_sysbios_hal_TimerNull_Status_FREE
#define TimerNull_PeriodType_MICROSECS ti_sysbios_hal_TimerNull_PeriodType_MICROSECS
#define TimerNull_PeriodType_COUNTS ti_sysbios_hal_TimerNull_PeriodType_COUNTS
#define TimerNull_Params ti_sysbios_hal_TimerNull_Params
#define TimerNull_getNumTimers ti_sysbios_hal_TimerNull_getNumTimers
#define TimerNull_getStatus ti_sysbios_hal_TimerNull_getStatus
#define TimerNull_startup ti_sysbios_hal_TimerNull_startup
#define TimerNull_getMaxTicks ti_sysbios_hal_TimerNull_getMaxTicks
#define TimerNull_setNextTick ti_sysbios_hal_TimerNull_setNextTick
#define TimerNull_start ti_sysbios_hal_TimerNull_start
#define TimerNull_stop ti_sysbios_hal_TimerNull_stop
#define TimerNull_setPeriod ti_sysbios_hal_TimerNull_setPeriod
#define TimerNull_setPeriodMicroSecs ti_sysbios_hal_TimerNull_setPeriodMicroSecs
#define TimerNull_getPeriod ti_sysbios_hal_TimerNull_getPeriod
#define TimerNull_getCount ti_sysbios_hal_TimerNull_getCount
#define TimerNull_getFreq ti_sysbios_hal_TimerNull_getFreq
#define TimerNull_getFunc ti_sysbios_hal_TimerNull_getFunc
#define TimerNull_setFunc ti_sysbios_hal_TimerNull_setFunc
#define TimerNull_trigger ti_sysbios_hal_TimerNull_trigger
#define TimerNull_getExpiredCounts ti_sysbios_hal_TimerNull_getExpiredCounts
#define TimerNull_getExpiredTicks ti_sysbios_hal_TimerNull_getExpiredTicks
#define TimerNull_getCurrentTick ti_sysbios_hal_TimerNull_getCurrentTick
#define TimerNull_Module_name ti_sysbios_hal_TimerNull_Module_name
#define TimerNull_Module_id ti_sysbios_hal_TimerNull_Module_id
#define TimerNull_Module_startup ti_sysbios_hal_TimerNull_Module_startup
#define TimerNull_Module_startupDone ti_sysbios_hal_TimerNull_Module_startupDone
#define TimerNull_Module_hasMask ti_sysbios_hal_TimerNull_Module_hasMask
#define TimerNull_Module_getMask ti_sysbios_hal_TimerNull_Module_getMask
#define TimerNull_Module_setMask ti_sysbios_hal_TimerNull_Module_setMask
#define TimerNull_Object_heap ti_sysbios_hal_TimerNull_Object_heap
#define TimerNull_Module_heap ti_sysbios_hal_TimerNull_Module_heap
#define TimerNull_construct ti_sysbios_hal_TimerNull_construct
#define TimerNull_create ti_sysbios_hal_TimerNull_create
#define TimerNull_handle ti_sysbios_hal_TimerNull_handle
#define TimerNull_struct ti_sysbios_hal_TimerNull_struct
#define TimerNull_Handle_label ti_sysbios_hal_TimerNull_Handle_label
#define TimerNull_Handle_name ti_sysbios_hal_TimerNull_Handle_name
#define TimerNull_Instance_init ti_sysbios_hal_TimerNull_Instance_init
#define TimerNull_Object_count ti_sysbios_hal_TimerNull_Object_count
#define TimerNull_Object_get ti_sysbios_hal_TimerNull_Object_get
#define TimerNull_Object_first ti_sysbios_hal_TimerNull_Object_first
#define TimerNull_Object_next ti_sysbios_hal_TimerNull_Object_next
#define TimerNull_Object_sizeof ti_sysbios_hal_TimerNull_Object_sizeof
#define TimerNull_Params_copy ti_sysbios_hal_TimerNull_Params_copy
#define TimerNull_Params_init ti_sysbios_hal_TimerNull_Params_init
#define TimerNull_Instance_finalize ti_sysbios_hal_TimerNull_Instance_finalize
#define TimerNull_delete ti_sysbios_hal_TimerNull_delete
#define TimerNull_destruct ti_sysbios_hal_TimerNull_destruct
#define TimerNull_Module_upCast ti_sysbios_hal_TimerNull_Module_upCast
#define TimerNull_Module_to_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Module_to_ti_sysbios_interfaces_ITimer
#define TimerNull_Handle_upCast ti_sysbios_hal_TimerNull_Handle_upCast
#define TimerNull_Handle_to_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Handle_to_ti_sysbios_interfaces_ITimer
#define TimerNull_Handle_downCast ti_sysbios_hal_TimerNull_Handle_downCast
#define TimerNull_Handle_from_ti_sysbios_interfaces_ITimer ti_sysbios_hal_TimerNull_Handle_from_ti_sysbios_interfaces_ITimer

#endif /* ti_sysbios_hal_TimerNull__localnames__done */
#endif
