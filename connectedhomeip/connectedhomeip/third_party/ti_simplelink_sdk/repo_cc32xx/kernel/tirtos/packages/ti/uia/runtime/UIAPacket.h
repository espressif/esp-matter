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

#ifndef ti_uia_runtime_UIAPacket__include
#define ti_uia_runtime_UIAPacket__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_UIAPacket__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_UIAPacket___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/UIAPacket__prologue.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <ti/uia/runtime/IUIATransfer.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Hdr */
struct ti_uia_runtime_UIAPacket_Hdr {
    xdc_Bits32 word1;
    xdc_Bits32 word2;
    xdc_Bits32 word3;
    xdc_Bits32 word4;
};

/* HdrType */
enum ti_uia_runtime_UIAPacket_HdrType {
    ti_uia_runtime_UIAPacket_HdrType_InvalidData = 0,
    ti_uia_runtime_UIAPacket_HdrType_MsgWithPID = 1,
    ti_uia_runtime_UIAPacket_HdrType_EventPktWithCRC = 2,
    ti_uia_runtime_UIAPacket_HdrType_MinEventPkt = 3,
    ti_uia_runtime_UIAPacket_HdrType_Reserved4 = 4,
    ti_uia_runtime_UIAPacket_HdrType_Reserved5 = 5,
    ti_uia_runtime_UIAPacket_HdrType_Reserved6 = 6,
    ti_uia_runtime_UIAPacket_HdrType_Reserved7 = 7,
    ti_uia_runtime_UIAPacket_HdrType_ChannelizedData = 8,
    ti_uia_runtime_UIAPacket_HdrType_Msg = 9,
    ti_uia_runtime_UIAPacket_HdrType_EventPkt = 10,
    ti_uia_runtime_UIAPacket_HdrType_CPUTrace = 11,
    ti_uia_runtime_UIAPacket_HdrType_STMTrace = 12,
    ti_uia_runtime_UIAPacket_HdrType_MemoryBuffer = 13,
    ti_uia_runtime_UIAPacket_HdrType_USER2 = 14,
    ti_uia_runtime_UIAPacket_HdrType_USER3 = 15
};
typedef enum ti_uia_runtime_UIAPacket_HdrType ti_uia_runtime_UIAPacket_HdrType;

/* PayloadEndian */
enum ti_uia_runtime_UIAPacket_PayloadEndian {
    ti_uia_runtime_UIAPacket_PayloadEndian_LITTLE = 0,
    ti_uia_runtime_UIAPacket_PayloadEndian_BIG = 1
};
typedef enum ti_uia_runtime_UIAPacket_PayloadEndian ti_uia_runtime_UIAPacket_PayloadEndian;

/* Footer */
struct ti_uia_runtime_UIAPacket_Footer {
    xdc_Int32 word1;
};

/* MsgType */
enum ti_uia_runtime_UIAPacket_MsgType {
    ti_uia_runtime_UIAPacket_MsgType_ACK = 0,
    ti_uia_runtime_UIAPacket_MsgType_CMD = 1,
    ti_uia_runtime_UIAPacket_MsgType_RESULT = 2,
    ti_uia_runtime_UIAPacket_MsgType_PARTIALRESULT = 3,
    ti_uia_runtime_UIAPacket_MsgType_NOTIFY = 4,
    ti_uia_runtime_UIAPacket_MsgType_FLOWCTRL = 5,
    ti_uia_runtime_UIAPacket_MsgType_DATA = 6,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED7,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED8,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED9,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED10,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED11,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED12,
    ti_uia_runtime_UIAPacket_MsgType_RESERVED13,
    ti_uia_runtime_UIAPacket_MsgType_NACK_BAD_DATA = 14,
    ti_uia_runtime_UIAPacket_MsgType_NACK_WITH_ERROR_CODE = 15
};
typedef enum ti_uia_runtime_UIAPacket_MsgType ti_uia_runtime_UIAPacket_MsgType;

