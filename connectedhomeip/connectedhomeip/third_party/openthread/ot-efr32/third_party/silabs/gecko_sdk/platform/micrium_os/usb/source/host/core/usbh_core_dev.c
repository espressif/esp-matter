/***************************************************************************//**
 * @file
 * @brief USB Host Device Operations
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
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_config_priv.h>
#include  <usb/source/host/core/usbh_core_ep_priv.h>
#include  <usb/source/host/core/usbh_core_class_priv.h>
#include  <usb/source/host/core/usbh_core_hub_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, DEV)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_DEV_LOCK_REF_CNT_MASK                      0x7Fu
#define  USBH_DEV_LOCK_REF_CNT_EXCL_BIT                  DEF_BIT_07

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum usbh_dev_locking_oper_type {
  USBH_DEV_LOCKING_ACQUIRE_SHARED,
  USBH_DEV_LOCKING_ACQUIRE_SHARED_COMPLETING,
  USBH_DEV_LOCKING_ACQUIRE_SHARED_CLOSING,
  USBH_DEV_LOCKING_RELEASE_SHARED,
  USBH_DEV_LOCKING_ACQUIRE_EXCLUSIVE
} USBH_DEV_LOCKING_OPER_TYPE;

typedef struct usbh_dev_locking_args {
  USBH_DEV_LOCKING_OPER_TYPE OperType;
  //                                                               When acquiring, contains valid handle.
  //                                                               When releasing, not used.
  USBH_DEV_HANDLE            Handle;

  //                                                               When acquiring, contains obtained DevPtr.
  //                                                               When releasing, contains DevPtr to release.
  USBH_DEV                   *DevPtr;
  RTOS_ERR                   *ErrPtr;                           // Ptr to variable that will contain err.
} USBH_DEV_LOCKING_ARGS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_DevAllocCallback(MEM_DYN_POOL *p_pool,
                                         MEM_SEG      *p_seg,
                                         void         *p_blk,
                                         void         *p_arg);

static CPU_INT08U USBH_DevAddrAcquire(USBH_HOST *p_host);

static void USBH_DevAddrRelease(USBH_HOST  *p_host,
                                CPU_INT08U dev_addr);

static USBH_DEV *USBH_DevAcquireClosing(USBH_DEV_HANDLE dev_handle,
                                        RTOS_ERR        *p_err);

static USBH_DEV *USBH_DevAcquire(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err);

static KAL_MON_RES USBH_DevMonOnEnter(void *p_mon_data,
                                      void *p_op_data);

static KAL_MON_RES USBH_DevMonOnEval(void *p_mon_data,
                                     void *p_eval_op_data,
                                     void *p_scan_op_data);

static void USBH_DevEnumNotifyStart(USBH_DEV *p_dev);

static void USBH_DevEnumNotifyEnd(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_DevSpdGet()
 *
 * @brief    Gets the device speed.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device speed,      if successful.
 *           USBH_DEV_SPD_NONE, if unsuccessful.
 *******************************************************************************************************/
USBH_DEV_SPD USBH_DevSpdGet(USBH_DEV_HANDLE dev_handle,
                            RTOS_ERR        *p_err)
{
  USBH_DEV     *p_dev;
  USBH_DEV_SPD dev_spd = USBH_DEV_SPD_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_SPD_NONE);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (dev_spd);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  dev_spd = p_dev->Spd;

end_rel:
  USBH_DevRelShared(p_dev);

  return (dev_spd);
}

/****************************************************************************************************//**
 *                                               USBH_DevAddrGet()
 *
 * @brief    Gets the device address.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device address,        if successful.
 *           USBH_DEV_ADDR_INVALID, if unsuccessful.
 *******************************************************************************************************/
CPU_INT08U USBH_DevAddrGet(USBH_DEV_HANDLE dev_handle,
                           RTOS_ERR        *p_err)
{
  CPU_INT08U dev_addr = USBH_DEV_ADDR_INVALID;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_ADDR_INVALID);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (dev_addr);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  dev_addr = p_dev->AddrCur;

end_rel:
  USBH_DevRelShared(p_dev);

  return (dev_addr);
}

/****************************************************************************************************//**
 *                                           USBH_DevHostNbrGet()
 *
 * @brief    Gets the host number to which the device is connected.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Host number, if successful.
 *           0,           if unsuccessful.
 *******************************************************************************************************/
CPU_INT08U USBH_DevHostNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err)
{
  CPU_INT08U host_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  host_nbr = USBH_HANDLE_HOST_IX_GET(p_dev->Handle);

  USBH_DevRelShared(p_dev);

  return (host_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_DevHC_NbrGet()
 *
 * @brief    Gets the host controller number to which the device is connected.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Host controller number, if successful.
 *           USBH_HC_NBR_NONE,       otherwise.
 *******************************************************************************************************/
CPU_INT08U USBH_DevHC_NbrGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err)
{
  CPU_INT08U hc_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_HC_NBR_NONE);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  hc_nbr = USBH_HANDLE_HC_IX_GET(p_dev->Handle);

  USBH_DevRelShared(p_dev);

  return (hc_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_DevHC_HandleGet()
 *
 * @brief    Gets the host controller handle to which the device is connected.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @return   Host controller handle
 *******************************************************************************************************/
USBH_HC_HANDLE USBH_DevHC_HandleGet(USBH_DEV_HANDLE dev_handle)
{
  USBH_HC_HANDLE hc_handle;

  hc_handle = USBH_HC_HANDLE_CREATE(USBH_HANDLE_HOST_IX_GET(dev_handle),
                                    USBH_HANDLE_HC_IX_GET(dev_handle));

  return (hc_handle);
}

/****************************************************************************************************//**
 *                                           USBH_DevSpecNbrGet()
 *
 * @brief    Gets the device specification number.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device specification number, if successful.
 *           0,                           otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bcdUSB' field of the device
 *               descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SPEC_NBR) == DEF_YES)
CPU_INT16U USBH_DevSpecNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err)
{
  CPU_INT16U spec_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  spec_nbr = p_dev->SpecNbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (spec_nbr);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevClassGet()
 *
 * @brief    Gets the device class code.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device class code.
 *
 * @note     (1) The value returned by this function corresponds to the 'bDeviceClass' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/
CPU_INT08U USBH_DevClassGet(USBH_DEV_HANDLE dev_handle,
                            RTOS_ERR        *p_err)
{
  CPU_INT08U class_code = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  class_code = p_dev->ClassCode;

end_rel:
  USBH_DevRelShared(p_dev);

  return (class_code);
}

/****************************************************************************************************//**
 *                                           USBH_DevSubclassGet()
 *
 * @brief    Gets the device subclass code.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device subclass code.
 *
 * @note     (1) The value returned by this function corresponds to the 'bDeviceSubClass' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_DevSubclassGet(USBH_DEV_HANDLE dev_handle,
                               RTOS_ERR        *p_err)
{
  CPU_INT08U subclass = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  subclass = p_dev->Subclass;

end_rel:
  USBH_DevRelShared(p_dev);

  return (subclass);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevProtocolGet()
 *
 * @brief    Gets the device protocol code.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device protocol code.
 *
 * @note     (1) The value returned by this function corresponds to the 'bDeviceProtocol' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1' .
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_DevProtocolGet(USBH_DEV_HANDLE dev_handle,
                               RTOS_ERR        *p_err)
{
  CPU_INT08U protocol = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  protocol = p_dev->Protocol;

end_rel:
  USBH_DevRelShared(p_dev);

  return (protocol);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevVendorID_Get()
 *
 * @brief    Gets the device vendor ID.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device protocol code, if successful.
 *           0,                    otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'idVendor' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
CPU_INT16U USBH_DevVendorID_Get(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err)
{
  CPU_INT16U vendor_id = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  vendor_id = p_dev->VendorID;

end_rel:
  USBH_DevRelShared(p_dev);

  return (vendor_id);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevProductID_Get()
 *
 * @brief    Gets the device product ID.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device protocol code, if successful.
 *           0,                    otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'idProduct' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
CPU_INT16U USBH_DevProductID_Get(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err)
{
  CPU_INT16U product_id = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  product_id = p_dev->ProductID;

end_rel:
  USBH_DevRelShared(p_dev);

  return (product_id);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevRelNbrGet()
 *
 * @brief    Gets the device release number.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device release number, if successful.
 *           0,                     otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bcdDevice' field of the
 *               device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
CPU_INT16U USBH_DevRelNbrGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err)
{
  CPU_INT16U dev_rel_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  dev_rel_nbr = p_dev->RelNbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (dev_rel_nbr);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_DevManufacturerStrGet()
 *
 * @brief    Gets the device manufacturer string.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    p_str_buf       Buffer that will receive the string descriptor.
 *
 * @param    str_buf_len     String buffer length in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_INVALID_DESC
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_USB_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *
 * @return   String length in octets, if successful.
 *           0,                       otherwise.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iManufacturer' field of the device descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0, section 9.6.1'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can determine if a device manufacturer string is present
 *               by passing a str_buf_len of 0. In this case, p_err will be set to
 *               RTOS_ERR_NOT_FOUND if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_DevManufacturerStrGet(USBH_DEV_HANDLE dev_handle,
                                      CPU_CHAR        *p_str_buf,
                                      CPU_INT08U      str_buf_len,
                                      RTOS_ERR        *p_err)
{
  CPU_INT08U str_desc_len = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_dev->ManufacturerStrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_dev->ManufacturerStrIx,
                               p_str_buf,
                               str_buf_len,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);

  return (str_desc_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevProductStrGet()
 *
 * @brief    Gets the device product string.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    p_str_buf       Buffer that will receive the string descriptor.
 *
 * @param    str_buf_len     String buffer length in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_INVALID_DESC
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_USB_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *
 * @return   String length in octets, if successful.
 *           0,                       if unsuccessful.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iProduct' field of the device descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0,
 *               section 9.6.1'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can be used to determine if a device product string is present or
 *               not by passing a str_buf_len of 0. In this case, p_err will be set to
 *               RTOS_ERR_NOT_FOUND if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_DevProductStrGet(USBH_DEV_HANDLE dev_handle,
                                 CPU_CHAR        *p_str_buf,
                                 CPU_INT08U      str_buf_len,
                                 RTOS_ERR        *p_err)
{
  CPU_INT08U str_desc_len = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_dev->ProdStrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_dev->ProdStrIx,
                               p_str_buf,
                               str_buf_len,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);

  return (str_desc_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevSerNbrStrGet()
 *
 * @brief    Gets the device serial number string.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    p_str_buf       Buffer that will receive the string descriptor.
 *
 * @param    str_buf_len     String buffer length in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_INVALID_DESC
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_USB_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *
 * @return   String length in octets, if successful.
 *           0,                       if unsuccessful.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iSerialNumber' field of the device descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0, section 9.6.1'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can be used to determine if a device serial number string is present or
 *               not by passing a str_buf_len of 0. In this case, p_err will be set to
 *               RTOS_ERR_NOT_FOUND if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_DevSerNbrStrGet(USBH_DEV_HANDLE dev_handle,
                                CPU_CHAR        *p_str_buf,
                                CPU_INT08U      str_buf_len,
                                RTOS_ERR        *p_err)
{
  CPU_INT08U str_desc_len = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_dev->SerNbrStrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_dev->SerNbrStrIx,
                               p_str_buf,
                               str_buf_len,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);

  return (str_desc_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevConfigQtyGet()
 *
 * @brief    Gets the quantity of configuration contained in the device.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Number of configuration, if successful.
 *           0,                       otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bNumConfigurations' field
 *               of the device descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.1'.
 *******************************************************************************************************/
CPU_INT08U USBH_DevConfigQtyGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err)
{
  CPU_INT08U cfg_qty = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  cfg_qty = p_dev->NbrConfig;

end_rel:
  USBH_DevRelShared(p_dev);

  return (cfg_qty);
}

/****************************************************************************************************//**
 *                                           USBH_DevConfigSet()
 *
 * @brief    Sets the given device configuration.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    cfg_nbr     Configuration number to set.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_NOT_SUPPORTED
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) Set configuration is not supported on hub devices.
 *
 * @note     (2) Two consecutive calls to this function will result in a failure of the second call,
 *               since the device handle passed as argument will necessarily be invalid.
 *******************************************************************************************************/
void USBH_DevConfigSet(USBH_DEV_HANDLE dev_handle,
                       CPU_INT08U      cfg_nbr,
                       RTOS_ERR        *p_err)
{
  USBH_HUB_EVENT                    *p_hub_event;
  USBH_HUB_EVENT_DEV_CONFIG_SET_ARG *p_set_cfg_arg;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBH_DEV *p_dev;
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Check as much as possible, before posting event.
  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (cfg_nbr > p_dev->NbrConfig) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    goto err_release;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  if (p_dev->ClassDrvPtr == &USBH_HUB_Drv) {                    // Set config not supported on hub dev.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto err_release;
  }
#endif

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto err_release;
  }

  USBH_DevRelShared(p_dev);                                     // No need to keep the p_dev longer.
#endif

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_DEV_CONFIG_SET;

  p_set_cfg_arg = (USBH_HUB_EVENT_DEV_CONFIG_SET_ARG *)p_hub_event->ArgPtr;

  p_set_cfg_arg->DevHandle = dev_handle;
  p_set_cfg_arg->CfgNbr = cfg_nbr;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
err_release:
  USBH_DevRelShared(p_dev);                                     // In err case, release p_dev and return.

  return;
#endif
}

