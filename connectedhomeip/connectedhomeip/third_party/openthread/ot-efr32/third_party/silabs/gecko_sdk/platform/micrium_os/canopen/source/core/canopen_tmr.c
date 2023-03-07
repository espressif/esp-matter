/***************************************************************************//**
 * @file
 * @brief CANopen Timer Management
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

/****************************************************************************************************//**
 * @note     (1) This file implements a high-speed timer management. The time events are stored in a
 *               linked list with delta-times between the events. Each timer event holds the root of
 *               another linked list, which contains the necessary actions for this event.
 *               See the following figure for an overview:
 *               @verbatim
 *                     FreeTmr -> T4 -> T5 -> T6 -> T7 -> 0
 *                     FreeAct -> A5 -> A6 -> A7 -> 0
 *
 *                                |--dt--|
 *                     UsedTmr -> T1 -> T3 -> T2 -> T0 -> 0
 *                                ||    ||    ||    |+-> A0 -> 0
 *                                ||    ||    ||    +----^
 *                                ||    ||    |+-------> A3 -> 0
 *                                ||    ||    +----------^
 *                                ||    |+-------------> A4 -> 0
 *                                ||    +----------------^
 *                                |+-------------------> A1 -> A2 -> 0
 *                                +----------------------------^
 *               @endverbatim
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_tmr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN CANopen_TmrInsert(CANOPEN_TMR        *p_tmr,
                                     CPU_INT32U         delta_new,
                                     CANOPEN_TMR_ACTION *p_act);

static void CANopen_TmrRemove(CANOPEN_TMR      *p_tmr,
                              CANOPEN_TMR_TIME *p_tmr_info_rem);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CANopen_TmrCreate()
 *
 * @brief   Creates the defined action and links this action into the event timer list at the correct
 *          timing.
 *
 * @param   p_tmr       Pointer to timer structure.
 *
 * @param   start_time  Delta time in ticks for the first timer event.
 *
 * @param   cycle_time  If this argument is different from 0, the delta time in ticks for the cyclic timer
 *                      events. If it is equal to 0, the timer will be a one-shot timer event.
 *
 * @param   fnct        Pointer to the action callback function.
 *
 * @param   p_arg       Pointer to the callback function parameter.
 *
 * @param   p_err       Error pointer.
 *
 * @return  The action identifier, if successful.
 *          < 0,                   if unsuccessful.
 *******************************************************************************************************/
