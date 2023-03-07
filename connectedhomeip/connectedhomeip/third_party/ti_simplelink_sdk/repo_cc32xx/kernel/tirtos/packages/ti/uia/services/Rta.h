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

#ifndef ti_uia_services_Rta__include
#define ti_uia_services_Rta__include

#ifndef __nested__
#define __nested__
#define ti_uia_services_Rta__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_services_Rta___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/services/package/package.defs.h>

#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <ti/uia/runtime/ServiceMgr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/IUIATransfer.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Command */
enum ti_uia_services_Rta_Command {
    ti_uia_services_Rta_Command_READ_MASK = 0,
    ti_uia_services_Rta_Command_WRITE_MASK = 1,
    ti_uia_services_Rta_Command_LOGGER_OFF = 2,
    ti_uia_services_Rta_Command_LOGGER_ON = 3,
    ti_uia_services_Rta_Command_GET_CPU_SPEED = 4,
    ti_uia_services_Rta_Command_RESET_LOGGER = 5,
    ti_uia_services_Rta_Command_CHANGE_PERIOD = 6,
    ti_uia_services_Rta_Command_START_TX = 7,
    ti_uia_services_Rta_Command_STOP_TX = 8,
    ti_uia_services_Rta_Command_LOGGER_OFF_ALL = 9,
    ti_uia_services_Rta_Command_LOGGER_ON_ALL = 10,
    ti_uia_services_Rta_Command_RESET_LOGGER_ALL = 11,
    ti_uia_services_Rta_Command_SNAPSHOT_ALL = 12
};
typedef enum ti_uia_services_Rta_Command ti_uia_services_Rta_Command;

/* ErrorCode */
enum ti_uia_services_Rta_ErrorCode {
    ti_uia_services_Rta_ErrorCode_NULLPOINTER = 0
};
typedef enum ti_uia_services_Rta_ErrorCode ti_uia_services_Rta_ErrorCode;

/* Packet */
struct ti_uia_services_Rta_Packet {
    ti_uia_runtime_UIAPacket_Hdr hdr;
    xdc_Bits32 arg0;
    xdc_Bits32 arg1;
};

