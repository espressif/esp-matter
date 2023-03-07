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

#ifndef ti_sysbios_knl_Idle__include
#define ti_sysbios_knl_Idle__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_knl_Idle__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_knl_Idle___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/package/package.defs.h>

#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_knl_Idle_FuncPtr)(void);


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_knl_Idle_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsEnabled ti_sysbios_knl_Idle_Module__diagsEnabled__C;
#ifdef ti_sysbios_knl_Idle_Module__diagsEnabled__CR
#define ti_sysbios_knl_Idle_Module__diagsEnabled__C (*((CT__ti_sysbios_knl_Idle_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__diagsEnabled (ti_sysbios_knl_Idle_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_knl_Idle_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsIncluded ti_sysbios_knl_Idle_Module__diagsIncluded__C;
#ifdef ti_sysbios_knl_Idle_Module__diagsIncluded__CR
#define ti_sysbios_knl_Idle_Module__diagsIncluded__C (*((CT__ti_sysbios_knl_Idle_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__diagsIncluded (ti_sysbios_knl_Idle_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_knl_Idle_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsMask ti_sysbios_knl_Idle_Module__diagsMask__C;
#ifdef ti_sysbios_knl_Idle_Module__diagsMask__CR
#define ti_sysbios_knl_Idle_Module__diagsMask__C (*((CT__ti_sysbios_knl_Idle_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__diagsMask (ti_sysbios_knl_Idle_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_knl_Idle_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__gateObj ti_sysbios_knl_Idle_Module__gateObj__C;
#ifdef ti_sysbios_knl_Idle_Module__gateObj__CR
#define ti_sysbios_knl_Idle_Module__gateObj__C (*((CT__ti_sysbios_knl_Idle_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__gateObj__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__gateObj (ti_sysbios_knl_Idle_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_knl_Idle_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__gatePrms ti_sysbios_knl_Idle_Module__gatePrms__C;
#ifdef ti_sysbios_knl_Idle_Module__gatePrms__CR
#define ti_sysbios_knl_Idle_Module__gatePrms__C (*((CT__ti_sysbios_knl_Idle_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__gatePrms (ti_sysbios_knl_Idle_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_knl_Idle_Module__id;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__id ti_sysbios_knl_Idle_Module__id__C;
#ifdef ti_sysbios_knl_Idle_Module__id__CR
#define ti_sysbios_knl_Idle_Module__id__C (*((CT__ti_sysbios_knl_Idle_Module__id*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__id__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__id (ti_sysbios_knl_Idle_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_knl_Idle_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerDefined ti_sysbios_knl_Idle_Module__loggerDefined__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerDefined__CR
#define ti_sysbios_knl_Idle_Module__loggerDefined__C (*((CT__ti_sysbios_knl_Idle_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerDefined (ti_sysbios_knl_Idle_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_knl_Idle_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerObj ti_sysbios_knl_Idle_Module__loggerObj__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerObj__CR
#define ti_sysbios_knl_Idle_Module__loggerObj__C (*((CT__ti_sysbios_knl_Idle_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerObj (ti_sysbios_knl_Idle_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_knl_Idle_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn0 ti_sysbios_knl_Idle_Module__loggerFxn0__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerFxn0__CR
#define ti_sysbios_knl_Idle_Module__loggerFxn0__C (*((CT__ti_sysbios_knl_Idle_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerFxn0 (ti_sysbios_knl_Idle_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_knl_Idle_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn1 ti_sysbios_knl_Idle_Module__loggerFxn1__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerFxn1__CR
#define ti_sysbios_knl_Idle_Module__loggerFxn1__C (*((CT__ti_sysbios_knl_Idle_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerFxn1 (ti_sysbios_knl_Idle_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_knl_Idle_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn2 ti_sysbios_knl_Idle_Module__loggerFxn2__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerFxn2__CR
#define ti_sysbios_knl_Idle_Module__loggerFxn2__C (*((CT__ti_sysbios_knl_Idle_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerFxn2 (ti_sysbios_knl_Idle_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_knl_Idle_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn4 ti_sysbios_knl_Idle_Module__loggerFxn4__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerFxn4__CR
#define ti_sysbios_knl_Idle_Module__loggerFxn4__C (*((CT__ti_sysbios_knl_Idle_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerFxn4 (ti_sysbios_knl_Idle_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_knl_Idle_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn8 ti_sysbios_knl_Idle_Module__loggerFxn8__C;
#ifdef ti_sysbios_knl_Idle_Module__loggerFxn8__CR
#define ti_sysbios_knl_Idle_Module__loggerFxn8__C (*((CT__ti_sysbios_knl_Idle_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_knl_Idle_Module__loggerFxn8 (ti_sysbios_knl_Idle_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_knl_Idle_Object__count;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Object__count ti_sysbios_knl_Idle_Object__count__C;
#ifdef ti_sysbios_knl_Idle_Object__count__CR
#define ti_sysbios_knl_Idle_Object__count__C (*((CT__ti_sysbios_knl_Idle_Object__count*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Object__count__C_offset)))
#else
#define ti_sysbios_knl_Idle_Object__count (ti_sysbios_knl_Idle_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_knl_Idle_Object__heap;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Object__heap ti_sysbios_knl_Idle_Object__heap__C;
#ifdef ti_sysbios_knl_Idle_Object__heap__CR
#define ti_sysbios_knl_Idle_Object__heap__C (*((CT__ti_sysbios_knl_Idle_Object__heap*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Object__heap__C_offset)))
#else
#define ti_sysbios_knl_Idle_Object__heap (ti_sysbios_knl_Idle_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_knl_Idle_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Object__sizeof ti_sysbios_knl_Idle_Object__sizeof__C;
#ifdef ti_sysbios_knl_Idle_Object__sizeof__CR
#define ti_sysbios_knl_Idle_Object__sizeof__C (*((CT__ti_sysbios_knl_Idle_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Object__sizeof__C_offset)))
#else
#define ti_sysbios_knl_Idle_Object__sizeof (ti_sysbios_knl_Idle_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_knl_Idle_Object__table;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_Object__table ti_sysbios_knl_Idle_Object__table__C;
#ifdef ti_sysbios_knl_Idle_Object__table__CR
#define ti_sysbios_knl_Idle_Object__table__C (*((CT__ti_sysbios_knl_Idle_Object__table*)(xdcRomConstPtr + ti_sysbios_knl_Idle_Object__table__C_offset)))
#else
#define ti_sysbios_knl_Idle_Object__table (ti_sysbios_knl_Idle_Object__table__C)
#endif