/* NACKErrorCode */
enum ti_uia_runtime_UIAPacket_NACKErrorCode {
    ti_uia_runtime_UIAPacket_NACKErrorCode_NO_REASON_SPECIFIED = 0,
    ti_uia_runtime_UIAPacket_NACKErrorCode_SERVICE_NOT_SUPPORTED = 1,
    ti_uia_runtime_UIAPacket_NACKErrorCode_CMD_NOT_SUPPORTED = 2,
    ti_uia_runtime_UIAPacket_NACKErrorCode_QUEUE_FULL = 3,
    ti_uia_runtime_UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS = 4,
    ti_uia_runtime_UIAPacket_NACKErrorCode_BAD_MESSAGE_LENGTH = 5
};
typedef enum ti_uia_runtime_UIAPacket_NACKErrorCode ti_uia_runtime_UIAPacket_NACKErrorCode;

/* HOST */
#define ti_uia_runtime_UIAPacket_HOST (0xFFFF)

/* BROADCAST */
#define ti_uia_runtime_UIAPacket_BROADCAST (0xFFFE)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_runtime_UIAPacket_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__diagsEnabled ti_uia_runtime_UIAPacket_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_UIAPacket_Module__diagsEnabled__CR
#define ti_uia_runtime_UIAPacket_Module__diagsEnabled__C (*((CT__ti_uia_runtime_UIAPacket_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__diagsEnabled (ti_uia_runtime_UIAPacket_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_UIAPacket_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__diagsIncluded ti_uia_runtime_UIAPacket_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_UIAPacket_Module__diagsIncluded__CR
#define ti_uia_runtime_UIAPacket_Module__diagsIncluded__C (*((CT__ti_uia_runtime_UIAPacket_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__diagsIncluded (ti_uia_runtime_UIAPacket_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_UIAPacket_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__diagsMask ti_uia_runtime_UIAPacket_Module__diagsMask__C;
#ifdef ti_uia_runtime_UIAPacket_Module__diagsMask__CR
#define ti_uia_runtime_UIAPacket_Module__diagsMask__C (*((CT__ti_uia_runtime_UIAPacket_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__diagsMask (ti_uia_runtime_UIAPacket_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_UIAPacket_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__gateObj ti_uia_runtime_UIAPacket_Module__gateObj__C;
#ifdef ti_uia_runtime_UIAPacket_Module__gateObj__CR
#define ti_uia_runtime_UIAPacket_Module__gateObj__C (*((CT__ti_uia_runtime_UIAPacket_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__gateObj (ti_uia_runtime_UIAPacket_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_UIAPacket_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__gatePrms ti_uia_runtime_UIAPacket_Module__gatePrms__C;
#ifdef ti_uia_runtime_UIAPacket_Module__gatePrms__CR
#define ti_uia_runtime_UIAPacket_Module__gatePrms__C (*((CT__ti_uia_runtime_UIAPacket_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__gatePrms (ti_uia_runtime_UIAPacket_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_UIAPacket_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__id ti_uia_runtime_UIAPacket_Module__id__C;
#ifdef ti_uia_runtime_UIAPacket_Module__id__CR
#define ti_uia_runtime_UIAPacket_Module__id__C (*((CT__ti_uia_runtime_UIAPacket_Module__id*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__id__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__id (ti_uia_runtime_UIAPacket_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_UIAPacket_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerDefined ti_uia_runtime_UIAPacket_Module__loggerDefined__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerDefined__CR
#define ti_uia_runtime_UIAPacket_Module__loggerDefined__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerDefined (ti_uia_runtime_UIAPacket_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_UIAPacket_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerObj ti_uia_runtime_UIAPacket_Module__loggerObj__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerObj__CR
#define ti_uia_runtime_UIAPacket_Module__loggerObj__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerObj (ti_uia_runtime_UIAPacket_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_UIAPacket_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerFxn0 ti_uia_runtime_UIAPacket_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerFxn0__CR
#define ti_uia_runtime_UIAPacket_Module__loggerFxn0__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerFxn0 (ti_uia_runtime_UIAPacket_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_UIAPacket_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerFxn1 ti_uia_runtime_UIAPacket_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerFxn1__CR
#define ti_uia_runtime_UIAPacket_Module__loggerFxn1__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerFxn1 (ti_uia_runtime_UIAPacket_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_UIAPacket_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerFxn2 ti_uia_runtime_UIAPacket_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerFxn2__CR
#define ti_uia_runtime_UIAPacket_Module__loggerFxn2__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerFxn2 (ti_uia_runtime_UIAPacket_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_UIAPacket_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerFxn4 ti_uia_runtime_UIAPacket_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerFxn4__CR
#define ti_uia_runtime_UIAPacket_Module__loggerFxn4__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerFxn4 (ti_uia_runtime_UIAPacket_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_UIAPacket_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Module__loggerFxn8 ti_uia_runtime_UIAPacket_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_UIAPacket_Module__loggerFxn8__CR
#define ti_uia_runtime_UIAPacket_Module__loggerFxn8__C (*((CT__ti_uia_runtime_UIAPacket_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Module__loggerFxn8 (ti_uia_runtime_UIAPacket_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_UIAPacket_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Object__count ti_uia_runtime_UIAPacket_Object__count__C;
#ifdef ti_uia_runtime_UIAPacket_Object__count__CR
#define ti_uia_runtime_UIAPacket_Object__count__C (*((CT__ti_uia_runtime_UIAPacket_Object__count*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Object__count__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Object__count (ti_uia_runtime_UIAPacket_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_UIAPacket_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Object__heap ti_uia_runtime_UIAPacket_Object__heap__C;
#ifdef ti_uia_runtime_UIAPacket_Object__heap__CR
#define ti_uia_runtime_UIAPacket_Object__heap__C (*((CT__ti_uia_runtime_UIAPacket_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Object__heap__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Object__heap (ti_uia_runtime_UIAPacket_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_UIAPacket_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Object__sizeof ti_uia_runtime_UIAPacket_Object__sizeof__C;
#ifdef ti_uia_runtime_UIAPacket_Object__sizeof__CR
#define ti_uia_runtime_UIAPacket_Object__sizeof__C (*((CT__ti_uia_runtime_UIAPacket_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Object__sizeof (ti_uia_runtime_UIAPacket_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_UIAPacket_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_Object__table ti_uia_runtime_UIAPacket_Object__table__C;
#ifdef ti_uia_runtime_UIAPacket_Object__table__CR
#define ti_uia_runtime_UIAPacket_Object__table__C (*((CT__ti_uia_runtime_UIAPacket_Object__table*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_Object__table__C_offset)))
#else
#define ti_uia_runtime_UIAPacket_Object__table (ti_uia_runtime_UIAPacket_Object__table__C)
#endif

