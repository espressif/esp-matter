/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#ifndef ti_uia_sysbios_LoggerStreamer2__INTERNAL__
#define ti_uia_sysbios_LoggerStreamer2__INTERNAL__

#ifndef ti_uia_sysbios_LoggerStreamer2__internalaccess
#define ti_uia_sysbios_LoggerStreamer2__internalaccess
#endif

#include <ti/uia/sysbios/LoggerStreamer2.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* enable */
#undef ti_uia_sysbios_LoggerStreamer2_enable
#define ti_uia_sysbios_LoggerStreamer2_enable ti_uia_sysbios_LoggerStreamer2_enable__E

/* disable */
#undef ti_uia_sysbios_LoggerStreamer2_disable
#define ti_uia_sysbios_LoggerStreamer2_disable ti_uia_sysbios_LoggerStreamer2_disable__E

/* getTransferType */
#undef ti_uia_sysbios_LoggerStreamer2_getTransferType
#define ti_uia_sysbios_LoggerStreamer2_getTransferType ti_uia_sysbios_LoggerStreamer2_getTransferType__E

/* getContents */
#undef ti_uia_sysbios_LoggerStreamer2_getContents
#define ti_uia_sysbios_LoggerStreamer2_getContents ti_uia_sysbios_LoggerStreamer2_getContents__E

/* isEmpty */
#undef ti_uia_sysbios_LoggerStreamer2_isEmpty
#define ti_uia_sysbios_LoggerStreamer2_isEmpty ti_uia_sysbios_LoggerStreamer2_isEmpty__E

/* getMaxLength */
#undef ti_uia_sysbios_LoggerStreamer2_getMaxLength
#define ti_uia_sysbios_LoggerStreamer2_getMaxLength ti_uia_sysbios_LoggerStreamer2_getMaxLength__E

/* getInstanceId */
#undef ti_uia_sysbios_LoggerStreamer2_getInstanceId
#define ti_uia_sysbios_LoggerStreamer2_getInstanceId ti_uia_sysbios_LoggerStreamer2_getInstanceId__E

/* getPriority */
#undef ti_uia_sysbios_LoggerStreamer2_getPriority
#define ti_uia_sysbios_LoggerStreamer2_getPriority ti_uia_sysbios_LoggerStreamer2_getPriority__E

/* setPriority */
#undef ti_uia_sysbios_LoggerStreamer2_setPriority
#define ti_uia_sysbios_LoggerStreamer2_setPriority ti_uia_sysbios_LoggerStreamer2_setPriority__E

/* reset */
#undef ti_uia_sysbios_LoggerStreamer2_reset
#define ti_uia_sysbios_LoggerStreamer2_reset ti_uia_sysbios_LoggerStreamer2_reset__E

/* writeMemoryRange */
#undef ti_uia_sysbios_LoggerStreamer2_writeMemoryRange
#define ti_uia_sysbios_LoggerStreamer2_writeMemoryRange ti_uia_sysbios_LoggerStreamer2_writeMemoryRange__E

/* setModuleIdToRouteToStatusLogger */
#undef ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger
#define ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E

/* initBuffer */
#undef ti_uia_sysbios_LoggerStreamer2_initBuffer
#define ti_uia_sysbios_LoggerStreamer2_initBuffer ti_uia_sysbios_LoggerStreamer2_initBuffer__E

/* flush */
#undef ti_uia_sysbios_LoggerStreamer2_flush
#define ti_uia_sysbios_LoggerStreamer2_flush ti_uia_sysbios_LoggerStreamer2_flush__E

/* prime */
#undef ti_uia_sysbios_LoggerStreamer2_prime
#define ti_uia_sysbios_LoggerStreamer2_prime ti_uia_sysbios_LoggerStreamer2_prime__E

/* validatePacket */
#undef ti_uia_sysbios_LoggerStreamer2_validatePacket
#define ti_uia_sysbios_LoggerStreamer2_validatePacket ti_uia_sysbios_LoggerStreamer2_validatePacket__E

