/***************************************************************************//**
 * @file
 * @brief Network Dad Layer - (Duplication Address Detection)
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

#ifdef  NET_DAD_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "net_dad_priv.h"
#include  "net_ndp_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

#define  NET_DAD_OBJ_POOL_NAME                  "Net DAD Object Pool"
#define  NET_DAD_SIGNAL_ERR_NAME                "Net DAD Signal Err"
#define  NET_DAD_SIGNAL_COMPL_NAME              "Net DAD Signal Complete"

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL NetDAD_Pool;
static NET_DAD_OBJ  *NetDAD_ObjListHeadPtr;
static NET_DAD_OBJ  *NetDAD_ObjListEndPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetDAD_Init()
 *
 * @brief    Initialize DAD module
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetDAD_Init(RTOS_ERR *p_err)
{
  //                                                               -------- CREATE DYNAMIC POOL FOR DAD OBJECT --------
  Mem_DynPoolCreate(NET_DAD_OBJ_POOL_NAME,
                    &NetDAD_Pool,
                    DEF_NULL,
                    sizeof(NET_DAD_OBJ),
                    sizeof(CPU_DATA),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
  }
}

/****************************************************************************************************//**
 *                                               NetDAD_Start()
 *
 * @brief    (1) Start Duplication Address Detection (DAD) procedure :
 *               - (a) Validate that DAD is enabled.
 *               - (b) Validate that IPv6 addrs object for addr to configured exists.
 *               - (c) Get a new DAD object in pool.
 *               - (d) Update DAD object parameters.
 *               - (e) Start NDP DAD process.
 *
 * @param    if_nbr          Network interface number.
 *
 * @param    p_addr          Pointer to the IPv6 addr to perform duplicate address detection.
 *
 * @param    addr_cfg_type   Type of Address Configuration :
 *                               - NET_IPV6_ADDR_CFG_TYPE_STATIC_BLOKING
 *                               - NET_IPV6_ADDR_CFG_TYPE_STATIC_NO_BLOKING
 *                               - NET_IPV6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING
 *                               - NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO
 *
 * @param    dad_hook_fnct   Pointer to callback function after DAD process is done.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
NET_DAD_STATUS NetDAD_Start(NET_IF_NBR             if_nbr,
                            NET_IPv6_ADDR          *p_addr,
                            NET_IPv6_ADDR_CFG_TYPE addr_cfg_type,
                            NET_DAD_FNCT           dad_hook_fnct,
                            RTOS_ERR               *p_err)
{
  NET_DAD_OBJ         *p_dad_obj;
  NET_IPv6_ADDR_OBJ   *p_addr_obj;
  CPU_INT08U          dad_retx_nbr;
  NET_IPv6_ADDR_STATE state;
  NET_DAD_STATUS      status = NET_DAD_STATUS_NONE;

  //                                                               ----------- VALIDATE THAT DAD IS ENABLED -----------
  dad_retx_nbr = NetNDP_DAD_GetMaxAttemptsNbr();
  if (dad_retx_nbr <= 0) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    goto exit;
  }

  //                                                               -------------- RECOVER IPV6 ADDRS OBJ --------------
  p_addr_obj = NetIPv6_GetAddrsHostOnIF(if_nbr, p_addr);
  if (p_addr_obj == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  //                                                               --------------- GET A NEW DAD OBJECT ---------------
  p_dad_obj = NetDAD_ObjGet(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ----------- UPDATE DAD OBJECT PARAMETERS -----------
  Mem_Copy(&p_dad_obj->Addr, p_addr, NET_IPv6_ADDR_SIZE);

  p_dad_obj->Fnct = dad_hook_fnct;

  switch (addr_cfg_type) {
    case NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING:
      p_dad_obj->NotifyComplEn = DEF_YES;
      break;

    case NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING:
      p_dad_obj->NotifyComplEn = DEF_NO;
      break;

    case NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING:
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
      p_dad_obj->NotifyComplEn = DEF_NO;
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_DAD_STATUS_NONE);
#endif
      break;

    case NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO:
      p_dad_obj->NotifyComplEn = DEF_NO;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_DAD_STATUS_NONE);
  }

  //                                                               ---------- CREATE NEW NDP CACHE FOR ADDR -----------
  NetNDP_DAD_Start(if_nbr, p_addr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               ---- WAIT FOR DAD SIGNAL COMPLETE IF BLOCKING EN ---
  if (addr_cfg_type == NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING) {
    Net_GlobalLockRelease();

    NetDAD_SignalWait(NET_DAD_SIGNAL_TYPE_COMPL, p_dad_obj, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_relock;
    }

    Net_GlobalLockAcquire((void *)NetDAD_Start);

    NetDAD_Stop(if_nbr, p_dad_obj);

    state = p_addr_obj->AddrState;
    switch (state) {
      case NET_IPv6_ADDR_STATE_PREFERRED:
      case NET_IPv6_ADDR_STATE_DEPRECATED:
        status = NET_DAD_STATUS_SUCCEED;
        break;

      case NET_IPv6_ADDR_STATE_DUPLICATED:
      case NET_IPv6_ADDR_STATE_TENTATIVE:
      case NET_IPv6_ADDR_STATE_NONE:
        status = NET_DAD_STATUS_FAIL;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_DAD_STATUS_NONE)
    }
  } else {
    status = NET_DAD_STATUS_IN_PROGRESS;
  }

  goto exit;

exit_relock:
  Net_GlobalLockAcquire((void *)NetDAD_Start);

exit_release:
  NetDAD_Stop(if_nbr, p_dad_obj);

exit:
  return (status);
}

/****************************************************************************************************//**
 *                                               NetDAD_Stop()
 *
 * @brief    Stop the current running DAD process.
 *
 * @param    if_nbr      Interface number of the address on which DAD is occurring.
 *
 * @param    p_dad_obj   Pointer to the current DAD object.
 *******************************************************************************************************/
