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
 *     CONVERTORS
 *
 *     EPILOGUE
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_uia_runtime_IUIATimestampProvider__include
#define ti_uia_runtime_IUIATimestampProvider__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_IUIATimestampProvider__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_IUIATimestampProvider___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/ITimestampProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_IUIATimestampProvider_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bits32 (*get32)(void);
    xdc_Void (*get64)(xdc_runtime_Types_Timestamp64* result);
    xdc_Void (*getFreq)(xdc_runtime_Types_FreqHz* freq);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_IUIATimestampProvider_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_uia_runtime_IUIATimestampProvider_Interface__BASE__C;
#else
#define ti_uia_runtime_IUIATimestampProvider_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_uia_runtime_IUIATimestampProvider_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATimestampProvider_Module_id(ti_uia_runtime_IUIATimestampProvider_Module mod);
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATimestampProvider_Module_id(ti_uia_runtime_IUIATimestampProvider_Module mod)
{
    return mod->__sysp->__mid;
}

/* get32 */
static inline xdc_Bits32 ti_uia_runtime_IUIATimestampProvider_get32(ti_uia_runtime_IUIATimestampProvider_Module mod);
static inline xdc_Bits32 ti_uia_runtime_IUIATimestampProvider_get32( ti_uia_runtime_IUIATimestampProvider_Module mod )
{
    return mod->get32();
}

/* get64 */
static inline xdc_Void ti_uia_runtime_IUIATimestampProvider_get64(ti_uia_runtime_IUIATimestampProvider_Module mod, xdc_runtime_Types_Timestamp64 *result);
static inline xdc_Void ti_uia_runtime_IUIATimestampProvider_get64( ti_uia_runtime_IUIATimestampProvider_Module mod, xdc_runtime_Types_Timestamp64 *result )
{
    mod->get64(result);
}

/* getFreq */
static inline xdc_Void ti_uia_runtime_IUIATimestampProvider_getFreq(ti_uia_runtime_IUIATimestampProvider_Module mod, xdc_runtime_Types_FreqHz *freq);
static inline xdc_Void ti_uia_runtime_IUIATimestampProvider_getFreq( ti_uia_runtime_IUIATimestampProvider_Module mod, xdc_runtime_Types_FreqHz *freq )
{
    mod->getFreq(freq);
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

/* get32_{FxnT,fxnP} */
typedef xdc_Bits32 (*ti_uia_runtime_IUIATimestampProvider_get32_FxnT)(void);
static inline ti_uia_runtime_IUIATimestampProvider_get32_FxnT ti_uia_runtime_IUIATimestampProvider_get32_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod);
static inline ti_uia_runtime_IUIATimestampProvider_get32_FxnT ti_uia_runtime_IUIATimestampProvider_get32_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod)
{
    return (ti_uia_runtime_IUIATimestampProvider_get32_FxnT)mod->get32;
}

/* get64_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IUIATimestampProvider_get64_FxnT)(xdc_runtime_Types_Timestamp64* result);
static inline ti_uia_runtime_IUIATimestampProvider_get64_FxnT ti_uia_runtime_IUIATimestampProvider_get64_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod);
static inline ti_uia_runtime_IUIATimestampProvider_get64_FxnT ti_uia_runtime_IUIATimestampProvider_get64_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod)
{
    return (ti_uia_runtime_IUIATimestampProvider_get64_FxnT)mod->get64;
}

/* getFreq_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IUIATimestampProvider_getFreq_FxnT)(xdc_runtime_Types_FreqHz* freq);
static inline ti_uia_runtime_IUIATimestampProvider_getFreq_FxnT ti_uia_runtime_IUIATimestampProvider_getFreq_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod);
static inline ti_uia_runtime_IUIATimestampProvider_getFreq_FxnT ti_uia_runtime_IUIATimestampProvider_getFreq_fxnP(ti_uia_runtime_IUIATimestampProvider_Module mod)
{
    return (ti_uia_runtime_IUIATimestampProvider_getFreq_FxnT)mod->getFreq;
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_ITimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_upCast(ti_uia_runtime_IUIATimestampProvider_Module m);
static inline xdc_runtime_ITimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_upCast(ti_uia_runtime_IUIATimestampProvider_Module m)
{
    return(xdc_runtime_ITimestampProvider_Module)m;
}

/* Module_to_xdc_runtime_ITimestampProvider */
#define ti_uia_runtime_IUIATimestampProvider_Module_to_xdc_runtime_ITimestampProvider ti_uia_runtime_IUIATimestampProvider_Module_upCast

/* Module_downCast */
static inline ti_uia_runtime_IUIATimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_downCast(xdc_runtime_ITimestampProvider_Module m);
static inline ti_uia_runtime_IUIATimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_downCast(xdc_runtime_ITimestampProvider_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_IUIATimestampProvider_Interface__BASE__C) {
            return (ti_uia_runtime_IUIATimestampProvider_Module)m;
        }
    }
    return NULL;
}

