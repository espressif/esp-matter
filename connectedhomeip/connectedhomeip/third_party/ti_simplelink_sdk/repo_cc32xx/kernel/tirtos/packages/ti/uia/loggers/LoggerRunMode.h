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

#ifndef ti_uia_loggers_LoggerRunMode__include
#define ti_uia_loggers_LoggerRunMode__include

#ifndef __nested__
#define __nested__
#define ti_uia_loggers_LoggerRunMode__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_loggers_LoggerRunMode___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/loggers/LoggerRunMode__prologue.h>
#include <ti/uia/loggers/package/package.defs.h>

#include <xdc/runtime/ILogger.h>
#include <ti/uia/runtime/ILoggerSnapshot.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IFilterLogger.h>
#include <xdc/runtime/IHeap.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* TransferType */
typedef ti_uia_runtime_IUIATransfer_TransferType ti_uia_loggers_LoggerRunMode_TransferType;

/* Priority */
typedef ti_uia_runtime_IUIATransfer_Priority ti_uia_loggers_LoggerRunMode_Priority;

/* TransportType */
enum ti_uia_loggers_LoggerRunMode_TransportType {
    ti_uia_loggers_LoggerRunMode_TransportType_JTAG = 0,
    ti_uia_loggers_LoggerRunMode_TransportType_ETHERNET = 1
};
typedef enum ti_uia_loggers_LoggerRunMode_TransportType ti_uia_loggers_LoggerRunMode_TransportType;

/* TransferType_RELIABLE */
#define ti_uia_loggers_LoggerRunMode_TransferType_RELIABLE ti_uia_runtime_IUIATransfer_TransferType_RELIABLE

/* TransferType_LOSSY */
#define ti_uia_loggers_LoggerRunMode_TransferType_LOSSY ti_uia_runtime_IUIATransfer_TransferType_LOSSY

/* Priority_LOW */
#define ti_uia_loggers_LoggerRunMode_Priority_LOW ti_uia_runtime_IUIATransfer_Priority_LOW

/* Priority_STANDARD */
#define ti_uia_loggers_LoggerRunMode_Priority_STANDARD ti_uia_runtime_IUIATransfer_Priority_STANDARD

/* Priority_HIGH */
#define ti_uia_loggers_LoggerRunMode_Priority_HIGH ti_uia_runtime_IUIATransfer_Priority_HIGH

