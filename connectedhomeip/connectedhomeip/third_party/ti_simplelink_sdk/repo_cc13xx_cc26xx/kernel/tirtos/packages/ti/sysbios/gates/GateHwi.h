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

#ifndef ti_sysbios_gates_GateHwi__include
#define ti_sysbios_gates_GateHwi__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_gates_GateHwi__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_gates_GateHwi___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/gates/package/package.defs.h>

#include <xdc/runtime/IGateProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Q_BLOCKING */
#define ti_sysbios_gates_GateHwi_Q_BLOCKING (1)

/* Q_PREEMPTING */
#define ti_sysbios_gates_GateHwi_Q_PREEMPTING (2)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_gates_GateHwi_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsEnabled ti_sysbios_gates_GateHwi_Module__diagsEnabled__C;
#ifdef ti_sysbios_gates_GateHwi_Module__diagsEnabled__CR
#define ti_sysbios_gates_GateHwi_Module__diagsEnabled__C (*((CT__ti_sysbios_gates_GateHwi_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__diagsEnabled (ti_sysbios_gates_GateHwi_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_gates_GateHwi_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsIncluded ti_sysbios_gates_GateHwi_Module__diagsIncluded__C;
#ifdef ti_sysbios_gates_GateHwi_Module__diagsIncluded__CR
#define ti_sysbios_gates_GateHwi_Module__diagsIncluded__C (*((CT__ti_sysbios_gates_GateHwi_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__diagsIncluded (ti_sysbios_gates_GateHwi_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_gates_GateHwi_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsMask ti_sysbios_gates_GateHwi_Module__diagsMask__C;
#ifdef ti_sysbios_gates_GateHwi_Module__diagsMask__CR
#define ti_sysbios_gates_GateHwi_Module__diagsMask__C (*((CT__ti_sysbios_gates_GateHwi_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__diagsMask (ti_sysbios_gates_GateHwi_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_gates_GateHwi_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__gateObj ti_sysbios_gates_GateHwi_Module__gateObj__C;
#ifdef ti_sysbios_gates_GateHwi_Module__gateObj__CR
#define ti_sysbios_gates_GateHwi_Module__gateObj__C (*((CT__ti_sysbios_gates_GateHwi_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__gateObj__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__gateObj (ti_sysbios_gates_GateHwi_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_gates_GateHwi_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__gatePrms ti_sysbios_gates_GateHwi_Module__gatePrms__C;
#ifdef ti_sysbios_gates_GateHwi_Module__gatePrms__CR
#define ti_sysbios_gates_GateHwi_Module__gatePrms__C (*((CT__ti_sysbios_gates_GateHwi_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__gatePrms (ti_sysbios_gates_GateHwi_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_gates_GateHwi_Module__id;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__id ti_sysbios_gates_GateHwi_Module__id__C;
#ifdef ti_sysbios_gates_GateHwi_Module__id__CR
#define ti_sysbios_gates_GateHwi_Module__id__C (*((CT__ti_sysbios_gates_GateHwi_Module__id*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__id__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__id (ti_sysbios_gates_GateHwi_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_gates_GateHwi_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerDefined ti_sysbios_gates_GateHwi_Module__loggerDefined__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerDefined__CR
#define ti_sysbios_gates_GateHwi_Module__loggerDefined__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerDefined (ti_sysbios_gates_GateHwi_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_gates_GateHwi_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerObj ti_sysbios_gates_GateHwi_Module__loggerObj__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerObj__CR
#define ti_sysbios_gates_GateHwi_Module__loggerObj__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerObj (ti_sysbios_gates_GateHwi_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_gates_GateHwi_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn0 ti_sysbios_gates_GateHwi_Module__loggerFxn0__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerFxn0__CR
#define ti_sysbios_gates_GateHwi_Module__loggerFxn0__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerFxn0 (ti_sysbios_gates_GateHwi_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_gates_GateHwi_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn1 ti_sysbios_gates_GateHwi_Module__loggerFxn1__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerFxn1__CR
#define ti_sysbios_gates_GateHwi_Module__loggerFxn1__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerFxn1 (ti_sysbios_gates_GateHwi_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_gates_GateHwi_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn2 ti_sysbios_gates_GateHwi_Module__loggerFxn2__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerFxn2__CR
#define ti_sysbios_gates_GateHwi_Module__loggerFxn2__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerFxn2 (ti_sysbios_gates_GateHwi_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_gates_GateHwi_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn4 ti_sysbios_gates_GateHwi_Module__loggerFxn4__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerFxn4__CR
#define ti_sysbios_gates_GateHwi_Module__loggerFxn4__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerFxn4 (ti_sysbios_gates_GateHwi_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_gates_GateHwi_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn8 ti_sysbios_gates_GateHwi_Module__loggerFxn8__C;
#ifdef ti_sysbios_gates_GateHwi_Module__loggerFxn8__CR
#define ti_sysbios_gates_GateHwi_Module__loggerFxn8__C (*((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Module__loggerFxn8 (ti_sysbios_gates_GateHwi_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_gates_GateHwi_Object__count;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Object__count ti_sysbios_gates_GateHwi_Object__count__C;
#ifdef ti_sysbios_gates_GateHwi_Object__count__CR
#define ti_sysbios_gates_GateHwi_Object__count__C (*((CT__ti_sysbios_gates_GateHwi_Object__count*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Object__count__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Object__count (ti_sysbios_gates_GateHwi_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_gates_GateHwi_Object__heap;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Object__heap ti_sysbios_gates_GateHwi_Object__heap__C;
#ifdef ti_sysbios_gates_GateHwi_Object__heap__CR
#define ti_sysbios_gates_GateHwi_Object__heap__C (*((CT__ti_sysbios_gates_GateHwi_Object__heap*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Object__heap__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Object__heap (ti_sysbios_gates_GateHwi_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_gates_GateHwi_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Object__sizeof ti_sysbios_gates_GateHwi_Object__sizeof__C;
#ifdef ti_sysbios_gates_GateHwi_Object__sizeof__CR
#define ti_sysbios_gates_GateHwi_Object__sizeof__C (*((CT__ti_sysbios_gates_GateHwi_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Object__sizeof__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Object__sizeof (ti_sysbios_gates_GateHwi_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_gates_GateHwi_Object__table;
__extern __FAR__ const CT__ti_sysbios_gates_GateHwi_Object__table ti_sysbios_gates_GateHwi_Object__table__C;
#ifdef ti_sysbios_gates_GateHwi_Object__table__CR
#define ti_sysbios_gates_GateHwi_Object__table__C (*((CT__ti_sysbios_gates_GateHwi_Object__table*)(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Object__table__C_offset)))
#else
#define ti_sysbios_gates_GateHwi_Object__table (ti_sysbios_gates_GateHwi_Object__table__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_gates_GateHwi_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_gates_GateHwi_Struct {
    const ti_sysbios_gates_GateHwi_Fxns__ *__fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_gates_GateHwi_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*query)(xdc_Int qual);
    xdc_IArg (*enter)(ti_sysbios_gates_GateHwi_Handle inst);
    xdc_Void (*leave)(ti_sysbios_gates_GateHwi_Handle inst, xdc_IArg key);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_gates_GateHwi_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_gates_GateHwi_Fxns__ ti_sysbios_gates_GateHwi_Module__FXNS__C;
