/***************************************************************************//**
 * @file
 * @brief USB Host Function Operations
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

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_fnct_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, FNCT)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                          MEM_SEG      *p_seg,
                                          void         *p_blk,
                                          void         *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_FnctIF_QtyGet()
 *
 * @brief    Gets the quantity of interface contained in given function.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Quantity of interface, if successful.
 *           0,                     otherwise.
 *******************************************************************************************************/
CPU_INT08U USBH_FnctIF_QtyGet(USBH_DEV_HANDLE  dev_handle,
                              USBH_FNCT_HANDLE fnct_handle,
                              RTOS_ERR         *p_err)
{
  CPU_INT08U if_qty = 0u;
  USBH_DEV   *p_dev;
  USBH_FNCT  *p_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if_qty = p_fnct->NbrIF;

end_rel:
  USBH_DevRelShared(p_dev);

  return (if_qty);
}

/****************************************************************************************************//**
 *                                           USBH_FnctClassGet()
 *
 * @brief    Get the function's class code.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   ClassCode code, if successful.
 *           0,              otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the class code.
 *               - (a) If the function has only one interface, it corresponds to the 'bInterfaceClass'
 *                     field of the interface descriptor. For more information, see 'Universal Serial Bus
 *                     specification, revision 2.0, section 9.6.5'.
 *               - (b) If the function has more than one interface, it corresponds to the 'bFunctionClass'
 *                     field of the Interface Association Descriptor (IAD). For more information, see 'USB ECN:
 *                     Interface Association Descriptor'.
 *               - (c) If the class code is defined at device level, it corresponds to the 'bDeviceClass'
 *                     field of the device descriptor. For more information, see 'Universal Serial Bus
 *                     specification, revision 2.0, section 9.6.1'.
 *******************************************************************************************************/
CPU_INT08U USBH_FnctClassGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             RTOS_ERR         *p_err)
{
  CPU_INT08U class_code = 0U;
  USBH_DEV   *p_dev;
  USBH_FNCT  *p_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  class_code = p_fnct->ClassCode;

end_rel:
  USBH_DevRelShared(p_dev);

  return (class_code);
}

/****************************************************************************************************//**
 *                                           USBH_FnctSubclassGet()
 *
 * @brief    Get the function's subclass code.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Subclass code,  if successful.
 *           0,              otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the subclass code.
 *               - (a) If the function has only one interface, it corresponds to the
 *                     'bInterfaceSubClass' field of the interface descriptor. For more information,
 *                     see 'Universal Serial Bus specification, revision 2.0, section 9.6.5'.
 *               - (b) If the function has more than one interface, it corresponds to the
 *                     'bFunctionSubClass' field of the Interface Association Descriptor (IAD).
 *                     For more information, see 'USB ECN: Interface Association Descriptor'.
 *               - (c) If the class code is defined at device level, it corresponds to the 'bDeviceSubClass'
 *                     field of the device descriptor. For more information, see 'Universal Serial Bus
 *                     specification, revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_FnctSubclassGet(USBH_DEV_HANDLE  dev_handle,
                                USBH_FNCT_HANDLE fnct_handle,
                                RTOS_ERR         *p_err)
{
  CPU_INT08U subclass = 0u;
  USBH_DEV   *p_dev;
  USBH_FNCT  *p_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  subclass = p_fnct->Subclass;

end_rel:
  USBH_DevRelShared(p_dev);

  return (subclass);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_FnctProtocolGet()
 *
 * @brief    Get the function's protocol code.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Protocol code,  if successful.
 *           0,              otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the protocol code.
 *               - (a) If the function has only one interface, it corresponds to the
 *                     'bInterfaceProtocol' field of the interface descriptor. For more information, see
 *                     'Universal Serial Bus specification, revision 2.0, section 9.6.5'.
 *               - (b) If the function has more than one interface, it corresponds to the
 *                     'bFunctionProtocol' field of the Interface Association Descriptor (IAD). See
 *                     For more information, see 'USB ECN: Interface Association Descriptor'.
 *               - (c) If the class code is defined at device level, it corresponds to the 'bDeviceProtocol'
 *                     field of the device descriptor. For more information, see 'Universal Serial Bus specification,
 *                     revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_FnctProtocolGet(USBH_DEV_HANDLE  dev_handle,
                                USBH_FNCT_HANDLE fnct_handle,
                                RTOS_ERR         *p_err)
{
  CPU_INT08U protocol = 0u;
  USBH_DEV   *p_dev;
  USBH_FNCT  *p_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  protocol = p_fnct->Protocol;

end_rel:
  USBH_DevRelShared(p_dev);

  return (protocol);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_FnctStrGet()
 *
 * @brief    Gets the given function's string descriptor.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    p_str_buf       Pointer to the buffer that will receive the string descriptor.
 *
 * @param    str_buf_len     String buffer length in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NOT_FOUND
 *
 * @return   String descriptor length in octets.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iConfiguration' field of the configuration descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0, section 9.6.3'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can determine if a function string is present or
 *               not by passing a str_buf_len of 0. In this case, p_err will be set to
 *               RTOS_ERR_NOT_FOUND if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_FnctStrGet(USBH_DEV_HANDLE  dev_handle,
                           USBH_FNCT_HANDLE fnct_handle,
                           CPU_CHAR         *p_str_buf,
                           CPU_INT08U       str_buf_len,
                           RTOS_ERR         *p_err)
{
  CPU_INT08U str_desc_len = 0u;
  USBH_DEV   *p_dev;
  USBH_FNCT  *p_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if (p_fnct->StrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_fnct->StrIx,
                               p_str_buf,
                               str_buf_len,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);

  return (str_desc_len);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_FnctInit()
 *
 * @brief    Initializes USB Host function module.
 *
 * @param    fnct_qty    Quantity of function.
 *
 * @param    p_err       Pointer to the variable that will receive the error code from hub function.
 *******************************************************************************************************/
