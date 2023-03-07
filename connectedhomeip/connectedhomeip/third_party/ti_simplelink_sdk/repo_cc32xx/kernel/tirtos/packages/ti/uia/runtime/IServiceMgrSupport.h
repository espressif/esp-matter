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

#ifndef ti_uia_runtime_IServiceMgrSupport__include
#define ti_uia_runtime_IServiceMgrSupport__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_IServiceMgrSupport__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_IServiceMgrSupport___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/IModule.h>
#include <ti/uia/runtime/UIAPacket.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_IServiceMgrSupport_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*freePacket)(ti_uia_runtime_UIAPacket_Hdr* packet);
    ti_uia_runtime_UIAPacket_Hdr *(*getFreePacket)(ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout);
    xdc_Void (*requestEnergy)(xdc_Int id);
    xdc_Bool (*sendPacket)(ti_uia_runtime_UIAPacket_Hdr* packet);
    xdc_Void (*setPeriod)(xdc_Int id, xdc_UInt32 periodInMs);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_IServiceMgrSupport_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_uia_runtime_IServiceMgrSupport_Interface__BASE__C;
#else
#define ti_uia_runtime_IServiceMgrSupport_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_uia_runtime_IServiceMgrSupport_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IServiceMgrSupport_Module_id(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_IServiceMgrSupport_Module_id(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return mod->__sysp->__mid;
}

/* freePacket */
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_freePacket(ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_Hdr *packet);
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_freePacket( ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_Hdr *packet )
{
    mod->freePacket(packet);
}

/* getFreePacket */
static inline ti_uia_runtime_UIAPacket_Hdr *ti_uia_runtime_IServiceMgrSupport_getFreePacket(ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout);
static inline ti_uia_runtime_UIAPacket_Hdr *ti_uia_runtime_IServiceMgrSupport_getFreePacket( ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout )
{
    return mod->getFreePacket(type, timeout);
}

/* requestEnergy */
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_requestEnergy(ti_uia_runtime_IServiceMgrSupport_Module mod, xdc_Int id);
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_requestEnergy( ti_uia_runtime_IServiceMgrSupport_Module mod, xdc_Int id )
{
    mod->requestEnergy(id);
}

/* sendPacket */
static inline xdc_Bool ti_uia_runtime_IServiceMgrSupport_sendPacket(ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_Hdr *packet);
static inline xdc_Bool ti_uia_runtime_IServiceMgrSupport_sendPacket( ti_uia_runtime_IServiceMgrSupport_Module mod, ti_uia_runtime_UIAPacket_Hdr *packet )
{
    return mod->sendPacket(packet);
}

/* setPeriod */
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_setPeriod(ti_uia_runtime_IServiceMgrSupport_Module mod, xdc_Int id, xdc_UInt32 periodInMs);
static inline xdc_Void ti_uia_runtime_IServiceMgrSupport_setPeriod( ti_uia_runtime_IServiceMgrSupport_Module mod, xdc_Int id, xdc_UInt32 periodInMs )
{
    mod->setPeriod(id, periodInMs);
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

/* freePacket_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IServiceMgrSupport_freePacket_FxnT)(ti_uia_runtime_UIAPacket_Hdr* packet);
static inline ti_uia_runtime_IServiceMgrSupport_freePacket_FxnT ti_uia_runtime_IServiceMgrSupport_freePacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline ti_uia_runtime_IServiceMgrSupport_freePacket_FxnT ti_uia_runtime_IServiceMgrSupport_freePacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return (ti_uia_runtime_IServiceMgrSupport_freePacket_FxnT)mod->freePacket;
}

/* getFreePacket_{FxnT,fxnP} */
typedef ti_uia_runtime_UIAPacket_Hdr *(*ti_uia_runtime_IServiceMgrSupport_getFreePacket_FxnT)(ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout);
static inline ti_uia_runtime_IServiceMgrSupport_getFreePacket_FxnT ti_uia_runtime_IServiceMgrSupport_getFreePacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline ti_uia_runtime_IServiceMgrSupport_getFreePacket_FxnT ti_uia_runtime_IServiceMgrSupport_getFreePacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return (ti_uia_runtime_IServiceMgrSupport_getFreePacket_FxnT)mod->getFreePacket;
}