/* funcList */
typedef ti_sysbios_knl_Idle_FuncPtr __T1_ti_sysbios_knl_Idle_funcList;
typedef struct { int length; ti_sysbios_knl_Idle_FuncPtr *elem; } ARRAY1_ti_sysbios_knl_Idle_funcList;
typedef struct { int length; ti_sysbios_knl_Idle_FuncPtr const *elem; } CARRAY1_ti_sysbios_knl_Idle_funcList;
typedef CARRAY1_ti_sysbios_knl_Idle_funcList __TA_ti_sysbios_knl_Idle_funcList;
typedef CARRAY1_ti_sysbios_knl_Idle_funcList CT__ti_sysbios_knl_Idle_funcList;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_funcList ti_sysbios_knl_Idle_funcList__C;
#ifdef ti_sysbios_knl_Idle_funcList__CR
#define ti_sysbios_knl_Idle_funcList (*((CT__ti_sysbios_knl_Idle_funcList*)(xdcRomConstPtr + ti_sysbios_knl_Idle_funcList__C_offset)))
#else
#define ti_sysbios_knl_Idle_funcList (ti_sysbios_knl_Idle_funcList__C)
#endif

/* coreList */
typedef xdc_UInt __T1_ti_sysbios_knl_Idle_coreList;
typedef struct { int length; xdc_UInt *elem; } ARRAY1_ti_sysbios_knl_Idle_coreList;
typedef struct { int length; xdc_UInt const *elem; } CARRAY1_ti_sysbios_knl_Idle_coreList;
typedef CARRAY1_ti_sysbios_knl_Idle_coreList __TA_ti_sysbios_knl_Idle_coreList;
typedef CARRAY1_ti_sysbios_knl_Idle_coreList CT__ti_sysbios_knl_Idle_coreList;
__extern __FAR__ const CT__ti_sysbios_knl_Idle_coreList ti_sysbios_knl_Idle_coreList__C;
#ifdef ti_sysbios_knl_Idle_coreList__CR
#define ti_sysbios_knl_Idle_coreList (*((CT__ti_sysbios_knl_Idle_coreList*)(xdcRomConstPtr + ti_sysbios_knl_Idle_coreList__C_offset)))
#else
#define ti_sysbios_knl_Idle_coreList (ti_sysbios_knl_Idle_coreList__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_knl_Idle_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_knl_Idle_Module__startupDone__S, "ti_sysbios_knl_Idle_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_knl_Idle_Module__startupDone__S( void);

