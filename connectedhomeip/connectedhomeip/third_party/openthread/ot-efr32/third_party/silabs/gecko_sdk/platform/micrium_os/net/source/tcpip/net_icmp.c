/***************************************************************************//**
 * @file
 * @brief Network Icmp Generic Layer - (Internet Control Message Protocol)
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>

#include  "net_icmp_priv.h"
#include  <net/include/net_cfg_net.h>
#include  "net_type_priv.h"
#include  "net_ip_priv.h"

#ifdef  NET_ICMPv4_MODULE_EN
#include  "net_icmpv4_priv.h"
#endif
#ifdef  NET_ICMPv6_MODULE_EN
#include  "net_icmpv6_priv.h"
#endif

#include  <common/include/kal.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  NET_ICMP_LOCK_NAME                 "Net ICMP Lock"
#define  NET_ICMP_INTERNAL_DATA_SEG_NAME    "Net ICMP internal data"
#define  NET_ICMP_ECHO_REQ_SEM_NAME         "Net ICMP Echo Req Sem"
#define  NET_ICMP_ECHO_REQ_POOL_NAME        "Net ICMP echo req pool"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_icmp_echo_req NET_ICMP_ECHO_REQ;

struct  net_icmp_echo_req {
  KAL_SEM_HANDLE Sem;
  CPU_INT16U     ID;
  CPU_INT16U     Seq;
  void           *SrcDataPtr;
  CPU_INT16U     SrcDataLen;
  CPU_BOOLEAN    DataCmp;
  SLIST_MEMBER   ListNode;
};

typedef  struct  net_icmpv4_data {
  MEM_SEG      *MemSegPtr;                                     // Mem Seg to alloc from.
  MEM_DYN_POOL EchoReqPool;
  SLIST_MEMBER *ObjListPtr;
} NET_ICMP_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static KAL_LOCK_HANDLE NetICMP_Lock;
static NET_ICMP_DATA   *NetICMP_DataPtr;
static CPU_INT16U      NetICMP_ID;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetICMP_TxEchoReq()
 *
 * @brief    Transmit an ICMPv4 or ICMPv6 echo request message.
 *
 * @param    p_addr_dest     Pointer to IP destination address to send the ICMP echo request.
 *
 * @param    addr_len        IP address length :
 *                               - NET_IPv4_ADDR_SIZE
 *                               - NET_IPv6_ADDR_SIZE
 *
 * @param    timeout_ms      Timeout value to wait for ICMP echo response.
 *
 * @param    p_data          Pointer to the data buffer to include in the ICMP echo request.
 *
 * @param    data_len        Number of data buffer octets to include in the ICMP echo request.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                               - RTOS_ERR_NET_NEXT_HOP
 *                               - RTOS_ERR_NET_IF_LINK_DOWN
 *                               - RTOS_ERR_NET_ICMP_ECHO_REPLY_DATA_CMP
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_OK,   if ICMP echo request message successfully sent to remote host.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetICMP_TxEchoReq(CPU_INT08U      *p_addr_dest,
                              NET_IP_ADDR_LEN addr_len,
                              CPU_INT32U      timeout_ms,
                              void            *p_data,
                              CPU_INT16U      data_len,
                              RTOS_ERR        *p_err)
{
  NET_ICMP_ECHO_REQ *p_echo_req_handle_new;
  KAL_SEM_HANDLE    sem_handle;
  CPU_INT16U        seq;
  CPU_BOOLEAN       result = DEF_FAIL;
  RTOS_ERR          local_err;
  RTOS_ERR_CODE     err_dbg_fail = RTOS_ERR_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if ((addr_len != NET_IPv4_ADDR_SIZE)
      && (addr_len != NET_IPv6_ADDR_SIZE)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }

#ifndef  NET_IPv4_MODULE_EN
  if (addr_len == NET_IPv4_ADDR_SIZE) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }
#endif

#ifndef  NET_IPv6_MODULE_EN
  if (addr_len == NET_IPv6_ADDR_SIZE) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }
#endif
#endif

  NetICMP_LockAcquire();

  p_echo_req_handle_new = (NET_ICMP_ECHO_REQ *)Mem_DynPoolBlkGet(&NetICMP_DataPtr->EchoReqPool,
                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit_release_lock;
  }

  sem_handle = KAL_SemCreate(NET_ICMP_ECHO_REQ_SEM_NAME, DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit_free_pool;
  }

  p_echo_req_handle_new->Sem = sem_handle;

  SList_PushBack(&(NetICMP_DataPtr->ObjListPtr), &(p_echo_req_handle_new->ListNode));

  p_echo_req_handle_new->ID = NetICMP_ID;
  NetICMP_ID++;

  p_echo_req_handle_new->SrcDataPtr = p_data;
  p_echo_req_handle_new->SrcDataLen = data_len;
  p_echo_req_handle_new->DataCmp = DEF_FAIL;

  if (addr_len == NET_IPv4_ADDR_SIZE) {
#ifdef  NET_ICMPv4_MODULE_EN
    seq = NetICMPv4_TxEchoReq((NET_IPv4_ADDR *)p_addr_dest,
                              p_echo_req_handle_new->ID,
                              p_data,
                              data_len,
                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      result = DEF_FAIL;
      goto exit_release;
    }

#else
    err_dbg_fail = RTOS_ERR_NOT_AVAIL;
    goto exit_release;
#endif
  } else if (addr_len == NET_IPv6_ADDR_SIZE) {
#ifdef  NET_ICMPv6_MODULE_EN
    seq = NetICMPv6_TxEchoReq((NET_IPv6_ADDR *)p_addr_dest,
                              p_echo_req_handle_new->ID,
                              p_data,
                              data_len,
                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      result = DEF_FAIL;
      goto exit_release;
    }

#else
    err_dbg_fail = RTOS_ERR_NOT_AVAIL;
    goto exit_release;
#endif
  } else {
    err_dbg_fail = RTOS_ERR_NOT_AVAIL;
    goto exit_release;
  }

  p_echo_req_handle_new->Seq = seq;

  NetICMP_LockRelease();                                        // Lock must be released before pending
  KAL_SemPend(sem_handle, KAL_OPT_PEND_NONE, timeout_ms, p_err);
  NetICMP_LockAcquire();                                        // Re-acquire lock as it will be released at the end
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit_release;
  }

  result = DEF_OK;

exit_release:

  KAL_SemDel(sem_handle);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    if (data_len > 0) {
      if (p_echo_req_handle_new->DataCmp != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ICMP_ECHO_REPLY_DATA_CMP);
      }
    }
  }

  SList_Rem(&(NetICMP_DataPtr->ObjListPtr), &(p_echo_req_handle_new->ListNode));

exit_free_pool:

  Mem_DynPoolBlkFree(&NetICMP_DataPtr->EchoReqPool,
                     p_echo_req_handle_new,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);

exit_release_lock:
  NetICMP_LockRelease();
  if (err_dbg_fail != RTOS_ERR_NONE) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, err_dbg_fail, DEF_FAIL);
  }

  return (result);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetICMP_Init()
 *
 * @brief    (1) Initialize Internet Control Message Protocol Layer:
 *               - (a) Initialize ICMP   OS objects
 *               - (b) Initialize ICMPv4 variables if IPv4 is present.
 *               - (c) Initialize ICMPv6 variables if IPv6 is present.
 *
 * @param    p_err   Error Pointer.
 *******************************************************************************************************/
