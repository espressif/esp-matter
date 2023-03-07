/***************************************************************************//**
 * @file
 * @brief USB Host Configuration Operations
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

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_config_priv.h>
#include  <usb/source/host/core/usbh_core_fnct_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>
#include  <usb/source/host/core/usbh_core_ep_priv.h>
#include  <usb/source/host/core/usbh_core_class_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_config.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CONFIG)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_CONFIG_FNCT_IF_MAX_QTY                        DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_ConfigFnctAdd(USBH_CONFIG *p_config,
                               USBH_FNCT   *p_fnct,
                               RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ConfigFnctQtyGet()
 *
 * @brief    Gets the quantity of function contained in device's current configuration.
 *
 * @param    dev_handle  Handle to the the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Quantity of function.
 *******************************************************************************************************/
CPU_INT08U USBH_ConfigFnctQtyGet(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err)
{
  CPU_INT08U config_cur = 0u;
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

  config_cur = p_dev->ConfigCur.NbrFnct;

end_rel:
  USBH_DevRelShared(p_dev);

  return (config_cur);
}

/****************************************************************************************************//**
 *                                           USBH_ConfigAttribGet()
 *
 * @brief    Gets the configuration's attributes.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Configuration's attributes.
 *
 * @note     (1) The value returned by this function corresponds to the 'bmAttributes' field
 *               of the configuration descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.3'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
CPU_INT08U USBH_ConfigAttribGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err)
{
  CPU_INT08U attr = 0u;
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

  attr = p_dev->ConfigCur.Attr;

end_rel:
  USBH_DevRelShared(p_dev);

  return (attr);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_ConfigMaxPwrGet()
 *
 * @brief    Gets the configuration's maximum power consumption.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Configuration's maximum power consumption.
 *
 * @note     (1) The value returned by this function corresponds to the 'bMaxPower' field of the
 *               configuration descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.3'.
 *******************************************************************************************************/

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
CPU_INT08U USBH_ConfigMaxPwrGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err)
{
  CPU_INT08U max_pwr = 0u;
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

  max_pwr = p_dev->ConfigCur.MaxPower;

end_rel:
  USBH_DevRelShared(p_dev);

  return (max_pwr);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_ConfigStrGet()
 *
 * @brief    Gets the configuration's string.
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
 * @return   String length in octets.
 *
 * @note     (1) The string returned by this function is retrieved via the 'GetDescriptor(String)'
 *               standard request using the 'iConfiguration' field of the configuration descriptor.
 *               For more information, see 'Universal Serial Bus specification, revision 2.0, section 9.6.3'.
 *
 * @note     (2) The string retrieved by this function is in unicode format.
 *
 * @note     (3) This function can be used to determine if a configuration string is present by passing a
 *               str_buf_len of 0. In this case, p_err will be set to RTOS_ERR_NOT_FOUND
 *               if no string is available.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_ConfigStrGet(USBH_DEV_HANDLE dev_handle,
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

  if (p_dev->ConfigCur.StrIx == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto end_rel;
  }

  str_desc_len = USBH_DevStrRd(p_dev,
                               p_dev->ConfigCur.StrIx,
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
 *                                               USBH_ConfigOpen()
 *
 * @brief    Opens given configuration.
 *
 * @param    p_dev       Pointer to the device.
 *
 * @param    cfg_nbr     Configuration number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Descriptor buffer shall be locked by caller.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_ConfigOpen(USBH_DEV   *p_dev,
                     CPU_INT08U cfg_nbr,
                     RTOS_ERR   *p_err)
{
  CPU_BOOLEAN probe_ok;
  CPU_BOOLEAN ep_open_ok;
  CPU_BOOLEAN config_accept;
  CPU_INT08U  desc_len;
  CPU_INT08U  desc_type;
  CPU_INT08U  fnct_if_cnt;
  CPU_INT08U  *p_desc_buf;
  CPU_INT16U  config_desc_len_tot;
  CPU_INT16U  cnt;
  USBH_CONFIG *p_config;
  USBH_FNCT   *p_fnct;
  USBH_IF     *p_if;
  USBH_EP     *p_ep;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT08U fnct_ix;
#endif

  p_desc_buf = USBH_Ptr->DescBufPtr;

  LOG_VRB(("Open config #", (u)cfg_nbr, " on device at address ", (u)p_dev->AddrCur));

  //                                                               -------------- READ CONFIG DESCRIPTOR --------------
  USBH_Ptr->DescLen = USBH_DevDescRdLocked(p_dev,
                                           USBH_DEV_REQ_RECIPIENT_DEV,
                                           USBH_DEV_REQ_TYPE_STD,
                                           USBH_DESC_TYPE_CONFIG,
                                           cfg_nbr - 1u,
                                           USBH_DESC_LEN_CONFIG,
                                           p_desc_buf,
                                           USBH_DESC_LEN_CONFIG,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  if (USBH_Ptr->DescLen != USBH_DESC_LEN_CONFIG) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    goto end_err;
  }

  config_desc_len_tot = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[2u]);
  if (config_desc_len_tot > USBH_InitCfg.MaxDescLen) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OVERFLOW_DESC);
    goto end_err;
  }

  if (config_desc_len_tot > USBH_Ptr->DescLen) {
    USBH_Ptr->DescLen = USBH_DevDescRdLocked(p_dev,
                                             USBH_DEV_REQ_RECIPIENT_DEV,
                                             USBH_DEV_REQ_TYPE_STD,
                                             USBH_DESC_TYPE_CONFIG,
                                             cfg_nbr - 1u,
                                             config_desc_len_tot,
                                             p_desc_buf,
                                             config_desc_len_tot,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_err;
    }

    if (USBH_Ptr->DescLen != config_desc_len_tot) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      goto end_err;
    }
  }

  //                                                               --------------- PARSE CONFIGURATION ----------------
  p_config = &p_dev->ConfigCur;                                 // Parse config desc.
  p_config->Nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);
    #if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
  p_config->Attr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[7u]);
    #endif
    #if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
  p_config->MaxPower = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[8u]);
    #endif
  p_config->NbrFnct = 0u;
    #if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_config->StrIx = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
    #endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  for (fnct_ix = 0u; fnct_ix < USBH_InitCfg.OptimizeSpd.FnctPerConfigQty; fnct_ix++) {
    p_config->FnctTblPtr[fnct_ix] = DEF_NULL;
  }
