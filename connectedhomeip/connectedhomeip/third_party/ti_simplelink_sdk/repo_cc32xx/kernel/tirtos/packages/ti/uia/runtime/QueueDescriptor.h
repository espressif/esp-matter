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

#ifndef ti_uia_runtime_QueueDescriptor__include
#define ti_uia_runtime_QueueDescriptor__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_QueueDescriptor__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_QueueDescriptor___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/QueueDescriptor__prologue.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* QueueType_NONE */
#define ti_uia_runtime_QueueDescriptor_QueueType_NONE (0)

/* QueueType_TOHOST_CMD_CIRCULAR_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_CMD_CIRCULAR_BUFFER (1)

/* QueueType_FROMHOST_CMD_CIRCULAR_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_FROMHOST_CMD_CIRCULAR_BUFFER (2)

/* QueueType_TOHOST_EVENT_CIRCULAR_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_CIRCULAR_BUFFER (3)

/* QueueType_TOHOST_EVENT_OVERFLOW_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_OVERFLOW_BUFFER (4)

/* QueueType_TOHOST_DATA_CIRCULAR_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_DATA_CIRCULAR_BUFFER (5)

/* QueueType_FROMHOST_DATA_CIRCULAR_BUFFER */
#define ti_uia_runtime_QueueDescriptor_QueueType_FROMHOST_DATA_CIRCULAR_BUFFER (6)

/* QueueType_TOHOST_EVENT_UIAPACKET_ARRAY */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_ARRAY (7)

/* QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE */
#define ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE (8)

