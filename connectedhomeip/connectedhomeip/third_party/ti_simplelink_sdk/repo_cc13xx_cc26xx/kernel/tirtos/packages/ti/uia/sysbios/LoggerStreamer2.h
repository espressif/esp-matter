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
 *     PER-INSTANCE TYPES
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

#ifndef ti_uia_sysbios_LoggerStreamer2__include
#define ti_uia_sysbios_LoggerStreamer2__include

#ifndef __nested__
#define __nested__
#define ti_uia_sysbios_LoggerStreamer2__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_sysbios_LoggerStreamer2___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/sysbios/LoggerStreamer2__prologue.h>
#include <ti/uia/sysbios/package/package.defs.h>

#include <xdc/runtime/ILogger.h>
#include <ti/uia/runtime/ILoggerSnapshot.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IFilterLogger.h>
#include <xdc/runtime/Assert.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* TransferType */
typedef ti_uia_runtime_IUIATransfer_TransferType ti_uia_sysbios_LoggerStreamer2_TransferType;

/* Priority */
typedef ti_uia_runtime_IUIATransfer_Priority ti_uia_sysbios_LoggerStreamer2_Priority;

/* TransportType */
enum ti_uia_sysbios_LoggerStreamer2_TransportType {
    ti_uia_sysbios_LoggerStreamer2_TransportType_UART = 0,
    ti_uia_sysbios_LoggerStreamer2_TransportType_USB = 1,
    ti_uia_sysbios_LoggerStreamer2_TransportType_ETHERNET = 2,
    ti_uia_sysbios_LoggerStreamer2_TransportType_CUSTOM = 3
};
typedef enum ti_uia_sysbios_LoggerStreamer2_TransportType ti_uia_sysbios_LoggerStreamer2_TransportType;

/* ExchangeFxnType */
typedef xdc_Ptr (*ti_uia_sysbios_LoggerStreamer2_ExchangeFxnType)(ti_uia_sysbios_LoggerStreamer2_Object* arg1, xdc_Ptr arg2);

/* PrimeFxnType */
typedef xdc_Ptr (*ti_uia_sysbios_LoggerStreamer2_PrimeFxnType)(ti_uia_sysbios_LoggerStreamer2_Object* arg1);

/* TransferType_RELIABLE */
#define ti_uia_sysbios_LoggerStreamer2_TransferType_RELIABLE ti_uia_runtime_IUIATransfer_TransferType_RELIABLE

/* TransferType_LOSSY */
#define ti_uia_sysbios_LoggerStreamer2_TransferType_LOSSY ti_uia_runtime_IUIATransfer_TransferType_LOSSY

/* Priority_LOW */
#define ti_uia_sysbios_LoggerStreamer2_Priority_LOW ti_uia_runtime_IUIATransfer_Priority_LOW

/* Priority_STANDARD */
#define ti_uia_sysbios_LoggerStreamer2_Priority_STANDARD ti_uia_runtime_IUIATransfer_Priority_STANDARD

/* Priority_HIGH */
#define ti_uia_sysbios_LoggerStreamer2_Priority_HIGH ti_uia_runtime_IUIATransfer_Priority_HIGH

/* Priority_SYNC */
#define ti_uia_sysbios_LoggerStreamer2_Priority_SYNC ti_uia_runtime_IUIATransfer_Priority_SYNC


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* WRITE0_SIZE_IN_BYTES */
#define ti_uia_sysbios_LoggerStreamer2_WRITE0_SIZE_IN_BYTES (8)

/* WRITE1_SIZE_IN_BYTES */
#define ti_uia_sysbios_LoggerStreamer2_WRITE1_SIZE_IN_BYTES (12)

/* WRITE2_SIZE_IN_BYTES */
#define ti_uia_sysbios_LoggerStreamer2_WRITE2_SIZE_IN_BYTES (16)

/* WRITE4_SIZE_IN_BYTES */
#define ti_uia_sysbios_LoggerStreamer2_WRITE4_SIZE_IN_BYTES (24)

/* WRITE8_SIZE_IN_BYTES */
#define ti_uia_sysbios_LoggerStreamer2_WRITE8_SIZE_IN_BYTES (40)

/* TIMESTAMP */
#define ti_uia_sysbios_LoggerStreamer2_TIMESTAMP (8)

