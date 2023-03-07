/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#ifndef ti_sysbios_utils_Load__INTERNAL__
#define ti_sysbios_utils_Load__INTERNAL__

#ifndef ti_sysbios_utils_Load__internalaccess
#define ti_sysbios_utils_Load__internalaccess
#endif

#include <ti/sysbios/utils/Load.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* getTaskLoad */
#undef ti_sysbios_utils_Load_getTaskLoad
#define ti_sysbios_utils_Load_getTaskLoad ti_sysbios_utils_Load_getTaskLoad__E

/* update */
#undef ti_sysbios_utils_Load_update
#define ti_sysbios_utils_Load_update ti_sysbios_utils_Load_update__E

/* updateCPULoad */
#undef ti_sysbios_utils_Load_updateCPULoad
#define ti_sysbios_utils_Load_updateCPULoad ti_sysbios_utils_Load_updateCPULoad__E

/* updateLoads */
#undef ti_sysbios_utils_Load_updateLoads
#define ti_sysbios_utils_Load_updateLoads ti_sysbios_utils_Load_updateLoads__E

/* updateContextsAndPost */
#undef ti_sysbios_utils_Load_updateContextsAndPost
#define ti_sysbios_utils_Load_updateContextsAndPost ti_sysbios_utils_Load_updateContextsAndPost__E

/* updateCurrentThreadTime */
#undef ti_sysbios_utils_Load_updateCurrentThreadTime
#define ti_sysbios_utils_Load_updateCurrentThreadTime ti_sysbios_utils_Load_updateCurrentThreadTime__E

/* updateThreadContexts */
#undef ti_sysbios_utils_Load_updateThreadContexts
#define ti_sysbios_utils_Load_updateThreadContexts ti_sysbios_utils_Load_updateThreadContexts__E

/* reset */
#undef ti_sysbios_utils_Load_reset
#define ti_sysbios_utils_Load_reset ti_sysbios_utils_Load_reset__E

/* getGlobalSwiLoad */
#undef ti_sysbios_utils_Load_getGlobalSwiLoad
#define ti_sysbios_utils_Load_getGlobalSwiLoad ti_sysbios_utils_Load_getGlobalSwiLoad__E

/* getGlobalHwiLoad */
#undef ti_sysbios_utils_Load_getGlobalHwiLoad
#define ti_sysbios_utils_Load_getGlobalHwiLoad ti_sysbios_utils_Load_getGlobalHwiLoad__E

/* getCPULoad */
#undef ti_sysbios_utils_Load_getCPULoad
#define ti_sysbios_utils_Load_getCPULoad ti_sysbios_utils_Load_getCPULoad__E

/* calculateLoad */
#undef ti_sysbios_utils_Load_calculateLoad
#define ti_sysbios_utils_Load_calculateLoad ti_sysbios_utils_Load_calculateLoad__E

/* setMinIdle */
#undef ti_sysbios_utils_Load_setMinIdle
#define ti_sysbios_utils_Load_setMinIdle ti_sysbios_utils_Load_setMinIdle__E

/* addTask */
#undef ti_sysbios_utils_Load_addTask
#define ti_sysbios_utils_Load_addTask ti_sysbios_utils_Load_addTask__E

/* removeTask */
#undef ti_sysbios_utils_Load_removeTask
#define ti_sysbios_utils_Load_removeTask ti_sysbios_utils_Load_removeTask__E

/* idleFxn */
#undef ti_sysbios_utils_Load_idleFxn
#define ti_sysbios_utils_Load_idleFxn ti_sysbios_utils_Load_idleFxn__E

/* idleFxnPwr */
#undef ti_sysbios_utils_Load_idleFxnPwr
#define ti_sysbios_utils_Load_idleFxnPwr ti_sysbios_utils_Load_idleFxnPwr__E

/* startup */
#undef ti_sysbios_utils_Load_startup
#define ti_sysbios_utils_Load_startup ti_sysbios_utils_Load_startup__E

/* taskCreateHook */
#undef ti_sysbios_utils_Load_taskCreateHook
#define ti_sysbios_utils_Load_taskCreateHook ti_sysbios_utils_Load_taskCreateHook__E