#else
#define ti_sysbios_gates_GateHwi_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_gates_GateHwi_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_gates_GateHwi_Module_startup( state ) (-1)

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Instance_init__E, "ti_sysbios_gates_GateHwi_Instance_init")
__extern xdc_Void ti_sysbios_gates_GateHwi_Instance_init__E(ti_sysbios_gates_GateHwi_Object *obj, const ti_sysbios_gates_GateHwi_Params *prms);

/* create */
xdc__CODESECT(ti_sysbios_gates_GateHwi_create, "ti_sysbios_gates_GateHwi_create")
__extern ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_create( const ti_sysbios_gates_GateHwi_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_gates_GateHwi_construct, "ti_sysbios_gates_GateHwi_construct")
__extern void ti_sysbios_gates_GateHwi_construct(ti_sysbios_gates_GateHwi_Struct *obj, const ti_sysbios_gates_GateHwi_Params *prms);

/* delete */
xdc__CODESECT(ti_sysbios_gates_GateHwi_delete, "ti_sysbios_gates_GateHwi_delete")
__extern void ti_sysbios_gates_GateHwi_delete(ti_sysbios_gates_GateHwi_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_gates_GateHwi_destruct, "ti_sysbios_gates_GateHwi_destruct")
__extern void ti_sysbios_gates_GateHwi_destruct(ti_sysbios_gates_GateHwi_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Handle__label__S, "ti_sysbios_gates_GateHwi_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_gates_GateHwi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Module__startupDone__S, "ti_sysbios_gates_GateHwi_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_gates_GateHwi_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Object__create__S, "ti_sysbios_gates_GateHwi_Object__create__S")
__extern xdc_Ptr ti_sysbios_gates_GateHwi_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Object__delete__S, "ti_sysbios_gates_GateHwi_Object__delete__S")
__extern xdc_Void ti_sysbios_gates_GateHwi_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Object__get__S, "ti_sysbios_gates_GateHwi_Object__get__S")
__extern xdc_Ptr ti_sysbios_gates_GateHwi_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Object__first__S, "ti_sysbios_gates_GateHwi_Object__first__S")
__extern xdc_Ptr ti_sysbios_gates_GateHwi_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Object__next__S, "ti_sysbios_gates_GateHwi_Object__next__S")
__extern xdc_Ptr ti_sysbios_gates_GateHwi_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_gates_GateHwi_Params__init__S, "ti_sysbios_gates_GateHwi_Params__init__S")
__extern xdc_Void ti_sysbios_gates_GateHwi_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* query__E */
#define ti_sysbios_gates_GateHwi_query ti_sysbios_gates_GateHwi_query__E
xdc__CODESECT(ti_sysbios_gates_GateHwi_query__E, "ti_sysbios_gates_GateHwi_query")
__extern xdc_Bool ti_sysbios_gates_GateHwi_query__E( xdc_Int qual);

