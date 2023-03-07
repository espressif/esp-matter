/***************************************************************************//**
 * @file
 * @brief USB Host Types Declarations
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

#ifndef  _USBH_CORE_TYPES_PRIV_H_
#define  _USBH_CORE_TYPES_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_hub.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       OBJECT ITERATOR MACROS
 *******************************************************************************************************/

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
#define  USBH_OBJ_ITERATOR_INIT_EXT(obj_child, obj_parent, obj_count, cont) \
  (obj_child) = (obj_parent)->cont##HeadPtr;                                \
  while ((obj_child) != DEF_NULL)

#define  USBH_OBJ_ITERATOR_INIT(obj_child, obj_parent, cont)    USBH_OBJ_ITERATOR_INIT_EXT(obj_child, obj_parent, 0, cont)

#define  USBH_OBJ_ITERATOR_NEXT_GET(obj_child)                  (obj_child) = (obj_child)->NextPtr

#else
#define  USBH_OBJ_ITERATOR_INIT_EXT(obj_child, obj_parent, obj_count, cont)   \
  {                                                                           \
    CPU_INT08U __obj_ix##cont;                                                \
                                                                              \
                                                                              \
    for (__obj_ix##cont = 0u; __obj_ix##cont < obj_count; __obj_ix##cont++) { \
      (obj_child) = (obj_parent)->cont##TblPtr[__obj_ix##cont];

#define  USBH_OBJ_ITERATOR_INIT(obj_child, obj_parent, cont)    USBH_OBJ_ITERATOR_INIT_EXT(obj_child, obj_parent, (obj_parent)->Nbr##cont, cont)

#define  USBH_OBJ_ITERATOR_NEXT_GET(obj_child)                  } }
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbh_host USBH_HOST;
typedef struct usbh_hc USBH_HC;
typedef struct usbh_hub_fnct USBH_HUB_FNCT;
typedef struct usbh_hub_event USBH_HUB_EVENT;
typedef struct usbh_async_event USBH_ASYNC_EVENT;
typedef struct usbh_dev USBH_DEV;
typedef struct usbh_config USBH_CONFIG;
typedef struct usbh_fnct USBH_FNCT;
typedef struct usbh_if USBH_IF;
typedef struct usbh_ep USBH_EP;
typedef struct usbh_ep_mgmt USBH_EP_MGMT;
typedef struct usbh_urb_mgmt USBH_URB_MGMT;

/********************************************************************************************************
 *                                       USB HOST CLASS DRIVER ITEM
 *******************************************************************************************************/

typedef struct usbh_class_drv_item {
  USBH_CLASS_DRV *ClassDrvPtr;                                  // Pointer to      class driver.
  struct usbh_class_drv_item *NextPtr;                          // Pointer to next class driver item.
} USBH_CLASS_DRV_ITEM;

/********************************************************************************************************
 *                                       ASYNCHRONOUS EVENT TYPES
 *******************************************************************************************************/

typedef enum usbh_async_event_type {
  USBH_ASYNC_EVENT_TYPE_NONE = 0u,

  USBH_ASYNC_EVENT_TYPE_URB_COMPLETE,                           // URB completed.
  USBH_ASYNC_EVENT_TYPE_URB_LIST_FREE                           // Free list of URBs and call callaback.
} USBH_ASYNC_EVENT_TYPE;

/********************************************************************************************************
 *                                               DEVICE STATES
 *******************************************************************************************************/

typedef enum usbh_dev_state {
  USBH_DEV_STATE_NONE = 0u,

  USBH_DEV_STATE_ATTCH = 1u,                                    // Device state attached.
  USBH_DEV_STATE_PWR = 2u,                                      // Device state powered.
  USBH_DEV_STATE_DFLT = 3u,                                     // Device state default.
  USBH_DEV_STATE_ADDR = 4u,                                     // Device state addressed.
  USBH_DEV_STATE_CONFIG = 5u                                    // Device state configured.
} USBH_DEV_STATE;

/********************************************************************************************************
 *                                               HUB EVENT TYPES
 *******************************************************************************************************/

typedef enum usbh_hub_event_type {
  USBH_HUB_EVENT_TYPE_NONE = 0u,

  USBH_HUB_EVENT_TYPE_PORT_REQ,                                 // Port req              event type.
  USBH_HUB_EVENT_TYPE_DEV_CONFIG_SET,                           // Set configuration req event type.
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  USBH_HUB_EVENT_TYPE_IF_ALT_SET,                               // Alt IF set req        event type.
#endif
  USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG,                         // Port status chng req  event type.
  USBH_HUB_EVENT_TYPE_HC_OPER                                   // HC oper req           event type.
} USBH_HUB_EVENT_TYPE;

typedef enum usbh_hc_oper_type {
  USBH_HC_OPER_TYPE_NONE = 0u,

  USBH_HC_OPER_TYPE_SUSPEND,                                    // Suspend operation on HC.
  USBH_HC_OPER_TYPE_RESUME,                                     // Resume  operation on HC.
  USBH_HC_OPER_TYPE_STOP                                        // Stop    operation on HC.
} USBH_HC_OPER_TYPE;

/********************************************************************************************************
 *                                               HUB STATES
 *******************************************************************************************************/

typedef enum usbh_hub_state {
  USBH_HUB_STATE_NONE = 0u,

  USBH_HUB_STATE_CONN,                                          // Hub state connected.
  USBH_HUB_STATE_SUSPEND                                        // Hub state suspended.
} USBH_HUB_STATE;

/********************************************************************************************************
 *                                           USB HUB EVENT DATA TYPE
 *******************************************************************************************************/

struct usbh_hub_event {
  USBH_HUB_EVENT_TYPE Type;                                     // Type of hub event.
  void *ArgPtr;                                                 // Ptr to event argument, varies depending of type.
  USBH_HUB_EVENT *NextPtr;                                      // Ptr to next HUB event in Q.
};

typedef struct usbh_hub_event_port_req_arg {
  USBH_DEV_HANDLE HUB_DevHandle;                                // Handle to HUB device on which the request is posted.
  USBH_HUB_PORT_REQ PortReq;                                    // Type of port request.
  CPU_INT08U PortNbr;                                           // Port number on which request is posted.

  USBH_PORT_REQ_CMPL PortReqCmplCallback;                       // Callback to notify when request has completed.
  void *CallbackArgPtr;                                         // Pointer to argument passed to callback.
} USBH_HUB_EVENT_PORT_REQ_ARG;

typedef struct usbh_hub_event_dev_config_set_arg {
  USBH_DEV_HANDLE DevHandle;                                    // Handle to device on which to set configuration.
  CPU_INT08U CfgNbr;                                            // Number of configuration to set.
} USBH_HUB_EVENT_DEV_CONFIG_SET_ARG;

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
typedef struct usbh_hub_event_if_alt_set_arg {
  USBH_DEV_HANDLE DevHandle;                                    // Handle to device   on which to set alternate IF.
  USBH_FNCT_HANDLE FnctHandle;                                  // Handle to function on which to set alternate IF.
  CPU_INT08U IF_Ix;                                             // Index of IF        on which to set alternate IF.
  CPU_INT08U IF_AltIx;                                          // Index of alternate IF to set.

  USBH_IF_ALT_SET_CMPL IF_AltSetCmplCallback;                   // Callback to notify when alternate IF has been set.
  void *CallbackArgPtr;                                         // Pointer to argument passed to callback.
} USBH_HUB_EVENT_IF_ALT_SET_ARG;
#endif

typedef struct usbh_hub_event_hc_oper {
  USBH_HC *HC_Ptr;                                              // Pointer to USBH_HC on which operation is requested.
  USBH_HC_OPER_TYPE Type;                                       // Type of operation requested.
  USBH_HC_OPER_CMPL HC_OperCmplCallback;                        // Callback to notify when operation has completed.
  void *CallbackArgPtr;                                         // Pointer to argument passed to callback.
} USBH_HUB_EVENT_HC_OPER;

/********************************************************************************************************
 *                                           ENDPOINT STATES
 *******************************************************************************************************/

typedef enum usbh_ep_state {
  USBH_EP_STATE_NONE,

  USBH_EP_STATE_READY,                                          // EP ready to receive URBs.
  USBH_EP_STATE_PREPARING,                                      // EP is preparing an(other) xfer.
  USBH_EP_STATE_QUEUED,                                         // EP has at least one URB queued.
  USBH_EP_STATE_COMPLETING,                                     // EP is completing a xfer.

  USBH_EP_STATE_ABORTING_CLOSING,                               // EP is currently aborting or closing.
  USBH_EP_STATE_ABORT_CLOSE_CMPL                                // EP waits for sync xfer to finish pending.
} USBH_EP_STATE;

/********************************************************************************************************
 *                                       HUB FUNCTION DATA TYPE
 *******************************************************************************************************/

struct usbh_hub_fnct {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_DEV *DevPtrList[USBH_HUB_NBR_PORT];                      // Tbl of pointers to dev (per port) connected to hub.
#endif

  USBH_DEV *DevListHeadPtr;                                     // Pointer to first dev connected to hub.
  USBH_DEV *DevListTailPtr;                                     // Pointer to last  dev connected to hub.
  CPU_INT08U PortBusyCnt;                                       // Number of port currently used.

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  CPU_INT08U *StatusBufPtr;                                     // Ptr to status chng buffer received from hub.

  CPU_INT08U ErrCnt;                                            // Intr xfer error count.

  USBH_EP_HANDLE EP_IntrHandle;                                 // Handle to intr in endpoint.

  USBH_DEV_HANDLE EventHubDevHandle;                            // Hub's device handle when event occured.
#endif

  CPU_INT08U PortStatusChng;                                    // Bitmap represents chng on hub's ports.

  USBH_HUB_STATE State;                                         // HUB state.

  CPU_INT08U NbrPort;                                           // Number of port on hub.
  CPU_INT16U Characteristics;                                   // Hub characteristics.
  CPU_INT08U PwrOn2PwrGood;                                     // Delay to apply after set pwr on port.

  USBH_DEV *DevPtr;                                             // Pointer to device.
  USBH_DEV_HANDLE Handle;                                       // Handle to device.

  USBH_HUB_EVENT PersistentHubEvent;                            // Persistent HUB event for port change requests.
};

/********************************************************************************************************
 *                                       USB ASYNC EVENT DATA TYPE
 *******************************************************************************************************/

struct usbh_async_event {
  USBH_ASYNC_EVENT_TYPE Type;                                   // Type of async event.

  USBH_DEV_HANDLE DevHandle;                                    // Device handle associated to event.
  USBH_EP_HANDLE EP_Handle;                                     // EP handle  associated to event.

  void *ArgPtr;                                                 // Ptr to event argument.

  USBH_ASYNC_EVENT *NextPtr;                                    // Ptr to next async event in Q.
};

/********************************************************************************************************
 *                               USB REQUEST BLOCK (URB) MANAGEMENT DATA TYPE
 *
 * Notes: (1) This structure contains URB related data used for stack internal management.
 *******************************************************************************************************/

struct usbh_urb_mgmt {
  USBH_ASYNC_FNCT AsyncFnct;                                    // Fnct to call upon completion of transfer.
  void *AsyncFnctArgPtr;                                        // Arg to pass to fnct upon completion of transfer.

  USBH_TOKEN Token;                                             // Data transfer token.

  CPU_INT08U *BufPtr;                                           // Pointer to data buffer.
  CPU_INT32U BufLen;                                            // Length of data buffer in octets.

#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
  CPU_INT16U FrmStart;                                          // Transfer starting frame (For ISOC EP only).
  CPU_INT08U FrmCnt;                                            // Frame count (For ISOC EP only).
#endif

  void *HCD_URB_DataPtr;                                        // Ptr to HCD's URB data.

  USBH_URB_MGMT *NextPtr;                                       // Ptr to next URB mgmt struct of EP.
};

/********************************************************************************************************
 *                                   USB ENDPOINT MANAGEMENT DATA TYPE
 *
 * Notes: (1) This structure contains endpoint data used for stack internal management.
 *******************************************************************************************************/

struct usbh_ep_mgmt {
  USBH_EP_STATE State;                                          // Current EP state.

  KAL_MON_HANDLE MonHandle;                                     // Handle to monitor to manage EP state.

  USBH_EP *EP_Ptr;                                              // Ptr to corresponding USBH_EP struct.
  USBH_EP_HANDLE Handle;                                        // Endpoint's handle.
  void *HCD_EP_DataPtr;                                         // Ptr to HCD's EP specific data.

  USBH_DEV *DevPtr;                                             // Ptr to EP's parent dev.
  USBH_FNCT *FnctPtr;                                           // Ptr to fnct to which this EP belongs.

  CPU_INT08U URB_Cnt;                                           // Nbr of URB submitted on endpoint.
  USBH_URB_MGMT *URB_TailPtr;                                   // Ptr to tail of submitted URB list.
  USBH_URB_MGMT *URB_HeadPtr;                                   // Ptr to head of submitted URB list.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_EP_MGMT *NextPtr;                                        // Pointer to next EP mgmt struct of dev.
#endif
};

/********************************************************************************************************
 *                                       USB ENDPOINT DATA TYPE
 *
 * Notes: (1) This structure contains data as reported by the endpoint descriptor. See the 'Universal
 *               Serial Bus Specification, revision 2.0, section 9.6.6' for more information on the endpoint
 *               descriptor.
 *******************************************************************************************************/

struct usbh_ep {
  CPU_INT08U Addr;                                              // EP addr.
  CPU_INT16U MaxPktSize;                                        // EP max pkt size.
  CPU_INT08U Attrib;                                            // EP attributes.

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U Interval;                                          // EP interval.
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_EP *NextPtr;                                             // Pointer to next EP structure.
#endif
};

/********************************************************************************************************
 *                                       USB INTERFACE DATA TYPE
 *
 * Notes: (1) This structure contains data as reported by the interface descriptor. See the 'Universal
 *               Serial Bus Specification, revision 2.0, section 9.6.5' for more information on the interface
 *               descriptor.
 *
 *           (2) This structure contains reference to 1 or more USBH_EP structures.
 *******************************************************************************************************/

struct usbh_if {
  CPU_INT08U NbrEP;                                             // Nbr of EP contained by the IF.
  CPU_INT08U Nbr;                                               // IF nbr.
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  CPU_INT08U AltNbr;                                            // IF alt nbr.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) == DEF_YES)
  CPU_INT08U ClassCode;                                         // IF's class code.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) == DEF_YES)
  CPU_INT08U Subclass;                                          // IF's subclass code.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) == DEF_YES)
  CPU_INT08U Protocol;                                          // IF's protocol code.
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U StrIx;                                             // IF's string ix.
#endif

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  CPU_INT08U AltCur;                                            // IF's currently selected alt IF index.
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_EP *EPTblPtr[USBH_DEV_NBR_EP - 2u];                      // Table of endpoints in interface.
#else
  USBH_EP *EPHeadPtr;                                           // Ptr to first EP of list.

  USBH_IF *NextPtr;                                             // Pointer to next interface structure.
#endif
};

/********************************************************************************************************
 *                                       USB FUNCTION DATA TYPE
 *
 * Notes: (1) (a) If a USB function contains more than one interface, this structure contains data as
 *                   reported by the Interface Association Descriptor.
 *
 *                   (1) See the 'USB ECN: Interface Association Descriptor' for more information on the
 *                       interface association descriptor.
 *
 *               (b) If a USB function contains only one interface, this structure contains data as reported
 *                   by the interface descriptor.
 *
 *                   (1) See the 'Universal Serial Bus Specification, revision 2.0, section 9.6.5' for more
 *                       information on the interface descriptor.
 *
 *               (c) If the USB function is defined at device level, this structure contains data as reported
 *                   by the device descriptor.
 *
 *           (2) This structure contains reference to 1 or more USBH_IF structures.
 *******************************************************************************************************/

struct usbh_fnct {
  USBH_FNCT_HANDLE Handle;                                      // Function handle.
  CPU_INT08U NbrIF;                                             // Nbr of IF contained in USB function.
  CPU_INT08U ClassCode;                                         // USB function's class code.

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
  CPU_INT08U Subclass;                                          // USB function's subclass code.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
  CPU_INT08U Protocol;                                          // USB function's protocol code.
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U StrIx;                                             // USB function's string ix.
#endif

  USBH_CLASS_DRV *ClassDrvPtr;                                  // Ptr to class drv associated to the function.
  void *ClassFnctPtr;                                           // Ptr to class drv specific data.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_IF **IFTblPtr;                                           // Ptr to table of interface.
#else
  USBH_IF *IFHeadPtr;                                           // Pointer to first interface of fnct.

  USBH_FNCT *NextPtr;                                           // Pointer to next function.
#endif
};

/********************************************************************************************************
 *                                       USB CONFIGURATION DATA TYPE
 *
 * Notes: (1) This structure contains data as reported by the Configuration descriptor. See the 'Universal
 *               Serial Bus Specification, revision 2.0, section 9.6.3' for more information on the
 *               configuration descriptor.
 *
 *           (2) This structure contains reference to 1 or more USBH_FNCT structures.
 *******************************************************************************************************/

struct usbh_config {
  CPU_INT08U Nbr;                                               // Config's value.

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
  CPU_INT08U Attr;                                              // Config's attributes.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
  CPU_INT08U MaxPower;                                          // Config's max power.
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U StrIx;                                             // Config's string ix.
#endif

  CPU_INT08U NbrFnct;                                           // Nbr of USB function contained in config.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_FNCT **FnctTblPtr;                                       // Pointer to function table.
#else
  USBH_FNCT *FnctHeadPtr;                                       // Pointer to first function.
#endif
};

/********************************************************************************************************
 *                                           USB DEVICE DATA TYPE
 *
 * Notes: (1) This structure contains data as reported by the device descriptor. See the 'Universal
 *               Serial Bus Specification, revision 2.0, section 9.6.1' for more information on the
 *               device descriptor.
 *
 *           (2) The AddrCur field contains the current address of the device. Before the first SetAddress
 *               request is issued, the device will respond to address 0.
 *
 *           (3) A USB device is represented by the following structures topology.
 *
 *                                           ----------------
 *                                           |   USBH_DEV   |______    ----------------
 *                                           |              |      |---| USBH_EP_MGMT |
 *                                           ----------------      |   |              |
 *                                   ________________|             |   ----------------
 *                                   |                             |           |
 *                           ----------------                      |       ----------------
 *                           | USBH_CONFIG  |                      |       |USBH_URB_MGMT |
 *                           |              |                      |       |              |
 *                           ----------------                      |       ----------------
 *                   _____________|________________ ____ ...       |               |
 *                   |                             |               |       ----------------
 *           ----------------               ----------------       |       |USBH_URB_MGMT |
 *           |  USBH_FNCT   |               |  USBH_FNCT   |       |       |              |
 *           |              |               |              |       |       ----------------
 *           ----------------               ----------------       |               |
 *               ___|_______________________ ...                   |              ___
 *               |                        |                        |              ///
 *       ----------------          ----------------                |
 *       |   USBH_IF    |          |   USBH_IF    |                |   ----------------
 *       |              |          |              |                |---| USBH_EP_MGMT |
 *       ----------------          ----------------                    |              |
 *            _|______________________ ...                            ----------------
 *           |                     |                                            |
 *       ----------------      ----------------                               ----------------
 *       |   USBH_EP    |      |   USBH_EP    |                               |USBH_URB_MGMT |
 *       |              |      |              |                               |              |
 *       ----------------      ----------------                               ----------------
 *                                                                                   |
 *                                                                                   ...
 *
 *               (a) A USBH_CONFIG contains a representation of the device as described by the configuration
 *                   descriptor. Each USBH_FNCT will contain at least one interface. When an Interface
 *                   Association Descriptor (IAD) is present, the function will contain more than one
 *                   interface.
 *
 *               (b) A USBH_DEV contains a USBH_EP_MGMT per opened endpoint. This structure contains data for
 *                   internal management of the endpoint.
 *
 *               (c) A USBH_EP_MGMT will contain one USBH_URB_MGMT per submitted URB on endpoint.
 *******************************************************************************************************/

struct usbh_dev {
  USBH_DEV_HANDLE Handle;                                       // USB device's device handle.
  USBH_EP_HANDLE CtrlEP_Handle;                                 // USB device's endpoint handle.

  KAL_MON_HANDLE MonHandle;                                     // Handle to monitor to manage dev locking.
  CPU_INT08U LockRefCnt;                                        // Ref cnt to make sure dev can be modified.

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SPEC_NBR) == DEF_YES)
  CPU_INT16U SpecNbr;                                           // USB device's specification nbr.
#endif

  CPU_INT08U ClassCode;                                         // USB device's class code.

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) == DEF_YES)
  CPU_INT08U Subclass;                                          // USB device's subclass code.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) == DEF_YES)
  CPU_INT08U Protocol;                                          // USB device's protocol code.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
  CPU_INT16U VendorID;                                          // USB device's vendor ID.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
  CPU_INT16U ProductID;                                         // USB device's product ID.
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
  CPU_INT16U RelNbr;                                            // USB device's release nbr.