/* Header */
struct ti_uia_runtime_QueueDescriptor_Header {
    xdc_Int structSize;
    ti_uia_runtime_QueueDescriptor_Header *next;
    xdc_UInt queueType;
    xdc_Bits32 *readPtr;
    xdc_Bits32 *writePtr;
    xdc_Bits32 *queueStartAdrs;
    xdc_SizeT queueSizeInMAUs;
    xdc_UInt instanceId;
    xdc_UInt ownerModuleId;
    xdc_UInt priority;
    xdc_Bits32 *numDroppedCtrAdrs;
    xdc_Bits32 *partialPacketWritePtr;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_runtime_QueueDescriptor_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__diagsEnabled ti_uia_runtime_QueueDescriptor_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__diagsEnabled__CR
#define ti_uia_runtime_QueueDescriptor_Module__diagsEnabled__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__diagsEnabled (ti_uia_runtime_QueueDescriptor_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_QueueDescriptor_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__diagsIncluded ti_uia_runtime_QueueDescriptor_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__diagsIncluded__CR
#define ti_uia_runtime_QueueDescriptor_Module__diagsIncluded__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__diagsIncluded (ti_uia_runtime_QueueDescriptor_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask ti_uia_runtime_QueueDescriptor_Module__diagsMask__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__diagsMask__CR
#define ti_uia_runtime_QueueDescriptor_Module__diagsMask__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__diagsMask (ti_uia_runtime_QueueDescriptor_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_QueueDescriptor_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__gateObj ti_uia_runtime_QueueDescriptor_Module__gateObj__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__gateObj__CR
#define ti_uia_runtime_QueueDescriptor_Module__gateObj__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__gateObj (ti_uia_runtime_QueueDescriptor_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_QueueDescriptor_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__gatePrms ti_uia_runtime_QueueDescriptor_Module__gatePrms__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__gatePrms__CR
#define ti_uia_runtime_QueueDescriptor_Module__gatePrms__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__gatePrms (ti_uia_runtime_QueueDescriptor_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_QueueDescriptor_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__id ti_uia_runtime_QueueDescriptor_Module__id__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__id__CR
#define ti_uia_runtime_QueueDescriptor_Module__id__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__id*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__id__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__id (ti_uia_runtime_QueueDescriptor_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_QueueDescriptor_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerDefined ti_uia_runtime_QueueDescriptor_Module__loggerDefined__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerDefined__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerDefined__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerDefined (ti_uia_runtime_QueueDescriptor_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_QueueDescriptor_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerObj ti_uia_runtime_QueueDescriptor_Module__loggerObj__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerObj__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerObj__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerObj (ti_uia_runtime_QueueDescriptor_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn0 ti_uia_runtime_QueueDescriptor_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerFxn0__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn0__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn0 (ti_uia_runtime_QueueDescriptor_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn1 ti_uia_runtime_QueueDescriptor_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerFxn1__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn1__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn1 (ti_uia_runtime_QueueDescriptor_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn2 ti_uia_runtime_QueueDescriptor_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerFxn2__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn2__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn2 (ti_uia_runtime_QueueDescriptor_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn4 ti_uia_runtime_QueueDescriptor_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerFxn4__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn4__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn4 (ti_uia_runtime_QueueDescriptor_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn8 ti_uia_runtime_QueueDescriptor_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_QueueDescriptor_Module__loggerFxn8__CR
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn8__C (*((CT__ti_uia_runtime_QueueDescriptor_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Module__loggerFxn8 (ti_uia_runtime_QueueDescriptor_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_QueueDescriptor_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Object__count ti_uia_runtime_QueueDescriptor_Object__count__C;
#ifdef ti_uia_runtime_QueueDescriptor_Object__count__CR
#define ti_uia_runtime_QueueDescriptor_Object__count__C (*((CT__ti_uia_runtime_QueueDescriptor_Object__count*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Object__count__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Object__count (ti_uia_runtime_QueueDescriptor_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_QueueDescriptor_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Object__heap ti_uia_runtime_QueueDescriptor_Object__heap__C;
#ifdef ti_uia_runtime_QueueDescriptor_Object__heap__CR
#define ti_uia_runtime_QueueDescriptor_Object__heap__C (*((CT__ti_uia_runtime_QueueDescriptor_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Object__heap__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Object__heap (ti_uia_runtime_QueueDescriptor_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_QueueDescriptor_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Object__sizeof ti_uia_runtime_QueueDescriptor_Object__sizeof__C;
#ifdef ti_uia_runtime_QueueDescriptor_Object__sizeof__CR
#define ti_uia_runtime_QueueDescriptor_Object__sizeof__C (*((CT__ti_uia_runtime_QueueDescriptor_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Object__sizeof (ti_uia_runtime_QueueDescriptor_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_QueueDescriptor_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_QueueDescriptor_Object__table ti_uia_runtime_QueueDescriptor_Object__table__C;
#ifdef ti_uia_runtime_QueueDescriptor_Object__table__CR
#define ti_uia_runtime_QueueDescriptor_Object__table__C (*((CT__ti_uia_runtime_QueueDescriptor_Object__table*)(xdcRomConstPtr + ti_uia_runtime_QueueDescriptor_Object__table__C_offset)))
#else
#define ti_uia_runtime_QueueDescriptor_Object__table (ti_uia_runtime_QueueDescriptor_Object__table__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_QueueDescriptor_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_QueueDescriptor_Module__startupDone__S, "ti_uia_runtime_QueueDescriptor_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_QueueDescriptor_Module__startupDone__S( void);

/* addToList__E */
#define ti_uia_runtime_QueueDescriptor_addToList ti_uia_runtime_QueueDescriptor_addToList__E
xdc__CODESECT(ti_uia_runtime_QueueDescriptor_addToList__E, "ti_uia_runtime_QueueDescriptor_addToList")
__extern xdc_Void ti_uia_runtime_QueueDescriptor_addToList__E( ti_uia_runtime_QueueDescriptor_Header *pHdrToAdd);

/* initHeader__E */
#define ti_uia_runtime_QueueDescriptor_initHeader ti_uia_runtime_QueueDescriptor_initHeader__E
xdc__CODESECT(ti_uia_runtime_QueueDescriptor_initHeader__E, "ti_uia_runtime_QueueDescriptor_initHeader")
__extern xdc_Void ti_uia_runtime_QueueDescriptor_initHeader__E( ti_uia_runtime_QueueDescriptor_Header *pHdr, xdc_Ptr start, xdc_SizeT size, xdc_UInt loggerModuleId, xdc_UInt loggerInstanceId, xdc_UInt loggerPriority, xdc_UInt type, xdc_Ptr pNumDroppedCtr);

/* removeFromList__E */
#define ti_uia_runtime_QueueDescriptor_removeFromList ti_uia_runtime_QueueDescriptor_removeFromList__E
xdc__CODESECT(ti_uia_runtime_QueueDescriptor_removeFromList__E, "ti_uia_runtime_QueueDescriptor_removeFromList")
__extern xdc_Void ti_uia_runtime_QueueDescriptor_removeFromList__E( ti_uia_runtime_QueueDescriptor_Header *pHdrToRemove);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_QueueDescriptor_Module_startupDone() ti_uia_runtime_QueueDescriptor_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_QueueDescriptor_Object_heap() ti_uia_runtime_QueueDescriptor_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_QueueDescriptor_Module_heap() ti_uia_runtime_QueueDescriptor_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_QueueDescriptor_Module__id ti_uia_runtime_QueueDescriptor_Module_id(void);
static inline CT__ti_uia_runtime_QueueDescriptor_Module__id ti_uia_runtime_QueueDescriptor_Module_id( void ) 
{
    return ti_uia_runtime_QueueDescriptor_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_QueueDescriptor_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_QueueDescriptor_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_QueueDescriptor_Module__diagsMask__C != (CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_QueueDescriptor_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_QueueDescriptor_Module_getMask(void)
{
    return (ti_uia_runtime_QueueDescriptor_Module__diagsMask__C != (CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask)NULL) ? *ti_uia_runtime_QueueDescriptor_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_QueueDescriptor_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_QueueDescriptor_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_QueueDescriptor_Module__diagsMask__C != (CT__ti_uia_runtime_QueueDescriptor_Module__diagsMask)NULL) {
        *ti_uia_runtime_QueueDescriptor_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/runtime/QueueDescriptor__epilogue.h>

#ifdef ti_uia_runtime_QueueDescriptor__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_QueueDescriptor__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_QueueDescriptor__internalaccess))

#ifndef ti_uia_runtime_QueueDescriptor__include_state
#define ti_uia_runtime_QueueDescriptor__include_state

/* Module_State */
struct ti_uia_runtime_QueueDescriptor_Module_State {
    xdc_Ptr mPtrToFirstDescriptor;
    xdc_UInt mUpdateCount;
    xdc_UInt32 is5555ifInitialized;
};

/* Module__state__V */
#ifndef ti_uia_runtime_QueueDescriptor_Module__state__VR
extern struct ti_uia_runtime_QueueDescriptor_Module_State__ ti_uia_runtime_QueueDescriptor_Module__state__V;
#else
#define ti_uia_runtime_QueueDescriptor_Module__state__V (*((struct ti_uia_runtime_QueueDescriptor_Module_State__*)(xdcRomStatePtr + ti_uia_runtime_QueueDescriptor_Module__state__V_offset)))
#endif

#endif /* ti_uia_runtime_QueueDescriptor__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_QueueDescriptor__nolocalnames)

#ifndef ti_uia_runtime_QueueDescriptor__localnames__done
#define ti_uia_runtime_QueueDescriptor__localnames__done

/* module prefix */
#define QueueDescriptor_QueueType_NONE ti_uia_runtime_QueueDescriptor_QueueType_NONE
#define QueueDescriptor_QueueType_TOHOST_CMD_CIRCULAR_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_CMD_CIRCULAR_BUFFER
#define QueueDescriptor_QueueType_FROMHOST_CMD_CIRCULAR_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_FROMHOST_CMD_CIRCULAR_BUFFER
#define QueueDescriptor_QueueType_TOHOST_EVENT_CIRCULAR_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_CIRCULAR_BUFFER
#define QueueDescriptor_QueueType_TOHOST_EVENT_OVERFLOW_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_OVERFLOW_BUFFER
#define QueueDescriptor_QueueType_TOHOST_DATA_CIRCULAR_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_DATA_CIRCULAR_BUFFER
#define QueueDescriptor_QueueType_FROMHOST_DATA_CIRCULAR_BUFFER ti_uia_runtime_QueueDescriptor_QueueType_FROMHOST_DATA_CIRCULAR_BUFFER
#define QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_ARRAY ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_ARRAY
#define QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE ti_uia_runtime_QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE
#define QueueDescriptor_Header ti_uia_runtime_QueueDescriptor_Header
#define QueueDescriptor_Module_State ti_uia_runtime_QueueDescriptor_Module_State
#define QueueDescriptor_addToList ti_uia_runtime_QueueDescriptor_addToList
#define QueueDescriptor_initHeader ti_uia_runtime_QueueDescriptor_initHeader
#define QueueDescriptor_removeFromList ti_uia_runtime_QueueDescriptor_removeFromList
#define QueueDescriptor_Module_name ti_uia_runtime_QueueDescriptor_Module_name
#define QueueDescriptor_Module_id ti_uia_runtime_QueueDescriptor_Module_id
#define QueueDescriptor_Module_startup ti_uia_runtime_QueueDescriptor_Module_startup
#define QueueDescriptor_Module_startupDone ti_uia_runtime_QueueDescriptor_Module_startupDone
#define QueueDescriptor_Module_hasMask ti_uia_runtime_QueueDescriptor_Module_hasMask
#define QueueDescriptor_Module_getMask ti_uia_runtime_QueueDescriptor_Module_getMask
#define QueueDescriptor_Module_setMask ti_uia_runtime_QueueDescriptor_Module_setMask
#define QueueDescriptor_Object_heap ti_uia_runtime_QueueDescriptor_Object_heap
#define QueueDescriptor_Module_heap ti_uia_runtime_QueueDescriptor_Module_heap

#endif /* ti_uia_runtime_QueueDescriptor__localnames__done */
#endif