/* Priority_SYNC */
#define ti_uia_loggers_LoggerRunMode_Priority_SYNC ti_uia_runtime_IUIATransfer_Priority_SYNC


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Instance_State */
typedef xdc_Char __T1_ti_uia_loggers_LoggerRunMode_Instance_State__hdr;
typedef xdc_Char *ARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__hdr;
typedef const xdc_Char *CARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__hdr;
typedef ARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__hdr __TA_ti_uia_loggers_LoggerRunMode_Instance_State__hdr;
typedef xdc_Char __T1_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray;
typedef xdc_Char *ARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray;
typedef const xdc_Char *CARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray;
typedef ARRAY1_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray __TA_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_loggers_LoggerRunMode_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__diagsEnabled ti_uia_loggers_LoggerRunMode_Module__diagsEnabled__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__diagsEnabled__CR
#define ti_uia_loggers_LoggerRunMode_Module__diagsEnabled__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__diagsEnabled (ti_uia_loggers_LoggerRunMode_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_loggers_LoggerRunMode_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__diagsIncluded ti_uia_loggers_LoggerRunMode_Module__diagsIncluded__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__diagsIncluded__CR
#define ti_uia_loggers_LoggerRunMode_Module__diagsIncluded__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__diagsIncluded (ti_uia_loggers_LoggerRunMode_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask ti_uia_loggers_LoggerRunMode_Module__diagsMask__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__diagsMask__CR
#define ti_uia_loggers_LoggerRunMode_Module__diagsMask__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__diagsMask__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__diagsMask (ti_uia_loggers_LoggerRunMode_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerRunMode_Module__gateObj;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__gateObj ti_uia_loggers_LoggerRunMode_Module__gateObj__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__gateObj__CR
#define ti_uia_loggers_LoggerRunMode_Module__gateObj__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__gateObj*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__gateObj__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__gateObj (ti_uia_loggers_LoggerRunMode_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerRunMode_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__gatePrms ti_uia_loggers_LoggerRunMode_Module__gatePrms__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__gatePrms__CR
#define ti_uia_loggers_LoggerRunMode_Module__gatePrms__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__gatePrms*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__gatePrms__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__gatePrms (ti_uia_loggers_LoggerRunMode_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_loggers_LoggerRunMode_Module__id;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__id ti_uia_loggers_LoggerRunMode_Module__id__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__id__CR
#define ti_uia_loggers_LoggerRunMode_Module__id__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__id*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__id__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__id (ti_uia_loggers_LoggerRunMode_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_loggers_LoggerRunMode_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerDefined ti_uia_loggers_LoggerRunMode_Module__loggerDefined__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerDefined__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerDefined__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerDefined__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerDefined (ti_uia_loggers_LoggerRunMode_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerRunMode_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerObj ti_uia_loggers_LoggerRunMode_Module__loggerObj__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerObj__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerObj__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerObj*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerObj__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerObj (ti_uia_loggers_LoggerRunMode_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn0 ti_uia_loggers_LoggerRunMode_Module__loggerFxn0__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerFxn0__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn0__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn0 (ti_uia_loggers_LoggerRunMode_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn1 ti_uia_loggers_LoggerRunMode_Module__loggerFxn1__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerFxn1__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn1__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn1 (ti_uia_loggers_LoggerRunMode_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn2 ti_uia_loggers_LoggerRunMode_Module__loggerFxn2__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerFxn2__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn2__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn2 (ti_uia_loggers_LoggerRunMode_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn4 ti_uia_loggers_LoggerRunMode_Module__loggerFxn4__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerFxn4__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn4__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn4 (ti_uia_loggers_LoggerRunMode_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn8 ti_uia_loggers_LoggerRunMode_Module__loggerFxn8__C;
#ifdef ti_uia_loggers_LoggerRunMode_Module__loggerFxn8__CR
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn8__C (*((CT__ti_uia_loggers_LoggerRunMode_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Module__loggerFxn8 (ti_uia_loggers_LoggerRunMode_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_loggers_LoggerRunMode_Object__count;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Object__count ti_uia_loggers_LoggerRunMode_Object__count__C;
#ifdef ti_uia_loggers_LoggerRunMode_Object__count__CR
#define ti_uia_loggers_LoggerRunMode_Object__count__C (*((CT__ti_uia_loggers_LoggerRunMode_Object__count*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Object__count__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Object__count (ti_uia_loggers_LoggerRunMode_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_loggers_LoggerRunMode_Object__heap;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Object__heap ti_uia_loggers_LoggerRunMode_Object__heap__C;
#ifdef ti_uia_loggers_LoggerRunMode_Object__heap__CR
#define ti_uia_loggers_LoggerRunMode_Object__heap__C (*((CT__ti_uia_loggers_LoggerRunMode_Object__heap*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Object__heap__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Object__heap (ti_uia_loggers_LoggerRunMode_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_loggers_LoggerRunMode_Object__sizeof;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Object__sizeof ti_uia_loggers_LoggerRunMode_Object__sizeof__C;
#ifdef ti_uia_loggers_LoggerRunMode_Object__sizeof__CR
#define ti_uia_loggers_LoggerRunMode_Object__sizeof__C (*((CT__ti_uia_loggers_LoggerRunMode_Object__sizeof*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Object__sizeof__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Object__sizeof (ti_uia_loggers_LoggerRunMode_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerRunMode_Object__table;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_Object__table ti_uia_loggers_LoggerRunMode_Object__table__C;
#ifdef ti_uia_loggers_LoggerRunMode_Object__table__CR
#define ti_uia_loggers_LoggerRunMode_Object__table__C (*((CT__ti_uia_loggers_LoggerRunMode_Object__table*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Object__table__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_Object__table (ti_uia_loggers_LoggerRunMode_Object__table__C)
#endif

/* filterByLevel */
typedef xdc_Bool CT__ti_uia_loggers_LoggerRunMode_filterByLevel;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_filterByLevel ti_uia_loggers_LoggerRunMode_filterByLevel__C;
#ifdef ti_uia_loggers_LoggerRunMode_filterByLevel__CR
#define ti_uia_loggers_LoggerRunMode_filterByLevel (*((CT__ti_uia_loggers_LoggerRunMode_filterByLevel*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_filterByLevel__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_filterByLevel__D
#define ti_uia_loggers_LoggerRunMode_filterByLevel (ti_uia_loggers_LoggerRunMode_filterByLevel__D)
#else
#define ti_uia_loggers_LoggerRunMode_filterByLevel (ti_uia_loggers_LoggerRunMode_filterByLevel__C)
#endif
#endif

/* customTransportType */
typedef xdc_String CT__ti_uia_loggers_LoggerRunMode_customTransportType;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_customTransportType ti_uia_loggers_LoggerRunMode_customTransportType__C;
#ifdef ti_uia_loggers_LoggerRunMode_customTransportType__CR
#define ti_uia_loggers_LoggerRunMode_customTransportType (*((CT__ti_uia_loggers_LoggerRunMode_customTransportType*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_customTransportType__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_customTransportType (ti_uia_loggers_LoggerRunMode_customTransportType__C)
#endif

/* isTimestampEnabled */
typedef xdc_Bool CT__ti_uia_loggers_LoggerRunMode_isTimestampEnabled;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_isTimestampEnabled ti_uia_loggers_LoggerRunMode_isTimestampEnabled__C;
#ifdef ti_uia_loggers_LoggerRunMode_isTimestampEnabled__CR
#define ti_uia_loggers_LoggerRunMode_isTimestampEnabled (*((CT__ti_uia_loggers_LoggerRunMode_isTimestampEnabled*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_isTimestampEnabled__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_isTimestampEnabled__D
#define ti_uia_loggers_LoggerRunMode_isTimestampEnabled (ti_uia_loggers_LoggerRunMode_isTimestampEnabled__D)
#else
#define ti_uia_loggers_LoggerRunMode_isTimestampEnabled (ti_uia_loggers_LoggerRunMode_isTimestampEnabled__C)
#endif
#endif

/* supportLoggerDisable */
typedef xdc_Bool CT__ti_uia_loggers_LoggerRunMode_supportLoggerDisable;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_supportLoggerDisable ti_uia_loggers_LoggerRunMode_supportLoggerDisable__C;
#ifdef ti_uia_loggers_LoggerRunMode_supportLoggerDisable__CR
#define ti_uia_loggers_LoggerRunMode_supportLoggerDisable (*((CT__ti_uia_loggers_LoggerRunMode_supportLoggerDisable*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_supportLoggerDisable__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_supportLoggerDisable__D
#define ti_uia_loggers_LoggerRunMode_supportLoggerDisable (ti_uia_loggers_LoggerRunMode_supportLoggerDisable__D)
#else
#define ti_uia_loggers_LoggerRunMode_supportLoggerDisable (ti_uia_loggers_LoggerRunMode_supportLoggerDisable__C)
#endif
#endif

/* level1Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_loggers_LoggerRunMode_level1Mask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_level1Mask ti_uia_loggers_LoggerRunMode_level1Mask__C;
#ifdef ti_uia_loggers_LoggerRunMode_level1Mask__CR
#define ti_uia_loggers_LoggerRunMode_level1Mask (*((CT__ti_uia_loggers_LoggerRunMode_level1Mask*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_level1Mask__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_level1Mask__D
#define ti_uia_loggers_LoggerRunMode_level1Mask (ti_uia_loggers_LoggerRunMode_level1Mask__D)
#else
#define ti_uia_loggers_LoggerRunMode_level1Mask (ti_uia_loggers_LoggerRunMode_level1Mask__C)
#endif
#endif

/* level2Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_loggers_LoggerRunMode_level2Mask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_level2Mask ti_uia_loggers_LoggerRunMode_level2Mask__C;
#ifdef ti_uia_loggers_LoggerRunMode_level2Mask__CR
#define ti_uia_loggers_LoggerRunMode_level2Mask (*((CT__ti_uia_loggers_LoggerRunMode_level2Mask*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_level2Mask__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_level2Mask__D
#define ti_uia_loggers_LoggerRunMode_level2Mask (ti_uia_loggers_LoggerRunMode_level2Mask__D)
#else
#define ti_uia_loggers_LoggerRunMode_level2Mask (ti_uia_loggers_LoggerRunMode_level2Mask__C)
#endif
#endif

/* level3Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_loggers_LoggerRunMode_level3Mask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_level3Mask ti_uia_loggers_LoggerRunMode_level3Mask__C;
#ifdef ti_uia_loggers_LoggerRunMode_level3Mask__CR
#define ti_uia_loggers_LoggerRunMode_level3Mask (*((CT__ti_uia_loggers_LoggerRunMode_level3Mask*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_level3Mask__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_level3Mask__D
#define ti_uia_loggers_LoggerRunMode_level3Mask (ti_uia_loggers_LoggerRunMode_level3Mask__D)
#else
#define ti_uia_loggers_LoggerRunMode_level3Mask (ti_uia_loggers_LoggerRunMode_level3Mask__C)
#endif
#endif

/* level4Mask */
typedef xdc_runtime_Diags_Mask CT__ti_uia_loggers_LoggerRunMode_level4Mask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_level4Mask ti_uia_loggers_LoggerRunMode_level4Mask__C;
#ifdef ti_uia_loggers_LoggerRunMode_level4Mask__CR
#define ti_uia_loggers_LoggerRunMode_level4Mask (*((CT__ti_uia_loggers_LoggerRunMode_level4Mask*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_level4Mask__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_level4Mask__D
#define ti_uia_loggers_LoggerRunMode_level4Mask (ti_uia_loggers_LoggerRunMode_level4Mask__D)
#else
#define ti_uia_loggers_LoggerRunMode_level4Mask (ti_uia_loggers_LoggerRunMode_level4Mask__C)
#endif
#endif

/* L_test */
typedef xdc_runtime_Log_Event CT__ti_uia_loggers_LoggerRunMode_L_test;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_L_test ti_uia_loggers_LoggerRunMode_L_test__C;
#ifdef ti_uia_loggers_LoggerRunMode_L_test__CR
#define ti_uia_loggers_LoggerRunMode_L_test (*((CT__ti_uia_loggers_LoggerRunMode_L_test*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_L_test__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_L_test (ti_uia_loggers_LoggerRunMode_L_test__C)
#endif

/* E_badLevel */
typedef xdc_runtime_Error_Id CT__ti_uia_loggers_LoggerRunMode_E_badLevel;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_E_badLevel ti_uia_loggers_LoggerRunMode_E_badLevel__C;
#ifdef ti_uia_loggers_LoggerRunMode_E_badLevel__CR
#define ti_uia_loggers_LoggerRunMode_E_badLevel (*((CT__ti_uia_loggers_LoggerRunMode_E_badLevel*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_E_badLevel__C_offset)))
#else
#define ti_uia_loggers_LoggerRunMode_E_badLevel (ti_uia_loggers_LoggerRunMode_E_badLevel__C)
#endif

/* numCores */
typedef xdc_Int CT__ti_uia_loggers_LoggerRunMode_numCores;
__extern __FAR__ const CT__ti_uia_loggers_LoggerRunMode_numCores ti_uia_loggers_LoggerRunMode_numCores__C;
#ifdef ti_uia_loggers_LoggerRunMode_numCores__CR
#define ti_uia_loggers_LoggerRunMode_numCores (*((CT__ti_uia_loggers_LoggerRunMode_numCores*)(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_numCores__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerRunMode_numCores__D
#define ti_uia_loggers_LoggerRunMode_numCores (ti_uia_loggers_LoggerRunMode_numCores__D)
#else
#define ti_uia_loggers_LoggerRunMode_numCores (ti_uia_loggers_LoggerRunMode_numCores__C)
#endif
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_loggers_LoggerRunMode_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    ti_uia_runtime_IUIATransfer_TransferType transferType;
    ti_uia_runtime_IUIATransfer_Priority priority;
    xdc_Int16 instanceId;
    xdc_SizeT transferBufSize;
    xdc_SizeT maxEventSize;
    xdc_SizeT bufSize;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_uia_loggers_LoggerRunMode_Struct {
    const ti_uia_loggers_LoggerRunMode_Fxns__ *__fxns;
    xdc_Bool f0;
    xdc_Int16 f1;
    xdc_Bool f2;
    xdc_UInt32 f3;
    xdc_UArg *f4;
    xdc_UArg *f5;
    xdc_UArg *f6;
    xdc_SizeT f7;
    xdc_SizeT f8;
    xdc_UInt16 f9;
    xdc_Bits32 f10;
    xdc_UInt16 f11;
    xdc_UInt16 f12;
    __TA_ti_uia_loggers_LoggerRunMode_Instance_State__hdr f13;
    __TA_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray f14;
    xdc_Int f15;
    xdc_UInt32 f16;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_loggers_LoggerRunMode_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_Bool (*disable)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_Void (*write0)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
    xdc_Void (*write1)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
    xdc_Void (*write2)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
    xdc_Void (*write4)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
    xdc_Void (*write8)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
    xdc_Void (*setFilterLevel)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
    xdc_runtime_Diags_Mask (*getFilterLevel)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Diags_EventLevel level);
    ti_uia_runtime_IUIATransfer_TransferType (*getTransferType)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_Bool (*getContents)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT* cpSize);
    xdc_Bool (*isEmpty)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_SizeT (*getMaxLength)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_UInt16 (*getInstanceId)(ti_uia_loggers_LoggerRunMode_Handle inst);
    ti_uia_runtime_IUIATransfer_Priority (*getPriority)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_Void (*setPriority)(ti_uia_loggers_LoggerRunMode_Handle inst, ti_uia_runtime_IUIATransfer_Priority priority);
    xdc_Void (*reset)(ti_uia_loggers_LoggerRunMode_Handle inst);
    xdc_Void (*writeMemoryRange)(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_loggers_LoggerRunMode_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_loggers_LoggerRunMode_Fxns__ ti_uia_loggers_LoggerRunMode_Module__FXNS__C;
#else
#define ti_uia_loggers_LoggerRunMode_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_loggers_LoggerRunMode_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_loggers_LoggerRunMode_Module_startup ti_uia_loggers_LoggerRunMode_Module_startup__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Module_startup__E, "ti_uia_loggers_LoggerRunMode_Module_startup")
__extern xdc_Int ti_uia_loggers_LoggerRunMode_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Module_startup__F, "ti_uia_loggers_LoggerRunMode_Module_startup")
__extern xdc_Int ti_uia_loggers_LoggerRunMode_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Instance_init__E, "ti_uia_loggers_LoggerRunMode_Instance_init")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_Instance_init__E(ti_uia_loggers_LoggerRunMode_Object *obj, const ti_uia_loggers_LoggerRunMode_Params *prms);

/* create */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_create, "ti_uia_loggers_LoggerRunMode_create")
__extern ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_create( const ti_uia_loggers_LoggerRunMode_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_construct, "ti_uia_loggers_LoggerRunMode_construct")
__extern void ti_uia_loggers_LoggerRunMode_construct(ti_uia_loggers_LoggerRunMode_Struct *obj, const ti_uia_loggers_LoggerRunMode_Params *prms);

/* delete */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_delete, "ti_uia_loggers_LoggerRunMode_delete")
__extern void ti_uia_loggers_LoggerRunMode_delete(ti_uia_loggers_LoggerRunMode_Handle *instp);