#endif

  CPU_INT08U CtrlMaxPacketSize;                                 // USB device's ctrl EP max pkt size.
  CPU_INT08U NbrConfig;                                         // USB device's nbr of configurations.
  USBH_DEV_SPD Spd;                                             // USB device's speed.

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U ManufacturerStrIx;                                 // USB device's manufacturer string ix.
  CPU_INT08U ProdStrIx;                                         // USB device's product string ix.
  CPU_INT08U SerNbrStrIx;                                       // USB device's serial nbr string ix.

  CPU_INT16U LangID;                                            // USB device's selected string lang ID.
#endif

  USBH_DEV_STATE State;                                         // USB device's current state.
  CPU_INT08U Addr;                                              // USB device's address.
  CPU_INT08U AddrCur;                                           // USB device's current address. See note (2).
  CPU_BOOLEAN IsSuspended;                                      // USB device's current suspend state.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_EP_MGMT *EP_MgmtTblPtr[USBH_DEV_NBR_EP];                 // USB device's EP management table.
#else
  USBH_EP_MGMT *EP_MgmtHeadPtr;                                 // Pointer to head of dev EP mgmt list.
#endif

  USBH_HOST *HostPtr;                                           // Pointer to host struct to which this dev is attch.
  USBH_HC *HC_Ptr;                                              // Pointer to HC struct to which this dev is attch.

  KAL_LOCK_HANDLE DfltEP_LockHandle;                            // Control endpoint lock.
  CPU_INT16U *SetupBufPtr;                                      // Buffer used for setup transactions.

  USBH_HUB_FNCT *HubFnctPtr;                                    // Pointer to hub fnct to which this dev is attch.
  CPU_INT08U PortNbr;                                           // Hub port number.

  USBH_CLASS_DRV *ClassDrvPtr;                                  // Ptr to class driver (If class defined at dev level).
  void *ClassFnctPtr;                                           // Ptr to function (If class defined at dev level).
  CPU_BOOLEAN IsClassDevLevel;                                  // Indicates if class is defined at dev level.

  USBH_CONFIG ConfigCur;                                        // Current configuration struct.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_DEV *HostDevNextPtr;                                     // Ptr to next USBH_DEV struct in host's dev list.
