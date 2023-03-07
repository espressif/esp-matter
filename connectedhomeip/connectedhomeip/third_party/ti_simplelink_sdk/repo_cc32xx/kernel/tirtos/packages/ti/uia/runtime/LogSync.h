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

#ifndef ti_uia_runtime_LogSync__include
#define ti_uia_runtime_LogSync__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_LogSync__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_LogSync___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/runtime/LogSync__prologue.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/ILogger.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Text.h>
#include <ti/uia/events/IUIAMetaProvider.h>
#include <ti/uia/runtime/IUIATraceSyncClient.h>
#include <xdc/runtime/ITimestampClient.h>
#include <ti/uia/runtime/package/LogSync_CpuTimestampProxy.h>
#include <ti/uia/runtime/package/LogSync_GlobalTimestampProxy.h>


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
typedef xdc_Bits32 CT__ti_uia_runtime_LogSync_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__diagsEnabled ti_uia_runtime_LogSync_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_LogSync_Module__diagsEnabled__CR
#define ti_uia_runtime_LogSync_Module__diagsEnabled__C (*((CT__ti_uia_runtime_LogSync_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__diagsEnabled (ti_uia_runtime_LogSync_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_LogSync_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__diagsIncluded ti_uia_runtime_LogSync_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_LogSync_Module__diagsIncluded__CR
#define ti_uia_runtime_LogSync_Module__diagsIncluded__C (*((CT__ti_uia_runtime_LogSync_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__diagsIncluded (ti_uia_runtime_LogSync_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_LogSync_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__diagsMask ti_uia_runtime_LogSync_Module__diagsMask__C;
#ifdef ti_uia_runtime_LogSync_Module__diagsMask__CR
#define ti_uia_runtime_LogSync_Module__diagsMask__C (*((CT__ti_uia_runtime_LogSync_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__diagsMask (ti_uia_runtime_LogSync_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_LogSync_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__gateObj ti_uia_runtime_LogSync_Module__gateObj__C;
#ifdef ti_uia_runtime_LogSync_Module__gateObj__CR
#define ti_uia_runtime_LogSync_Module__gateObj__C (*((CT__ti_uia_runtime_LogSync_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__gateObj (ti_uia_runtime_LogSync_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_LogSync_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__gatePrms ti_uia_runtime_LogSync_Module__gatePrms__C;
#ifdef ti_uia_runtime_LogSync_Module__gatePrms__CR
#define ti_uia_runtime_LogSync_Module__gatePrms__C (*((CT__ti_uia_runtime_LogSync_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__gatePrms (ti_uia_runtime_LogSync_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_LogSync_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__id ti_uia_runtime_LogSync_Module__id__C;
#ifdef ti_uia_runtime_LogSync_Module__id__CR
#define ti_uia_runtime_LogSync_Module__id__C (*((CT__ti_uia_runtime_LogSync_Module__id*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__id__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__id (ti_uia_runtime_LogSync_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_LogSync_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerDefined ti_uia_runtime_LogSync_Module__loggerDefined__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerDefined__CR
#define ti_uia_runtime_LogSync_Module__loggerDefined__C (*((CT__ti_uia_runtime_LogSync_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerDefined (ti_uia_runtime_LogSync_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_LogSync_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerObj ti_uia_runtime_LogSync_Module__loggerObj__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerObj__CR
#define ti_uia_runtime_LogSync_Module__loggerObj__C (*((CT__ti_uia_runtime_LogSync_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerObj (ti_uia_runtime_LogSync_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_LogSync_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerFxn0 ti_uia_runtime_LogSync_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerFxn0__CR
#define ti_uia_runtime_LogSync_Module__loggerFxn0__C (*((CT__ti_uia_runtime_LogSync_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerFxn0 (ti_uia_runtime_LogSync_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_LogSync_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerFxn1 ti_uia_runtime_LogSync_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerFxn1__CR
#define ti_uia_runtime_LogSync_Module__loggerFxn1__C (*((CT__ti_uia_runtime_LogSync_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerFxn1 (ti_uia_runtime_LogSync_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_LogSync_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerFxn2 ti_uia_runtime_LogSync_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerFxn2__CR
#define ti_uia_runtime_LogSync_Module__loggerFxn2__C (*((CT__ti_uia_runtime_LogSync_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerFxn2 (ti_uia_runtime_LogSync_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_LogSync_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerFxn4 ti_uia_runtime_LogSync_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerFxn4__CR
#define ti_uia_runtime_LogSync_Module__loggerFxn4__C (*((CT__ti_uia_runtime_LogSync_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerFxn4 (ti_uia_runtime_LogSync_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_LogSync_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Module__loggerFxn8 ti_uia_runtime_LogSync_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_LogSync_Module__loggerFxn8__CR
#define ti_uia_runtime_LogSync_Module__loggerFxn8__C (*((CT__ti_uia_runtime_LogSync_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_LogSync_Module__loggerFxn8 (ti_uia_runtime_LogSync_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_LogSync_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Object__count ti_uia_runtime_LogSync_Object__count__C;
#ifdef ti_uia_runtime_LogSync_Object__count__CR
#define ti_uia_runtime_LogSync_Object__count__C (*((CT__ti_uia_runtime_LogSync_Object__count*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Object__count__C_offset)))
#else
#define ti_uia_runtime_LogSync_Object__count (ti_uia_runtime_LogSync_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_LogSync_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Object__heap ti_uia_runtime_LogSync_Object__heap__C;
#ifdef ti_uia_runtime_LogSync_Object__heap__CR
#define ti_uia_runtime_LogSync_Object__heap__C (*((CT__ti_uia_runtime_LogSync_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Object__heap__C_offset)))
#else
#define ti_uia_runtime_LogSync_Object__heap (ti_uia_runtime_LogSync_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_LogSync_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Object__sizeof ti_uia_runtime_LogSync_Object__sizeof__C;
#ifdef ti_uia_runtime_LogSync_Object__sizeof__CR
#define ti_uia_runtime_LogSync_Object__sizeof__C (*((CT__ti_uia_runtime_LogSync_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_LogSync_Object__sizeof (ti_uia_runtime_LogSync_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_LogSync_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_Object__table ti_uia_runtime_LogSync_Object__table__C;
#ifdef ti_uia_runtime_LogSync_Object__table__CR
#define ti_uia_runtime_LogSync_Object__table__C (*((CT__ti_uia_runtime_LogSync_Object__table*)(xdcRomConstPtr + ti_uia_runtime_LogSync_Object__table__C_offset)))
#else
#define ti_uia_runtime_LogSync_Object__table (ti_uia_runtime_LogSync_Object__table__C)
#endif

