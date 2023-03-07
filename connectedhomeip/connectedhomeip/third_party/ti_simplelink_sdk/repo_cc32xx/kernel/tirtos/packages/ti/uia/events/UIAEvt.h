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

#ifndef ti_uia_events_UIAEvt__include
#define ti_uia_events_UIAEvt__include

#ifndef __nested__
#define __nested__
#define ti_uia_events_UIAEvt__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_events_UIAEvt___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/events/package/package.defs.h>

#include <xdc/runtime/Diags.h>
#include <ti/uia/events/IUIAEvent.h>
#include <xdc/runtime/Log.h>
#include <ti/uia/events/DvtTypes.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_events_UIAEvt_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__diagsEnabled ti_uia_events_UIAEvt_Module__diagsEnabled__C;
#ifdef ti_uia_events_UIAEvt_Module__diagsEnabled__CR
#define ti_uia_events_UIAEvt_Module__diagsEnabled__C (*((CT__ti_uia_events_UIAEvt_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__diagsEnabled (ti_uia_events_UIAEvt_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_events_UIAEvt_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__diagsIncluded ti_uia_events_UIAEvt_Module__diagsIncluded__C;
#ifdef ti_uia_events_UIAEvt_Module__diagsIncluded__CR
#define ti_uia_events_UIAEvt_Module__diagsIncluded__C (*((CT__ti_uia_events_UIAEvt_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__diagsIncluded (ti_uia_events_UIAEvt_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_events_UIAEvt_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__diagsMask ti_uia_events_UIAEvt_Module__diagsMask__C;
#ifdef ti_uia_events_UIAEvt_Module__diagsMask__CR
#define ti_uia_events_UIAEvt_Module__diagsMask__C (*((CT__ti_uia_events_UIAEvt_Module__diagsMask*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__diagsMask__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__diagsMask (ti_uia_events_UIAEvt_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_events_UIAEvt_Module__gateObj;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__gateObj ti_uia_events_UIAEvt_Module__gateObj__C;
#ifdef ti_uia_events_UIAEvt_Module__gateObj__CR
#define ti_uia_events_UIAEvt_Module__gateObj__C (*((CT__ti_uia_events_UIAEvt_Module__gateObj*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__gateObj__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__gateObj (ti_uia_events_UIAEvt_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_events_UIAEvt_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__gatePrms ti_uia_events_UIAEvt_Module__gatePrms__C;
#ifdef ti_uia_events_UIAEvt_Module__gatePrms__CR
#define ti_uia_events_UIAEvt_Module__gatePrms__C (*((CT__ti_uia_events_UIAEvt_Module__gatePrms*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__gatePrms__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__gatePrms (ti_uia_events_UIAEvt_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_events_UIAEvt_Module__id;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__id ti_uia_events_UIAEvt_Module__id__C;
#ifdef ti_uia_events_UIAEvt_Module__id__CR
#define ti_uia_events_UIAEvt_Module__id__C (*((CT__ti_uia_events_UIAEvt_Module__id*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__id__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__id (ti_uia_events_UIAEvt_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_events_UIAEvt_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerDefined ti_uia_events_UIAEvt_Module__loggerDefined__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerDefined__CR
#define ti_uia_events_UIAEvt_Module__loggerDefined__C (*((CT__ti_uia_events_UIAEvt_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerDefined__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerDefined (ti_uia_events_UIAEvt_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_events_UIAEvt_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerObj ti_uia_events_UIAEvt_Module__loggerObj__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerObj__CR
#define ti_uia_events_UIAEvt_Module__loggerObj__C (*((CT__ti_uia_events_UIAEvt_Module__loggerObj*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerObj__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerObj (ti_uia_events_UIAEvt_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_events_UIAEvt_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerFxn0 ti_uia_events_UIAEvt_Module__loggerFxn0__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerFxn0__CR
#define ti_uia_events_UIAEvt_Module__loggerFxn0__C (*((CT__ti_uia_events_UIAEvt_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerFxn0 (ti_uia_events_UIAEvt_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_events_UIAEvt_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerFxn1 ti_uia_events_UIAEvt_Module__loggerFxn1__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerFxn1__CR
#define ti_uia_events_UIAEvt_Module__loggerFxn1__C (*((CT__ti_uia_events_UIAEvt_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerFxn1 (ti_uia_events_UIAEvt_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_events_UIAEvt_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerFxn2 ti_uia_events_UIAEvt_Module__loggerFxn2__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerFxn2__CR
#define ti_uia_events_UIAEvt_Module__loggerFxn2__C (*((CT__ti_uia_events_UIAEvt_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerFxn2 (ti_uia_events_UIAEvt_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_events_UIAEvt_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerFxn4 ti_uia_events_UIAEvt_Module__loggerFxn4__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerFxn4__CR
#define ti_uia_events_UIAEvt_Module__loggerFxn4__C (*((CT__ti_uia_events_UIAEvt_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerFxn4 (ti_uia_events_UIAEvt_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_events_UIAEvt_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Module__loggerFxn8 ti_uia_events_UIAEvt_Module__loggerFxn8__C;
#ifdef ti_uia_events_UIAEvt_Module__loggerFxn8__CR
#define ti_uia_events_UIAEvt_Module__loggerFxn8__C (*((CT__ti_uia_events_UIAEvt_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_events_UIAEvt_Module__loggerFxn8 (ti_uia_events_UIAEvt_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_events_UIAEvt_Object__count;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Object__count ti_uia_events_UIAEvt_Object__count__C;
#ifdef ti_uia_events_UIAEvt_Object__count__CR
#define ti_uia_events_UIAEvt_Object__count__C (*((CT__ti_uia_events_UIAEvt_Object__count*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Object__count__C_offset)))
#else
#define ti_uia_events_UIAEvt_Object__count (ti_uia_events_UIAEvt_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_events_UIAEvt_Object__heap;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Object__heap ti_uia_events_UIAEvt_Object__heap__C;
#ifdef ti_uia_events_UIAEvt_Object__heap__CR
#define ti_uia_events_UIAEvt_Object__heap__C (*((CT__ti_uia_events_UIAEvt_Object__heap*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Object__heap__C_offset)))
#else
#define ti_uia_events_UIAEvt_Object__heap (ti_uia_events_UIAEvt_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_events_UIAEvt_Object__sizeof;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Object__sizeof ti_uia_events_UIAEvt_Object__sizeof__C;
#ifdef ti_uia_events_UIAEvt_Object__sizeof__CR
#define ti_uia_events_UIAEvt_Object__sizeof__C (*((CT__ti_uia_events_UIAEvt_Object__sizeof*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Object__sizeof__C_offset)))
#else
#define ti_uia_events_UIAEvt_Object__sizeof (ti_uia_events_UIAEvt_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_events_UIAEvt_Object__table;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_Object__table ti_uia_events_UIAEvt_Object__table__C;
#ifdef ti_uia_events_UIAEvt_Object__table__CR
#define ti_uia_events_UIAEvt_Object__table__C (*((CT__ti_uia_events_UIAEvt_Object__table*)(xdcRomConstPtr + ti_uia_events_UIAEvt_Object__table__C_offset)))
#else
#define ti_uia_events_UIAEvt_Object__table (ti_uia_events_UIAEvt_Object__table__C)
#endif

