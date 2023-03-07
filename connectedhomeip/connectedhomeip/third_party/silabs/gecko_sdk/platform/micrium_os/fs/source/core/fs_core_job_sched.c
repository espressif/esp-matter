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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_job_sched_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/core/fs_core_list_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/collections/bitmap_priv.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/include/kal.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (JOB, SCHED)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS

#define  JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX       ((JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX)-1)

#define  JOB_SCHED_ORDER_CHK_MAX_HOP_CNT                10u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct job_sched_tp_sort_data {
  JOB_SCHED     *JobSchedPtr;
  JOB_SCHED_JOB *StartJobPtr;
  CPU_INT32U    EndJobSeqNo;
  CPU_BOOLEAN   Fwd;
} JOB_SCHED_TP_SORT_DATA;

typedef struct job_sched_is_ordered_dfs_trunc_cb_data {
  CPU_SIZE_T       MaxVisitCnt;
  CPU_SIZE_T       CurVisitCnt;
  JOB_SCHED_JOB_IX TargetJobIx;
  CPU_BOOLEAN      TargetFound;
} JOB_SCHED_IS_ORDERED_DFS_TRUNC_CB_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const JOB_SCHED_JOB_HANDLE JobSched_VoidJobHandle = JOB_SCHED_JOB_HANDLE_INIT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  JOB_SCHED_JOB_IX_GET(p_job_sched, p_job)              ((p_job) - (p_job_sched)->JobTbl)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   HIGH LEVEL JOB MANAGEMENT FUNCTIONS
 *******************************************************************************************************/

static CPU_BOOLEAN JobSched_Sort(JOB_SCHED     *p_job_sched,
                                 JOB_SCHED_JOB *p_job_start,
                                 JOB_SCHED_JOB *p_job_end,
                                 RTOS_ERR      *p_err);

static JOB_SCHED_GRAPH_DFS_STATE JobSched_SortCb(JOB_SCHED_GRAPH  *p_graph,
                                                 JOB_SCHED_JOB_IX job_ix,
                                                 void             *p_arg,
                                                 RTOS_ERR         *p_err);

static JOB_SCHED_JOB *JobSched_JobGet(JOB_SCHED         *p_job_sched,
                                      JOB_SCHED_EXEC_CB exec_cb,
                                      void              *p_exec_cb_data,
                                      RTOS_ERR          *p_err);

static void JobSched_ExecInternal(JOB_SCHED         *p_job_sched,
                                  JOB_SCHED_JOB     *p_job_exec,
                                  JOB_SCHED_EXEC_CB exec_cb,
                                  void              *p_exec_cb_data,
                                  RTOS_ERR          *p_err);

static void JobSched_SeqNoRescale(JOB_SCHED *p_job_sched);

static CPU_BOOLEAN JobSched_TryPostpone(JOB_SCHED        *p_job_sched,
                                        JOB_SCHED_JOB_IX stub_job_ix,
                                        JOB_SCHED_JOB_IX target_job_ix);

static CPU_BOOLEAN JobSched_AreOrdered(JOB_SCHED            *p_job_sched,
                                       JOB_SCHED_JOB_HANDLE prev_job_handle,
                                       JOB_SCHED_JOB_HANDLE next_job_handle,
                                       CPU_SIZE_T           max_hop_cnt,
                                       RTOS_ERR             *p_err);

static JOB_SCHED_GRAPH_DFS_STATE JobSched_IsOrderedDfsTruncCb(JOB_SCHED_GRAPH  *p_graph,
                                                              JOB_SCHED_JOB_IX job_ix,
                                                              void             *p_arg,
                                                              RTOS_ERR         *p_err);

/********************************************************************************************************
 *                                           GRAPH FUNCTIONS
 *******************************************************************************************************/

static void JobSched_Graph_Init(JOB_SCHED_GRAPH     *p_graph,
                                JOB_SCHED_GRAPH_CFG *p_cfg,
                                RTOS_ERR            *p_err);

static void JobSched_Graph_JobAdd(JOB_SCHED_GRAPH  *p_graph,
                                  JOB_SCHED_JOB_IX job_ix);

static void JobSched_Graph_JobRem(JOB_SCHED_GRAPH  *p_graph,
                                  JOB_SCHED_JOB_IX job_ix);

static CPU_BOOLEAN JobSched_Graph_EdgeAdd(JOB_SCHED_GRAPH  *p_graph,
                                          JOB_SCHED_JOB_IX pred_job_ix,
                                          JOB_SCHED_JOB_IX succr_job_ix);

static void JobSched_Graph_EdgeRem(JOB_SCHED_GRAPH  *p_graph,
                                   JOB_SCHED_JOB_IX pred_job_ix,
                                   JOB_SCHED_JOB_IX succr_job_ix);

static CPU_BOOLEAN JobSched_Graph_EdgeExist(JOB_SCHED_GRAPH  *p_graph,
                                            JOB_SCHED_JOB_IX pred_job_ix,
                                            JOB_SCHED_JOB_IX succr_job_ix);

static JOB_SCHED_JOB_IX JobSched_Graph_NextSiblingGet(JOB_SCHED_GRAPH              *p_graph,
                                                      JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state);

static JOB_SCHED_JOB_IX JobSched_Graph_FirstPredGet(JOB_SCHED_GRAPH              *p_graph,
                                                    JOB_SCHED_JOB_IX             job_ix,
                                                    JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state);

static JOB_SCHED_JOB_IX JobSched_Graph_FirstSuccrGet(JOB_SCHED_GRAPH              *p_graph,
                                                     JOB_SCHED_JOB_IX             job_ix,
                                                     JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state);