/* write0 */
#undef ti_uia_sysbios_LoggerStreamer2_write0
#define ti_uia_sysbios_LoggerStreamer2_write0 ti_uia_sysbios_LoggerStreamer2_write0__E

/* write1 */
#undef ti_uia_sysbios_LoggerStreamer2_write1
#define ti_uia_sysbios_LoggerStreamer2_write1 ti_uia_sysbios_LoggerStreamer2_write1__E

/* write2 */
#undef ti_uia_sysbios_LoggerStreamer2_write2
#define ti_uia_sysbios_LoggerStreamer2_write2 ti_uia_sysbios_LoggerStreamer2_write2__E

/* write4 */
#undef ti_uia_sysbios_LoggerStreamer2_write4
#define ti_uia_sysbios_LoggerStreamer2_write4 ti_uia_sysbios_LoggerStreamer2_write4__E

/* write8 */
#undef ti_uia_sysbios_LoggerStreamer2_write8
#define ti_uia_sysbios_LoggerStreamer2_write8 ti_uia_sysbios_LoggerStreamer2_write8__E

/* setFilterLevel */
#undef ti_uia_sysbios_LoggerStreamer2_setFilterLevel
#define ti_uia_sysbios_LoggerStreamer2_setFilterLevel ti_uia_sysbios_LoggerStreamer2_setFilterLevel__E

/* getFilterLevel */
#undef ti_uia_sysbios_LoggerStreamer2_getFilterLevel
#define ti_uia_sysbios_LoggerStreamer2_getFilterLevel ti_uia_sysbios_LoggerStreamer2_getFilterLevel__E

/* getBufSize */
#undef ti_uia_sysbios_LoggerStreamer2_getBufSize
#define ti_uia_sysbios_LoggerStreamer2_getBufSize ti_uia_sysbios_LoggerStreamer2_getBufSize__E

/* getContext */
#undef ti_uia_sysbios_LoggerStreamer2_getContext
#define ti_uia_sysbios_LoggerStreamer2_getContext ti_uia_sysbios_LoggerStreamer2_getContext__E

/* setContext */
#undef ti_uia_sysbios_LoggerStreamer2_setContext
#define ti_uia_sysbios_LoggerStreamer2_setContext ti_uia_sysbios_LoggerStreamer2_setContext__E

/* filterOutEvent */
#define LoggerStreamer2_filterOutEvent ti_uia_sysbios_LoggerStreamer2_filterOutEvent__I

/* Module_startup */
#undef ti_uia_sysbios_LoggerStreamer2_Module_startup
#define ti_uia_sysbios_LoggerStreamer2_Module_startup ti_uia_sysbios_LoggerStreamer2_Module_startup__E

/* Instance_init */
#undef ti_uia_sysbios_LoggerStreamer2_Instance_init
#define ti_uia_sysbios_LoggerStreamer2_Instance_init ti_uia_sysbios_LoggerStreamer2_Instance_init__E

/* Instance_finalize */
#undef ti_uia_sysbios_LoggerStreamer2_Instance_finalize
#define ti_uia_sysbios_LoggerStreamer2_Instance_finalize ti_uia_sysbios_LoggerStreamer2_Instance_finalize__E

/* module */
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__state__VR
#define LoggerStreamer2_module ((ti_uia_sysbios_LoggerStreamer2_Module_State *)(xdcRomStatePtr + ti_uia_sysbios_LoggerStreamer2_Module__state__V_offset))
#define module ((ti_uia_sysbios_LoggerStreamer2_Module_State *)(xdcRomStatePtr + ti_uia_sysbios_LoggerStreamer2_Module__state__V_offset))
#else
#define LoggerStreamer2_module ((ti_uia_sysbios_LoggerStreamer2_Module_State *)(xdc__MODOBJADDR__(ti_uia_sysbios_LoggerStreamer2_Module__state__V)))
#if !defined(__cplusplus) ||!defined(ti_uia_sysbios_LoggerStreamer2__cplusplus)
#define module ((ti_uia_sysbios_LoggerStreamer2_Module_State *)(xdc__MODOBJADDR__(ti_uia_sysbios_LoggerStreamer2_Module__state__V)))
#endif
#endif

