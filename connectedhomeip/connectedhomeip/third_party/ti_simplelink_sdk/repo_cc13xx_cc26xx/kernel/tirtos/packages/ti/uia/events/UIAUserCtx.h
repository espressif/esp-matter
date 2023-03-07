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
 *     INTERNAL DEFINITIONS
 *     MODULE-WIDE CONFIGS
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

#ifndef ti_uia_events_UIAUserCtx__include
#define ti_uia_events_UIAUserCtx__include

#ifndef __nested__
#define __nested__
#define ti_uia_events_UIAUserCtx__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_events_UIAUserCtx___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/events/UIAUserCtx__prologue.h>
#include <ti/uia/events/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/Diags.h>
#include <ti/uia/events/IUIACtx.h>
#include <xdc/runtime/Log.h>
#include <ti/uia/events/DvtTypes.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* IsLoggingEnabledFxn */
typedef ti_uia_events_IUIACtx_IsLoggingEnabledFxn ti_uia_events_UIAUserCtx_IsLoggingEnabledFxn;


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_events_UIAUserCtx_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__diagsEnabled ti_uia_events_UIAUserCtx_Module__diagsEnabled__C;
#ifdef ti_uia_events_UIAUserCtx_Module__diagsEnabled__CR
#define ti_uia_events_UIAUserCtx_Module__diagsEnabled__C (*((CT__ti_uia_events_UIAUserCtx_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__diagsEnabled (ti_uia_events_UIAUserCtx_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_events_UIAUserCtx_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__diagsIncluded ti_uia_events_UIAUserCtx_Module__diagsIncluded__C;
#ifdef ti_uia_events_UIAUserCtx_Module__diagsIncluded__CR
#define ti_uia_events_UIAUserCtx_Module__diagsIncluded__C (*((CT__ti_uia_events_UIAUserCtx_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__diagsIncluded (ti_uia_events_UIAUserCtx_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_events_UIAUserCtx_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__diagsMask ti_uia_events_UIAUserCtx_Module__diagsMask__C;
#ifdef ti_uia_events_UIAUserCtx_Module__diagsMask__CR
#define ti_uia_events_UIAUserCtx_Module__diagsMask__C (*((CT__ti_uia_events_UIAUserCtx_Module__diagsMask*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__diagsMask__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__diagsMask (ti_uia_events_UIAUserCtx_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_events_UIAUserCtx_Module__gateObj;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__gateObj ti_uia_events_UIAUserCtx_Module__gateObj__C;
#ifdef ti_uia_events_UIAUserCtx_Module__gateObj__CR
#define ti_uia_events_UIAUserCtx_Module__gateObj__C (*((CT__ti_uia_events_UIAUserCtx_Module__gateObj*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__gateObj__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__gateObj (ti_uia_events_UIAUserCtx_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_events_UIAUserCtx_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__gatePrms ti_uia_events_UIAUserCtx_Module__gatePrms__C;
#ifdef ti_uia_events_UIAUserCtx_Module__gatePrms__CR
#define ti_uia_events_UIAUserCtx_Module__gatePrms__C (*((CT__ti_uia_events_UIAUserCtx_Module__gatePrms*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__gatePrms__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__gatePrms (ti_uia_events_UIAUserCtx_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_events_UIAUserCtx_Module__id;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__id ti_uia_events_UIAUserCtx_Module__id__C;
#ifdef ti_uia_events_UIAUserCtx_Module__id__CR
#define ti_uia_events_UIAUserCtx_Module__id__C (*((CT__ti_uia_events_UIAUserCtx_Module__id*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__id__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__id (ti_uia_events_UIAUserCtx_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_events_UIAUserCtx_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerDefined ti_uia_events_UIAUserCtx_Module__loggerDefined__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerDefined__CR
#define ti_uia_events_UIAUserCtx_Module__loggerDefined__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerDefined__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerDefined (ti_uia_events_UIAUserCtx_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_events_UIAUserCtx_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerObj ti_uia_events_UIAUserCtx_Module__loggerObj__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerObj__CR
#define ti_uia_events_UIAUserCtx_Module__loggerObj__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerObj*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerObj__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerObj (ti_uia_events_UIAUserCtx_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_events_UIAUserCtx_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerFxn0 ti_uia_events_UIAUserCtx_Module__loggerFxn0__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerFxn0__CR
#define ti_uia_events_UIAUserCtx_Module__loggerFxn0__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerFxn0 (ti_uia_events_UIAUserCtx_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_events_UIAUserCtx_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerFxn1 ti_uia_events_UIAUserCtx_Module__loggerFxn1__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerFxn1__CR
#define ti_uia_events_UIAUserCtx_Module__loggerFxn1__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerFxn1 (ti_uia_events_UIAUserCtx_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_events_UIAUserCtx_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerFxn2 ti_uia_events_UIAUserCtx_Module__loggerFxn2__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerFxn2__CR
#define ti_uia_events_UIAUserCtx_Module__loggerFxn2__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerFxn2 (ti_uia_events_UIAUserCtx_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_events_UIAUserCtx_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerFxn4 ti_uia_events_UIAUserCtx_Module__loggerFxn4__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerFxn4__CR
#define ti_uia_events_UIAUserCtx_Module__loggerFxn4__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerFxn4 (ti_uia_events_UIAUserCtx_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_events_UIAUserCtx_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Module__loggerFxn8 ti_uia_events_UIAUserCtx_Module__loggerFxn8__C;
#ifdef ti_uia_events_UIAUserCtx_Module__loggerFxn8__CR
#define ti_uia_events_UIAUserCtx_Module__loggerFxn8__C (*((CT__ti_uia_events_UIAUserCtx_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Module__loggerFxn8 (ti_uia_events_UIAUserCtx_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_events_UIAUserCtx_Object__count;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Object__count ti_uia_events_UIAUserCtx_Object__count__C;
#ifdef ti_uia_events_UIAUserCtx_Object__count__CR
#define ti_uia_events_UIAUserCtx_Object__count__C (*((CT__ti_uia_events_UIAUserCtx_Object__count*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Object__count__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Object__count (ti_uia_events_UIAUserCtx_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_events_UIAUserCtx_Object__heap;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Object__heap ti_uia_events_UIAUserCtx_Object__heap__C;
#ifdef ti_uia_events_UIAUserCtx_Object__heap__CR
#define ti_uia_events_UIAUserCtx_Object__heap__C (*((CT__ti_uia_events_UIAUserCtx_Object__heap*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Object__heap__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Object__heap (ti_uia_events_UIAUserCtx_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_events_UIAUserCtx_Object__sizeof;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Object__sizeof ti_uia_events_UIAUserCtx_Object__sizeof__C;
#ifdef ti_uia_events_UIAUserCtx_Object__sizeof__CR
#define ti_uia_events_UIAUserCtx_Object__sizeof__C (*((CT__ti_uia_events_UIAUserCtx_Object__sizeof*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Object__sizeof__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Object__sizeof (ti_uia_events_UIAUserCtx_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_events_UIAUserCtx_Object__table;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_Object__table ti_uia_events_UIAUserCtx_Object__table__C;
#ifdef ti_uia_events_UIAUserCtx_Object__table__CR
#define ti_uia_events_UIAUserCtx_Object__table__C (*((CT__ti_uia_events_UIAUserCtx_Object__table*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Object__table__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_Object__table (ti_uia_events_UIAUserCtx_Object__table__C)
#endif