/* NO_TIMESTAMP */
#define ti_uia_sysbios_LoggerStreamer2_NO_TIMESTAMP (0)


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled (ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded (ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__diagsMask (ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_sysbios_LoggerStreamer2_Module__gateObj;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__gateObj ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__gateObj__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__gateObj*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__gateObj (ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_sysbios_LoggerStreamer2_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__gatePrms ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__gatePrms*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__gatePrms (ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_sysbios_LoggerStreamer2_Module__id;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__id ti_uia_sysbios_LoggerStreamer2_Module__id__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__id__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__id__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__id*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__id__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__id (ti_uia_sysbios_LoggerStreamer2_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined (ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerObj ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerObj*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerObj (ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0 (ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1 (ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2 (ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4 (ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__CR
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8 (ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_sysbios_LoggerStreamer2_Object__count;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Object__count ti_uia_sysbios_LoggerStreamer2_Object__count__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Object__count__CR
#define ti_uia_sysbios_LoggerStreamer2_Object__count__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Object__count*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Object__count__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Object__count (ti_uia_sysbios_LoggerStreamer2_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_sysbios_LoggerStreamer2_Object__heap;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Object__heap ti_uia_sysbios_LoggerStreamer2_Object__heap__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Object__heap__CR
#define ti_uia_sysbios_LoggerStreamer2_Object__heap__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Object__heap*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Object__heap__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Object__heap (ti_uia_sysbios_LoggerStreamer2_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_sysbios_LoggerStreamer2_Object__sizeof;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Object__sizeof ti_uia_sysbios_LoggerStreamer2_Object__sizeof__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Object__sizeof__CR
#define ti_uia_sysbios_LoggerStreamer2_Object__sizeof__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Object__sizeof*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Object__sizeof__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Object__sizeof (ti_uia_sysbios_LoggerStreamer2_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_sysbios_LoggerStreamer2_Object__table;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_Object__table ti_uia_sysbios_LoggerStreamer2_Object__table__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_Object__table__CR
#define ti_uia_sysbios_LoggerStreamer2_Object__table__C (*((CT__ti_uia_sysbios_LoggerStreamer2_Object__table*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Object__table__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_Object__table (ti_uia_sysbios_LoggerStreamer2_Object__table__C)
#endif

/* filterByLevel */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_filterByLevel;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_filterByLevel ti_uia_sysbios_LoggerStreamer2_filterByLevel__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_filterByLevel__CR
#define ti_uia_sysbios_LoggerStreamer2_filterByLevel (*((CT__ti_uia_sysbios_LoggerStreamer2_filterByLevel*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_filterByLevel__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_filterByLevel__D
#define ti_uia_sysbios_LoggerStreamer2_filterByLevel (ti_uia_sysbios_LoggerStreamer2_filterByLevel__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_filterByLevel (ti_uia_sysbios_LoggerStreamer2_filterByLevel__C)
#endif
#endif

/* customTransportType */
typedef xdc_String CT__ti_uia_sysbios_LoggerStreamer2_customTransportType;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_customTransportType ti_uia_sysbios_LoggerStreamer2_customTransportType__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_customTransportType__CR
#define ti_uia_sysbios_LoggerStreamer2_customTransportType (*((CT__ti_uia_sysbios_LoggerStreamer2_customTransportType*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_customTransportType__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_customTransportType (ti_uia_sysbios_LoggerStreamer2_customTransportType__C)
#endif

/* isTimestampEnabled */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__CR
#define ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled (*((CT__ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__D
#define ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled (ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled (ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled__C)
#endif
#endif

/* isBadPacketDetectionEnabled */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__CR
#define ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled (*((CT__ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__D
#define ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled (ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled (ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled__C)
#endif
#endif

/* supportLoggerDisable */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__CR
#define ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable (*((CT__ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__D
#define ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable (ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable (ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable__C)
#endif
#endif

/* testForNullWrPtr */
typedef xdc_Bool CT__ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__CR
#define ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr (*((CT__ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__D
#define ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr (ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr (ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr__C)
#endif
#endif

/* statusLogger */
typedef xdc_runtime_IFilterLogger_Handle CT__ti_uia_sysbios_LoggerStreamer2_statusLogger;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_statusLogger ti_uia_sysbios_LoggerStreamer2_statusLogger__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_statusLogger__CR
#define ti_uia_sysbios_LoggerStreamer2_statusLogger (*((CT__ti_uia_sysbios_LoggerStreamer2_statusLogger*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_statusLogger__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_statusLogger (ti_uia_sysbios_LoggerStreamer2_statusLogger__C)
#endif

/* level1Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_sysbios_LoggerStreamer2_level1Mask;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_level1Mask ti_uia_sysbios_LoggerStreamer2_level1Mask__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_level1Mask__CR
#define ti_uia_sysbios_LoggerStreamer2_level1Mask (*((CT__ti_uia_sysbios_LoggerStreamer2_level1Mask*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_level1Mask__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_level1Mask__D
#define ti_uia_sysbios_LoggerStreamer2_level1Mask (ti_uia_sysbios_LoggerStreamer2_level1Mask__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_level1Mask (ti_uia_sysbios_LoggerStreamer2_level1Mask__C)
#endif
#endif

/* level2Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_sysbios_LoggerStreamer2_level2Mask;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_level2Mask ti_uia_sysbios_LoggerStreamer2_level2Mask__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_level2Mask__CR
#define ti_uia_sysbios_LoggerStreamer2_level2Mask (*((CT__ti_uia_sysbios_LoggerStreamer2_level2Mask*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_level2Mask__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_level2Mask__D
#define ti_uia_sysbios_LoggerStreamer2_level2Mask (ti_uia_sysbios_LoggerStreamer2_level2Mask__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_level2Mask (ti_uia_sysbios_LoggerStreamer2_level2Mask__C)
#endif
#endif

/* level3Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_sysbios_LoggerStreamer2_level3Mask;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_level3Mask ti_uia_sysbios_LoggerStreamer2_level3Mask__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_level3Mask__CR
#define ti_uia_sysbios_LoggerStreamer2_level3Mask (*((CT__ti_uia_sysbios_LoggerStreamer2_level3Mask*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_level3Mask__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_level3Mask__D
#define ti_uia_sysbios_LoggerStreamer2_level3Mask (ti_uia_sysbios_LoggerStreamer2_level3Mask__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_level3Mask (ti_uia_sysbios_LoggerStreamer2_level3Mask__C)
#endif
#endif

/* level4Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_sysbios_LoggerStreamer2_level4Mask;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_level4Mask ti_uia_sysbios_LoggerStreamer2_level4Mask__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_level4Mask__CR
#define ti_uia_sysbios_LoggerStreamer2_level4Mask (*((CT__ti_uia_sysbios_LoggerStreamer2_level4Mask*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_level4Mask__C_offset)))
#else
#ifdef ti_uia_sysbios_LoggerStreamer2_level4Mask__D
#define ti_uia_sysbios_LoggerStreamer2_level4Mask (ti_uia_sysbios_LoggerStreamer2_level4Mask__D)
#else
#define ti_uia_sysbios_LoggerStreamer2_level4Mask (ti_uia_sysbios_LoggerStreamer2_level4Mask__C)
#endif
#endif

/* L_test */
typedef xdc_runtime_Log_Event CT__ti_uia_sysbios_LoggerStreamer2_L_test;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_L_test ti_uia_sysbios_LoggerStreamer2_L_test__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_L_test__CR
#define ti_uia_sysbios_LoggerStreamer2_L_test (*((CT__ti_uia_sysbios_LoggerStreamer2_L_test*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_L_test__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_L_test (ti_uia_sysbios_LoggerStreamer2_L_test__C)
#endif

/* E_badLevel */
typedef xdc_runtime_Error_Id CT__ti_uia_sysbios_LoggerStreamer2_E_badLevel;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_E_badLevel ti_uia_sysbios_LoggerStreamer2_E_badLevel__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_E_badLevel__CR
#define ti_uia_sysbios_LoggerStreamer2_E_badLevel (*((CT__ti_uia_sysbios_LoggerStreamer2_E_badLevel*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_E_badLevel__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_E_badLevel (ti_uia_sysbios_LoggerStreamer2_E_badLevel__C)
#endif

/* A_invalidBuffer */
typedef xdc_runtime_Assert_Id CT__ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer;
__extern __FAR__ const CT__ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer__C;
#ifdef ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer__CR
#define ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer (*((CT__ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer*)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer__C_offset)))
#else
#define ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer (ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_sysbios_LoggerStreamer2_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    ti_uia_runtime_IUIATransfer_TransferType transferType;
    ti_uia_runtime_IUIATransfer_Priority priority;
    xdc_UArg context;
    ti_uia_sysbios_LoggerStreamer2_PrimeFxnType primeFxn;
    ti_uia_sysbios_LoggerStreamer2_ExchangeFxnType exchangeFxn;
    xdc_Int16 instanceId;
    xdc_SizeT maxEventSize;
    xdc_SizeT bufSize;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_uia_sysbios_LoggerStreamer2_Struct {
    const ti_uia_sysbios_LoggerStreamer2_Fxns__ *__fxns;
    xdc_Bool f0;
    xdc_Int16 f1;
    ti_uia_sysbios_LoggerStreamer2_PrimeFxnType f2;
    ti_uia_sysbios_LoggerStreamer2_ExchangeFxnType f3;
    xdc_UArg f4;
    xdc_Bool f5;
    xdc_UInt32 f6;
    xdc_UArg *f7;
    xdc_UArg *f8;
    xdc_UArg *f9;
    xdc_SizeT f10;
    xdc_SizeT f11;
    xdc_Int f12;
    xdc_UInt16 f13;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_sysbios_LoggerStreamer2_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_Bool (*disable)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_Void (*write0)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
    xdc_Void (*write1)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
    xdc_Void (*write2)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
    xdc_Void (*write4)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
    xdc_Void (*write8)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
    xdc_Void (*setFilterLevel)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
    xdc_runtime_Diags_Mask (*getFilterLevel)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Diags_EventLevel level);
    ti_uia_runtime_IUIATransfer_TransferType (*getTransferType)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_Bool (*getContents)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT* cpSize);
    xdc_Bool (*isEmpty)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_SizeT (*getMaxLength)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_UInt16 (*getInstanceId)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    ti_uia_runtime_IUIATransfer_Priority (*getPriority)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_Void (*setPriority)(ti_uia_sysbios_LoggerStreamer2_Handle inst, ti_uia_runtime_IUIATransfer_Priority priority);
    xdc_Void (*reset)(ti_uia_sysbios_LoggerStreamer2_Handle inst);
    xdc_Void (*writeMemoryRange)(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_sysbios_LoggerStreamer2_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_sysbios_LoggerStreamer2_Fxns__ ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C;
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_sysbios_LoggerStreamer2_Module_startup ti_uia_sysbios_LoggerStreamer2_Module_startup__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Module_startup__E, "ti_uia_sysbios_LoggerStreamer2_Module_startup")
__extern xdc_Int ti_uia_sysbios_LoggerStreamer2_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Module_startup__F, "ti_uia_sysbios_LoggerStreamer2_Module_startup")
__extern xdc_Int ti_uia_sysbios_LoggerStreamer2_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Instance_init__E, "ti_uia_sysbios_LoggerStreamer2_Instance_init")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_Instance_init__E(ti_uia_sysbios_LoggerStreamer2_Object *obj, const ti_uia_sysbios_LoggerStreamer2_Params *prms);

/* create */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_create, "ti_uia_sysbios_LoggerStreamer2_create")
__extern ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_create( const ti_uia_sysbios_LoggerStreamer2_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_construct, "ti_uia_sysbios_LoggerStreamer2_construct")
__extern void ti_uia_sysbios_LoggerStreamer2_construct(ti_uia_sysbios_LoggerStreamer2_Struct *obj, const ti_uia_sysbios_LoggerStreamer2_Params *prms);

/* delete */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_delete, "ti_uia_sysbios_LoggerStreamer2_delete")
__extern void ti_uia_sysbios_LoggerStreamer2_delete(ti_uia_sysbios_LoggerStreamer2_Handle *instp);

/* destruct */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_destruct, "ti_uia_sysbios_LoggerStreamer2_destruct")
__extern void ti_uia_sysbios_LoggerStreamer2_destruct(ti_uia_sysbios_LoggerStreamer2_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Handle__label__S, "ti_uia_sysbios_LoggerStreamer2_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_sysbios_LoggerStreamer2_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Module__startupDone__S, "ti_uia_sysbios_LoggerStreamer2_Module__startupDone__S")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Object__create__S, "ti_uia_sysbios_LoggerStreamer2_Object__create__S")
__extern xdc_Ptr ti_uia_sysbios_LoggerStreamer2_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Object__delete__S, "ti_uia_sysbios_LoggerStreamer2_Object__delete__S")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Object__get__S, "ti_uia_sysbios_LoggerStreamer2_Object__get__S")
__extern xdc_Ptr ti_uia_sysbios_LoggerStreamer2_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Object__first__S, "ti_uia_sysbios_LoggerStreamer2_Object__first__S")
__extern xdc_Ptr ti_uia_sysbios_LoggerStreamer2_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Object__next__S, "ti_uia_sysbios_LoggerStreamer2_Object__next__S")
__extern xdc_Ptr ti_uia_sysbios_LoggerStreamer2_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_Params__init__S, "ti_uia_sysbios_LoggerStreamer2_Params__init__S")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* enable__E */
#define ti_uia_sysbios_LoggerStreamer2_enable ti_uia_sysbios_LoggerStreamer2_enable__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_enable__E, "ti_uia_sysbios_LoggerStreamer2_enable")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_enable__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* disable__E */
#define ti_uia_sysbios_LoggerStreamer2_disable ti_uia_sysbios_LoggerStreamer2_disable__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_disable__E, "ti_uia_sysbios_LoggerStreamer2_disable")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_disable__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getTransferType__E */
#define ti_uia_sysbios_LoggerStreamer2_getTransferType ti_uia_sysbios_LoggerStreamer2_getTransferType__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getTransferType__E, "ti_uia_sysbios_LoggerStreamer2_getTransferType")
__extern ti_uia_runtime_IUIATransfer_TransferType ti_uia_sysbios_LoggerStreamer2_getTransferType__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getContents__E */
#define ti_uia_sysbios_LoggerStreamer2_getContents ti_uia_sysbios_LoggerStreamer2_getContents__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getContents__E, "ti_uia_sysbios_LoggerStreamer2_getContents")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_getContents__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT *cpSize);

/* isEmpty__E */
#define ti_uia_sysbios_LoggerStreamer2_isEmpty ti_uia_sysbios_LoggerStreamer2_isEmpty__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_isEmpty__E, "ti_uia_sysbios_LoggerStreamer2_isEmpty")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_isEmpty__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getMaxLength__E */
#define ti_uia_sysbios_LoggerStreamer2_getMaxLength ti_uia_sysbios_LoggerStreamer2_getMaxLength__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getMaxLength__E, "ti_uia_sysbios_LoggerStreamer2_getMaxLength")
__extern xdc_SizeT ti_uia_sysbios_LoggerStreamer2_getMaxLength__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getInstanceId__E */
#define ti_uia_sysbios_LoggerStreamer2_getInstanceId ti_uia_sysbios_LoggerStreamer2_getInstanceId__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getInstanceId__E, "ti_uia_sysbios_LoggerStreamer2_getInstanceId")
__extern xdc_UInt16 ti_uia_sysbios_LoggerStreamer2_getInstanceId__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getPriority__E */
#define ti_uia_sysbios_LoggerStreamer2_getPriority ti_uia_sysbios_LoggerStreamer2_getPriority__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getPriority__E, "ti_uia_sysbios_LoggerStreamer2_getPriority")
__extern ti_uia_runtime_IUIATransfer_Priority ti_uia_sysbios_LoggerStreamer2_getPriority__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* setPriority__E */
#define ti_uia_sysbios_LoggerStreamer2_setPriority ti_uia_sysbios_LoggerStreamer2_setPriority__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_setPriority__E, "ti_uia_sysbios_LoggerStreamer2_setPriority")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_setPriority__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, ti_uia_runtime_IUIATransfer_Priority priority);

/* reset__E */
#define ti_uia_sysbios_LoggerStreamer2_reset ti_uia_sysbios_LoggerStreamer2_reset__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_reset__E, "ti_uia_sysbios_LoggerStreamer2_reset")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_reset__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* writeMemoryRange__E */
#define ti_uia_sysbios_LoggerStreamer2_writeMemoryRange ti_uia_sysbios_LoggerStreamer2_writeMemoryRange__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_writeMemoryRange__E, "ti_uia_sysbios_LoggerStreamer2_writeMemoryRange")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_writeMemoryRange__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);

/* setModuleIdToRouteToStatusLogger__E */
#define ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E, "ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E( xdc_runtime_Types_ModuleId mid);

/* initBuffer__E */
#define ti_uia_sysbios_LoggerStreamer2_initBuffer ti_uia_sysbios_LoggerStreamer2_initBuffer__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_initBuffer__E, "ti_uia_sysbios_LoggerStreamer2_initBuffer")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_initBuffer__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_Ptr buffer, xdc_UInt16 src);

/* flush__E */
#define ti_uia_sysbios_LoggerStreamer2_flush ti_uia_sysbios_LoggerStreamer2_flush__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_flush__E, "ti_uia_sysbios_LoggerStreamer2_flush")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_flush__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* prime__E */
#define ti_uia_sysbios_LoggerStreamer2_prime ti_uia_sysbios_LoggerStreamer2_prime__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_prime__E, "ti_uia_sysbios_LoggerStreamer2_prime")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_prime__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_Ptr buffer);

/* validatePacket__E */
#define ti_uia_sysbios_LoggerStreamer2_validatePacket ti_uia_sysbios_LoggerStreamer2_validatePacket__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_validatePacket__E, "ti_uia_sysbios_LoggerStreamer2_validatePacket")
__extern xdc_Char *ti_uia_sysbios_LoggerStreamer2_validatePacket__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_UInt32 *writePtr, xdc_UInt32 numBytesInPacket);

/* write0__E */
#define ti_uia_sysbios_LoggerStreamer2_write0 ti_uia_sysbios_LoggerStreamer2_write0__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_write0__E, "ti_uia_sysbios_LoggerStreamer2_write0")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_write0__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);

/* write1__E */
#define ti_uia_sysbios_LoggerStreamer2_write1 ti_uia_sysbios_LoggerStreamer2_write1__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_write1__E, "ti_uia_sysbios_LoggerStreamer2_write1")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_write1__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);

/* write2__E */
#define ti_uia_sysbios_LoggerStreamer2_write2 ti_uia_sysbios_LoggerStreamer2_write2__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_write2__E, "ti_uia_sysbios_LoggerStreamer2_write2")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_write2__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);

/* write4__E */
#define ti_uia_sysbios_LoggerStreamer2_write4 ti_uia_sysbios_LoggerStreamer2_write4__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_write4__E, "ti_uia_sysbios_LoggerStreamer2_write4")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_write4__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);

/* write8__E */
#define ti_uia_sysbios_LoggerStreamer2_write8 ti_uia_sysbios_LoggerStreamer2_write8__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_write8__E, "ti_uia_sysbios_LoggerStreamer2_write8")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_write8__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);

/* setFilterLevel__E */
#define ti_uia_sysbios_LoggerStreamer2_setFilterLevel ti_uia_sysbios_LoggerStreamer2_setFilterLevel__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_setFilterLevel__E, "ti_uia_sysbios_LoggerStreamer2_setFilterLevel")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_setFilterLevel__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);

/* getFilterLevel__E */
#define ti_uia_sysbios_LoggerStreamer2_getFilterLevel ti_uia_sysbios_LoggerStreamer2_getFilterLevel__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getFilterLevel__E, "ti_uia_sysbios_LoggerStreamer2_getFilterLevel")
__extern xdc_runtime_Diags_Mask ti_uia_sysbios_LoggerStreamer2_getFilterLevel__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_runtime_Diags_EventLevel level);

/* getBufSize__E */
#define ti_uia_sysbios_LoggerStreamer2_getBufSize ti_uia_sysbios_LoggerStreamer2_getBufSize__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getBufSize__E, "ti_uia_sysbios_LoggerStreamer2_getBufSize")
__extern xdc_SizeT ti_uia_sysbios_LoggerStreamer2_getBufSize__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* getContext__E */
#define ti_uia_sysbios_LoggerStreamer2_getContext ti_uia_sysbios_LoggerStreamer2_getContext__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_getContext__E, "ti_uia_sysbios_LoggerStreamer2_getContext")
__extern xdc_UArg ti_uia_sysbios_LoggerStreamer2_getContext__E( ti_uia_sysbios_LoggerStreamer2_Handle instp);

/* setContext__E */
#define ti_uia_sysbios_LoggerStreamer2_setContext ti_uia_sysbios_LoggerStreamer2_setContext__E
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_setContext__E, "ti_uia_sysbios_LoggerStreamer2_setContext")
__extern xdc_Void ti_uia_sysbios_LoggerStreamer2_setContext__E( ti_uia_sysbios_LoggerStreamer2_Handle instp, xdc_UArg context);

