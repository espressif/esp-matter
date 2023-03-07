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

#ifndef ti_uia_runtime_ServiceMgr_SupportProxy__include
#define ti_uia_runtime_ServiceMgr_SupportProxy__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_ServiceMgr_SupportProxy__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_ServiceMgr_SupportProxy___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <ti/uia/runtime/IServiceMgrSupport.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled (ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded (ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask (ti_uia_runtime_ServiceMgr_SupportProxy_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj (ti_uia_runtime_ServiceMgr_SupportProxy_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms (ti_uia_runtime_ServiceMgr_SupportProxy_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__id;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__id ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__id*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__id (ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0 ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0 (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1 ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1 (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2 ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2 (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4 ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4 (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8 ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8 (ti_uia_runtime_ServiceMgr_SupportProxy_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__count;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__count ti_uia_runtime_ServiceMgr_SupportProxy_Object__count__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Object__count__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__count__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__count*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Object__count__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__count (ti_uia_runtime_ServiceMgr_SupportProxy_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap (ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof (ti_uia_runtime_ServiceMgr_SupportProxy_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__table;
__extern __FAR__ const CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__table ti_uia_runtime_ServiceMgr_SupportProxy_Object__table__C;
#ifdef ti_uia_runtime_ServiceMgr_SupportProxy_Object__table__CR
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__table__C (*((CT__ti_uia_runtime_ServiceMgr_SupportProxy_Object__table*)(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Object__table__C_offset)))
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object__table (ti_uia_runtime_ServiceMgr_SupportProxy_Object__table__C)
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_ServiceMgr_SupportProxy_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*freePacket)(ti_uia_runtime_UIAPacket_Hdr* packet);
    ti_uia_runtime_UIAPacket_Hdr *(*getFreePacket)(ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout);
    xdc_Void (*requestEnergy)(xdc_Int id);
    xdc_Bool (*sendPacket)(ti_uia_runtime_UIAPacket_Hdr* packet);
    xdc_Void (*setPeriod)(xdc_Int id, xdc_UInt32 periodInMs);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_ServiceMgr_SupportProxy_Module__FXNS__CR

/* Module__FXNS__C */
__extern const ti_uia_runtime_ServiceMgr_SupportProxy_Fxns__ ti_uia_runtime_ServiceMgr_SupportProxy_Module__FXNS__C;
#else
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module__FXNS__C (*(xdcRomConstPtr + ti_uia_runtime_ServiceMgr_SupportProxy_Module__FXNS__C_offset))
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module_startup( state ) (-1)

/* Handle__label__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Handle__label__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_uia_runtime_ServiceMgr_SupportProxy_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab);

/* Module__startupDone__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Module__startupDone__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Module__startupDone__S")
__extern xdc_Bool ti_uia_runtime_ServiceMgr_SupportProxy_Module__startupDone__S( void);

/* Object__get__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Object__get__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Object__get__S")
__extern xdc_Ptr ti_uia_runtime_ServiceMgr_SupportProxy_Object__get__S( xdc_Ptr oarr, xdc_Int i);

/* Object__first__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Object__first__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Object__first__S")
__extern xdc_Ptr ti_uia_runtime_ServiceMgr_SupportProxy_Object__first__S( void);

/* Object__next__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Object__next__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Object__next__S")
__extern xdc_Ptr ti_uia_runtime_ServiceMgr_SupportProxy_Object__next__S( xdc_Ptr obj);

/* Params__init__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Params__init__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Params__init__S")
__extern xdc_Void ti_uia_runtime_ServiceMgr_SupportProxy_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz);

/* Proxy__abstract__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__abstract__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__abstract__S")
__extern xdc_Bool ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__abstract__S( void);

/* Proxy__delegate__S */
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__delegate__S, "ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__delegate__S")
__extern xdc_CPtr ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__delegate__S( void);

/* freePacket__E */
#define ti_uia_runtime_ServiceMgr_SupportProxy_freePacket ti_uia_runtime_ServiceMgr_SupportProxy_freePacket__E
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_freePacket__E, "ti_uia_runtime_ServiceMgr_SupportProxy_freePacket")
__extern xdc_Void ti_uia_runtime_ServiceMgr_SupportProxy_freePacket__E( ti_uia_runtime_UIAPacket_Hdr *packet);

/* getFreePacket__E */
#define ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket__E
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket__E, "ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket")
__extern ti_uia_runtime_UIAPacket_Hdr *ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket__E( ti_uia_runtime_UIAPacket_HdrType type, xdc_UInt timeout);

/* requestEnergy__E */
#define ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy__E
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy__E, "ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy")
__extern xdc_Void ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy__E( xdc_Int id);

