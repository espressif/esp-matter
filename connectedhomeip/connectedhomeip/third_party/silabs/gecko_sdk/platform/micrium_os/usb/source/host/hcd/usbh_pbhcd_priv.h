/***************************************************************************//**
 * @file
 * @brief USB Host - Pipe-Based Host Controller Driver Interface
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

#ifndef  _USBH_PBHCD_PRIV_H_
#define  _USBH_PBHCD_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <cpu/include/cpu.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_pbhci.h>

#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_PBHCI_XFER_LEN_ALL                          DEF_INT_32U_MAX_VAL

#define  USBH_PBHCI_XFER_STATUS_NONE                      DEF_BIT_NONE
#define  USBH_PBHCI_XFER_STATUS_NACK                      DEF_BIT_00
#define  USBH_PBHCI_XFER_STATUS_CMPL                      DEF_BIT_01

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               EP PID
 *******************************************************************************************************/

//                                                                 PIDs for data pkt.
typedef enum usbh_pbhci_pid_data {
  USBH_PBHCI_PID_DATA0 = 0u,
  USBH_PBHCI_PID_DATA1,
  USBH_PBHCI_PID_DATA2,
  USBH_PBHCI_PID_DATAM
} USBH_PBHCI_PID_DATA;

/********************************************************************************************************
 *                               PBHCI HOST CONTROLLER DRIVER CONFIGURATION
 *******************************************************************************************************/

typedef struct usbh_pbhci_hcd_cfg {
  CPU_INT08U  MaxPeriodicInterval;                              // Maximum periodic interval in frm.
  CPU_INT08U  MaxPeriodicIntervalHS;                            // Maximum periodic interval in frm when ctrlr in HS.

  CPU_INT08U  RH_PortQty;                                       // Quantity of port(s) on root hub.
  CPU_BOOLEAN RemoteWakeupCapable;                              // Indicates if HC supports remote wakeup signaling.
} USBH_PBHCI_HCD_CFG;

/********************************************************************************************************
 *                                       HOST CONTROLLER PBHCI DRIVER
 *******************************************************************************************************/

struct usbh_pbhci_hc_drv {
  CPU_INT08U           HostNbr;                                 // Host nbr.
  CPU_INT08U           HC_Nbr;                                  // Host Controller nbr
  CPU_INT08U           PBHCI_HC_Nbr;                            // PBHCI Host Controller nbr.

  void                 *DataPtr;                                // Ptr to HCD data.
  USBH_PBHCI_HC_API    *API_Ptr;                                // Ptr to PBHCI API.
  USBH_HC_HW_INFO      *HW_InfoPtr;                             // Ptr to HC configuration.
  USBH_PBHCI_BSP_API   *BSP_API_Ptr;                            // Ptr to BSP API.

  USBH_PBHCI_PIPE_INFO *PipeInfoTblPtr;                         // Ptr to pipe information table.
};

/********************************************************************************************************
 *                               PBHCI INITIALIZATION EXTENDED CONFIGURATIONS
 *******************************************************************************************************/

typedef struct usbh_pbhci_init_cfg_ext {
  USBH_HC_CFG_DEDICATED_MEM *DedicatedMemCfgPtr;                // Ptr to dedicated memory configuration. See note (1).

  CPU_INT08U                PipeQty;                            // Quantity of pipe.
  CPU_INT08U                PipeMaxNbr;                         // Highest pipe nbr avail.
} USBH_PBHCI_INIT_CFG_EXT;

/********************************************************************************************************
 *                                   PBHCI HOST CONTROLLER DRIVER API
 *******************************************************************************************************/

struct usbh_pbhci_hc_api {
  //                                                               Init    HC.
  void (*Init)         (USBH_PBHCI_HC_DRV       *p_pbhci_hc_drv,
                        MEM_SEG                 *p_mem_seg,
                        MEM_SEG                 *p_mem_seg_buf,
                        USBH_PBHCI_INIT_CFG_EXT *p_init_cfg_ext,
                        RTOS_ERR                *p_err);

  //                                                               Un-init HC.
  void (*UnInit)       (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        RTOS_ERR          *p_err);

  //                                                               Start   HC.
  void (*Start)        (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        RTOS_ERR          *p_err);

  //                                                               Stop    HC.
  void (*Stop)         (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        RTOS_ERR          *p_err);