/* filterOutEvent__I */
#define ti_uia_sysbios_LoggerStreamer2_filterOutEvent ti_uia_sysbios_LoggerStreamer2_filterOutEvent__I
xdc__CODESECT(ti_uia_sysbios_LoggerStreamer2_filterOutEvent__I, "ti_uia_sysbios_LoggerStreamer2_filterOutEvent")
__extern xdc_Bool ti_uia_sysbios_LoggerStreamer2_filterOutEvent__I( xdc_runtime_Diags_Mask mask);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast(void);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast(void)
{
    return (ti_uia_runtime_ILoggerSnapshot_Module)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C;
}

/* Module_to_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_sysbios_LoggerStreamer2_Module_to_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Module_upCast

/* Handle_upCast */
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast(ti_uia_sysbios_LoggerStreamer2_Handle i);
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast(ti_uia_sysbios_LoggerStreamer2_Handle i)
{
    return (ti_uia_runtime_ILoggerSnapshot_Handle)i;
}

/* Handle_to_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_sysbios_LoggerStreamer2_Handle_to_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Handle_upCast

/* Handle_downCast */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast(ti_uia_runtime_ILoggerSnapshot_Handle i);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast(ti_uia_runtime_ILoggerSnapshot_Handle i)
{
    ti_uia_runtime_ILoggerSnapshot_Handle i2 = (ti_uia_runtime_ILoggerSnapshot_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C) ? (ti_uia_sysbios_LoggerStreamer2_Handle)i : (ti_uia_sysbios_LoggerStreamer2_Handle)NULL;
}