/* maxPktLengthInBytes */
typedef xdc_Int CT__ti_uia_runtime_UIAPacket_maxPktLengthInBytes;
__extern __FAR__ const CT__ti_uia_runtime_UIAPacket_maxPktLengthInBytes ti_uia_runtime_UIAPacket_maxPktLengthInBytes__C;
#ifdef ti_uia_runtime_UIAPacket_maxPktLengthInBytes__CR
#define ti_uia_runtime_UIAPacket_maxPktLengthInBytes (*((CT__ti_uia_runtime_UIAPacket_maxPktLengthInBytes*)(xdcRomConstPtr + ti_uia_runtime_UIAPacket_maxPktLengthInBytes__C_offset)))
#else
#ifdef ti_uia_runtime_UIAPacket_maxPktLengthInBytes__D
#define ti_uia_runtime_UIAPacket_maxPktLengthInBytes (ti_uia_runtime_UIAPacket_maxPktLengthInBytes__D)
#else
#define ti_uia_runtime_UIAPacket_maxPktLengthInBytes (ti_uia_runtime_UIAPacket_maxPktLengthInBytes__C)
#endif
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_UIAPacket_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_UIAPacket_Module__startupDone__S, "ti_uia_runtime_UIAPacket_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_UIAPacket_Module__startupDone__S( void);