#endif

  USBH_DEV *HubListNextPtr;                                     // Ptr to next USBH_DEV struct in hub's dev list.
  USBH_DEV *HubListPrevPtr;                                     // Ptr to prev USBH_DEV struct in hub's dev list.
};

/********************************************************************************************************
 *                                       USB HOST CONTROLLER DATA TYPE
 *******************************************************************************************************/

struct usbh_hc {
  CPU_INT08U Ix;                                                // Host controller index.
  CPU_CHAR *Name;                                               // Host Controller name.
  USBH_HC_DRV Drv;                                              // Host controller driver.

  USBH_HUB_FNCT RH_Fnct;                                        // Root Hub function.

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  CPU_BOOLEAN IsStopped;                                        // Flag indicating if HC is stopped or not.
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_HC *NextPtr;                                             // Ptr to next Host Controller of host.
#endif
};

/********************************************************************************************************
 *                                           USB HOST DATA TYPE
 *******************************************************************************************************/

struct usbh_host {
  CPU_INT08U Ix;                                                // Host index.

  CPU_INT08U HC_NbrNext;                                        // Index of next free host controller.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_HC **HC_TblPtr;                                          // Host controller table.

  USBH_DEV **DevTbl;                                            // Table of devices.
  CPU_INT32U *DevAddrPool;                                      // Pointer to device address pool.
#else
  USBH_DEV *DevHeadPtr;                                         // Pointer to head of device list.
  CPU_INT32U DevAddrPool;                                       // Device address pool.

