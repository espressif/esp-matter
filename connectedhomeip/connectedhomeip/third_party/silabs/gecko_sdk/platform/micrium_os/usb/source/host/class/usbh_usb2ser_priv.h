/***************************************************************************//**
 * @file
 * @brief USB Host - USB-To-Serial
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

#ifndef  _USBH_USB2SER_PRIV_H_
#define  _USBH_USB2SER_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/source/host/class/usbh_class_core_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   USBH_USB2SER_MODULE
#define  USBH_USB2SER_EXT
#else
#define  USBH_USB2SER_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ FUNCTION ADAPTER DRV DATA -------------
typedef struct usbh_usb2ser_fnct_adapter_drv_data {
  USBH_DEV_HANDLE          DevHandle;                           // Dev  handle of this usb2ser fnct.
  USBH_FNCT_HANDLE         FnctHandle;                          // Fnct handle of this usb2ser fnct.

  CPU_INT08U               *AdapterCtrlBufPtr;                  // Ptr to adapter's buffer for control requests.
  CPU_INT08U               *AdapterStatusBufPtr;                // Ptr to adapter's buffer to retrieve serial status.

  void                     *DataPtr;                            // Pointer to adapter driver function data.
  USBH_USB2SER_FNCT_HANDLE Handle;                              // Handle to usb-to-serial class function.
} USBH_USB2SER_FNCT_ADAPTER_DRV_DATA;

//                                                                 ------------------ ADAPTER DRIVER ------------------
struct usbh_usb2ser_adapter_drv {
  USBH_CLASS_DRV               *ClassDrvPtr;                    // Ptr to adapter drv's class drv struct.
  USBH_USB2SER_DEV_ADAPTER_DRV *DevAdapterDrvPtr;               // Ptr to adapter drv's dev adapter drv struct.
};

/********************************************************************************************************
 *                                   USB-TO-SERIAL DEVICE ADAPTER DRIVER
 *******************************************************************************************************/

struct usbh_usb2ser_dev_adapter_drv {
  //                                                               Device adapter driver initialization.
  void (*Init)          (MEM_SEG             *p_mem_seg,
                         USBH_USB2SER_APP_ID *id_tbl,
                         CPU_INT08U          id_tbl_len,
                         RTOS_ERR            *p_err);

  //                                                               Adapter connected.
  void       (*Conn)          (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data);

  //                                                               Port number retrieve.
  CPU_INT08U (*PortNbrGet)    (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                               RTOS_ERR                           *p_err);

  //                                                               Adapter reset.
  void (*Reset)         (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         USBH_USB2SER_RESET_SEL             sel,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter set baud rate.
  void (*BaudRateSet)   (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_INT32U                         baudrate,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter retrieve baud rate.
  CPU_INT32U (*BaudRateGet)   (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                               RTOS_ERR                           *p_err);

  //                                                               Adapter data set.
  void (*DataSet)       (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_INT08U                         data_size,
                         USBH_USB2SER_PARITY                parity,
                         USBH_USB2SER_STOP_BITS             stop_bits,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter data params get.
  void (*DataGet)       (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_INT08U                         *p_data_size,
                         USBH_USB2SER_PARITY                *p_parity,
                         USBH_USB2SER_STOP_BITS             *p_stop_bits,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter break signa; set.
  void (*BrkSignalSet)  (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_BOOLEAN                        set,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter modem control (dtr, rts) set.
  void (*ModemCtrlSet)  (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_BOOLEAN                        dtr_en,
                         CPU_BOOLEAN                        dtr_set,
                         CPU_BOOLEAN                        rts_en,
                         CPU_BOOLEAN                        rts_set,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter modem control (dtr, rts) get.
  void (*ModemCtrlGet)  (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_BOOLEAN                        *p_dtr_en,
                         CPU_BOOLEAN                        *p_dtr_set,
                         CPU_BOOLEAN                        *p_rts_en,
                         CPU_BOOLEAN                        *p_rts_set,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter flow control set (hardware).
  void (*HW_FlowCtrlSet)(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter flow control get (hardware).
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL (*HW_FlowCtrlGet)(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                       RTOS_ERR                           *p_err);

  //                                                               Adapter flow control set (software).
  void (*SW_FlowCtrlSet)(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_BOOLEAN                        en,
                         CPU_INT08U                         xon_char,
                         CPU_INT08U                         xoff_char,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter flow control get (software).
  CPU_BOOLEAN (*SW_FlowCtrlGet)(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                CPU_INT08U                         *p_xon_char,
                                CPU_INT08U                         *p_xoff_char,
                                RTOS_ERR                           *p_err);

  //                                                               Adapter asynchronous data rx.
  void (*RxAsync)       (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_INT08U                         *p_buf,
                         CPU_INT32U                         buf_len,
                         RTOS_ERR                           *p_err);

  //                                                               Adapter asynchronous data tx.
  void (*TxAsync)       (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                         CPU_INT08U                         *p_buf,
                         CPU_INT32U                         buf_len,
                         void                               *p_arg,
                         RTOS_ERR                           *p_err);

  CPU_INT16U BufLenCtrl;                                        // Length of buffer used for control req.
  CPU_INT32U BufLenStatus;                                      // Length of buffer used to retrieve serial status.
  CPU_INT32U BufLenRxFS;                                        // Length of buffer used for rx with FS devices.
  CPU_INT32U BufLenRxHS;                                        // Length of buffer used for rx with HS devices.

  CPU_INT16U FnctLen;                                           // Length of internal data of adapter driver.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern CPU_INT32U USBH_USB2SER_ReqTimeout;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_USB2SER_FnctAsyncRxCmpl(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                  CPU_INT08U                         *p_buf,
                                  CPU_INT08U                         *p_buf_data,
                                  CPU_INT32U                         buf_len,
                                  CPU_INT32U                         data_len,
                                  RTOS_ERR                           err);

void USBH_USB2SER_FnctAsyncTxCmpl(USBH_DEV_HANDLE dev_handle,
                                  USBH_EP_HANDLE  ep_handle,
                                  CPU_INT08U      *p_buf,
                                  CPU_INT32U      buf_len,
                                  CPU_INT32U      xfer_len,
                                  void            *p_arg,
                                  RTOS_ERR        err);

void USBH_USB2SER_FnctStatusUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                   USBH_USB2SER_SERIAL_STATUS         status);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
