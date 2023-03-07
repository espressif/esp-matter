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
 *     CREATE ARGS
 *     PER-INSTANCE TYPES
 *     VIRTUAL FUNCTIONS
 *     FUNCTION STUBS
 *     FUNCTION SELECTORS
 *
 *     EPILOGUE
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_interfaces_IHwi__include
#define ti_sysbios_interfaces_IHwi__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_interfaces_IHwi__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_interfaces_IHwi___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/interfaces/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_FuncPtr)(xdc_UArg arg1);

/* Irp */
typedef xdc_UArg ti_sysbios_interfaces_IHwi_Irp;

/* HookSet */
struct ti_sysbios_interfaces_IHwi_HookSet {
    xdc_Void (*registerFxn)(xdc_Int arg1);
    xdc_Void (*createFxn)(ti_sysbios_interfaces_IHwi_Handle arg1, xdc_runtime_Error_Block* arg2);
    xdc_Void (*beginFxn)(ti_sysbios_interfaces_IHwi_Handle arg1);
    xdc_Void (*endFxn)(ti_sysbios_interfaces_IHwi_Handle arg1);
    xdc_Void (*deleteFxn)(ti_sysbios_interfaces_IHwi_Handle arg1);
};

/* MaskingOption */
enum ti_sysbios_interfaces_IHwi_MaskingOption {
    ti_sysbios_interfaces_IHwi_MaskingOption_NONE,
    ti_sysbios_interfaces_IHwi_MaskingOption_ALL,
    ti_sysbios_interfaces_IHwi_MaskingOption_SELF,
    ti_sysbios_interfaces_IHwi_MaskingOption_BITMASK,
    ti_sysbios_interfaces_IHwi_MaskingOption_LOWER
};
typedef enum ti_sysbios_interfaces_IHwi_MaskingOption ti_sysbios_interfaces_IHwi_MaskingOption;

/* StackInfo */
struct ti_sysbios_interfaces_IHwi_StackInfo {
    xdc_SizeT hwiStackPeak;
    xdc_SizeT hwiStackSize;
    xdc_Ptr hwiStackBase;
};


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_interfaces_IHwi_Args__create {
    xdc_Int intNum;
    ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn;
} ti_sysbios_interfaces_IHwi_Args__create;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_interfaces_IHwi_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    ti_sysbios_interfaces_IHwi_MaskingOption maskSetting;
    xdc_UArg arg;
    xdc_Bool enableInt;
    xdc_Int eventId;
    xdc_Int priority;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_interfaces_IHwi_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*getStackInfo)(ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth);
    xdc_Bool (*getCoreStackInfo)(ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth, xdc_UInt coreId);
    xdc_Void (*startup)(void);
    xdc_UInt (*disable)(void);
    xdc_UInt (*enable)(void);
    xdc_Void (*restore)(xdc_UInt key);
    xdc_Void (*switchFromBootStack)(void);
    xdc_Void (*post)(xdc_UInt intNum);
    xdc_Char *(*getTaskSP)(void);
    xdc_UInt (*disableInterrupt)(xdc_UInt intNum);
    xdc_UInt (*enableInterrupt)(xdc_UInt intNum);
    xdc_Void (*restoreInterrupt)(xdc_UInt intNum, xdc_UInt key);
    xdc_Void (*clearInterrupt)(xdc_UInt intNum);
    ti_sysbios_interfaces_IHwi_FuncPtr (*getFunc)(void* inst, xdc_UArg* arg);
    xdc_Void (*setFunc)(void* inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg);
    xdc_Ptr (*getHookContext)(void* inst, xdc_Int id);
    xdc_Void (*setHookContext)(void* inst, xdc_Int id, xdc_Ptr hookContext);
    ti_sysbios_interfaces_IHwi_Irp (*getIrp)(void* inst);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_sysbios_interfaces_IHwi_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_sysbios_interfaces_IHwi_Interface__BASE__C;