/* Module_State */
typedef ti_uia_runtime_IUIATransfer_Handle __T1_ti_uia_services_Rta_Module_State__loggers;
typedef ti_uia_runtime_IUIATransfer_Handle *ARRAY1_ti_uia_services_Rta_Module_State__loggers;
typedef const ti_uia_runtime_IUIATransfer_Handle *CARRAY1_ti_uia_services_Rta_Module_State__loggers;
typedef ARRAY1_ti_uia_services_Rta_Module_State__loggers __TA_ti_uia_services_Rta_Module_State__loggers;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_services_Rta_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__diagsEnabled ti_uia_services_Rta_Module__diagsEnabled__C;
#ifdef ti_uia_services_Rta_Module__diagsEnabled__CR
#define ti_uia_services_Rta_Module__diagsEnabled__C (*((CT__ti_uia_services_Rta_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_services_Rta_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_services_Rta_Module__diagsEnabled (ti_uia_services_Rta_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_services_Rta_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__diagsIncluded ti_uia_services_Rta_Module__diagsIncluded__C;
#ifdef ti_uia_services_Rta_Module__diagsIncluded__CR
#define ti_uia_services_Rta_Module__diagsIncluded__C (*((CT__ti_uia_services_Rta_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_services_Rta_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_services_Rta_Module__diagsIncluded (ti_uia_services_Rta_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_services_Rta_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__diagsMask ti_uia_services_Rta_Module__diagsMask__C;
#ifdef ti_uia_services_Rta_Module__diagsMask__CR
#define ti_uia_services_Rta_Module__diagsMask__C (*((CT__ti_uia_services_Rta_Module__diagsMask*)(xdcRomConstPtr + ti_uia_services_Rta_Module__diagsMask__C_offset)))
#else
#define ti_uia_services_Rta_Module__diagsMask (ti_uia_services_Rta_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_services_Rta_Module__gateObj;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__gateObj ti_uia_services_Rta_Module__gateObj__C;
#ifdef ti_uia_services_Rta_Module__gateObj__CR
#define ti_uia_services_Rta_Module__gateObj__C (*((CT__ti_uia_services_Rta_Module__gateObj*)(xdcRomConstPtr + ti_uia_services_Rta_Module__gateObj__C_offset)))
#else
#define ti_uia_services_Rta_Module__gateObj (ti_uia_services_Rta_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_services_Rta_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__gatePrms ti_uia_services_Rta_Module__gatePrms__C;
#ifdef ti_uia_services_Rta_Module__gatePrms__CR
#define ti_uia_services_Rta_Module__gatePrms__C (*((CT__ti_uia_services_Rta_Module__gatePrms*)(xdcRomConstPtr + ti_uia_services_Rta_Module__gatePrms__C_offset)))
#else
#define ti_uia_services_Rta_Module__gatePrms (ti_uia_services_Rta_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_services_Rta_Module__id;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__id ti_uia_services_Rta_Module__id__C;
#ifdef ti_uia_services_Rta_Module__id__CR
#define ti_uia_services_Rta_Module__id__C (*((CT__ti_uia_services_Rta_Module__id*)(xdcRomConstPtr + ti_uia_services_Rta_Module__id__C_offset)))
#else
#define ti_uia_services_Rta_Module__id (ti_uia_services_Rta_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_services_Rta_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerDefined ti_uia_services_Rta_Module__loggerDefined__C;
#ifdef ti_uia_services_Rta_Module__loggerDefined__CR
#define ti_uia_services_Rta_Module__loggerDefined__C (*((CT__ti_uia_services_Rta_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerDefined__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerDefined (ti_uia_services_Rta_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_services_Rta_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerObj ti_uia_services_Rta_Module__loggerObj__C;
#ifdef ti_uia_services_Rta_Module__loggerObj__CR
#define ti_uia_services_Rta_Module__loggerObj__C (*((CT__ti_uia_services_Rta_Module__loggerObj*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerObj__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerObj (ti_uia_services_Rta_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_services_Rta_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerFxn0 ti_uia_services_Rta_Module__loggerFxn0__C;
#ifdef ti_uia_services_Rta_Module__loggerFxn0__CR
#define ti_uia_services_Rta_Module__loggerFxn0__C (*((CT__ti_uia_services_Rta_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerFxn0 (ti_uia_services_Rta_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_services_Rta_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerFxn1 ti_uia_services_Rta_Module__loggerFxn1__C;
#ifdef ti_uia_services_Rta_Module__loggerFxn1__CR
#define ti_uia_services_Rta_Module__loggerFxn1__C (*((CT__ti_uia_services_Rta_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerFxn1 (ti_uia_services_Rta_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_services_Rta_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerFxn2 ti_uia_services_Rta_Module__loggerFxn2__C;
#ifdef ti_uia_services_Rta_Module__loggerFxn2__CR
#define ti_uia_services_Rta_Module__loggerFxn2__C (*((CT__ti_uia_services_Rta_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerFxn2 (ti_uia_services_Rta_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_services_Rta_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerFxn4 ti_uia_services_Rta_Module__loggerFxn4__C;
#ifdef ti_uia_services_Rta_Module__loggerFxn4__CR
#define ti_uia_services_Rta_Module__loggerFxn4__C (*((CT__ti_uia_services_Rta_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerFxn4 (ti_uia_services_Rta_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_services_Rta_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_services_Rta_Module__loggerFxn8 ti_uia_services_Rta_Module__loggerFxn8__C;
#ifdef ti_uia_services_Rta_Module__loggerFxn8__CR
#define ti_uia_services_Rta_Module__loggerFxn8__C (*((CT__ti_uia_services_Rta_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_services_Rta_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_services_Rta_Module__loggerFxn8 (ti_uia_services_Rta_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_services_Rta_Object__count;
__extern __FAR__ const CT__ti_uia_services_Rta_Object__count ti_uia_services_Rta_Object__count__C;
#ifdef ti_uia_services_Rta_Object__count__CR
#define ti_uia_services_Rta_Object__count__C (*((CT__ti_uia_services_Rta_Object__count*)(xdcRomConstPtr + ti_uia_services_Rta_Object__count__C_offset)))
#else
#define ti_uia_services_Rta_Object__count (ti_uia_services_Rta_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_services_Rta_Object__heap;
__extern __FAR__ const CT__ti_uia_services_Rta_Object__heap ti_uia_services_Rta_Object__heap__C;
#ifdef ti_uia_services_Rta_Object__heap__CR
#define ti_uia_services_Rta_Object__heap__C (*((CT__ti_uia_services_Rta_Object__heap*)(xdcRomConstPtr + ti_uia_services_Rta_Object__heap__C_offset)))
#else
#define ti_uia_services_Rta_Object__heap (ti_uia_services_Rta_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_services_Rta_Object__sizeof;
__extern __FAR__ const CT__ti_uia_services_Rta_Object__sizeof ti_uia_services_Rta_Object__sizeof__C;
#ifdef ti_uia_services_Rta_Object__sizeof__CR
#define ti_uia_services_Rta_Object__sizeof__C (*((CT__ti_uia_services_Rta_Object__sizeof*)(xdcRomConstPtr + ti_uia_services_Rta_Object__sizeof__C_offset)))
#else
#define ti_uia_services_Rta_Object__sizeof (ti_uia_services_Rta_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_services_Rta_Object__table;
__extern __FAR__ const CT__ti_uia_services_Rta_Object__table ti_uia_services_Rta_Object__table__C;
#ifdef ti_uia_services_Rta_Object__table__CR
#define ti_uia_services_Rta_Object__table__C (*((CT__ti_uia_services_Rta_Object__table*)(xdcRomConstPtr + ti_uia_services_Rta_Object__table__C_offset)))
#else
#define ti_uia_services_Rta_Object__table (ti_uia_services_Rta_Object__table__C)
#endif

