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
 *     FUNCTION DECLARATIONS
 *     SYSTEM FUNCTIONS
 *
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_uia_runtime_LogCtxChg__include
#define ti_uia_runtime_LogCtxChg__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_LogCtxChg__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_LogCtxChg___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/LogCtxChg__prologue.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/LoggerTypes.h>
#include <xdc/runtime/ILogger.h>
#include <ti/uia/runtime/CtxFilter.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/IModule.h>


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
typedef xdc_Bits32 CT__ti_uia_runtime_LogCtxChg_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__diagsEnabled ti_uia_runtime_LogCtxChg_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__diagsEnabled__CR
#define ti_uia_runtime_LogCtxChg_Module__diagsEnabled__C (*((CT__ti_uia_runtime_LogCtxChg_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__diagsEnabled (ti_uia_runtime_LogCtxChg_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_LogCtxChg_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__diagsIncluded ti_uia_runtime_LogCtxChg_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__diagsIncluded__CR
#define ti_uia_runtime_LogCtxChg_Module__diagsIncluded__C (*((CT__ti_uia_runtime_LogCtxChg_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__diagsIncluded (ti_uia_runtime_LogCtxChg_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_LogCtxChg_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__diagsMask ti_uia_runtime_LogCtxChg_Module__diagsMask__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__diagsMask__CR
#define ti_uia_runtime_LogCtxChg_Module__diagsMask__C (*((CT__ti_uia_runtime_LogCtxChg_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__diagsMask (ti_uia_runtime_LogCtxChg_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_LogCtxChg_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__gateObj ti_uia_runtime_LogCtxChg_Module__gateObj__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__gateObj__CR
#define ti_uia_runtime_LogCtxChg_Module__gateObj__C (*((CT__ti_uia_runtime_LogCtxChg_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__gateObj (ti_uia_runtime_LogCtxChg_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_LogCtxChg_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__gatePrms ti_uia_runtime_LogCtxChg_Module__gatePrms__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__gatePrms__CR
#define ti_uia_runtime_LogCtxChg_Module__gatePrms__C (*((CT__ti_uia_runtime_LogCtxChg_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__gatePrms (ti_uia_runtime_LogCtxChg_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_LogCtxChg_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__id ti_uia_runtime_LogCtxChg_Module__id__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__id__CR
#define ti_uia_runtime_LogCtxChg_Module__id__C (*((CT__ti_uia_runtime_LogCtxChg_Module__id*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__id__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__id (ti_uia_runtime_LogCtxChg_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_LogCtxChg_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerDefined ti_uia_runtime_LogCtxChg_Module__loggerDefined__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerDefined__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerDefined__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerDefined (ti_uia_runtime_LogCtxChg_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_LogCtxChg_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerObj ti_uia_runtime_LogCtxChg_Module__loggerObj__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerObj__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerObj__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerObj (ti_uia_runtime_LogCtxChg_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn0 ti_uia_runtime_LogCtxChg_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerFxn0__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn0__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn0 (ti_uia_runtime_LogCtxChg_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn1 ti_uia_runtime_LogCtxChg_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerFxn1__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn1__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn1 (ti_uia_runtime_LogCtxChg_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn2 ti_uia_runtime_LogCtxChg_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerFxn2__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn2__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn2 (ti_uia_runtime_LogCtxChg_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn4 ti_uia_runtime_LogCtxChg_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerFxn4__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn4__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn4 (ti_uia_runtime_LogCtxChg_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn8 ti_uia_runtime_LogCtxChg_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_LogCtxChg_Module__loggerFxn8__CR
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn8__C (*((CT__ti_uia_runtime_LogCtxChg_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Module__loggerFxn8 (ti_uia_runtime_LogCtxChg_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_LogCtxChg_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Object__count ti_uia_runtime_LogCtxChg_Object__count__C;
#ifdef ti_uia_runtime_LogCtxChg_Object__count__CR
#define ti_uia_runtime_LogCtxChg_Object__count__C (*((CT__ti_uia_runtime_LogCtxChg_Object__count*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Object__count__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Object__count (ti_uia_runtime_LogCtxChg_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_LogCtxChg_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Object__heap ti_uia_runtime_LogCtxChg_Object__heap__C;
#ifdef ti_uia_runtime_LogCtxChg_Object__heap__CR
#define ti_uia_runtime_LogCtxChg_Object__heap__C (*((CT__ti_uia_runtime_LogCtxChg_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Object__heap__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Object__heap (ti_uia_runtime_LogCtxChg_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_LogCtxChg_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Object__sizeof ti_uia_runtime_LogCtxChg_Object__sizeof__C;
#ifdef ti_uia_runtime_LogCtxChg_Object__sizeof__CR
#define ti_uia_runtime_LogCtxChg_Object__sizeof__C (*((CT__ti_uia_runtime_LogCtxChg_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Object__sizeof (ti_uia_runtime_LogCtxChg_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_LogCtxChg_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_Object__table ti_uia_runtime_LogCtxChg_Object__table__C;
#ifdef ti_uia_runtime_LogCtxChg_Object__table__CR
#define ti_uia_runtime_LogCtxChg_Object__table__C (*((CT__ti_uia_runtime_LogCtxChg_Object__table*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_Object__table__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_Object__table (ti_uia_runtime_LogCtxChg_Object__table__C)
#endif

/* isTimestampEnabled */
typedef xdc_Bool CT__ti_uia_runtime_LogCtxChg_isTimestampEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_isTimestampEnabled ti_uia_runtime_LogCtxChg_isTimestampEnabled__C;
#ifdef ti_uia_runtime_LogCtxChg_isTimestampEnabled__CR
#define ti_uia_runtime_LogCtxChg_isTimestampEnabled (*((CT__ti_uia_runtime_LogCtxChg_isTimestampEnabled*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_isTimestampEnabled__C_offset)))
#else
#ifdef ti_uia_runtime_LogCtxChg_isTimestampEnabled__D
#define ti_uia_runtime_LogCtxChg_isTimestampEnabled (ti_uia_runtime_LogCtxChg_isTimestampEnabled__D)
#else
#define ti_uia_runtime_LogCtxChg_isTimestampEnabled (ti_uia_runtime_LogCtxChg_isTimestampEnabled__C)
#endif
#endif

/* loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_LogCtxChg_loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_loggerDefined ti_uia_runtime_LogCtxChg_loggerDefined__C;
#ifdef ti_uia_runtime_LogCtxChg_loggerDefined__CR
#define ti_uia_runtime_LogCtxChg_loggerDefined (*((CT__ti_uia_runtime_LogCtxChg_loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_loggerDefined__C_offset)))
#else
#ifdef ti_uia_runtime_LogCtxChg_loggerDefined__D
#define ti_uia_runtime_LogCtxChg_loggerDefined (ti_uia_runtime_LogCtxChg_loggerDefined__D)
#else
#define ti_uia_runtime_LogCtxChg_loggerDefined (ti_uia_runtime_LogCtxChg_loggerDefined__C)
#endif
#endif

/* ctxFilterEnabled */
typedef xdc_Bool CT__ti_uia_runtime_LogCtxChg_ctxFilterEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_ctxFilterEnabled ti_uia_runtime_LogCtxChg_ctxFilterEnabled__C;
#ifdef ti_uia_runtime_LogCtxChg_ctxFilterEnabled__CR
#define ti_uia_runtime_LogCtxChg_ctxFilterEnabled (*((CT__ti_uia_runtime_LogCtxChg_ctxFilterEnabled*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_ctxFilterEnabled__C_offset)))
#else
#ifdef ti_uia_runtime_LogCtxChg_ctxFilterEnabled__D
#define ti_uia_runtime_LogCtxChg_ctxFilterEnabled (ti_uia_runtime_LogCtxChg_ctxFilterEnabled__D)
#else
#define ti_uia_runtime_LogCtxChg_ctxFilterEnabled (ti_uia_runtime_LogCtxChg_ctxFilterEnabled__C)
#endif
#endif

/* loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_LogCtxChg_loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_loggerObj ti_uia_runtime_LogCtxChg_loggerObj__C;
#ifdef ti_uia_runtime_LogCtxChg_loggerObj__CR
#define ti_uia_runtime_LogCtxChg_loggerObj (*((CT__ti_uia_runtime_LogCtxChg_loggerObj*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_loggerObj__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_loggerObj (ti_uia_runtime_LogCtxChg_loggerObj__C)
#endif

/* loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_LogCtxChg_loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_loggerFxn2 ti_uia_runtime_LogCtxChg_loggerFxn2__C;
#ifdef ti_uia_runtime_LogCtxChg_loggerFxn2__CR
#define ti_uia_runtime_LogCtxChg_loggerFxn2 (*((CT__ti_uia_runtime_LogCtxChg_loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_loggerFxn2 (ti_uia_runtime_LogCtxChg_loggerFxn2__C)
#endif

/* loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_LogCtxChg_loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_LogCtxChg_loggerFxn8 ti_uia_runtime_LogCtxChg_loggerFxn8__C;
#ifdef ti_uia_runtime_LogCtxChg_loggerFxn8__CR
#define ti_uia_runtime_LogCtxChg_loggerFxn8 (*((CT__ti_uia_runtime_LogCtxChg_loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_LogCtxChg_loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_LogCtxChg_loggerFxn8 (ti_uia_runtime_LogCtxChg_loggerFxn8__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_LogCtxChg_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_LogCtxChg_Module__startupDone__S, "ti_uia_runtime_LogCtxChg_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_LogCtxChg_Module__startupDone__S( void);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_LogCtxChg_Module_startupDone() ti_uia_runtime_LogCtxChg_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_LogCtxChg_Object_heap() ti_uia_runtime_LogCtxChg_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_LogCtxChg_Module_heap() ti_uia_runtime_LogCtxChg_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_LogCtxChg_Module__id ti_uia_runtime_LogCtxChg_Module_id(void);
static inline CT__ti_uia_runtime_LogCtxChg_Module__id ti_uia_runtime_LogCtxChg_Module_id( void ) 
{
    return ti_uia_runtime_LogCtxChg_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_LogCtxChg_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_LogCtxChg_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_LogCtxChg_Module__diagsMask__C != (CT__ti_uia_runtime_LogCtxChg_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_LogCtxChg_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_LogCtxChg_Module_getMask(void)
{
    return (ti_uia_runtime_LogCtxChg_Module__diagsMask__C != (CT__ti_uia_runtime_LogCtxChg_Module__diagsMask)NULL) ? *ti_uia_runtime_LogCtxChg_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_LogCtxChg_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_LogCtxChg_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_LogCtxChg_Module__diagsMask__C != (CT__ti_uia_runtime_LogCtxChg_Module__diagsMask)NULL) {
        *ti_uia_runtime_LogCtxChg_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/runtime/LogCtxChg__epilogue.h>

#ifdef ti_uia_runtime_LogCtxChg__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_LogCtxChg__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_LogCtxChg__internalaccess))

#ifndef ti_uia_runtime_LogCtxChg__include_state
#define ti_uia_runtime_LogCtxChg__include_state


#endif /* ti_uia_runtime_LogCtxChg__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_LogCtxChg__nolocalnames)

#ifndef ti_uia_runtime_LogCtxChg__localnames__done
#define ti_uia_runtime_LogCtxChg__localnames__done

/* module prefix */
#define LogCtxChg_isTimestampEnabled ti_uia_runtime_LogCtxChg_isTimestampEnabled
#define LogCtxChg_loggerDefined ti_uia_runtime_LogCtxChg_loggerDefined
#define LogCtxChg_ctxFilterEnabled ti_uia_runtime_LogCtxChg_ctxFilterEnabled
#define LogCtxChg_loggerObj ti_uia_runtime_LogCtxChg_loggerObj
#define LogCtxChg_loggerFxn2 ti_uia_runtime_LogCtxChg_loggerFxn2
#define LogCtxChg_loggerFxn8 ti_uia_runtime_LogCtxChg_loggerFxn8
#define LogCtxChg_putCtxChg1 ti_uia_runtime_LogCtxChg_putCtxChg1
#define LogCtxChg_putCtxChg2 ti_uia_runtime_LogCtxChg_putCtxChg2
#define LogCtxChg_putCtxChg8 ti_uia_runtime_LogCtxChg_putCtxChg8
#define LogCtxChg_app ti_uia_runtime_LogCtxChg_app
#define LogCtxChg_channel ti_uia_runtime_LogCtxChg_channel
#define LogCtxChg_thread ti_uia_runtime_LogCtxChg_thread
#define LogCtxChg_threadAndFunc ti_uia_runtime_LogCtxChg_threadAndFunc
#define LogCtxChg_frame ti_uia_runtime_LogCtxChg_frame
#define LogCtxChg_hwiStart ti_uia_runtime_LogCtxChg_hwiStart
#define LogCtxChg_hwiStop ti_uia_runtime_LogCtxChg_hwiStop
#define LogCtxChg_swiStart ti_uia_runtime_LogCtxChg_swiStart
#define LogCtxChg_swiStop ti_uia_runtime_LogCtxChg_swiStop
#define LogCtxChg_user ti_uia_runtime_LogCtxChg_user
#define LogCtxChg_Module_name ti_uia_runtime_LogCtxChg_Module_name
#define LogCtxChg_Module_id ti_uia_runtime_LogCtxChg_Module_id
#define LogCtxChg_Module_startup ti_uia_runtime_LogCtxChg_Module_startup
#define LogCtxChg_Module_startupDone ti_uia_runtime_LogCtxChg_Module_startupDone
#define LogCtxChg_Module_hasMask ti_uia_runtime_LogCtxChg_Module_hasMask
#define LogCtxChg_Module_getMask ti_uia_runtime_LogCtxChg_Module_getMask
#define LogCtxChg_Module_setMask ti_uia_runtime_LogCtxChg_Module_setMask
#define LogCtxChg_Object_heap ti_uia_runtime_LogCtxChg_Object_heap
#define LogCtxChg_Module_heap ti_uia_runtime_LogCtxChg_Module_heap

#endif /* ti_uia_runtime_LogCtxChg__localnames__done */
#endif