#else
#define ti_sysbios_interfaces_IHwi_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_sysbios_interfaces_IHwi_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* create */
xdc__CODESECT(ti_sysbios_interfaces_IHwi_create, "ti_sysbios_interfaces_IHwi_create")
__extern ti_sysbios_interfaces_IHwi_Handle ti_sysbios_interfaces_IHwi_create(ti_sysbios_interfaces_IHwi_Module mod, xdc_Int intNum, ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn, const ti_sysbios_interfaces_IHwi_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_sysbios_interfaces_IHwi_delete, "ti_sysbios_interfaces_IHwi_delete")
__extern xdc_Void ti_sysbios_interfaces_IHwi_delete(ti_sysbios_interfaces_IHwi_Handle *inst);

/* Handle_to_Module */
static inline ti_sysbios_interfaces_IHwi_Module ti_sysbios_interfaces_IHwi_Handle_to_Module(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_Module ti_sysbios_interfaces_IHwi_Handle_to_Module(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return inst->__fxns;
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_interfaces_IHwi_Handle_label(ti_sysbios_interfaces_IHwi_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_sysbios_interfaces_IHwi_Handle_label(ti_sysbios_interfaces_IHwi_Handle inst, xdc_runtime_Types_Label *lab)
{
    return inst->__fxns->__sysp->__label(inst, lab);
}

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_IHwi_Module_id(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_runtime_Types_ModuleId ti_sysbios_interfaces_IHwi_Module_id(ti_sysbios_interfaces_IHwi_Module mod)
{
    return mod->__sysp->__mid;
}

/* getStackInfo */
static inline xdc_Bool ti_sysbios_interfaces_IHwi_getStackInfo(ti_sysbios_interfaces_IHwi_Module mod, ti_sysbios_interfaces_IHwi_StackInfo *stkInfo, xdc_Bool computeStackDepth);
static inline xdc_Bool ti_sysbios_interfaces_IHwi_getStackInfo( ti_sysbios_interfaces_IHwi_Module mod, ti_sysbios_interfaces_IHwi_StackInfo *stkInfo, xdc_Bool computeStackDepth )
{
    return mod->getStackInfo(stkInfo, computeStackDepth);
}

/* getCoreStackInfo */
static inline xdc_Bool ti_sysbios_interfaces_IHwi_getCoreStackInfo(ti_sysbios_interfaces_IHwi_Module mod, ti_sysbios_interfaces_IHwi_StackInfo *stkInfo, xdc_Bool computeStackDepth, xdc_UInt coreId);
static inline xdc_Bool ti_sysbios_interfaces_IHwi_getCoreStackInfo( ti_sysbios_interfaces_IHwi_Module mod, ti_sysbios_interfaces_IHwi_StackInfo *stkInfo, xdc_Bool computeStackDepth, xdc_UInt coreId )
{
    return mod->getCoreStackInfo(stkInfo, computeStackDepth, coreId);
}

/* startup */
static inline xdc_Void ti_sysbios_interfaces_IHwi_startup(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_Void ti_sysbios_interfaces_IHwi_startup( ti_sysbios_interfaces_IHwi_Module mod )
{
    mod->startup();
}

/* disable */
static inline xdc_UInt ti_sysbios_interfaces_IHwi_disable(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_IHwi_disable( ti_sysbios_interfaces_IHwi_Module mod )
{
    return mod->disable();
}

/* enable */
static inline xdc_UInt ti_sysbios_interfaces_IHwi_enable(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_UInt ti_sysbios_interfaces_IHwi_enable( ti_sysbios_interfaces_IHwi_Module mod )
{
    return mod->enable();
}

/* restore */
static inline xdc_Void ti_sysbios_interfaces_IHwi_restore(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt key);
static inline xdc_Void ti_sysbios_interfaces_IHwi_restore( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt key )
{
    mod->restore(key);
}

/* switchFromBootStack */
static inline xdc_Void ti_sysbios_interfaces_IHwi_switchFromBootStack(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_Void ti_sysbios_interfaces_IHwi_switchFromBootStack( ti_sysbios_interfaces_IHwi_Module mod )
{
    mod->switchFromBootStack();
}

/* post */
static inline xdc_Void ti_sysbios_interfaces_IHwi_post(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum);
static inline xdc_Void ti_sysbios_interfaces_IHwi_post( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum )
{
    mod->post(intNum);
}

/* getTaskSP */
static inline xdc_Char *ti_sysbios_interfaces_IHwi_getTaskSP(ti_sysbios_interfaces_IHwi_Module mod);
static inline xdc_Char *ti_sysbios_interfaces_IHwi_getTaskSP( ti_sysbios_interfaces_IHwi_Module mod )
{
    return mod->getTaskSP();
}

/* disableInterrupt */
static inline xdc_UInt ti_sysbios_interfaces_IHwi_disableInterrupt(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum);
static inline xdc_UInt ti_sysbios_interfaces_IHwi_disableInterrupt( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum )
{
    return mod->disableInterrupt(intNum);
}

/* enableInterrupt */
static inline xdc_UInt ti_sysbios_interfaces_IHwi_enableInterrupt(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum);
static inline xdc_UInt ti_sysbios_interfaces_IHwi_enableInterrupt( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum )
{
    return mod->enableInterrupt(intNum);
}

/* restoreInterrupt */
static inline xdc_Void ti_sysbios_interfaces_IHwi_restoreInterrupt(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum, xdc_UInt key);
static inline xdc_Void ti_sysbios_interfaces_IHwi_restoreInterrupt( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum, xdc_UInt key )
{
    mod->restoreInterrupt(intNum, key);
}

/* clearInterrupt */
static inline xdc_Void ti_sysbios_interfaces_IHwi_clearInterrupt(ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum);
static inline xdc_Void ti_sysbios_interfaces_IHwi_clearInterrupt( ti_sysbios_interfaces_IHwi_Module mod, xdc_UInt intNum )
{
    mod->clearInterrupt(intNum);
}

/* getFunc */
static inline ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_interfaces_IHwi_getFunc(ti_sysbios_interfaces_IHwi_Handle inst, xdc_UArg *arg);
static inline ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_interfaces_IHwi_getFunc( ti_sysbios_interfaces_IHwi_Handle inst, xdc_UArg *arg )
{
    return inst->__fxns->getFunc((void*)inst, arg);
}

/* setFunc */
static inline xdc_Void ti_sysbios_interfaces_IHwi_setFunc(ti_sysbios_interfaces_IHwi_Handle inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg);
static inline xdc_Void ti_sysbios_interfaces_IHwi_setFunc( ti_sysbios_interfaces_IHwi_Handle inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg )
{
    inst->__fxns->setFunc((void*)inst, fxn, arg);
}

/* getHookContext */
static inline xdc_Ptr ti_sysbios_interfaces_IHwi_getHookContext(ti_sysbios_interfaces_IHwi_Handle inst, xdc_Int id);
static inline xdc_Ptr ti_sysbios_interfaces_IHwi_getHookContext( ti_sysbios_interfaces_IHwi_Handle inst, xdc_Int id )
{
    return inst->__fxns->getHookContext((void*)inst, id);
}

/* setHookContext */
static inline xdc_Void ti_sysbios_interfaces_IHwi_setHookContext(ti_sysbios_interfaces_IHwi_Handle inst, xdc_Int id, xdc_Ptr hookContext);
static inline xdc_Void ti_sysbios_interfaces_IHwi_setHookContext( ti_sysbios_interfaces_IHwi_Handle inst, xdc_Int id, xdc_Ptr hookContext )
{
    inst->__fxns->setHookContext((void*)inst, id, hookContext);
}

/* getIrp */
static inline ti_sysbios_interfaces_IHwi_Irp ti_sysbios_interfaces_IHwi_getIrp(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_Irp ti_sysbios_interfaces_IHwi_getIrp( ti_sysbios_interfaces_IHwi_Handle inst )
{
    return inst->__fxns->getIrp((void*)inst);
}


/*
 * ======== FUNCTION SELECTORS ========
 */

/* These functions return function pointers for module and instance functions.
 * The functions accept modules and instances declared as types defined in this
 * interface, but they return functions defined for the actual objects passed
 * as parameters. These functions are not invoked by any generated code or
 * XDCtools internal code.
 */

/* getStackInfo_{FxnT,fxnP} */
typedef xdc_Bool (*ti_sysbios_interfaces_IHwi_getStackInfo_FxnT)(ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth);
static inline ti_sysbios_interfaces_IHwi_getStackInfo_FxnT ti_sysbios_interfaces_IHwi_getStackInfo_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_getStackInfo_FxnT ti_sysbios_interfaces_IHwi_getStackInfo_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_getStackInfo_FxnT)mod->getStackInfo;
}

/* getCoreStackInfo_{FxnT,fxnP} */
typedef xdc_Bool (*ti_sysbios_interfaces_IHwi_getCoreStackInfo_FxnT)(ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth, xdc_UInt coreId);
static inline ti_sysbios_interfaces_IHwi_getCoreStackInfo_FxnT ti_sysbios_interfaces_IHwi_getCoreStackInfo_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_getCoreStackInfo_FxnT ti_sysbios_interfaces_IHwi_getCoreStackInfo_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_getCoreStackInfo_FxnT)mod->getCoreStackInfo;
}

/* startup_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_startup_FxnT)(void);
static inline ti_sysbios_interfaces_IHwi_startup_FxnT ti_sysbios_interfaces_IHwi_startup_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_startup_FxnT ti_sysbios_interfaces_IHwi_startup_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_startup_FxnT)mod->startup;
}

/* disable_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_IHwi_disable_FxnT)(void);
static inline ti_sysbios_interfaces_IHwi_disable_FxnT ti_sysbios_interfaces_IHwi_disable_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_disable_FxnT ti_sysbios_interfaces_IHwi_disable_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_disable_FxnT)mod->disable;
}

/* enable_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_IHwi_enable_FxnT)(void);
static inline ti_sysbios_interfaces_IHwi_enable_FxnT ti_sysbios_interfaces_IHwi_enable_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_enable_FxnT ti_sysbios_interfaces_IHwi_enable_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_enable_FxnT)mod->enable;
}

/* restore_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_restore_FxnT)(xdc_UInt key);
static inline ti_sysbios_interfaces_IHwi_restore_FxnT ti_sysbios_interfaces_IHwi_restore_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_restore_FxnT ti_sysbios_interfaces_IHwi_restore_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_restore_FxnT)mod->restore;
}

/* switchFromBootStack_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_switchFromBootStack_FxnT)(void);
static inline ti_sysbios_interfaces_IHwi_switchFromBootStack_FxnT ti_sysbios_interfaces_IHwi_switchFromBootStack_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_switchFromBootStack_FxnT ti_sysbios_interfaces_IHwi_switchFromBootStack_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_switchFromBootStack_FxnT)mod->switchFromBootStack;
}

/* post_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_post_FxnT)(xdc_UInt intNum);
static inline ti_sysbios_interfaces_IHwi_post_FxnT ti_sysbios_interfaces_IHwi_post_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_post_FxnT ti_sysbios_interfaces_IHwi_post_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_post_FxnT)mod->post;
}

/* getTaskSP_{FxnT,fxnP} */
typedef xdc_Char *(*ti_sysbios_interfaces_IHwi_getTaskSP_FxnT)(void);
static inline ti_sysbios_interfaces_IHwi_getTaskSP_FxnT ti_sysbios_interfaces_IHwi_getTaskSP_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_getTaskSP_FxnT ti_sysbios_interfaces_IHwi_getTaskSP_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_getTaskSP_FxnT)mod->getTaskSP;
}

/* disableInterrupt_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_IHwi_disableInterrupt_FxnT)(xdc_UInt intNum);
static inline ti_sysbios_interfaces_IHwi_disableInterrupt_FxnT ti_sysbios_interfaces_IHwi_disableInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_disableInterrupt_FxnT ti_sysbios_interfaces_IHwi_disableInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_disableInterrupt_FxnT)mod->disableInterrupt;
}

/* enableInterrupt_{FxnT,fxnP} */
typedef xdc_UInt (*ti_sysbios_interfaces_IHwi_enableInterrupt_FxnT)(xdc_UInt intNum);
static inline ti_sysbios_interfaces_IHwi_enableInterrupt_FxnT ti_sysbios_interfaces_IHwi_enableInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_enableInterrupt_FxnT ti_sysbios_interfaces_IHwi_enableInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_enableInterrupt_FxnT)mod->enableInterrupt;
}

/* restoreInterrupt_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_restoreInterrupt_FxnT)(xdc_UInt intNum, xdc_UInt key);
static inline ti_sysbios_interfaces_IHwi_restoreInterrupt_FxnT ti_sysbios_interfaces_IHwi_restoreInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_restoreInterrupt_FxnT ti_sysbios_interfaces_IHwi_restoreInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_restoreInterrupt_FxnT)mod->restoreInterrupt;
}

/* clearInterrupt_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_clearInterrupt_FxnT)(xdc_UInt intNum);
static inline ti_sysbios_interfaces_IHwi_clearInterrupt_FxnT ti_sysbios_interfaces_IHwi_clearInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod);
static inline ti_sysbios_interfaces_IHwi_clearInterrupt_FxnT ti_sysbios_interfaces_IHwi_clearInterrupt_fxnP(ti_sysbios_interfaces_IHwi_Module mod)
{
    return (ti_sysbios_interfaces_IHwi_clearInterrupt_FxnT)mod->clearInterrupt;
}

/* getFunc_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_IHwi_FuncPtr (*ti_sysbios_interfaces_IHwi_getFunc_FxnT)(xdc_Void *inst, xdc_UArg* arg);
static inline ti_sysbios_interfaces_IHwi_getFunc_FxnT ti_sysbios_interfaces_IHwi_getFunc_fxnP(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_getFunc_FxnT ti_sysbios_interfaces_IHwi_getFunc_fxnP(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return (ti_sysbios_interfaces_IHwi_getFunc_FxnT)inst->__fxns->getFunc;
}

/* setFunc_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_setFunc_FxnT)(xdc_Void *inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg);
static inline ti_sysbios_interfaces_IHwi_setFunc_FxnT ti_sysbios_interfaces_IHwi_setFunc_fxnP(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_setFunc_FxnT ti_sysbios_interfaces_IHwi_setFunc_fxnP(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return (ti_sysbios_interfaces_IHwi_setFunc_FxnT)inst->__fxns->setFunc;
}

/* getHookContext_{FxnT,fxnP} */
typedef xdc_Ptr (*ti_sysbios_interfaces_IHwi_getHookContext_FxnT)(xdc_Void *inst, xdc_Int id);
static inline ti_sysbios_interfaces_IHwi_getHookContext_FxnT ti_sysbios_interfaces_IHwi_getHookContext_fxnP(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_getHookContext_FxnT ti_sysbios_interfaces_IHwi_getHookContext_fxnP(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return (ti_sysbios_interfaces_IHwi_getHookContext_FxnT)inst->__fxns->getHookContext;
}

/* setHookContext_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_interfaces_IHwi_setHookContext_FxnT)(xdc_Void *inst, xdc_Int id, xdc_Ptr hookContext);
static inline ti_sysbios_interfaces_IHwi_setHookContext_FxnT ti_sysbios_interfaces_IHwi_setHookContext_fxnP(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_setHookContext_FxnT ti_sysbios_interfaces_IHwi_setHookContext_fxnP(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return (ti_sysbios_interfaces_IHwi_setHookContext_FxnT)inst->__fxns->setHookContext;
}

/* getIrp_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_IHwi_Irp (*ti_sysbios_interfaces_IHwi_getIrp_FxnT)(xdc_Void *inst);
static inline ti_sysbios_interfaces_IHwi_getIrp_FxnT ti_sysbios_interfaces_IHwi_getIrp_fxnP(ti_sysbios_interfaces_IHwi_Handle inst);
static inline ti_sysbios_interfaces_IHwi_getIrp_FxnT ti_sysbios_interfaces_IHwi_getIrp_fxnP(ti_sysbios_interfaces_IHwi_Handle inst)
{
    return (ti_sysbios_interfaces_IHwi_getIrp_FxnT)inst->__fxns->getIrp;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_interfaces_IHwi__top__
#undef __nested__
#endif

#endif /* ti_sysbios_interfaces_IHwi__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_interfaces_IHwi__nolocalnames)

#ifndef ti_sysbios_interfaces_IHwi__localnames__done
#define ti_sysbios_interfaces_IHwi__localnames__done

/* module prefix */
#define IHwi_Instance ti_sysbios_interfaces_IHwi_Instance
#define IHwi_Handle ti_sysbios_interfaces_IHwi_Handle
#define IHwi_Module ti_sysbios_interfaces_IHwi_Module
#define IHwi_FuncPtr ti_sysbios_interfaces_IHwi_FuncPtr
#define IHwi_Irp ti_sysbios_interfaces_IHwi_Irp
#define IHwi_HookSet ti_sysbios_interfaces_IHwi_HookSet
#define IHwi_MaskingOption ti_sysbios_interfaces_IHwi_MaskingOption
#define IHwi_StackInfo ti_sysbios_interfaces_IHwi_StackInfo
#define IHwi_MaskingOption_NONE ti_sysbios_interfaces_IHwi_MaskingOption_NONE
#define IHwi_MaskingOption_ALL ti_sysbios_interfaces_IHwi_MaskingOption_ALL
#define IHwi_MaskingOption_SELF ti_sysbios_interfaces_IHwi_MaskingOption_SELF
#define IHwi_MaskingOption_BITMASK ti_sysbios_interfaces_IHwi_MaskingOption_BITMASK
#define IHwi_MaskingOption_LOWER ti_sysbios_interfaces_IHwi_MaskingOption_LOWER
#define IHwi_Params ti_sysbios_interfaces_IHwi_Params
#define IHwi_getStackInfo ti_sysbios_interfaces_IHwi_getStackInfo
#define IHwi_getStackInfo_fxnP ti_sysbios_interfaces_IHwi_getStackInfo_fxnP
#define IHwi_getStackInfo_FxnT ti_sysbios_interfaces_IHwi_getStackInfo_FxnT
#define IHwi_getCoreStackInfo ti_sysbios_interfaces_IHwi_getCoreStackInfo
#define IHwi_getCoreStackInfo_fxnP ti_sysbios_interfaces_IHwi_getCoreStackInfo_fxnP
#define IHwi_getCoreStackInfo_FxnT ti_sysbios_interfaces_IHwi_getCoreStackInfo_FxnT
#define IHwi_startup ti_sysbios_interfaces_IHwi_startup
#define IHwi_startup_fxnP ti_sysbios_interfaces_IHwi_startup_fxnP
#define IHwi_startup_FxnT ti_sysbios_interfaces_IHwi_startup_FxnT
#define IHwi_disable ti_sysbios_interfaces_IHwi_disable
#define IHwi_disable_fxnP ti_sysbios_interfaces_IHwi_disable_fxnP
#define IHwi_disable_FxnT ti_sysbios_interfaces_IHwi_disable_FxnT
#define IHwi_enable ti_sysbios_interfaces_IHwi_enable
#define IHwi_enable_fxnP ti_sysbios_interfaces_IHwi_enable_fxnP
#define IHwi_enable_FxnT ti_sysbios_interfaces_IHwi_enable_FxnT
#define IHwi_restore ti_sysbios_interfaces_IHwi_restore
#define IHwi_restore_fxnP ti_sysbios_interfaces_IHwi_restore_fxnP
#define IHwi_restore_FxnT ti_sysbios_interfaces_IHwi_restore_FxnT
#define IHwi_switchFromBootStack ti_sysbios_interfaces_IHwi_switchFromBootStack
#define IHwi_switchFromBootStack_fxnP ti_sysbios_interfaces_IHwi_switchFromBootStack_fxnP
#define IHwi_switchFromBootStack_FxnT ti_sysbios_interfaces_IHwi_switchFromBootStack_FxnT
#define IHwi_post ti_sysbios_interfaces_IHwi_post
#define IHwi_post_fxnP ti_sysbios_interfaces_IHwi_post_fxnP
#define IHwi_post_FxnT ti_sysbios_interfaces_IHwi_post_FxnT
#define IHwi_getTaskSP ti_sysbios_interfaces_IHwi_getTaskSP
#define IHwi_getTaskSP_fxnP ti_sysbios_interfaces_IHwi_getTaskSP_fxnP
#define IHwi_getTaskSP_FxnT ti_sysbios_interfaces_IHwi_getTaskSP_FxnT
#define IHwi_disableInterrupt ti_sysbios_interfaces_IHwi_disableInterrupt
#define IHwi_disableInterrupt_fxnP ti_sysbios_interfaces_IHwi_disableInterrupt_fxnP
#define IHwi_disableInterrupt_FxnT ti_sysbios_interfaces_IHwi_disableInterrupt_FxnT
#define IHwi_enableInterrupt ti_sysbios_interfaces_IHwi_enableInterrupt
#define IHwi_enableInterrupt_fxnP ti_sysbios_interfaces_IHwi_enableInterrupt_fxnP
#define IHwi_enableInterrupt_FxnT ti_sysbios_interfaces_IHwi_enableInterrupt_FxnT
#define IHwi_restoreInterrupt ti_sysbios_interfaces_IHwi_restoreInterrupt
#define IHwi_restoreInterrupt_fxnP ti_sysbios_interfaces_IHwi_restoreInterrupt_fxnP
#define IHwi_restoreInterrupt_FxnT ti_sysbios_interfaces_IHwi_restoreInterrupt_FxnT
#define IHwi_clearInterrupt ti_sysbios_interfaces_IHwi_clearInterrupt
#define IHwi_clearInterrupt_fxnP ti_sysbios_interfaces_IHwi_clearInterrupt_fxnP
#define IHwi_clearInterrupt_FxnT ti_sysbios_interfaces_IHwi_clearInterrupt_FxnT
#define IHwi_getFunc ti_sysbios_interfaces_IHwi_getFunc
#define IHwi_getFunc_fxnP ti_sysbios_interfaces_IHwi_getFunc_fxnP
#define IHwi_getFunc_FxnT ti_sysbios_interfaces_IHwi_getFunc_FxnT
#define IHwi_setFunc ti_sysbios_interfaces_IHwi_setFunc
#define IHwi_setFunc_fxnP ti_sysbios_interfaces_IHwi_setFunc_fxnP
#define IHwi_setFunc_FxnT ti_sysbios_interfaces_IHwi_setFunc_FxnT
#define IHwi_getHookContext ti_sysbios_interfaces_IHwi_getHookContext
#define IHwi_getHookContext_fxnP ti_sysbios_interfaces_IHwi_getHookContext_fxnP
#define IHwi_getHookContext_FxnT ti_sysbios_interfaces_IHwi_getHookContext_FxnT
#define IHwi_setHookContext ti_sysbios_interfaces_IHwi_setHookContext
#define IHwi_setHookContext_fxnP ti_sysbios_interfaces_IHwi_setHookContext_fxnP
#define IHwi_setHookContext_FxnT ti_sysbios_interfaces_IHwi_setHookContext_FxnT
#define IHwi_getIrp ti_sysbios_interfaces_IHwi_getIrp
#define IHwi_getIrp_fxnP ti_sysbios_interfaces_IHwi_getIrp_fxnP
#define IHwi_getIrp_FxnT ti_sysbios_interfaces_IHwi_getIrp_FxnT
#define IHwi_Module_name ti_sysbios_interfaces_IHwi_Module_name
#define IHwi_create ti_sysbios_interfaces_IHwi_create
#define IHwi_delete ti_sysbios_interfaces_IHwi_delete
#define IHwi_Handle_label ti_sysbios_interfaces_IHwi_Handle_label
#define IHwi_Handle_to_Module ti_sysbios_interfaces_IHwi_Handle_to_Module

#endif /* ti_sysbios_interfaces_IHwi__localnames__done */
#endif