/* getLength__E */
#define ti_uia_runtime_UIAPacket_getLength ti_uia_runtime_UIAPacket_getLength__E
xdc__CODESECT(ti_uia_runtime_UIAPacket_getLength__E, "ti_uia_runtime_UIAPacket_getLength")
__extern xdc_Int32 ti_uia_runtime_UIAPacket_getLength__E( ti_uia_runtime_UIAPacket_Hdr *pHdr);
xdc__CODESECT(ti_uia_runtime_UIAPacket_getLength__F, "ti_uia_runtime_UIAPacket_getLength")
__extern xdc_Int32 ti_uia_runtime_UIAPacket_getLength__F( ti_uia_runtime_UIAPacket_Hdr *pHdr);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_UIAPacket_Module_startupDone() ti_uia_runtime_UIAPacket_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_UIAPacket_Object_heap() ti_uia_runtime_UIAPacket_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_UIAPacket_Module_heap() ti_uia_runtime_UIAPacket_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_UIAPacket_Module__id ti_uia_runtime_UIAPacket_Module_id(void);
static inline CT__ti_uia_runtime_UIAPacket_Module__id ti_uia_runtime_UIAPacket_Module_id( void ) 
{
    return ti_uia_runtime_UIAPacket_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_UIAPacket_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_UIAPacket_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_UIAPacket_Module__diagsMask__C != (CT__ti_uia_runtime_UIAPacket_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_UIAPacket_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_UIAPacket_Module_getMask(void)
{
    return (ti_uia_runtime_UIAPacket_Module__diagsMask__C != (CT__ti_uia_runtime_UIAPacket_Module__diagsMask)NULL) ? *ti_uia_runtime_UIAPacket_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_UIAPacket_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_UIAPacket_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_UIAPacket_Module__diagsMask__C != (CT__ti_uia_runtime_UIAPacket_Module__diagsMask)NULL) {
        *ti_uia_runtime_UIAPacket_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/runtime/UIAPacket__epilogue.h>

#ifdef ti_uia_runtime_UIAPacket__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_UIAPacket__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_UIAPacket__internalaccess))

#ifndef ti_uia_runtime_UIAPacket__include_state
#define ti_uia_runtime_UIAPacket__include_state


#endif /* ti_uia_runtime_UIAPacket__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_UIAPacket__nolocalnames)

#ifndef ti_uia_runtime_UIAPacket__localnames__done
#define ti_uia_runtime_UIAPacket__localnames__done

/* module prefix */
#define UIAPacket_Hdr ti_uia_runtime_UIAPacket_Hdr
#define UIAPacket_HdrType ti_uia_runtime_UIAPacket_HdrType
#define UIAPacket_PayloadEndian ti_uia_runtime_UIAPacket_PayloadEndian
#define UIAPacket_Footer ti_uia_runtime_UIAPacket_Footer
#define UIAPacket_MsgType ti_uia_runtime_UIAPacket_MsgType
#define UIAPacket_NACKErrorCode ti_uia_runtime_UIAPacket_NACKErrorCode
#define UIAPacket_HOST ti_uia_runtime_UIAPacket_HOST
#define UIAPacket_BROADCAST ti_uia_runtime_UIAPacket_BROADCAST
#define UIAPacket_HdrType_InvalidData ti_uia_runtime_UIAPacket_HdrType_InvalidData
#define UIAPacket_HdrType_MsgWithPID ti_uia_runtime_UIAPacket_HdrType_MsgWithPID
#define UIAPacket_HdrType_EventPktWithCRC ti_uia_runtime_UIAPacket_HdrType_EventPktWithCRC
#define UIAPacket_HdrType_MinEventPkt ti_uia_runtime_UIAPacket_HdrType_MinEventPkt
#define UIAPacket_HdrType_Reserved4 ti_uia_runtime_UIAPacket_HdrType_Reserved4
#define UIAPacket_HdrType_Reserved5 ti_uia_runtime_UIAPacket_HdrType_Reserved5
#define UIAPacket_HdrType_Reserved6 ti_uia_runtime_UIAPacket_HdrType_Reserved6
#define UIAPacket_HdrType_Reserved7 ti_uia_runtime_UIAPacket_HdrType_Reserved7
#define UIAPacket_HdrType_ChannelizedData ti_uia_runtime_UIAPacket_HdrType_ChannelizedData
#define UIAPacket_HdrType_Msg ti_uia_runtime_UIAPacket_HdrType_Msg
#define UIAPacket_HdrType_EventPkt ti_uia_runtime_UIAPacket_HdrType_EventPkt
#define UIAPacket_HdrType_CPUTrace ti_uia_runtime_UIAPacket_HdrType_CPUTrace
#define UIAPacket_HdrType_STMTrace ti_uia_runtime_UIAPacket_HdrType_STMTrace
#define UIAPacket_HdrType_MemoryBuffer ti_uia_runtime_UIAPacket_HdrType_MemoryBuffer
#define UIAPacket_HdrType_USER2 ti_uia_runtime_UIAPacket_HdrType_USER2
#define UIAPacket_HdrType_USER3 ti_uia_runtime_UIAPacket_HdrType_USER3
#define UIAPacket_PayloadEndian_LITTLE ti_uia_runtime_UIAPacket_PayloadEndian_LITTLE
#define UIAPacket_PayloadEndian_BIG ti_uia_runtime_UIAPacket_PayloadEndian_BIG
#define UIAPacket_MsgType_ACK ti_uia_runtime_UIAPacket_MsgType_ACK
#define UIAPacket_MsgType_CMD ti_uia_runtime_UIAPacket_MsgType_CMD
#define UIAPacket_MsgType_RESULT ti_uia_runtime_UIAPacket_MsgType_RESULT
#define UIAPacket_MsgType_PARTIALRESULT ti_uia_runtime_UIAPacket_MsgType_PARTIALRESULT
#define UIAPacket_MsgType_NOTIFY ti_uia_runtime_UIAPacket_MsgType_NOTIFY
#define UIAPacket_MsgType_FLOWCTRL ti_uia_runtime_UIAPacket_MsgType_FLOWCTRL
#define UIAPacket_MsgType_DATA ti_uia_runtime_UIAPacket_MsgType_DATA
#define UIAPacket_MsgType_RESERVED7 ti_uia_runtime_UIAPacket_MsgType_RESERVED7
#define UIAPacket_MsgType_RESERVED8 ti_uia_runtime_UIAPacket_MsgType_RESERVED8
#define UIAPacket_MsgType_RESERVED9 ti_uia_runtime_UIAPacket_MsgType_RESERVED9
#define UIAPacket_MsgType_RESERVED10 ti_uia_runtime_UIAPacket_MsgType_RESERVED10
#define UIAPacket_MsgType_RESERVED11 ti_uia_runtime_UIAPacket_MsgType_RESERVED11
#define UIAPacket_MsgType_RESERVED12 ti_uia_runtime_UIAPacket_MsgType_RESERVED12
#define UIAPacket_MsgType_RESERVED13 ti_uia_runtime_UIAPacket_MsgType_RESERVED13
#define UIAPacket_MsgType_NACK_BAD_DATA ti_uia_runtime_UIAPacket_MsgType_NACK_BAD_DATA
#define UIAPacket_MsgType_NACK_WITH_ERROR_CODE ti_uia_runtime_UIAPacket_MsgType_NACK_WITH_ERROR_CODE
#define UIAPacket_NACKErrorCode_NO_REASON_SPECIFIED ti_uia_runtime_UIAPacket_NACKErrorCode_NO_REASON_SPECIFIED
#define UIAPacket_NACKErrorCode_SERVICE_NOT_SUPPORTED ti_uia_runtime_UIAPacket_NACKErrorCode_SERVICE_NOT_SUPPORTED
#define UIAPacket_NACKErrorCode_CMD_NOT_SUPPORTED ti_uia_runtime_UIAPacket_NACKErrorCode_CMD_NOT_SUPPORTED
#define UIAPacket_NACKErrorCode_QUEUE_FULL ti_uia_runtime_UIAPacket_NACKErrorCode_QUEUE_FULL
#define UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS ti_uia_runtime_UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS
#define UIAPacket_NACKErrorCode_BAD_MESSAGE_LENGTH ti_uia_runtime_UIAPacket_NACKErrorCode_BAD_MESSAGE_LENGTH
#define UIAPacket_maxPktLengthInBytes ti_uia_runtime_UIAPacket_maxPktLengthInBytes
#define UIAPacket_swizzle ti_uia_runtime_UIAPacket_swizzle
#define UIAPacket_swizzle16 ti_uia_runtime_UIAPacket_swizzle16
#define UIAPacket_getHdrType ti_uia_runtime_UIAPacket_getHdrType
#define UIAPacket_setHdrType ti_uia_runtime_UIAPacket_setHdrType
#define UIAPacket_getPayloadEndianness ti_uia_runtime_UIAPacket_getPayloadEndianness
#define UIAPacket_setPayloadEndianness ti_uia_runtime_UIAPacket_setPayloadEndianness
#define UIAPacket_getMsgLength ti_uia_runtime_UIAPacket_getMsgLength
#define UIAPacket_setMsgLength ti_uia_runtime_UIAPacket_setMsgLength
#define UIAPacket_getEventLength ti_uia_runtime_UIAPacket_getEventLength
#define UIAPacket_getMinEventLength ti_uia_runtime_UIAPacket_getMinEventLength
#define UIAPacket_setEventLength ti_uia_runtime_UIAPacket_setEventLength
#define UIAPacket_setEventLengthFast ti_uia_runtime_UIAPacket_setEventLengthFast
#define UIAPacket_setMinEventPacketLength ti_uia_runtime_UIAPacket_setMinEventPacketLength
#define UIAPacket_getLength ti_uia_runtime_UIAPacket_getLength
#define UIAPacket_getSequenceCount ti_uia_runtime_UIAPacket_getSequenceCount
#define UIAPacket_setSequenceCount ti_uia_runtime_UIAPacket_setSequenceCount
#define UIAPacket_setSequenceCountFast ti_uia_runtime_UIAPacket_setSequenceCountFast
#define UIAPacket_setSequenceCounts ti_uia_runtime_UIAPacket_setSequenceCounts
#define UIAPacket_setMinEventPacketSequenceCount ti_uia_runtime_UIAPacket_setMinEventPacketSequenceCount
#define UIAPacket_getLoggerPriority ti_uia_runtime_UIAPacket_getLoggerPriority
#define UIAPacket_setLoggerPriority ti_uia_runtime_UIAPacket_setLoggerPriority
#define UIAPacket_getLoggerModuleId ti_uia_runtime_UIAPacket_getLoggerModuleId
#define UIAPacket_setLoggerModuleId ti_uia_runtime_UIAPacket_setLoggerModuleId
#define UIAPacket_getLoggerInstanceId ti_uia_runtime_UIAPacket_getLoggerInstanceId
#define UIAPacket_setLoggerInstanceId ti_uia_runtime_UIAPacket_setLoggerInstanceId
#define UIAPacket_getMsgType ti_uia_runtime_UIAPacket_getMsgType
#define UIAPacket_setMsgType ti_uia_runtime_UIAPacket_setMsgType
#define UIAPacket_getCmdId ti_uia_runtime_UIAPacket_getCmdId
#define UIAPacket_setCmdId ti_uia_runtime_UIAPacket_setCmdId
#define UIAPacket_getServiceId ti_uia_runtime_UIAPacket_getServiceId
#define UIAPacket_setServiceId ti_uia_runtime_UIAPacket_setServiceId
#define UIAPacket_getTag ti_uia_runtime_UIAPacket_getTag
#define UIAPacket_setTag ti_uia_runtime_UIAPacket_setTag
#define UIAPacket_getDestAdrs ti_uia_runtime_UIAPacket_getDestAdrs
#define UIAPacket_setDestAdrs ti_uia_runtime_UIAPacket_setDestAdrs
#define UIAPacket_getSenderAdrs ti_uia_runtime_UIAPacket_getSenderAdrs
#define UIAPacket_setSenderAdrs ti_uia_runtime_UIAPacket_setSenderAdrs
#define UIAPacket_initMsgHdr ti_uia_runtime_UIAPacket_initMsgHdr
#define UIAPacket_initEventRecHdr ti_uia_runtime_UIAPacket_initEventRecHdr
#define UIAPacket_initMinEventRecHdr ti_uia_runtime_UIAPacket_initMinEventRecHdr
#define UIAPacket_getFooter ti_uia_runtime_UIAPacket_getFooter
#define UIAPacket_setInvalidHdr ti_uia_runtime_UIAPacket_setInvalidHdr
#define UIAPacket_Module_name ti_uia_runtime_UIAPacket_Module_name
#define UIAPacket_Module_id ti_uia_runtime_UIAPacket_Module_id
#define UIAPacket_Module_startup ti_uia_runtime_UIAPacket_Module_startup
#define UIAPacket_Module_startupDone ti_uia_runtime_UIAPacket_Module_startupDone
#define UIAPacket_Module_hasMask ti_uia_runtime_UIAPacket_Module_hasMask
#define UIAPacket_Module_getMask ti_uia_runtime_UIAPacket_Module_getMask
#define UIAPacket_Module_setMask ti_uia_runtime_UIAPacket_Module_setMask
#define UIAPacket_Object_heap ti_uia_runtime_UIAPacket_Object_heap
#define UIAPacket_Module_heap ti_uia_runtime_UIAPacket_Module_heap

#endif /* ti_uia_runtime_UIAPacket__localnames__done */
#endif
