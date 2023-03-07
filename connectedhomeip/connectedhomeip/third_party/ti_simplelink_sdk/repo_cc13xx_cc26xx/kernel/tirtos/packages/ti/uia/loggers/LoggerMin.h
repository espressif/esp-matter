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

#ifndef ti_uia_loggers_LoggerMin__include
#define ti_uia_loggers_LoggerMin__include

#ifndef __nested__
#define __nested__
#define ti_uia_loggers_LoggerMin__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_loggers_LoggerMin___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/loggers/LoggerMin__prologue.h>
#include <ti/uia/loggers/package/package.defs.h>

#include <xdc/runtime/ILogger.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/EventHdr.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* TimestampSize */
enum ti_uia_loggers_LoggerMin_TimestampSize {
    ti_uia_loggers_LoggerMin_TimestampSize_NONE = 0,
    ti_uia_loggers_LoggerMin_TimestampSize_32b = 1,
    ti_uia_loggers_LoggerMin_TimestampSize_64b = 2
};
typedef enum ti_uia_loggers_LoggerMin_TimestampSize ti_uia_loggers_LoggerMin_TimestampSize;


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Module_State */
typedef xdc_Char __T1_ti_uia_loggers_LoggerMin_Module_State__packetBuffer;
typedef xdc_Char *ARRAY1_ti_uia_loggers_LoggerMin_Module_State__packetBuffer;
typedef const xdc_Char *CARRAY1_ti_uia_loggers_LoggerMin_Module_State__packetBuffer;
typedef ARRAY1_ti_uia_loggers_LoggerMin_Module_State__packetBuffer __TA_ti_uia_loggers_LoggerMin_Module_State__packetBuffer;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_loggers_LoggerMin_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__diagsEnabled ti_uia_loggers_LoggerMin_Module__diagsEnabled__C;
#ifdef ti_uia_loggers_LoggerMin_Module__diagsEnabled__CR
#define ti_uia_loggers_LoggerMin_Module__diagsEnabled__C (*((CT__ti_uia_loggers_LoggerMin_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__diagsEnabled (ti_uia_loggers_LoggerMin_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_loggers_LoggerMin_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__diagsIncluded ti_uia_loggers_LoggerMin_Module__diagsIncluded__C;
#ifdef ti_uia_loggers_LoggerMin_Module__diagsIncluded__CR
#define ti_uia_loggers_LoggerMin_Module__diagsIncluded__C (*((CT__ti_uia_loggers_LoggerMin_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__diagsIncluded (ti_uia_loggers_LoggerMin_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_loggers_LoggerMin_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__diagsMask ti_uia_loggers_LoggerMin_Module__diagsMask__C;
#ifdef ti_uia_loggers_LoggerMin_Module__diagsMask__CR
#define ti_uia_loggers_LoggerMin_Module__diagsMask__C (*((CT__ti_uia_loggers_LoggerMin_Module__diagsMask*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsMask__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__diagsMask (ti_uia_loggers_LoggerMin_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerMin_Module__gateObj;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__gateObj ti_uia_loggers_LoggerMin_Module__gateObj__C;
#ifdef ti_uia_loggers_LoggerMin_Module__gateObj__CR
#define ti_uia_loggers_LoggerMin_Module__gateObj__C (*((CT__ti_uia_loggers_LoggerMin_Module__gateObj*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__gateObj__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__gateObj (ti_uia_loggers_LoggerMin_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerMin_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__gatePrms ti_uia_loggers_LoggerMin_Module__gatePrms__C;
#ifdef ti_uia_loggers_LoggerMin_Module__gatePrms__CR
#define ti_uia_loggers_LoggerMin_Module__gatePrms__C (*((CT__ti_uia_loggers_LoggerMin_Module__gatePrms*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__gatePrms__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__gatePrms (ti_uia_loggers_LoggerMin_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_loggers_LoggerMin_Module__id;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__id ti_uia_loggers_LoggerMin_Module__id__C;
#ifdef ti_uia_loggers_LoggerMin_Module__id__CR
#define ti_uia_loggers_LoggerMin_Module__id__C (*((CT__ti_uia_loggers_LoggerMin_Module__id*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__id__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__id (ti_uia_loggers_LoggerMin_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_loggers_LoggerMin_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerDefined ti_uia_loggers_LoggerMin_Module__loggerDefined__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerDefined__CR
#define ti_uia_loggers_LoggerMin_Module__loggerDefined__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerDefined__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerDefined (ti_uia_loggers_LoggerMin_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerMin_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerObj ti_uia_loggers_LoggerMin_Module__loggerObj__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerObj__CR
#define ti_uia_loggers_LoggerMin_Module__loggerObj__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerObj*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerObj__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerObj (ti_uia_loggers_LoggerMin_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_loggers_LoggerMin_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerFxn0 ti_uia_loggers_LoggerMin_Module__loggerFxn0__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn0__CR
#define ti_uia_loggers_LoggerMin_Module__loggerFxn0__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerFxn0 (ti_uia_loggers_LoggerMin_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_loggers_LoggerMin_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerFxn1 ti_uia_loggers_LoggerMin_Module__loggerFxn1__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn1__CR
#define ti_uia_loggers_LoggerMin_Module__loggerFxn1__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerFxn1 (ti_uia_loggers_LoggerMin_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_loggers_LoggerMin_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerFxn2 ti_uia_loggers_LoggerMin_Module__loggerFxn2__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn2__CR
#define ti_uia_loggers_LoggerMin_Module__loggerFxn2__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerFxn2 (ti_uia_loggers_LoggerMin_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_loggers_LoggerMin_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerFxn4 ti_uia_loggers_LoggerMin_Module__loggerFxn4__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn4__CR
#define ti_uia_loggers_LoggerMin_Module__loggerFxn4__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerFxn4 (ti_uia_loggers_LoggerMin_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_loggers_LoggerMin_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Module__loggerFxn8 ti_uia_loggers_LoggerMin_Module__loggerFxn8__C;
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn8__CR
#define ti_uia_loggers_LoggerMin_Module__loggerFxn8__C (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Module__loggerFxn8 (ti_uia_loggers_LoggerMin_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_loggers_LoggerMin_Object__count;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Object__count ti_uia_loggers_LoggerMin_Object__count__C;
#ifdef ti_uia_loggers_LoggerMin_Object__count__CR
#define ti_uia_loggers_LoggerMin_Object__count__C (*((CT__ti_uia_loggers_LoggerMin_Object__count*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Object__count__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Object__count (ti_uia_loggers_LoggerMin_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_loggers_LoggerMin_Object__heap;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Object__heap ti_uia_loggers_LoggerMin_Object__heap__C;
#ifdef ti_uia_loggers_LoggerMin_Object__heap__CR
#define ti_uia_loggers_LoggerMin_Object__heap__C (*((CT__ti_uia_loggers_LoggerMin_Object__heap*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Object__heap__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Object__heap (ti_uia_loggers_LoggerMin_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_loggers_LoggerMin_Object__sizeof;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Object__sizeof ti_uia_loggers_LoggerMin_Object__sizeof__C;
#ifdef ti_uia_loggers_LoggerMin_Object__sizeof__CR
#define ti_uia_loggers_LoggerMin_Object__sizeof__C (*((CT__ti_uia_loggers_LoggerMin_Object__sizeof*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Object__sizeof__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Object__sizeof (ti_uia_loggers_LoggerMin_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_loggers_LoggerMin_Object__table;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_Object__table ti_uia_loggers_LoggerMin_Object__table__C;
#ifdef ti_uia_loggers_LoggerMin_Object__table__CR
#define ti_uia_loggers_LoggerMin_Object__table__C (*((CT__ti_uia_loggers_LoggerMin_Object__table*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Object__table__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_Object__table (ti_uia_loggers_LoggerMin_Object__table__C)
#endif