/* warning */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_warning;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_warning ti_uia_events_UIAEvt_warning__C;
#ifdef ti_uia_events_UIAEvt_warning__CR
#define ti_uia_events_UIAEvt_warning (*((CT__ti_uia_events_UIAEvt_warning*)(xdcRomConstPtr + ti_uia_events_UIAEvt_warning__C_offset)))
#else
#define ti_uia_events_UIAEvt_warning (ti_uia_events_UIAEvt_warning__C)
#endif

/* warningWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_warningWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_warningWithStr ti_uia_events_UIAEvt_warningWithStr__C;
#ifdef ti_uia_events_UIAEvt_warningWithStr__CR
#define ti_uia_events_UIAEvt_warningWithStr (*((CT__ti_uia_events_UIAEvt_warningWithStr*)(xdcRomConstPtr + ti_uia_events_UIAEvt_warningWithStr__C_offset)))
#else
#define ti_uia_events_UIAEvt_warningWithStr (ti_uia_events_UIAEvt_warningWithStr__C)
#endif

/* info */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_info;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_info ti_uia_events_UIAEvt_info__C;
#ifdef ti_uia_events_UIAEvt_info__CR
#define ti_uia_events_UIAEvt_info (*((CT__ti_uia_events_UIAEvt_info*)(xdcRomConstPtr + ti_uia_events_UIAEvt_info__C_offset)))
#else
#define ti_uia_events_UIAEvt_info (ti_uia_events_UIAEvt_info__C)
#endif

/* infoWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_infoWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_infoWithStr ti_uia_events_UIAEvt_infoWithStr__C;
#ifdef ti_uia_events_UIAEvt_infoWithStr__CR
#define ti_uia_events_UIAEvt_infoWithStr (*((CT__ti_uia_events_UIAEvt_infoWithStr*)(xdcRomConstPtr + ti_uia_events_UIAEvt_infoWithStr__C_offset)))
#else
#define ti_uia_events_UIAEvt_infoWithStr (ti_uia_events_UIAEvt_infoWithStr__C)
#endif

/* detail */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_detail;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_detail ti_uia_events_UIAEvt_detail__C;
#ifdef ti_uia_events_UIAEvt_detail__CR
#define ti_uia_events_UIAEvt_detail (*((CT__ti_uia_events_UIAEvt_detail*)(xdcRomConstPtr + ti_uia_events_UIAEvt_detail__C_offset)))
#else
#define ti_uia_events_UIAEvt_detail (ti_uia_events_UIAEvt_detail__C)
#endif

/* detailWithStr */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_detailWithStr;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_detailWithStr ti_uia_events_UIAEvt_detailWithStr__C;
#ifdef ti_uia_events_UIAEvt_detailWithStr__CR
#define ti_uia_events_UIAEvt_detailWithStr (*((CT__ti_uia_events_UIAEvt_detailWithStr*)(xdcRomConstPtr + ti_uia_events_UIAEvt_detailWithStr__C_offset)))
#else
#define ti_uia_events_UIAEvt_detailWithStr (ti_uia_events_UIAEvt_detailWithStr__C)
#endif

/* intWithKey */
typedef xdc_runtime_Log_Event CT__ti_uia_events_UIAEvt_intWithKey;
__extern __FAR__ const CT__ti_uia_events_UIAEvt_intWithKey ti_uia_events_UIAEvt_intWithKey__C;
#ifdef ti_uia_events_UIAEvt_intWithKey__CR
#define ti_uia_events_UIAEvt_intWithKey (*((CT__ti_uia_events_UIAEvt_intWithKey*)(xdcRomConstPtr + ti_uia_events_UIAEvt_intWithKey__C_offset)))
#else
#define ti_uia_events_UIAEvt_intWithKey (ti_uia_events_UIAEvt_intWithKey__C)
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_events_UIAEvt_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_events_UIAEvt_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_events_UIAEvt_Fxns__ ti_uia_events_UIAEvt_Module__FXNS__C;
#else
#define ti_uia_events_UIAEvt_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_events_UIAEvt_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_events_UIAEvt_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_events_UIAEvt_Module__startupDone__S, "ti_uia_events_UIAEvt_Module__startupDone__S")
__extern xdc_Bool ti_uia_events_UIAEvt_Module__startupDone__S( void);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAEvt_Module_upCast(void);
static inline ti_uia_events_IUIAEvent_Module ti_uia_events_UIAEvt_Module_upCast(void)
{
    return (ti_uia_events_IUIAEvent_Module)&ti_uia_events_UIAEvt_Module__FXNS__C;
}