/* injectIntoTraceFxn */
typedef ti_uia_runtime_LoggerTypes_InjectIntoTraceFxn CT__ti_uia_runtime_LogSync_injectIntoTraceFxn;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_injectIntoTraceFxn ti_uia_runtime_LogSync_injectIntoTraceFxn__C;
#ifdef ti_uia_runtime_LogSync_injectIntoTraceFxn__CR
#define ti_uia_runtime_LogSync_injectIntoTraceFxn (*((CT__ti_uia_runtime_LogSync_injectIntoTraceFxn*)(xdcRomConstPtr + ti_uia_runtime_LogSync_injectIntoTraceFxn__C_offset)))
#else
#define ti_uia_runtime_LogSync_injectIntoTraceFxn (ti_uia_runtime_LogSync_injectIntoTraceFxn__C)
#endif

/* cpuTimestampCyclesPerTick */
typedef xdc_UInt32 CT__ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__C;
#ifdef ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__CR
#define ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick (*((CT__ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick*)(xdcRomConstPtr + ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__C_offset)))
#else
#ifdef ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__D
#define ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick (ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__D)
#else
#define ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick (ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick__C)
#endif
#endif

/* globalTimestampCpuCyclesPerTick */
typedef xdc_UInt32 CT__ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__C;
#ifdef ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__CR
#define ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick (*((CT__ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick*)(xdcRomConstPtr + ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__C_offset)))
#else
#ifdef ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__D
#define ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick (ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__D)
#else
#define ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick (ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick__C)
#endif
#endif

/* maxGlobalClockFreq */
typedef xdc_runtime_Types_FreqHz CT__ti_uia_runtime_LogSync_maxGlobalClockFreq;
__extern __FAR__ const CT__ti_uia_runtime_LogSync_maxGlobalClockFreq ti_uia_runtime_LogSync_maxGlobalClockFreq__C;
#ifdef ti_uia_runtime_LogSync_maxGlobalClockFreq__CR
#define ti_uia_runtime_LogSync_maxGlobalClockFreq (*((CT__ti_uia_runtime_LogSync_maxGlobalClockFreq*)(xdcRomConstPtr + ti_uia_runtime_LogSync_maxGlobalClockFreq__C_offset)))
#else
#define ti_uia_runtime_LogSync_maxGlobalClockFreq (ti_uia_runtime_LogSync_maxGlobalClockFreq__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_runtime_LogSync_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_uia_runtime_LogSync_Struct {
    const ti_uia_runtime_LogSync_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_LogSync_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_LogSync_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_runtime_LogSync_Fxns__ ti_uia_runtime_LogSync_Module__FXNS__C;