/* bufSize */
typedef xdc_SizeT CT__ti_uia_loggers_LoggerMin_bufSize;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_bufSize ti_uia_loggers_LoggerMin_bufSize__C;
#ifdef ti_uia_loggers_LoggerMin_bufSize__CR
#define ti_uia_loggers_LoggerMin_bufSize (*((CT__ti_uia_loggers_LoggerMin_bufSize*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_bufSize__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerMin_bufSize__D
#define ti_uia_loggers_LoggerMin_bufSize (ti_uia_loggers_LoggerMin_bufSize__D)
#else
#define ti_uia_loggers_LoggerMin_bufSize (ti_uia_loggers_LoggerMin_bufSize__C)
#endif
#endif

/* numCores */
typedef xdc_Int CT__ti_uia_loggers_LoggerMin_numCores;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_numCores ti_uia_loggers_LoggerMin_numCores__C;
#ifdef ti_uia_loggers_LoggerMin_numCores__CR
#define ti_uia_loggers_LoggerMin_numCores (*((CT__ti_uia_loggers_LoggerMin_numCores*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_numCores__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerMin_numCores__D
#define ti_uia_loggers_LoggerMin_numCores (ti_uia_loggers_LoggerMin_numCores__D)
#else
#define ti_uia_loggers_LoggerMin_numCores (ti_uia_loggers_LoggerMin_numCores__C)
#endif
#endif

