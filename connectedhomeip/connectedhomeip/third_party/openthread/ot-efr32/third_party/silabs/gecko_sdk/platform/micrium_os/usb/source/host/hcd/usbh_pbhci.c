/***************************************************************************//**
 * @file
 * @brief USB Host - Pipe-Based Host Controller Interface (Pbhci)
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
 * @note             (1) The Pipe-Based Host Controller Interface (PBHCI) is a generic module that allows to
 *                       interface Pipe-Based USB Host Controllers with the USB Host core via its linked-based
 *                       oriented API. The PBHCI is seen by the core as a typical driver. It is an
 *                       incomplete driver in itself and requires a lower level driver to handle hardware
 *                       specific operations via a pipe-based oriented API. These driver are called the
 *                       Pipe-Based Host Controller Drivers (PBHCD).
 *                       Following is a list that summarizes the features of the PBHCI:
 *                       - (a) Handling of transfer queuing.
 *                       - (b) Handling of periodic transfers.
 *                       - (c) Management of pipe-endpoint dynamic association.
 *                       - (d) Management of root hub port(s) status.
 *                       - (e) Management of USB transfers vs transactions.
 *                       - (f) Verification of bandwidth availability.
 *
 * @note             (2) The PBHCI interfaces with the core as a typical driver.
 *                   Typical linked-list based driver:      Pipe-based Host Controller Driver:
 *                   @verbatim
 *                       +------------------+                    +------------------+
 *                       |       Core       |                    |       Core       |
 *                       |                  |                    |                  |
 *                       +--------+---------+                    +--------+---------+
 *                                |                                       |
 *                                |                                       |
 *                       +--------+---------+                    +--------+---------+
 *                       | Host Controller  |                    |      PBHCI       |
 *                       |  Driver (HCD)    |                    |                  |
 *                       +--------+---------+                    +--------+---------+
 *                                |                                       |
 *                                |                                       |
 *                                |                              +--------+---------+
 *                                |                              |      PBHCD       |
 *                                |                              |                  |
 *                                |                              +--------+---------+
 *                                |                                       |
 *                                |                                       |                         Software
 *                     -----------|---------------------------------------|---------------------------------
 *                                |                                       |                         Hardware
 *                       +--------+---------+                    +--------+---------+
 *                       | Host Controller  |                    | Host Controller  |
 *                       |                  |                    |                  |
 *                       +------------------+                    +------------------+
 *                   @endverbatim
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST PBHCI requires USB Host Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <usb/source/host/core/usbh_core_priv.h>

#include  <usb/source/host/hcd/usbh_pbhcd_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>

#include  <usb/include/host/usbh_pbhci.h>
#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_ep.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, HCD, PBHCI)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_DBG_PBHCI_STATS_EN                        DEF_DISABLED

#define  USBH_PBHCI_XFER_TYPE_ASYNC_BULK_IX               0u
#define  USBH_PBHCI_XFER_TYPE_ASYNC_CTRL_IX               1u

#define  USBH_PBHCI_PIPE_ALLOC_ERR_CNT_MAX               10u    // Max retry cnt for periodic xfer when no pipe avail.

#define  USBH_PBHCI_HC_MAX_QTY_OPTIMIZE_SPD               4u

#define  USBH_PBHCI_PIPE_LIST_QTY                         3u

#define  USBH_PBHCI_EP_ASYNC_LIST_QTY                     2u

#define  USBH_PBHCI_PERIODIC_MAX_INTERVAL               128u

/********************************************************************************************************
 *                                               DEBUG MACROS
 *******************************************************************************************************/

#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
#include  "../../core/host.h"

#define  USBH_DBG_PBHCI_HOST_QTY                          4u
#define  USBH_DBG_PBHCI_HC_QTY                            4u
#define  USBH_DBG_PBHCI_DEV_QTY                           7u
#define  USBH_DBG_PBHCI_SCHED_EVENT_QTY                   3u
#define  USBH_DBG_PBHCI_EVENT_PORT_QTY                    4u
#define  USBH_DBG_PBHCI_URB_DONE_QTY                     16u
#define  USBH_DBG_PBHCI_XFER_CMPL_QTY                     4u

#define  USBH_DBG_PBHCI_STATS_RESET()                       Mem_Clr((void *)&USBH_DbgPBHCI_Stats, \
                                                                    (CPU_SIZE_T) sizeof(USBH_DBG_PBHCI_STATS));
#define  USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, stat) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].stat++;
#define  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(host_nbr, hc_nbr, ix) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].EventPortTbl[ix].Cnt++;
#define  USBH_DBG_PBHCI_STATS_SCHED_EVENT_INC(host_nbr, hc_nbr, ix) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].SchedEventTbl[ix].Cnt++;
#define  USBH_DBG_PBHCI_STATS_DEVICE_INC(host_nbr, hc_nbr, dev_nbr, stat) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].DevTbl[dev_nbr].stat++;
#define  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, stat) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].DevTbl[dev_nbr].EpTbl[ep_phy_nbr].stat++;
#define  USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, ix) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].DevTbl[dev_nbr].EpTbl[ep_phy_nbr].UrbDoneTbl[ix].Cnt++;
#define  USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, ix) \
  USBH_DbgPBHCI_Stats[host_nbr][hc_nbr].DevTbl[dev_nbr].EpTbl[ep_phy_nbr].XferCmplTbl[ix].Cnt++;
#else
#define  USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, stat)
#define  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(host_nbr, hc_nbr, ix)
#define  USBH_DBG_PBHCI_STATS_SCHED_EVENT_INC(host_nbr, hc_nbr, ix)
#define  USBH_DBG_PBHCI_STATS_DEVICE_INC(host_nbr, hc_nbr, dev_nbr, stat)
#define  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, stat)
#define  USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, ix)
#define  USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, ix)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATION
 *******************************************************************************************************/

typedef struct usbh_pbhci_hc USBH_PBHCI_HC;
typedef struct usbh_pbhci_pipe USBH_PBHCI_PIPE;
typedef struct usbh_pbhci_ed USBH_PBHCI_ED;

/********************************************************************************************************
 *                                           PBHCI EVENT TYPE
 *
 * Note(s) : (1) Start-of-frame event (USBH_PBHCI_EVENT_TYPE_SOF). Indicates to the PBHCI that a start of
 *               frame occurred and that it must potentially schedule periodic (interrupt or isochronous)
 *               transfer(s).
 *
 *           (2) Pipe complete event (USBH_PBHCI_EVENT_TYPE_PIPE_CMPL). Indicates to the PBHCI that a transfer
 *               has completed on a given pipe.
 *******************************************************************************************************/

typedef enum usbh_pbhci_event_type {
  USBH_PBHCI_EVENT_TYPE_NONE = 0u,

  USBH_PBHCI_EVENT_TYPE_SOF,                                    // Start-of-frame    event. See Note (1).
  USBH_PBHCI_EVENT_TYPE_PIPE_CMPL                               // Transfer complete event. See Note (2).
} USBH_PBHCI_EVENT_TYPE;

/********************************************************************************************************
 *                                           PIPE DATA TYPE
 *
 * Note(s) : (1) This data structure represents a hardware pipe.
 *******************************************************************************************************/

struct usbh_pbhci_pipe {
  USBH_PBHCI_PIPE_INFO *PipeInfoPtr;                            // Ptr to pipe info structure related to this pipe.

  //                                                               Ptr to next pipe in free list.
  USBH_PBHCI_PIPE      *PipeNextPtrTbl[USBH_PBHCI_PIPE_LIST_QTY];

  //                                                               Ptr to prev pipe in free list.
  USBH_PBHCI_PIPE      *PipePrevPtrTbl[USBH_PBHCI_PIPE_LIST_QTY];

  USBH_PBHCI_ED        *CurED_Ptr;                              // Ptr to ED struct currently assoc to this pipe.
  USBH_EP_HANDLE       CurEP_Handle;                            // Handle of EP assoc to this pipe.

  CPU_BOOLEAN          MultiTypeAble;                           // Flag indicates if pipe capable of multiple xfer type.
};

/********************************************************************************************************
 *                                       TRANSFER DESCRIPTOR DATA TYPE
 *******************************************************************************************************/

typedef struct usbh_pbhci_td {
  CPU_INT08U           *BufPtr;                                 // Ptr to data buffer.
  CPU_INT32U           BufLen;                                  // Data buffer length.
  CPU_INT32U           XferLen;                                 // Current xfer length.
  CPU_INT32U           XferLenNext;                             // Length of data xfer currently handled by driver.

  USBH_PBHCI_ED        *ED_Ptr;                                 // Ptr to ED.
  USBH_TOKEN           Token;                                   // Transfer token.

  CPU_BOOLEAN          IsSubmitted;                             // Flag indicates if transfer is submitted to driver.

  RTOS_ERR             Err;                                     // Error code.

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U           PipeAllocErrCnt;                         // Pipe allocation error count for this xfer.
#endif

  struct usbh_pbhci_td *NextPtr;                                // Ptr to next TD.
} USBH_PBHCI_TD;

/********************************************************************************************************
 *                                       ENDPOINT DESCRIPTOR DATA TYPE
 *******************************************************************************************************/

struct usbh_pbhci_ed {
  USBH_PBHCI_PIPE      *PipePtr;                                // Ptr to current or last pipe used for this ED.

  USBH_HCD_EP_PARAMS   EP_Params;                               // Struct that contains EP parameters.
  CPU_INT08U           PipeAttr;                                // Bitmap that contains pipe attr to use with this EP.

  CPU_BOOLEAN          Suspended;                               // Flag indicates if processing of ED is suspended.

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U           IntervalIx;                              // Index of interval. Used only with periodic EP.
#endif

  USBH_PBHCI_PID_DATA  CurPid;                                  // Current endpoint data PID.

  USBH_PBHCI_TD        *TD_HeadPtr;                             // Ptr to head of TD list.
  USBH_PBHCI_TD        *TD_TailPtr;                             // Ptr to tail of TD list.

  USBH_DEV_HANDLE      DevHandle;                               // Device   handle.
  USBH_EP_HANDLE       EP_Handle;                               // Endpoint handle.

  struct usbh_pbhci_ed *NextPtr;                                // Ptr to next ED.
};

/********************************************************************************************************
 *                                       PERIODIC NODE DATA TYPE
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
typedef struct usbh_pbhci_periodic_node {
  USBH_PBHCI_ED *ED_HeadPtr;                                    // Ptr to head of ED list.
} USBH_PBHCI_PERIODIC_NODE;
#endif

/********************************************************************************************************
 *                                       PERIODIC BRANCH DATA TYPE
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
typedef struct usbh_pbhci_periodic_branch {
  CPU_INT16U RemBandwidth;                                      // Indicates remaining BW (in bytes) for this branch.
} USBH_PBHCI_PERIODIC_BRANCH;
#endif

/********************************************************************************************************
 *                                           EVENT DATA TYPE
 *******************************************************************************************************/

//                                                                 ---------------- GENERIC EVENT TYPE ----------------
typedef struct usbh_pbhci_event {
  USBH_PBHCI_EVENT_TYPE   Type;                                 // PBHCI event type.
  USBH_PBHCI_HC_DRV       *PBHCI_HC_DrvPtr;                     // Ptr to HC driver structure.

  struct usbh_pbhci_event *NextPtr;                             // Ptr to next pbhci event.
} USBH_PBHCI_EVENT;

//                                                                 ----------------- XFER CMPL EVENT ------------------
typedef struct usbh_pbhci_event_xfer_cmpl {
  USBH_PBHCI_EVENT_TYPE   Type;
  USBH_PBHCI_HC_DRV       *PBHCI_HC_DrvPtr;

  struct usbh_pbhci_event *NextPtr;

  CPU_INT08U              PipeNbr;                              // Pipe nbr.
  CPU_INT08U              Status;                               // Bitmap for status flags.
  CPU_INT32U              XferLen;
  RTOS_ERR                Err;                                  // Error associated to xfer.
} USBH_PBHCI_EVENT_XFER_CMPL;

//                                                                 --------------- START-OF-FRAME EVENT ---------------
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
typedef struct usbh_pbhci_event_sof {
  USBH_PBHCI_EVENT_TYPE   Type;
  USBH_PBHCI_HC_DRV       *PBHCI_HC_DrvPtr;

  struct usbh_pbhci_event *NextPtr;

  CPU_INT16U              FrmNbr;                               // Frame number.
} USBH_PBHCI_EVENT_SOF;
#endif

/********************************************************************************************************
 *                                           ASYNC LIST DATA TYPE
 *******************************************************************************************************/

typedef struct usbh_pbhci_async_list {
  USBH_PBHCI_ED *HeadPtr;                                       // Ptr to head of async ED list.
  USBH_PBHCI_ED *CurPtr;                                        // Ptr to current async ED to schedule.
} USBH_PBHCI_ASYNC_LIST;

/********************************************************************************************************
 *                                           HOST CONTROLLER
 *
 * Note(s) : (1) The PBHCI maintains three (3) lists of free pipe, one for each type (Bulk, Interrupt and
 *               Isochronous). Some pipes may be in more than one list at the same type if they are capable
 *               of multiple transfer type.
 *
 *           (2) The pipe free lists are ordered in an optimized way that will ensure best persistence in
 *               pipe <-> endpoint association. This will limit the number of call to HCD's PipeCfg()
 *               function.
 *
 *               Each list is ordered in the following way:
 *
 *   PipeFreeHeadTblPtr                            PipeFreeCfgTblPtr                       PipeFreeTailTblPtr
 *           |                                             |                                           |
 *           |                                             |                                           |
 *           |  Pipe(s) only            Pipe(s) capable    ->  Pipe(s) only           Pipe(s) capable  |
 *           -> capable of         ->   of multiple       -->  capable of         ->  of multiple     <-
 *               list's xfer type        xfer types             list's xfer type       xfer types
 *
 *           |________________________________________|     |________________________________________|
 *                                   |                                              |
 *               Pipes that were never configured before        Pipes that have already been configured
 *               or that have been de-configured
 *
 *           (3) RH_PortStatusCurTblPtr and RH_StatusCur contain status changes that reflects the current
 *               state of the Root Hub. RH_PortStatusTblPtr and RH_Status contains the status that are
 *               reported to HUB class. The current and reported status are merged together at appropriate
 *               time.
 *******************************************************************************************************/

struct usbh_pbhci_hc {
  USBH_PBHCI_HC_DRV HC_Drv;                                     // HC driver structure.

  MEM_SEG           *MemSegPtr;                                 // Pointer to mem seg used to alloc internal data.
  MEM_SEG           *MemSegBufPtr;                              // Pointer to mem seg used to alloc buffers.

  USBH_HUB_STATUS   *RH_PortStatusTblPtr;                       // Root hub port(s) status table.
  USBH_HUB_STATUS   *RH_PortStatusCurTblPtr;                    // Root hub port(s) current status table. See note (3).
  USBH_HUB_STATUS   RH_Status;                                  // Root hub status.
  USBH_HUB_STATUS   RH_StatusCur;                               // Root hub status current. See note (3).
  CPU_BOOLEAN       RH_IntEn;                                   // Root hub interrupt enable status.
  USBH_DEV_SPD      RH_SpdCur;                                  // Current Root Hub speed.

  KAL_LOCK_HANDLE   LockHandle;                                 // Handle to HC lock.

  //                                                               ------------------ ED & TD POOLS -------------------
  MEM_DYN_POOL      ED_Pool;                                    // Endpoint Descriptors pool.
  MEM_DYN_POOL      TD_Pool;                                    // Transfer Descriptors pool.

  //                                                               ------------------- ASYNC LISTS --------------------
  //                                                               Bulk and Ctrl ED async list.
  USBH_PBHCI_ASYNC_LIST AsyncListTbl[USBH_PBHCI_EP_ASYNC_LIST_QTY];

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  //                                                               ---------------- PERIODIC LIST DATA ----------------
  USBH_PBHCI_PERIODIC_NODE   *PeriodicNodeTblPtr;               // Ptr to periodic binary tree node  table.
  USBH_PBHCI_PERIODIC_BRANCH *PeriodicBranchTblPtr;             // Ptr to periodic branch structures table.
  CPU_INT08U                 BranchQty;                         // Qty of nodes in first level of binary tree.
  CPU_INT08U                 LevelQty;                          // Qty of level in binary tree.

  CPU_INT16U                 PeriodicEP_OpenCnt;                // Cnt of opened periodic EP.
#endif

  //                                                               ------------------ PIPE MGMT DATA ------------------
  CPU_BOOLEAN     CtrlXferInProgress;                           // Flag indicates if ctrl xfer currently in progress.
  USBH_PBHCI_PIPE PipeCtrl;                                     // Dflt ctrl pipe.

  //                                                               Ptr to head of pipe free lists.
  USBH_PBHCI_PIPE *PipeFreeHeadTblPtr[USBH_PBHCI_PIPE_LIST_QTY];

  //                                                               Ptr to jnct between multi & single xfer cfg pipes.
  USBH_PBHCI_PIPE *PipeFreeCfgTblPtr[USBH_PBHCI_PIPE_LIST_QTY];

  //                                                               Ptr to tail of pipe free lists.
  USBH_PBHCI_PIPE *PipeFreeTailTblPtr[USBH_PBHCI_PIPE_LIST_QTY];

  USBH_PBHCI_PIPE **PipePtrTblPtr;                              // Ptr to pipe table.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  USBH_PBHCI_HC   *NextPtr;                                     // Pointer to next PBHCI host controller.
#endif
};

/********************************************************************************************************
 *                                       PBHCI ROOT STRUCTURE DATA TYPE
 *******************************************************************************************************/

typedef struct usbh_pbhci {
  CPU_INT08U                 HC_IxNext;                         // HC next index.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)                   // HC ptr table.
  USBH_PBHCI_HC              *HC_TblPtr[USBH_PBHCI_HC_MAX_QTY_OPTIMIZE_SPD];
#else
  USBH_PBHCI_HC              *HC_HeadPtr;                       // Ptr to head of HC list.
#endif

  KAL_TASK_HANDLE            SchedTaskHandle;                   // Handle to PBHCI task.
  KAL_SEM_HANDLE             SchedSemHandle;                    // Handle to PBHCI task sem.

  USBH_PBHCI_EVENT           *EventQHeadPtr;                    // Ptr to head of PBHCI event Q.
  USBH_PBHCI_EVENT           *EventQTailPtr;                    // Ptr to tail of PBHCI event Q.

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  USBH_PBHCI_EVENT_SOF       *EventSOF_FreeHeadPtr;             // Ptr to head of free PBHCI Start-Of-Frame event pool.
#endif

  USBH_PBHCI_EVENT_XFER_CMPL *EventXferCmplFreeHeadPtr;         // Ptr to head of free PBHCI xfer complete  event pool.
} USBH_PBHCI;

/********************************************************************************************************
 *                                               DEBUG STATS
 *******************************************************************************************************/

#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
typedef struct usbh_dbg_pbhci_sched_event_stats {
  USBH_PBHCI_EVENT_TYPE Type;
  CPU_INT32U            Cnt;
} USBH_DBG_PBHCI_SCHED_EVENT_STATS;

typedef enum usbh_dbg_pbhci_event_port_type {
  USBH_DBG_PBHCI_EVENT_PORT_CONN = 0u,
  USBH_DBG_PBHCI_EVENT_PORT_DISCONN = 1u,
  USBH_DBG_PBHCI_EVENT_PORT_OVERCURRENT = 2u,
  USBH_DBG_PBHCI_EVENT_PORT_REMOTE_WAKE_UP = 3u
}USBH_DBG_PBHCI_EVENT_PORT_TYPE;

typedef struct usbh_dbg_pbhci_event_port_stats {
  USBH_DBG_PBHCI_EVENT_PORT_TYPE Type;
  CPU_INT32U                     Cnt;
} USBH_DBG_PBHCI_EVENT_PORT_STATS;

typedef enum usbh_dbg_pbhci_urb_done_type {
  USBH_DBG_PBHCI_URB_DONE_ERR = 0u,

  USBH_DBG_PBHCI_URB_DONE_IN_ERR = 1u,
  USBH_DBG_PBHCI_URB_DONE_IN_ISOC = 2u,
  USBH_DBG_PBHCI_URB_DONE_IN_XFER_CMPL = 3u,
  USBH_DBG_PBHCI_URB_DONE_IN_CTRL_BULK = 4u,

  USBH_DBG_PBHCI_URB_DONE_OUT_ISOC = 5u,
  USBH_DBG_PBHCI_URB_DONE_OUT_BUF_LEN = 6u,
  USBH_DBG_PBHCI_URB_DONE_OUT_CTRL_BULK = 7u,

  USBH_DBG_PBHCI_URB_DONE_NEXT_BULK_ASSOC = 8u,
  USBH_DBG_PBHCI_URB_DONE_NEXT_BULK_XFER = 9u,

  USBH_DBG_PBHCI_URB_DONE_NEXT_CTRL_CFG = 10u,
  USBH_DBG_PBHCI_URB_DONE_NEXT_CTRL_XFER = 11u,

  USBH_DBG_PBHCI_URB_DONE_PIPE_ALLOC_MAX = 12u,
  USBH_DBG_PBHCI_URB_DONE_PIPE_ASSOC = 13u,
  USBH_DBG_PBHCI_URB_DONE_PIPE_XFER = 14u,

  USBH_DBG_PBHCI_URB_DONE_PIPE_ISOC = 15u
} USBH_DBG_PBHCI_URB_DONE_TYPE;

typedef struct usbh_dbg_pbhci_urb_done_stats {
  USBH_DBG_PBHCI_URB_DONE_TYPE Type;
  CPU_INT32U                   Cnt;
} USBH_DBG_PBHCI_URB_DONE_STATS;

typedef enum usbh_dbg_pbhci_xfer_cmpl_type {
  USBH_DBG_PBHCI_XFER_CMPL_TD_ERR = 0u,
  USBH_DBG_PBHCI_XFER_CMPL_IN_CTRL_BULK = 1u,
  USBH_DBG_PBHCI_XFER_CMPL_IN_ELSE = 2u,
  USBH_DBG_PBHCI_XFER_CMPL_OUT_CTRL_BULK = 3u
} USBH_DBG_PBHCI_XFER_CMPL_TYPE;

typedef struct usbh_dbg_pbhci_xfer_cmpl_stats {
  USBH_DBG_PBHCI_XFER_CMPL_TYPE Type;
  CPU_INT32U                    Cnt;
} USBH_DBG_PBHCI_XFER_CMPL_STATS;