void NetICMP_Init(RTOS_ERR *p_err)
{
  MEM_SEG *p_seg;

  NetICMP_ID = 1u;
  NetICMP_DataPtr = DEF_NULL;

  //                                                               --------------- INITIALIZE ICMP LOCK ---------------
  //                                                               Create ICMP lock signal ...
  //                                                               ... with ICMP access available (see Note #1d1).
  NetICMP_Lock = KAL_LockCreate(NET_ICMP_LOCK_NAME,
                                DEF_NULL,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_seg = DEF_NULL;
  NetICMP_DataPtr = (NET_ICMP_DATA *)Mem_SegAlloc(NET_ICMP_INTERNAL_DATA_SEG_NAME,
                                                  p_seg,
                                                  sizeof(NET_ICMP_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetICMP_DataPtr->MemSegPtr = p_seg;

  SList_Init(&NetICMP_DataPtr->ObjListPtr);

  Mem_DynPoolCreate(NET_ICMP_ECHO_REQ_POOL_NAME,
                    &NetICMP_DataPtr->EchoReqPool,
                    NetICMP_DataPtr->MemSegPtr,
                    sizeof(NET_ICMP_ECHO_REQ),
                    sizeof(CPU_ALIGN),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#ifdef  NET_ICMPv4_MODULE_EN
  NetICMPv4_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#ifdef  NET_ICMPv6_MODULE_EN
  NetICMPv6_Init();
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMP_LockAcquire()
 *
 * @brief    Acquire mutually exclusive access to ICMP layer.
 *
 * @note     (1) ICMP access MUST be acquired--i.e. MUST wait for access; do NOT timeout.
 *               Failure to acquire ICMP access will prevent network task(s)/operation(s)
 *               from functioning.
 *               ICMP access MUST be acquired exclusively by only a single task at any one time.
 *******************************************************************************************************/
void NetICMP_LockAcquire(void)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Acquire exclusive ICMP access without timeout
  KAL_LockAcquire(NetICMP_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetICMP_LockRelease()
 *
 * @brief    Release mutually exclusive access to ICMP layer.
 *
 * @note     (1) ICMP access MUST be released--i.e. MUST unlock access without failure.
 *               - (a) Failure to release ICMP access will prevent network task(s)/operation(s) from
 *                     functioning.  Thus, ICMP access is assumed to be successfully released since
 *                     NO OS error handling could be performed to counteract failure.
 *               See also 'NetICMP_LockAcquire()  Note #1'.
 *******************************************************************************************************/
void NetICMP_LockRelease(void)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_LockRelease(NetICMP_Lock, &local_err);                    // Release exclusive network access.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetICMP_RxEchoReply()
 *
 * @brief    - (1) Received a ICMPv4 or ICMPv6 Echo Reply message.
 *           - (2) Compare date received with data sent.
 *
 * @param    id          ICMP message ID.
 *
 * @param    seq         ICMP sequence number.
 *
 * @param    p_data      Pointer to data received in the ICMP reply message.
 *
 * @param    data_len    ICMP reply message data length.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetICMP_RxEchoReply(CPU_INT16U id,
                         CPU_INT16U seq,
                         CPU_INT08U *p_data,
                         CPU_INT16U data_len,
                         RTOS_ERR   *p_err)
{
  NET_ICMP_ECHO_REQ *p_icmp_echo_req = DEF_NULL;
  CPU_BOOLEAN       found = DEF_NO;

  NetICMP_LockAcquire();

  SLIST_FOR_EACH_ENTRY(NetICMP_DataPtr->ObjListPtr, p_icmp_echo_req, NET_ICMP_ECHO_REQ, ListNode) {
    if ((p_icmp_echo_req->ID == id)
        && (p_icmp_echo_req->Seq == seq)) {
      if (p_icmp_echo_req->SrcDataPtr != DEF_NULL) {
        if (p_icmp_echo_req->SrcDataLen != data_len) {
          p_icmp_echo_req->DataCmp = DEF_FAIL;
        } else {
          p_icmp_echo_req->DataCmp = Mem_Cmp(p_data,
                                             p_icmp_echo_req->SrcDataPtr,
                                             data_len);
        }
      } else if (data_len == 0u) {
        p_icmp_echo_req->DataCmp = DEF_OK;
      } else {
        p_icmp_echo_req->DataCmp = DEF_FAIL;
      }

      KAL_SemPost(p_icmp_echo_req->Sem, KAL_OPT_PEND_NONE, p_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit_release_lock;
  }

exit_release_lock:
  NetICMP_LockRelease();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
