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

#ifndef ti_sysbios_interfaces_ICore__include
#define ti_sysbios_interfaces_ICore__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_ICore__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_ICore___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/interfaces/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_ICore_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_UInt (*getId)(void);
    xdc_Void (*interruptCore)(xdc_UInt coreId);
    xdc_IArg (*lock)(void);
    xdc_Void (*unlock)(void);
    xdc_UInt (*hwiDisable)(void);
    xdc_UInt (*hwiEnable)(void);
    xdc_Void (*hwiRestore)(xdc_UInt key);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_ICore_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_ICore_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_ICore_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_ICore_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ICore_Module_id(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ICore_Module_id(ti_sysbios_interfaces_ICore_Module mod)
{
    return mod->__sysp->__mid;
}

/* getId */
static inline xdc_UInt ti_sysbios_interfaces_ICore_getId(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_ICore_getId( ti_sysbios_interfaces_ICore_Module mod )
{
    return mod->getId();
}

/* interruptCore */
static inline xdc_Void ti_sysbios_interfaces_ICore_interruptCore(ti_sysbios_interfaces_ICore_Module mod, xdc_UInt coreId);
static inline xdc_Void ti_sysbios_interfaces_ICore_interruptCore( ti_sysbios_interfaces_ICore_Module mod, xdc_UInt coreId )
{
    mod->interruptCore(coreId);
}

/* lock */
static inline xdc_IArg ti_sysbios_interfaces_ICore_lock(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_IArg ti_sysbios_interfaces_ICore_lock( ti_sysbios_interfaces_ICore_Module mod )
{
    return mod->lock();
}

/* unlock */
static inline xdc_Void ti_sysbios_interfaces_ICore_unlock(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_Void ti_sysbios_interfaces_ICore_unlock( ti_sysbios_interfaces_ICore_Module mod )
{
    mod->unlock();
}

/* hwiDisable */
static inline xdc_UInt ti_sysbios_interfaces_ICore_hwiDisable(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_ICore_hwiDisable( ti_sysbios_interfaces_ICore_Module mod )
{
    return mod->hwiDisable();
}

/* hwiEnable */
static inline xdc_UInt ti_sysbios_interfaces_ICore_hwiEnable(ti_sysbios_interfaces_ICore_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_ICore_hwiEnable( ti_sysbios_interfaces_ICore_Module mod )
{
    return mod->hwiEnable();
}

/* hwiRestore */
static inline xdc_Void ti_sysbios_interfaces_ICore_hwiRestore(ti_sysbios_interfaces_ICore_Module mod, xdc_UInt key);
static inline xdc_Void ti_sysbios_interfaces_ICore_hwiRestore( ti_sysbios_interfaces_ICore_Module mod, xdc_UInt key )
{
    mod->hwiRestore(key);
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

/* getId_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_ICore_getId_FxnT)(void);
static inline ti_sysbios_interfaces_ICore_getId_FxnT ti_sysbios_interfaces_ICore_getId_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_getId_FxnT ti_sysbios_interfaces_ICore_getId_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_getId_FxnT)mod->getId;
}

/* interruptCore_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICore_interruptCore_FxnT)(xdc_UInt coreId);
static inline ti_sysbios_interfaces_ICore_interruptCore_FxnT ti_sysbios_interfaces_ICore_interruptCore_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_interruptCore_FxnT ti_sysbios_interfaces_ICore_interruptCore_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_interruptCore_FxnT)mod->interruptCore;
}

/* lock_{FxnT,fxnP} */
typedef xdc_IArg (*ti_sysbios_interfaces_ICore_lock_FxnT)(void);
static inline ti_sysbios_interfaces_ICore_lock_FxnT ti_sysbios_interfaces_ICore_lock_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_lock_FxnT ti_sysbios_interfaces_ICore_lock_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_lock_FxnT)mod->lock;
}

/* unlock_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICore_unlock_FxnT)(void);
static inline ti_sysbios_interfaces_ICore_unlock_FxnT ti_sysbios_interfaces_ICore_unlock_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_unlock_FxnT ti_sysbios_interfaces_ICore_unlock_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_unlock_FxnT)mod->unlock;
}

/* hwiDisable_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_ICore_hwiDisable_FxnT)(void);
static inline ti_sysbios_interfaces_ICore_hwiDisable_FxnT ti_sysbios_interfaces_ICore_hwiDisable_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_hwiDisable_FxnT ti_sysbios_interfaces_ICore_hwiDisable_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_hwiDisable_FxnT)mod->hwiDisable;
}

/* hwiEnable_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_ICore_hwiEnable_FxnT)(void);
static inline ti_sysbios_interfaces_ICore_hwiEnable_FxnT ti_sysbios_interfaces_ICore_hwiEnable_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_hwiEnable_FxnT ti_sysbios_interfaces_ICore_hwiEnable_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_hwiEnable_FxnT)mod->hwiEnable;
}

/* hwiRestore_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICore_hwiRestore_FxnT)(xdc_UInt key);
static inline ti_sysbios_interfaces_ICore_hwiRestore_FxnT ti_sysbios_interfaces_ICore_hwiRestore_fxnP(ti_sysbios_interfaces_ICore_Module mod);
static inline ti_sysbios_interfaces_ICore_hwiRestore_FxnT ti_sysbios_interfaces_ICore_hwiRestore_fxnP(ti_sysbios_interfaces_ICore_Module mod)
{
    return (ti_sysbios_interfaces_ICore_hwiRestore_FxnT)mod->hwiRestore;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_ICore__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_ICore__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_ICore__nolocalnames)

#ifndef ti_sysbios_interfaces_ICore__localnames__done
#define ti_sysbios_interfaces_ICore__localnames__done

/* module prefix */
#define ICore_Module ti_sysbios_interfaces_ICore_Module
#define ICore_getId ti_sysbios_interfaces_ICore_getId
#define ICore_getId_fxnP ti_sysbios_interfaces_ICore_getId_fxnP
#define ICore_getId_FxnT ti_sysbios_interfaces_ICore_getId_FxnT
#define ICore_interruptCore ti_sysbios_interfaces_ICore_interruptCore
#define ICore_interruptCore_fxnP ti_sysbios_interfaces_ICore_interruptCore_fxnP
#define ICore_interruptCore_FxnT ti_sysbios_interfaces_ICore_interruptCore_FxnT
#define ICore_lock ti_sysbios_interfaces_ICore_lock
#define ICore_lock_fxnP ti_sysbios_interfaces_ICore_lock_fxnP
#define ICore_lock_FxnT ti_sysbios_interfaces_ICore_lock_FxnT
#define ICore_unlock ti_sysbios_interfaces_ICore_unlock
#define ICore_unlock_fxnP ti_sysbios_interfaces_ICore_unlock_fxnP
#define ICore_unlock_FxnT ti_sysbios_interfaces_ICore_unlock_FxnT
#define ICore_hwiDisable ti_sysbios_interfaces_ICore_hwiDisable
#define ICore_hwiDisable_fxnP ti_sysbios_interfaces_ICore_hwiDisable_fxnP
#define ICore_hwiDisable_FxnT ti_sysbios_interfaces_ICore_hwiDisable_FxnT
#define ICore_hwiEnable ti_sysbios_interfaces_ICore_hwiEnable
#define ICore_hwiEnable_fxnP ti_sysbios_interfaces_ICore_hwiEnable_fxnP
#define ICore_hwiEnable_FxnT ti_sysbios_interfaces_ICore_hwiEnable_FxnT
#define ICore_hwiRestore ti_sysbios_interfaces_ICore_hwiRestore
#define ICore_hwiRestore_fxnP ti_sysbios_interfaces_ICore_hwiRestore_fxnP
#define ICore_hwiRestore_FxnT ti_sysbios_interfaces_ICore_hwiRestore_FxnT
#define ICore_Module_name ti_sysbios_interfaces_ICore_Module_name

#endif /* ti_sysbios_interfaces_ICore__localnames__done */
#endif