/* Module_from_xdc_runtime_ITimestampProvider */
#define ti_uia_runtime_IUIATimestampProvider_Module_from_xdc_runtime_ITimestampProvider ti_uia_runtime_IUIATimestampProvider_Module_downCast

/* Module_upCast2 */
static inline xdc_runtime_ITimestampClient_Module ti_uia_runtime_IUIATimestampProvider_Module_upCast2(ti_uia_runtime_IUIATimestampProvider_Module m);
static inline xdc_runtime_ITimestampClient_Module ti_uia_runtime_IUIATimestampProvider_Module_upCast2(ti_uia_runtime_IUIATimestampProvider_Module m)
{
    return(xdc_runtime_ITimestampClient_Module)m;
}

/* Module_to_xdc_runtime_ITimestampClient */
#define ti_uia_runtime_IUIATimestampProvider_Module_to_xdc_runtime_ITimestampClient ti_uia_runtime_IUIATimestampProvider_Module_upCast2

/* Module_downCast2 */
static inline ti_uia_runtime_IUIATimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_downCast2(xdc_runtime_ITimestampClient_Module m);
static inline ti_uia_runtime_IUIATimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module_downCast2(xdc_runtime_ITimestampClient_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_IUIATimestampProvider_Interface__BASE__C) {
            return (ti_uia_runtime_IUIATimestampProvider_Module)m;
        }
    }
    return NULL;
}

/* Module_from_xdc_runtime_ITimestampClient */
#define ti_uia_runtime_IUIATimestampProvider_Module_from_xdc_runtime_ITimestampClient ti_uia_runtime_IUIATimestampProvider_Module_downCast2


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_IUIATimestampProvider__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_IUIATimestampProvider__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_IUIATimestampProvider__nolocalnames)

#ifndef ti_uia_runtime_IUIATimestampProvider__localnames__done
#define ti_uia_runtime_IUIATimestampProvider__localnames__done

/* module prefix */
#define IUIATimestampProvider_Module ti_uia_runtime_IUIATimestampProvider_Module
#define IUIATimestampProvider_get32 ti_uia_runtime_IUIATimestampProvider_get32
#define IUIATimestampProvider_get32_fxnP ti_uia_runtime_IUIATimestampProvider_get32_fxnP
#define IUIATimestampProvider_get32_FxnT ti_uia_runtime_IUIATimestampProvider_get32_FxnT
#define IUIATimestampProvider_get64 ti_uia_runtime_IUIATimestampProvider_get64
#define IUIATimestampProvider_get64_fxnP ti_uia_runtime_IUIATimestampProvider_get64_fxnP
#define IUIATimestampProvider_get64_FxnT ti_uia_runtime_IUIATimestampProvider_get64_FxnT
#define IUIATimestampProvider_getFreq ti_uia_runtime_IUIATimestampProvider_getFreq
#define IUIATimestampProvider_getFreq_fxnP ti_uia_runtime_IUIATimestampProvider_getFreq_fxnP
#define IUIATimestampProvider_getFreq_FxnT ti_uia_runtime_IUIATimestampProvider_getFreq_FxnT
#define IUIATimestampProvider_Module_name ti_uia_runtime_IUIATimestampProvider_Module_name
#define IUIATimestampProvider_Module_upCast ti_uia_runtime_IUIATimestampProvider_Module_upCast
#define IUIATimestampProvider_Module_to_xdc_runtime_ITimestampProvider ti_uia_runtime_IUIATimestampProvider_Module_to_xdc_runtime_ITimestampProvider
#define IUIATimestampProvider_Module_downCast ti_uia_runtime_IUIATimestampProvider_Module_downCast
#define IUIATimestampProvider_Module_from_xdc_runtime_ITimestampProvider ti_uia_runtime_IUIATimestampProvider_Module_from_xdc_runtime_ITimestampProvider
#define IUIATimestampProvider_Module_upCast2 ti_uia_runtime_IUIATimestampProvider_Module_upCast2
#define IUIATimestampProvider_Module_to_xdc_runtime_ITimestampClient ti_uia_runtime_IUIATimestampProvider_Module_to_xdc_runtime_ITimestampClient
#define IUIATimestampProvider_Module_downCast2 ti_uia_runtime_IUIATimestampProvider_Module_downCast2
#define IUIATimestampProvider_Module_from_xdc_runtime_ITimestampClient ti_uia_runtime_IUIATimestampProvider_Module_from_xdc_runtime_ITimestampClient

#endif /* ti_uia_runtime_IUIATimestampProvider__localnames__done */
#endif
