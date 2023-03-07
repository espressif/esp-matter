/***************************************************************************//**
 * @file
 * @brief USB Host Interface Operations
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

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_config.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_class_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_fnct_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>
#include  <usb/source/host/core/usbh_core_ep_priv.h>
#include  <usb/source/host/core/usbh_core_hub_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, IF)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_IF_NbrGet()
 *
 * @brief    Gets the interface number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Interface number.
 *
 * @note     (1) The value returned by this function corresponds to the 'bInterfaceNumber' field
 *               of the interface descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.5'.
 *******************************************************************************************************/
CPU_INT08U USBH_IF_NbrGet(USBH_DEV_HANDLE  dev_handle,
                          USBH_FNCT_HANDLE fnct_handle,
                          CPU_INT08U       if_ix,
                          RTOS_ERR         *p_err)
{
  CPU_INT08U if_nbr = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if_nbr = p_if->Nbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_IF_AltNbrGet()
 *
 * @brief    Gets the interface alternate number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Interface alternate number.
 *
 * @note     (1) The value returned by this function corresponds to the 'bAlternateSetting' field
 *               of the interface descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.5'.
 *******************************************************************************************************/

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_AltNbrGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             RTOS_ERR         *p_err)
{
  CPU_INT08U if_alt_nbr = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if_alt_nbr = p_if->AltNbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (if_alt_nbr);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_AltIxCurGet()
 *
 * @brief    Gets the interface's currently selected alternate number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Currently selected interface alternate number.
 *******************************************************************************************************/

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_AltIxCurGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err)
{
  CPU_INT08U if_alt_cur = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if (p_if->AltNbr != 0u) {                                     // Only p_if structure of alt IF 0 stores cur alt IF.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    goto end_rel;
  }

  if_alt_cur = p_if->AltCur;

end_rel:
  USBH_DevRelShared(p_dev);

  return (if_alt_cur);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_EP_QtyGet()
 *
 * @brief    Gets the quantity of endpoint contained in interface.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Quantity of endpoint, if successful.
 *           0,                    otherwise.
 *******************************************************************************************************/
CPU_INT08U USBH_IF_EP_QtyGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             RTOS_ERR         *p_err)
{
  CPU_INT08U ep_qty = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  ep_qty = p_if->NbrEP;

end_rel:
  USBH_DevRelShared(p_dev);

  return (ep_qty);
}

/****************************************************************************************************//**
 *                                           USBH_IF_ClassGet()
 *
 * @brief    Gets the interface class code number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Interface class code, if successful.
 *           0,                    otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bInterfaceClass' field
 *               of the interface descriptor. For more information, see 'Universal Serial Bus
 *               specification, revision 2.0, section 9.6.5'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) == DEF_YES)
CPU_INT08U USBH_IF_ClassGet(USBH_DEV_HANDLE  dev_handle,
                            USBH_FNCT_HANDLE fnct_handle,
                            CPU_INT08U       if_ix,
                            RTOS_ERR         *p_err)
{
  CPU_INT08U class_code = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  class_code = p_if->ClassCode;

end_rel:
  USBH_DevRelShared(p_dev);

  return (class_code);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_SubclassGet()
 *
 * @brief    Gets the interface subclass code number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Interface subclass code, if successful.
 *           0,                       otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bInterfaceSubClass' field
 *               of the interface descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.5'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_IF_SubclassGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err)
{
  CPU_INT08U subclass = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  subclass = p_if->Subclass;

end_rel:
  USBH_DevRelShared(p_dev);

  return (subclass);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_ProtocolGet()
 *
 * @brief    Gets the interface protocol code number.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Interface protocol code, if successful.
 *           0,                       otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bInterfaceProtcol' field
 *               of the interface descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.5'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_IF_ProtocolGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err)
{
  CPU_INT08U protocol = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

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

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  protocol = p_if->Protocol;

end_rel:
  USBH_DevRelShared(p_dev);

  return (protocol);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_IF_StrGet()
 *
 * @brief    Gets the interfaces's string.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    p_str_buf       Buffer that will receive the string descriptor.
 *
 * @param    str_buf_len     String buffer length in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_NOT_FOUND
 *
 * @return   String length in octets.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iInterface' field of the interface descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0, section 9.6.5'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can be used to determine if an interface string is present or
 *               not by passing a str_buf_len of 0. In that case p_err will be set to
 *               RTOS_ERR_NOT_FOUND if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_StrGet(USBH_DEV_HANDLE  dev_handle,
                          USBH_FNCT_HANDLE fnct_handle,
                          CPU_INT08U       if_ix,
                          CPU_CHAR         *p_str_buf,
                          CPU_INT08U       str_buf_len,
                          RTOS_ERR         *p_err)
{
  CPU_INT08U str_desc_len = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if (p_if->StrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_if->StrIx,
                               p_str_buf,
                               str_buf_len,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);

  return (str_desc_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_DescExtraGet()
 *
 * @brief    Gets the interfaces's extra descriptor(s).
 *
 * @param    fnct_handle         Handle to the function.
 *
 * @param    if_ix               Index of the interface.
 *
 * @param    p_desc_extra_len    Pointer to the variable that will receive total extra descriptors
 *                               length in octets.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_NULL_PTR
 *                                   - RTOS_ERR_INVALID_STATE
 *
 * @return   Pointer to the buffer that contains interface's extra descriptor(s).
 *
 * @note     (1) First descriptor returned by this function will always be the interface descriptor
 *               itself.
 *******************************************************************************************************/
