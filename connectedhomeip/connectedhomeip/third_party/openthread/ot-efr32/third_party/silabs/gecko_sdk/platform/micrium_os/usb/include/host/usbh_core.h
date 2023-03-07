/***************************************************************************//**
 * @file
 * @brief USB Host General Operations
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

/****************************************************************************************************//**
 * @defgroup USBH USB Host API
 * @brief   USB Host API
 *
 * @defgroup USBH_CORE USB Host Core API
 * @ingroup USBH
 * @brief   USB Host Core API
 *
 * @addtogroup USBH_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_H_
#define  _USBH_CORE_H_

#if !defined(SL_SUPRESS_MICRIUMOS_USB_HOST_DEPRECATION_WARNINGS)
#warning  Warning: MicriumOS-USB-Host is deprecated and will be removed in an upcoming release.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_types.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core_handle.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_ep.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   HOST_MODULE
#define  USBH_CORE_EXT
#else
#define  USBH_CORE_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef USBH_CFG_ISOC_EN
#define  USBH_CFG_ISOC_EN                           DEF_DISABLED
#endif

#define  USBH_HC_NBR_NONE                               0xFFu

/********************************************************************************************************
 *                                   PERIODIC TRANSFERS MAXIMUM INTERVAL
 *******************************************************************************************************/

#define  USBH_PERIODIC_XFER_MAX_INTERVAL_DFLT             0u

#define  USBH_PERIODIC_XFER_MAX_INTERVAL_001              1u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_002              2u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_004              4u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_008              8u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_016             16u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_032             32u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_064             64u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_128            128u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_256            256u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_512            512u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_1024          1024u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_2048          2048u
#define  USBH_PERIODIC_XFER_MAX_INTERVAL_4096          4096u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbh_hc_drv USBH_HC_DRV;
typedef struct usbh_hc_dedicated_mem_info USBH_HC_DEDICATED_MEM_INFO;
typedef struct usbh_hc_cfg_ext USBH_HC_CFG_EXT;
typedef const struct usbh_hc_drv_api USBH_HC_DRV_API;
typedef const struct usbh_hc_bsp_api USBH_HC_BSP_API;
typedef const struct usbh_hc_rh_api USBH_HC_RH_API;

/********************************************************************************************************
 *                                       USB HOST EVENT FUNCTIONS
 *
 * Note(s) : (1) This structure contains a set of application generic callbacks.
 *
 *           (2) DevResume() is not supported and is present for future considerations.
 *******************************************************************************************************/

typedef const struct usbh_event_fncts {
  //                                                               Indicates a dev conn. App must return DEF_YES.
  CPU_BOOLEAN (*DevConnAccept)(void);

  //                                                               Device config was retrieved. App can decline it.
  CPU_BOOLEAN (*DevConfigAccept)(void);

  //                                                               Indicates dev was set to configured state.
  void (*DevConfig)(USBH_DEV_HANDLE dev_handle,
                    CPU_INT08U      config_nbr,
                    RTOS_ERR        err);

  //                                                               Indicates fnct of dev did not connect successfully.
  void (*FnctConnFail)(USBH_FNCT_HANDLE fnct_handle,
                       RTOS_ERR         err);

  //                                                               Indicates dev conn failed.
  void (*DevConnFail)(CPU_INT08U hub_addr,
                      CPU_INT08U port_nbr,
                      RTOS_ERR   err);

  //                                                               Indicates dev was resumed after a remote wakeup.
  //                                                               Not yet implemented. For future considerations.
  void (*DevResume)(USBH_DEV_HANDLE dev_handle);

  //                                                               Indicates dev has been disconnected.
  void (*DevDisconn)(USBH_DEV_HANDLE dev_handle);
} USBH_EVENT_FNCTS;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

typedef void (*USBH_HCD_ISR_HANDLE_FNCT)(USBH_HC_DRV *p_hc_drv);

/****************************************************************************************************//**
 *                                           HUB STATUS DATA TYPE
 *
 * @note     (1) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2.7.
 *******************************************************************************************************/

typedef struct usbh_hub_status {
  CPU_INT16U Status;
  CPU_INT16U Chng;
} USBH_HUB_STATUS;

/****************************************************************************************************//**
 *                                       USB HOST CONFIGURATION
 * @{
 *******************************************************************************************************/

//                                                                 ------- USB HOST OPTIMIZE SPD CONFIGURATIONS -------
typedef struct usbh_cfg_optimize_spd {
  CPU_INT08U HC_PerHostQty;                                     ///< Quantity of Host Controller per host.
  CPU_INT08U DevPerHostQty;                                     ///< Quantity of device per host.
  CPU_INT08U FnctPerConfigQty;                                  ///< Quantity of function per USB configurations.
  CPU_INT08U IF_PerFnctQty;                                     ///< Quantity of interfaces per function.
} USBH_CFG_OPTIMIZE_SPD;

