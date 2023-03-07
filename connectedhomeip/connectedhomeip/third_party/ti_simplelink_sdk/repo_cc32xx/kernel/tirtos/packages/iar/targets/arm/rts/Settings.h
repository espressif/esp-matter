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

#ifndef iar_targets_arm_rts_Settings__include
#define iar_targets_arm_rts_Settings__include

#ifndef __nested__
#define __nested__
#define iar_targets_arm_rts_Settings__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define iar_targets_arm_rts_Settings___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <iar/targets/arm/rts/package/package.defs.h>

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
typedef xdc_Bits32 CT__iar_targets_arm_rts_Settings_Module__diagsEnabled;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__diagsEnabled iar_targets_arm_rts_Settings_Module__diagsEnabled__C;
#ifdef iar_targets_arm_rts_Settings_Module__diagsEnabled__CR
#define iar_targets_arm_rts_Settings_Module__diagsEnabled__C (*((CT__iar_targets_arm_rts_Settings_Module__diagsEnabled*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__diagsEnabled__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__diagsEnabled (iar_targets_arm_rts_Settings_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__iar_targets_arm_rts_Settings_Module__diagsIncluded;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__diagsIncluded iar_targets_arm_rts_Settings_Module__diagsIncluded__C;
#ifdef iar_targets_arm_rts_Settings_Module__diagsIncluded__CR
#define iar_targets_arm_rts_Settings_Module__diagsIncluded__C (*((CT__iar_targets_arm_rts_Settings_Module__diagsIncluded*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__diagsIncluded__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__diagsIncluded (iar_targets_arm_rts_Settings_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__iar_targets_arm_rts_Settings_Module__diagsMask;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__diagsMask iar_targets_arm_rts_Settings_Module__diagsMask__C;
#ifdef iar_targets_arm_rts_Settings_Module__diagsMask__CR
#define iar_targets_arm_rts_Settings_Module__diagsMask__C (*((CT__iar_targets_arm_rts_Settings_Module__diagsMask*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__diagsMask__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__diagsMask (iar_targets_arm_rts_Settings_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__iar_targets_arm_rts_Settings_Module__gateObj;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__gateObj iar_targets_arm_rts_Settings_Module__gateObj__C;
#ifdef iar_targets_arm_rts_Settings_Module__gateObj__CR
#define iar_targets_arm_rts_Settings_Module__gateObj__C (*((CT__iar_targets_arm_rts_Settings_Module__gateObj*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__gateObj__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__gateObj (iar_targets_arm_rts_Settings_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__iar_targets_arm_rts_Settings_Module__gatePrms;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__gatePrms iar_targets_arm_rts_Settings_Module__gatePrms__C;
#ifdef iar_targets_arm_rts_Settings_Module__gatePrms__CR
#define iar_targets_arm_rts_Settings_Module__gatePrms__C (*((CT__iar_targets_arm_rts_Settings_Module__gatePrms*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__gatePrms__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__gatePrms (iar_targets_arm_rts_Settings_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__iar_targets_arm_rts_Settings_Module__id;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__id iar_targets_arm_rts_Settings_Module__id__C;
#ifdef iar_targets_arm_rts_Settings_Module__id__CR
#define iar_targets_arm_rts_Settings_Module__id__C (*((CT__iar_targets_arm_rts_Settings_Module__id*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__id__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__id (iar_targets_arm_rts_Settings_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__iar_targets_arm_rts_Settings_Module__loggerDefined;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerDefined iar_targets_arm_rts_Settings_Module__loggerDefined__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerDefined__CR
#define iar_targets_arm_rts_Settings_Module__loggerDefined__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerDefined*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerDefined__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerDefined (iar_targets_arm_rts_Settings_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__iar_targets_arm_rts_Settings_Module__loggerObj;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerObj iar_targets_arm_rts_Settings_Module__loggerObj__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerObj__CR
#define iar_targets_arm_rts_Settings_Module__loggerObj__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerObj*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerObj__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerObj (iar_targets_arm_rts_Settings_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__iar_targets_arm_rts_Settings_Module__loggerFxn0;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerFxn0 iar_targets_arm_rts_Settings_Module__loggerFxn0__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerFxn0__CR
#define iar_targets_arm_rts_Settings_Module__loggerFxn0__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerFxn0*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerFxn0__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerFxn0 (iar_targets_arm_rts_Settings_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__iar_targets_arm_rts_Settings_Module__loggerFxn1;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerFxn1 iar_targets_arm_rts_Settings_Module__loggerFxn1__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerFxn1__CR
#define iar_targets_arm_rts_Settings_Module__loggerFxn1__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerFxn1*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerFxn1__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerFxn1 (iar_targets_arm_rts_Settings_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__iar_targets_arm_rts_Settings_Module__loggerFxn2;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerFxn2 iar_targets_arm_rts_Settings_Module__loggerFxn2__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerFxn2__CR
#define iar_targets_arm_rts_Settings_Module__loggerFxn2__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerFxn2*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerFxn2__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerFxn2 (iar_targets_arm_rts_Settings_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__iar_targets_arm_rts_Settings_Module__loggerFxn4;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerFxn4 iar_targets_arm_rts_Settings_Module__loggerFxn4__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerFxn4__CR
#define iar_targets_arm_rts_Settings_Module__loggerFxn4__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerFxn4*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerFxn4__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerFxn4 (iar_targets_arm_rts_Settings_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__iar_targets_arm_rts_Settings_Module__loggerFxn8;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Module__loggerFxn8 iar_targets_arm_rts_Settings_Module__loggerFxn8__C;
#ifdef iar_targets_arm_rts_Settings_Module__loggerFxn8__CR
#define iar_targets_arm_rts_Settings_Module__loggerFxn8__C (*((CT__iar_targets_arm_rts_Settings_Module__loggerFxn8*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Module__loggerFxn8__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Module__loggerFxn8 (iar_targets_arm_rts_Settings_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__iar_targets_arm_rts_Settings_Object__count;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Object__count iar_targets_arm_rts_Settings_Object__count__C;
#ifdef iar_targets_arm_rts_Settings_Object__count__CR
#define iar_targets_arm_rts_Settings_Object__count__C (*((CT__iar_targets_arm_rts_Settings_Object__count*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Object__count__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Object__count (iar_targets_arm_rts_Settings_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__iar_targets_arm_rts_Settings_Object__heap;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Object__heap iar_targets_arm_rts_Settings_Object__heap__C;
#ifdef iar_targets_arm_rts_Settings_Object__heap__CR
#define iar_targets_arm_rts_Settings_Object__heap__C (*((CT__iar_targets_arm_rts_Settings_Object__heap*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Object__heap__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Object__heap (iar_targets_arm_rts_Settings_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__iar_targets_arm_rts_Settings_Object__sizeof;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Object__sizeof iar_targets_arm_rts_Settings_Object__sizeof__C;
#ifdef iar_targets_arm_rts_Settings_Object__sizeof__CR
#define iar_targets_arm_rts_Settings_Object__sizeof__C (*((CT__iar_targets_arm_rts_Settings_Object__sizeof*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Object__sizeof__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Object__sizeof (iar_targets_arm_rts_Settings_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__iar_targets_arm_rts_Settings_Object__table;
__extern __FAR__ const CT__iar_targets_arm_rts_Settings_Object__table iar_targets_arm_rts_Settings_Object__table__C;
#ifdef iar_targets_arm_rts_Settings_Object__table__CR
#define iar_targets_arm_rts_Settings_Object__table__C (*((CT__iar_targets_arm_rts_Settings_Object__table*)(xdcRomConstPtr + iar_targets_arm_rts_Settings_Object__table__C_offset)))
#else
#define iar_targets_arm_rts_Settings_Object__table (iar_targets_arm_rts_Settings_Object__table__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define iar_targets_arm_rts_Settings_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(iar_targets_arm_rts_Settings_Module__startupDone__S, "iar_targets_arm_rts_Settings_Module__startupDone__S")
__extern xdc_Bool iar_targets_arm_rts_Settings_Module__startupDone__S( void);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define iar_targets_arm_rts_Settings_Module_startupDone() iar_targets_arm_rts_Settings_Module__startupDone__S()