typedef struct usbh_dbg_pbhci_ep_stats {
  //                                                               Xfer Cmpl-related debug stats
  CPU_INT32U                     XferCmplProcessTot;
  CPU_INT32U                     EventPipeRxCmpl;
  CPU_INT32U                     EventPipeTxCmpl;
  //                                                               Xfer Cmpl exit without an USBH_URB_Done() Call.
  USBH_DBG_PBHCI_XFER_CMPL_STATS XferCmplTbl[USBH_DBG_PBHCI_XFER_CMPL_QTY];
  //                                                               Xfer Cmpl exit with    an USBH_URB_Done() Call.
  USBH_DBG_PBHCI_URB_DONE_STATS  UrbDoneTbl[USBH_DBG_PBHCI_URB_DONE_QTY];

  //                                                               HCD API-related debug stats
  CPU_INT32U                     HcdEpOpenEnter;
  CPU_INT32U                     HcdEpOpenError;
  CPU_INT32U                     HcdEpOpenSuccess;
  CPU_INT32U                     HcdEpClose;
  CPU_INT32U                     HcdEpSuspendEnter;
  CPU_INT32U                     HcdEpSuspendError;
  CPU_INT32U                     HcdEpSuspendSuccess;
  CPU_INT32U                     HcdEpHaltClr;
  CPU_INT32U                     HcdUrbSubmitEnter;
  CPU_INT32U                     HcdUrbSubmitError;
  CPU_INT32U                     HcdUrbSubmitSuccess;
  CPU_INT32U                     HcdUrbComplete;
  CPU_INT32U                     HcdUrbAbort;

  //                                                               Pipe-related debug stats
  CPU_INT32U                     PipeAllocEnter;
  CPU_INT32U                     PipeAllocError;
  CPU_INT32U                     PipeAllocSuccess;
  CPU_INT32U                     PipeAssocEnter;
  CPU_INT32U                     PipeAssocError;
  CPU_INT32U                     PipeAssocSuccess;
  CPU_INT32U                     PipeFree;
  CPU_INT32U                     PipeFreeUnCfg;
  CPU_INT32U                     PipeAbortFree;
  CPU_INT32U                     PipeAbortNoFree;
  CPU_INT32U                     PipeCfg;
  CPU_INT32U                     PipeWr;
  CPU_INT32U                     PipeTxError;
  CPU_INT32U                     PipeTxStart;
  CPU_INT32U                     PipeRxStart;
  CPU_INT32U                     PipeRd;
} USBH_DBG_PBHCI_EP_STATS;

typedef struct usbh_dbg_pbhci_dev_stats {
  USBH_DBG_PBHCI_EP_STATS EpTbl[USBH_DEV_NBR_EP];

  CPU_INT32U              HcdDevConn;
  CPU_INT32U              HcdDevDisconn;
} USBH_DBG_PBHCI_DEV_STATS;

typedef struct usbh_dbg_pbhci_stats {
  USBH_DBG_PBHCI_DEV_STATS         DevTbl[USBH_DBG_PBHCI_DEV_QTY];

  //                                                               Event-related debug stats
  USBH_DBG_PBHCI_SCHED_EVENT_STATS SchedEventTbl[USBH_DBG_PBHCI_SCHED_EVENT_QTY];
  CPU_INT32U                       SchedEventTot;
  USBH_DBG_PBHCI_EVENT_PORT_STATS  EventPortTbl[USBH_DBG_PBHCI_EVENT_PORT_QTY];
  CPU_INT32U                       EventPipeRxCmpl;
  CPU_INT32U                       EventPipeTxCmpl;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT32U                       EventSOFEnter;
  CPU_INT32U                       EventSOFNone;
  CPU_INT32U                       EventSOFError;
  CPU_INT32U                       EventSOFSuccess;
#endif
  //                                                               Xfer Cmpl-related debug stats
  CPU_INT32U                       XferCmplProcessTot;
  CPU_INT32U                       XferCmplEdNotFound;

  //                                                               HCD API-related debug stats
  CPU_INT32U                       HcdStart;
  CPU_INT32U                       HcdStop;
  CPU_INT32U                       HcdSuspend;
  CPU_INT32U                       HcdResume;
  CPU_INT32U                       HcdFrameNbrGet;
  CPU_INT32U                       HcdPortResetSet;
  CPU_INT32U                       HcdPortResetClr;
  CPU_INT32U                       HcdPortSuspendSet;
  CPU_INT32U                       HcdPortSuspendClr;
  CPU_INT32U                       HcdSofIntEn;
  CPU_INT32U                       HcdSofIntDis;
} USBH_DBG_PBHCI_STATS;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_PBHCI_INIT_CFG USBH_PBHCI_InitCfg = {
  .SOF_EventQty = 10u,
  .MemSegPtr = DEF_NULL,

  .SchedTaskStkSizeElements = 512u,
  .SchedTaskStkPtr = DEF_NULL
};
#else
extern USBH_PBHCI_INIT_CFG USBH_PBHCI_InitCfg;
#endif

static USBH_PBHCI *USBH_PBHCI_Ptr = DEF_NULL;

#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
static USBH_DBG_PBHCI_STATS USBH_DbgPBHCI_Stats[USBH_DBG_PBHCI_HOST_QTY][USBH_DBG_PBHCI_HC_QTY];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_PBHCI_INIT_CFG USBH_PBHCI_InitCfgDflt = {
  .SOF_EventQty = 10u,
  .MemSegPtr = DEF_NULL,

  .SchedTaskStkSizeElements = 512u,
  .SchedTaskStkPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 *                                           DRIVER API FUNCTIONS
 *******************************************************************************************************/

static void USBH_HCD_Init(USBH_HC_DRV     *p_hc_drv,
                          MEM_SEG         *p_mem_seg,
                          MEM_SEG         *p_mem_seg_buf,
                          USBH_HC_CFG_EXT *p_hc_cfg_ext,
                          RTOS_ERR        *p_err);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_UnInit(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err);
#endif

static void USBH_HCD_Start(USBH_HC_DRV *p_hc_drv,
                           RTOS_ERR    *p_err);

static void USBH_HCD_Stop(USBH_HC_DRV *p_hc_drv,
                          RTOS_ERR    *p_err);

static void USBH_HCD_Suspend(USBH_HC_DRV *p_hc_drv,
                             RTOS_ERR    *p_err);

static void USBH_HCD_Resume(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err);

static CPU_INT16U USBH_HCD_FrameNbrGet(USBH_HC_DRV *p_hc_drv);

static void USBH_HCD_EP_Open(USBH_HC_DRV        *p_hc_drv,
                             USBH_DEV_HANDLE    dev_handle,
                             USBH_EP_HANDLE     ep_handle,
                             USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                             void               **pp_hcd_ep_data,
                             RTOS_ERR           *p_err);

static void USBH_HCD_EP_Close(USBH_HC_DRV *p_hc_drv,
                              void        *p_hcd_ep_data,
                              RTOS_ERR    *p_err);

static void USBH_HCD_EP_Suspend(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN suspend,
                                RTOS_ERR    *p_err);

static void USBH_HCD_EP_HaltClr(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN data_toggle_clr,
                                RTOS_ERR    *p_err);

static void USBH_HCD_URB_Submit(USBH_HC_DRV         *p_hc_drv,
                                void                *p_hcd_ep_data,
                                void                **pp_hcd_urb_data,
                                USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                RTOS_ERR            *p_err);

static CPU_INT32U USBH_HCD_URB_Complete(USBH_HC_DRV         *p_hc_drv,
                                        void                *p_hcd_ep_data,
                                        void                *p_hcd_urb_data,
                                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                        RTOS_ERR            *p_err);

static void USBH_HCD_URB_Abort(USBH_HC_DRV         *p_hc_drv,
                               void                *p_hcd_ep_data,
                               void                *p_hcd_urb_data,
                               USBH_HCD_URB_PARAMS *p_hcd_urb_params);

/********************************************************************************************************
 *                                       ROOT HUB API FUNCTIONS
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_HCD_RH_InfosGet(USBH_HC_DRV        *p_hc_drv,
                                        USBH_HUB_ROOT_INFO *p_rh_info);

static CPU_BOOLEAN USBH_HCD_RH_PortStatusGet(USBH_HC_DRV     *p_hc_drv,
                                             CPU_INT08U      port_nbr,
                                             USBH_HUB_STATUS *p_port_status);

static CPU_BOOLEAN USBH_HCD_RH_PortReq(USBH_HC_DRV *p_hc_drv,
                                       CPU_INT08U  port_nbr,
                                       CPU_INT08U  req,
                                       CPU_INT16U  feature);

static CPU_BOOLEAN USBH_HCD_RH_IntEn(USBH_HC_DRV *p_hc_drv,
                                     CPU_BOOLEAN en);

/********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 *******************************************************************************************************/

static void USBH_PBHCI_EventPipeXferCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                         CPU_INT08U        pipe_nbr,
                                         CPU_INT32U        xfer_len,
                                         CPU_INT08U        status,
                                         RTOS_ERR          err);

static void USBH_PBHCI_SchedTaskHandler(void *p_arg);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_PBHCI_PeriodicSched(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                     CPU_INT16U        frm_nbr);
#endif

static void USBH_PBHCI_XferCmplProcess(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       CPU_INT08U        pipe_nbr,
                                       CPU_INT32U        xfer_len,
                                       CPU_INT08U        status,
                                       RTOS_ERR          err);

static void USBH_PBHCI_AsyncListED_Insert(USBH_PBHCI_ASYNC_LIST *p_async_list,
                                          USBH_PBHCI_ED         *p_ed);

static void USBH_PBHCI_AsyncListED_Remove(USBH_PBHCI_ASYNC_LIST *p_async_list,
                                          USBH_PBHCI_ED         *p_ed);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT16U USBH_PBHCI_PeriodicListED_Insert(USBH_PBHCI_HC *p_pbhci_hc,
                                                   USBH_PBHCI_ED *p_ed,
                                                   RTOS_ERR      *p_err);

static void USBH_PBHCI_PeriodicListED_Remove(USBH_PBHCI_HC *p_pbhci_hc,
                                             USBH_PBHCI_ED *p_ed);

static CPU_INT08U USBH_PBHCI_BTreeIxCompute(CPU_INT08U interval,
                                            CPU_INT08U interval_ix);

static CPU_INT16U USBH_PBHCI_ED_BW_Get(USBH_HCD_EP_PARAMS *p_ep_params);
#endif

static void USBH_PBHCI_NextCtrlSubmit(USBH_PBHCI_HC *p_pbhci_hc);

static void USBH_PBHCI_NextBulkSubmit(USBH_PBHCI_HC *p_pbhci_hc);

static USBH_PBHCI_TD *USBH_PBHCI_TdNextAsyncFind(USBH_PBHCI_ASYNC_LIST *p_async_list);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_PBHCI_IntrTD_Submit(USBH_PBHCI_HC *p_pbhci_hc,
                                     USBH_PBHCI_TD *p_td);

#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
static void USBH_PBHCI_IsocTD_Submit(USBH_PBHCI_HC *p_pbhci_hc,
                                     USBH_PBHCI_TD *p_td);
#endif
#endif

static void USBH_PBHCI_ED_TD_Insert(USBH_PBHCI_ED *p_ed,
                                    USBH_PBHCI_TD *p_td);

static void USBH_PBHCI_ED_TD_Pop(USBH_PBHCI_ED *p_ed);

static USBH_PBHCI_PIPE *USBH_PBHCI_PipeAlloc(USBH_PBHCI_HC *p_pbhci_hc,
                                             USBH_PBHCI_ED *p_ed);

static void USBH_PBHCI_PipeListRemove(USBH_PBHCI_HC   *p_pbhci_hc,
                                      USBH_PBHCI_PIPE *p_pipe);

static void USBH_PBHCI_PipeAssoc(USBH_PBHCI_HC   *p_pbhci_hc,
                                 USBH_PBHCI_PIPE *p_pipe,
                                 USBH_PBHCI_ED   *p_ed,
                                 RTOS_ERR        *p_err);

static void USBH_PBHCI_PipeFree(USBH_PBHCI_HC   *p_pbhci_hc,
                                USBH_PBHCI_PIPE *p_pipe);

static void USBH_PBHCI_PipeFreeUncfg(USBH_PBHCI_HC   *p_pbhci_hc,
                                     USBH_PBHCI_PIPE *p_pipe);

static void USBH_PBHCI_PipeXfer(USBH_PBHCI_HC   *p_pbhci_hc,
                                USBH_PBHCI_PIPE *p_pipe,
                                USBH_PBHCI_TD   *p_td,
                                RTOS_ERR        *p_err);

static void USBH_PBHCI_PipeTx(USBH_PBHCI_HC   *p_pbhci_hc,
                              USBH_PBHCI_PIPE *p_pipe,
                              USBH_PBHCI_TD   *p_td,
                              RTOS_ERR        *p_err);

static void USBH_PBHCI_PipeRxStart(USBH_PBHCI_HC   *p_pbhci_hc,
                                   USBH_PBHCI_PIPE *p_pipe,
                                   USBH_PBHCI_TD   *p_td,
                                   RTOS_ERR        *p_err);

static void USBH_PBHCI_PipeAbortAndFree(USBH_PBHCI_HC   *p_pbhci_hc,
                                        USBH_PBHCI_PIPE *p_pipe,
                                        RTOS_ERR        *p_err);

static USBH_PBHCI_EVENT *USBH_PBHCI_EventGet(USBH_PBHCI_EVENT **pp_free_list_head);

static void USBH_PBHCI_EventFree(USBH_PBHCI_EVENT **pp_free_list_head,
                                 USBH_PBHCI_EVENT *p_pbhci_event);

static void USBH_PBHCI_EventQAdd(USBH_PBHCI_EVENT *p_pbhci_event);

static void USBH_PBHCI_HC_Lock(USBH_PBHCI_HC *p_pbhci_hc);

static void USBH_PBHCI_HC_Unlock(USBH_PBHCI_HC *p_pbhci_hc);

