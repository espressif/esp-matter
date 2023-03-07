/***************************************************************************//**
 * @file
 * @brief Network Buffer Management
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_cfg_net.h>
#ifdef  NET_BUF_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/net_util.h>
#include  <net/include/net_cfg_net.h>
#include  "net_buf_priv.h"
#include  "net_priv.h"
#include  "net_conn_priv.h"
#include  "net_tcp_priv.h"
#include  "net_if_priv.h"

#ifdef  NET_IF_LOOPBACK_MODULE_EN
#include  "net_if_loopback_priv.h"
#endif

#ifdef  NET_IF_ETHER_MODULE_EN
#include  "net_if_ether_priv.h"
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
#include  "net_if_wifi_priv.h"
#endif

#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_buf_data {
  NET_BUF_POOLS *BufTbl;
  NET_BUF_QTY   BufID_Ctr;
} NET_BUF_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_BUF_DATA *NetBuf_DataPtr;

static NET_BUF_QTY NetBuf_ID_Ctr;                               // Global buf ID ctr.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetBuf_FreeHandler(NET_BUF *p_buf);

static void NetBuf_ClrHdr(NET_BUF_HDR *p_buf_hdr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetBuf_PoolStatGet()
 *
 * @brief    Get network buffer statistics pool.
 *
 * @param    if_nbr  Interface number to get network buffer statistics.
 *
 * @return   Network buffer statistics pool, if NO error(s).
 *           NULL           statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) [INTERNAL] Return values MUST be initialized PRIOR to all other validation or function
 *           handling in case of any error(s).
 *
 * @note     (4) [INTERNAL] 'NetBufStatPool's MUST ALWAYS be accessed exclusively in critical sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetBuf_PoolStatGet(NET_IF_NBR if_nbr)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_BUF_POOLS *p_pool;
  CPU_BOOLEAN   is_valid = DEF_NO;
  CORE_DECLARE_IRQ_STATE;
#endif

  PP_UNUSED_PARAM(if_nbr);

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err (see Note #3).

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)

  PP_UNUSED_PARAM(is_valid);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetBuf_PoolStatGet);            // See Note #1b.

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return (stat_pool);
  }
#endif

  //                                                               -------------- GET NET BUF STAT POOL ---------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  CORE_ENTER_ATOMIC();
  stat_pool = p_pool->NetBufStatPool;
  CORE_EXIT_ATOMIC();

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                       NetBuf_PoolStatResetMaxUsed()
 *
 * @brief    Reset network buffer statistics pool's maximum number of entries used.
 *
 * @param    if_nbr  Interface number to reset network buffer statistics.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetBuf_PoolStatResetMaxUsed(NET_IF_NBR if_nbr)
{
  NET_BUF_POOLS *p_pool = DEF_NULL;
  CPU_BOOLEAN   is_valid = DEF_NO;

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(p_pool);
  PP_UNUSED_PARAM(is_valid);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_PoolStatResetMaxUsed);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return;
  }
#endif

  //                                                               ------------- RESET NET BUF STAT POOL --------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  NetStat_PoolResetUsedMax(&p_pool->NetBufStatPool);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif
}

/****************************************************************************************************//**
 *                                       NetBuf_RxLargePoolStatGet()
 *
 * @brief    Get large receive network buffer statistics pool.
 *
 * @param    if_nbr  Interface number to get network buffer statistics.
 *
 * @return   Large receive network buffer statistics pool, if NO error(s).
 *           NULL statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) [INTERNAL] Return values MUST be initialized PRIOR to all other validation or function
 *               handling in case of any error(s).
 *
 * @note     (4) [INTERNAL] 'RxBufLargeStatPool's MUST ALWAYS be accessed exclusively in critical sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetBuf_RxLargePoolStatGet(NET_IF_NBR if_nbr)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_BUF_POOLS *p_pool;
  CPU_BOOLEAN   is_valid = DEF_NO;
  CORE_DECLARE_IRQ_STATE;
#endif

  PP_UNUSED_PARAM(if_nbr);

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err (see Note #3).

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)

  PP_UNUSED_PARAM(is_valid);
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_RxLargePoolStatGet);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return (stat_pool);
  }
#endif

  //                                                               -------------- GET NET BUF STAT POOL ---------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  CORE_ENTER_ATOMIC();
  stat_pool = p_pool->RxBufLargeStatPool;
  CORE_EXIT_ATOMIC();

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                   NetBuf_RxLargePoolStatResetMaxUsed()
 *
 * @brief    Reset large receive network buffer statistics pool's maximum number of entries used.
 *
 * @param    if_nbr  Interface number to reset network buffer statistics.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetBuf_RxLargePoolStatResetMaxUsed(NET_IF_NBR if_nbr)
{
  NET_BUF_POOLS *p_pool = DEF_NULL;
  CPU_BOOLEAN   is_valid = DEF_NO;

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(p_pool);
  PP_UNUSED_PARAM(is_valid);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_RxLargePoolStatResetMaxUsed);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return;
  }
#endif

  //                                                               ------------- RESET NET BUF STAT POOL --------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  NetStat_PoolResetUsedMax(&p_pool->RxBufLargeStatPool);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif
}

/****************************************************************************************************//**
 *                                       NetBuf_TxLargePoolStatGet()
 *
 * @brief    Get large transmit network buffer statistics pool.
 *
 * @param    if_nbr  Interface number to get network buffer statistics.
 *
 * @return   Large transmit network buffer statistics pool, if NO error(s).
 *           NULL  statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) [INTERNAL] Return values MUST be initialized PRIOR to all other validation or function
 *               handling in case of any error(s).
 *
 * @note     (4) [INTERNAL] 'TxBufLargeStatPool's MUST ALWAYS be accessed exclusively in critical sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetBuf_TxLargePoolStatGet(NET_IF_NBR if_nbr)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_BUF_POOLS *p_pool;
  CPU_BOOLEAN   is_valid = DEF_NO;
  CORE_DECLARE_IRQ_STATE;
#endif

  PP_UNUSED_PARAM(if_nbr);

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err (see Note #3).

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)

  PP_UNUSED_PARAM(is_valid);
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_TxLargePoolStatGet);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return (stat_pool);
  }
#endif

  //                                                               -------------- GET NET BUF STAT POOL ---------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  CORE_ENTER_ATOMIC();
  stat_pool = p_pool->TxBufLargeStatPool;
  CORE_EXIT_ATOMIC();

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                   NetBuf_TxLargePoolStatResetMaxUsed()
 *
 * @brief    Reset large receive network buffer statistics pool's maximum number of entries used.
 *
 * @param    if_nbr  Interface number to reset network buffer statistics.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetBuf_TxLargePoolStatResetMaxUsed(NET_IF_NBR if_nbr)
{
  NET_BUF_POOLS *p_pool = DEF_NULL;
  CPU_BOOLEAN   is_valid = DEF_NO;

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(p_pool);
  PP_UNUSED_PARAM(is_valid);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_TxLargePoolStatResetMaxUsed);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return;
  }
#endif

  //                                                               ------------- RESET NET BUF STAT POOL --------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  NetStat_PoolResetUsedMax(&p_pool->TxBufLargeStatPool);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif
}

/****************************************************************************************************//**
 *                                       NetBuf_TxSmallPoolStatGet()
 *
 * @brief    Get small transmit network buffer statistics pool.
 *
 * @param    if_nbr  Interface number to get network buffer statistics.
 *
 * @return   Small transmit network buffer statistics pool, if NO error(s).
 *           NULL statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) [INTERNAL] Return values MUST be initialized PRIOR to all other validation or function
 *               handling in case of any error(s).
 *
 * @note     (4) [INTERNAL] 'TxBufSmallStatPool's MUST ALWAYS be accessed exclusively in critical sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetBuf_TxSmallPoolStatGet(NET_IF_NBR if_nbr)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_BUF_POOLS *p_pool;
  CPU_BOOLEAN   is_valid = DEF_NO;
  CORE_DECLARE_IRQ_STATE;
#endif

  PP_UNUSED_PARAM(if_nbr);

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err (see Note #3).

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_TxSmallPoolStatGet);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return (stat_pool);
  }
#endif

  //                                                               -------------- GET NET BUF STAT POOL ---------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  CORE_ENTER_ATOMIC();
  stat_pool = p_pool->TxBufSmallStatPool;
  CORE_EXIT_ATOMIC();

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(is_valid);
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                   NetBuf_TxSmallPoolStatResetMaxUsed()
 *
 * @brief    Reset small transmit network buffer statistics pool's maximum number of entries used.
 *
 * @param    if_nbr  Interface number to reset network buffer statistics.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetBuf_TxSmallPoolStatResetMaxUsed(NET_IF_NBR if_nbr)
{
  NET_BUF_POOLS *p_pool = DEF_NULL;
  CPU_BOOLEAN   is_valid = DEF_NO;

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(p_pool);
  PP_UNUSED_PARAM(is_valid);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetBuf_TxSmallPoolStatResetMaxUsed);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    Net_GlobalLockRelease();
    return;
  }
#endif

  //                                                               ------------- RESET NET BUF STAT POOL --------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  NetStat_PoolResetUsedMax(&p_pool->TxBufSmallStatPool);

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
 *                                               NetBuf_Init()
 *
 * @brief    (1) Initialize Network Buffer Management Module :
 *               - (a) Initialize network buffer pools
 *               - (b) Initialize network buffer ID counter
 *******************************************************************************************************/