/****************************************************************************************************//**
 *                                           USBH_DevConfigGet()
 *
 * @brief    Gets the currently selected configuration.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device current configuration number.
 *******************************************************************************************************/
CPU_INT08U USBH_DevConfigGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err)
{
  CPU_INT08U config_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_dev->State != USBH_DEV_STATE_CONFIG) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  config_nbr = p_dev->ConfigCur.Nbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (config_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_DevPortNbrGet()
 *
 * @brief    Gets the device hub port number.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device current configuration number.
 *******************************************************************************************************/
CPU_INT08U USBH_DevPortNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err)
{
  CPU_INT08U port_nbr = 0u;
  USBH_DEV   *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  port_nbr = p_dev->PortNbr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (port_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_DevHubHandleGet()
 *
 * @brief    Gets Handle to device's hub.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device's hub handle,     if successful.
 *           USBH_DEV_HANDLE_RH,      if nearest hub is root hub.
 *           USBH_DEV_HANDLE_INVALID, if an error occured.
 *******************************************************************************************************/
USBH_DEV_HANDLE USBH_DevHubHandleGet(USBH_DEV_HANDLE dev_handle,
                                     RTOS_ERR        *p_err)
{
  USBH_DEV        *p_dev;
  USBH_DEV        *p_hub_dev;
  USBH_HUB_FNCT   *p_hub_fnct;
  USBH_DEV_HANDLE hub_handle = USBH_DEV_HANDLE_INVALID;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_hub_fnct = p_dev->HubFnctPtr;
  p_hub_dev = p_hub_fnct->DevPtr;
  if (p_hub_dev != DEF_NULL) {
    hub_handle = p_hub_dev->Handle;
  } else {
    hub_handle = USBH_DEV_HANDLE_RH_CREATE(USBH_HANDLE_HOST_IX_GET(p_dev->Handle),
                                           USBH_HANDLE_HC_IX_GET(p_dev->Handle));
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

end_rel:
  USBH_DevRelShared(p_dev);

  return (hub_handle);
}

/****************************************************************************************************//**
 *                                       USBH_DevHS_HubNearestHandleGet()
 *
 * @brief    Gets the handle of device's nearest hub working at high-speed.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_port_nbr  Pointer to the variable that will receive the port number of the high-speed hub (if not null).
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_DEV_SPD
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Device's nearest high-speed hub handle, if successful.
 *           USBH_DEV_HANDLE_RH,                     if the nearest high-speed hub is the root hub.
 *           USBH_DEV_HANDLE_INVALID,                if an error occurred.
 *******************************************************************************************************/
USBH_DEV_HANDLE USBH_DevHS_HubNearestHandleGet(USBH_DEV_HANDLE dev_handle,
                                               CPU_INT08U      *p_port_nbr,
                                               RTOS_ERR        *p_err)
{
  CPU_INT08U      port_nbr;
  USBH_DEV        *p_dev;
  USBH_DEV        *p_hub_dev;
  USBH_HUB_FNCT   *p_hub_fnct;
  USBH_DEV_HANDLE hub_handle = USBH_DEV_HANDLE_INVALID;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  if (p_dev->HC_Ptr->Drv.HW_InfoPtr->RH_Spd != USBH_DEV_SPD_HIGH) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_SPD);
    goto end_rel;
  }

  p_hub_fnct = p_dev->HubFnctPtr;
  port_nbr = p_dev->PortNbr;
  while ((p_hub_fnct->DevPtr != DEF_NULL)
         && (p_hub_fnct->DevPtr->Spd != USBH_DEV_SPD_HIGH)) {
    port_nbr = p_hub_fnct->DevPtr->PortNbr;
    p_hub_fnct = p_hub_fnct->DevPtr->HubFnctPtr;
  }

  p_hub_dev = p_hub_fnct->DevPtr;
  if (p_hub_dev != DEF_NULL) {
    hub_handle = p_hub_dev->Handle;
  } else {
    hub_handle = USBH_DEV_HANDLE_RH_CREATE(USBH_HANDLE_HOST_IX_GET(p_dev->Handle),
                                           USBH_HANDLE_HC_IX_GET(p_dev->Handle));
  }

  if (p_port_nbr != DEF_NULL) {
    *p_port_nbr = port_nbr;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

end_rel:
  USBH_DevRelShared(p_dev);

  return (hub_handle);
}

/****************************************************************************************************//**
 *                                               USBH_DevDescRd()
 *
 * @brief    Reads the descriptor from the device.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    recipient       Recipient of the Get descriptor request.
 *                               - USBH_REQ_RECIPIENT_DEV
 *                               - USBH_REQ_RECIPIENT_IF
 *                               - USBH_REQ_RECIPIENT_EP
 *
 * @param    type            Request type.
 *                               - USBH_REQ_TYPE_STD
 *                               - USBH_REQ_TYPE_CLASS
 *                               - USBH_REQ_TYPE_VENDOR
 *
 * @param    desc_type       Descriptor type.
 *                               - USBH_DESC_TYPE_DEV
 *                               - USBH_DESC_TYPE_CONFIG
 *                               - USBH_DESC_TYPE_STR
 *                               - USBH_DESC_TYPE_DEV_QUALIFIER
 *                               - USBH_DESC_TYPE_OTHER_SPD_CONFIG
 *                               - USBH_DESC_TYPE_IF_PWR
 *                               - USBH_DESC_TYPE_OTG
 *
 * @param    desc_ix         wIndex value that should be used in the setup request.
 *
 * @param    desc_len_req    wLength value to use in the setup request.
 *
 * @param    p_desc_buf      Pointer to the buffer that will receive the descriptor.
 *
 * @param    desc_buf_len    Length in octets of the descriptor buffer.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_USB_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *
 * @return   Descriptor length in octets.
 *
 * @note     (1) The GET_DESCRIPTOR request is described in 'Universal Serial Bus Specification
 *               Revision 2.0, section 9.4.3'.
 *******************************************************************************************************/
CPU_INT16U USBH_DevDescRd(USBH_DEV_HANDLE dev_handle,
                          CPU_INT08U      recipient,
                          CPU_INT08U      type,
                          CPU_INT08U      desc_type,
                          CPU_INT08U      desc_ix,
                          CPU_INT16U      desc_len_req,
                          CPU_INT08U      *p_desc_buf,
                          CPU_INT16U      desc_buf_len,
                          RTOS_ERR        *p_err)
{
  CPU_INT16U xfer_len;
  USBH_DEV   *p_dev;

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  xfer_len = USBH_DevDescRdLocked(p_dev,
                                  recipient,
                                  type,
                                  desc_type,
                                  desc_ix,
                                  desc_len_req,
                                  p_desc_buf,
                                  desc_buf_len,
                                  p_err);

  USBH_DevRelShared(p_dev);

  return (xfer_len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_DevInit()
 *
 * @brief    Initializes USB Host device module.
 *
 * @param    dev_qty     Quantity of device.
 *
 * @param    p_err       Pointer to the variable that will receive the error code from hub function.
 *******************************************************************************************************/
void USBH_DevInit(CPU_INT16U dev_qty,
                  RTOS_ERR   *p_err)
{
  Mem_DynPoolCreatePersistent("USBH - Device pool",
                              &USBH_Ptr->DevPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_DEV),
                              sizeof(CPU_ALIGN),
                              (dev_qty != USBH_OBJ_QTY_UNLIMITED) ? dev_qty : 0u,
                              (dev_qty != USBH_OBJ_QTY_UNLIMITED) ? dev_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_DevAllocCallback,
                              DEF_NULL,
                              p_err);
}

/****************************************************************************************************//**
 *                                               USBH_DevConn()
 *
 * @brief    Connects a device.
 *
 * @param    dev_spd     Device speed.
 *                       USBH_DEV_SPD_LOW
 *                       USBH_DEV_SPD_FULL
 *                       USBH_DEV_SPD_HIGH
 *
 * @param    p_host      Pointer to the host to which device is connected.
 *
 * @param    p_hc        Pointer to the host controller to which device is connected.
 *
 * @param    p_hub_fnct  Pointer to the hub function to which device is connected.
 *
 * @param    port_nbr    Hub port number to which device is connected.
 *
 * @return   Pointer to the USB device, if device connected and is at least in addressed state.
 *           0,                     otherwise.
 *
 * @note     (1) This function MUST only be called from the hub task. Caller MUST acquire lock on hub
 *               only.
 *******************************************************************************************************/
USBH_DEV *USBH_DevConn(USBH_DEV_SPD  dev_spd,
                       USBH_HOST     *p_host,
                       USBH_HC       *p_hc,
                       USBH_HUB_FNCT *p_hub_fnct,
                       CPU_INT08U    port_nbr)
{
  CPU_BOOLEAN     probe_ok;
  CPU_BOOLEAN     dev_accept;
  CPU_BOOLEAN     must_disconn = DEF_NO;
  CPU_INT08U      dev_addr;
  CPU_INT08U      hub_dev_addr;
  CPU_INT08U      *p_desc_buf;
  CPU_INT16U      desc_len;
  USBH_DEV_HANDLE dev_handle;
  USBH_DEV        *p_dev = DEF_NULL;
  RTOS_ERR        err;

  dev_addr = USBH_DevAddrAcquire(p_host);                       // Acquire an address for this new device.
  if (dev_addr == USBH_DEV_ADDR_INVALID) {
    RTOS_ERR_SET(err, RTOS_ERR_DEV_ALLOC);
    goto end_err_dev;
  }

  p_dev = (USBH_DEV *)Mem_DynPoolBlkGet(&USBH_Ptr->DevPool,
                                        &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(err, RTOS_ERR_DEV_ALLOC);
    goto end_err_rel_addr;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)                   // Insert device in host's list.
  p_host->DevTbl[dev_addr - 1u] = p_dev;
#else
  {
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    p_dev->HostDevNextPtr = p_host->DevHeadPtr;
    p_host->DevHeadPtr = p_dev;
    CORE_EXIT_ATOMIC();
  }
#endif

  //                                                               Init device.
  USBH_DEV_HANDLE_UPDATE(p_dev, p_host->Ix, p_hc->Ix, dev_addr);
  p_dev->State = USBH_DEV_STATE_ATTCH;
  p_dev->IsSuspended = DEF_NO;
  p_dev->AddrCur = 0u;
  p_dev->Spd = dev_spd;
  p_dev->Addr = dev_addr;
  p_dev->HubFnctPtr = p_hub_fnct;
  p_dev->PortNbr = port_nbr;
  p_dev->HC_Ptr = p_hc;
  p_dev->HostPtr = p_host;
  p_dev->ClassDrvPtr = DEF_NULL;
  p_dev->ClassFnctPtr = DEF_NULL;
  p_dev->IsClassDevLevel = DEF_NO;

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_dev->LangID = 0u;
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_dev->EP_MgmtTblPtr[0u] = DEF_NULL;
#else
  p_dev->EP_MgmtHeadPtr = DEF_NULL;
#endif

  dev_handle = p_dev->Handle;

  switch (p_dev->Spd) {
    case USBH_DEV_SPD_HIGH:
      desc_len = USBH_DESC_LEN_DEV;
      p_dev->CtrlMaxPacketSize = 64u;

      LOG_VRB(("High-Speed device connected on host #", (u)p_dev->HostPtr->Ix));
      break;

    case USBH_DEV_SPD_FULL:
      desc_len = 8u;
      p_dev->CtrlMaxPacketSize = 64u;

      LOG_VRB(("Full-Speed device connected on host #", (u)p_dev->HostPtr->Ix));
      break;

    case USBH_DEV_SPD_LOW:
    default:
      desc_len = 8u;
      p_dev->CtrlMaxPacketSize = 8u;

      LOG_VRB(("Low-Speed device connected on host #", (u)p_dev->HostPtr->Ix));
      break;
  }

  p_dev->CtrlEP_Handle = USBH_EP_Open(p_dev,                    // Open dflt ctrl EPs.
                                      DEF_NULL,
                                      DEF_NULL,
                                      0u,
                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_err_rel_addr_and_dev;
  }

  USBH_DevEnumNotifyStart(p_dev);

  p_desc_buf = USBH_Ptr->DescBufPtr;

  USBH_Ptr->DescLen = USBH_DevDescRdLocked(p_dev,               // Can use 'Locked' fnct since handle is 'new'.
                                           USBH_DEV_REQ_RECIPIENT_DEV,
                                           USBH_DEV_REQ_TYPE_STD,
                                           USBH_DESC_TYPE_DEV,
                                           0u,
                                           desc_len,
                                           p_desc_buf,
                                           desc_len,
                                           &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    must_disconn = DEF_YES;
    goto end_err;
  }

  if (USBH_Ptr->DescLen != desc_len) {
    RTOS_ERR_SET(err, RTOS_ERR_INVALID_DESC);
    must_disconn = DEF_YES;
    goto end_err;
  }

  //                                                               Retrieve ctrl EP max pkt size.
  p_dev->CtrlMaxPacketSize = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[7u]);

  USBH_HUB_PortResetProcess(p_hub_fnct,
                            port_nbr,
                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    must_disconn = DEF_YES;
    goto end_err;
  }

  USBH_DevAddrSet(p_dev, &err);                                 // Set new addr to dev. This call will re-open ctrl EP.
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    must_disconn = DEF_YES;
    goto end_err;
  }

  //                                                               Can use 'Locked' fnct since handle is 'new'.
  USBH_Ptr->DescLen = USBH_DevDescRdLocked(p_dev,               // Read again dev desc.
                                           USBH_DEV_REQ_RECIPIENT_DEV,
                                           USBH_DEV_REQ_TYPE_STD,
                                           USBH_DESC_TYPE_DEV,
                                           0u,
                                           USBH_DESC_LEN_DEV,
                                           p_desc_buf,
                                           USBH_DESC_LEN_DEV,
                                           &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  if (USBH_Ptr->DescLen != USBH_DESC_LEN_DEV) {
    RTOS_ERR_SET(err, RTOS_ERR_INVALID_DESC);
    goto end_err;
  }

  //                                                               Parse dev desc.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SPEC_NBR) == DEF_YES)
  p_dev->SpecNbr = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[2u]);
#endif
  p_dev->ClassCode = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[4u]);
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) == DEF_YES)
  p_dev->Subclass = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) == DEF_YES)
  p_dev->Protocol = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
  p_dev->VendorID = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[8u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
  p_dev->ProductID = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[10u]);
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
  p_dev->RelNbr = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[12u]);
