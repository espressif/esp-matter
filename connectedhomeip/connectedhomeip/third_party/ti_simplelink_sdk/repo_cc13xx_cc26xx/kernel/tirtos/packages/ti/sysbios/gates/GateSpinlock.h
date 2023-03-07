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

#ifndef ti_sysbios_gates_GateSpinlock__include
#define ti_sysbios_gates_GateSpinlock__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_gates_GateSpinlock__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_gates_GateSpinlock___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/gates/package/package.defs.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/IGateProvider.h>
#include <ti/sysbios/knl/Task.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Q_BLOCKING */
#define ti_sysbios_gates_GateSpinlock_Q_BLOCKING (1)

/* Q_PREEMPTING */
#define ti_sysbios_gates_GateSpinlock_Q_PREEMPTING (2)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_gates_GateSpinlock_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__diagsEnabled ti_sysbios_gates_GateSpinlock_Module__diagsEnabled__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__diagsEnabled__CR
#define ti_sysbios_gates_GateSpinlock_Module__diagsEnabled__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__diagsEnabled (ti_sysbios_gates_GateSpinlock_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_gates_GateSpinlock_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__diagsIncluded ti_sysbios_gates_GateSpinlock_Module__diagsIncluded__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__diagsIncluded__CR
#define ti_sysbios_gates_GateSpinlock_Module__diagsIncluded__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__diagsIncluded (ti_sysbios_gates_GateSpinlock_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask ti_sysbios_gates_GateSpinlock_Module__diagsMask__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__diagsMask__CR
#define ti_sysbios_gates_GateSpinlock_Module__diagsMask__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__diagsMask (ti_sysbios_gates_GateSpinlock_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_gates_GateSpinlock_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__gateObj ti_sysbios_gates_GateSpinlock_Module__gateObj__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__gateObj__CR
#define ti_sysbios_gates_GateSpinlock_Module__gateObj__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__gateObj__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__gateObj (ti_sysbios_gates_GateSpinlock_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_gates_GateSpinlock_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__gatePrms ti_sysbios_gates_GateSpinlock_Module__gatePrms__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__gatePrms__CR
#define ti_sysbios_gates_GateSpinlock_Module__gatePrms__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__gatePrms (ti_sysbios_gates_GateSpinlock_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_gates_GateSpinlock_Module__id;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__id ti_sysbios_gates_GateSpinlock_Module__id__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__id__CR
#define ti_sysbios_gates_GateSpinlock_Module__id__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__id*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__id__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__id (ti_sysbios_gates_GateSpinlock_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_gates_GateSpinlock_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerDefined ti_sysbios_gates_GateSpinlock_Module__loggerDefined__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerDefined__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerDefined__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerDefined (ti_sysbios_gates_GateSpinlock_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_gates_GateSpinlock_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerObj ti_sysbios_gates_GateSpinlock_Module__loggerObj__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerObj__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerObj__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerObj (ti_sysbios_gates_GateSpinlock_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn0 ti_sysbios_gates_GateSpinlock_Module__loggerFxn0__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerFxn0__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn0__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn0 (ti_sysbios_gates_GateSpinlock_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn1 ti_sysbios_gates_GateSpinlock_Module__loggerFxn1__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerFxn1__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn1__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn1 (ti_sysbios_gates_GateSpinlock_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn2 ti_sysbios_gates_GateSpinlock_Module__loggerFxn2__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerFxn2__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn2__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn2 (ti_sysbios_gates_GateSpinlock_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn4 ti_sysbios_gates_GateSpinlock_Module__loggerFxn4__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerFxn4__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn4__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn4 (ti_sysbios_gates_GateSpinlock_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn8 ti_sysbios_gates_GateSpinlock_Module__loggerFxn8__C;
#ifdef ti_sysbios_gates_GateSpinlock_Module__loggerFxn8__CR
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn8__C (*((CT__ti_sysbios_gates_GateSpinlock_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Module__loggerFxn8 (ti_sysbios_gates_GateSpinlock_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_gates_GateSpinlock_Object__count;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Object__count ti_sysbios_gates_GateSpinlock_Object__count__C;
#ifdef ti_sysbios_gates_GateSpinlock_Object__count__CR
#define ti_sysbios_gates_GateSpinlock_Object__count__C (*((CT__ti_sysbios_gates_GateSpinlock_Object__count*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Object__count__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Object__count (ti_sysbios_gates_GateSpinlock_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_gates_GateSpinlock_Object__heap;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Object__heap ti_sysbios_gates_GateSpinlock_Object__heap__C;
#ifdef ti_sysbios_gates_GateSpinlock_Object__heap__CR
#define ti_sysbios_gates_GateSpinlock_Object__heap__C (*((CT__ti_sysbios_gates_GateSpinlock_Object__heap*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Object__heap__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Object__heap (ti_sysbios_gates_GateSpinlock_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_gates_GateSpinlock_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Object__sizeof ti_sysbios_gates_GateSpinlock_Object__sizeof__C;
#ifdef ti_sysbios_gates_GateSpinlock_Object__sizeof__CR
#define ti_sysbios_gates_GateSpinlock_Object__sizeof__C (*((CT__ti_sysbios_gates_GateSpinlock_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Object__sizeof__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Object__sizeof (ti_sysbios_gates_GateSpinlock_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_gates_GateSpinlock_Object__table;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_Object__table ti_sysbios_gates_GateSpinlock_Object__table__C;
#ifdef ti_sysbios_gates_GateSpinlock_Object__table__CR
#define ti_sysbios_gates_GateSpinlock_Object__table__C (*((CT__ti_sysbios_gates_GateSpinlock_Object__table*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Object__table__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_Object__table (ti_sysbios_gates_GateSpinlock_Object__table__C)
#endif

/* A_badContext */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_gates_GateSpinlock_A_badContext;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_A_badContext ti_sysbios_gates_GateSpinlock_A_badContext__C;
#ifdef ti_sysbios_gates_GateSpinlock_A_badContext__CR
#define ti_sysbios_gates_GateSpinlock_A_badContext (*((CT__ti_sysbios_gates_GateSpinlock_A_badContext*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_A_badContext__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_A_badContext (ti_sysbios_gates_GateSpinlock_A_badContext__C)
#endif

/* A_invalidQuality */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_gates_GateSpinlock_A_invalidQuality;
__extern __FAR__ const CT__ti_sysbios_gates_GateSpinlock_A_invalidQuality ti_sysbios_gates_GateSpinlock_A_invalidQuality__C;
#ifdef ti_sysbios_gates_GateSpinlock_A_invalidQuality__CR
#define ti_sysbios_gates_GateSpinlock_A_invalidQuality (*((CT__ti_sysbios_gates_GateSpinlock_A_invalidQuality*)(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_A_invalidQuality__C_offset)))
#else
#define ti_sysbios_gates_GateSpinlock_A_invalidQuality (ti_sysbios_gates_GateSpinlock_A_invalidQuality__C)
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_gates_GateSpinlock_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_gates_GateSpinlock_Struct {
    const ti_sysbios_gates_GateSpinlock_Fxns__ *__fxns;
    volatile xdc_UInt f0;
    volatile ti_sysbios_knl_Task_Handle f1;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_gates_GateSpinlock_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*query)(xdc_Int qual);
    xdc_IArg (*enter)(ti_sysbios_gates_GateSpinlock_Handle inst);
    xdc_Void (*leave)(ti_sysbios_gates_GateSpinlock_Handle inst, xdc_IArg key);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_gates_GateSpinlock_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_gates_GateSpinlock_Fxns__ ti_sysbios_gates_GateSpinlock_Module__FXNS__C;
#else
#define ti_sysbios_gates_GateSpinlock_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_gates_GateSpinlock_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_gates_GateSpinlock_Module_startup( state ) (-1)

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Instance_init__E, "ti_sysbios_gates_GateSpinlock_Instance_init")
__extern xdc_Void ti_sysbios_gates_GateSpinlock_Instance_init__E(ti_sysbios_gates_GateSpinlock_Object *obj, const ti_sysbios_gates_GateSpinlock_Params *prms);

/* create */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_create, "ti_sysbios_gates_GateSpinlock_create")
__extern ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_create( const ti_sysbios_gates_GateSpinlock_Params *prms, xdc_runtime_Error_Block *eb);

/* construct */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_construct, "ti_sysbios_gates_GateSpinlock_construct")
__extern void ti_sysbios_gates_GateSpinlock_construct(ti_sysbios_gates_GateSpinlock_Struct *obj, const ti_sysbios_gates_GateSpinlock_Params *prms);

/* delete */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_delete, "ti_sysbios_gates_GateSpinlock_delete")
__extern void ti_sysbios_gates_GateSpinlock_delete(ti_sysbios_gates_GateSpinlock_Handle *instp);