CPU_INT08U *USBH_IF_DescExtraGet(USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT16U       *p_desc_extra_len,
                                 RTOS_ERR         *p_err)
{
  CPU_INT08U *p_desc_buf;
  CPU_INT08U *p_desc_extra_buf = DEF_NULL;
  CPU_INT16U cnt;
  CPU_INT16U extra_desc_start_ix = 0u;
  USBH_IF    *p_if;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  if (p_desc_extra_len == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return (DEF_NULL);
  }

  CORE_ENTER_ATOMIC();
  if (USBH_Ptr->DescDevPtr == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return (DEF_NULL);
  }
  CORE_EXIT_ATOMIC();

  p_if = USBH_IF_PtrGet(&USBH_Ptr->DescDevPtr->ConfigCur,
                        fnct_handle,
                        if_ix,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  cnt = USBH_DESC_LEN_CONFIG;                                   // Skip config desc.
  p_desc_buf = USBH_Ptr->DescBufPtr;
  *p_desc_extra_len = 0u;

  //                                                               ----------------- RETRIEVE IF DESC -----------------
  while (cnt < USBH_Ptr->DescLen) {
    CPU_INT08U desc_len = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt]);
    CPU_INT08U desc_type = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 1u]);

    if (desc_type == USBH_DESC_TYPE_IF) {
      if (extra_desc_start_ix == 0u) {
        CPU_INT08U desc_if_nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 2u]);
        CPU_INT08U desc_alt_if_nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 3u]);
        CPU_INT08U alt_if_nbr;

                #if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
        alt_if_nbr = p_if->AltNbr;
                #else
        alt_if_nbr = 0u;
                #endif

        if ((desc_if_nbr == p_if->Nbr)
            && (desc_alt_if_nbr == alt_if_nbr)) {
          extra_desc_start_ix = cnt;
        }
      } else {
        break;
      }
    } else if (((desc_type == USBH_DESC_TYPE_EP)
                || (desc_type == USBH_DESC_TYPE_IF_ASSOCIATION))
               && (extra_desc_start_ix != 0u)) {
      break;
    }

    if (extra_desc_start_ix != 0u) {
      *p_desc_extra_len += desc_len;
    }

    cnt += desc_len;
  }

  p_desc_extra_buf = &p_desc_buf[extra_desc_start_ix];

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_desc_extra_buf);
}

