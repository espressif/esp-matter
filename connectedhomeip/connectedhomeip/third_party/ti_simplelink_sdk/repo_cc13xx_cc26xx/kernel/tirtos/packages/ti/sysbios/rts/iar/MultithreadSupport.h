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

#ifndef ti_sysbios_rts_iar_MultithreadSupport__include
#define ti_sysbios_rts_iar_MultithreadSupport__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_rts_iar_MultithreadSupport__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_rts_iar_MultithreadSupport___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/rts/iar/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/IModule.h>


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
typedef xdc_Bits32 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled (ti_sysbios_rts_iar_MultithreadSupport_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded (ti_sysbios_rts_iar_MultithreadSupport_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask (ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj (ti_sysbios_rts_iar_MultithreadSupport_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms (ti_sysbios_rts_iar_MultithreadSupport_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_rts_iar_MultithreadSupport_Module__id;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__id ti_sysbios_rts_iar_MultithreadSupport_Module__id__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__id__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__id__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__id*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__id__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__id (ti_sysbios_rts_iar_MultithreadSupport_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0 ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0 (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1 ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1 (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2 ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2 (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4 ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4 (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8 ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8 (ti_sysbios_rts_iar_MultithreadSupport_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_rts_iar_MultithreadSupport_Object__count;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Object__count ti_sysbios_rts_iar_MultithreadSupport_Object__count__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Object__count__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Object__count__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Object__count*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Object__count__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Object__count (ti_sysbios_rts_iar_MultithreadSupport_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_rts_iar_MultithreadSupport_Object__heap;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Object__heap ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Object__heap__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Object__heap*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Object__heap (ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof (ti_sysbios_rts_iar_MultithreadSupport_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_rts_iar_MultithreadSupport_Object__table;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_Object__table ti_sysbios_rts_iar_MultithreadSupport_Object__table__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_Object__table__CR
#define ti_sysbios_rts_iar_MultithreadSupport_Object__table__C (*((CT__ti_sysbios_rts_iar_MultithreadSupport_Object__table*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_Object__table__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Object__table (ti_sysbios_rts_iar_MultithreadSupport_Object__table__C)
#endif

/* enableMultithreadSupport */
typedef xdc_Bool CT__ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__CR
#define ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport (*((CT__ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__C_offset)))
#else
#ifdef ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__D
#define ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport (ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__D)
#else
#define ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport (ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport__C)
#endif
#endif

/* A_badThreadType */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType__CR
#define ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType (*((CT__ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType (ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType__C)
#endif

/* A_badLockRelease */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease;
__extern __FAR__ const CT__ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease__C;
#ifdef ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease__CR
#define ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease (*((CT__ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease*)(xdcRomConstPtr + ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease__C_offset)))
#else
#define ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease (ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease__C)
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_rts_iar_MultithreadSupport_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_Module__startupDone__S, "ti_sysbios_rts_iar_MultithreadSupport_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_rts_iar_MultithreadSupport_Module__startupDone__S( void);

/* perThreadAccess__I */
#define ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess__I, "ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess")
__extern xdc_Void *ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess__I( xdc_Void *symbp);

/* getTlsPtr__I */
#define ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr__I, "ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr")
__extern xdc_Void *ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr__I( void);

/* getTlsAddr__I */
#define ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr__I, "ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr")
__extern xdc_Void *ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr__I( void);

/* initLock__I */
#define ti_sysbios_rts_iar_MultithreadSupport_initLock ti_sysbios_rts_iar_MultithreadSupport_initLock__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_initLock__I, "ti_sysbios_rts_iar_MultithreadSupport_initLock")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_initLock__I( xdc_Void **ptr);

/* destroyLock__I */
#define ti_sysbios_rts_iar_MultithreadSupport_destroyLock ti_sysbios_rts_iar_MultithreadSupport_destroyLock__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_destroyLock__I, "ti_sysbios_rts_iar_MultithreadSupport_destroyLock")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_destroyLock__I( xdc_Void **ptr);

/* acquireLock__I */
#define ti_sysbios_rts_iar_MultithreadSupport_acquireLock ti_sysbios_rts_iar_MultithreadSupport_acquireLock__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_acquireLock__I, "ti_sysbios_rts_iar_MultithreadSupport_acquireLock")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_acquireLock__I( xdc_Void **ptr);

/* releaseLock__I */
#define ti_sysbios_rts_iar_MultithreadSupport_releaseLock ti_sysbios_rts_iar_MultithreadSupport_releaseLock__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_releaseLock__I, "ti_sysbios_rts_iar_MultithreadSupport_releaseLock")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_releaseLock__I( xdc_Void **ptr);

/* taskCreateHook__I */
#define ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook__I, "ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook__I( ti_sysbios_knl_Task_Handle task, xdc_runtime_Error_Block *eb);

/* taskDeleteHook__I */
#define ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook__I, "ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook__I( ti_sysbios_knl_Task_Handle task);

/* taskRegHook__I */
#define ti_sysbios_rts_iar_MultithreadSupport_taskRegHook ti_sysbios_rts_iar_MultithreadSupport_taskRegHook__I
xdc__CODESECT(ti_sysbios_rts_iar_MultithreadSupport_taskRegHook__I, "ti_sysbios_rts_iar_MultithreadSupport_taskRegHook")
__extern xdc_Void ti_sysbios_rts_iar_MultithreadSupport_taskRegHook__I( xdc_Int id);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_rts_iar_MultithreadSupport_Module_startupDone() ti_sysbios_rts_iar_MultithreadSupport_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_rts_iar_MultithreadSupport_Object_heap() ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C

/* Module_heap */
#define ti_sysbios_rts_iar_MultithreadSupport_Module_heap() ti_sysbios_rts_iar_MultithreadSupport_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_rts_iar_MultithreadSupport_Module__id ti_sysbios_rts_iar_MultithreadSupport_Module_id(void);
static inline CT__ti_sysbios_rts_iar_MultithreadSupport_Module__id ti_sysbios_rts_iar_MultithreadSupport_Module_id( void ) 
{
    return ti_sysbios_rts_iar_MultithreadSupport_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_rts_iar_MultithreadSupport_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_rts_iar_MultithreadSupport_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C != (CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_rts_iar_MultithreadSupport_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_rts_iar_MultithreadSupport_Module_getMask(void)
{
    return (ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C != (CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask)NULL) ? *ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_rts_iar_MultithreadSupport_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_rts_iar_MultithreadSupport_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C != (CT__ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask)NULL) {
        *ti_sysbios_rts_iar_MultithreadSupport_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_rts_iar_MultithreadSupport__top__
#undef __nested__
#endif

#endif /* ti_sysbios_rts_iar_MultithreadSupport__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_rts_iar_MultithreadSupport__internalaccess))

#ifndef ti_sysbios_rts_iar_MultithreadSupport__include_state
#define ti_sysbios_rts_iar_MultithreadSupport__include_state

/* Module_State */
struct ti_sysbios_rts_iar_MultithreadSupport_Module_State {
    xdc_Int taskHId;
    xdc_Ptr deletedTaskTLSPtr;
    ti_sysbios_knl_Task_Handle curTaskHandle;
    ti_sysbios_knl_Semaphore_Handle lock;
};

/* Module__state__V */
#ifndef ti_sysbios_rts_iar_MultithreadSupport_Module__state__VR
extern struct ti_sysbios_rts_iar_MultithreadSupport_Module_State__ ti_sysbios_rts_iar_MultithreadSupport_Module__state__V;
#else
#define ti_sysbios_rts_iar_MultithreadSupport_Module__state__V (*((struct ti_sysbios_rts_iar_MultithreadSupport_Module_State__*)(xdcRomStatePtr + ti_sysbios_rts_iar_MultithreadSupport_Module__state__V_offset)))
#endif

#endif /* ti_sysbios_rts_iar_MultithreadSupport__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_rts_iar_MultithreadSupport__nolocalnames)

#ifndef ti_sysbios_rts_iar_MultithreadSupport__localnames__done
#define ti_sysbios_rts_iar_MultithreadSupport__localnames__done

/* module prefix */
#define MultithreadSupport_Module_State ti_sysbios_rts_iar_MultithreadSupport_Module_State
#define MultithreadSupport_enableMultithreadSupport ti_sysbios_rts_iar_MultithreadSupport_enableMultithreadSupport
#define MultithreadSupport_A_badThreadType ti_sysbios_rts_iar_MultithreadSupport_A_badThreadType
#define MultithreadSupport_A_badLockRelease ti_sysbios_rts_iar_MultithreadSupport_A_badLockRelease
#define MultithreadSupport_Module_name ti_sysbios_rts_iar_MultithreadSupport_Module_name
#define MultithreadSupport_Module_id ti_sysbios_rts_iar_MultithreadSupport_Module_id
#define MultithreadSupport_Module_startup ti_sysbios_rts_iar_MultithreadSupport_Module_startup
#define MultithreadSupport_Module_startupDone ti_sysbios_rts_iar_MultithreadSupport_Module_startupDone
#define MultithreadSupport_Module_hasMask ti_sysbios_rts_iar_MultithreadSupport_Module_hasMask
#define MultithreadSupport_Module_getMask ti_sysbios_rts_iar_MultithreadSupport_Module_getMask
#define MultithreadSupport_Module_setMask ti_sysbios_rts_iar_MultithreadSupport_Module_setMask
#define MultithreadSupport_Object_heap ti_sysbios_rts_iar_MultithreadSupport_Object_heap
#define MultithreadSupport_Module_heap ti_sysbios_rts_iar_MultithreadSupport_Module_heap

#endif /* ti_sysbios_rts_iar_MultithreadSupport__localnames__done */
#endif