#else
  p_config->FnctHeadPtr = DEF_NULL;
#endif

  cnt = USBH_DESC_LEN_CONFIG;
  fnct_if_cnt = 0u;
  p_fnct = DEF_NULL;
  p_if = DEF_NULL;
  p_ep = DEF_NULL;

  if (p_dev->IsClassDevLevel == DEF_YES) {                      // If class defined at dev level, create a dev fnct.
    p_fnct = USBH_FnctDevCreate(p_dev, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_err;
    }

    USBH_ConfigFnctAdd(p_config, p_fnct, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      USBH_FnctDel(p_fnct);
      goto end_err;
    }

    fnct_if_cnt = USBH_CONFIG_FNCT_IF_MAX_QTY;
  }

  while (cnt < USBH_Ptr->DescLen) {
    desc_len = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt]);
    desc_type = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 1u]);

    if ((desc_len > (USBH_Ptr->DescLen - cnt))
        || (desc_len < 2u)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      goto end_err;
    }

    switch (desc_type) {
      case USBH_DESC_TYPE_IF:
#if (USBH_CFG_ALT_IF_EN == DEF_DISABLED)
      {                                                         // Discard any alternate interfaces.
        CPU_INT08U if_alt_nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 3u]);

        if (if_alt_nbr != 0u) {
          cnt += desc_len;
          continue;
        }
      }
#endif
        p_if = USBH_IF_Create(&p_desc_buf[cnt], p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end_err;
        }

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
        if ((p_if->AltNbr != 0u)                                // Alt IF are pushed inside same fnct.
            && (fnct_if_cnt < USBH_CONFIG_FNCT_IF_MAX_QTY)) {
          fnct_if_cnt++;
        }
