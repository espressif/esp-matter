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

#ifndef ti_uia_runtime_IUIATraceSyncProvider__include
#define ti_uia_runtime_IUIATraceSyncProvider__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_IUIATraceSyncProvider__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_IUIATraceSyncProvider___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <ti/uia/events/IUIAMetaProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* ContextType */
enum ti_uia_runtime_IUIATraceSyncProvider_ContextType {
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved0 = 0,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_SyncPoint = 1,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_ContextChange = 2,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Snapshot = 3,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved4 = 4,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved5 = 5,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved6 = 6,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved7 = 7,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved8 = 8,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved9 = 9,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved10 = 10,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved11 = 11,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved12 = 12,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved13 = 13,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_Global32bTimestamp = 14,
    ti_uia_runtime_IUIATraceSyncProvider_ContextType_User = 15
};
typedef enum ti_uia_runtime_IUIATraceSyncProvider_ContextType ti_uia_runtime_IUIATraceSyncProvider_ContextType;


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_IUIATraceSyncProvider_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*injectIntoTrace)(xdc_UInt32 serialNum, ti_uia_runtime_IUIATraceSyncProvider_ContextType ctxType);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_IUIATraceSyncProvider_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_uia_runtime_IUIATraceSyncProvider_Interface__BASE__C;
#else
#define ti_uia_runtime_IUIATraceSyncProvider_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_uia_runtime_IUIATraceSyncProvider_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATraceSyncProvider_Module_id(ti_uia_runtime_IUIATraceSyncProvider_Module mod);
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATraceSyncProvider_Module_id(ti_uia_runtime_IUIATraceSyncProvider_Module mod)
{
    return mod->__sysp->__mid;
}

/* injectIntoTrace */
static inline xdc_Void ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace(ti_uia_runtime_IUIATraceSyncProvider_Module mod, xdc_UInt32 serialNum, ti_uia_runtime_IUIATraceSyncProvider_ContextType ctxType);
static inline xdc_Void ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace( ti_uia_runtime_IUIATraceSyncProvider_Module mod, xdc_UInt32 serialNum, ti_uia_runtime_IUIATraceSyncProvider_ContextType ctxType )
{
    mod->injectIntoTrace(serialNum, ctxType);
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

/* injectIntoTrace_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_FxnT)(xdc_UInt32 serialNum, ti_uia_runtime_IUIATraceSyncProvider_ContextType ctxType);
static inline ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_FxnT ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_fxnP(ti_uia_runtime_IUIATraceSyncProvider_Module mod);
static inline ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_FxnT ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_fxnP(ti_uia_runtime_IUIATraceSyncProvider_Module mod)
{
    return (ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_FxnT)mod->injectIntoTrace;
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_IUIATraceSyncProvider_Module_upCast(ti_uia_runtime_IUIATraceSyncProvider_Module m);
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_IUIATraceSyncProvider_Module_upCast(ti_uia_runtime_IUIATraceSyncProvider_Module m)
{
    return(ti_uia_events_IUIAMetaProvider_Module)m;
}

/* Module_to_ti_uia_events_IUIAMetaProvider */
#define ti_uia_runtime_IUIATraceSyncProvider_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncProvider_Module_upCast

/* Module_downCast */
static inline ti_uia_runtime_IUIATraceSyncProvider_Module ti_uia_runtime_IUIATraceSyncProvider_Module_downCast(ti_uia_events_IUIAMetaProvider_Module m);
static inline ti_uia_runtime_IUIATraceSyncProvider_Module ti_uia_runtime_IUIATraceSyncProvider_Module_downCast(ti_uia_events_IUIAMetaProvider_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_IUIATraceSyncProvider_Interface__BASE__C) {
            return (ti_uia_runtime_IUIATraceSyncProvider_Module)m;
        }
    }
    return NULL;
}

/* Module_from_ti_uia_events_IUIAMetaProvider */
#define ti_uia_runtime_IUIATraceSyncProvider_Module_from_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncProvider_Module_downCast


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_IUIATraceSyncProvider__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_IUIATraceSyncProvider__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_IUIATraceSyncProvider__nolocalnames)

#ifndef ti_uia_runtime_IUIATraceSyncProvider__localnames__done
#define ti_uia_runtime_IUIATraceSyncProvider__localnames__done

/* module prefix */
#define IUIATraceSyncProvider_Module ti_uia_runtime_IUIATraceSyncProvider_Module
#define IUIATraceSyncProvider_ContextType ti_uia_runtime_IUIATraceSyncProvider_ContextType
#define IUIATraceSyncProvider_ContextType_Reserved0 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved0
#define IUIATraceSyncProvider_ContextType_SyncPoint ti_uia_runtime_IUIATraceSyncProvider_ContextType_SyncPoint
#define IUIATraceSyncProvider_ContextType_ContextChange ti_uia_runtime_IUIATraceSyncProvider_ContextType_ContextChange
#define IUIATraceSyncProvider_ContextType_Snapshot ti_uia_runtime_IUIATraceSyncProvider_ContextType_Snapshot
#define IUIATraceSyncProvider_ContextType_Reserved4 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved4
#define IUIATraceSyncProvider_ContextType_Reserved5 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved5
#define IUIATraceSyncProvider_ContextType_Reserved6 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved6
#define IUIATraceSyncProvider_ContextType_Reserved7 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved7
#define IUIATraceSyncProvider_ContextType_Reserved8 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved8
#define IUIATraceSyncProvider_ContextType_Reserved9 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved9
#define IUIATraceSyncProvider_ContextType_Reserved10 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved10
#define IUIATraceSyncProvider_ContextType_Reserved11 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved11
#define IUIATraceSyncProvider_ContextType_Reserved12 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved12
#define IUIATraceSyncProvider_ContextType_Reserved13 ti_uia_runtime_IUIATraceSyncProvider_ContextType_Reserved13
#define IUIATraceSyncProvider_ContextType_Global32bTimestamp ti_uia_runtime_IUIATraceSyncProvider_ContextType_Global32bTimestamp
#define IUIATraceSyncProvider_ContextType_User ti_uia_runtime_IUIATraceSyncProvider_ContextType_User
#define IUIATraceSyncProvider_injectIntoTrace ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace
#define IUIATraceSyncProvider_injectIntoTrace_fxnP ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_fxnP
#define IUIATraceSyncProvider_injectIntoTrace_FxnT ti_uia_runtime_IUIATraceSyncProvider_injectIntoTrace_FxnT
#define IUIATraceSyncProvider_Module_name ti_uia_runtime_IUIATraceSyncProvider_Module_name
#define IUIATraceSyncProvider_Module_upCast ti_uia_runtime_IUIATraceSyncProvider_Module_upCast
#define IUIATraceSyncProvider_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncProvider_Module_to_ti_uia_events_IUIAMetaProvider
#define IUIATraceSyncProvider_Module_downCast ti_uia_runtime_IUIATraceSyncProvider_Module_downCast
#define IUIATraceSyncProvider_Module_from_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncProvider_Module_from_ti_uia_events_IUIAMetaProvider

#endif /* ti_uia_runtime_IUIATraceSyncProvider__localnames__done */
#endif