void USBH_FnctInit(CPU_INT16U fnct_qty,
                   RTOS_ERR   *p_err)
{
  Mem_DynPoolCreatePersistent("USBH - Function pool",           // Create USB function pool.
                              &USBH_Ptr->FnctPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_FNCT),
                              sizeof(CPU_ALIGN),
                              (fnct_qty != USBH_OBJ_QTY_UNLIMITED) ? fnct_qty : 0u,
                              (fnct_qty != USBH_OBJ_QTY_UNLIMITED) ? fnct_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_FnctAllocCallback,
                              DEF_NULL,
                              p_err);
}

/****************************************************************************************************//**
 *                                               USBH_FnctCreate()
 *
 * @brief    Creates a USB function.
 *
 * @param    p_desc_buf  Pointer to the buffer that contains interface or interface association (IAD)
 *                       descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the USB function,    if successful.
 *           0,                              otherwise.
 *******************************************************************************************************/
USBH_FNCT *USBH_FnctCreate(CPU_INT08U *p_desc_buf,
                           RTOS_ERR   *p_err)
{
  CPU_INT08U desc_type;
  USBH_FNCT  *p_fnct;

  p_fnct = (USBH_FNCT *)Mem_DynPoolBlkGet(&USBH_Ptr->FnctPool,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  Mem_Clr((void *)p_fnct->IFTblPtr,
          (sizeof(USBH_IF *) * USBH_InitCfg.OptimizeSpd.IF_PerFnctQty));
#else
  p_fnct->IFHeadPtr = DEF_NULL;
  p_fnct->NextPtr = DEF_NULL;
#endif

  desc_type = p_desc_buf[1u];

  if (desc_type == USBH_DESC_TYPE_IF_ASSOCIATION) {
    p_fnct->ClassCode = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[4u]);
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
    p_fnct->Subclass = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
    p_fnct->Protocol = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
#endif
#if (USBH_CFG_STR_EN == DEF_ENABLED)
    p_fnct->StrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[7u]);
#endif
  } else {
    p_fnct->ClassCode = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
    p_fnct->Subclass = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
    p_fnct->Protocol = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[7u]);
#endif
#if (USBH_CFG_STR_EN == DEF_ENABLED)
    p_fnct->StrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[8u]);
#endif
  }
  p_fnct->NbrIF = 0u;
  p_fnct->ClassFnctPtr = DEF_NULL;
  p_fnct->ClassDrvPtr = DEF_NULL;

  LOG_VRB(("Create function class ", (u)p_fnct->ClassCode));

  return (p_fnct);
}

/****************************************************************************************************//**
 *                                           USBH_FnctDevCreate()
 *
 * @brief    Create a function when class is defined at device level.
 *
 * @param    p_dev   Pointer to the device.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the USB function,    if successful.
 *           0,                              otherwise.
 *******************************************************************************************************/
