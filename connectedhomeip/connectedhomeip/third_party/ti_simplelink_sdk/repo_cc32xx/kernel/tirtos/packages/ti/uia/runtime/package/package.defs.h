/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#ifndef ti_uia_runtime__
#define ti_uia_runtime__


/*
 * ======== module ti.uia.runtime.CtxFilter ========
 */

typedef struct ti_uia_runtime_CtxFilter_Module_State ti_uia_runtime_CtxFilter_Module_State;

/*
 * ======== module ti.uia.runtime.EventHdr ========
 */


/*
 * ======== module ti.uia.runtime.LogCtxChg ========
 */


/*
 * ======== interface ti.uia.runtime.IUIATraceSyncClient ========
 */

typedef struct ti_uia_runtime_IUIATraceSyncClient_Fxns__ ti_uia_runtime_IUIATraceSyncClient_Fxns__;
typedef const struct ti_uia_runtime_IUIATraceSyncClient_Fxns__* ti_uia_runtime_IUIATraceSyncClient_Module;

/*
 * ======== module ti.uia.runtime.LogSnapshot ========
 */

typedef struct ti_uia_runtime_LogSnapshot_EventRec ti_uia_runtime_LogSnapshot_EventRec;
typedef struct ti_uia_runtime_LogSnapshot_Fxns__ ti_uia_runtime_LogSnapshot_Fxns__;
typedef const struct ti_uia_runtime_LogSnapshot_Fxns__* ti_uia_runtime_LogSnapshot_Module;

/*
 * ======== module ti.uia.runtime.LogSync ========
 */

typedef struct ti_uia_runtime_LogSync_Module_State ti_uia_runtime_LogSync_Module_State;
typedef struct ti_uia_runtime_LogSync_Fxns__ ti_uia_runtime_LogSync_Fxns__;
typedef const struct ti_uia_runtime_LogSync_Fxns__* ti_uia_runtime_LogSync_Module;
typedef struct ti_uia_runtime_LogSync_Params ti_uia_runtime_LogSync_Params;
typedef struct ti_uia_runtime_LogSync_Object ti_uia_runtime_LogSync_Object;
typedef struct ti_uia_runtime_LogSync_Struct ti_uia_runtime_LogSync_Struct;
typedef ti_uia_runtime_LogSync_Object* ti_uia_runtime_LogSync_Handle;
typedef ti_uia_runtime_LogSync_Object* ti_uia_runtime_LogSync_Instance;

/*
 * ======== module ti.uia.runtime.LoggerSM ========
 */

typedef struct ti_uia_runtime_LoggerSM_Module_State ti_uia_runtime_LoggerSM_Module_State;
typedef struct ti_uia_runtime_LoggerSM_SharedObj ti_uia_runtime_LoggerSM_SharedObj;
typedef struct ti_uia_runtime_LoggerSM_Fxns__ ti_uia_runtime_LoggerSM_Fxns__;
typedef const struct ti_uia_runtime_LoggerSM_Fxns__* ti_uia_runtime_LoggerSM_Module;
typedef struct ti_uia_runtime_LoggerSM_Params ti_uia_runtime_LoggerSM_Params;
typedef struct ti_uia_runtime_LoggerSM_Object ti_uia_runtime_LoggerSM_Object;
typedef struct ti_uia_runtime_LoggerSM_Struct ti_uia_runtime_LoggerSM_Struct;
typedef ti_uia_runtime_LoggerSM_Object* ti_uia_runtime_LoggerSM_Handle;
typedef ti_uia_runtime_LoggerSM_Object* ti_uia_runtime_LoggerSM_Instance;

/*
 * ======== module ti.uia.runtime.LoggerTypes ========
 */


/*
 * ======== module ti.uia.runtime.MultiCoreTypes ========
 */

typedef struct ti_uia_runtime_MultiCoreTypes_ServiceHdr ti_uia_runtime_MultiCoreTypes_ServiceHdr;
typedef struct ti_uia_runtime_MultiCoreTypes_RegisterMsg ti_uia_runtime_MultiCoreTypes_RegisterMsg;

/*
 * ======== module ti.uia.runtime.QueueDescriptor ========
 */

typedef struct ti_uia_runtime_QueueDescriptor_Header ti_uia_runtime_QueueDescriptor_Header;
typedef struct ti_uia_runtime_QueueDescriptor_Module_State ti_uia_runtime_QueueDescriptor_Module_State;

/*
 * ======== module ti.uia.runtime.UIAMetaData ========
 */


/*
 * ======== module ti.uia.runtime.UIAPacket ========
 */

typedef struct ti_uia_runtime_UIAPacket_Hdr ti_uia_runtime_UIAPacket_Hdr;
typedef struct ti_uia_runtime_UIAPacket_Footer ti_uia_runtime_UIAPacket_Footer;

/*
 * ======== module ti.uia.runtime.ServiceMgr ========
 */

typedef struct ti_uia_runtime_ServiceMgr_Module_State ti_uia_runtime_ServiceMgr_Module_State;