/* timestampSize */
typedef ti_uia_loggers_LoggerMin_TimestampSize CT__ti_uia_loggers_LoggerMin_timestampSize;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_timestampSize ti_uia_loggers_LoggerMin_timestampSize__C;
#ifdef ti_uia_loggers_LoggerMin_timestampSize__CR
#define ti_uia_loggers_LoggerMin_timestampSize (*((CT__ti_uia_loggers_LoggerMin_timestampSize*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_timestampSize__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_timestampSize (ti_uia_loggers_LoggerMin_timestampSize__C)
#endif

/* L_test */
typedef xdc_runtime_Log_Event CT__ti_uia_loggers_LoggerMin_L_test;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_L_test ti_uia_loggers_LoggerMin_L_test__C;
#ifdef ti_uia_loggers_LoggerMin_L_test__CR
#define ti_uia_loggers_LoggerMin_L_test (*((CT__ti_uia_loggers_LoggerMin_L_test*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_L_test__C_offset)))
#else
#define ti_uia_loggers_LoggerMin_L_test (ti_uia_loggers_LoggerMin_L_test__C)
#endif

/* supportLoggerDisable */
typedef xdc_Bool CT__ti_uia_loggers_LoggerMin_supportLoggerDisable;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_supportLoggerDisable ti_uia_loggers_LoggerMin_supportLoggerDisable__C;
#ifdef ti_uia_loggers_LoggerMin_supportLoggerDisable__CR
#define ti_uia_loggers_LoggerMin_supportLoggerDisable (*((CT__ti_uia_loggers_LoggerMin_supportLoggerDisable*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_supportLoggerDisable__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerMin_supportLoggerDisable__D
#define ti_uia_loggers_LoggerMin_supportLoggerDisable (ti_uia_loggers_LoggerMin_supportLoggerDisable__D)
#else
#define ti_uia_loggers_LoggerMin_supportLoggerDisable (ti_uia_loggers_LoggerMin_supportLoggerDisable__C)
#endif
#endif

/* endpointId */
typedef xdc_Bits16 CT__ti_uia_loggers_LoggerMin_endpointId;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_endpointId ti_uia_loggers_LoggerMin_endpointId__C;
#ifdef ti_uia_loggers_LoggerMin_endpointId__CR
#define ti_uia_loggers_LoggerMin_endpointId (*((CT__ti_uia_loggers_LoggerMin_endpointId*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_endpointId__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerMin_endpointId__D
#define ti_uia_loggers_LoggerMin_endpointId (ti_uia_loggers_LoggerMin_endpointId__D)
#else
#define ti_uia_loggers_LoggerMin_endpointId (ti_uia_loggers_LoggerMin_endpointId__C)
#endif
#endif