#endif
  p_dev->NbrConfig = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[17u]);
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_dev->ManufacturerStrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[14u]);
  p_dev->ProdStrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[15u]);
  p_dev->SerNbrStrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[16u]);
#endif

  //                                                               Notify app of dev connection.
  dev_accept = USBH_DevConnAcceptNotify();
  if (dev_accept != DEF_YES) {
    RTOS_ERR_SET(err, RTOS_ERR_DEV_CONN_DECLINED);
    must_disconn = DEF_YES;
    goto end_err;
  }

  probe_ok = USBH_ClassProbeDev(p_dev, &err);                   // Probe classes using dev desc.
  if ((probe_ok == DEF_OK)
      && (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)) {
    goto end_err;
  }

  USBH_ConfigOpen(p_dev, 1u, &err);                             // Open default config.
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  //                                                               After USBH_ConfigOpen(), the DEV_HANDLE has been ...
  //                                                               provided to the app. Its fields should not be modif.
  USBH_DevEnumNotifyEnd();

  return (p_dev);

end_err:
  hub_dev_addr = (p_hub_fnct->DevPtr == DEF_NULL) ? USBH_DEV_ADDR_RH : p_hub_fnct->DevPtr->AddrCur;

  USBH_DevConnFailNotify(hub_dev_addr, port_nbr, err);
  USBH_DevEnumNotifyEnd();

  if (must_disconn == DEF_YES) {                                // In some error cases, dev must disconnect.
    USBH_DevDisconn(dev_handle);
    p_dev = DEF_NULL;
  }

  return (p_dev);