#endif

        if (fnct_if_cnt == 0u) {                                // Create a fnct per IF.
          p_fnct = USBH_FnctCreate(&p_desc_buf[cnt], p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto end_err;
          }

          USBH_ConfigFnctAdd(p_config, p_fnct, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            USBH_FnctDel(p_fnct);
            goto end_err;
          }

          fnct_if_cnt = 1u;
        }

        USBH_FnctIF_Add(p_fnct, p_if, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          USBH_IF_Del(p_if);
          goto end_err;
        }

        fnct_if_cnt--;
        break;

      case USBH_DESC_TYPE_EP:
        p_ep = USBH_EP_Create(&p_desc_buf[cnt], p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end_err;
        }

        USBH_IF_EP_Add(p_if, p_ep, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          USBH_EP_Del(p_ep);
          goto end_err;
        }
        break;

      case USBH_DESC_TYPE_IF_ASSOCIATION:
        p_fnct = USBH_FnctCreate(&p_desc_buf[cnt], p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end_err;
        }

        fnct_if_cnt = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 3u]);

        USBH_ConfigFnctAdd(p_config, p_fnct, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          USBH_FnctDel(p_fnct);
          goto end_err;
        }
        break;

      default:
        LOG_VRB(("Open config -> Extra desc catched -> type ", (u)desc_type));
        break;
    }

    cnt += desc_len;
  }

  //                                                               Ensure app accepts dev configuration.
  config_accept = USBH_ConfigAcceptNotify();
  if (config_accept != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DEV_CONN_DECLINED);
    goto end_err;
  }

  //                                                               --------------- PROBE CLASS DRIVERS ----------------
  if (p_dev->IsClassDevLevel != DEF_YES) {
    probe_ok = USBH_ClassProbeFnct(p_dev->Handle,               // Probe all class drivers for all fncts.
                                   p_config);
  } else {
    probe_ok = USBH_ClassProbeDevFnct(p_dev);                   // Call ProbeFnct() (if avail) for dev class driver.
  }
  if (probe_ok == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_DRV_NOT_FOUND);
    goto end_err;
  }

  ep_open_ok = USBH_ClassEP_Open(p_dev);                        // Attempt to open EPs.
  if (ep_open_ok == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    goto end_err;
  }

  USBH_ConfigSet(p_dev, cfg_nbr, p_err);                        // Issue set config req.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  USBH_ClassConn(p_config);

  USBH_ClassIF_DfltAltSet(p_config);

  //                                                               Notify app of dev configuration.
  //                                                               Use of p_dev not safe after this call.
  USBH_ConfigSetNotify(p_dev->Handle, cfg_nbr, *p_err);

  return;

end_err:
  {
    RTOS_ERR local_err;

    USBH_ConfigClose(p_dev);

    USBH_ConfigSet(p_dev, 0u, &local_err);                      // Issue SetConfiguration(0) req if err.

    //                                                             Notify  must be called once we no longer use p_dev.
    USBH_ConfigSetNotify(p_dev->Handle,                         // Use of p_dev not safe after this call.
                         cfg_nbr,
                         *p_err);

    USBH_ConfigSetNotify(p_dev->Handle,
                         0u,
                         local_err);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ConfigClose()
 *
 * @brief    Close current configuration.
 *
 * @param    p_dev   Pointer to the device.
 *
 * @note     (1) This function MUST only be called by the hub task. Caller MUST acquire lock on dev.
 *******************************************************************************************************/
void USBH_ConfigClose(USBH_DEV *p_dev)
{
  USBH_CONFIG *p_config;

  p_config = &p_dev->ConfigCur;

  LOG_VRB(("Close config #", (u)p_config->Nbr, " on device at address ", (u)p_dev->AddrCur));

  USBH_ClassEP_Close(p_dev);                                    // Close EPs associated to class drivers.

  USBH_ClassDisconn(p_config);                                  // Disconn class drivers.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ix;

    for (ix = 0u; ix < p_config->NbrFnct; ix++) {
      USBH_FnctDel(p_config->FnctTblPtr[ix]);
    }
  }
#else
  {
    USBH_FNCT *p_fnct;
    USBH_FNCT *p_fnct_next;

    p_fnct = p_config->FnctHeadPtr;
    while (p_fnct != DEF_NULL) {
      p_fnct_next = p_fnct->NextPtr;

      USBH_FnctDel(p_fnct);
      p_fnct = p_fnct_next;
    }
  }
#endif

  p_config->Nbr = 0u;
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
  p_config->Attr = 0u;
#endif
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
  p_config->MaxPower = 0u;
#endif
  p_config->NbrFnct = 0u;
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_config->StrIx = 0u;
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_config->FnctHeadPtr = DEF_NULL;
#endif

  p_dev->State = USBH_DEV_STATE_ADDR;
}