/* loggerInstanceId */
typedef xdc_Bits16 CT__ti_uia_loggers_LoggerMin_loggerInstanceId;
__extern __FAR__ const CT__ti_uia_loggers_LoggerMin_loggerInstanceId ti_uia_loggers_LoggerMin_loggerInstanceId__C;
#ifdef ti_uia_loggers_LoggerMin_loggerInstanceId__CR
#define ti_uia_loggers_LoggerMin_loggerInstanceId (*((CT__ti_uia_loggers_LoggerMin_loggerInstanceId*)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_loggerInstanceId__C_offset)))
#else
#ifdef ti_uia_loggers_LoggerMin_loggerInstanceId__D
#define ti_uia_loggers_LoggerMin_loggerInstanceId (ti_uia_loggers_LoggerMin_loggerInstanceId__D)
#else
#define ti_uia_loggers_LoggerMin_loggerInstanceId (ti_uia_loggers_LoggerMin_loggerInstanceId__C)
#endif
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_loggers_LoggerMin_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_uia_loggers_LoggerMin_Struct {
    const ti_uia_loggers_LoggerMin_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_loggers_LoggerMin_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(ti_uia_loggers_LoggerMin_Handle inst);
    xdc_Bool (*disable)(ti_uia_loggers_LoggerMin_Handle inst);
    xdc_Void (*write0)(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
    xdc_Void (*write1)(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
    xdc_Void (*write2)(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
    xdc_Void (*write4)(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
    xdc_Void (*write8)(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_loggers_LoggerMin_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_loggers_LoggerMin_Fxns__ ti_uia_loggers_LoggerMin_Module__FXNS__C;
#else
#define ti_uia_loggers_LoggerMin_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_loggers_LoggerMin_Module_startup ti_uia_loggers_LoggerMin_Module_startup__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_Module_startup__E, "ti_uia_loggers_LoggerMin_Module_startup")
__extern xdc_Int ti_uia_loggers_LoggerMin_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_uia_loggers_LoggerMin_Module_startup__F, "ti_uia_loggers_LoggerMin_Module_startup")
__extern xdc_Int ti_uia_loggers_LoggerMin_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Instance_init__E, "ti_uia_loggers_LoggerMin_Instance_init")
__extern xdc_Void ti_uia_loggers_LoggerMin_Instance_init__E(ti_uia_loggers_LoggerMin_Object *obj, const ti_uia_loggers_LoggerMin_Params *prms);

/* create */
xdc__CODESECT(ti_uia_loggers_LoggerMin_create, "ti_uia_loggers_LoggerMin_create")
__extern ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_create( const ti_uia_loggers_LoggerMin_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_uia_loggers_LoggerMin_construct, "ti_uia_loggers_LoggerMin_construct")
__extern void ti_uia_loggers_LoggerMin_construct(ti_uia_loggers_LoggerMin_Struct *obj, const ti_uia_loggers_LoggerMin_Params *prms);

/* delete */
xdc__CODESECT(ti_uia_loggers_LoggerMin_delete, "ti_uia_loggers_LoggerMin_delete")
__extern void ti_uia_loggers_LoggerMin_delete(ti_uia_loggers_LoggerMin_Handle *instp);

/* destruct */
xdc__CODESECT(ti_uia_loggers_LoggerMin_destruct, "ti_uia_loggers_LoggerMin_destruct")
__extern void ti_uia_loggers_LoggerMin_destruct(ti_uia_loggers_LoggerMin_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Handle__label__S, "ti_uia_loggers_LoggerMin_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_loggers_LoggerMin_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Module__startupDone__S, "ti_uia_loggers_LoggerMin_Module__startupDone__S")
__extern xdc_Bool ti_uia_loggers_LoggerMin_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Object__create__S, "ti_uia_loggers_LoggerMin_Object__create__S")
__extern xdc_Ptr ti_uia_loggers_LoggerMin_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Object__delete__S, "ti_uia_loggers_LoggerMin_Object__delete__S")
__extern xdc_Void ti_uia_loggers_LoggerMin_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Object__get__S, "ti_uia_loggers_LoggerMin_Object__get__S")
__extern xdc_Ptr ti_uia_loggers_LoggerMin_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Object__first__S, "ti_uia_loggers_LoggerMin_Object__first__S")
__extern xdc_Ptr ti_uia_loggers_LoggerMin_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Object__next__S, "ti_uia_loggers_LoggerMin_Object__next__S")
__extern xdc_Ptr ti_uia_loggers_LoggerMin_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_loggers_LoggerMin_Params__init__S, "ti_uia_loggers_LoggerMin_Params__init__S")
__extern xdc_Void ti_uia_loggers_LoggerMin_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* flush__E */
#define ti_uia_loggers_LoggerMin_flush ti_uia_loggers_LoggerMin_flush__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_flush__E, "ti_uia_loggers_LoggerMin_flush")
__extern xdc_Void ti_uia_loggers_LoggerMin_flush__E( void);