/* destruct */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_destruct, "ti_uia_loggers_LoggerRunMode_destruct")
__extern void ti_uia_loggers_LoggerRunMode_destruct(ti_uia_loggers_LoggerRunMode_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Handle__label__S, "ti_uia_loggers_LoggerRunMode_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_loggers_LoggerRunMode_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Module__startupDone__S, "ti_uia_loggers_LoggerRunMode_Module__startupDone__S")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Object__create__S, "ti_uia_loggers_LoggerRunMode_Object__create__S")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Object__delete__S, "ti_uia_loggers_LoggerRunMode_Object__delete__S")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Object__get__S, "ti_uia_loggers_LoggerRunMode_Object__get__S")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Object__first__S, "ti_uia_loggers_LoggerRunMode_Object__first__S")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Object__next__S, "ti_uia_loggers_LoggerRunMode_Object__next__S")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_Params__init__S, "ti_uia_loggers_LoggerRunMode_Params__init__S")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* enable__E */
#define ti_uia_loggers_LoggerRunMode_enable ti_uia_loggers_LoggerRunMode_enable__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_enable__E, "ti_uia_loggers_LoggerRunMode_enable")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_enable__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* disable__E */
#define ti_uia_loggers_LoggerRunMode_disable ti_uia_loggers_LoggerRunMode_disable__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_disable__E, "ti_uia_loggers_LoggerRunMode_disable")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_disable__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* getTransferType__E */
#define ti_uia_loggers_LoggerRunMode_getTransferType ti_uia_loggers_LoggerRunMode_getTransferType__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getTransferType__E, "ti_uia_loggers_LoggerRunMode_getTransferType")
__extern ti_uia_runtime_IUIATransfer_TransferType ti_uia_loggers_LoggerRunMode_getTransferType__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* getContents__E */
#define ti_uia_loggers_LoggerRunMode_getContents ti_uia_loggers_LoggerRunMode_getContents__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getContents__E, "ti_uia_loggers_LoggerRunMode_getContents")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_getContents__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT *cpSize);

/* isEmpty__E */
#define ti_uia_loggers_LoggerRunMode_isEmpty ti_uia_loggers_LoggerRunMode_isEmpty__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_isEmpty__E, "ti_uia_loggers_LoggerRunMode_isEmpty")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_isEmpty__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* getMaxLength__E */
#define ti_uia_loggers_LoggerRunMode_getMaxLength ti_uia_loggers_LoggerRunMode_getMaxLength__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getMaxLength__E, "ti_uia_loggers_LoggerRunMode_getMaxLength")
__extern xdc_SizeT ti_uia_loggers_LoggerRunMode_getMaxLength__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* getInstanceId__E */
#define ti_uia_loggers_LoggerRunMode_getInstanceId ti_uia_loggers_LoggerRunMode_getInstanceId__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getInstanceId__E, "ti_uia_loggers_LoggerRunMode_getInstanceId")
__extern xdc_UInt16 ti_uia_loggers_LoggerRunMode_getInstanceId__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* getPriority__E */
#define ti_uia_loggers_LoggerRunMode_getPriority ti_uia_loggers_LoggerRunMode_getPriority__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getPriority__E, "ti_uia_loggers_LoggerRunMode_getPriority")
__extern ti_uia_runtime_IUIATransfer_Priority ti_uia_loggers_LoggerRunMode_getPriority__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* setPriority__E */
#define ti_uia_loggers_LoggerRunMode_setPriority ti_uia_loggers_LoggerRunMode_setPriority__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_setPriority__E, "ti_uia_loggers_LoggerRunMode_setPriority")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_setPriority__E( ti_uia_loggers_LoggerRunMode_Handle instp, ti_uia_runtime_IUIATransfer_Priority priority);

/* reset__E */
#define ti_uia_loggers_LoggerRunMode_reset ti_uia_loggers_LoggerRunMode_reset__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_reset__E, "ti_uia_loggers_LoggerRunMode_reset")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_reset__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* writeMemoryRange__E */
#define ti_uia_loggers_LoggerRunMode_writeMemoryRange ti_uia_loggers_LoggerRunMode_writeMemoryRange__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_writeMemoryRange__E, "ti_uia_loggers_LoggerRunMode_writeMemoryRange")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_writeMemoryRange__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);

/* isUploadRequired__E */
#define ti_uia_loggers_LoggerRunMode_isUploadRequired ti_uia_loggers_LoggerRunMode_isUploadRequired__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_isUploadRequired__E, "ti_uia_loggers_LoggerRunMode_isUploadRequired")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_isUploadRequired__E( void);

/* idleHook__E */
#define ti_uia_loggers_LoggerRunMode_idleHook ti_uia_loggers_LoggerRunMode_idleHook__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_idleHook__E, "ti_uia_loggers_LoggerRunMode_idleHook")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_idleHook__E( void);

/* initBuffer__E */
#define ti_uia_loggers_LoggerRunMode_initBuffer ti_uia_loggers_LoggerRunMode_initBuffer__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_initBuffer__E, "ti_uia_loggers_LoggerRunMode_initBuffer")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_initBuffer__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_Ptr buffer, xdc_UInt16 src);

/* flush__E */
#define ti_uia_loggers_LoggerRunMode_flush ti_uia_loggers_LoggerRunMode_flush__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_flush__E, "ti_uia_loggers_LoggerRunMode_flush")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_flush__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* prime__E */
#define ti_uia_loggers_LoggerRunMode_prime ti_uia_loggers_LoggerRunMode_prime__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_prime__E, "ti_uia_loggers_LoggerRunMode_prime")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_prime__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* exchange__E */
#define ti_uia_loggers_LoggerRunMode_exchange ti_uia_loggers_LoggerRunMode_exchange__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_exchange__E, "ti_uia_loggers_LoggerRunMode_exchange")
__extern xdc_Ptr ti_uia_loggers_LoggerRunMode_exchange__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_Ptr full, xdc_Ptr lastWritePtr);

