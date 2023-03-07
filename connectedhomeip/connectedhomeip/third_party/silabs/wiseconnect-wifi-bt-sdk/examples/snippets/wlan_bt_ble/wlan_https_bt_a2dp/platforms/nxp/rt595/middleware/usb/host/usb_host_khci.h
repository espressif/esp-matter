/*
 * Copyright (c) 2015 -2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_HOST_KHCI_H__
#define __USB_HOST_KHCI_H__

/*******************************************************************************
 * KHCI driver private structures, enumerations, macros, functions
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_KHCI_MAX_SPEED_DETECTION_COUNT 3

/* usb khci atom transaction error results */
#define USB_KHCI_ATOM_TR_PID_ERROR (-1)
#define USB_KHCI_ATOM_TR_EOF_ERROR (-2)
#define USB_KHCI_ATOM_TR_CRC_ERROR (-4)
#define USB_KHCI_ATOM_TR_TO (-16)
#define USB_KHCI_ATOM_TR_DMA_ERROR (-32)
#define USB_KHCI_ATOM_TR_BTS_ERROR (-128)
#define USB_KHCI_ATOM_TR_NAK (-256)
#define USB_KHCI_ATOM_TR_DATA_ERROR (-512)
#define USB_KHCI_ATOM_TR_STALL (-1024)
#define USB_KHCI_ATOM_TR_RESET (-2048)
#define USB_KHCI_ATOM_TR_BUS_TIMEOUT (-4096)
#define USB_KHCI_ATOM_TR_INVALID (-8192)

/* KHCI event bits */
#define USB_KHCI_EVENT_ATTACH 0x01
#define USB_KHCI_EVENT_RESET 0x02
#define USB_KHCI_EVENT_TOK_DONE 0x04
#define USB_KHCI_EVENT_SOF_TOK 0x08
#define USB_KHCI_EVENT_DETACH 0x10
#define USB_KHCI_EVENT_MSG 0x20
#define USB_KHCI_EVENT_ISO_MSG 0x40
#define USB_KHCI_EVENT_RESUME 0x80
#define USB_KHCI_EVENT_MASK 0xff

typedef enum _transfer_status
{
    kTransfer_Idle = 0,
    kTransfer_Setup0,
    kTransfer_Setup1,
    kTransfer_Setup2,
    kTransfer_Setup3,
} transfer_status_t;

typedef enum _khci_intr_type
{
    kIntr_UsbRst = 0x01,
    kIntr_Error = 0x02,
    kIntr_SofTok = 0x04,
    kIntr_TokDne = 0x08,
    kIntr_Sleep = 0x10,
    kIntr_Resume = 0x20,
    kIntr_Attach = 0x40,
    kIntr_Stall = 0x80,
} khci_intr_type_t;

typedef enum _tr_type
{
    kTr_Ctrl = 0,
    kTr_In,
    kTr_Out,
    kTr_OutHndsk,
    kTr_InHndsk,
    kTr_Unknown
} tr_type_t;

/* Transaction type */
typedef enum _tr_request_type
{
    kTr_MsgUnknown = 0, /*Unknown - not used */
    kTr_MsgSetup,       /* Set up transaction */
    kTr_MsgSend,        /* Send transaction */
    kTr_MsgRecv         /* Receive transaction */
} tr_request_type_t;

typedef enum tr_request_state
{
    kTr_MsgIdle = 0, /*Normal transfer */
    kTr_MsgNak,      /* NAK transfer */
    kTr_BusTimeout,
} tr_request_state_t;

typedef enum khci_tr_state
{
    kKhci_TrGetMsg = 0,
    kKhci_IsoTrGetMsg,
    kKhci_TrStartTransmit,
    kKhci_TrTransmiting,
    kKhci_TrTransmitDone,
    kKhci_TrNone
} khci_tr_state_t;

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
typedef enum bus_suspend_request_state
{
    kBus_Idle = 0,
    kBus_StartSuspend,
    kBus_Suspended,
    kBus_StartResume,
} bus_suspend_request_state_t;
#endif

/* Defines the USB KHCI time out value from USB specification */
#define USB_TIMEOUT_NODATA (500)
#define USB_TIMEOUT_TOHOST (5000)
#define USB_TIMEOUT_TODEVICE (5000)
#define USB_TIMEOUT_OTHER (10000)
#define USB_TIMEOUT_DEFAULT (500)