/****************************************************************************************************//**
 *                                           USBH_IF_EP_AddrNextGet()
 *
 * @brief    Retrieves the address of the next endpoint available on interface.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    ep_addr_prev    Previous endpoint address, or 0u to obtain first endpoint address.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Next interface's available endpoint's address, if successful.
 *           0u,                                            if an error occurred or if the last
 *                                                          endpoint was reached.
 *******************************************************************************************************/
CPU_INT08U USBH_IF_EP_AddrNextGet(USBH_DEV_HANDLE  dev_handle,
                                  USBH_FNCT_HANDLE fnct_handle,
                                  CPU_INT08U       if_ix,
                                  CPU_INT08U       ep_addr_prev,
                                  RTOS_ERR         *p_err)
{
  CPU_INT08U ep_addr_next = 0u;
  USBH_DEV   *p_dev;
  USBH_IF    *p_if;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_EP *p_ep;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (ep_addr_next);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ep_phy_nbr;

    if (ep_addr_prev == 0u) {
      ep_phy_nbr = 2u;
    } else {
      ep_phy_nbr = USBH_EP_ADDR_TO_PHY(ep_addr_prev) + 1u;
    }

    while (ep_phy_nbr < USBH_DEV_NBR_EP) {
      if (p_if->EPTblPtr[ep_phy_nbr - 2u] != DEF_NULL) {
        ep_addr_next = p_if->EPTblPtr[ep_phy_nbr - 2u]->Addr;
        break;
      }

      ep_phy_nbr++;
    }
  }
    #else
  p_ep = p_if->EPHeadPtr;
  if (ep_addr_prev != 0u) {
    while (p_ep != DEF_NULL) {
      if ((p_ep->Addr == ep_addr_prev)
          && (p_ep->NextPtr != DEF_NULL)) {
        ep_addr_next = p_ep->NextPtr->Addr;
        break;
      }
      p_ep = p_ep->NextPtr;
    }
  } else if (p_ep != DEF_NULL) {
    ep_addr_next = p_ep->Addr;
  }
    #endif

end_rel:
  USBH_DevRelShared(p_dev);

  return (ep_addr_next);
}

/****************************************************************************************************//**
 *                                               USBH_IF_AltSet()
 *
 * @brief    Sets the alternate interface and attempt to open related endpoints.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Index of the interface.
 *
 * @param    if_alt_ix       Index of the alternate interface.
 *
 * @param    callback_fnct   Function to call when operation is completed.
 *
 * @param    p_arg           Pointer to the argument that will be passed to the callback function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
void USBH_IF_AltSet(USBH_DEV_HANDLE      dev_handle,
                    USBH_FNCT_HANDLE     fnct_handle,
                    CPU_INT08U           if_ix,
                    CPU_INT08U           if_alt_ix,
                    USBH_IF_ALT_SET_CMPL callback_fnct,
                    void                 *p_arg,
                    RTOS_ERR             *p_err)
{
  USBH_HUB_EVENT                *p_hub_event;
  USBH_HUB_EVENT_IF_ALT_SET_ARG *p_set_alt_if_arg;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBH_DEV *p_dev;
#endif

    #if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    USBH_IF *p_if;
    USBH_IF *p_if_alt;

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    //                                                             Check as much as possible, before posting event.
    p_dev = USBH_DevAcquireShared(dev_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (p_dev->State != USBH_DEV_STATE_CONFIG) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      goto err_release;
    }

    p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto err_release;
    }

    if (p_if->AltNbr != 0u) {                                   // Make sure it is root IF.
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto err_release;
    }

    p_if_alt = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_alt_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto err_release;
    }

    if (p_if->Nbr != p_if_alt->Nbr) {                           // IF and alt IF must have same nbr.
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto err_release;
    }

    (void)USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto err_release;
    }

    USBH_DevRelShared(p_dev);                                   // No need to keep the p_dev longer.
  }
    #endif

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_IF_ALT_SET;

  p_set_alt_if_arg = (USBH_HUB_EVENT_IF_ALT_SET_ARG *)p_hub_event->ArgPtr;

  p_set_alt_if_arg->DevHandle = dev_handle;
  p_set_alt_if_arg->FnctHandle = fnct_handle;
  p_set_alt_if_arg->IF_Ix = if_ix;
  p_set_alt_if_arg->IF_AltIx = if_alt_ix;
  p_set_alt_if_arg->IF_AltSetCmplCallback = callback_fnct;
  p_set_alt_if_arg->CallbackArgPtr = p_arg;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
err_release:
  USBH_DevRelShared(p_dev);                                     // In err case, release p_dev and return.

  return;
#endif
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_IF_Init()
 *
 * @brief    Initializes USB Host interface module.
 *
 * @param    if_qty  Quantity of interfaces.
 *
 * @param    p_err   Pointer to the variable that will receive the error code from hub function.
 *******************************************************************************************************/