/* taskDeleteHook */
#undef ti_sysbios_utils_Load_taskDeleteHook
#define ti_sysbios_utils_Load_taskDeleteHook ti_sysbios_utils_Load_taskDeleteHook__E

/* taskSwitchHook */
#undef ti_sysbios_utils_Load_taskSwitchHook
#define ti_sysbios_utils_Load_taskSwitchHook ti_sysbios_utils_Load_taskSwitchHook__E

/* swiBeginHook */
#undef ti_sysbios_utils_Load_swiBeginHook
#define ti_sysbios_utils_Load_swiBeginHook ti_sysbios_utils_Load_swiBeginHook__E

/* swiEndHook */
#undef ti_sysbios_utils_Load_swiEndHook
#define ti_sysbios_utils_Load_swiEndHook ti_sysbios_utils_Load_swiEndHook__E

/* hwiBeginHook */
#undef ti_sysbios_utils_Load_hwiBeginHook
#define ti_sysbios_utils_Load_hwiBeginHook ti_sysbios_utils_Load_hwiBeginHook__E

/* hwiEndHook */
#undef ti_sysbios_utils_Load_hwiEndHook
#define ti_sysbios_utils_Load_hwiEndHook ti_sysbios_utils_Load_hwiEndHook__E

/* taskRegHook */
#undef ti_sysbios_utils_Load_taskRegHook
#define ti_sysbios_utils_Load_taskRegHook ti_sysbios_utils_Load_taskRegHook__E

/* logLoads */
#define Load_logLoads ti_sysbios_utils_Load_logLoads__I

/* logCPULoad */
#define Load_logCPULoad ti_sysbios_utils_Load_logCPULoad__I

/* Module_startup */
#undef ti_sysbios_utils_Load_Module_startup
#define ti_sysbios_utils_Load_Module_startup ti_sysbios_utils_Load_Module_startup__E

/* Instance_init */
#undef ti_sysbios_utils_Load_Instance_init
#define ti_sysbios_utils_Load_Instance_init ti_sysbios_utils_Load_Instance_init__E

/* Instance_finalize */
#undef ti_sysbios_utils_Load_Instance_finalize
#define ti_sysbios_utils_Load_Instance_finalize ti_sysbios_utils_Load_Instance_finalize__E

/* module */
#ifdef ti_sysbios_utils_Load_Module__state__VR
#define Load_module ((ti_sysbios_utils_Load_Module_State *)(xdcRomStatePtr + ti_sysbios_utils_Load_Module__state__V_offset))
#define module ((ti_sysbios_utils_Load_Module_State *)(xdcRomStatePtr + ti_sysbios_utils_Load_Module__state__V_offset))
#else
#define Load_module ((ti_sysbios_utils_Load_Module_State *)(xdc__MODOBJADDR__(ti_sysbios_utils_Load_Module__state__V)))
#if !defined(__cplusplus) ||!defined(ti_sysbios_utils_Load__cplusplus)
#define module ((ti_sysbios_utils_Load_Module_State *)(xdc__MODOBJADDR__(ti_sysbios_utils_Load_Module__state__V)))
#endif
#endif

/* per-module runtime symbols */
#undef Module__MID
#ifdef ti_sysbios_utils_Load_Module__id__CR
#define Module__MID (*((CT__ti_sysbios_utils_Load_Module__id *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__id__C_offset)))
#else
#define Module__MID ti_sysbios_utils_Load_Module__id__C
#endif

#undef Module__DGSINCL
#ifdef ti_sysbios_utils_Load_Module__diagsIncluded__CR
#define Module__DGSINCL (*((CT__ti_sysbios_utils_Load_Module__diagsIncluded *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsIncluded__C_offset)))
#else
#define Module__DGSINCL ti_sysbios_utils_Load_Module__diagsIncluded__C
#endif

#undef Module__DGSENAB
#ifdef ti_sysbios_utils_Load_Module__diagsEnabled__CR
#define Module__DGSENAB (*((CT__ti_sysbios_utils_Load_Module__diagsEnabled *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsEnabled__C_offset)))
#else
#define Module__DGSENAB ti_sysbios_utils_Load_Module__diagsEnabled__C
#endif