/* isLoggingEnabledFxn */
typedef ti_uia_events_IUIACtx_IsLoggingEnabledFxn CT__ti_uia_events_UIAUserCtx_isLoggingEnabledFxn;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_isLoggingEnabledFxn ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__C;
#ifdef ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__CR
#define ti_uia_events_UIAUserCtx_isLoggingEnabledFxn (*((CT__ti_uia_events_UIAUserCtx_isLoggingEnabledFxn*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_isLoggingEnabledFxn (ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__C)
#endif

/* ENABLEMASK */
typedef xdc_Bits16 CT__ti_uia_events_UIAUserCtx_ENABLEMASK;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_ENABLEMASK ti_uia_events_UIAUserCtx_ENABLEMASK__C;
#ifdef ti_uia_events_UIAUserCtx_ENABLEMASK__CR
#define ti_uia_events_UIAUserCtx_ENABLEMASK (*((CT__ti_uia_events_UIAUserCtx_ENABLEMASK*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_ENABLEMASK__C_offset)))
#else
#ifdef ti_uia_events_UIAUserCtx_ENABLEMASK__D
#define ti_uia_events_UIAUserCtx_ENABLEMASK (ti_uia_events_UIAUserCtx_ENABLEMASK__D)
#else
#define ti_uia_events_UIAUserCtx_ENABLEMASK (ti_uia_events_UIAUserCtx_ENABLEMASK__C)
#endif
#endif