#else
#define ti_uia_runtime_LogSync_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_runtime_LogSync_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_LogSync_Module_startup ti_uia_runtime_LogSync_Module_startup__E
xdc__CODESECT(ti_uia_runtime_LogSync_Module_startup__E, "ti_uia_runtime_LogSync_Module_startup")
__extern xdc_Int ti_uia_runtime_LogSync_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_uia_runtime_LogSync_Module_startup__F, "ti_uia_runtime_LogSync_Module_startup")
__extern xdc_Int ti_uia_runtime_LogSync_Module_startup__F( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_uia_runtime_LogSync_Instance_init__E, "ti_uia_runtime_LogSync_Instance_init")
__extern xdc_Void ti_uia_runtime_LogSync_Instance_init__E(ti_uia_runtime_LogSync_Object *obj, const ti_uia_runtime_LogSync_Params *prms);

/* create */
xdc__CODESECT(ti_uia_runtime_LogSync_create, "ti_uia_runtime_LogSync_create")
__extern ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_create( const ti_uia_runtime_LogSync_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_uia_runtime_LogSync_construct, "ti_uia_runtime_LogSync_construct")
__extern void ti_uia_runtime_LogSync_construct(ti_uia_runtime_LogSync_Struct *obj, const ti_uia_runtime_LogSync_Params *prms);

/* delete */
xdc__CODESECT(ti_uia_runtime_LogSync_delete, "ti_uia_runtime_LogSync_delete")
__extern void ti_uia_runtime_LogSync_delete(ti_uia_runtime_LogSync_Handle *instp);

/* destruct */
xdc__CODESECT(ti_uia_runtime_LogSync_destruct, "ti_uia_runtime_LogSync_destruct")
__extern void ti_uia_runtime_LogSync_destruct(ti_uia_runtime_LogSync_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Handle__label__S, "ti_uia_runtime_LogSync_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_runtime_LogSync_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Module__startupDone__S, "ti_uia_runtime_LogSync_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_LogSync_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Object__create__S, "ti_uia_runtime_LogSync_Object__create__S")
__extern xdc_Ptr ti_uia_runtime_LogSync_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Object__delete__S, "ti_uia_runtime_LogSync_Object__delete__S")
__extern xdc_Void ti_uia_runtime_LogSync_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Object__get__S, "ti_uia_runtime_LogSync_Object__get__S")
__extern xdc_Ptr ti_uia_runtime_LogSync_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Object__first__S, "ti_uia_runtime_LogSync_Object__first__S")
__extern xdc_Ptr ti_uia_runtime_LogSync_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Object__next__S, "ti_uia_runtime_LogSync_Object__next__S")
__extern xdc_Ptr ti_uia_runtime_LogSync_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_runtime_LogSync_Params__init__S, "ti_uia_runtime_LogSync_Params__init__S")
__extern xdc_Void ti_uia_runtime_LogSync_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* enable__E */
#define ti_uia_runtime_LogSync_enable ti_uia_runtime_LogSync_enable__E
xdc__CODESECT(ti_uia_runtime_LogSync_enable__E, "ti_uia_runtime_LogSync_enable")
__extern xdc_Bool ti_uia_runtime_LogSync_enable__E( void);

/* disable__E */
#define ti_uia_runtime_LogSync_disable ti_uia_runtime_LogSync_disable__E
xdc__CODESECT(ti_uia_runtime_LogSync_disable__E, "ti_uia_runtime_LogSync_disable")
__extern xdc_Bool ti_uia_runtime_LogSync_disable__E( void);

/* idleHook__E */
#define ti_uia_runtime_LogSync_idleHook ti_uia_runtime_LogSync_idleHook__E
xdc__CODESECT(ti_uia_runtime_LogSync_idleHook__E, "ti_uia_runtime_LogSync_idleHook")
__extern xdc_Void ti_uia_runtime_LogSync_idleHook__E( void);

/* timerHook__E */
#define ti_uia_runtime_LogSync_timerHook ti_uia_runtime_LogSync_timerHook__E
xdc__CODESECT(ti_uia_runtime_LogSync_timerHook__E, "ti_uia_runtime_LogSync_timerHook")
__extern xdc_Void ti_uia_runtime_LogSync_timerHook__E( xdc_UArg arg);

