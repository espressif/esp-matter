/***************************************************************************//**
 * @file
 * @brief USB Host Core Declarations
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

#ifndef  _USBH_CORE_PRIV_H_
#define  _USBH_CORE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
#include  <usb/include/host/usbh_pbhci.h>
#endif

#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   HOST_MODULE
#define  HOST_EXT
#else
#define  HOST_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_OBJ_QTY_UNLIMITED                            0u

/********************************************************************************************************
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-5, and
 *               Section 9.4.3.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_DESC_TYPE_STR                                3u
#define  USBH_DESC_TYPE_OTHER_SPD_CONFIG                   7u
#define  USBH_DESC_TYPE_OTG                                9u

/********************************************************************************************************
 *                                       STANDARD DESCRIPTOR LENGTH
 *******************************************************************************************************/

#define  USBH_DESC_LEN_HDR                                 2u
#define  USBH_DESC_LEN_OTHER_SPD_CONFIG                    9u
#define  USBH_DESC_LEN_OTG                                 3u
#define  USBH_SETUP_PKT_LEN                                8u

/********************************************************************************************************
 *                                           DEFAULT LANGUAGE ID
 *******************************************************************************************************/

#define  USBH_DESC_STR_LANGID                           0x00u

/********************************************************************************************************
 *                                           FEATURE SELECTORS
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-6, and
 *               Section 9.4.1.
 *
 *           (2) For a 'clear feature' setup request, the 'wValue' field may contain one of these values.
 *******************************************************************************************************/

#define  USBH_FEATURE_SEL_EP_HALT                          0u
#define  USBH_FEATURE_SEL_DEV_REMOTE_WAKEUP                1u
#define  USBH_FEATURE_SEL_TEST_MODE                        2u
#define  USBH_FEATURE_SEL_B_HNP_EN                         3u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern USBH          *USBH_Ptr;                                 // Pointer to USBH struct.
extern USBH_INIT_CFG USBH_InitCfg;                              // Global configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       HANDLE CONVERSION MACROS
 *******************************************************************************************************/

//                                                                 -------------------- HC HANDLE ---------------------
#define  USBH_HC_HANDLE_CREATE(host_ix, hc_ix)                  (((host_ix) & 0x0F) | (((hc_ix) & 0x07) << 4u))
#define  USBH_HC_HANDLE_HOST_IX_GET(hc_handle)                  ((CPU_INT08U)(((hc_handle)  & 0x0Fu)))
#define  USBH_HC_HANDLE_HC_IX_GET(hc_handle)                    ((CPU_INT08U)(((hc_handle)  & 0x70u) >> 4u))

//                                                                 ------------ CORE GENERIC HANDLE MACROS ------------
#define  USBH_CORE_HANDLE_UPDATE(core_object, ix_0, ix_1)       DEF_BIT_FIELD_WR((core_object)->Handle,                       \
                                                                                 ((ix_0) & 0xFFu) | (((ix_1) & 0xFFu) << 8u), \
                                                                                 0x0000FFFFu)

#define  USBH_CORE_HANDLE_VALIDATE(core_object, core_handle)     (((core_handle) == (core_object)->Handle) ? DEF_OK : DEF_FAIL)

#define  USBH_CORE_HANDLE_INVALIDATE(core_object)                ((core_object)->Handle = ((core_object)->Handle + 0x00010000u) \
                                                                                          | USBH_CORE_HANDLE_INVALID)

//                                                                 --------------- DEVICE HANDLE MACROS ---------------
#define  USBH_DEV_HANDLE_IS_CLOSING_BIT                         DEF_BIT_15
#define  USBH_DEV_HANDLE_UPDATE(dev, host_ix, hc_ix, dev_addr)  USBH_CORE_HANDLE_UPDATE(dev,      \
                                                                                        dev_addr, \
                                                                                        (((host_ix) & 0x0F) | (((hc_ix) & 0x07) << 4u)))

#define  USBH_DEV_HANDLE_VALIDATE(dev, dev_handle)        ((((((dev_handle) & (~USBH_DEV_HANDLE_IS_CLOSING_BIT)) == (((dev)->Handle) & (~USBH_DEV_HANDLE_IS_CLOSING_BIT))) \
                                                             && (((dev_handle) &  USBH_DEV_HANDLE_INVALID_MSK) != USBH_DEV_HANDLE_INVALID))                                \
                                                            || ((dev_handle) == USBH_DEV_HANDLE_NOTIFICATION))                                                             \
                                                           ? DEF_OK : DEF_FAIL)