/* SYNCID */
typedef xdc_Bits16 CT__ti_uia_events_UIAUserCtx_SYNCID;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_SYNCID ti_uia_events_UIAUserCtx_SYNCID__C;
#ifdef ti_uia_events_UIAUserCtx_SYNCID__CR
#define ti_uia_events_UIAUserCtx_SYNCID (*((CT__ti_uia_events_UIAUserCtx_SYNCID*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_SYNCID__C_offset)))
#else
#ifdef ti_uia_events_UIAUserCtx_SYNCID__D
#define ti_uia_events_UIAUserCtx_SYNCID (ti_uia_events_UIAUserCtx_SYNCID__D)
#else
#define ti_uia_events_UIAUserCtx_SYNCID (ti_uia_events_UIAUserCtx_SYNCID__C)
#endif
#endif

/* ctxChg */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAUserCtx_ctxChg;
__extern __FAR__ const CT__ti_uia_events_UIAUserCtx_ctxChg ti_uia_events_UIAUserCtx_ctxChg__C;
#ifdef ti_uia_events_UIAUserCtx_ctxChg__CR
#define ti_uia_events_UIAUserCtx_ctxChg (*((CT__ti_uia_events_UIAUserCtx_ctxChg*)(xdcRomConstPtr + ti_uia_events_UIAUserCtx_ctxChg__C_offset)))
#else
#define ti_uia_events_UIAUserCtx_ctxChg (ti_uia_events_UIAUserCtx_ctxChg__C)
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_events_UIAUserCtx_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_events_UIAUserCtx_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_events_UIAUserCtx_Fxns__ ti_uia_events_UIAUserCtx_Module__FXNS__C;
#else
#define ti_uia_events_UIAUserCtx_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_events_UIAUserCtx_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_events_UIAUserCtx_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_events_UIAUserCtx_Module__startupDone__S, "ti_uia_events_UIAUserCtx_Module__startupDone__S")
__extern xdc_Bool ti_uia_events_UIAUserCtx_Module__startupDone__S( void);

/* getCtxId__E */
#define ti_uia_events_UIAUserCtx_getCtxId ti_uia_events_UIAUserCtx_getCtxId__E
xdc__CODESECT(ti_uia_events_UIAUserCtx_getCtxId__E, "ti_uia_events_UIAUserCtx_getCtxId")
__extern xdc_UInt ti_uia_events_UIAUserCtx_getCtxId__E( void);

/* getEnableOnValue__E */
#define ti_uia_events_UIAUserCtx_getEnableOnValue ti_uia_events_UIAUserCtx_getEnableOnValue__E
xdc__CODESECT(ti_uia_events_UIAUserCtx_getEnableOnValue__E, "ti_uia_events_UIAUserCtx_getEnableOnValue")
__extern xdc_UInt ti_uia_events_UIAUserCtx_getEnableOnValue__E( void);