end_err_rel_addr_and_dev:
  {
    RTOS_ERR err_lib;

    Mem_DynPoolBlkFree(&USBH_Ptr->DevPool,
                       (void *)p_dev,
                       &err_lib);
    (void)&err_lib;
  }

end_err_rel_addr:
  USBH_DevAddrRelease(p_host, dev_addr);

end_err_dev:
  hub_dev_addr = (p_hub_fnct->DevPtr == DEF_NULL) ? USBH_DEV_ADDR_RH : p_hub_fnct->DevPtr->AddrCur;

  USBH_DevConnFailNotify(hub_dev_addr, port_nbr, err);
  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                               USBH_DevDisconn()
 *
 * @brief    Disconnects a device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @note     (1) This function MUST only be called from the hub task. Caller MUST NOT acquire lock.
 *
 * @note     (2) At this point, the p_dev will no longer be used, even internally. Invalidate handle
 *               first and then clear the bit marking this device as being used exclusively. This will
 *               allow future uses of this p_dev structure.
 *******************************************************************************************************/
void USBH_DevDisconn(USBH_DEV_HANDLE dev_handle)
{
  USBH_DEV *p_dev;
  RTOS_ERR err;

  p_dev = USBH_DevAcquireClosing(dev_handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring p_dev in USBH_DevDisconn -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  USBH_EP_AbortLocked(p_dev,
                      p_dev->CtrlEP_Handle,
                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Aborting ctrl EP in USBH_DevDisconn -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  USBH_DevRelShared(p_dev);

  p_dev = USBH_DevAcquire(dev_handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Trying to acquire dev in disconn -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  USBH_ConfigClose(p_dev);                                      // Close current config and free rsrc.

  USBH_DevEnumNotifyStart(p_dev);
  USBH_DevDisconnNotify(dev_handle);
  USBH_DevEnumNotifyEnd();

  USBH_EP_Close(p_dev,                                          // Close ctrl EP.
                p_dev->CtrlEP_Handle,
                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Closing ctrl EP -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  //                                                               Invalidate and make avail, see Note #2.
  USBH_DEV_HANDLE_INVALIDATE(p_dev);
  DEF_BIT_CLR(p_dev->LockRefCnt, (CPU_INT08U)USBH_DEV_LOCK_REF_CNT_EXCL_BIT);

  LOG_VRB(("Device on host #", (u)p_dev->HostPtr->Ix,
           " disconnected at addr ", (u)p_dev->AddrCur));

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)                   // Remove device from host's list.
  p_dev->HostPtr->DevTbl[p_dev->Addr - 1u] = DEF_NULL;
#else
  {
    USBH_HOST *p_host;
    CORE_DECLARE_IRQ_STATE;

    p_host = p_dev->HostPtr;

    CORE_ENTER_ATOMIC();
    if (p_host->DevHeadPtr == p_dev) {
      p_host->DevHeadPtr = p_dev->HostDevNextPtr;
    } else {
      USBH_DEV *p_dev_prev;
      USBH_DEV *p_dev_cur;

      p_dev_prev = DEF_NULL;
      p_dev_cur = p_host->DevHeadPtr;

      while (p_dev_cur != p_dev) {
        p_dev_prev = p_dev_cur;
        p_dev_cur = p_dev_cur->HostDevNextPtr;
      }

      p_dev_prev->HostDevNextPtr = p_dev->HostDevNextPtr;
    }
    CORE_EXIT_ATOMIC();
  }
#endif

  USBH_DevAddrRelease(p_dev->HostPtr, p_dev->Addr);             // Release device address.

  Mem_DynPoolBlkFree(&USBH_Ptr->DevPool,
                     (void *)p_dev,
                     &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing device -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/****************************************************************************************************//**
 *                                               USBH_DevSuspend()
 *
 * @brief    Suspends a device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @note     (1) This function MUST only be called from the hub task. Since the HUB task is the
 *               highest priority  task that can modify USBH fields, once the lock is acquired, it is
 *               safe to do anything as long as the task does not pend/delay.
 *******************************************************************************************************/
void USBH_DevSuspend(USBH_DEV_HANDLE dev_handle)
{
  USBH_DEV *p_dev;
  RTOS_ERR err;

  p_dev = USBH_DevAcquireShared(dev_handle, &err);              // See Note #1.
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to acquire device for EP suspend -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  USBH_EP_SuspendLocked(p_dev, DEF_YES);

  if (p_dev->IsSuspended == DEF_NO) {
    USBH_ClassSuspend(&p_dev->ConfigCur);

    p_dev->IsSuspended = DEF_YES;
  }

  USBH_DevRelShared(p_dev);
}

/****************************************************************************************************//**
 *                                               USBH_DevResume()
 *
 * @brief    Resumes a device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @note     (1) This function MUST only be called from the hub task. Since the HUB task is the
 *               highest priority  task that can modify USBH fields, once the lock is acquired, it is
 *               safe to do anything as long as the task does not pend/delay.
 *******************************************************************************************************/
void USBH_DevResume(USBH_DEV_HANDLE dev_handle)
{
  USBH_DEV *p_dev;
  RTOS_ERR err;

  p_dev = USBH_DevAcquireShared(dev_handle, &err);              // See Note #1.
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to acquire device for EP resume -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  if (p_dev->IsSuspended == DEF_YES) {
    p_dev->IsSuspended = DEF_NO;

    USBH_ClassResume(&p_dev->ConfigCur);
  }

  USBH_EP_SuspendLocked(p_dev, DEF_NO);

  USBH_DevRelShared(p_dev);
}

/****************************************************************************************************//**
 *                                               USBH_DevAddrSet()
 *
 * @brief    Assign address to device.
 *
 * @param    p_dev   Pointer to the device.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) The SET_ADDRESS request is described in "Universal Serial Bus Specification
 *               Revision 2.0", section 9.4.6.
 *
 * @note     (2) USB 2.0 spec states that after issuing SET_ADDRESS request, the host must wait
 *               at least 2 millisecs. During this time the device will go into the addressed state.
 *
 * @note     (3) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_DevAddrSet(USBH_DEV *p_dev,
                     RTOS_ERR *p_err)
{
  CPU_INT08U retry;
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("Device on host #", (u)p_dev->HostPtr->Ix,
           " address set to ", (u)p_dev->Addr));

  retry = 3u;
  while (retry > 0u) {
    retry--;
    (void)USBH_EP_CtrlXferLocked(p_dev,
                                 USBH_DEV_REQ_SET_ADDR,
                                 (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_RECIPIENT_DEV),
                                 p_dev->Addr,
                                 0u,
                                 DEF_NULL,
                                 0u,
                                 0u,
                                 std_req_timeout_ms,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {             // If err is IO, retry xfer.
      RTOS_ERR err_abort;

      USBH_EP_AbortLocked(p_dev,
                          p_dev->CtrlEP_Handle,
                          &err_abort);
      if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
        LOG_ERR(("Aborting endpoint after ctrl xfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
      }

      KAL_Dly(100u);
    } else {
      break;
    }
  }
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_EP_CtrlReset(p_dev, p_err);

  p_dev->AddrCur = p_dev->Addr;
  p_dev->State = USBH_DEV_STATE_ADDR;

  KAL_Dly(10u + 20u);                                           // See Note (2).

  return;
}

/****************************************************************************************************//**
 *                                       USBH_DevConfigSetProcess()
 *
 * @brief    Change current configuration of device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    cfg_nbr     Configuration number to set.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) This function MUST only be called from the hub task. Caller MUST NOT acquire lock.
 *
 * @note     (2) If we end up at that point and that p_dev is NULL, it means that the handle provided
 *               by the application has became invalid between its call to DevConfigSet() and the
 *               call to this function. This may be the case if two consecutive DevConfigSet() calls
 *               have been done, which is not supported, or if the device has been disconnected.
 *               In this case, the callback is called but USBH_DEV_HANDLE_NOTIFICATION will not be
 *               associated to a USBH_DEV structure and its usage will result in a
 *               RTOS_ERR_INVALID_HANDLE error.
 *
 * @note     (3) At this point, the p_dev will no longer be used, even internally. Invalidate handle
 *               first and then clear the bit marking this device as being used exclusively. This will
 *               allow future uses of this p_dev structure.
 *******************************************************************************************************/
void USBH_DevConfigSetProcess(USBH_DEV_HANDLE dev_handle,
                              CPU_INT08U      cfg_nbr,
                              RTOS_ERR        *p_err)
{
  USBH_DEV *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Acquire lock on dev on which config is being set.
  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_notify;
  }

  if (cfg_nbr > p_dev->NbrConfig) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    goto end_rel_shared_notify;
  }

  if (((cfg_nbr == p_dev->ConfigCur.Nbr)
       && (p_dev->State == USBH_DEV_STATE_CONFIG))
      || ((cfg_nbr == 0u)
          && (p_dev->State == USBH_DEV_STATE_ADDR))) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto end_rel_shared_notify;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  if (p_dev->ClassDrvPtr == &USBH_HUB_Drv) {                    // Set config not supported on hub dev.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel_shared_notify;
  }
#endif

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel_shared_notify;
  }

  if (p_dev->IsSuspended == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DEV_SUSPEND);
    goto end_rel_shared_notify;
  }

  USBH_DevRelShared(p_dev);

  (void)USBH_DevAcquire(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_notify;
  }

  if (p_dev->State == USBH_DEV_STATE_CONFIG) {
    USBH_ConfigClose(p_dev);                                    // Close current config.
  }

  //                                                               Invalidate and make avail, see Note #3.
  USBH_DEV_HANDLE_INVALIDATE(p_dev);
  DEF_BIT_CLR(p_dev->LockRefCnt, (CPU_INT08U)USBH_DEV_LOCK_REF_CNT_EXCL_BIT);

  USBH_DEV_HANDLE_UPDATE(p_dev, p_dev->HostPtr->Ix, p_dev->HC_Ptr->Ix, p_dev->AddrCur);

  USBH_EP_CtrlReset(p_dev, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_notify_reset_cfg;
  }

  USBH_DevEnumNotifyStart(p_dev);

  if (cfg_nbr != 0u) {
    USBH_ConfigOpen(p_dev, cfg_nbr, p_err);                     // Open new config.
  } else {
    USBH_ConfigSet(p_dev, 0u, p_err);                           // Issue SetConfiguration(0) req if requested by app.

    USBH_ConfigSetNotify(p_dev->Handle,
                         0u,
                         *p_err);
  }
  USBH_DevEnumNotifyEnd();

  return;

end_rel_shared_notify:
  USBH_DevEnumNotifyStart(p_dev);
  USBH_ConfigSetNotify(dev_handle,
                       cfg_nbr,
                       *p_err);
  USBH_DevEnumNotifyEnd();
  USBH_DevRelShared(p_dev);
  return;

end_rel_notify:
  if (p_dev != DEF_NULL) {                                      // See Note #2.
    USBH_DevEnumNotifyStart(p_dev);
    USBH_ConfigSetNotify(dev_handle,
                         cfg_nbr,
                         *p_err);
    USBH_DevEnumNotifyEnd();
  } else {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_DEV_HANDLE_HAS_CHANGED);
    USBH_ConfigSetNotify(dev_handle,
                         cfg_nbr,
                         local_err);
  }

  return;

end_rel_notify_reset_cfg:
  {
    RTOS_ERR local_err;

    USBH_DevEnumNotifyStart(p_dev);

    USBH_ConfigSet(p_dev, 0u, &local_err);                      // Try to set cfg 0.

    USBH_ConfigSetNotify(p_dev->Handle,
                         cfg_nbr,
                         *p_err);

    USBH_ConfigSetNotify(p_dev->Handle,
                         0u,
                         local_err);
    USBH_DevEnumNotifyEnd();
  }
  return;
}