//                                                                 -------- USB HOST INIT ALLOC CONFIGURATIONS --------
typedef struct usbh_cfg_init_alloc {
  CPU_INT16U DevQtyTot;                                         ///< Quantity of devices (total).
  CPU_INT16U FnctQtyTot;                                        ///< Quantity of functions  (total).
  CPU_INT16U IF_QtyTot;                                         ///< Quantity of interfaces (total).
  CPU_INT16U EP_QtyTot;                                         ///< Quantity of endpoints  (total).
  CPU_INT16U EP_QtyOpenTot;                                     ///< Quantity of simultaneously opened endpoint (total).
  CPU_INT16U URB_QtyTot;                                        ///< Quantity of available URBs. (total)
  CPU_INT08U HubFnctQtyTot;                                     ///< Quantity of hub functions. (total)
  CPU_INT08U HubEventQtyTot;                                    ///< Quantity of hub events. (total)
} USBH_CFG_INIT_ALLOC;

//                                                                 ------------- USB HOST CONFIGURATIONS --------------
typedef struct usbh_init_cfg {
  CPU_INT16U            MaxDescLen;                             ///< Maximum descriptor length (in bytes).
  CPU_SIZE_T            BufAlignOctets;                         ///< Buffer alignement, in octets.
  USBH_EVENT_FNCTS      *EventFnctsPtr;                         ///< Ptr to structure containing the event functions.
  MEM_SEG               *MemSegPtr;                             ///< Ptr to mem segment for internal data allocations.
  MEM_SEG               *MemSegBufPtr;                          ///< Ptr to mem segment for buffer allocations.

  CPU_INT32U            AsyncTaskStkSizeElements;               ///< Asynchronous task's stack size in qty of elements.
  void                  *AsyncTaskStkPtr;                       ///< Ptr to asynchronous stack's stack base.

  CPU_INT32U            HubTaskStkSizeElements;                 ///< Hub task's stack size in qty of elements.
  void                  *HubTaskStkPtr;                         ///< Ptr to Hub stack's stack base.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_CFG_OPTIMIZE_SPD OptimizeSpd;                            ///< Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_CFG_INIT_ALLOC InitAlloc;                                ///< Configurations for allocations at init mode.
#endif
} USBH_INIT_CFG;

///< @}

/****************************************************************************************************//**
 *                                       HOST CONTROLLER CONFIGURATION
 *
 * @note     (1) Structure only needed IF controller uses dedicated memory for data buffers. Pointer MUST
 *               be null otherwise.
 *******************************************************************************************************/

//                                                                 ----------- HC INIT ALLOC CONFIGURATIONS -----------
typedef struct usbh_hc_cfg_init_alloc {
  CPU_SIZE_T EP_DescQty;                                        ///< Qty of EP   desc (ctrl, bulk and intr) to allocate.
  CPU_SIZE_T EP_DescIsocQty;                                    ///< Qty of EP   desc (isoc)                to allocate.
  CPU_SIZE_T XferDescQty;                                       ///< Qty of xfer desc (Ctrl, bulk and intr) to allocate.
  CPU_SIZE_T XferDescIsocQty;                                   ///< Qty of xfer desc (isoc)                to allocate.
} USBH_HC_CFG_INIT_ALLOC;

//                                                                 ---------- HC OPTIMIZE SPD CONFIGURATIONS ----------
typedef struct usbh_hc_cfg_optimize_spd {
  CPU_SIZE_T XferDescQty;                                       ///< Qty of xfer desc (ctrl, bulk and intr).
  CPU_SIZE_T XferDescIsocQty;                                   ///< Qty of xfer desc (isoc).
} USBH_HC_CFG_OPTIMIZE_SPD;

//                                                                 --------- HC DEDICATED MEM CONFIGURATIONS ----------
typedef struct usbh_hc_cfg_dedicated_mem {
  CPU_SIZE_T DataBufQty;                                        ///< Qty of data buffer.
  CPU_SIZE_T DataBufLen;                                        ///< Length, in octets, of data buffers.
} USBH_HC_CFG_DEDICATED_MEM;

//                                                                 ------------ HC EXTENDED CONFIGURATIONS ------------
struct usbh_hc_cfg_ext {
  CPU_INT16U                MaxPeriodicInterval;                ///< Maximum periodic interval.

  USBH_HC_CFG_DEDICATED_MEM *DedicatedMemCfgPtr;                ///< Ptr to dedicated memory configuration. See note (1).