/* sendPacket__E */
#define ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket__E
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket__E, "ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket")
__extern xdc_Bool ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket__E( ti_uia_runtime_UIAPacket_Hdr *packet);

/* setPeriod__E */
#define ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod__E
xdc__CODESECT(ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod__E, "ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod")
__extern xdc_Void ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod__E( xdc_Int id, xdc_UInt32 periodInMs);


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_runtime_IServiceMgrSupport_Module ti_uia_runtime_ServiceMgr_SupportProxy_Module_upCast(void);
static inline ti_uia_runtime_IServiceMgrSupport_Module ti_uia_runtime_ServiceMgr_SupportProxy_Module_upCast(void)
{
    return (ti_uia_runtime_IServiceMgrSupport_Module)ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__delegate__S();
}

/* Module_to_ti_uia_runtime_IServiceMgrSupport */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module_to_ti_uia_runtime_IServiceMgrSupport ti_uia_runtime_ServiceMgr_SupportProxy_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module_startupDone() ti_uia_runtime_ServiceMgr_SupportProxy_Module__startupDone__S()

/* Object_heap */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Object_heap() ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C

/* Module_heap */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Module_heap() ti_uia_runtime_ServiceMgr_SupportProxy_Object__heap__C

/* Module_id */
static inline CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__id ti_uia_runtime_ServiceMgr_SupportProxy_Module_id(void);
static inline CT__ti_uia_runtime_ServiceMgr_SupportProxy_Module__id ti_uia_runtime_ServiceMgr_SupportProxy_Module_id( void ) 
{
    return ti_uia_runtime_ServiceMgr_SupportProxy_Module__id__C;
}

/* Proxy_abstract */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Proxy_abstract() ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__abstract__S()

/* Proxy_delegate */
#define ti_uia_runtime_ServiceMgr_SupportProxy_Proxy_delegate() ((ti_uia_runtime_IServiceMgrSupport_Module)ti_uia_runtime_ServiceMgr_SupportProxy_Proxy__delegate__S())


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_ServiceMgr_SupportProxy__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_ServiceMgr_SupportProxy__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_ServiceMgr_SupportProxy__nolocalnames)

#ifndef ti_uia_runtime_ServiceMgr_SupportProxy__localnames__done
#define ti_uia_runtime_ServiceMgr_SupportProxy__localnames__done

/* module prefix */
#define ServiceMgr_SupportProxy_freePacket ti_uia_runtime_ServiceMgr_SupportProxy_freePacket
#define ServiceMgr_SupportProxy_getFreePacket ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket
#define ServiceMgr_SupportProxy_requestEnergy ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy
#define ServiceMgr_SupportProxy_sendPacket ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket
#define ServiceMgr_SupportProxy_setPeriod ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod
#define ServiceMgr_SupportProxy_Module_name ti_uia_runtime_ServiceMgr_SupportProxy_Module_name
#define ServiceMgr_SupportProxy_Module_id ti_uia_runtime_ServiceMgr_SupportProxy_Module_id
#define ServiceMgr_SupportProxy_Module_startup ti_uia_runtime_ServiceMgr_SupportProxy_Module_startup
#define ServiceMgr_SupportProxy_Module_startupDone ti_uia_runtime_ServiceMgr_SupportProxy_Module_startupDone
#define ServiceMgr_SupportProxy_Module_hasMask ti_uia_runtime_ServiceMgr_SupportProxy_Module_hasMask
#define ServiceMgr_SupportProxy_Module_getMask ti_uia_runtime_ServiceMgr_SupportProxy_Module_getMask
#define ServiceMgr_SupportProxy_Module_setMask ti_uia_runtime_ServiceMgr_SupportProxy_Module_setMask
#define ServiceMgr_SupportProxy_Object_heap ti_uia_runtime_ServiceMgr_SupportProxy_Object_heap
#define ServiceMgr_SupportProxy_Module_heap ti_uia_runtime_ServiceMgr_SupportProxy_Module_heap
#define ServiceMgr_SupportProxy_Proxy_abstract ti_uia_runtime_ServiceMgr_SupportProxy_Proxy_abstract
#define ServiceMgr_SupportProxy_Proxy_delegate ti_uia_runtime_ServiceMgr_SupportProxy_Proxy_delegate
#define ServiceMgr_SupportProxy_Module_upCast ti_uia_runtime_ServiceMgr_SupportProxy_Module_upCast
#define ServiceMgr_SupportProxy_Module_to_ti_uia_runtime_IServiceMgrSupport ti_uia_runtime_ServiceMgr_SupportProxy_Module_to_ti_uia_runtime_IServiceMgrSupport

#endif /* ti_uia_runtime_ServiceMgr_SupportProxy__localnames__done */
#endif