void NetBuf_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err)
{
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_BUF_POOLS *p_pool;
  NET_IF_NBR    if_nbr;
#endif

  NetBuf_DataPtr = (NET_BUF_DATA *)Mem_SegAlloc("Net Buffer module data",
                                                p_mem_seg,
                                                sizeof(NET_BUF_DATA),
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  NetBuf_DataPtr->BufTbl = (NET_BUF_POOLS *)Mem_SegAlloc("Net Buffer Table",
                                                         p_mem_seg,
                                                         sizeof(NET_BUF_POOLS) * Net_CoreDataPtr->IF_NbrTot,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------------ INIT BUF POOLS ------------------
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  if_nbr = NET_IF_NBR_BASE;
  for (; if_nbr < Net_CoreDataPtr->IF_NbrTot; if_nbr++) {
    p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];

    //                                                             Clr net buf stat pools.
    NetStat_PoolClr(&p_pool->NetBufStatPool);
    NetStat_PoolClr(&p_pool->RxBufLargeStatPool);
    NetStat_PoolClr(&p_pool->TxBufLargeStatPool);
    NetStat_PoolClr(&p_pool->TxBufSmallStatPool);
  }
#endif
  //                                                               ------------------ INIT BUF ID CTR -----------------
  NetBuf_ID_Ctr = NET_BUF_ID_INIT;
}

/****************************************************************************************************//**
 *                                               NetBuf_PoolInit()
 *
 * @brief    (1) Allocate & initialize a network buffer pool :
 *               - (a) Allocate   network buffer pool
 *               - (b) Initialize network buffer pool statistics
 *
 * @param    if_nbr          Interface number to initialize network buffer pools.
 *
 * @param    type            Network buffer pool type :
 *                               - NET_BUF_TYPE_BUF        Network buffer                pool.
 *                               - NET_BUF_TYPE_RX_LARGE   Network buffer large receive  pool.
 *                               - NET_BUF_TYPE_TX_LARGE   Network buffer large transmit pool.
 *                               - NET_BUF_TYPE_TX_SMALL   Network buffer small transmit pool.
 *
 * @param    p_pool_name     Name of the pool.
 *
 * @param    pmem_base_addr  Network buffer memory pool base address :
 *                           Null (0) address        Network buffers allocated from general-
 *                           purpose heap.
 *                           Non-null address        Network buffers allocated from specified
 *                           base address of dedicated memory.
 *
 * @param    mem_size        Size      of network buffer memory pool to initialize (in octets).
 *
 * @param    blk_nbr         Number    of network buffer blocks      to initialize.
 *
 * @param    blk_size        Size      of network buffer blocks      to initialize (in octets).
 *
 * @param    blk_align       Alignment of network buffer blocks      to initialize (in octets).
 *
 * @param    p_mem_pool      Pointer to memory pool structure.
 *
 * @param    poctets_reqd    Pointer to a variable to ... :
 *                               - (a) Return the number of octets required to successfully
 *                                     allocate the network buffer pool, if any error(s);
 *                               - (b) Return 0, otherwise.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetBuf_PoolInit(NET_IF_NBR   if_nbr,
                     NET_BUF_TYPE type,
                     CPU_CHAR     *p_pool_name,
                     void         *p_mem_base_addr,
                     CPU_SIZE_T   mem_size,
                     CPU_SIZE_T   blk_nbr,
                     CPU_SIZE_T   blk_size,
                     CPU_SIZE_T   blk_align,
                     RTOS_ERR     *p_err)
{
  NET_BUF_POOLS *p_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL *p_stat_pool;
#endif
  MEM_DYN_POOL *p_mem_pool;

  PP_UNUSED_PARAM(p_mem_base_addr);
  PP_UNUSED_PARAM(mem_size);

  //                                                               ------------------ INIT BUF POOL -------------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  switch (type) {
    case NET_BUF_TYPE_BUF:
      p_mem_pool = &p_pool->NetBufPool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->NetBufStatPool;
#endif
      break;

    case NET_BUF_TYPE_RX_LARGE:
      p_mem_pool = &p_pool->RxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->RxBufLargeStatPool;
#endif
      break;

    case NET_BUF_TYPE_TX_LARGE:
      p_mem_pool = &p_pool->TxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->TxBufLargeStatPool;
#endif
      break;

    case NET_BUF_TYPE_TX_SMALL:
      p_mem_pool = &p_pool->TxBufSmallPool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->TxBufSmallStatPool;
#endif
      break;

    case NET_BUF_TYPE_NONE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit;
  }

  //                                                               Create net buf mem pool.
  Mem_DynPoolCreate(p_pool_name,
                    p_mem_pool,
                    DEF_NULL,
                    blk_size,
                    blk_align,
                    blk_nbr,
                    blk_nbr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------------- INIT BUF STAT POOL ----------------
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NetStat_PoolInit(p_stat_pool,
                   blk_nbr);
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetBuf_PoolCfgValidate()
 *
 * @brief    (1) Validate network buffer pool configuration :
 *               - (a) Validate configured number of network buffers
 *               - (b) Validate configured size   of network buffers
 *
 * @param    if_type     Interface type:
 *                           - NET_IF_TYPE_LOOPBACK
 *                           - NET_IF_TYPE_ETHER
 *                           - NET_IF_TYPE_WIFI
 *
 *
 * Argument(s) : if_type     Interface type:
 *                               NET_IF_TYPE_LOOPBACK
 *                               NET_IF_TYPE_ETHER
 *                               NET_IF_TYPE_WIFI
 *
 *               p_dev_cfg    Pointer to network interface's device configuration.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Each network interface/device MUST configure :
 *
 *                   (a) At least one large receive  buffer.
 *                   (b) At least one       transmit buffer; however, zero large OR zero small transmit
 *                       buffers MAY be configured.
 *
 *               (3) All   network  buffer data area sizes MUST be configured greater than or equal
 *                   to NET_BUF_DATA_SIZE_MIN.
 *
 *                   Large transmit buffer data area sizes MUST be configured greater than or equal
 *                   to small transmit buffer data area sizes.
 *******************************************************************************************************/
void NetBuf_PoolCfgValidate(NET_IF_TYPE if_type,
                            NET_DEV_CFG *p_dev_cfg)
{
  NET_BUF_QTY nbr_bufs_tx = 0;
  CPU_INT32U  rx_buf_size_min = 0;
  CPU_INT32U  tx_buf_size_min = 0;

  //                                                               -------------- VALIDATE NBR BUFS ---------------
  //                                                               Validate nbr rx bufs (see Note #2a).
  RTOS_ASSERT_DBG((p_dev_cfg->RxBufLargeNbr >= NET_BUF_NBR_RX_LARGE_MIN), RTOS_ERR_INVALID_CFG,; );

  //                                                               Validate nbr tx bufs (see Note #2b).
  nbr_bufs_tx = p_dev_cfg->TxBufLargeNbr + p_dev_cfg->TxBufSmallNbr;
  RTOS_ASSERT_DBG((nbr_bufs_tx >= NET_BUF_NBR_TX_MIN), RTOS_ERR_INVALID_CFG,; );

  switch (if_type) {
#ifdef NET_IF_LOOPBACK_MODULE_EN
    case NET_IF_TYPE_LOOPBACK:
      rx_buf_size_min = NET_IF_LOOPBACK_BUF_RX_LEN_MIN;
      tx_buf_size_min = NET_IF_LOOPBACK_BUF_TX_LEN_MIN;
      break;
#endif

#ifdef NET_IF_ETHER_MODULE_EN
    case NET_IF_TYPE_ETHER:
      rx_buf_size_min = NET_IF_ETHER_BUF_RX_LEN_MIN;
      tx_buf_size_min = NET_IF_ETHER_BUF_TX_LEN_MIN;
      break;
#endif

#ifdef NET_IF_WIFI_MODULE_EN
    case NET_IF_TYPE_WIFI:
      rx_buf_size_min = NET_IF_WIFI_BUF_RX_LEN_MIN;
      tx_buf_size_min = NET_IF_WIFI_BUF_TX_LEN_MIN;
      break;
#endif

    case NET_IF_TYPE_SERIAL:
    case NET_IF_TYPE_PPP:
    case NET_IF_TYPE_NONE:
    default:
      RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
  }

  //                                                               ----------- VALIDATE BUF DATA SIZES ------------
  //                                                               See Note #3.
  //                                                               Validate large rx buf data size.
  RTOS_ASSERT_DBG((p_dev_cfg->RxBufLargeSize >= rx_buf_size_min), RTOS_ERR_INVALID_CFG,; );

  if (p_dev_cfg->TxBufLargeNbr > 0) {                               // If any large tx bufs cfg'd, ...
                                                                    // ... validate large tx buf size (see Note #3).
    RTOS_ASSERT_DBG((p_dev_cfg->TxBufLargeSize >= tx_buf_size_min), RTOS_ERR_INVALID_CFG,; );
  }

  if (p_dev_cfg->TxBufSmallNbr > 0) {                               // If any small tx bufs cfg'd, ...
                                                                    // ... validate small tx buf size (see Note #3).
    RTOS_ASSERT_DBG((p_dev_cfg->TxBufSmallSize >= tx_buf_size_min), RTOS_ERR_INVALID_CFG,; );
  }

  if ((p_dev_cfg->TxBufLargeNbr > 0)                                // If both large tx bufs                     ...
      && (p_dev_cfg->TxBufSmallNbr > 0)) {                          // ... AND small tx bufs cfg'd,              ...
                                                                    // ... validate large vs. small tx buf sizes ...
                                                                    // ... (see Note #3).
    RTOS_ASSERT_DBG((p_dev_cfg->TxBufLargeSize >= p_dev_cfg->TxBufSmallSize), RTOS_ERR_INVALID_CFG,; );
  }
}

/****************************************************************************************************//**
 *                                               NetBuf_Get()
 *
 * @brief    (1) Allocate & initialize a network buffer :
 *               - (a) Get        network buffer
 *                   - (1) For transmit operations, also get network buffer data area
 *               - (b) Initialize network buffer
 *               - (c) Update     network buffer pool statistics
 *               - (d) Return pointer to buffer
 *                     OR
 *                     Null pointer & error code, on failure
 *
 * @param    if_nbr          Interface number to get network buffer.
 *
 * @param    transaction     Transaction type :
 *                               - NET_TRANSACTION_RX        Receive  transaction.
 *                               - NET_TRANSACTION_TX        Transmit transaction.
 *
 * @param    size            Requested buffer size  to store buffer data (see Note #2).
 *
 * @param    ix              Requested buffer index to store buffer data; MUST NOT be pre-adjusted by
 *                           interface's configured index offset(s)  [see Note #3].
 *
 * @param    pix_offset      Pointer to a variable to return the interface's receive/transmit index offset, if NO error(s);
 *                           Return 0, otherwise.
 *
 * @param    flags           Flags to select buffer options; bit-field flags logically OR'd :
 *                           NET_BUF_FLAG_NONE           NO buffer flags selected.
 *                           NET_BUF_FLAG_CLR_MEM        Clear buffer memory (i.e. set each buffer
 *                           data octet to 0x00).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to network buffer, if NO error(s).
 *           Pointer to NULL,           otherwise.
 *
 * @note     (2) 'size' of 0 octets allowed.
 *
 * @note     (3) 'ix' argument automatically adjusted for interface's configured network buffer data
 *               area index offset(s) & MUST NOT be pre-adjusted by caller function(s).
 *
 * @note     (4) Buffer memory cleared                in NetBuf_GetDataPtr(), but
 *               buffer flag NET_BUF_FLAG_CLR_MEM set in NetBuf_Get().
 *
 * @note     (5) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
NET_BUF *NetBuf_Get(NET_IF_NBR      if_nbr,
                    NET_TRANSACTION transaction,
                    NET_BUF_SIZE    size,
                    NET_BUF_SIZE    ix,
                    NET_BUF_SIZE    *p_ix_offset,
                    NET_BUF_FLAGS   flags,
                    RTOS_ERR        *p_err)
{
  NET_IF        *p_if;
  NET_DEV_CFG   *p_dev_cfg;
  NET_BUF       *p_buf = DEF_NULL;
  NET_BUF_HDR   *p_buf_hdr;
  NET_BUF_POOLS *p_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL *p_stat_pool;
#endif
  MEM_DYN_POOL *p_mem_pool;
  NET_BUF_SIZE ix_offset_unused;
  NET_BUF_SIZE data_size;
  NET_BUF_TYPE type;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               -------------- DEALLOCATE TX BUFFERS ---------------
  do {
    CPU_INT08U *p_buf_data;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    p_buf_data = NetIF_TxDeallocQPend(&local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      NetIF_TxPktListDealloc(p_buf_data);
    }
  } while (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE);

  if (p_ix_offset == DEF_NULL) {                                // If NOT avail, ...
    p_ix_offset = (NET_BUF_SIZE *)&ix_offset_unused;            // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(ix_offset_unused);                          // Prevent possible 'variable unused' warning.
  }

  *p_ix_offset = 0u;                                            // Init ix for err (see Note #5).

  //                                                               --------------------- GET BUF ----------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_dev_cfg = (NET_DEV_CFG *) p_if->Dev_Cfg;
  p_pool = (NET_BUF_POOLS *)&NetBuf_DataPtr->BufTbl[if_nbr];
  p_mem_pool = (MEM_DYN_POOL *)&p_pool->NetBufPool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  p_stat_pool = (NET_STAT_POOL *)&p_pool->NetBufStatPool;
#endif
  p_buf = (NET_BUF *) Mem_DynPoolBlkGet(p_mem_pool,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.Buf.NoneAvailCtr);
    goto exit;
  }

  //                                                               --------------------- INIT BUF ---------------------
  p_buf_hdr = &p_buf->Hdr;
  NetBuf_ClrHdr(p_buf_hdr);
  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_USED);             // Set buf as used.
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_CLR_MEM);          // Set buf data area as clr (see Note #4).
#endif
  NET_BUF_GET_ID(p_buf_hdr->ID);                                // Get buf ID.
  p_buf_hdr->RefCtr = 1u;                                       // Set ref ctr to 1; NetBuf_Get() caller is first ref.
  p_buf_hdr->IF_Nbr = if_nbr;                                   // Set buf's IF nbrs.
  p_buf_hdr->IF_NbrTx = if_nbr;
  PP_UNUSED_PARAM(flags);                                       // Prevent 'variable unused' warning (see Note #6).

  switch (transaction) {
    case NET_TRANSACTION_RX:                                    // Cfg buf for prev'ly alloc'd rx buf data area.
      p_buf_hdr->Type = NET_BUF_TYPE_RX_LARGE;
      p_buf_hdr->Size = p_dev_cfg->RxBufLargeSize + p_dev_cfg->RxBufIxOffset;
      *p_ix_offset = p_dev_cfg->RxBufIxOffset;
      //                                                           Ptr to rx buf data area MUST be linked by caller.
      break;

    case NET_TRANSACTION_TX:                                    // Get/cfg tx buf & data area.
      p_buf->DataPtr = NetBuf_GetDataPtr(p_if,
                                         transaction,
                                         size,
                                         ix,
                                         p_ix_offset,
                                         &data_size,
                                         &type,
                                         p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        Mem_DynPoolBlkFree(p_mem_pool, p_buf, &local_err);
        RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
        NetStat_PoolEntryUsedDec(p_stat_pool, &local_err);
#endif

        p_buf = DEF_NULL;
        goto exit;
      }

      p_buf_hdr->Type = type;
      p_buf_hdr->Size = data_size + *p_ix_offset;
      break;

    case NET_TRANSACTION_NONE:
    default:
      Mem_DynPoolBlkFree(p_mem_pool, p_buf, &local_err);
      RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      NetStat_PoolEntryUsedDec(p_stat_pool, &local_err);
#endif

      p_buf = DEF_NULL;
      NET_CTR_ERR_INC(Net_ErrCtrs.Buf.InvTransactionTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);
  }

  //                                                               -------------- UPDATE BUF POOL STATS ---------------
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NetStat_PoolEntryUsedInc(p_stat_pool, &local_err);
#endif

exit:
  return (p_buf);                                               // --------------------- RTN BUF ----------------------
}

/****************************************************************************************************//**
 *                                           NetBuf_GetDataPtr()
 *
 * @brief    (1) Get network buffer data area of sufficient size :
 *               - (a) Get    network buffer data area
 *               - (b) Update network buffer data area pool statistics
 *               - (c) Return pointer to network buffer data area
 *                     OR
 *                     Null pointer & error code, on failure
 *
 * @param    p_if            Pointer to interface to get network buffer data area.
 *
 * @param    transaction     Transaction type :
 *                               - NET_TRANSACTION_RX        Receive  transaction.
 *                               - NET_TRANSACTION_TX        Transmit transaction.
 *
 * @param    size            Requested buffer size  to store buffer data (see Note #3).
 *
 * @param    ix_start        Requested buffer index to store buffer data; MUST NOT be pre-adjusted by
 *                           interface's configured index offset(s)  [see Note #4].
 *
 * @param    p_ix_offset     Pointer to a variable to ... :
 *                               - (a) Return the interface's receive/transmit index offset, if NO error(s);
 *                               - (b) Return 0,                                             otherwise.
 *
 * @param    p_data_size     Pointer to a variable to ... :
 *                               - (a) Return the size of the network buffer data area, if NO error(s);
 *                               - (b) Return 0,                                        otherwise.
 *
 * @param    p_type          Pointer to a variable to ... :
 *                               - (a) Return the network buffer type, if NO error(s);
 *                               - (b) Return NET_BUF_TYPE_NONE,       otherwise.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to network buffer data area, if NO error(s).
 *           Pointer to NULL,                     otherwise.
 *
 * @note     (2) 'size' & 'ix' argument check NOT required unless 'NET_BUF_SIZE's native data type
 *               'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
 *
 * @note     (3) 'size' of 0 octets allowed.
 *
 * @note     (4) 'ix_start' argument automatically adjusted for interface's configured network buffer
 *               data area index offset(s) & MUST NOT be pre-adjusted by caller function(s).
 *
 * @note     (5) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *
 * @note     (6) Network transmit buffers are allocated an appropriately-sized network buffer data area
 *               based on the total requested buffer size & index :
 *               - (a) A small transmit buffer data area will be allocated if :
 *                   - (1) any small transmit  buffer data areas are available, AND ...
 *                   - (2) the total requested buffer size & index is less than or equal to small transmit
 *                         buffers' configured data area size.
 *               - (b) A large transmit buffer data area will be allocated if :
 *                   - (1) NO  small transmit  buffer data areas are available; OR  ...
 *                   - (2) any large transmit  buffer data areas are available, AND
 *                         the total requested buffer size & index is :
 *                       - (a) greater than small transmit buffers' configured data area size AND ...
 *                       - (b) less than or equal to large transmit buffers' configured data area size.
 *
 * @note     (7) Since each network buffer data area allocates additional octets for its configured
 *               offset(s) [see 'net_if.c  NetIF_BufPoolInit()  Note #3'], the network buffer data
 *               area size does NOT need to be adjusted by the number of additional offset octets.
 *
 * @note     (8) Buffer memory cleared in NetBuf_GetDataPtr() instead of in NetBuf_Free() handlers so
 *               that the data in any freed buffer data area may be inspected until that buffer data
 *               area is next allocated.
 *******************************************************************************************************/
CPU_INT08U *NetBuf_GetDataPtr(NET_IF          *p_if,
                              NET_TRANSACTION transaction,
                              NET_BUF_SIZE    size,
                              NET_BUF_SIZE    ix_start,
                              NET_BUF_SIZE    *p_ix_offset,
                              NET_BUF_SIZE    *p_data_size,
                              NET_BUF_TYPE    *p_type,
                              RTOS_ERR        *p_err)
{
  NET_DEV_CFG *p_dev_cfg;
  CPU_INT08U  *p_data = DEF_NULL;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL *p_stat_pool = DEF_NULL;
#endif
  NET_BUF_POOLS *p_pool;
  MEM_DYN_POOL  *p_mem_pool;
  NET_BUF_SIZE  ix_offset = 0u;
  NET_BUF_SIZE  ix_offset_unused;
  NET_BUF_SIZE  data_size_unused;
  NET_BUF_SIZE  size_len;
  NET_BUF_SIZE  size_data;
  NET_BUF_TYPE  type = NET_BUF_TYPE_NONE;
  NET_BUF_TYPE  type_unused;
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Init rtn vals for err (see Note #5).
  if (p_ix_offset == DEF_NULL) {                                // If NOT avail, ...
    p_ix_offset = (NET_BUF_SIZE *)&ix_offset_unused;            // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(ix_offset_unused);                          // Prevent possible 'variable unused' warning.
  }

  *p_ix_offset = 0u;

  if (p_data_size == DEF_NULL) {                                // If NOT avail, ...
    p_data_size = (NET_BUF_SIZE *)&data_size_unused;            // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(data_size_unused);                          // Prevent possible 'variable unused' warning.
  }

  *p_data_size = 0u;

  if (p_type == DEF_NULL) {                                     // If NOT avail, ...
    p_type = (NET_BUF_TYPE *)&type_unused;                      // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(type_unused);                               // Prevent possible 'variable unused' warning.
  }

  *p_type = NET_BUF_TYPE_NONE;

  //                                                               ------------------ VALIDATE IX's -------------------
  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;
  switch (transaction) {
    case NET_TRANSACTION_RX:
      ix_offset = p_dev_cfg->RxBufIxOffset;
      break;

    case NET_TRANSACTION_TX:
      ix_offset = p_dev_cfg->TxBufIxOffset;
      break;

    case NET_TRANSACTION_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Buf.InvTransactionTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

  size_len = size + ix_start;                                   // Calc tot req'd size from start ix (see Note #7).
                                                                // Discard possible size len ovf's.
  if (size_len < size) {
    NET_CTR_ERR_INC(Net_ErrCtrs.Buf.LenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }
  if (size_len < ix_start) {
    NET_CTR_ERR_INC(Net_ErrCtrs.Buf.LenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }

  //                                                               ---------------- GET BUF DATA AREA -----------------
  p_pool = (NET_BUF_POOLS *)&NetBuf_DataPtr->BufTbl[p_if->Nbr];
  p_data = DEF_NULL;
  size_data = 0u;

  switch (transaction) {
    case NET_TRANSACTION_RX:
      if (size_len <= p_dev_cfg->RxBufLargeSize) {
        size_data = p_dev_cfg->RxBufLargeSize;
        type = NET_BUF_TYPE_RX_LARGE;
        p_mem_pool = &p_pool->RxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
        p_stat_pool = &p_pool->RxBufLargeStatPool;
#endif
        p_data = (CPU_INT08U *)Mem_DynPoolBlkGet(p_mem_pool,
                                                 p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      } else {
        NET_CTR_ERR_INC(Net_ErrCtrs.Buf.SizeCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
        goto exit;
      }
      break;

    case NET_TRANSACTION_TX:
      if ((p_dev_cfg->TxBufSmallNbr > 0)                        // If small tx bufs avail         (see Note #6a) & ...
          && (p_dev_cfg->TxBufSmallSize >= size_len)) {         // .. small tx buf  >= req'd size (see Note #6a),  ...
        size_data = p_dev_cfg->TxBufSmallSize;
        type = NET_BUF_TYPE_TX_SMALL;
        p_mem_pool = &p_pool->TxBufSmallPool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
        p_stat_pool = &p_pool->TxBufSmallStatPool;
#endif
        //                                                         .. get a small tx buf data area.
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        p_data = (CPU_INT08U *)Mem_DynPoolBlkGet(p_mem_pool,
                                                 &local_err);
      }

      if ((p_data == DEF_NULL)                                  // If small tx bufs NOT avail     (see Note #6b); OR ...
          && (p_dev_cfg->TxBufLargeNbr > 0)                     // .. large tx bufs     avail     (see Note #6b) &  ...
          && (p_dev_cfg->TxBufLargeSize >= size_len)) {         // .. large tx buf  >= req'd size (see Note #6b),   ...
        size_data = p_dev_cfg->TxBufLargeSize;
        type = NET_BUF_TYPE_TX_LARGE;
        p_mem_pool = &p_pool->TxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
        p_stat_pool = &p_pool->TxBufLargeStatPool;
#endif
        //                                                         .. get a large tx buf data area.
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        p_data = (CPU_INT08U *)Mem_DynPoolBlkGet(p_mem_pool,
                                                 &local_err);
      }

      if (size_len > size_data) {                               // If tot req'd size > avail buf size, ...
        p_data = DEF_NULL;
        NET_CTR_ERR_INC(Net_ErrCtrs.Buf.SizeCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);               // ... rtn err.
        goto exit;
      }

      if (p_data == DEF_NULL) {                                 // If NO appropriately-sized data area avail, ...
        RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);              // ... rtn err.
        goto exit;
      }
      break;

    case NET_TRANSACTION_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Buf.InvTransactionTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                     // Clr ALL buf data octets (see Note #8).
  Mem_Clr(p_data, size_data);
#endif

  //                                                               -------------- UPDATE BUF POOL STATS ---------------
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedInc(p_stat_pool, &local_err);
#endif

  //                                                               ---------------- RTN BUF DATA AREA -----------------
  *p_ix_offset = ix_offset;
  *p_data_size = size_data;
  *p_type = type;

exit:
  return (p_data);
}

/****************************************************************************************************//**
 *                                           NetBuf_GetMaxSize()
 *
 * @brief    Get maximum possible buffer allocation size starting at a specific buffer index.
 *
 * @param    if_nbr          Interface number to get maximum network buffer size.
 *
 * @param    transaction     Transaction type :
 *                               - NET_TRANSACTION_RX          Receive  transaction.
 *                               - NET_TRANSACTION_TX          Transmit transaction.
 *
 * @param    p_buf           Pointer to a network buffer.
 *
 * @param    ix_start        Requested buffer index to store buffer data.
 *
 * @return   Maximum buffer size for a specified network buffer or interface, if NO error(s).
 *           0, otherwise.
 *
 * @note     (1) Although network buffers' data area MAY be declared with an additional CPU word size
 *               (see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #2b'), this additional CPU word size
 *               does NOT increase the overall useable network buffer data area size.
 *
 * @note     (2) Since each network buffer data area allocates additional octets for its configured
 *               offset(s) [see 'net_if.c  NetIF_BufPoolInit()  Note #3'], the network buffer data
 *               area size does NOT need to be adjusted by the number of additional offset octets.
 *******************************************************************************************************/
NET_BUF_SIZE NetBuf_GetMaxSize(NET_IF_NBR      if_nbr,
                               NET_TRANSACTION transaction,
                               NET_BUF         *p_buf,
                               NET_BUF_SIZE    ix_start)
{
  NET_IF       *p_if;
  NET_BUF_HDR  *p_buf_hdr;
  NET_DEV_CFG  *p_dev_cfg;
  NET_BUF_SIZE max_size = 0u;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_buf != DEF_NULL) {                                      // Chk p_buf's max size.
    p_buf_hdr = &p_buf->Hdr;

    if (ix_start < p_buf_hdr->Size) {
      max_size = p_buf_hdr->Size - ix_start;
    }
  } else {                                                      // Else chk specific IF's cfg'd max buf size.
    p_if = NetIF_Get(if_nbr, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      max_size = 0u;
      goto exit;
    }

    p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;
    switch (transaction) {
      case NET_TRANSACTION_RX:
        if (p_dev_cfg->RxBufLargeNbr > 0u) {
          if (ix_start < p_dev_cfg->RxBufLargeSize) {
            max_size = p_dev_cfg->RxBufLargeSize - ix_start;
          }
        }
        break;

      case NET_TRANSACTION_TX:
        if (p_dev_cfg->TxBufLargeNbr > 0u) {
          if (ix_start < p_dev_cfg->TxBufLargeSize) {
            max_size = p_dev_cfg->TxBufLargeSize - ix_start;
          }
        } else if (p_dev_cfg->TxBufSmallNbr > 0u) {
          if (ix_start < p_dev_cfg->TxBufSmallSize) {
            max_size = p_dev_cfg->TxBufSmallSize - ix_start;
          }
        } else {
          ;
        }
        break;

      case NET_TRANSACTION_NONE:
      default:
        max_size = 0u;
        NET_CTR_ERR_INC(Net_ErrCtrs.Buf.InvTransactionTypeCtr);
        goto exit;
    }
  }

exit:
  return (max_size);
}

/****************************************************************************************************//**
 *                                               NetBuf_Free()
 *
 * @brief    (1) Free a network buffer :
 *               - (a) Free network   buffer
 *               - (b) Free IP option buffer                   See Note #2
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @note     (2) Since any single IP packet requires only a single network buffer to receive IP
 *               options (see 'net_ip.c  NetIP_RxPktValidate()  Note #1bC'), then no more than ONE
 *               network buffer should be linked as an IP options buffer from another buffer.
 *******************************************************************************************************/
void NetBuf_Free(NET_BUF *p_buf)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF     *p_buf_ip_opt;
#endif

  //                                                               ------------------- VALIDATE PTR -------------------
  if (p_buf == DEF_NULL) {
    return;
  }

  //                                                               ------------------ FREE NET BUF(s) -----------------
#ifdef  NET_IPv4_MODULE_EN
  p_buf_hdr = &p_buf->Hdr;
  p_buf_ip_opt = p_buf_hdr->IP_OptPtr;
#endif

  NetBuf_FreeHandler(p_buf);                                    // Free net buf.
#ifdef  NET_IPv4_MODULE_EN
  if (p_buf_ip_opt != DEF_NULL) {                               // If avail, ...
    NetBuf_FreeHandler(p_buf_ip_opt);                           // ... free IP opt buf (see Note #2).
  }
#endif
}

/****************************************************************************************************//**
 *                                               NetBuf_FreeBuf()
 *
 * @brief    Free a network buffer.
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @param    p_ctr   Pointer to possible error counter.
 *
 * @return   Number of network buffers freed.
 *
 * @note     (1) Buffers are NOT validated for 'Type' or 'USED' before freeing. #### NET-808
 *               See also 'NetBuf_FreeHandler()  Note #2'.
 *
 * @note     (2) Buffers may be referenced by multiple layers.  Therefore, the buffer's reference
 *               counter MUST be checked before freeing the buffer.
 *******************************************************************************************************/
NET_BUF_QTY NetBuf_FreeBuf(NET_BUF *p_buf,
                           NET_CTR *p_ctr)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_QTY nbr_freed;

  nbr_freed = 0u;

  if (p_buf != DEF_NULL) {
    p_buf_hdr = &p_buf->Hdr;
    if (p_buf_hdr->RefCtr > 1) {                                // If     buf ref'd by multiple layers (see Note #2), ..
      p_buf_hdr->RefCtr--;                                      // .. dec buf ref ctr.
    } else {                                                    // Else free buf.
      NetBuf_Free(p_buf);
    }

    if (p_ctr != DEF_NULL) {                                    // If avail, ...
      NET_CTR_ERR_INC(*p_ctr);                                  // ... inc err ctr.
    }

    nbr_freed++;
  }

  return (nbr_freed);
}

/****************************************************************************************************//**
 *                                           NetBuf_FreeBufList()
 *
 * @brief    Free a network buffer list.
 *           - (1) Network buffer lists are implemented as doubly-linked lists :
 *               - (a) 'p_buf_list' points to the head of the buffer list.
 *               - (b) Buffer's 'PrevBufPtr' & 'NextBufPtr' doubly-link each buffer in a buffer list.
 *                     @verbatim
 *                                   ---        Head of         -------
 *                                       ^       Buffer List  ---->|     |
 *                                       |                         |     |
 *                                       |     (see Note #1a)      |     |
 *                                       |                         |     |
 *                                       |                         |     |
 *                                       |                         -------
 *                                       |                           | ^
 *                                       |                           | |
 *                                       |                           v |
 *                                                               -------
 *                               Buffer List                    |     |
 *                               (see Note #1)                   |     |
 *                                                               |     |
 *                                       |                         |     |
 *                                       |                         |     |
 *                                       |                         -------
 *                                       |                           | ^
 *                                       |           NextBufPtr ---> | | <--- PrevBufPtr
 *                                       |         (see Note #1b)    v |    (see Note #1b)
 *                                       |                         -------
 *                                       |                         |     |
 *                                       v                         |     |
 *                                   ---                        -------
 *                     @endverbatim
 * @param    p_buf_list  Pointer to a buffer list.
 *
 * @param    p_ctr       Pointer to possible error counter.
 *
 * @return   Number of network buffers freed.
 *
 * @note     (2) Buffers are NOT validated for 'Type' or 'USED' before freeing. #### NET-808
 *               See also 'NetBuf_FreeHandler()  Note #2'.
 *
 * @note     (3) Buffers may be referenced by multiple layers.  Therefore, the buffer's reference
 *               counter MUST be checked before freeing the buffer.
 *
 * @note     (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
 *               their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed
 *               but should be unlinked in their entirety.
 *******************************************************************************************************/
NET_BUF_QTY NetBuf_FreeBufList(NET_BUF *p_buf_list,
                               NET_CTR *p_ctr)
{
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_prev;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_HDR *p_buf_prev_hdr;
  NET_BUF_QTY nbr_freed;

  p_buf = p_buf_list;
  p_buf_prev = DEF_NULL;
  nbr_freed = 0u;

  while (p_buf != DEF_NULL) {                                   // Free ALL bufs in buf list.
    p_buf_hdr = &p_buf->Hdr;
    p_buf_next = p_buf_hdr->NextBufPtr;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    p_buf_hdr->PrevPrimListPtr = DEF_NULL;
    p_buf_hdr->NextPrimListPtr = DEF_NULL;
#endif

    if (p_buf_hdr->RefCtr > 1) {                                // If     buf ref'd by multiple layers (see Note #3), ..
      p_buf_hdr->RefCtr--;                                      // .. dec buf ref ctr.
      p_buf_hdr->PrevBufPtr = p_buf_prev;
      p_buf_hdr->NextBufPtr = DEF_NULL;
      if (p_buf_prev != DEF_NULL) {                             // If prev buf non-NULL, ...
        p_buf_prev_hdr = &p_buf_prev->Hdr;
        p_buf_prev_hdr->NextBufPtr = p_buf;                     // ... set prev buf's next ptr to cur buf.
      }
      p_buf_prev = p_buf;                                       // Set cur buf as new prev buf (see Note #4).
    } else {                                                    // Else free buf.
      NetBuf_Free(p_buf);
    }

    if (p_ctr != DEF_NULL) {                                    // If avail, ...
      NET_CTR_ERR_INC(*p_ctr);                                  // ... inc err ctr.
    }

    nbr_freed++;

    p_buf = p_buf_next;
  }

  return (nbr_freed);
}

/****************************************************************************************************//**
 *                                       NetBuf_FreeBufQ_PrimList()
 *
 * @brief    Free a network buffer queue, organized by the buffers' primary buffer lists.
 *           - (1) Network buffer queues are implemented as multiply-linked lists :
 *               - (a) 'p_buf_q' points to the head of the buffer queue.
 *               - (b) Buffers are multiply-linked to form a queue of buffer lists.
 *                     In the diagram below, ... :
 *                   - (1) The top horizontal row  represents the queue of buffer lists.
 *                   - (2) Each    vertical column represents buffer fragments in the same buffer list.
 *                   - (3) Buffers' 'PrevPrimListPtr' & 'NextPrimListPtr' doubly-link each buffer list's
 *                         head buffer to form the queue of buffer lists.
 *                   - (4) Buffer's 'PrevBufPtr'      & 'NextBufPtr'      doubly-link each buffer in a
 *                         buffer list.
 *                           @verbatim
 *                                               |                                               |
 *                                               |<--------------- Buffer Queue ---------------->|
 *                                               |                (see Note #1b1)                |
 *
 *                                                            NextPrimListPtr
 *                                                            (see Note #1b3)
 *                                                                        |
 *                                                                        |
 *                       ---         Head of       -------       -------  v    -------       -------
 *                       ^            Buffer  ---->|     |------>|     |------>|     |------>|     |
 *                       |            Queue        |     |       |     |       |     |       |     |
 *                       |                         |     |<------|     |<------|     |<------|     |
 *                       |       (see Note #1a)    |     |       |     |  ^    |     |       |     |
 *                       |                         |     |       |     |  |    |     |       |     |
 *                       |                         -------       -------  |    -------       -------
 *                       |                           | ^                  |      | ^
 *                       |                           | |       PrevPrimListPtr   | |
 *                       |                           v |       (see Note #1b3)   v |
 *                       |                         -------                     -------
 *                                                 |     |                     |     |
 *               Fragments in the                  |     |                     |     |
 *               same Buffer List                  |     |                     |     |
 *               (see Note #1b2)                   |     |                     |     |
 *                                                 |     |                     |     |
 *                       |                         -------                     -------
 *                       |                           | ^                         | ^
 *                       |           NextBufPtr ---> | | <--- PrevBufPtr         | |
 *                       |        (see Note #1b4)    v |   (see Note #1b4)       v |
 *                       |                         -------                     -------
 *                       |                         |     |                     |     |
 *                       |                         |     |                     |     |
 *                       |                         |     |                     -------
 *                       |                         |     |
 *                       v                         |     |
 *                       ---                       -------
 *                           @endverbatim
 * @param    p_buf_q     Pointer to a buffer queue.
 *
 * @param    p_ctr       Pointer to possible error counter.
 *
 * @return   Number of network buffers freed.
 *
 * @note     (2) Buffers are NOT validated for 'Type' or 'USED' before freeing. #### NET-808
 *               See also 'NetBuf_FreeHandler()  Note #2'.
 *
 * @note     (3) Buffers may be referenced by multiple layers.  Therefore, the buffers' reference
 *               counters MUST be checked before freeing the buffer(s).
 *
 * @note     (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
 *               their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed
 *               but should be unlinked in their entirety.
 *******************************************************************************************************/
NET_BUF_QTY NetBuf_FreeBufQ_PrimList(NET_BUF *p_buf_q,
                                     NET_CTR *p_ctr)
{
  NET_BUF     *p_buf_list;
  NET_BUF     *p_buf_list_next;
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_prev;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_QTY nbr_freed;

  p_buf_list = p_buf_q;
  nbr_freed = 0u;

  while (p_buf_list != DEF_NULL) {                              // Free ALL buf lists in buf Q.
    p_buf_hdr = &p_buf_list->Hdr;
    p_buf_list_next = p_buf_hdr->NextPrimListPtr;
    p_buf_hdr->PrevPrimListPtr = DEF_NULL;
    p_buf_hdr->NextPrimListPtr = DEF_NULL;

    p_buf = p_buf_list;
    p_buf_prev = DEF_NULL;

    while (p_buf != DEF_NULL) {                                 // Free ALL bufs in buf list.
      p_buf_hdr = &p_buf->Hdr;
      p_buf_next = p_buf_hdr->NextBufPtr;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
      p_buf_hdr->PrevPrimListPtr = DEF_NULL;
      p_buf_hdr->NextPrimListPtr = DEF_NULL;
#endif

      if (p_buf_hdr->RefCtr > 1) {                              // If     buf ref'd by multiple layers (see Note #3), ..
        p_buf_hdr->RefCtr--;                                    // .. dec buf ref ctr.
        p_buf_hdr->PrevBufPtr = p_buf_prev;
        p_buf_hdr->NextBufPtr = DEF_NULL;
        if (p_buf_prev != DEF_NULL) {                           // If prev buf non-NULL, ...
          p_buf_hdr = &p_buf_prev->Hdr;
          p_buf_hdr->NextBufPtr = p_buf;                        // ... set prev buf's next ptr to cur buf.
        }
        p_buf_prev = p_buf;                                     // Set cur buf as new prev buf (see Note #4).
      } else {                                                  // Else free buf.
        NetBuf_Free(p_buf);
      }

      if (p_ctr != DEF_NULL) {                                  // If avail, ...
        NET_CTR_ERR_INC(*p_ctr);                                // ... inc err ctr.
      }

      nbr_freed++;

      p_buf = p_buf_next;
    }

    p_buf_list = p_buf_list_next;
  }

  return (nbr_freed);
}

/****************************************************************************************************//**
 *                                           NetBuf_FreeBufQ_SecList()
 *
 * @brief    Free a network buffer queue, organized by the buffers' secondary buffer lists.
 *           - (1) Network buffer queues are implemented as multiply-linked lists :
 *               - (a) 'p_buf_q' points to the head of the buffer queue.
 *               - (b) Buffers are multiply-linked to form a queue of buffer lists.
 *                     In the diagram below, ... :
 *                   - (1) The top horizontal row  represents the queue of buffer lists.
 *                   - (2) Each    vertical column represents buffer fragments in the same buffer list.
 *                   - (3) Buffers' 'PrevSecListPtr' & 'NextSecListPtr' doubly-link each buffer list's
 *                         head buffer to form the queue of buffer lists.
 *                   - (4) Buffer's 'PrevBufPtr'     & 'NextBufPtr'     doubly-link each buffer in a
 *                         buffer list.
 *                         @verbatim
 *                                               |                                               |
 *                                               |<--------------- Buffer Queue ---------------->|
 *                                               |                (see Note #1b1)                |
 *
 *                                                               NextSecListPtr
 *                                                           (see Note #1b3)
 *                                                                       |
 *                                                                       |
 *                       ---          Head of       -------       -------  v    -------       -------
 *                       ^            Buffer  ---->|     |------>|     |------>|     |------>|     |
 *                       |            Queue        |     |       |     |       |     |       |     |
 *                       |                         |     |<------|     |<------|     |<------|     |
 *                       |       (see Note #1a)    |     |       |     |  ^    |     |       |     |
 *                       |                         |     |       |     |  |    |     |       |     |
 *                       |                         -------       -------  |    -------       -------
 *                       |                           | ^                  |      | ^
 *                       |                           | |        PrevSecListPtr   | |
 *                       |                           v |       (see Note #1b3)   v |
 *                       |                         -------                     -------
 *                                               |     |                     |     |
 *               Fragments in the                 |     |                     |     |
 *               same Buffer List                 |     |                     |     |
 *               (see Note #1b2)                  |     |                     |     |
 *                                               |     |                     |     |
 *                       |                         -------                     -------
 *                       |                           | ^                         | ^
 *                       |           NextBufPtr ---> | | <--- PrevBufPtr         | |
 *                       |        (see Note #1b4)    v |   (see Note #1b4)       v |
 *                       |                         -------                     -------
 *                       |                         |     |                     |     |
 *                       |                         |     |                     |     |
 *                       |                         |     |                     -------
 *                       |                         |     |
 *                       v                         |     |
 *                       ---                        -------
 *                         @endverbatim
 * @param    p_buf_q         Pointer to a buffer queue.
 *
 * @param    p_ctr           Pointer to possible error counter.
 *
 * @param    p_fnct_unlink   Pointer to possible unlink function.
 *
 * @return   Number of network buffers freed.
 *
 * @note     (2) Buffers are NOT validated for 'Type' or 'USED' before freeing. #### NET-808
 **              See also 'NetBuf_FreeHandler()  Note #2'.
 *
 * @note     (3) Buffers may be referenced by multiple layers.  Therefore, the buffers' reference
 *               counters MUST be checked before freeing the buffer(s).
 *
 * @note     (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
 *               their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed
 *               but should be unlinked in their entirety.
 *
 * @note     (5) Since buffers' unlink functions are intended to unlink a buffer from a secondary
 *               buffer queue list; the secondary buffer queue list's unlink function MUST be cleared
 *               before freeing the buffer to avoid unlinking the buffer(s) from the secondary buffer
 *               queue list multiple times.
 *               See also 'NetBuf_FreeHandler()  Note #3'.
 *******************************************************************************************************/
NET_BUF_QTY NetBuf_FreeBufQ_SecList(NET_BUF      *p_buf_q,
                                    NET_CTR      *p_ctr,
                                    NET_BUF_FNCT fnct_unlink)
{
  NET_BUF     *p_buf_list;
  NET_BUF     *p_buf_list_next;
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_prev;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_QTY nbr_freed;

  p_buf_list = p_buf_q;
  nbr_freed = 0u;

  while (p_buf_list != DEF_NULL) {                              // Free ALL buf lists in buf Q.
    p_buf_hdr = &p_buf_list->Hdr;
    p_buf_list_next = p_buf_hdr->NextSecListPtr;

    p_buf = p_buf_list;
    p_buf_prev = DEF_NULL;

    while (p_buf != DEF_NULL) {                                 // Free ALL bufs in buf list.
      p_buf_hdr = &p_buf->Hdr;
      p_buf_next = p_buf_hdr->NextBufPtr;
      //                                                           Clr unlink & sec list ptrs (see Note #5).
      if (p_buf_hdr->UnlinkFnctPtr == fnct_unlink) {
        p_buf_hdr->UnlinkFnctPtr = DEF_NULL;
        p_buf_hdr->UnlinkObjPtr = DEF_NULL;
        p_buf_hdr->PrevSecListPtr = DEF_NULL;
        p_buf_hdr->NextSecListPtr = DEF_NULL;
      }

      if (p_buf_hdr->RefCtr > 1) {                              // If     buf ref'd by multiple layers (see Note #3), ..
        p_buf_hdr->RefCtr--;                                    // .. dec buf ref ctr.
        p_buf_hdr->PrevBufPtr = p_buf_prev;
        p_buf_hdr->NextBufPtr = DEF_NULL;
        if (p_buf_prev != DEF_NULL) {                           // If prev buf non-NULL, ...
          p_buf_hdr = &p_buf_prev->Hdr;
          p_buf_hdr->NextBufPtr = p_buf;                        // ... set prev buf's next ptr to cur buf.
        }
        p_buf_prev = p_buf;                                     // Set cur buf as new prev buf (see Note #4).
      } else {                                                  // Else free buf.
        NetBuf_Free(p_buf);
      }

      if (p_ctr != DEF_NULL) {                                  // If avail, ...
        NET_CTR_ERR_INC(*p_ctr);                                // ... inc err ctr.
      }

      nbr_freed++;

      p_buf = p_buf_next;
    }

    p_buf_list = p_buf_list_next;
  }

  return (nbr_freed);
}

/****************************************************************************************************//**
 *                                       NetBuf_FreeBufDataAreaRx()
 *
 * @brief    Free a receive network buffer data area.
 *
 * @param    if_nbr      Network interface number freeing network buffer data area.
 *
 * @param    p_buf_data  Pointer to network buffer data area to free.
 *******************************************************************************************************/
void NetBuf_FreeBufDataAreaRx(NET_IF_NBR if_nbr,
                              CPU_INT08U *p_buf_data)
{
  NET_BUF_POOLS *p_pool;
  MEM_DYN_POOL  *p_mem_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL *p_stat_pool;
#endif
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------ VALIDATE PTR --------------------
  if (p_buf_data == DEF_NULL) {
    return;
  }

  //                                                               -------------- FREE RX BUF DATA AREA ---------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  p_mem_pool = &p_pool->RxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  p_stat_pool = &p_pool->RxBufLargeStatPool;
#endif

  Mem_DynPoolBlkFree(p_mem_pool,
                     p_buf_data,
                     &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)                       // If buf data area freed to pool, ...
  NetStat_PoolEntryUsedDec(p_stat_pool, &local_err);            // ... update buf pool stats.
#endif
}

/****************************************************************************************************//**
 *                                               NetBuf_DataRd()
 *
 * @brief    (1) Read data from network buffer's DATA area :
 *               - (a) Validate data read index & size
 *               - (b) Read     data from buffer
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @param    ix      Index into buffer's DATA area.
 *
 * @param    len     Number of octets to read (see Note #2).
 *
 * @param    p_dest  Pointer to destination to read data into (see Note #3).
 *
 * @note     (2) Data read of 0 octets allowed.
 *
 * @note     (3) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
 *
 * @note     (4) 'ix' & 'len' argument check NOT required unless 'NET_BUF_SIZE's native data type
 *               'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
 *
 * @note     (5) Buffer 'Size' is NOT re-validated; validated in NetBuf_Get().
 *******************************************************************************************************/
void NetBuf_DataRd(NET_BUF      *p_buf,
                   NET_BUF_SIZE ix,
                   NET_BUF_SIZE len,
                   CPU_INT08U   *p_dest)
{
  CPU_INT08U *p_data;

  if (len < 1) {                                                // If req'd len = 0,    rtn null rd (see Note #2).
    goto exit;
  }

  //                                                               ------------------- RD BUF DATA --------------------
  //                                                               Req'd ix & len within  buf DATA area; ...
  p_data = &p_buf->DataPtr[ix];                                 // ... set ptr to ix into buf DATA area, ...
  Mem_Copy(p_dest,                                              // ... & copy len nbr DATA buf octets to dest.
           p_data,
           len);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetBuf_DataWr()
 *
 * @brief    (1) Write data into network buffer's DATA area :
 *               - (a) Validate data write index & size
 *               - (b) Write    data into buffer
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @param    ix      Index into buffer's DATA area.
 *
 * @param    len     Number of octets to write (see Note #2).
 *
 * @param    p_src   Pointer to data to write.
 *
 * @note     (2) Data write of 0 octets allowed.
 *
 * @note     (3) 'ix' & 'len' argument check NOT required unless 'NET_BUF_SIZE's native data type
 *               'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
 *
 * @note     (4) Buffer 'Size' is NOT re-validated; validated in NetBuf_Get().
 *******************************************************************************************************/
void NetBuf_DataWr(NET_BUF      *p_buf,
                   NET_BUF_SIZE ix,
                   NET_BUF_SIZE len,
                   CPU_INT08U   *p_src)
{
  CPU_INT08U *p_data;

  if (len < 1) {                                                // If req'd len = 0,    rtn null wr (see Note #2).
    goto exit;
  }

  //                                                               ------------------- WR BUF DATA --------------------
  //                                                               Req'd ix & len within  buf DATA area; ...
  p_data = &p_buf->DataPtr[ix];                                 // ... set ptr to ix into buf DATA area, ...
  Mem_Copy(p_data,                                              // ... & copy len nbr src octets into DATA buf.
           p_src,
           len);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetBuf_DataCopy()
 *
 * @brief    (1) Copy data from one network buffer's DATA area to another network buffer's DATA area :
 *               - (a) Validate data copy indices & sizes
 *               - (b) Copy     data between buffers
 *
 * @param    p_buf_dest  Pointer to destination network buffer.
 *
 * @param    p_buf_src   Pointer to source      network buffer.
 *
 * @param    ix_dest     Index into destination buffer's DATA area.
 *
 * @param    ix_src      Index into source      buffer's DATA area.
 *
 * @param    len         Number of octets to copy (see Note #2).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Data copy of 0 octets allowed.
 *
 * @note     (3) 'ix_&&&' & 'len' argument check NOT required unless 'NET_BUF_SIZE's native data type
 *               'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
 *
 * @note     (4) Buffer 'Size's are NOT re-validated; validated in NetBuf_Get().
 *******************************************************************************************************/
void NetBuf_DataCopy(NET_BUF      *p_buf_dest,
                     NET_BUF      *p_buf_src,
                     NET_BUF_SIZE ix_dest,
                     NET_BUF_SIZE ix_src,
                     NET_BUF_SIZE len,
                     RTOS_ERR     *p_err)
{
  CPU_INT08U *p_data_dest;
  CPU_INT08U *p_data_src;

  //                                                               --------------- VALIDATE IX's/SIZES ----------------
  if (len < 1) {                                                // If req'd len  = 0,    rtn null copy (see Note #2).
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  //                                                               ------------------ COPY BUF DATA -------------------
  //                                                               Req'd ix's & len within buf DATA areas; ...
  p_data_dest = &p_buf_dest->DataPtr[ix_dest];                  // ... set ptrs to ix into buf DATA areas, ...
  p_data_src = &p_buf_src->DataPtr[ix_src];
  Mem_Copy(p_data_dest,                                         // ... & copy len nbr DATA buf octets      ...
           p_data_src,                                          // ... from src to dest buf.
           len);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetBuf_IsUsed()
 *
 * @brief    Validate buffer in use.
 *
 * @param    p_buf   Pointer to object to validate as a network buffer in use.
 *
 * @return   DEF_YES, buffer   valid &      in use.
 *           DEF_NO,  buffer invalid or NOT in use.
 *
 * @note     (1) NetBuf_IsUsed() MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetBuf_IsUsed(NET_BUF *p_buf)
{
  NET_BUF_HDR *p_buf_hdr = DEF_NULL;
  CPU_BOOLEAN used = DEF_NO;

  //                                                               ------------------ VALIDATE PTR --------------------
  if (p_buf == DEF_NULL) {
    goto exit;
  }
  //                                                               ------------------ VALIDATE TYPE -------------------
  p_buf_hdr = &p_buf->Hdr;
  switch (p_buf_hdr->Type) {
    case NET_BUF_TYPE_RX_LARGE:
    case NET_BUF_TYPE_TX_LARGE:
    case NET_BUF_TYPE_TX_SMALL:
      break;

    case NET_BUF_TYPE_NONE:
    default:
      goto exit;
  }

  //                                                               ---------------- VALIDATE BUF USED -----------------
  used = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_USED);

exit:
  return (used);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetBuf_FreeHandler()
 *
 * @brief    (1) Free a network buffer :
 *               - (a) Configure buffer free by buffer type
 *               - (b) Unlink    buffer from network layer(s)                          See Note #3
 *               - (c) Clear     buffer controls
 *               - (d) Free      buffer & data area back to buffer pools
 *               - (e) Update    buffer pool statistics
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @note     (2) #### To prevent freeing a buffer already freed via auxiliary pointer(s),
 *               NetBuf_FreeHandler() checks the buffer's 'USED' flag BEFORE freeing the buffer.
 *               @n
 *               This prevention is only best-effort since any invalid duplicate  buffer frees MAY be
 *               asynchronous to potentially valid buffer gets.  Thus the invalid buffer free(s) MAY
 *               corrupt the buffer's valid operation(s).
 *               @n
 *               However, since the primary tasks of the network protocol suite are prevented from
 *               running concurrently (see 'net.h  Note #3'), it is NOT necessary to protect network
 *               buffer resources from possible corruption since no asynchronous access from other
 *               network tasks is possible.
 *
 * @note     (3) If a network buffer's unlink function is available, it is assumed that the function
 *               correctly unlinks the network buffer from any other network layer(s).
 *******************************************************************************************************/
static void NetBuf_FreeHandler(NET_BUF *p_buf)
{
  CPU_BOOLEAN   used;
  NET_IF_NBR    if_nbr;
  NET_BUF_HDR   *p_buf_hdr;
  NET_BUF_POOLS *p_pool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL *p_stat_pool = DEF_NULL;
#endif
  MEM_DYN_POOL *p_mem_pool = DEF_NULL;
  NET_BUF_FNCT unlink_fnct;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ---------------- VALIDATE BUF USED -----------------
  used = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_USED);
  if (used != DEF_YES) {                                        // If buf NOT used, ...
    NET_CTR_ERR_INC(Net_ErrCtrs.Buf.NotUsedCtr);
    goto exit;                                                  // ... rtn but do NOT free (see Note #2).
  }

  if_nbr = p_buf_hdr->IF_Nbr;

  //                                                               ------------------- CFG BUF FREE -------------------
  p_pool = &NetBuf_DataPtr->BufTbl[if_nbr];
  switch (p_buf_hdr->Type) {
    case NET_BUF_TYPE_RX_LARGE:
      p_mem_pool = &p_pool->RxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->RxBufLargeStatPool;
#endif
      break;

    case NET_BUF_TYPE_TX_LARGE:
      p_mem_pool = &p_pool->TxBufLargePool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->TxBufLargeStatPool;
#endif
      break;

    case NET_BUF_TYPE_TX_SMALL:
      p_mem_pool = &p_pool->TxBufSmallPool;
#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
      p_stat_pool = &p_pool->TxBufSmallStatPool;
#endif
      break;

    case NET_BUF_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Buf.InvTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               -------------------- UNLINK BUF --------------------
  unlink_fnct = p_buf_hdr->UnlinkFnctPtr;
  if (unlink_fnct != DEF_NULL) {                                // If unlink fnct avail, ..
    unlink_fnct(p_buf);                                         // .. unlink buf from other layer(s) [see Note #3].
  }

  //                                                               ---------------------- CLR BUF ---------------------
  //                                                               Set buf as NOT used.
  DEF_BIT_CLR(p_buf_hdr->Flags, (NET_BUF_FLAGS)NET_BUF_FLAG_USED);

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetBuf_ClrHdr(p_buf_hdr);
#endif

  //                                                               -------------- FREE NET BUF DATA AREA --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  Mem_DynPoolBlkFree(p_mem_pool,
                     p_buf->DataPtr,
                     &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)                       // If buf data area freed to pool, ...
  NetStat_PoolEntryUsedDec(p_stat_pool, &local_err);            // ... update buf pool stats.
#endif

  //                                                               ------------------- FREE NET BUF -------------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  Mem_DynPoolBlkFree(&p_pool->NetBufPool,
                     p_buf,
                     &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)                       // If buf freed to pool,      ...
  NetStat_PoolEntryUsedDec(&p_pool->NetBufStatPool, &local_err);   // ... update buf pool stats.
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetBuf_ClrHdr()
 *
 * @brief    Clear network buffer header controls.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *******************************************************************************************************/
static void NetBuf_ClrHdr(NET_BUF_HDR *p_buf_hdr)
{
  p_buf_hdr->Type = NET_BUF_TYPE_NONE;
  p_buf_hdr->Size = 0u;
  p_buf_hdr->Flags = NET_BUF_FLAG_NONE;

  p_buf_hdr->RefCtr = 0u;
  p_buf_hdr->ID = NET_BUF_ID_NONE;

  p_buf_hdr->IF_Nbr = NET_IF_NBR_NONE;
  p_buf_hdr->IF_NbrTx = NET_IF_NBR_NONE;

  p_buf_hdr->PrevPrimListPtr = DEF_NULL;
  p_buf_hdr->NextPrimListPtr = DEF_NULL;
  p_buf_hdr->PrevSecListPtr = DEF_NULL;
  p_buf_hdr->NextSecListPtr = DEF_NULL;
  p_buf_hdr->PrevTxListPtr = DEF_NULL;
  p_buf_hdr->NextTxListPtr = DEF_NULL;
  p_buf_hdr->PrevBufPtr = DEF_NULL;
  p_buf_hdr->NextBufPtr = DEF_NULL;

  if (p_buf_hdr->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_buf_hdr->TmrPtr);
  }
  p_buf_hdr->TmrPtr = DEF_NULL;

  p_buf_hdr->UnlinkFnctPtr = DEF_NULL;
  p_buf_hdr->UnlinkObjPtr = DEF_NULL;

  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_NONE;
  p_buf_hdr->ProtocolHdrTypeIF = NET_PROTOCOL_TYPE_NONE;
  p_buf_hdr->ProtocolHdrTypeIF_Sub = NET_PROTOCOL_TYPE_NONE;
  p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_NONE;
  p_buf_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_NONE;
  p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_NONE;

  p_buf_hdr->IF_HdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IF_HdrLen = 0u;
#ifdef  NET_ARP_MODULE_EN
  p_buf_hdr->ARP_MsgIx = NET_BUF_IX_NONE;
  p_buf_hdr->ARP_MsgLen = 0u;
#endif
  p_buf_hdr->IP_HdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IP_HdrLen = 0u;

  p_buf_hdr->ICMP_MsgIx = NET_BUF_IX_NONE;
  p_buf_hdr->ICMP_MsgLen = 0u;
  p_buf_hdr->ICMP_HdrLen = 0u;
#ifdef  NET_IGMP_MODULE_EN
  p_buf_hdr->IGMP_MsgIx = NET_BUF_IX_NONE;
  p_buf_hdr->IGMP_MsgLen = 0u;
#endif
  p_buf_hdr->TransportHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->TransportHdrLen = 0u;
  p_buf_hdr->TransportTotLen = 0u;
  p_buf_hdr->TransportDataLen = 0u;
  p_buf_hdr->DataIx = NET_BUF_IX_NONE;
  p_buf_hdr->DataLen = 0u;
  p_buf_hdr->TotLen = 0u;

#ifdef  NET_ARP_MODULE_EN
  p_buf_hdr->ARP_AddrHW_Ptr = DEF_NULL;
  p_buf_hdr->ARP_AddrProtocolPtr = DEF_NULL;
#endif

  p_buf_hdr->IP_TotLen = 0u;
  p_buf_hdr->IP_DataLen = 0u;
  p_buf_hdr->IP_DatagramLen = 0u;
  p_buf_hdr->IP_FragSizeTot = NET_IP_FRAG_SIZE_NONE;
  p_buf_hdr->IP_FragSizeCur = 0u;

#ifdef  NET_IPv4_MODULE_EN
  p_buf_hdr->IP_Flags_FragOffset = NET_IPv4_HDR_FLAG_NONE | NET_IPv4_HDR_FRAG_OFFSET_NONE;
  p_buf_hdr->IP_ID = NET_IPv4_ID_NONE;
  p_buf_hdr->IP_AddrSrc = NET_IPv4_ADDR_NONE;
  p_buf_hdr->IP_AddrDest = NET_IPv4_ADDR_NONE;
  p_buf_hdr->IP_AddrNextRoute = NET_IPv4_ADDR_NONE;
  p_buf_hdr->IP_AddrNextRouteNetOrder = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_NONE);
  p_buf_hdr->IP_OptPtr = DEF_NULL;
#endif
#ifdef  NET_IPv6_MODULE_EN
  p_buf_hdr->IPv6_Flags_FragOffset = NET_IPv6_FRAG_NONE;
  Mem_Clr(&p_buf_hdr->IPv6_AddrSrc, NET_IPv6_ADDR_SIZE);
  Mem_Clr(&p_buf_hdr->IPv6_AddrDest, NET_IPv6_ADDR_SIZE);
  Mem_Clr(&p_buf_hdr->IPv6_AddrNextRoute, NET_IPv6_ADDR_SIZE);
  p_buf_hdr->IP_HdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_ExtHdrLen = 0u;
  p_buf_hdr->IPv6_HopByHopHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_RoutingHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_FragHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_ESP_HdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_AuthHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_DestHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_MobilityHdrIx = NET_BUF_IX_NONE;
  p_buf_hdr->IPv6_ID = 0u;
#endif

#ifdef  NET_NDP_MODULE_EN
  p_buf_hdr->NDP_AddrHW_Ptr = DEF_NULL;
  p_buf_hdr->NDP_AddrProtocolPtr = DEF_NULL;
#endif

  p_buf_hdr->TransportPortSrc = NET_PORT_NBR_NONE;
  p_buf_hdr->TransportPortDest = NET_PORT_NBR_NONE;

#ifdef  NET_TCP_MODULE_EN
  p_buf_hdr->TCP_HdrLen_Flags = NET_TCP_HDR_LEN_NONE | NET_TCP_HDR_FLAG_NONE;
  p_buf_hdr->TCP_SegLenInit = 0u;
  p_buf_hdr->TCP_SegLenLast = 0u;
  p_buf_hdr->TCP_SegLen = 0u;
  p_buf_hdr->TCP_SegLenData = 0u;
  p_buf_hdr->TCP_SegReTxCtr = 0u;
  p_buf_hdr->TCP_SegSync = DEF_NO;
  p_buf_hdr->TCP_SegClose = DEF_NO;
  p_buf_hdr->TCP_SegReset = DEF_NO;
  p_buf_hdr->TCP_SegAck = DEF_NO;
  p_buf_hdr->TCP_SegAckTxd = DEF_NO;
  p_buf_hdr->TCP_SegAckTxReqCode = NET_TCP_CONN_TX_ACK_NONE;
  p_buf_hdr->TCP_SeqNbrInit = NET_TCP_SEQ_NBR_NONE;
  p_buf_hdr->TCP_SeqNbrLast = NET_TCP_SEQ_NBR_NONE;
  p_buf_hdr->TCP_SeqNbr = NET_TCP_SEQ_NBR_NONE;
  p_buf_hdr->TCP_AckNbr = NET_TCP_ACK_NBR_NONE;
  p_buf_hdr->TCP_AckNbrLast = NET_TCP_ACK_NBR_NONE;
  p_buf_hdr->TCP_MaxSegSize = NET_TCP_MAX_SEG_SIZE_NONE;
  p_buf_hdr->TCP_WinSize = NET_TCP_WIN_SIZE_NONE;
  p_buf_hdr->TCP_WinSizeLast = NET_TCP_WIN_SIZE_NONE;
  p_buf_hdr->TCP_RTT_TS_Rxd_ms = NET_TCP_TX_RTT_TS_NONE;
  p_buf_hdr->TCP_RTT_TS_Txd_ms = NET_TCP_TX_RTT_TS_NONE;
  p_buf_hdr->TCP_Flags = NET_TCP_FLAG_NONE;
#endif

  p_buf_hdr->Conn_ID = NET_CONN_ID_NONE;
  p_buf_hdr->Conn_ID_Transport = NET_CONN_ID_NONE;
  p_buf_hdr->Conn_ID_App = NET_CONN_ID_NONE;
  p_buf_hdr->ConnType = NET_CONN_TYPE_CONN_NONE;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_BUF_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