/* initQueueDescriptor__E */
#define ti_uia_loggers_LoggerRunMode_initQueueDescriptor ti_uia_loggers_LoggerRunMode_initQueueDescriptor__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_initQueueDescriptor__E, "ti_uia_loggers_LoggerRunMode_initQueueDescriptor")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_initQueueDescriptor__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Types_ModuleId mid);

/* write0__E */
#define ti_uia_loggers_LoggerRunMode_write0 ti_uia_loggers_LoggerRunMode_write0__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_write0__E, "ti_uia_loggers_LoggerRunMode_write0")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_write0__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);

/* write1__E */
#define ti_uia_loggers_LoggerRunMode_write1 ti_uia_loggers_LoggerRunMode_write1__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_write1__E, "ti_uia_loggers_LoggerRunMode_write1")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_write1__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);

/* write2__E */
#define ti_uia_loggers_LoggerRunMode_write2 ti_uia_loggers_LoggerRunMode_write2__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_write2__E, "ti_uia_loggers_LoggerRunMode_write2")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_write2__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);

/* write4__E */
#define ti_uia_loggers_LoggerRunMode_write4 ti_uia_loggers_LoggerRunMode_write4__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_write4__E, "ti_uia_loggers_LoggerRunMode_write4")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_write4__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);

/* write8__E */
#define ti_uia_loggers_LoggerRunMode_write8 ti_uia_loggers_LoggerRunMode_write8__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_write8__E, "ti_uia_loggers_LoggerRunMode_write8")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_write8__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);

