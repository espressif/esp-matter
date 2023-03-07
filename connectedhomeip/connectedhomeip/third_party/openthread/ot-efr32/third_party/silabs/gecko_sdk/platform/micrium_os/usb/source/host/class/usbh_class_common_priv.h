/***************************************************************************//**
 * @file
 * @brief USB Host Class Common Interface
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

#ifndef  _USBH_CLASS_COMMON_PRIV_H_
#define  _USBH_CLASS_COMMON_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_class.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_CLASS_FNCT_QTY_UNLIMITED                                  0u

#define  USBH_CLASS_HANDLE_IX_MSK                                        0x00FFu
#define  USBH_CLASS_HANDLE_VALIDATE_CNT_MSK                              0xFF00u

/********************************************************************************************************
 *                                           CLASS HANDLE MACROS
 *******************************************************************************************************/

//                                                                 Create new class fnct handle with index.
#define  USBH_CLASS_HANDLE_CREATE(ix)                                   ((ix) & USBH_CLASS_HANDLE_IX_MSK)

//                                                                 Retrieve class function index from handle.
#define  USBH_CLASS_HANDLE_IX_GET(class_handle)                         (CPU_INT08U)((class_handle) & USBH_CLASS_HANDLE_IX_MSK)

//                                                                 Validate class fnct handle.
#define  USBH_CLASS_HANDLE_VALIDATE(class_handle, object_class_handle) (((class_handle) == (object_class_handle)) ? DEF_VALID : DEF_INVALID)

//                                                                 Invalidate given class fnct handle.
#define  USBH_CLASS_HANDLE_INVALIDATE(object_class_handle)              ((object_class_handle) += 0x0100u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Class function allocation callback.
typedef CPU_BOOLEAN (*USBH_CLASS_FNCT_ALLOC_FNCT)  (void *p_class_fnct_data);

/********************************************************************************************************
 *                                           FORWARD DECLARATION
 *******************************************************************************************************/

typedef struct usbh_class_fnct_pool USBH_CLASS_FNCT_POOL;
typedef struct usbh_class_fnct USBH_CLASS_FNCT;

/********************************************************************************************************
 *                                           CLASS FUNCTION POOL
 *******************************************************************************************************/

struct usbh_class_fnct_pool {
  MEM_DYN_POOL               Pool;                              // Persistent pool of class objects.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_CLASS_FNCT            **FnctPtrTbl;                      // Table of pointers to class functions.
#else
  USBH_CLASS_FNCT            *FnctHeadPtr;                      // Pointer to head of class funcitons list.
#endif

  CPU_INT08U                 ClassFnctIxNext;                   // Class function index to use on next allocation.

  USBH_CLASS_FNCT_ALLOC_FNCT FnctAllocCallback;                 // Class callback to call when new class fnct is alloc.

#if ((DEF_BIT_IS_SET(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, RTOS_CFG_MODULE_USBH) == DEF_ENABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
  CPU_INT08U ClassFnctQty;                                      // Quantity of class functions.
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_ClassFnctPoolCreate(const CPU_CHAR             *p_name,
                              USBH_CLASS_FNCT_POOL       *p_class_fnct_pool,
                              MEM_SEG                    *p_seg,
                              CPU_SIZE_T                 class_fnct_len,
                              CPU_SIZE_T                 fnct_qty,
                              USBH_CLASS_FNCT_ALLOC_FNCT class_alloc_fnct,
                              RTOS_ERR                   *p_err);
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_ClassFnctPoolDel(USBH_CLASS_FNCT_POOL *p_class_fnct_pool,
                           RTOS_ERR             *p_err);
#endif

USBH_CLASS_FNCT_HANDLE USBH_ClassFnctAlloc(USBH_CLASS_FNCT_POOL *p_class_fnct_pool,
                                           USBH_DEV_HANDLE      dev_handle,
                                           RTOS_ERR             *p_err);

void USBH_ClassFnctFree(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                        USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                        RTOS_ERR               *p_err);

void *USBH_ClassFnctAcquire(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                            USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                            CPU_BOOLEAN            exclusive,
                            CPU_INT32U             timeout,
                            RTOS_ERR               *p_err);

void USBH_ClassFnctRelease(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                           USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                           CPU_BOOLEAN            exclusive);

void *USBH_ClassFnctGet(USBH_CLASS_FNCT_POOL   *p_class_fnct_pool,
                        USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                        RTOS_ERR               *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