void NetDAD_Stop(NET_IF_NBR  if_nbr,
                 NET_DAD_OBJ *p_dad_obj)
{
  RTOS_ERR local_err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_dad_obj->Fnct = DEF_NULL;
  p_dad_obj->NotifyComplEn = DEF_NO;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemSet(p_dad_obj->SignalCompl, 0, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  KAL_SemSet(p_dad_obj->SignalErr, 0, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  NetNDP_DAD_Stop(if_nbr, &p_dad_obj->Addr);

  NetDAD_ObjRelease(p_dad_obj);
}

/****************************************************************************************************//**
 *                                               NetDAD_ObjGet()
 *
 * @brief    Obtain a DAD object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to new DAD object created.
 *
 * @note     (1) NetDAD_ObjGet() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_DAD_OBJ *NetDAD_ObjGet(RTOS_ERR *p_err)
{
  NET_DAD_OBJ *p_obj;

  //                                                               ------------ GET DAD OBJ FROM DYN POOL -------------
  p_obj = (NET_DAD_OBJ *)Mem_DynPoolBlkGet(&NetDAD_Pool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------ INIT DAD OBJ SIGNAL ERROR -------------
  p_obj->SignalErr = KAL_SemCreate(NET_DAD_SIGNAL_ERR_NAME,
                                   DEF_NULL,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               ----------- INIT DAD OBJ SIGNAL COMPLETE -----------
  p_obj->SignalCompl = KAL_SemCreate(NET_DAD_SIGNAL_COMPL_NAME,
                                     DEF_NULL,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    KAL_SemDel(p_obj->SignalErr);
    goto exit_release;
  }

  //                                                               ---------- INIT OTHER DAD OBJ PARAMETERS ----------
  NetIPv6_AddrUnspecifiedSet(&p_obj->Addr);
  p_obj->NotifyComplEn = DEF_NO;
  p_obj->Fnct = DEF_NULL;

  //                                                               --------------- UPDATE DAD OBJ LIST ----------------
  if (NetDAD_ObjListHeadPtr == DEF_NULL) {
    NetDAD_ObjListHeadPtr = p_obj;
    NetDAD_ObjListEndPtr = p_obj;
  } else {
    NetDAD_ObjListEndPtr->NextPtr = p_obj;
    NetDAD_ObjListEndPtr = p_obj;
  }

  goto exit;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    Mem_DynPoolBlkFree(&NetDAD_Pool, p_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
    p_obj = DEF_NULL;
  }

exit:
  return (p_obj);
}

/****************************************************************************************************//**
 *                                           NetDAD_ObjRelease()
 *
 * @brief    Release DAD object.
 *
 * @param    p_dad_obj   Pointer to DAD object to release.
 *
 * @note     (1) NetDAD_ObjRelease() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
void NetDAD_ObjRelease(NET_DAD_OBJ *p_dad_obj)
{
  NET_DAD_OBJ *p_obj_prev;
  NET_DAD_OBJ *p_obj;
  CPU_BOOLEAN found;

  //                                                               --------------- UPDATE DAD OBJ LIST ----------------
  found = DEF_NO;
  p_obj_prev = DEF_NULL;
  p_obj = NetDAD_ObjListHeadPtr;
  while (p_obj != DEF_NULL) {
    if (p_obj == p_dad_obj) {
      if (p_obj == NetDAD_ObjListHeadPtr) {
        NetDAD_ObjListHeadPtr = p_obj->NextPtr;

        if (NetDAD_ObjListEndPtr == p_obj) {
          NetDAD_ObjListHeadPtr = DEF_NULL;
          NetDAD_ObjListEndPtr = DEF_NULL;
        }
      } else if (p_obj == NetDAD_ObjListEndPtr) {
        NetDAD_ObjListEndPtr = p_obj_prev;
      } else {
        p_obj_prev->NextPtr = p_obj->NextPtr;
      }

      found = DEF_YES;
      break;
    }
    p_obj_prev = p_obj;
    p_obj = p_obj->NextPtr;
  }

  if (found == DEF_YES) {
    RTOS_ERR local_err;

    //                                                             ------------- RELEASE KAL SEMAPHORES --------------
    KAL_SemDel(p_dad_obj->SignalCompl);
    p_dad_obj->SignalCompl.SemObjPtr = DEF_NULL;

    KAL_SemDel(p_dad_obj->SignalErr);
    p_dad_obj->SignalErr.SemObjPtr = DEF_NULL;

    //                                                             ------------------- FREE DAD OBJ -------------------
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(&NetDAD_Pool, p_dad_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
  }
}

/****************************************************************************************************//**
 *                                           NetDAD_ObjSrch()
 *
 * @brief    Search DAD object with specific IPv6 address in DAD object list.
 *
 * @param    p_addr      Pointer to IPv6 address
 *
 * @note     (1) NetDAD_ObjSrch() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_DAD_OBJ *NetDAD_ObjSrch(NET_IPv6_ADDR *p_addr)
{
  NET_DAD_OBJ *p_obj = DEF_NULL;
  CPU_BOOLEAN identical = DEF_NO;

  p_obj = NetDAD_ObjListHeadPtr;

  while (p_obj != DEF_NULL) {
    identical = NetIPv6_IsAddrsIdentical(p_addr, &p_obj->Addr);
    if (identical == DEF_YES) {
      break;
    }

    p_obj = p_obj->NextPtr;
  }

  return (p_obj);
}

/****************************************************************************************************//**
 *                                           NetDAD_SignalWait()
 *
 * @brief    Wait for a NDP DAD signal.
 *
 * @param    signal_type     DAD signal type :
 *                               - NET_DAD_SIGNAL_TYPE_ERR
 *                               - NET_DAD_SIGNAL_TYPE_COMPL
 *
 * @param    p_dad_obj       Pointer to current DAD object.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetIPv6_DAD_SignalWait() is called by network protocol suite function(s) BUT
 *               MUST be called with the global network lock NOT acquired.
 *******************************************************************************************************/
void NetDAD_SignalWait(NET_DAD_SIGNAL_TYPE signal_type,
                       NET_DAD_OBJ         *p_dad_obj,
                       RTOS_ERR            *p_err)
{
  switch (signal_type) {
    case NET_DAD_SIGNAL_TYPE_ERR:
      KAL_SemPend(p_dad_obj->SignalErr, KAL_OPT_PEND_NON_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
        goto exit;
      }
      break;

    case NET_DAD_SIGNAL_TYPE_COMPL:
      KAL_SemPend(p_dad_obj->SignalCompl, KAL_OPT_PEND_BLOCKING, NET_IPv6_DAD_SIGNAL_TIMEOUT_MS, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
        goto exit;
      }
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetDAD_Signal()
 *
 * @brief    Post a IPv6 DAD signal.
 *
 * @param    signal_type     DAD signal type :
 *                               - NET_DAD_SIGNAL_TYPE_ERR
 *                               - NET_DAD_SIGNAL_TYPE_COMPL
 *
 * @param    p_dad_obj       Pointer to the current DAD object.
 *
 * @note     (1) NetDAD_Signal() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
void NetDAD_Signal(NET_DAD_SIGNAL_TYPE signal_type,
                   NET_DAD_OBJ         *p_dad_obj)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  switch (signal_type) {
    //                                                             --------------- POST DAD SIGNAL ERR ----------------
    case NET_DAD_SIGNAL_TYPE_ERR:
      KAL_SemPost(p_dad_obj->SignalErr, KAL_OPT_PEND_NONE, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      break;

    case NET_DAD_SIGNAL_TYPE_COMPL:
      if (p_dad_obj->NotifyComplEn == DEF_YES) {
        KAL_SemPost(p_dad_obj->SignalCompl, KAL_OPT_PEND_NONE, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DAD_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