/* setEnableOnValue__E */
#define ti_uia_events_UIAUserCtx_setEnableOnValue ti_uia_events_UIAUserCtx_setEnableOnValue__E
xdc__CODESECT(ti_uia_events_UIAUserCtx_setEnableOnValue__E, "ti_uia_events_UIAUserCtx_setEnableOnValue")
__extern xdc_Void ti_uia_events_UIAUserCtx_setEnableOnValue__E( xdc_UInt value);

/* isLoggingEnabled__E */
#define ti_uia_events_UIAUserCtx_isLoggingEnabled ti_uia_events_UIAUserCtx_isLoggingEnabled__E
xdc__CODESECT(ti_uia_events_UIAUserCtx_isLoggingEnabled__E, "ti_uia_events_UIAUserCtx_isLoggingEnabled")
__extern xdc_Bool ti_uia_events_UIAUserCtx_isLoggingEnabled__E( xdc_UInt newUserCtx);

/* setOldValue__E */
#define ti_uia_events_UIAUserCtx_setOldValue ti_uia_events_UIAUserCtx_setOldValue__E
xdc__CODESECT(ti_uia_events_UIAUserCtx_setOldValue__E, "ti_uia_events_UIAUserCtx_setOldValue")
__extern xdc_UInt ti_uia_events_UIAUserCtx_setOldValue__E( xdc_UInt newValue);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_events_IUIACtx_Module ti_uia_events_UIAUserCtx_Module_upCast(void);
static inline ti_uia_events_IUIACtx_Module ti_uia_events_UIAUserCtx_Module_upCast(void)
{
    return (ti_uia_events_IUIACtx_Module)&ti_uia_events_UIAUserCtx_Module__FXNS__C;
}

/* Module_to_ti_uia_events_IUIACtx */
#define ti_uia_events_UIAUserCtx_Module_to_ti_uia_events_IUIACtx ti_uia_events_UIAUserCtx_Module_upCast

/* Module_upCast2 */
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAUserCtx_Module_upCast2(void);
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAUserCtx_Module_upCast2(void)
{
    return (ti_uia_events_IUIAEvent_Module)&ti_uia_events_UIAUserCtx_Module__FXNS__C;
}

/* Module_to_ti_uia_events_IUIAEvent */
#define ti_uia_events_UIAUserCtx_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAUserCtx_Module_upCast2


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_events_UIAUserCtx_Module_startupDone() ti_uia_events_UIAUserCtx_Module__startupDone__S()

/* Object_heap */
#define ti_uia_events_UIAUserCtx_Object_heap() ti_uia_events_UIAUserCtx_Object__heap__C

/* Module_heap */
#define ti_uia_events_UIAUserCtx_Module_heap() ti_uia_events_UIAUserCtx_Object__heap__C

