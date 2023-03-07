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
 *     MODULE-WIDE CONFIGS
 *     PER-INSTANCE TYPES
 *     VIRTUAL FUNCTIONS
 *     FUNCTION DECLARATIONS
 *     CONVERTORS
 *     SYSTEM FUNCTIONS
 *
 *     EPILOGUE
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_BIOS_RtsGateProxy__include
#define ti_sysbios_BIOS_RtsGateProxy__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_BIOS_RtsGateProxy__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_BIOS_RtsGateProxy___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/package/package.defs.h>

#include <xdc/runtime/IGateProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Q_BLOCKING */
#define ti_sysbios_BIOS_RtsGateProxy_Q_BLOCKING (1)

/* Q_PREEMPTING */
#define ti_sysbios_BIOS_RtsGateProxy_Q_PREEMPTING (2)


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled (ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded (ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask (ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_BIOS_RtsGateProxy_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__gateObj ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__gateObj (ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms (ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_BIOS_RtsGateProxy_Module__id;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__id ti_sysbios_BIOS_RtsGateProxy_Module__id__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__id__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__id__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__id*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__id__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__id (ti_sysbios_BIOS_RtsGateProxy_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined (ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj (ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0 (ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1 (ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2 (ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4 (ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__CR
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8 (ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_BIOS_RtsGateProxy_Object__count;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Object__count ti_sysbios_BIOS_RtsGateProxy_Object__count__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Object__count__CR
#define ti_sysbios_BIOS_RtsGateProxy_Object__count__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Object__count*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Object__count__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Object__count (ti_sysbios_BIOS_RtsGateProxy_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_BIOS_RtsGateProxy_Object__heap;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Object__heap ti_sysbios_BIOS_RtsGateProxy_Object__heap__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Object__heap__CR
#define ti_sysbios_BIOS_RtsGateProxy_Object__heap__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Object__heap*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Object__heap__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Object__heap (ti_sysbios_BIOS_RtsGateProxy_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_BIOS_RtsGateProxy_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Object__sizeof ti_sysbios_BIOS_RtsGateProxy_Object__sizeof__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Object__sizeof__CR
#define ti_sysbios_BIOS_RtsGateProxy_Object__sizeof__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Object__sizeof__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Object__sizeof (ti_sysbios_BIOS_RtsGateProxy_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_BIOS_RtsGateProxy_Object__table;
__extern __FAR__ const CT__ti_sysbios_BIOS_RtsGateProxy_Object__table ti_sysbios_BIOS_RtsGateProxy_Object__table__C;
#ifdef ti_sysbios_BIOS_RtsGateProxy_Object__table__CR
#define ti_sysbios_BIOS_RtsGateProxy_Object__table__C (*((CT__ti_sysbios_BIOS_RtsGateProxy_Object__table*)(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Object__table__C_offset)))
#else
#define ti_sysbios_BIOS_RtsGateProxy_Object__table (ti_sysbios_BIOS_RtsGateProxy_Object__table__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_BIOS_RtsGateProxy_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_BIOS_RtsGateProxy_Struct {
    const ti_sysbios_BIOS_RtsGateProxy_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_BIOS_RtsGateProxy_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*query)(xdc_Int qual);
    xdc_IArg (*enter)(ti_sysbios_BIOS_RtsGateProxy_Handle inst);
    xdc_Void (*leave)(ti_sysbios_BIOS_RtsGateProxy_Handle inst, xdc_IArg key);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_BIOS_RtsGateProxy_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_BIOS_RtsGateProxy_Fxns__ ti_sysbios_BIOS_RtsGateProxy_Module__FXNS__C;
#else
#define ti_sysbios_BIOS_RtsGateProxy_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_BIOS_RtsGateProxy_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_BIOS_RtsGateProxy_Module_startup( state ) (-1)

/* create */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_create, "ti_sysbios_BIOS_RtsGateProxy_create")
__extern ti_sysbios_BIOS_RtsGateProxy_Handle ti_sysbios_BIOS_RtsGateProxy_create( const ti_sysbios_BIOS_RtsGateProxy_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_delete, "ti_sysbios_BIOS_RtsGateProxy_delete")
__extern void ti_sysbios_BIOS_RtsGateProxy_delete(ti_sysbios_BIOS_RtsGateProxy_Handle *instp);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Handle__label__S, "ti_sysbios_BIOS_RtsGateProxy_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_BIOS_RtsGateProxy_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Module__startupDone__S, "ti_sysbios_BIOS_RtsGateProxy_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_BIOS_RtsGateProxy_Module__startupDone__S( void);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Object__get__S, "ti_sysbios_BIOS_RtsGateProxy_Object__get__S")
__extern xdc_Ptr ti_sysbios_BIOS_RtsGateProxy_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Object__first__S, "ti_sysbios_BIOS_RtsGateProxy_Object__first__S")
__extern xdc_Ptr ti_sysbios_BIOS_RtsGateProxy_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Object__next__S, "ti_sysbios_BIOS_RtsGateProxy_Object__next__S")
__extern xdc_Ptr ti_sysbios_BIOS_RtsGateProxy_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Params__init__S, "ti_sysbios_BIOS_RtsGateProxy_Params__init__S")
__extern xdc_Void ti_sysbios_BIOS_RtsGateProxy_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* Proxy__abstract__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S, "ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S")
__extern xdc_Bool ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S( void);

/* Proxy__delegate__S */
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S, "ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S")
__extern xdc_CPtr ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S( void);

/* query__E */
#define ti_sysbios_BIOS_RtsGateProxy_query ti_sysbios_BIOS_RtsGateProxy_query__E
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_query__E, "ti_sysbios_BIOS_RtsGateProxy_query")
__extern xdc_Bool ti_sysbios_BIOS_RtsGateProxy_query__E( xdc_Int qual);

/* enter__E */
#define ti_sysbios_BIOS_RtsGateProxy_enter ti_sysbios_BIOS_RtsGateProxy_enter__E
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_enter__E, "ti_sysbios_BIOS_RtsGateProxy_enter")
__extern xdc_IArg ti_sysbios_BIOS_RtsGateProxy_enter__E( ti_sysbios_BIOS_RtsGateProxy_Handle instp);

/* leave__E */
#define ti_sysbios_BIOS_RtsGateProxy_leave ti_sysbios_BIOS_RtsGateProxy_leave__E
xdc__CODESECT(ti_sysbios_BIOS_RtsGateProxy_leave__E, "ti_sysbios_BIOS_RtsGateProxy_leave")
__extern xdc_Void ti_sysbios_BIOS_RtsGateProxy_leave__E( ti_sysbios_BIOS_RtsGateProxy_Handle instp, xdc_IArg key);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IGateProvider_Module ti_sysbios_BIOS_RtsGateProxy_Module_upCast(void);
static inline xdc_runtime_IGateProvider_Module ti_sysbios_BIOS_RtsGateProxy_Module_upCast(void)
{
    return (xdc_runtime_IGateProvider_Module)ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S();
}

/* Module_to_xdc_runtime_IGateProvider */
#define ti_sysbios_BIOS_RtsGateProxy_Module_to_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_BIOS_RtsGateProxy_Handle_upCast(ti_sysbios_BIOS_RtsGateProxy_Handle i);
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_BIOS_RtsGateProxy_Handle_upCast(ti_sysbios_BIOS_RtsGateProxy_Handle i)
{
    return (xdc_runtime_IGateProvider_Handle)i;
}

/* Handle_to_xdc_runtime_IGateProvider */
#define ti_sysbios_BIOS_RtsGateProxy_Handle_to_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_BIOS_RtsGateProxy_Handle ti_sysbios_BIOS_RtsGateProxy_Handle_downCast(xdc_runtime_IGateProvider_Handle i);
static inline ti_sysbios_BIOS_RtsGateProxy_Handle ti_sysbios_BIOS_RtsGateProxy_Handle_downCast(xdc_runtime_IGateProvider_Handle i)
{
    xdc_runtime_IGateProvider_Handle i2 = (xdc_runtime_IGateProvider_Handle)i;
    if (ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S() != 0U) {
        return (ti_sysbios_BIOS_RtsGateProxy_Handle)i;
    }
    return ((const void*)i2->__fxns == (const void*)ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S()) ? (ti_sysbios_BIOS_RtsGateProxy_Handle)i : (ti_sysbios_BIOS_RtsGateProxy_Handle)NULL;
}

/* Handle_from_xdc_runtime_IGateProvider */
#define ti_sysbios_BIOS_RtsGateProxy_Handle_from_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_BIOS_RtsGateProxy_Module_startupDone() ti_sysbios_BIOS_RtsGateProxy_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_BIOS_RtsGateProxy_Object_heap() ti_sysbios_BIOS_RtsGateProxy_Object__heap__C

/* Module_heap */
#define ti_sysbios_BIOS_RtsGateProxy_Module_heap() ti_sysbios_BIOS_RtsGateProxy_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_BIOS_RtsGateProxy_Module__id ti_sysbios_BIOS_RtsGateProxy_Module_id(void);
static inline CT__ti_sysbios_BIOS_RtsGateProxy_Module__id ti_sysbios_BIOS_RtsGateProxy_Module_id( void ) 
{
    return ti_sysbios_BIOS_RtsGateProxy_Module__id__C;
}

/* Proxy_abstract */
#define ti_sysbios_BIOS_RtsGateProxy_Proxy_abstract() ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S()

/* Proxy_delegate */
#define ti_sysbios_BIOS_RtsGateProxy_Proxy_delegate() ((xdc_runtime_IGateProvider_Module)ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S())

/* Params_init */
static inline void ti_sysbios_BIOS_RtsGateProxy_Params_init(ti_sysbios_BIOS_RtsGateProxy_Params *prms);
static inline void ti_sysbios_BIOS_RtsGateProxy_Params_init( ti_sysbios_BIOS_RtsGateProxy_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_BIOS_RtsGateProxy_Params__init__S(prms, NULL, sizeof(ti_sysbios_BIOS_RtsGateProxy_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_BIOS_RtsGateProxy_Params_copy(ti_sysbios_BIOS_RtsGateProxy_Params *dst, const ti_sysbios_BIOS_RtsGateProxy_Params *src);
static inline void ti_sysbios_BIOS_RtsGateProxy_Params_copy(ti_sysbios_BIOS_RtsGateProxy_Params *dst, const ti_sysbios_BIOS_RtsGateProxy_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_BIOS_RtsGateProxy_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_BIOS_RtsGateProxy_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_BIOS_RtsGateProxy__top__
#undef __nested__
#endif

#endif /* ti_sysbios_BIOS_RtsGateProxy__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_BIOS_RtsGateProxy__nolocalnames)

#ifndef ti_sysbios_BIOS_RtsGateProxy__localnames__done
#define ti_sysbios_BIOS_RtsGateProxy__localnames__done

/* module prefix */
#define BIOS_RtsGateProxy_Instance ti_sysbios_BIOS_RtsGateProxy_Instance
#define BIOS_RtsGateProxy_Handle ti_sysbios_BIOS_RtsGateProxy_Handle
#define BIOS_RtsGateProxy_Module ti_sysbios_BIOS_RtsGateProxy_Module
#define BIOS_RtsGateProxy_Object ti_sysbios_BIOS_RtsGateProxy_Object
#define BIOS_RtsGateProxy_Struct ti_sysbios_BIOS_RtsGateProxy_Struct
#define BIOS_RtsGateProxy_Q_BLOCKING ti_sysbios_BIOS_RtsGateProxy_Q_BLOCKING
#define BIOS_RtsGateProxy_Q_PREEMPTING ti_sysbios_BIOS_RtsGateProxy_Q_PREEMPTING
#define BIOS_RtsGateProxy_Params ti_sysbios_BIOS_RtsGateProxy_Params
#define BIOS_RtsGateProxy_query ti_sysbios_BIOS_RtsGateProxy_query
#define BIOS_RtsGateProxy_enter ti_sysbios_BIOS_RtsGateProxy_enter
#define BIOS_RtsGateProxy_leave ti_sysbios_BIOS_RtsGateProxy_leave
#define BIOS_RtsGateProxy_Module_name ti_sysbios_BIOS_RtsGateProxy_Module_name
#define BIOS_RtsGateProxy_Module_id ti_sysbios_BIOS_RtsGateProxy_Module_id
#define BIOS_RtsGateProxy_Module_startup ti_sysbios_BIOS_RtsGateProxy_Module_startup
#define BIOS_RtsGateProxy_Module_startupDone ti_sysbios_BIOS_RtsGateProxy_Module_startupDone
#define BIOS_RtsGateProxy_Module_hasMask ti_sysbios_BIOS_RtsGateProxy_Module_hasMask
#define BIOS_RtsGateProxy_Module_getMask ti_sysbios_BIOS_RtsGateProxy_Module_getMask
#define BIOS_RtsGateProxy_Module_setMask ti_sysbios_BIOS_RtsGateProxy_Module_setMask
#define BIOS_RtsGateProxy_Object_heap ti_sysbios_BIOS_RtsGateProxy_Object_heap
#define BIOS_RtsGateProxy_Module_heap ti_sysbios_BIOS_RtsGateProxy_Module_heap
#define BIOS_RtsGateProxy_construct ti_sysbios_BIOS_RtsGateProxy_construct
#define BIOS_RtsGateProxy_create ti_sysbios_BIOS_RtsGateProxy_create
#define BIOS_RtsGateProxy_handle ti_sysbios_BIOS_RtsGateProxy_handle
#define BIOS_RtsGateProxy_struct ti_sysbios_BIOS_RtsGateProxy_struct
#define BIOS_RtsGateProxy_Handle_label ti_sysbios_BIOS_RtsGateProxy_Handle_label
#define BIOS_RtsGateProxy_Handle_name ti_sysbios_BIOS_RtsGateProxy_Handle_name
#define BIOS_RtsGateProxy_Instance_init ti_sysbios_BIOS_RtsGateProxy_Instance_init
#define BIOS_RtsGateProxy_Object_count ti_sysbios_BIOS_RtsGateProxy_Object_count
#define BIOS_RtsGateProxy_Object_get ti_sysbios_BIOS_RtsGateProxy_Object_get
#define BIOS_RtsGateProxy_Object_first ti_sysbios_BIOS_RtsGateProxy_Object_first
#define BIOS_RtsGateProxy_Object_next ti_sysbios_BIOS_RtsGateProxy_Object_next
#define BIOS_RtsGateProxy_Object_sizeof ti_sysbios_BIOS_RtsGateProxy_Object_sizeof
#define BIOS_RtsGateProxy_Params_copy ti_sysbios_BIOS_RtsGateProxy_Params_copy
#define BIOS_RtsGateProxy_Params_init ti_sysbios_BIOS_RtsGateProxy_Params_init
#define BIOS_RtsGateProxy_Proxy_abstract ti_sysbios_BIOS_RtsGateProxy_Proxy_abstract
#define BIOS_RtsGateProxy_Proxy_delegate ti_sysbios_BIOS_RtsGateProxy_Proxy_delegate
#define BIOS_RtsGateProxy_delete ti_sysbios_BIOS_RtsGateProxy_delete
#define BIOS_RtsGateProxy_destruct ti_sysbios_BIOS_RtsGateProxy_destruct
#define BIOS_RtsGateProxy_Module_upCast ti_sysbios_BIOS_RtsGateProxy_Module_upCast
#define BIOS_RtsGateProxy_Module_to_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Module_to_xdc_runtime_IGateProvider
#define BIOS_RtsGateProxy_Handle_upCast ti_sysbios_BIOS_RtsGateProxy_Handle_upCast
#define BIOS_RtsGateProxy_Handle_to_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Handle_to_xdc_runtime_IGateProvider
#define BIOS_RtsGateProxy_Handle_downCast ti_sysbios_BIOS_RtsGateProxy_Handle_downCast
#define BIOS_RtsGateProxy_Handle_from_xdc_runtime_IGateProvider ti_sysbios_BIOS_RtsGateProxy_Handle_from_xdc_runtime_IGateProvider

#endif /* ti_sysbios_BIOS_RtsGateProxy__localnames__done */
#endif