  //                                                               Ptr to optimize spd cfg. Can be DEF_NULL if ...
  //                                                               USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED.
  USBH_HC_CFG_OPTIMIZE_SPD *CfgOptimizeSpdPtr;
  //                                                               Ptr to allocation at init cfg. Can be DEF_NULL ...
  //                                                               if USBH_CFG_INIT_ALLOC_EN is set to DEF_DISABLED.
  USBH_HC_CFG_INIT_ALLOC   *CfgInitAllocPtr;
};

//                                                                 ------ HC CONFIGURATION FOR DEDICATED MEMORY -------
struct usbh_hc_dedicated_mem_info {
  MEM_SEG    *MemSegPtr;                                        ///< Pointer to dedicated memory segment.
  CPU_SIZE_T BufAlignOctets;                                    ///< Alignment of buf on dedicated mem in octets.
};

/********************************************************************************************************
 *                                       USB HOST CONTROLLER BSP API
 *
 * Note(s) : (1) Can be null if no requirements regarding dedicated memory usage.
 *******************************************************************************************************/

struct usbh_hc_bsp_api {
  //                                                               Init BSP.
  CPU_BOOLEAN (*Init)   (USBH_HCD_ISR_HANDLE_FNCT isr_fnct,
                         USBH_HC_DRV              *p_hc_drv);

  CPU_BOOLEAN (*ClkCfg) (void);                                 // Init USB controller clock.

  CPU_BOOLEAN (*IO_Cfg) (void);                                 // Init IO pin multiplexing.

  CPU_BOOLEAN (*IntCfg) (void);                                 // Init interrupt control.

  CPU_BOOLEAN (*PwrCfg) (void);                                 // Init power control.

  CPU_BOOLEAN (*Start)  (void);                                 // Start communication.

  CPU_BOOLEAN (*Stop)   (void);                                 // Stop communication.
};

/*
 ********************************************************************************************************
 *                                           HOST CONTROLLER TYPE
 *******************************************************************************************************/

typedef enum usbh_hc_type {
  USBH_HC_TYPE_LIST,                                            ///< Controller is list-based (ohci, ehci, ...).
  USBH_HC_TYPE_PIPE                                             ///< Controller is pipe-based.
} USBH_HC_TYPE;

/****************************************************************************************************//**
 *                               USB HOST CONTROLLER HARDWARE INFORMATION
 *
 * @note     (1) Null pointer means no dedicated memory.
 *******************************************************************************************************/

typedef struct usbh_hc_hw_info {
  CPU_ADDR                   BaseAddr;                          ///< HC reg's base addr.
  USBH_DEV_SPD               RH_Spd;                            ///< Root hub speed.
  USBH_HC_TYPE               HC_Type;                           ///< HC type, pipe or list.

  //                                                               -------- DEDICATED MEMORY INFO (SEE NOTE 1) --------
  USBH_HC_DEDICATED_MEM_INFO *HW_DescDedicatedMemInfoPtr;       ///< Ptr to dedicated memory info for HW descriptors.
  USBH_HC_DEDICATED_MEM_INFO *DataBufDedicatedMemInfoPtr;       ///< Ptr to dedicated memory info for data buffers.
} USBH_HC_HW_INFO;

/****************************************************************************************************//**
 *                           USB HOST LIST-BASED CONTROLLER HARDWARE INFORMATION
 *
 * @note     (1) This description structure is used for list-based controllers.
 *******************************************************************************************************/

typedef struct usbh_hc_hcd_hw_info {
  USBH_HC_HW_INFO HW_Info;                                      ///< Host controller common hardware info.

  USBH_HC_DRV_API *DrvAPI_Ptr;                                  ///< Pointer to driver API.
  USBH_HC_RH_API  *RH_API_Ptr;                                  ///< Pointer to root hub driver API.
  USBH_HC_BSP_API *BSP_API_Ptr;                                 ///< Pointer to BSP API.
} USBH_HC_HCD_HW_INFO;

/********************************************************************************************************
 *                                   USB HOST CALLBACK FUNCTIONS TYPES
 *******************************************************************************************************/