/* Module_id */
static inline CT__ti_uia_events_UIAUserCtx_Module__id ti_uia_events_UIAUserCtx_Module_id(void);
static inline CT__ti_uia_events_UIAUserCtx_Module__id ti_uia_events_UIAUserCtx_Module_id( void ) 
{
    return ti_uia_events_UIAUserCtx_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_events_UIAUserCtx_Module_hasMask(void);
static inline xdc_Bool ti_uia_events_UIAUserCtx_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_events_UIAUserCtx_Module__diagsMask__C != (CT__ti_uia_events_UIAUserCtx_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_events_UIAUserCtx_Module_getMask(void);
static inline xdc_Bits16 ti_uia_events_UIAUserCtx_Module_getMask(void)
{
    return (ti_uia_events_UIAUserCtx_Module__diagsMask__C != (CT__ti_uia_events_UIAUserCtx_Module__diagsMask)NULL) ? *ti_uia_events_UIAUserCtx_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_events_UIAUserCtx_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_events_UIAUserCtx_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_events_UIAUserCtx_Module__diagsMask__C != (CT__ti_uia_events_UIAUserCtx_Module__diagsMask)NULL) {
        *ti_uia_events_UIAUserCtx_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/events/UIAUserCtx__epilogue.h>

#ifdef ti_uia_events_UIAUserCtx__top__
#undef __nested__
#endif

#endif /* ti_uia_events_UIAUserCtx__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_events_UIAUserCtx__internalaccess))

#ifndef ti_uia_events_UIAUserCtx__include_state
#define ti_uia_events_UIAUserCtx__include_state

/* Module_State */
struct ti_uia_events_UIAUserCtx_Module_State {
    xdc_UInt mLastValue;
    xdc_UInt mEnableOnValue;
};

/* Module__state__V */
#ifndef ti_uia_events_UIAUserCtx_Module__state__VR
extern struct ti_uia_events_UIAUserCtx_Module_State__ ti_uia_events_UIAUserCtx_Module__state__V;
#else
#define ti_uia_events_UIAUserCtx_Module__state__V (*((struct ti_uia_events_UIAUserCtx_Module_State__*)(xdcRomStatePtr + ti_uia_events_UIAUserCtx_Module__state__V_offset)))
#endif

#endif /* ti_uia_events_UIAUserCtx__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_events_UIAUserCtx__nolocalnames)

#ifndef ti_uia_events_UIAUserCtx__localnames__done
#define ti_uia_events_UIAUserCtx__localnames__done

/* module prefix */
#define UIAUserCtx_IsLoggingEnabledFxn ti_uia_events_UIAUserCtx_IsLoggingEnabledFxn
#define UIAUserCtx_Module_State ti_uia_events_UIAUserCtx_Module_State
#define UIAUserCtx_isLoggingEnabledFxn ti_uia_events_UIAUserCtx_isLoggingEnabledFxn
#define UIAUserCtx_ENABLEMASK ti_uia_events_UIAUserCtx_ENABLEMASK
#define UIAUserCtx_SYNCID ti_uia_events_UIAUserCtx_SYNCID
#define UIAUserCtx_ctxChg ti_uia_events_UIAUserCtx_ctxChg
#define UIAUserCtx_getCtxId ti_uia_events_UIAUserCtx_getCtxId
#define UIAUserCtx_getEnableOnValue ti_uia_events_UIAUserCtx_getEnableOnValue
#define UIAUserCtx_setEnableOnValue ti_uia_events_UIAUserCtx_setEnableOnValue
#define UIAUserCtx_isLoggingEnabled ti_uia_events_UIAUserCtx_isLoggingEnabled
#define UIAUserCtx_setOldValue ti_uia_events_UIAUserCtx_setOldValue
#define UIAUserCtx_Module_name ti_uia_events_UIAUserCtx_Module_name
#define UIAUserCtx_Module_id ti_uia_events_UIAUserCtx_Module_id
#define UIAUserCtx_Module_startup ti_uia_events_UIAUserCtx_Module_startup
#define UIAUserCtx_Module_startupDone ti_uia_events_UIAUserCtx_Module_startupDone
#define UIAUserCtx_Module_hasMask ti_uia_events_UIAUserCtx_Module_hasMask
#define UIAUserCtx_Module_getMask ti_uia_events_UIAUserCtx_Module_getMask
#define UIAUserCtx_Module_setMask ti_uia_events_UIAUserCtx_Module_setMask
#define UIAUserCtx_Object_heap ti_uia_events_UIAUserCtx_Object_heap
#define UIAUserCtx_Module_heap ti_uia_events_UIAUserCtx_Module_heap
#define UIAUserCtx_Module_upCast ti_uia_events_UIAUserCtx_Module_upCast
#define UIAUserCtx_Module_to_ti_uia_events_IUIACtx ti_uia_events_UIAUserCtx_Module_to_ti_uia_events_IUIACtx
#define UIAUserCtx_Module_upCast2 ti_uia_events_UIAUserCtx_Module_upCast2
#define UIAUserCtx_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAUserCtx_Module_to_ti_uia_events_IUIAEvent

#endif /* ti_uia_events_UIAUserCtx__localnames__done */
#endif