USBH_FNCT *USBH_FnctDevCreate(USBH_DEV *p_dev,
                              RTOS_ERR *p_err)
{
  USBH_FNCT *p_fnct;

  p_fnct = (USBH_FNCT *)Mem_DynPoolBlkGet(&USBH_Ptr->FnctPool,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  Mem_Clr((void *)p_fnct->IFTblPtr,
          (sizeof(USBH_IF *) * USBH_InitCfg.OptimizeSpd.IF_PerFnctQty));
#else
  p_fnct->IFHeadPtr = DEF_NULL;
  p_fnct->NextPtr = DEF_NULL;
#endif

  p_fnct->ClassCode = p_dev->ClassCode;
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
  p_fnct->Subclass = p_dev->Subclass;
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
  p_fnct->Protocol = p_dev->Protocol;
#endif
  p_fnct->NbrIF = 0u;
  p_fnct->ClassDrvPtr = p_dev->ClassDrvPtr;
  p_fnct->ClassFnctPtr = p_dev->ClassFnctPtr;
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_fnct->StrIx = 0u;
#endif

  LOG_VRB(("Create device function class ", (u)p_fnct->ClassCode));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_fnct);
}

/****************************************************************************************************//**
 *                                               USBH_FnctDel()
 *
 * @brief    Deletes given function.
 *
 * @param    p_fnct  Pointer to the function.
 *******************************************************************************************************/
void USBH_FnctDel(USBH_FNCT *p_fnct)
{
  RTOS_ERR err_lib;

  USBH_FNCT_HANDLE_INVALIDATE(p_fnct);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U if_ix;

    for (if_ix = 0u; if_ix < p_fnct->NbrIF; if_ix++) {
      USBH_IF_Del(p_fnct->IFTblPtr[if_ix]);
    }
  }
#else
  {
    USBH_IF *p_if;
    USBH_IF *p_if_next;

    p_if = p_fnct->IFHeadPtr;
    while (p_if != DEF_NULL) {
      p_if_next = p_if->NextPtr;
      USBH_IF_Del(p_if);
      p_if = p_if_next;
    }
  }
#endif

  Mem_DynPoolBlkFree(&USBH_Ptr->FnctPool,
                     (void *)p_fnct,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing function to pool", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  LOG_VRB(("Delete function class ", (u)p_fnct->ClassCode));
}

/****************************************************************************************************//**
 *                                               USBH_FnctIF_Add()
 *
 * @brief    Adds interface to function.
 *
 * @param    p_fnct  Pointer to the function.
 *
 * @param    p_if    Pointer to the interface.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Order of the interfaces in the function list's (when USBH_CFG_OPTIMIZE_SPD_EN is set
 *               to DEF_DISABLED) must follow the interface index.
 *******************************************************************************************************/
