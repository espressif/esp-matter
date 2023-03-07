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

#ifndef ti_uia_runtime_IUIATraceSyncClient__include
#define ti_uia_runtime_IUIATraceSyncClient__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_IUIATraceSyncClient__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_IUIATraceSyncClient___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <ti/uia/runtime/IUIATraceSyncProvider.h>
#include <ti/uia/events/IUIAMetaProvider.h>
#include <ti/uia/runtime/LoggerTypes.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_IUIATraceSyncClient_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_IUIATraceSyncClient_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_uia_runtime_IUIATraceSyncClient_Interface__BASE__C;
#else
#define ti_uia_runtime_IUIATraceSyncClient_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_uia_runtime_IUIATraceSyncClient_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATraceSyncClient_Module_id(ti_uia_runtime_IUIATraceSyncClient_Module mod);
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IUIATraceSyncClient_Module_id(ti_uia_runtime_IUIATraceSyncClient_Module mod)
{
    return mod->__sysp->__mid;
}


/*
 * ======== FUNCTION SELECTORS ========
 */


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_IUIATraceSyncClient_Module_upCast(ti_uia_runtime_IUIATraceSyncClient_Module m);
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_IUIATraceSyncClient_Module_upCast(ti_uia_runtime_IUIATraceSyncClient_Module m)
{
    return(ti_uia_events_IUIAMetaProvider_Module)m;
}

/* Module_to_ti_uia_events_IUIAMetaProvider */
#define ti_uia_runtime_IUIATraceSyncClient_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncClient_Module_upCast

/* Module_downCast */
static inline ti_uia_runtime_IUIATraceSyncClient_Module ti_uia_runtime_IUIATraceSyncClient_Module_downCast(ti_uia_events_IUIAMetaProvider_Module m);
static inline ti_uia_runtime_IUIATraceSyncClient_Module ti_uia_runtime_IUIATraceSyncClient_Module_downCast(ti_uia_events_IUIAMetaProvider_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_IUIATraceSyncClient_Interface__BASE__C) {
            return (ti_uia_runtime_IUIATraceSyncClient_Module)m;
        }
    }
    return NULL;
}

/* Module_from_ti_uia_events_IUIAMetaProvider */
#define ti_uia_runtime_IUIATraceSyncClient_Module_from_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncClient_Module_downCast


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_IUIATraceSyncClient__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_IUIATraceSyncClient__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_IUIATraceSyncClient__nolocalnames)

#ifndef ti_uia_runtime_IUIATraceSyncClient__localnames__done
#define ti_uia_runtime_IUIATraceSyncClient__localnames__done

/* module prefix */
#define IUIATraceSyncClient_Module ti_uia_runtime_IUIATraceSyncClient_Module
#define IUIATraceSyncClient_Module_name ti_uia_runtime_IUIATraceSyncClient_Module_name
#define IUIATraceSyncClient_Module_upCast ti_uia_runtime_IUIATraceSyncClient_Module_upCast
#define IUIATraceSyncClient_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncClient_Module_to_ti_uia_events_IUIAMetaProvider
#define IUIATraceSyncClient_Module_downCast ti_uia_runtime_IUIATraceSyncClient_Module_downCast
#define IUIATraceSyncClient_Module_from_ti_uia_events_IUIAMetaProvider ti_uia_runtime_IUIATraceSyncClient_Module_from_ti_uia_events_IUIAMetaProvider

#endif /* ti_uia_runtime_IUIATraceSyncClient__localnames__done */
#endif
