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
 *     PER-INSTANCE TYPES
 *     VIRTUAL FUNCTIONS
 *     FUNCTION STUBS
 *     FUNCTION SELECTORS
 *     CONVERTORS
 *
 *     EPILOGUE
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_uia_runtime_ILoggerSnapshot__include
#define ti_uia_runtime_ILoggerSnapshot__include

#ifndef __nested__
#define __nested__
#define ti_uia_runtime_ILoggerSnapshot__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_uia_runtime_ILoggerSnapshot___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/uia/runtime/package/package.defs.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>
#include <ti/uia/runtime/IUIATransfer.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* TransferType */
typedef ti_uia_runtime_IUIATransfer_TransferType ti_uia_runtime_ILoggerSnapshot_TransferType;

/* Priority */
typedef ti_uia_runtime_IUIATransfer_Priority ti_uia_runtime_ILoggerSnapshot_Priority;

/* TransferType_RELIABLE */
#define ti_uia_runtime_ILoggerSnapshot_TransferType_RELIABLE ti_uia_runtime_IUIATransfer_TransferType_RELIABLE

/* TransferType_LOSSY */
#define ti_uia_runtime_ILoggerSnapshot_TransferType_LOSSY ti_uia_runtime_IUIATransfer_TransferType_LOSSY

/* Priority_LOW */
#define ti_uia_runtime_ILoggerSnapshot_Priority_LOW ti_uia_runtime_IUIATransfer_Priority_LOW

/* Priority_STANDARD */
#define ti_uia_runtime_ILoggerSnapshot_Priority_STANDARD ti_uia_runtime_IUIATransfer_Priority_STANDARD

/* Priority_HIGH */
#define ti_uia_runtime_ILoggerSnapshot_Priority_HIGH ti_uia_runtime_IUIATransfer_Priority_HIGH