/* setFilterLevel__E */
#define ti_uia_loggers_LoggerRunMode_setFilterLevel ti_uia_loggers_LoggerRunMode_setFilterLevel__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_setFilterLevel__E, "ti_uia_loggers_LoggerRunMode_setFilterLevel")
__extern xdc_Void ti_uia_loggers_LoggerRunMode_setFilterLevel__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);

/* getFilterLevel__E */
#define ti_uia_loggers_LoggerRunMode_getFilterLevel ti_uia_loggers_LoggerRunMode_getFilterLevel__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getFilterLevel__E, "ti_uia_loggers_LoggerRunMode_getFilterLevel")
__extern xdc_runtime_Diags_Mask ti_uia_loggers_LoggerRunMode_getFilterLevel__E( ti_uia_loggers_LoggerRunMode_Handle instp, xdc_runtime_Diags_EventLevel level);

/* getBufSize__E */
#define ti_uia_loggers_LoggerRunMode_getBufSize ti_uia_loggers_LoggerRunMode_getBufSize__E
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_getBufSize__E, "ti_uia_loggers_LoggerRunMode_getBufSize")
__extern xdc_SizeT ti_uia_loggers_LoggerRunMode_getBufSize__E( ti_uia_loggers_LoggerRunMode_Handle instp);

/* filterOutEvent__I */
#define ti_uia_loggers_LoggerRunMode_filterOutEvent ti_uia_loggers_LoggerRunMode_filterOutEvent__I
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_filterOutEvent__I, "ti_uia_loggers_LoggerRunMode_filterOutEvent")
__extern xdc_Bool ti_uia_loggers_LoggerRunMode_filterOutEvent__I( xdc_runtime_Diags_Mask mask);

/* writeStart__I */
#define ti_uia_loggers_LoggerRunMode_writeStart ti_uia_loggers_LoggerRunMode_writeStart__I
xdc__CODESECT(ti_uia_loggers_LoggerRunMode_writeStart__I, "ti_uia_loggers_LoggerRunMode_writeStart")
__extern xdc_UArg *ti_uia_loggers_LoggerRunMode_writeStart__I( ti_uia_loggers_LoggerRunMode_Object *obj, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt16 numBytes);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_loggers_LoggerRunMode_Module_upCast(void);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_loggers_LoggerRunMode_Module_upCast(void)
{
    return (ti_uia_runtime_ILoggerSnapshot_Module)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C;
}

/* Module_to_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_loggers_LoggerRunMode_Module_to_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Module_upCast

/* Handle_upCast */
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast(ti_uia_loggers_LoggerRunMode_Handle i);
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast(ti_uia_loggers_LoggerRunMode_Handle i)
{
    return (ti_uia_runtime_ILoggerSnapshot_Handle)i;
}

/* Handle_to_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_loggers_LoggerRunMode_Handle_to_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Handle_upCast

/* Handle_downCast */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast(ti_uia_runtime_ILoggerSnapshot_Handle i);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast(ti_uia_runtime_ILoggerSnapshot_Handle i)
{
    ti_uia_runtime_ILoggerSnapshot_Handle i2 = (ti_uia_runtime_ILoggerSnapshot_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C) ? (ti_uia_loggers_LoggerRunMode_Handle)i : (ti_uia_loggers_LoggerRunMode_Handle)NULL;
}

/* Handle_from_ti_uia_runtime_ILoggerSnapshot */
#define ti_uia_loggers_LoggerRunMode_Handle_from_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Handle_downCast

/* Module_upCast2 */
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_loggers_LoggerRunMode_Module_upCast2(void);
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_loggers_LoggerRunMode_Module_upCast2(void)
{
    return (ti_uia_runtime_IUIATransfer_Module)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C;
}

/* Module_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_loggers_LoggerRunMode_Module_to_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Module_upCast2

/* Handle_upCast2 */
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast2(ti_uia_loggers_LoggerRunMode_Handle i);
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast2(ti_uia_loggers_LoggerRunMode_Handle i)
{
    return (ti_uia_runtime_IUIATransfer_Handle)i;
}

/* Handle_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_loggers_LoggerRunMode_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Handle_upCast2

/* Handle_downCast2 */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast2(ti_uia_runtime_IUIATransfer_Handle i);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast2(ti_uia_runtime_IUIATransfer_Handle i)
{
    ti_uia_runtime_IUIATransfer_Handle i2 = (ti_uia_runtime_IUIATransfer_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C) ? (ti_uia_loggers_LoggerRunMode_Handle)i : (ti_uia_loggers_LoggerRunMode_Handle)NULL;
}

/* Handle_from_ti_uia_runtime_IUIATransfer */
#define ti_uia_loggers_LoggerRunMode_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Handle_downCast2