  //                                                               Suspend HC.
  void (*Suspend)      (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        RTOS_ERR          *p_err);

  //                                                               Resume  HC.
  void (*Resume)       (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        RTOS_ERR          *p_err);

  //                                                               Get HC frame number.
  CPU_INT16U  (*FrameNbrGet)  (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv);

  //                                                               Reset port.
  CPU_BOOLEAN (*PortResetSet) (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               CPU_INT08U        port_nbr);

  //                                                               Clr reset port.
  CPU_BOOLEAN (*PortResetClr) (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               CPU_INT08U        port_nbr,
                               USBH_DEV_SPD      *p_dev_spd);

  //                                                               Suspend port.
  CPU_BOOLEAN (*PortSuspend)  (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               CPU_INT08U        port_nbr,
                               CPU_BOOLEAN       set);

  //                                                               Device connection.
  void (*DevConn)      (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        USBH_DEV_HANDLE   dev_handle,
                        CPU_INT08U        dev_addr,
                        USBH_DEV_SPD      dev_spd,
                        RTOS_ERR          *p_err);

  //                                                               Device disconnection.
  void (*DevDisconn)   (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        USBH_DEV_HANDLE   dev_handle,
                        CPU_INT08U        dev_addr);

  //                                                               Retrieve current PID data toggle of pipe.
  USBH_PBHCI_PID_DATA (*PipePidCurGet)(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       CPU_INT08U        pipe_nbr);

  //                                                               Cfg given pipe.
  void (*PipeCfg)      (USBH_PBHCI_HC_DRV   *p_pbhci_hc_drv,
                        CPU_INT08U          pipe_nbr,
                        USBH_HCD_EP_PARAMS  *p_hcd_ep_params,
                        USBH_TOKEN          pid_token,
                        USBH_PBHCI_PID_DATA pid_data_cur,
                        RTOS_ERR            *p_err);

  //                                                               Write data to transmit on pipe.
  CPU_INT32U (*PipeWr)       (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              CPU_INT08U        pipe_nbr,
                              CPU_INT08U        *p_buf,
                              CPU_INT32U        buf_len,
                              RTOS_ERR          *p_err);

  //                                                               Start data transmit on pipe.
  void (*PipeTxStart)  (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        CPU_INT08U        pipe_nbr,
                        CPU_INT08U        *p_buf,
                        CPU_INT32U        buf_len,
                        RTOS_ERR          *p_err);

  //                                                               Start data reception on pipe.
  void (*PipeRxStart)  (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        CPU_INT08U        pipe_nbr,
                        CPU_INT08U        *p_buf,
                        CPU_INT32U        buf_len,
                        RTOS_ERR          *p_err);

  //                                                               Read received data from pipe.
  CPU_INT32U (*PipeRd)       (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              CPU_INT08U        pipe_nbr,
                              CPU_INT08U        *p_buf,
                              CPU_INT32U        buf_len,
                              CPU_INT08U        *p_status,
                              RTOS_ERR          *p_err);

  //                                                               Abort pipe.
  void (*PipeAbort)    (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                        CPU_INT08U        pipe_nbr,
                        RTOS_ERR          *p_err);

  //                                                               Root hub int disable.
  CPU_BOOLEAN (*SOF_IntEn)    (USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               CPU_BOOLEAN       en);

  //                                                               Ptr to HCD configuration struct.
  USBH_PBHCI_HCD_CFG *CfgPtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_PBHCI_EventPortConn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              CPU_INT08U        port_nbr);

void USBH_PBHCI_EventPortDisconn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                 CPU_INT08U        port_nbr);

void USBH_PBHCI_EventPortOvercurrent(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                     CPU_INT08U        port_nbr,
                                     CPU_BOOLEAN       set);

void USBH_PBHCI_EventPortRemoteWakeup(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                      CPU_INT08U        port_nbr);

void USBH_PBHCI_EventPipeRxCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                CPU_INT08U        pipe_nbr,
                                CPU_INT08U        status,
                                RTOS_ERR          err);

void USBH_PBHCI_EventPipeTxCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                CPU_INT08U        pipe_nbr,
                                CPU_INT32U        xfer_len,
                                CPU_INT08U        status,
                                RTOS_ERR          err);

void USBH_PBHCI_EventSOF(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                         CPU_INT16U        frm_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
