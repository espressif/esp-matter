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

#ifndef ti_sysbios_interfaces_ICache__include
#define ti_sysbios_interfaces_ICache__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_ICache__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_ICache___VERS 200


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

/* Type */
enum ti_sysbios_interfaces_ICache_Type {
    ti_sysbios_interfaces_ICache_Type_L1P = 0x1,
    ti_sysbios_interfaces_ICache_Type_L1D = 0x2,
    ti_sysbios_interfaces_ICache_Type_L1 = 0x3,
    ti_sysbios_interfaces_ICache_Type_L2P = 0x4,
    ti_sysbios_interfaces_ICache_Type_L2D = 0x8,
    ti_sysbios_interfaces_ICache_Type_L2 = 0xC,
    ti_sysbios_interfaces_ICache_Type_ALLP = 0x5,
    ti_sysbios_interfaces_ICache_Type_ALLD = 0xA,
    ti_sysbios_interfaces_ICache_Type_ALL = 0x7fff
};
typedef enum ti_sysbios_interfaces_ICache_Type ti_sysbios_interfaces_ICache_Type;


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_ICache_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*enable)(xdc_Bits16 type);
    xdc_Void (*disable)(xdc_Bits16 type);
    xdc_Void (*inv)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
    xdc_Void (*wb)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
    xdc_Void (*wbInv)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
    xdc_Void (*wbAll)(void);
    xdc_Void (*wbInvAll)(void);
    xdc_Void (*wait)(void);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_ICache_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_ICache_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_ICache_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_ICache_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ICache_Module_id(ti_sysbios_interfaces_ICache_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_ICache_Module_id(ti_sysbios_interfaces_ICache_Module mod)
{
    return mod->__sysp->__mid;
}

/* enable */
static inline xdc_Void ti_sysbios_interfaces_ICache_enable(ti_sysbios_interfaces_ICache_Module mod, xdc_Bits16 type);
static inline xdc_Void ti_sysbios_interfaces_ICache_enable( ti_sysbios_interfaces_ICache_Module mod, xdc_Bits16 type )
{
    mod->enable(type);
}

/* disable */
static inline xdc_Void ti_sysbios_interfaces_ICache_disable(ti_sysbios_interfaces_ICache_Module mod, xdc_Bits16 type);
static inline xdc_Void ti_sysbios_interfaces_ICache_disable( ti_sysbios_interfaces_ICache_Module mod, xdc_Bits16 type )
{
    mod->disable(type);
}

/* inv */
static inline xdc_Void ti_sysbios_interfaces_ICache_inv(ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline xdc_Void ti_sysbios_interfaces_ICache_inv( ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    mod->inv(blockPtr, byteCnt, type, wait);
}

/* wb */
static inline xdc_Void ti_sysbios_interfaces_ICache_wb(ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline xdc_Void ti_sysbios_interfaces_ICache_wb( ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    mod->wb(blockPtr, byteCnt, type, wait);
}

/* wbInv */
static inline xdc_Void ti_sysbios_interfaces_ICache_wbInv(ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline xdc_Void ti_sysbios_interfaces_ICache_wbInv( ti_sysbios_interfaces_ICache_Module mod, xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    mod->wbInv(blockPtr, byteCnt, type, wait);
}

/* wbAll */
static inline xdc_Void ti_sysbios_interfaces_ICache_wbAll(ti_sysbios_interfaces_ICache_Module mod);
static inline xdc_Void ti_sysbios_interfaces_ICache_wbAll( ti_sysbios_interfaces_ICache_Module mod )
{
    mod->wbAll();
}

/* wbInvAll */
static inline xdc_Void ti_sysbios_interfaces_ICache_wbInvAll(ti_sysbios_interfaces_ICache_Module mod);
static inline xdc_Void ti_sysbios_interfaces_ICache_wbInvAll( ti_sysbios_interfaces_ICache_Module mod )
{
    mod->wbInvAll();
}

/* wait */
static inline xdc_Void ti_sysbios_interfaces_ICache_wait(ti_sysbios_interfaces_ICache_Module mod);
static inline xdc_Void ti_sysbios_interfaces_ICache_wait( ti_sysbios_interfaces_ICache_Module mod )
{
    mod->wait();
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

/* enable_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_enable_FxnT)(xdc_Bits16 type);
static inline ti_sysbios_interfaces_ICache_enable_FxnT ti_sysbios_interfaces_ICache_enable_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_enable_FxnT ti_sysbios_interfaces_ICache_enable_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_enable_FxnT)mod->enable;
}

/* disable_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_disable_FxnT)(xdc_Bits16 type);
static inline ti_sysbios_interfaces_ICache_disable_FxnT ti_sysbios_interfaces_ICache_disable_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_disable_FxnT ti_sysbios_interfaces_ICache_disable_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_disable_FxnT)mod->disable;
}

/* inv_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_inv_FxnT)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline ti_sysbios_interfaces_ICache_inv_FxnT ti_sysbios_interfaces_ICache_inv_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_inv_FxnT ti_sysbios_interfaces_ICache_inv_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_inv_FxnT)mod->inv;
}

/* wb_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_wb_FxnT)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline ti_sysbios_interfaces_ICache_wb_FxnT ti_sysbios_interfaces_ICache_wb_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_wb_FxnT ti_sysbios_interfaces_ICache_wb_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_wb_FxnT)mod->wb;
}

/* wbInv_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_wbInv_FxnT)(xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait);
static inline ti_sysbios_interfaces_ICache_wbInv_FxnT ti_sysbios_interfaces_ICache_wbInv_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_wbInv_FxnT ti_sysbios_interfaces_ICache_wbInv_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_wbInv_FxnT)mod->wbInv;
}

/* wbAll_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_wbAll_FxnT)(void);
static inline ti_sysbios_interfaces_ICache_wbAll_FxnT ti_sysbios_interfaces_ICache_wbAll_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_wbAll_FxnT ti_sysbios_interfaces_ICache_wbAll_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_wbAll_FxnT)mod->wbAll;
}

/* wbInvAll_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_wbInvAll_FxnT)(void);
static inline ti_sysbios_interfaces_ICache_wbInvAll_FxnT ti_sysbios_interfaces_ICache_wbInvAll_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_wbInvAll_FxnT ti_sysbios_interfaces_ICache_wbInvAll_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_wbInvAll_FxnT)mod->wbInvAll;
}

/* wait_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_ICache_wait_FxnT)(void);
static inline ti_sysbios_interfaces_ICache_wait_FxnT ti_sysbios_interfaces_ICache_wait_fxnP(ti_sysbios_interfaces_ICache_Module mod);
static inline ti_sysbios_interfaces_ICache_wait_FxnT ti_sysbios_interfaces_ICache_wait_fxnP(ti_sysbios_interfaces_ICache_Module mod)
{
    return (ti_sysbios_interfaces_ICache_wait_FxnT)mod->wait;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_ICache__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_ICache__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_ICache__nolocalnames)

#ifndef ti_sysbios_interfaces_ICache__localnames__done
#define ti_sysbios_interfaces_ICache__localnames__done

/* module prefix */
#define ICache_Module ti_sysbios_interfaces_ICache_Module
#define ICache_Type ti_sysbios_interfaces_ICache_Type
#define ICache_Type_L1P ti_sysbios_interfaces_ICache_Type_L1P
#define ICache_Type_L1D ti_sysbios_interfaces_ICache_Type_L1D
#define ICache_Type_L1 ti_sysbios_interfaces_ICache_Type_L1
#define ICache_Type_L2P ti_sysbios_interfaces_ICache_Type_L2P
#define ICache_Type_L2D ti_sysbios_interfaces_ICache_Type_L2D
#define ICache_Type_L2 ti_sysbios_interfaces_ICache_Type_L2
#define ICache_Type_ALLP ti_sysbios_interfaces_ICache_Type_ALLP
#define ICache_Type_ALLD ti_sysbios_interfaces_ICache_Type_ALLD
#define ICache_Type_ALL ti_sysbios_interfaces_ICache_Type_ALL
#define ICache_enable ti_sysbios_interfaces_ICache_enable
#define ICache_enable_fxnP ti_sysbios_interfaces_ICache_enable_fxnP
#define ICache_enable_FxnT ti_sysbios_interfaces_ICache_enable_FxnT
#define ICache_disable ti_sysbios_interfaces_ICache_disable
#define ICache_disable_fxnP ti_sysbios_interfaces_ICache_disable_fxnP
#define ICache_disable_FxnT ti_sysbios_interfaces_ICache_disable_FxnT
#define ICache_inv ti_sysbios_interfaces_ICache_inv
#define ICache_inv_fxnP ti_sysbios_interfaces_ICache_inv_fxnP
#define ICache_inv_FxnT ti_sysbios_interfaces_ICache_inv_FxnT
#define ICache_wb ti_sysbios_interfaces_ICache_wb
#define ICache_wb_fxnP ti_sysbios_interfaces_ICache_wb_fxnP
#define ICache_wb_FxnT ti_sysbios_interfaces_ICache_wb_FxnT
#define ICache_wbInv ti_sysbios_interfaces_ICache_wbInv
#define ICache_wbInv_fxnP ti_sysbios_interfaces_ICache_wbInv_fxnP
#define ICache_wbInv_FxnT ti_sysbios_interfaces_ICache_wbInv_FxnT
#define ICache_wbAll ti_sysbios_interfaces_ICache_wbAll
#define ICache_wbAll_fxnP ti_sysbios_interfaces_ICache_wbAll_fxnP
#define ICache_wbAll_FxnT ti_sysbios_interfaces_ICache_wbAll_FxnT
#define ICache_wbInvAll ti_sysbios_interfaces_ICache_wbInvAll
#define ICache_wbInvAll_fxnP ti_sysbios_interfaces_ICache_wbInvAll_fxnP
#define ICache_wbInvAll_FxnT ti_sysbios_interfaces_ICache_wbInvAll_FxnT
#define ICache_wait ti_sysbios_interfaces_ICache_wait
#define ICache_wait_fxnP ti_sysbios_interfaces_ICache_wait_fxnP
#define ICache_wait_FxnT ti_sysbios_interfaces_ICache_wait_FxnT
#define ICache_Module_name ti_sysbios_interfaces_ICache_Module_name

#endif /* ti_sysbios_interfaces_ICache__localnames__done */
#endif