/* LD_recordsSent */
typedef xdc_runtime_Log_Event CT__ti_uia_services_Rta_LD_recordsSent;
__extern __FAR__ const CT__ti_uia_services_Rta_LD_recordsSent ti_uia_services_Rta_LD_recordsSent__C;
#ifdef ti_uia_services_Rta_LD_recordsSent__CR
#define ti_uia_services_Rta_LD_recordsSent (*((CT__ti_uia_services_Rta_LD_recordsSent*)(xdcRomConstPtr + ti_uia_services_Rta_LD_recordsSent__C_offset)))
#else
#define ti_uia_services_Rta_LD_recordsSent (ti_uia_services_Rta_LD_recordsSent__C)
#endif

/* LD_cmdRcvd */
typedef xdc_runtime_Log_Event CT__ti_uia_services_Rta_LD_cmdRcvd;
__extern __FAR__ const CT__ti_uia_services_Rta_LD_cmdRcvd ti_uia_services_Rta_LD_cmdRcvd__C;
#ifdef ti_uia_services_Rta_LD_cmdRcvd__CR
#define ti_uia_services_Rta_LD_cmdRcvd (*((CT__ti_uia_services_Rta_LD_cmdRcvd*)(xdcRomConstPtr + ti_uia_services_Rta_LD_cmdRcvd__C_offset)))
#else
#define ti_uia_services_Rta_LD_cmdRcvd (ti_uia_services_Rta_LD_cmdRcvd__C)
#endif

/* LD_writeMask */
typedef xdc_runtime_Log_Event CT__ti_uia_services_Rta_LD_writeMask;
__extern __FAR__ const CT__ti_uia_services_Rta_LD_writeMask ti_uia_services_Rta_LD_writeMask__C;
#ifdef ti_uia_services_Rta_LD_writeMask__CR
#define ti_uia_services_Rta_LD_writeMask (*((CT__ti_uia_services_Rta_LD_writeMask*)(xdcRomConstPtr + ti_uia_services_Rta_LD_writeMask__C_offset)))
#else
#define ti_uia_services_Rta_LD_writeMask (ti_uia_services_Rta_LD_writeMask__C)
#endif