/* write__E */
#define ti_uia_loggers_LoggerMin_write ti_uia_loggers_LoggerMin_write__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write__E, "ti_uia_loggers_LoggerMin_write")
__extern xdc_Void ti_uia_loggers_LoggerMin_write__E( xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg numBytes, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);

/* write0__E */
#define ti_uia_loggers_LoggerMin_write0 ti_uia_loggers_LoggerMin_write0__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write0__E, "ti_uia_loggers_LoggerMin_write0")
__extern xdc_Void ti_uia_loggers_LoggerMin_write0__E( ti_uia_loggers_LoggerMin_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);

/* write1__E */
#define ti_uia_loggers_LoggerMin_write1 ti_uia_loggers_LoggerMin_write1__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write1__E, "ti_uia_loggers_LoggerMin_write1")
__extern xdc_Void ti_uia_loggers_LoggerMin_write1__E( ti_uia_loggers_LoggerMin_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);

/* write2__E */
#define ti_uia_loggers_LoggerMin_write2 ti_uia_loggers_LoggerMin_write2__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write2__E, "ti_uia_loggers_LoggerMin_write2")
__extern xdc_Void ti_uia_loggers_LoggerMin_write2__E( ti_uia_loggers_LoggerMin_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);

/* write4__E */
#define ti_uia_loggers_LoggerMin_write4 ti_uia_loggers_LoggerMin_write4__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write4__E, "ti_uia_loggers_LoggerMin_write4")
__extern xdc_Void ti_uia_loggers_LoggerMin_write4__E( ti_uia_loggers_LoggerMin_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);

/* write8__E */
#define ti_uia_loggers_LoggerMin_write8 ti_uia_loggers_LoggerMin_write8__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_write8__E, "ti_uia_loggers_LoggerMin_write8")
__extern xdc_Void ti_uia_loggers_LoggerMin_write8__E( ti_uia_loggers_LoggerMin_Handle instp, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);

/* enable__E */
#define ti_uia_loggers_LoggerMin_enable ti_uia_loggers_LoggerMin_enable__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_enable__E, "ti_uia_loggers_LoggerMin_enable")
__extern xdc_Bool ti_uia_loggers_LoggerMin_enable__E( ti_uia_loggers_LoggerMin_Handle instp);

/* disable__E */
#define ti_uia_loggers_LoggerMin_disable ti_uia_loggers_LoggerMin_disable__E
xdc__CODESECT(ti_uia_loggers_LoggerMin_disable__E, "ti_uia_loggers_LoggerMin_disable")
__extern xdc_Bool ti_uia_loggers_LoggerMin_disable__E( ti_uia_loggers_LoggerMin_Handle instp);

/* getContents__I */
#define ti_uia_loggers_LoggerMin_getContents ti_uia_loggers_LoggerMin_getContents__I
xdc__CODESECT(ti_uia_loggers_LoggerMin_getContents__I, "ti_uia_loggers_LoggerMin_getContents")
__extern xdc_Bool ti_uia_loggers_LoggerMin_getContents__I( ti_uia_loggers_LoggerMin_Object *obj, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT *cpSize);

/* isEmpty__I */
#define ti_uia_loggers_LoggerMin_isEmpty ti_uia_loggers_LoggerMin_isEmpty__I
xdc__CODESECT(ti_uia_loggers_LoggerMin_isEmpty__I, "ti_uia_loggers_LoggerMin_isEmpty")
__extern xdc_Bool ti_uia_loggers_LoggerMin_isEmpty__I( ti_uia_loggers_LoggerMin_Object *obj);