#define NAK_RETRY_TIME (1)
#define RETRY_TIME (3)

/* Define USB buffer descriptor operator MACRO definitions; This part is not included in header files  */
#define USB_KHCI_BDT_BASE ((uint32_t *)(&bdt[0]))
#define USB_KHCI_BD_PTR(ep, rxtx, odd) \
    ((((uint32_t)USB_KHCI_BDT_BASE) & 0xfffffe00U) | ((ep & 0x0fu) << 5) | ((rxtx & 1u) << 4) | ((odd & 1u) << 3))

#define USB_KHCI_BD_CTRL(ep, rxtx, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, rxtx, odd)))
#define USB_KHCI_BD_CTRL_RX(ep, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, 0, odd)))
#define USB_KHCI_BD_CTRL_TX(ep, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, 1, odd)))

#define USB_KHCI_BD_ADDR(ep, rxtx, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, rxtx, odd) + 1))
#define USB_KHCI_BD_ADDR_RX(ep, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, 0, odd) + 1))
#define USB_KHCI_BD_ADDR_TX(ep, odd) (*((uint32_t *)USB_KHCI_BD_PTR(ep, 1, odd) + 1))

/* Define USB buffer descriptor definitions; This part is not included in header files  */
#define USB_KHCI_BD_BC(n) ((n & 0x3ffu) << 16)
#define USB_KHCI_BD_OWN 0x80u
#define USB_KHCI_BD_DATA01(n) ((n & 1u) << 6)
#define USB_KHCI_BD_DATA0 USB_KHCI_BD_DATA01(0u)
#define USB_KHCI_BD_DATA1 USB_KHCI_BD_DATA01(1u)
#define USB_KHCI_BD_KEEP 0x20u
#define USB_KHCI_BD_NINC 0x10u
#define USB_KHCI_BD_DTS 0x08u
#define USB_KHCI_BD_STALL 0x04u
#define USB_KHCI_BD_PID(n) ((n & 0x0fu) << 2)

#define USB_HostKhciLock() USB_OsaMutexLock(usbHostPointer->khciMutex)
#define USB_HostKhciUnlock() USB_OsaMutexUnlock(usbHostPointer->khciMutex)

typedef struct _khci_xfer_sts
{
    uint32_t rxLen;
    uint8_t *rxBuf;
    uint8_t *rxBufOrig;
    uint8_t isDmaAlign;
} khci_xfer_sts_t;

/*******************************************************************************
 * KHCI driver public structures, enumerations, macros, functions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_controller_khci
 * @{
 */

/*! @brief  The value programmed into the threshold register must reserve enough time to ensure the worst case
   transaction completes. In general, the worst case transaction is an IN token followed by a data packet from the target
   followed by the response from the host. The actual time required is a function of the maximum packet size on the bus. Set the
   KHCICFG_THSLD_DELAY to 0x65 to meet the worst case.*/
   
#define KHCICFG_THSLD_DELAY 0x65

