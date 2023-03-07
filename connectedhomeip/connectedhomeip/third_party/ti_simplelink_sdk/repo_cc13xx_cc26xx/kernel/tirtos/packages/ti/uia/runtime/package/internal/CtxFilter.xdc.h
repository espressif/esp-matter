/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#ifndef ti_uia_runtime_CtxFilter__INTERNAL__
#define ti_uia_runtime_CtxFilter__INTERNAL__

#ifndef ti_uia_runtime_CtxFilter__internalaccess
#define ti_uia_runtime_CtxFilter__internalaccess
#endif

#include <ti/uia/runtime/CtxFilter.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* isCtxEnabled */
#undef ti_uia_runtime_CtxFilter_isCtxEnabled
#define ti_uia_runtime_CtxFilter_isCtxEnabled ti_uia_runtime_CtxFilter_isCtxEnabled__E

/* setCtxEnabled */
#undef ti_uia_runtime_CtxFilter_setCtxEnabled
#define ti_uia_runtime_CtxFilter_setCtxEnabled ti_uia_runtime_CtxFilter_setCtxEnabled__E

/* setContextFilterFlags */
#undef ti_uia_runtime_CtxFilter_setContextFilterFlags
#define ti_uia_runtime_CtxFilter_setContextFilterFlags ti_uia_runtime_CtxFilter_setContextFilterFlags__E

/* isLoggingEnabledForAppCtx */
#undef ti_uia_runtime_CtxFilter_isLoggingEnabledForAppCtx
#define ti_uia_runtime_CtxFilter_isLoggingEnabledForAppCtx ti_uia_runtime_CtxFilter_isLoggingEnabledForAppCtx__E

/* isLoggingEnabledForChanCtx */
#undef ti_uia_runtime_CtxFilter_isLoggingEnabledForChanCtx
#define ti_uia_runtime_CtxFilter_isLoggingEnabledForChanCtx ti_uia_runtime_CtxFilter_isLoggingEnabledForChanCtx__E

/* isLoggingEnabledForFrameCtx */
#undef ti_uia_runtime_CtxFilter_isLoggingEnabledForFrameCtx
#define ti_uia_runtime_CtxFilter_isLoggingEnabledForFrameCtx ti_uia_runtime_CtxFilter_isLoggingEnabledForFrameCtx__E

/* isLoggingEnabledForThreadCtx */
#undef ti_uia_runtime_CtxFilter_isLoggingEnabledForThreadCtx
#define ti_uia_runtime_CtxFilter_isLoggingEnabledForThreadCtx ti_uia_runtime_CtxFilter_isLoggingEnabledForThreadCtx__E

/* isLoggingEnabledForUserCtx */
#undef ti_uia_runtime_CtxFilter_isLoggingEnabledForUserCtx
#define ti_uia_runtime_CtxFilter_isLoggingEnabledForUserCtx ti_uia_runtime_CtxFilter_isLoggingEnabledForUserCtx__E

/* Module_startup */
#undef ti_uia_runtime_CtxFilter_Module_startup
#define ti_uia_runtime_CtxFilter_Module_startup ti_uia_runtime_CtxFilter_Module_startup__E

/* Instance_init */
#undef ti_uia_runtime_CtxFilter_Instance_init
#define ti_uia_runtime_CtxFilter_Instance_init ti_uia_runtime_CtxFilter_Instance_init__E

/* Instance_finalize */
#undef ti_uia_runtime_CtxFilter_Instance_finalize
#define ti_uia_runtime_CtxFilter_Instance_finalize ti_uia_runtime_CtxFilter_Instance_finalize__E

/* module */
#ifdef ti_uia_runtime_CtxFilter_Module__state__VR
#define CtxFilter_module ((ti_uia_runtime_CtxFilter_Module_State *)(xdcRomStatePtr + ti_uia_runtime_CtxFilter_Module__state__V_offset))
#define module ((ti_uia_runtime_CtxFilter_Module_State *)(xdcRomStatePtr + ti_uia_runtime_CtxFilter_Module__state__V_offset))
#else
#define CtxFilter_module ((ti_uia_runtime_CtxFilter_Module_State *)(xdc__MODOBJADDR__(ti_uia_runtime_CtxFilter_Module__state__V)))
#if !defined(__cplusplus) ||!defined(ti_uia_runtime_CtxFilter__cplusplus)
#define module ((ti_uia_runtime_CtxFilter_Module_State *)(xdc__MODOBJADDR__(ti_uia_runtime_CtxFilter_Module__state__V)))
#endif
#endif

/* per-module runtime symbols */
#undef Module__MID
#ifdef ti_uia_runtime_CtxFilter_Module__id__CR
#define Module__MID (*((CT__ti_uia_runtime_CtxFilter_Module__id *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__id__C_offset)))
#else
#define Module__MID ti_uia_runtime_CtxFilter_Module__id__C
#endif