/* enter__E */
#define ti_sysbios_gates_GateHwi_enter ti_sysbios_gates_GateHwi_enter__E
xdc__CODESECT(ti_sysbios_gates_GateHwi_enter__E, "ti_sysbios_gates_GateHwi_enter")
__extern xdc_IArg ti_sysbios_gates_GateHwi_enter__E( ti_sysbios_gates_GateHwi_Handle instp);

/* leave__E */
#define ti_sysbios_gates_GateHwi_leave ti_sysbios_gates_GateHwi_leave__E
xdc__CODESECT(ti_sysbios_gates_GateHwi_leave__E, "ti_sysbios_gates_GateHwi_leave")
__extern xdc_Void ti_sysbios_gates_GateHwi_leave__E( ti_sysbios_gates_GateHwi_Handle instp, xdc_IArg key);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IGateProvider_Module ti_sysbios_gates_GateHwi_Module_upCast(void);
static inline xdc_runtime_IGateProvider_Module ti_sysbios_gates_GateHwi_Module_upCast(void)
{
    return (xdc_runtime_IGateProvider_Module)&ti_sysbios_gates_GateHwi_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateHwi_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_gates_GateHwi_Handle_upCast(ti_sysbios_gates_GateHwi_Handle i);
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_gates_GateHwi_Handle_upCast(ti_sysbios_gates_GateHwi_Handle i)
{
    return (xdc_runtime_IGateProvider_Handle)i;
}

/* Handle_to_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateHwi_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Handle_downCast(xdc_runtime_IGateProvider_Handle i);
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Handle_downCast(xdc_runtime_IGateProvider_Handle i)
{
    xdc_runtime_IGateProvider_Handle i2 = (xdc_runtime_IGateProvider_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_sysbios_gates_GateHwi_Module__FXNS__C) ? (ti_sysbios_gates_GateHwi_Handle)i : (ti_sysbios_gates_GateHwi_Handle)NULL;
}

/* Handle_from_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateHwi_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_gates_GateHwi_Module_startupDone() ti_sysbios_gates_GateHwi_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_gates_GateHwi_Object_heap() ti_sysbios_gates_GateHwi_Object__heap__C

/* Module_heap */
#define ti_sysbios_gates_GateHwi_Module_heap() ti_sysbios_gates_GateHwi_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_gates_GateHwi_Module__id ti_sysbios_gates_GateHwi_Module_id(void);
static inline CT__ti_sysbios_gates_GateHwi_Module__id ti_sysbios_gates_GateHwi_Module_id( void ) 
{
    return ti_sysbios_gates_GateHwi_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_gates_GateHwi_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_gates_GateHwi_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_gates_GateHwi_Module__diagsMask__C != (CT__ti_sysbios_gates_GateHwi_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_gates_GateHwi_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_gates_GateHwi_Module_getMask(void)
{
    return (ti_sysbios_gates_GateHwi_Module__diagsMask__C != (CT__ti_sysbios_gates_GateHwi_Module__diagsMask)NULL) ? *ti_sysbios_gates_GateHwi_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_gates_GateHwi_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_gates_GateHwi_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_gates_GateHwi_Module__diagsMask__C != (CT__ti_sysbios_gates_GateHwi_Module__diagsMask)NULL) {
        *ti_sysbios_gates_GateHwi_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_gates_GateHwi_Params_init(ti_sysbios_gates_GateHwi_Params *prms);
static inline void ti_sysbios_gates_GateHwi_Params_init( ti_sysbios_gates_GateHwi_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_gates_GateHwi_Params__init__S(prms, NULL, sizeof(ti_sysbios_gates_GateHwi_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_gates_GateHwi_Params_copy(ti_sysbios_gates_GateHwi_Params *dst, const ti_sysbios_gates_GateHwi_Params *src);
static inline void ti_sysbios_gates_GateHwi_Params_copy(ti_sysbios_gates_GateHwi_Params *dst, const ti_sysbios_gates_GateHwi_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_gates_GateHwi_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_gates_GateHwi_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_gates_GateHwi_Object_count() ti_sysbios_gates_GateHwi_Object__count__C

/* Object_sizeof */
#define ti_sysbios_gates_GateHwi_Object_sizeof() ti_sysbios_gates_GateHwi_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_get(ti_sysbios_gates_GateHwi_Object *oarr, int i);
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_get(ti_sysbios_gates_GateHwi_Object *oarr, int i) 
{
    return (ti_sysbios_gates_GateHwi_Handle)ti_sysbios_gates_GateHwi_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_first(void);
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_first(void)
{
    return (ti_sysbios_gates_GateHwi_Handle)ti_sysbios_gates_GateHwi_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_next(ti_sysbios_gates_GateHwi_Object *obj);
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_Object_next(ti_sysbios_gates_GateHwi_Object *obj)
{
    return (ti_sysbios_gates_GateHwi_Handle)ti_sysbios_gates_GateHwi_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_gates_GateHwi_Handle_label(ti_sysbios_gates_GateHwi_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_gates_GateHwi_Handle_label(ti_sysbios_gates_GateHwi_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_gates_GateHwi_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_gates_GateHwi_Handle_name(ti_sysbios_gates_GateHwi_Handle inst);
static inline xdc_String ti_sysbios_gates_GateHwi_Handle_name(ti_sysbios_gates_GateHwi_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_gates_GateHwi_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_handle(ti_sysbios_gates_GateHwi_Struct *str);
static inline ti_sysbios_gates_GateHwi_Handle ti_sysbios_gates_GateHwi_handle(ti_sysbios_gates_GateHwi_Struct *str)
{
    return (ti_sysbios_gates_GateHwi_Handle)str;
}

/* struct */
static inline ti_sysbios_gates_GateHwi_Struct *ti_sysbios_gates_GateHwi_struct(ti_sysbios_gates_GateHwi_Handle inst);
static inline ti_sysbios_gates_GateHwi_Struct *ti_sysbios_gates_GateHwi_struct(ti_sysbios_gates_GateHwi_Handle inst)
{
    return (ti_sysbios_gates_GateHwi_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_gates_GateHwi__top__
#undef __nested__
#endif

#endif /* ti_sysbios_gates_GateHwi__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_gates_GateHwi__internalaccess))

#ifndef ti_sysbios_gates_GateHwi__include_state
#define ti_sysbios_gates_GateHwi__include_state

/* Object */
struct ti_sysbios_gates_GateHwi_Object {
    const ti_sysbios_gates_GateHwi_Fxns__ *__fxns;
};

#endif /* ti_sysbios_gates_GateHwi__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_gates_GateHwi__nolocalnames)

#ifndef ti_sysbios_gates_GateHwi__localnames__done
#define ti_sysbios_gates_GateHwi__localnames__done

/* module prefix */
#define GateHwi_Instance ti_sysbios_gates_GateHwi_Instance
#define GateHwi_Handle ti_sysbios_gates_GateHwi_Handle
#define GateHwi_Module ti_sysbios_gates_GateHwi_Module
#define GateHwi_Object ti_sysbios_gates_GateHwi_Object
#define GateHwi_Struct ti_sysbios_gates_GateHwi_Struct
#define GateHwi_Q_BLOCKING ti_sysbios_gates_GateHwi_Q_BLOCKING
#define GateHwi_Q_PREEMPTING ti_sysbios_gates_GateHwi_Q_PREEMPTING
#define GateHwi_Instance_State ti_sysbios_gates_GateHwi_Instance_State
#define GateHwi_Params ti_sysbios_gates_GateHwi_Params
#define GateHwi_query ti_sysbios_gates_GateHwi_query
#define GateHwi_enter ti_sysbios_gates_GateHwi_enter
#define GateHwi_leave ti_sysbios_gates_GateHwi_leave
#define GateHwi_Module_name ti_sysbios_gates_GateHwi_Module_name
#define GateHwi_Module_id ti_sysbios_gates_GateHwi_Module_id
#define GateHwi_Module_startup ti_sysbios_gates_GateHwi_Module_startup
#define GateHwi_Module_startupDone ti_sysbios_gates_GateHwi_Module_startupDone
#define GateHwi_Module_hasMask ti_sysbios_gates_GateHwi_Module_hasMask
#define GateHwi_Module_getMask ti_sysbios_gates_GateHwi_Module_getMask
#define GateHwi_Module_setMask ti_sysbios_gates_GateHwi_Module_setMask
#define GateHwi_Object_heap ti_sysbios_gates_GateHwi_Object_heap
#define GateHwi_Module_heap ti_sysbios_gates_GateHwi_Module_heap
#define GateHwi_construct ti_sysbios_gates_GateHwi_construct
#define GateHwi_create ti_sysbios_gates_GateHwi_create
#define GateHwi_handle ti_sysbios_gates_GateHwi_handle
#define GateHwi_struct ti_sysbios_gates_GateHwi_struct
#define GateHwi_Handle_label ti_sysbios_gates_GateHwi_Handle_label
#define GateHwi_Handle_name ti_sysbios_gates_GateHwi_Handle_name
#define GateHwi_Instance_init ti_sysbios_gates_GateHwi_Instance_init
#define GateHwi_Object_count ti_sysbios_gates_GateHwi_Object_count
#define GateHwi_Object_get ti_sysbios_gates_GateHwi_Object_get
#define GateHwi_Object_first ti_sysbios_gates_GateHwi_Object_first
#define GateHwi_Object_next ti_sysbios_gates_GateHwi_Object_next
#define GateHwi_Object_sizeof ti_sysbios_gates_GateHwi_Object_sizeof
#define GateHwi_Params_copy ti_sysbios_gates_GateHwi_Params_copy
#define GateHwi_Params_init ti_sysbios_gates_GateHwi_Params_init
#define GateHwi_delete ti_sysbios_gates_GateHwi_delete
#define GateHwi_destruct ti_sysbios_gates_GateHwi_destruct
#define GateHwi_Module_upCast ti_sysbios_gates_GateHwi_Module_upCast
#define GateHwi_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Module_to_xdc_runtime_IGateProvider
#define GateHwi_Handle_upCast ti_sysbios_gates_GateHwi_Handle_upCast
#define GateHwi_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_to_xdc_runtime_IGateProvider
#define GateHwi_Handle_downCast ti_sysbios_gates_GateHwi_Handle_downCast
#define GateHwi_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_from_xdc_runtime_IGateProvider

#endif /* ti_sysbios_gates_GateHwi__localnames__done */
#endif