/*! @brief KHCI controller driver instance structure */
typedef struct _usb_khci_host_state_struct
{
    volatile USB_Type *usbRegBase;              /*!< The base address of the register */
    void *hostHandle;                           /*!< Related host handle*/
    usb_host_pipe_t *pipeDescriptorBasePointer; /*!< Pipe descriptor bas pointer*/
    usb_osa_event_handle khciEventPointer;      /*!< KHCI event*/
    usb_osa_mutex_handle khciMutex;             /*!< KHCI mutex*/
    usb_host_transfer_t
        *periodicListPointer; /*!< KHCI periodic list pointer, which link is an interrupt and an ISO transfer request*/
    usb_host_transfer_t *asyncListPointer; /*!< KHCI async list pointer, which link controls and bulk transfer request*/
    khci_xfer_sts_t sXferSts;              /*!< KHCI transfer status structure for the DAM ALIGN workaround */
    uint8_t *khciSwapBufPointer;           /*!< KHCI swap buffer pointer for the DAM ALIGN workaround*/
    volatile uint32_t trState;             /*!< KHCI transfer state*/
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    uint64_t matchTick;
#endif
    uint8_t asyncListAvtive;    /*!< KHCI async list is active*/
    uint8_t periodicListAvtive; /*!< KHCI periodic list is active*/
    uint8_t rxBd;               /*!< RX buffer descriptor toggle bits*/
    uint8_t txBd;               /*!< TX buffer descriptor toggle bits*/
    uint8_t deviceSpeed;        /*!< Device speed*/
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    bus_suspend_request_state_t busSuspendStatus; /*!< Bus Suspend Status*/
#endif
    int8_t deviceAttached; /*!< Device attach/detach state */
} usb_khci_host_state_struct_t, *ptr_usb_host_khci_state_struct_t;

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * @name USB host KHCI APIs
 * @{
 */

/*!
 * @brief Creates the USB host KHCI instance.
 *
 * This function initializes the USB host KHCI controller driver.
 *
 * @param controllerId      The controller ID of the USB IP. See the enumeration usb_controller_index_t.
 * @param hostHandle The host level handle.
 * @param controllerHandle  Returns the controller instance handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_AllocFail            Allocates memory failed.
 * @retval kStatus_USB_Error                Host mutex create failed, KHCI mutex or KHCI event create failed.
 *                                          Or, KHCI IP initialize failed.
 */
extern usb_status_t USB_HostKhciCreate(uint8_t controllerId,
                                       usb_host_handle hostHandle,
                                       usb_host_controller_handle *controllerHandle);

/*!
 * @brief Destroys the USB host KHCI instance.
 *
 * This function deinitializes the USB host KHCI controller driver.
 *
 * @param controllerHandle  The controller handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
extern usb_status_t USB_HostKhciDestory(usb_host_controller_handle controllerHandle);

/*!
 * @brief Opens the USB host pipe.
 *
 * This function opens a pipe according to the pipe_init_ptr parameter.
 *
 * @param controllerHandle    The controller handle.
 * @param pipeHandlePointer   The pipe handle pointer used to return the pipe handle.
 * @param pipeInitPointer     It is used to initialize the pipe.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_Error                There is no idle pipe.
 *
 */
extern usb_status_t USB_HostKhciOpenPipe(usb_host_controller_handle controllerHandle,
                                         usb_host_pipe_handle *pipeHandlePointer,
                                         usb_host_pipe_init_t *pipeInitPointer);

/*!
 * @brief Closes the USB host pipe.
 *
 * This function closes a pipe and frees the related resources.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The closing pipe handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
extern usb_status_t USB_HostKhciClosePipe(usb_host_controller_handle controllerHandle, usb_host_pipe_handle pipeHandle);

/*!
 * @brief Sends data to the pipe.
 *
 * This function requests to send the transfer to the specified pipe.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The sending pipe handle.
 * @param transfer          The transfer information.
 *
 * @retval kStatus_USB_Success                  Send successful.
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for KHCI.
 */
extern usb_status_t USB_HostKhciWritePipe(usb_host_controller_handle controllerHandle,
                                          usb_host_pipe_handle pipeHandle,
                                          usb_host_transfer_t *transfer);

/*!
 * @brief Receives data from the pipe.
 *
 * This function requests to receive the transfer from the specified pipe.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The receiving pipe handle.
 * @param transfer          The transfer information.
 *
 * @retval kStatus_USB_Success                  Receive successful.
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for KHCI.
 */
extern usb_status_t USB_HostKhciReadpipe(usb_host_controller_handle controllerHandle,
                                         usb_host_pipe_handle pipeHandle,
                                         usb_host_transfer_t *transfer);

/*!
 * @brief Controls the KHCI.
 *
 * This function controls the KHCI.
 *
 * @param controllerHandle The controller handle.
 * @param ioctlEvent       See the enumeration host_bus_control_t.
 * @param ioctlParam       The control parameter.
 *
 * @retval kStatus_USB_Success                Cancel successful.
 * @retval kStatus_USB_InvalidHandle        The controllerHandle is a NULL pointer.
 */
extern usb_status_t USB_HostKciIoctl(usb_host_controller_handle controllerHandle,
                                     uint32_t ioctlEvent,
                                     void *ioctlParam);

/*! @}*/
#ifdef __cplusplus
}
#endif
/*! @}*/
#endif /* __USB_HOST_KHCI_H__ */