/* Object_heap */
#define iar_targets_arm_rts_Settings_Object_heap() iar_targets_arm_rts_Settings_Object__heap__C

/* Module_heap */
#define iar_targets_arm_rts_Settings_Module_heap() iar_targets_arm_rts_Settings_Object__heap__C

/* Module_id */
static inline CT__iar_targets_arm_rts_Settings_Module__id iar_targets_arm_rts_Settings_Module_id(void);
static inline CT__iar_targets_arm_rts_Settings_Module__id iar_targets_arm_rts_Settings_Module_id( void ) 
{
    return iar_targets_arm_rts_Settings_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool iar_targets_arm_rts_Settings_Module_hasMask(void);
static inline xdc_Bool iar_targets_arm_rts_Settings_Module_hasMask(void)
{
    return (xdc_Bool)(iar_targets_arm_rts_Settings_Module__diagsMask__C != (CT__iar_targets_arm_rts_Settings_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 iar_targets_arm_rts_Settings_Module_getMask(void);
static inline xdc_Bits16 iar_targets_arm_rts_Settings_Module_getMask(void)
{
    return (iar_targets_arm_rts_Settings_Module__diagsMask__C != (CT__iar_targets_arm_rts_Settings_Module__diagsMask)NULL) ? *iar_targets_arm_rts_Settings_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void iar_targets_arm_rts_Settings_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void iar_targets_arm_rts_Settings_Module_setMask(xdc_Bits16 mask)
{
    if (iar_targets_arm_rts_Settings_Module__diagsMask__C != (CT__iar_targets_arm_rts_Settings_Module__diagsMask)NULL) {
        *iar_targets_arm_rts_Settings_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef iar_targets_arm_rts_Settings__top__
#undef __nested__
#endif

#endif /* iar_targets_arm_rts_Settings__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(iar_targets_arm_rts_Settings__internalaccess))

#ifndef iar_targets_arm_rts_Settings__include_state
#define iar_targets_arm_rts_Settings__include_state


#endif /* iar_targets_arm_rts_Settings__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(iar_targets_arm_rts_Settings__nolocalnames)

#ifndef iar_targets_arm_rts_Settings__localnames__done
#define iar_targets_arm_rts_Settings__localnames__done

/* module prefix */
#define Settings_Module_name iar_targets_arm_rts_Settings_Module_name
#define Settings_Module_id iar_targets_arm_rts_Settings_Module_id
#define Settings_Module_startup iar_targets_arm_rts_Settings_Module_startup
#define Settings_Module_startupDone iar_targets_arm_rts_Settings_Module_startupDone
#define Settings_Module_hasMask iar_targets_arm_rts_Settings_Module_hasMask
#define Settings_Module_getMask iar_targets_arm_rts_Settings_Module_getMask
#define Settings_Module_setMask iar_targets_arm_rts_Settings_Module_setMask
#define Settings_Object_heap iar_targets_arm_rts_Settings_Object_heap
#define Settings_Module_heap iar_targets_arm_rts_Settings_Module_heap

#endif /* iar_targets_arm_rts_Settings__localnames__done */
#endif