typedef void (*USBH_HC_OPER_CMPL) (CPU_INT08U host_nbr,
                                   CPU_INT08U hc_nbr,
                                   void       *p_arg,
                                   RTOS_ERR   err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_FRM_NBR_MASK                                  DEF_BIT_FIELD(11u, 0u)
#define  USBH_UFRM_NBR_MASK                                 DEF_BIT_FIELD(3u, 11u)
#define  USBH_FRM_NBR_FMT(frm_nbr, ufrm_nbr)               (((frm_nbr) & USBH_FRM_NBR_MASK) | (((ufrm_nbr) << 11u) & USBH_UFRM_NBR_MASK))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_INIT_CFG USBH_InitCfgDflt;                    // USBH core dflt configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBH_ConfigureMaxDescLen(CPU_INT16U max_desc_len);

void USBH_ConfigureEventFncts(const USBH_EVENT_FNCTS *p_event_fncts);

void USBH_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                          MEM_SEG *p_mem_seg_buf);

void USBH_ConfigureAsyncTaskStk(CPU_INT32U stk_size_elements,
                                void       *p_stk);

void USBH_ConfigureHubTaskStk(CPU_INT32U stk_size_elements,
                              void       *p_stk);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_ConfigureOptimizeSpdCfg(const USBH_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_ConfigureInitAllocCfg(const USBH_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                       USB HOST CORE FUNCTIONS
 *******************************************************************************************************/

//                                                                 --------- USB HOST STACK GENERAL FUNCTIONS ---------
void USBH_Init(CPU_INT08U host_qty,
               RTOS_ERR   *p_err);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_UnInit(RTOS_ERR *p_err);
#endif

//                                                                 ------------ HOST CONTROLLER FUNCTIONS -------------
USBH_HC_HANDLE USBH_HC_Add(const CPU_CHAR        *name,
                           const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                           RTOS_ERR              *p_err);

USBH_HC_HANDLE USBH_HC_HandleGetFromName(const CPU_CHAR *name);

void USBH_HC_Start(USBH_HC_HANDLE hc_handle,
                   RTOS_ERR       *p_err);

void USBH_HC_Stop(USBH_HC_HANDLE    hc_handle,
                  USBH_HC_OPER_CMPL callback_fnct,
                  void              *p_arg,
                  RTOS_ERR          *p_err);

void USBH_HC_Suspend(USBH_HC_HANDLE    hc_handle,
                     USBH_HC_OPER_CMPL callback_fnct,
                     void              *p_arg,
                     RTOS_ERR          *p_err);

void USBH_HC_Resume(USBH_HC_HANDLE    hc_handle,
                    USBH_HC_OPER_CMPL callback_fnct,
                    void              *p_arg,
                    RTOS_ERR          *p_err);

CPU_INT16U USBH_HC_FrameNbrGet(USBH_HC_HANDLE hc_handle,
                               RTOS_ERR       *p_err);

void USBH_AsyncTaskPrioSet(CPU_INT08U prio,
                           RTOS_ERR   *p_err);

void USBH_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                           RTOS_ERR   *p_err);

#if (USBH_CFG_STR_EN == DEF_ENABLED)
void USBH_PreferredStrLangID_Set(CPU_INT16U preferred_str_lang_id,
                                 RTOS_ERR   *p_err);
#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  USBH_CFG_OPTIMIZE_SPD_EN
#error  "USBH_CFG_OPTIMIZE_SPD_EN              not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_OPTIMIZE_SPD_EN != DEF_ENABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN != DEF_DISABLED))
#error  "USBH_CFG_OPTIMIZE_SPD_EN              illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_INIT_ALLOC_EN
#error  "USBH_CFG_INIT_ALLOC_EN                not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_INIT_ALLOC_EN != DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN != DEF_DISABLED))
#error  "USBH_CFG_INIT_ALLOC_EN                illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_ALT_IF_EN
#error  "USBH_CFG_ALT_IF_EN                    not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_ALT_IF_EN != DEF_ENABLED) \
  && (USBH_CFG_ALT_IF_EN != DEF_DISABLED))
#error  "USBH_CFG_ALT_IF_EN                    illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_STR_EN
#error  "USBH_CFG_STR_EN                       not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_STR_EN != DEF_ENABLED) \
  && (USBH_CFG_STR_EN != DEF_DISABLED))
#error  "USBH_CFG_STR_EN                       illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_PERIODIC_XFER_EN
#error  "USBH_CFG_PERIODIC_XFER_EN             not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_PERIODIC_XFER_EN != DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN != DEF_DISABLED))
#error  "USBH_CFG_PERIODIC_XFER_EN             illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_UNINIT_EN
#error  "USBH_CFG_UNINIT_EN                    not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_CFG_UNINIT_EN != DEF_ENABLED) \
  && (USBH_CFG_UNINIT_EN != DEF_DISABLED))
#error  "USBH_CFG_UNINIT_EN                    illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#ifndef  USBH_CFG_FIELD_EN_MASK
#error  "USBH_CFG_FIELD_EN_MASK                not #define'd in 'usbh_cfg.h'"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
