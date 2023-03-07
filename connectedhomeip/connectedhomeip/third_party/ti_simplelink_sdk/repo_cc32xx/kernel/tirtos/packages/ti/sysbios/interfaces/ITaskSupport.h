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

#ifndef ti_sysbios_interfaces_ITaskSupport__include
#define ti_sysbios_interfaces_ITaskSupport__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_ITaskSupport__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_ITaskSupport___VERS 200


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

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_interfaces_ITaskSupport_FuncPtr)(void);


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_ITaskSupport_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Ptr (*start)(xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block* eb);
    xdc_Void (*swap)(xdc_Ptr* oldtskContext, xdc_Ptr* newtskContext);
    xdc_Bool (*checkStack)(xdc_Char* stack, xdc_SizeT size);
    xdc_SizeT (*stackUsed)(xdc_Char* stack, xdc_SizeT size);
    xdc_UInt (*getStackAlignment)(void);
    xdc_SizeT (*getDefaultStackSize)(void);
    xdc_Ptr (*getCheckValueAddr)(xdc_Ptr curTask);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_ITaskSupport_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_ITaskSupport_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_ITaskSupport_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_ITaskSupport_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ITaskSupport_Module_id(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ITaskSupport_Module_id(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return mod->__sysp->__mid;
}

/* start */
static inline xdc_Ptr ti_sysbios_interfaces_ITaskSupport_start(ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block *eb);
static inline xdc_Ptr ti_sysbios_interfaces_ITaskSupport_start( ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block *eb )
{
    return mod->start(curTask, enter, exit, eb);
}

/* swap */
static inline xdc_Void ti_sysbios_interfaces_ITaskSupport_swap(ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr *oldtskContext, xdc_Ptr *newtskContext);
static inline xdc_Void ti_sysbios_interfaces_ITaskSupport_swap( ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr *oldtskContext, xdc_Ptr *newtskContext )
{
    mod->swap(oldtskContext, newtskContext);
}

/* checkStack */
static inline xdc_Bool ti_sysbios_interfaces_ITaskSupport_checkStack(ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Char *stack, xdc_SizeT size);
static inline xdc_Bool ti_sysbios_interfaces_ITaskSupport_checkStack( ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Char *stack, xdc_SizeT size )
{
    return mod->checkStack(stack, size);
}

/* stackUsed */
static inline xdc_SizeT ti_sysbios_interfaces_ITaskSupport_stackUsed(ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Char *stack, xdc_SizeT size);
static inline xdc_SizeT ti_sysbios_interfaces_ITaskSupport_stackUsed( ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Char *stack, xdc_SizeT size )
{
    return mod->stackUsed(stack, size);
}

/* getStackAlignment */
static inline xdc_UInt ti_sysbios_interfaces_ITaskSupport_getStackAlignment(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_ITaskSupport_getStackAlignment( ti_sysbios_interfaces_ITaskSupport_Module mod )
{
    return mod->getStackAlignment();
}

/* getDefaultStackSize */
static inline xdc_SizeT ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline xdc_SizeT ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize( ti_sysbios_interfaces_ITaskSupport_Module mod )
{
    return mod->getDefaultStackSize();
}

/* getCheckValueAddr */
static inline xdc_Ptr ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr(ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr curTask);
static inline xdc_Ptr ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr( ti_sysbios_interfaces_ITaskSupport_Module mod, xdc_Ptr curTask )
{
    return mod->getCheckValueAddr(curTask);
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

/* start_{FxnT,fxnP} */
typedef xdc_Ptr (*ti_sysbios_interfaces_ITaskSupport_start_FxnT)(xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block* eb);
static inline ti_sysbios_interfaces_ITaskSupport_start_FxnT ti_sysbios_interfaces_ITaskSupport_start_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_start_FxnT ti_sysbios_interfaces_ITaskSupport_start_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_start_FxnT)mod->start;
}

/* swap_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ITaskSupport_swap_FxnT)(xdc_Ptr* oldtskContext, xdc_Ptr* newtskContext);
static inline ti_sysbios_interfaces_ITaskSupport_swap_FxnT ti_sysbios_interfaces_ITaskSupport_swap_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_swap_FxnT ti_sysbios_interfaces_ITaskSupport_swap_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_swap_FxnT)mod->swap;
}

/* checkStack_{FxnT,fxnP} */
typedef xdc_Bool (*ti_sysbios_interfaces_ITaskSupport_checkStack_FxnT)(xdc_Char* stack, xdc_SizeT size);
static inline ti_sysbios_interfaces_ITaskSupport_checkStack_FxnT ti_sysbios_interfaces_ITaskSupport_checkStack_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_checkStack_FxnT ti_sysbios_interfaces_ITaskSupport_checkStack_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_checkStack_FxnT)mod->checkStack;
}

