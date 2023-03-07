/***************************************************************************//**
 * @file
 * @brief Common - Platform Manager
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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_def.h>
#include  <common/include/platform_mgr.h>

#include  <common/source/collections/slist_priv.h>
#include  <common/source/common/common_priv.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (COMMON, PLATFORM_MGR)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

#define  PLATFORM_MGR_INIT_CFG_DFLT         {  \
    .PoolBlkQtyInit = 0u,                      \
    .PoolBlkQtyMax = LIB_MEM_BLK_QTY_UNLIMITED \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct platform_mgr_data {
  SLIST_MEMBER    *PlatformMgrListHeadPtr;
  MEM_DYN_POOL    Pool;
  KAL_LOCK_HANDLE LockHandle;
} PLATFORM_MGR_DATA;

typedef struct platform_mgr_list_adapter {
  SLIST_MEMBER            ListNode;
  const PLATFORM_MGR_ITEM *ItemPtr;
} PLATFORM_MGR_LIST_ADAPTER;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static PLATFORM_MGR_DATA *PlatformMgrPtr;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const PLATFORM_MGR_INIT_CFG  PlatformMgr_InitCfgDflt = PLATFORM_MGR_INIT_CFG_DFLT;
static PLATFORM_MGR_INIT_CFG PlatformMgr_InitCfg = PLATFORM_MGR_INIT_CFG_DFLT;
#else
extern const PLATFORM_MGR_INIT_CFG PlatformMgr_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static PLATFORM_MGR_LIST_ADAPTER *PlatformMgrInternalGet(PLATFORM_MGR_PREDICATE predicate_fnct,
                                                         void                   *p_predicate_data,
                                                         RTOS_ERR               *p_err);

static CPU_BOOLEAN PlatformMgrGetByNamePredicate(const PLATFORM_MGR_ITEM *p_item,
                                                 void                    *p_predicate_data);

static CPU_BOOLEAN PlatformMgrGetByItemPtrPredicate(const PLATFORM_MGR_ITEM *p_item,
                                                    void                    *p_predicate_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           PlatformMgQtyConfigure()
 *
 * @brief    Configure the quantities that are/can be allocated by the platform manager.
 *
 * @param    p_cfg   Pointer to the structure containing the platform manager quantity configuration.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Common_Init().
 *               If it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void PlatformMgQtyConfigure(PLATFORM_MGR_INIT_CFG *p_cfg)
{
  RTOS_ASSERT_DBG((PlatformMgrPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG((p_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  PlatformMgr_InitCfg = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                           PlatformMgrItemInitAdd()
 *
 * @brief    Add item to the platform manager.
 *
 * @param    p_item  Pointer to the item to add.
 *
 * @note     (1) This function does not return an error and instead asserts if adding the item failed.
 *               It is intended to be used during initialization only.
 *******************************************************************************************************/
void PlatformMgrItemInitAdd(const PLATFORM_MGR_ITEM *p_item)
{
  RTOS_ERR err;

  PlatformMgrItemAdd(p_item, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err),; );
}