static CPU_BOOLEAN JobSched_Graph_DepthFirstSearch(JOB_SCHED_GRAPH              *p_graph,
                                                   JOB_SCHED_JOB_IX             start_job_ix,
                                                   JOB_SCHED_GRAPH_DFS_TRUNC_CB trunc_cb,
                                                   void                         *p_trunc_cb_data,
                                                   CPU_INT08U                   mode,
                                                   RTOS_ERR                     *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               JobSched_Init()
 *
 * @brief    Initialize a job scheduler.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    p_cfg           Pointer to a job scheduler configuration structure.
 *
 * @param    p_seg           Pointer to an optional memory segment.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void JobSched_Init(JOB_SCHED     *p_job_sched,
                   JOB_SCHED_CFG *p_cfg,
                   MEM_SEG       *p_seg,
                   RTOS_ERR      *p_err)
{
  JOB_SCHED_GRAPH_CFG graph_cfg;
  JOB_SCHED_JOB_IX    job_ix;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_cfg->MaxJobCnt < (JOB_SCHED_JOB_IX)-1,
                            *p_err, RTOS_ERR_INVALID_CFG,; );

    RTOS_ASSERT_DBG_ERR_SET(p_cfg->MaxOrderingLinkCnt < (JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX)-1,
                            *p_err, RTOS_ERR_INVALID_CFG,; );

    //                                                             Graph init.
    graph_cfg.AdapterCnt = p_cfg->MaxJobCnt;
    graph_cfg.AdjListAdapterCnt = p_cfg->MaxOrderingLinkCnt;
    graph_cfg.MemSegPtr = p_seg;

    BREAK_ON_ERR(JobSched_Graph_Init(&p_job_sched->Graph,
                                     &graph_cfg,
                                     p_err));

    //                                                             Job list init.
    BREAK_ON_ERR(IB_DList_Init(&p_job_sched->JobList,
                               p_seg,
                               p_cfg->MaxJobCnt,
                               p_err));

    IB_DList_EndInit(&p_job_sched->FreeJobListEnd);

    IB_DList_EndInit(&p_job_sched->SortedJobListEnd);

    //                                                             Job sched init.
    p_job_sched->CurId = 0u;
    p_job_sched->CurSeqNo = 0u;
    p_job_sched->ExecFunc = p_cfg->ExecFunc;
    p_job_sched->ExecFuncDataPtr = p_cfg->ExecFuncDataPtr;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    p_job_sched->OnOrderAdd = p_cfg->OnOrderAdd;
    p_job_sched->OnOrderRem = p_cfg->OnOrderRem;
#endif

    //                                                             Job tbl alloc.
    BREAK_ON_ERR(p_job_sched->JobTbl = (JOB_SCHED_JOB *)Mem_SegAlloc("FS - Job scheduler job tbl",
                                                                     p_seg,
                                                                     sizeof(JOB_SCHED_JOB) * p_cfg->MaxJobCnt,
                                                                     p_err));

    //                                                             All jobs are initially free.
    for (job_ix = 0u; job_ix < p_cfg->MaxJobCnt; job_ix++) {
      IB_DList_Push(&p_job_sched->JobList,
                    &p_job_sched->FreeJobListEnd,
                    job_ix);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               JobSched_Add()
 *
 * @brief    Add a job to the job scheduler.
 *
 * @param    p_job_sched         Pointer to a job scheduler.
 *
 * @param    prev_job_handle     Handle to the job that must be executed before the added job.
 *
 * @param    p_data              Pointer to caller-specific data to be tied to the added job.
 *
 * @param    p_dbg_data          Pointer to data used for cache debug purposes.
 *
 * @param    exec_cb             Job execution callback.
 *
 * @param    p_exec_cb_data      Pointer to a data structure specific to the execution callback.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Added job handle.
 *
 * @note     (1) When the current sequence number overflows, existing job sequence numbers are rescaled,
 *               i.e. the smallest sequence number becomes zero and other sequence numbers are offset
 *               accordingly.
 *
 * @note     (2) Since the maximum value allowed by the job ID type is a reserved value, the current ID
 *               wraps around after maximum value minus one.
 *******************************************************************************************************/
JOB_SCHED_JOB_HANDLE JobSched_Add(JOB_SCHED            *p_job_sched,
                                  JOB_SCHED_JOB_HANDLE prev_job_handle,
                                  void                 *p_data,
                                  void                 *p_dbg_data,
                                  JOB_SCHED_EXEC_CB    exec_cb,
                                  void                 *p_exec_cb_data,
                                  RTOS_ERR             *p_err)
{
  JOB_SCHED_JOB        *p_new_job;
  JOB_SCHED_JOB_IX     new_job_ix;
  JOB_SCHED_JOB_HANDLE new_job_handle;

  WITH_SCOPE_BEGIN(p_err) {
    new_job_handle = JobSched_VoidJobHandle;

    BREAK_ON_ERR(p_new_job = JobSched_JobGet(p_job_sched,
                                             exec_cb,
                                             p_exec_cb_data,
                                             p_err));

    new_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_new_job);
    JobSched_Graph_JobAdd(&p_job_sched->Graph, new_job_ix);

    //                                                             See Note #1.
    if (p_job_sched->CurSeqNo == (JOB_SCHED_JOB_SEQ_NO)-1) {
      JobSched_SeqNoRescale(p_job_sched);
    }

    //                                                             See Note #2.
    if (p_job_sched->CurId == (JOB_SCHED_JOB_ID)-1) {
      p_job_sched->CurId = 0u;
    }

    //                                                             New job init.
    p_new_job->Id = p_job_sched->CurId++;
    p_new_job->SeqNo = p_job_sched->CurSeqNo++;
    p_new_job->DataPtr = p_data;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    p_new_job->DbgDataPtr = p_dbg_data;
#else
    PP_UNUSED_PARAM(p_dbg_data);
#endif

    //                                                             New job handle init.
    new_job_handle.JobId = p_new_job->Id;
    new_job_handle.JobIx = JOB_SCHED_JOB_IX_GET(p_job_sched, p_new_job);

    //                                                             Add ordering requirement if needed.
    if (JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, prev_job_handle)) {
      BREAK_ON_ERR(JobSched_Order(p_job_sched,
                                  prev_job_handle,
                                  new_job_handle,
                                  exec_cb,
                                  p_exec_cb_data,
                                  p_err));
    }
  } WITH_SCOPE_END

  return (new_job_handle);
}

/****************************************************************************************************//**
 *                                           JobSched_DataGet()
 *
 * @brief    Get caller-specific job data.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    job_handle      Handle to the job to get data from.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to the caller-specific job data.
 *******************************************************************************************************/
void *JobSched_DataGet(JOB_SCHED            *p_job_sched,
                       JOB_SCHED_JOB_HANDLE job_handle,
                       RTOS_ERR             *p_err)
{
  JOB_SCHED_JOB *p_job;

  if (!JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, job_handle)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  p_job = JOB_SCHED_JOB_GET(p_job_sched, job_handle.JobIx);

  return (p_job->DataPtr);
}

/****************************************************************************************************//**
 *                                           JobSched_DbgDataGet()
 *
 * @brief    Get caller-specific debug data.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    job_handle      Handle to the job to get data from.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to the caller-specific debug data.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
void *JobSched_DbgDataGet(JOB_SCHED            *p_job_sched,
                          JOB_SCHED_JOB_HANDLE job_handle,
                          RTOS_ERR             *p_err)
{
  JOB_SCHED_JOB *p_job;

  if (!JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, job_handle)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  p_job = JOB_SCHED_JOB_GET(p_job_sched, job_handle.JobIx);

  return (p_job->DbgDataPtr);
}
#endif

/****************************************************************************************************//**
 *                                               JobSched_Order()
 *
 * @brief    Order two jobs.
 *
 * @param    p_job_sched         Pointer to a job scheduler.
 *
 * @param    prev_job_handle     Handle to the previous job.
 *
 * @param    next_job_handle     Handle to the next job.
 *
 * @param    exec_cb             Job execution callback.
 *
 * @param    p_exec_cb_data      Pointer to a data structure specific to the execution callback.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) If a cycle is detected, then the 'next' job precedes the 'previous' job. Although the
 *               cycle can be broken by flushing any job in the cycle, flushing the 'next' job minimizes
 *               the number of jobs to be executed as it is the job with the least number of dependencies.
 *
 * @note     (2) If edges are missing the 'prev' job must be flushed first to respect the given ordering
 *               requirement.
 *******************************************************************************************************/