/* periodInMs */
typedef xdc_Int CT__ti_uia_services_Rta_periodInMs;
__extern __FAR__ const CT__ti_uia_services_Rta_periodInMs ti_uia_services_Rta_periodInMs__C;
#ifdef ti_uia_services_Rta_periodInMs__CR
#define ti_uia_services_Rta_periodInMs (*((CT__ti_uia_services_Rta_periodInMs*)(xdcRomConstPtr + ti_uia_services_Rta_periodInMs__C_offset)))
#else
#ifdef ti_uia_services_Rta_periodInMs__D
#define ti_uia_services_Rta_periodInMs (ti_uia_services_Rta_periodInMs__D)
#else
#define ti_uia_services_Rta_periodInMs (ti_uia_services_Rta_periodInMs__C)
#endif
#endif

/* SERVICEID */
typedef ti_uia_runtime_ServiceMgr_ServiceId CT__ti_uia_services_Rta_SERVICEID;
__extern __FAR__ const CT__ti_uia_services_Rta_SERVICEID ti_uia_services_Rta_SERVICEID__C;
#ifdef ti_uia_services_Rta_SERVICEID__CR
#define ti_uia_services_Rta_SERVICEID (*((CT__ti_uia_services_Rta_SERVICEID*)(xdcRomConstPtr + ti_uia_services_Rta_SERVICEID__C_offset)))
#else
#define ti_uia_services_Rta_SERVICEID (ti_uia_services_Rta_SERVICEID__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_services_Rta_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_services_Rta_Module__startupDone__S, "ti_uia_services_Rta_Module__startupDone__S")
__extern xdc_Bool ti_uia_services_Rta_Module__startupDone__S( void);

/* processCallback__E */
#define ti_uia_services_Rta_processCallback ti_uia_services_Rta_processCallback__E
xdc__CODESECT(ti_uia_services_Rta_processCallback__E, "ti_uia_services_Rta_processCallback")
__extern xdc_Void ti_uia_services_Rta_processCallback__E( ti_uia_runtime_ServiceMgr_Reason reason, ti_uia_runtime_UIAPacket_Hdr *cmd);
xdc__CODESECT(ti_uia_services_Rta_processCallback__F, "ti_uia_services_Rta_processCallback")
__extern xdc_Void ti_uia_services_Rta_processCallback__F( ti_uia_runtime_ServiceMgr_Reason reason, ti_uia_runtime_UIAPacket_Hdr *cmd);

/* disableAllLogs__E */
#define ti_uia_services_Rta_disableAllLogs ti_uia_services_Rta_disableAllLogs__E
xdc__CODESECT(ti_uia_services_Rta_disableAllLogs__E, "ti_uia_services_Rta_disableAllLogs")
__extern xdc_Void ti_uia_services_Rta_disableAllLogs__E( void);
xdc__CODESECT(ti_uia_services_Rta_disableAllLogs__F, "ti_uia_services_Rta_disableAllLogs")
__extern xdc_Void ti_uia_services_Rta_disableAllLogs__F( void);

/* enableAllLogs__E */
#define ti_uia_services_Rta_enableAllLogs ti_uia_services_Rta_enableAllLogs__E
xdc__CODESECT(ti_uia_services_Rta_enableAllLogs__E, "ti_uia_services_Rta_enableAllLogs")
__extern xdc_Void ti_uia_services_Rta_enableAllLogs__E( void);
xdc__CODESECT(ti_uia_services_Rta_enableAllLogs__F, "ti_uia_services_Rta_enableAllLogs")
__extern xdc_Void ti_uia_services_Rta_enableAllLogs__F( void);

/* snapshotAllLogs__E */
#define ti_uia_services_Rta_snapshotAllLogs ti_uia_services_Rta_snapshotAllLogs__E
xdc__CODESECT(ti_uia_services_Rta_snapshotAllLogs__E, "ti_uia_services_Rta_snapshotAllLogs")
__extern xdc_Void ti_uia_services_Rta_snapshotAllLogs__E( xdc_UArg reset, xdc_UArg waitPeriod);
xdc__CODESECT(ti_uia_services_Rta_snapshotAllLogs__F, "ti_uia_services_Rta_snapshotAllLogs")
__extern xdc_Void ti_uia_services_Rta_snapshotAllLogs__F( xdc_UArg reset, xdc_UArg waitPeriod);

