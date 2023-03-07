/***************************************************************************//**
 * @file
 * @brief USB Host Controllers-Core Interface
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

#ifndef  _USBH_HCD_CORE_PRIV_H_
#define  _USBH_HCD_CORE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const CPU_INT16U USBH_HC_BranchMaxPeriodicBW[];

extern const CPU_INT08U USBH_HC_BW_Overhead[][2u];

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_HC_SPD_LOW_IX                                0u
#define  USBH_HC_SPD_FULL_IX                               1u
#define  USBH_HC_SPD_HIGH_IX                               2u

#define  USBH_HC_XFER_TYPE_INTR_IX                         0u
#define  USBH_HC_XFER_TYPE_ISOC_IX                         1u
#define  USBH_HC_XFER_TYPE_BULK_IX                         2u
#define  USBH_HC_XFER_TYPE_CTRL_IX                         3u

#define  USBH_HC_SPD_IX_GET(spd)                     ((spd) == USBH_DEV_SPD_FULL) ? USBH_HC_SPD_FULL_IX \
  : (((spd) == USBH_DEV_SPD_HIGH) ? USBH_HC_SPD_HIGH_IX : USBH_HC_SPD_LOW_IX)

#define  USBH_HC_XFER_TYPE_IX_GET(xfer_type)         ((xfer_type) == USBH_EP_TYPE_INTR) ? USBH_HC_XFER_TYPE_INTR_IX \
  : (((xfer_type) == USBH_EP_TYPE_ISOC) ? USBH_HC_XFER_TYPE_ISOC_IX                                                 \
     : (((xfer_type) == USBH_EP_TYPE_BULK) ? USBH_HC_XFER_TYPE_BULK_IX : USBH_HC_XFER_TYPE_CTRL_IX))

#define  USBH_HC_MAX_PERIODIC_BW_GET(spd)           USBH_HC_BranchMaxPeriodicBW[USBH_HC_SPD_IX_GET(spd)]

#define  USBH_HC_BW_OVERHEAD_GET(spd, xfer_type)    USBH_HC_BW_Overhead[USBH_HC_SPD_IX_GET(spd)][USBH_HC_XFER_TYPE_IX_GET(xfer_type)]

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ROOT HUB INFO
 *
 * Note(s) : (1) This structure is used to retrieve information related to the root hub that would
 *               generally be retrieved via standard requests otherwise.
 *******************************************************************************************************/

typedef struct usbh_hub_root_info {
  CPU_INT08U NbrPort;
  CPU_INT16U Characteristics;
  CPU_INT08U PwrOn2PwrGood;
} USBH_HUB_ROOT_INFO;

/********************************************************************************************************
 *                               HOST CONTROLLER DRIVER ENDPOINT PARAMETERS
 *
 * Note(s) : (1) This structure is used by the core when an endpoint is opened in the host controller
 *               driver.
 *
 *           (2) MaxPktSize field has the same format as the wMaxPacketSize field of the endpoint
 *               descriptor. That is:
 *
 *               Bits     0                         10 11         12 13        15
 *                           +---------------------------+-------------+-----------+
 *               Content     | Max packet size           | Transaction | Reserved  |
 *                           |                           | per uFrame  |           |
 *                           +---------------------------+-------------+-----------+
 *******************************************************************************************************/

typedef struct usbh_hcd_ep_params {
  USBH_DEV_HANDLE DevHandle;                                    // Device Handle.
  CPU_INT08U      DevAddr;                                      // Device address.
  USBH_DEV_SPD    DevSpd;                                       // Device speed.

  CPU_INT08U      Addr;                                         // Endpoint address.
  CPU_INT08U      Type;                                         // Endpoint type.
  CPU_INT16U      MaxPktSize;                                   // Endpoint max packet size. See note (2).
  CPU_INT16U      Interval;                                     // Endpoint interval value in (micro)frames.
} USBH_HCD_EP_PARAMS;

/********************************************************************************************************
 *                           HOST CONTROLLER DRIVER USB REQUEST BLOCK PARAMETERS
 *
 * Note(s) : (1) This structure is used by the core when a URB is submitted to the host controller driver.
 *******************************************************************************************************/

typedef struct usbh_hcd_urb_params {
  CPU_INT08U *BufPtr;                                           // Pointer to buffer of data to send/recv.
  CPU_INT32U BufLen;                                            // Buffer length in octets.

  USBH_TOKEN Token;                                             // URB transfer token.

#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
  CPU_INT16U FrmStart;                                          // Transfer starting frame (For ISOC EP only).
  CPU_INT08U FrmCnt;                                            // Frame count (For ISOC EP only).
#endif
} USBH_HCD_URB_PARAMS;

/********************************************************************************************************
 *                                   USB HOST CONTROLLER DRIVER API
 *******************************************************************************************************/

struct usbh_hc_drv_api {
  //                                                               Init    HC.
  void (*Init)         (USBH_HC_DRV     *p_hc_drv,
                        MEM_SEG         *p_mem_seg,
                        MEM_SEG         *p_mem_seg_buf,
                        USBH_HC_CFG_EXT *p_hc_cfg_ext,
                        RTOS_ERR        *p_err);

