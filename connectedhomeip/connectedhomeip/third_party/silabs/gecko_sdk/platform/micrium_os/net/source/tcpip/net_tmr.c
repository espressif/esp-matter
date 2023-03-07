/***************************************************************************//**
 * @file
 * @brief Network Timer Management
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

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "net_priv.h"
#include  "net_tmr_priv.h"
#include  "net_task_priv.h"

#include  <net/source/tcpip/net_util_priv.h>
#include  <common/include/lib_utils.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (NET)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_NET

#ifndef NET_TMR_CFG_NBR_TMR
#define  NET_TMR_CFG_NBR_TMR                LIB_MEM_BLK_QTY_UNLIMITED
#endif

#define  NET_TMR_POOL_MIN_QTY               1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL NetTmr_Pool;

#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
static NET_STAT_POOL NetTmr_PoolStat;
#endif

static NET_TMR *NetTmr_QHead;                                   // Pointer to the head of the Timer Queue.
static NET_TMR *NetTmr_ListHead;                                // Pointer to head of Timer List.
static NET_TMR *NetTmr_ListTimedOutHead;                        // Pointer to head of Timed Out Timer List.

static CPU_INT32U NetTmr_LastTS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetTmr_ListAdd(NET_TMR *p_tmr);

static void NetTmr_ListRem(NET_TMR *p_tmr);

static void NetTmr_ListTimedOutAdd(NET_TMR *p_tmr);

static NET_TMR *NetTmr_ListTimedOutRemHead(void);

static void NetTmr_ListTimedOutRem(NET_TMR *p_tmr);

static void NetTmr_QAdd(NET_TMR *p_tmr);

static NET_TMR *NetTmr_QRemHead(void);

static void NetTmr_QRem(NET_TMR *p_tmr);

static CPU_INT32U NetTmr_TimeElapsedGet(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetTmr_PoolStatGet()
 *
 * @brief    Get network timer statistics pool.
 *
 * @return   Network timer statistics pool, if NO error(s).
 *           NULL          statistics pool, otherwise.
 *
 * @note     (1) NetTmr_PoolStatGet() blocked until network initialization completes; return NULL
 *               statistics pool.
 *
 * @note     (2) 'NetTmr_PoolStat' MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
NET_STAT_POOL NetTmr_PoolStatGet(void)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err.

#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  stat_pool = NetTmr_PoolStat;
  CORE_EXIT_ATOMIC();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                       NetTmr_PoolStatResetMaxUsed()
 *
 * @brief    Reset network timer statistics pool's maximum number of entries used.
 *
 * @note     (1) NetTmr_PoolStatResetMaxUsed() blocked until network initialization completes.
 *           - (a) However, since 'NetTmr_PoolStat' is reset when network initialization completes;
 *                   NO error is returned.
 *******************************************************************************************************/
void NetTmr_PoolStatResetMaxUsed(void)
{
#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  //                                                               Acquire net lock.
  Net_GlobalLockAcquire((void *)NetTmr_PoolStatResetMaxUsed);

  NetStat_PoolResetUsedMax(&NetTmr_PoolStat);                   // Reset net tmr stat pool.

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetTmr_Init()
 *
 * @brief    (1) Initialize Network Timer Management Module :
 *               - (a) Initialize timer pool
 *               - (b) Initialize timer stat pool
 *               - (c) Initialize timer task list pointer
 *
 * @param    p_mem_seg   Pointer to memory segment to use to allocate the Network Timers pool.
 *                       DEF_NULL to use the configured HEAP memory.
 *
 *
 * Argument(s) : p_mem_seg   Pointer to memory segment to use to allocate the Network Timers pool.
 *                           DEF_NULL to use the configured HEAP memory.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) The following network timer initialization MUST be sequenced as follows :
 *
 *                   (a) NetTmr_Init()      MUST precede ALL other network timer initialization functions.
 *******************************************************************************************************/