/* Priority_SYNC */
#define ti_uia_runtime_ILoggerSnapshot_Priority_SYNC ti_uia_runtime_IUIATransfer_Priority_SYNC


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_uia_runtime_ILoggerSnapshot_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    ti_uia_runtime_IUIATransfer_TransferType transferType;
    ti_uia_runtime_IUIATransfer_Priority priority;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_uia_runtime_ILoggerSnapshot_Fxns__ {
    const xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(void* inst);
    xdc_Bool (*disable)(void* inst);
    xdc_Void (*write0)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
    xdc_Void (*write1)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
    xdc_Void (*write2)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
    xdc_Void (*write4)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
    xdc_Void (*write8)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
    xdc_Void (*setFilterLevel)(void* inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
    xdc_runtime_Diags_Mask (*getFilterLevel)(void* inst, xdc_runtime_Diags_EventLevel level);
    ti_uia_runtime_IUIATransfer_TransferType (*getTransferType)(void* inst);
    xdc_Bool (*getContents)(void* inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT* cpSize);
    xdc_Bool (*isEmpty)(void* inst);
    xdc_SizeT (*getMaxLength)(void* inst);
    xdc_UInt16 (*getInstanceId)(void* inst);
    ti_uia_runtime_IUIATransfer_Priority (*getPriority)(void* inst);
    xdc_Void (*setPriority)(void* inst, ti_uia_runtime_IUIATransfer_Priority priority);
    xdc_Void (*reset)(void* inst);
    xdc_Void (*writeMemoryRange)(void* inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);
    xdc_runtime_Types_SysFxns2 __sfxns;
};
#ifndef ti_uia_runtime_ILoggerSnapshot_Module__BASE__CR

/* Interface__BASE__C */
__extern const xdc_runtime_Types_Base ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C;
#else
#define ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C (*((xdc_runtime_Types_Base *)(xdcRomConstPtr + ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C_offset)))
#endif


/*
 * ======== FUNCTION STUBS ========
 */

/* create */
xdc__CODESECT(ti_uia_runtime_ILoggerSnapshot_create, "ti_uia_runtime_ILoggerSnapshot_create")
__extern ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_create(ti_uia_runtime_ILoggerSnapshot_Module mod, const ti_uia_runtime_ILoggerSnapshot_Params *prms, xdc_runtime_Error_Block *eb);

/* delete */
xdc__CODESECT(ti_uia_runtime_ILoggerSnapshot_delete, "ti_uia_runtime_ILoggerSnapshot_delete")
__extern xdc_Void ti_uia_runtime_ILoggerSnapshot_delete(ti_uia_runtime_ILoggerSnapshot_Handle *inst);

/* Handle_to_Module */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Handle_to_Module(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Handle_to_Module(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return inst->__fxns;
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_uia_runtime_ILoggerSnapshot_Handle_label(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Types_Label *lab);
static inline xdc_runtime_Types_Label *ti_uia_runtime_ILoggerSnapshot_Handle_label(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Types_Label *lab)
{
    return inst->__fxns->__sysp->__label(inst, lab);
}

/* Module_id */
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_ILoggerSnapshot_Module_id(ti_uia_runtime_ILoggerSnapshot_Module mod);
static inline xdc_runtime_Types_ModuleId ti_uia_runtime_ILoggerSnapshot_Module_id(ti_uia_runtime_ILoggerSnapshot_Module mod)
{
    return mod->__sysp->__mid;
}

/* enable */
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_enable(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_enable( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->enable((void*)inst);
}

/* disable */
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_disable(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_disable( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->disable((void*)inst);
}

/* write0 */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write0(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write0( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid )
{
    inst->__fxns->write0((void*)inst, evt, mid);
}

/* write1 */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write1(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write1( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1 )
{
    inst->__fxns->write1((void*)inst, evt, mid, a1);
}

/* write2 */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write2(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write2( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2 )
{
    inst->__fxns->write2((void*)inst, evt, mid, a1, a2);
}

/* write4 */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write4(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write4( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4 )
{
    inst->__fxns->write4((void*)inst, evt, mid, a1, a2, a3, a4);
}

/* write8 */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write8(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_write8( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8 )
{
    inst->__fxns->write8((void*)inst, evt, mid, a1, a2, a3, a4, a5, a6, a7, a8);
}

/* setFilterLevel */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_setFilterLevel(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_setFilterLevel( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel )
{
    inst->__fxns->setFilterLevel((void*)inst, mask, filterLevel);
}

/* getFilterLevel */
static inline xdc_runtime_Diags_Mask ti_uia_runtime_ILoggerSnapshot_getFilterLevel(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Diags_EventLevel level);
static inline xdc_runtime_Diags_Mask ti_uia_runtime_ILoggerSnapshot_getFilterLevel( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Diags_EventLevel level )
{
    return inst->__fxns->getFilterLevel((void*)inst, level);
}

/* getTransferType */
static inline ti_uia_runtime_IUIATransfer_TransferType ti_uia_runtime_ILoggerSnapshot_getTransferType(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_IUIATransfer_TransferType ti_uia_runtime_ILoggerSnapshot_getTransferType( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->getTransferType((void*)inst);
}

/* getContents */
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_getContents(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT *cpSize);
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_getContents( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT *cpSize )
{
    return inst->__fxns->getContents((void*)inst, hdrBuf, size, cpSize);
}

/* isEmpty */
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_isEmpty(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_Bool ti_uia_runtime_ILoggerSnapshot_isEmpty( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->isEmpty((void*)inst);
}

/* getMaxLength */
static inline xdc_SizeT ti_uia_runtime_ILoggerSnapshot_getMaxLength(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_SizeT ti_uia_runtime_ILoggerSnapshot_getMaxLength( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->getMaxLength((void*)inst);
}

/* getInstanceId */
static inline xdc_UInt16 ti_uia_runtime_ILoggerSnapshot_getInstanceId(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_UInt16 ti_uia_runtime_ILoggerSnapshot_getInstanceId( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->getInstanceId((void*)inst);
}

/* getPriority */
static inline ti_uia_runtime_IUIATransfer_Priority ti_uia_runtime_ILoggerSnapshot_getPriority(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_IUIATransfer_Priority ti_uia_runtime_ILoggerSnapshot_getPriority( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    return inst->__fxns->getPriority((void*)inst);
}

/* setPriority */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_setPriority(ti_uia_runtime_ILoggerSnapshot_Handle inst, ti_uia_runtime_IUIATransfer_Priority priority);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_setPriority( ti_uia_runtime_ILoggerSnapshot_Handle inst, ti_uia_runtime_IUIATransfer_Priority priority )
{
    inst->__fxns->setPriority((void*)inst, priority);
}

/* reset */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_reset(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_reset( ti_uia_runtime_ILoggerSnapshot_Handle inst )
{
    inst->__fxns->reset((void*)inst);
}

/* writeMemoryRange */
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_writeMemoryRange(ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);
static inline xdc_Void ti_uia_runtime_ILoggerSnapshot_writeMemoryRange( ti_uia_runtime_ILoggerSnapshot_Handle inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs )
{
    inst->__fxns->writeMemoryRange((void*)inst, evt, mid, snapshotId, fileName, LineNum, fmt, startAdrs, lengthInMAUs);
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

/* enable_{FxnT,fxnP} */
typedef xdc_Bool (*ti_uia_runtime_ILoggerSnapshot_enable_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_enable_FxnT ti_uia_runtime_ILoggerSnapshot_enable_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_enable_FxnT ti_uia_runtime_ILoggerSnapshot_enable_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_enable_FxnT)inst->__fxns->enable;
}

/* disable_{FxnT,fxnP} */
typedef xdc_Bool (*ti_uia_runtime_ILoggerSnapshot_disable_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_disable_FxnT ti_uia_runtime_ILoggerSnapshot_disable_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_disable_FxnT ti_uia_runtime_ILoggerSnapshot_disable_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_disable_FxnT)inst->__fxns->disable;
}

/* write0_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_write0_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid);
static inline ti_uia_runtime_ILoggerSnapshot_write0_FxnT ti_uia_runtime_ILoggerSnapshot_write0_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_write0_FxnT ti_uia_runtime_ILoggerSnapshot_write0_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_write0_FxnT)inst->__fxns->write0;
}

/* write1_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_write1_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1);
static inline ti_uia_runtime_ILoggerSnapshot_write1_FxnT ti_uia_runtime_ILoggerSnapshot_write1_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_write1_FxnT ti_uia_runtime_ILoggerSnapshot_write1_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_write1_FxnT)inst->__fxns->write1;
}

/* write2_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_write2_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2);
static inline ti_uia_runtime_ILoggerSnapshot_write2_FxnT ti_uia_runtime_ILoggerSnapshot_write2_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_write2_FxnT ti_uia_runtime_ILoggerSnapshot_write2_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_write2_FxnT)inst->__fxns->write2;
}

/* write4_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_write4_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4);
static inline ti_uia_runtime_ILoggerSnapshot_write4_FxnT ti_uia_runtime_ILoggerSnapshot_write4_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_write4_FxnT ti_uia_runtime_ILoggerSnapshot_write4_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_write4_FxnT)inst->__fxns->write4;
}

/* write8_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_write8_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8);
static inline ti_uia_runtime_ILoggerSnapshot_write8_FxnT ti_uia_runtime_ILoggerSnapshot_write8_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_write8_FxnT ti_uia_runtime_ILoggerSnapshot_write8_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_write8_FxnT)inst->__fxns->write8;
}

/* setFilterLevel_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_setFilterLevel_FxnT)(xdc_Void *inst, xdc_runtime_Diags_Mask mask, xdc_runtime_Diags_EventLevel filterLevel);
static inline ti_uia_runtime_ILoggerSnapshot_setFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_setFilterLevel_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_setFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_setFilterLevel_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_setFilterLevel_FxnT)inst->__fxns->setFilterLevel;
}

/* getFilterLevel_{FxnT,fxnP} */
typedef xdc_runtime_Diags_Mask (*ti_uia_runtime_ILoggerSnapshot_getFilterLevel_FxnT)(xdc_Void *inst, xdc_runtime_Diags_EventLevel level);
static inline ti_uia_runtime_ILoggerSnapshot_getFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_getFilterLevel_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_getFilterLevel_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getFilterLevel_FxnT)inst->__fxns->getFilterLevel;
}

/* getTransferType_{FxnT,fxnP} */
typedef ti_uia_runtime_IUIATransfer_TransferType (*ti_uia_runtime_ILoggerSnapshot_getTransferType_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_getTransferType_FxnT ti_uia_runtime_ILoggerSnapshot_getTransferType_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getTransferType_FxnT ti_uia_runtime_ILoggerSnapshot_getTransferType_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getTransferType_FxnT)inst->__fxns->getTransferType;
}

/* getContents_{FxnT,fxnP} */
typedef xdc_Bool (*ti_uia_runtime_ILoggerSnapshot_getContents_FxnT)(xdc_Void *inst, xdc_Ptr hdrBuf, xdc_SizeT size, xdc_SizeT* cpSize);
static inline ti_uia_runtime_ILoggerSnapshot_getContents_FxnT ti_uia_runtime_ILoggerSnapshot_getContents_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getContents_FxnT ti_uia_runtime_ILoggerSnapshot_getContents_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getContents_FxnT)inst->__fxns->getContents;
}

/* isEmpty_{FxnT,fxnP} */
typedef xdc_Bool (*ti_uia_runtime_ILoggerSnapshot_isEmpty_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_isEmpty_FxnT ti_uia_runtime_ILoggerSnapshot_isEmpty_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_isEmpty_FxnT ti_uia_runtime_ILoggerSnapshot_isEmpty_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_isEmpty_FxnT)inst->__fxns->isEmpty;
}

/* getMaxLength_{FxnT,fxnP} */
typedef xdc_SizeT (*ti_uia_runtime_ILoggerSnapshot_getMaxLength_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_getMaxLength_FxnT ti_uia_runtime_ILoggerSnapshot_getMaxLength_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getMaxLength_FxnT ti_uia_runtime_ILoggerSnapshot_getMaxLength_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getMaxLength_FxnT)inst->__fxns->getMaxLength;
}

/* getInstanceId_{FxnT,fxnP} */
typedef xdc_UInt16 (*ti_uia_runtime_ILoggerSnapshot_getInstanceId_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_getInstanceId_FxnT ti_uia_runtime_ILoggerSnapshot_getInstanceId_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getInstanceId_FxnT ti_uia_runtime_ILoggerSnapshot_getInstanceId_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getInstanceId_FxnT)inst->__fxns->getInstanceId;
}

/* getPriority_{FxnT,fxnP} */
typedef ti_uia_runtime_IUIATransfer_Priority (*ti_uia_runtime_ILoggerSnapshot_getPriority_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_getPriority_FxnT ti_uia_runtime_ILoggerSnapshot_getPriority_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_getPriority_FxnT ti_uia_runtime_ILoggerSnapshot_getPriority_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_getPriority_FxnT)inst->__fxns->getPriority;
}

/* setPriority_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_setPriority_FxnT)(xdc_Void *inst, ti_uia_runtime_IUIATransfer_Priority priority);
static inline ti_uia_runtime_ILoggerSnapshot_setPriority_FxnT ti_uia_runtime_ILoggerSnapshot_setPriority_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_setPriority_FxnT ti_uia_runtime_ILoggerSnapshot_setPriority_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_setPriority_FxnT)inst->__fxns->setPriority;
}

/* reset_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_reset_FxnT)(xdc_Void *inst);
static inline ti_uia_runtime_ILoggerSnapshot_reset_FxnT ti_uia_runtime_ILoggerSnapshot_reset_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_reset_FxnT ti_uia_runtime_ILoggerSnapshot_reset_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_reset_FxnT)inst->__fxns->reset;
}

/* writeMemoryRange_{FxnT,fxnP} */
typedef xdc_Void (*ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_FxnT)(xdc_Void *inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_UInt32 snapshotId, xdc_IArg fileName, xdc_IArg LineNum, xdc_IArg fmt, xdc_IArg startAdrs, xdc_UInt32 lengthInMAUs);
static inline ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_FxnT ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst);
static inline ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_FxnT ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_fxnP(ti_uia_runtime_ILoggerSnapshot_Handle inst)
{
    return (ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_FxnT)inst->__fxns->writeMemoryRange;
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast(ti_uia_runtime_ILoggerSnapshot_Module m);
static inline ti_uia_runtime_IUIATransfer_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast(ti_uia_runtime_ILoggerSnapshot_Module m)
{
    return(ti_uia_runtime_IUIATransfer_Module)m;
}

/* Module_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_runtime_ILoggerSnapshot_Module_to_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Module_upCast

/* Module_downCast */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast(ti_uia_runtime_IUIATransfer_Module m);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast(ti_uia_runtime_IUIATransfer_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Module)m;
        }
    }
    return NULL;
}

/* Module_from_ti_uia_runtime_IUIATransfer */
#define ti_uia_runtime_ILoggerSnapshot_Module_from_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Module_downCast

/* Handle_upCast */
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast(ti_uia_runtime_ILoggerSnapshot_Handle i);
static inline ti_uia_runtime_IUIATransfer_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast(ti_uia_runtime_ILoggerSnapshot_Handle i)
{
    return (ti_uia_runtime_IUIATransfer_Handle)i;
}

/* Handle_to_ti_uia_runtime_IUIATransfer */
#define ti_uia_runtime_ILoggerSnapshot_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Handle_upCast

/* Handle_downCast */
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast(ti_uia_runtime_IUIATransfer_Handle i);
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast(ti_uia_runtime_IUIATransfer_Handle i)
{
    ti_uia_runtime_IUIATransfer_Handle i2 = (ti_uia_runtime_IUIATransfer_Handle)i;
    const xdc_runtime_Types_Base* b;
    for (b = i2->__fxns->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Handle)i;
        }
    }
	return 0;
}

/* Handle_from_ti_uia_runtime_IUIATransfer */
#define ti_uia_runtime_ILoggerSnapshot_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Handle_downCast

/* Module_upCast2 */
static inline xdc_runtime_IFilterLogger_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast2(ti_uia_runtime_ILoggerSnapshot_Module m);
static inline xdc_runtime_IFilterLogger_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast2(ti_uia_runtime_ILoggerSnapshot_Module m)
{
    return(xdc_runtime_IFilterLogger_Module)m;
}

/* Module_to_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_ILoggerSnapshot_Module_to_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Module_upCast2

/* Module_downCast2 */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast2(xdc_runtime_IFilterLogger_Module m);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast2(xdc_runtime_IFilterLogger_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Module)m;
        }
    }
    return NULL;
}