#define  USBH_DEV_HANDLE_INVALIDATE(p_dev)                    USBH_CORE_HANDLE_INVALIDATE(p_dev)
#define  USBH_DEV_HANDLE_SET_CLOSING(p_dev)                   DEF_BIT_SET((p_dev)->Handle, USBH_DEV_HANDLE_IS_CLOSING_BIT)
#define  USBH_DEV_HANDLE_CLR_CLOSING(p_dev)                   DEF_BIT_CLR((p_dev)->Handle, USBH_DEV_HANDLE_IS_CLOSING_BIT)

#define  USBH_HANDLE_DEV_ADDR_GET(dev_handle)               ((CPU_INT08U)( (dev_handle)  & 0x000000FFu))
#define  USBH_HANDLE_HOST_IX_GET(dev_handle)                ((CPU_INT08U)(((dev_handle)  & 0x00000F00u) >>  8u))
#define  USBH_HANDLE_HC_IX_GET(dev_handle)                  ((CPU_INT08U)(((dev_handle)  & 0x00007000u) >> 12u))
#define  USBH_DEV_HANDLE_IS_CLOSING(dev_handle)               DEF_BIT_IS_SET((dev_handle), USBH_DEV_HANDLE_IS_CLOSING_BIT)

#define  USBH_DEV_HANDLE_RH_CREATE(host_ix, hc_ix)          (USBH_DEV_ADDR_RH & 0xFFu) | (((((host_ix) & 0x0F) | (((hc_ix) & 0x07) << 4u))) << 8u)

//                                                                 -------------- FUNCTION HANDLE MACROS --------------
#define  USBH_FNCT_HANDLE_UPDATE(fnct, config_nbr, fnct_ix) USBH_CORE_HANDLE_UPDATE(fnct, fnct_ix, config_nbr)

#define  USBH_FNCT_HANDLE_VALIDATE(fnct, fnct_handle)     (((USBH_CORE_HANDLE_VALIDATE(fnct, fnct_handle) == DEF_OK) \
                                                            && (((fnct_handle) & USBH_FNCT_HANDLE_INVALID_MSK) != USBH_FNCT_HANDLE_INVALID)) ? DEF_OK : DEF_FAIL)
#define  USBH_FNCT_HANDLE_INVALIDATE(fnct)                   USBH_CORE_HANDLE_INVALIDATE(fnct)

#define  USBH_HANDLE_FNCT_IX_GET(fnct_handle)               ((CPU_INT08U)( (fnct_handle) & 0x00FFu))
#define  USBH_HANDLE_CONFIG_NBR_GET(fnct_handle)            ((CPU_INT08U)(((fnct_handle) & 0xFF00u) >>  8u))

//                                                                 -------------- ENDPOINT HANDLE MACROS --------------
#define  USBH_EP_HANDLE_UPDATE(ep, if_ix, ep_phy_nbr)       USBH_CORE_HANDLE_UPDATE(ep, ep_phy_nbr, if_ix)

#define  USBH_EP_HANDLE_VALIDATE(ep, ep_handle)           (((USBH_CORE_HANDLE_VALIDATE(ep, ep_handle) == DEF_OK) \
                                                            && (((ep_handle) & USBH_EP_HANDLE_INVALID_MSK) != USBH_EP_HANDLE_INVALID)) ? DEF_OK : DEF_FAIL)
#define  USBH_EP_HANDLE_INVALIDATE(ep)                       USBH_CORE_HANDLE_INVALIDATE(ep)

#define  USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)              ((CPU_INT08U)(ep_handle   &     0x00FFu))
#define  USBH_HANDLE_IF_IX_GET(ep_handle)                   ((CPU_INT08U)((ep_handle   &     0xFF00u) >>  8u))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT08U USBH_HC_AddInternal(const CPU_CHAR        *name,
                               CPU_INT08U            host_nbr,
                               USBH_HC_HW_INFO       *p_hw_info,
                               USBH_HC_DRV_API       *p_drv_api,
                               USBH_HC_RH_API        *p_rh_api,
                               USBH_HC_BSP_API       *p_bsp_api,
                               const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                               RTOS_ERR              *p_err);

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
CPU_INT08U USBH_PBHCI_HC_AddInternal(const CPU_CHAR        *name,
                                     CPU_INT08U            host_nbr,
                                     USBH_PBHCI_HC_HW_INFO *p_hc_hw_info,
                                     const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                                     RTOS_ERR              *p_err);
#endif

USBH_HC *USBH_HC_PtrGet(USBH_HOST  *p_host,
                        CPU_INT08U hc_nbr,
                        RTOS_ERR   *p_err);

void USBH_AsyncEventPost(USBH_ASYNC_EVENT_TYPE event_type,
                         USBH_DEV_HANDLE       dev_handle,
                         USBH_EP_HANDLE        ep_handle,
                         void                  *p_arg);

void USBH_AsyncEventFree(USBH_ASYNC_EVENT *p_async_event);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