/****************************************************************************************************//**
 *                                               USBH_DevStrRd()
 *
 * @brief    Read string descriptor from device.
 *
 * @param    p_dev       Pointer on USBH_DEV structure.
 *
 * @param    str_ix      Index of string.
 *
 * @param    p_buf       Pointer to the buffer that will receive the string descriptor.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Number of octets received, if successful.
 *           0,                         otherwise.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request. See 'Universal Serial Bus specification, revision 2.0, section
 *               9.6.3' for more information.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) Device must be acquired by caller.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_DevStrRd(USBH_DEV   *p_dev,
                         CPU_INT08U str_ix,
                         void       *p_buf,
                         CPU_INT08U buf_len,
                         RTOS_ERR   *p_err)
{
  CPU_INT08U retry;
  CPU_INT08U xfer_len = 0u;
  CPU_INT08U cnt;
  CPU_INT08U *p_buf08;
  CPU_INT16U lang_id;
  CPU_INT16U preffered_lang_id;
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  preffered_lang_id = USBH_Ptr->PreferredStrLangID;
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((buf_len >= 4u), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  LOG_VRB(("String at ix ", (u)str_ix, " read for dev addr ", (u)p_dev->Addr));

  //                                                               ------------ READ LANG ID TBL FROM DEV -------------
  if (p_dev->LangID == 0u) {
    LOG_VRB(("Reading lang ID tbl for dev addr ", (u)p_dev->Addr));

    retry = 3u;
    while (retry > 0u) {
      retry--;

      //                                                           Req lang ID tbl.
      xfer_len = (CPU_INT08U)USBH_EP_CtrlXferLocked(p_dev,
                                                    USBH_DEV_REQ_GET_DESC,
                                                    (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_RECIPIENT_DEV),
                                                    ((USBH_DESC_TYPE_STR << 8u) | USBH_DESC_STR_LANGID),
                                                    0u,
                                                    (CPU_INT08U *)p_buf,
                                                    buf_len,
                                                    buf_len,
                                                    std_req_timeout_ms,
                                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {           // If err is IO, retry xfer.
        RTOS_ERR err_abort;

        USBH_EP_AbortLocked(p_dev,
                            p_dev->CtrlEP_Handle,
                            &err_abort);
        if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
          LOG_ERR(("Aborting endpoint after ctrl xfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
        }

        KAL_Dly(100u);
      } else {
        break;
      }
    }
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    if (xfer_len < 4u) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      return (0u);
    }

    //                                                             ------------- DETERMINE LANG ID TO USE -------------
    p_buf08 = (CPU_INT08U *)p_buf;
    cnt = 2u;

    //                                                             Use first lang id by dflt.
    p_dev->LangID = MEM_VAL_GET_INT16U_LITTLE(&p_buf08[cnt]);

    while (cnt < xfer_len) {
      lang_id = MEM_VAL_GET_INT16U_LITTLE(&p_buf08[cnt]);
      if (lang_id == preffered_lang_id) {
        p_dev->LangID = preffered_lang_id;
        break;
      }

      cnt += 2u;
    }
  }

  //                                                               --------------- READ STRING FROM DEV ---------------
  retry = 3u;
  while (retry > 0u) {
    retry--;

    xfer_len = (CPU_INT08U)USBH_EP_CtrlXferLocked(p_dev,
                                                  USBH_DEV_REQ_GET_DESC,
                                                  (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_RECIPIENT_DEV),
                                                  ((USBH_DESC_TYPE_STR << 8u) | str_ix),
                                                  p_dev->LangID,
                                                  (CPU_INT08U *)p_buf,
                                                  buf_len,
                                                  buf_len,
                                                  std_req_timeout_ms,
                                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {             // If err is IO, retry xfer.
      RTOS_ERR err_abort;

      USBH_EP_AbortLocked(p_dev,
                          p_dev->CtrlEP_Handle,
                          &err_abort);
      if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
        LOG_ERR(("Aborting endpoint after ctrl xfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
      }

      KAL_Dly(100u);
    } else {
      break;
    }
  }

  return (xfer_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevDescRdLocked()
 *
 * @brief    Read descriptor from device.
 *
 * @param    p_dev           Pointer to the USBH_DEV structure.
 *
 * @param    recipient       Recipient of the get descriptor request.
 *                           USBH_REQ_RECIPIENT_DEV
 *                           USBH_REQ_RECIPIENT_IF
 *                           USBH_REQ_RECIPIENT_EP
 *
 * @param    type            Request type.
 *                           USBH_REQ_TYPE_STD
 *                           USBH_REQ_TYPE_CLASS
 *                           USBH_REQ_TYPE_VENDOR
 *
 * @param    desc_type       Descriptor type.
 *                           USBH_DESC_TYPE_DEV
 *                           USBH_DESC_TYPE_CONFIG
 *                           USBH_DESC_TYPE_STR
 *                           USBH_DESC_TYPE_DEV_QUALIFIER
 *                           USBH_DESC_TYPE_OTHER_SPD_CONFIG
 *                           USBH_DESC_TYPE_IF_PWR
 *                           USBH_DESC_TYPE_OTG
 *                           ClassCode  specific descriptor type
 *                           Vendor specific descriptor type
 *
 * @param    desc_ix         wIndex value that should be used in the setup request.
 *
 * @param    desc_len_req    wLength value to use in the setup request.
 *
 * @param    p_desc_buf      Pointer to the buffer that will receive the descriptor.
 *
 * @param    desc_buf_len    Length in octets of the descriptor buffer.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Descriptor length in octets.
 *
 * @note     (1) The GET_DESCRIPTOR request is described in 'Universal Serial Bus Specification
 *               Revision 2.0, section 9.4.3'.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
CPU_INT16U USBH_DevDescRdLocked(USBH_DEV   *p_dev,
                                CPU_INT08U recipient,
                                CPU_INT08U type,
                                CPU_INT08U desc_type,
                                CPU_INT08U desc_ix,
                                CPU_INT16U desc_len_req,
                                CPU_INT08U *p_desc_buf,
                                CPU_INT16U desc_buf_len,
                                RTOS_ERR   *p_err)
{
  CPU_INT08U retry;
  CPU_INT16U xfer_len = 0u;
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((recipient == USBH_DEV_REQ_RECIPIENT_DEV)
                           || (recipient == USBH_DEV_REQ_RECIPIENT_IF)
                           || (recipient == USBH_DEV_REQ_RECIPIENT_EP)
                           || (recipient == USBH_DEV_REQ_RECIPIENT_OTHER)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((type == USBH_DEV_REQ_TYPE_STD)
                           || (type == USBH_DEV_REQ_TYPE_CLASS)
                           || (type == USBH_DEV_REQ_TYPE_VENDOR)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_desc_buf != DEF_NULL)
                           && (desc_buf_len != 0u)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  LOG_VRB(("Desc rd, recipient = ", (X)recipient, ", type = ", (X)type, ", desc type = ", (X)desc_type));

  retry = 10u;
  while (retry > 0u) {
    retry--;

    xfer_len = USBH_EP_CtrlXferLocked(p_dev,
                                      USBH_DEV_REQ_GET_DESC,
                                      (recipient | type | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                                      ((desc_type << 8u) | (desc_ix)),
                                      0u,
                                      p_desc_buf,
                                      desc_len_req,
                                      desc_buf_len,
                                      std_req_timeout_ms,
                                      p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO)              // If err is IO, retry xfer.
        || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT)) {
      RTOS_ERR err_abort;

      USBH_EP_AbortLocked(p_dev,
                          p_dev->CtrlEP_Handle,
                          &err_abort);
      if (RTOS_ERR_CODE_GET(err_abort) != RTOS_ERR_NONE) {
        LOG_ERR(("Aborting endpoint after ctrl xfer -> ", RTOS_ERR_LOG_ARG_GET(err_abort)));
      }

      KAL_Dly(300u);
    } else {
      break;
    }
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                               USBH_DevGet()
 *
 * @brief    Gets a pointer to given device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @return   Pointer to the device structure, if successful.
 *           0,                           otherwise.
 *******************************************************************************************************/