/* Module_from_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_ILoggerSnapshot_Module_from_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Module_downCast2

/* Handle_upCast2 */
static inline xdc_runtime_IFilterLogger_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast2(ti_uia_runtime_ILoggerSnapshot_Handle i);
static inline xdc_runtime_IFilterLogger_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast2(ti_uia_runtime_ILoggerSnapshot_Handle i)
{
    return (xdc_runtime_IFilterLogger_Handle)i;
}

/* Handle_to_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_ILoggerSnapshot_Handle_to_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Handle_upCast2

/* Handle_downCast2 */
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast2(xdc_runtime_IFilterLogger_Handle i);
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast2(xdc_runtime_IFilterLogger_Handle i)
{
    xdc_runtime_IFilterLogger_Handle i2 = (xdc_runtime_IFilterLogger_Handle)i;
    const xdc_runtime_Types_Base* b;
    for (b = i2->__fxns->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Handle)i;
        }
    }
	return 0;
}

/* Handle_from_xdc_runtime_IFilterLogger */
#define ti_uia_runtime_ILoggerSnapshot_Handle_from_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Handle_downCast2

/* Module_upCast3 */
static inline xdc_runtime_ILogger_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast3(ti_uia_runtime_ILoggerSnapshot_Module m);
static inline xdc_runtime_ILogger_Module ti_uia_runtime_ILoggerSnapshot_Module_upCast3(ti_uia_runtime_ILoggerSnapshot_Module m)
{
    return(xdc_runtime_ILogger_Module)m;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_uia_runtime_ILoggerSnapshot_Module_to_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Module_upCast3

/* Module_downCast3 */
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast3(xdc_runtime_ILogger_Module m);
static inline ti_uia_runtime_ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module_downCast3(xdc_runtime_ILogger_Module m)
{
    const xdc_runtime_Types_Base* b;
    for (b = m->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Module)m;
        }
    }
    return NULL;
}

