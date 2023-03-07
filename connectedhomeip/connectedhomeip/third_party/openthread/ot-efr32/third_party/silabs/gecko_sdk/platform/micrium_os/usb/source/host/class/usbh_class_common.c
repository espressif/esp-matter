/***************************************************************************//**
 * @file
 * @brief USB Host - Class Common Framework
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

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core_handle.h>
#include  <usb/include/host/usbh_class.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>

#include  <usb/source/host/class/usbh_class_common_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, COMMON)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               OP LOCK POLICY
 *******************************************************************************************************/

static OP_LOCK_OP_DESC USBH_OpLockExclusive =
{
  (DEF_BIT_07 | DEF_BIT_06 | DEF_BIT_05 | DEF_BIT_04 | DEF_BIT_03 | DEF_BIT_02 | DEF_BIT_01 | DEF_BIT_00),
  DEF_BIT_07
};

static OP_LOCK_OP_DESC USBH_OpLockShared =
{
  DEF_BIT_07,
  (DEF_BIT_06 | DEF_BIT_05 | DEF_BIT_04 | DEF_BIT_03 | DEF_BIT_02 | DEF_BIT_01 | DEF_BIT_00)
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CLASS FUNCTION BASE TYPE
 *******************************************************************************************************/

struct usbh_class_fnct {
  USBH_CLASS_FNCT_HANDLE Handle;                                // Class function's handle.
  OP_LOCK                Lock;                                  // Class function's lock.
  USBH_DEV_HANDLE        DevHandle;                             // Device handle.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  struct usbh_class_fnct *NextPtr;                              // Ptr to next class function.
#endif

  CPU_DATA               ClassFnctData;                         // Addr of beginning of class driver's function data.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_CLASS_FNCT *USBH_ClassFnctPtrGet(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                                             USBH_CLASS_FNCT_HANDLE class_fnct_handle);

static CPU_BOOLEAN USBH_Class_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                                MEM_SEG      *p_seg,
                                                void         *p_blk,
                                                void         *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_ClassFnctPoolCreate()
 *
 * @brief    Creates a pool of class functions.
 *
 * @param    p_name                  Pointer to class function pool name.
 *
 * @param    p_class_fnct_pool       Pointer to class function pool structure.
 *
 * @param    p_seg                   Pointer to memory segment from which the pool should allocate the
 *                                   class functions.
 *
 * @param    class_fnct_data_len     Length in bytes of the class function's data.
 *
 * @param    fnct_qty                Quantity of class function to allocate. If no limit,
 *                                   USBH_CLASS_FNCT_QTY_UNLIMITED.
 *
 * @param    class_alloc_fnct        Callback function to be called when new class function is allocated.
 *
 * @param    p_err                   Pointer to variable that will receive the error code from this function.
 *******************************************************************************************************/
void USBH_ClassFnctPoolCreate(const CPU_CHAR             *p_name,
                              USBH_CLASS_FNCT_POOL       *p_class_fnct_pool,
                              MEM_SEG                    *p_seg,
                              CPU_SIZE_T                 class_fnct_data_len,
                              CPU_SIZE_T                 fnct_qty,
                              USBH_CLASS_FNCT_ALLOC_FNCT class_alloc_fnct,
                              RTOS_ERR                   *p_err)
{
  CPU_SIZE_T class_fnct_len_tot = sizeof(USBH_CLASS_FNCT) + class_fnct_data_len - sizeof(CPU_DATA);

#if ((USBH_CFG_INIT_ALLOC_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED))
  (void)&fnct_qty;
#endif

  p_class_fnct_pool->ClassFnctIxNext = 0u;
  p_class_fnct_pool->FnctAllocCallback = class_alloc_fnct;
#if ((RTOS_ARG_CHK_EXT_EN == DEF_ENABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
  p_class_fnct_pool->ClassFnctQty = fnct_qty;
#endif

  Mem_DynPoolCreatePersistent(p_name,
                              &p_class_fnct_pool->Pool,
                              p_seg,
                              class_fnct_len_tot,
                              sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                              fnct_qty,
                              fnct_qty,
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
                              0u,
                              fnct_qty,
#else
                              0,
                              LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                              USBH_Class_FnctAllocCallback,
                              (void *)p_class_fnct_pool,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_class_fnct_pool->FnctPtrTbl = (USBH_CLASS_FNCT **)Mem_SegAlloc("USBH - Class fnct ptr tbl",
                                                                   p_seg,
                                                                   (fnct_qty * sizeof(USBH_CLASS_FNCT *)),
                                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#else
  p_class_fnct_pool->FnctHeadPtr = DEF_NULL;
#endif
}

/****************************************************************************************************//**
 *                                           USBH_ClassFnctPoolDel()
 *
 * @brief    Deletes anything related to a pool of class functions.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool structure.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this
 *                               function.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_ClassFnctPoolDel(USBH_CLASS_FNCT_POOL *p_class_fnct_pool,
                           RTOS_ERR             *p_err)
{
  USBH_CLASS_FNCT   *p_class_fnct;
  MEM_DYN_POOL_INFO pool_info;

  (void)Mem_DynPoolBlkNbrAvailGet(&p_class_fnct_pool->Pool,
                                  &pool_info,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  while (pool_info.BlkNbrAvailCnt != 0u) {
    p_class_fnct = (USBH_CLASS_FNCT *)Mem_DynPoolBlkGet(&p_class_fnct_pool->Pool,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    pool_info.BlkNbrAvailCnt--;

    OpLockDel(&p_class_fnct->Lock);
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_ClassFnctAlloc()
 *
 * @brief    Allocates a class function.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    dev_handle          Handle to device.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this
 *                               function.
 *
 * @return   Handle to class function.
 *******************************************************************************************************/
USBH_CLASS_FNCT_HANDLE USBH_ClassFnctAlloc(USBH_CLASS_FNCT_POOL *p_class_fnct_pool,
                                           USBH_DEV_HANDLE      dev_handle,
                                           RTOS_ERR             *p_err)
{
  USBH_CLASS_FNCT        *p_class_fnct;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;
  CORE_DECLARE_IRQ_STATE;

  p_class_fnct = (USBH_CLASS_FNCT *)Mem_DynPoolBlkGet(&p_class_fnct_pool->Pool,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_CLASS_FNCT_HANDLE_INVALID);
  }

  class_fnct_handle = p_class_fnct->Handle;

  p_class_fnct->DevHandle = dev_handle;

  CORE_ENTER_ATOMIC();
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_class_fnct->NextPtr = p_class_fnct_pool->FnctHeadPtr;
  p_class_fnct_pool->FnctHeadPtr = p_class_fnct;
#else
  p_class_fnct_pool->FnctPtrTbl[USBH_CLASS_HANDLE_IX_GET(p_class_fnct->Handle)] = p_class_fnct;
#endif
  CORE_EXIT_ATOMIC();

  return (class_fnct_handle);
}

/****************************************************************************************************//**
 *                                           USBH_ClassFnctFree()
 *
 * @brief    Frees a class function.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    class_fnct_handle   Handle to class function.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this
 *                               function.
 *******************************************************************************************************/
void USBH_ClassFnctFree(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                        USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                        RTOS_ERR               *p_err)
{
  USBH_CLASS_FNCT *p_class_fnct;

  p_class_fnct = USBH_ClassFnctPtrGet(p_class_fnct_pool,
                                      class_fnct_handle);
  if (p_class_fnct == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return;
  }

  USBH_CLASS_HANDLE_INVALIDATE(p_class_fnct->Handle);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  //                                                               Remove fnct from tbl.
  p_class_fnct_pool->FnctPtrTbl[USBH_CLASS_HANDLE_IX_GET(p_class_fnct->Handle)] = DEF_NULL;
#else
  {
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();                                        // Remove fnct from list.
    if (p_class_fnct_pool->FnctHeadPtr == p_class_fnct) {
      p_class_fnct_pool->FnctHeadPtr = p_class_fnct->NextPtr;
    } else {
      USBH_CLASS_FNCT *p_class_fnct_prev = p_class_fnct_pool->FnctHeadPtr;

      while (p_class_fnct_prev->NextPtr != p_class_fnct) {
        p_class_fnct_prev = p_class_fnct_prev->NextPtr;
      }

      if (p_class_fnct_prev != DEF_NULL) {
        p_class_fnct_prev->NextPtr = p_class_fnct->NextPtr;
      }
    }
    CORE_EXIT_ATOMIC();
  }
#endif

  Mem_DynPoolBlkFree(&p_class_fnct_pool->Pool,
                     (void *)p_class_fnct,
                     p_err);
}

/****************************************************************************************************//**
 *                                           USBH_ClassFnctAcquire()
 *
 * @brief    Acquires a class function and retrieves class function's data.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    class_fnct_handle   Handle to class function.
 *
 * @param    exclusive           Flag that indicates if access to class function should be exclusive
 *                               or not.
 *
 * @param    timeout             Timeout, in milliseconds.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this
 *                               function.
 *
 * @return   Class function's data.
 *******************************************************************************************************/
void *USBH_ClassFnctAcquire(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                            USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                            CPU_BOOLEAN            exclusive,
                            CPU_INT32U             timeout,
                            RTOS_ERR               *p_err)
{
  USBH_CLASS_FNCT *p_class_fnct;
  OP_LOCK_OP_DESC *p_op_lock_op_desc;
  void            *ret_val;

  p_class_fnct = USBH_ClassFnctPtrGet(p_class_fnct_pool,
                                      class_fnct_handle);
  if (p_class_fnct == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  p_op_lock_op_desc = (exclusive == DEF_YES) ? &USBH_OpLockExclusive : &USBH_OpLockShared;

  OpLockAcquire(&p_class_fnct->Lock,
                p_op_lock_op_desc,
                timeout,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  if (USBH_CLASS_HANDLE_VALIDATE(class_fnct_handle, p_class_fnct->Handle) == DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    ret_val = (void *)&p_class_fnct->ClassFnctData;
  } else {
    OpLockRelease(&p_class_fnct->Lock,
                  &USBH_OpLockShared);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    ret_val = DEF_NULL;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           USBH_ClassFnctRelease()
 *
 * @brief    Releases class function.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    class_fnct_handle   Handle to class function.
 *
 * @param    exclusive           Flag that indicates if access to class function was exclusive or not.
 *******************************************************************************************************/
void USBH_ClassFnctRelease(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                           USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                           CPU_BOOLEAN            exclusive)
{
  USBH_CLASS_FNCT *p_class_fnct;
  OP_LOCK_OP_DESC *p_op_lock_op_desc;

  p_class_fnct = USBH_ClassFnctPtrGet(p_class_fnct_pool,
                                      class_fnct_handle);
  if (p_class_fnct == DEF_NULL) {
    LOG_ERR(("Releasing class function -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_INVALID_HANDLE)));
    return;
  }

  p_op_lock_op_desc = (exclusive == DEF_YES) ? &USBH_OpLockExclusive : &USBH_OpLockShared;

  OpLockRelease(&p_class_fnct->Lock,
                p_op_lock_op_desc);
}

/****************************************************************************************************//**
 *                                           USBH_ClassFnctGet()
 *
 * @brief    Retrieves a class function without locking it. This function should be called ONLY from
 *           the hub task.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    class_fnct_handle   Handle to class function.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *
 * @return   Class function's data.
 *******************************************************************************************************/
void *USBH_ClassFnctGet(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                        USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                        RTOS_ERR               *p_err)
{
  USBH_CLASS_FNCT *p_class_fnct;

  p_class_fnct = USBH_ClassFnctPtrGet(p_class_fnct_pool,
                                      class_fnct_handle);
  if (p_class_fnct == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  if (USBH_CLASS_HANDLE_VALIDATE(class_fnct_handle, p_class_fnct->Handle) != DEF_VALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return ((void *)&p_class_fnct->ClassFnctData);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ClassFnctPtrGet()
 *
 * @brief    Retrieves pointer on class function.
 *
 * @param    p_class_fnct_pool   Pointer to class function pool.
 *
 * @param    class_fnct_handle   Handle to class function.
 *
 * @return   Pointer on class function.
 *******************************************************************************************************/
static USBH_CLASS_FNCT *USBH_ClassFnctPtrGet(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                                             USBH_CLASS_FNCT_HANDLE class_fnct_handle)
{
  USBH_CLASS_FNCT *p_class_fnct;
  CORE_DECLARE_IRQ_STATE;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U class_fnct_ix = USBH_CLASS_HANDLE_IX_GET(class_fnct_handle);

    RTOS_ASSERT_DBG((class_fnct_ix < p_class_fnct_pool->ClassFnctIxNext), RTOS_ERR_INVALID_HANDLE, DEF_NULL);

    CORE_ENTER_ATOMIC();
    p_class_fnct = p_class_fnct_pool->FnctPtrTbl[class_fnct_ix];
  }
#else
  CORE_ENTER_ATOMIC();
  p_class_fnct = p_class_fnct_pool->FnctHeadPtr;
  while ((p_class_fnct != DEF_NULL)
         && (p_class_fnct->Handle != class_fnct_handle)) {
    p_class_fnct = p_class_fnct->NextPtr;
  }
#endif
  CORE_EXIT_ATOMIC();

  return (p_class_fnct);
}

/****************************************************************************************************//**
 *                                       USBH_Class_FnctAllocCallback()
 *
 * @brief    Allocates extra data on class function when they are allocated.
 *
 * @param    p_pool  Pointer to memory pool.
 *
 * @param    p_seg   Pointer to memory segment.
 *
 * @param    p_blk   Pointer to allocated block.
 *
 * @param    p_arg   Pointer to argument.
 *
 * @return   DEF_OK,     if extra allocations were successful.
 *           DEF_FAIL,   if extra allocations Failed.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_Class_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                                MEM_SEG      *p_seg,
                                                void         *p_blk,
                                                void         *p_arg)
{
  CPU_BOOLEAN          ok = DEF_OK;
  USBH_CLASS_FNCT      *p_class_fnct = (USBH_CLASS_FNCT *)p_blk;
  USBH_CLASS_FNCT_POOL *p_class_fnct_pool = (USBH_CLASS_FNCT_POOL *)p_arg;
  RTOS_ERR             err_op_lock;
  CORE_DECLARE_IRQ_STATE;

  (void)&p_pool;
  (void)&p_seg;

  OpLockCreate(&p_class_fnct->Lock, &err_op_lock);              // Create shared lock for class function.
  if (RTOS_ERR_CODE_GET(err_op_lock) != RTOS_ERR_NONE) {
    return (ok);
  }
  //                                                               Init class fnct handle.
  CORE_ENTER_ATOMIC();
  p_class_fnct->Handle = USBH_CLASS_HANDLE_CREATE(p_class_fnct_pool->ClassFnctIxNext);
  p_class_fnct_pool->ClassFnctIxNext++;
  CORE_EXIT_ATOMIC();

  if (p_class_fnct_pool->FnctAllocCallback != DEF_NULL) {
    ok = p_class_fnct_pool->FnctAllocCallback(&p_class_fnct->ClassFnctData);
  }

  if (ok != DEF_OK) {
    CORE_ENTER_ATOMIC();
    p_class_fnct_pool->ClassFnctIxNext--;
    CORE_EXIT_ATOMIC();
  }

  return (ok);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