/* Handle_from_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_sysbios_LoggerStreamer2_Handle_from_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Handle_downCast

/* Module_upCast2 */
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast2(void);
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast2(void)
{
    return (ti_uia_runtime_IUIATransfer_Module)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C;
}

/* Module_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_sysbios_LoggerStreamer2_Module_to_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Module_upCast2

/* Handle_upCast2 */
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast2(ti_uia_sysbios_LoggerStreamer2_Handle i);
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast2(ti_uia_sysbios_LoggerStreamer2_Handle i)
{
    return (ti_uia_runtime_IUIATransfer_Handle)i;
}

/* Handle_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_sysbios_LoggerStreamer2_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Handle_upCast2

/* Handle_downCast2 */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast2(ti_uia_runtime_IUIATransfer_Handle i);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast2(ti_uia_runtime_IUIATransfer_Handle i)
{
    ti_uia_runtime_IUIATransfer_Handle i2 = (ti_uia_runtime_IUIATransfer_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C) ? (ti_uia_sysbios_LoggerStreamer2_Handle)i : (ti_uia_sysbios_LoggerStreamer2_Handle)NULL;
}

/* Handle_from_ti_uia_runtime_IUIATransfer */
#define ti_uia_sysbios_LoggerStreamer2_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Handle_downCast2