/* Module_from_xdc_runtime_ILogger */
#define ti_uia_runtime_ILoggerSnapshot_Module_from_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Module_downCast3

/* Handle_upCast3 */
static inline xdc_runtime_ILogger_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast3(ti_uia_runtime_ILoggerSnapshot_Handle i);
static inline xdc_runtime_ILogger_Handle ti_uia_runtime_ILoggerSnapshot_Handle_upCast3(ti_uia_runtime_ILoggerSnapshot_Handle i)
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_uia_runtime_ILoggerSnapshot_Handle_to_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Handle_upCast3

/* Handle_downCast3 */
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast3(xdc_runtime_ILogger_Handle i);
static inline ti_uia_runtime_ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle_downCast3(xdc_runtime_ILogger_Handle i)
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    const xdc_runtime_Types_Base* b;
    for (b = i2->__fxns->__base; b != NULL; b = b->base) {
        if (b == &ti_uia_runtime_ILoggerSnapshot_Interface__BASE__C) {
            return (ti_uia_runtime_ILoggerSnapshot_Handle)i;
        }
    }
	return 0;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_uia_runtime_ILoggerSnapshot_Handle_from_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Handle_downCast3


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_uia_runtime_ILoggerSnapshot__top__
#undef __nested__
#endif

#endif /* ti_uia_runtime_ILoggerSnapshot__include */


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_uia_runtime_ILoggerSnapshot__nolocalnames)

