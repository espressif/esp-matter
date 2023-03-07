/***************************************************************************//**
 * @file
 * @brief USB Host Class Operations
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

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/core/usbh_core_class_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_config_priv.h>
#include  <usb/source/host/core/usbh_core_fnct_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ClassDrvReg()
 *
 * @brief    Registers a class driver to the USB host stack core.
 *
 * @param    p_class_drv     Pointer to the class driver.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *
 * @note     (1) This function should not be called once USBH has been started.
 *******************************************************************************************************/
void USBH_ClassDrvReg(USBH_CLASS_DRV *p_class_drv,
                      RTOS_ERR       *p_err)
{
  USBH_CLASS_DRV_ITEM *p_class_drv_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_class_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_class_drv->ProbeDev != DEF_NULL)
                           || (p_class_drv->ProbeFnct != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Alloc class driver item.
  p_class_drv_item = (USBH_CLASS_DRV_ITEM *)Mem_SegAlloc("USBH - Class driver item",
                                                         USBH_InitCfg.MemSegPtr,
                                                         sizeof(USBH_CLASS_DRV_ITEM),
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_class_drv_item->ClassDrvPtr = p_class_drv;

  p_class_drv_item->NextPtr = USBH_Ptr->ClassDrvItemHeadPtr;
  USBH_Ptr->ClassDrvItemHeadPtr = p_class_drv_item;

  LOG_VRB(("Class driver added -> ", (s)p_class_drv->NamePtr));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ClassProbeDev()
 *
 * @brief    Probe all class drivers and attempts to find one that is willing to handle the device.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK      If a class driver is normally able to handle device.
 *           DEF_FAIL    Otherwise
 *
 * @note     (1) This function will probe each class driver using their ProbeDev() function. If a
 *               class driver is normally able to handle it, it MUST return DEF_OK.
 *               - (a) If a class driver is found and would normally be able to handle the device, but
 *                     an error occur (allocation of function, for instance), the class driver must
 *                     return DEF_OK and use p_err to signal the error.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_ClassProbeDev(USBH_DEV *p_dev,
                               RTOS_ERR *p_err)
{
  CPU_BOOLEAN         probe_result = DEF_FAIL;
  USBH_CLASS_DRV_ITEM *p_class_drv_item;

  LOG_VRB(("Probing dev class driver for dev ", (u)p_dev->AddrCur));

  p_class_drv_item = USBH_Ptr->ClassDrvItemHeadPtr;

  while ((p_class_drv_item != DEF_NULL)                         // Browse all class drivers.
         && (probe_result == DEF_FAIL)) {
    void           *p_class_fnct;
    USBH_CLASS_DRV *p_class_drv = p_class_drv_item->ClassDrvPtr;

    if (p_class_drv->ProbeDev != DEF_NULL) {
      probe_result = p_class_drv->ProbeDev(p_dev->Handle,       // Probe class driver.
                                           p_dev->ClassCode,
                                           &p_class_fnct,
                                           p_err);
      if ((probe_result == DEF_OK)
          && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
        p_dev->ClassDrvPtr = p_class_drv;                       // ClassCode driver takes device. Assign this class driver.
        p_dev->ClassFnctPtr = p_class_fnct;
        p_dev->IsClassDevLevel = DEF_YES;

        LOG_VRB(("Found dev class driver ", (s)p_class_drv->NamePtr));
      }
    }

    p_class_drv_item = p_class_drv_item->NextPtr;
  }

  return (probe_result);
}

/****************************************************************************************************//**
 *                                           USBH_ClassProbeDevFnct()
 *
 * @brief    Probe device class driver for device function and ensure class driver can handle it.
 *
 * @param    p_dev   Pointer to device.
 *
 * @return   DEF_OK,      if class driver probe was successful or if not ProbeFnct() available.
 *           DEF_FAIL,    otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_ClassProbeDevFnct(USBH_DEV *p_dev)
{
  CPU_BOOLEAN probe_ok = DEF_OK;
  USBH_CONFIG *p_config = &p_dev->ConfigCur;
  USBH_FNCT   *p_fnct;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  LOG_VRB(("Probing fnct class driver for dev ", (u)p_dev->AddrCur));

  //                                                               For each config's functions...
  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_FNCT_HANDLE fnct_handle = p_fnct->Handle;
    void             *p_class_fnct_ptr = DEF_NULL;
    USBH_CLASS_DRV   *p_class_drv = p_dev->ClassDrvPtr;

    if (p_class_drv->ProbeFnct != DEF_NULL) {
      LOG_VRB(("Probing class drv for dev fnct class ", (u)p_fnct->ClassCode));

      probe_ok = p_class_drv->ProbeFnct(p_dev->Handle,
                                        fnct_handle,
                                        p_fnct->ClassCode,
                                        &p_class_fnct_ptr,
                                        &err);
      if ((probe_ok == DEF_OK)
          && (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)) {
        p_fnct->ClassDrvPtr = p_class_drv;                      // ClassCode driver takes the fnct. Assign it to fnct.
        p_fnct->ClassFnctPtr = p_class_fnct_ptr;
      }
    } else {
      p_fnct->ClassDrvPtr = p_class_drv;                        // No ProbeFnct() available.
    }

    if ((probe_ok == DEF_FAIL)                                  // ClassCode driver ProbeFnct() failed ...
        || (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)) {
      if (probe_ok == DEF_OK) {
        LOG_ERR(("Class drv probe -> ", RTOS_ERR_LOG_ARG_GET(err)));
      } else {
        LOG_ERR(("Class drv probe -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_CLASS_DRV_NOT_FOUND)));
      }

      //                                                           Notify app that USB fnct is declined.
      USBH_FnctConnFailNotify(p_fnct->Handle,
                              err);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }

  return (probe_ok);
}

/****************************************************************************************************//**
 *                                           USBH_ClassProbeFnct()
 *
 * @brief    Probe all class drivers for all device's function and attempt to find one that is willing
 *           to manage the function.
 *
 * @param    dev_handle  Device handle.
 *
 * @param    p_config    Pointer to configuration
 *
 * @return   DEF_OK,      if a class driver was found for at least one function.
 *           DEF_FAIL,    otherwise.
 *
 * @note     (1) This function will probe each class driver using their Probe() function for each
 *               USB function present on a device. If a class driver is normally able to handle the
 *               function, it MUST return DEF_OK.
 *               - (a) If a class driver is found and would normally be able to handle the device, but
 *                     an error occur (allocation of function, for instance), the class driver must
 *                     return DEF_OK and use p_err to signal the error.
 *
 * @note     (2) In case a class driver is not found for a function or one is found but an error
 *               occurs, this function will call the FnctConnFail() generic application callback
 *               function.
 *
 * @note     (3) Device must be acquired by caller.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_ClassProbeFnct(USBH_DEV_HANDLE dev_handle,
                                USBH_CONFIG     *p_config)
{
  CPU_BOOLEAN probe_result = DEF_FAIL;
  CPU_BOOLEAN probe_ok;
  USBH_FNCT   *p_fnct;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  LOG_VRB(("Probing fnct class driver for dev ", (u)dev_handle));

  //                                                               For each config's functions...
  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_FNCT_HANDLE    fnct_handle = p_fnct->Handle;
    USBH_CLASS_DRV_ITEM *p_class_drv_item = USBH_Ptr->ClassDrvItemHeadPtr;

    probe_ok = DEF_FAIL;

    LOG_VRB(("Probing class drv for fnct class ", (u)p_fnct->ClassCode));

    while ((p_class_drv_item != DEF_NULL)                       // Probe all class drivers.
           && (probe_ok == DEF_FAIL)
           && (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)) {
      void           *p_class_fnct_ptr = DEF_NULL;
      USBH_CLASS_DRV *p_class_drv = p_class_drv_item->ClassDrvPtr;

      if (p_class_drv->ProbeFnct != DEF_NULL) {
        probe_ok = p_class_drv->ProbeFnct(dev_handle,
                                          fnct_handle,
                                          p_fnct->ClassCode,
                                          &p_class_fnct_ptr,
                                          &err);
        if ((probe_ok == DEF_OK)
            && (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)) {
          p_fnct->ClassDrvPtr = p_class_drv;                    // ClassCode driver takes the fnct. Assign it to fnct.
          p_fnct->ClassFnctPtr = p_class_fnct_ptr;

          LOG_VRB(("Found fnct class driver ", (s)p_class_drv->NamePtr));

          probe_result = DEF_OK;                                // At least one of dev's fnct can be handled.
        }
      }

      p_class_drv_item = p_class_drv_item->NextPtr;
    }

    if ((probe_ok == DEF_FAIL)                                  // No class driver found for function ...
        || (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)) {
      if (probe_ok == DEF_OK) {
        LOG_ERR(("Class drv probe -> ", RTOS_ERR_LOG_ARG_GET(err)));
      } else {
        RTOS_ERR_SET(err, RTOS_ERR_CLASS_DRV_NOT_FOUND);
        LOG_ERR(("Class drv probe -> ", RTOS_ERR_LOG_ARG_GET(err)));
      }

      //                                                           Notify app that USB fnct is declined.
      USBH_FnctConnFailNotify(p_fnct->Handle,
                              err);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }

  return (probe_result);
}

/****************************************************************************************************//**
 *                                           USBH_ClassEP_Open()
 *
 * @brief    Opens device endpoints for all functions for which a class driver was found.
 *
 * @param    p_dev   Pointer to device.
 *
 * @return   DEF_OK      If all endpoints for at least one function were successfully opened.
 *           DEF_FAIL    Otherwise.
 *
 * @note     (1) If this function is unable to open all the endpoints for a given function, it will
 *               call the FnctConnFail() application generic callback.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_ClassEP_Open(USBH_DEV *p_dev)
{
  CPU_BOOLEAN ep_open_result;
  USBH_CONFIG *p_config;
  USBH_FNCT   *p_fnct;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  p_config = &p_dev->ConfigCur;
  ep_open_result = DEF_FAIL;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    void           *p_class_fnct_ptr = p_fnct->ClassFnctPtr;
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if (p_class_drv != DEF_NULL) {                              // Skip fncts for which we don't have a class driver.
      USBH_FnctEP_Open(p_dev, p_fnct, &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        ep_open_result = DEF_OK;                                // At least one of dev's fnct can be handled.
      } else {                                                  // Cannot open at least one EP of fnct. Disconn fnct.
        p_fnct->ClassDrvPtr = DEF_NULL;
        p_fnct->ClassFnctPtr = DEF_NULL;

        p_class_drv->Disconn(p_class_fnct_ptr);

        //                                                         Notify app that USB fnct is declined.
        USBH_FnctConnFailNotify(p_fnct->Handle,
                                err);
      }
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }

  return (ep_open_result);
}

/****************************************************************************************************//**
 *                                           USBH_ClassEP_Close()
 *
 * @brief    Closes device endpoints that are attached to a function for which we have a class driver.
 *
 * @param    p_dev   Pointer to device.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_ClassEP_Close(USBH_DEV *p_dev)
{
  USBH_CONFIG *p_config = &p_dev->ConfigCur;
  USBH_FNCT   *p_fnct;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {              // For each config's function ...
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if (p_class_drv != DEF_NULL) {                              // Skip fncts for which we don't have a class driver.
      USBH_FnctEP_Close(p_dev,
                        p_fnct,
                        &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Closing fnct EPs -> ", RTOS_ERR_LOG_ARG_GET(err)));
      }
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                               USBH_ClassConn()
 *
 * @brief    Informs all class drivers associated to configuration that it is ready for communication.
 *
 * @param    p_config    Pointer to USB configuration.
 *
 * @note     (1) Once class drivers get their Conn() callback, they can start communication.
 *
 * @note     (2) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_ClassConn(USBH_CONFIG *p_config)
{
  USBH_FNCT *p_fnct;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if ((p_class_drv != DEF_NULL)                               // Call conn callback for fncts we have class driver.
        && (p_class_drv->Conn != DEF_NULL)) {
      p_class_drv->Conn(p_fnct->ClassFnctPtr);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ClassDisconn()
 *
 * @brief    Disconnects all class drivers associated to configuration.
 *
 * @param    p_config    Pointer to USB configuration.
 *
 * @note     (1) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_ClassDisconn(USBH_CONFIG *p_config)
{
  USBH_FNCT *p_fnct;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if ((p_class_drv != DEF_NULL)
        && (p_class_drv->Disconn != DEF_NULL)) {
      p_class_drv->Disconn(p_fnct->ClassFnctPtr);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ClassIF_DfltAltSet()
 *
 * @brief    Informs all class drivers associated to configuration that default interfaces are set.
 *
 * @param    p_config    Pointer to USB configuration.
 *
 * @note     (1) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_ClassIF_DfltAltSet(USBH_CONFIG *p_config)
{
  USBH_FNCT *p_fnct;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if ((p_class_drv != DEF_NULL)                               // Call conn callback for fncts we have class driver.
        && (p_class_drv->IF_AltSet != DEF_NULL)) {
      CPU_INT08U if_ix = 0u;
      USBH_IF    *p_if;

      USBH_OBJ_ITERATOR_INIT(p_if, p_fnct, IF) {
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
        if (p_if->AltNbr == 0u) {
#endif
        p_class_drv->IF_AltSet(p_fnct->ClassFnctPtr, if_ix);
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
      }
#endif

        if_ix++;

        USBH_OBJ_ITERATOR_NEXT_GET(p_if);
      }

#if (USBH_CFG_ALT_IF_EN == DEF_DISABLED)
      PP_UNUSED_PARAM(p_if);
#endif
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ClassSuspend()
 *
 * @brief    Suspends all class drivers associated to configuration.
 *
 * @param    p_config    Pointer to USB configuration.
 *******************************************************************************************************/
void USBH_ClassSuspend(USBH_CONFIG *p_config)
{
  USBH_FNCT *p_fnct;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if ((p_class_drv != DEF_NULL)
        && (p_class_drv->Suspend != DEF_NULL)) {
      p_class_drv->Suspend(p_fnct->ClassFnctPtr);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ClassResume()
 *
 * @brief    Resumes all class drivers associated to configuration.
 *
 * @param    p_config    Pointer to USB configuration.
 *******************************************************************************************************/
void USBH_ClassResume(USBH_CONFIG *p_config)
{
  USBH_FNCT *p_fnct;

  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    USBH_CLASS_DRV *p_class_drv = p_fnct->ClassDrvPtr;

    if ((p_class_drv != DEF_NULL)
        && (p_class_drv->Resume != DEF_NULL)) {
      p_class_drv->Resume(p_fnct->ClassFnctPtr);
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ClassesUnInit()
 *
 * @brief    Un-initalizes all classes.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_ClassesUnInit(void)
{
  USBH_CLASS_DRV_ITEM *p_class_drv_item = USBH_Ptr->ClassDrvItemHeadPtr;
  USBH_CLASS_DRV *p_class_drv;

  while (p_class_drv_item != DEF_NULL) {
    p_class_drv = p_class_drv_item->ClassDrvPtr;
    if ((p_class_drv != DEF_NULL)
        && (p_class_drv->UnInit != DEF_NULL)) {
      p_class_drv->UnInit();
    }
    p_class_drv_item = p_class_drv_item->NextPtr;
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