void USBH_FnctIF_Add(USBH_FNCT *p_fnct,
                     USBH_IF   *p_if,
                     RTOS_ERR  *p_err)
{
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  if (p_if->AltNbr == 0u) {                                     // Set self as cur alternate interface.
    p_if->AltCur = p_fnct->NbrIF;
  }
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  if (p_fnct->NbrIF >= USBH_InitCfg.OptimizeSpd.IF_PerFnctQty) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_fnct->IFTblPtr[p_fnct->NbrIF] = p_if;
#else
  if (p_fnct->NbrIF == 0u) {
    p_fnct->IFHeadPtr = p_if;
  } else {
    USBH_IF *p_if_last;

    p_if_last = p_fnct->IFHeadPtr;
    while (p_if_last->NextPtr != DEF_NULL) {
      p_if_last = p_if_last->NextPtr;
    }

    p_if_last->NextPtr = p_if;
  }
#endif

  p_fnct->NbrIF++;

  LOG_VRB(("Add interface to fnct class ", (u)p_fnct->ClassCode));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_FnctEP_Open()
 *
 * @brief    Attempt to open all endpoints of all default interfaces contained in function.
 *
 * @param    p_dev   Pointer to the device.
 *
 * @param    p_fnct  Pointer to the function structure.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_FnctEP_Open(USBH_DEV  *p_dev,
                      USBH_FNCT *p_fnct,
                      RTOS_ERR  *p_err)
{
  CPU_INT08U if_ix;
  USBH_IF    *p_if;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_if = p_fnct->IFHeadPtr;
#endif

  for (if_ix = 0u; if_ix < p_fnct->NbrIF; if_ix++) {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    p_if = p_fnct->IFTblPtr[if_ix];
#endif

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
    if (p_if->AltNbr == 0u) {
#endif
    USBH_IF_AltEP_Open(p_dev,
                       p_fnct,
                       if_ix,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  }
#endif
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
    p_if = p_if->NextPtr;
#endif
  }

#if (USBH_CFG_ALT_IF_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_if);
#endif

  LOG_VRB(("Endpoint opened for function class ", (u)p_fnct->ClassCode));
}

/****************************************************************************************************//**
 *                                           USBH_FnctEP_Close()
 *
 * @brief    Attempt to close all endpoints of all interfaces contained in function.
 *
 * @param    p_dev   Pointer to the device.
 *
 * @param    p_fnct  Pointer to the function structure.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_FnctEP_Close(USBH_DEV  *p_dev,
                       USBH_FNCT *p_fnct,
                       RTOS_ERR  *p_err)
{
  CPU_INT08U if_ix;
  USBH_IF *p_if;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_if = p_fnct->IFHeadPtr;
#endif

  for (if_ix = 0u; if_ix < p_fnct->NbrIF; if_ix++) {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    p_if = p_fnct->IFTblPtr[if_ix];
#endif

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
    if (p_if->AltNbr == 0u) {
      USBH_IF_AltEP_Close(p_dev,
                          p_fnct,
                          p_if->AltCur,
                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    }
#else
    USBH_IF_AltEP_Close(p_dev,
                        p_fnct,
                        if_ix,
                        p_err);
#endif
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
    p_if = p_if->NextPtr;
#endif
  }

#if (USBH_CFG_ALT_IF_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_if);
#endif

  LOG_VRB(("Endpoint closed for function class ", (u)p_fnct->ClassCode));
}

/****************************************************************************************************//**
 *                                           USBH_FnctConnFailNotify()
 *
 * @brief    Notify application that a function connection has failed.
 *
 * @param    fnct_handle     Handle to function that failed.
 *
 * @param    err             Error code.
 *
 * @note     (1) At that point, the application may use USBH_DEV_HANDLE_NOTIFICATION as a dev_handle
 *               to obtain more information on the device or function.
 *******************************************************************************************************/
void USBH_FnctConnFailNotify(USBH_FNCT_HANDLE fnct_handle,
                             RTOS_ERR         err)
{
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->FnctConnFail != DEF_NULL)) {
    p_event_fncts->FnctConnFail(fnct_handle, err);
  }
}

/****************************************************************************************************//**
 *                                               USBH_FnctPtrGet()
 *
 * @brief    Gets a pointer to function.
 *
 * @param    p_config        Pointer to the configuration structure.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the function structure.
 *******************************************************************************************************/
USBH_FNCT *USBH_FnctPtrGet(USBH_CONFIG      *p_config,
                           USBH_FNCT_HANDLE fnct_handle,
                           RTOS_ERR         *p_err)
{
  CPU_INT08U fnct_ix;
  USBH_FNCT *p_fnct;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  CPU_INT08U fnct_srch_ix;
#endif

  fnct_ix = USBH_HANDLE_FNCT_IX_GET(fnct_handle);

  RTOS_ASSERT_DBG_ERR_SET((fnct_ix < p_config->NbrFnct), *p_err, RTOS_ERR_INVALID_HANDLE, DEF_NULL);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_fnct = p_config->FnctTblPtr[fnct_ix];
#else
  p_fnct = p_config->FnctHeadPtr;
  for (fnct_srch_ix = 0u; fnct_srch_ix < fnct_ix; fnct_srch_ix++) {
    p_fnct = p_fnct->NextPtr;
  }
#endif

  if ((p_fnct == DEF_NULL)
      || (USBH_FNCT_HANDLE_VALIDATE(p_fnct, fnct_handle) != DEF_OK)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_fnct);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_FnctAllocCallback()
 *
 * @brief    Function called when a new function is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                          MEM_SEG      *p_seg,
                                          void         *p_blk,
                                          void         *p_arg)
{
  USBH_FNCT *p_fnct = (USBH_FNCT *)p_blk;

  (void)&p_pool;
  (void)&p_seg;
  (void)&p_arg;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    RTOS_ERR err_lib;

    p_fnct->IFTblPtr = Mem_SegAlloc("USBH - Fnct IF ptr table",
                                    p_seg,
                                    USBH_InitCfg.OptimizeSpd.IF_PerFnctQty * sizeof(USBH_IF *),
                                    &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }
#endif

  p_fnct->Handle = USBH_FNCT_HANDLE_INVALID;

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