#undef Module__DGSMASK
#ifdef ti_sysbios_utils_Load_Module__diagsMask__CR
#define Module__DGSMASK (*((CT__ti_sysbios_utils_Load_Module__diagsMask *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__diagsMask__C_offset)))
#else
#define Module__DGSMASK ti_sysbios_utils_Load_Module__diagsMask__C
#endif

#undef Module__LOGDEF
#ifdef ti_sysbios_utils_Load_Module__loggerDefined__CR
#define Module__LOGDEF (*((CT__ti_sysbios_utils_Load_Module__loggerDefined *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerDefined__C_offset)))
#else
#define Module__LOGDEF ti_sysbios_utils_Load_Module__loggerDefined__C
#endif

#undef Module__LOGOBJ
#ifdef ti_sysbios_utils_Load_Module__loggerObj__CR
#define Module__LOGOBJ ti_sysbios_utils_Load_Module__loggerObj__R
#define Module__LOGOBJ (*((CT__ti_sysbios_utils_Load_Module__loggerObj *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerObj__C_offset)))
#else
#define Module__LOGOBJ ti_sysbios_utils_Load_Module__loggerObj__C
#endif

#undef Module__LOGFXN0
#ifdef ti_sysbios_utils_Load_Module__loggerFxn0__CR
#define Module__LOGFXN0 (*((CT__ti_sysbios_utils_Load_Module__loggerFxn0 *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn0__C_offset)))
#else
#define Module__LOGFXN0 ti_sysbios_utils_Load_Module__loggerFxn0__C
#endif

#undef Module__LOGFXN1
#ifdef ti_sysbios_utils_Load_Module__loggerFxn1__CR
#define Module__LOGFXN1 (*((CT__ti_sysbios_utils_Load_Module__loggerFxn1 *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn1__C_offset)))
#else
#define Module__LOGFXN1 ti_sysbios_utils_Load_Module__loggerFxn1__C
#endif

#undef Module__LOGFXN2
#ifdef ti_sysbios_utils_Load_Module__loggerFxn2__CR
#define Module__LOGFXN2 (*((CT__ti_sysbios_utils_Load_Module__loggerFxn2 *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn2__C_offset)))
#else
#define Module__LOGFXN2 ti_sysbios_utils_Load_Module__loggerFxn2__C
#endif

#undef Module__LOGFXN4
#ifdef ti_sysbios_utils_Load_Module__loggerFxn4__CR
#define Module__LOGFXN4 (*((CT__ti_sysbios_utils_Load_Module__loggerFxn4 *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn4__C_offset)))
#else
#define Module__LOGFXN4 ti_sysbios_utils_Load_Module__loggerFxn4__C
#endif

#undef Module__LOGFXN8
#ifdef ti_sysbios_utils_Load_Module__loggerFxn8__CR
#define Module__LOGFXN8 (*((CT__ti_sysbios_utils_Load_Module__loggerFxn8 *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__loggerFxn8__C_offset)))
#else
#define Module__LOGFXN8 ti_sysbios_utils_Load_Module__loggerFxn8__C
#endif

#undef Module__G_OBJ
#ifdef ti_sysbios_utils_Load_Module__gateObj__CR
#define Module__G_OBJ (*((CT__ti_sysbios_utils_Load_Module__gateObj *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__gateObj__C_offset)))
#else
#define Module__G_OBJ ti_sysbios_utils_Load_Module__gateObj__C
#endif

#undef Module__G_PRMS
#ifdef ti_sysbios_utils_Load_Module__gatePrms__CR
#define Module__G_PRMS (*((CT__ti_sysbios_utils_Load_Module__gatePrms *)(xdcRomConstPtr + ti_sysbios_utils_Load_Module__gatePrms__C_offset)))
#else
#define Module__G_PRMS ti_sysbios_utils_Load_Module__gatePrms__C
#endif

#undef Module__GP_create
#define Module__GP_create ti_sysbios_utils_Load_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_utils_Load_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_utils_Load_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_utils_Load_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_utils_Load_Module_GateProxy_query


#endif /* ti_sysbios_utils_Load__INTERNAL____ */
