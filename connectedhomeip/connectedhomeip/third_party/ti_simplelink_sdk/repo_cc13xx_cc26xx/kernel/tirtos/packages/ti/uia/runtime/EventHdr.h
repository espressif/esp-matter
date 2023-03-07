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

#ifndef ti_uia_runtime_EventHdr__include
#define ti_uia_runtime_EventHdr__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_EventHdr__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_EventHdr___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/EventHdr__prologue.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* HdrType */
enum ti_uia_runtime_EventHdr_HdrType {
    ti_uia_runtime_EventHdr_HdrType_Event = 0,
    ti_uia_runtime_EventHdr_HdrType_EventWithTimestamp = 1,
    ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotId = 2,
    ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAndTimestamp = 3,
    ti_uia_runtime_EventHdr_HdrType_EventWith32bTimestamp = 4,
    ti_uia_runtime_EventHdr_HdrType_EventWithTimestampAndEndpointId = 5,
    ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAnd32bTimestamp = 6,
    ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAndTimestampAndEndpointId = 7,
    ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWithNoTimestamp = 8,
    ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWith32bTimestamp = 9,
    ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWith64bTimestamp = 10,
    ti_uia_runtime_EventHdr_HdrType_SnapshotEventWithPrevLen = 11,
    ti_uia_runtime_EventHdr_HdrType_SnapshotEventWithPrevLenAnd64bTimestamp = 12,
    ti_uia_runtime_EventHdr_HdrType_Reserved13 = 13,
    ti_uia_runtime_EventHdr_HdrType_Reserved14 = 14,
    ti_uia_runtime_EventHdr_HdrType_Reserved15 = 15,
    ti_uia_runtime_EventHdr_HdrType_Reserved16 = 16,
    ti_uia_runtime_EventHdr_HdrType_Reserved17 = 17,
    ti_uia_runtime_EventHdr_HdrType_Reserved18 = 18,
    ti_uia_runtime_EventHdr_HdrType_Reserved19 = 19,
    ti_uia_runtime_EventHdr_HdrType_Reserved20 = 20,
    ti_uia_runtime_EventHdr_HdrType_Reserved21 = 21,
    ti_uia_runtime_EventHdr_HdrType_Reserved22 = 22,
    ti_uia_runtime_EventHdr_HdrType_Reserved23 = 23,
    ti_uia_runtime_EventHdr_HdrType_Reserved24 = 24,
    ti_uia_runtime_EventHdr_HdrType_Reserved25 = 25,
    ti_uia_runtime_EventHdr_HdrType_Reserved26 = 26,
    ti_uia_runtime_EventHdr_HdrType_Reserved27 = 27,
    ti_uia_runtime_EventHdr_HdrType_Reserved28 = 28,
    ti_uia_runtime_EventHdr_HdrType_Reserved29 = 29,
    ti_uia_runtime_EventHdr_HdrType_Reserved30 = 30,
    ti_uia_runtime_EventHdr_HdrType_Reserved31 = 31
};
typedef enum ti_uia_runtime_EventHdr_HdrType ti_uia_runtime_EventHdr_HdrType;


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_runtime_EventHdr_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__diagsEnabled ti_uia_runtime_EventHdr_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_EventHdr_Module__diagsEnabled__CR
#define ti_uia_runtime_EventHdr_Module__diagsEnabled__C (*((CT__ti_uia_runtime_EventHdr_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__diagsEnabled (ti_uia_runtime_EventHdr_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_EventHdr_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__diagsIncluded ti_uia_runtime_EventHdr_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_EventHdr_Module__diagsIncluded__CR
#define ti_uia_runtime_EventHdr_Module__diagsIncluded__C (*((CT__ti_uia_runtime_EventHdr_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__diagsIncluded (ti_uia_runtime_EventHdr_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_EventHdr_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__diagsMask ti_uia_runtime_EventHdr_Module__diagsMask__C;
#ifdef ti_uia_runtime_EventHdr_Module__diagsMask__CR
#define ti_uia_runtime_EventHdr_Module__diagsMask__C (*((CT__ti_uia_runtime_EventHdr_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__diagsMask (ti_uia_runtime_EventHdr_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_EventHdr_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__gateObj ti_uia_runtime_EventHdr_Module__gateObj__C;
#ifdef ti_uia_runtime_EventHdr_Module__gateObj__CR
#define ti_uia_runtime_EventHdr_Module__gateObj__C (*((CT__ti_uia_runtime_EventHdr_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__gateObj (ti_uia_runtime_EventHdr_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_EventHdr_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__gatePrms ti_uia_runtime_EventHdr_Module__gatePrms__C;
#ifdef ti_uia_runtime_EventHdr_Module__gatePrms__CR
#define ti_uia_runtime_EventHdr_Module__gatePrms__C (*((CT__ti_uia_runtime_EventHdr_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__gatePrms (ti_uia_runtime_EventHdr_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_EventHdr_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__id ti_uia_runtime_EventHdr_Module__id__C;
#ifdef ti_uia_runtime_EventHdr_Module__id__CR
#define ti_uia_runtime_EventHdr_Module__id__C (*((CT__ti_uia_runtime_EventHdr_Module__id*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__id__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__id (ti_uia_runtime_EventHdr_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_EventHdr_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerDefined ti_uia_runtime_EventHdr_Module__loggerDefined__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerDefined__CR
#define ti_uia_runtime_EventHdr_Module__loggerDefined__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerDefined (ti_uia_runtime_EventHdr_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_EventHdr_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerObj ti_uia_runtime_EventHdr_Module__loggerObj__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerObj__CR
#define ti_uia_runtime_EventHdr_Module__loggerObj__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerObj (ti_uia_runtime_EventHdr_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_EventHdr_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerFxn0 ti_uia_runtime_EventHdr_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerFxn0__CR
#define ti_uia_runtime_EventHdr_Module__loggerFxn0__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerFxn0 (ti_uia_runtime_EventHdr_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_EventHdr_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerFxn1 ti_uia_runtime_EventHdr_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerFxn1__CR
#define ti_uia_runtime_EventHdr_Module__loggerFxn1__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerFxn1 (ti_uia_runtime_EventHdr_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_EventHdr_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerFxn2 ti_uia_runtime_EventHdr_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerFxn2__CR
#define ti_uia_runtime_EventHdr_Module__loggerFxn2__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerFxn2 (ti_uia_runtime_EventHdr_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_EventHdr_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerFxn4 ti_uia_runtime_EventHdr_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerFxn4__CR
#define ti_uia_runtime_EventHdr_Module__loggerFxn4__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerFxn4 (ti_uia_runtime_EventHdr_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_EventHdr_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Module__loggerFxn8 ti_uia_runtime_EventHdr_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_EventHdr_Module__loggerFxn8__CR
#define ti_uia_runtime_EventHdr_Module__loggerFxn8__C (*((CT__ti_uia_runtime_EventHdr_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Module__loggerFxn8 (ti_uia_runtime_EventHdr_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_EventHdr_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Object__count ti_uia_runtime_EventHdr_Object__count__C;
#ifdef ti_uia_runtime_EventHdr_Object__count__CR
#define ti_uia_runtime_EventHdr_Object__count__C (*((CT__ti_uia_runtime_EventHdr_Object__count*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Object__count__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Object__count (ti_uia_runtime_EventHdr_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_EventHdr_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Object__heap ti_uia_runtime_EventHdr_Object__heap__C;
#ifdef ti_uia_runtime_EventHdr_Object__heap__CR
#define ti_uia_runtime_EventHdr_Object__heap__C (*((CT__ti_uia_runtime_EventHdr_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Object__heap__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Object__heap (ti_uia_runtime_EventHdr_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_EventHdr_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Object__sizeof ti_uia_runtime_EventHdr_Object__sizeof__C;
#ifdef ti_uia_runtime_EventHdr_Object__sizeof__CR
#define ti_uia_runtime_EventHdr_Object__sizeof__C (*((CT__ti_uia_runtime_EventHdr_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Object__sizeof (ti_uia_runtime_EventHdr_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_EventHdr_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_EventHdr_Object__table ti_uia_runtime_EventHdr_Object__table__C;
#ifdef ti_uia_runtime_EventHdr_Object__table__CR
#define ti_uia_runtime_EventHdr_Object__table__C (*((CT__ti_uia_runtime_EventHdr_Object__table*)(xdcRomConstPtr + ti_uia_runtime_EventHdr_Object__table__C_offset)))
#else
#define ti_uia_runtime_EventHdr_Object__table (ti_uia_runtime_EventHdr_Object__table__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_EventHdr_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_EventHdr_Module__startupDone__S, "ti_uia_runtime_EventHdr_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_EventHdr_Module__startupDone__S( void);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_EventHdr_Module_startupDone() ti_uia_runtime_EventHdr_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_EventHdr_Object_heap() ti_uia_runtime_EventHdr_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_EventHdr_Module_heap() ti_uia_runtime_EventHdr_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_EventHdr_Module__id ti_uia_runtime_EventHdr_Module_id(void);
static inline CT__ti_uia_runtime_EventHdr_Module__id ti_uia_runtime_EventHdr_Module_id( void ) 
{
    return ti_uia_runtime_EventHdr_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_EventHdr_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_EventHdr_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_EventHdr_Module__diagsMask__C != (CT__ti_uia_runtime_EventHdr_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_EventHdr_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_EventHdr_Module_getMask(void)
{
    return (ti_uia_runtime_EventHdr_Module__diagsMask__C != (CT__ti_uia_runtime_EventHdr_Module__diagsMask)NULL) ? *ti_uia_runtime_EventHdr_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_EventHdr_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_EventHdr_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_EventHdr_Module__diagsMask__C != (CT__ti_uia_runtime_EventHdr_Module__diagsMask)NULL) {
        *ti_uia_runtime_EventHdr_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/runtime/EventHdr__epilogue.h>

#ifdef ti_uia_runtime_EventHdr__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_EventHdr__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_EventHdr__internalaccess))

#ifndef ti_uia_runtime_EventHdr__include_state
#define ti_uia_runtime_EventHdr__include_state


#endif /* ti_uia_runtime_EventHdr__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_EventHdr__nolocalnames)

#ifndef ti_uia_runtime_EventHdr__localnames__done
#define ti_uia_runtime_EventHdr__localnames__done

/* module prefix */
#define EventHdr_HdrType ti_uia_runtime_EventHdr_HdrType
#define EventHdr_HdrType_Event ti_uia_runtime_EventHdr_HdrType_Event
#define EventHdr_HdrType_EventWithTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithTimestamp
#define EventHdr_HdrType_EventWithSnapshotId ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotId
#define EventHdr_HdrType_EventWithSnapshotIdAndTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAndTimestamp
#define EventHdr_HdrType_EventWith32bTimestamp ti_uia_runtime_EventHdr_HdrType_EventWith32bTimestamp
#define EventHdr_HdrType_EventWithTimestampAndEndpointId ti_uia_runtime_EventHdr_HdrType_EventWithTimestampAndEndpointId
#define EventHdr_HdrType_EventWithSnapshotIdAnd32bTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAnd32bTimestamp
#define EventHdr_HdrType_EventWithSnapshotIdAndTimestampAndEndpointId ti_uia_runtime_EventHdr_HdrType_EventWithSnapshotIdAndTimestampAndEndpointId
#define EventHdr_HdrType_EventWithPrevLenWithNoTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWithNoTimestamp
#define EventHdr_HdrType_EventWithPrevLenWith32bTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWith32bTimestamp
#define EventHdr_HdrType_EventWithPrevLenWith64bTimestamp ti_uia_runtime_EventHdr_HdrType_EventWithPrevLenWith64bTimestamp
#define EventHdr_HdrType_SnapshotEventWithPrevLen ti_uia_runtime_EventHdr_HdrType_SnapshotEventWithPrevLen
#define EventHdr_HdrType_SnapshotEventWithPrevLenAnd64bTimestamp ti_uia_runtime_EventHdr_HdrType_SnapshotEventWithPrevLenAnd64bTimestamp
#define EventHdr_HdrType_Reserved13 ti_uia_runtime_EventHdr_HdrType_Reserved13
#define EventHdr_HdrType_Reserved14 ti_uia_runtime_EventHdr_HdrType_Reserved14
#define EventHdr_HdrType_Reserved15 ti_uia_runtime_EventHdr_HdrType_Reserved15
#define EventHdr_HdrType_Reserved16 ti_uia_runtime_EventHdr_HdrType_Reserved16
#define EventHdr_HdrType_Reserved17 ti_uia_runtime_EventHdr_HdrType_Reserved17
#define EventHdr_HdrType_Reserved18 ti_uia_runtime_EventHdr_HdrType_Reserved18
#define EventHdr_HdrType_Reserved19 ti_uia_runtime_EventHdr_HdrType_Reserved19
#define EventHdr_HdrType_Reserved20 ti_uia_runtime_EventHdr_HdrType_Reserved20
#define EventHdr_HdrType_Reserved21 ti_uia_runtime_EventHdr_HdrType_Reserved21
#define EventHdr_HdrType_Reserved22 ti_uia_runtime_EventHdr_HdrType_Reserved22
#define EventHdr_HdrType_Reserved23 ti_uia_runtime_EventHdr_HdrType_Reserved23
#define EventHdr_HdrType_Reserved24 ti_uia_runtime_EventHdr_HdrType_Reserved24
#define EventHdr_HdrType_Reserved25 ti_uia_runtime_EventHdr_HdrType_Reserved25
#define EventHdr_HdrType_Reserved26 ti_uia_runtime_EventHdr_HdrType_Reserved26
#define EventHdr_HdrType_Reserved27 ti_uia_runtime_EventHdr_HdrType_Reserved27
#define EventHdr_HdrType_Reserved28 ti_uia_runtime_EventHdr_HdrType_Reserved28
#define EventHdr_HdrType_Reserved29 ti_uia_runtime_EventHdr_HdrType_Reserved29
#define EventHdr_HdrType_Reserved30 ti_uia_runtime_EventHdr_HdrType_Reserved30
#define EventHdr_HdrType_Reserved31 ti_uia_runtime_EventHdr_HdrType_Reserved31
#define EventHdr_getHdrType ti_uia_runtime_EventHdr_getHdrType
#define EventHdr_setHdrType ti_uia_runtime_EventHdr_setHdrType
#define EventHdr_getLength ti_uia_runtime_EventHdr_getLength
#define EventHdr_setLength ti_uia_runtime_EventHdr_setLength
#define EventHdr_getSeqCount ti_uia_runtime_EventHdr_getSeqCount
#define EventHdr_setSeqCount ti_uia_runtime_EventHdr_setSeqCount
#define EventHdr_genEventHdrWord1 ti_uia_runtime_EventHdr_genEventHdrWord1
#define EventHdr_genEventWithPrevLenHdrWord1 ti_uia_runtime_EventHdr_genEventWithPrevLenHdrWord1
#define EventHdr_getEventHdrWithPrevLenSeqCount ti_uia_runtime_EventHdr_getEventHdrWithPrevLenSeqCount
#define EventHdr_getPrevLength ti_uia_runtime_EventHdr_getPrevLength
#define EventHdr_Module_name ti_uia_runtime_EventHdr_Module_name
#define EventHdr_Module_id ti_uia_runtime_EventHdr_Module_id
#define EventHdr_Module_startup ti_uia_runtime_EventHdr_Module_startup
#define EventHdr_Module_startupDone ti_uia_runtime_EventHdr_Module_startupDone
#define EventHdr_Module_hasMask ti_uia_runtime_EventHdr_Module_hasMask
#define EventHdr_Module_getMask ti_uia_runtime_EventHdr_Module_getMask
#define EventHdr_Module_setMask ti_uia_runtime_EventHdr_Module_setMask
#define EventHdr_Object_heap ti_uia_runtime_EventHdr_Object_heap
#define EventHdr_Module_heap ti_uia_runtime_EventHdr_Module_heap

#endif /* ti_uia_runtime_EventHdr__localnames__done */
#endif