void USBH_IF_Init(CPU_INT16U if_qty,
                  RTOS_ERR   *p_err)
{
  Mem_DynPoolCreate("USBH - IF pool",
                    &USBH_Ptr->IF_Pool,
                    USBH_InitCfg.MemSegPtr,
                    sizeof(USBH_IF),
                    sizeof(CPU_ALIGN),
                    (if_qty != USBH_OBJ_QTY_UNLIMITED) ? if_qty : 0u,
                    (if_qty != USBH_OBJ_QTY_UNLIMITED) ? if_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
}

/****************************************************************************************************//**
 *                                               USBH_IF_Create()
 *
 * @brief    Creates an interface.
 *
 * @param    p_desc_buf  Pointer to the buffer that contains interface descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the interface, if successful.
 *           0,                    otherwise.
 *******************************************************************************************************/
USBH_IF *USBH_IF_Create(CPU_INT08U *p_desc_buf,
                        RTOS_ERR   *p_err)
{
  USBH_IF *p_if;

  p_if = (USBH_IF *)Mem_DynPoolBlkGet(&USBH_Ptr->IF_Pool,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  Mem_Clr((void *)p_if->EPTblPtr,
          (sizeof(USBH_EP *) * (USBH_DEV_NBR_EP - 2u)));
#else
  p_if->EPHeadPtr = DEF_NULL;
  p_if->NextPtr = DEF_NULL;
#endif

  p_if->Nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[2u]);
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  p_if->AltNbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[3u]);
#endif
  p_if->NbrEP = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[4u]);
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) == DEF_YES)
  p_if->ClassCode = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) == DEF_YES)
  p_if->Subclass = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) == DEF_YES)
  p_if->Protocol = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[7u]);
#endif
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  p_if->AltCur = 0u;
#endif
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_if->StrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[8u]);
#endif

  LOG_VRB(("Create interface class ", (u)MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u])));

  return (p_if);
}

/****************************************************************************************************//**
 *                                               USBH_IF_Del()
 *
 * @brief    Deletes interface.
 *
 * @param    p_if    Pointer to the interface.
 *******************************************************************************************************/
void USBH_IF_Del(USBH_IF *p_if)
{
  RTOS_ERR err_lib;
  USBH_EP  *p_ep;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ep_ix;

    for (ep_ix = 0u; ep_ix < (USBH_DEV_NBR_EP - 2u); ep_ix++) {
      p_ep = p_if->EPTblPtr[ep_ix];
      if (p_ep != DEF_NULL) {
        USBH_EP_Del(p_ep);
      }
    }
  }
#else
  {
    USBH_EP *p_ep_next;

    p_ep = p_if->EPHeadPtr;
    while (p_ep != DEF_NULL) {
      p_ep_next = p_ep->NextPtr;
      USBH_EP_Del(p_ep);
      p_ep = p_ep_next;
    }
  }
#endif

  Mem_DynPoolBlkFree(&USBH_Ptr->IF_Pool,
                     (void *)p_if,
                     &err_lib);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_FAIL,; );

  LOG_VRB(("Delete interface"));
}