/* stackUsed_{FxnT,fxnP} */
typedef xdc_SizeT (*ti_sysbios_interfaces_ITaskSupport_stackUsed_FxnT)(xdc_Char* stack, xdc_SizeT size);
static inline ti_sysbios_interfaces_ITaskSupport_stackUsed_FxnT ti_sysbios_interfaces_ITaskSupport_stackUsed_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_stackUsed_FxnT ti_sysbios_interfaces_ITaskSupport_stackUsed_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_stackUsed_FxnT)mod->stackUsed;
}

/* getStackAlignment_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_ITaskSupport_getStackAlignment_FxnT)(void);
static inline ti_sysbios_interfaces_ITaskSupport_getStackAlignment_FxnT ti_sysbios_interfaces_ITaskSupport_getStackAlignment_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_getStackAlignment_FxnT ti_sysbios_interfaces_ITaskSupport_getStackAlignment_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_getStackAlignment_FxnT)mod->getStackAlignment;
}

/* getDefaultStackSize_{FxnT,fxnP} */
typedef xdc_SizeT (*ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_FxnT)(void);
static inline ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_FxnT ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_FxnT ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_FxnT)mod->getDefaultStackSize;
}

/* getCheckValueAddr_{FxnT,fxnP} */
typedef xdc_Ptr (*ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_FxnT)(xdc_Ptr curTask);
static inline ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_FxnT ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod);
static inline ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_FxnT ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_fxnP(ti_sysbios_interfaces_ITaskSupport_Module mod)
{
    return (ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_FxnT)mod->getCheckValueAddr;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_ITaskSupport__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_ITaskSupport__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_ITaskSupport__nolocalnames)

#ifndef ti_sysbios_interfaces_ITaskSupport__localnames__done
#define ti_sysbios_interfaces_ITaskSupport__localnames__done

/* module prefix */
#define ITaskSupport_Module ti_sysbios_interfaces_ITaskSupport_Module
#define ITaskSupport_FuncPtr ti_sysbios_interfaces_ITaskSupport_FuncPtr
#define ITaskSupport_start ti_sysbios_interfaces_ITaskSupport_start
#define ITaskSupport_start_fxnP ti_sysbios_interfaces_ITaskSupport_start_fxnP
#define ITaskSupport_start_FxnT ti_sysbios_interfaces_ITaskSupport_start_FxnT
#define ITaskSupport_swap ti_sysbios_interfaces_ITaskSupport_swap
#define ITaskSupport_swap_fxnP ti_sysbios_interfaces_ITaskSupport_swap_fxnP
#define ITaskSupport_swap_FxnT ti_sysbios_interfaces_ITaskSupport_swap_FxnT
#define ITaskSupport_checkStack ti_sysbios_interfaces_ITaskSupport_checkStack
#define ITaskSupport_checkStack_fxnP ti_sysbios_interfaces_ITaskSupport_checkStack_fxnP
#define ITaskSupport_checkStack_FxnT ti_sysbios_interfaces_ITaskSupport_checkStack_FxnT
#define ITaskSupport_stackUsed ti_sysbios_interfaces_ITaskSupport_stackUsed
#define ITaskSupport_stackUsed_fxnP ti_sysbios_interfaces_ITaskSupport_stackUsed_fxnP
#define ITaskSupport_stackUsed_FxnT ti_sysbios_interfaces_ITaskSupport_stackUsed_FxnT
#define ITaskSupport_getStackAlignment ti_sysbios_interfaces_ITaskSupport_getStackAlignment
#define ITaskSupport_getStackAlignment_fxnP ti_sysbios_interfaces_ITaskSupport_getStackAlignment_fxnP
#define ITaskSupport_getStackAlignment_FxnT ti_sysbios_interfaces_ITaskSupport_getStackAlignment_FxnT
#define ITaskSupport_getDefaultStackSize ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize
#define ITaskSupport_getDefaultStackSize_fxnP ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_fxnP
#define ITaskSupport_getDefaultStackSize_FxnT ti_sysbios_interfaces_ITaskSupport_getDefaultStackSize_FxnT
#define ITaskSupport_getCheckValueAddr ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr
#define ITaskSupport_getCheckValueAddr_fxnP ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_fxnP
#define ITaskSupport_getCheckValueAddr_FxnT ti_sysbios_interfaces_ITaskSupport_getCheckValueAddr_FxnT
#define ITaskSupport_Module_name ti_sysbios_interfaces_ITaskSupport_Module_name

#endif /* ti_sysbios_interfaces_ITaskSupport__localnames__done */
#endif