/*
 * ======== module ti.uia.runtime.Transport ========
 */

typedef struct ti_uia_runtime_Transport_FxnSet ti_uia_runtime_Transport_FxnSet;

/*
 * ======== interface ti.uia.runtime.ICtxFilterCallback ========
 */

typedef struct ti_uia_runtime_ICtxFilterCallback_Fxns__ ti_uia_runtime_ICtxFilterCallback_Fxns__;
typedef const struct ti_uia_runtime_ICtxFilterCallback_Fxns__* ti_uia_runtime_ICtxFilterCallback_Module;
typedef struct ti_uia_runtime_ICtxFilterCallback_Params ti_uia_runtime_ICtxFilterCallback_Params;
typedef struct ti_uia_runtime_ICtxFilterCallback___Object { ti_uia_runtime_ICtxFilterCallback_Fxns__* __fxns; xdc_Bits32 __label; } *ti_uia_runtime_ICtxFilterCallback_Handle;

/*
 * ======== interface ti.uia.runtime.IUIATransfer ========
 */

typedef struct ti_uia_runtime_IUIATransfer_Fxns__ ti_uia_runtime_IUIATransfer_Fxns__;
typedef const struct ti_uia_runtime_IUIATransfer_Fxns__* ti_uia_runtime_IUIATransfer_Module;
typedef struct ti_uia_runtime_IUIATransfer_Params ti_uia_runtime_IUIATransfer_Params;
typedef struct ti_uia_runtime_IUIATransfer___Object { ti_uia_runtime_IUIATransfer_Fxns__* __fxns; xdc_Bits32 __label; } *ti_uia_runtime_IUIATransfer_Handle;

/*
 * ======== interface ti.uia.runtime.ILoggerSnapshot ========
 */

typedef struct ti_uia_runtime_ILoggerSnapshot_Fxns__ ti_uia_runtime_ILoggerSnapshot_Fxns__;
typedef const struct ti_uia_runtime_ILoggerSnapshot_Fxns__* ti_uia_runtime_ILoggerSnapshot_Module;
typedef struct ti_uia_runtime_ILoggerSnapshot_Params ti_uia_runtime_ILoggerSnapshot_Params;
typedef struct ti_uia_runtime_ILoggerSnapshot___Object { ti_uia_runtime_ILoggerSnapshot_Fxns__* __fxns; xdc_Bits32 __label; } *ti_uia_runtime_ILoggerSnapshot_Handle;

/*
 * ======== interface ti.uia.runtime.IUIATimestampProvider ========
 */

typedef struct ti_uia_runtime_IUIATimestampProvider_Fxns__ ti_uia_runtime_IUIATimestampProvider_Fxns__;
typedef const struct ti_uia_runtime_IUIATimestampProvider_Fxns__* ti_uia_runtime_IUIATimestampProvider_Module;

/*
 * ======== interface ti.uia.runtime.IUIATraceSyncProvider ========
 */

typedef struct ti_uia_runtime_IUIATraceSyncProvider_Fxns__ ti_uia_runtime_IUIATraceSyncProvider_Fxns__;
typedef const struct ti_uia_runtime_IUIATraceSyncProvider_Fxns__* ti_uia_runtime_IUIATraceSyncProvider_Module;

/*
 * ======== interface ti.uia.runtime.IServiceMgrSupport ========
 */

typedef struct ti_uia_runtime_IServiceMgrSupport_Fxns__ ti_uia_runtime_IServiceMgrSupport_Fxns__;
typedef const struct ti_uia_runtime_IServiceMgrSupport_Fxns__* ti_uia_runtime_IServiceMgrSupport_Module;

/*
 * ======== module ti.uia.runtime.LogSync_CpuTimestampProxy ========
 */

typedef struct ti_uia_runtime_LogSync_CpuTimestampProxy_Fxns__ ti_uia_runtime_LogSync_CpuTimestampProxy_Fxns__;
typedef const struct ti_uia_runtime_LogSync_CpuTimestampProxy_Fxns__* ti_uia_runtime_LogSync_CpuTimestampProxy_Module;

/*
 * ======== module ti.uia.runtime.LogSync_GlobalTimestampProxy ========
 */

typedef struct ti_uia_runtime_LogSync_GlobalTimestampProxy_Fxns__ ti_uia_runtime_LogSync_GlobalTimestampProxy_Fxns__;
typedef const struct ti_uia_runtime_LogSync_GlobalTimestampProxy_Fxns__* ti_uia_runtime_LogSync_GlobalTimestampProxy_Module;

/*
 * ======== module ti.uia.runtime.ServiceMgr_SupportProxy ========
 */

typedef struct ti_uia_runtime_ServiceMgr_SupportProxy_Fxns__ ti_uia_runtime_ServiceMgr_SupportProxy_Fxns__;
typedef const struct ti_uia_runtime_ServiceMgr_SupportProxy_Fxns__* ti_uia_runtime_ServiceMgr_SupportProxy_Module;


#endif /* ti_uia_runtime__ */ 