/* resetAllLogs__E */
#define ti_uia_services_Rta_resetAllLogs ti_uia_services_Rta_resetAllLogs__E
xdc__CODESECT(ti_uia_services_Rta_resetAllLogs__E, "ti_uia_services_Rta_resetAllLogs")
__extern xdc_Void ti_uia_services_Rta_resetAllLogs__E( void);
xdc__CODESECT(ti_uia_services_Rta_resetAllLogs__F, "ti_uia_services_Rta_resetAllLogs")
__extern xdc_Void ti_uia_services_Rta_resetAllLogs__F( void);

/* startDataTx__E */
#define ti_uia_services_Rta_startDataTx ti_uia_services_Rta_startDataTx__E
xdc__CODESECT(ti_uia_services_Rta_startDataTx__E, "ti_uia_services_Rta_startDataTx")
__extern xdc_Void ti_uia_services_Rta_startDataTx__E( void);
xdc__CODESECT(ti_uia_services_Rta_startDataTx__F, "ti_uia_services_Rta_startDataTx")
__extern xdc_Void ti_uia_services_Rta_startDataTx__F( void);

/* stopDataTx__E */
#define ti_uia_services_Rta_stopDataTx ti_uia_services_Rta_stopDataTx__E
xdc__CODESECT(ti_uia_services_Rta_stopDataTx__E, "ti_uia_services_Rta_stopDataTx")
__extern xdc_Void ti_uia_services_Rta_stopDataTx__E( void);
xdc__CODESECT(ti_uia_services_Rta_stopDataTx__F, "ti_uia_services_Rta_stopDataTx")
__extern xdc_Void ti_uia_services_Rta_stopDataTx__F( void);

/* sendEvents__I */
#define ti_uia_services_Rta_sendEvents ti_uia_services_Rta_sendEvents__I
xdc__CODESECT(ti_uia_services_Rta_sendEvents__I, "ti_uia_services_Rta_sendEvents")
__extern xdc_Void ti_uia_services_Rta_sendEvents__I( void);

/* processMsg__I */
#define ti_uia_services_Rta_processMsg ti_uia_services_Rta_processMsg__I
xdc__CODESECT(ti_uia_services_Rta_processMsg__I, "ti_uia_services_Rta_processMsg")
__extern xdc_Void ti_uia_services_Rta_processMsg__I( ti_uia_runtime_UIAPacket_Hdr *cmd);

/* flushLogger__I */
#define ti_uia_services_Rta_flushLogger ti_uia_services_Rta_flushLogger__I
xdc__CODESECT(ti_uia_services_Rta_flushLogger__I, "ti_uia_services_Rta_flushLogger")
__extern xdc_Void ti_uia_services_Rta_flushLogger__I( ti_uia_runtime_IUIATransfer_Handle logger, xdc_UInt loggerNum);

/* acknowledgeCmd__I */
#define ti_uia_services_Rta_acknowledgeCmd ti_uia_services_Rta_acknowledgeCmd__I
xdc__CODESECT(ti_uia_services_Rta_acknowledgeCmd__I, "ti_uia_services_Rta_acknowledgeCmd")
__extern xdc_Void ti_uia_services_Rta_acknowledgeCmd__I( ti_uia_services_Rta_Packet *resp);

/* readMask__I */
#define ti_uia_services_Rta_readMask ti_uia_services_Rta_readMask__I
xdc__CODESECT(ti_uia_services_Rta_readMask__I, "ti_uia_services_Rta_readMask")
__extern ti_uia_runtime_UIAPacket_MsgType ti_uia_services_Rta_readMask__I( ti_uia_services_Rta_Packet *resp, xdc_UArg addr);