/* putSyncPoint__E */
#define ti_uia_runtime_LogSync_putSyncPoint ti_uia_runtime_LogSync_putSyncPoint__E
xdc__CODESECT(ti_uia_runtime_LogSync_putSyncPoint__E, "ti_uia_runtime_LogSync_putSyncPoint")
__extern xdc_Void ti_uia_runtime_LogSync_putSyncPoint__E( void);

/* writeSyncPointRaw__E */
#define ti_uia_runtime_LogSync_writeSyncPointRaw ti_uia_runtime_LogSync_writeSyncPointRaw__E
xdc__CODESECT(ti_uia_runtime_LogSync_writeSyncPointRaw__E, "ti_uia_runtime_LogSync_writeSyncPointRaw")
__extern xdc_Void ti_uia_runtime_LogSync_writeSyncPointRaw__E( const xdc_runtime_Types_Timestamp64 *cpuTS, const xdc_runtime_Types_Timestamp64 *globalTS, const xdc_runtime_Types_FreqHz *globalTickFreq);
xdc__CODESECT(ti_uia_runtime_LogSync_writeSyncPointRaw__F, "ti_uia_runtime_LogSync_writeSyncPointRaw")
__extern xdc_Void ti_uia_runtime_LogSync_writeSyncPointRaw__F( const xdc_runtime_Types_Timestamp64 *cpuTS, const xdc_runtime_Types_Timestamp64 *globalTS, const xdc_runtime_Types_FreqHz *globalTickFreq);

/* isSyncEventRequired__E */
#define ti_uia_runtime_LogSync_isSyncEventRequired ti_uia_runtime_LogSync_isSyncEventRequired__E
xdc__CODESECT(ti_uia_runtime_LogSync_isSyncEventRequired__E, "ti_uia_runtime_LogSync_isSyncEventRequired")
__extern xdc_Bool ti_uia_runtime_LogSync_isSyncEventRequired__E( void);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_runtime_IUIATraceSyncClient_Module ti_uia_runtime_LogSync_Module_upCast(void);
static inline ti_uia_runtime_IUIATraceSyncClient_Module ti_uia_runtime_LogSync_Module_upCast(void)
{
    return (ti_uia_runtime_IUIATraceSyncClient_Module)&ti_uia_runtime_LogSync_Module__FXNS__C;
}

/* Module_to_ti_uia_runtime_IUIATraceSyncClient */
#define ti_uia_runtime_LogSync_Module_to_ti_uia_runtime_IUIATraceSyncClient ti_uia_runtime_LogSync_Module_upCast

/* Module_upCast2 */
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_LogSync_Module_upCast2(void);
static inline ti_uia_events_IUIAMetaProvider_Module ti_uia_runtime_LogSync_Module_upCast2(void)
{
    return (ti_uia_events_IUIAMetaProvider_Module)&ti_uia_runtime_LogSync_Module__FXNS__C;
}