/* destruct */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_destruct, "ti_sysbios_gates_GateSpinlock_destruct")
__extern void ti_sysbios_gates_GateSpinlock_destruct(ti_sysbios_gates_GateSpinlock_Struct *obj);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Handle__label__S, "ti_sysbios_gates_GateSpinlock_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_gates_GateSpinlock_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Module__startupDone__S, "ti_sysbios_gates_GateSpinlock_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_gates_GateSpinlock_Module__startupDone__S( void);

/* Object__create__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Object__create__S, "ti_sysbios_gates_GateSpinlock_Object__create__S")
__extern xdc_Ptr ti_sysbios_gates_GateSpinlock_Object__create__S( xdc_CPtr aa, const xdc_UChar *pa, xdc_SizeT psz, xdc_runtime_Error_Block *eb);

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Object__delete__S, "ti_sysbios_gates_GateSpinlock_Object__delete__S")
__extern xdc_Void ti_sysbios_gates_GateSpinlock_Object__delete__S( xdc_Ptr instp);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Object__get__S, "ti_sysbios_gates_GateSpinlock_Object__get__S")
__extern xdc_Ptr ti_sysbios_gates_GateSpinlock_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Object__first__S, "ti_sysbios_gates_GateSpinlock_Object__first__S")
__extern xdc_Ptr ti_sysbios_gates_GateSpinlock_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Object__next__S, "ti_sysbios_gates_GateSpinlock_Object__next__S")
__extern xdc_Ptr ti_sysbios_gates_GateSpinlock_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_Params__init__S, "ti_sysbios_gates_GateSpinlock_Params__init__S")
__extern xdc_Void ti_sysbios_gates_GateSpinlock_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* query__E */
#define ti_sysbios_gates_GateSpinlock_query ti_sysbios_gates_GateSpinlock_query__E
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_query__E, "ti_sysbios_gates_GateSpinlock_query")
__extern xdc_Bool ti_sysbios_gates_GateSpinlock_query__E( xdc_Int qual);