CPU_INT16S CANopen_TmrCreate(CANOPEN_TMR      *p_tmr,
                             CPU_INT32U       start_time,
                             CPU_INT32U       cycle_time,
                             CANOPEN_TMR_FNCT fnct,
                             void             *p_arg,
                             RTOS_ERR         *p_err)
{
  CANOPEN_TMR_ACTION *p_act;
  CPU_BOOLEAN        result;
  CPU_INT16S         ret_val;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, CANOPEN_TMR_INVALID_ID);

  RTOS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, CANOPEN_TMR_INVALID_ID);

  RTOS_ASSERT_DBG_ERR_SET((p_tmr->NodePtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, CANOPEN_TMR_INVALID_ID);

  RTOS_ASSERT_DBG_ERR_SET((fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, CANOPEN_TMR_INVALID_ID);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (start_time == 0u) {                                       // see, if starttime is 0 (zero)
    start_time = cycle_time;                                    // last chance: set starttime to cycletime
  }

  RTOS_ASSERT_DBG_ERR_SET(((start_time != 0u) || (cycle_time != 0u)), *p_err, RTOS_ERR_INVALID_ARG, CANOPEN_TMR_INVALID_ID);

  RTOS_ASSERT_DBG_ERR_SET((p_tmr->ActionFreeListHeadPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, CANOPEN_TMR_INVALID_ID);

  CORE_ENTER_ATOMIC();

  p_act = p_tmr->ActionFreeListHeadPtr;                         // fetch first action from action pool list
  p_tmr->ActionFreeListHeadPtr = p_act->NextActionPtr;          // set first action to next action in list
  p_act->NextActionPtr = DEF_NULL;
  p_act->Fnct = fnct;
  p_act->ParamPtr = p_arg;
  p_act->CycleTime = cycle_time;

  result = CANopen_TmrInsert(p_tmr, start_time, p_act);         // insert event timer with created action
  if (result != DEF_OK) {
    p_act->CycleTime = 0u;
    p_act->ParamPtr = DEF_NULL;
    p_act->Fnct = DEF_NULL;
    p_act->NextActionPtr = p_tmr->ActionFreeListHeadPtr;        // put back the action into action pool
    p_tmr->ActionFreeListHeadPtr = p_act;
    ret_val = CANOPEN_TMR_INVALID_ID;                           // indicate function error to caller
    RTOS_ERR_SET(*p_err, RTOS_ERR_NO_MORE_RSRC);
  } else {
    ret_val = (CPU_INT16S)(p_act->Id);
  }

  CORE_EXIT_ATOMIC();

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           CANopen_TmrDel()
 *
 * @brief    Deletes the defined action and removes the timer, if this was the last action for this
 *           timer event.
 *
 * @param    p_tmr       Pointer to timer structure.
 *
 * @param    act_id      Action identifier, returned by CANopen_TmrCreate().
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void CANopen_TmrDel(CANOPEN_TMR *p_tmr,
                    CPU_INT16S  act_id,
                    RTOS_ERR    *p_err)
{
  CANOPEN_TMR_TIME   *p_timer_event;
  CANOPEN_TMR_ACTION *p_cur_act;
  CANOPEN_TMR_ACTION *p_prev_act;
  CANOPEN_TMR_ACTION *p_del_act = DEF_NULL;
  CPU_BOOLEAN        result = DEF_FAIL;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(( (act_id >= 0) && (act_id < (CPU_INT16S)(p_tmr->MaxTmrQty)) ), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();

  p_timer_event = p_tmr->TmrUsedListHeadPtr;                      // set timer pointer to root of timer list
  while ((p_timer_event != DEF_NULL)
         && (p_del_act == DEF_NULL)) {                            // loop through timer list until act found
    p_cur_act = p_timer_event->ActionPtr;                         // get root of action list of timer event

    if (p_cur_act->Id == (CPU_INT16U)act_id) {                    // --- FIRST ACTION IN ACTION LIST ---
      p_del_act = p_cur_act;                                      // remember action info for deletion
      p_timer_event->ActionPtr = p_cur_act->NextActionPtr;        // remove action from action list
    } else {                                                      // otherwise: first action does not match
      p_prev_act = p_cur_act;                                     // set previous action info
      p_cur_act = p_cur_act->NextActionPtr;                       // set next action info

      while ((p_cur_act != DEF_NULL)
             && (p_del_act == DEF_NULL)) {                        // until end of list OR del action found
        if (p_cur_act->Id == (CPU_INT16U)act_id) {                // see, if action id matches given id
          p_del_act = p_cur_act;                                  // remember action info for deletion
          p_prev_act->NextActionPtr = p_cur_act->NextActionPtr;   // remove action from list
          if (p_cur_act->NextActionPtr == DEF_NULL) {             // --- LAST ACTION IN ACTION LIST ---
            p_timer_event->ActionEndPtr = p_prev_act;             // update pointer to end of action list
          }
        }
        p_prev_act = p_cur_act;                                   // update previous action in action list
        p_cur_act = p_cur_act->NextActionPtr;                     // switch to next action in action list
      }
    }

    if (p_del_act == DEF_NULL) {                                  // see, if action is not found
      p_timer_event = p_timer_event->NextPtr;                     // switch to next timer event in timer list
    }
  }

  if (p_del_act == DEF_NULL) {                                    // see, if action is not found in used list
    p_timer_event = p_tmr->TmrElapsedListHeadPtr;                 // set timer pointer to root of elapsed list
    while ((p_timer_event != DEF_NULL)
           && (p_del_act == DEF_NULL)) {                          // loop through timer list until act found
      p_cur_act = p_timer_event->ActionPtr;                       // get root of action list of timer event

      if (p_cur_act->Id == (CPU_INT16U)act_id) {                  // --- FIRST ACTION IN ACTION LIST ---
        p_del_act = p_cur_act;                                    // remember action info for deletion
        p_timer_event->ActionPtr = p_cur_act->NextActionPtr;      // remove action from action list
      } else {                                                    // otherwise: first action does not match
        p_prev_act = p_cur_act;                                   // set previous action info
        p_cur_act = p_cur_act->NextActionPtr;                     // set next action info

        while ((p_cur_act != DEF_NULL)
               && (p_del_act == DEF_NULL)) {                      // until end of list OR del action found
          if (p_cur_act->Id == (CPU_INT16U)act_id) {              // see, if action id matches given id
            p_del_act = p_cur_act;                                // remember action info for deletion
            p_prev_act->NextActionPtr = p_cur_act->NextActionPtr; // remove action from list
                                                                  // --- LAST ACTION IN ACTION LIST ---
            if (p_cur_act->NextActionPtr == DEF_NULL) {
              //                                                   update pointer to end of action list
              p_timer_event->ActionEndPtr = p_prev_act;
            }
          }

          p_prev_act = p_cur_act;                               // update previous action in action list
          p_cur_act = p_cur_act->NextActionPtr;                 // switch to next action in action list
        }
      }

      if (p_del_act == DEF_NULL) {                              // see, if action is not found
        p_timer_event = p_timer_event->NextPtr;                 // switch to next timer event in timer list
      }
    }
  }

  if (p_del_act != DEF_NULL) {                                  // see, if delete action is found
    p_del_act->CycleTime = 0u;                                  // clear cycletime of deleted action
    p_del_act->ParamPtr = DEF_NULL;                             // clear callback function parameter
    p_del_act->Fnct = DEF_NULL;                                 // clear pointer to callback function
    p_del_act->NextActionPtr = p_tmr->ActionFreeListHeadPtr;    // link freed action in front of
    p_tmr->ActionFreeListHeadPtr = p_del_act;                   // free action list

    if (p_timer_event != DEF_NULL) {                            // see, if timer is valid
      if (p_timer_event->ActionPtr == DEF_NULL) {               // see, if action was last action for timer
        p_timer_event->ActionEndPtr = DEF_NULL;                 // clear action list end pointer
        CANopen_TmrRemove(p_tmr, p_timer_event);                // yes: remove timer from used timer list
      }
      result = DEF_OK;                                          // indicate successful operation
    }
  }

  CORE_EXIT_ATOMIC();

  if (result != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
  }
}

/****************************************************************************************************//**
 *                                          CANopen_TmrServ()
 *
 * @brief    Checks for the timer event to be elapsed, and removes it from the used timer event list.
 *
 * @param    p_tmr   Pointer to timer structure.
 *
 * @param    p_err   Error pointer.
 *
 * @return   DEF_YES, if the timer has elapsed,
 *           DEF_NO,  if the timer has not elapsed.
 *******************************************************************************************************/
CPU_BOOLEAN CANopen_TmrServ(CANOPEN_TMR *p_tmr,
                            RTOS_ERR    *p_err)
{
  CANOPEN_TMR_TIME *p_elapsed_time_event;
  CPU_BOOLEAN      result = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, result);

  RTOS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, result);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  if (p_tmr->TimeRelative > 0u) {                               // see, if a timer event delay is present
    p_tmr->TimeRelative--;
    if (p_tmr->TimeRelative == 0u) {                            // see, if event is elapsed
      p_elapsed_time_event = p_tmr->TmrUsedListHeadPtr;         // get pointer to timer event data
                                                                // and remove this event from event list
      p_tmr->TmrUsedListHeadPtr = p_elapsed_time_event->NextPtr;
      p_elapsed_time_event->NextPtr = DEF_NULL;
      if (p_tmr->TmrUsedListHeadPtr != DEF_NULL) {              // see, if further timer events exists
                                                                // yes: reload event delay counter
        p_tmr->TimeRelative = p_tmr->TmrUsedListHeadPtr->Delta;
        p_tmr->TimeAbsolute += p_tmr->TmrUsedListHeadPtr->Delta;
      } else {                                                  // otherwise: no further event in list
        p_tmr->TimeRelative = 0u;                               // stop timer
        p_tmr->TimeAbsolute = 0u;
      }
      if (p_tmr->TmrElapsedListHeadPtr == DEF_NULL) {           // see, if elapsed timer list is empty
                                                                // yes: set timer in elapsed list front
        p_tmr->TmrElapsedListHeadPtr = p_elapsed_time_event;
      } else {                                                  // otherwise: elapsed list is not empty
                                                                // place elapsed timer in front of list
        p_elapsed_time_event->NextPtr = p_tmr->TmrElapsedListHeadPtr;
        p_tmr->TmrElapsedListHeadPtr = p_elapsed_time_event;
      }
      result = DEF_YES;                                         // indicate elapsed timer
    }
  }
  CORE_EXIT_ATOMIC();

  return (result);
}

/****************************************************************************************************//**
 *                                         CANopen_TmrProcess()
 *
 * @brief    Processes elapsed timer actions.
 *
 * @param    p_tmr   Pointer to timer structure.
 *
 * @note     (1) This function handles all actions after an event timer is elapsed. The function
 *               removes the timer information from the used timer event list and performs all actions of
 *               the linked action list.
 *******************************************************************************************************/
void CANopen_TmrProcess(CANOPEN_TMR *p_tmr)
{
  CANOPEN_TMR_TIME   *p_tmr_info;
  CANOPEN_TMR_ACTION *p_cur_act;
  CANOPEN_TMR_ACTION *p_next_act;
  CANOPEN_TMR_FNCT   fnct;
  void               *p_arg;
  CPU_BOOLEAN        result;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG((p_tmr != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  while (p_tmr->TmrElapsedListHeadPtr != DEF_NULL) {            // loop through all elapsed timer
    CORE_ENTER_ATOMIC();
    p_tmr_info = p_tmr->TmrElapsedListHeadPtr;
    p_tmr->TmrElapsedListHeadPtr = p_tmr_info->NextPtr;         // update elapsed timer list

    p_cur_act = p_tmr_info->ActionPtr;                          // get action list out of timer event
    p_tmr_info->ActionPtr = DEF_NULL;
    p_tmr_info->ActionEndPtr = DEF_NULL;
    p_tmr_info->Delta = 0u;
    p_tmr_info->NextPtr = p_tmr->TmrFreeListHeadPtr;            // link free timer event in front
    p_tmr->TmrFreeListHeadPtr = p_tmr_info;                     // of free timer event list
    CORE_EXIT_ATOMIC();

    while (p_cur_act != DEF_NULL) {                             // traverse through linked action list
      p_next_act = p_cur_act->NextActionPtr;                    // hold pointer to next action
      p_cur_act->NextActionPtr = DEF_NULL;
      fnct = p_cur_act->Fnct;
      p_arg = p_cur_act->ParamPtr;

      if (p_cur_act->CycleTime == 0u) {                         // see, if timer event is a single event
        p_cur_act->ParamPtr = DEF_NULL;
        p_cur_act->Fnct = DEF_NULL;
        CORE_ENTER_ATOMIC();
        //                                                         link free action struct in front of free action list
        p_cur_act->NextActionPtr = p_tmr->ActionFreeListHeadPtr;
        p_tmr->ActionFreeListHeadPtr = p_cur_act;
        CORE_EXIT_ATOMIC();
      } else {                                                  // otherwise: cyclic timer event
        CORE_ENTER_ATOMIC();
        //                                                         insert new timer event
        result = CANopen_TmrInsert(p_tmr, p_cur_act->CycleTime, p_cur_act);
        CORE_EXIT_ATOMIC();
        if (result != DEF_OK) {
          RTOS_ASSERT_CRITICAL((result == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
      }
      fnct(p_arg);

      p_cur_act = p_next_act;                                   // switch to next action in list
    }
  }
}

/****************************************************************************************************//**
 *                                           CANopen_TmrInit()
 *
 * @brief    Initializes the timer data structure.
 *
 * @param    p_tmr       Pointer to timer structure.
 *
 * @param    p_node      Pointer to parent node.
 *
 * @param    p_tmr_mem   Pointer to memory block array.
 *
 * @param    tmr_qty     Number of timers.
 *******************************************************************************************************/
void CANopen_TmrInit(CANOPEN_TMR     *p_tmr,
                     CANOPEN_NODE    *p_node,
                     CANOPEN_TMR_MEM *p_tmr_mem,
                     CPU_INT16U      tmr_qty)
{
  p_tmr->NodePtr = p_node;                                      // store parent node information
  p_tmr->MaxTmrQty = tmr_qty;                                   // set number of elements in act/tmr-lists
  p_tmr->TmrPoolPtr = &p_tmr_mem->Tmr;                          // set root of timer pool
  p_tmr->ActionPoolPtr = &p_tmr_mem->Action;                    // set root of action pool

  CANopen_TmrReset(p_tmr);                                      // reset timers (create linked list)
}

/****************************************************************************************************//**
 *                                          CANopen_TmrReset()
 *
 * @brief    Resets a timer.
 *
 * @param    p_tmr   Pointer to timer structure.
 *
 * @note     (1) This function creates the linked lists of the unused action information and event time
 *               structures.
 *******************************************************************************************************/
void CANopen_TmrReset(CANOPEN_TMR *p_tmr)
{
  CANOPEN_TMR_MEM    *p_mem = (CANOPEN_TMR_MEM *)p_tmr->ActionPoolPtr;
  CANOPEN_TMR_ACTION *p_act_array = p_tmr->ActionPoolPtr;
  CANOPEN_TMR_TIME   *p_tmr_array = p_tmr->TmrPoolPtr;
  CPU_INT16U         id = 0u;
  CPU_INT16U         blk_ix;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_tmr->TimeRelative = 0u;
  p_tmr->TmrUsedListHeadPtr = DEF_NULL;
  p_tmr->TmrElapsedListHeadPtr = DEF_NULL;
  p_tmr->TmrFreeListHeadPtr = p_tmr->TmrPoolPtr;                // set root of free timer list
  p_tmr->ActionFreeListHeadPtr = p_tmr->ActionPoolPtr;          // set root of free action list

  for (blk_ix = 1; blk_ix <= p_tmr->MaxTmrQty; blk_ix++) {
    if (blk_ix < p_tmr->MaxTmrQty) {
      p_act_array->NextActionPtr = &p_mem[blk_ix].Action;       // connect action to next action
      p_tmr_array->NextPtr = &p_mem[blk_ix].Tmr;                // connect time to next time
    } else {                                                    // otherwise: last element in list
      p_act_array->NextActionPtr = DEF_NULL;
      p_tmr_array->NextPtr = DEF_NULL;
    }
    p_act_array->Id = id;                                       // set unique action identifier
    p_act_array->Fnct = DEF_NULL;
    p_act_array->ParamPtr = DEF_NULL;
    p_act_array->CycleTime = 0u;
    p_tmr_array->Delta = 0u;
    p_tmr_array->ActionPtr = DEF_NULL;
    p_tmr_array->ActionEndPtr = DEF_NULL;
    p_act_array = p_act_array->NextActionPtr;                   // switch to next action info
    p_tmr_array = p_tmr_array->NextPtr;                         // switch to next time info
    id++;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           CANopen_TmrClr()
 *
 * @brief    Clears a timer.
 *
 * @param    p_tmr   Pointer to timer structure.
 *
 * @note     (1) This function deletes all timers, which are created within the CANopen stack. The timers
 *               created by the application will still be active after this function call.
 *******************************************************************************************************/
void CANopen_TmrClr(CANOPEN_TMR *p_tmr)
{
  CANOPEN_NODE *p_node;
  RTOS_ERR     local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_node = p_tmr->NodePtr;

  CORE_ENTER_ATOMIC();
  if (p_node->Nmt.TmrId >= 0) {
    CANopen_TmrDel(p_tmr,
                   p_node->Nmt.TmrId,
                   &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  p_node->Nmt.TmrId = CANOPEN_TMR_INVALID_ID;                   // indicate no timer for heartbeat
  CORE_EXIT_ATOMIC();

#if (CANOPEN_TPDO_MAX_QTY > 0)
  {
    CANOPEN_TPDO *p_pdo;
    CPU_INT16U   num;

    for (num = 0u; num < CANOPEN_TPDO_MAX_QTY; num++) {         // loop through all possible supported TPDO
      p_pdo = &p_node->TpdoTbl[num];                            // get ptr to TPDO in list
                                                                // see, if a timer is set for timer event
      if (p_pdo->AsyncEventTmrId > CANOPEN_TMR_INVALID_ID) {
        CORE_ENTER_ATOMIC();
        CANopen_TmrDel(p_tmr,
                       p_pdo->AsyncEventTmrId,
                       &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        p_pdo->AsyncEventTmrId = CANOPEN_TMR_INVALID_ID;        // indicate no timer for timer event
        CORE_EXIT_ATOMIC();
      }
      //                                                           see, if a timer is set for inhibit time
      if (p_pdo->AsyncInhibitTmrId > CANOPEN_TMR_INVALID_ID) {
        CORE_ENTER_ATOMIC();
        CANopen_TmrDel(p_tmr,
                       p_pdo->AsyncInhibitTmrId,
                       &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        //                                                         indicate no timer for inhibit timing
        p_pdo->AsyncInhibitTmrId = CANOPEN_TMR_INVALID_ID;
        CORE_EXIT_ATOMIC();
      }
    }
  }
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CANopen_TmrInsert()
 *
 * @brief    Inserts a timer in the used timer list.
 *
 * @param    p_tmr       Pointer to timer structure.
 *
 * @param    delta_new   Delta time for new action.
 *
 * @param    p_act       Pointer to action information structure.
 *
 * @return   DEF_OK,   if timer successfully inserted,
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) This function inserts an action into the used timer list. First, this function
 *               checks for an existing timer event on the same time and links the action to the
 *               existing timer information. If no timer event exists on the same time, a new timer event
 *               will be created and inserted into the used timer list in a way, that the resulting
 *               used timer list is a sorted list with precalculated delta times between the time events.
 *******************************************************************************************************/
static CPU_BOOLEAN CANopen_TmrInsert(CANOPEN_TMR        *p_tmr,
                                     CPU_INT32U         delta_new,
                                     CANOPEN_TMR_ACTION *p_act)
{
  CPU_INT32U       delta_next;
  CANOPEN_TMR_TIME *p_tmr_info;
  CANOPEN_TMR_TIME *p_tmr_result = DEF_NULL;
  CPU_BOOLEAN      result = DEF_FAIL;

  delta_next = p_tmr->TimeRelative;                             // get remaining delta time to next event
  p_tmr_info = p_tmr->TmrUsedListHeadPtr;                       // get used timer list
  if (p_tmr_info == DEF_NULL) {                                 // --- FIRST TIMER ---
    p_tmr_result = p_tmr->TmrFreeListHeadPtr;                   // yes: get free timer
    p_tmr->TmrFreeListHeadPtr = p_tmr_result->NextPtr;          // remove timer from free list
    p_tmr_result->Delta = delta_new;
    p_tmr_result->ActionPtr = p_act;
    p_tmr_result->ActionEndPtr = p_act;
    p_tmr_result->NextPtr = DEF_NULL;
    p_tmr->TmrUsedListHeadPtr = p_tmr_result;                   // set timer in used list

    p_tmr->TimeRelative = p_tmr_result->Delta;                  // set delta time to first event
    p_tmr->TimeAbsolute += p_tmr_result->Delta;
  } else {
    while ((delta_new > delta_next)
           && (p_tmr_result == DEF_NULL)) {
      if (p_tmr_info->NextPtr == DEF_NULL) {                    // --- BEHIND LAST TIMER ---
                                                                // yes: get free timer
        p_tmr_result = p_tmr->TmrFreeListHeadPtr;
        p_tmr->TmrFreeListHeadPtr = p_tmr_result->NextPtr;      // remove timer from free list
        p_tmr_result->Delta = delta_new - delta_next;
        p_tmr_result->ActionPtr = p_act;
        p_tmr_result->ActionEndPtr = p_act;
        p_tmr_result->NextPtr = DEF_NULL;                       // last element in used list
        p_tmr_info->NextPtr = p_tmr_result;                     // set timer at end of used list
      } else {                                                  // --- BETWEEN TWO TIMERS ---
        delta_next += p_tmr_info->NextPtr->Delta;               // calc. delta time from now to next tmr
        if (delta_new < delta_next) {                           // see, if new tmr elapses before next tmr
                                                                // yes: get free timer
          p_tmr_result = p_tmr->TmrFreeListHeadPtr;
          //                                                       remove timer from free list
          p_tmr->TmrFreeListHeadPtr = p_tmr_result->NextPtr;
          p_tmr_result->NextPtr = p_tmr_info->NextPtr;
          p_tmr_info->NextPtr = p_tmr_result;
          p_tmr_result->Delta = delta_new - (delta_next - p_tmr_result->NextPtr->Delta);
          p_tmr_result->ActionPtr = p_act;                      // link action to new timer
          p_tmr_result->ActionEndPtr = p_act;                   // update end of action list pointer
          p_tmr_result->NextPtr->Delta = delta_next - delta_new;
        } else {
          p_tmr_info = p_tmr_info->NextPtr;                     // yes: goto next timer
        }
      }
    }

    if (p_tmr_result == DEF_NULL) {
      if (delta_new == delta_next) {                            // --- EQUAL EXISTING TIMER ---
        p_tmr_info->ActionEndPtr->NextActionPtr = p_act;
        p_tmr_info->ActionEndPtr = p_act;
        p_tmr_result = p_tmr_info;                              // set function result
      } else if (delta_new < delta_next) {                      // --- BEFORE FIRST TIMER ---
                                                                // yes: get free timer
        p_tmr_result = p_tmr->TmrFreeListHeadPtr;
        //                                                         remove timer from free list
        p_tmr->TmrFreeListHeadPtr = p_tmr_result->NextPtr;
        p_tmr_result->Delta = delta_new;
        p_tmr_result->ActionPtr = p_act;
        p_tmr_result->ActionEndPtr = p_act;
        p_tmr_result->NextPtr = p_tmr_info;                     // first element in used list
        p_tmr->TmrUsedListHeadPtr = p_tmr_result;               // set timer in used list
                                                                // correct next delta time
        p_tmr_info->Delta = delta_next - delta_new;

        p_tmr->TimeRelative = p_tmr_result->Delta;              // set delta time to first event
        p_tmr->TimeAbsolute += p_tmr_result->Delta;
      }
    }
  }

  if (p_tmr_result != DEF_NULL) {
    result = DEF_OK;
  }

  return (result);
}

/****************************************************************************************************//**
 *                                          CANopen_TmrRemove()
 *
 * @brief    Removes timer information from the used timer list.
 *
 * @param    p_tmr            Pointer to timer structure.
 *
 * @param    p_tmr_info_rem   Pointer to timer information structure to be removed from the list.
 *******************************************************************************************************/
static void CANopen_TmrRemove(CANOPEN_TMR      *p_tmr,
                              CANOPEN_TMR_TIME *p_tmr_info_rem)
{
  CANOPEN_TMR_TIME *p_tmr_info_local;

  if (p_tmr_info_rem != DEF_NULL) {                             // see, if parameter is valid pointer
    if (p_tmr->TmrUsedListHeadPtr == p_tmr_info_rem) {
      if (p_tmr_info_rem->NextPtr == DEF_NULL) {                // --- LAST ENTRY IN LIST ---
        p_tmr->TimeRelative = 0u;                               // stop timer
        p_tmr->TimeAbsolute = 0u;
        //                                                         remove first timer from list
        p_tmr->TmrUsedListHeadPtr = p_tmr_info_rem->NextPtr;
      } else {                                                  // --- FIRST ENTRY ---
                                                                // calculate new delta time
        p_tmr_info_rem->NextPtr->Delta += p_tmr->TimeRelative;
        //                                                         remove first timer from list
        p_tmr->TmrUsedListHeadPtr = p_tmr_info_rem->NextPtr;
        //                                                         reload timer with new delta time
        p_tmr->TimeRelative = p_tmr->TmrUsedListHeadPtr->Delta;
        p_tmr->TimeAbsolute += p_tmr->TmrUsedListHeadPtr->Delta;
      }

      //                                                           append timer to free list
      p_tmr_info_rem->NextPtr = p_tmr->TmrFreeListHeadPtr;
      p_tmr->TmrFreeListHeadPtr = p_tmr_info_rem;
    } else {
      p_tmr_info_local = p_tmr->TmrUsedListHeadPtr;
      do {
        if (p_tmr_info_local->NextPtr == p_tmr_info_rem) {      // --- ENTRY WITHIN LIST ---
                                                                // remove entry from list
          p_tmr_info_local->NextPtr = p_tmr_info_rem->NextPtr;

          if (p_tmr_info_rem->NextPtr != DEF_NULL) {            // --- LAST ENTRY IN LIST ---
                                                                // calculate following delta time
            p_tmr_info_local->NextPtr->Delta += p_tmr_info_rem->Delta;
          }
          //                                                       append timer to free list
          p_tmr_info_rem->NextPtr = p_tmr->TmrFreeListHeadPtr;
          p_tmr->TmrFreeListHeadPtr = p_tmr_info_rem;
          p_tmr_info_rem = DEF_NULL;                            // indicate timer is removed
        }
        p_tmr_info_local = p_tmr_info_local->NextPtr;
      } while ((p_tmr_info_local != DEF_NULL) && (p_tmr_info_rem != DEF_NULL));
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