/* writeMask__I */
#define ti_uia_services_Rta_writeMask ti_uia_services_Rta_writeMask__I
xdc__CODESECT(ti_uia_services_Rta_writeMask__I, "ti_uia_services_Rta_writeMask")
__extern ti_uia_runtime_UIAPacket_MsgType ti_uia_services_Rta_writeMask__I( ti_uia_services_Rta_Packet *resp, xdc_UArg addr, xdc_UArg val);

/* enableLog__I */
#define ti_uia_services_Rta_enableLog ti_uia_services_Rta_enableLog__I
xdc__CODESECT(ti_uia_services_Rta_enableLog__I, "ti_uia_services_Rta_enableLog")
__extern xdc_Void ti_uia_services_Rta_enableLog__I( xdc_UArg log);

/* disableLog__I */
#define ti_uia_services_Rta_disableLog ti_uia_services_Rta_disableLog__I
xdc__CODESECT(ti_uia_services_Rta_disableLog__I, "ti_uia_services_Rta_disableLog")
__extern xdc_Void ti_uia_services_Rta_disableLog__I( xdc_UArg log);

/* getCpuSpeed__I */
#define ti_uia_services_Rta_getCpuSpeed ti_uia_services_Rta_getCpuSpeed__I
xdc__CODESECT(ti_uia_services_Rta_getCpuSpeed__I, "ti_uia_services_Rta_getCpuSpeed")
__extern xdc_Void ti_uia_services_Rta_getCpuSpeed__I( ti_uia_services_Rta_Packet *resp);

/* resetLog__I */
#define ti_uia_services_Rta_resetLog ti_uia_services_Rta_resetLog__I
xdc__CODESECT(ti_uia_services_Rta_resetLog__I, "ti_uia_services_Rta_resetLog")
__extern xdc_Void ti_uia_services_Rta_resetLog__I( xdc_UArg log);

/* changePeriod__I */
#define ti_uia_services_Rta_changePeriod ti_uia_services_Rta_changePeriod__I
xdc__CODESECT(ti_uia_services_Rta_changePeriod__I, "ti_uia_services_Rta_changePeriod")
__extern xdc_Void ti_uia_services_Rta_changePeriod__I( xdc_UArg period);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_services_Rta_Module_startupDone() ti_uia_services_Rta_Module__startupDone__S()

/* Object_heap */
#define ti_uia_services_Rta_Object_heap() ti_uia_services_Rta_Object__heap__C

/* Module_heap */
#define ti_uia_services_Rta_Module_heap() ti_uia_services_Rta_Object__heap__C