/****************************************************************************************************//**
 *                                       PlatformMgrItemGetByName()
 *
 * @brief    Get item with the specified 'id_str' from the platform manager.
 *
 * @param    id_str  String with the ID of the item to get.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Pointer to item, if found.
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
PLATFORM_MGR_ITEM *PlatformMgrItemGetByName(const CPU_CHAR *id_str,
                                            RTOS_ERR       *p_err)
{
  PLATFORM_MGR_ITEM *p_ret_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(id_str != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  p_ret_item = PlatformMgrItemGet(PlatformMgrGetByNamePredicate, (void *)id_str, p_err);

  return (p_ret_item);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               PlatformMgrInit()
 *
 * @brief    Initialize the platform manager sub-module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void PlatformMgrInit(RTOS_ERR *p_err)
{
  MEM_SEG     *p_mem_seg;
  CPU_BOOLEAN feat_is_ok;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (PlatformMgr_InitCfg.PoolBlkQtyMax > 0u) {
    feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
    feat_is_ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_NONE);
    feat_is_ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
    RTOS_ASSERT_DBG_ERR_SET((feat_is_ok == DEF_OK), *p_err, RTOS_ERR_NOT_AVAIL,; );

    p_mem_seg = Common_MemSegPtrGet();

    PlatformMgrPtr = (PLATFORM_MGR_DATA *)Mem_SegAlloc("Platform Mgr - Data",
                                                       p_mem_seg,
                                                       sizeof(PLATFORM_MGR_DATA),
                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_DynPoolCreate("Platform Mgr - Dyn Pool",
                      &PlatformMgrPtr->Pool,
                      p_mem_seg,
                      sizeof(PLATFORM_MGR_LIST_ADAPTER),
                      sizeof(CPU_ALIGN),
                      PlatformMgr_InitCfg.PoolBlkQtyInit,
                      PlatformMgr_InitCfg.PoolBlkQtyMax,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    PlatformMgrPtr->LockHandle = KAL_LockCreate("Platform Mgr Lock", DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    SList_Init(&(PlatformMgrPtr->PlatformMgrListHeadPtr));
  }
}

/****************************************************************************************************//**
 *                                           PlatformMgrItemAlloc()
 *
 * @brief    Allocate platform manager item.
 *
 * @param    item_size   Size of the item to allocate.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Allocated platform manager item, if NO error(s),
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
PLATFORM_MGR_ITEM *PlatformMgrItemAlloc(CPU_INT32U item_size,
                                        RTOS_ERR   *p_err)
{
  PLATFORM_MGR_ITEM *p_item;
  MEM_SEG           *p_mem_seg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(item_size != 0u, *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  p_mem_seg = Common_MemSegPtrGet();

  p_item = (PLATFORM_MGR_ITEM *)Mem_SegAlloc("HW_Info Item",
                                             p_mem_seg,
                                             item_size,
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_item);
}

/****************************************************************************************************//**
 *                                           PlatformMgrItemAdd()
 *
 * @brief    Add item to the platform manager.
 *
 * @param    p_item  Pointer to the item to add.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void PlatformMgrItemAdd(const PLATFORM_MGR_ITEM *p_item,
                        RTOS_ERR                *p_err)
{
  PLATFORM_MGR_LIST_ADAPTER *p_list_adapter;
  RTOS_ERR                  local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(p_item != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(PlatformMgrPtr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  (void)PlatformMgrInternalGet(PlatformMgrGetByNamePredicate, (void *)p_item->StrID, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NOT_FOUND) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto unlock_exit;
  }

  p_list_adapter = (PLATFORM_MGR_LIST_ADAPTER *)Mem_DynPoolBlkGet(&PlatformMgrPtr->Pool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto unlock_exit;
  }

  p_list_adapter->ItemPtr = p_item;

  SList_Push(&(PlatformMgrPtr->PlatformMgrListHeadPtr),
             &p_list_adapter->ListNode);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

unlock_exit:
  KAL_LockRelease(PlatformMgrPtr->LockHandle, &local_err);
  if ((RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE)
      && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, local_err);
  }
}

/****************************************************************************************************//**
 *                                           PlatformMgrItemGet()
 *
 * @brief    Get item identified by predicate function passed.
 *
 * @param    predicate_fnct      Function that will be called for each item to know if it is the item
 *                               that needs to be removed.
 *
 * @param    p_predicate_data    Pointer to argument that will be passed to predicate.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_FOUND
 *
 * @return   Pointer to item, if found.
 *           DEF_NULL, otherwise.
 *
 * @note     (1) No call to the platform mgr should be done from the predicate, as the lock is
 *               acquired at that moment. Doing so would result in a deadlock.
 *******************************************************************************************************/