/* Module_to_ti_uia_events_IUIAEvent */
#define ti_uia_events_UIAEvt_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAEvt_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_events_UIAEvt_Module_startupDone() ti_uia_events_UIAEvt_Module__startupDone__S()

/* Object_heap */
#define ti_uia_events_UIAEvt_Object_heap() ti_uia_events_UIAEvt_Object__heap__C

/* Module_heap */
#define ti_uia_events_UIAEvt_Module_heap() ti_uia_events_UIAEvt_Object__heap__C

/* Module_id */
static inline CT__ti_uia_events_UIAEvt_Module__id ti_uia_events_UIAEvt_Module_id(void);
static inline CT__ti_uia_events_UIAEvt_Module__id ti_uia_events_UIAEvt_Module_id( void ) 
{
    return ti_uia_events_UIAEvt_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_events_UIAEvt_Module_hasMask(void);
static inline xdc_Bool ti_uia_events_UIAEvt_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_events_UIAEvt_Module__diagsMask__C != (CT__ti_uia_events_UIAEvt_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_events_UIAEvt_Module_getMask(void);
static inline xdc_Bits16 ti_uia_events_UIAEvt_Module_getMask(void)
{
    return (ti_uia_events_UIAEvt_Module__diagsMask__C != (CT__ti_uia_events_UIAEvt_Module__diagsMask)NULL) ? *ti_uia_events_UIAEvt_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_events_UIAEvt_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_events_UIAEvt_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_events_UIAEvt_Module__diagsMask__C != (CT__ti_uia_events_UIAEvt_Module__diagsMask)NULL) {
        *ti_uia_events_UIAEvt_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_events_UIAEvt__top__
#undef __nested__
#endif

#endif /* ti_uia_events_UIAEvt__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_events_UIAEvt__internalaccess))

#ifndef ti_uia_events_UIAEvt__include_state
#define ti_uia_events_UIAEvt__include_state


#endif /* ti_uia_events_UIAEvt__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_events_UIAEvt__nolocalnames)

#ifndef ti_uia_events_UIAEvt__localnames__done
#define ti_uia_events_UIAEvt__localnames__done

/* module prefix */
#define UIAEvt_warning ti_uia_events_UIAEvt_warning
#define UIAEvt_warningWithStr ti_uia_events_UIAEvt_warningWithStr
#define UIAEvt_info ti_uia_events_UIAEvt_info
#define UIAEvt_infoWithStr ti_uia_events_UIAEvt_infoWithStr
#define UIAEvt_detail ti_uia_events_UIAEvt_detail
#define UIAEvt_detailWithStr ti_uia_events_UIAEvt_detailWithStr
#define UIAEvt_intWithKey ti_uia_events_UIAEvt_intWithKey
#define UIAEvt_Module_name ti_uia_events_UIAEvt_Module_name
#define UIAEvt_Module_id ti_uia_events_UIAEvt_Module_id
#define UIAEvt_Module_startup ti_uia_events_UIAEvt_Module_startup
#define UIAEvt_Module_startupDone ti_uia_events_UIAEvt_Module_startupDone
#define UIAEvt_Module_hasMask ti_uia_events_UIAEvt_Module_hasMask
#define UIAEvt_Module_getMask ti_uia_events_UIAEvt_Module_getMask
#define UIAEvt_Module_setMask ti_uia_events_UIAEvt_Module_setMask
#define UIAEvt_Object_heap ti_uia_events_UIAEvt_Object_heap
#define UIAEvt_Module_heap ti_uia_events_UIAEvt_Module_heap
#define UIAEvt_Module_upCast ti_uia_events_UIAEvt_Module_upCast
#define UIAEvt_Module_to_ti_uia_events_IUIAEvent ti_uia_events_UIAEvt_Module_to_ti_uia_events_IUIAEvent

#endif /* ti_uia_events_UIAEvt__localnames__done */
#endif