/* Module_id */
static inline CT__ti_uia_services_Rta_Module__id ti_uia_services_Rta_Module_id(void);
static inline CT__ti_uia_services_Rta_Module__id ti_uia_services_Rta_Module_id( void ) 
{
    return ti_uia_services_Rta_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_services_Rta_Module_hasMask(void);
static inline xdc_Bool ti_uia_services_Rta_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_services_Rta_Module__diagsMask__C != (CT__ti_uia_services_Rta_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_services_Rta_Module_getMask(void);
static inline xdc_Bits16 ti_uia_services_Rta_Module_getMask(void)
{
    return (ti_uia_services_Rta_Module__diagsMask__C != (CT__ti_uia_services_Rta_Module__diagsMask)NULL) ? *ti_uia_services_Rta_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_services_Rta_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_services_Rta_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_services_Rta_Module__diagsMask__C != (CT__ti_uia_services_Rta_Module__diagsMask)NULL) {
        *ti_uia_services_Rta_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_services_Rta__top__
#undef __nested__
#endif

#endif /* ti_uia_services_Rta__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_services_Rta__internalaccess))

#ifndef ti_uia_services_Rta__include_state
#define ti_uia_services_Rta__include_state

/* Module_State */
struct ti_uia_services_Rta_Module_State {
    __TA_ti_uia_services_Rta_Module_State__loggers loggers;
    xdc_UInt numLoggers;
    xdc_UInt totalPacketsSent;
    xdc_Int period;
    xdc_Bits16 seq;
    xdc_Bool txData;
    xdc_Bool snapshot;
};

/* Module__state__V */
#ifndef ti_uia_services_Rta_Module__state__VR
extern struct ti_uia_services_Rta_Module_State__ ti_uia_services_Rta_Module__state__V;
#else
#define ti_uia_services_Rta_Module__state__V (*((struct ti_uia_services_Rta_Module_State__*)(xdcRomStatePtr + ti_uia_services_Rta_Module__state__V_offset)))
#endif

#endif /* ti_uia_services_Rta__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_services_Rta__nolocalnames)

#ifndef ti_uia_services_Rta__localnames__done
#define ti_uia_services_Rta__localnames__done

/* module prefix */
#define Rta_Command ti_uia_services_Rta_Command
#define Rta_ErrorCode ti_uia_services_Rta_ErrorCode
#define Rta_Packet ti_uia_services_Rta_Packet
#define Rta_Module_State ti_uia_services_Rta_Module_State
#define Rta_Command_READ_MASK ti_uia_services_Rta_Command_READ_MASK
#define Rta_Command_WRITE_MASK ti_uia_services_Rta_Command_WRITE_MASK
#define Rta_Command_LOGGER_OFF ti_uia_services_Rta_Command_LOGGER_OFF
#define Rta_Command_LOGGER_ON ti_uia_services_Rta_Command_LOGGER_ON
#define Rta_Command_GET_CPU_SPEED ti_uia_services_Rta_Command_GET_CPU_SPEED
#define Rta_Command_RESET_LOGGER ti_uia_services_Rta_Command_RESET_LOGGER
#define Rta_Command_CHANGE_PERIOD ti_uia_services_Rta_Command_CHANGE_PERIOD
#define Rta_Command_START_TX ti_uia_services_Rta_Command_START_TX
#define Rta_Command_STOP_TX ti_uia_services_Rta_Command_STOP_TX
#define Rta_Command_LOGGER_OFF_ALL ti_uia_services_Rta_Command_LOGGER_OFF_ALL
#define Rta_Command_LOGGER_ON_ALL ti_uia_services_Rta_Command_LOGGER_ON_ALL
#define Rta_Command_RESET_LOGGER_ALL ti_uia_services_Rta_Command_RESET_LOGGER_ALL
#define Rta_Command_SNAPSHOT_ALL ti_uia_services_Rta_Command_SNAPSHOT_ALL
#define Rta_ErrorCode_NULLPOINTER ti_uia_services_Rta_ErrorCode_NULLPOINTER
#define Rta_LD_recordsSent ti_uia_services_Rta_LD_recordsSent
#define Rta_LD_cmdRcvd ti_uia_services_Rta_LD_cmdRcvd
#define Rta_LD_writeMask ti_uia_services_Rta_LD_writeMask
#define Rta_periodInMs ti_uia_services_Rta_periodInMs
#define Rta_SERVICEID ti_uia_services_Rta_SERVICEID
#define Rta_processCallback ti_uia_services_Rta_processCallback
#define Rta_disableAllLogs ti_uia_services_Rta_disableAllLogs
#define Rta_enableAllLogs ti_uia_services_Rta_enableAllLogs
#define Rta_snapshotAllLogs ti_uia_services_Rta_snapshotAllLogs
#define Rta_resetAllLogs ti_uia_services_Rta_resetAllLogs
#define Rta_startDataTx ti_uia_services_Rta_startDataTx
#define Rta_stopDataTx ti_uia_services_Rta_stopDataTx
#define Rta_Module_name ti_uia_services_Rta_Module_name
#define Rta_Module_id ti_uia_services_Rta_Module_id
#define Rta_Module_startup ti_uia_services_Rta_Module_startup
#define Rta_Module_startupDone ti_uia_services_Rta_Module_startupDone
#define Rta_Module_hasMask ti_uia_services_Rta_Module_hasMask
#define Rta_Module_getMask ti_uia_services_Rta_Module_getMask
#define Rta_Module_setMask ti_uia_services_Rta_Module_setMask
#define Rta_Object_heap ti_uia_services_Rta_Object_heap
#define Rta_Module_heap ti_uia_services_Rta_Module_heap

#endif /* ti_uia_services_Rta__localnames__done */
#endif