/* Module_to_ti_uia_events_IUIAMetaProvider */
#define ti_uia_runtime_LogSync_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_LogSync_Module_upCast2


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_LogSync_Module_startupDone() ti_uia_runtime_LogSync_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_LogSync_Object_heap() ti_uia_runtime_LogSync_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_LogSync_Module_heap() ti_uia_runtime_LogSync_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_LogSync_Module__id ti_uia_runtime_LogSync_Module_id(void);
static inline CT__ti_uia_runtime_LogSync_Module__id ti_uia_runtime_LogSync_Module_id( void ) 
{
    return ti_uia_runtime_LogSync_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_uia_runtime_LogSync_Module_hasMask(void);
static inline xdc_Bool ti_uia_runtime_LogSync_Module_hasMask(void)
{
    return (xdc_Bool)(ti_uia_runtime_LogSync_Module__diagsMask__C != (CT__ti_uia_runtime_LogSync_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_uia_runtime_LogSync_Module_getMask(void);
static inline xdc_Bits16 ti_uia_runtime_LogSync_Module_getMask(void)
{
    return (ti_uia_runtime_LogSync_Module__diagsMask__C != (CT__ti_uia_runtime_LogSync_Module__diagsMask)NULL) ? *ti_uia_runtime_LogSync_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_uia_runtime_LogSync_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_uia_runtime_LogSync_Module_setMask(xdc_Bits16 mask)
{
    if (ti_uia_runtime_LogSync_Module__diagsMask__C != (CT__ti_uia_runtime_LogSync_Module__diagsMask)NULL) {
        *ti_uia_runtime_LogSync_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_uia_runtime_LogSync_Params_init(ti_uia_runtime_LogSync_Params *prms);
static inline void ti_uia_runtime_LogSync_Params_init( ti_uia_runtime_LogSync_Params *prms ) 
{
    if (prms != NULL) {
        ti_uia_runtime_LogSync_Params__init__S(prms, NULL, sizeof(ti_uia_runtime_LogSync_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_uia_runtime_LogSync_Params_copy(ti_uia_runtime_LogSync_Params *dst, const ti_uia_runtime_LogSync_Params *src);
static inline void ti_uia_runtime_LogSync_Params_copy(ti_uia_runtime_LogSync_Params *dst, const ti_uia_runtime_LogSync_Params *src) 
{
    if (dst != NULL) {
        ti_uia_runtime_LogSync_Params__init__S(dst, (const void *)src, sizeof(ti_uia_runtime_LogSync_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_uia_runtime_LogSync_Object_count() ti_uia_runtime_LogSync_Object__count__C

/* Object_sizeof */
#define ti_uia_runtime_LogSync_Object_sizeof() ti_uia_runtime_LogSync_Object__sizeof__C

/* Object_get */
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_get(ti_uia_runtime_LogSync_Object *oarr, int i);
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_get(ti_uia_runtime_LogSync_Object *oarr, int i) 
{
    return (ti_uia_runtime_LogSync_Handle)ti_uia_runtime_LogSync_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_first(void);
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_first(void)
{
    return (ti_uia_runtime_LogSync_Handle)ti_uia_runtime_LogSync_Object__first__S();
}

/* Object_next */
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_next(ti_uia_runtime_LogSync_Object *obj);
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_Object_next(ti_uia_runtime_LogSync_Object *obj)
{
    return (ti_uia_runtime_LogSync_Handle)ti_uia_runtime_LogSync_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_runtime_LogSync_Handle_label(ti_uia_runtime_LogSync_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_runtime_LogSync_Handle_label(ti_uia_runtime_LogSync_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_uia_runtime_LogSync_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_uia_runtime_LogSync_Handle_name(ti_uia_runtime_LogSync_Handle inst);
static inline xdc_String ti_uia_runtime_LogSync_Handle_name(ti_uia_runtime_LogSync_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_uia_runtime_LogSync_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_handle(ti_uia_runtime_LogSync_Struct *str);
static inline ti_uia_runtime_LogSync_Handle ti_uia_runtime_LogSync_handle(ti_uia_runtime_LogSync_Struct *str)
{
    return (ti_uia_runtime_LogSync_Handle)str;
}

/* struct */
static inline ti_uia_runtime_LogSync_Struct *ti_uia_runtime_LogSync_struct(ti_uia_runtime_LogSync_Handle inst);
static inline ti_uia_runtime_LogSync_Struct *ti_uia_runtime_LogSync_struct(ti_uia_runtime_LogSync_Handle inst)
{
    return (ti_uia_runtime_LogSync_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/uia/runtime/LogSync__epilogue.h>

#ifdef ti_uia_runtime_LogSync__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_LogSync__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_uia_runtime_LogSync__internalaccess))

#ifndef ti_uia_runtime_LogSync__include_state
#define ti_uia_runtime_LogSync__include_state

/* Module_State */
struct ti_uia_runtime_LogSync_Module_State {
    xdc_UInt32 numTimesHalted;
    xdc_UInt32 serialNumber;
    xdc_Bool isEnabled;
};

/* Module__state__V */
#ifndef ti_uia_runtime_LogSync_Module__state__VR
extern struct ti_uia_runtime_LogSync_Module_State__ ti_uia_runtime_LogSync_Module__state__V;
#else
#define ti_uia_runtime_LogSync_Module__state__V (*((struct ti_uia_runtime_LogSync_Module_State__*)(xdcRomStatePtr + ti_uia_runtime_LogSync_Module__state__V_offset)))
#endif

/* Object */
struct ti_uia_runtime_LogSync_Object {
    const ti_uia_runtime_LogSync_Fxns__ *__fxns;
};

#endif /* ti_uia_runtime_LogSync__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_LogSync__nolocalnames)

#ifndef ti_uia_runtime_LogSync__localnames__done
#define ti_uia_runtime_LogSync__localnames__done

/* module prefix */
#define LogSync_Instance ti_uia_runtime_LogSync_Instance
#define LogSync_Handle ti_uia_runtime_LogSync_Handle
#define LogSync_Module ti_uia_runtime_LogSync_Module
#define LogSync_Object ti_uia_runtime_LogSync_Object
#define LogSync_Struct ti_uia_runtime_LogSync_Struct
#define LogSync_Module_State ti_uia_runtime_LogSync_Module_State
#define LogSync_Instance_State ti_uia_runtime_LogSync_Instance_State
#define LogSync_injectIntoTraceFxn ti_uia_runtime_LogSync_injectIntoTraceFxn
#define LogSync_cpuTimestampCyclesPerTick ti_uia_runtime_LogSync_cpuTimestampCyclesPerTick
#define LogSync_globalTimestampCpuCyclesPerTick ti_uia_runtime_LogSync_globalTimestampCpuCyclesPerTick
#define LogSync_maxGlobalClockFreq ti_uia_runtime_LogSync_maxGlobalClockFreq
#define LogSync_Params ti_uia_runtime_LogSync_Params
#define LogSync_enable ti_uia_runtime_LogSync_enable
#define LogSync_disable ti_uia_runtime_LogSync_disable
#define LogSync_idleHook ti_uia_runtime_LogSync_idleHook
#define LogSync_timerHook ti_uia_runtime_LogSync_timerHook
#define LogSync_putSyncPoint ti_uia_runtime_LogSync_putSyncPoint
#define LogSync_writeSyncPoint ti_uia_runtime_LogSync_writeSyncPoint
#define LogSync_writeSyncPointRaw ti_uia_runtime_LogSync_writeSyncPointRaw
#define LogSync_isSyncEventRequired ti_uia_runtime_LogSync_isSyncEventRequired
#define LogSync_Module_name ti_uia_runtime_LogSync_Module_name
#define LogSync_Module_id ti_uia_runtime_LogSync_Module_id
#define LogSync_Module_startup ti_uia_runtime_LogSync_Module_startup
#define LogSync_Module_startupDone ti_uia_runtime_LogSync_Module_startupDone
#define LogSync_Module_hasMask ti_uia_runtime_LogSync_Module_hasMask
#define LogSync_Module_getMask ti_uia_runtime_LogSync_Module_getMask
#define LogSync_Module_setMask ti_uia_runtime_LogSync_Module_setMask
#define LogSync_Object_heap ti_uia_runtime_LogSync_Object_heap
#define LogSync_Module_heap ti_uia_runtime_LogSync_Module_heap
#define LogSync_construct ti_uia_runtime_LogSync_construct
#define LogSync_create ti_uia_runtime_LogSync_create
#define LogSync_handle ti_uia_runtime_LogSync_handle
#define LogSync_struct ti_uia_runtime_LogSync_struct
#define LogSync_Handle_label ti_uia_runtime_LogSync_Handle_label
#define LogSync_Handle_name ti_uia_runtime_LogSync_Handle_name
#define LogSync_Instance_init ti_uia_runtime_LogSync_Instance_init
#define LogSync_Object_count ti_uia_runtime_LogSync_Object_count
#define LogSync_Object_get ti_uia_runtime_LogSync_Object_get
#define LogSync_Object_first ti_uia_runtime_LogSync_Object_first
#define LogSync_Object_next ti_uia_runtime_LogSync_Object_next
#define LogSync_Object_sizeof ti_uia_runtime_LogSync_Object_sizeof
#define LogSync_Params_copy ti_uia_runtime_LogSync_Params_copy
#define LogSync_Params_init ti_uia_runtime_LogSync_Params_init
#define LogSync_delete ti_uia_runtime_LogSync_delete
#define LogSync_destruct ti_uia_runtime_LogSync_destruct
#define LogSync_Module_upCast ti_uia_runtime_LogSync_Module_upCast
#define LogSync_Module_to_ti_uia_runtime_IUIATraceSyncClient ti_uia_runtime_LogSync_Module_to_ti_uia_runtime_IUIATraceSyncClient
#define LogSync_Module_upCast2 ti_uia_runtime_LogSync_Module_upCast2
#define LogSync_Module_to_ti_uia_events_IUIAMetaProvider ti_uia_runtime_LogSync_Module_to_ti_uia_events_IUIAMetaProvider

/* proxies */
#include <ti/uia/runtime/package/LogSync_CpuTimestampProxy.h>

/* proxies */
#include <ti/uia/runtime/package/LogSync_GlobalTimestampProxy.h>

#endif /* ti_uia_runtime_LogSync__localnames__done */
#endif