/* genTimestamp__I */
#define ti_uia_loggers_LoggerMin_genTimestamp ti_uia_loggers_LoggerMin_genTimestamp__I
xdc__CODESECT(ti_uia_loggers_LoggerMin_genTimestamp__I, "ti_uia_loggers_LoggerMin_genTimestamp")
__extern xdc_Ptr ti_uia_loggers_LoggerMin_genTimestamp__I( xdc_Ptr writePtr);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_ILogger_Module ti_uia_loggers_LoggerMin_Module_upCast(void);
static inline xdc_runtime_ILogger_Module ti_uia_loggers_LoggerMin_Module_upCast(void)
{
    return (xdc_runtime_ILogger_Module)&ti_uia_loggers_LoggerMin_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerMin_Module_to_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_ILogger_Handle ti_uia_loggers_LoggerMin_Handle_upCast(ti_uia_loggers_LoggerMin_Handle i);
static inline xdc_runtime_ILogger_Handle ti_uia_loggers_LoggerMin_Handle_upCast(ti_uia_loggers_LoggerMin_Handle i)
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerMin_Handle_to_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Handle_upCast

/* Handle_downCast */
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Handle_downCast(xdc_runtime_ILogger_Handle i);
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Handle_downCast(xdc_runtime_ILogger_Handle i)
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_uia_loggers_LoggerMin_Module__FXNS__C) ? (ti_uia_loggers_LoggerMin_Handle)i : (ti_uia_loggers_LoggerMin_Handle)NULL;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_uia_loggers_LoggerMin_Handle_from_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_loggers_LoggerMin_Module_startupDone() ti_uia_loggers_LoggerMin_Module__startupDone__S()

/* Object_heap */
#define ti_uia_loggers_LoggerMin_Object_heap() ti_uia_loggers_LoggerMin_Object__heap__C

/* Module_heap */
#define ti_uia_loggers_LoggerMin_Module_heap() ti_uia_loggers_LoggerMin_Object__heap__C