  USBH_HC *HC_HeadPtr;                                          // Host controller table.
#endif
};

/********************************************************************************************************
 *                                       USB HOST ROOT DATA TYPE
 *******************************************************************************************************/

typedef struct usbh {
  CPU_INT08U HostQty;                                           // Quantity of hosts.
  CPU_INT32U StdReqTimeout;                                     // Timeout in ms for the std req.
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  CPU_INT16U PreferredStrLangID;                                // Preferred lang ID when retrieving str from dev.
#endif

  CPU_INT08U *DescBufPtr;                                       // Ptr to buffer used to retrieve descriptors from devs.
  CPU_INT16U DescLen;                                           // Length of desc contained in DescBufPtr.
  USBH_DEV *DescDevPtr;                                         // Pointer to dev that currently use desc buf.

  CPU_INT08U HostNbrNext;                                       // Next host number to allocate.
  USBH_HOST *HostTblPtr;                                        // Ptr to host table.

  USBH_CLASS_DRV_ITEM *ClassDrvItemHeadPtr;                     // Ptr to head of class drv items.

  KAL_TASK_UUID HUB_TaskUUID;                                   // UUID for HUB task.

  KAL_TASK_HANDLE AsyncTaskHandle;                              // Handle to async event task.
  KAL_LOCK_HANDLE AsyncEventListLock;                           // Handle to async list lock.
  KAL_SEM_HANDLE AsyncEventSem;                                 // Handle to async event sem.
  USBH_ASYNC_EVENT *AsyncEventQHeadPtr;                         // Ptr to 1st  async event to process by async task.
  USBH_ASYNC_EVENT *AsyncEventQTailPtr;                         // Ptr to last async event to process by async task.

  USBH_ASYNC_EVENT *AsyncEventFreeHeadPtr;                      // Pointer to head list of free async event struct.

  MEM_DYN_POOL DevPool;                                         // Dyn mem pool of USB devices.
  MEM_DYN_POOL FnctPool;                                        // Dyn mem pool of USB functions.
  MEM_DYN_POOL IF_Pool;                                         // Dyn mem pool of USB interfaces.
  MEM_DYN_POOL EP_Pool;                                         // Dyn mem pool of USB endpoint.
  MEM_DYN_POOL EP_MgmtPool;                                     // Dyn mem pool of USB endpoint management structures.
  MEM_DYN_POOL URB_MgmtPool;                                    // Dyn mem pool of USB URB management structures.
} USBH;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
