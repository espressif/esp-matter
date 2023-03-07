/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#ifndef ti_uia_loggers_LoggerMin__INTERNAL__
#define ti_uia_loggers_LoggerMin__INTERNAL__

#ifndef ti_uia_loggers_LoggerMin__internalaccess
#define ti_uia_loggers_LoggerMin__internalaccess
#endif

#include <ti/uia/loggers/LoggerMin.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* flush */
#undef ti_uia_loggers_LoggerMin_flush
#define ti_uia_loggers_LoggerMin_flush ti_uia_loggers_LoggerMin_flush__E

/* write */
#undef ti_uia_loggers_LoggerMin_write
#define ti_uia_loggers_LoggerMin_write ti_uia_loggers_LoggerMin_write__E

/* write0 */
#undef ti_uia_loggers_LoggerMin_write0
#define ti_uia_loggers_LoggerMin_write0 ti_uia_loggers_LoggerMin_write0__E

/* write1 */
#undef ti_uia_loggers_LoggerMin_write1
#define ti_uia_loggers_LoggerMin_write1 ti_uia_loggers_LoggerMin_write1__E

/* write2 */
#undef ti_uia_loggers_LoggerMin_write2
#define ti_uia_loggers_LoggerMin_write2 ti_uia_loggers_LoggerMin_write2__E

/* write4 */
#undef ti_uia_loggers_LoggerMin_write4
#define ti_uia_loggers_LoggerMin_write4 ti_uia_loggers_LoggerMin_write4__E

/* write8 */
#undef ti_uia_loggers_LoggerMin_write8
#define ti_uia_loggers_LoggerMin_write8 ti_uia_loggers_LoggerMin_write8__E

/* enable */
#undef ti_uia_loggers_LoggerMin_enable
#define ti_uia_loggers_LoggerMin_enable ti_uia_loggers_LoggerMin_enable__E

/* disable */
#undef ti_uia_loggers_LoggerMin_disable
#define ti_uia_loggers_LoggerMin_disable ti_uia_loggers_LoggerMin_disable__E

/* getContents */
#define LoggerMin_getContents ti_uia_loggers_LoggerMin_getContents__I

/* isEmpty */
#define LoggerMin_isEmpty ti_uia_loggers_LoggerMin_isEmpty__I

/* genTimestamp */
#define LoggerMin_genTimestamp ti_uia_loggers_LoggerMin_genTimestamp__I

/* Module_startup */
#undef ti_uia_loggers_LoggerMin_Module_startup
#define ti_uia_loggers_LoggerMin_Module_startup ti_uia_loggers_LoggerMin_Module_startup__E

/* Instance_init */
#undef ti_uia_loggers_LoggerMin_Instance_init
#define ti_uia_loggers_LoggerMin_Instance_init ti_uia_loggers_LoggerMin_Instance_init__E

/* Instance_finalize */
#undef ti_uia_loggers_LoggerMin_Instance_finalize
#define ti_uia_loggers_LoggerMin_Instance_finalize ti_uia_loggers_LoggerMin_Instance_finalize__E

/* module */
#ifdef ti_uia_loggers_LoggerMin_Module__state__VR
#define LoggerMin_module ((ti_uia_loggers_LoggerMin_Module_State *)(xdcRomStatePtr + ti_uia_loggers_LoggerMin_Module__state__V_offset))
#define module ((ti_uia_loggers_LoggerMin_Module_State *)(xdcRomStatePtr + ti_uia_loggers_LoggerMin_Module__state__V_offset))
#else
#define LoggerMin_module ((ti_uia_loggers_LoggerMin_Module_State *)(xdc__MODOBJADDR__(ti_uia_loggers_LoggerMin_Module__state__V)))
#if !defined(__cplusplus) ||!defined(ti_uia_loggers_LoggerMin__cplusplus)
#define module ((ti_uia_loggers_LoggerMin_Module_State *)(xdc__MODOBJADDR__(ti_uia_loggers_LoggerMin_Module__state__V)))
#endif
#endif

/* per-module runtime symbols */
#undef Module__MID
#ifdef ti_uia_loggers_LoggerMin_Module__id__CR
#define Module__MID (*((CT__ti_uia_loggers_LoggerMin_Module__id *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__id__C_offset)))
#else
#define Module__MID ti_uia_loggers_LoggerMin_Module__id__C
#endif

#undef Module__DGSINCL
#ifdef ti_uia_loggers_LoggerMin_Module__diagsIncluded__CR
#define Module__DGSINCL (*((CT__ti_uia_loggers_LoggerMin_Module__diagsIncluded *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsIncluded__C_offset)))
#else
#define Module__DGSINCL ti_uia_loggers_LoggerMin_Module__diagsIncluded__C
#endif

