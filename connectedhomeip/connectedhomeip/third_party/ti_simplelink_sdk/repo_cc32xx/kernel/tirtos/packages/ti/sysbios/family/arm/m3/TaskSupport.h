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

#ifndef ti_sysbios_family_arm_m3_TaskSupport__include
#define ti_sysbios_family_arm_m3_TaskSupport__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_m3_TaskSupport__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_family_arm_m3_TaskSupport___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/family/arm/m3/package/package.defs.h>

#include <ti/sysbios/interfaces/ITaskSupport.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef ti_sysbios_interfaces_ITaskSupport_FuncPtr ti_sysbios_family_arm_m3_TaskSupport_FuncPtr;


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled (ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded (ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask (ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj (ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms (ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id ti_sysbios_family_arm_m3_TaskSupport_Module__id__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__id__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__id__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__id__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__id (ti_sysbios_family_arm_m3_TaskSupport_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0 (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1 (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2 (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4 (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8 (ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_m3_TaskSupport_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__count ti_sysbios_family_arm_m3_TaskSupport_Object__count__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Object__count__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Object__count__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Object__count*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Object__count__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Object__count (ti_sysbios_family_arm_m3_TaskSupport_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_m3_TaskSupport_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__heap ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Object__heap__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Object__heap*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Object__heap (ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof (ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_m3_TaskSupport_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__table ti_sysbios_family_arm_m3_TaskSupport_Object__table__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_Object__table__CR
#define ti_sysbios_family_arm_m3_TaskSupport_Object__table__C (*((CT__ti_sysbios_family_arm_m3_TaskSupport_Object__table*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Object__table__C_offset)))
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Object__table (ti_sysbios_family_arm_m3_TaskSupport_Object__table__C)
#endif

/* defaultStackSize */
typedef xdc_SizeT CT__ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__CR
#define ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize (*((CT__ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C_offset)))
#else
#ifdef ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__D
#define ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize (ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__D)
#else
#define ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize (ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C)
#endif
#endif

/* stackAlignment */
typedef xdc_UInt CT__ti_sysbios_family_arm_m3_TaskSupport_stackAlignment;
__extern __FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_stackAlignment ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C;
#ifdef ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__CR
#define ti_sysbios_family_arm_m3_TaskSupport_stackAlignment (*((CT__ti_sysbios_family_arm_m3_TaskSupport_stackAlignment*)(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C_offset)))
#else
#ifdef ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__D
#define ti_sysbios_family_arm_m3_TaskSupport_stackAlignment (ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__D)
#else
#define ti_sysbios_family_arm_m3_TaskSupport_stackAlignment (ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C)
#endif
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_family_arm_m3_TaskSupport_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Ptr (*start)(xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block* eb);
    xdc_Void (*swap)(xdc_Ptr* oldtskContext, xdc_Ptr* newtskContext);
    xdc_Bool (*checkStack)(xdc_Char* stack, xdc_SizeT size);
    xdc_SizeT (*stackUsed)(xdc_Char* stack, xdc_SizeT size);
    xdc_UInt (*getStackAlignment)(void);
    xdc_SizeT (*getDefaultStackSize)(void);
    xdc_Ptr (*getCheckValueAddr)(xdc_Ptr curTask);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_family_arm_m3_TaskSupport_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_sysbios_family_arm_m3_TaskSupport_Fxns__ ti_sysbios_family_arm_m3_TaskSupport_Module__FXNS__C;
#else
#define ti_sysbios_family_arm_m3_TaskSupport_Module__FXNS__C (*(xdcRomConstPtr + ti_sysbios_family_arm_m3_TaskSupport_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_m3_TaskSupport_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S, "ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S( void);

/* start__E */
#define ti_sysbios_family_arm_m3_TaskSupport_start ti_sysbios_family_arm_m3_TaskSupport_start__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_start__E, "ti_sysbios_family_arm_m3_TaskSupport_start")
__extern xdc_Ptr ti_sysbios_family_arm_m3_TaskSupport_start__E( xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block *eb);

/* swap__E */
#define ti_sysbios_family_arm_m3_TaskSupport_swap ti_sysbios_family_arm_m3_TaskSupport_swap__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_swap__E, "ti_sysbios_family_arm_m3_TaskSupport_swap")
__extern xdc_Void ti_sysbios_family_arm_m3_TaskSupport_swap__E( xdc_Ptr *oldtskContext, xdc_Ptr *newtskContext);

/* checkStack__E */
#define ti_sysbios_family_arm_m3_TaskSupport_checkStack ti_sysbios_family_arm_m3_TaskSupport_checkStack__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_checkStack__E, "ti_sysbios_family_arm_m3_TaskSupport_checkStack")
__extern xdc_Bool ti_sysbios_family_arm_m3_TaskSupport_checkStack__E( xdc_Char *stack, xdc_SizeT size);

/* stackUsed__E */
#define ti_sysbios_family_arm_m3_TaskSupport_stackUsed ti_sysbios_family_arm_m3_TaskSupport_stackUsed__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_stackUsed__E, "ti_sysbios_family_arm_m3_TaskSupport_stackUsed")
__extern xdc_SizeT ti_sysbios_family_arm_m3_TaskSupport_stackUsed__E( xdc_Char *stack, xdc_SizeT size);

/* getStackAlignment__E */
#define ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment__E, "ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment")
__extern xdc_UInt ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment__E( void);

/* getDefaultStackSize__E */
#define ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize__E, "ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize")
__extern xdc_SizeT ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize__E( void);

/* getCheckValueAddr__E */
#define ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr__E
xdc__CODESECT(ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr__E, "ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr")
__extern xdc_Ptr ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr__E( xdc_Ptr curTask);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_sysbios_interfaces_ITaskSupport_Module ti_sysbios_family_arm_m3_TaskSupport_Module_upCast(void);
static inline ti_sysbios_interfaces_ITaskSupport_Module ti_sysbios_family_arm_m3_TaskSupport_Module_upCast(void)
{
    return (ti_sysbios_interfaces_ITaskSupport_Module)&ti_sysbios_family_arm_m3_TaskSupport_Module__FXNS__C;
}

/* Module_to_ti_sysbios_interfaces_ITaskSupport */
#define ti_sysbios_family_arm_m3_TaskSupport_Module_to_ti_sysbios_interfaces_ITaskSupport ti_sysbios_family_arm_m3_TaskSupport_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_m3_TaskSupport_Module_startupDone() ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_m3_TaskSupport_Object_heap() ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_m3_TaskSupport_Module_heap() ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id ti_sysbios_family_arm_m3_TaskSupport_Module_id(void);
static inline CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id ti_sysbios_family_arm_m3_TaskSupport_Module_id( void ) 
{
    return ti_sysbios_family_arm_m3_TaskSupport_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_m3_TaskSupport_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_family_arm_m3_TaskSupport_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C != (CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_m3_TaskSupport_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_family_arm_m3_TaskSupport_Module_getMask(void)
{
    return (ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C != (CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask)NULL) ? *ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_m3_TaskSupport_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_family_arm_m3_TaskSupport_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C != (CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask)NULL) {
        *ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_family_arm_m3_TaskSupport__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_m3_TaskSupport__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_m3_TaskSupport__internalaccess))

#ifndef ti_sysbios_family_arm_m3_TaskSupport__include_state
#define ti_sysbios_family_arm_m3_TaskSupport__include_state


#endif /* ti_sysbios_family_arm_m3_TaskSupport__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_m3_TaskSupport__nolocalnames)

#ifndef ti_sysbios_family_arm_m3_TaskSupport__localnames__done
#define ti_sysbios_family_arm_m3_TaskSupport__localnames__done

/* module prefix */
#define TaskSupport_FuncPtr ti_sysbios_family_arm_m3_TaskSupport_FuncPtr
#define TaskSupport_defaultStackSize ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize
#define TaskSupport_stackAlignment ti_sysbios_family_arm_m3_TaskSupport_stackAlignment
#define TaskSupport_start ti_sysbios_family_arm_m3_TaskSupport_start
#define TaskSupport_swap ti_sysbios_family_arm_m3_TaskSupport_swap
#define TaskSupport_checkStack ti_sysbios_family_arm_m3_TaskSupport_checkStack
#define TaskSupport_stackUsed ti_sysbios_family_arm_m3_TaskSupport_stackUsed
#define TaskSupport_getStackAlignment ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment
#define TaskSupport_getDefaultStackSize ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize
#define TaskSupport_getCheckValueAddr ti_sysbios_family_arm_m3_TaskSupport_getCheckValueAddr
#define TaskSupport_Module_name ti_sysbios_family_arm_m3_TaskSupport_Module_name
#define TaskSupport_Module_id ti_sysbios_family_arm_m3_TaskSupport_Module_id
#define TaskSupport_Module_startup ti_sysbios_family_arm_m3_TaskSupport_Module_startup
#define TaskSupport_Module_startupDone ti_sysbios_family_arm_m3_TaskSupport_Module_startupDone
#define TaskSupport_Module_hasMask ti_sysbios_family_arm_m3_TaskSupport_Module_hasMask
#define TaskSupport_Module_getMask ti_sysbios_family_arm_m3_TaskSupport_Module_getMask
#define TaskSupport_Module_setMask ti_sysbios_family_arm_m3_TaskSupport_Module_setMask
#define TaskSupport_Object_heap ti_sysbios_family_arm_m3_TaskSupport_Object_heap
#define TaskSupport_Module_heap ti_sysbios_family_arm_m3_TaskSupport_Module_heap
#define TaskSupport_Module_upCast ti_sysbios_family_arm_m3_TaskSupport_Module_upCast
#define TaskSupport_Module_to_ti_sysbios_interfaces_ITaskSupport ti_sysbios_family_arm_m3_TaskSupport_Module_to_ti_sysbios_interfaces_ITaskSupport

#endif /* ti_sysbios_family_arm_m3_TaskSupport__localnames__done */
#endif