USBH_DEV *USBH_DevGet(USBH_DEV_HANDLE dev_handle)
{
  CPU_INT08U host_ix;
  CPU_INT08U dev_addr;
  USBH_HOST  *p_host;
  USBH_DEV   *p_dev;

  if (dev_handle == USBH_DEV_HANDLE_NOTIFICATION) {
    return (USBH_Ptr->DescDevPtr);
  }

  host_ix = USBH_HANDLE_HOST_IX_GET(dev_handle);
  dev_addr = USBH_HANDLE_DEV_ADDR_GET(dev_handle);

  RTOS_ASSERT_DBG((host_ix < USBH_Ptr->HostQty), RTOS_ERR_NULL_PTR, DEF_NULL);

  p_host = &USBH_Ptr->HostTblPtr[host_ix];
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    CPU_INT08U ix = (dev_addr - 1u) / DEF_INT_32_NBR_BITS;
    CPU_INT08U pos = (dev_addr - 1u) % DEF_INT_32_NBR_BITS;

    RTOS_ASSERT_DBG(((dev_addr != 0u)
                     && (dev_addr <= USBH_InitCfg.OptimizeSpd.DevPerHostQty)), RTOS_ERR_INVALID_ARG, DEF_NULL);

    //                                                             Make sure addr is currently in use.
    if (DEF_BIT_IS_CLR(p_host->DevAddrPool[ix], DEF_BIT(pos)) == DEF_FALSE) {
      return (DEF_NULL);
    }
  }
#endif

  p_dev = p_host->DevTbl[dev_addr - 1u];
#else
  p_dev = p_host->DevHeadPtr;
  while ((p_dev != DEF_NULL)
         && (p_dev->Addr != dev_addr)) {
    p_dev = p_dev->HostDevNextPtr;
  }
#endif

  return (p_dev);
}

/****************************************************************************************************//**
 *                                           USBH_DevAcquireShared()
 *
 * @brief    Get pointer to given device, marking it as used, preventing it from being deleted.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the device structure, if successful.
 *           DEF_NULL,                        otherwise.
 *
 * @note     (1) This function is used to mark the device as used using a reference couting system.
 *               This system prevents another task from acquiring the device in order to modify/delete
 *               it as long as any other task has acquired it in shared.
 *******************************************************************************************************/
USBH_DEV *USBH_DevAcquireShared(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err)
{
  USBH_DEV_LOCKING_ARGS dev_locking_args;
  RTOS_ERR              err_kal;

  dev_locking_args.OperType = USBH_DEV_LOCKING_ACQUIRE_SHARED;
  dev_locking_args.Handle = dev_handle;
  dev_locking_args.DevPtr = USBH_DevGet(dev_handle);
  dev_locking_args.ErrPtr = p_err;

  if (dev_locking_args.DevPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (USBH_DEV_HANDLE_VALIDATE(dev_locking_args.DevPtr, dev_handle) != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  KAL_MonOp(dev_locking_args.DevPtr->MonHandle,
            &dev_locking_args,
            USBH_DevMonOnEnter,
            DEF_NULL,
            KAL_OPT_MON_NO_SCHED,
            KAL_TIMEOUT_INFINITE,
            &err_kal);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)              // Err handling, if err not already set. See Note #1.
      && (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, err_kal);
  }

  return (dev_locking_args.DevPtr);
}

/****************************************************************************************************//**
 *                                       USBH_DevAcquireCompleting()
 *
 * @brief    Get pointer to given device, marking it as used, preventing it from being deleted. This
 *           function must be called when competing a transfer from the asynchronous task only.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the device structure, if successful.
 *           DEF_NULL,                        otherwise.
 *
 * @note     (1) This function is used to mark the device as used using a reference couting system.
 *               This system prevents another task from acquiring the device in order to modify/delete
 *               it as long as any other task has acquired it in shared.
 *******************************************************************************************************/
USBH_DEV *USBH_DevAcquireCompleting(USBH_DEV_HANDLE dev_handle,
                                    RTOS_ERR        *p_err)
{
  USBH_DEV_LOCKING_ARGS dev_locking_args;
  RTOS_ERR              err_kal;

  dev_locking_args.OperType = USBH_DEV_LOCKING_ACQUIRE_SHARED_COMPLETING;
  dev_locking_args.Handle = dev_handle;
  dev_locking_args.DevPtr = USBH_DevGet(dev_handle);
  dev_locking_args.ErrPtr = p_err;

  if (dev_locking_args.DevPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (USBH_DEV_HANDLE_VALIDATE(dev_locking_args.DevPtr, dev_handle) != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  KAL_MonOp(dev_locking_args.DevPtr->MonHandle,
            &dev_locking_args,
            USBH_DevMonOnEnter,
            DEF_NULL,
            KAL_OPT_MON_NO_SCHED,
            KAL_TIMEOUT_INFINITE,
            &err_kal);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)              // Err handling, if err not already set. See Note #1.
      && (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, err_kal);
  }

  return (dev_locking_args.DevPtr);
}

/****************************************************************************************************//**
 *                                           USBH_DevRelShared()
 *
 * @brief    Frees reference to device previously acquired.
 *
 * @param    p_dev   Pointer to the device structure.
 *******************************************************************************************************/
