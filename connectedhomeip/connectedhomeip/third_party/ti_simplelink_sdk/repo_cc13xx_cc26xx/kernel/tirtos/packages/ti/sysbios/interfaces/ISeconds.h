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

#ifndef ti_sysbios_interfaces_ISeconds__include
#define ti_sysbios_interfaces_ISeconds__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_ISeconds__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_ISeconds___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/interfaces/package/package.defs.h>

#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Time */
struct ti_sysbios_interfaces_ISeconds_Time {
    xdc_UInt32 secsHi;
    xdc_UInt32 secs;
    xdc_UInt32 nsecs;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_ISeconds_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_UInt32 (*get)(void);
    xdc_UInt32 (*getTime)(ti_sysbios_interfaces_ISeconds_Time* ts);
    xdc_Void (*set)(xdc_UInt32 seconds);
    xdc_UInt32 (*setTime)(ti_sysbios_interfaces_ISeconds_Time* ts);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_ISeconds_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_ISeconds_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_ISeconds_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_ISeconds_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ISeconds_Module_id(ti_sysbios_interfaces_ISeconds_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ISeconds_Module_id(ti_sysbios_interfaces_ISeconds_Module mod)
{
    return mod->__sysp->__mid;
}

/* get */
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_get(ti_sysbios_interfaces_ISeconds_Module mod);
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_get( ti_sysbios_interfaces_ISeconds_Module mod )
{
    return mod->get();
}

/* getTime */
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_getTime(ti_sysbios_interfaces_ISeconds_Module mod, ti_sysbios_interfaces_ISeconds_Time *ts);
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_getTime( ti_sysbios_interfaces_ISeconds_Module mod, ti_sysbios_interfaces_ISeconds_Time *ts )
{
    return mod->getTime(ts);
}

/* set */
static inline xdc_Void ti_sysbios_interfaces_ISeconds_set(ti_sysbios_interfaces_ISeconds_Module mod, xdc_UInt32 seconds);
static inline xdc_Void ti_sysbios_interfaces_ISeconds_set( ti_sysbios_interfaces_ISeconds_Module mod, xdc_UInt32 seconds )
{
    mod->set(seconds);
}

/* setTime */
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_setTime(ti_sysbios_interfaces_ISeconds_Module mod, ti_sysbios_interfaces_ISeconds_Time *ts);
static inline xdc_UInt32 ti_sysbios_interfaces_ISeconds_setTime( ti_sysbios_interfaces_ISeconds_Module mod, ti_sysbios_interfaces_ISeconds_Time *ts )
{
    return mod->setTime(ts);
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

/* get_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ISeconds_get_FxnT)(void);
static inline ti_sysbios_interfaces_ISeconds_get_FxnT ti_sysbios_interfaces_ISeconds_get_fxnP(ti_sysbios_interfaces_ISeconds_Module mod);
static inline ti_sysbios_interfaces_ISeconds_get_FxnT ti_sysbios_interfaces_ISeconds_get_fxnP(ti_sysbios_interfaces_ISeconds_Module mod)
{
    return (ti_sysbios_interfaces_ISeconds_get_FxnT)mod->get;
}

/* getTime_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ISeconds_getTime_FxnT)(ti_sysbios_interfaces_ISeconds_Time* ts);
static inline ti_sysbios_interfaces_ISeconds_getTime_FxnT ti_sysbios_interfaces_ISeconds_getTime_fxnP(ti_sysbios_interfaces_ISeconds_Module mod);
static inline ti_sysbios_interfaces_ISeconds_getTime_FxnT ti_sysbios_interfaces_ISeconds_getTime_fxnP(ti_sysbios_interfaces_ISeconds_Module mod)
{
    return (ti_sysbios_interfaces_ISeconds_getTime_FxnT)mod->getTime;
}

/* set_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ISeconds_set_FxnT)(xdc_UInt32 seconds);
static inline ti_sysbios_interfaces_ISeconds_set_FxnT ti_sysbios_interfaces_ISeconds_set_fxnP(ti_sysbios_interfaces_ISeconds_Module mod);
static inline ti_sysbios_interfaces_ISeconds_set_FxnT ti_sysbios_interfaces_ISeconds_set_fxnP(ti_sysbios_interfaces_ISeconds_Module mod)
{
    return (ti_sysbios_interfaces_ISeconds_set_FxnT)mod->set;
}

/* setTime_{FxnT,fxnP} */
typedef xdc_UInt32 (*ti_sysbios_interfaces_ISeconds_setTime_FxnT)(ti_sysbios_interfaces_ISeconds_Time* ts);
static inline ti_sysbios_interfaces_ISeconds_setTime_FxnT ti_sysbios_interfaces_ISeconds_setTime_fxnP(ti_sysbios_interfaces_ISeconds_Module mod);
static inline ti_sysbios_interfaces_ISeconds_setTime_FxnT ti_sysbios_interfaces_ISeconds_setTime_fxnP(ti_sysbios_interfaces_ISeconds_Module mod)
{
    return (ti_sysbios_interfaces_ISeconds_setTime_FxnT)mod->setTime;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_ISeconds__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_ISeconds__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_ISeconds__nolocalnames)

#ifndef ti_sysbios_interfaces_ISeconds__localnames__done
#define ti_sysbios_interfaces_ISeconds__localnames__done

/* module prefix */
#define ISeconds_Module ti_sysbios_interfaces_ISeconds_Module
#define ISeconds_Time ti_sysbios_interfaces_ISeconds_Time
#define ISeconds_get ti_sysbios_interfaces_ISeconds_get
#define ISeconds_get_fxnP ti_sysbios_interfaces_ISeconds_get_fxnP
#define ISeconds_get_FxnT ti_sysbios_interfaces_ISeconds_get_FxnT
#define ISeconds_getTime ti_sysbios_interfaces_ISeconds_getTime
#define ISeconds_getTime_fxnP ti_sysbios_interfaces_ISeconds_getTime_fxnP
#define ISeconds_getTime_FxnT ti_sysbios_interfaces_ISeconds_getTime_FxnT
#define ISeconds_set ti_sysbios_interfaces_ISeconds_set
#define ISeconds_set_fxnP ti_sysbios_interfaces_ISeconds_set_fxnP
#define ISeconds_set_FxnT ti_sysbios_interfaces_ISeconds_set_FxnT
#define ISeconds_setTime ti_sysbios_interfaces_ISeconds_setTime
#define ISeconds_setTime_fxnP ti_sysbios_interfaces_ISeconds_setTime_fxnP
#define ISeconds_setTime_FxnT ti_sysbios_interfaces_ISeconds_setTime_FxnT
#define ISeconds_Module_name ti_sysbios_interfaces_ISeconds_Module_name

#endif /* ti_sysbios_interfaces_ISeconds__localnames__done */
#endif