/****************************************************************************************************//**
 *                                               USBH_IF_EP_Add()
 *
 * @brief    Adds endpoint to interface.
 *
 * @param    p_if    Pointer to the interface.
 *
 * @param    p_ep    Pointer to the endpoint.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Order of the endpoints in the interface list (when USBH_CFG_OPTIMIZE_SPD_EN is set to
 *               DEF_DISABLED) has no importance.
 *******************************************************************************************************/
void USBH_IF_EP_Add(USBH_IF  *p_if,
                    USBH_EP  *p_ep,
                    RTOS_ERR *p_err)
{
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ep_phy_nbr;

    ep_phy_nbr = USBH_EP_ADDR_TO_PHY(p_ep->Addr);
    p_if->EPTblPtr[ep_phy_nbr - 2u] = p_ep;
  }
#else
  if (p_if->EPHeadPtr == DEF_NULL) {
    p_if->EPHeadPtr = p_ep;
  } else {
    p_ep->NextPtr = p_if->EPHeadPtr;
    p_if->EPHeadPtr = p_ep;
  }
#endif

  LOG_VRB(("Added to interface endpoint address ", (X)p_ep->Addr));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBH_IF_PtrGet()
 *
 * @brief    Gets a pointer to given interface.
 *
 * @param    p_config        Pointer to the configuration.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    if_ix           Interface index.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the interface, if successful.
 *           0,                        otherwise.
 *******************************************************************************************************/
USBH_IF *USBH_IF_PtrGet(USBH_CONFIG      *p_config,
                        USBH_FNCT_HANDLE fnct_handle,
                        CPU_INT08U       if_ix,
                        RTOS_ERR         *p_err)
{
  USBH_FNCT *p_fnct;
  USBH_IF   *p_if;

  p_fnct = USBH_FnctPtrGet(p_config, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  if (if_ix >= p_fnct->NbrIF) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (DEF_NULL);
  }

  p_if = USBH_IF_PtrAtIxGet(p_fnct, if_ix);
  if (p_if == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }

  return (p_if);
}

/****************************************************************************************************//**
 *                                           USBH_IF_PtrAtIxGet()
 *
 * @brief    Gets pointer to interface from function at specified index.
 *
 * @param    p_fnct  Pointer to the function.
 *
 * @param    ------  Argument validated by caller.
 *
 * @param    if_ix   Interface index.
 *
 * @param    -----   Argument validated by caller.
 *
 * @return   Pointer to the interface, if successful.
 *           0,                        otherwise.
 *******************************************************************************************************/
USBH_IF *USBH_IF_PtrAtIxGet(USBH_FNCT  *p_fnct,
                            CPU_INT08U if_ix)
{
  USBH_IF *p_if;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  CPU_INT08U if_srch_ix;
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_if = p_fnct->IFTblPtr[if_ix];
#else
  p_if = p_fnct->IFHeadPtr;
  for (if_srch_ix = 0u; if_srch_ix < if_ix; if_srch_ix++) {
    p_if = p_if->NextPtr;
  }
#endif

  return (p_if);
}

/****************************************************************************************************//**
 *                                           USBH_IF_AltSetProcess()
 *
 * @brief    Processes the change of alternate interface for an interface.
 *
 * @param    dev_handle      Handle to device   on which to change the alternate interface.
 *
 * @param    fnct_handle     Handle to function on which to change the alternate interface.
 *
 * @param    if_ix           Index of interface on which to change the alternate interface.
 *
 * @param    if_alt_ix       Index of alternate interface to set.
 *
 * @param    p_err           Pointer to the variable that will receive the error code from hub function.
 *
 * @return   Previous alternate interface index.
 *******************************************************************************************************/

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_AltSetProcess(USBH_DEV_HANDLE  dev_handle,
                                 USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       if_alt_ix,
                                 RTOS_ERR         *p_err)
{
  CPU_INT08U     prev_alt_if_ix = 0u;
  USBH_DEV       *p_dev;
  USBH_FNCT      *p_fnct;
  USBH_IF        *p_if;
  USBH_IF        *p_if_alt;
  USBH_CLASS_DRV *p_class_drv;
  RTOS_ERR       local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_dev->State != USBH_DEV_STATE_CONFIG) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  if (p_dev->IsSuspended == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DEV_SUSPEND);
    goto end_rel;
  }

  p_if = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (p_if->AltNbr != 0u) {                                     // Make sure it is root IF.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    goto end_rel;
  }