void USBH_DevRelShared(USBH_DEV *p_dev)
{
  USBH_DEV_LOCKING_ARGS dev_locking_args;
  RTOS_ERR              err_kal;
  RTOS_ERR              err_usbh;

  dev_locking_args.OperType = USBH_DEV_LOCKING_RELEASE_SHARED;
  dev_locking_args.DevPtr = p_dev;
  dev_locking_args.ErrPtr = &err_usbh;

  if (p_dev == DEF_NULL) {
    LOG_ERR(("Releasing p_dev -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_NULL_PTR)));
    return;
  }

  KAL_MonOp(p_dev->MonHandle,
            &dev_locking_args,
            USBH_DevMonOnEnter,
            DEF_NULL,
            KAL_OPT_MON_NONE,
            KAL_TIMEOUT_INFINITE,
            &err_kal);
  if ((RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE)            // Err handling, if err not already set.
      || (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE)) {
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      LOG_ERR(("Releasing p_dev -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    } else {
      LOG_ERR(("Releasing p_dev -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
    }
  }
}

/****************************************************************************************************//**
 *                                       USBH_DevConnAcceptNotify()
 *
 * @brief    Verify with application that device can be accepted.
 *
 * @return   DEF_YES, if the application has accepted the configuration to be set,
 *
 *           DEF_NO,  otherwise.
 *
 * @note     (1) At that point, the application may use USBH_DEV_HANDLE_NOTIFICATION as a dev_handle
 *               to obtain more information on the device that is being connected.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_DevConnAcceptNotify(void)
{
  CPU_BOOLEAN      dev_accept = DEF_YES;
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevConnAccept != DEF_NULL)) {
    dev_accept = p_event_fncts->DevConnAccept();
  }

  return (dev_accept);
}

/****************************************************************************************************//**
 *                                           USBH_DevConnFailNotify()
 *
 * @brief    Notify the application that a device connection has failed.
 *
 * @param    hub_addr    Address of the HUB on which failure occurred.
 *
 * @param    port_nbr    Port number on which failure occurred.
 *
 * @param    err         Error code.
 *
 * @note     (1) At that point, the application may use USBH_DEV_HANDLE_NOTIFICATION as a dev_handle
 *               to obtain more information on the device that has failed.
 *******************************************************************************************************/
void USBH_DevConnFailNotify(CPU_INT08U hub_addr,
                            CPU_INT08U port_nbr,
                            RTOS_ERR   err)
{
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevConnFail != DEF_NULL)) {
    p_event_fncts->DevConnFail(hub_addr, port_nbr, err);
  }
}

/****************************************************************************************************//**
 *                                           USBH_DevResumeNotify()
 *
 * @brief    Notify the application that a device has resumed.
 *
 * @param    dev_handle  Handle to device.
 *
 * @note     (1) This function is a placeholder for when remote wake-up will be supported.
 *******************************************************************************************************/

#if 0
void USBH_DevResumeNotify(USBH_DEV_HANDLE dev_handle)
{
  USBH_EVENT_FNCTS *p_event_fncts = USBH_Ptr->EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevResume != DEF_NULL)) {
    p_event_fncts->DevResume(dev_handle);
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_DevDisconnNotify()
 *
 * @brief    Notify application that a device has been disconnected.
 *
 * @param    dev_handle  Handle to device
 *
 * @note     (1) At that point, the application may use USBH_DEV_HANDLE_NOTIFICATION as a dev_handle
 *               to obtain more information on the device that has disconnected.
 *******************************************************************************************************/
void USBH_DevDisconnNotify(USBH_DEV_HANDLE dev_handle)
{
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevDisconn != DEF_NULL)) {
    p_event_fncts->DevDisconn(dev_handle);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_DevAllocCallback()
 *
 * @brief    Function called when a new device is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @param    p_arg   Pointer to the argument passed at pool creation. Unused in this case.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_DevAllocCallback(MEM_DYN_POOL *p_pool,
                                         MEM_SEG      *p_seg,
                                         void         *p_blk,
                                         void         *p_arg)
{
  RTOS_ERR err;
  USBH_DEV *p_dev;

  (void)&p_pool;
  (void)&p_seg;
  (void)&p_arg;

  p_dev = (USBH_DEV *)p_blk;

  //                                                               Alloc buffer used for setup pkt.
  p_dev->SetupBufPtr = (CPU_INT16U *)Mem_SegAllocHW("USBH - Dev setup buf",
                                                    USBH_InitCfg.MemSegBufPtr,
                                                    USBH_SETUP_PKT_LEN,
                                                    USBH_InitCfg.BufAlignOctets,
                                                    DEF_NULL,
                                                    &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating dev setup buffer -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_DEV_ALLOC)));
    return (DEF_FAIL);
  }

  //                                                               Create lock for dflt EP.
  p_dev->DfltEP_LockHandle = KAL_LockCreate("USBH - Dev dflt EP lock",
                                            DEF_NULL,
                                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating dev deflt EP lock -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return (DEF_FAIL);
  }

  p_dev->MonHandle = KAL_MonCreate("USBH - Dev Mon",
                                   DEF_NULL,
                                   DEF_NULL,
                                   DEF_NULL,
                                   &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Creating monitor from USBH_DevAllocCallback() -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return (DEF_FAIL);
  }

  p_dev->LockRefCnt = 0u;
  p_dev->Handle = USBH_DEV_HANDLE_INVALID;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ep_cnt;

    for (ep_cnt = 0u; ep_cnt < USBH_EP_MAX_NBR; ep_cnt++) {     // Clr EP table.
      p_dev->EP_MgmtTblPtr[ep_cnt] = DEF_NULL;
    }

    p_dev->ConfigCur.FnctTblPtr = (USBH_FNCT **)Mem_SegAlloc("USBH - Config fnct table",
                                                             p_seg,
                                                             sizeof(USBH_FNCT *) * USBH_InitCfg.OptimizeSpd.FnctPerConfigQty,
                                                             &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Allocating dev config list of fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
      return (DEF_FAIL);
    }
  }
#endif

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_DevAddrAcquire()
 *
 * @brief    Acquires an available device address on given host.
 *
 * @param    p_host  Pointer to the host.
 *
 * @return   Device address,        if no error.
 *           USBH_DEV_ADDR_INVALID, otherwise.
 *
 * @note     (1) Each host maintains a bitmap that represents the available device addresses. When
 *               USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED, the bitmap is 32 bit wide, hence
 *               limiting the number of devices to 32.
 *******************************************************************************************************/
static CPU_INT08U USBH_DevAddrAcquire(USBH_HOST *p_host)
{
  CPU_INT08U dev_addr;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U cnt = 0u;
    CPU_INT08U max_cnt = ((USBH_InitCfg.OptimizeSpd.DevPerHostQty + DEF_INT_32_NBR_BITS - 1u) / DEF_INT_32_NBR_BITS);

    while ((cnt < max_cnt)
           && (p_host->DevAddrPool[cnt] == DEF_BIT_NONE)) {
      cnt++;
    }
    if (cnt >= max_cnt) {
      return (USBH_DEV_ADDR_INVALID);
    }

    //                                                             Retrieve first dev addr available on host.
    dev_addr = CPU_CntTrailZeros32(p_host->DevAddrPool[cnt]);

    DEF_BIT_CLR(p_host->DevAddrPool[cnt],
                DEF_BIT(dev_addr));                             // Mark address as unavailable.

    dev_addr += (cnt * DEF_INT_32_NBR_BITS) + 1u;
  }
#else
  dev_addr = CPU_CntTrailZeros32(p_host->DevAddrPool);          // Retrieve first dev addr available on host.
  if (dev_addr >= DEF_INT_32_NBR_BITS) {
    return (USBH_DEV_ADDR_INVALID);
  }

  DEF_BIT_CLR(p_host->DevAddrPool, DEF_BIT(dev_addr));          // Mark address as unavailable.

  dev_addr += 1u;                                               // Device addresses are 1 based.
#endif

  return (dev_addr);
}

/****************************************************************************************************//**
 *                                           USBH_DevAddrRelease()
 *
 * @brief    Releases a device address on given host and make it available for another device.
 *
 * @param    p_host      Pointer to the host.
 *
 * @param    dev_addr    Device address to release.
 *******************************************************************************************************/
static void USBH_DevAddrRelease(USBH_HOST  *p_host,
                                CPU_INT08U dev_addr)
{
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U bit = (dev_addr - 1u) % DEF_INT_32_NBR_BITS;
    CPU_INT08U ix = (dev_addr - 1u) / DEF_INT_32_NBR_BITS;

    DEF_BIT_SET(p_host->DevAddrPool[ix],
                DEF_BIT(bit));
  }
#else
  DEF_BIT_SET(p_host->DevAddrPool,
              DEF_BIT(dev_addr - 1u));
#endif
}

/****************************************************************************************************//**
 *                                           USBH_DevAcquireClosing()
 *
 * @brief    Get pointer to given device, marking it as 'closing'.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the device structure, if successful.
 *           DEF_NULL,                        otherwise.
 *
 * @note     (1) This function is used to mark the device as closing to allow aborting of the ctrl EP
 *               and making sure that no new transfers are started on this endpoint, since the device
 *               is about to be closed.
 *******************************************************************************************************/
