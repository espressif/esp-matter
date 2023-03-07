/***************************************************************************//**
 * @file
 * @brief USB Device Core Internal Functions
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

#ifndef  _USBD_CORE_PRIV_H_
#define  _USBD_CORE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CORE EVENTS DATA TYPE
 *******************************************************************************************************/

typedef enum usbd_event_code {
  USBD_EVENT_BUS_RESET = 0u,
  USBD_EVENT_BUS_SUSPEND,
  USBD_EVENT_BUS_RESUME,
  USBD_EVENT_BUS_CONN,
  USBD_EVENT_BUS_DISCONN,
  USBD_EVENT_BUS_HS,
  USBD_EVENT_EP,
  USBD_EVENT_SETUP
} USBD_EVENT_CODE;

/********************************************************************************************************
 *                                           ENDPOINT STATES
 *******************************************************************************************************/

typedef enum usbd_ep_state {
  USBD_EP_STATE_CLOSE = 0,
  USBD_EP_STATE_OPEN,
  USBD_EP_STATE_STALL
} USBD_EP_STATE;

/********************************************************************************************************
 *                                           TRANSFER STATES
 *
 * Note(s): (1) If an asynchronous transfer cannot be fully queued in the driver, no more transfer can be
 *               queued, to respect the transfers sequence.
 *               For example, if a driver can queue only 512 bytes at once and the class/application needs
 *               to queue 518 bytes, the first 512 bytes will be queued and it will be impossible to queue
 *               another transaction. The remaining 6 bytes will only be queued when the previous (512
 *               bytes) transaction completes. The state of the endpoint will be changed to
 *               USBD_EP_XFER_TYPE_ASYNC_PARTIAL and other transfers could be queued after this one.
 *******************************************************************************************************/

typedef enum usbd_xfer_state {
  USBD_XFER_STATE_NONE = 0,                                     // No            xfer    in progress.
  USBD_XFER_STATE_SYNC,                                         // Sync          xfer    in progress.
  USBD_XFER_STATE_ASYNC,                                        // Async         xfer(s) in progress.
  USBD_XFER_STATE_ASYNC_PARTIAL                                 // Partial async xfer(s) in progress (see Note #1).
} USBD_XFER_STATE;

/********************************************************************************************************
 *                                               URB STATES
 *******************************************************************************************************/

typedef enum usbd_urb_state {
  USBD_URB_STATE_IDLE = 0,                                      // URB is in the memory pool, not used by any EP.
  USBD_URB_STATE_XFER_SYNC,                                     // URB is used for a   sync xfer.
  USBD_URB_STATE_XFER_ASYNC                                     // URB is used for an async xfer.
} USBD_URB_STATE;

/********************************************************************************************************
 *                                   ENDPOINT USB REQUEST BLOCK DATA TYPE
 *
 * Note(s): (1) The 'Flags' field is used as a bitmap. The following bits are used:
 *
 *                   D7..2 Reserved (reset to zero)
 *                   D1    End-of-transfer:
 *                               If this bit is set and transfer length is multiple of maximum packet
 *                               size, a zero-length packet is transferred to indicate a short transfer to
 *                               the host.
 *                   D0    Extra URB:
 *                               If this bit is set, it indicates that this URB is considered an 'extra'
 *                               URB, that is shared amongst all endpoints. If this bit is cleared, it
 *                               indicates that this URB is 'reserved' to allow every endpoint to have at
 *                               least one URB available at any time.
 *******************************************************************************************************/

typedef struct usbd_urb {
  CPU_INT08U      *BufPtr;                                      // Pointer to buffer.
  CPU_INT32U      BufLen;                                       // Buffer length.
  CPU_INT32U      XferLen;                                      // Length that has been transferred.
  CPU_INT32U      NextXferLen;                                  // Length of the next transfer.
  CPU_INT08U      Flags;                                        // Flags (see Note #1).
  USBD_URB_STATE  State;                                        // State of the transaction.
  USBD_ASYNC_FNCT AsyncFnct;                                    // Asynchronous notification function.
  void            *AsyncFnctArg;                                // Asynchronous function argument.
  RTOS_ERR        Err;                                          // Error passed to callback, if any.
  struct usbd_urb *NextPtr;                                     // Pointer to next     URB in list.
} USBD_URB;