/* Module_id */
static inline CT__ti_uia_loggers_LoggerMin_Module__id ti_uia_loggers_LoggerMin_Module_id(void);
static inline CT__ti_uia_loggers_LoggerMin_Module__id ti_uia_loggers_LoggerMin_Module_id( void ) 
{
    return ti_uia_loggers_LoggerMin_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_loggers_LoggerMin_Module_hasMask(void);
static inline xdc_Bool ti_uia_loggers_LoggerMin_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_loggers_LoggerMin_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerMin_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_loggers_LoggerMin_Module_getMask(void);
static inline xdc_Bits16 ti_uia_loggers_LoggerMin_Module_getMask(void)
{
    return (ti_uia_loggers_LoggerMin_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerMin_Module__diagsMask)NULL) ? *ti_uia_loggers_LoggerMin_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_loggers_LoggerMin_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_loggers_LoggerMin_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_loggers_LoggerMin_Module__diagsMask__C != (CT__ti_uia_loggers_LoggerMin_Module__diagsMask)NULL) {
        *ti_uia_loggers_LoggerMin_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_uia_loggers_LoggerMin_Params_init(ti_uia_loggers_LoggerMin_Params *prms);
static inline void ti_uia_loggers_LoggerMin_Params_init( ti_uia_loggers_LoggerMin_Params *prms ) 
{
    if (prms != NULL) {
        ti_uia_loggers_LoggerMin_Params__init__S(prms, NULL, sizeof(ti_uia_loggers_LoggerMin_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_uia_loggers_LoggerMin_Params_copy(ti_uia_loggers_LoggerMin_Params *dst, const ti_uia_loggers_LoggerMin_Params *src);
static inline void ti_uia_loggers_LoggerMin_Params_copy(ti_uia_loggers_LoggerMin_Params *dst, const ti_uia_loggers_LoggerMin_Params *src) 
{
    if (dst != NULL) {
        ti_uia_loggers_LoggerMin_Params__init__S(dst, (const void *)src, sizeof(ti_uia_loggers_LoggerMin_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_uia_loggers_LoggerMin_Object_count() ti_uia_loggers_LoggerMin_Object__count__C

/* Object_sizeof */
#define ti_uia_loggers_LoggerMin_Object_sizeof() ti_uia_loggers_LoggerMin_Object__sizeof__C

/* Object_get */
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_get(ti_uia_loggers_LoggerMin_Object *oarr, int i);
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_get(ti_uia_loggers_LoggerMin_Object *oarr, int i) 
{
    return (ti_uia_loggers_LoggerMin_Handle)ti_uia_loggers_LoggerMin_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_first(void);
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_first(void)
{
    return (ti_uia_loggers_LoggerMin_Handle)ti_uia_loggers_LoggerMin_Object__first__S();
}

/* Object_next */
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_next(ti_uia_loggers_LoggerMin_Object *obj);
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_Object_next(ti_uia_loggers_LoggerMin_Object *obj)
{
    return (ti_uia_loggers_LoggerMin_Handle)ti_uia_loggers_LoggerMin_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_loggers_LoggerMin_Handle_label(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_loggers_LoggerMin_Handle_label(ti_uia_loggers_LoggerMin_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_uia_loggers_LoggerMin_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_uia_loggers_LoggerMin_Handle_name(ti_uia_loggers_LoggerMin_Handle inst);
static inline xdc_String ti_uia_loggers_LoggerMin_Handle_name(ti_uia_loggers_LoggerMin_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_uia_loggers_LoggerMin_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_handle(ti_uia_loggers_LoggerMin_Struct *str);
static inline ti_uia_loggers_LoggerMin_Handle ti_uia_loggers_LoggerMin_handle(ti_uia_loggers_LoggerMin_Struct *str)
{
    return (ti_uia_loggers_LoggerMin_Handle)str;
}

/* struct */
static inline ti_uia_loggers_LoggerMin_Struct *ti_uia_loggers_LoggerMin_struct(ti_uia_loggers_LoggerMin_Handle inst);
static inline ti_uia_loggers_LoggerMin_Struct *ti_uia_loggers_LoggerMin_struct(ti_uia_loggers_LoggerMin_Handle inst)
{
    return (ti_uia_loggers_LoggerMin_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/loggers/LoggerMin__epilogue.h>

#ifdef ti_uia_loggers_LoggerMin__top__
#undef __nested__
#endif

#endif /* ti_uia_loggers_LoggerMin__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_loggers_LoggerMin__internalaccess))

#ifndef ti_uia_loggers_LoggerMin__include_state
#define ti_uia_loggers_LoggerMin__include_state

/* Module_State */
struct ti_uia_loggers_LoggerMin_Module_State {
    xdc_Bool enabled;
    xdc_Bool empty;
    xdc_UInt16 numBytesInPrevEvent;
    xdc_UInt16 droppedEvents;
    __TA_ti_uia_loggers_LoggerMin_Module_State__packetBuffer packetBuffer;
    xdc_UInt32 *start;
    xdc_UInt32 *write;
    xdc_UInt32 *end;
    xdc_UInt16 eventSequenceNum;
    xdc_UInt16 pktSequenceNum;
    ti_uia_runtime_EventHdr_HdrType eventType;
};

/* Module__state__V */
#ifndef ti_uia_loggers_LoggerMin_Module__state__VR
extern struct ti_uia_loggers_LoggerMin_Module_State__ ti_uia_loggers_LoggerMin_Module__state__V;
#else
#define ti_uia_loggers_LoggerMin_Module__state__V (*((struct ti_uia_loggers_LoggerMin_Module_State__*)(xdcRomStatePtr + ti_uia_loggers_LoggerMin_Module__state__V_offset)))
#endif

/* Object */
struct ti_uia_loggers_LoggerMin_Object {
    const ti_uia_loggers_LoggerMin_Fxns__ *__fxns;
};

#endif /* ti_uia_loggers_LoggerMin__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_loggers_LoggerMin__nolocalnames)

#ifndef ti_uia_loggers_LoggerMin__localnames__done
#define ti_uia_loggers_LoggerMin__localnames__done

/* module prefix */
#define LoggerMin_Instance ti_uia_loggers_LoggerMin_Instance
#define LoggerMin_Handle ti_uia_loggers_LoggerMin_Handle
#define LoggerMin_Module ti_uia_loggers_LoggerMin_Module
#define LoggerMin_Object ti_uia_loggers_LoggerMin_Object
#define LoggerMin_Struct ti_uia_loggers_LoggerMin_Struct
#define LoggerMin_TimestampSize ti_uia_loggers_LoggerMin_TimestampSize
#define LoggerMin_Module_State ti_uia_loggers_LoggerMin_Module_State
#define LoggerMin_Instance_State ti_uia_loggers_LoggerMin_Instance_State
#define LoggerMin_TimestampSize_NONE ti_uia_loggers_LoggerMin_TimestampSize_NONE
#define LoggerMin_TimestampSize_32b ti_uia_loggers_LoggerMin_TimestampSize_32b
#define LoggerMin_TimestampSize_64b ti_uia_loggers_LoggerMin_TimestampSize_64b
#define LoggerMin_bufSize ti_uia_loggers_LoggerMin_bufSize
#define LoggerMin_numCores ti_uia_loggers_LoggerMin_numCores
#define LoggerMin_timestampSize ti_uia_loggers_LoggerMin_timestampSize
#define LoggerMin_L_test ti_uia_loggers_LoggerMin_L_test
#define LoggerMin_supportLoggerDisable ti_uia_loggers_LoggerMin_supportLoggerDisable
#define LoggerMin_endpointId ti_uia_loggers_LoggerMin_endpointId
#define LoggerMin_loggerInstanceId ti_uia_loggers_LoggerMin_loggerInstanceId
#define LoggerMin_Params ti_uia_loggers_LoggerMin_Params
#define LoggerMin_initBuffer ti_uia_loggers_LoggerMin_initBuffer
#define LoggerMin_flush ti_uia_loggers_LoggerMin_flush
#define LoggerMin_write ti_uia_loggers_LoggerMin_write
#define LoggerMin_write0 ti_uia_loggers_LoggerMin_write0
#define LoggerMin_write1 ti_uia_loggers_LoggerMin_write1
#define LoggerMin_write2 ti_uia_loggers_LoggerMin_write2
#define LoggerMin_write4 ti_uia_loggers_LoggerMin_write4
#define LoggerMin_write8 ti_uia_loggers_LoggerMin_write8
#define LoggerMin_enable ti_uia_loggers_LoggerMin_enable
#define LoggerMin_disable ti_uia_loggers_LoggerMin_disable
#define LoggerMin_Module_name ti_uia_loggers_LoggerMin_Module_name
#define LoggerMin_Module_id ti_uia_loggers_LoggerMin_Module_id
#define LoggerMin_Module_startup ti_uia_loggers_LoggerMin_Module_startup
#define LoggerMin_Module_startupDone ti_uia_loggers_LoggerMin_Module_startupDone
#define LoggerMin_Module_hasMask ti_uia_loggers_LoggerMin_Module_hasMask
#define LoggerMin_Module_getMask ti_uia_loggers_LoggerMin_Module_getMask
#define LoggerMin_Module_setMask ti_uia_loggers_LoggerMin_Module_setMask
#define LoggerMin_Object_heap ti_uia_loggers_LoggerMin_Object_heap
#define LoggerMin_Module_heap ti_uia_loggers_LoggerMin_Module_heap
#define LoggerMin_construct ti_uia_loggers_LoggerMin_construct
#define LoggerMin_create ti_uia_loggers_LoggerMin_create
#define LoggerMin_handle ti_uia_loggers_LoggerMin_handle
#define LoggerMin_struct ti_uia_loggers_LoggerMin_struct
#define LoggerMin_Handle_label ti_uia_loggers_LoggerMin_Handle_label
#define LoggerMin_Handle_name ti_uia_loggers_LoggerMin_Handle_name
#define LoggerMin_Instance_init ti_uia_loggers_LoggerMin_Instance_init
#define LoggerMin_Object_count ti_uia_loggers_LoggerMin_Object_count
#define LoggerMin_Object_get ti_uia_loggers_LoggerMin_Object_get
#define LoggerMin_Object_first ti_uia_loggers_LoggerMin_Object_first
#define LoggerMin_Object_next ti_uia_loggers_LoggerMin_Object_next
#define LoggerMin_Object_sizeof ti_uia_loggers_LoggerMin_Object_sizeof
#define LoggerMin_Params_copy ti_uia_loggers_LoggerMin_Params_copy
#define LoggerMin_Params_init ti_uia_loggers_LoggerMin_Params_init
#define LoggerMin_delete ti_uia_loggers_LoggerMin_delete
#define LoggerMin_destruct ti_uia_loggers_LoggerMin_destruct
#define LoggerMin_Module_upCast ti_uia_loggers_LoggerMin_Module_upCast
#define LoggerMin_Module_to_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Module_to_xdc_runtime_ILogger
#define LoggerMin_Handle_upCast ti_uia_loggers_LoggerMin_Handle_upCast
#define LoggerMin_Handle_to_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Handle_to_xdc_runtime_ILogger
#define LoggerMin_Handle_downCast ti_uia_loggers_LoggerMin_Handle_downCast
#define LoggerMin_Handle_from_xdc_runtime_ILogger ti_uia_loggers_LoggerMin_Handle_from_xdc_runtime_ILogger

#endif /* ti_uia_loggers_LoggerMin__localnames__done */
#endif