void JobSched_Order(JOB_SCHED            *p_job_sched,
                    JOB_SCHED_JOB_HANDLE prev_job_handle,
                    JOB_SCHED_JOB_HANDLE next_job_handle,
                    JOB_SCHED_EXEC_CB    exec_cb,
                    void                 *p_exec_cb_data,
                    RTOS_ERR             *p_err)
{
  JOB_SCHED_JOB    *p_prev_job;
  JOB_SCHED_JOB    *p_next_job;
  JOB_SCHED_JOB_IX prev_job_ix;
  JOB_SCHED_JOB_IX next_job_ix;
  CPU_BOOLEAN      edge_exist;
  CPU_BOOLEAN      cycle_exist;
  CPU_BOOLEAN      is_ordered;

  WITH_SCOPE_BEGIN(p_err) {
    p_prev_job = JOB_SCHED_JOB_GET(p_job_sched, prev_job_handle.JobIx);
    p_next_job = JOB_SCHED_JOB_GET(p_job_sched, next_job_handle.JobIx);

    if (JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, prev_job_handle)
        && JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, next_job_handle)) {
      BREAK_ON_ERR(is_ordered = JobSched_AreOrdered(p_job_sched,
                                                    prev_job_handle,
                                                    next_job_handle,
                                                    JOB_SCHED_ORDER_CHK_MAX_HOP_CNT,
                                                    p_err));
    } else {
      is_ordered = DEF_NO;
    }

    if (!is_ordered) {
      edge_exist = DEF_NO;
      BREAK_ON_ERR(while) (!edge_exist
                           && JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, prev_job_handle)
                           && JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, next_job_handle)) {
        prev_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_prev_job);
        next_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_next_job);

        if (prev_job_ix == next_job_ix) {
          break;
        }

        cycle_exist = DEF_NO;
        if (p_next_job->SeqNo < p_prev_job->SeqNo) {
          BREAK_ON_ERR(cycle_exist = JobSched_Sort(p_job_sched,
                                                   p_prev_job,
                                                   p_next_job,
                                                   p_err));

          if (cycle_exist) {                                    // See Note #1.
            CPU_BOOLEAN postpone_done = DEF_NO;
            if (p_prev_job->DataPtr == DEF_NULL) {
              postpone_done = JobSched_TryPostpone(p_job_sched,
                                                   prev_job_ix,
                                                   next_job_ix);
            }

            if (!postpone_done) {
              BREAK_ON_ERR(JobSched_ExecInternal(p_job_sched,
                                                 p_next_job,
                                                 exec_cb,
                                                 p_exec_cb_data,
                                                 p_err));
            }
          }
        }

        if (!cycle_exist) {
          edge_exist = JobSched_Graph_EdgeExist(&p_job_sched->Graph,
                                                prev_job_ix,
                                                next_job_ix);
          if (!edge_exist) {
            edge_exist = JobSched_Graph_EdgeAdd(&p_job_sched->Graph,
                                                prev_job_ix,
                                                next_job_ix);

            if (!edge_exist) {                                  // See Note #2.
              BREAK_ON_ERR(JobSched_ExecInternal(p_job_sched,
                                                 p_prev_job,
                                                 exec_cb,
                                                 p_exec_cb_data,
                                                 p_err));
            }
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
            else {
              if (p_job_sched->OnOrderAdd != DEF_NULL) {
                p_job_sched->OnOrderAdd(p_prev_job->DbgDataPtr, p_next_job->DbgDataPtr);
              }
            }
#endif
          }
        }
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           JobSched_StubJobTryRem()
 *
 * @brief    Remove a stub job if possible.
 *
 * @param    p_job_sched         Pointer to a job scheduler instance.
 *
 * @param    stub_job_handle     Stub job handle.
 *
 * @return   DEF_YES, if the stub was removed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN JobSched_StubJobTryRem(JOB_SCHED            *p_job_sched,
                                   JOB_SCHED_JOB_HANDLE stub_job_handle)
{
  JOB_SCHED_JOB                *p_stub_job;
  JOB_SCHED_JOB_IX             stub_job_ix;
  JOB_SCHED_JOB_IX             src_job_ix;
  JOB_SCHED_JOB_IX             dest_job_ix;
  JOB_SCHED_JOB_IX             src_job_cnt;
  JOB_SCHED_JOB_IX             dest_job_cnt;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;

  if (!JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, stub_job_handle)) {
    return (DEF_NO);
  }

  stub_job_ix = stub_job_handle.JobIx;
  p_stub_job = JOB_SCHED_JOB_GET(p_job_sched, stub_job_ix);

  RTOS_ASSERT_CRITICAL((p_stub_job->DataPtr == DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  src_job_cnt = 0u;
  for (src_job_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
       src_job_ix != JOB_SCHED_VOID_JOB_IX;
       src_job_ix = JobSched_Graph_NextSiblingGet(&p_job_sched->Graph, &browse_state)) {
    src_job_cnt++;
  }

  dest_job_cnt = 0u;
  for (dest_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
       dest_job_ix != JOB_SCHED_VOID_JOB_IX;
       dest_job_ix = JobSched_Graph_NextSiblingGet(&p_job_sched->Graph, &browse_state)) {
    dest_job_cnt++;
  }

  if ((src_job_cnt > 1u) && (dest_job_cnt > 1u)) {
    return (DEF_NO);
  }

  if (src_job_cnt > dest_job_cnt) {
    dest_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    if (dest_job_ix != JOB_SCHED_VOID_JOB_IX) {
      JobSched_Graph_EdgeRem(&p_job_sched->Graph, stub_job_ix, dest_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      if (p_job_sched->OnOrderRem != DEF_NULL) {
        JOB_SCHED_JOB *p_dest_job;
        p_dest_job = JOB_SCHED_JOB_GET(p_job_sched, dest_job_ix);
        p_job_sched->OnOrderRem(p_stub_job->DbgDataPtr, p_dest_job->DbgDataPtr);
      }
#endif
    }

    src_job_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    while (src_job_ix != JOB_SCHED_VOID_JOB_IX) {
      JobSched_Graph_EdgeRem(&p_job_sched->Graph, src_job_ix, stub_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      if (p_job_sched->OnOrderRem != DEF_NULL) {
        JOB_SCHED_JOB *p_src_job;
        p_src_job = JOB_SCHED_JOB_GET(p_job_sched, src_job_ix);
        p_job_sched->OnOrderRem(p_src_job->DbgDataPtr, p_stub_job->DbgDataPtr);
      }
#endif

      if (dest_job_ix != JOB_SCHED_VOID_JOB_IX) {
        JobSched_Graph_EdgeAdd(&p_job_sched->Graph, src_job_ix, dest_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
        if (p_job_sched->OnOrderAdd != DEF_NULL) {
          JOB_SCHED_JOB *p_src_job;
          JOB_SCHED_JOB *p_dest_job;
          p_src_job = JOB_SCHED_JOB_GET(p_job_sched, src_job_ix);
          p_dest_job = JOB_SCHED_JOB_GET(p_job_sched, dest_job_ix);
          p_job_sched->OnOrderAdd(p_src_job->DbgDataPtr, p_dest_job->DbgDataPtr);
        }
#endif
      }
      src_job_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    }
  } else {
    src_job_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    if (src_job_ix != JOB_SCHED_VOID_JOB_IX) {
      JobSched_Graph_EdgeRem(&p_job_sched->Graph, src_job_ix, stub_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      if (p_job_sched->OnOrderRem != DEF_NULL) {
        JOB_SCHED_JOB *p_src_job;
        p_src_job = JOB_SCHED_JOB_GET(p_job_sched, src_job_ix);
        p_job_sched->OnOrderRem(p_src_job->DbgDataPtr, p_stub_job->DbgDataPtr);
      }
#endif
    }

    dest_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    while (dest_job_ix != JOB_SCHED_VOID_JOB_IX) {
      JobSched_Graph_EdgeRem(&p_job_sched->Graph, stub_job_ix, dest_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      if (p_job_sched->OnOrderRem != DEF_NULL) {
        JOB_SCHED_JOB *p_dest_job;
        p_dest_job = JOB_SCHED_JOB_GET(p_job_sched, dest_job_ix);
        p_job_sched->OnOrderRem(p_stub_job->DbgDataPtr, p_dest_job->DbgDataPtr);
      }
#endif

      if (src_job_ix != JOB_SCHED_VOID_JOB_IX) {
        JobSched_Graph_EdgeAdd(&p_job_sched->Graph, src_job_ix, dest_job_ix);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
        if (p_job_sched->OnOrderAdd != DEF_NULL) {
          JOB_SCHED_JOB *p_src_job;
          JOB_SCHED_JOB *p_dest_job;
          p_src_job = JOB_SCHED_JOB_GET(p_job_sched, src_job_ix);
          p_dest_job = JOB_SCHED_JOB_GET(p_job_sched, dest_job_ix);
          p_job_sched->OnOrderAdd(p_src_job->DbgDataPtr, p_dest_job->DbgDataPtr);
        }
#endif
      }
      dest_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
    }
  }

  JobSched_Graph_JobRem(&p_job_sched->Graph, stub_job_ix);

  IB_DList_Rem(&p_job_sched->JobList,
               &p_job_sched->SortedJobListEnd,
               stub_job_ix);

  IB_DList_Push(&p_job_sched->JobList,
                &p_job_sched->FreeJobListEnd,
                stub_job_ix);

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           JobSched_TryPostpone()
 *
 * @brief    Try to postpone a given job after a stub successor.
 *
 * @param    p_job_sched     Pointer to a job scheduler instance.
 *
 * @param    stub_job_ix     Stub job index.
 *
 * @param    target_job_ix   Target job index.
 *
 * @return   DEF_YES, if the job has been postponed.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_TryPostpone(JOB_SCHED        *p_job_sched,
                                        JOB_SCHED_JOB_IX stub_job_ix,
                                        JOB_SCHED_JOB_IX target_job_ix)
{
  JOB_SCHED_JOB                *p_stub_job;
  JOB_SCHED_JOB                *p_stub_parent_job;
  JOB_SCHED_JOB                *p_target_job;
  JOB_SCHED_JOB_IX             stub_child_job_ix;
  JOB_SCHED_JOB_IX             stub_parent_job_ix;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;
  CPU_BOOLEAN                  target_found;

  p_stub_job = JOB_SCHED_JOB_GET(p_job_sched, stub_job_ix);
  p_target_job = JOB_SCHED_JOB_GET(p_job_sched, target_job_ix);

  RTOS_ASSERT_CRITICAL(p_stub_job->DataPtr == DEF_NULL,
                       RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  target_found = DEF_NO;
  for (stub_parent_job_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
       stub_parent_job_ix != JOB_SCHED_VOID_JOB_IX;
       stub_parent_job_ix = JobSched_Graph_NextSiblingGet(&p_job_sched->Graph, &browse_state)) {
    p_stub_parent_job = JOB_SCHED_JOB_GET(p_job_sched, stub_parent_job_ix);
    if (p_stub_parent_job->Id == p_target_job->Id) {
      target_found = DEF_YES;
      break;
    }
  }

  stub_child_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph, stub_job_ix, &browse_state);
  if (target_found && (stub_child_job_ix == JOB_SCHED_VOID_JOB_IX)) {
    JobSched_Graph_EdgeRem(&p_job_sched->Graph, target_job_ix, stub_job_ix);

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    if (p_job_sched->OnOrderRem != DEF_NULL) {
      p_job_sched->OnOrderRem(p_target_job->DbgDataPtr, p_stub_job->DbgDataPtr);
    }
#endif

    return (DEF_YES);
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                       JobSched_IsOrderedDfsTruncCb()
 *
 * @brief    Ordering check depth-first search truncation callback.
 *
 * @param    p_graph     Pointer to a graph instance.
 *
 * @param    job_ix      Job index.
 *
 * @param    p_arg       Pointer to an ordering check callback data structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Depth-first search state.
 *******************************************************************************************************/
static JOB_SCHED_GRAPH_DFS_STATE JobSched_IsOrderedDfsTruncCb(JOB_SCHED_GRAPH  *p_graph,
                                                              JOB_SCHED_JOB_IX job_ix,
                                                              void             *p_arg,
                                                              RTOS_ERR         *p_err)
{
  JOB_SCHED_IS_ORDERED_DFS_TRUNC_CB_DATA *p_dfs_trunc_data;

  PP_UNUSED_PARAM(p_graph);
  PP_UNUSED_PARAM(p_err);

  p_dfs_trunc_data = (JOB_SCHED_IS_ORDERED_DFS_TRUNC_CB_DATA *)p_arg;

  if (job_ix == p_dfs_trunc_data->TargetJobIx) {
    p_dfs_trunc_data->TargetFound = DEF_YES;
    return (JOB_SCHED_GRAPH_DFS_STATE_STOP);
  }

  p_dfs_trunc_data->CurVisitCnt += 1u;

  if (p_dfs_trunc_data->CurVisitCnt == p_dfs_trunc_data->MaxVisitCnt) {
    return (JOB_SCHED_GRAPH_DFS_STATE_STOP);
  }

  return (JOB_SCHED_GRAPH_DFS_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                           JobSched_AreOrdered()
 *
 * @brief    Check whether two jobs are ordered with respect to each other.
 *
 * @param    p_job_sched         Pointer to a job scheduler instance.
 *
 * @param    prev_job_handle     Handle to previous job.
 *
 * @param    next_job_handle     Handle to next job.
 *
 * @param    max_hop_cnt         Maximum jobs to be visited before reporting jobs as non-ordered.
 *
 * @param    p_err               Error pointer.
 *
 * @return   DEF_YES, if the jobs are ordered.
 *           DEF_NO, if the jobs are NOT ordered OR 'max_visit_cnt' jobs have been inspected.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_AreOrdered(JOB_SCHED            *p_job_sched,
                                       JOB_SCHED_JOB_HANDLE prev_job_handle,
                                       JOB_SCHED_JOB_HANDLE next_job_handle,
                                       CPU_SIZE_T           max_hop_cnt,
                                       RTOS_ERR             *p_err)
{
  JOB_SCHED_IS_ORDERED_DFS_TRUNC_CB_DATA dfs_trunc_data;
  CPU_BOOLEAN                            cycle_exist;

  dfs_trunc_data.CurVisitCnt = 0u;
  dfs_trunc_data.MaxVisitCnt = max_hop_cnt;
  dfs_trunc_data.TargetJobIx = next_job_handle.JobIx;
  dfs_trunc_data.TargetFound = DEF_NO;

  cycle_exist = JobSched_Graph_DepthFirstSearch(&p_job_sched->Graph,
                                                prev_job_handle.JobIx,
                                                &JobSched_IsOrderedDfsTruncCb,
                                                (void *)&dfs_trunc_data,
                                                JOB_SCHED_GRAPH_DFS_MODE_FWD,
                                                p_err);

  RTOS_ASSERT_CRITICAL(!cycle_exist, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  return (dfs_trunc_data.TargetFound);
}

/****************************************************************************************************//**
 *                                               JobSched_Exec()
 *
 * @brief    Execute a given job, taking into account dependencies.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    job_handle      Handle to the job to be executed.
 *
 * @param    exec_cb         $$$$ Add description for 'exec_cb'
 *
 * @param    p_exec_cb_arg   $$$$ Add description for 'p_exec_cb_arg'
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void JobSched_Exec(JOB_SCHED            *p_job_sched,
                   JOB_SCHED_JOB_HANDLE job_handle,
                   JOB_SCHED_EXEC_CB    exec_cb,
                   void                 *p_exec_cb_arg,
                   RTOS_ERR             *p_err)
{
  JOB_SCHED_JOB *p_job;

  if (!JOB_SCHED_JOB_HANDLE_IS_VALID(p_job_sched, job_handle)) {
    return;
  }

  p_job = JOB_SCHED_JOB_GET(p_job_sched, job_handle.JobIx);
  JobSched_ExecInternal(p_job_sched,
                        p_job,
                        exec_cb,
                        p_exec_cb_arg,
                        p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           JobSched_SeqNoRescale()
 *
 * @brief    Rescale job sequence numbers.
 *
 * @param    p_job_sched     Pointer to a job scheduler instance.
 *******************************************************************************************************/
static void JobSched_SeqNoRescale(JOB_SCHED *p_job_sched)
{
  JOB_SCHED_JOB        *p_cur_job;
  JOB_SCHED_JOB_SEQ_NO min_seq_no;
  JOB_SCHED_JOB_SEQ_NO max_seq_no;
  JOB_SCHED_JOB_IX     cur_job_ix;

  //                                                               ----------------- FIND MIN SEQ NO ------------------
  min_seq_no = (JOB_SCHED_JOB_SEQ_NO)-1;
  max_seq_no = 0u;
  for (cur_job_ix = IB_DList_FirstGet(&p_job_sched->SortedJobListEnd);
       cur_job_ix != JOB_SCHED_VOID_JOB_IX;
       cur_job_ix = IB_DList_NextGet(&p_job_sched->JobList, cur_job_ix)) {
    p_cur_job = JOB_SCHED_JOB_GET(p_job_sched, cur_job_ix);

    if (p_cur_job->SeqNo < min_seq_no) {
      min_seq_no = p_cur_job->SeqNo;
    }

    if (p_cur_job->SeqNo > max_seq_no) {
      max_seq_no = p_cur_job->SeqNo;
    }
  }

  //                                                               --------------------- RELABEL ----------------------
  for (cur_job_ix = IB_DList_FirstGet(&p_job_sched->SortedJobListEnd);
       cur_job_ix != JOB_SCHED_VOID_JOB_IX;
       cur_job_ix = IB_DList_NextGet(&p_job_sched->JobList, cur_job_ix)) {
    p_cur_job = JOB_SCHED_JOB_GET(p_job_sched, cur_job_ix);
    p_cur_job->SeqNo -= min_seq_no;
  }

  p_job_sched->CurSeqNo = max_seq_no - min_seq_no + 1u;
}

/****************************************************************************************************//**
 *                                               JobSched_Sort()
 *
 * @brief    Sort jobs between the given start and end jobs.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    p_job_start     Pointer to the start job.
 *
 * @param    p_job_end       Pointer to the end job.
 *
 * @param    p_err           Error pointer.
 *
 * @return   DEF_YES, if a cycle is detected.
 *           DEF_NO,  if no cycle is detected.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_Sort(JOB_SCHED     *p_job_sched,
                                 JOB_SCHED_JOB *p_job_start,
                                 JOB_SCHED_JOB *p_job_end,
                                 RTOS_ERR      *p_err)
{
  JOB_SCHED_JOB          *p_cur_job;
  JOB_SCHED_JOB_IX       cur_job_ix;
  JOB_SCHED_JOB_IX       prev_job_ix;
  JOB_SCHED_JOB_IX       start_job_ix;
  JOB_SCHED_JOB_IX       end_job_ix;
  JOB_SCHED_JOB_IX       after_start_job_ix;
  JOB_SCHED_JOB_IX       before_end_job_ix;
  JOB_SCHED_TP_SORT_DATA tp_sort_data;
  JOB_SCHED_JOB_SEQ_NO   start_job_seq_no;
  JOB_SCHED_JOB_SEQ_NO   end_job_seq_no;
  JOB_SCHED_JOB_SEQ_NO   seq_no;
  CPU_BOOLEAN            cycle_exist;

  if (p_job_start == p_job_end) {                               // Chk if trivial cycle.
    return (DEF_YES);
  }

  start_job_seq_no = p_job_start->SeqNo;
  end_job_seq_no = p_job_end->SeqNo;

  start_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_job_start);
  end_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_job_end);
  after_start_job_ix = IB_DList_NextGet(&p_job_sched->JobList, start_job_ix);
  before_end_job_ix = IB_DList_PrevGet(&p_job_sched->JobList, end_job_ix);

  //                                                               -------------------- FWD SEARCH --------------------
  tp_sort_data.JobSchedPtr = p_job_sched;
  tp_sort_data.EndJobSeqNo = start_job_seq_no;
  tp_sort_data.StartJobPtr = p_job_start;
  tp_sort_data.Fwd = DEF_YES;

  cycle_exist = JobSched_Graph_DepthFirstSearch(&p_job_sched->Graph,
                                                end_job_ix,
                                                JobSched_SortCb,
                                                &tp_sort_data,
                                                JOB_SCHED_GRAPH_DFS_MODE_FWD,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }
  if (cycle_exist) {
    return (DEF_YES);
  }
  //                                                               --------------------- SORTING ----------------------
  prev_job_ix = start_job_ix;
  for (cur_job_ix = IB_DList_FirstGet(&p_job_sched->Graph.DiscoveredListEnd);
       cur_job_ix != JOB_SCHED_VOID_JOB_IX;
       cur_job_ix = IB_DList_NextGet(&p_job_sched->Graph.DfsList, cur_job_ix)) {
    IB_DList_Rem(&p_job_sched->JobList,
                 &p_job_sched->SortedJobListEnd,
                 cur_job_ix);

    IB_DList_InsertAfter(&p_job_sched->JobList,
                         &p_job_sched->SortedJobListEnd,
                         prev_job_ix,
                         cur_job_ix);

    prev_job_ix = cur_job_ix;
  }

  //                                                               -------------------- RELABELING --------------------
  seq_no = end_job_seq_no;
  if (before_end_job_ix != JOB_SCHED_VOID_JOB_IX) {
    cur_job_ix = IB_DList_NextGet(&p_job_sched->JobList, before_end_job_ix);
  } else {
    cur_job_ix = IB_DList_FirstGet(&p_job_sched->SortedJobListEnd);
  }

  for (; cur_job_ix != after_start_job_ix;
       cur_job_ix = IB_DList_NextGet(&p_job_sched->JobList, cur_job_ix)) {
    p_cur_job = JOB_SCHED_JOB_GET(p_job_sched, cur_job_ix);
    p_cur_job->SeqNo = seq_no++;
  }

  RTOS_ASSERT_CRITICAL((seq_no - 1u < p_job_sched->CurSeqNo), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                               JobSched_SortCb()
 *
 * @brief    Job scheduler sort depth first search callback.
 *
 * @param    p_graph     Pointer to the browsed graph instance.
 *
 * @param    job_ix
 *
 * @param    p_arg       Pointer to caller-specific callback argument.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Browse state.
 *******************************************************************************************************/
static JOB_SCHED_GRAPH_DFS_STATE JobSched_SortCb(JOB_SCHED_GRAPH  *p_graph,
                                                 JOB_SCHED_JOB_IX job_ix,
                                                 void             *p_arg,
                                                 RTOS_ERR         *p_err)
{
  JOB_SCHED_JOB          *p_job;
  JOB_SCHED_TP_SORT_DATA *p_sort_data;

  PP_UNUSED_PARAM(p_graph);
  PP_UNUSED_PARAM(p_err);

  p_sort_data = (JOB_SCHED_TP_SORT_DATA *)p_arg;

  p_job = JOB_SCHED_JOB_GET(p_sort_data->JobSchedPtr, job_ix);

  if (p_sort_data->StartJobPtr == p_job) {
    return (JOB_SCHED_GRAPH_DFS_STATE_CYCLE);
  }

  if (p_sort_data->Fwd) {
    if (p_job->SeqNo > p_sort_data->EndJobSeqNo) {
      return (JOB_SCHED_GRAPH_DFS_STATE_DROP);
    } else {
      return (JOB_SCHED_GRAPH_DFS_STATE_CONTINUE);
    }
  } else {
    if (p_job->SeqNo < p_sort_data->EndJobSeqNo) {
      return (JOB_SCHED_GRAPH_DFS_STATE_DROP);
    } else {
      return (JOB_SCHED_GRAPH_DFS_STATE_CONTINUE);
    }
  }
}

/****************************************************************************************************//**
 *                                           JobSched_ExecInternal()
 *
 * @brief    Execute a given job, taking into account dependencies.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    p_job_exec      Pointer to job to be executed.
 *
 * @param    exec_cb         Job execution callback.
 *
 * @param    p_exec_cb_data  Pointer to a data structure specific to the execution callback.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) All cycles should be detected and broken upon adding a new job. A cycle should never
 *               be detected during job execution. Otherwise the job scheduler is corrupted.
 *******************************************************************************************************/
static void JobSched_ExecInternal(JOB_SCHED         *p_job_sched,
                                  JOB_SCHED_JOB     *p_job_exec,
                                  JOB_SCHED_EXEC_CB exec_cb,
                                  void              *p_exec_cb_data,
                                  RTOS_ERR          *p_err)
{
  JOB_SCHED_JOB                *p_cur_job;
  void                         *p_dbg_data;
  JOB_SCHED_JOB_IX             next_job_ix;
  JOB_SCHED_JOB_IX             cur_job_ix;
  JOB_SCHED_JOB_IX             exec_job_ix;
  CPU_BOOLEAN                  cycle_exist;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             -------------- LOOK FOR UPSTREAM BLKS --------------
    exec_job_ix = JOB_SCHED_JOB_IX_GET(p_job_sched, p_job_exec);
    BREAK_ON_ERR(cycle_exist = JobSched_Graph_DepthFirstSearch(&p_job_sched->Graph,
                                                               exec_job_ix,
                                                               DEF_NULL,
                                                               DEF_NULL,
                                                               JOB_SCHED_GRAPH_DFS_MODE_BWD,
                                                               p_err));

    //                                                             See Note #1.
    RTOS_ASSERT_CRITICAL((!cycle_exist), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    BREAK_ON_ERR(for) (cur_job_ix = IB_DList_LastGet(&p_job_sched->Graph.DiscoveredListEnd);
                       cur_job_ix != JOB_SCHED_VOID_JOB_IX;
                       cur_job_ix = IB_DList_PrevGet(&p_job_sched->Graph.DfsList, cur_job_ix)) {
      p_cur_job = JOB_SCHED_JOB_GET(p_job_sched, cur_job_ix);

      while ((next_job_ix = JobSched_Graph_FirstSuccrGet(&p_job_sched->Graph,
                                                         cur_job_ix,
                                                         &browse_state)) != JOB_SCHED_VOID_JOB_IX) {
        JobSched_Graph_EdgeRem(&p_job_sched->Graph, cur_job_ix, next_job_ix);

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
        if (p_job_sched->OnOrderRem != DEF_NULL) {
          JOB_SCHED_JOB *p_next_job;
          p_next_job = JOB_SCHED_JOB_GET(p_job_sched, next_job_ix);
          p_job_sched->OnOrderRem(p_cur_job->DbgDataPtr, p_next_job->DbgDataPtr);
        }
#endif
      }

      JobSched_Graph_JobRem(&p_job_sched->Graph, cur_job_ix);

      IB_DList_Rem(&p_job_sched->JobList,
                   &p_job_sched->SortedJobListEnd,
                   cur_job_ix);

      IB_DList_Push(&p_job_sched->JobList,
                    &p_job_sched->FreeJobListEnd,
                    cur_job_ix);

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      p_dbg_data = p_cur_job->DbgDataPtr;
#else
      p_dbg_data = DEF_NULL;
#endif

      BREAK_ON_ERR(exec_cb(p_cur_job->DataPtr,
                           p_dbg_data,
                           p_exec_cb_data,
                           p_err));

      p_cur_job->Id = (JOB_SCHED_JOB_ID)-1;
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               JobSched_JobGet()
 *
 * @brief    Get a new job from a job scheduler.
 *
 * @param    p_job_sched     Pointer to a job scheduler.
 *
 * @param    exec_cb         Job execution callback.
 *
 * @param    p_exec_cb_data  Pointer to a data structure specific to the execution callback.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to newly allocated job.
 *
 * @note     (1) Orphan stub jobs and old jobs (ones with a sequence number lower than half the sequence
 *               number range) are recycled before attempting to get a job from the free list.
 *******************************************************************************************************/
static JOB_SCHED_JOB *JobSched_JobGet(JOB_SCHED         *p_job_sched,
                                      JOB_SCHED_EXEC_CB exec_cb,
                                      void              *p_exec_cb_data,
                                      RTOS_ERR          *p_err)
{
  JOB_SCHED_JOB                *p_job = DEF_NULL;
  JOB_SCHED_JOB                *p_cur_job;
  JOB_SCHED_JOB                *p_exec_job;
  JOB_SCHED_JOB_IX             job_ix;
  JOB_SCHED_JOB_IX             exec_job_ix;
  JOB_SCHED_JOB_IX             cur_job_ix;
  JOB_SCHED_JOB_IX             cur_job_parent_ix;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;

  WITH_SCOPE_BEGIN(p_err) {
    job_ix = (JOB_SCHED_JOB_IX)-1;
    BREAK_ON_ERR(while) (job_ix == (JOB_SCHED_JOB_IX)-1) {
      //                                                           See Note #1.
      p_exec_job = DEF_NULL;
      for (cur_job_ix = IB_DList_FirstGet(&p_job_sched->SortedJobListEnd);
           cur_job_ix != JOB_SCHED_VOID_JOB_IX;
           cur_job_ix = IB_DList_NextGet(&p_job_sched->JobList, cur_job_ix)) {
        p_cur_job = JOB_SCHED_JOB_GET(p_job_sched, cur_job_ix);
        cur_job_parent_ix = JobSched_Graph_FirstPredGet(&p_job_sched->Graph, cur_job_ix, &browse_state);
        if ((p_cur_job->DataPtr == DEF_NULL) && (cur_job_parent_ix == JOB_SCHED_VOID_JOB_IX)) {
          p_exec_job = p_cur_job;
        } else if (p_job_sched->CurSeqNo - p_cur_job->SeqNo > (JOB_SCHED_JOB_SEQ_NO)((JOB_SCHED_JOB_SEQ_NO)-1 / 2u)) {
          p_exec_job = p_cur_job;
          break;
        }
      }

      if (p_exec_job != DEF_NULL) {
        BREAK_ON_ERR(JobSched_ExecInternal(p_job_sched,
                                           p_exec_job,
                                           exec_cb,
                                           p_exec_cb_data,
                                           p_err));
      }

      job_ix = IB_DList_Pop(&p_job_sched->JobList, &p_job_sched->FreeJobListEnd);

      if (job_ix == (JOB_SCHED_JOB_IX)-1) {
        exec_job_ix = IB_DList_FirstGet(&p_job_sched->SortedJobListEnd);
        p_exec_job = JOB_SCHED_JOB_GET(p_job_sched, exec_job_ix);

        BREAK_ON_ERR(JobSched_ExecInternal(p_job_sched,
                                           p_exec_job,
                                           exec_cb,
                                           p_exec_cb_data,
                                           p_err));
      }
    }

    IB_DList_PushBack(&p_job_sched->JobList,
                      &p_job_sched->SortedJobListEnd,
                      job_ix);

    p_job = JOB_SCHED_JOB_GET(p_job_sched, job_ix);
  } WITH_SCOPE_END

  return (p_job);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       GRAPH FUNCTION DEFINITIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           JobSched_Graph_Init()
 *
 * @brief    Init job scheduler graph instance.
 *
 * @param    p_graph     Pointer to a graph instance.
 *
 * @param    p_cfg       Pointer to a graph configuration structure.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void JobSched_Graph_Init(JOB_SCHED_GRAPH     *p_graph,
                                JOB_SCHED_GRAPH_CFG *p_cfg,
                                RTOS_ERR            *p_err)
{
  CPU_SIZE_T map_size;

  //                                                               Adjacency list adapter count must be even.
  RTOS_ASSERT_CRITICAL((p_cfg->AdjListAdapterCnt & 1) == 0u, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  RTOS_ASSERT_DBG(p_cfg->AdapterCnt < JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX, RTOS_ERR_INVALID_CFG,; );

  p_graph->AdapterTbl = (JOB_SCHED_GRAPH_ADAPTER *)Mem_SegAlloc("FS - Job scheduler graph adapter",
                                                                p_cfg->MemSegPtr,
                                                                p_cfg->AdapterCnt * sizeof(JOB_SCHED_GRAPH_ADAPTER),
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (JOB_SCHED_JOB_IX job_ix = 0u; job_ix < p_cfg->AdapterCnt; job_ix++) {
    p_graph->AdapterTbl[job_ix].PredListHeadIx = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
    p_graph->AdapterTbl[job_ix].SuccrListHeadIx = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
  }

  p_graph->AdapterCnt = p_cfg->AdapterCnt;

  //                                                               Depth-first search list init.
  IB_DList_Init(&p_graph->DfsList,
                p_cfg->MemSegPtr,
                p_cfg->AdapterCnt,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  IB_DList_EndInit(&p_graph->VisitedListEnd);
  IB_DList_EndInit(&p_graph->DiscoveredListEnd);

  //                                                               Depth-first search node marking maps alloc.
  map_size = (p_cfg->AdapterCnt - 1u) / 8u  + 1u;

  p_graph->DiscoveredJobMapPtr = (CPU_INT08U *)Mem_SegAlloc("FS - Job scheduler discovered job map",
                                                            p_cfg->MemSegPtr,
                                                            map_size,
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_graph->VisitedJobMapPtr = (CPU_INT08U *)Mem_SegAlloc("FS - Job scheduler visited job map",
                                                         p_cfg->MemSegPtr,
                                                         map_size,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Adjacency list init.
  p_graph->AdjList.AdapterTbl = (JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER *)Mem_SegAlloc("FS - Job scheduler graph adj list adapter",
                                                                                 p_cfg->MemSegPtr,
                                                                                 p_cfg->AdjListAdapterCnt * sizeof(JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER),
                                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_graph->AdjList.FreeListHeadIx = 0u;
  for (JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX ix = 0u; ix < p_cfg->AdjListAdapterCnt - 1u; ix++) {
    p_graph->AdjList.AdapterTbl[ix].NextIx = ix + 1u;
    p_graph->AdjList.AdapterTbl[ix].JobIx = JOB_SCHED_VOID_JOB_IX;
  }
  p_graph->AdjList.AdapterTbl[p_cfg->AdjListAdapterCnt - 1u].NextIx = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
}

/****************************************************************************************************//**
 *                                           JobSched_Graph_JobAdd()
 *
 * @brief    Add a job to a graph.
 *
 * @param    p_graph     Pointer to a graph instance.
 *
 * @param    job_ix      Job index.
 *******************************************************************************************************/
static void JobSched_Graph_JobAdd(JOB_SCHED_GRAPH  *p_graph,
                                  JOB_SCHED_JOB_IX job_ix)
{
  RTOS_ASSERT_CRITICAL((p_graph->AdapterTbl[job_ix].SuccrListHeadIx == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX)
                       && (p_graph->AdapterTbl[job_ix].PredListHeadIx == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX),
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           JobSched_Graph_JobRem()
 *
 * @brief    Remove a job from a graph.
 *
 * @param    p_graph     Pointer to a graph instance.
 *
 * @param    job_ix      Job index.
 *******************************************************************************************************/
static void JobSched_Graph_JobRem(JOB_SCHED_GRAPH  *p_graph,
                                  JOB_SCHED_JOB_IX job_ix)
{
  JOB_SCHED_JOB_IX             pred_job_ix;
  JOB_SCHED_JOB_IX             succr_job_ix;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;

  pred_job_ix = JobSched_Graph_FirstPredGet(p_graph, job_ix, &browse_state);
  succr_job_ix = JobSched_Graph_FirstSuccrGet(p_graph, job_ix, &browse_state);

  RTOS_ASSERT_CRITICAL((pred_job_ix == JOB_SCHED_VOID_JOB_IX)
                       && (succr_job_ix == JOB_SCHED_VOID_JOB_IX),
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; )

  p_graph->AdapterTbl[job_ix].PredListHeadIx = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
  p_graph->AdapterTbl[job_ix].SuccrListHeadIx = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
}

/****************************************************************************************************//**
 *                                           JobSched_Graph_EdgeAdd()
 *
 * @brief    Add a directed edge between to jobs.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    pred_job_ix     Predecessor job index.
 *
 * @param    succr_job_ix    Successor job index.
 *
 * @return   DEF_YES, if the edge has been added.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_Graph_EdgeAdd(JOB_SCHED_GRAPH  *p_graph,
                                          JOB_SCHED_JOB_IX pred_job_ix,
                                          JOB_SCHED_JOB_IX succr_job_ix)
{
  JOB_SCHED_GRAPH_ADJ_LIST            *p_adj_list;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX list_adapter_ix;

  p_adj_list = &p_graph->AdjList;

  //                                                               -------- ADD PRED JOB TO SUCCR'S PRED LIST ---------
  list_adapter_ix = p_adj_list->FreeListHeadIx;
  if (list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    return (DEF_NO);
  }
  p_adj_list->FreeListHeadIx = p_adj_list->AdapterTbl[list_adapter_ix].NextIx;

  p_adj_list->AdapterTbl[list_adapter_ix].JobIx = pred_job_ix;
  p_adj_list->AdapterTbl[list_adapter_ix].NextIx = p_graph->AdapterTbl[succr_job_ix].PredListHeadIx;
  p_graph->AdapterTbl[succr_job_ix].PredListHeadIx = list_adapter_ix;

  //                                                               -------- ADD SUCCR JOB TO PRED'S SUCCR LIST --------
  list_adapter_ix = p_adj_list->FreeListHeadIx;
  if (list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    return (DEF_NO);
  }
  p_adj_list->FreeListHeadIx = p_adj_list->AdapterTbl[list_adapter_ix].NextIx;

  p_adj_list->AdapterTbl[list_adapter_ix].JobIx = succr_job_ix;
  p_adj_list->AdapterTbl[list_adapter_ix].NextIx = p_graph->AdapterTbl[pred_job_ix].SuccrListHeadIx;
  p_graph->AdapterTbl[pred_job_ix].SuccrListHeadIx = list_adapter_ix;

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           JobSched_Graph_EdgeRem()
 *
 * @brief    Remove an edge between two jobs.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    pred_job_ix     Predecessor job index.
 *
 * @param    succr_job_ix    Successor job index.
 *******************************************************************************************************/
static void JobSched_Graph_EdgeRem(JOB_SCHED_GRAPH  *p_graph,
                                   JOB_SCHED_JOB_IX pred_job_ix,
                                   JOB_SCHED_JOB_IX succr_job_ix)
{
  JOB_SCHED_GRAPH_ADAPTER             *p_pred_adapter;
  JOB_SCHED_GRAPH_ADAPTER             *p_succr_adapter;
  JOB_SCHED_GRAPH_ADJ_LIST            *p_adj_list;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX cur_list_adapter_ix;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX prev_list_adapter_ix;
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX next_list_adapter_ix;
  CPU_BOOLEAN                         job_found;

  p_adj_list = &p_graph->AdjList;

  p_pred_adapter = &p_graph->AdapterTbl[pred_job_ix];
  p_succr_adapter = &p_graph->AdapterTbl[succr_job_ix];

  job_found = DEF_NO;
  prev_list_adapter_ix = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
  for (cur_list_adapter_ix = p_pred_adapter->SuccrListHeadIx;
       cur_list_adapter_ix != JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
       cur_list_adapter_ix = p_graph->AdjList.AdapterTbl[cur_list_adapter_ix].NextIx) {
    if (p_graph->AdjList.AdapterTbl[cur_list_adapter_ix].JobIx == succr_job_ix) {
      job_found = DEF_YES;
      break;
    }
    prev_list_adapter_ix = cur_list_adapter_ix;
  }

  RTOS_ASSERT_CRITICAL(job_found, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  next_list_adapter_ix = p_adj_list->AdapterTbl[cur_list_adapter_ix].NextIx;
  if (prev_list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    p_pred_adapter->SuccrListHeadIx = next_list_adapter_ix;
  } else {
    p_adj_list->AdapterTbl[prev_list_adapter_ix].NextIx = next_list_adapter_ix;
  }

  p_adj_list->AdapterTbl[cur_list_adapter_ix].NextIx = p_adj_list->FreeListHeadIx;
  p_adj_list->FreeListHeadIx = cur_list_adapter_ix;

  job_found = DEF_NO;
  prev_list_adapter_ix = JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
  for (cur_list_adapter_ix = p_succr_adapter->PredListHeadIx;
       cur_list_adapter_ix != JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX;
       cur_list_adapter_ix = p_graph->AdjList.AdapterTbl[cur_list_adapter_ix].NextIx) {
    if (p_graph->AdjList.AdapterTbl[cur_list_adapter_ix].JobIx == pred_job_ix) {
      job_found = DEF_YES;
      break;
    }
    prev_list_adapter_ix = cur_list_adapter_ix;
  }

  RTOS_ASSERT_CRITICAL(job_found, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  next_list_adapter_ix = p_adj_list->AdapterTbl[cur_list_adapter_ix].NextIx;
  if (prev_list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    p_succr_adapter->PredListHeadIx = next_list_adapter_ix;
  } else {
    p_adj_list->AdapterTbl[prev_list_adapter_ix].NextIx = next_list_adapter_ix;
  }

  p_adj_list->AdapterTbl[cur_list_adapter_ix].NextIx = p_adj_list->FreeListHeadIx;
  p_adj_list->FreeListHeadIx = cur_list_adapter_ix;
}

/****************************************************************************************************//**
 *                                       JobSched_Graph_EdgeExist()
 *
 * @brief    Check whether an edge exist between two jobs.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    pred_job_ix     Predecessor job index.
 *
 * @param    succr_job_ix    Successor job index.
 *
 * @return   DEF_YES, if an edge exist.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_Graph_EdgeExist(JOB_SCHED_GRAPH  *p_graph,
                                            JOB_SCHED_JOB_IX pred_job_ix,
                                            JOB_SCHED_JOB_IX succr_job_ix)
{
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;
  JOB_SCHED_JOB_IX             job_ix;

  for (job_ix = JobSched_Graph_FirstSuccrGet(p_graph, pred_job_ix, &browse_state);
       job_ix != JOB_SCHED_VOID_JOB_IX;
       job_ix = JobSched_Graph_NextSiblingGet(p_graph, &browse_state)) {
    if (job_ix == succr_job_ix) {
      return (DEF_YES);
    }
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                       JobSched_Graph_NextSiblingGet()
 *
 * @brief    Get next job sibling.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    p_browse_state  Pointer to a browse state.
 *
 * @return   Index of the next job sibling.
 *******************************************************************************************************/
static JOB_SCHED_JOB_IX JobSched_Graph_NextSiblingGet(JOB_SCHED_GRAPH              *p_graph,
                                                      JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state)
{
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX next_list_adapter_ix;

  next_list_adapter_ix = p_graph->AdjList.AdapterTbl[p_browse_state->CurListAdapterIx].NextIx;
  if (next_list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    return (JOB_SCHED_VOID_JOB_IX);
  }

  p_browse_state->CurListAdapterIx = next_list_adapter_ix;

  return (p_graph->AdjList.AdapterTbl[next_list_adapter_ix].JobIx);
}

/****************************************************************************************************//**
 *                                       JobSched_Graph_FirstPredGet()
 *
 * @brief    Get first job predecessor in the graph.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    job_ix          Job index.
 *
 * @param    p_browse_state  Pointer to a browse state.
 *
 * @return   Index of the first job predecessor.
 *******************************************************************************************************/
static JOB_SCHED_JOB_IX JobSched_Graph_FirstPredGet(JOB_SCHED_GRAPH              *p_graph,
                                                    JOB_SCHED_JOB_IX             job_ix,
                                                    JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state)
{
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX first_list_adapter_ix;

  first_list_adapter_ix = p_graph->AdapterTbl[job_ix].PredListHeadIx;
  if (first_list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    return (JOB_SCHED_VOID_JOB_IX);
  }

  p_browse_state->CurListAdapterIx = first_list_adapter_ix;

  return (p_graph->AdjList.AdapterTbl[first_list_adapter_ix].JobIx);
}

/****************************************************************************************************//**
 *                                       JobSched_Graph_FirstSuccrGet()
 *
 * @brief    Get first job successor in the graph.
 *
 * @param    p_graph         Pointer to a graph instance.
 *
 * @param    job_ix          Job index.
 *
 * @param    p_browse_state  Pointer to a browse state.
 *
 * @return   Index of the first job successor.
 *******************************************************************************************************/
static JOB_SCHED_JOB_IX JobSched_Graph_FirstSuccrGet(JOB_SCHED_GRAPH              *p_graph,
                                                     JOB_SCHED_JOB_IX             job_ix,
                                                     JOB_SCHED_GRAPH_BROWSE_STATE *p_browse_state)
{
  JOB_SCHED_GRAPH_ADJ_LIST_ADAPTER_IX first_list_adapter_ix;

  first_list_adapter_ix = p_graph->AdapterTbl[job_ix].SuccrListHeadIx;
  if (first_list_adapter_ix == JOB_SCHED_GRAPH_ADJ_LIST_VOID_ADAPTER_IX) {
    return (JOB_SCHED_VOID_JOB_IX);
  }

  p_browse_state->CurListAdapterIx = first_list_adapter_ix;

  return (p_graph->AdjList.AdapterTbl[first_list_adapter_ix].JobIx);
}

/****************************************************************************************************//**
 *                                       JobSched_Graph_DepthFirstSearch()
 *
 * @brief    Search job with the state JOB_SCHED_GRAPH_DFS_STATE_CYCLE in the graph.
 *
 * @param    p_graph             Pointer to graph instance.
 *
 * @param    start_job_ix        Start index of job to search from.
 *
 * @param    trunc_cb            Pointer to truncate callback.
 *
 * @param    p_trunc_cb_data     Pointer to data specific to truncate callback.
 *
 * @param    mode                DFS mode:
 *                                   - JOB_SCHED_GRAPH_DFS_MODE_FWD
 *                                   - JOB_SCHED_GRAPH_DFS_MODE_BWD
 *                                   - JOB_SCHED_GRAPH_DFS_MODE_EXCLUDE_FIRST
 *
 * @param    p_err               Error pointer.
 *
 * @return   DEF_YES, if state is JOB_SCHED_GRAPH_DFS_STATE_CYCLE for the given job.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN JobSched_Graph_DepthFirstSearch(JOB_SCHED_GRAPH              *p_graph,
                                                   JOB_SCHED_JOB_IX             start_job_ix,
                                                   JOB_SCHED_GRAPH_DFS_TRUNC_CB trunc_cb,
                                                   void                         *p_trunc_cb_data,
                                                   CPU_INT08U                   mode,
                                                   RTOS_ERR                     *p_err)
{
  JOB_SCHED_JOB_IX             end_job_ix;
  JOB_SCHED_JOB_IX             cur_visited_job_ix;
  JOB_SCHED_GRAPH_BROWSE_STATE browse_state;
  JOB_SCHED_GRAPH_DFS_STATE    dfs_state;
  CPU_BOOLEAN                  path_end;
  CPU_BOOLEAN                  is_fwd;
  CPU_SIZE_T                   map_size;

  //                                                               -------------------- INIT LISTS --------------------
  IB_DList_EndInit(&p_graph->VisitedListEnd);
  IB_DList_EndInit(&p_graph->DiscoveredListEnd);

  //                                                               ------------------- INIT BITMAPS -------------------
  map_size = (p_graph->AdapterCnt - 1) / 8 + 1;
  Mem_Clr((void *)p_graph->VisitedJobMapPtr, map_size);
  Mem_Clr((void *)p_graph->DiscoveredJobMapPtr, map_size);

  //                                                               --------------- PROCESS FIRST VERTEX ---------------
  if (trunc_cb == DEF_NULL) {
    dfs_state = JOB_SCHED_GRAPH_DFS_STATE_CONTINUE;
  } else {
    dfs_state = trunc_cb(p_graph,
                         start_job_ix,
                         p_trunc_cb_data,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NO);
    }

    if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_DROP) {
      return (DEF_NO);
    } else if ((dfs_state == JOB_SCHED_GRAPH_DFS_STATE_TURN_BACK) || (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_STOP)) {
      Bitmap_BitSet(p_graph->DiscoveredJobMapPtr, start_job_ix);
      IB_DList_Push(&p_graph->DfsList, &p_graph->DiscoveredListEnd, start_job_ix);
      return (DEF_NO);
    } else if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_CYCLE) {
      return (DEF_YES);
    }
  }

  if (DEF_BIT_IS_CLR(mode, JOB_SCHED_GRAPH_DFS_MODE_EXCLUDE_FIRST)) {
    IB_DList_Push(&p_graph->DfsList, &p_graph->VisitedListEnd, start_job_ix);
    Bitmap_BitSet(p_graph->VisitedJobMapPtr, start_job_ix);
  }

  //                                                               --------------- PROCESS ALL VERTICES ---------------
  while ((cur_visited_job_ix = IB_DList_FirstGet(&p_graph->VisitedListEnd)) != JOB_SCHED_VOID_JOB_IX) {
    JOB_SCHED_JOB_IX adj_job_ix;

    path_end = DEF_YES;
    is_fwd = DEF_BIT_IS_SET(mode, JOB_SCHED_GRAPH_DFS_MODE_FWD);

    for (adj_job_ix = is_fwd ? JobSched_Graph_FirstSuccrGet(p_graph, cur_visited_job_ix, &browse_state)
                      : JobSched_Graph_FirstPredGet(p_graph, cur_visited_job_ix, &browse_state);
         adj_job_ix != JOB_SCHED_VOID_JOB_IX;
         adj_job_ix = JobSched_Graph_NextSiblingGet(p_graph, &browse_state)) {
      if (!Bitmap_BitIsSet(p_graph->VisitedJobMapPtr, adj_job_ix)) {
        if (trunc_cb == DEF_NULL) {
          dfs_state = JOB_SCHED_GRAPH_DFS_STATE_CONTINUE;
        } else {
          dfs_state = trunc_cb(p_graph,
                               adj_job_ix,
                               p_trunc_cb_data,
                               p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (DEF_NO);
          }
        }

        if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_CYCLE) {
          return (DEF_YES);
        }

        if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_CONTINUE) {
          path_end = DEF_NO;
          IB_DList_Push(&p_graph->DfsList, &p_graph->VisitedListEnd, adj_job_ix);
          Bitmap_BitSet(p_graph->VisitedJobMapPtr, adj_job_ix);
        } else if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_TURN_BACK) {
          IB_DList_Push(&p_graph->DfsList, &p_graph->DiscoveredListEnd, adj_job_ix);
        } else if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_DROP) {
          //                                                       Nothing to do.
        } else if (dfs_state == JOB_SCHED_GRAPH_DFS_STATE_STOP) {
          return (DEF_NO);
        } else {
          CPU_SW_EXCEPTION(DEF_NO);
        }
      } else {
        if (!Bitmap_BitIsSet(p_graph->DiscoveredJobMapPtr, adj_job_ix)) {
          path_end = DEF_NO;
          IB_DList_Rem(&p_graph->DfsList, &p_graph->VisitedListEnd, adj_job_ix);
          IB_DList_Push(&p_graph->DfsList, &p_graph->VisitedListEnd, adj_job_ix);
        }
      }
    }

    if (path_end) {
      end_job_ix = IB_DList_Pop(&p_graph->DfsList, &p_graph->VisitedListEnd);
      if (end_job_ix != JOB_SCHED_VOID_JOB_IX) {
        Bitmap_BitSet(p_graph->DiscoveredJobMapPtr, end_job_ix);
        IB_DList_Push(&p_graph->DfsList, &p_graph->DiscoveredListEnd, end_job_ix);
      }
    }
  }

  return (DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