/* requestEnergy_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IServiceMgrSupport_requestEnergy_FxnT)(xdc_Int id);
static inline ti_uia_runtime_IServiceMgrSupport_requestEnergy_FxnT ti_uia_runtime_IServiceMgrSupport_requestEnergy_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline ti_uia_runtime_IServiceMgrSupport_requestEnergy_FxnT ti_uia_runtime_IServiceMgrSupport_requestEnergy_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return (ti_uia_runtime_IServiceMgrSupport_requestEnergy_FxnT)mod->requestEnergy;
}

/* sendPacket_{FxnT,fxnP} */
typedef xdc_Bool (*ti_uia_runtime_IServiceMgrSupport_sendPacket_FxnT)(ti_uia_runtime_UIAPacket_Hdr* packet);
static inline ti_uia_runtime_IServiceMgrSupport_sendPacket_FxnT ti_uia_runtime_IServiceMgrSupport_sendPacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline ti_uia_runtime_IServiceMgrSupport_sendPacket_FxnT ti_uia_runtime_IServiceMgrSupport_sendPacket_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return (ti_uia_runtime_IServiceMgrSupport_sendPacket_FxnT)mod->sendPacket;
}

/* setPeriod_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_IServiceMgrSupport_setPeriod_FxnT)(xdc_Int id, xdc_UInt32 periodInMs);
static inline ti_uia_runtime_IServiceMgrSupport_setPeriod_FxnT ti_uia_runtime_IServiceMgrSupport_setPeriod_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod);
static inline ti_uia_runtime_IServiceMgrSupport_setPeriod_FxnT ti_uia_runtime_IServiceMgrSupport_setPeriod_fxnP(ti_uia_runtime_IServiceMgrSupport_Module mod)
{
    return (ti_uia_runtime_IServiceMgrSupport_setPeriod_FxnT)mod->setPeriod;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_IServiceMgrSupport__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_IServiceMgrSupport__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_IServiceMgrSupport__nolocalnames)

#ifndef ti_uia_runtime_IServiceMgrSupport__localnames__done
#define ti_uia_runtime_IServiceMgrSupport__localnames__done

/* module prefix */
#define IServiceMgrSupport_Module ti_uia_runtime_IServiceMgrSupport_Module
#define IServiceMgrSupport_freePacket ti_uia_runtime_IServiceMgrSupport_freePacket
#define IServiceMgrSupport_freePacket_fxnP ti_uia_runtime_IServiceMgrSupport_freePacket_fxnP
#define IServiceMgrSupport_freePacket_FxnT ti_uia_runtime_IServiceMgrSupport_freePacket_FxnT
#define IServiceMgrSupport_getFreePacket ti_uia_runtime_IServiceMgrSupport_getFreePacket
#define IServiceMgrSupport_getFreePacket_fxnP ti_uia_runtime_IServiceMgrSupport_getFreePacket_fxnP
#define IServiceMgrSupport_getFreePacket_FxnT ti_uia_runtime_IServiceMgrSupport_getFreePacket_FxnT
#define IServiceMgrSupport_requestEnergy ti_uia_runtime_IServiceMgrSupport_requestEnergy
#define IServiceMgrSupport_requestEnergy_fxnP ti_uia_runtime_IServiceMgrSupport_requestEnergy_fxnP
#define IServiceMgrSupport_requestEnergy_FxnT ti_uia_runtime_IServiceMgrSupport_requestEnergy_FxnT
#define IServiceMgrSupport_sendPacket ti_uia_runtime_IServiceMgrSupport_sendPacket
#define IServiceMgrSupport_sendPacket_fxnP ti_uia_runtime_IServiceMgrSupport_sendPacket_fxnP
#define IServiceMgrSupport_sendPacket_FxnT ti_uia_runtime_IServiceMgrSupport_sendPacket_FxnT
#define IServiceMgrSupport_setPeriod ti_uia_runtime_IServiceMgrSupport_setPeriod
#define IServiceMgrSupport_setPeriod_fxnP ti_uia_runtime_IServiceMgrSupport_setPeriod_fxnP
#define IServiceMgrSupport_setPeriod_FxnT ti_uia_runtime_IServiceMgrSupport_setPeriod_FxnT
#define IServiceMgrSupport_Module_name ti_uia_runtime_IServiceMgrSupport_Module_name

#endif /* ti_uia_runtime_IServiceMgrSupport__localnames__done */
#endif
