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
 *     PER-INSTANCE TYPES
 *     VIRTUAL FUNCTIONS
 *     FUNCTION STUBS
 *     FUNCTION SELECTORS
 *
 *     EPILOGUE
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_interfaces_ITimer__include
#define ti_sysbios_interfaces_ITimer__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_ITimer__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_ITimer___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/interfaces/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_FuncPtr)(xdc_UArg arg1);

/* ANY */
#define ti_sysbios_interfaces_ITimer_ANY (~0)

/* StartMode */
enum ti_sysbios_interfaces_ITimer_StartMode {
    ti_sysbios_interfaces_ITimer_StartMode_AUTO,
    ti_sysbios_interfaces_ITimer_StartMode_USER
};
typedef enum ti_sysbios_interfaces_ITimer_StartMode ti_sysbios_interfaces_ITimer_StartMode;

/* RunMode */
enum ti_sysbios_interfaces_ITimer_RunMode {
    ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS,
    ti_sysbios_interfaces_ITimer_RunMode_ONESHOT,
    ti_sysbios_interfaces_ITimer_RunMode_DYNAMIC
};
typedef enum ti_sysbios_interfaces_ITimer_RunMode ti_sysbios_interfaces_ITimer_RunMode;

/* Status */
enum ti_sysbios_interfaces_ITimer_Status {
    ti_sysbios_interfaces_ITimer_Status_INUSE,
    ti_sysbios_interfaces_ITimer_Status_FREE
};
typedef enum ti_sysbios_interfaces_ITimer_Status ti_sysbios_interfaces_ITimer_Status;