/* enter__E */
#define ti_sysbios_gates_GateSpinlock_enter ti_sysbios_gates_GateSpinlock_enter__E
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_enter__E, "ti_sysbios_gates_GateSpinlock_enter")
__extern xdc_IArg ti_sysbios_gates_GateSpinlock_enter__E( ti_sysbios_gates_GateSpinlock_Handle instp);

/* enterHwi__E */
#define ti_sysbios_gates_GateSpinlock_enterHwi ti_sysbios_gates_GateSpinlock_enterHwi__E
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_enterHwi__E, "ti_sysbios_gates_GateSpinlock_enterHwi")
__extern xdc_IArg ti_sysbios_gates_GateSpinlock_enterHwi__E( ti_sysbios_gates_GateSpinlock_Handle instp);

/* leave__E */
#define ti_sysbios_gates_GateSpinlock_leave ti_sysbios_gates_GateSpinlock_leave__E
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_leave__E, "ti_sysbios_gates_GateSpinlock_leave")
__extern xdc_Void ti_sysbios_gates_GateSpinlock_leave__E( ti_sysbios_gates_GateSpinlock_Handle instp, xdc_IArg key);

/* leaveHwi__E */
#define ti_sysbios_gates_GateSpinlock_leaveHwi ti_sysbios_gates_GateSpinlock_leaveHwi__E
xdc__CODESECT(ti_sysbios_gates_GateSpinlock_leaveHwi__E, "ti_sysbios_gates_GateSpinlock_leaveHwi")
__extern xdc_Void ti_sysbios_gates_GateSpinlock_leaveHwi__E( ti_sysbios_gates_GateSpinlock_Handle instp, xdc_IArg key);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IGateProvider_Module ti_sysbios_gates_GateSpinlock_Module_upCast(void);
static inline xdc_runtime_IGateProvider_Module ti_sysbios_gates_GateSpinlock_Module_upCast(void)
{
    return (xdc_runtime_IGateProvider_Module)&ti_sysbios_gates_GateSpinlock_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateSpinlock_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_gates_GateSpinlock_Handle_upCast(ti_sysbios_gates_GateSpinlock_Handle i);
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_gates_GateSpinlock_Handle_upCast(ti_sysbios_gates_GateSpinlock_Handle i)
{
    return (xdc_runtime_IGateProvider_Handle)i;
}

/* Handle_to_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateSpinlock_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Handle_downCast(xdc_runtime_IGateProvider_Handle i);
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Handle_downCast(xdc_runtime_IGateProvider_Handle i)
{
    xdc_runtime_IGateProvider_Handle i2 = (xdc_runtime_IGateProvider_Handle)i;
    return ((const void*)i2->__fxns == (const void*)&ti_sysbios_gates_GateSpinlock_Module__FXNS__C) ? (ti_sysbios_gates_GateSpinlock_Handle)i : (ti_sysbios_gates_GateSpinlock_Handle)NULL;
}

/* Handle_from_xdc_runtime_IGateProvider */
#define ti_sysbios_gates_GateSpinlock_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_gates_GateSpinlock_Module_startupDone() ti_sysbios_gates_GateSpinlock_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_gates_GateSpinlock_Object_heap() ti_sysbios_gates_GateSpinlock_Object__heap__C

/* Module_heap */
#define ti_sysbios_gates_GateSpinlock_Module_heap() ti_sysbios_gates_GateSpinlock_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_gates_GateSpinlock_Module__id ti_sysbios_gates_GateSpinlock_Module_id(void);
static inline CT__ti_sysbios_gates_GateSpinlock_Module__id ti_sysbios_gates_GateSpinlock_Module_id( void ) 
{
    return ti_sysbios_gates_GateSpinlock_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_gates_GateSpinlock_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_gates_GateSpinlock_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_gates_GateSpinlock_Module__diagsMask__C != (CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_gates_GateSpinlock_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_gates_GateSpinlock_Module_getMask(void)
{
    return (ti_sysbios_gates_GateSpinlock_Module__diagsMask__C != (CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask)NULL) ? *ti_sysbios_gates_GateSpinlock_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_gates_GateSpinlock_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_gates_GateSpinlock_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_gates_GateSpinlock_Module__diagsMask__C != (CT__ti_sysbios_gates_GateSpinlock_Module__diagsMask)NULL) {
        *ti_sysbios_gates_GateSpinlock_Module__diagsMask__C = mask;
    }
}

/* Params_init */
static inline void ti_sysbios_gates_GateSpinlock_Params_init(ti_sysbios_gates_GateSpinlock_Params *prms);
static inline void ti_sysbios_gates_GateSpinlock_Params_init( ti_sysbios_gates_GateSpinlock_Params *prms ) 
{
    if (prms != NULL) {
        ti_sysbios_gates_GateSpinlock_Params__init__S(prms, NULL, sizeof(ti_sysbios_gates_GateSpinlock_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_gates_GateSpinlock_Params_copy(ti_sysbios_gates_GateSpinlock_Params *dst, const ti_sysbios_gates_GateSpinlock_Params *src);
static inline void ti_sysbios_gates_GateSpinlock_Params_copy(ti_sysbios_gates_GateSpinlock_Params *dst, const ti_sysbios_gates_GateSpinlock_Params *src) 
{
    if (dst != NULL) {
        ti_sysbios_gates_GateSpinlock_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_gates_GateSpinlock_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_gates_GateSpinlock_Object_count() ti_sysbios_gates_GateSpinlock_Object__count__C

/* Object_sizeof */
#define ti_sysbios_gates_GateSpinlock_Object_sizeof() ti_sysbios_gates_GateSpinlock_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_get(ti_sysbios_gates_GateSpinlock_Object *oarr, int i);
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_get(ti_sysbios_gates_GateSpinlock_Object *oarr, int i) 
{
    return (ti_sysbios_gates_GateSpinlock_Handle)ti_sysbios_gates_GateSpinlock_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_first(void);
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_first(void)
{
    return (ti_sysbios_gates_GateSpinlock_Handle)ti_sysbios_gates_GateSpinlock_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_next(ti_sysbios_gates_GateSpinlock_Object *obj);
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Object_next(ti_sysbios_gates_GateSpinlock_Object *obj)
{
    return (ti_sysbios_gates_GateSpinlock_Handle)ti_sysbios_gates_GateSpinlock_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_gates_GateSpinlock_Handle_label(ti_sysbios_gates_GateSpinlock_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_gates_GateSpinlock_Handle_label(ti_sysbios_gates_GateSpinlock_Handle inst, xdc_runtime_Types_Label *lab)
{
    return ti_sysbios_gates_GateSpinlock_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline xdc_String ti_sysbios_gates_GateSpinlock_Handle_name(ti_sysbios_gates_GateSpinlock_Handle inst);
static inline xdc_String ti_sysbios_gates_GateSpinlock_Handle_name(ti_sysbios_gates_GateSpinlock_Handle inst)
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_gates_GateSpinlock_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_handle(ti_sysbios_gates_GateSpinlock_Struct *str);
static inline ti_sysbios_gates_GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_handle(ti_sysbios_gates_GateSpinlock_Struct *str)
{
    return (ti_sysbios_gates_GateSpinlock_Handle)str;
}

/* struct */
static inline ti_sysbios_gates_GateSpinlock_Struct *ti_sysbios_gates_GateSpinlock_struct(ti_sysbios_gates_GateSpinlock_Handle inst);
static inline ti_sysbios_gates_GateSpinlock_Struct *ti_sysbios_gates_GateSpinlock_struct(ti_sysbios_gates_GateSpinlock_Handle inst)
{
    return (ti_sysbios_gates_GateSpinlock_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_gates_GateSpinlock__top__
#undef __nested__
#endif

#endif /* ti_sysbios_gates_GateSpinlock__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_gates_GateSpinlock__internalaccess))

#ifndef ti_sysbios_gates_GateSpinlock__include_state
#define ti_sysbios_gates_GateSpinlock__include_state

/* Object */
struct ti_sysbios_gates_GateSpinlock_Object {
    const ti_sysbios_gates_GateSpinlock_Fxns__ *__fxns;
    volatile xdc_UInt hwiKey;
    volatile ti_sysbios_knl_Task_Handle owner;
};

#endif /* ti_sysbios_gates_GateSpinlock__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_gates_GateSpinlock__nolocalnames)

#ifndef ti_sysbios_gates_GateSpinlock__localnames__done
#define ti_sysbios_gates_GateSpinlock__localnames__done

/* module prefix */
#define GateSpinlock_Instance ti_sysbios_gates_GateSpinlock_Instance
#define GateSpinlock_Handle ti_sysbios_gates_GateSpinlock_Handle
#define GateSpinlock_Module ti_sysbios_gates_GateSpinlock_Module
#define GateSpinlock_Object ti_sysbios_gates_GateSpinlock_Object
#define GateSpinlock_Struct ti_sysbios_gates_GateSpinlock_Struct
#define GateSpinlock_Q_BLOCKING ti_sysbios_gates_GateSpinlock_Q_BLOCKING
#define GateSpinlock_Q_PREEMPTING ti_sysbios_gates_GateSpinlock_Q_PREEMPTING
#define GateSpinlock_Instance_State ti_sysbios_gates_GateSpinlock_Instance_State
#define GateSpinlock_A_badContext ti_sysbios_gates_GateSpinlock_A_badContext
#define GateSpinlock_A_invalidQuality ti_sysbios_gates_GateSpinlock_A_invalidQuality
#define GateSpinlock_Params ti_sysbios_gates_GateSpinlock_Params
#define GateSpinlock_query ti_sysbios_gates_GateSpinlock_query
#define GateSpinlock_enter ti_sysbios_gates_GateSpinlock_enter
#define GateSpinlock_enterHwi ti_sysbios_gates_GateSpinlock_enterHwi
#define GateSpinlock_leave ti_sysbios_gates_GateSpinlock_leave
#define GateSpinlock_leaveHwi ti_sysbios_gates_GateSpinlock_leaveHwi
#define GateSpinlock_Module_name ti_sysbios_gates_GateSpinlock_Module_name
#define GateSpinlock_Module_id ti_sysbios_gates_GateSpinlock_Module_id
#define GateSpinlock_Module_startup ti_sysbios_gates_GateSpinlock_Module_startup
#define GateSpinlock_Module_startupDone ti_sysbios_gates_GateSpinlock_Module_startupDone
#define GateSpinlock_Module_hasMask ti_sysbios_gates_GateSpinlock_Module_hasMask
#define GateSpinlock_Module_getMask ti_sysbios_gates_GateSpinlock_Module_getMask
#define GateSpinlock_Module_setMask ti_sysbios_gates_GateSpinlock_Module_setMask
#define GateSpinlock_Object_heap ti_sysbios_gates_GateSpinlock_Object_heap
#define GateSpinlock_Module_heap ti_sysbios_gates_GateSpinlock_Module_heap
#define GateSpinlock_construct ti_sysbios_gates_GateSpinlock_construct
#define GateSpinlock_create ti_sysbios_gates_GateSpinlock_create
#define GateSpinlock_handle ti_sysbios_gates_GateSpinlock_handle
#define GateSpinlock_struct ti_sysbios_gates_GateSpinlock_struct
#define GateSpinlock_Handle_label ti_sysbios_gates_GateSpinlock_Handle_label
#define GateSpinlock_Handle_name ti_sysbios_gates_GateSpinlock_Handle_name
#define GateSpinlock_Instance_init ti_sysbios_gates_GateSpinlock_Instance_init
#define GateSpinlock_Object_count ti_sysbios_gates_GateSpinlock_Object_count
#define GateSpinlock_Object_get ti_sysbios_gates_GateSpinlock_Object_get
#define GateSpinlock_Object_first ti_sysbios_gates_GateSpinlock_Object_first
#define GateSpinlock_Object_next ti_sysbios_gates_GateSpinlock_Object_next
#define GateSpinlock_Object_sizeof ti_sysbios_gates_GateSpinlock_Object_sizeof
#define GateSpinlock_Params_copy ti_sysbios_gates_GateSpinlock_Params_copy
#define GateSpinlock_Params_init ti_sysbios_gates_GateSpinlock_Params_init
#define GateSpinlock_delete ti_sysbios_gates_GateSpinlock_delete
#define GateSpinlock_destruct ti_sysbios_gates_GateSpinlock_destruct
#define GateSpinlock_Module_upCast ti_sysbios_gates_GateSpinlock_Module_upCast
#define GateSpinlock_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Module_to_xdc_runtime_IGateProvider
#define GateSpinlock_Handle_upCast ti_sysbios_gates_GateSpinlock_Handle_upCast
#define GateSpinlock_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Handle_to_xdc_runtime_IGateProvider
#define GateSpinlock_Handle_downCast ti_sysbios_gates_GateSpinlock_Handle_downCast
#define GateSpinlock_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateSpinlock_Handle_from_xdc_runtime_IGateProvider

#endif /* ti_sysbios_gates_GateSpinlock__localnames__done */
#endif