/********************************************************************************************************
 *                                           ENDPOINT DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_ep {
  USBD_EP_STATE   State;                                        // EP   state.
  USBD_XFER_STATE XferState;                                    // Xfer state.
  CPU_INT08U      Addr;                                         // Address.
  CPU_INT08U      Attrib;                                       // Attributes.
  CPU_INT16U      MaxPktSize;                                   // Maximum packet size.
  CPU_INT08U      Interval;                                     // Interval.
  CPU_INT08U      TransPerFrame;                                // Transaction per microframe (HS only).
  CPU_INT08U      Ix;                                           // Allocation index.
#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  CPU_BOOLEAN     URB_MainAvail;                                // Flag indicating if main URB associated to EP avail.
#endif
  USBD_URB        *URB_HeadPtr;                                 // USB request block head of the list.
  USBD_URB        *URB_TailPtr;                                 // USB request block tail of the list.
} USBD_EP;

/********************************************************************************************************
 *                                   ENDPOINT INFORMATION DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_ep_info {
  CPU_INT08U          Addr;                                     // Endpoint address.
  CPU_INT08U          Attrib;                                   // Endpoint attributes.
  CPU_INT08U          Interval;                                 // Endpoint interval.
  CPU_INT16U          MaxPktSize;
  CPU_INT08U          SyncAddr;                                 // Audio Class Only: associated sync endpoint.
  CPU_INT08U          SyncRefresh;                              // Audio Class Only: sync feedback rate.
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  struct usbd_ep_info *NextPtr;                                 // Pointer to next interface group structure.
#endif
} USBD_EP_INFO;

/********************************************************************************************************
 *                                   DEVICE INTERFACE GROUP DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_if_grp {
  CPU_INT08U         ClassCode;                                 // IF     class code.
  CPU_INT08U         ClassSubCode;                              // IF sub class code.
  CPU_INT08U         ClassProtocolCode;                         // IF protocol  code.
  CPU_INT08U         IF_Start;                                  // IF index of the first IFs associated with a group.
  CPU_INT08U         IF_Cnt;                                    // Number of contiguous  IFs associated with a group.
#if (USBD_CFG_STR_EN == DEF_ENABLED)
  const CPU_CHAR     *NamePtr;                                  // IF group name.
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  struct usbd_if_grp *NextPtr;                                  // Pointer to next interface group structure.
#endif
} USBD_IF_GRP;

/********************************************************************************************************
 *                                   INTERFACE ALTERNATE SETTING DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_if_alt {
  void               *ClassArgPtr;                              // Dev class drv arg ptr specific to alternate setting.
  CPU_INT32U         EP_AllocMap;                               // EP allocation bitmap.
  CPU_INT08U         EP_NbrTotal;                               // Number of EP.

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  const CPU_CHAR     *NamePtr;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  USBD_EP_INFO       *EP_TblPtrs[USBD_EP_MAX_NBR];
  CPU_INT32U         EP_TblMap;
#else
  USBD_EP_INFO       *EP_HeadPtr;
  USBD_EP_INFO       *EP_TailPtr;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  struct usbd_if_alt *NextPtr;                                  // Pointer to next alternate setting structure.
#endif
} USBD_IF_ALT;

/********************************************************************************************************
 *                                           INTERFACE DATA TYPE
 *
 * Note(s):  (1) The interface structure contains information about the USB interfaces. It contains a
 *               list of all alternate settings (including the default interface).
 *
 *                               IFs         | ---------------------  Alt IF Settings ------------------- |
 *                                               Dflt           Alt_0          Alt_1     ...        Alt_n
 *                   -----       +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |  -----  | IF_0   |---->| IF_0_0  |--->| IF_0_0  |-->| IF_0_1  |-- ... --> | IF_0_1  |
 *                   |    |    +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |    |        |
 *                   |   GRP0      V
 *                   |    |    +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |    |    | IF_1   |---->| IF_1_0  |--->| IF_1_0  |-->| IF_1_1  |-- ... --> | IF_1_1  |
 *                   |  -----  +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |              |
 *           CONFIGx  |              V
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |         | IF_2   |---->| IF_1_0  |--->| IF_1_0  |-->| IF_1_1  |-- ... --> | IF_1_1  |
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |             .
 *                   |             .
 *                   |             .
 *                   |             |
 *                   |             V
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |         | IF_n   |---->| IF_n_0  |--->| IF_n_0  |-->| IF_n_1  |-- ... --> | IF_n_1  |
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   ------
 *
 *               (2) Interfaces can be combined together creating a logical group.  This logical group
 *                   represents a function. The device uses the Interface Association Descriptor (IAD)
 *                   to notify the host that multiple interfaces belong to one single function.    The
 *                   'GrpNbr' stores the logical group number that the interface belongs to. By default,
 *                   it is defined to 'USBD_IF_GRP_NBR_NONE'.
 *
 *           (3)  The 'EP_AllocMap' is a bitmap of the allocated physical endpoints.
 *******************************************************************************************************/

