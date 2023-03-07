/***************************************************************************//**
 * @file
 * @brief Network Service Task Module
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_SVC_TASK_PRIV_H_
#define  _NET_SVC_TASK_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_def.h"
#include  "../../include/net_type.h"
#include  "../../include/net_sock.h"

#include  <common/include/kal.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_types.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_SVC_TASK_SOCK_FLAG_NONE         DEF_BIT_NONE
#define  NET_SVC_TASK_SOCK_FLAG_BLOCK        DEF_BIT_00
#define  NET_SVC_TASK_SOCK_FLAG_BIND         DEF_BIT_01

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  void *NET_SVC_TASK_HANDLE;

typedef  struct  net_svc_task_child NET_SVC_TASK_CHILD;

typedef  void (*NET_SVC_TASK_ON_START)   (NET_SVC_TASK_CHILD *,         // Child handle
                                          RTOS_ERR *);

typedef  CPU_BOOLEAN (*NET_SVC_TASK_ON_STOP)    (NET_SVC_TASK_CHILD *,
                                                 RTOS_ERR *);

typedef  void (*NET_SVC_TASK_ON_ERR)     (NET_SVC_TASK_CHILD *,         // Child handle
                                          RTOS_ERR);                    // Error code

typedef  void (*NET_SVC_TASK_ON_MSG)     (NET_SVC_TASK_CHILD *,         // Child handle
                                          CPU_INT32U,                   // Child Message
                                          void *,                       // Child Message argument
                                          RTOS_ERR *);                  // Error return

typedef  void (*NET_SVC_TASK_ON_SOCK_ERR)(NET_SVC_TASK_CHILD *,         // Child handle
                                          NET_SOCK_ID,                  // Socket ID
                                          void *);                      // Socket|connection argument

typedef  void (*NET_SVC_TASK_ON_SOCK_OP) (NET_SVC_TASK_CHILD *,
                                          NET_SOCK_ID,
                                          void *,
                                          RTOS_ERR *);                  // TODO_NET keep error ?

typedef  struct  net_svc_task_child_cfg {
  MEM_SEG    *MemSegPtr;
  CPU_INT08U TmrNbrMax;
  CPU_INT08U SockNbrMax;
  CPU_INT08U MsgNbrMax;
} NET_SVC_TASK_CHILD_CFG;

typedef  enum  net_svc_task_sock_op_type {
  NET_SVC_TASK_SOCK_OP_TYPE_NONE = 0,
  NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY,
  NET_SVC_TASK_SOCK_OP_TYPE_RX_DATA,
  NET_SVC_TASK_SOCK_OP_TYPE_TX_RDY,
  NET_SVC_TASK_SOCK_OP_TYPE_TX_DATA,
  NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY,
  NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_DATA
} NET_SVC_TASK_SOCK_OP_TYPE;

typedef  struct  net_svr_task_sock {
  NET_SOCK_ID               SockID;
  NET_SVC_TASK_SOCK_OP_TYPE OpType;
  NET_IF_NBR                IF_ID;

  NET_SVC_TASK_ON_SOCK_OP   OnRx;
  NET_SVC_TASK_ON_SOCK_OP   OnTx;
  NET_SVC_TASK_ON_SOCK_ERR  OnErr;
  NET_SVC_TASK_ON_STOP      OnStop;                             // TODO_NET : OnStop what for ?

  void                      *FnctArgPtr;

  SLIST_MEMBER              ListNode;
} NET_SVC_TASK_SOCK;

typedef  struct  net_svc_task_hooks {
  NET_SVC_TASK_ON_START OnStart;
  NET_SVC_TASK_ON_STOP  OnStop;
  NET_SVC_TASK_ON_MSG   OnMsg;
  NET_SVC_TASK_ON_ERR   OnErr;
} NET_SVC_TASK_HOOKS;

struct  net_svc_task_child {
  NET_SVC_TASK_CHILD_CFG *CfgPtr;

  MEM_DYN_POOL           *PoolMsgPtr;
  MEM_DYN_POOL           *PoolTmrPtr;
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  MEM_DYN_POOL           *PoolSockPtr;

  SLIST_MEMBER           *SockListPtr;
#endif
  void                   *ChildDataPtr;

  NET_SVC_TASK_HANDLE    TaskHandle;
  NET_SVC_TASK_HOOKS     *HooksPtr;

  SLIST_MEMBER           ListNode;
};

typedef  enum  net_svc_task_tmr_type {
  NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
  NET_SVC_TASK_TMR_TYPE_PERIODIC
} NET_SVC_TASK_TMR_TYPE;

typedef  void (*NET_SVC_TASK_TMR_FNCT)   (NET_SVC_TASK_CHILD *,         // Child handle
                                          void *,
                                          RTOS_ERR *);

typedef  void * NET_SVC_TASK_TMR_HANDLE;
typedef  void * NET_SVC_TASK_SOCK_HANDLE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_SVC_TASK_SOCK_SET_OP_TYPE(p_sock, type)     ((NET_SVC_TASK_SOCK *)p_sock)->OpType = type;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_SVC_TASK_HANDLE NetSvcTask_Init(CPU_INT32U   task_prio,
                                    CPU_STK_SIZE stk_size_elements,
                                    void         *p_stk,
                                    MEM_SEG      *p_mem_seg,
                                    RTOS_ERR     *p_err);

void NetSvcTask_Start(NET_SVC_TASK_HANDLE task_handle,
                      RTOS_ERR            *p_err);

void NetSvcTask_Stop(NET_SVC_TASK_HANDLE task_handle,
                     RTOS_ERR            *p_err);

void NetSvcTask_PrioSet(NET_SVC_TASK_HANDLE task_handle,
                        RTOS_TASK_PRIO      prio,
                        RTOS_ERR            *p_err);

NET_SVC_TASK_CHILD *NetSvcTask_ChildStart(NET_SVC_TASK_HANDLE    task_handle,
                                          NET_SVC_TASK_CHILD_CFG *p_child_cfg,
                                          void                   *p_child_data,
                                          NET_SVC_TASK_HOOKS     *p_hooks,
                                          RTOS_ERR               *p_err);

void NetSvcTask_ChildStop(NET_SVC_TASK_CHILD *p_child,
                          RTOS_ERR           *p_err);

void NetSvcTask_ChildMsgPost(NET_SVC_TASK_CHILD *p_child,
                             CPU_INT32U         child_msg_type,
                             void               *p_child_msg_arg,
                             CPU_BOOLEAN        block,
                             RTOS_ERR           *p_err);

NET_SVC_TASK_TMR_HANDLE NetSvcTask_TmrCreate(NET_SVC_TASK_CHILD    *p_child,
                                             NET_SVC_TASK_TMR_TYPE type,
                                             NET_SVC_TASK_TMR_FNCT fnct,
                                             void                  *p_arg,
                                             CPU_INT32U            timeout_ms,
                                             RTOS_ERR              *p_err);

void NetSvcTask_TmrSet(NET_SVC_TASK_TMR_HANDLE tmr_handle,
                       NET_SVC_TASK_TMR_FNCT   fnct,
                       void                    *p_arg,
                       CPU_INT32U              timeout_ms,
                       RTOS_ERR                *p_err);

void NetSvcTask_TmrDel(NET_SVC_TASK_TMR_HANDLE tmr_handle,
                       RTOS_ERR                *p_err);

NET_SOCK_ID NetSvc_SockCreateClient(NET_SVC_TASK_CHILD       *p_child,
                                    NET_SOCK_PROTOCOL_FAMILY protocol_family,
                                    NET_SOCK_TYPE            sock_type,
                                    NET_SOCK_PROTOCOL        protocol,
                                    NET_FLAGS                flags,
                                    NET_SOCK_ADDR            *p_addr_bind,
                                    NET_SOCK_ADDR_LEN        addr_len,
                                    RTOS_ERR                 *p_err);

NET_SOCK_ID NetSvc_SockCreateServer(NET_SVC_TASK_CHILD       *p_child,
                                    NET_SOCK_PROTOCOL_FAMILY protocol_family,
                                    NET_SOCK_TYPE            sock_type,
                                    NET_SOCK_PROTOCOL        protocol,
                                    NET_FLAGS                flags,
                                    NET_SOCK_ADDR            *p_addr_bind,
                                    NET_SOCK_ADDR_LEN        addr_len,
                                    NET_SOCK_Q_SIZE          sock_q_size,
                                    RTOS_ERR                 *p_err);

NET_SVC_TASK_SOCK *NetSvcTask_SockHandleCreate(NET_SVC_TASK_CHILD        *p_child,
                                               NET_SOCK_ID               sock_id,
                                               NET_SVC_TASK_SOCK_OP_TYPE type,
                                               NET_SVC_TASK_ON_SOCK_OP   rx_hook,
                                               NET_SVC_TASK_ON_SOCK_OP   tx_hook,
                                               NET_SVC_TASK_ON_SOCK_ERR  err_hook,
                                               void                      *p_hook_args,
                                               RTOS_ERR                  *p_err);

void NetSvcTask_SockHandleTypeSet(NET_SVC_TASK_SOCK         *p_svc_task_sock,
                                  NET_SVC_TASK_SOCK_OP_TYPE type);

void NetSvcTask_SockHandleDel(NET_SVC_TASK_CHILD *p_child,
                              NET_SVC_TASK_SOCK  *p_svc_task_sock,
                              RTOS_ERR           *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_SVC_TASK_PRIV_H_