static USBH_DEV *USBH_DevAcquireClosing(USBH_DEV_HANDLE dev_handle,
                                        RTOS_ERR        *p_err)
{
  USBH_DEV_LOCKING_ARGS dev_locking_args;
  RTOS_ERR              err_kal;

  dev_locking_args.OperType = USBH_DEV_LOCKING_ACQUIRE_SHARED_CLOSING;
  dev_locking_args.Handle = dev_handle;
  dev_locking_args.DevPtr = USBH_DevGet(dev_handle);
  dev_locking_args.ErrPtr = p_err;

  if (dev_locking_args.DevPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (USBH_DEV_HANDLE_VALIDATE(dev_locking_args.DevPtr, dev_handle) != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }
#endif

  KAL_MonOp(dev_locking_args.DevPtr->MonHandle,
            &dev_locking_args,
            USBH_DevMonOnEnter,
            DEF_NULL,
            KAL_OPT_MON_NO_SCHED,
            KAL_TIMEOUT_INFINITE,
            &err_kal);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)              // Err handling, if err not already set. See Note #1.
      && (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, err_kal);
  }

  return (dev_locking_args.DevPtr);
}

/****************************************************************************************************//**
 *                                               USBH_DevAcquire()
 *
 * @brief    Acquire exclusive lock on device and gets a pointer to given device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the device structure, if successful.
 *           DEF_NULL,                        otherwise.
 *
 * @note     (1) HUB task never needs to actually acquire a shared lock, since it already always has
 *               the exclusive lock for a given device. And since the HUB task is the only task that
 *               can acquire an exclusive lock, even for other devices, it is safe to avoid acquiring
 *               the lock in shared mode, since there is no other task to protect the HUB task from.
 *******************************************************************************************************/
static USBH_DEV *USBH_DevAcquire(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err)
{
  USBH_DEV_LOCKING_ARGS dev_locking_args;
  RTOS_ERR              err_kal;

  dev_locking_args.OperType = USBH_DEV_LOCKING_ACQUIRE_EXCLUSIVE;
  dev_locking_args.Handle = dev_handle;
  dev_locking_args.DevPtr = USBH_DevGet(dev_handle);
  dev_locking_args.ErrPtr = p_err;

  if (dev_locking_args.DevPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (USBH_DEV_HANDLE_VALIDATE(dev_locking_args.DevPtr, dev_handle) != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  KAL_MonOp(dev_locking_args.DevPtr->MonHandle,
            &dev_locking_args,
            USBH_DevMonOnEnter,
            USBH_DevMonOnEval,
            KAL_OPT_MON_NO_SCHED,
            KAL_TIMEOUT_INFINITE,
            &err_kal);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)              // Err handling, if err not already set. See Note #1.
      && (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE)) {
    RTOS_ERR_COPY(*p_err, err_kal);
  }

  return (dev_locking_args.DevPtr);
}

/****************************************************************************************************//**
 *                                           USBH_DevMonOnEnter()
 *
 * @brief    Enter function used by monitor for device locking.
 *
 * @param    p_mon_data  Pointer to the data passed at monitor creation. In this case, DEF_NULL.
 *
 * @param    p_op_data   Pointer to the data passed to KAL_MonOp(). In this case, the USBH_DEV_LOCKING_ARGS
 *                       on the caller's stack.
 *
 * @return   KAL monitor result, which is a combination of the following flags:
 *               KAL_MON_RES_ALLOW       To block or allow task being evaluated to run.
 *               KAL_MON_RES_STOP_EVAL   To stop evaluation other tasks.
 *******************************************************************************************************/
static KAL_MON_RES USBH_DevMonOnEnter(void *p_mon_data,
                                      void *p_op_data)
{
  USBH_DEV_LOCKING_ARGS *p_dev_locking_args = (USBH_DEV_LOCKING_ARGS *)p_op_data;
  USBH_DEV              *p_dev = p_dev_locking_args->DevPtr;
  KAL_MON_RES           ret_val = (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
  KAL_TASK_UUID         cur_uuid;

  (void)&p_mon_data;

  cur_uuid = KAL_TaskUUID_Get();

  if ((p_dev_locking_args->OperType == USBH_DEV_LOCKING_ACQUIRE_SHARED)
      || (p_dev_locking_args->OperType == USBH_DEV_LOCKING_ACQUIRE_SHARED_CLOSING)) {
    if ((((USBH_DEV_HANDLE_VALIDATE(p_dev, p_dev_locking_args->Handle) == DEF_OK)
          && (USBH_DEV_HANDLE_IS_CLOSING(p_dev->Handle) == DEF_NO))
         || (p_dev_locking_args->Handle == USBH_DEV_HANDLE_NOTIFICATION)
         || (cur_uuid == USBH_Ptr->HUB_TaskUUID))
        && (((p_dev->LockRefCnt + 1u) & USBH_DEV_LOCK_REF_CNT_MASK) != 0u)) {
      RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_NONE);
      p_dev->LockRefCnt++;

      if (p_dev_locking_args->OperType == USBH_DEV_LOCKING_ACQUIRE_SHARED_CLOSING) {
        USBH_DEV_HANDLE_SET_CLOSING(p_dev);
      }
    } else {
      RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_INVALID_STATE);
      p_dev_locking_args->DevPtr = DEF_NULL;
    }
  } else if (p_dev_locking_args->OperType == USBH_DEV_LOCKING_ACQUIRE_SHARED_COMPLETING) {
    if ((USBH_DEV_HANDLE_VALIDATE(p_dev, p_dev_locking_args->Handle) == DEF_OK)
        && (DEF_BIT_IS_CLR(p_dev->LockRefCnt, USBH_DEV_LOCK_REF_CNT_EXCL_BIT))
        && (((p_dev->LockRefCnt + 1u) & USBH_DEV_LOCK_REF_CNT_MASK) != 0u)) {
      RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_NONE);
      p_dev->LockRefCnt++;
    } else {
      RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_INVALID_STATE);
      p_dev_locking_args->DevPtr = DEF_NULL;
    }
  } else if (p_dev_locking_args->OperType == USBH_DEV_LOCKING_RELEASE_SHARED) {
    p_dev->LockRefCnt--;
    if (p_dev->LockRefCnt == 0u) {
      ret_val = KAL_MON_RES_ALLOW;                              // If RefCnt == 0u, do not stop eval, task could resume.
    }
    RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_NONE);
  } else {                                                      // OperType is USBH_DEV_LOCKING_ACQUIRE_EXCLUSIVE.
    if (USBH_DEV_HANDLE_VALIDATE(p_dev, p_dev_locking_args->Handle) == DEF_OK) {
      if (p_dev->LockRefCnt != 0u) {
        ret_val = (KAL_MON_RES_BLOCK | KAL_MON_RES_STOP_EVAL);
      } else {
        DEF_BIT_SET(p_dev->LockRefCnt, USBH_DEV_LOCK_REF_CNT_EXCL_BIT);
      }
    } else {
      ret_val = KAL_MON_RES_ALLOW;
      RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_INVALID_HANDLE);
    }
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           USBH_DevMonOnEval()
 *
 * @brief    Evaluation function used by monitor device locking.
 *
 * @param    p_mon_data      Pointer to the data passed at monitor creation. In this case, DEF_NULL.
 *
 * @param    p_eval_op_data  Pointer to the data passed to KAL_MonOp(). In this case, the
 *                           USBH_DEV_LOCKING_ARGS on one of the previous caller's stack.
 *
 * @param    p_scan_op_data  Pointer to the data passed to KAL_MonOp(). In this case, the
 *                           USBH_DEV_LOCKING_ARGS on the caller's stack.
 *
 * @return   KAL monitor result, which consists of a combination of the following flags:
 *               KAL_MON_RES_BLOCK or KAL_MON_RES_ALLOW  To block or allow task being evaluated to run.
 *               KAL_MON_RES_STOP_EVAL                   To stop evaluation other tasks.
 *
 * @note     (1) This function is only called when acquiring the device in exclusive mode (from HUB
 *               task.
 *******************************************************************************************************/
static KAL_MON_RES USBH_DevMonOnEval(void *p_mon_data,
                                     void *p_eval_op_data,
                                     void *p_scan_op_data)
{
  USBH_DEV_LOCKING_ARGS *p_dev_locking_args = (USBH_DEV_LOCKING_ARGS *)p_eval_op_data;
  USBH_DEV              *p_dev = p_dev_locking_args->DevPtr;
  KAL_MON_RES           ret_val = (KAL_MON_RES_BLOCK | KAL_MON_RES_STOP_EVAL);

  (void)&p_mon_data;
  (void)&p_scan_op_data;

  if (USBH_DEV_HANDLE_VALIDATE(p_dev, p_dev_locking_args->Handle) == DEF_OK) {
    if (p_dev->LockRefCnt == 0u) {
      DEF_BIT_SET(p_dev->LockRefCnt, USBH_DEV_LOCK_REF_CNT_EXCL_BIT);
      ret_val = (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
    }
  } else {
    ret_val = KAL_MON_RES_ALLOW;
    RTOS_ERR_SET(*p_dev_locking_args->ErrPtr, RTOS_ERR_INVALID_HANDLE);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           USBH_DevEnumNotifyStart()
 *
 * @brief    Save pointer to USBH_DEV currently being enumerated.
 *
 * @param    p_dev   Pointer to the device currently being enumerated.
 *******************************************************************************************************/
static void USBH_DevEnumNotifyStart(USBH_DEV *p_dev)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  USBH_Ptr->DescDevPtr = p_dev;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBH_DevEnumNotifyEnd()
 *
 * @brief    Indicates enumeration of device is completed.
 *******************************************************************************************************/
static void USBH_DevEnumNotifyEnd(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  USBH_Ptr->DescDevPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