#ifndef ti_uia_runtime_ILoggerSnapshot__localnames__done
#define ti_uia_runtime_ILoggerSnapshot__localnames__done

/* module prefix */
#define ILoggerSnapshot_Instance ti_uia_runtime_ILoggerSnapshot_Instance
#define ILoggerSnapshot_Handle ti_uia_runtime_ILoggerSnapshot_Handle
#define ILoggerSnapshot_Module ti_uia_runtime_ILoggerSnapshot_Module
#define ILoggerSnapshot_TransferType ti_uia_runtime_ILoggerSnapshot_TransferType
#define ILoggerSnapshot_Priority ti_uia_runtime_ILoggerSnapshot_Priority
#define ILoggerSnapshot_TransferType_RELIABLE ti_uia_runtime_ILoggerSnapshot_TransferType_RELIABLE
#define ILoggerSnapshot_TransferType_LOSSY ti_uia_runtime_ILoggerSnapshot_TransferType_LOSSY
#define ILoggerSnapshot_Priority_LOW ti_uia_runtime_ILoggerSnapshot_Priority_LOW
#define ILoggerSnapshot_Priority_STANDARD ti_uia_runtime_ILoggerSnapshot_Priority_STANDARD
#define ILoggerSnapshot_Priority_HIGH ti_uia_runtime_ILoggerSnapshot_Priority_HIGH
#define ILoggerSnapshot_Priority_SYNC ti_uia_runtime_ILoggerSnapshot_Priority_SYNC
#define ILoggerSnapshot_Params ti_uia_runtime_ILoggerSnapshot_Params
#define ILoggerSnapshot_enable ti_uia_runtime_ILoggerSnapshot_enable
#define ILoggerSnapshot_enable_fxnP ti_uia_runtime_ILoggerSnapshot_enable_fxnP
#define ILoggerSnapshot_enable_FxnT ti_uia_runtime_ILoggerSnapshot_enable_FxnT
#define ILoggerSnapshot_disable ti_uia_runtime_ILoggerSnapshot_disable
#define ILoggerSnapshot_disable_fxnP ti_uia_runtime_ILoggerSnapshot_disable_fxnP
#define ILoggerSnapshot_disable_FxnT ti_uia_runtime_ILoggerSnapshot_disable_FxnT
#define ILoggerSnapshot_write0 ti_uia_runtime_ILoggerSnapshot_write0
#define ILoggerSnapshot_write0_fxnP ti_uia_runtime_ILoggerSnapshot_write0_fxnP
#define ILoggerSnapshot_write0_FxnT ti_uia_runtime_ILoggerSnapshot_write0_FxnT
#define ILoggerSnapshot_write1 ti_uia_runtime_ILoggerSnapshot_write1
#define ILoggerSnapshot_write1_fxnP ti_uia_runtime_ILoggerSnapshot_write1_fxnP
#define ILoggerSnapshot_write1_FxnT ti_uia_runtime_ILoggerSnapshot_write1_FxnT
#define ILoggerSnapshot_write2 ti_uia_runtime_ILoggerSnapshot_write2
#define ILoggerSnapshot_write2_fxnP ti_uia_runtime_ILoggerSnapshot_write2_fxnP
#define ILoggerSnapshot_write2_FxnT ti_uia_runtime_ILoggerSnapshot_write2_FxnT
#define ILoggerSnapshot_write4 ti_uia_runtime_ILoggerSnapshot_write4
#define ILoggerSnapshot_write4_fxnP ti_uia_runtime_ILoggerSnapshot_write4_fxnP
#define ILoggerSnapshot_write4_FxnT ti_uia_runtime_ILoggerSnapshot_write4_FxnT
#define ILoggerSnapshot_write8 ti_uia_runtime_ILoggerSnapshot_write8
#define ILoggerSnapshot_write8_fxnP ti_uia_runtime_ILoggerSnapshot_write8_fxnP
#define ILoggerSnapshot_write8_FxnT ti_uia_runtime_ILoggerSnapshot_write8_FxnT
#define ILoggerSnapshot_setFilterLevel ti_uia_runtime_ILoggerSnapshot_setFilterLevel
#define ILoggerSnapshot_setFilterLevel_fxnP ti_uia_runtime_ILoggerSnapshot_setFilterLevel_fxnP
#define ILoggerSnapshot_setFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_setFilterLevel_FxnT
#define ILoggerSnapshot_getFilterLevel ti_uia_runtime_ILoggerSnapshot_getFilterLevel
#define ILoggerSnapshot_getFilterLevel_fxnP ti_uia_runtime_ILoggerSnapshot_getFilterLevel_fxnP
#define ILoggerSnapshot_getFilterLevel_FxnT ti_uia_runtime_ILoggerSnapshot_getFilterLevel_FxnT
#define ILoggerSnapshot_getTransferType ti_uia_runtime_ILoggerSnapshot_getTransferType
#define ILoggerSnapshot_getTransferType_fxnP ti_uia_runtime_ILoggerSnapshot_getTransferType_fxnP
#define ILoggerSnapshot_getTransferType_FxnT ti_uia_runtime_ILoggerSnapshot_getTransferType_FxnT
#define ILoggerSnapshot_getContents ti_uia_runtime_ILoggerSnapshot_getContents
#define ILoggerSnapshot_getContents_fxnP ti_uia_runtime_ILoggerSnapshot_getContents_fxnP
#define ILoggerSnapshot_getContents_FxnT ti_uia_runtime_ILoggerSnapshot_getContents_FxnT
#define ILoggerSnapshot_isEmpty ti_uia_runtime_ILoggerSnapshot_isEmpty
#define ILoggerSnapshot_isEmpty_fxnP ti_uia_runtime_ILoggerSnapshot_isEmpty_fxnP
#define ILoggerSnapshot_isEmpty_FxnT ti_uia_runtime_ILoggerSnapshot_isEmpty_FxnT
#define ILoggerSnapshot_getMaxLength ti_uia_runtime_ILoggerSnapshot_getMaxLength
#define ILoggerSnapshot_getMaxLength_fxnP ti_uia_runtime_ILoggerSnapshot_getMaxLength_fxnP
#define ILoggerSnapshot_getMaxLength_FxnT ti_uia_runtime_ILoggerSnapshot_getMaxLength_FxnT
#define ILoggerSnapshot_getInstanceId ti_uia_runtime_ILoggerSnapshot_getInstanceId
#define ILoggerSnapshot_getInstanceId_fxnP ti_uia_runtime_ILoggerSnapshot_getInstanceId_fxnP
#define ILoggerSnapshot_getInstanceId_FxnT ti_uia_runtime_ILoggerSnapshot_getInstanceId_FxnT
#define ILoggerSnapshot_getPriority ti_uia_runtime_ILoggerSnapshot_getPriority
#define ILoggerSnapshot_getPriority_fxnP ti_uia_runtime_ILoggerSnapshot_getPriority_fxnP
#define ILoggerSnapshot_getPriority_FxnT ti_uia_runtime_ILoggerSnapshot_getPriority_FxnT
#define ILoggerSnapshot_setPriority ti_uia_runtime_ILoggerSnapshot_setPriority
#define ILoggerSnapshot_setPriority_fxnP ti_uia_runtime_ILoggerSnapshot_setPriority_fxnP
#define ILoggerSnapshot_setPriority_FxnT ti_uia_runtime_ILoggerSnapshot_setPriority_FxnT
#define ILoggerSnapshot_reset ti_uia_runtime_ILoggerSnapshot_reset
#define ILoggerSnapshot_reset_fxnP ti_uia_runtime_ILoggerSnapshot_reset_fxnP
#define ILoggerSnapshot_reset_FxnT ti_uia_runtime_ILoggerSnapshot_reset_FxnT
#define ILoggerSnapshot_writeMemoryRange ti_uia_runtime_ILoggerSnapshot_writeMemoryRange
#define ILoggerSnapshot_writeMemoryRange_fxnP ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_fxnP
#define ILoggerSnapshot_writeMemoryRange_FxnT ti_uia_runtime_ILoggerSnapshot_writeMemoryRange_FxnT
#define ILoggerSnapshot_Module_name ti_uia_runtime_ILoggerSnapshot_Module_name
#define ILoggerSnapshot_delete ti_uia_runtime_ILoggerSnapshot_delete
#define ILoggerSnapshot_Handle_label ti_uia_runtime_ILoggerSnapshot_Handle_label
#define ILoggerSnapshot_Handle_to_Module ti_uia_runtime_ILoggerSnapshot_Handle_to_Module
#define ILoggerSnapshot_Module_upCast ti_uia_runtime_ILoggerSnapshot_Module_upCast
#define ILoggerSnapshot_Module_to_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Module_to_ti_uia_runtime_IUIATransfer
#define ILoggerSnapshot_Module_downCast ti_uia_runtime_ILoggerSnapshot_Module_downCast
#define ILoggerSnapshot_Module_from_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Module_from_ti_uia_runtime_IUIATransfer
#define ILoggerSnapshot_Handle_upCast ti_uia_runtime_ILoggerSnapshot_Handle_upCast
#define ILoggerSnapshot_Handle_to_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Handle_to_ti_uia_runtime_IUIATransfer
#define ILoggerSnapshot_Handle_downCast ti_uia_runtime_ILoggerSnapshot_Handle_downCast
#define ILoggerSnapshot_Handle_from_ti_uia_runtime_IUIATransfer ti_uia_runtime_ILoggerSnapshot_Handle_from_ti_uia_runtime_IUIATransfer
#define ILoggerSnapshot_Module_upCast2 ti_uia_runtime_ILoggerSnapshot_Module_upCast2
#define ILoggerSnapshot_Module_to_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Module_to_xdc_runtime_IFilterLogger
#define ILoggerSnapshot_Module_downCast2 ti_uia_runtime_ILoggerSnapshot_Module_downCast2
#define ILoggerSnapshot_Module_from_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Module_from_xdc_runtime_IFilterLogger
#define ILoggerSnapshot_Handle_upCast2 ti_uia_runtime_ILoggerSnapshot_Handle_upCast2
#define ILoggerSnapshot_Handle_to_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Handle_to_xdc_runtime_IFilterLogger
#define ILoggerSnapshot_Handle_downCast2 ti_uia_runtime_ILoggerSnapshot_Handle_downCast2
#define ILoggerSnapshot_Handle_from_xdc_runtime_IFilterLogger ti_uia_runtime_ILoggerSnapshot_Handle_from_xdc_runtime_IFilterLogger
#define ILoggerSnapshot_Module_upCast3 ti_uia_runtime_ILoggerSnapshot_Module_upCast3
#define ILoggerSnapshot_Module_to_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Module_to_xdc_runtime_ILogger
#define ILoggerSnapshot_Module_downCast3 ti_uia_runtime_ILoggerSnapshot_Module_downCast3
#define ILoggerSnapshot_Module_from_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Module_from_xdc_runtime_ILogger
#define ILoggerSnapshot_Handle_upCast3 ti_uia_runtime_ILoggerSnapshot_Handle_upCast3
#define ILoggerSnapshot_Handle_to_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Handle_to_xdc_runtime_ILogger
#define ILoggerSnapshot_Handle_downCast3 ti_uia_runtime_ILoggerSnapshot_Handle_downCast3
#define ILoggerSnapshot_Handle_from_xdc_runtime_ILogger ti_uia_runtime_ILoggerSnapshot_Handle_from_xdc_runtime_ILogger

#endif /* ti_uia_runtime_ILoggerSnapshot__localnames__done */
#endif