/* loop__E */
#define ti_sysbios_knl_Idle_loop ti_sysbios_knl_Idle_loop__E
xdc__CODESECT(ti_sysbios_knl_Idle_loop__E, "ti_sysbios_knl_Idle_loop")
__extern xdc_Void ti_sysbios_knl_Idle_loop__E( xdc_UArg arg1, xdc_UArg arg2);

/* run__E */
#define ti_sysbios_knl_Idle_run ti_sysbios_knl_Idle_run__E
xdc__CODESECT(ti_sysbios_knl_Idle_run__E, "ti_sysbios_knl_Idle_run")
__extern xdc_Void ti_sysbios_knl_Idle_run__E( void);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_knl_Idle_Module_startupDone() ti_sysbios_knl_Idle_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_knl_Idle_Object_heap() ti_sysbios_knl_Idle_Object__heap__C

/* Module_heap */
#define ti_sysbios_knl_Idle_Module_heap() ti_sysbios_knl_Idle_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_knl_Idle_Module__id ti_sysbios_knl_Idle_Module_id(void);
static inline CT__ti_sysbios_knl_Idle_Module__id ti_sysbios_knl_Idle_Module_id( void ) 
{
    return ti_sysbios_knl_Idle_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_knl_Idle_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_knl_Idle_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_knl_Idle_Module__diagsMask__C != (CT__ti_sysbios_knl_Idle_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_knl_Idle_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_knl_Idle_Module_getMask(void)
{
    return (ti_sysbios_knl_Idle_Module__diagsMask__C != (CT__ti_sysbios_knl_Idle_Module__diagsMask)NULL) ? *ti_sysbios_knl_Idle_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_knl_Idle_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_knl_Idle_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_knl_Idle_Module__diagsMask__C != (CT__ti_sysbios_knl_Idle_Module__diagsMask)NULL) {
        *ti_sysbios_knl_Idle_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_knl_Idle__top__
#undef __nested__
#endif

#endif /* ti_sysbios_knl_Idle__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_knl_Idle__internalaccess))

#ifndef ti_sysbios_knl_Idle__include_state
#define ti_sysbios_knl_Idle__include_state


#endif /* ti_sysbios_knl_Idle__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_knl_Idle__nolocalnames)

#ifndef ti_sysbios_knl_Idle__localnames__done
#define ti_sysbios_knl_Idle__localnames__done

/* module prefix */
#define Idle_FuncPtr ti_sysbios_knl_Idle_FuncPtr
#define Idle_funcList ti_sysbios_knl_Idle_funcList
#define Idle_coreList ti_sysbios_knl_Idle_coreList
#define Idle_loop ti_sysbios_knl_Idle_loop
#define Idle_run ti_sysbios_knl_Idle_run
#define Idle_Module_name ti_sysbios_knl_Idle_Module_name
#define Idle_Module_id ti_sysbios_knl_Idle_Module_id
#define Idle_Module_startup ti_sysbios_knl_Idle_Module_startup
#define Idle_Module_startupDone ti_sysbios_knl_Idle_Module_startupDone
#define Idle_Module_hasMask ti_sysbios_knl_Idle_Module_hasMask
#define Idle_Module_getMask ti_sysbios_knl_Idle_Module_getMask
#define Idle_Module_setMask ti_sysbios_knl_Idle_Module_setMask
#define Idle_Object_heap ti_sysbios_knl_Idle_Object_heap
#define Idle_Module_heap ti_sysbios_knl_Idle_Module_heap

#endif /* ti_sysbios_knl_Idle__localnames__done */
#endif
