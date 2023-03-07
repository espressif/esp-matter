/***************************************************************************//**
 * @file
 * @brief Kernel - Message Handling Services
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_KERNEL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MICRIUM_SOURCE
#include "../include/os.h"
#include "os_priv.h"

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_msg__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_MSG_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                               OS_MsgPoolInit()
 *
 * @brief    This function is called by OSInit() to initialize the free list of OS_MSGs.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MsgPoolInit(RTOS_ERR *p_err)
{
  OS_MSG     *p_msg1;
  OS_MSG     *p_msg2;
  OS_MSG_QTY i;
  OS_MSG_QTY loops;

  p_msg1 = OSCfg_MsgPoolBasePtr;
  p_msg2 = OSCfg_MsgPoolBasePtr;
  p_msg2++;
  loops = OSCfg_MsgPoolSize - 1u;
  for (i = 0u; i < loops; i++) {                                // Init. list of free OS_MSGs
    p_msg1->NextPtr = p_msg2;
    p_msg1->MsgPtr = DEF_NULL;
    p_msg1->MsgSize = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_msg1->MsgTS = 0u;
#endif
    p_msg1++;
    p_msg2++;
  }
  p_msg1->NextPtr = DEF_NULL;                                   // Last OS_MSG
  p_msg1->MsgPtr = DEF_NULL;
  p_msg1->MsgSize = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_msg1->MsgTS = 0u;
#endif

  OSMsgPool.NextPtr = OSCfg_MsgPoolBasePtr;
  OSMsgPool.NbrFree = OSCfg_MsgPoolSize;
  OSMsgPool.NbrUsed = 0u;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSMsgPool.NbrUsedMax = 0u;
#endif
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OS_MsgQFreeAll()
 *
 * @brief    This function returns all the messages in a message queue to the free list.
 *
 * @param    p_msg_q     Pointer to the OS_MSG_Q structure containing messages to free.
 *
 * @return   The number of OS_MSGs returned to the free list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
OS_MSG_QTY OS_MsgQFreeAll(OS_MSG_Q *p_msg_q)
{
  OS_MSG     *p_msg;
  OS_MSG_QTY qty;

  qty = p_msg_q->NbrEntries;                                    // Get the number of OS_MSGs being freed
  if (p_msg_q->NbrEntries > 0u) {
    p_msg = p_msg_q->InPtr;                                     // Point to end of message chain
    p_msg->NextPtr = OSMsgPool.NextPtr;
    OSMsgPool.NextPtr = p_msg_q->OutPtr;                        // Point to beginning of message chain
    OSMsgPool.NbrUsed -= p_msg_q->NbrEntries;                   // Update statistics for free list of messages
    OSMsgPool.NbrFree += p_msg_q->NbrEntries;
    p_msg_q->NbrEntries = 0u;                                   // Flush the message queue
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_msg_q->NbrEntriesMax = 0u;
#endif
    p_msg_q->InPtr = DEF_NULL;
    p_msg_q->OutPtr = DEF_NULL;
  }
  return (qty);
}

/****************************************************************************************************//**
 *                                               OS_MsgQInit()
 *
 * @brief    This function is called to initialize a message queue.
 *
 * @param    p_msg_q     Pointer to the message queue to initialize.
 *
 * @param    size        The maximum number of entries that the message queue can have.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MsgQInit(OS_MSG_Q   *p_msg_q,
                 OS_MSG_QTY size)
{
  *p_msg_q = (OS_MSG_Q){ 0 };
  p_msg_q->NbrEntriesSize = size;
}

/****************************************************************************************************//**
 *                                               OS_MsgQGet()
 *
 * @brief    This function retrieves a message from a message queue.
 *
 * @param    p_msg_q     Pointer to the message queue where we want to extract the message from.
 *
 * @param    p_msg_size  Pointer to where the size (in bytes) of the message will be placed.
 *
 * @param    p_ts        Pointer to where the timestamp will be placed.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
 *
 * @return   The message (a pointer).
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void *OS_MsgQGet(OS_MSG_Q    *p_msg_q,
                 OS_MSG_SIZE *p_msg_size,
                 CPU_TS      *p_ts,
                 RTOS_ERR    *p_err)
{
  OS_MSG *p_msg;
  void   *p_void;

#if (OS_CFG_TS_EN == DEF_DISABLED)
  (void)p_ts;                                                   // Prevent compiler warning for not using 'ts'
#endif

  if (p_msg_q->NbrEntries == 0u) {                              // Is the queue empty?
    *p_msg_size = 0u;                                           // Yes
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = 0u;
    }
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return (DEF_NULL);
  }

  p_msg = p_msg_q->OutPtr;                                      // No, get the next message to extract from the queue
  p_void = p_msg->MsgPtr;
  *p_msg_size = p_msg->MsgSize;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  if (p_ts != DEF_NULL) {
    *p_ts = p_msg->MsgTS;
  }
#endif

  p_msg_q->OutPtr = p_msg->NextPtr;                             // Point to next message to extract

  if (p_msg_q->OutPtr == DEF_NULL) {                            // Are there any more messages in the queue?
    p_msg_q->InPtr = DEF_NULL;                                  // No
    p_msg_q->NbrEntries = 0u;
  } else {
    p_msg_q->NbrEntries--;                                      // Yes, One less message in the queue
  }

  p_msg->NextPtr = OSMsgPool.NextPtr;                           // Return message control block to free list
  OSMsgPool.NextPtr = p_msg;
  OSMsgPool.NbrFree++;
  OSMsgPool.NbrUsed--;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (p_void);
}

/****************************************************************************************************//**
 *                                               OS_MsgQPut()
 *
 * @brief    This function places a message in a message queue.
 *
 * @param    p_msg_q     Pointer to the OS_TCB of the task to post the message to.
 *
 * @param    p_void      Pointer to the message to send.
 *
 * @param    msg_size    The size of the message (in bytes).
 *
 * @param    opt         Specifies whether the message will be posted in FIFO or LIFO order:
 *                       OS_OPT_POST_FIFO    First-in, First-out.
 *                       OS_OPT_POST_LIFO    Last-in,  First-out.
 *
 * @param    ts          Timestamp as to when the message was posted.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NO_MORE_RSRC
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MsgQPut(OS_MSG_Q    *p_msg_q,
                void        *p_void,
                OS_MSG_SIZE msg_size,
                OS_OPT      opt,
                CPU_TS      ts,
                RTOS_ERR    *p_err)
{
  OS_MSG *p_msg;
  OS_MSG *p_msg_in;

#if (OS_CFG_TS_EN == DEF_DISABLED)
  (void)ts;                                                     // Prevent compiler warning for not using 'ts'
#endif

  if (p_msg_q->NbrEntries >= p_msg_q->NbrEntriesSize) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);                   // Message queue cannot accept any more messages
    return;
  }

  if (OSMsgPool.NbrFree == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NO_MORE_RSRC);                // No more OS_MSG to use
    return;
  }

  p_msg = OSMsgPool.NextPtr;                                    // Remove message control block from free list
  OSMsgPool.NextPtr = p_msg->NextPtr;
  OSMsgPool.NbrFree--;
  OSMsgPool.NbrUsed++;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  if (OSMsgPool.NbrUsedMax < OSMsgPool.NbrUsed) {
    OSMsgPool.NbrUsedMax = OSMsgPool.NbrUsed;
  }
#endif

  if (p_msg_q->NbrEntries == 0u) {                              // Is this first message placed in the queue?
    p_msg_q->InPtr = p_msg;                                     // Yes
    p_msg_q->OutPtr = p_msg;
    p_msg_q->NbrEntries = 1u;
    p_msg->NextPtr = DEF_NULL;
  } else {                                                      // No
    if ((opt & OS_OPT_POST_LIFO) == OS_OPT_POST_FIFO) {         // Is it FIFO or LIFO?
      p_msg_in = p_msg_q->InPtr;                                // FIFO, add to the head
      p_msg_in->NextPtr = p_msg;
      p_msg_q->InPtr = p_msg;
      p_msg->NextPtr = DEF_NULL;
    } else {
      p_msg->NextPtr = p_msg_q->OutPtr;                         // LIFO, add to the tail
      p_msg_q->OutPtr = p_msg;
    }
    p_msg_q->NbrEntries++;
  }

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  if (p_msg_q->NbrEntriesMax < p_msg_q->NbrEntries) {
    p_msg_q->NbrEntriesMax = p_msg_q->NbrEntries;
  }
#endif

  p_msg->MsgPtr = p_void;                                       // Deposit message in the message queue entry
  p_msg->MsgSize = msg_size;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_msg->MsgTS = ts;
#endif
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