  //                                                               Un-Init HC.
  void (*UnInit)       (USBH_HC_DRV *p_hc_drv,
                        RTOS_ERR    *p_err);

  //                                                               Start   HC.
  void (*Start)        (USBH_HC_DRV *p_hc_drv,
                        RTOS_ERR    *p_err);

  //                                                               Stop    HC.
  void (*Stop)         (USBH_HC_DRV *p_hc_drv,
                        RTOS_ERR    *p_err);

  //                                                               Suspend HC.
  void (*Suspend)      (USBH_HC_DRV *p_hc_drv,
                        RTOS_ERR    *p_err);

  //                                                               Resume  HC.
  void (*Resume)       (USBH_HC_DRV *p_hc_drv,
                        RTOS_ERR    *p_err);

  //                                                               Get HC frame number.
  CPU_INT16U (*FrmNbrGet)    (USBH_HC_DRV *p_hc_drv);

  //                                                               Open endpoint.
  void       (*EP_Open)      (USBH_HC_DRV        *p_hc_drv,
                              USBH_DEV_HANDLE    dev_handle,
                              USBH_EP_HANDLE     ep_handle,
                              USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                              void               **pp_hcd_ep_data,
                              RTOS_ERR           *p_err);

  //                                                               Close endpoint.
  void (*EP_Close)     (USBH_HC_DRV *p_hc_drv,
                        void        *p_hcd_ep_data,
                        RTOS_ERR    *p_err);

  //                                                               Suspends/resumes endpoint processing.
  void (*EP_Suspend)   (USBH_HC_DRV *p_hc_drv,
                        void        *p_hcd_ep_data,
                        CPU_BOOLEAN suspend,
                        RTOS_ERR    *p_err);

  //                                                               Clr endpoint halt condition + reset data toggle.
  void (*EP_HaltClr)   (USBH_HC_DRV *p_hc_drv,
                        void        *p_hcd_ep_data,
                        CPU_BOOLEAN data_toggle_clr,
                        RTOS_ERR    *p_err);

  //                                                               Submit URB on given endpoint.
  void (*URB_Submit)   (USBH_HC_DRV         *p_hc_drv,
                        void                *p_hcd_ep_data,
                        void                **pp_hcd_urb_data,
                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                        RTOS_ERR            *p_err);

  //                                                               Notify HCD that core is done with URB.
  CPU_INT32U (*URB_Complete) (USBH_HC_DRV         *p_hc_drv,
                              void                *p_hcd_ep_data,
                              void                *p_hcd_urb_data,
                              USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                              RTOS_ERR            *p_err);

  //                                                               Abort given URB.
  void (*URB_Abort)    (USBH_HC_DRV         *p_hc_drv,
                        void                *p_hcd_ep_data,
                        void                *p_hcd_urb_data,
                        USBH_HCD_URB_PARAMS *p_hcd_urb_params);
};

/********************************************************************************************************
 *                               USB HOST CONTROLLER ROOT HUB DRIVER API
 *******************************************************************************************************/

struct usbh_hc_rh_api {
  //                                                               Get root hub info.
  CPU_BOOLEAN (*InfosGet)      (USBH_HC_DRV        *p_hc_drv,
                                USBH_HUB_ROOT_INFO *p_rh_info);

  //                                                               Get port status.
  CPU_BOOLEAN (*PortStatusGet) (USBH_HC_DRV     *p_hc_drv,
                                CPU_INT08U      port_nbr,
                                USBH_HUB_STATUS *p_port_status);

  //                                                               Port request.
  CPU_BOOLEAN (*PortReq)       (USBH_HC_DRV *p_hc_drv,
                                CPU_INT08U  port_nbr,
                                CPU_INT08U  req,
                                CPU_INT16U  feature);

  //                                                               Enable/Disable RH interrupt.
  CPU_BOOLEAN (*IntEn)         (USBH_HC_DRV *p_hc_drv,
                                CPU_BOOLEAN en);
};

/********************************************************************************************************
 *                                           HOST CONTROLLER DRIVER
 *
 * Note(s) : (1) Null pointer means no dedicated memory.
 *******************************************************************************************************/

struct usbh_hc_drv {
  CPU_INT08U      HostNbr;                                      // Host number.
  CPU_INT08U      Nbr;                                          // HC number.

  void            *DataPtr;                                     // Drv's data.
  USBH_HC_HW_INFO *HW_InfoPtr;
  USBH_HC_DRV_API *API_Ptr;                                     // Ptr to HC drv API struct.
  USBH_HC_RH_API  *RH_API_Ptr;                                  // Ptr to RH drv API struct.
  USBH_HC_BSP_API *BSP_API_Ptr;                                 // Ptr to HC BSP API struct.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_URB_Done(USBH_DEV_HANDLE dev_handle,
                   USBH_EP_HANDLE  ep_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