/* PeriodType */
enum ti_sysbios_interfaces_ITimer_PeriodType {
    ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS,
    ti_sysbios_interfaces_ITimer_PeriodType_COUNTS
};
typedef enum ti_sysbios_interfaces_ITimer_PeriodType ti_sysbios_interfaces_ITimer_PeriodType;


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_interfaces_ITimer_Args__create {
    xdc_Int id;
    ti_sysbios_interfaces_ITimer_FuncPtr tickFxn;
} ti_sysbios_interfaces_ITimer_Args__create;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_interfaces_ITimer_Params {
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
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_ITimer_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_UInt (*getNumTimers)(void);
    ti_sysbios_interfaces_ITimer_Status (*getStatus)(xdc_UInt id);
    xdc_Void (*startup)(void);
    xdc_UInt32 (*getMaxTicks)(void* inst);
    xdc_Void (*setNextTick)(void* inst, xdc_UInt32 ticks);
    xdc_Void (*start)(void* inst);
    xdc_Void (*stop)(void* inst);
    xdc_Void (*setPeriod)(void* inst, xdc_UInt32 period);
    xdc_Bool (*setPeriodMicroSecs)(void* inst, xdc_UInt32 microsecs);
    xdc_UInt32 (*getPeriod)(void* inst);
    xdc_UInt32 (*getCount)(void* inst);
    xdc_Void (*getFreq)(void* inst, xdc_runtime_Types_FreqHz* freq);
    ti_sysbios_interfaces_ITimer_FuncPtr (*getFunc)(void* inst, xdc_UArg* arg);
    xdc_Void (*setFunc)(void* inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg);
    xdc_Void (*trigger)(void* inst, xdc_UInt32 cycles);
    xdc_UInt32 (*getExpiredCounts)(void* inst);
    xdc_UInt32 (*getExpiredTicks)(void* inst, xdc_UInt32 tickPeriod);
    xdc_UInt32 (*getCurrentTick)(void* inst, xdc_Bool save);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_ITimer_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_ITimer_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_ITimer_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_ITimer_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* create */
xdc__CODESECT(ti_sysbios_interfaces_ITimer_create, "ti_sysbios_interfaces_ITimer_create")
__extern ti_sysbios_interfaces_ITimer_Handle ti_sysbios_interfaces_ITimer_create(ti_sysbios_interfaces_ITimer_Module mod, xdc_Int id, ti_sysbios_interfaces_ITimer_FuncPtr tickFxn, const ti_sysbios_interfaces_ITimer_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_interfaces_ITimer_delete, "ti_sysbios_interfaces_ITimer_delete")
__extern xdc_Void ti_sysbios_interfaces_ITimer_delete(ti_sysbios_interfaces_ITimer_Handle *inst);

/* Handle_to_Module */
static inline ti_sysbios_interfaces_ITimer_Module ti_sysbios_interfaces_ITimer_Handle_to_Module(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_Module ti_sysbios_interfaces_ITimer_Handle_to_Module(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return inst->__fxns;
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_interfaces_ITimer_Handle_label(ti_sysbios_interfaces_ITimer_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_interfaces_ITimer_Handle_label(ti_sysbios_interfaces_ITimer_Handle inst, xdc_runtime_Types_Label *lab)
{
    return inst->__fxns->__sysp->__label(inst, lab);
}

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ITimer_Module_id(ti_sysbios_interfaces_ITimer_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ITimer_Module_id(ti_sysbios_interfaces_ITimer_Module mod)
{
    return mod->__sysp->__mid;
}

/* getNumTimers */
static inline xdc_UInt ti_sysbios_interfaces_ITimer_getNumTimers(ti_sysbios_interfaces_ITimer_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_ITimer_getNumTimers( ti_sysbios_interfaces_ITimer_Module mod )
{
    return mod->getNumTimers();
}

/* getStatus */
static inline ti_sysbios_interfaces_ITimer_Status ti_sysbios_interfaces_ITimer_getStatus(ti_sysbios_interfaces_ITimer_Module mod, xdc_UInt id);
static inline ti_sysbios_interfaces_ITimer_Status ti_sysbios_interfaces_ITimer_getStatus( ti_sysbios_interfaces_ITimer_Module mod, xdc_UInt id )
{
    return mod->getStatus(id);
}

/* startup */
static inline xdc_Void ti_sysbios_interfaces_ITimer_startup(ti_sysbios_interfaces_ITimer_Module mod);
static inline xdc_Void ti_sysbios_interfaces_ITimer_startup( ti_sysbios_interfaces_ITimer_Module mod )
{
    mod->startup();
}

/* getMaxTicks */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getMaxTicks(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getMaxTicks( ti_sysbios_interfaces_ITimer_Handle inst )
{
    return inst->__fxns->getMaxTicks((void*)inst);
}

/* setNextTick */
static inline xdc_Void ti_sysbios_interfaces_ITimer_setNextTick(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 ticks);
static inline xdc_Void ti_sysbios_interfaces_ITimer_setNextTick( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 ticks )
{
    inst->__fxns->setNextTick((void*)inst, ticks);
}

/* start */
static inline xdc_Void ti_sysbios_interfaces_ITimer_start(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_Void ti_sysbios_interfaces_ITimer_start( ti_sysbios_interfaces_ITimer_Handle inst )
{
    inst->__fxns->start((void*)inst);
}

/* stop */
static inline xdc_Void ti_sysbios_interfaces_ITimer_stop(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_Void ti_sysbios_interfaces_ITimer_stop( ti_sysbios_interfaces_ITimer_Handle inst )
{
    inst->__fxns->stop((void*)inst);
}

/* setPeriod */
static inline xdc_Void ti_sysbios_interfaces_ITimer_setPeriod(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 period);
static inline xdc_Void ti_sysbios_interfaces_ITimer_setPeriod( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 period )
{
    inst->__fxns->setPeriod((void*)inst, period);
}

/* setPeriodMicroSecs */
static inline xdc_Bool ti_sysbios_interfaces_ITimer_setPeriodMicroSecs(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 microsecs);
static inline xdc_Bool ti_sysbios_interfaces_ITimer_setPeriodMicroSecs( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 microsecs )
{
    return inst->__fxns->setPeriodMicroSecs((void*)inst, microsecs);
}

/* getPeriod */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getPeriod(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getPeriod( ti_sysbios_interfaces_ITimer_Handle inst )
{
    return inst->__fxns->getPeriod((void*)inst);
}

/* getCount */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getCount(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getCount( ti_sysbios_interfaces_ITimer_Handle inst )
{
    return inst->__fxns->getCount((void*)inst);
}

/* getFreq */
static inline xdc_Void ti_sysbios_interfaces_ITimer_getFreq(ti_sysbios_interfaces_ITimer_Handle inst, xdc_runtime_Types_FreqHz *freq);
static inline xdc_Void ti_sysbios_interfaces_ITimer_getFreq( ti_sysbios_interfaces_ITimer_Handle inst, xdc_runtime_Types_FreqHz *freq )
{
    inst->__fxns->getFreq((void*)inst, freq);
}

/* getFunc */
static inline ti_sysbios_interfaces_ITimer_FuncPtr ti_sysbios_interfaces_ITimer_getFunc(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UArg *arg);
static inline ti_sysbios_interfaces_ITimer_FuncPtr ti_sysbios_interfaces_ITimer_getFunc( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UArg *arg )
{
    return inst->__fxns->getFunc((void*)inst, arg);
}

/* setFunc */
static inline xdc_Void ti_sysbios_interfaces_ITimer_setFunc(ti_sysbios_interfaces_ITimer_Handle inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg);
static inline xdc_Void ti_sysbios_interfaces_ITimer_setFunc( ti_sysbios_interfaces_ITimer_Handle inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg )
{
    inst->__fxns->setFunc((void*)inst, fxn, arg);
}

/* trigger */
static inline xdc_Void ti_sysbios_interfaces_ITimer_trigger(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 cycles);
static inline xdc_Void ti_sysbios_interfaces_ITimer_trigger( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 cycles )
{
    inst->__fxns->trigger((void*)inst, cycles);
}

/* getExpiredCounts */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getExpiredCounts(ti_sysbios_interfaces_ITimer_Handle inst);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getExpiredCounts( ti_sysbios_interfaces_ITimer_Handle inst )
{
    return inst->__fxns->getExpiredCounts((void*)inst);
}

/* getExpiredTicks */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getExpiredTicks(ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 tickPeriod);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getExpiredTicks( ti_sysbios_interfaces_ITimer_Handle inst, xdc_UInt32 tickPeriod )
{
    return inst->__fxns->getExpiredTicks((void*)inst, tickPeriod);
}

/* getCurrentTick */
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getCurrentTick(ti_sysbios_interfaces_ITimer_Handle inst, xdc_Bool save);
static inline xdc_UInt32 ti_sysbios_interfaces_ITimer_getCurrentTick( ti_sysbios_interfaces_ITimer_Handle inst, xdc_Bool save )
{
    return inst->__fxns->getCurrentTick((void*)inst, save);
}


/*
 * ======== FUNCTION SELECTORS ========
 */

/* These functions return function pointers for module and instance functions.
 * The functions accept modules and instances declared as types defined in this
 * interface, but they return functions defined for the actual objects passed
 * as parameters. These functions are not invoked by any generated code or
 * XDCtools internal code.
 */

/* getNumTimers_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_ITimer_getNumTimers_FxnT)(void);
static inline ti_sysbios_interfaces_ITimer_getNumTimers_FxnT ti_sysbios_interfaces_ITimer_getNumTimers_fxnP(ti_sysbios_interfaces_ITimer_Module mod);
static inline ti_sysbios_interfaces_ITimer_getNumTimers_FxnT ti_sysbios_interfaces_ITimer_getNumTimers_fxnP(ti_sysbios_interfaces_ITimer_Module mod)
{
    return (ti_sysbios_interfaces_ITimer_getNumTimers_FxnT)mod->getNumTimers;
}

/* getStatus_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_ITimer_Status (*ti_sysbios_interfaces_ITimer_getStatus_FxnT)(xdc_UInt id);
static inline ti_sysbios_interfaces_ITimer_getStatus_FxnT ti_sysbios_interfaces_ITimer_getStatus_fxnP(ti_sysbios_interfaces_ITimer_Module mod);
static inline ti_sysbios_interfaces_ITimer_getStatus_FxnT ti_sysbios_interfaces_ITimer_getStatus_fxnP(ti_sysbios_interfaces_ITimer_Module mod)
{
    return (ti_sysbios_interfaces_ITimer_getStatus_FxnT)mod->getStatus;
}

/* startup_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_startup_FxnT)(void);
static inline ti_sysbios_interfaces_ITimer_startup_FxnT ti_sysbios_interfaces_ITimer_startup_fxnP(ti_sysbios_interfaces_ITimer_Module mod);
static inline ti_sysbios_interfaces_ITimer_startup_FxnT ti_sysbios_interfaces_ITimer_startup_fxnP(ti_sysbios_interfaces_ITimer_Module mod)
{
    return (ti_sysbios_interfaces_ITimer_startup_FxnT)mod->startup;
}

/* getMaxTicks_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getMaxTicks_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_getMaxTicks_FxnT ti_sysbios_interfaces_ITimer_getMaxTicks_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getMaxTicks_FxnT ti_sysbios_interfaces_ITimer_getMaxTicks_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getMaxTicks_FxnT)inst->__fxns->getMaxTicks;
}

/* setNextTick_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_setNextTick_FxnT)(xdc_Void *inst, xdc_UInt32 ticks);
static inline ti_sysbios_interfaces_ITimer_setNextTick_FxnT ti_sysbios_interfaces_ITimer_setNextTick_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_setNextTick_FxnT ti_sysbios_interfaces_ITimer_setNextTick_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_setNextTick_FxnT)inst->__fxns->setNextTick;
}

/* start_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_start_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_start_FxnT ti_sysbios_interfaces_ITimer_start_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_start_FxnT ti_sysbios_interfaces_ITimer_start_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_start_FxnT)inst->__fxns->start;
}

/* stop_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_stop_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_stop_FxnT ti_sysbios_interfaces_ITimer_stop_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_stop_FxnT ti_sysbios_interfaces_ITimer_stop_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_stop_FxnT)inst->__fxns->stop;
}

/* setPeriod_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_setPeriod_FxnT)(xdc_Void *inst, xdc_UInt32 period);
static inline ti_sysbios_interfaces_ITimer_setPeriod_FxnT ti_sysbios_interfaces_ITimer_setPeriod_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_setPeriod_FxnT ti_sysbios_interfaces_ITimer_setPeriod_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_setPeriod_FxnT)inst->__fxns->setPeriod;
}

/* setPeriodMicroSecs_{FxnT,fxnP} */
typedef xdc_Bool (*ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_FxnT)(xdc_Void *inst, xdc_UInt32 microsecs);
static inline ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_FxnT ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_FxnT ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_FxnT)inst->__fxns->setPeriodMicroSecs;
}

/* getPeriod_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getPeriod_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_getPeriod_FxnT ti_sysbios_interfaces_ITimer_getPeriod_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getPeriod_FxnT ti_sysbios_interfaces_ITimer_getPeriod_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getPeriod_FxnT)inst->__fxns->getPeriod;
}

/* getCount_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getCount_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_getCount_FxnT ti_sysbios_interfaces_ITimer_getCount_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getCount_FxnT ti_sysbios_interfaces_ITimer_getCount_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getCount_FxnT)inst->__fxns->getCount;
}

/* getFreq_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_getFreq_FxnT)(xdc_Void *inst, xdc_runtime_Types_FreqHz* freq);
static inline ti_sysbios_interfaces_ITimer_getFreq_FxnT ti_sysbios_interfaces_ITimer_getFreq_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getFreq_FxnT ti_sysbios_interfaces_ITimer_getFreq_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getFreq_FxnT)inst->__fxns->getFreq;
}

/* getFunc_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_ITimer_FuncPtr (*ti_sysbios_interfaces_ITimer_getFunc_FxnT)(xdc_Void *inst, xdc_UArg* arg);
static inline ti_sysbios_interfaces_ITimer_getFunc_FxnT ti_sysbios_interfaces_ITimer_getFunc_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getFunc_FxnT ti_sysbios_interfaces_ITimer_getFunc_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getFunc_FxnT)inst->__fxns->getFunc;
}

/* setFunc_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_setFunc_FxnT)(xdc_Void *inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg);
static inline ti_sysbios_interfaces_ITimer_setFunc_FxnT ti_sysbios_interfaces_ITimer_setFunc_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_setFunc_FxnT ti_sysbios_interfaces_ITimer_setFunc_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_setFunc_FxnT)inst->__fxns->setFunc;
}

/* trigger_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITimer_trigger_FxnT)(xdc_Void *inst, xdc_UInt32 cycles);
static inline ti_sysbios_interfaces_ITimer_trigger_FxnT ti_sysbios_interfaces_ITimer_trigger_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_trigger_FxnT ti_sysbios_interfaces_ITimer_trigger_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_trigger_FxnT)inst->__fxns->trigger;
}

/* getExpiredCounts_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getExpiredCounts_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_ITimer_getExpiredCounts_FxnT ti_sysbios_interfaces_ITimer_getExpiredCounts_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getExpiredCounts_FxnT ti_sysbios_interfaces_ITimer_getExpiredCounts_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getExpiredCounts_FxnT)inst->__fxns->getExpiredCounts;
}

/* getExpiredTicks_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getExpiredTicks_FxnT)(xdc_Void *inst, xdc_UInt32 tickPeriod);
static inline ti_sysbios_interfaces_ITimer_getExpiredTicks_FxnT ti_sysbios_interfaces_ITimer_getExpiredTicks_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getExpiredTicks_FxnT ti_sysbios_interfaces_ITimer_getExpiredTicks_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getExpiredTicks_FxnT)inst->__fxns->getExpiredTicks;
}

/* getCurrentTick_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ITimer_getCurrentTick_FxnT)(xdc_Void *inst, xdc_Bool save);
static inline ti_sysbios_interfaces_ITimer_getCurrentTick_FxnT ti_sysbios_interfaces_ITimer_getCurrentTick_fxnP(ti_sysbios_interfaces_ITimer_Handle inst);
static inline ti_sysbios_interfaces_ITimer_getCurrentTick_FxnT ti_sysbios_interfaces_ITimer_getCurrentTick_fxnP(ti_sysbios_interfaces_ITimer_Handle inst)
{
    return (ti_sysbios_interfaces_ITimer_getCurrentTick_FxnT)inst->__fxns->getCurrentTick;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_ITimer__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_ITimer__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_ITimer__nolocalnames)

#ifndef ti_sysbios_interfaces_ITimer__localnames__done
#define ti_sysbios_interfaces_ITimer__localnames__done

/* module prefix */
#define ITimer_Instance ti_sysbios_interfaces_ITimer_Instance
#define ITimer_Handle ti_sysbios_interfaces_ITimer_Handle
#define ITimer_Module ti_sysbios_interfaces_ITimer_Module
#define ITimer_FuncPtr ti_sysbios_interfaces_ITimer_FuncPtr
#define ITimer_ANY ti_sysbios_interfaces_ITimer_ANY
#define ITimer_StartMode ti_sysbios_interfaces_ITimer_StartMode
#define ITimer_RunMode ti_sysbios_interfaces_ITimer_RunMode
#define ITimer_Status ti_sysbios_interfaces_ITimer_Status
#define ITimer_PeriodType ti_sysbios_interfaces_ITimer_PeriodType
#define ITimer_StartMode_AUTO ti_sysbios_interfaces_ITimer_StartMode_AUTO
#define ITimer_StartMode_USER ti_sysbios_interfaces_ITimer_StartMode_USER
#define ITimer_RunMode_CONTINUOUS ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS
#define ITimer_RunMode_ONESHOT ti_sysbios_interfaces_ITimer_RunMode_ONESHOT
#define ITimer_RunMode_DYNAMIC ti_sysbios_interfaces_ITimer_RunMode_DYNAMIC
#define ITimer_Status_INUSE ti_sysbios_interfaces_ITimer_Status_INUSE
#define ITimer_Status_FREE ti_sysbios_interfaces_ITimer_Status_FREE
#define ITimer_PeriodType_MICROSECS ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS
#define ITimer_PeriodType_COUNTS ti_sysbios_interfaces_ITimer_PeriodType_COUNTS
#define ITimer_Params ti_sysbios_interfaces_ITimer_Params
#define ITimer_getNumTimers ti_sysbios_interfaces_ITimer_getNumTimers
#define ITimer_getNumTimers_fxnP ti_sysbios_interfaces_ITimer_getNumTimers_fxnP
#define ITimer_getNumTimers_FxnT ti_sysbios_interfaces_ITimer_getNumTimers_FxnT
#define ITimer_getStatus ti_sysbios_interfaces_ITimer_getStatus
#define ITimer_getStatus_fxnP ti_sysbios_interfaces_ITimer_getStatus_fxnP
#define ITimer_getStatus_FxnT ti_sysbios_interfaces_ITimer_getStatus_FxnT
#define ITimer_startup ti_sysbios_interfaces_ITimer_startup
#define ITimer_startup_fxnP ti_sysbios_interfaces_ITimer_startup_fxnP
#define ITimer_startup_FxnT ti_sysbios_interfaces_ITimer_startup_FxnT
#define ITimer_getMaxTicks ti_sysbios_interfaces_ITimer_getMaxTicks
#define ITimer_getMaxTicks_fxnP ti_sysbios_interfaces_ITimer_getMaxTicks_fxnP
#define ITimer_getMaxTicks_FxnT ti_sysbios_interfaces_ITimer_getMaxTicks_FxnT
#define ITimer_setNextTick ti_sysbios_interfaces_ITimer_setNextTick
#define ITimer_setNextTick_fxnP ti_sysbios_interfaces_ITimer_setNextTick_fxnP
#define ITimer_setNextTick_FxnT ti_sysbios_interfaces_ITimer_setNextTick_FxnT
#define ITimer_start ti_sysbios_interfaces_ITimer_start
#define ITimer_start_fxnP ti_sysbios_interfaces_ITimer_start_fxnP
#define ITimer_start_FxnT ti_sysbios_interfaces_ITimer_start_FxnT
#define ITimer_stop ti_sysbios_interfaces_ITimer_stop
#define ITimer_stop_fxnP ti_sysbios_interfaces_ITimer_stop_fxnP
#define ITimer_stop_FxnT ti_sysbios_interfaces_ITimer_stop_FxnT
#define ITimer_setPeriod ti_sysbios_interfaces_ITimer_setPeriod
#define ITimer_setPeriod_fxnP ti_sysbios_interfaces_ITimer_setPeriod_fxnP
#define ITimer_setPeriod_FxnT ti_sysbios_interfaces_ITimer_setPeriod_FxnT
#define ITimer_setPeriodMicroSecs ti_sysbios_interfaces_ITimer_setPeriodMicroSecs
#define ITimer_setPeriodMicroSecs_fxnP ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_fxnP
#define ITimer_setPeriodMicroSecs_FxnT ti_sysbios_interfaces_ITimer_setPeriodMicroSecs_FxnT
#define ITimer_getPeriod ti_sysbios_interfaces_ITimer_getPeriod
#define ITimer_getPeriod_fxnP ti_sysbios_interfaces_ITimer_getPeriod_fxnP
#define ITimer_getPeriod_FxnT ti_sysbios_interfaces_ITimer_getPeriod_FxnT
#define ITimer_getCount ti_sysbios_interfaces_ITimer_getCount
#define ITimer_getCount_fxnP ti_sysbios_interfaces_ITimer_getCount_fxnP
#define ITimer_getCount_FxnT ti_sysbios_interfaces_ITimer_getCount_FxnT
#define ITimer_getFreq ti_sysbios_interfaces_ITimer_getFreq
#define ITimer_getFreq_fxnP ti_sysbios_interfaces_ITimer_getFreq_fxnP
#define ITimer_getFreq_FxnT ti_sysbios_interfaces_ITimer_getFreq_FxnT
#define ITimer_getFunc ti_sysbios_interfaces_ITimer_getFunc
#define ITimer_getFunc_fxnP ti_sysbios_interfaces_ITimer_getFunc_fxnP
#define ITimer_getFunc_FxnT ti_sysbios_interfaces_ITimer_getFunc_FxnT
#define ITimer_setFunc ti_sysbios_interfaces_ITimer_setFunc
#define ITimer_setFunc_fxnP ti_sysbios_interfaces_ITimer_setFunc_fxnP
#define ITimer_setFunc_FxnT ti_sysbios_interfaces_ITimer_setFunc_FxnT
#define ITimer_trigger ti_sysbios_interfaces_ITimer_trigger
#define ITimer_trigger_fxnP ti_sysbios_interfaces_ITimer_trigger_fxnP
#define ITimer_trigger_FxnT ti_sysbios_interfaces_ITimer_trigger_FxnT
#define ITimer_getExpiredCounts ti_sysbios_interfaces_ITimer_getExpiredCounts
#define ITimer_getExpiredCounts_fxnP ti_sysbios_interfaces_ITimer_getExpiredCounts_fxnP
#define ITimer_getExpiredCounts_FxnT ti_sysbios_interfaces_ITimer_getExpiredCounts_FxnT
#define ITimer_getExpiredTicks ti_sysbios_interfaces_ITimer_getExpiredTicks
#define ITimer_getExpiredTicks_fxnP ti_sysbios_interfaces_ITimer_getExpiredTicks_fxnP
#define ITimer_getExpiredTicks_FxnT ti_sysbios_interfaces_ITimer_getExpiredTicks_FxnT
#define ITimer_getCurrentTick ti_sysbios_interfaces_ITimer_getCurrentTick
#define ITimer_getCurrentTick_fxnP ti_sysbios_interfaces_ITimer_getCurrentTick_fxnP
#define ITimer_getCurrentTick_FxnT ti_sysbios_interfaces_ITimer_getCurrentTick_FxnT
#define ITimer_Module_name ti_sysbios_interfaces_ITimer_Module_name
#define ITimer_create ti_sysbios_interfaces_ITimer_create
#define ITimer_delete ti_sysbios_interfaces_ITimer_delete
#define ITimer_Handle_label ti_sysbios_interfaces_ITimer_Handle_label
#define ITimer_Handle_to_Module ti_sysbios_interfaces_ITimer_Handle_to_Module

#endif /* ti_sysbios_interfaces_ITimer__localnames__done */
#endif
