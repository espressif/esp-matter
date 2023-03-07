/***************************************************************************//**
 * @file
 * @brief USB Host - Pipe-Based Host Controller Interface (PBHCI)
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
 * @defgroup USBH_PBHCI USB Host PBHCI API
 * @ingroup USBH
 * @brief   USB Host PBHCI API
 *
 * @addtogroup USBH_PBHCI
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_PBHCI_H_
#define  _USBH_PBHCI_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   PERIODIC TRANSFERS MAXIMUM PRECISION
 *******************************************************************************************************/

//                                                                 Infinite interval. Should be used ONLY by PBHCDs.
#define  USBH_PBHCI_PERIODIC_XFER_MAX_PRECISION_INIFINITE  USBH_PERIODIC_XFER_MAX_INTERVAL_128

/********************************************************************************************************
 *                                   USB PIPE INFORMATION TABLE DEFINES
 *******************************************************************************************************/

//                                                                 -------------- PIPE TYPE BIT DEFINES ---------------
#define  USBH_PIPE_INFO_TYPE_CTRL               DEF_BIT_03
#define  USBH_PIPE_INFO_TYPE_BULK               DEF_BIT_00
#define  USBH_PIPE_INFO_TYPE_INTR               DEF_BIT_01
#define  USBH_PIPE_INFO_TYPE_ISOC               DEF_BIT_02

#define  USBH_PIPE_INFO_TYPE_ALL               (USBH_PIPE_INFO_TYPE_ISOC | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR)

//                                                                 ------------ PIPE DIRECTION BIT DEFINES ------------
#define  USBH_PIPE_INFO_DIR_OUT                 DEF_BIT_04
#define  USBH_PIPE_INFO_DIR_IN                  DEF_BIT_05

#define  USBH_PIPE_INFO_DIR_BOTH               (USBH_PIPE_INFO_DIR_OUT | USBH_PIPE_INFO_DIR_IN)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USB HOST PBHCI CONFIGURATIONS
 *
 * @note     (1) Represents the quantity of Start-Of-Frame event. Each time a start of frame issued by the
 *               host controller, PBHCI verifies if a periodic transfer must be scheduled and will post an
 *               event. Ignored when periodic transfers are disabled.
 *******************************************************************************************************/

typedef struct usbh_pbhci_init_cfg {
  CPU_INT16U SOF_EventQty;                                      ///< Quantity of Start-Of-Frame event. See Note (1).

  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment for internal data allocations.

  CPU_INT32U SchedTaskStkSizeElements;                          ///< Scheduler task's stack size in qty of elements.
  void       *SchedTaskStkPtr;                                  ///< Ptr to scheduler tasks's stack base.
} USBH_PBHCI_INIT_CFG;

/********************************************************************************************************
 *                                           FORWARD DECLARATION
 *******************************************************************************************************/

typedef const struct usbh_pbhci_pipe_info USBH_PBHCI_PIPE_INFO;
typedef const struct usbh_pbhci_hc_api USBH_PBHCI_HC_API;
typedef const struct usbh_pbhci_bsp_api USBH_PBHCI_BSP_API;
typedef struct usbh_pbhci_hc_drv USBH_PBHCI_HC_DRV;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

typedef void (*USBH_PBHCD_ISR_HANDLE_FNCT)(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv);

/********************************************************************************************************
 *                                   USB HOST CONTROLLER PBHCI BSP API
 *******************************************************************************************************/

struct usbh_pbhci_bsp_api {
  //                                                               Init BSP.
  CPU_BOOLEAN (*Init)   (USBH_PBHCD_ISR_HANDLE_FNCT isr_fnct,
                         USBH_PBHCI_HC_DRV          *p_pbhci_hc_drv);

  CPU_BOOLEAN (*ClkCfg) (void);                                 // Init USB controller clock.

  CPU_BOOLEAN (*IO_Cfg) (void);                                 // Init IO pin multiplexing.

  CPU_BOOLEAN (*IntCfg) (void);                                 // Init interrupt control.

  CPU_BOOLEAN (*PwrCfg) (void);                                 // Init power control.

  CPU_BOOLEAN (*Start)  (void);                                 // Start communication.

  CPU_BOOLEAN (*Stop)   (void);                                 // Stop  communication.

  void        *ExtBSP_API;                                      // Pointer to drv specific extended BSP API.
};

/********************************************************************************************************
 *                                           PBHCI PIPE INFORMATION
 *******************************************************************************************************/

struct usbh_pbhci_pipe_info {
  CPU_INT08U Attrib;                                            // Pipe attributes (type(s) + direction(s)).
  CPU_INT08U Nbr;                                               // Pipe nbr.
  CPU_INT16U MaxPktSize;                                        // Pipe maximum packet length.
  CPU_INT32U MaxBufLen;                                         // Pipe maximum buffer length.
};

/********************************************************************************************************
 *                           USB HOST CONTROLLER PBHCI HARDWARE INFORMATION
 *
 * Note(s) : (1) Null pointer means no dedicated memory.
 *******************************************************************************************************/

typedef struct usbh_pbhci_hc_hw_info {
  USBH_HC_HW_INFO      HW_Info;                                 // Pointer to common HC hardware information.

  USBH_PBHCI_HC_API    *DrvAPI_Ptr;                             // Pointer to driver API.
  USBH_PBHCI_BSP_API   *BSP_API_Ptr;                            // Pointer to BSP API.
  USBH_PBHCI_PIPE_INFO *PipeInfoTblPtr;                         // Pointer to pipe info table.
} USBH_PBHCI_HC_HW_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_PBHCI_INIT_CFG USBH_PBHCI_InitCfgDflt;        // PBHCI default configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_PBHCI_ConfigureSOF_EventQty(CPU_INT16U sof_event_qty);

void USBH_PBHCI_ConfigureSchedTaskStk(CPU_INT32U stk_size_elements,
                                      void       *p_stk);

void USBH_PBHCI_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void USBH_PBHCI_Init(RTOS_ERR *p_err);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_PBHCI_UnInit(RTOS_ERR *p_err);
#endif

void USBH_PBHCI_SchedTaskPrioSet(CPU_INT08U prio,
                                 RTOS_ERR   *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