/* per-module runtime symbols */
#undef Module__MID
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__id__CR
#define Module__MID (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__id *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__id__C_offset)))
#else
#define Module__MID ti_uia_sysbios_LoggerStreamer2_Module__id__C
#endif

#undef Module__DGSINCL
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__CR
#define Module__DGSINCL (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C_offset)))
#else
#define Module__DGSINCL ti_uia_sysbios_LoggerStreamer2_Module__diagsIncluded__C
#endif

#undef Module__DGSENAB
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__CR
#define Module__DGSENAB (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C_offset)))
#else
#define Module__DGSENAB ti_uia_sysbios_LoggerStreamer2_Module__diagsEnabled__C
#endif

#undef Module__DGSMASK
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__CR
#define Module__DGSMASK (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__diagsMask *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C_offset)))
#else
#define Module__DGSMASK ti_uia_sysbios_LoggerStreamer2_Module__diagsMask__C
#endif

#undef Module__LOGDEF
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__CR
#define Module__LOGDEF (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C_offset)))
#else
#define Module__LOGDEF ti_uia_sysbios_LoggerStreamer2_Module__loggerDefined__C
#endif

#undef Module__LOGOBJ
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__CR
#define Module__LOGOBJ ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__R
#define Module__LOGOBJ (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerObj *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C_offset)))
#else
#define Module__LOGOBJ ti_uia_sysbios_LoggerStreamer2_Module__loggerObj__C
#endif

#undef Module__LOGFXN0
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__CR
#define Module__LOGFXN0 (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0 *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C_offset)))
#else
#define Module__LOGFXN0 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn0__C
#endif

#undef Module__LOGFXN1
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__CR
#define Module__LOGFXN1 (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1 *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C_offset)))
#else
#define Module__LOGFXN1 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn1__C
#endif

#undef Module__LOGFXN2
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__CR
#define Module__LOGFXN2 (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2 *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C_offset)))
#else
#define Module__LOGFXN2 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn2__C
#endif

#undef Module__LOGFXN4
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__CR
#define Module__LOGFXN4 (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4 *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C_offset)))
#else
#define Module__LOGFXN4 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn4__C
#endif

#undef Module__LOGFXN8
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__CR
#define Module__LOGFXN8 (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8 *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C_offset)))
#else
#define Module__LOGFXN8 ti_uia_sysbios_LoggerStreamer2_Module__loggerFxn8__C
#endif

#undef Module__G_OBJ
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__gateObj__CR
#define Module__G_OBJ (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__gateObj *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C_offset)))
#else
#define Module__G_OBJ ti_uia_sysbios_LoggerStreamer2_Module__gateObj__C
#endif

#undef Module__G_PRMS
#ifdef ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__CR
#define Module__G_PRMS (*((CT__ti_uia_sysbios_LoggerStreamer2_Module__gatePrms *)(xdcRomConstPtr + ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C_offset)))
#else
#define Module__G_PRMS ti_uia_sysbios_LoggerStreamer2_Module__gatePrms__C
#endif

#undef Module__GP_create
#define Module__GP_create ti_uia_sysbios_LoggerStreamer2_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_uia_sysbios_LoggerStreamer2_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_uia_sysbios_LoggerStreamer2_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_uia_sysbios_LoggerStreamer2_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_uia_sysbios_LoggerStreamer2_Module_GateProxy_query

/* Object__sizingError */
#line 1 "Error_inconsistent_object_size_in_ti.uia.sysbios.LoggerStreamer2"
typedef char ti_uia_sysbios_LoggerStreamer2_Object__sizingError[(sizeof(ti_uia_sysbios_LoggerStreamer2_Object) > sizeof(ti_uia_sysbios_LoggerStreamer2_Struct)) ? -1 : 1];


#endif /* ti_uia_sysbios_LoggerStreamer2__INTERNAL____ */