typedef struct usbd_if {
  //                                                               ------------ INTERFACE CLASS INFORMATION -----------
  CPU_INT08U     ClassCode;                                     // Device interface     class code.
  CPU_INT08U     ClassSubCode;                                  // Device interface sub class code.
  CPU_INT08U     ClassProtocolCode;                             // Device interface protocol  code.
  USBD_CLASS_DRV *ClassDrvPtr;                                  // Device class driver pointer.
  void           *ClassArgPtr;                                  // Dev class drv arg ptr specific to interface.
                                                                // ----------- INTERFACE ALTERNATE SETTINGS -----------
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  //                                                               IF alternate settings array.
  USBD_IF_ALT    **AltTblPtrs;
#else
  USBD_IF_ALT    *AltHeadPtr;                                   // IF alternate settings linked-list.
  USBD_IF_ALT    *AltTailPtr;
#endif
  USBD_IF_ALT    *AltCurPtr;                                    // Pointer to current alternate setting.
  CPU_INT08U     AltCur;                                        // Alternate setting selected by host.
  CPU_INT08U     AltNbrTotal;                                   // Number of alternate settings supported by this IF.
  CPU_INT08U     GrpNbr;                                        // Interface group number.
  CPU_INT32U     EP_AllocMap;                                   // EP allocation bitmap.
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  struct usbd_if *NextPtr;                                      // Pointer to next interface structure.
#endif
} USBD_IF;

/********************************************************************************************************
 *                                   DEVICE CONFIGURATION DATA TYPE
 *
 * Note(s):  (1) The configuration structure contains information about USB configurations. It contains a
 *               list of interfaces.
 *
 *                                       CONFIG       | ----------------- INTERFACES  ---------------- |
 *                           -----       +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_0 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_1 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                   DEVICEx |         | CONFIG_2 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |             .
 *                           |             .
 *                           |             .
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_n |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                       ------       +----------+    +------+    +------+    +------+          +------+
 *******************************************************************************************************/

typedef struct usbd_config {                                    // -------------- CONFIGURATION STRUCTURE -------------
  CPU_INT08U     Attrib;                                        // Configuration attributes.
  CPU_INT16U     MaxPwr;                                        // Maximum bus power drawn.
  CPU_INT16U     DescLen;                                       // Configuration descriptor length.

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  const CPU_CHAR *NamePtr;                                      // Configuration name.
#endif

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Interface & group list:
  USBD_IF            **IF_TblPtrs;                              // Interfaces list (array).
                                                                // Interfaces group list (array).
  USBD_IF_GRP        **IF_GrpTblPtrs;

#else
  USBD_IF            *IF_HeadPtr;                               // Interfaces list (linked list).
  USBD_IF            *IF_TailPtr;
  USBD_IF_GRP        *IF_GrpHeadPtr;                            // Interfaces group list (linked list).
  USBD_IF_GRP        *IF_GrpTailPtr;
#endif

  CPU_INT08U         IF_NbrTotal;                               // Number of interfaces in this configuration.
  CPU_INT08U         IF_GrpNbrTotal;                            // Number of interfaces group.
  CPU_INT32U         EP_AllocMap;                               // EP allocation bitmap.

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  CPU_INT08U         ConfigOtherSpd;                            // Other-speed configuration.
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  struct usbd_config *NextPtr;
#endif
} USBD_CONFIG;