/* Module_upCast3 */
static inline xdc_runtime_IFilterLogger_Module ti_uia_loggers_LoggerRunMode_Module_upCast3(void);
static inline xdc_runtime_IFilterLogger_Module ti_uia_loggers_LoggerRunMode_Module_upCast3(void)
{
    return (xdc_runtime_IFilterLogger_Module)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IFilterLogger */
#define ti_uia_loggers_LoggerRunMode_Module_to_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Module_upCast3

/* Handle_upCast3 */
static inline xdc_runtime_IFilterLogger_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast3(ti_uia_loggers_LoggerRunMode_Handle i);
static inline xdc_runtime_IFilterLogger_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast3(ti_uia_loggers_LoggerRunMode_Handle i)
{
    return (xdc_runtime_IFilterLogger_Handle)i;
}

/* Handle_to_xdc_runtime_IFilterLogger */
#define ti_uia_loggers_LoggerRunMode_Handle_to_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Handle_upCast3

/* Handle_downCast3 */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast3(xdc_runtime_IFilterLogger_Handle i);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast3(xdc_runtime_IFilterLogger_Handle i)
{
    xdc_runtime_IFilterLogger_Handle i2 = (xdc_runtime_IFilterLogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C) ? (ti_uia_loggers_LoggerRunMode_Handle)i : (ti_uia_loggers_LoggerRunMode_Handle)NULL;
}

/* Handle_from_xdc_runtime_IFilterLogger */
#define ti_uia_loggers_LoggerRunMode_Handle_from_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Handle_downCast3

/* Module_upCast4 */
static inline xdc_runtime_ILogger_Module ti_uia_loggers_LoggerRunMode_Module_upCast4(void);
static inline xdc_runtime_ILogger_Module ti_uia_loggers_LoggerRunMode_Module_upCast4(void)
{
    return (xdc_runtime_ILogger_Module)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerRunMode_Module_to_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Module_upCast4

/* Handle_upCast4 */
static inline xdc_runtime_ILogger_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast4(ti_uia_loggers_LoggerRunMode_Handle i);
static inline xdc_runtime_ILogger_Handle ti_uia_loggers_LoggerRunMode_Handle_upCast4(ti_uia_loggers_LoggerRunMode_Handle i)
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerRunMode_Handle_to_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Handle_upCast4

/* Handle_downCast4 */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast4(xdc_runtime_ILogger_Handle i);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle_downCast4(xdc_runtime_ILogger_Handle i)
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_loggers_LoggerRunMode_Module__FXNS__C) ? (ti_uia_loggers_LoggerRunMode_Handle)i : (ti_uia_loggers_LoggerRunMode_Handle)NULL;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerRunMode_Handle_from_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Handle_downCast4


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_loggers_LoggerRunMode_Module_startupDone() ti_uia_loggers_LoggerRunMode_Module__startupDone__S()

/* Object_heap */
#define ti_uia_loggers_LoggerRunMode_Object_heap() ti_uia_loggers_LoggerRunMode_Object__heap__C

/* Module_heap */
#define ti_uia_loggers_LoggerRunMode_Module_heap() ti_uia_loggers_LoggerRunMode_Object__heap__C

/* Module_id */
static inline CT__ti_uia_loggers_LoggerRunMode_Module__id ti_uia_loggers_LoggerRunMode_Module_id(void);
static inline CT__ti_uia_loggers_LoggerRunMode_Module__id ti_uia_loggers_LoggerRunMode_Module_id( void ) 
{
    return ti_uia_loggers_LoggerRunMode_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_loggers_LoggerRunMode_Module_hasMask(void);
static inline xdc_Bool ti_uia_loggers_LoggerRunMode_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_loggers_LoggerRunMode_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_loggers_LoggerRunMode_Module_getMask(void);
static inline xdc_Bits16 ti_uia_loggers_LoggerRunMode_Module_getMask(void)
{
    return (ti_uia_loggers_LoggerRunMode_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask)NULL) ? *ti_uia_loggers_LoggerRunMode_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_loggers_LoggerRunMode_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_loggers_LoggerRunMode_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_loggers_LoggerRunMode_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerRunMode_Module__diagsMask)NULL) {
        *ti_uia_loggers_LoggerRunMode_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_uia_loggers_LoggerRunMode_Params_init(ti_uia_loggers_LoggerRunMode_Params *prms);
static inline void ti_uia_loggers_LoggerRunMode_Params_init( ti_uia_loggers_LoggerRunMode_Params *prms ) 
{
    if (prms != NULL) {
        ti_uia_loggers_LoggerRunMode_Params__init__S(prms, NULL, sizeof(ti_uia_loggers_LoggerRunMode_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_uia_loggers_LoggerRunMode_Params_copy(ti_uia_loggers_LoggerRunMode_Params *dst, const ti_uia_loggers_LoggerRunMode_Params *src);
static inline void ti_uia_loggers_LoggerRunMode_Params_copy(ti_uia_loggers_LoggerRunMode_Params *dst, const ti_uia_loggers_LoggerRunMode_Params *src) 
{
    if (dst != NULL) {
        ti_uia_loggers_LoggerRunMode_Params__init__S(dst, (const void *)src, sizeof(ti_uia_loggers_LoggerRunMode_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_uia_loggers_LoggerRunMode_Object_count() ti_uia_loggers_LoggerRunMode_Object__count__C

/* Object_sizeof */
#define ti_uia_loggers_LoggerRunMode_Object_sizeof() ti_uia_loggers_LoggerRunMode_Object__sizeof__C

/* Object_get */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_get(ti_uia_loggers_LoggerRunMode_Object *oarr, int i);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_get(ti_uia_loggers_LoggerRunMode_Object *oarr, int i) 
{
    return (ti_uia_loggers_LoggerRunMode_Handle)ti_uia_loggers_LoggerRunMode_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_first(void);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_first(void)
{
    return (ti_uia_loggers_LoggerRunMode_Handle)ti_uia_loggers_LoggerRunMode_Object__first__S();
}

/* Object_next */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_next(ti_uia_loggers_LoggerRunMode_Object *obj);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Object_next(ti_uia_loggers_LoggerRunMode_Object *obj)
{
    return (ti_uia_loggers_LoggerRunMode_Handle)ti_uia_loggers_LoggerRunMode_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_loggers_LoggerRunMode_Handle_label(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_loggers_LoggerRunMode_Handle_label(ti_uia_loggers_LoggerRunMode_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_uia_loggers_LoggerRunMode_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_uia_loggers_LoggerRunMode_Handle_name(ti_uia_loggers_LoggerRunMode_Handle inst);
static inline xdc_String ti_uia_loggers_LoggerRunMode_Handle_name(ti_uia_loggers_LoggerRunMode_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_uia_loggers_LoggerRunMode_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_handle(ti_uia_loggers_LoggerRunMode_Struct *str);
static inline ti_uia_loggers_LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_handle(ti_uia_loggers_LoggerRunMode_Struct *str)
{
    return (ti_uia_loggers_LoggerRunMode_Handle)str;
}

/* struct */
static inline ti_uia_loggers_LoggerRunMode_Struct *ti_uia_loggers_LoggerRunMode_struct(ti_uia_loggers_LoggerRunMode_Handle inst);
static inline ti_uia_loggers_LoggerRunMode_Struct *ti_uia_loggers_LoggerRunMode_struct(ti_uia_loggers_LoggerRunMode_Handle inst)
{
    return (ti_uia_loggers_LoggerRunMode_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/loggers/LoggerRunMode__epilogue.h>

#ifdef ti_uia_loggers_LoggerRunMode__top__
#undef __nested__
#endif

#endif /* ti_uia_loggers_LoggerRunMode__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_loggers_LoggerRunMode__internalaccess))

#ifndef ti_uia_loggers_LoggerRunMode__include_state
#define ti_uia_loggers_LoggerRunMode__include_state

/* Module_State */
struct ti_uia_loggers_LoggerRunMode_Module_State {
    xdc_Bool enabled;
    xdc_runtime_Diags_Mask level1;
    xdc_runtime_Diags_Mask level2;
    xdc_runtime_Diags_Mask level3;
    xdc_Bits32 lastUploadTstamp;
};

/* Module__state__V */
#ifndef ti_uia_loggers_LoggerRunMode_Module__state__VR
extern struct ti_uia_loggers_LoggerRunMode_Module_State__ ti_uia_loggers_LoggerRunMode_Module__state__V;
#else
#define ti_uia_loggers_LoggerRunMode_Module__state__V (*((struct ti_uia_loggers_LoggerRunMode_Module_State__*)(xdcRomStatePtr + ti_uia_loggers_LoggerRunMode_Module__state__V_offset)))
#endif

/* Object */
struct ti_uia_loggers_LoggerRunMode_Object {
    const ti_uia_loggers_LoggerRunMode_Fxns__ *__fxns;
    xdc_Bool enabled;
    xdc_Int16 instanceId;
    xdc_Bool primeStatus;
    xdc_UInt32 bufSize;
    xdc_UArg *buffer;
    xdc_UArg *write;
    xdc_UArg *end;
    xdc_SizeT maxEventSizeUArgs;
    xdc_SizeT maxEventSize;
    xdc_UInt16 numBytesInPrevEvent;
    xdc_Bits32 droppedEvents;
    xdc_UInt16 eventSequenceNum;
    xdc_UInt16 pktSequenceNum;
    __TA_ti_uia_loggers_LoggerRunMode_Instance_State__hdr hdr;
    __TA_ti_uia_loggers_LoggerRunMode_Instance_State__packetArray packetArray;
    xdc_Int numPackets;
    xdc_UInt32 packetSize;
};

#endif /* ti_uia_loggers_LoggerRunMode__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_loggers_LoggerRunMode__nolocalnames)

#ifndef ti_uia_loggers_LoggerRunMode__localnames__done
#define ti_uia_loggers_LoggerRunMode__localnames__done

/* module prefix */
#define LoggerRunMode_Instance ti_uia_loggers_LoggerRunMode_Instance
#define LoggerRunMode_Handle ti_uia_loggers_LoggerRunMode_Handle
#define LoggerRunMode_Module ti_uia_loggers_LoggerRunMode_Module
#define LoggerRunMode_Object ti_uia_loggers_LoggerRunMode_Object
#define LoggerRunMode_Struct ti_uia_loggers_LoggerRunMode_Struct
#define LoggerRunMode_TransferType ti_uia_loggers_LoggerRunMode_TransferType
#define LoggerRunMode_Priority ti_uia_loggers_LoggerRunMode_Priority
#define LoggerRunMode_TransportType ti_uia_loggers_LoggerRunMode_TransportType
#define LoggerRunMode_Module_State ti_uia_loggers_LoggerRunMode_Module_State
#define LoggerRunMode_Instance_State ti_uia_loggers_LoggerRunMode_Instance_State
#define LoggerRunMode_TransferType_RELIABLE ti_uia_loggers_LoggerRunMode_TransferType_RELIABLE
#define LoggerRunMode_TransferType_LOSSY ti_uia_loggers_LoggerRunMode_TransferType_LOSSY
#define LoggerRunMode_Priority_LOW ti_uia_loggers_LoggerRunMode_Priority_LOW
#define LoggerRunMode_Priority_STANDARD ti_uia_loggers_LoggerRunMode_Priority_STANDARD
#define LoggerRunMode_Priority_HIGH ti_uia_loggers_LoggerRunMode_Priority_HIGH
#define LoggerRunMode_Priority_SYNC ti_uia_loggers_LoggerRunMode_Priority_SYNC
#define LoggerRunMode_TransportType_JTAG ti_uia_loggers_LoggerRunMode_TransportType_JTAG
#define LoggerRunMode_TransportType_ETHERNET ti_uia_loggers_LoggerRunMode_TransportType_ETHERNET
#define LoggerRunMode_filterByLevel ti_uia_loggers_LoggerRunMode_filterByLevel
#define LoggerRunMode_customTransportType ti_uia_loggers_LoggerRunMode_customTransportType
#define LoggerRunMode_isTimestampEnabled ti_uia_loggers_LoggerRunMode_isTimestampEnabled
#define LoggerRunMode_supportLoggerDisable ti_uia_loggers_LoggerRunMode_supportLoggerDisable
#define LoggerRunMode_level1Mask ti_uia_loggers_LoggerRunMode_level1Mask
#define LoggerRunMode_level2Mask ti_uia_loggers_LoggerRunMode_level2Mask
#define LoggerRunMode_level3Mask ti_uia_loggers_LoggerRunMode_level3Mask
#define LoggerRunMode_level4Mask ti_uia_loggers_LoggerRunMode_level4Mask
#define LoggerRunMode_L_test ti_uia_loggers_LoggerRunMode_L_test
#define LoggerRunMode_E_badLevel ti_uia_loggers_LoggerRunMode_E_badLevel
#define LoggerRunMode_numCores ti_uia_loggers_LoggerRunMode_numCores
#define LoggerRunMode_Params ti_uia_loggers_LoggerRunMode_Params
#define LoggerRunMode_enable ti_uia_loggers_LoggerRunMode_enable
#define LoggerRunMode_disable ti_uia_loggers_LoggerRunMode_disable
#define LoggerRunMode_getTransferType ti_uia_loggers_LoggerRunMode_getTransferType
#define LoggerRunMode_getContents ti_uia_loggers_LoggerRunMode_getContents
#define LoggerRunMode_isEmpty ti_uia_loggers_LoggerRunMode_isEmpty
#define LoggerRunMode_getMaxLength ti_uia_loggers_LoggerRunMode_getMaxLength
#define LoggerRunMode_getInstanceId ti_uia_loggers_LoggerRunMode_getInstanceId
#define LoggerRunMode_getPriority ti_uia_loggers_LoggerRunMode_getPriority
#define LoggerRunMode_setPriority ti_uia_loggers_LoggerRunMode_setPriority
#define LoggerRunMode_reset ti_uia_loggers_LoggerRunMode_reset
#define LoggerRunMode_writeMemoryRange ti_uia_loggers_LoggerRunMode_writeMemoryRange
#define LoggerRunMode_isUploadRequired ti_uia_loggers_LoggerRunMode_isUploadRequired
#define LoggerRunMode_idleHook ti_uia_loggers_LoggerRunMode_idleHook
#define LoggerRunMode_initBuffer ti_uia_loggers_LoggerRunMode_initBuffer
#define LoggerRunMode_flush ti_uia_loggers_LoggerRunMode_flush
#define LoggerRunMode_prime ti_uia_loggers_LoggerRunMode_prime
#define LoggerRunMode_exchange ti_uia_loggers_LoggerRunMode_exchange
#define LoggerRunMode_initQueueDescriptor ti_uia_loggers_LoggerRunMode_initQueueDescriptor
#define LoggerRunMode_write0 ti_uia_loggers_LoggerRunMode_write0
#define LoggerRunMode_write1 ti_uia_loggers_LoggerRunMode_write1
#define LoggerRunMode_write2 ti_uia_loggers_LoggerRunMode_write2
#define LoggerRunMode_write4 ti_uia_loggers_LoggerRunMode_write4
#define LoggerRunMode_write8 ti_uia_loggers_LoggerRunMode_write8
#define LoggerRunMode_setFilterLevel ti_uia_loggers_LoggerRunMode_setFilterLevel
#define LoggerRunMode_getFilterLevel ti_uia_loggers_LoggerRunMode_getFilterLevel
#define LoggerRunMode_getBufSize ti_uia_loggers_LoggerRunMode_getBufSize
#define LoggerRunMode_Module_name ti_uia_loggers_LoggerRunMode_Module_name
#define LoggerRunMode_Module_id ti_uia_loggers_LoggerRunMode_Module_id
#define LoggerRunMode_Module_startup ti_uia_loggers_LoggerRunMode_Module_startup
#define LoggerRunMode_Module_startupDone ti_uia_loggers_LoggerRunMode_Module_startupDone
#define LoggerRunMode_Module_hasMask ti_uia_loggers_LoggerRunMode_Module_hasMask
#define LoggerRunMode_Module_getMask ti_uia_loggers_LoggerRunMode_Module_getMask
#define LoggerRunMode_Module_setMask ti_uia_loggers_LoggerRunMode_Module_setMask
#define LoggerRunMode_Object_heap ti_uia_loggers_LoggerRunMode_Object_heap
#define LoggerRunMode_Module_heap ti_uia_loggers_LoggerRunMode_Module_heap
#define LoggerRunMode_construct ti_uia_loggers_LoggerRunMode_construct
#define LoggerRunMode_create ti_uia_loggers_LoggerRunMode_create
#define LoggerRunMode_handle ti_uia_loggers_LoggerRunMode_handle
#define LoggerRunMode_struct ti_uia_loggers_LoggerRunMode_struct
#define LoggerRunMode_Handle_label ti_uia_loggers_LoggerRunMode_Handle_label
#define LoggerRunMode_Handle_name ti_uia_loggers_LoggerRunMode_Handle_name
#define LoggerRunMode_Instance_init ti_uia_loggers_LoggerRunMode_Instance_init
#define LoggerRunMode_Object_count ti_uia_loggers_LoggerRunMode_Object_count
#define LoggerRunMode_Object_get ti_uia_loggers_LoggerRunMode_Object_get
#define LoggerRunMode_Object_first ti_uia_loggers_LoggerRunMode_Object_first
#define LoggerRunMode_Object_next ti_uia_loggers_LoggerRunMode_Object_next
#define LoggerRunMode_Object_sizeof ti_uia_loggers_LoggerRunMode_Object_sizeof
#define LoggerRunMode_Params_copy ti_uia_loggers_LoggerRunMode_Params_copy
#define LoggerRunMode_Params_init ti_uia_loggers_LoggerRunMode_Params_init
#define LoggerRunMode_delete ti_uia_loggers_LoggerRunMode_delete
#define LoggerRunMode_destruct ti_uia_loggers_LoggerRunMode_destruct
#define LoggerRunMode_Module_upCast ti_uia_loggers_LoggerRunMode_Module_upCast
#define LoggerRunMode_Module_to_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Module_to_ti_uia_runtime_ILoggerSnapshot
#define LoggerRunMode_Handle_upCast ti_uia_loggers_LoggerRunMode_Handle_upCast
#define LoggerRunMode_Handle_to_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Handle_to_ti_uia_runtime_ILoggerSnapshot
#define LoggerRunMode_Handle_downCast ti_uia_loggers_LoggerRunMode_Handle_downCast
#define LoggerRunMode_Handle_from_ti_uia_runtime_ILoggerSnapshot ti_uia_loggers_LoggerRunMode_Handle_from_ti_uia_runtime_ILoggerSnapshot
#define LoggerRunMode_Module_upCast2 ti_uia_loggers_LoggerRunMode_Module_upCast2
#define LoggerRunMode_Module_to_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Module_to_ti_uia_runtime_IUIATransfer
#define LoggerRunMode_Handle_upCast2 ti_uia_loggers_LoggerRunMode_Handle_upCast2
#define LoggerRunMode_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Handle_to_ti_uia_runtime_IUIATransfer
#define LoggerRunMode_Handle_downCast2 ti_uia_loggers_LoggerRunMode_Handle_downCast2
#define LoggerRunMode_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_loggers_LoggerRunMode_Handle_from_ti_uia_runtime_IUIATransfer
#define LoggerRunMode_Module_upCast3 ti_uia_loggers_LoggerRunMode_Module_upCast3
#define LoggerRunMode_Module_to_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Module_to_xdc_runtime_IFilterLogger
#define LoggerRunMode_Handle_upCast3 ti_uia_loggers_LoggerRunMode_Handle_upCast3
#define LoggerRunMode_Handle_to_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Handle_to_xdc_runtime_IFilterLogger
#define LoggerRunMode_Handle_downCast3 ti_uia_loggers_LoggerRunMode_Handle_downCast3
#define LoggerRunMode_Handle_from_xdc_runtime_IFilterLogger ti_uia_loggers_LoggerRunMode_Handle_from_xdc_runtime_IFilterLogger
#define LoggerRunMode_Module_upCast4 ti_uia_loggers_LoggerRunMode_Module_upCast4
#define LoggerRunMode_Module_to_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Module_to_xdc_runtime_ILogger
#define LoggerRunMode_Handle_upCast4 ti_uia_loggers_LoggerRunMode_Handle_upCast4
#define LoggerRunMode_Handle_to_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Handle_to_xdc_runtime_ILogger
#define LoggerRunMode_Handle_downCast4 ti_uia_loggers_LoggerRunMode_Handle_downCast4
#define LoggerRunMode_Handle_from_xdc_runtime_ILogger ti_uia_loggers_LoggerRunMode_Handle_from_xdc_runtime_ILogger

#endif /* ti_uia_loggers_LoggerRunMode__localnames__done */
#endif