static USBH_PBHCI_HC *USBH_PBHCI_HC_PtrGet(CPU_INT08U pbhci_hc_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                   INITIALIZED GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- PBHCI DRIVER API -----------------
USBH_HC_DRV_API USBH_HCD_API_PBHCI = {
  USBH_HCD_Init,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_HCD_UnInit,
#else
  DEF_NULL,
#endif
  USBH_HCD_Start,
  USBH_HCD_Stop,
  USBH_HCD_Suspend,
  USBH_HCD_Resume,
  USBH_HCD_FrameNbrGet,

  USBH_HCD_EP_Open,
  USBH_HCD_EP_Close,
  USBH_HCD_EP_Suspend,
  USBH_HCD_EP_HaltClr,

  USBH_HCD_URB_Submit,
  USBH_HCD_URB_Complete,
  USBH_HCD_URB_Abort
};

//                                                                 ------------ PBHCI ROOT HUB DRIVER API -------------
USBH_HC_RH_API USBH_HCD_API_RH_PBHCI = {
  USBH_HCD_RH_InfosGet,
  USBH_HCD_RH_PortStatusGet,

  USBH_HCD_RH_PortReq,

  USBH_HCD_RH_IntEn
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_PBHCI_ConfigureSOF_EventQty()
 *
 * @brief    Configures the quantity of Start-Of-Frame events.
 *
 * @param    sof_event_qty   Quantity of start-of-frame events.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host PBHCI module is initialized via the
 *               USBH_PBHCI Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_PBHCI_ConfigureSOF_EventQty(CPU_INT16U sof_event_qty)
{
  RTOS_ASSERT_CRITICAL((USBH_PBHCI_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_PBHCI_InitCfg.SOF_EventQty = sof_event_qty;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_PBHCI_ConfigureSchedTaskStk()
 *
 * @brief    Configures the USB host PBHCI scheduler task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be
 *                               allocated from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host PBHCI module is initialized via the
 *               USBH_PBHCI Init() function.
 *
 * @note     (3) In order to change the priority of the USB host PBHCI scheduler task, use the function
 *               USBH_PBHCI_SchedTaskPrioSet().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_PBHCI_ConfigureSchedTaskStk(CPU_INT32U stk_size_elements,
                                      void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((USBH_PBHCI_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_PBHCI_InitCfg.SchedTaskStkSizeElements = stk_size_elements;
  USBH_PBHCI_InitCfg.SchedTaskStkPtr = p_stk;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_PBHCI_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg   Pointer to memory segment to use when allocating control data.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the PBHCI module is initialized via the
 *               USBH_PBHCI_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_PBHCI_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((USBH_PBHCI_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_PBHCI_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_PBHCI_UnInit()
 *
 * @brief    Un-initializes Pipe-Based Host Controller Interface (PBHCI).
 *
 * @param    p_err   Pointer to the variable that will receive this return error code
 *                   from this function :
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_PBHCI_UnInit(RTOS_ERR *p_err)
{
  KAL_TaskDel(USBH_PBHCI_Ptr->SchedTaskHandle);
  KAL_SemDel(USBH_PBHCI_Ptr->SchedSemHandle);

  USBH_PBHCI_Ptr = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_PBHCI_Init()
 *
 * @brief    Initializes the Pipe-Based Host Controller Interface (PBHCI).
 *
 * @param    p_err   Pointer to the variable that will receive one of these return error codes
 *                   from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_PBHCI_Init(RTOS_ERR *p_err)
{
  USBH_PBHCI *p_pbhci;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT16U event_cnt;
#endif
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Alloc PBHCI root struct.
  p_pbhci = (USBH_PBHCI *)Mem_SegAlloc("USBH - PBHCI Root struct",
                                       USBH_PBHCI_InitCfg.MemSegPtr,
                                       sizeof(USBH_PBHCI),
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_pbhci->HC_HeadPtr = DEF_NULL;
#endif
  p_pbhci->HC_IxNext = 0u;
  p_pbhci->EventQHeadPtr = DEF_NULL;
  p_pbhci->EventQTailPtr = DEF_NULL;
  p_pbhci->SchedTaskHandle = KAL_TaskHandleNull;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  //                                                               Alloc and init SOF events.
  p_pbhci->EventSOF_FreeHeadPtr = (USBH_PBHCI_EVENT_SOF *)Mem_SegAlloc("USBH - PBHCI HC ptr table",
                                                                       USBH_PBHCI_InitCfg.MemSegPtr,
                                                                       sizeof(USBH_PBHCI_EVENT_SOF) * USBH_PBHCI_InitCfg.SOF_EventQty,
                                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (event_cnt = 0u; event_cnt < (USBH_PBHCI_InitCfg.SOF_EventQty - 1u); event_cnt++) {
    p_pbhci->EventSOF_FreeHeadPtr[event_cnt].NextPtr = (USBH_PBHCI_EVENT *)&p_pbhci->EventSOF_FreeHeadPtr[event_cnt + 1u];
    p_pbhci->EventSOF_FreeHeadPtr[event_cnt].Type = USBH_PBHCI_EVENT_TYPE_SOF;
  }
  p_pbhci->EventSOF_FreeHeadPtr[event_cnt].NextPtr = DEF_NULL;
  p_pbhci->EventSOF_FreeHeadPtr[event_cnt].Type = USBH_PBHCI_EVENT_TYPE_SOF;
#endif

  p_pbhci->EventXferCmplFreeHeadPtr = DEF_NULL;                 // Event xfer cmpl allocated when HC added.

  CORE_ENTER_ATOMIC();
  USBH_PBHCI_Ptr = p_pbhci;
  CORE_EXIT_ATOMIC();

  //                                                               ---------- CREATE REQUIRED KERNEL OBJECTS ----------
  //                                                               Create sched semaphore.
  USBH_PBHCI_Ptr->SchedSemHandle = KAL_SemCreate("USBH - PBHCI sched sem",
                                                 DEF_NULL,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create sched task.
  USBH_PBHCI_Ptr->SchedTaskHandle = KAL_TaskAlloc("USBH - PBHCI sched task",
                                                  (CPU_STK *)USBH_PBHCI_InitCfg.SchedTaskStkPtr,
                                                  USBH_PBHCI_InitCfg.SchedTaskStkSizeElements,
                                                  DEF_NULL,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(USBH_PBHCI_Ptr->SchedTaskHandle,
                 USBH_PBHCI_SchedTaskHandler,
                 DEF_NULL,
                 USBH_PBHCI_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  LOG_VRB(("PBHCI init OK."));
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_SchedTaskPrioSet()
 *
 * @brief    Assigns a new priority to the USB host PBHCI scheduler task.
 *
 * @param    prio    New priority of the the scheduler task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void USBH_PBHCI_SchedTaskPrioSet(CPU_INT08U prio,
                                 RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE sched_task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_PBHCI_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  sched_task_handle = USBH_PBHCI_Ptr->SchedTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(sched_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPortConn()
 *
 * @brief    Port connection event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    port_nbr        Hub port number. Must be > 0.
 *
 * @note     (1) In a typical ISR, it is important to treat disconnection interrupts and call
 *               USBH_PBHCI_EventPortDisconn() before treating connection interrupts and call
 *               USBH_PBHCI_EventPortConn(). Otherwise, if both a connection and a disconnection
 *               interrupts are treated within a same ISR, the call to USBH_PBHCI_EventPortDisconn()
 *               would cancel the call to USBH_PBHCI_EventPortConn().
 *******************************************************************************************************/
void USBH_PBHCI_EventPortConn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              CPU_INT08U        port_nbr)
{
  USBH_PBHCI_HC   *p_pbhci_hc;
  USBH_HUB_STATUS *p_rh_port_status;
  CORE_DECLARE_IRQ_STATE;

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  p_rh_port_status = &p_pbhci_hc->RH_PortStatusCurTblPtr[port_nbr - 1u];

  CORE_ENTER_ATOMIC();                                          // Update port status.
  DEF_BIT_SET(p_rh_port_status->Status,
              USBH_HUB_STATUS_PORT_CONN);

  DEF_BIT_SET(p_rh_port_status->Chng,
              USBH_HUB_STATUS_C_PORT_CONN);

  if (p_pbhci_hc->RH_IntEn == DEF_ENABLED) {
    p_pbhci_hc->RH_IntEn = DEF_DISABLED;
    CORE_EXIT_ATOMIC();

    //                                                             Notify HUB class of RH event.
    USBH_HUB_RootEvent(p_pbhci_hc_drv->HostNbr,
                       p_pbhci_hc_drv->HC_Nbr,
                       DEF_BIT(port_nbr));
  } else {
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("Dev connect on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " port #", (u)port_nbr));

  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_DBG_PBHCI_EVENT_PORT_CONN);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPortDisconn()
 *
 * @brief    Port disconnection event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    port_nbr        Hub port number. Must be > 0.
 *
 * @note     (1) In a typical ISR, it is important to treat disconnection interrupts and call
 *               USBH_PBHCI_EventPortDisconn() before treating connection interrupts and call
 *               USBH_PBHCI_EventPortConn(). Otherwise, if both a connection and a disconnection
 *               interrupts are treated within a same ISR, the call to USBH_PBHCI_EventPortDisconn()
 *               would cancel the call to USBH_PBHCI_EventPortConn().
 *******************************************************************************************************/
void USBH_PBHCI_EventPortDisconn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                 CPU_INT08U        port_nbr)
{
  USBH_PBHCI_HC   *p_pbhci_hc;
  USBH_HUB_STATUS *p_rh_port_status;
  CORE_DECLARE_IRQ_STATE;

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  p_rh_port_status = &p_pbhci_hc->RH_PortStatusCurTblPtr[port_nbr - 1u];

  CORE_ENTER_ATOMIC();                                          // Update port status.
  DEF_BIT_CLR(p_rh_port_status->Status,
              (CPU_INT16U)USBH_HUB_STATUS_PORT_CONN);
  DEF_BIT_SET(p_rh_port_status->Chng,
              USBH_HUB_STATUS_C_PORT_CONN);

  if (p_pbhci_hc->RH_IntEn == DEF_ENABLED) {
    p_pbhci_hc->RH_IntEn = DEF_DISABLED;
    CORE_EXIT_ATOMIC();

    //                                                             Notify HUB class of RH event.
    USBH_HUB_RootEvent(p_pbhci_hc_drv->HostNbr,
                       p_pbhci_hc_drv->HC_Nbr,
                       DEF_BIT(port_nbr));
  } else {
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("Dev disconnect on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " port #", (u)port_nbr));

  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_DBG_PBHCI_EVENT_PORT_DISCONN);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPortOvercurrent()
 *
 * @brief    Port overcurrent event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    port_nbr        Hub port number. 0 means overcurrent on hub.
 *
 * @param    set             Indicates if overcurrent situation is set or cleared.
 *                           DEF_SET     Overcurrent situation set.
 *                           DEF_CLR     Overcurrent situation cleared.
 *
 * @note     (1) This function must be called by the PBHCD once an overcurrent situation occurs.
 *               It MUST be called again when the overcurrent situation disappear.
 *******************************************************************************************************/
void USBH_PBHCI_EventPortOvercurrent(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                     CPU_INT08U        port_nbr,
                                     CPU_BOOLEAN       set)
{
  USBH_PBHCI_HC   *p_pbhci_hc;
  USBH_HUB_STATUS *p_rh_port_status;
  CORE_DECLARE_IRQ_STATE;

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);

  CORE_ENTER_ATOMIC();                                          // Update port status.

  if (port_nbr > 0u) {
    p_rh_port_status = &p_pbhci_hc->RH_PortStatusCurTblPtr[port_nbr - 1u];

    if (set == DEF_SET) {
      DEF_BIT_SET(p_rh_port_status->Status,
                  USBH_HUB_STATUS_PORT_OVER_CUR);
    } else {
      DEF_BIT_CLR(p_rh_port_status->Status,
                  (CPU_INT16U)USBH_HUB_STATUS_PORT_OVER_CUR);
    }

    DEF_BIT_SET(p_rh_port_status->Chng,
                USBH_HUB_STATUS_C_PORT_OVER_CUR);
  } else {
    p_rh_port_status = &p_pbhci_hc->RH_StatusCur;

    if (set == DEF_SET) {
      DEF_BIT_SET(p_rh_port_status->Status,
                  USBH_HUB_STATUS_OVER_CUR);
    } else {
      DEF_BIT_CLR(p_rh_port_status->Status,
                  (CPU_INT16U)USBH_HUB_STATUS_OVER_CUR);
    }

    DEF_BIT_SET(p_rh_port_status->Chng,
                USBH_HUB_STATUS_C_OVER_CUR);
  }

  if (p_pbhci_hc->RH_IntEn == DEF_ENABLED) {
    p_pbhci_hc->RH_IntEn = DEF_DISABLED;
    CORE_EXIT_ATOMIC();

    //                                                             Notify HUB class of RH event.
    USBH_HUB_RootEvent(p_pbhci_hc_drv->HostNbr,
                       p_pbhci_hc_drv->HC_Nbr,
                       DEF_BIT(port_nbr));
  } else {
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("Port overcurrent on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " port #", (u)port_nbr));

  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_DBG_PBHCI_EVENT_PORT_OVERCURRENT);
}

/****************************************************************************************************//**
 *                                   USBH_PBHCI_EventPortRemoteWakeup()
 *
 * @brief    Remote wakeup event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    port_nbr        Hub port number. Must be > 0.
 *
 * @note     (1) Feature not yet supported. For future considerations.
 *******************************************************************************************************/
void USBH_PBHCI_EventPortRemoteWakeup(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                      CPU_INT08U        port_nbr)
{
  (void)&p_pbhci_hc_drv;
  (void)&port_nbr;

  USBH_DBG_PBHCI_STATS_EVENT_PORT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_DBG_PBHCI_EVENT_PORT_REMOTE_WAKE_UP);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPipeRxCmpl()
 *
 * @brief    Receive complete event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    status          Bitmap indicating some status flags.
 *
 * @param    err             Transfer error code. RTOS_ERR_NONE when transfer was successful.
 *******************************************************************************************************/
void USBH_PBHCI_EventPipeRxCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                CPU_INT08U        pipe_nbr,
                                CPU_INT08U        status,
                                RTOS_ERR          err)
{
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  USBH_PBHCI_HC *p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  USBH_PBHCI_ED *p_ed = p_pbhci_hc->PipePtrTblPtr[pipe_nbr]->CurED_Ptr;
  CPU_INT08U    dev_nbr;
  CPU_INT08U    ep_phy_nbr;

  if (p_ed != DEF_NULL) {
    dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
    ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, dev_nbr, ep_phy_nbr, EventPipeRxCmpl);
  }
#endif

  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventPipeRxCmpl);

  USBH_PBHCI_EventPipeXferCmpl(p_pbhci_hc_drv,
                               pipe_nbr,
                               USBH_PBHCI_XFER_LEN_ALL,
                               status,
                               err);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPipeTxCmpl()
 *
 * @brief    Transmit complete event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    xfer_len        Transmit length in bytes. USBH_PBHCI_XFER_LEN_ALL if length returned
 *                           in PipeTxStart() function is valid.
 *
 * @param    status          Bitmap indicating some status flags.
 *
 * @param    err             Transfer error code. RTOS_ERR_NONE when transfer was successful.
 *******************************************************************************************************/
void USBH_PBHCI_EventPipeTxCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                CPU_INT08U        pipe_nbr,
                                CPU_INT32U        xfer_len,
                                CPU_INT08U        status,
                                RTOS_ERR          err)
{
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  USBH_PBHCI_HC *p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  USBH_PBHCI_ED *p_ed = p_pbhci_hc->PipePtrTblPtr[pipe_nbr]->CurED_Ptr;
  CPU_INT08U    dev_nbr;
  CPU_INT08U    ep_phy_nbr;

  if (p_ed != DEF_NULL) {
    dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
    ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, dev_nbr, ep_phy_nbr, EventPipeTxCmpl);
  }
#endif

  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventPipeTxCmpl);

  USBH_PBHCI_EventPipeXferCmpl(p_pbhci_hc_drv,
                               pipe_nbr,
                               xfer_len,
                               status,
                               err);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_EventSOF()
 *
 * @brief    Signals a Start-Of-Frame event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    frm_nbr         Frame number (no micro frame).
 *
 * @note     (1) frm_nbr should only contain the current frame number. uFrame number doesn't need to
 *               be appended.
 *
 * @note     (2) Should normally be called from the SOF ISR. It is not necessary to call this function
 *               until the SOF_IntEn() function is called to enable the SOF interruptions.
 *******************************************************************************************************/
void USBH_PBHCI_EventSOF(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                         CPU_INT16U        frm_nbr)
{
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT16U                 adj_frm_nbr;
  USBH_PBHCI_EVENT_SOF       *p_pbhci_event_sof;
  USBH_PBHCI_HC              *p_pbhci_hc;
  USBH_PBHCI_PERIODIC_BRANCH *p_branch;
  CORE_DECLARE_IRQ_STATE;

  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventSOFEnter);

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  adj_frm_nbr = frm_nbr & DEF_BIT_FIELD(p_pbhci_hc->LevelQty - 1u, 0u);
  p_branch = &p_pbhci_hc->PeriodicBranchTblPtr[adj_frm_nbr];

  //                                                               Ensure at least one ED is present in current branch.
  CORE_ENTER_ATOMIC();
  if (p_branch->RemBandwidth >= USBH_HC_MAX_PERIODIC_BW_GET(p_pbhci_hc->RH_SpdCur)) {
    CORE_EXIT_ATOMIC();
    USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventSOFNone);
    return;
  }
  CORE_EXIT_ATOMIC();

  p_pbhci_event_sof = (USBH_PBHCI_EVENT_SOF *)USBH_PBHCI_EventGet((USBH_PBHCI_EVENT **)&USBH_PBHCI_Ptr->EventSOF_FreeHeadPtr);

  //                                                               If at least one ED present in branch, treat it.
  if (p_pbhci_event_sof != DEF_NULL) {
    RTOS_ERR err;

    p_pbhci_event_sof->PBHCI_HC_DrvPtr = p_pbhci_hc_drv;
    p_pbhci_event_sof->FrmNbr = adj_frm_nbr;

    USBH_PBHCI_EventQAdd((USBH_PBHCI_EVENT *)p_pbhci_event_sof);

    KAL_SemPost(USBH_PBHCI_Ptr->SchedSemHandle,
                KAL_OPT_POST_NONE,
                &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Posting PBHCI event -> ", RTOS_ERR_LOG_ARG_GET(err)));
            #if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
      USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventSOFError);
    } else {
      USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventSOFSuccess);
            #endif
    }
  } else {
    LOG_ERR(("Allocating PBHCI event for SOF event. SOF LOST! -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_ALLOC)));
    USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, EventSOFError);
  }
#else
  (void)&p_pbhci_hc_drv;
  (void)&frm_nbr;
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_PBHCI_HC_AddInternal()
 *
 * @brief    Adds a Host Controller to the Pipe-Based Host Controller Interface (PBHCI).
 *
 * @param    name            Name of the USB controller.
 *
 * @param    host_nbr        Host number.
 *
 * @param    p_hc_hw_info    Pointer to the Host Controller hardware information structure.
 *
 * @param    p_hc_cfg_ext    Pointer to the structure that contains the extended host controller
 *                           configuration.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_NOT_AVAIL
 *
 * @note     (1) The desired maximum precision for periodic transfers (interrupt and isochronous) can
 *               be specified with this parameter. This represents the maximum interval that can be
 *               supported. Any larger interval specified by devices will be polled using this value.
 *               Possible values are 1, 2, 4, 8, 16, 32, 64, and 128.
 *
 * @note     (2) [INTERNAL] This function must first register the Host Controller to the core. It is
 *               necessary to do this first since it is the USBH_HCD_Init() function that will
 *               allocate the Host Controller structure. This function cannot allocate the Host
 *               Controller structure because it must know the hc_nbr first, which is assigned by the
 *               core when the USBH_HC_Add() function is called.
 *******************************************************************************************************/
CPU_INT08U USBH_PBHCI_HC_AddInternal(const CPU_CHAR        *name,
                                     CPU_INT08U            host_nbr,
                                     USBH_PBHCI_HC_HW_INFO *p_hc_hw_info,
                                     const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                                     RTOS_ERR              *p_err)
{
  CPU_INT08U cnt;
  CPU_INT08U pipe_nbr_high;
  CPU_INT08U pipe_qty;
  CPU_INT08U hc_nbr;
  CPU_INT08U pipe_attrib_temp;
  CPU_INT08U port_ix;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U periodic_xfer_precision;
#endif
  USBH_PBHCI_HC              *p_pbhci_hc = DEF_NULL;
  USBH_PBHCI_HC_DRV          *p_pbhci_hc_drv;
  USBH_PBHCI_ASYNC_LIST      *p_async_list;
  USBH_PBHCI_PIPE_INFO       *p_pipe_info;
  USBH_PBHCI_PIPE            *p_pipe;
  USBH_PBHCI_EVENT_XFER_CMPL *p_event_xfer_cmpl_head;
  USBH_PBHCI_INIT_CFG_EXT    init_cfg_ext;
  USBH_PBHCI_HC_API          *p_pbhci_hc_api;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------------- CHK ARG ----------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_HC_NBR_NONE);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgOptimizeSpdPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgInitAllocPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescQty), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescIsocQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  RTOS_ASSERT_DBG_ERR_SET((p_hc_hw_info != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET(((p_hc_hw_info->DrvAPI_Ptr != DEF_NULL)
                           && (p_hc_hw_info->PipeInfoTblPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);

  p_pbhci_hc_api = p_hc_hw_info->DrvAPI_Ptr;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if ((p_hc_cfg_ext == DEF_NULL)
      || (p_hc_cfg_ext->MaxPeriodicInterval == USBH_PERIODIC_XFER_MAX_INTERVAL_DFLT)) {
    if (p_hc_hw_info->HW_Info.RH_Spd == USBH_DEV_SPD_HIGH) {
      periodic_xfer_precision = p_pbhci_hc_api->CfgPtr->MaxPeriodicIntervalHS;
    } else {
      periodic_xfer_precision = p_pbhci_hc_api->CfgPtr->MaxPeriodicInterval;
    }
  } else {
    periodic_xfer_precision = (CPU_INT08U)p_hc_cfg_ext->MaxPeriodicInterval;
  }

  RTOS_ASSERT_DBG_ERR_SET((MATH_IS_PWR2(periodic_xfer_precision) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG, USBH_HC_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((((p_hc_hw_info->HW_Info.RH_Spd != USBH_DEV_SPD_HIGH)
                            || (periodic_xfer_precision <= p_pbhci_hc_api->CfgPtr->MaxPeriodicIntervalHS))
                           && (periodic_xfer_precision <= p_pbhci_hc_api->CfgPtr->MaxPeriodicInterval)), *p_err, RTOS_ERR_NOT_SUPPORTED, USBH_HC_NBR_NONE);
#endif

  //                                                               Add host controller to core. See Note (2)
  hc_nbr = USBH_HC_AddInternal(name,
                               host_nbr,
                               &p_hc_hw_info->HW_Info,
                               &USBH_HCD_API_PBHCI,
                               &USBH_HCD_API_RH_PBHCI,
                               DEF_NULL,
                               p_hc_cfg_ext,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  CORE_ENTER_ATOMIC();                                          // PBHCI HC nbr is unknown. Retrieve w/hc & host nbr.
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  for (cnt = USBH_PBHCI_Ptr->HC_IxNext; cnt > 0u; cnt--) {
    p_pbhci_hc = USBH_PBHCI_Ptr->HC_TblPtr[cnt - 1u];

    if ((p_pbhci_hc->HC_Drv.HC_Nbr == hc_nbr)
        && (p_pbhci_hc->HC_Drv.HostNbr == host_nbr)) {
      break;
    }
  }
#else
  p_pbhci_hc = USBH_PBHCI_Ptr->HC_HeadPtr;
  while ((p_pbhci_hc != DEF_NULL)
         && (p_pbhci_hc->HC_Drv.HC_Nbr != hc_nbr)
         && (p_pbhci_hc->HC_Drv.HostNbr != host_nbr)) {
    p_pbhci_hc = p_pbhci_hc->NextPtr;
  }
#endif
  CORE_EXIT_ATOMIC();

  p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  p_pbhci_hc_drv->API_Ptr = p_hc_hw_info->DrvAPI_Ptr;
  p_pbhci_hc_drv->BSP_API_Ptr = p_hc_hw_info->BSP_API_Ptr;
  p_pbhci_hc_drv->HW_InfoPtr = &p_hc_hw_info->HW_Info;
  p_pbhci_hc_drv->PipeInfoTblPtr = p_hc_hw_info->PipeInfoTblPtr;

  //                                                               Create Host Controller global lock.
  p_pbhci_hc->LockHandle = KAL_LockCreate("USBH - PBHCI HC lock",
                                          DEF_NULL,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               ------------------ INIT ROOT HUB -------------------
  //                                                               Alloc port status table.
  p_pbhci_hc->RH_PortStatusTblPtr = (USBH_HUB_STATUS *)Mem_SegAlloc("USBH - PBHCI port status tbl",
                                                                    USBH_PBHCI_InitCfg.MemSegPtr,
                                                                    sizeof(USBH_HUB_STATUS) * p_pbhci_hc_api->CfgPtr->RH_PortQty,
                                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               Init port(s) status.
  for (port_ix = 0u; port_ix < p_pbhci_hc_api->CfgPtr->RH_PortQty; port_ix++) {
    p_pbhci_hc->RH_PortStatusTblPtr[port_ix].Status = DEF_BIT_NONE;
    p_pbhci_hc->RH_PortStatusTblPtr[port_ix].Chng = DEF_BIT_NONE;
  }

  //                                                               Alloc port status temp table.
  p_pbhci_hc->RH_PortStatusCurTblPtr = (USBH_HUB_STATUS *)Mem_SegAlloc("USBH - PBHCI port status tbl",
                                                                       USBH_PBHCI_InitCfg.MemSegPtr,
                                                                       sizeof(USBH_HUB_STATUS) * p_pbhci_hc_api->CfgPtr->RH_PortQty,
                                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               Clr temp port status.
  Mem_Clr((void *)p_pbhci_hc->RH_PortStatusCurTblPtr,
          sizeof(USBH_HUB_STATUS) * p_pbhci_hc_api->CfgPtr->RH_PortQty);

  //                                                               Init hub status.
  p_pbhci_hc->RH_Status.Status = DEF_BIT_NONE;
  p_pbhci_hc->RH_Status.Chng = DEF_BIT_NONE;
  p_pbhci_hc->RH_StatusCur.Status = DEF_BIT_NONE;
  p_pbhci_hc->RH_StatusCur.Chng = DEF_BIT_NONE;
  p_pbhci_hc->RH_IntEn = DEF_DISABLED;
  p_pbhci_hc->RH_SpdCur = p_hc_hw_info->HW_Info.RH_Spd;

  //                                                               --------------- INIT CTRL ASYNC LIST ---------------
  p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_CTRL_IX];
  p_async_list->CurPtr = DEF_NULL;
  p_async_list->HeadPtr = DEF_NULL;

  //                                                               --------------- INIT BULK ASYNC LIST ---------------
  p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_BULK_IX];
  p_async_list->CurPtr = DEF_NULL;
  p_async_list->HeadPtr = DEF_NULL;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  //                                                               ---------------- INIT PERIODIC LIST ----------------
  p_pbhci_hc->BranchQty = periodic_xfer_precision;
  p_pbhci_hc->LevelQty = CPU_CntTrailZeros(periodic_xfer_precision) + 1u;

  //                                                               Alloc periodic node table.
  p_pbhci_hc->PeriodicNodeTblPtr = (USBH_PBHCI_PERIODIC_NODE *)Mem_SegAlloc("USBH - PBHCI periodic node tbl",
                                                                            USBH_PBHCI_InitCfg.MemSegPtr,
                                                                            sizeof(USBH_PBHCI_PERIODIC_NODE) * ((periodic_xfer_precision * 2u) - 1u),
                                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               Alloc periodic branch table.
  p_pbhci_hc->PeriodicBranchTblPtr = (USBH_PBHCI_PERIODIC_BRANCH *)Mem_SegAlloc("USBH - PBHCI periodic branch table",
                                                                                USBH_PBHCI_InitCfg.MemSegPtr,
                                                                                sizeof(USBH_PBHCI_PERIODIC_BRANCH) * periodic_xfer_precision,
                                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               At beginning, all bandwidth is available.
  for (cnt = 0u; cnt < periodic_xfer_precision; cnt++) {
    p_pbhci_hc->PeriodicBranchTblPtr[cnt].RemBandwidth = USBH_HC_MAX_PERIODIC_BW_GET(p_pbhci_hc->RH_SpdCur);
  }
#endif

  //                                                               -------------- CREATE ED AND TD POOLS --------------
  (void)Mem_DynPoolCreate("USBH - PBHCI ED pool",               // Create Endpoint Descriptor (ED) pool.
                          &p_pbhci_hc->ED_Pool,
                          USBH_PBHCI_InitCfg.MemSegPtr,
                          sizeof(USBH_PBHCI_ED),
                          sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                          p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty + p_hc_cfg_ext->CfgInitAllocPtr->EP_DescIsocQty,
                          p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty + p_hc_cfg_ext->CfgInitAllocPtr->EP_DescIsocQty,
#else
                          0u,
                          LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  (void)Mem_DynPoolCreate("USBH - PBHCI TD pool",               // Create Transfer Descriptor (TD) pool.
                          &p_pbhci_hc->TD_Pool,
                          USBH_PBHCI_InitCfg.MemSegPtr,
                          sizeof(USBH_PBHCI_TD),
                          sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                          p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty + p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty,
                          p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty + p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty,
#else
                          0u,
                          LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               ------------ ALLOC PIPES & BUILD LISTS -------------
  p_pipe_info = p_hc_hw_info->PipeInfoTblPtr;                   // Retrieve highest pipe nbr.
  pipe_nbr_high = 0u;
  pipe_qty = 0u;
  while (p_pipe_info->Attrib != DEF_BIT_NONE) {
    if (p_pipe_info->Nbr > pipe_nbr_high) {
      pipe_nbr_high = p_pipe_info->Nbr;
    }

    pipe_qty++;
    p_pipe_info++;
  }

  //                                                               Alloc pipe xfer complete event structures.
  p_event_xfer_cmpl_head = (USBH_PBHCI_EVENT_XFER_CMPL *)Mem_SegAlloc("USBH - Event xfer cmpl table",
                                                                      USBH_PBHCI_InitCfg.MemSegPtr,
                                                                      (sizeof(USBH_PBHCI_EVENT_XFER_CMPL) * pipe_qty),
                                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  for (cnt = 0u; cnt < pipe_qty - 1u; cnt++) {
    p_event_xfer_cmpl_head[cnt].Type = USBH_PBHCI_EVENT_TYPE_PIPE_CMPL;
    p_event_xfer_cmpl_head[cnt].NextPtr = (USBH_PBHCI_EVENT *)&p_event_xfer_cmpl_head[cnt + 1u];
  }
  p_event_xfer_cmpl_head[cnt].Type = USBH_PBHCI_EVENT_TYPE_PIPE_CMPL;

  //                                                               Add event to free list.
  CORE_ENTER_ATOMIC();
  p_event_xfer_cmpl_head[cnt].NextPtr = (USBH_PBHCI_EVENT *)USBH_PBHCI_Ptr->EventXferCmplFreeHeadPtr;
  USBH_PBHCI_Ptr->EventXferCmplFreeHeadPtr = p_event_xfer_cmpl_head;
  CORE_EXIT_ATOMIC();

  //                                                               Alloc pipe ptr table.
  p_pbhci_hc->PipePtrTblPtr = (USBH_PBHCI_PIPE **)Mem_SegAlloc("USBH - PBHCI pipe table",
                                                               USBH_PBHCI_InitCfg.MemSegPtr,
                                                               (sizeof(USBH_PBHCI_PIPE *) * (pipe_nbr_high + 1u)),
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  //                                                               PBHCI expects first pipe to be ctrl pipe.
  p_pipe_info = p_hc_hw_info->PipeInfoTblPtr;
  p_pbhci_hc->PipeCtrl.PipeInfoPtr = p_pipe_info;
  p_pbhci_hc->CtrlXferInProgress = DEF_NO;
  p_pbhci_hc->PipePtrTblPtr[0u] = &p_pbhci_hc->PipeCtrl;

  for (cnt = 0u; cnt < USBH_PBHCI_PIPE_LIST_QTY; cnt++) {
    p_pbhci_hc->PipeFreeHeadTblPtr[cnt] = DEF_NULL;
    p_pbhci_hc->PipeFreeTailTblPtr[cnt] = DEF_NULL;
    p_pbhci_hc->PipeFreeCfgTblPtr[cnt] = DEF_NULL;
  }

  p_pipe_info++;
  while (p_pipe_info->Attrib != DEF_BIT_NONE) {
    p_pipe = (USBH_PBHCI_PIPE *)Mem_SegAlloc("USBH - PBHCI pipe",
                                             USBH_PBHCI_InitCfg.MemSegPtr,
                                             sizeof(USBH_PBHCI_PIPE),
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (USBH_HC_NBR_NONE);
    }

    p_pbhci_hc->PipePtrTblPtr[p_pipe_info->Nbr] = p_pipe;

    p_pipe->PipeInfoPtr = p_pipe_info;
    p_pipe->CurED_Ptr = DEF_NULL;

    //                                                             Determine if pipe able of multiple xfer type.
    pipe_attrib_temp = (p_pipe_info->Attrib & USBH_PIPE_INFO_TYPE_ALL);
    pipe_attrib_temp &= ~DEF_BIT(CPU_CntTrailZeros(pipe_attrib_temp));
    p_pipe->MultiTypeAble = (pipe_attrib_temp == DEF_BIT_NONE) ? DEF_NO : DEF_YES;

    //                                                             Add pipe to free list(s).
    USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);

    p_pipe_info++;
  }

  //                                                               -------------------- INIT HCD. ---------------------
  init_cfg_ext.PipeQty = pipe_qty;
  init_cfg_ext.PipeMaxNbr = pipe_nbr_high;
  init_cfg_ext.DedicatedMemCfgPtr = p_hc_cfg_ext->DedicatedMemCfgPtr;
  p_pbhci_hc_api->Init(&p_pbhci_hc->HC_Drv,
                       p_pbhci_hc->MemSegPtr,
                       p_pbhci_hc->MemSegBufPtr,
                       &init_cfg_ext,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_NBR_NONE);
  }

  LOG_VRB(("HC added OK on host #", (u)host_nbr,
           " HC #", (u)hc_nbr));

  return (hc_nbr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   HOST CONTROLLER DRIVER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_HCD_Init()
 *
 * @brief    Initializes host controller.
 *
 * @param    p_hc_drv        Pointer to the HC driver structure.
 *
 * @param    p_mem_seg       Pointer to the memory segment where to allocate control information.
 *
 * @param    p_mem_seg_buf   Pointer to the memory segment where to allocate buffers.
 *
 * @param    p_hc_cfg_ext    Pointer to the extended Host Controller configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_ALLOC
 *
 * @note     (1) This function must allocate the Host Controller structure and assign it to the HC
 *               table. This will allow the USBH_PBHCI_HC_Add() function to continue its initialization
 *               process.
 *******************************************************************************************************/
static void USBH_HCD_Init(USBH_HC_DRV     *p_hc_drv,
                          MEM_SEG         *p_mem_seg,
                          MEM_SEG         *p_mem_seg_buf,
                          USBH_HC_CFG_EXT *p_hc_cfg_ext,
                          RTOS_ERR        *p_err)
{
  CPU_INT08U    pbhci_hc_nbr;
  USBH_PBHCI_HC *p_pbhci_hc;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_hc_cfg_ext);

  CORE_ENTER_ATOMIC();
  pbhci_hc_nbr = USBH_PBHCI_Ptr->HC_IxNext;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  if (pbhci_hc_nbr >= USBH_PBHCI_HC_MAX_QTY_OPTIMIZE_SPD) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }
#endif

  USBH_PBHCI_Ptr->HC_IxNext++;
  CORE_EXIT_ATOMIC();

  p_pbhci_hc = (USBH_PBHCI_HC *)Mem_SegAlloc("USBH - PBHCI Host Controller",
                                             USBH_PBHCI_InitCfg.MemSegPtr,
                                             sizeof(USBH_PBHCI_HC),
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_pbhci_hc->HC_Drv.PBHCI_HC_Nbr = pbhci_hc_nbr;               // Assign PBHCI's specific HC nbr.
  p_pbhci_hc->HC_Drv.HostNbr = p_hc_drv->HostNbr;
  p_pbhci_hc->HC_Drv.HC_Nbr = p_hc_drv->Nbr;
  p_pbhci_hc->MemSegPtr = p_mem_seg;
  p_pbhci_hc->MemSegBufPtr = p_mem_seg_buf;

  p_hc_drv->DataPtr = (void *)p_pbhci_hc;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_PBHCI_Ptr->HC_TblPtr[pbhci_hc_nbr] = p_pbhci_hc;
#else
  CORE_ENTER_ATOMIC();                                          // Add PBHCI HC to list.
  p_pbhci_hc->NextPtr = USBH_PBHCI_Ptr->HC_HeadPtr;
  USBH_PBHCI_Ptr->HC_HeadPtr = p_pbhci_hc;
  CORE_EXIT_ATOMIC();
#endif

#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  {
    CPU_INT08U host_ix;
    CPU_INT08U hc_ix;
    CPU_INT08U dev_ix;
    CPU_INT08U ep_ix;
    CPU_INT08U ix;

    USBH_DBG_PBHCI_STATS_RESET();
    for (host_ix = 0u; host_ix < USBH_DBG_PBHCI_HOST_QTY; host_ix++) {
      for (hc_ix = 0u; hc_ix < USBH_DBG_PBHCI_HC_QTY; hc_ix++) {
        for (ix = 0u; ix < USBH_DBG_PBHCI_SCHED_EVENT_QTY; ix++) {
          USBH_DbgPBHCI_Stats[host_ix][hc_ix].SchedEventTbl[ix].Type = (USBH_PBHCI_EVENT_TYPE)ix;
        }
        for (ix = 0u; ix < USBH_DBG_PBHCI_EVENT_PORT_QTY; ix++) {
          USBH_DbgPBHCI_Stats[host_ix][hc_ix].EventPortTbl[ix].Type = (USBH_DBG_PBHCI_EVENT_PORT_TYPE)ix;
        }
        for (dev_ix = 0u; dev_ix < USBH_DBG_PBHCI_HOST_QTY; dev_ix++) {
          for (ep_ix = 0u; ep_ix < USBH_DBG_PBHCI_HC_QTY; ep_ix++) {
            for (ix = 0u; ix < USBH_DBG_PBHCI_URB_DONE_QTY; ix++) {
              USBH_DbgPBHCI_Stats[host_ix][hc_ix].DevTbl[dev_ix].EpTbl[ep_ix].UrbDoneTbl[ix].Type = (USBH_DBG_PBHCI_URB_DONE_TYPE)ix;
            }
            for (ix = 0u; ix < USBH_DBG_PBHCI_XFER_CMPL_QTY; ix++) {
              USBH_DbgPBHCI_Stats[host_ix][hc_ix].DevTbl[dev_ix].EpTbl[ep_ix].XferCmplTbl[ix].Type = (USBH_DBG_PBHCI_XFER_CMPL_TYPE)ix;
            }
          }
        }
      }
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                               USBH_HCD_UnInit()
 *
 * @brief    Un-initializes host controller.
 *
 * @param    p_hc_drv    Pointer to the HC driver structure.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_UnInit(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  if (p_pbhci_hc_api->UnInit != DEF_NULL) {
    p_pbhci_hc_api->UnInit(p_pbhci_hc_drv, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("UnInit PBHCD -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  KAL_LockDel(p_pbhci_hc->LockHandle);

  return;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HCD_Start()
 *
 * @brief    Starts Host Controller.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_Start(USBH_HC_DRV *p_hc_drv,
                           RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  LOG_VRB(("Start HC on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr));
  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdStart);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_pbhci_hc->PeriodicEP_OpenCnt = 0u;
#endif

  p_pbhci_hc_api->Start(p_pbhci_hc_drv, p_err);
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Stop()
 *
 * @brief    Stops Host Controller.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_Stop(USBH_HC_DRV *p_hc_drv,
                          RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  LOG_VRB(("Stop HC on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr));
  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdStop);

  p_pbhci_hc_api->Stop(p_pbhci_hc_drv, p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_Suspend()
 *
 * @brief    Suspends Host Controller.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_Suspend(USBH_HC_DRV *p_hc_drv,
                             RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  LOG_VRB(("Suspend HC on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr));
  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdSuspend);

  p_pbhci_hc_api->Suspend(p_pbhci_hc_drv, p_err);
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Resume()
 *
 * @brief    Resumes Host Controller.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_Resume(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  LOG_VRB(("Resume HC on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr));
  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdResume);

  p_pbhci_hc_api->Resume(p_pbhci_hc_drv, p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_FrameNbrGet()
 *
 * @brief    Retrieves current frame number.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @return   Current frame number.
 *******************************************************************************************************/
static CPU_INT16U USBH_HCD_FrameNbrGet(USBH_HC_DRV *p_hc_drv)
{
  CPU_INT16U        frame_nbr;
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  frame_nbr = p_pbhci_hc_api->FrameNbrGet(p_pbhci_hc_drv);
  USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdFrameNbrGet);

  return (frame_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Open()
 *
 * @brief    Opens an endpoint.
 *
 * @param    p_hc_drv            Pointer to the Host Controller driver.
 *
 * @param    dev_handle          Device handle.
 *
 * @param    ep_handle           Endpoint handle.
 *
 * @param    p_hcd_ep_params     Pointer to the structure that contains endpoint parameters.
 *
 * @param    pp_hcd_ep_data      Pointer to the variable that will receive pointer to HCD's endpoint
 *                               specific data.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_EP_BW_NOT_AVAIL
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_EP_NONE_AVAIL
 *                                   - RTOS_ERR_POOL_EMPTY
 *******************************************************************************************************/
static void USBH_HCD_EP_Open(USBH_HC_DRV        *p_hc_drv,
                             USBH_DEV_HANDLE    dev_handle,
                             USBH_EP_HANDLE     ep_handle,
                             USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                             void               **pp_hcd_ep_data,
                             RTOS_ERR           *p_err)
{
  USBH_PBHCI_HC         *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV     *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API     *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  USBH_PBHCI_ED         *p_ed;
  USBH_PBHCI_ASYNC_LIST *p_async_list;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
  USBH_PBHCI_PIPE *p_pipe;
#endif
  CPU_INT16U interval;
#endif
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)dev_handle - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(ep_handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenEnter);
  //                                                               ---------- VALIDATE ENDPOINT IS SUPPORTED ----------
  //                                                               High bandwidth not supported by PBHCI.
  if (USBH_EP_NBR_TRANSACTION_GET(p_hcd_ep_params->MaxPktSize) != USBH_EP_NBR_TRANSACTION_1) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenError);
    return;
  }

  //                                                               Ensure at least one pipe covers EP requirements.
  if (p_hcd_ep_params->Type == USBH_EP_TYPE_CTRL) {
    if (p_pbhci_hc->PipeCtrl.PipeInfoPtr->MaxPktSize < p_hcd_ep_params->MaxPktSize) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenError);
      return;
    }
  } else {
    CPU_BOOLEAN          accept = DEF_NO;
    CPU_INT08U           pipe_attr;
    USBH_PBHCI_PIPE_INFO *p_pipe_info = p_pbhci_hc_drv->PipeInfoTblPtr;

    pipe_attr = (p_hcd_ep_params->Type == USBH_EP_TYPE_BULK) ? USBH_PIPE_INFO_TYPE_BULK
                : (p_hcd_ep_params->Type == USBH_EP_TYPE_INTR) ? USBH_PIPE_INFO_TYPE_INTR : USBH_PIPE_INFO_TYPE_ISOC;

    while (p_pipe_info->Attrib != DEF_BIT_NONE) {
      if ((DEF_BIT_IS_SET(p_pipe_info->Attrib, pipe_attr) == DEF_YES)
          && (p_pipe_info->MaxPktSize >= p_hcd_ep_params->MaxPktSize)) {
        accept = DEF_YES;
        break;
      }

      p_pipe_info++;
    }

    if (accept == DEF_NO) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenError);
      return;
    }
  }

  //                                                               Allocate structure for endpoint descriptor.
  p_ed = (USBH_PBHCI_ED *)Mem_DynPoolBlkGet(&p_pbhci_hc->ED_Pool,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to allocate endpoint descriptor."));
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenError);
    return;
  }

  p_ed->Suspended = DEF_NO;
  p_ed->EP_Params = *p_hcd_ep_params;
  p_ed->PipePtr = DEF_NULL;
  p_ed->NextPtr = DEF_NULL;
  p_ed->DevHandle = dev_handle;
  p_ed->EP_Handle = ep_handle;
  p_ed->TD_HeadPtr = DEF_NULL;
  p_ed->TD_TailPtr = DEF_NULL;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_ed->IntervalIx = 0u;
#endif

  USBH_PBHCI_HC_Lock(p_pbhci_hc);

  switch (p_hcd_ep_params->Type) {
    case USBH_EP_TYPE_CTRL:                                     // Control endpoints have a dedicated pipe.
      p_ed->PipeAttr = USBH_PIPE_INFO_TYPE_CTRL;

      p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_CTRL_IX];
      USBH_PBHCI_AsyncListED_Insert(p_async_list, p_ed);

      if (p_pbhci_hc_api->DevConn != DEF_NULL) {
        USBH_DBG_PBHCI_STATS_DEVICE_INC(host_nbr, hc_nbr, dev_nbr, HcdDevConn);

        p_pbhci_hc_api->DevConn(p_pbhci_hc_drv,
                                dev_handle,
                                p_hcd_ep_params->DevAddr,
                                p_hcd_ep_params->DevSpd,
                                p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end_err;
        }
      }
      break;

    case USBH_EP_TYPE_BULK:
      p_ed->PipeAttr = USBH_PIPE_INFO_TYPE_BULK;
      p_ed->CurPid = USBH_PBHCI_PID_DATA0;

      p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_BULK_IX];
      USBH_PBHCI_AsyncListED_Insert(p_async_list, p_ed);
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
    case USBH_EP_TYPE_ISOC:                                     // Pipe statically allocated on EP open for isoc.
      p_ed->PipeAttr = USBH_PIPE_INFO_TYPE_ISOC;
      p_ed->CurPid = USBH_PBHCI_PID_DATA0;

      p_pipe = USBH_PBHCI_PipeAlloc(p_pbhci_hc, p_ed);
      if (p_pipe == DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_NONE_AVAIL);
        goto end_err;
      }

      USBH_PBHCI_PipeAssoc(p_pbhci_hc,
                           p_pipe,
                           p_ed,
                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);           // Must mark pipe as uncfg as state in HCD is unknown.
        goto end_err;
      }
      //                                                           Break intentionnally omitted.
#endif

    case USBH_EP_TYPE_INTR:
      if (p_hcd_ep_params->Type == USBH_EP_TYPE_INTR) {
        p_ed->PipeAttr = USBH_PIPE_INFO_TYPE_INTR;
      }

      p_ed->CurPid = USBH_PBHCI_PID_DATA0;

      interval = p_hcd_ep_params->Interval;
      if (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
        interval = (interval + 7u) / 8u;                        // Convert uFrame in Frame.
        p_ed->EP_Params.Interval = interval;
      }

      p_ed->EP_Params.Interval = USBH_PBHCI_PeriodicListED_Insert(p_pbhci_hc,
                                                                  p_ed,
                                                                  p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
        if (p_hcd_ep_params->Type == USBH_EP_TYPE_ISOC) {
          //                                                       Must free the pipe if err detected.
          USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);
        }
#endif
        goto end_err;
      }
      break;
#endif

    default:
      break;
  }

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if ((p_hcd_ep_params->Type == USBH_EP_TYPE_INTR)
      || (p_hcd_ep_params->Type == USBH_EP_TYPE_ISOC)) {
    p_pbhci_hc->PeriodicEP_OpenCnt++;

    if (p_pbhci_hc->PeriodicEP_OpenCnt == 1u) {                 // If first periodic EP, en SOF int in PBHCD.
      USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, HcdSofIntEn);

      p_pbhci_hc_api->SOF_IntEn(p_pbhci_hc_drv, DEF_ENABLED);
    }
  }
#endif

  USBH_PBHCI_HC_Unlock(p_pbhci_hc);

  if (USBH_EP_IS_IN(p_hcd_ep_params->Addr) == DEF_YES) {
    DEF_BIT_SET(p_ed->PipeAttr, USBH_PIPE_INFO_DIR_IN);
  } else {
    DEF_BIT_SET(p_ed->PipeAttr, USBH_PIPE_INFO_DIR_OUT);
  }

  *pp_hcd_ep_data = (void *)p_ed;

  LOG_VRB(("Endpoint open on host #", (u)p_hc_drv->HostNbr,
           " HC #", (u)p_hc_drv->Nbr,
           " endpoint address ", (X)p_ed->EP_Params.Addr));

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenSuccess);

  return;

end_err:
  {
    RTOS_ERR err_local;

    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpOpenError);
    USBH_PBHCI_HC_Unlock(p_pbhci_hc);

    Mem_DynPoolBlkFree(&p_pbhci_hc->ED_Pool,
                       (void *)p_ed,
                       &err_local);
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Close()
 *
 * @brief    Closes an endpoint.
 *
 * @param    p_hc_drv        Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data   Pointer to the HCD's endpoint specific data.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_POOL_FULL
 *******************************************************************************************************/
static void USBH_HCD_EP_Close(USBH_HC_DRV *p_hc_drv,
                              void        *p_hcd_ep_data,
                              RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC         *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_ASYNC_LIST *p_async_list;
  USBH_PBHCI_HC_DRV     *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API     *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  USBH_PBHCI_ED         *p_ed = (USBH_PBHCI_ED *)p_hcd_ep_data;
  CPU_INT08U            ep_type = p_ed->EP_Params.Type;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpClose);
  USBH_PBHCI_HC_Lock(p_pbhci_hc);

  switch (ep_type) {
    case USBH_EP_TYPE_CTRL:
      p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_CTRL_IX];
      USBH_PBHCI_AsyncListED_Remove(p_async_list, p_ed);

      if (p_pbhci_hc_api->DevDisconn != DEF_NULL) {
        USBH_DBG_PBHCI_STATS_DEVICE_INC(host_nbr, hc_nbr, dev_nbr, HcdDevDisconn);

        p_pbhci_hc_api->DevDisconn(p_pbhci_hc_drv,
                                   p_ed->DevHandle,
                                   p_ed->EP_Params.DevAddr);
      }
      break;

    case USBH_EP_TYPE_BULK:
      p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_BULK_IX];
      USBH_PBHCI_AsyncListED_Remove(p_async_list, p_ed);
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    case USBH_EP_TYPE_ISOC:
    case USBH_EP_TYPE_INTR:
      p_pbhci_hc->PeriodicEP_OpenCnt--;

      if (p_pbhci_hc->PeriodicEP_OpenCnt == 0u) {               // If last periodic EP, disable SOF int.
        USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, HcdSofIntDis);

        p_pbhci_hc_api->SOF_IntEn(p_pbhci_hc_drv, DEF_DISABLED);
      }

      USBH_PBHCI_PeriodicListED_Remove(p_pbhci_hc, p_ed);
      break;
#endif

    default:
      break;
  }

  //                                                               If pipe currently assoc to EP, free and uncfg it.
  if (ep_type != USBH_EP_TYPE_CTRL) {
    USBH_PBHCI_PIPE *p_pipe = p_ed->PipePtr;

    if ((p_pipe != DEF_NULL)
        && (p_pipe->CurED_Ptr == p_ed)) {
      if (ep_type != USBH_EP_TYPE_ISOC) {
        USBH_PBHCI_PipeListRemove(p_pbhci_hc, p_pipe);
      }

      USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);
    }
  }

  USBH_PBHCI_HC_Unlock(p_pbhci_hc);

  //                                                               Free Endpoint Descriptor.
  Mem_DynPoolBlkFree(&p_pbhci_hc->ED_Pool,
                     (void *)p_ed,
                     p_err);

  LOG_VRB(("Endpoint close on host #", (u)p_hc_drv->HostNbr,
           " HC #", (u)p_hc_drv->Nbr,
           " endpoint address ", (X)p_ed->EP_Params.Addr));
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Suspend()
 *
 * @brief    Suspends TD processing on endpoint.
 *
 * @param    p_hc_drv        Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data   Pointer to the HCD's endpoint specific data.
 *
 * @param    suspend         Flag that indicates if TD processing should be suspended or resumed.
 *                           DEF_YES     Processing suspended.
 *                           DEF_NO      Processing resumed.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_EP_Suspend(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN suspend,
                                RTOS_ERR    *p_err)
{
  USBH_PBHCI_HC *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_ED *p_ed = (USBH_PBHCI_ED *)p_hcd_ep_data;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpSuspendEnter);
  USBH_PBHCI_HC_Lock(p_pbhci_hc);

  p_ed->Suspended = suspend;

  if (suspend == DEF_YES) {
    USBH_PBHCI_TD *p_td = p_ed->TD_HeadPtr;                     // Abort xfer that were submitted to PBHCD.

    if ((p_td != DEF_NULL)
        && (p_td->IsSubmitted == DEF_YES)) {
      //                                                          *
      CPU_BOOLEAN      abort = DEF_YES;
      USBH_PBHCI_EVENT *p_pbhci_event;
      CORE_DECLARE_IRQ_STATE;

      CORE_ENTER_ATOMIC();
      p_pbhci_event = USBH_PBHCI_Ptr->EventQHeadPtr;
      while (p_pbhci_event != DEF_NULL) {
        if ((p_pbhci_event->Type == USBH_PBHCI_EVENT_TYPE_PIPE_CMPL)
            && (((USBH_PBHCI_EVENT_XFER_CMPL *)p_pbhci_event)->PipeNbr == p_ed->PipePtr->PipeInfoPtr->Nbr)
            && (p_pbhci_event->PBHCI_HC_DrvPtr == &(p_pbhci_hc->HC_Drv))) {
          abort = DEF_NO;
          break;
        }

        p_pbhci_event = p_pbhci_event->NextPtr;
      }
      CORE_EXIT_ATOMIC();
      //

      if (abort == DEF_YES) {
        USBH_PBHCI_PipeAbortAndFree(p_pbhci_hc,
                                    p_ed->PipePtr,
                                    p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          p_ed->Suspended = DEF_NO;
          USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpSuspendError);
          goto end_unlock;
        }

        p_td->IsSubmitted = DEF_NO;
      }
    }
  } else {
    switch (p_ed->EP_Params.Type) {
      case USBH_EP_TYPE_CTRL:                                   // If no ctrl xfer in progress, start next xfer.
        if (p_pbhci_hc->CtrlXferInProgress == DEF_NO) {
          USBH_PBHCI_NextCtrlSubmit(p_pbhci_hc);
        }
        break;

      case USBH_EP_TYPE_BULK:
        USBH_PBHCI_NextBulkSubmit(p_pbhci_hc);                  // Start bulk xfer in case some xfers were submitted.
        break;

      case USBH_EP_TYPE_INTR:
      case USBH_EP_TYPE_ISOC:
      default:
        break;
    }
  }

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpSuspendSuccess);
end_unlock:
  USBH_PBHCI_HC_Unlock(p_pbhci_hc);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_HaltClr()
 *
 * @brief    Clears halt condition on endpoint.
 *
 * @param    p_hc_drv            Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to the HCD's endpoint specific data.
 *
 * @param    data_toggle_clr     Flag that indicates if data toggle must be reset on endpoint.
 *                               DEF_YES     Reset data toggle.
 *                               DEF_NO      Do not reset data toggle.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBH_HCD_EP_HaltClr(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN data_toggle_clr,
                                RTOS_ERR    *p_err)
{
  USBH_TOKEN        token_pid;
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  USBH_PBHCI_ED     *p_ed = (USBH_PBHCI_ED *)p_hcd_ep_data;
  USBH_PBHCI_PIPE   *p_pipe;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdEpHaltClr);
  LOG_VRB(("Endpoint halt clear on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " endpoint address ", (X)p_ed->EP_Params.Addr));

  USBH_PBHCI_HC_Lock(p_pbhci_hc);

  if (data_toggle_clr == DEF_YES) {
    p_ed->CurPid = USBH_PBHCI_PID_DATA0;                        // Reset data toggle.
  }

  p_pipe = p_ed->PipePtr;
  if ((p_pipe != DEF_NULL)
      && (p_pipe->CurED_Ptr == p_ed)) {
    token_pid = (USBH_EP_IS_IN(p_ed->EP_Params.Addr) == DEF_YES) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeCfg);

    p_pbhci_hc_api->PipeCfg(p_pbhci_hc_drv,                     // Re-configure pipe to ensure it is in known state.
                            p_pipe->PipeInfoPtr->Nbr,
                            &p_ed->EP_Params,
                            token_pid,
                            p_ed->CurPid,
                            p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  USBH_PBHCI_HC_Unlock(p_pbhci_hc);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Submit()
 *
 * @brief    Submits a USB Request Block (URB) on endpoint.
 *
 * @param    p_hc_drv            Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to the HCD's endpoint specific data.
 *
 * @param    pp_hcd_urb_data     Pointer to the variable that will receive pointer to HCD's URB specific
 *                               data.
 *
 * @param    p_hcd_urb_params    Pointer to the structure that contains URB parameters.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *******************************************************************************************************/
static void USBH_HCD_URB_Submit(USBH_HC_DRV         *p_hc_drv,
                                void                *p_hcd_ep_data,
                                void                **pp_hcd_urb_data,
                                USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                RTOS_ERR            *p_err)
{
  USBH_PBHCI_HC       *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV   *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API   *p_pbhci_hc_api;
  USBH_PBHCI_PIPE     *p_pipe;
  USBH_PBHCI_ED       *p_ed = (USBH_PBHCI_ED *)p_hcd_ep_data;
  USBH_PBHCI_TD       *p_td = DEF_NULL;
  USBH_PBHCI_PID_DATA pid_data;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitEnter);
  LOG_VRB(("URB submit on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " endpoint address ", (X)p_ed->EP_Params.Addr));

  p_td = (USBH_PBHCI_TD *)Mem_DynPoolBlkGet(&p_pbhci_hc->TD_Pool,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to allocate transfer descriptor."));
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitError);
    return;
  }

  *pp_hcd_urb_data = (void *)p_td;

  p_td->BufPtr = p_hcd_urb_params->BufPtr;
  p_td->BufLen = p_hcd_urb_params->BufLen;
  p_td->IsSubmitted = DEF_NO;
  p_td->Token = p_hcd_urb_params->Token;
  p_td->ED_Ptr = p_ed;
  p_td->XferLen = 0u;
  p_td->XferLenNext = 0u;
  p_td->NextPtr = DEF_NULL;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_td->PipeAllocErrCnt = 0u;
#endif

  USBH_PBHCI_HC_Lock(p_pbhci_hc);

  switch (p_ed->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
      if ((p_pbhci_hc->CtrlXferInProgress == DEF_YES)
          || (p_ed->Suspended == DEF_YES)) {
        //                                                         TD is queued and will be processed later.
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
        break;
      }

      p_td->IsSubmitted = DEF_YES;

      p_pipe = &p_pbhci_hc->PipeCtrl;
      p_pipe->CurED_Ptr = p_ed;
      p_pipe->CurEP_Handle = p_ed->EP_Handle;
      p_ed->PipePtr = p_pipe;
      p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
      pid_data = (p_td->Token != USBH_TOKEN_SETUP) ? USBH_PBHCI_PID_DATA1 : USBH_PBHCI_PID_DATA0;

      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeCfg);
      p_pbhci_hc_api->PipeCfg(p_pbhci_hc_drv,                   // Configure ctrl pipe.
                              p_pipe->PipeInfoPtr->Nbr,
                              &p_ed->EP_Params,
                              p_td->Token,
                              pid_data,
                              p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitError);
        break;
      }

      USBH_PBHCI_PipeXfer(p_pbhci_hc,
                          p_pipe,
                          p_td,
                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        p_pbhci_hc->CtrlXferInProgress = DEF_YES;
                 #if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
      } else {
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitError);
                 #endif
      }
      break;

    case USBH_EP_TYPE_BULK:
      if ((p_ed->TD_HeadPtr != DEF_NULL)                        // TD is queued and will be processed later.
          || (p_ed->Suspended == DEF_YES)) {
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
        break;
      }

      p_pipe = USBH_PBHCI_PipeAlloc(p_pbhci_hc,
                                    p_ed);
      if (p_pipe == DEF_NULL) {
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
        //                                                         No pipe avail. URB Q-ed. Will be processed once a bulk-able pipe gets freed.
        break;
      }
      p_td->IsSubmitted = DEF_YES;

      USBH_PBHCI_PipeAssoc(p_pbhci_hc,
                           p_pipe,
                           p_ed,
                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);           // Must mark pipe as uncfg as state in HCD is unknown.
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitError);
        break;
      }

      USBH_PBHCI_PipeXfer(p_pbhci_hc,
                          p_pipe,
                          p_td,
                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
             #if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitError);
      } else {
        USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
             #endif
      }
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    case USBH_EP_TYPE_INTR:
    case USBH_EP_TYPE_ISOC:
      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbSubmitSuccess);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBH_PBHCI_ED_TD_Insert(p_ed, p_td);
  }

  USBH_PBHCI_HC_Unlock(p_pbhci_hc);

  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      && (p_td != DEF_NULL)) {
    RTOS_ERR local_err;

    Mem_DynPoolBlkFree(&p_pbhci_hc->TD_Pool,
                       (void *)p_td,
                       &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing TD after submit failed -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Complete()
 *
 * @brief    Completes a URB once it has been transferred or an error occurred.
 *
 * @param    p_hc_drv            Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to the HCD's endpoint specific data.
 *
 * @param    p_hcd_urb_data      Pointer to the HCD's URB      specific data.
 *
 * @param    p_hcd_urb_params    Pointer to the structure that contains URB parameters.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *
 * @return   Number of bytes transferred.
 *******************************************************************************************************/
static CPU_INT32U USBH_HCD_URB_Complete(USBH_HC_DRV         *p_hc_drv,
                                        void                *p_hcd_ep_data,
                                        void                *p_hcd_urb_data,
                                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                        RTOS_ERR            *p_err)
{
  RTOS_ERR      err_local;
  CPU_INT32U    xfer_len;
  USBH_PBHCI_HC *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_TD *p_td = (USBH_PBHCI_TD *)p_hcd_urb_data;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_td->ED_Ptr->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_td->ED_Ptr->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbComplete);
  (void)&p_hcd_ep_data;
  (void)&p_hcd_urb_params;

  xfer_len = p_td->XferLen;

  LOG_VRB(("URB cmpl on host #", (u)p_hc_drv->HostNbr,
           " HC #", (u)p_hc_drv->Nbr,
           " endpoint address ", (X)p_td->ED_Ptr->EP_Params.Addr));

  RTOS_ERR_COPY(*p_err, p_td->Err);

  Mem_DynPoolBlkFree(&p_pbhci_hc->TD_Pool,
                     (void *)p_td,
                     &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing TD when complete -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Abort()
 *
 * @brief    Aborts specified URB on endpoint.
 *
 * @param    p_hc_drv            Pointer to the Host Controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to the HCD's endpoint specific data.
 *
 * @param    p_hcd_urb_data      Pointer to the HCD's URB      specific data.
 *
 * @param    p_hcd_urb_params    Pointer to the structure that contains URB parameters.
 *******************************************************************************************************/
static void USBH_HCD_URB_Abort(USBH_HC_DRV         *p_hc_drv,
                               void                *p_hcd_ep_data,
                               void                *p_hcd_urb_data,
                               USBH_HCD_URB_PARAMS *p_hcd_urb_params)
{
  RTOS_ERR      err;
  USBH_PBHCI_ED *p_ed = (USBH_PBHCI_ED *)p_hcd_ep_data;
  USBH_PBHCI_TD *p_td = p_ed->TD_HeadPtr;
  USBH_PBHCI_TD *p_td_prev = DEF_NULL;
  USBH_PBHCI_TD *p_td_to_abort = (USBH_PBHCI_TD *)p_hcd_urb_data;
  USBH_PBHCI_HC *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  (void)&p_hcd_urb_params;
  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, HcdUrbAbort);

  USBH_PBHCI_HC_Lock(p_pbhci_hc);
  while ((p_td != DEF_NULL)                                     // Find TD in ED's list.
         && (p_td != p_td_to_abort)) {
    p_td_prev = p_td;
    p_td = p_td->NextPtr;
  }

  if (p_td == p_td_to_abort) {
    if (p_td_prev != DEF_NULL) {                                // Remove TD from list.
      p_td_prev->NextPtr = p_td->NextPtr;
    }

    if (p_ed->TD_HeadPtr == p_td) {
      p_ed->TD_HeadPtr = p_td->NextPtr;
    }

    if (p_ed->TD_TailPtr == p_td) {
      p_ed->TD_TailPtr = p_td_prev;
    }

    if (p_td->IsSubmitted == DEF_YES) {                         // Abort pipe if TD was submitted.
      USBH_PBHCI_PipeAbortAndFree(p_pbhci_hc,
                                  p_ed->PipePtr,
                                  &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("While aborting and freeing pipe -> ", RTOS_ERR_LOG_ARG_GET(err)));
      }
    }
  }

  USBH_PBHCI_HC_Unlock(p_pbhci_hc);

  Mem_DynPoolBlkFree(&p_pbhci_hc->TD_Pool,
                     (void *)p_td_to_abort,
                     &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing TD in abort -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                               HOST CONTROLLER DRIVER ROOT HUB FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_InfosGet()
 *
 * @brief    Retrieves information on root hub.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    p_rh_info   Pointer to the root hub info structure to be filled by this function.
 *
 * @return   DEF_OK,     If root hub information successfully retrieved.
 *           DEF_FAIL,   Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_InfosGet(USBH_HC_DRV        *p_hc_drv,
                                        USBH_HUB_ROOT_INFO *p_rh_info)
{
  USBH_PBHCI_HC_API *p_pbhci_hc_api = ((USBH_PBHCI_HC *)p_hc_drv->DataPtr)->HC_Drv.API_Ptr;

  p_rh_info->NbrPort = p_pbhci_hc_api->CfgPtr->RH_PortQty;
  p_rh_info->Characteristics = DEF_BIT_NONE;
  p_rh_info->PwrOn2PwrGood = 10u;

  LOG_VRB(("Root hub info get on host #", (u)p_hc_drv->HostNbr, " HC #", (u)p_hc_drv->Nbr));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_HCD_RH_PortStatusGet()
 *
 * @brief    Retrieve port status changes and port status of Root Hub.
 *
 * @param    p_hc_drv        Pointer to the Host Controller driver.
 *
 * @param    port_nbr        Port number. 0 retrieves HUB status.
 *
 * @param    p_port_status   Pointer to the port Status structure to be filled by this function.
 *
 * @return   DEF_OK,     If port status successfully retrieved.
 *           DEF_FAIL,   Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortStatusGet(USBH_HC_DRV     *p_hc_drv,
                                             CPU_INT08U      port_nbr,
                                             USBH_HUB_STATUS *p_port_status)
{
  USBH_PBHCI_HC   *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_HUB_STATUS *p_rh_port_status;
  USBH_HUB_STATUS *p_rh_port_status_temp;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (port_nbr != USBH_HUB_PORT_HUB_STATUS) {                   // Retrieve status of a specific hub port.
    p_rh_port_status = &p_pbhci_hc->RH_PortStatusTblPtr[port_nbr - 1u];
    p_rh_port_status_temp = &p_pbhci_hc->RH_PortStatusCurTblPtr[port_nbr - 1u];

    //                                                             Apply changes that occurred.
    p_rh_port_status->Chng |= p_rh_port_status_temp->Chng;

    DEF_BIT_FIELD_WR(p_rh_port_status->Status,
                     p_rh_port_status_temp->Status,
                     (CPU_INT16U)USBH_HUB_STATUS_PORT_CONN);
    DEF_BIT_FIELD_WR(p_rh_port_status->Status,
                     p_rh_port_status_temp->Status,
                     (CPU_INT16U)USBH_HUB_STATUS_PORT_OVER_CUR);

    if (DEF_BIT_IS_CLR(p_rh_port_status->Status, USBH_HUB_STATUS_PORT_CONN) == DEF_YES) {
      DEF_BIT_CLR(p_rh_port_status->Status, (CPU_INT16U)USBH_HUB_STATUS_PORT_EN);
    }

    p_rh_port_status_temp->Chng = DEF_BIT_NONE;
  } else {                                                      // Retrieve status of root hub.
    p_rh_port_status = &p_pbhci_hc->RH_Status;
    p_rh_port_status_temp = &p_pbhci_hc->RH_StatusCur;

    //                                                             Apply changes that occurred.
    p_rh_port_status->Chng |= p_rh_port_status_temp->Chng;

    DEF_BIT_FIELD_WR(p_rh_port_status->Status,
                     p_rh_port_status_temp->Status,
                     (CPU_INT16U)USBH_HUB_STATUS_OVER_CUR);

    p_rh_port_status_temp->Chng = DEF_BIT_NONE;
  }

  p_port_status->Status = p_rh_port_status->Status;
  p_port_status->Chng = p_rh_port_status->Chng;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("Port status get host #", (u)p_hc_drv->HostNbr,
           " HC #", (u)p_hc_drv->Nbr,
           " port #", (u)port_nbr));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_PortReq()
 *
 * @brief    Executes root hub port request.
 *
 * @param    p_hc_drv    Pointer to the Host Controller driver.
 *
 * @param    port_nbr    Port number. 0 retrieveS HUB status.
 *
 * @param    req         Request.
 *
 * @param    feature     Feature.
 *
 * @return   DEF_OK,     If port request successfully executed.
 *           DEF_FAIL,   Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortReq(USBH_HC_DRV *p_hc_drv,
                                       CPU_INT08U  port_nbr,
                                       CPU_INT08U  req,
                                       CPU_INT16U  feature)
{
  CPU_BOOLEAN       ok = DEF_OK;
  USBH_DEV_SPD      dev_spd;
  USBH_HUB_STATUS   *p_rh_port_status;
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  CORE_DECLARE_IRQ_STATE;

  p_rh_port_status = (port_nbr != USBH_HUB_PORT_HUB_STATUS) ? &p_pbhci_hc->RH_PortStatusTblPtr[port_nbr - 1u]
                     : &p_pbhci_hc->RH_Status;

  LOG_VRB(("Port req on host #", (u)p_hc_drv->HostNbr,
           " HC #", (u)p_hc_drv->Nbr,
           " port #", (u)port_nbr,
           " req ", (X)req));

  switch (req) {
    case USBH_DEV_REQ_SET_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdPortSuspendSet);

          ok = p_pbhci_hc_api->PortSuspend(p_pbhci_hc_drv,
                                           port_nbr,
                                           DEF_SET);
          if (ok == DEF_OK) {
            CORE_ENTER_ATOMIC();
            DEF_BIT_SET(p_rh_port_status->Status,
                        USBH_HUB_STATUS_PORT_SUSPEND);
            CORE_EXIT_ATOMIC();
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_RESET:
          USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdPortResetSet);

          ok = p_pbhci_hc_api->PortResetSet(p_pbhci_hc_drv,
                                            port_nbr);
          if (ok == DEF_OK) {
            CORE_ENTER_ATOMIC();
            DEF_BIT_SET(p_rh_port_status->Status,
                        USBH_HUB_STATUS_PORT_RESET);

            DEF_BIT_SET(p_rh_port_status->Status,
                        USBH_HUB_STATUS_PORT_EN);

            DEF_BIT_SET(p_rh_port_status->Chng,
                        USBH_HUB_STATUS_C_PORT_RESET);
            CORE_EXIT_ATOMIC();
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          CORE_ENTER_ATOMIC();
          DEF_BIT_SET(p_rh_port_status->Status,
                      USBH_HUB_STATUS_PORT_PWR);
          CORE_EXIT_ATOMIC();
          break;

        case USBH_HUB_FEATURE_SEL_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_PORT_EN:
        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
        default:
          break;
      }
      break;

    case USBH_DEV_REQ_CLR_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdPortSuspendClr);

          ok = p_pbhci_hc_api->PortSuspend(&p_pbhci_hc->HC_Drv,
                                           port_nbr,
                                           DEF_CLR);
          if (ok == DEF_OK) {
            CORE_ENTER_ATOMIC();
            DEF_BIT_CLR(p_rh_port_status->Status,
                        (CPU_INT16U)USBH_HUB_STATUS_PORT_SUSPEND);
            CORE_EXIT_ATOMIC();
          }
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
          CORE_ENTER_ATOMIC();
          DEF_BIT_CLR(p_rh_port_status->Chng,
                      (CPU_INT16U)USBH_HUB_STATUS_C_PORT_CONN);
          CORE_EXIT_ATOMIC();
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
          CORE_ENTER_ATOMIC();
          DEF_BIT_CLR(p_rh_port_status->Chng,
                      (CPU_INT16U)USBH_HUB_STATUS_C_OVER_CUR);
          CORE_EXIT_ATOMIC();
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
          USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, HcdPortResetClr);

          ok = p_pbhci_hc_api->PortResetClr(p_pbhci_hc_drv,
                                            port_nbr,
                                            &dev_spd);

          p_rh_port_status->Status &= ~USBH_HUB_STATUS_PORT_SPD_MASK;

          switch (dev_spd) {
            case USBH_DEV_SPD_LOW:
              CORE_ENTER_ATOMIC();
              DEF_BIT_SET(p_rh_port_status->Status,
                          USBH_HUB_STATUS_PORT_SPD_LOW);
              CORE_EXIT_ATOMIC();
              break;

            case USBH_DEV_SPD_FULL:
              CORE_ENTER_ATOMIC();
              DEF_BIT_SET(p_rh_port_status->Status,
                          USBH_HUB_STATUS_PORT_SPD_FULL);
              CORE_EXIT_ATOMIC();
              break;

            case USBH_DEV_SPD_HIGH:
              CORE_ENTER_ATOMIC();
              DEF_BIT_SET(p_rh_port_status->Status,
                          USBH_HUB_STATUS_PORT_SPD_HIGH);
              CORE_EXIT_ATOMIC();
              break;

            case USBH_DEV_SPD_NONE:
            default:
              CORE_ENTER_ATOMIC();                              // Update cur RH port status to clr conn status.
              DEF_BIT_CLR(p_pbhci_hc->RH_PortStatusCurTblPtr[port_nbr - 1u].Status,
                          (CPU_INT16U)USBH_HUB_STATUS_PORT_CONN);
              CORE_EXIT_ATOMIC();
              break;
          }

          CORE_ENTER_ATOMIC();
          DEF_BIT_CLR(p_rh_port_status->Chng,
                      (CPU_INT16U)USBH_HUB_STATUS_C_PORT_RESET);

          DEF_BIT_CLR(p_rh_port_status->Status,
                      (CPU_INT16U)USBH_HUB_STATUS_PORT_RESET);
          CORE_EXIT_ATOMIC();

          //                                                       Update current RH speed.
          if ((p_pbhci_hc_api->CfgPtr->RH_PortQty == 1u)
              && (p_pbhci_hc->RH_SpdCur != dev_spd)) {
            p_pbhci_hc->RH_SpdCur = dev_spd;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
            {
              CPU_INT08U cnt;

              //                                                   Update max bw available according to new speed.
              for (cnt = 0u; cnt < p_pbhci_hc->BranchQty; cnt++) {
                p_pbhci_hc->PeriodicBranchTblPtr[cnt].RemBandwidth = USBH_HC_MAX_PERIODIC_BW_GET(p_pbhci_hc->RH_SpdCur);
              }
            }
#endif
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          CORE_ENTER_ATOMIC();
          DEF_BIT_CLR(p_rh_port_status->Status,
                      (CPU_INT16U)USBH_HUB_STATUS_PORT_PWR);
          CORE_EXIT_ATOMIC();
          break;

        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_PORT_RESET:
        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
        case USBH_HUB_FEATURE_SEL_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_PORT_EN:
        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
        default:
          break;
      }
      break;

    case USBH_DEV_REQ_GET_STATUS:
    case USBH_DEV_REQ_SET_ADDR:
    case USBH_DEV_REQ_GET_DESC:
    case USBH_DEV_REQ_SET_DESC:
    case USBH_DEV_REQ_GET_CFG:
    case USBH_DEV_REQ_SET_CFG:
    case USBH_DEV_REQ_GET_IF:
    case USBH_DEV_REQ_SET_IF:
    case USBH_DEV_REQ_SYNCH_FRAME:
      break;

    default:
      ok = DEF_FAIL;
      break;
  }

  return (ok);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_IntEn()
 *
 * @brief    Enables/disables root hub interrupt.
 *
 * @param    p_hc_drv    Pointer to the driver structure
 *
 * @param    en          Flags that indicates if root hub interrupts must enabled or disabled.
 *                       DEF_ENABLED     Enable  root hub interruptions.
 *                       DEF_DISABLED    Disable root hub interruptions.
 *
 * @return   DEF_OK,     If interrupt successfully enabled/disabled.
 *           DEF_FAIL,   Otherwise.
 *
 * @note     (1) Hub class asks to disable Root hub interruptions when it has an event to process on
 *               this hub. Since it is not easily possible to disable root hub interrupts on most of
 *               the Pipe-Based Host Controllers, the status and status change are simply updated
 *               locally and reported to the hub class when it calls the PortStatusGet() function.
 *               If any port status changes occur during the time the root hub interruptions are
 *               disabled, they are reported to the hub class when it re-enables the root hub
 *               interruptions.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_IntEn(USBH_HC_DRV *p_hc_drv,
                                     CPU_BOOLEAN en)
{
  CPU_INT08U        port_cnt;
  CPU_INT08U        port_chng = DEF_BIT_NONE;
  CPU_INT16U        port_status_chng;
  USBH_PBHCI_HC     *p_pbhci_hc = (USBH_PBHCI_HC *)p_hc_drv->DataPtr;
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  CORE_DECLARE_IRQ_STATE;

  if (en == DEF_DISABLED) {                                     // RH int dis intentionally ignored.
    return (DEF_OK);
  }

  CORE_ENTER_ATOMIC();                                          // Check if any port changes to report.
  for (port_cnt = 0u; port_cnt < p_pbhci_hc->HC_Drv.API_Ptr->CfgPtr->RH_PortQty; port_cnt++) {
    port_status_chng = p_pbhci_hc->RH_PortStatusTblPtr[port_cnt].Chng;
    port_status_chng |= p_pbhci_hc->RH_PortStatusCurTblPtr[port_cnt].Chng;

    if (port_status_chng != DEF_BIT_NONE) {
      DEF_BIT_SET(port_chng, DEF_BIT(port_cnt + 1u));
    }
  }

  port_status_chng = p_pbhci_hc->RH_Status.Chng;                // Check if any hub chng to report.
  port_status_chng |= p_pbhci_hc->RH_StatusCur.Chng;

  if (port_status_chng != DEF_BIT_NONE) {
    DEF_BIT_SET(port_chng, DEF_BIT_00);
  }

  if (port_chng != DEF_BIT_NONE) {
    CORE_EXIT_ATOMIC();

    USBH_HUB_RootEvent(p_pbhci_hc_drv->HostNbr,
                       p_pbhci_hc_drv->HC_Nbr,
                       port_chng);
  } else {
    p_pbhci_hc->RH_IntEn = DEF_ENABLED;
    CORE_EXIT_ATOMIC();
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_PBHCI_EventPipeXferCmpl()
 *
 * @brief    Transfer complete event.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI Host Controller driver.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    status          Bitmap indicating some status flags.
 *
 * @param    err             Transfer error code. RTOS_ERR_NONE when transfer was successful.
 *******************************************************************************************************/
static void USBH_PBHCI_EventPipeXferCmpl(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                         CPU_INT08U        pipe_nbr,
                                         CPU_INT32U        xfer_len,
                                         CPU_INT08U        status,
                                         RTOS_ERR          err)
{
  USBH_PBHCI_EVENT_XFER_CMPL *p_pbhci_event_xfer_cmpl;

  LOG_VRB(("Pipe xfer cmpl on host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " pipe #", (u)pipe_nbr));

  p_pbhci_event_xfer_cmpl = (USBH_PBHCI_EVENT_XFER_CMPL *)USBH_PBHCI_EventGet((USBH_PBHCI_EVENT **)&USBH_PBHCI_Ptr->EventXferCmplFreeHeadPtr);

  if (p_pbhci_event_xfer_cmpl != DEF_NULL) {
    RTOS_ERR err_local;

    p_pbhci_event_xfer_cmpl->PBHCI_HC_DrvPtr = p_pbhci_hc_drv;
    p_pbhci_event_xfer_cmpl->PipeNbr = pipe_nbr;
    p_pbhci_event_xfer_cmpl->Status = status;
    p_pbhci_event_xfer_cmpl->XferLen = xfer_len;
    RTOS_ERR_COPY(p_pbhci_event_xfer_cmpl->Err, err);

    USBH_PBHCI_EventQAdd((USBH_PBHCI_EVENT *)p_pbhci_event_xfer_cmpl);

    KAL_SemPost(USBH_PBHCI_Ptr->SchedSemHandle,
                KAL_OPT_POST_NONE,
                &err_local);
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
      LOG_ERR(("Posting PBHCI event", RTOS_ERR_LOG_ARG_GET(err_local)));
    }
  } else {
    LOG_ERR(("Allocation of PBHCI event for xfer cmpl event failed", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_ALLOC)));
  }
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_SchedTaskHandler()
 *
 * @brief    Scheduler task handler.
 *
 * @param    p_arg   Pointer to the task's variable.
 *******************************************************************************************************/
static void USBH_PBHCI_SchedTaskHandler(void *p_arg)
{
  (void)&p_arg;

  while (DEF_ON) {
    USBH_PBHCI_HC *p_pbhci_hc;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    CPU_INT16U frm_nbr;
#endif
    CPU_INT08U        pipe_nbr;
    CPU_INT08U        status;
    CPU_INT32U        xfer_len;
    USBH_PBHCI_HC_DRV *p_pbhci_hc_drv;
    USBH_PBHCI_EVENT  *p_pbhci_event;
    RTOS_ERR          err;
    CORE_DECLARE_IRQ_STATE;

    KAL_SemPend(USBH_PBHCI_Ptr->SchedSemHandle,                 // Wait for an event to occur.
                KAL_OPT_PEND_NONE,
                0u,
                &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Pending on async event -> ", RTOS_ERR_LOG_ARG_GET(err)));
      continue;
    }

    CORE_ENTER_ATOMIC();                                        // Retrieve next event to process.
    p_pbhci_event = USBH_PBHCI_Ptr->EventQHeadPtr;
    if (p_pbhci_event == DEF_NULL) {
      CORE_EXIT_ATOMIC();
      continue;
    }

    p_pbhci_hc_drv = p_pbhci_event->PBHCI_HC_DrvPtr;
    CORE_EXIT_ATOMIC();

    p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->HC_Nbr);
    USBH_PBHCI_HC_Lock(p_pbhci_hc);

    CORE_ENTER_ATOMIC();
    USBH_PBHCI_Ptr->EventQHeadPtr = p_pbhci_event->NextPtr;

    if (USBH_PBHCI_Ptr->EventQHeadPtr == DEF_NULL) {
      USBH_PBHCI_Ptr->EventQTailPtr = DEF_NULL;
    }
    CORE_EXIT_ATOMIC();

    USBH_DBG_PBHCI_STATS_GLOBAL_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, SchedEventTot);

    switch (p_pbhci_event->Type) {
      case USBH_PBHCI_EVENT_TYPE_PIPE_CMPL:                     // Data transfer completed on pipe.
        USBH_DBG_PBHCI_STATS_SCHED_EVENT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_PBHCI_EVENT_TYPE_PIPE_CMPL);

        pipe_nbr = ((USBH_PBHCI_EVENT_XFER_CMPL *)p_pbhci_event)->PipeNbr;
        status = ((USBH_PBHCI_EVENT_XFER_CMPL *)p_pbhci_event)->Status;
        xfer_len = ((USBH_PBHCI_EVENT_XFER_CMPL *)p_pbhci_event)->XferLen;
        RTOS_ERR_COPY(err, ((USBH_PBHCI_EVENT_XFER_CMPL *)p_pbhci_event)->Err);

        USBH_PBHCI_EventFree((USBH_PBHCI_EVENT **)&USBH_PBHCI_Ptr->EventXferCmplFreeHeadPtr,
                             p_pbhci_event);

        USBH_PBHCI_XferCmplProcess(p_pbhci_hc_drv,
                                   pipe_nbr,
                                   xfer_len,
                                   status,
                                   err);
        break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
      case USBH_PBHCI_EVENT_TYPE_SOF:                           // Start-Of-Frame occurred.
        USBH_DBG_PBHCI_STATS_SCHED_EVENT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_PBHCI_EVENT_TYPE_SOF);

        frm_nbr = ((USBH_PBHCI_EVENT_SOF *)p_pbhci_event)->FrmNbr;

        USBH_PBHCI_EventFree((USBH_PBHCI_EVENT **)&USBH_PBHCI_Ptr->EventSOF_FreeHeadPtr,
                             p_pbhci_event);

        USBH_PBHCI_PeriodicSched(p_pbhci_hc_drv,
                                 frm_nbr);
        break;
#endif

      case USBH_PBHCI_EVENT_TYPE_NONE:
      default:
        USBH_DBG_PBHCI_STATS_SCHED_EVENT_INC(p_pbhci_hc_drv->HostNbr, p_pbhci_hc_drv->HC_Nbr, USBH_PBHCI_EVENT_TYPE_NONE);
        LOG_ERR(("Unknown PBHCI event type."));
        break;
    }

    USBH_PBHCI_HC_Unlock(p_pbhci_hc);
  }
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_PeriodicSched()
 *
 * @brief    Handles scheduling of periodic transfers (interrupt and isochronous).
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI HC driver.
 *
 * @param    frm_nbr         Frame number.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_PBHCI_PeriodicSched(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                     CPU_INT16U        frm_nbr)
{
  CPU_INT08U    interval_nbr = frm_nbr;
  CPU_INT08U    interval_qty;
  CPU_INT08U    level_cur;
  CPU_INT16U    btree_ix_cnt = 0u;
  USBH_PBHCI_HC *p_pbhci_hc;

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  interval_qty = p_pbhci_hc->BranchQty;
  level_cur = p_pbhci_hc->LevelQty;

  do {
    USBH_PBHCI_ED            *p_ed;
    USBH_PBHCI_PERIODIC_NODE *p_node;

    p_node = &p_pbhci_hc->PeriodicNodeTblPtr[btree_ix_cnt + interval_nbr];
    p_ed = p_node->ED_HeadPtr;
    while ((p_ed != DEF_NULL)
           && (p_ed->Suspended == DEF_NO)) {
      USBH_PBHCI_TD *p_td = p_ed->TD_HeadPtr;

      if (p_td != DEF_NULL) {
        switch (p_ed->EP_Params.Type) {
          case USBH_EP_TYPE_INTR:
            if (p_td->IsSubmitted == DEF_NO) {
              USBH_PBHCI_IntrTD_Submit(p_pbhci_hc, p_td);
            }
            break;

#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
          case USBH_EP_TYPE_ISOC:
            USBH_PBHCI_IsocTD_Submit(p_pbhci_hc, p_td);
            break;
#endif

          case USBH_EP_TYPE_CTRL:
          case USBH_EP_TYPE_BULK:
          default:
            break;
        }
      }

      p_ed = p_ed->NextPtr;
    }

    btree_ix_cnt += interval_qty;
    interval_qty = interval_qty >> 1u;                          // Divide interval by 2.
    level_cur--;
    if (interval_nbr > 0u) {
      DEF_BIT_CLR(interval_nbr, DEF_BIT(level_cur - 1u));
    }
  } while (interval_qty >= 1u);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_PBHCI_XferCmplProcess()
 *
 * @brief    Completes a USB transfer.
 *
 * @param    p_pbhci_hc_drv  Pointer to the PBHCI HC driver.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    xfer_len        Length of the transfer, in bytes.
 *
 * @param    status          Bitmap indicating some status flags.
 *
 * @param    err             Error code from transfer.
 *******************************************************************************************************/
static void USBH_PBHCI_XferCmplProcess(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       CPU_INT08U        pipe_nbr,
                                       CPU_INT32U        xfer_len,
                                       CPU_INT08U        status,
                                       RTOS_ERR          err)
{
  CPU_INT32U        rx_len = 0u;
  USBH_PBHCI_HC     *p_pbhci_hc;
  USBH_PBHCI_PIPE   *p_pipe;
  USBH_PBHCI_ED     *p_ed;
  USBH_PBHCI_TD     *p_td_cur;
  USBH_PBHCI_HC_API *p_pbhci_hc_api;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr;
  CPU_INT08U ep_phy_nbr;
#endif

  USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, XferCmplProcessTot);

  p_pbhci_hc = USBH_PBHCI_HC_PtrGet(p_pbhci_hc_drv->PBHCI_HC_Nbr);
  p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  p_pipe = p_pbhci_hc->PipePtrTblPtr[pipe_nbr];

  LOG_VRB(("Xfer cmpl process host #", (u)p_pbhci_hc_drv->HostNbr,
           " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
           " pipe #", (u)pipe_nbr));

  p_ed = p_pipe->CurED_Ptr;
  if ((p_ed == DEF_NULL)
      || (p_ed->EP_Handle != p_pipe->CurEP_Handle)) {
    USBH_DBG_PBHCI_STATS_GLOBAL_INC(host_nbr, hc_nbr, XferCmplEdNotFound);
    return;                                                     // EP is closed.
  }
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif
  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, XferCmplProcessTot);

  p_td_cur = p_ed->TD_HeadPtr;
  if ((p_td_cur == DEF_NULL)
      || (p_td_cur->IsSubmitted == DEF_NO)) {
    USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_XFER_CMPL_TD_ERR);
    return;
  }

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(p_td_cur->Err, err);                          // If err during xfer, stop + notify core end of xfer.

    USBH_PBHCI_ED_TD_Pop(p_ed);

    if ((p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL)
        && (p_ed->EP_Params.Type != USBH_EP_TYPE_ISOC)) {
      USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
    }
    USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_ERR);
    USBH_URB_Done(p_ed->DevHandle,
                  p_ed->EP_Handle);

    goto sched_xfer;
  }

  //                                                               ----------------- PROCESS IN XFER ------------------
  if (p_td_cur->Token == USBH_TOKEN_IN) {
    CPU_INT08U pipe_rd_status;

    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeRd);
    rx_len = p_pbhci_hc_api->PipeRd(p_pbhci_hc_drv,             // Read data from pipe.
                                    pipe_nbr,
                                    &p_td_cur->BufPtr[p_td_cur->XferLen],
                                    p_td_cur->BufLen - p_td_cur->XferLen,
                                    &pipe_rd_status,
                                    &p_td_cur->Err);
    if (RTOS_ERR_CODE_GET(p_td_cur->Err) != RTOS_ERR_NONE) {
      //                                                           PBHCD reports that xfer is complete.
      if ((p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL)
          && (p_ed->EP_Params.Type != USBH_EP_TYPE_ISOC)) {
        USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
      }
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_IN_ERR);

      USBH_URB_Done(p_ed->DevHandle,
                    p_ed->EP_Handle);

      goto sched_xfer;
    }

    p_td_cur->XferLen += rx_len;

    //                                                             Chk if xfer complete.
    if (p_ed->EP_Params.Type == USBH_EP_TYPE_ISOC) {
      RTOS_ERR_SET(p_td_cur->Err, RTOS_ERR_NONE);
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_IN_ISOC);
      USBH_URB_Done(p_ed->DevHandle,
                    p_ed->EP_Handle);
    } else if ((DEF_BIT_IS_CLR(status, USBH_PBHCI_XFER_STATUS_NACK)         )
               && (((rx_len % p_ed->EP_Params.MaxPktSize) != 0u)
                   || (p_td_cur->XferLen >= p_td_cur->BufLen)
                   || (rx_len == 0u)
                   || (DEF_BIT_IS_SET(pipe_rd_status, USBH_PBHCI_XFER_STATUS_CMPL)))) {
      //                                                           Xfer is complete.
      RTOS_ERR_SET(p_td_cur->Err, RTOS_ERR_NONE);

      USBH_PBHCI_ED_TD_Pop(p_ed);

      if (p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL) {
        USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
      }
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_IN_XFER_CMPL);
      USBH_URB_Done(p_ed->DevHandle,
                    p_ed->EP_Handle);
    } else if ((p_ed->EP_Params.Type == USBH_EP_TYPE_CTRL)
               || (p_ed->EP_Params.Type == USBH_EP_TYPE_BULK)) {
      USBH_PBHCI_PipeRxStart(p_pbhci_hc,                        // Receive next chunk of data.
                             p_pipe,
                             p_td_cur,
                             &p_td_cur->Err);
      if (RTOS_ERR_CODE_GET(p_td_cur->Err) == RTOS_ERR_NONE) {
        USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_XFER_CMPL_IN_CTRL_BULK);
        return;
      } else {
        if (p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL) {
          USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
        }
        USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_IN_CTRL_BULK);
        USBH_URB_Done(p_ed->DevHandle,
                      p_ed->EP_Handle);
      }
    } else {
      USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);

      p_td_cur->IsSubmitted = DEF_NO;
      USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_XFER_CMPL_IN_ELSE);
    }

    //                                                             ----------------- PROCESS OUT XFER -----------------
  } else {
    CPU_INT32U tx_len;

    tx_len = (xfer_len == USBH_PBHCI_XFER_LEN_ALL) ? p_td_cur->XferLenNext : xfer_len;

    //                                                             Chk if xfer complete.
    p_td_cur->XferLen += tx_len;
    if (p_ed->EP_Params.Type == USBH_EP_TYPE_ISOC) {
      RTOS_ERR_SET(p_td_cur->Err, RTOS_ERR_NONE);
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_OUT_ISOC);

      USBH_URB_Done(p_ed->DevHandle,
                    p_ed->EP_Handle);
    } else if (p_td_cur->XferLen >= p_td_cur->BufLen) {
      //                                                           Xfer complete.
      RTOS_ERR_SET(p_td_cur->Err, RTOS_ERR_NONE);

      USBH_PBHCI_ED_TD_Pop(p_ed);

      if (p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL) {
        USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
      }
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_OUT_BUF_LEN);

      USBH_URB_Done(p_ed->DevHandle,
                    p_ed->EP_Handle);
    } else if ((p_ed->EP_Params.Type == USBH_EP_TYPE_CTRL)
               || (p_ed->EP_Params.Type == USBH_EP_TYPE_BULK)) {
      //                                                           Transmit next chunk of data.
      USBH_PBHCI_PipeTx(p_pbhci_hc,
                        p_pipe,
                        p_td_cur,
                        &p_td_cur->Err);
      if (RTOS_ERR_CODE_GET(p_td_cur->Err) == RTOS_ERR_NONE) {
        USBH_DBG_PBHCI_STATS_XFER_CMPL_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_XFER_CMPL_OUT_CTRL_BULK);
        return;
      } else {
        if (p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL) {
          USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
        }
        USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_OUT_CTRL_BULK);

        USBH_URB_Done(p_ed->DevHandle,
                      p_ed->EP_Handle);
      }
    } else {
      USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);

      p_td_cur->IsSubmitted = DEF_NO;
    }
  }

sched_xfer:
  switch (p_ed->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
      USBH_PBHCI_NextCtrlSubmit(p_pbhci_hc);
      break;

    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      USBH_PBHCI_NextBulkSubmit(p_pbhci_hc);
      break;

    case USBH_EP_TYPE_ISOC:
    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_AsyncListED_Insert()
 *
 * @brief    Inserts an endpoint descriptor into a given asynchronous list.
 *
 * @param    p_async_list    Pointer to the asynchronous list.
 *
 * @param    p_ed            Pointer to the endpoint descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_AsyncListED_Insert(USBH_PBHCI_ASYNC_LIST *p_async_list,
                                          USBH_PBHCI_ED         *p_ed)
{
  if (p_async_list->HeadPtr != DEF_NULL) {
    p_ed->NextPtr = p_async_list->HeadPtr;
  } else {
    p_async_list->CurPtr = p_ed;
  }

  p_async_list->HeadPtr = p_ed;
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_AsyncListED_Remove()
 *
 * @brief    Removes an Endpoint Descriptor from a given asynchronous list.
 *
 * @param    p_async_list    Pointer to the asynchronous list.
 *
 * @param    p_ed            Pointer to the endpoint descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_AsyncListED_Remove(USBH_PBHCI_ASYNC_LIST *p_async_list,
                                          USBH_PBHCI_ED         *p_ed)
{
  USBH_PBHCI_ED *p_ed_cur = p_async_list->CurPtr;
  USBH_PBHCI_ED *p_ed_prev = DEF_NULL;

  do {
    p_ed_prev = p_ed_cur;
    p_ed_cur = p_ed_cur->NextPtr;

    if (p_ed_cur == DEF_NULL) {
      p_ed_cur = p_async_list->HeadPtr;
    }
  } while ((p_ed_cur != p_async_list->CurPtr)
           && (p_ed_cur != p_ed));

  if (p_ed_cur == p_ed) {
    if (p_ed_cur == p_ed_prev) {
      p_async_list->HeadPtr = DEF_NULL;                         // Only ED in list.
      p_async_list->CurPtr = DEF_NULL;
    } else {
      if ((p_ed_prev != DEF_NULL)
          && (p_ed_prev->NextPtr != DEF_NULL)) {
        if (p_ed_prev != p_ed->NextPtr) {
          p_ed_prev->NextPtr = p_ed->NextPtr;
        } else {
          p_ed_prev->NextPtr = DEF_NULL;
        }
      }

      if (p_async_list->HeadPtr == p_ed) {
        p_async_list->HeadPtr = p_ed->NextPtr;
      }

      if (p_async_list->CurPtr == p_ed) {
        p_async_list->CurPtr = p_ed_prev;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                   USBH_PBHCI_PeriodicListED_Insert()
 *
 * @brief    Inserts an Endpoint Descriptor (ED) into the periodic list in function of its max pkt size
 *           and interval values.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_ed        Pointer to the Endpoint Descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @return   Endpoint adjusted interval used by PBHCI.
 *
 * @note     (1) The periodic list is arranged in a binary tree fashion. Below is an example of a 4
 *               level binary tree.
 *
 *                       ->  0   1     2       3
 *                       ->  3   2     1       0
 *                       ->  8   4     2       1     (Also gives number of nodes in level)
 *
 *               B0           O0
 *                              > O8
 *               B1           O4     \
 *                                   > O12
 *               B2           O2     /     \
 *                              > O10       \
 *               B3           O6             \
 *                                           > O14
 *               B4           O1             /
 *                              > O9        /
 *               B5           O5     \     /
 *                                   > O13
 *               B6           O3     /
 *                              > O11
 *               B7           O7
 *
 *               ^            ^
 *               |            |
 *           ranches     First level
 *
 *           - (a) The periodic list is arranged in a binary tree fashion in order to handle the
 *                   interval and bandwidth management correctly.
 *
 *           - (b) The periodic list uses another table that contains information about each
 *                   branch of the binary tree. Each unique path to the last node (in this case O14)
 *                   has a branch. The branch will keep the remaining bandwidth available in the
 *                   branch/path.
 *
 *           - (c) The indexes of the branches are continuous (0-1-2-3-...). However, the indexes of
 *                   the binary tree itself are arranged in order to make sure the scheduling behaves
 *                   correctly.
 *
 *               - (1) At each start of frame, the PBHCI will treat a single path. For instance, if
 *                       the current frame number is 0, the PBHCI will treat the nodes in this order:
 *                       O0->O8->O12->O14. This way, the tranfers Q-ed under EDs of O0 will be treated
 *                       at each 8 frames, the ones under O8 at each 4 frames, the ones under O12 at
 *                       each 2 frames and finally the ones under O14 at each frame.
 *
 * @note     (2) This function must add an ED in a certain node. The node is determined by the
 *               bInterval value of the endpoint. Using example in (1), a endpoint with a bInterval
 *               of 4 frames could be inserted in either O8, O9, O10 or O11. This function will choose
 *               the one that has the highest available bandwidth.
 *
 * @note     (3) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT16U USBH_PBHCI_PeriodicListED_Insert(USBH_PBHCI_HC *p_pbhci_hc,
                                                   USBH_PBHCI_ED *p_ed,
                                                   RTOS_ERR      *p_err)
{
  CPU_INT08U                 level;
  CPU_INT08U                 depth;
  CPU_INT08U                 adjusted_interval;
  CPU_INT08U                 interval_branch_qty;
  CPU_INT08U                 max_bw_interval_ix = 0u;
  CPU_INT08U                 interval_branch_cnt;
  CPU_INT16U                 interval_branch_start_ix;
  CPU_INT16U                 branch_cnt;
  CPU_INT16U                 interval;
  CPU_INT16U                 needed_bw;
  CPU_INT16U                 btree_ix;
  CPU_INT16U                 node_qty_tot;
  CPU_INT16U                 max_bw = 0u;
  CPU_INT08U                 interval_cnt = 0u;
  USBH_PBHCI_PERIODIC_NODE   *p_node;
  USBH_PBHCI_PERIODIC_BRANCH *p_branch;
  CORE_DECLARE_IRQ_STATE;

  interval = p_ed->EP_Params.Interval;
  if (interval > p_pbhci_hc->BranchQty) {
    interval = p_pbhci_hc->BranchQty;
  }

  node_qty_tot = (p_pbhci_hc->BranchQty * 2u) - 1u;
  level = (CPU_CntLeadZeros08((CPU_INT08U)interval) - (8u - p_pbhci_hc->LevelQty));
  depth = (p_pbhci_hc->LevelQty - 1u) - level;
  adjusted_interval = DEF_BIT(depth);
  interval_branch_qty = DEF_BIT(level);
  needed_bw = USBH_PBHCI_ED_BW_Get(&p_ed->EP_Params);

  //                                                               Find interval that has most avail BW.
  for (branch_cnt = 0u; branch_cnt < p_pbhci_hc->BranchQty; branch_cnt += interval_branch_qty) {
    CPU_INT16U interval_min_bw;

    interval_min_bw = USBH_HC_MAX_PERIODIC_BW_GET(p_pbhci_hc->RH_SpdCur);

    //                                                             Find branch for cur interval that has less BW avail.
    for (interval_branch_cnt = 0u; interval_branch_cnt < interval_branch_qty; interval_branch_cnt++) {
      p_branch = &p_pbhci_hc->PeriodicBranchTblPtr[USBH_PBHCI_BTreeIxCompute(p_pbhci_hc->BranchQty, branch_cnt + interval_branch_cnt)];

      interval_min_bw = DEF_MIN(p_branch->RemBandwidth, interval_min_bw);
    }

    if (max_bw < interval_min_bw) {                             // If more avail BW then prev intervals, keep this one.
      max_bw = interval_min_bw;
      max_bw_interval_ix = interval_cnt;
    }

    //                                                             If no BW used for given interval, stop searching now.
    if (interval_min_bw == USBH_HC_MAX_PERIODIC_BW_GET(p_pbhci_hc->RH_SpdCur)) {
      break;
    }

    interval_cnt++;
  }

  if (((CPU_INT16S)(max_bw - needed_bw)) < 0) {                 // Ensure enough BW avail for EP.
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_BW_NOT_AVAIL);
    return (0u);
  }

  p_ed->IntervalIx = max_bw_interval_ix;

  //                                                               Update avail BW for branches has path to interval.
  interval_branch_start_ix = max_bw_interval_ix * interval_branch_qty;
  CORE_ENTER_ATOMIC();
  for (interval_branch_cnt = 0u; interval_branch_cnt < interval_branch_qty; interval_branch_cnt++) {
    p_branch = &p_pbhci_hc->PeriodicBranchTblPtr[USBH_PBHCI_BTreeIxCompute(p_pbhci_hc->BranchQty, interval_branch_start_ix + interval_branch_cnt)];

    p_branch->RemBandwidth -= needed_bw;
  }
  CORE_EXIT_ATOMIC();

  //                                                               Convert branch interval ix to binary tree ix.
  btree_ix = node_qty_tot - (DEF_BIT(depth + 1u) - 1u);
  btree_ix += USBH_PBHCI_BTreeIxCompute(adjusted_interval, max_bw_interval_ix);
  p_node = &p_pbhci_hc->PeriodicNodeTblPtr[btree_ix];

  //                                                               Insert ED in binary tree node.
  if (p_node->ED_HeadPtr != DEF_NULL) {
    p_ed->NextPtr = p_node->ED_HeadPtr;
  }

  p_node->ED_HeadPtr = p_ed;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (adjusted_interval);
}
#endif

/****************************************************************************************************//**
 *                                   USBH_PBHCI_PeriodicListED_Remove()
 *
 * @brief    Removes an Endpoint Descriptor (ED) from the periodic list in function.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_ed        Pointer to the Endpoint Descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_PBHCI_PeriodicListED_Remove(USBH_PBHCI_HC *p_pbhci_hc,
                                             USBH_PBHCI_ED *p_ed)
{
  CPU_INT08U                 level;
  CPU_INT08U                 depth;
  CPU_INT08U                 adjusted_interval;
  CPU_INT08U                 interval_branch_qty;
  CPU_INT08U                 interval_branch_cnt;
  CPU_INT16U                 interval_branch_start_ix;
  CPU_INT16U                 interval;
  CPU_INT16U                 needed_bw;
  CPU_INT16U                 btree_ix;
  CPU_INT16U                 node_qty_tot;
  USBH_PBHCI_ED              *p_ed_cur;
  USBH_PBHCI_ED              *p_ed_prev;
  USBH_PBHCI_PERIODIC_NODE   *p_node;
  USBH_PBHCI_PERIODIC_BRANCH *p_branch;
  CORE_DECLARE_IRQ_STATE;

  interval = p_ed->EP_Params.Interval;
  if (interval > p_pbhci_hc->BranchQty) {
    interval = p_pbhci_hc->BranchQty;
  }

  node_qty_tot = (p_pbhci_hc->BranchQty * 2u) - 1u;
  level = (CPU_CntLeadZeros08((CPU_INT08U)interval) - (8u - p_pbhci_hc->LevelQty));
  depth = (p_pbhci_hc->LevelQty - 1u) - level;
  adjusted_interval = DEF_BIT(depth);
  interval_branch_qty = DEF_BIT(level);
  needed_bw = USBH_PBHCI_ED_BW_Get(&p_ed->EP_Params);

  //                                                               Update avail BW for branches has path to interval.
  interval_branch_start_ix = p_ed->IntervalIx * interval_branch_qty;
  CORE_ENTER_ATOMIC();
  for (interval_branch_cnt = 0u; interval_branch_cnt < interval_branch_qty; interval_branch_cnt++) {
    p_branch = &p_pbhci_hc->PeriodicBranchTblPtr[USBH_PBHCI_BTreeIxCompute(p_pbhci_hc->BranchQty, interval_branch_start_ix + interval_branch_cnt)];

    p_branch->RemBandwidth += needed_bw;
  }
  CORE_EXIT_ATOMIC();

  //                                                               Convert branch interval ix to binary tree ix.
  btree_ix = node_qty_tot - (DEF_BIT(depth + 1u) - 1u);
  btree_ix += USBH_PBHCI_BTreeIxCompute(adjusted_interval, p_ed->IntervalIx);
  p_node = &p_pbhci_hc->PeriodicNodeTblPtr[btree_ix];

  //                                                               Remove ED from binary tree node.
  p_ed_prev = DEF_NULL;

  p_ed_cur = p_node->ED_HeadPtr;
  while ((p_ed_cur != DEF_NULL)
         && (p_ed_cur != p_ed)) {
    p_ed_prev = p_ed_cur;
    p_ed_cur = p_ed_cur->NextPtr;
  }

  if (p_ed_cur != DEF_NULL) {
    if (p_ed_prev != DEF_NULL) {
      p_ed_prev->NextPtr = p_ed->NextPtr;
    }

    if (p_node->ED_HeadPtr == p_ed) {
      p_node->ED_HeadPtr = p_ed->NextPtr;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       USBH_PBHCI_BTreeIxCompute()
 *
 * @brief    Converts an interval index to a binary tree index.
 *
 * @param    interval        Interval.
 *
 * @param    interval_ix     Interval index.
 *
 * @return   Binary tree index.
 *
 * @note     (1) The interval index is a continuous index (0-1-2-...) retrieved after computation of
 *               the bandwidth in the branches. The index for this interval in the binary tree is, on
 *               the other hand, not continuous and organized in a way that helps periodic scheduling.
 *
 *               The following example stands for 16 intervals. This functions simply reverse the
 *               bits of the index.
 *
 *               Interval index    -> 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
 *               Binary tree index -> 0  8  4  12 2  10 6  14 1  9  5  13 3  11 7  15
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT08U USBH_PBHCI_BTreeIxCompute(CPU_INT08U interval,
                                            CPU_INT08U interval_ix)
{
  CPU_INT08U btree_ix = interval_ix;
  CPU_INT08U depth;

  depth = CPU_CntTrailZeros08(interval);
  btree_ix = (((btree_ix & 0xAAu) >> 1u) | ((btree_ix & 0x55u) << 1u));
  btree_ix = (((btree_ix & 0xCCu) >> 2u) | ((btree_ix & 0x33u) << 2u));
  btree_ix = ((btree_ix          >> 4u) |  (btree_ix << 4u));
  btree_ix = (btree_ix >> (DEF_INT_08_NBR_BITS - depth));

  return (btree_ix);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_PBHCI_ED_BW_Get()
 *
 * @brief    Compute needed bandwidth in bytes for given periodic endpoint.
 *
 * @param    p_ep_params     Pointer to the endpoint parameters
 *
 * @return   Bandwidth usage in bytes.
 *
 * @note     (1) The bandwidth usage of a periodic endpoint is its maximum packet size plus protocol
 *               and hardware overhead.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT16U USBH_PBHCI_ED_BW_Get(USBH_HCD_EP_PARAMS *p_ep_params)
{
  CPU_INT16U bw;

  bw = p_ep_params->MaxPktSize;
  bw += USBH_HC_BW_OVERHEAD_GET(p_ep_params->DevSpd, p_ep_params->Type);

  return (bw);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_PBHCI_NextCtrlSubmit()
 *
 * @brief    Finds next control TD to submit from control asynchronous ED list.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_NextCtrlSubmit(USBH_PBHCI_HC *p_pbhci_hc)
{
  USBH_PBHCI_ASYNC_LIST *p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_CTRL_IX];
  USBH_PBHCI_TD         *p_td_next;

  do {
    USBH_PBHCI_ED       *p_ed_next;
    USBH_PBHCI_HC_DRV   *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
    USBH_PBHCI_HC_API   *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
    USBH_PBHCI_PIPE     *p_pipe = &p_pbhci_hc->PipeCtrl;
    USBH_PBHCI_PID_DATA pid_data;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
    CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
    CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
    CPU_INT08U dev_nbr;
    CPU_INT08U ep_phy_nbr;
#endif

    p_td_next = USBH_PBHCI_TdNextAsyncFind(p_async_list);
    if (p_td_next == DEF_NULL) {                                // No more async xfer to submit in async list.
      p_pbhci_hc->CtrlXferInProgress = DEF_NO;

      return;
    }

    p_pbhci_hc->CtrlXferInProgress = DEF_YES;

    p_ed_next = p_td_next->ED_Ptr;
    p_td_next->IsSubmitted = DEF_YES;
    p_pipe->CurED_Ptr = p_ed_next;
    p_pipe->CurEP_Handle = p_ed_next->EP_Handle;
    p_ed_next->PipePtr = p_pipe;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
    dev_nbr = (CPU_INT08U)(p_ed_next->DevHandle) - 1u;
    ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed_next->EP_Handle);
#endif

    //                                                             Configure ctrl pipe.
    pid_data = (p_td_next->Token != USBH_TOKEN_SETUP) ? USBH_PBHCI_PID_DATA1 : USBH_PBHCI_PID_DATA0;
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeCfg);
    p_pbhci_hc_api->PipeCfg(p_pbhci_hc_drv,
                            p_pipe->PipeInfoPtr->Nbr,
                            &p_ed_next->EP_Params,
                            p_td_next->Token,
                            pid_data,
                            &p_td_next->Err);
    if (RTOS_ERR_CODE_GET(p_td_next->Err) != RTOS_ERR_NONE) {
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_NEXT_CTRL_CFG);
      USBH_URB_Done(p_ed_next->DevHandle,
                    p_ed_next->EP_Handle);

      continue;
    }

    USBH_PBHCI_PipeXfer(p_pbhci_hc,
                        p_pipe,
                        p_td_next,
                        &p_td_next->Err);
    if (RTOS_ERR_CODE_GET(p_td_next->Err) != RTOS_ERR_NONE) {
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_NEXT_CTRL_XFER);
      USBH_URB_Done(p_ed_next->DevHandle,
                    p_ed_next->EP_Handle);

      continue;
    }
  } while (RTOS_ERR_CODE_GET(p_td_next->Err) != RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_NextBulkSubmit()
 *
 * @brief    Finds next bulk TD to submit from bulk asynchronous ED list.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_NextBulkSubmit(USBH_PBHCI_HC *p_pbhci_hc)
{
  USBH_PBHCI_ASYNC_LIST *p_async_list = &p_pbhci_hc->AsyncListTbl[USBH_PBHCI_XFER_TYPE_ASYNC_BULK_IX];
  USBH_PBHCI_PIPE       *p_pipe_next;
  USBH_PBHCI_ED         *p_ed_next;
  USBH_PBHCI_TD         *p_td_next;

  do {
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
    CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
    CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
    CPU_INT08U dev_nbr;
    CPU_INT08U ep_phy_nbr;
#endif

    p_td_next = USBH_PBHCI_TdNextAsyncFind(p_async_list);
    if (p_td_next == DEF_NULL) {                                // No more async xfer to submit in async list.
      return;
    }

    p_ed_next = p_td_next->ED_Ptr;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
    dev_nbr = (CPU_INT08U)(p_ed_next->DevHandle) - 1u;
    ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed_next->EP_Handle);
#endif
    p_pipe_next = USBH_PBHCI_PipeAlloc(p_pbhci_hc,
                                       p_ed_next);
    if (p_pipe_next != DEF_NULL) {
      p_td_next->IsSubmitted = DEF_YES;

      USBH_PBHCI_PipeAssoc(p_pbhci_hc,
                           p_pipe_next,
                           p_ed_next,
                           &p_td_next->Err);
      if (RTOS_ERR_CODE_GET(p_td_next->Err) != RTOS_ERR_NONE) {
        USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_NEXT_BULK_ASSOC);
        goto end_xfer_err;
      }

      USBH_PBHCI_PipeXfer(p_pbhci_hc,
                          p_pipe_next,
                          p_td_next,
                          &p_td_next->Err);
      if (RTOS_ERR_CODE_GET(p_td_next->Err) != RTOS_ERR_NONE) {
        USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_NEXT_BULK_XFER);
        goto end_xfer_err;
      }
    }
  } while (p_pipe_next != DEF_NULL);

  return;

end_xfer_err:
  USBH_URB_Done(p_ed_next->DevHandle,
                p_ed_next->EP_Handle);

  USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc,                          // Must mark pipe as uncfg as state in HCD is unknown.
                           p_pipe_next);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_TdNextAsyncFind()
 *
 * @brief    Finds next TD to submit in given asynchronous ED list.
 *
 * @param    p_async_list    Pointer to the asynchronous list.
 *
 * @return   Pointer to the TD to submit.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static USBH_PBHCI_TD *USBH_PBHCI_TdNextAsyncFind(USBH_PBHCI_ASYNC_LIST *p_async_list)
{
  USBH_PBHCI_ED *p_ed_cur = p_async_list->CurPtr;

  if (p_ed_cur == DEF_NULL) {
    return (DEF_NULL);
  }

  do {
    p_ed_cur = p_ed_cur->NextPtr;
    if (p_ed_cur == DEF_NULL) {
      p_ed_cur = p_async_list->HeadPtr;
    }

    if (p_ed_cur->Suspended == DEF_NO) {
      USBH_PBHCI_TD *p_td = p_ed_cur->TD_HeadPtr;

      if ((p_td != DEF_NULL)
          && (p_td->IsSubmitted == DEF_NO)) {
        p_async_list->CurPtr = p_ed_cur;
        return (p_td);
      }
    }
  } while (p_ed_cur != p_async_list->CurPtr);

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_IntrTD_Submit()
 *
 * @brief    Submits an interrupt TD to driver.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_td        Pointer to the Transfer Descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_PBHCI_IntrTD_Submit(USBH_PBHCI_HC *p_pbhci_hc,
                                     USBH_PBHCI_TD *p_td)
{
  USBH_PBHCI_ED   *p_ed = p_td->ED_Ptr;
  USBH_PBHCI_PIPE *p_pipe;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  p_pipe = USBH_PBHCI_PipeAlloc(p_pbhci_hc, p_ed);
  if (p_pipe == DEF_NULL) {
    LOG_DBG(("WARNING -> No pipe avail for intr xfer on dev addr ", (u)p_ed->EP_Params.DevAddr,
             " on endpoint #", (u)USBH_EP_ADDR_TO_LOG(p_ed->EP_Params.Addr)));

    p_td->PipeAllocErrCnt++;
    if (p_td->PipeAllocErrCnt >= USBH_PBHCI_PIPE_ALLOC_ERR_CNT_MAX) {
      RTOS_ERR_SET(p_td->Err, RTOS_ERR_EP_NONE_AVAIL);

      USBH_PBHCI_ED_TD_Pop(p_ed);
      USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_PIPE_ALLOC_MAX);
      USBH_URB_Done(p_ed->DevHandle, p_ed->EP_Handle);
    }

    return;
  }

  p_td->PipeAllocErrCnt = 0u;
  p_td->IsSubmitted = DEF_YES;

  USBH_PBHCI_PipeAssoc(p_pbhci_hc,
                       p_pipe,
                       p_ed,
                       &p_td->Err);
  if (RTOS_ERR_CODE_GET(p_td->Err) != RTOS_ERR_NONE) {          // Assoc failed.
    USBH_PBHCI_ED_TD_Pop(p_ed);
    USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_PIPE_ASSOC);
    USBH_URB_Done(p_ed->DevHandle, p_ed->EP_Handle);
    USBH_PBHCI_PipeFreeUncfg(p_pbhci_hc, p_pipe);               // Must mark pipe as uncfg as state in HCD is unknown.

    return;
  }

  USBH_PBHCI_PipeXfer(p_pbhci_hc,
                      p_pipe,
                      p_td,
                      &p_td->Err);
  if (RTOS_ERR_CODE_GET(p_td->Err) != RTOS_ERR_NONE) {
    USBH_PBHCI_ED_TD_Pop(p_ed);
    USBH_DBG_PBHCI_STATS_URB_DONE_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, USBH_DBG_PBHCI_URB_DONE_PIPE_XFER);
    USBH_URB_Done(p_ed->DevHandle, p_ed->EP_Handle);
    USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
  }
}
#endif

/****************************************************************************************************//**
 *                                       USBH_PBHCI_IsocTD_Submit()
 *
 * @brief    Submits an isochronous TD to driver.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_td        Pointer to the Transfer Descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
  && (USBH_CFG_ISOC_EN == DEF_ENABLED))
static void USBH_PBHCI_IsocTD_Submit(USBH_PBHCI_HC *p_pbhci_hc,
                                     USBH_PBHCI_TD *p_td)
{
  USBH_PBHCI_ED   *p_ed;
  USBH_PBHCI_PIPE *p_pipe;

  p_ed = p_td->ED_Ptr;
  p_pipe = p_ed->PipePtr;

  USBH_PBHCI_PipeXfer(p_pbhci_hc,
                      p_pipe,
                      p_td,
                      &p_td->Err);

  USBH_PBHCI_ED_TD_Pop(p_ed);

  if (RTOS_ERR_CODE_GET(p_td->Err) != RTOS_ERR_NONE) {
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc->HC_Drv.HostNbr,
                                      p_pbhci_hc->HC_Drv.HC_Nbr,
                                      (CPU_INT08U)(p_ed->DevHandle) - 1u,
                                      USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle),
                                      PipeFree);
    USBH_URB_Done(p_ed->DevHandle, p_ed->EP_Handle);
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_PBHCI_ED_TD_Insert()
 *
 * @brief    Inserts a TD to ED's TD list.
 *
 * @param    p_ed    Pointer to the Endpoint Descriptor.
 *
 * @param    p_td    Pointer to the Transfer Descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_ED_TD_Insert(USBH_PBHCI_ED *p_ed,
                                    USBH_PBHCI_TD *p_td)
{
  //                                                               Insert TD at tail of ED's list.
  if (p_ed->TD_TailPtr != DEF_NULL) {
    p_ed->TD_TailPtr->NextPtr = p_td;
  } else {
    p_ed->TD_HeadPtr = p_td;
  }

  p_ed->TD_TailPtr = p_td;
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_ED_TD_Pop()
 *
 * @brief    Removes first TD from ED's TD list.
 *
 * @param    p_ed    Pointer to the Endpoint Descriptor.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_ED_TD_Pop(USBH_PBHCI_ED *p_ed)
{
  USBH_PBHCI_TD *p_td;

  p_td = p_ed->TD_HeadPtr;
  p_ed->TD_HeadPtr = p_td->NextPtr;
  if (p_ed->TD_HeadPtr == DEF_NULL) {
    p_ed->TD_TailPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeAlloc()
 *
 * @brief    Allocates a pipe for given Endpoint Descriptor.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_ed        Pointer to the Endpoint Descriptor.
 *
 * @return   Pointer to the pipe.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *
 * @note     (2) This function will not configure or associate a pipe to the Endpoint Descriptor.
 *               It will only remove it from the free list. The function USBH_PBHCI_PipeAssoc() must be
 *               called to configure the pipe.
 *
 * @note     (3) This function will try to allocate the pipe that was previously associated to this
 *               Endpoint Descriptor (if any). If no pipe was previously associated to this ED or if
 *               it is not currently free, it will allocate the one at the head of the list.
 *******************************************************************************************************/
static USBH_PBHCI_PIPE *USBH_PBHCI_PipeAlloc(USBH_PBHCI_HC *p_pbhci_hc,
                                             USBH_PBHCI_ED *p_ed)
{
  CPU_INT08U      pipe_type_ix;
  CPU_INT08U      pipe_type;
  USBH_PBHCI_PIPE *p_pipe;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAllocEnter);
  //                                                               Find a free pipe that matches.
  if ((p_ed->PipePtr != DEF_NULL)                               // Attempts to use prev pipe assoc to ED. See note (3).
      && (p_ed->PipePtr->CurED_Ptr == p_ed)) {
    p_pipe = p_ed->PipePtr;
  } else {
    pipe_type = (p_ed->PipeAttr & USBH_PIPE_INFO_TYPE_ALL);
    pipe_type_ix = CPU_CntTrailZeros(pipe_type);
    p_pipe = p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix];

    if (p_pipe == DEF_NULL) {                                   // No free pipe avail at the moment.
      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAllocError);
      return (DEF_NULL);
    }
  }

  //                                                               Remove pipe from each list it is part of.
  USBH_PBHCI_PipeListRemove(p_pbhci_hc, p_pipe);

  LOG_VRB(("Pipe alloc host #", (u)p_pbhci_hc->HC_Drv.HostNbr,
           " HC #", (u)p_pbhci_hc->HC_Drv.HC_Nbr,
           " endpoint log #", (u)USBH_EP_ADDR_TO_LOG(p_ed->EP_Params.Addr),
           " pipe #", (u)p_pipe->PipeInfoPtr->Nbr));
  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAllocSuccess);

  return (p_pipe);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_PipeListRemove()
 *
 * @brief    Removes a pipe from the pipe free list(s).
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe to remove.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeListRemove(USBH_PBHCI_HC   *p_pbhci_hc,
                                      USBH_PBHCI_PIPE *p_pipe)
{
  CPU_INT08U      pipe_type_ix;
  CPU_INT08U      pipe_type_temp;
  USBH_PBHCI_PIPE *p_pipe_head;
  USBH_PBHCI_PIPE *p_pipe_tail;
  USBH_PBHCI_PIPE *p_pipe_cfg;

  //                                                               Remove pipe from each list it is part of.
  pipe_type_temp = (p_pipe->PipeInfoPtr->Attrib & USBH_PIPE_INFO_TYPE_ALL);
  while (pipe_type_temp != DEF_BIT_NONE) {
    pipe_type_ix = CPU_CntTrailZeros(pipe_type_temp);

    p_pipe_head = p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix];
    p_pipe_tail = p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix];
    p_pipe_cfg = p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix];

    if (p_pipe_head == p_pipe) {                                // First pipe of list.
      p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix] = p_pipe->PipeNextPtrTbl[pipe_type_ix];

      if (p_pipe->PipeNextPtrTbl[pipe_type_ix] != DEF_NULL) {
        p_pipe->PipeNextPtrTbl[pipe_type_ix]->PipePrevPtrTbl[pipe_type_ix] = DEF_NULL;
      }
    }

    if (p_pipe_tail == p_pipe) {                                // Last pipe of list.
      p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix] = p_pipe->PipePrevPtrTbl[pipe_type_ix];

      if (p_pipe->PipePrevPtrTbl[pipe_type_ix] != DEF_NULL) {
        p_pipe->PipePrevPtrTbl[pipe_type_ix]->PipeNextPtrTbl[pipe_type_ix] = DEF_NULL;
      }
    }

    if (p_pipe_cfg == p_pipe) {                                 // Cfg pipe ptr points to pipe selected.
      p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix] = p_pipe->PipeNextPtrTbl[pipe_type_ix];
    }

    //                                                             Adjust list.
    if ((p_pipe->PipeNextPtrTbl[pipe_type_ix] != DEF_NULL)
        && (p_pipe->PipePrevPtrTbl[pipe_type_ix] != DEF_NULL)) {
      p_pipe->PipeNextPtrTbl[pipe_type_ix]->PipePrevPtrTbl[pipe_type_ix] = p_pipe->PipePrevPtrTbl[pipe_type_ix];
      p_pipe->PipePrevPtrTbl[pipe_type_ix]->PipeNextPtrTbl[pipe_type_ix] = p_pipe->PipeNextPtrTbl[pipe_type_ix];
    }

    p_pipe->PipeNextPtrTbl[pipe_type_ix] = DEF_NULL;
    p_pipe->PipePrevPtrTbl[pipe_type_ix] = DEF_NULL;

    DEF_BIT_CLR(pipe_type_temp, DEF_BIT(pipe_type_ix));
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeAssoc()
 *
 * @brief    Associates a pipe to an endpoint.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @param    p_ed        Pointer to the Endpoint Descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) This function will associate given pipe to given Endpoint Descriptor. If the pipe was
 *               not previously associated to this Endpoint, this function will also call the HCD's
 *               PipeCfg() function.
 *
 * @note     (2) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeAssoc(USBH_PBHCI_HC   *p_pbhci_hc,
                                 USBH_PBHCI_PIPE *p_pipe,
                                 USBH_PBHCI_ED   *p_ed,
                                 RTOS_ERR        *p_err)
{
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc->HC_Drv.HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc->HC_Drv.HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAssocEnter);

  if (p_pipe->CurED_Ptr != p_ed) {
    USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
    USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
    USBH_TOKEN        token_pid;

    token_pid = (USBH_EP_IS_IN(p_ed->EP_Params.Addr) == DEF_YES) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeCfg);
    p_pbhci_hc_api->PipeCfg(p_pbhci_hc_drv,                     // Configure allocated pipe.
                            p_pipe->PipeInfoPtr->Nbr,
                            &p_ed->EP_Params,
                            token_pid,
                            p_ed->CurPid,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAssocError);
      return;
    }

    p_pipe->CurED_Ptr = p_ed;
    p_pipe->CurEP_Handle = p_ed->EP_Handle;

    LOG_VRB(("Pipe assoc host #", (u)p_pbhci_hc_drv->HostNbr,
             " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
             " endpoint log #", (u)USBH_EP_ADDR_TO_LOG(p_ed->EP_Params.Addr),
             " pipe #", (u)p_pipe->PipeInfoPtr->Nbr));
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  p_ed->PipePtr = p_pipe;
  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAssocSuccess);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeFree()
 *
 * @brief    Frees a pipe and put it back to the free list.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *
 * @note     (2) The pipes that are freed using this function are always considered as configured.
 *               Hence, the head of the configured pipe list is where the PipeFreeHeadTblPtr points
 *               and the tail of the list is where the PipeFreeTailTblPtr points. If the pipe is able
 *               of only one transfer type, it will be inserted at the head of the list. Otherwise,
 *               it will be added at the tail.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeFree(USBH_PBHCI_HC   *p_pbhci_hc,
                                USBH_PBHCI_PIPE *p_pipe)
{
  CPU_INT08U    pipe_attrib_temp = (p_pipe->PipeInfoPtr->Attrib & USBH_PIPE_INFO_TYPE_ALL);
  USBH_PBHCI_ED *p_ed = p_pipe->CurED_Ptr;

  if (p_ed != DEF_NULL) {
    USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
    USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

    //                                                             Store current pipe's PID.
    p_ed->CurPid = p_pbhci_hc_api->PipePidCurGet(p_pbhci_hc_drv,
                                                 p_pipe->PipeInfoPtr->Nbr);

    LOG_VRB(("Pipe free host #", (u)p_pbhci_hc_drv->HostNbr,
             " HC #", (u)p_pbhci_hc_drv->HC_Nbr,
             " endpoint address #", (X)p_ed->EP_Params.Addr,
             " pipe #", (u)p_pipe->PipeInfoPtr->Nbr));

    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc_drv->HostNbr,
                                      p_pbhci_hc_drv->HC_Nbr,
                                      (CPU_INT08U)(p_ed->DevHandle) - 1u,
                                      USBH_HANDLE_EP_PHY_NBR_GET(p_pipe->CurEP_Handle),
                                      PipeFree);
  }

  while (pipe_attrib_temp != DEF_BIT_NONE) {                    // Put back pipe in free list(s).
    CPU_INT08U      pipe_type_ix;
    USBH_PBHCI_PIPE *p_pipe_head;
    USBH_PBHCI_PIPE *p_pipe_tail;
    USBH_PBHCI_PIPE *p_pipe_cfg;

    pipe_type_ix = CPU_CntTrailZeros(pipe_attrib_temp);

    p_pipe->PipePrevPtrTbl[pipe_type_ix] = DEF_NULL;
    p_pipe->PipeNextPtrTbl[pipe_type_ix] = DEF_NULL;

    p_pipe_head = p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix];
    p_pipe_tail = p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix];
    p_pipe_cfg = p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix];

    if ((p_pipe->MultiTypeAble == DEF_YES)                      // Pipe able of mult xfer type or first pipe freed.
        || (p_pipe_cfg == DEF_NULL)) {
      if (p_pipe_tail != DEF_NULL) {
        p_pipe->PipePrevPtrTbl[pipe_type_ix] = p_pipe_tail;
        p_pipe_tail->PipeNextPtrTbl[pipe_type_ix] = p_pipe;
      }

      if (p_pipe_cfg == DEF_NULL) {
        p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix] = p_pipe;
      }

      p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix] = p_pipe;
    } else {                                                    // Pipe able of one xfer type.
      p_pipe->PipeNextPtrTbl[pipe_type_ix] = p_pipe_cfg;
      p_pipe->PipePrevPtrTbl[pipe_type_ix] = p_pipe_cfg->PipePrevPtrTbl[pipe_type_ix];

      if (p_pipe_cfg->PipePrevPtrTbl[pipe_type_ix] != DEF_NULL) {
        p_pipe_cfg->PipePrevPtrTbl[pipe_type_ix]->PipeNextPtrTbl[pipe_type_ix] = p_pipe;
      }

      p_pipe_cfg->PipePrevPtrTbl[pipe_type_ix] = p_pipe;

      if (p_pipe_head == p_pipe_cfg) {
        p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix] = p_pipe;
      }

      if (p_pipe_tail == DEF_NULL) {
        p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix] = p_pipe;
      }

      p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix] = p_pipe;
    }

    if (p_pipe_head == DEF_NULL) {
      p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix] = p_pipe;
    }

    DEF_BIT_CLR(pipe_attrib_temp, DEF_BIT(pipe_type_ix));
  }
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_PipeFreeUncfg()
 *
 * @brief    Frees a pipe that is not configured and put it back to the free list.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *
 * @note     (2) This function inserts the pipe at the tail of the list if it is able of multiple
 *               transfer type. Otherwise, it will insert it at the head. However, the tail of the
 *               un-configured pipe list is where the PipeFreeTailTblPtr points if NO configured pipe
 *               have been freed. Otherwise, the tail of the list is where the previous pointer of the
 *               PipeFreeCfgTblPtr points.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeFreeUncfg(USBH_PBHCI_HC   *p_pbhci_hc,
                                     USBH_PBHCI_PIPE *p_pipe)
{
  CPU_INT08U pipe_attrib_temp = (p_pipe->PipeInfoPtr->Attrib & USBH_PIPE_INFO_TYPE_ALL);

  //                                                               Add pipe to free list(s).
  while (pipe_attrib_temp != DEF_BIT_NONE) {
    CPU_INT08U      pipe_type_ix;
    USBH_PBHCI_PIPE *p_pipe_head;
    USBH_PBHCI_PIPE *p_pipe_tail;
    USBH_PBHCI_PIPE **pp_pipe_tail;

    pipe_type_ix = CPU_CntTrailZeros(pipe_attrib_temp);

    p_pipe->PipePrevPtrTbl[pipe_type_ix] = DEF_NULL;
    p_pipe->PipeNextPtrTbl[pipe_type_ix] = DEF_NULL;

    p_pipe_head = p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix];

    //                                                             Determine list tail. See note (2).
    if (p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix] != DEF_NULL) {
      p_pipe_tail = p_pbhci_hc->PipeFreeCfgTblPtr[pipe_type_ix]->PipePrevPtrTbl[pipe_type_ix];
      pp_pipe_tail = DEF_NULL;
    } else {
      p_pipe_tail = p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix];
      pp_pipe_tail = &p_pbhci_hc->PipeFreeTailTblPtr[pipe_type_ix];
    }

    if (p_pipe->MultiTypeAble == DEF_YES) {                     // If pipe able of mult xfer type, insert at tail.
      if (p_pipe_tail != DEF_NULL) {
        p_pipe_tail->PipeNextPtrTbl[pipe_type_ix] = p_pipe;
      }

      if (p_pipe_head == DEF_NULL) {
        p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix] = p_pipe;
      }

      p_pipe->PipePrevPtrTbl[pipe_type_ix] = p_pipe_tail;

      if (pp_pipe_tail != DEF_NULL) {
        *pp_pipe_tail = p_pipe;
        p_pipe->PipeNextPtrTbl[pipe_type_ix] = DEF_NULL;
      } else if (p_pipe_tail != DEF_NULL) {
        p_pipe->PipeNextPtrTbl[pipe_type_ix] = p_pipe_tail->PipeNextPtrTbl[pipe_type_ix];

        if (p_pipe_tail->PipeNextPtrTbl[pipe_type_ix] != DEF_NULL) {
          p_pipe_tail->PipeNextPtrTbl[pipe_type_ix]->PipePrevPtrTbl[pipe_type_ix] = p_pipe;
        }
      }
    } else {                                                    // Otherwise, insert at head.
      if (p_pipe_head != DEF_NULL) {
        p_pipe_head->PipePrevPtrTbl[pipe_type_ix] = p_pipe;
      }

      p_pipe->PipeNextPtrTbl[pipe_type_ix] = p_pipe_head;
      p_pipe->PipePrevPtrTbl[pipe_type_ix] = DEF_NULL;
      p_pbhci_hc->PipeFreeHeadTblPtr[pipe_type_ix] = p_pipe;

      if ((p_pipe_tail == DEF_NULL)
          && (pp_pipe_tail != DEF_NULL)) {
        *pp_pipe_tail = p_pipe;
      }
    }

    DEF_BIT_CLR(pipe_attrib_temp, DEF_BIT(pipe_type_ix));
  }

    #if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  if (p_pipe->CurED_Ptr != DEF_NULL) {
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc->HC_Drv.HostNbr,
                                      p_pbhci_hc->HC_Drv.HC_Nbr,
                                      (CPU_INT08U)(p_pipe->CurED_Ptr->DevHandle) - 1u,
                                      USBH_HANDLE_EP_PHY_NBR_GET(p_pipe->CurEP_Handle),
                                      PipeFreeUnCfg);
  }
    #endif

  p_pipe->CurED_Ptr = DEF_NULL;
  p_pipe->CurEP_Handle = USBH_EP_HANDLE_INVALID;
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeXfer()
 *
 * @brief    Transfers data on given pipe.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @param    p_td        Pointer to the transfer descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeXfer(USBH_PBHCI_HC   *p_pbhci_hc,
                                USBH_PBHCI_PIPE *p_pipe,
                                USBH_PBHCI_TD   *p_td,
                                RTOS_ERR        *p_err)
{
  if (p_td->Token == USBH_TOKEN_IN) {
    USBH_PBHCI_PipeRxStart(p_pbhci_hc,
                           p_pipe,
                           p_td,
                           p_err);
  } else {
    USBH_PBHCI_PipeTx(p_pbhci_hc,
                      p_pipe,
                      p_td,
                      p_err);
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeTx()
 *
 * @brief    Transmits data on given pipe.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @param    p_td        Pointer to the transfer descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeTx(USBH_PBHCI_HC   *p_pbhci_hc,
                              USBH_PBHCI_PIPE *p_pipe,
                              USBH_PBHCI_TD   *p_td,
                              RTOS_ERR        *p_err)
{
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_td->ED_Ptr->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_td->ED_Ptr->EP_Handle);
#endif

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeWr);
  p_td->XferLenNext = p_pbhci_hc_api->PipeWr(p_pbhci_hc_drv,
                                             p_pipe->PipeInfoPtr->Nbr,
                                             &p_td->BufPtr[p_td->XferLen],
                                             p_td->BufLen - p_td->XferLen,
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeTxError);
    return;
  }

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeTxStart);
  p_pbhci_hc_api->PipeTxStart(p_pbhci_hc_drv,
                              p_pipe->PipeInfoPtr->Nbr,
                              &p_td->BufPtr[p_td->XferLen],
                              p_td->XferLenNext,
                              p_err);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_PipeRxStart()
 *
 * @brief    Starts data reception on given pipe.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @param    p_td        Pointer to the transfer descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeRxStart(USBH_PBHCI_HC   *p_pbhci_hc,
                                   USBH_PBHCI_PIPE *p_pipe,
                                   USBH_PBHCI_TD   *p_td,
                                   RTOS_ERR        *p_err)
{
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;

  USBH_DBG_PBHCI_STATS_ENDPOINT_INC(p_pbhci_hc_drv->HostNbr,
                                    p_pbhci_hc_drv->HC_Nbr,
                                    (CPU_INT08U)(p_td->ED_Ptr->DevHandle) - 1u,
                                    USBH_HANDLE_EP_PHY_NBR_GET(p_pipe->CurEP_Handle),
                                    PipeRxStart);
  p_pbhci_hc_api->PipeRxStart(p_pbhci_hc_drv,
                              p_pipe->PipeInfoPtr->Nbr,
                              &p_td->BufPtr[p_td->XferLen],
                              p_td->BufLen - p_td->XferLen,
                              p_err);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCI_PipeAbortAndFree()
 *
 * @brief    Starts data reception on given pipe.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *
 * @param    p_pipe      Pointer to the pipe.
 *
 * @param    p_td        Pointer to the transfer descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) The Host Controller must be locked by the caller function.
 *******************************************************************************************************/