/****************************************************************************************************//**
 *                                               USBH_ConfigSet()
 *
 * @brief    Selects configuration of specified device.
 *
 * @param    p_dev       Pointer to the USB device.
 *
 * @param    cfg_nbr     Configuration number to be selected.
 *
 * @param    -------  Argument validated by caller.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) The SET_CONFIGURATION request is described in "Universal Serial Bus Specification
 *                   Revision 2.0", section 9.4.6.
 *           - (a) If the device is in the default state, the behaviour of the device is not
 *                   specified. Implementations stall such request.
 *           - (b) If the device is in the addressed state &
 *               - (1) ... the configuration number is zero, then the device remains in the address state.
 *               - (2) ... the configuration number is non-zero & matches a valid configuration
 *                           number, then that configuration is selected & the device enters the
 *                           configured state.
 *               - (3) ... the configuration number is non-zero & does NOT match a valid configuration
 *                           number, then the device responds with a request error.
 *           - (c) If the device is in the configured state &
 *               - (1) ... the configuration number is zero, then the device enters the address state.
 *               - (2) ... the configuration number is non-zero & matches a valid configuration
 *                           number, then that configuration is selected & the device remains in the
 *                           configured state.
 *               - (3) ... the configuration number is non-zero & does NOT match a valid configuration
 *                           number, then the device responds with a request error.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_ConfigSet(USBH_DEV   *p_dev,
                    CPU_INT08U cfg_nbr,
                    RTOS_ERR   *p_err)
{
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("Set config #", (u)cfg_nbr, " on device at address ", (u)p_dev->AddrCur));
  (void)USBH_EP_CtrlXferLocked(p_dev,
                               USBH_DEV_REQ_SET_CFG,
                               (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_RECIPIENT_DEV),
                               cfg_nbr,
                               0u,
                               DEF_NULL,
                               0u,
                               0u,
                               std_req_timeout_ms,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_dev->State = USBH_DEV_STATE_CONFIG;
  }
}

/****************************************************************************************************//**
 *                                           USBH_ConfigAcceptNotify()
 *
 * @brief    Verify with application that configuration of device is accepted.
 *
 * @return   DEF_YES, if the application has accepted the configuration to be set,
 *
 *           DEF_NO,  otherwise.
 *
 * @note     (1) At that point, the application may use USBH_DEV_HANDLE_NOTIFICATION as a dev_handle
 *               to obtain more information on the device/configuration that is being set.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_ConfigAcceptNotify(void)
{
  CPU_BOOLEAN      config_accept = DEF_YES;
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevConfigAccept != DEF_NULL)) {
    config_accept = p_event_fncts->DevConfigAccept();
  }

  return (config_accept);
}

/****************************************************************************************************//**
 *                                           USBH_ConfigSetNotify()
 *
 * @brief    Notify application that configuration of device has been set.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    cfg_nbr     Configuration number that has been set.
 *
 * @param    err         Error code.
 *
 * @note     (1) At that point, the dev_handle is valid and the application should consider saving it
 *               for later use. Also, USBH_DEV_HANDLE_NOTIFICATION should NOT be used.
 *******************************************************************************************************/
void USBH_ConfigSetNotify(USBH_DEV_HANDLE dev_handle,
                          CPU_INT08U      cfg_nbr,
                          RTOS_ERR        err)
{
  USBH_EVENT_FNCTS *p_event_fncts = USBH_InitCfg.EventFnctsPtr;

  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->DevConfig != DEF_NULL)) {
    p_event_fncts->DevConfig(dev_handle, cfg_nbr, err);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ConfigFnctAdd()
 *
 * @brief    Adds function to configuration.
 *
 * @param    p_config    Pointer to the configuration.
 *
 * @param    p_fnct      Pointer to the function.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Order of the functions in the configuration list's (when USBH_CFG_OPTIMIZE_SPD_EN is
 *               set to DEF_DISABLED) must follow the function index.
 *******************************************************************************************************/
static void USBH_ConfigFnctAdd(USBH_CONFIG *p_config,
                               USBH_FNCT   *p_fnct,
                               RTOS_ERR    *p_err)
{
  LOG_VRB(("Added function class ", (u)p_fnct->ClassCode, " to config #", (u)p_config->Nbr));

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  if (p_config->NbrFnct >= USBH_InitCfg.OptimizeSpd.FnctPerConfigQty) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_config->FnctTblPtr[p_config->NbrFnct] = p_fnct;
#else
  if (p_config->NbrFnct == 0u) {
    p_config->FnctHeadPtr = p_fnct;
  } else {
    USBH_FNCT *p_fnct_last;

    p_fnct_last = p_config->FnctHeadPtr;
    while (p_fnct_last->NextPtr != DEF_NULL) {
      p_fnct_last = p_fnct_last->NextPtr;
    }

    p_fnct_last->NextPtr = p_fnct;
  }
#endif

  USBH_FNCT_HANDLE_UPDATE(p_fnct,
                          p_config->Nbr,
                          p_config->NbrFnct);

  p_config->NbrFnct++;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
