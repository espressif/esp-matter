/***************************************************************************//**
 * @file
 * @brief File System - Core Job Scheduler Operations
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

#ifndef  FS_CORE_JOB_SCHED_PRIV_H_
#define  FS_CORE_JOB_SCHED_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_list_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  JOB_SCHED_VOID_JOB_IX                                    ((JOB_SCHED_JOB_IX)-1)

#define  JOB_SCHED_VOID_JOB_ID                                    ((JOB_SCHED_JOB_ID)-1)

#define  JOB_SCHED_JOB_HANDLE_INIT                                { .JobIx = JOB_SCHED_VOID_JOB_IX, \
                                                                    .JobId = JOB_SCHED_VOID_JOB_ID }

#define  JOB_SCHED_GRAPH_DFS_MODE_NONE                            DEF_BIT_NONE
#define  JOB_SCHED_GRAPH_DFS_MODE_FWD                             DEF_BIT_00
#define  JOB_SCHED_GRAPH_DFS_MODE_BWD                             DEF_BIT_01
#define  JOB_SCHED_GRAPH_DFS_MODE_EXCLUDE_FIRST                   DEF_BIT_02

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT08U JOB_SCHED_JOB_IX;
typedef CPU_INT16U JOB_SCHED_JOB_ID;
typedef CPU_INT16U JOB_SCHED_JOB_SEQ_NO;

typedef CPU_INT08U JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX;
typedef CPU_INT08U JOB_SCHED_GRAPH_LIST_ITEM_IX;
typedef CPU_INT08U JOB_SCHED_GRAPH_LIST_QTY;

typedef struct job_sched_graph_list_adapter {
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX NextIx;
  JOB_SCHED_GRAPH_LIST_ITEM_IX        JobIx;
} JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER;

typedef struct job_sched_graph_list {
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX FreeListHeadIx;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER    *AdapterTbl;
} JOB_SCHED_GRAPH_ADJ_LIST;

typedef struct job_sched_graph_browse_state {
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX CurListAdapterIx;
} JOB_SCHED_GRAPH_BROWSE_STATE;

typedef struct job_sched_graph_adapter {
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX PredListHeadIx;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX SuccrListHeadIx;
} JOB_SCHED_GRAPH_ADAPTER;

typedef struct job_sched_graph_cfg {
  MEM_SEG                             *MemSegPtr;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX AdjListAdapterCnt;
  JOB_SCHED_JOB_IX                    AdapterCnt;
} JOB_SCHED_GRAPH_CFG;

typedef struct job_sched_graph {
  JOB_SCHED_GRAPH_ADJ_LIST AdjList;
  JOB_SCHED_JOB_IX         AdapterCnt;
  JOB_SCHED_GRAPH_ADAPTER  *AdapterTbl;
  CPU_INT08U               *VisitedJobMapPtr;
  CPU_INT08U               *DiscoveredJobMapPtr;
  IB_DLIST                 DfsList;
  IB_DLIST_END             VisitedListEnd;
  IB_DLIST_END             DiscoveredListEnd;
} JOB_SCHED_GRAPH;

typedef enum job_sched_graph_dfs_state {
  JOB_SCHED_GRAPH_DFS_STATE_CONTINUE,
  JOB_SCHED_GRAPH_DFS_STATE_CYCLE,
  JOB_SCHED_GRAPH_DFS_STATE_DROP,
  JOB_SCHED_GRAPH_DFS_STATE_TURN_BACK,
  JOB_SCHED_GRAPH_DFS_STATE_STOP
} JOB_SCHED_GRAPH_DFS_STATE;

typedef int (*JOB_SCHED_RAW_PRINT_CB)(const char *, ...);

typedef struct job_sched_graph_print_data {
  void (*ObjPrint)(JOB_SCHED_JOB_IX       obj_ix,
                   JOB_SCHED_RAW_PRINT_CB p_raw_print_cb,
                   void                   *p_data);

  int (*RawPrint)(const char *, ...);
} JOB_SCHED_GRAPH_PRINT_DATA;

typedef JOB_SCHED_GRAPH_DFS_STATE (*JOB_SCHED_GRAPH_DFS_TRUNC_CB) (JOB_SCHED_GRAPH  *p_graph,
                                                                   JOB_SCHED_JOB_IX job_ix,
                                                                   void             *p_arg,
                                                                   RTOS_ERR         *p_err);

typedef struct job_sched_job_handle {
  JOB_SCHED_JOB_IX JobIx;
  JOB_SCHED_JOB_ID JobId;
} JOB_SCHED_JOB_HANDLE;

typedef void (*JOB_SCHED_EXEC_CB) (void     *p_job_data,
                                   void     *p_dbg_data,
                                   void     *p_caller_data,
                                   RTOS_ERR *p_err);

typedef struct job_sched_job {
  JOB_SCHED_JOB_SEQ_NO SeqNo;
  JOB_SCHED_JOB_ID     Id;
  void                 *DataPtr;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
  void                 *DbgDataPtr;
#endif
} JOB_SCHED_JOB;

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
typedef void (*JOB_SCHED_ON_ORDER_ADD) (void *p_prev_job_data,
                                        void *p_next_job_data);

typedef void (*JOB_SCHED_ON_ORDER_REM) (void *p_prev_job_data,
                                        void *p_next_job_data);
#endif

typedef struct job_sched_cfg {
  JOB_SCHED_JOB_IX       MaxJobCnt;
  CPU_SIZE_T             MaxOrderingLinkCnt;
  JOB_SCHED_EXEC_CB      ExecFunc;
  void                   *ExecFuncDataPtr;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
  JOB_SCHED_ON_ORDER_ADD OnOrderAdd;
  JOB_SCHED_ON_ORDER_REM OnOrderRem;
#endif
} JOB_SCHED_CFG;

typedef struct job_sched {
  JOB_SCHED_JOB          *JobTbl;
  JOB_SCHED_GRAPH        Graph;
  IB_DLIST               JobList;
  IB_DLIST_END           SortedJobListEnd;
  IB_DLIST_END           FreeJobListEnd;
  JOB_SCHED_EXEC_CB      ExecFunc;
  void                   *ExecFuncDataPtr;
  JOB_SCHED_JOB_ID       CurId;
  JOB_SCHED_JOB_SEQ_NO   CurSeqNo;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
  JOB_SCHED_ON_ORDER_ADD OnOrderAdd;
  JOB_SCHED_ON_ORDER_REM OnOrderRem;
#endif
} JOB_SCHED;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const JOB_SCHED_JOB_HANDLE JobSched_VoidJobHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  JOB_SCHED_JOB_HANDLE_IS_VOID(job_handle)                  ((job_handle).JobIx == (JOB_SCHED_JOB_IX)-1)

#define  JOB_SCHED_JOB_GET(p_job_sched, job_ix)                    (&(p_job_sched)->JobTbl[job_ix])

#define  JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, job_handle)    (!JOB_SCHED_JOB_HANDLE_IS_VOID(job_handle) \
                                                                    && ((job_handle).JobId == JOB_SCHED_JOB_GET(p_job_sched, (job_handle).JobIx)->Id))

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void JobSched_Init(JOB_SCHED     *p_job_sched,
                   JOB_SCHED_CFG *p_cfg,
                   MEM_SEG       *p_seg,
                   RTOS_ERR      *p_err);

void *JobSched_DataGet(JOB_SCHED            *p_job_sched,
                       JOB_SCHED_JOB_HANDLE job_handle,
                       RTOS_ERR             *p_err);

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
void *JobSched_DbgDataGet(JOB_SCHED            *p_job_sched,
                          JOB_SCHED_JOB_HANDLE job_handle,
                          RTOS_ERR             *p_err);
#endif

void JobSched_Order(JOB_SCHED            *p_job_sched,
                    JOB_SCHED_JOB_HANDLE prev_job_handle,
                    JOB_SCHED_JOB_HANDLE next_job_handle,
                    JOB_SCHED_EXEC_CB    exec_cb,
                    void                 *p_exec_cb_data,
                    RTOS_ERR             *p_err);

CPU_BOOLEAN JobSched_StubJobTryRem(JOB_SCHED            *p_job_sched,
                                   JOB_SCHED_JOB_HANDLE job_handle);

void JobSched_Exec(JOB_SCHED            *p_job_sched,
                   JOB_SCHED_JOB_HANDLE job_handle,
                   JOB_SCHED_EXEC_CB    exec_cb,
                   void                 *p_exec_cb_arg,
                   RTOS_ERR             *p_err);

JOB_SCHED_JOB_HANDLE JobSched_Add(JOB_SCHED            *p_job_sched,
                                  JOB_SCHED_JOB_HANDLE prev_job_handle,
                                  void                 *p_data,
                                  void                 *p_dbg_data,
                                  JOB_SCHED_EXEC_CB    exec_cb,
                                  void                 *p_exec_cb_data,
                                  RTOS_ERR             *p_err);
/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