#undef Module__DGSENAB
#ifdef ti_uia_loggers_LoggerMin_Module__diagsEnabled__CR
#define Module__DGSENAB (*((CT__ti_uia_loggers_LoggerMin_Module__diagsEnabled *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsEnabled__C_offset)))
#else
#define Module__DGSENAB ti_uia_loggers_LoggerMin_Module__diagsEnabled__C
#endif

#undef Module__DGSMASK
#ifdef ti_uia_loggers_LoggerMin_Module__diagsMask__CR
#define Module__DGSMASK (*((CT__ti_uia_loggers_LoggerMin_Module__diagsMask *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__diagsMask__C_offset)))
#else
#define Module__DGSMASK ti_uia_loggers_LoggerMin_Module__diagsMask__C
#endif

#undef Module__LOGDEF
#ifdef ti_uia_loggers_LoggerMin_Module__loggerDefined__CR
#define Module__LOGDEF (*((CT__ti_uia_loggers_LoggerMin_Module__loggerDefined *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerDefined__C_offset)))
#else
#define Module__LOGDEF ti_uia_loggers_LoggerMin_Module__loggerDefined__C
#endif

#undef Module__LOGOBJ
#ifdef ti_uia_loggers_LoggerMin_Module__loggerObj__CR
#define Module__LOGOBJ ti_uia_loggers_LoggerMin_Module__loggerObj__R
#define Module__LOGOBJ (*((CT__ti_uia_loggers_LoggerMin_Module__loggerObj *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerObj__C_offset)))
#else
#define Module__LOGOBJ ti_uia_loggers_LoggerMin_Module__loggerObj__C
#endif

#undef Module__LOGFXN0
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn0__CR
#define Module__LOGFXN0 (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn0 *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn0__C_offset)))
#else
#define Module__LOGFXN0 ti_uia_loggers_LoggerMin_Module__loggerFxn0__C
#endif

#undef Module__LOGFXN1
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn1__CR
#define Module__LOGFXN1 (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn1 *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn1__C_offset)))
#else
#define Module__LOGFXN1 ti_uia_loggers_LoggerMin_Module__loggerFxn1__C
#endif

#undef Module__LOGFXN2
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn2__CR
#define Module__LOGFXN2 (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn2 *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn2__C_offset)))
#else
#define Module__LOGFXN2 ti_uia_loggers_LoggerMin_Module__loggerFxn2__C
#endif

#undef Module__LOGFXN4
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn4__CR
#define Module__LOGFXN4 (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn4 *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn4__C_offset)))
#else
#define Module__LOGFXN4 ti_uia_loggers_LoggerMin_Module__loggerFxn4__C
#endif

#undef Module__LOGFXN8
#ifdef ti_uia_loggers_LoggerMin_Module__loggerFxn8__CR
#define Module__LOGFXN8 (*((CT__ti_uia_loggers_LoggerMin_Module__loggerFxn8 *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__loggerFxn8__C_offset)))
#else
#define Module__LOGFXN8 ti_uia_loggers_LoggerMin_Module__loggerFxn8__C
#endif

#undef Module__G_OBJ
#ifdef ti_uia_loggers_LoggerMin_Module__gateObj__CR
#define Module__G_OBJ (*((CT__ti_uia_loggers_LoggerMin_Module__gateObj *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__gateObj__C_offset)))
#else
#define Module__G_OBJ ti_uia_loggers_LoggerMin_Module__gateObj__C
#endif

#undef Module__G_PRMS
#ifdef ti_uia_loggers_LoggerMin_Module__gatePrms__CR
#define Module__G_PRMS (*((CT__ti_uia_loggers_LoggerMin_Module__gatePrms *)(xdcRomConstPtr + ti_uia_loggers_LoggerMin_Module__gatePrms__C_offset)))
#else
#define Module__G_PRMS ti_uia_loggers_LoggerMin_Module__gatePrms__C
#endif

#undef Module__GP_create
#define Module__GP_create ti_uia_loggers_LoggerMin_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_uia_loggers_LoggerMin_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_uia_loggers_LoggerMin_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_uia_loggers_LoggerMin_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_uia_loggers_LoggerMin_Module_GateProxy_query

/* Object__sizingError */
#line 1 "Error_inconsistent_object_size_in_ti.uia.loggers.LoggerMin"
typedef char ti_uia_loggers_LoggerMin_Object__sizingError[(sizeof(ti_uia_loggers_LoggerMin_Object) > sizeof(ti_uia_loggers_LoggerMin_Struct)) ? -1 : 1];


#endif /* ti_uia_loggers_LoggerMin__INTERNAL____ */