/********************************************************************************************************
 *                                           USB DEVICE DATA TYPE
 *
 * Note(s):  (1) A USB device could contain multiple configurations. A configuration is a set of
 *               interfaces.
 *
 *               USB Spec 2.0 section 9.2.6.6 states "device capable of operation at high-speed
 *               can operate in either full- or high-speed. The device always knows its operational
 *               speed due to having to manage its transceivers correctly as part of reset processing."
 *
 *               "A device also operates at a single speed after completing the reset sequence. In
 *               particular, there is no speed switch during normal operation. However, a high-
 *               speed capable device may have configurations that are speed dependent. That is,
 *               it may have some configurations that are only possible when operating at high-
 *               speed or some that are only possible when operating at full-speed. High-speed
 *               capable devices must support reporting their speed dependent configurations."
 *
 *               The device structure contains two list of configurations for HS and FS.
 *
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *                   | HS_CONFIG |--->| CONFIG_0 |--->| CONFIG_1 |--->| CONFIG_2 |-- ... -->| CONFIG_n |
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *                   | FS_CONFIG |--->| CONFIG_0 |--->| CONFIG_1 |--->| CONFIG_2 |-- ... -->| CONFIG_n |
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *
 *           (2) If the USB stack is optimized for speed, objects (Configs, IFs, EPs, etc) are implemented
 *               using a hash linking. Pointers are stored in an array allowing easy access by index.
 *
 *           (3) If the USB stack is optimized for size,  objects (Configs, IFs, EPs, etc) are implemented
 *               using a link list. Objects are linked dynamically reducing the overall memory footprint.
 *******************************************************************************************************/