/* Module_upCast3 */
static inline xdc_runtime_IFilterLogger_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast3(void);
static inline xdc_runtime_IFilterLogger_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast3(void)
{
    return (xdc_runtime_IFilterLogger_Module)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IFilterLogger */
#define ti_uia_sysbios_LoggerStreamer2_Module_to_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Module_upCast3

/* Handle_upCast3 */
static inline xdc_runtime_IFilterLogger_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast3(ti_uia_sysbios_LoggerStreamer2_Handle i);
static inline xdc_runtime_IFilterLogger_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast3(ti_uia_sysbios_LoggerStreamer2_Handle i)
{
    return (xdc_runtime_IFilterLogger_Handle)i;
}

/* Handle_to_xdc_runtime_IFilterLogger */
#define ti_uia_sysbios_LoggerStreamer2_Handle_to_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Handle_upCast3

/* Handle_downCast3 */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast3(xdc_runtime_IFilterLogger_Handle i);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast3(xdc_runtime_IFilterLogger_Handle i)
{
    xdc_runtime_IFilterLogger_Handle i2 = (xdc_runtime_IFilterLogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C) ? (ti_uia_sysbios_LoggerStreamer2_Handle)i : (ti_uia_sysbios_LoggerStreamer2_Handle)NULL;
}

/* Handle_from_xdc_runtime_IFilterLogger */
#define ti_uia_sysbios_LoggerStreamer2_Handle_from_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Handle_downCast3

/* Module_upCast4 */
static inline xdc_runtime_ILogger_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast4(void);
static inline xdc_runtime_ILogger_Module ti_uia_sysbios_LoggerStreamer2_Module_upCast4(void)
{
    return (xdc_runtime_ILogger_Module)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_uia_sysbios_LoggerStreamer2_Module_to_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Module_upCast4

/* Handle_upCast4 */
static inline xdc_runtime_ILogger_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast4(ti_uia_sysbios_LoggerStreamer2_Handle i);
static inline xdc_runtime_ILogger_Handle ti_uia_sysbios_LoggerStreamer2_Handle_upCast4(ti_uia_sysbios_LoggerStreamer2_Handle i)
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_uia_sysbios_LoggerStreamer2_Handle_to_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Handle_upCast4

/* Handle_downCast4 */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast4(xdc_runtime_ILogger_Handle i);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle_downCast4(xdc_runtime_ILogger_Handle i)
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_sysbios_LoggerStreamer2_Module__FXNS__C) ? (ti_uia_sysbios_LoggerStreamer2_Handle)i : (ti_uia_sysbios_LoggerStreamer2_Handle)NULL;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_uia_sysbios_LoggerStreamer2_Handle_from_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Handle_downCast4


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_sysbios_LoggerStreamer2_Module_startupDone() ti_uia_sysbios_LoggerStreamer2_Module__startupDone__S()

/* Object_heap */
#define ti_uia_sysbios_LoggerStreamer2_Object_heap() ti_uia_sysbios_LoggerStreamer2_Object__heap__C

/* Module_heap */
#define ti_uia_sysbios_LoggerStreamer2_Module_heap() ti_uia_sysbios_LoggerStreamer2_Object__heap__C

/* Module_id */
static inline CT__ti_uia_sysbios_LoggerStreamer2_Module__id ti_uia_sysbios_LoggerStreamer2_Module_id(void);
static inline CT__ti_uia_sysbios_LoggerStreamer2_Module__id ti_uia_sysbios_LoggerStreamer2_Module_id( void ) 
{
    return ti_uia_sysbios_LoggerStreamer2_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_sysbios_LoggerStreamer2_Module_hasMask(void);
static inline xdc_Bool ti_uia_sysbios_LoggerStreamer2_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C != (CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_sysbios_LoggerStreamer2_Module_getMask(void);
static inline xdc_Bits16 ti_uia_sysbios_LoggerStreamer2_Module_getMask(void)
{
    return (ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C != (CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask)NULL) ? *ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_sysbios_LoggerStreamer2_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_sysbios_LoggerStreamer2_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C != (CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask)NULL) {
        *ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_uia_sysbios_LoggerStreamer2_Params_init(ti_uia_sysbios_LoggerStreamer2_Params *prms);
static inline void ti_uia_sysbios_LoggerStreamer2_Params_init( ti_uia_sysbios_LoggerStreamer2_Params *prms ) 
{
    if (prms != NULL) {
        ti_uia_sysbios_LoggerStreamer2_Params__init__S(prms, NULL, sizeof(ti_uia_sysbios_LoggerStreamer2_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_uia_sysbios_LoggerStreamer2_Params_copy(ti_uia_sysbios_LoggerStreamer2_Params *dst, const ti_uia_sysbios_LoggerStreamer2_Params *src);
static inline void ti_uia_sysbios_LoggerStreamer2_Params_copy(ti_uia_sysbios_LoggerStreamer2_Params *dst, const ti_uia_sysbios_LoggerStreamer2_Params *src) 
{
    if (dst != NULL) {
        ti_uia_sysbios_LoggerStreamer2_Params__init__S(dst, (const void *)src, sizeof(ti_uia_sysbios_LoggerStreamer2_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_uia_sysbios_LoggerStreamer2_Object_count() ti_uia_sysbios_LoggerStreamer2_Object__count__C

/* Object_sizeof */
#define ti_uia_sysbios_LoggerStreamer2_Object_sizeof() ti_uia_sysbios_LoggerStreamer2_Object__sizeof__C

/* Object_get */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_get(ti_uia_sysbios_LoggerStreamer2_Object *oarr, int i);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_get(ti_uia_sysbios_LoggerStreamer2_Object *oarr, int i) 
{
    return (ti_uia_sysbios_LoggerStreamer2_Handle)ti_uia_sysbios_LoggerStreamer2_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_first(void);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_first(void)
{
    return (ti_uia_sysbios_LoggerStreamer2_Handle)ti_uia_sysbios_LoggerStreamer2_Object__first__S();
}

/* Object_next */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_next(ti_uia_sysbios_LoggerStreamer2_Object *obj);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Object_next(ti_uia_sysbios_LoggerStreamer2_Object *obj)
{
    return (ti_uia_sysbios_LoggerStreamer2_Handle)ti_uia_sysbios_LoggerStreamer2_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_sysbios_LoggerStreamer2_Handle_label(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_sysbios_LoggerStreamer2_Handle_label(ti_uia_sysbios_LoggerStreamer2_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_uia_sysbios_LoggerStreamer2_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_uia_sysbios_LoggerStreamer2_Handle_name(ti_uia_sysbios_LoggerStreamer2_Handle inst);
static inline xdc_String ti_uia_sysbios_LoggerStreamer2_Handle_name(ti_uia_sysbios_LoggerStreamer2_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_uia_sysbios_LoggerStreamer2_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_handle(ti_uia_sysbios_LoggerStreamer2_Struct *str);
static inline ti_uia_sysbios_LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_handle(ti_uia_sysbios_LoggerStreamer2_Struct *str)
{
    return (ti_uia_sysbios_LoggerStreamer2_Handle)str;
}

/* struct */
static inline ti_uia_sysbios_LoggerStreamer2_Struct *ti_uia_sysbios_LoggerStreamer2_struct(ti_uia_sysbios_LoggerStreamer2_Handle inst);
static inline ti_uia_sysbios_LoggerStreamer2_Struct *ti_uia_sysbios_LoggerStreamer2_struct(ti_uia_sysbios_LoggerStreamer2_Handle inst)
{
    return (ti_uia_sysbios_LoggerStreamer2_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/sysbios/LoggerStreamer2__epilogue.h>

#ifdef ti_uia_sysbios_LoggerStreamer2__top__
#undef __nested__
#endif

#endif /* ti_uia_sysbios_LoggerStreamer2__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_sysbios_LoggerStreamer2__internalaccess))

#ifndef ti_uia_sysbios_LoggerStreamer2__include_state
#define ti_uia_sysbios_LoggerStreamer2__include_state

/* Module_State */
struct ti_uia_sysbios_LoggerStreamer2_Module_State {
    xdc_Bool enabled;
    xdc_runtime_Diags_Mask level1;
    xdc_runtime_Diags_Mask level2;
    xdc_runtime_Diags_Mask level3;
    xdc_runtime_Types_ModuleId moduleIdToRouteToStatusLogger;
};

/* Module__state__V */
#ifndef ti_uia_sysbios_LoggerStreamer2_Module__state__VR
extern struct ti_uia_sysbios_LoggerStreamer2_Module_State__ ti_uia_sysbios_LoggerStreamer2_Module__state__V;
#else
#define ti_uia_sysbios_LoggerStreamer2_Module__state__V (*((struct ti_uia_sysbios_LoggerStreamer2_Module_State__*)(xdcRomStatePtr + ti_uia_sysbios_LoggerStreamer2_Module__state__V_offset)))
#endif

/* Object */
struct ti_uia_sysbios_LoggerStreamer2_Object {
    const ti_uia_sysbios_LoggerStreamer2_Fxns__ *__fxns;
    xdc_Bool enabled;
    xdc_Int16 instanceId;
    ti_uia_sysbios_LoggerStreamer2_PrimeFxnType primeFxn;
    ti_uia_sysbios_LoggerStreamer2_ExchangeFxnType exchangeFxn;
    xdc_UArg context;
    xdc_Bool primeStatus;
    xdc_UInt32 bufSize;
    xdc_UArg *buffer;
    xdc_UArg *write;
    xdc_UArg *end;
    xdc_SizeT maxEventSizeUArg;
    xdc_SizeT maxEventSize;
    xdc_Int droppedEvents;
    xdc_UInt16 seqNumber;
};

#endif /* ti_uia_sysbios_LoggerStreamer2__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_sysbios_LoggerStreamer2__nolocalnames)

#ifndef ti_uia_sysbios_LoggerStreamer2__localnames__done
#define ti_uia_sysbios_LoggerStreamer2__localnames__done

/* module prefix */
#define LoggerStreamer2_Instance ti_uia_sysbios_LoggerStreamer2_Instance
#define LoggerStreamer2_Handle ti_uia_sysbios_LoggerStreamer2_Handle
#define LoggerStreamer2_Module ti_uia_sysbios_LoggerStreamer2_Module
#define LoggerStreamer2_Object ti_uia_sysbios_LoggerStreamer2_Object
#define LoggerStreamer2_Struct ti_uia_sysbios_LoggerStreamer2_Struct
#define LoggerStreamer2_TransferType ti_uia_sysbios_LoggerStreamer2_TransferType
#define LoggerStreamer2_Priority ti_uia_sysbios_LoggerStreamer2_Priority
#define LoggerStreamer2_TransportType ti_uia_sysbios_LoggerStreamer2_TransportType
#define LoggerStreamer2_ExchangeFxnType ti_uia_sysbios_LoggerStreamer2_ExchangeFxnType
#define LoggerStreamer2_PrimeFxnType ti_uia_sysbios_LoggerStreamer2_PrimeFxnType
#define LoggerStreamer2_WRITE0_SIZE_IN_BYTES ti_uia_sysbios_LoggerStreamer2_WRITE0_SIZE_IN_BYTES
#define LoggerStreamer2_WRITE1_SIZE_IN_BYTES ti_uia_sysbios_LoggerStreamer2_WRITE1_SIZE_IN_BYTES
#define LoggerStreamer2_WRITE2_SIZE_IN_BYTES ti_uia_sysbios_LoggerStreamer2_WRITE2_SIZE_IN_BYTES
#define LoggerStreamer2_WRITE4_SIZE_IN_BYTES ti_uia_sysbios_LoggerStreamer2_WRITE4_SIZE_IN_BYTES
#define LoggerStreamer2_WRITE8_SIZE_IN_BYTES ti_uia_sysbios_LoggerStreamer2_WRITE8_SIZE_IN_BYTES
#define LoggerStreamer2_TIMESTAMP ti_uia_sysbios_LoggerStreamer2_TIMESTAMP
#define LoggerStreamer2_NO_TIMESTAMP ti_uia_sysbios_LoggerStreamer2_NO_TIMESTAMP
#define LoggerStreamer2_Module_State ti_uia_sysbios_LoggerStreamer2_Module_State
#define LoggerStreamer2_Instance_State ti_uia_sysbios_LoggerStreamer2_Instance_State
#define LoggerStreamer2_TransferType_RELIABLE ti_uia_sysbios_LoggerStreamer2_TransferType_RELIABLE
#define LoggerStreamer2_TransferType_LOSSY ti_uia_sysbios_LoggerStreamer2_TransferType_LOSSY
#define LoggerStreamer2_Priority_LOW ti_uia_sysbios_LoggerStreamer2_Priority_LOW
#define LoggerStreamer2_Priority_STANDARD ti_uia_sysbios_LoggerStreamer2_Priority_STANDARD
#define LoggerStreamer2_Priority_HIGH ti_uia_sysbios_LoggerStreamer2_Priority_HIGH
#define LoggerStreamer2_Priority_SYNC ti_uia_sysbios_LoggerStreamer2_Priority_SYNC
#define LoggerStreamer2_TransportType_UART ti_uia_sysbios_LoggerStreamer2_TransportType_UART
#define LoggerStreamer2_TransportType_USB ti_uia_sysbios_LoggerStreamer2_TransportType_USB
#define LoggerStreamer2_TransportType_ETHERNET ti_uia_sysbios_LoggerStreamer2_TransportType_ETHERNET
#define LoggerStreamer2_TransportType_CUSTOM ti_uia_sysbios_LoggerStreamer2_TransportType_CUSTOM
#define LoggerStreamer2_filterByLevel ti_uia_sysbios_LoggerStreamer2_filterByLevel
#define LoggerStreamer2_customTransportType ti_uia_sysbios_LoggerStreamer2_customTransportType
#define LoggerStreamer2_isTimestampEnabled ti_uia_sysbios_LoggerStreamer2_isTimestampEnabled
#define LoggerStreamer2_isBadPacketDetectionEnabled ti_uia_sysbios_LoggerStreamer2_isBadPacketDetectionEnabled
#define LoggerStreamer2_supportLoggerDisable ti_uia_sysbios_LoggerStreamer2_supportLoggerDisable
#define LoggerStreamer2_testForNullWrPtr ti_uia_sysbios_LoggerStreamer2_testForNullWrPtr
#define LoggerStreamer2_statusLogger ti_uia_sysbios_LoggerStreamer2_statusLogger
#define LoggerStreamer2_level1Mask ti_uia_sysbios_LoggerStreamer2_level1Mask
#define LoggerStreamer2_level2Mask ti_uia_sysbios_LoggerStreamer2_level2Mask
#define LoggerStreamer2_level3Mask ti_uia_sysbios_LoggerStreamer2_level3Mask
#define LoggerStreamer2_level4Mask ti_uia_sysbios_LoggerStreamer2_level4Mask
#define LoggerStreamer2_L_test ti_uia_sysbios_LoggerStreamer2_L_test
#define LoggerStreamer2_E_badLevel ti_uia_sysbios_LoggerStreamer2_E_badLevel
#define LoggerStreamer2_A_invalidBuffer ti_uia_sysbios_LoggerStreamer2_A_invalidBuffer
#define LoggerStreamer2_Params ti_uia_sysbios_LoggerStreamer2_Params
#define LoggerStreamer2_enable ti_uia_sysbios_LoggerStreamer2_enable
#define LoggerStreamer2_disable ti_uia_sysbios_LoggerStreamer2_disable
#define LoggerStreamer2_getTransferType ti_uia_sysbios_LoggerStreamer2_getTransferType
#define LoggerStreamer2_getContents ti_uia_sysbios_LoggerStreamer2_getContents
#define LoggerStreamer2_isEmpty ti_uia_sysbios_LoggerStreamer2_isEmpty
#define LoggerStreamer2_getMaxLength ti_uia_sysbios_LoggerStreamer2_getMaxLength
#define LoggerStreamer2_getInstanceId ti_uia_sysbios_LoggerStreamer2_getInstanceId
#define LoggerStreamer2_getPriority ti_uia_sysbios_LoggerStreamer2_getPriority
#define LoggerStreamer2_setPriority ti_uia_sysbios_LoggerStreamer2_setPriority
#define LoggerStreamer2_reset ti_uia_sysbios_LoggerStreamer2_reset
#define LoggerStreamer2_writeMemoryRange ti_uia_sysbios_LoggerStreamer2_writeMemoryRange
#define LoggerStreamer2_setModuleIdToRouteToStatusLogger ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger
#define LoggerStreamer2_initBuffer ti_uia_sysbios_LoggerStreamer2_initBuffer
#define LoggerStreamer2_flush ti_uia_sysbios_LoggerStreamer2_flush
#define LoggerStreamer2_prime ti_uia_sysbios_LoggerStreamer2_prime
#define LoggerStreamer2_validatePacket ti_uia_sysbios_LoggerStreamer2_validatePacket
#define LoggerStreamer2_write0 ti_uia_sysbios_LoggerStreamer2_write0
#define LoggerStreamer2_write1 ti_uia_sysbios_LoggerStreamer2_write1
#define LoggerStreamer2_write2 ti_uia_sysbios_LoggerStreamer2_write2
#define LoggerStreamer2_write4 ti_uia_sysbios_LoggerStreamer2_write4
#define LoggerStreamer2_write8 ti_uia_sysbios_LoggerStreamer2_write8
#define LoggerStreamer2_setFilterLevel ti_uia_sysbios_LoggerStreamer2_setFilterLevel
#define LoggerStreamer2_getFilterLevel ti_uia_sysbios_LoggerStreamer2_getFilterLevel
#define LoggerStreamer2_getBufSize ti_uia_sysbios_LoggerStreamer2_getBufSize
#define LoggerStreamer2_getContext ti_uia_sysbios_LoggerStreamer2_getContext
#define LoggerStreamer2_setContext ti_uia_sysbios_LoggerStreamer2_setContext
#define LoggerStreamer2_Module_name ti_uia_sysbios_LoggerStreamer2_Module_name
#define LoggerStreamer2_Module_id ti_uia_sysbios_LoggerStreamer2_Module_id
#define LoggerStreamer2_Module_startup ti_uia_sysbios_LoggerStreamer2_Module_startup
#define LoggerStreamer2_Module_startupDone ti_uia_sysbios_LoggerStreamer2_Module_startupDone
#define LoggerStreamer2_Module_hasMask ti_uia_sysbios_LoggerStreamer2_Module_hasMask
#define LoggerStreamer2_Module_getMask ti_uia_sysbios_LoggerStreamer2_Module_getMask
#define LoggerStreamer2_Module_setMask ti_uia_sysbios_LoggerStreamer2_Module_setMask
#define LoggerStreamer2_Object_heap ti_uia_sysbios_LoggerStreamer2_Object_heap
#define LoggerStreamer2_Module_heap ti_uia_sysbios_LoggerStreamer2_Module_heap
#define LoggerStreamer2_construct ti_uia_sysbios_LoggerStreamer2_construct
#define LoggerStreamer2_create ti_uia_sysbios_LoggerStreamer2_create
#define LoggerStreamer2_handle ti_uia_sysbios_LoggerStreamer2_handle
#define LoggerStreamer2_struct ti_uia_sysbios_LoggerStreamer2_struct
#define LoggerStreamer2_Handle_label ti_uia_sysbios_LoggerStreamer2_Handle_label
#define LoggerStreamer2_Handle_name ti_uia_sysbios_LoggerStreamer2_Handle_name
#define LoggerStreamer2_Instance_init ti_uia_sysbios_LoggerStreamer2_Instance_init
#define LoggerStreamer2_Object_count ti_uia_sysbios_LoggerStreamer2_Object_count
#define LoggerStreamer2_Object_get ti_uia_sysbios_LoggerStreamer2_Object_get
#define LoggerStreamer2_Object_first ti_uia_sysbios_LoggerStreamer2_Object_first
#define LoggerStreamer2_Object_next ti_uia_sysbios_LoggerStreamer2_Object_next
#define LoggerStreamer2_Object_sizeof ti_uia_sysbios_LoggerStreamer2_Object_sizeof
#define LoggerStreamer2_Params_copy ti_uia_sysbios_LoggerStreamer2_Params_copy
#define LoggerStreamer2_Params_init ti_uia_sysbios_LoggerStreamer2_Params_init
#define LoggerStreamer2_delete ti_uia_sysbios_LoggerStreamer2_delete
#define LoggerStreamer2_destruct ti_uia_sysbios_LoggerStreamer2_destruct
#define LoggerStreamer2_Module_upCast ti_uia_sysbios_LoggerStreamer2_Module_upCast
#define LoggerStreamer2_Module_to_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Module_to_ti_uia_runtime_ILoggerSnapshot
#define LoggerStreamer2_Handle_upCast ti_uia_sysbios_LoggerStreamer2_Handle_upCast
#define LoggerStreamer2_Handle_to_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Handle_to_ti_uia_runtime_ILoggerSnapshot
#define LoggerStreamer2_Handle_downCast ti_uia_sysbios_LoggerStreamer2_Handle_downCast
#define LoggerStreamer2_Handle_from_ti_uia_runtime_ILoggerSnapshot ti_uia_sysbios_LoggerStreamer2_Handle_from_ti_uia_runtime_ILoggerSnapshot
#define LoggerStreamer2_Module_upCast2 ti_uia_sysbios_LoggerStreamer2_Module_upCast2
#define LoggerStreamer2_Module_to_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Module_to_ti_uia_runtime_IUIATransfer
#define LoggerStreamer2_Handle_upCast2 ti_uia_sysbios_LoggerStreamer2_Handle_upCast2
#define LoggerStreamer2_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Handle_to_ti_uia_runtime_IUIATransfer
#define LoggerStreamer2_Handle_downCast2 ti_uia_sysbios_LoggerStreamer2_Handle_downCast2
#define LoggerStreamer2_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_sysbios_LoggerStreamer2_Handle_from_ti_uia_runtime_IUIATransfer
#define LoggerStreamer2_Module_upCast3 ti_uia_sysbios_LoggerStreamer2_Module_upCast3
#define LoggerStreamer2_Module_to_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Module_to_xdc_runtime_IFilterLogger
#define LoggerStreamer2_Handle_upCast3 ti_uia_sysbios_LoggerStreamer2_Handle_upCast3
#define LoggerStreamer2_Handle_to_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Handle_to_xdc_runtime_IFilterLogger
#define LoggerStreamer2_Handle_downCast3 ti_uia_sysbios_LoggerStreamer2_Handle_downCast3
#define LoggerStreamer2_Handle_from_xdc_runtime_IFilterLogger ti_uia_sysbios_LoggerStreamer2_Handle_from_xdc_runtime_IFilterLogger
#define LoggerStreamer2_Module_upCast4 ti_uia_sysbios_LoggerStreamer2_Module_upCast4
#define LoggerStreamer2_Module_to_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Module_to_xdc_runtime_ILogger
#define LoggerStreamer2_Handle_upCast4 ti_uia_sysbios_LoggerStreamer2_Handle_upCast4
#define LoggerStreamer2_Handle_to_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Handle_to_xdc_runtime_ILogger
#define LoggerStreamer2_Handle_downCast4 ti_uia_sysbios_LoggerStreamer2_Handle_downCast4
#define LoggerStreamer2_Handle_from_xdc_runtime_ILogger ti_uia_sysbios_LoggerStreamer2_Handle_from_xdc_runtime_ILogger

#endif /* ti_uia_sysbios_LoggerStreamer2__localnames__done */
#endif