void NetTmr_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err)
{
  Mem_DynPoolCreate("Net Tmr Pool",
                    &NetTmr_Pool,
                    p_mem_seg,
                    sizeof(NET_TMR),
                    sizeof(CPU_SIZE_T),
                    NET_TMR_POOL_MIN_QTY,
                    NET_TMR_CFG_NBR_TMR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  //                                                               ------------------ INIT TMR POOL -------------------
  NetStat_PoolInit(&NetTmr_PoolStat,
                   NET_TMR_CFG_NBR_TMR);
#endif

  //                                                               -------------- INIT TMR TASK LIST PTR --------------
  NetTmr_QHead = DEF_NULL;
  NetTmr_ListHead = DEF_NULL;
  NetTmr_ListTimedOutHead = DEF_NULL;

  NetTmr_LastTS = 0;
}

/****************************************************************************************************//**
 *                                               NetTmr_Handler()
 *
 * @brief    (1) Handle network timers list:
 *           - (a) Decrement first network timer delta
 *           - (b) For any timer that expires :
 *               - (1) Execute timer's callback function
 *               - (2) Free from Timer List if not periodic, else re-put in list.
 *
 * @param    delta_ms    Time elapsed since last timer processing.
 *
 * @note     (2) Timers management:
 *               - (a) Timers are managed in a doubly-linked Timer List.
 *                   - (1) 'NetTmr_ListHead' points to the head of the Timer List.
 *                   - (2) Timers' 'PrevPtr' & 'NextPtr' doubly-link each timer to form the Timer List.
 *               - (b) The network timers list is sorted so that the Head of the list is the smallest
 *                    timeout. The timers are sorted with the help of deltas. Each timer as a delta value
 *                    relative to the timer preceding him in the list. When a new timer is added or when
 *                    a timer is set, the list is process to set the timer delta and to insert the timer
 *                    at its rightful place in the list.
 *
 * @note     (3) NetTmr_Handler() is called by the Network Task when the task is wake from a new received
 *               packet or from a new timer setting.
 *
 * @note     (4) When a one-shot network timer expires, the timer SHOULD be freed PRIOR to executing the timer
 *               callback function.  This ensures that at least one timer is available if the timer
 *               callback function requires a timer.
 *******************************************************************************************************/
void NetTmr_Handler(void)
{
  NET_TMR      *p_tmr = DEF_NULL;
  NET_TMR      *p_tmr_next = DEF_NULL;
  void         *obj = DEF_NULL;
  CPU_FNCT_PTR fnct = DEF_NULL;
  CPU_BOOLEAN  done = DEF_NO;
  CPU_INT32U   delta_ms = 0;

  delta_ms = NetTmr_TimeElapsedGet();

  //                                                               ------------- PROCESS THE TIMERS LIST --------------
  p_tmr = NetTmr_ListHead;
  while ((p_tmr != DEF_NULL)
         && (done != DEF_YES) ) {
    p_tmr_next = p_tmr->NextPtr;

    if (p_tmr->Delta > delta_ms) {                              // SMALLEST TIMEOUT IS BIGGER THAN TIME ELAPSED
      p_tmr->Delta -= delta_ms;                                 // Decrement first delta in the timer list.
      done = DEF_YES;
    } else if (p_tmr->Delta == delta_ms) {                      // SMALLEST TIMEOUT IS EQUAL TO TIME ELAPSED
      delta_ms -= p_tmr->Delta;                                 // Decrement time elapsed.
      p_tmr->Delta = 0;

      if (p_tmr_next != DEF_NULL) {                             // Check if other timers are at same timeout time.
        if (p_tmr_next->Delta != 0) {
          done = DEF_YES;
        }
      }

      NetTmr_ListRem(p_tmr);                                    // Remove timer from the list.
      NetTmr_ListTimedOutAdd(p_tmr);
    } else {                                                    // TIMEOUT IS LESS THAN TIME ELAPSED
      delta_ms -= p_tmr->Delta;                                 // Decrement time elapsed.
      p_tmr->Delta = 0;

      NetTmr_ListRem(p_tmr);                                    // Remove timer from the list.
      NetTmr_ListTimedOutAdd(p_tmr);
    }

    p_tmr = p_tmr_next;
  }

  //                                                               -------- ADD TIMERS IN Q TO THE TIMERS LIST --------
  while (NetTmr_QHead != DEF_NULL) {
    p_tmr = NetTmr_QRemHead();

    //                                                             Remove the time elapse since the timer creation and
    //                                                             the moment the timer is added to the list.
    if (p_tmr->TS_ms > NetTmr_LastTS) {
      delta_ms = (DEF_INT_32U_MAX_VAL - p_tmr->TS_ms) + NetTmr_LastTS;
    } else {
      delta_ms = NetTmr_LastTS - p_tmr->TS_ms;
    }

    if (p_tmr->Delta > delta_ms) {
      p_tmr->Delta -= delta_ms;

      NetTmr_ListAdd(p_tmr);
    } else {
      p_tmr->Delta = 0;

      NetTmr_ListTimedOutAdd(p_tmr);
    }
  }

  //                                                               ----------- PROCESS TIMERS THAT TIMED OUT ----------
  while (NetTmr_ListTimedOutHead != DEF_NULL) {
    p_tmr = NetTmr_ListTimedOutRemHead();

    obj = p_tmr->Obj;                                           // Get obj for ...
    fnct = p_tmr->Fnct;                                         // ... tmr callback fnct.

    if (p_tmr->Flags.IsPeriodic == DEF_YES) {
      p_tmr->Delta = p_tmr->Val;
      NetTmr_ListAdd(p_tmr);
    } else {
      NetTmr_Free(p_tmr);
    }

    if (fnct != DEF_NULL) {                                     // ... & if avail,             ...
      fnct(obj);                                                // ... exec tmr callback fnct.
    }
  }
}

/****************************************************************************************************//**
 *                                               NetTmr_Get()
 *
 * @brief    (1) Allocate & initialize a network timer :
 *               - (a) Get        timer
 *               - (b) Validate   timer
 *               - (c) Initialize timer
 *               - (d) Insert     timer at right place in the list
 *               - (e) Update timer pool statistics
 *               - (f) Return pointer to timer
 *                     OR
 *                     Null pointer & error code, on failure
 *
 * @param    fnct        Pointer to callback function to execute when timer expires (see Note #3).
 *
 * @param    obj         Pointer to object that requests a timer (MAY be NULL).
 *
 * @param    time_ms     Initial timer value (in milliseconds) [see Note #4].
 *
 * @param    opt_flags   Flags to select timer options; bit-field flags logically OR'd :
 *                           - NET_TMR_OPT_NONE        NO timer options selected. One-shot timer by default.
 *                           - NET_TMR_OPT_ONE_SHOT    One-shot timer specified.
 *                           - NET_TMR_OPT_PERIODIC    Periodic timer specified.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to network timer, if NO error(s).
 *           Pointer to NULL,          otherwise.
 *
 * @note     (2) Timer value of 0 ms allowed; next tick will expire timer.
 *******************************************************************************************************/
NET_TMR *NetTmr_Get(CPU_FNCT_PTR fnct,
                    void         *obj,
                    CPU_INT32U   time_ms,
                    NET_TMR_OPT  opt_flags,
                    RTOS_ERR     *p_err)
{
  NET_TMR     *p_tmr = DEF_NULL;
  CPU_BOOLEAN is_periodic = DEF_NO;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               --------------------- GET TMR ----------------------
  p_tmr = (NET_TMR *)Mem_DynPoolBlkGet(&NetTmr_Pool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.Tmr.NoneAvailCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  //                                                               --------------------- INIT TMR ---------------------
  p_tmr->Obj = obj;
  p_tmr->Fnct = fnct;
  p_tmr->Val = time_ms;                                         // Set tmr val (in ms).
  p_tmr->Delta = time_ms;
  p_tmr->NextPtr = DEF_NULL;
  p_tmr->PrevPtr = DEF_NULL;

  p_tmr->TS_ms = NetUtil_TS_Get_ms_Internal();

  is_periodic = DEF_BIT_IS_SET(opt_flags, NET_TMR_OPT_PERIODIC);
  p_tmr->Flags.IsPeriodic = (is_periodic == DEF_YES) ? DEF_YES : DEF_NO;

  //                                                               -------------- INSERT TMR INTO TMR Q ---------------
  NetTmr_QAdd(p_tmr);

#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  //                                                               --------------- UPDATE TMR POOL STATS --------------
  NetStat_PoolEntryUsedInc(&NetTmr_PoolStat, &local_err);
#endif

  PP_UNUSED_PARAM(local_err);

  //                                                               -- SIGNAL NETWORK TASK THAT A NEW TIMER WAS ADDED --
  NetTask_SignalPost();

exit:
  return (p_tmr);                                               // Return timer.
}

/****************************************************************************************************//**
 *                                               NetTmr_Free()
 *
 * @brief    (1) Free a network timer :
 *               - (a) Remove timer from Timer List
 *               - (b) Clear  timer controls
 *               - (c) Free   timer back to timer pool
 *               - (d) Update timer pool statistics
 *
 * @param    p_tmr   Pointer to a network timer.
 *******************************************************************************************************/
void NetTmr_Free(NET_TMR *p_tmr)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------ VALIDATE PTR --------------------
  if (p_tmr == DEF_NULL) {
    return;
  }

  //                                                               -------------- REMOVE TIMER FROM LIST --------------
  NetTmr_ListRem(p_tmr);
  NetTmr_QRem(p_tmr);
  NetTmr_ListTimedOutRem(p_tmr);

  //                                                               --------------------- FREE TMR ---------------------
  Mem_DynPoolBlkFree(&NetTmr_Pool, p_tmr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_TMR_EN == DEF_ENABLED)
  //                                                               -------------- UPDATE TMR POOL STATS ---------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedDec(&NetTmr_PoolStat, &local_err);
#endif

  PP_UNUSED_PARAM(local_err);
}

/****************************************************************************************************//**
 *                                               NetTmr_Set()
 *
 * @brief    Update a network timer with a new callback function & timer value.
 *
 * @param    p_tmr       Pointer to a network timer.
 *
 * @param    fnct        Pointer to callback function to execute when timer expires (see Note #2).
 *
 * @param    time_ms     Update timer value (in milliseconds).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Assumes network timer is ALREADY owned by a valid network object.
 *
 * @note     (2) Timer value of 0 ticks/seconds allowed; next tick will expire timer.
 *******************************************************************************************************/
void NetTmr_Set(NET_TMR      *p_tmr,
                CPU_FNCT_PTR fnct,
                CPU_INT32U   time_ms)
{
  //                                                               -------------- REMOVE TIMER FROM LIST --------------
  NetTmr_ListRem(p_tmr);
  NetTmr_QRem(p_tmr);
  NetTmr_ListTimedOutRem(p_tmr);

  //                                                               --------------------- SET TIMER --------------------
  p_tmr->Fnct = fnct;
  p_tmr->Val = time_ms;
  p_tmr->Delta = time_ms;

  p_tmr->TS_ms = NetUtil_TS_Get_ms_Internal();

  //                                                               --------------- RE-ADD TIMER TO THE Q --------------
  NetTmr_QAdd(p_tmr);

  //                                                               ----- SIGNAL NETWORK TASK THAT A TIMER WAS SET -----
  NetTask_SignalPost();
}

/****************************************************************************************************//**
 *                                       NetTmr_ListMinTimeoutGet()
 *
 * @brief    Retrieve the minimum timeout value in the Network timers list.
 *
 * @return   Value of the minimum timeout.
 *           DEF_INT_32U_MAX_VAL  if no timeout in the list.
 *******************************************************************************************************/
CPU_INT32U NetTmr_ListMinTimeoutGet(void)
{
  CPU_INT32U timeout_ms = DEF_INT_32U_MAX_VAL;

  if (NetTmr_ListHead != DEF_NULL) {
    CPU_INT32U timeout_max_ms = 0;

    timeout_ms = NetTmr_ListHead->Delta;
    timeout_max_ms = NetUtil_TS_GetMaxDly_ms();

    timeout_ms = DEF_MIN(timeout_ms, timeout_max_ms);
  }

  return (timeout_ms);
}

/****************************************************************************************************//**
 *                                           NetTmr_TimeRemainGet()
 *
 * @brief    Get the remain time for a given timer.
 *
 * @param    p_tmr   Pointer to object timer to retrieve remaining time.
 *
 * @return   Time in milliseconds remaining for this timer.
 *           DEF_INT_32U_MAX_VAL if timer not found or invalid.
 *
 * @note     (1) Since the timer handler is not called periodically, but only when an event
 *               occurs (packet received, new timer, smallest timer timed out) the time remaining
 *               cannot be calculated precisely.
 *               Therefore the value return is the best effort of calculation for the time remaining
 *               that can be perform with this timer handling scheme.
 *******************************************************************************************************/
CPU_INT32U NetTmr_TimeRemainGet(NET_TMR *p_tmr)
{
  NET_TMR     *p_item = DEF_NULL;
  CPU_INT32U  time = 0;
  CPU_BOOLEAN found = DEF_NO;

  if (p_tmr == DEF_NULL) {
    return (DEF_INT_32U_MAX_VAL);
  }

  p_item = NetTmr_ListHead;
  while ((p_item != DEF_NULL)
         && (found != DEF_YES) ) {
    time += p_item->Delta;

    if (p_item == p_tmr) {
      found = DEF_YES;
    }

    p_item = p_item->NextPtr;
  }

  if (found != DEF_YES) {
    return (DEF_INT_32U_MAX_VAL);
  }

  return (time);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetTmr_ListAdd()
 *
 * @brief    Add a timer to the sorted network timer list.
 *
 * @param    p_tmr   Pointer to the network timer to add.
 *
 * @note     (1) Timers management:
 *
 *                       - (a) Timers are managed in a doubly-linked Timer List.
 *                       - (1) 'NetTmr_ListHead' points to the head of the Timer List.
 *                       - (2) Timers' 'PrevPtr' & 'NextPtr' doubly-link each timer to form the Timer List.
 *
 *                       - (b) The network timers list is sorted so that the Head of the list is the smallest
 *                   timeout. The timers are sorted with the help of deltas. Each timer as a delta value
 *                   relative to the timer preceding him in the list. When a new timer is added or when
 *                   a timer is set, the list is process to set the timer delta and to insert the timer
 *                   at its rightful place in the list.
 *******************************************************************************************************/
static void NetTmr_ListAdd(NET_TMR *p_tmr)
{
  NET_TMR *p_item;

  if (NetTmr_ListHead != DEF_NULL) {                            // TIMER LIST NOT EMPTY
    p_item = NetTmr_ListHead;
    while (p_item != DEF_NULL) {
      if (p_tmr->Delta >= p_item->Delta) {                      // New timer's Delta is >= than current one in list.
        p_tmr->Delta -= p_item->Delta;                          // ... Decrement new timer's delta and ...

        if (p_item->NextPtr != DEF_NULL) {                      // .. go to next timer in list if any ...
          p_item = p_item->NextPtr;
        } else {
          p_tmr->PrevPtr = p_item;                              // ... or if no more timer, add new timer at the end.
          p_tmr->NextPtr = p_item->NextPtr;
          p_item->NextPtr = p_tmr;

          if (p_tmr->NextPtr != DEF_NULL) {
            p_tmr->NextPtr->PrevPtr = p_tmr;
          }
          break;
        }
      } else {                                                  // New timer's Delta is < than current one in list.
        p_item->Delta -= p_tmr->Delta;                          // ... Decrement the current timer delta in the list...

        p_tmr->NextPtr = p_item;                                // ... and add the timer before the current one.
        p_tmr->PrevPtr = p_item->PrevPtr;
        p_item->PrevPtr = p_tmr;

        if (p_tmr->PrevPtr != DEF_NULL) {
          p_tmr->PrevPtr->NextPtr = p_tmr;
        } else {
          NetTmr_ListHead = p_tmr;
        }
        break;
      }
    }
  } else {                                                      // NO TIMER IN THE LIST
    NetTmr_ListHead = p_tmr;                                    // Add new timer at head of list.
    p_tmr->PrevPtr = DEF_NULL;
    p_tmr->NextPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                               NetTmr_ListRem()
 *
 * @brief    Remove a specific network timer from the Network Timer list.
 *
 * @param    p_tmr   Pointer to the network timer to remove.
 *
 * @note     (1) Timers management:
 *
 *                       - (a) Timers are managed in a doubly-linked Timer List.
 *                       - (1) 'NetTmr_ListHead' points to the head of the Timer List.
 *                       - (2) Timers' 'PrevPtr' & 'NextPtr' doubly-link each timer to form the Timer List.
 *
 *                       - (b) The network timers list is sorted so that the Head of the list is the smallest
 *                   timeout. The timers are sorted with the help of deltas. Each timer as a delta value
 *                   relative to the timer preceding him in the list. When a new timer is added or when
 *                   a timer is set, the list is process to set the timer delta and to insert the timer
 *                   at its rightful place in the list.
 *******************************************************************************************************/
static void NetTmr_ListRem(NET_TMR *p_tmr)
{
  NET_TMR *p_prev;
  NET_TMR *p_next;

  p_prev = p_tmr->PrevPtr;
  p_next = p_tmr->NextPtr;

  if (p_tmr == NetTmr_ListHead) {
    NetTmr_ListHead = p_next;
  } else {
    if (p_prev != DEF_NULL) {
      p_prev->NextPtr = p_next;
    }
  }

  if (p_next != DEF_NULL) {
    p_next->PrevPtr = p_prev;
    p_next->Delta += p_tmr->Delta;
  }

  p_tmr->PrevPtr = DEF_NULL;
  p_tmr->NextPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           NetTmr_ListTimedOutAdd()
 *
 * @brief    Add a timer to the Timed Out timer list.
 *           The new timer is added at the head of the list.
 *
 * @param    p_tmr   Pointer to timer object to add to the Timed Out list.
 *
 * @note     (1) The Timed Out timer list is used to store the timer that timed out and that need to
 *                   be process.
 *******************************************************************************************************/
static void NetTmr_ListTimedOutAdd(NET_TMR *p_tmr)
{
  p_tmr->NextPtr = NetTmr_ListTimedOutHead;
  NetTmr_ListTimedOutHead = p_tmr;
}

/****************************************************************************************************//**
 *                                       NetTmr_ListTimedOutRemHead()
 *
 * @brief    Remove the head timer of the Timed out timer list.
 *
 * @return   Pointer to timer that was removed.
 *
 * @note     (1) The Timed Out timer list is used to store the timer that timed out and that need to
 *               be process.
 *******************************************************************************************************/
static NET_TMR *NetTmr_ListTimedOutRemHead(void)
{
  NET_TMR *p_tmr;

  p_tmr = NetTmr_ListTimedOutHead;

  if (p_tmr == DEF_NULL) {
    return (DEF_NULL);
  }

  NetTmr_ListTimedOutHead = p_tmr->NextPtr;

  p_tmr->NextPtr = DEF_NULL;

  return (p_tmr);
}

/****************************************************************************************************//**
 *                                           NetTmr_ListTimedOutRem()
 *
 * @brief    Remove a timer from the timers Timed-Out list.
 *
 * @param    p_tmr   Pointer to timer object to remove from the timers Timed-Out list.
 *******************************************************************************************************/
static void NetTmr_ListTimedOutRem(NET_TMR *p_tmr)
{
  NET_TMR     *p_tmr_item = DEF_NULL;
  NET_TMR     *p_tmr_prev = DEF_NULL;
  CPU_BOOLEAN is_found = DEF_NO;

  if (p_tmr == DEF_NULL) {
    return;
  }

  p_tmr_item = NetTmr_ListTimedOutHead;

  while (p_tmr_item != DEF_NULL) {
    if (p_tmr_item == p_tmr) {
      is_found = DEF_YES;
      break;
    }
    p_tmr_prev = p_tmr_item;
    p_tmr_item = p_tmr_item->NextPtr;
  }

  if (is_found == DEF_YES) {
    if (p_tmr_prev != DEF_NULL) {
      p_tmr_prev->NextPtr = p_tmr->NextPtr;
    } else {
      NetTmr_ListTimedOutHead = p_tmr->NextPtr;
    }

    p_tmr->NextPtr = DEF_NULL;
    p_tmr->PrevPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                               NetTmr_QAdd()
 *
 * @brief    Add a timer to the timer queue.
 *           The new timer is added at the head of the Q.
 *
 * @param    p_tmr   Pointer to timer object to add to the timers Q.
 *
 * @note     (1) The timer Q is used to store the timer that need to be added to the timer list.
 *******************************************************************************************************/
static void NetTmr_QAdd(NET_TMR *p_tmr)
{
  NET_TMR     *p_tmr_item = DEF_NULL;
  CPU_BOOLEAN is_found = DEF_NO;

  p_tmr_item = NetTmr_QHead;
  while (p_tmr_item != DEF_NULL) {
    if (p_tmr_item == p_tmr) {
      is_found = DEF_YES;
      break;
    }
    p_tmr_item = p_tmr_item->NextPtr;
  }

  if (is_found != DEF_YES) {
    p_tmr->NextPtr = NetTmr_QHead;
    NetTmr_QHead = p_tmr;
  }
}

/****************************************************************************************************//**
 *                                               NetTmr_QRemHead()
 *
 * @brief    Remove the head timer of the timers Q.
 *
 * @return   Pointer to timer that was removed.
 *
 * @note     (1) The timer Q is used to store the timer that need to be added to the timer list.
 *******************************************************************************************************/
static NET_TMR *NetTmr_QRemHead(void)
{
  NET_TMR *p_tmr;

  p_tmr = NetTmr_QHead;

  if (p_tmr == DEF_NULL) {
    return (DEF_NULL);
  }

  NetTmr_QHead = p_tmr->NextPtr;

  p_tmr->NextPtr = DEF_NULL;
  p_tmr->PrevPtr = DEF_NULL;

  return (p_tmr);
}

/****************************************************************************************************//**
 *                                               NetTmr_QRem()
 *
 * @brief    Remove a timer from the timers Q.
 *
 * @param    p_tmr   Pointer to timer object to remove from the timers Q.
 *******************************************************************************************************/
static void NetTmr_QRem(NET_TMR *p_tmr)
{
  NET_TMR     *p_tmr_item = DEF_NULL;
  NET_TMR     *p_tmr_prev = DEF_NULL;
  CPU_BOOLEAN is_found = DEF_NO;

  if (p_tmr == DEF_NULL) {
    return;
  }

  p_tmr_item = NetTmr_QHead;

  while (p_tmr_item != DEF_NULL) {
    if (p_tmr_item == p_tmr) {
      is_found = DEF_YES;
      break;
    }
    p_tmr_prev = p_tmr_item;
    p_tmr_item = p_tmr_item->NextPtr;
  }

  if (is_found == DEF_YES) {
    if (p_tmr_prev != DEF_NULL) {
      p_tmr_prev->NextPtr = p_tmr->NextPtr;
    } else {
      NetTmr_QHead = p_tmr->NextPtr;
    }

    p_tmr->NextPtr = DEF_NULL;
    p_tmr->PrevPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                           NetTask_TimeElapsedGet()
 *
 * @brief    Get the time elapsed since the last time this function was called.
 *
 * @return   time elapsed in milliseconds.
 *******************************************************************************************************/
static CPU_INT32U NetTmr_TimeElapsedGet(void)
{
  CPU_INT32U ts_cur_ms;
  CPU_INT32U delta;

  ts_cur_ms = NetUtil_TS_Get_ms_Internal();

  if (NetTmr_LastTS > ts_cur_ms) {
    delta = (DEF_INT_32U_MAX_VAL - NetTmr_LastTS) + ts_cur_ms;
  } else {
    delta = ts_cur_ms - NetTmr_LastTS;
  }

  NetTmr_LastTS = ts_cur_ms;

  return (delta);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