typedef struct usbd_dev {                                       // ----------------- DEVICE STRUCTURE -----------------
  CPU_CHAR       *Name;                                         // Device name.
  CPU_INT08U     Addr;                                          // Device address assigned by host.
  USBD_DEV_STATE State;                                         // Device state.
  USBD_DEV_STATE StatePrev;                                     // Device previous state.
  CPU_BOOLEAN    ConnStatus;                                    // Device connection status.
  USBD_DEV_SPD   Spd;                                           // Device operating speed.
  CPU_INT08U     Nbr;                                           // Device instance number
  USBD_DEV_CFG   DevCfg;                                        // Device configuration.
  USBD_DRV       Drv;                                           // Device driver information.
                                                                // --------------- DEVICE CONFIGURATIONS --------------
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Configuration list (see Note #1).
                                                                // FS configuration array (see Note #2).
  USBD_CONFIG    **ConfigFS_SpdTblPtrs;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  //                                                               HS configuration array (see Note #2).
  USBD_CONFIG    **ConfigHS_SpdTblPtrs;
#endif
#else
  USBD_CONFIG    *ConfigFS_HeadPtr;                             // FS configuration linked-list (see Note #3).
  USBD_CONFIG    *ConfigFS_TailPtr;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  USBD_CONFIG    *ConfigHS_HeadPtr;                             // HS configuration linked-list (see Note #3).
  USBD_CONFIG    *ConfigHS_TailPtr;
#endif
#endif
  USBD_CONFIG    *ConfigCurPtr;                                 // Current device configuration pointer.
  CPU_INT08U     ConfigCurNbr;                                  // Current device configuration number.

  CPU_INT08U     ConfigFS_TotalNbr;                             // Number of FS configurations supported by the device.
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  CPU_INT08U     ConfigHS_TotalNbr;                             // Number of HS configurations supported by the device.
#endif
  //                                                               ---- CONFIGURATION AND STRING DESCRIPTOR BUFFER ----
  CPU_INT08U     *ActualBufPtr;                                 // Pointer to the buffer where data will be written.
  CPU_INT08U     *DescBufPtr;                                   // Configuration & string descriptor buffer.
  CPU_INT08U     DescBufIx;                                     // Configuration & string descriptor buffer index.
  CPU_INT16U     DescBufReqLen;                                 // Configuration & string descriptor requested length.
  CPU_INT16U     DescBufMaxLen;                                 // Configuration & string descriptor maximum length.
  RTOS_ERR       *DescBufErrPtr;                                // Configuration & string descriptor error pointer.
                                                                // --------------- ENDPOINT INFORMATION  --------------
  CPU_INT16U     EP_CtrlMaxPktSize;                             // Ctrl EP maximum packet size.
  CPU_INT08U     EP_IF_Tbl[USBD_EP_MAX_NBR];                    // EP to IF number reference table.
  CPU_INT08U     EP_MaxPhyNbr;                                  // EP Maximum physical number.
                                                                // ------------------ STRING STORAGE  -----------------
#if (USBD_CFG_STR_EN == DEF_ENABLED)
  CPU_CHAR       **StrDesc_Tbl;                                 // String pointers table.
  CPU_INT08U     StrMaxIx;                                      // Current String index.
#endif
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  CPU_INT08U     StrMS_VendorCode;                              // Microsoft Vendor code used in Microsoft OS str.
#endif

  USBD_BUS_FNCTS *BusFnctsPtr;                                  // Pointer to bus events callback functions.
  USBD_SETUP_REQ SetupReq;                                      // Setup request.
  USBD_SETUP_REQ SetupReqNext;                                  // Next setup request.

  CPU_BOOLEAN    SelfPwr;                                       // Device self powered?

  CPU_BOOLEAN    RemoteWakeup;                                  // Remote Wakeup feature.

  CPU_INT08U     *CtrlStatusBufPtr;                             // Buf used for ctrl status xfers.
} USBD_DEV;

/********************************************************************************************************
 *                                               USB CORE EVENTS
 *
 * Note(s) : (1) USB device driver queues bus and transaction events to the core task queue using
 *               the 'USBD_CORE_EVENT' structure.
 *******************************************************************************************************/

typedef struct usbd_core_event {
  USBD_EVENT_CODE Type;                                         // Core event type.
  USBD_DRV        *DrvPtr;                                      // Pointer to driver structure.
  CPU_INT08U      EP_Addr;                                      // Endpoint address.
  RTOS_ERR        Err;                                          // Error Code returned by Driver, if any.
} USBD_CORE_EVENT;

/********************************************************************************************************
 *                                           USB OBJECTS POOL
 *
 * Note(s) : (1) USB objects (device, configuration, interfaces, alternative interface, endpoint, etc)
 *               are allocated from their pools.
 *
 *               (a) USB objects CANNOT be returned to the pool.
 *******************************************************************************************************/

typedef struct usbd {
  USBD_DEV        *DevTbl;                                      // Device object pool.
  CPU_INT08U      DevNbrNext;

  USBD_CONFIG     *ConfigTbl;                                   // Configuration object pool.
  CPU_INT08U      ConfigNbrNext;

  USBD_IF         *IF_Tbl;                                      // Interface object pool.
  CPU_INT08U      IF_NbrNext;

  //                                                               Alternative interface object pool.
  USBD_IF_ALT     *IF_AltTbl;
  CPU_INT08U      IF_AltNbrNext;

  //                                                               Interface group object pool.
  USBD_IF_GRP     *IF_GrpTbl;
  CPU_INT08U      IF_GrpNbrNext;

  //                                                               Endpoints object pool.
  USBD_EP_INFO    *EP_InfoTbl;
  CPU_INT08U      EP_InfoNbrNext;

  CPU_INT32U      CoreEventPoolIx;
  USBD_CORE_EVENT *CoreEventPoolData;
  USBD_CORE_EVENT **CoreEventPoolPtrs;

  USBD_EP         **EP_Tbl;
  USBD_EP         ** *EP_TblPtrs;
  CPU_INT08U      *EP_OpenCtrTbl;
  CPU_INT32U      *EP_OpenBitMapTbl;
  USBD_URB        **URB_Tbl;
  USBD_URB        **URB_TblPtr;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U      StrQtyPerDev;
#endif

  CPU_INT16U      StdReqTimeoutMs;
  CPU_INT08U      DevQty;

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  CPU_INT08U      *URB_ExtraAvailCntTbl;                        // Nbr of extra URB currently used.
#endif

#if (DEF_BIT_IS_SET(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, (RTOS_CFG_MODULE_USBD)))
  CPU_INT08U EP_OpenQty;
#endif
} USBD;

extern USBD *USBD_Ptr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 ------------ DEVICE INTERNAL FUNCTIONS  ------------
USBD_DRV *USBD_DrvRefGet(CPU_INT08U dev_nbr);

void USBD_CoreTaskHandler(CPU_INT08U dev_nbr);

//                                                                 ------------ ENDPOINT INTERNAL FUNCTIONS -----------
void USBD_EP_Init(MEM_SEG      *p_mem_seg,
                  USBD_QTY_CFG *p_qty_cfg,
                  RTOS_ERR     *p_err);

void USBD_EventEP(USBD_DRV   *p_drv,
                  CPU_INT08U ep_addr,
                  RTOS_ERR   err);

void USBD_CtrlOpen(CPU_INT08U dev_nbr,
                   CPU_INT16U max_pkt_size,
                   RTOS_ERR   *p_err);

void USBD_CtrlClose(CPU_INT08U dev_nbr,
                    RTOS_ERR   *p_err);

void USBD_CtrlStall(CPU_INT08U dev_nbr,
                    RTOS_ERR   *p_err);

void USBD_CtrlRxStatus(CPU_INT08U dev_nbr,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err);

void USBD_CtrlTxStatus(CPU_INT08U dev_nbr,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err);

void USBD_EP_Open(USBD_DRV   *p_drv,
                  CPU_INT08U ep_addr,
                  CPU_INT16U max_pkt_size,
                  CPU_INT08U attrib,
                  CPU_INT08U interval,
                  RTOS_ERR   *p_err);

void USBD_EP_Close(USBD_DRV   *p_drv,
                   CPU_INT08U ep_addr,
                   RTOS_ERR   *p_err);

void USBD_EP_XferAsyncProcess(USBD_DRV   *p_drv,
                              CPU_INT08U ep_addr,
                              RTOS_ERR   xfer_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *                                       DEFINED IN OS'S usbd_os.c
 ********************************************************************************************************
 *******************************************************************************************************/

void USBD_OS_Init(USBD_QTY_CFG *p_qty_cfg,
                  MEM_SEG      *p_mem_seg,
                  RTOS_ERR     *p_err);

void USBD_OS_TaskCreate(RTOS_TASK_CFG *p_task_cfg,
                        CPU_INT08U    dev_nbr,
                        CPU_INT16U    event_qty,
                        RTOS_ERR      *p_err);

void USBD_OS_DevTaskPrioSet(CPU_INT08U     dev_nbr,
                            RTOS_TASK_PRIO prio,
                            RTOS_ERR       *p_err);

void USBD_OS_EP_SignalCreate(CPU_INT08U dev_nbr,
                             CPU_INT08U ep_ix,
                             RTOS_ERR   *p_err);

void USBD_OS_EP_SignalDel(CPU_INT08U dev_nbr,
                          CPU_INT08U ep_ix);

void USBD_OS_EP_SignalPend(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           CPU_INT16U timeout_ms,
                           RTOS_ERR   *p_err);

void USBD_OS_EP_SignalAbort(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix,
                            RTOS_ERR   *p_err);

void USBD_OS_EP_SignalPost(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           RTOS_ERR   *p_err);

void USBD_OS_EP_LockCreate(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           RTOS_ERR   *p_err);

void USBD_OS_EP_LockDel(CPU_INT08U dev_nbr,
                        CPU_INT08U ep_ix);

void USBD_OS_EP_LockAcquire(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix,
                            RTOS_ERR   *p_err);

void USBD_OS_EP_LockRelease(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix);

void USBD_OS_DbgEventRdy(void);

void USBD_OS_DbgEventWait(void);

void *USBD_OS_CoreEventGet(CPU_INT08U dev_nbr,
                           RTOS_ERR   *p_err);

void USBD_OS_CoreEventPut(CPU_INT08U dev_nbr,
                          void       *p_event);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