static void USBH_PBHCI_PipeAbortAndFree(USBH_PBHCI_HC   *p_pbhci_hc,
                                        USBH_PBHCI_PIPE *p_pipe,
                                        RTOS_ERR        *p_err)
{
  USBH_PBHCI_HC_DRV *p_pbhci_hc_drv = &p_pbhci_hc->HC_Drv;
  USBH_PBHCI_HC_API *p_pbhci_hc_api = p_pbhci_hc_drv->API_Ptr;
  USBH_PBHCI_ED     *p_ed = p_pipe->CurED_Ptr;
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  CPU_INT08U host_nbr = p_pbhci_hc_drv->HostNbr;
  CPU_INT08U hc_nbr = p_pbhci_hc_drv->HC_Nbr;
  CPU_INT08U dev_nbr = (CPU_INT08U)(p_ed->DevHandle) - 1u;
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(p_ed->EP_Handle);
#endif

  p_pbhci_hc_api->PipeAbort(p_pbhci_hc_drv,
                            p_pipe->PipeInfoPtr->Nbr,
                            p_err);

  if ((p_ed->EP_Params.Type != USBH_EP_TYPE_CTRL)
      && (p_ed->EP_Params.Type != USBH_EP_TYPE_ISOC)) {
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAbortFree);
    USBH_PBHCI_PipeFree(p_pbhci_hc, p_pipe);
#if (USBH_DBG_PBHCI_STATS_EN == DEF_ENABLED)
  } else {
    USBH_DBG_PBHCI_STATS_ENDPOINT_INC(host_nbr, hc_nbr, dev_nbr, ep_phy_nbr, PipeAbortNoFree);
#endif
  }

  switch (p_ed->EP_Params.Type) {                               // Pipe now avail, attempt to submit next.
    case USBH_EP_TYPE_CTRL:
      USBH_PBHCI_NextCtrlSubmit(p_pbhci_hc);
      break;

    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      USBH_PBHCI_NextBulkSubmit(p_pbhci_hc);                    // Pipe freed. May be used for bulk xfer.
      break;

    case USBH_EP_TYPE_ISOC:
    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_EventGet()
 *
 * @brief    Gets a free PBHCI event.
 *
 * @param    pp_free_list_head   Pointer to the head pointer of free event list.
 *
 * @return   Free PBHCI event,   if no error.
 *           Null pointer,       otherwise.
 *******************************************************************************************************/
static USBH_PBHCI_EVENT *USBH_PBHCI_EventGet(USBH_PBHCI_EVENT **pp_free_list_head)
{
  USBH_PBHCI_EVENT *p_pbhci_event;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_pbhci_event = *pp_free_list_head;
  if (p_pbhci_event == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    return (DEF_NULL);
  }

  *pp_free_list_head = p_pbhci_event->NextPtr;
  CORE_EXIT_ATOMIC();

  p_pbhci_event->NextPtr = DEF_NULL;

  return (p_pbhci_event);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_EventFree()
 *
 * @brief    Frees a PBHCI event.
 *
 * @param    pp_free_list_head   Pointer to the head pointer of free event list.
 *
 * @param    p_pbhci_event       Pointer to the PBHCI event.
 *******************************************************************************************************/
static void USBH_PBHCI_EventFree(USBH_PBHCI_EVENT **pp_free_list_head,
                                 USBH_PBHCI_EVENT *p_pbhci_event)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_pbhci_event->NextPtr = *pp_free_list_head;
  *pp_free_list_head = p_pbhci_event;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_EventQAdd()
 *
 * @brief    Adds a PBHCI event to the event queue.
 *
 * @param    p_pbhci_event   Pointer to the PBHCI event.
 *******************************************************************************************************/
static void USBH_PBHCI_EventQAdd(USBH_PBHCI_EVENT *p_pbhci_event)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (USBH_PBHCI_Ptr->EventQHeadPtr == DEF_NULL) {
    USBH_PBHCI_Ptr->EventQHeadPtr = p_pbhci_event;
  }

  if (USBH_PBHCI_Ptr->EventQTailPtr != DEF_NULL) {
    USBH_PBHCI_Ptr->EventQTailPtr->NextPtr = p_pbhci_event;
  }

  USBH_PBHCI_Ptr->EventQTailPtr = p_pbhci_event;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_HC_Lock()
 *
 * @brief    Locks the Host Controller.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *******************************************************************************************************/
static void USBH_PBHCI_HC_Lock(USBH_PBHCI_HC *p_pbhci_hc)
{
  RTOS_ERR err_local;

  KAL_LockAcquire(p_pbhci_hc->LockHandle,
                  KAL_OPT_PEND_NONE,
                  0u,
                  &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Locking HC", RTOS_ERR_LOG_ARG_GET(err_local)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_HC_Unlock()
 *
 * @brief    Unlocks the Host Controller.
 *
 * @param    p_pbhci_hc  Pointer to the PBHCI Host Controller.
 *******************************************************************************************************/
static void USBH_PBHCI_HC_Unlock(USBH_PBHCI_HC *p_pbhci_hc)
{
  RTOS_ERR err_local;

  KAL_LockRelease(p_pbhci_hc->LockHandle,
                  &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Unlocking HC", RTOS_ERR_LOG_ARG_GET(err_local)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_PBHCI_HC_PtrGet()
 *
 * @brief    Gets pointer to PBHCI Host Controller at given index.
 *
 * @param    pbhci_hc_nbr    PBHCI Host Controller number.
 *
 * @return   Pointer to the PBHCI host controller.
 *******************************************************************************************************/
static USBH_PBHCI_HC *USBH_PBHCI_HC_PtrGet(CPU_INT08U pbhci_hc_nbr)
{
  USBH_PBHCI_HC *p_pbhci_hc;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (pbhci_hc_nbr >= USBH_PBHCI_Ptr->HC_IxNext) {
    CORE_EXIT_ATOMIC();
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG, DEF_NULL);
  }
#endif

  p_pbhci_hc = USBH_PBHCI_Ptr->HC_TblPtr[pbhci_hc_nbr];
#else
  p_pbhci_hc = USBH_PBHCI_Ptr->HC_HeadPtr;
  while ((p_pbhci_hc != DEF_NULL)
         && (p_pbhci_hc->HC_Drv.PBHCI_HC_Nbr != pbhci_hc_nbr)) {
    p_pbhci_hc = p_pbhci_hc->NextPtr;
  }
#endif
  CORE_EXIT_ATOMIC();

  return (p_pbhci_hc);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL))