PLATFORM_MGR_ITEM *PlatformMgrItemGet(PLATFORM_MGR_PREDICATE predicate_fnct,
                                      void                   *p_predicate_data,
                                      RTOS_ERR               *p_err)
{
  PLATFORM_MGR_LIST_ADAPTER *p_list_adapt;
  PLATFORM_MGR_ITEM         *p_ret_item;
  RTOS_ERR                  local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(predicate_fnct != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  KAL_LockAcquire(PlatformMgrPtr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_list_adapt = PlatformMgrInternalGet(predicate_fnct, p_predicate_data, p_err);
  p_ret_item = (PLATFORM_MGR_ITEM *)((p_list_adapt != DEF_NULL) ? (p_list_adapt->ItemPtr) : DEF_NULL);

  KAL_LockRelease(PlatformMgrPtr->LockHandle, &local_err);
  if ((RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE)
      && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, local_err);
  }

  return (p_ret_item);
}

/****************************************************************************************************//**
 *                                           PlatformMgrItemRem()
 *
 * @brief    Remove specified item from the platform manager.
 *
 * @param    p_item  Pointer to the item to remove.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void PlatformMgrItemRem(const PLATFORM_MGR_ITEM *p_item,
                        RTOS_ERR                *p_err)
{
  PLATFORM_MGR_LIST_ADAPTER *p_list_adapter;
  RTOS_ERR                  local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(p_item != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(PlatformMgrPtr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_list_adapter = PlatformMgrInternalGet(PlatformMgrGetByItemPtrPredicate, (void *)p_item, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto unlock_exit;
  }

  SList_Rem(&(PlatformMgrPtr->PlatformMgrListHeadPtr), &(p_list_adapter->ListNode));

  Mem_DynPoolBlkFree(&PlatformMgrPtr->Pool, (void *)p_list_adapter, &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

unlock_exit:
  KAL_LockRelease(PlatformMgrPtr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           PlatformMgrInternalGet()
 *
 * @brief    Finds and returns list adapter identified by the predicate.
 *
 * @param    predicate_fnct      Function that will be called for each item to know if it is the item
 *                               for which the list adapter needs to be returned.
 *
 * @param    p_predicate_data    Pointer to argument passed to predicate.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_FOUND
 *
 * @return   Pointer to list adapter, if found.
 *           DEF_NULL, otherwise.
 *
 * @note     (1) The platform mgr lock must be acquired when calling this function.
 *******************************************************************************************************/
static PLATFORM_MGR_LIST_ADAPTER *PlatformMgrInternalGet(PLATFORM_MGR_PREDICATE predicate_fnct,
                                                         void                   *p_predicate_data,
                                                         RTOS_ERR               *p_err)
{
  PLATFORM_MGR_LIST_ADAPTER *p_list_adapt = DEF_NULL;
  PLATFORM_MGR_LIST_ADAPTER *p_list_adapt_iter;
  CPU_BOOLEAN               ret_val;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  if (PlatformMgrPtr->PlatformMgrListHeadPtr != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(PlatformMgrPtr->PlatformMgrListHeadPtr, p_list_adapt_iter, PLATFORM_MGR_LIST_ADAPTER, ListNode) {
      ret_val = predicate_fnct(p_list_adapt_iter->ItemPtr, p_predicate_data);
      if (ret_val == DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        p_list_adapt = p_list_adapt_iter;
        break;
      }
    }
  }

  return (p_list_adapt);
}

/****************************************************************************************************//**
 *                                       PlatformMgrGetByNamePredicate()
 *
 * @brief    Predicate function to obtain a platform manager item based on its string ID.
 *
 * @param    p_item              Pointer to item that needs to be evaluated.
 *
 * @param    p_predicate_data    Pointer to predicate data argument provided by caller, in this case,
 *                               the string ID of the item to match.
 *
 * @return   DEF_YES, if the item is the one that should be used,
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN PlatformMgrGetByNamePredicate(const PLATFORM_MGR_ITEM *p_item,
                                                 void                    *p_predicate_data)
{
  CPU_CHAR    *id_str = (CPU_CHAR *)p_predicate_data;
  CPU_BOOLEAN ret_val = DEF_NO;

  if (Str_Cmp_N(p_item->StrID, id_str, DEF_INT_08U_MAX_VAL) == 0) {
    ret_val = DEF_YES;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                   PlatformMgrGetByItemPtrPredicate()
 *
 * @brief    Predicate function to obtain a platform manager item based on its item pointer.
 *
 * @param    p_item              Pointer to item that needs to be evaluated.
 *
 * @param    p_predicate_data    Pointer to predicate data argument provided by caller, in this case,
 *                               the item pointer to match.
 *
 * @return   DEF_YES, if the item is the one that should be used,
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN PlatformMgrGetByItemPtrPredicate(const PLATFORM_MGR_ITEM *p_item,
                                                    void                    *p_predicate_data)
{
  PLATFORM_MGR_ITEM *p_item_to_find = (PLATFORM_MGR_ITEM *)p_predicate_data;
  CPU_BOOLEAN       ret_val = DEF_NO;

  if (p_item_to_find == p_item) {
    ret_val = DEF_YES;
  }

  return (ret_val);
}