#endif

  p_if_alt = USBH_IF_PtrGet(&p_dev->ConfigCur, fnct_handle, if_alt_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (p_if->Nbr != p_if_alt->Nbr) {                             // IF and alt IF must have same nbr.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    goto end_rel;
  }
#endif

  p_fnct = USBH_FnctPtrGet(&p_dev->ConfigCur, fnct_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  prev_alt_if_ix = p_if->AltCur;
  USBH_IF_AltEP_Close(p_dev, p_fnct, p_if->AltCur, p_err);      // Close EPs on current alt IF.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  USBH_IF_AltEP_Open(p_dev, p_fnct, if_alt_ix, p_err);          // Attempt to open EPs on new alt IF.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Opening Alt IF -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto end_restore_open;
  }

  (void)USBH_EP_CtrlXferLocked(p_dev,                           // Issue SetInterface std req.
                               USBH_DEV_REQ_SET_IF,
                               (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_RECIPIENT_IF),
                               p_if_alt->AltNbr,
                               p_if_alt->Nbr,
                               DEF_NULL,
                               0u,
                               0u,
                               USBH_Ptr->StdReqTimeout,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Sending std req to change Alt IF -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto end_restore_close_open;
  }

  p_if->AltCur = if_alt_ix;

  p_class_drv = p_fnct->ClassDrvPtr;
  if (p_class_drv->IF_AltSet != DEF_NULL) {
    p_class_drv->IF_AltSet(p_fnct->ClassFnctPtr, if_alt_ix);
  }

  LOG_VRB(("Alt IF #", (u)if_alt_ix, " set for IF #", (u)if_ix));

  USBH_DevRelShared(p_dev);

  return (prev_alt_if_ix);

end_restore_close_open:
  USBH_IF_AltEP_Close(p_dev, p_fnct, if_alt_ix, &local_err);    // Close EPs on alt IF that was opened.
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Closing Alt IF to re-open previous one -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }

end_restore_open:
  USBH_IF_AltEP_Open(p_dev,                                     // Attempt to recover from failed Open, revert to prev.
                     p_fnct,
                     prev_alt_if_ix,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Re-opening previous Alt IF -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }

end_rel:
  USBH_DevRelShared(p_dev);

  return (prev_alt_if_ix);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_IF_AltEP_Open()
 *
 * @brief    Opens alternate interface's endpoints.
 *
 * @param    p_dev       Pointer to the device structure.
 *
 * @param    p_fnct      Pointer to the function structure.
 *
 * @param    if_alt_ix   Index of alternate interface.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_IF_AltEP_Open(USBH_DEV   *p_dev,
                        USBH_FNCT  *p_fnct,
                        CPU_INT08U if_alt_ix,
                        RTOS_ERR   *p_err)
{
  CPU_BOOLEAN    ep_needed = DEF_YES;
  USBH_IF        *p_if_alt;
  USBH_EP        *p_ep;
  USBH_EP_HANDLE ep_handle;
  RTOS_ERR       ep_close_err;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT08U ep_ix;
  CPU_INT08U ep_cnt;
#endif

  p_if_alt = USBH_IF_PtrAtIxGet(p_fnct, if_alt_ix);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  ep_cnt = 0u;
  ep_ix = 0u;
  while ((ep_cnt < p_if_alt->NbrEP)
         && (ep_ix < 30u)
         && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    p_ep = p_if_alt->EPTblPtr[ep_ix];
#else
  p_ep = p_if_alt->EPHeadPtr;
  while ((p_ep != DEF_NULL)
         && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
#endif

    if (p_ep != DEF_NULL) {
      //                                                           Ensure class driver needs the endpoint.
      if (p_fnct->ClassDrvPtr->EP_Probe != DEF_NULL) {
        ep_needed = p_fnct->ClassDrvPtr->EP_Probe(p_fnct->ClassFnctPtr,
                                                  if_alt_ix,
                                                  USBH_EP_TYPE_GET(p_ep->Attrib),
                                                  USBH_EP_IS_IN(p_ep->Addr));
      }

      if (ep_needed == DEF_YES) {
        ep_handle = USBH_EP_Open(p_dev,
                                 p_fnct,
                                 p_ep,
                                 if_alt_ix,
                                 p_err);
        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
          p_fnct->ClassDrvPtr->EP_Open(p_fnct->ClassFnctPtr,
                                       ep_handle,
                                       if_alt_ix,
                                       USBH_EP_TYPE_GET(p_ep->Attrib),
                                       USBH_EP_IS_IN(p_ep->Addr));
        } else {
          USBH_IF_AltEP_Close(p_dev,
                              p_fnct,
                              if_alt_ix,
                              &ep_close_err);
        }
                #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
        ep_cnt++;
                #endif
      }
    }

        #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    ep_ix++;
        #else
    p_ep = p_ep->NextPtr;
        #endif
  }

  LOG_VRB(("Endpoint opened for alt interface index ", (u)if_alt_ix));
}

/****************************************************************************************************//**
 *                                           USBH_IF_AltEP_Close()
 *
 * @brief    Closes alternate interface's endpoints.
 *
 * @param    p_dev       Pointer to the device structure.
 *
 * @param    p_fnct      Pointer to the function structure.
 *
 * @param    if_alt_ix   Index of alternate interface.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_IF_AltEP_Close(USBH_DEV   *p_dev,
                         USBH_FNCT  *p_fnct,
                         CPU_INT08U if_alt_ix,
                         RTOS_ERR   *p_err)
{
  USBH_EP_MGMT *p_ep_mgmt;
  RTOS_ERR     ep_close_err;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT08U ep_phy_nbr_ix;
#endif
  CORE_DECLARE_IRQ_STATE;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  for (ep_phy_nbr_ix = 2u; ep_phy_nbr_ix < USBH_DEV_NBR_EP; ep_phy_nbr_ix++) {
    CORE_ENTER_ATOMIC();
    p_ep_mgmt = p_dev->EP_MgmtTblPtr[ep_phy_nbr_ix];
    CORE_EXIT_ATOMIC();

#else
  CORE_ENTER_ATOMIC();
  p_ep_mgmt = p_dev->EP_MgmtHeadPtr;
  CORE_EXIT_ATOMIC();

  while (p_ep_mgmt != DEF_NULL) {
#endif
    if ((p_ep_mgmt->FnctPtr == p_fnct)
        && (USBH_HANDLE_IF_IX_GET(p_ep_mgmt->Handle) == if_alt_ix)
        && (USBH_HANDLE_EP_PHY_NBR_GET(p_ep_mgmt->Handle) != 0u)) {
      //                                                           Call class callback fnct.
      p_fnct->ClassDrvPtr->EP_Close(p_fnct->ClassFnctPtr,
                                    p_ep_mgmt->Handle,
                                    if_alt_ix);

      USBH_EP_Close(p_dev, p_ep_mgmt->Handle, &ep_close_err);
      if (RTOS_ERR_CODE_GET(ep_close_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Closing EP -> ", RTOS_ERR_LOG_ARG_GET(ep_close_err)));
      }
    }
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
    CORE_ENTER_ATOMIC();
    p_ep_mgmt = p_ep_mgmt->NextPtr;
    CORE_EXIT_ATOMIC();
#endif
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("Endpoint closed for alt interface index ", (u)if_alt_ix));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