#undef Module__DGSINCL
#ifdef ti_uia_runtime_CtxFilter_Module__diagsIncluded__CR
#define Module__DGSINCL (*((CT__ti_uia_runtime_CtxFilter_Module__diagsIncluded *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__diagsIncluded__C_offset)))
#else
#define Module__DGSINCL ti_uia_runtime_CtxFilter_Module__diagsIncluded__C
#endif

#undef Module__DGSENAB
#ifdef ti_uia_runtime_CtxFilter_Module__diagsEnabled__CR
#define Module__DGSENAB (*((CT__ti_uia_runtime_CtxFilter_Module__diagsEnabled *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__diagsEnabled__C_offset)))
#else
#define Module__DGSENAB ti_uia_runtime_CtxFilter_Module__diagsEnabled__C
#endif

#undef Module__DGSMASK
#ifdef ti_uia_runtime_CtxFilter_Module__diagsMask__CR
#define Module__DGSMASK (*((CT__ti_uia_runtime_CtxFilter_Module__diagsMask *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__diagsMask__C_offset)))
#else
#define Module__DGSMASK ti_uia_runtime_CtxFilter_Module__diagsMask__C
#endif

#undef Module__LOGDEF
#ifdef ti_uia_runtime_CtxFilter_Module__loggerDefined__CR
#define Module__LOGDEF (*((CT__ti_uia_runtime_CtxFilter_Module__loggerDefined *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerDefined__C_offset)))
#else
#define Module__LOGDEF ti_uia_runtime_CtxFilter_Module__loggerDefined__C
#endif

#undef Module__LOGOBJ
#ifdef ti_uia_runtime_CtxFilter_Module__loggerObj__CR
#define Module__LOGOBJ ti_uia_runtime_CtxFilter_Module__loggerObj__R
#define Module__LOGOBJ (*((CT__ti_uia_runtime_CtxFilter_Module__loggerObj *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerObj__C_offset)))
#else
#define Module__LOGOBJ ti_uia_runtime_CtxFilter_Module__loggerObj__C
#endif

#undef Module__LOGFXN0
#ifdef ti_uia_runtime_CtxFilter_Module__loggerFxn0__CR
#define Module__LOGFXN0 (*((CT__ti_uia_runtime_CtxFilter_Module__loggerFxn0 *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerFxn0__C_offset)))
#else
#define Module__LOGFXN0 ti_uia_runtime_CtxFilter_Module__loggerFxn0__C
#endif

#undef Module__LOGFXN1
#ifdef ti_uia_runtime_CtxFilter_Module__loggerFxn1__CR
#define Module__LOGFXN1 (*((CT__ti_uia_runtime_CtxFilter_Module__loggerFxn1 *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerFxn1__C_offset)))
#else
#define Module__LOGFXN1 ti_uia_runtime_CtxFilter_Module__loggerFxn1__C
#endif

#undef Module__LOGFXN2
#ifdef ti_uia_runtime_CtxFilter_Module__loggerFxn2__CR
#define Module__LOGFXN2 (*((CT__ti_uia_runtime_CtxFilter_Module__loggerFxn2 *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerFxn2__C_offset)))
#else
#define Module__LOGFXN2 ti_uia_runtime_CtxFilter_Module__loggerFxn2__C
#endif

#undef Module__LOGFXN4
#ifdef ti_uia_runtime_CtxFilter_Module__loggerFxn4__CR
#define Module__LOGFXN4 (*((CT__ti_uia_runtime_CtxFilter_Module__loggerFxn4 *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerFxn4__C_offset)))
#else
#define Module__LOGFXN4 ti_uia_runtime_CtxFilter_Module__loggerFxn4__C
#endif

#undef Module__LOGFXN8
#ifdef ti_uia_runtime_CtxFilter_Module__loggerFxn8__CR
#define Module__LOGFXN8 (*((CT__ti_uia_runtime_CtxFilter_Module__loggerFxn8 *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__loggerFxn8__C_offset)))
#else
#define Module__LOGFXN8 ti_uia_runtime_CtxFilter_Module__loggerFxn8__C
#endif

#undef Module__G_OBJ
#ifdef ti_uia_runtime_CtxFilter_Module__gateObj__CR
#define Module__G_OBJ (*((CT__ti_uia_runtime_CtxFilter_Module__gateObj *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__gateObj__C_offset)))
#else
#define Module__G_OBJ ti_uia_runtime_CtxFilter_Module__gateObj__C
#endif

#undef Module__G_PRMS
#ifdef ti_uia_runtime_CtxFilter_Module__gatePrms__CR
#define Module__G_PRMS (*((CT__ti_uia_runtime_CtxFilter_Module__gatePrms *)(xdcRomConstPtr + ti_uia_runtime_CtxFilter_Module__gatePrms__C_offset)))
#else
#define Module__G_PRMS ti_uia_runtime_CtxFilter_Module__gatePrms__C
#endif

#undef Module__GP_create
#define Module__GP_create ti_uia_runtime_CtxFilter_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_uia_runtime_CtxFilter_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_uia_runtime_CtxFilter_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_uia_runtime_CtxFilter_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_uia_runtime_CtxFilter_Module_GateProxy_query


#endif /* ti_uia_runtime_CtxFilter__INTERNAL____ */
