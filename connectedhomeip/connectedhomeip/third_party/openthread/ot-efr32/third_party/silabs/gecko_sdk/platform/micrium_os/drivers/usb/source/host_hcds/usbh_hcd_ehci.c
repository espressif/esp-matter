/***************************************************************************//**
 * @file
 * @brief USB Host - Generic Ehci Driver
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
 * @note             (1) This driver is compatible with USB host controllers implementing the Enhanced Host
 *                       Controller Interface (EHCI) specification V1.0 and above maintained by Intel
 *                       Corporation. The specification can be downloaded here:
 *                       http://www.intel.com/content/www/us/en/io/universal-serial-bus/ehci-specification.html
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_HCD_MODULE
#include  <em_core.h>

#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <cpu/include/cpu.h>
#include  <cpu/include/cpu_cache.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_def.h>
#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_hub.h>

#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_lib_priv.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, HCD, EHCI)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

//                                                                 Required alignment of EHCI descriptors in bytes.
#define  EHCI_QH_ALIGN                         32u
#define  EHCI_QTD_ALIGN                        32u
#define  EHCI_PERIODIC_FRM_LIST_ALIGN          4096u

#define  EHCI_RH_PORT_MAX_QTY                  8u               // Maximum quantity of RH port supported by HCD.

#define  EHCI_64BIT_SEGMENT_MSK                DEF_BIT_FIELD(32u, 32u)

//                                                                 Determine if buf should be copied to ded mem or not.
#define  USBH_EHCI_DED_MEM_BUF_SHOULD_COPY(p_hc_drv, p_buf, buf_len)                  \
  ((((p_hc_drv)->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL)                  \
    && (buf_len > 0u)                                                                 \
    && ((((USBH_HCD_DATA *)(p_hc_drv)->DataPtr)->DedMemAddrStart > (CPU_ADDR)(p_buf)) \
        || (( (USBH_HCD_DATA *)(p_hc_drv)->DataPtr)->DedMemAddrEnd < (CPU_ADDR)(p_buf) + (buf_len)))) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 *                                           CAPABILITY REGISTERS
 *******************************************************************************************************/

//                                                                 -------------------- HCSPARAMS ---------------------
#define  EHCI_HCSPARAMS_N_TT_MSK               DEF_BIT_FIELD(4u, 24u) // Synopsys only.
#define  EHCI_HCSPARAMS_N_PTT_MSK              DEF_BIT_FIELD(4u, 20u) // Synopsys only.
#define  EHCI_HCSPARAMS_DPN_MSK                DEF_BIT_FIELD(4u, 20u)
#define  EHCI_HCSPARAMS_P_INDICATOR            DEF_BIT_16
#define  EHCI_HCSPARAMS_N_CC_MSK               DEF_BIT_FIELD(4u, 12u)
#define  EHCI_HCSPARAMS_N_PCC_MSK              DEF_BIT_FIELD(4u, 8u)
#define  EHCI_HCSPARAMS_PRR                    DEF_BIT_07
#define  EHCI_HCSPARAMS_PPC                    DEF_BIT_04
#define  EHCI_HCSPARAMS_N_PORTS_MSK            DEF_BIT_FIELD(4u, 0u)

//                                                                 -------------------- HCCPARAMS ---------------------
#define  EHCI_HCCPARAMS_EECP                   DEF_BIT_FIELD(8u, 8u)
#define  EHCI_HCCPARAMS_IST                    DEF_BIT_FIELD(4u, 4u)
#define  EHCI_HCCPARAMS_ASPC                   DEF_BIT_02
#define  EHCI_HCCPARAMS_PFLF                   DEF_BIT_01
#define  EHCI_HCCPARAMS_64BIT                  DEF_BIT_00

/********************************************************************************************************
 *                                           OPERATIONAL REGISTERS
 *******************************************************************************************************/
//                                                                 ---------------------- USBCMD ----------------------
#define  EHCI_USBCMD_ITC_MSK                   DEF_BIT_FIELD(8u, 16u)
#define  EHCI_USBCMD_FLS2                      DEF_BIT_15       // Synopsys only.
#define  EHCI_USBCMD_ATDTW                     DEF_BIT_14       // Synopsys only.
#define  EHCI_USBCMD_SUTW                      DEF_BIT_13       // Synopsys only.
#define  EHCI_USBCMD_ASPME                     DEF_BIT_11
#define  EHCI_USBCMD_ASPMC_MSK                 DEF_BIT_FIELD(2u, 8u)
#define  EHCI_USBCMD_LHCR                      DEF_BIT_07
#define  EHCI_USBCMD_IOAAD                     DEF_BIT_06
#define  EHCI_USBCMD_ASE                       DEF_BIT_05
#define  EHCI_USBCMD_PSE                       DEF_BIT_04
#define  EHCI_USBCMD_FLS_MSK                   DEF_BIT_FIELD(2u, 2u)
#define  EHCI_USBCMD_HCRESET                   DEF_BIT_01
#define  EHCI_USBCMD_RS                        DEF_BIT_00

#define  EHCI_USBCMD_ITC_1                     1u
#define  EHCI_USBCMD_ITC_2                     2u
#define  EHCI_USBCMD_ITC_4                     4u
#define  EHCI_USBCMD_ITC_8                     8u
#define  EHCI_USBCMD_ITC_16                    16u
#define  EHCI_USBCMD_ITC_32                    32u
#define  EHCI_USBCMD_ITC_64                    64u

#define  EHCI_USBCMD_FLS_1024_64               0u
#define  EHCI_USBCMD_FLS_512_32                1u
#define  EHCI_USBCMD_FLS_256_16                2u
#define  EHCI_USBCMD_FLS_128_8                 3u               // Synopsys only.

//                                                                 ---------------------- USBSTS ----------------------
#define  EHCI_USBSTS_UPI                       DEF_BIT_19       // Synopsys only.
#define  EHCI_USBSTS_UAI                       DEF_BIT_18       // Synopsys only.
#define  EHCI_USBSTS_NAKI                      DEF_BIT_16       // Synopsys only.
#define  EHCI_USBSTS_ASS                       DEF_BIT_15
#define  EHCI_USBSTS_PSS                       DEF_BIT_14
#define  EHCI_USBSTS_RECL                      DEF_BIT_13
#define  EHCI_USBSTS_HC_HAL                    DEF_BIT_12
#define  EHCI_USBSTS_ULPII                     DEF_BIT_10       // Synopsys only.
#define  EHCI_USBSTS_SLI                       DEF_BIT_08       // Synopsys only.
#define  EHCI_USBSTS_SRI                       DEF_BIT_07       // Synopsys only.
#define  EHCI_USBSTS_URI                       DEF_BIT_06       // Synopsys only.
#define  EHCI_USBSTS_IOAA                      DEF_BIT_05
#define  EHCI_USBSTS_HSE                       DEF_BIT_04
#define  EHCI_USBSTS_FLR                       DEF_BIT_03
#define  EHCI_USBSTS_PCD                       DEF_BIT_02
#define  EHCI_USBSTS_USBERRINT                 DEF_BIT_01
#define  EHCI_USBSTS_USBINT                    DEF_BIT_00

//                                                                 --------------------- USBINTR ----------------------
#define  EHCI_USBINTR_UPIE                     DEF_BIT_19       // Synopsys only.
#define  EHCI_USBINTR_UAIE                     DEF_BIT_18       // Synopsys only.
#define  EHCI_USBINTR_NAKE                     DEF_BIT_16       // Synopsys only.
#define  EHCI_USBINTR_ULPIE                    DEF_BIT_10       // Synopsys only.
#define  EHCI_USBINTR_SLE                      DEF_BIT_08       // Synopsys only.
#define  EHCI_USBINTR_SRE                      DEF_BIT_07       // Synopsys only.
#define  EHCI_USBINTR_IOAAE                    DEF_BIT_05
#define  EHCI_USBINTR_HSEE                     DEF_BIT_04
#define  EHCI_USBINTR_FLRE                     DEF_BIT_03
#define  EHCI_USBINTR_PCIE                     DEF_BIT_02
#define  EHCI_USBINTR_USBEIE                   DEF_BIT_01
#define  EHCI_USBINTR_USBIE                    DEF_BIT_00

//                                                                 --------------------- FRINDEX ----------------------
#define  EHCI_FRINDEX_FI_MSK                   DEF_BIT_FIELD(14u, 0u)
#define  EHCI_FRINDEX_FI_FRAME_MSK             DEF_BIT_FIELD(11u, 3u)
#define  EHCI_FRINDEX_FI_UFRAME_MSK            DEF_BIT_FIELD(3u, 0u)

//                                                                 ----------------- PERIODICLISTBASE -----------------
#define  EHCI_PERIODICLIST_BA                  DEF_BIT_FIELD(19u, 12u)

//                                                                 ------------------ ASYNCLISTADDR -------------------
#define  EHCI_ASYNCLISTADDR_LPL                DEF_BIT_FIELD(27u, 5u)

//                                                                 ---------------- TTCTRL (SYNOPSYS) -----------------
#define  EHCI_TTCTRL_TTHA                      DEF_BIT_FIELD(7u, 24u)

//                                                                 --------------- BURSTSIZE (SYNOPSYS) ---------------
#define  EHCI_BURSTSIZE_TXPBURST               DEF_BIT_FIELD(8u, 8u)
#define  EHCI_BURSTSIZE_RXPBURST               DEF_BIT_FIELD(8u, 0u)

//                                                                 ------------- TXFILLTUNING (SYNOPSYS) --------------
#define  EHCI_TXFILLTUNING_TXFIFOTHRES         DEF_BIT_FIELD(6u, 16u)
#define  EHCI_TXFILLTUNING_TXSCHHEALTH         DEF_BIT_FIELD(5u, 8u)
#define  EHCI_TXFILLTUNING_TXSCHOH             DEF_BIT_FIELD(7u, 0u)

//                                                                 ------------- ULPI_VIEWPORT (SYNOPSYS) --------------
#define  EHCI_ULPI_VIEWPORT_WU                 DEF_BIT_31
#define  EHCI_ULPI_VIEWPORT_RUN                DEF_BIT_30
#define  EHCI_ULPI_VIEWPORT_RW                 DEF_BIT_29
#define  EHCI_ULPI_VIEWPORT_SS                 DEF_BIT_27
#define  EHCI_ULPI_VIEWPORT_PORT               DEF_BIT_FIELD(3u, 24u)
#define  EHCI_ULPI_VIEWPORT_ADDR               DEF_BIT_FIELD(8u, 16u)
#define  EHCI_ULPI_VIEWPORT_DATRD              DEF_BIT_FIELD(8u, 8u)
#define  EHCI_ULPI_VIEWPORT_DATWR              DEF_BIT_FIELD(8u, 0u)

//                                                                 --------------------- CFGFLAG ----------------------
#define  EHCI_CONFIGFLAG_CF                    DEF_BIT_00

//                                                                 ---------------------- PORTSC ----------------------
#define  EHCI_PORTSC_PTS_MSK                   DEF_BIT_FIELD(2u, 30u) // Synopsys only.
#define  EHCI_PORTSC_PSPD_MSK                  DEF_BIT_FIELD(2u, 26u) // Synopsys only.
#define  EHCI_PORTSC_PFSC                      DEF_BIT_24             // Synopsys only.
#define  EHCI_PORTSC_PHCD                      DEF_BIT_23             // Synopsys only.
#define  EHCI_PORTSC_WKOC_E                    DEF_BIT_22
#define  EHCI_PORTSC_WKDSCNNT_E                DEF_BIT_21
#define  EHCI_PORTSC_WKCNNT_E                  DEF_BIT_20
#define  EHCI_PORTSC_PTC_MSK                   DEF_BIT_FIELD(4u, 16u)
#define  EHCI_PORTSC_PIC_MSK                   DEF_BIT_FIELD(2u, 14u)
#define  EHCI_PORTSC_PO                        DEF_BIT_13
#define  EHCI_PORTSC_PP                        DEF_BIT_12
#define  EHCI_PORTSC_LS_MSK                    DEF_BIT_FIELD(2u, 10u)
#define  EHCI_PORTSC_HSP                       DEF_BIT_09       // Synopsys only.
#define  EHCI_PORTSC_PR                        DEF_BIT_08
#define  EHCI_PORTSC_SUSP                      DEF_BIT_07
#define  EHCI_PORTSC_FPR                       DEF_BIT_06
#define  EHCI_PORTSC_OCC                       DEF_BIT_05
#define  EHCI_PORTSC_OCA                       DEF_BIT_04
#define  EHCI_PORTSC_PEDC                      DEF_BIT_03
#define  EHCI_PORTSC_PED                       DEF_BIT_02
#define  EHCI_PORTSC_CSC                       DEF_BIT_01
#define  EHCI_PORTSC_CCS                       DEF_BIT_00

#define  EHCI_PORTSC_PTS_ULPI                  2u

#define  EHCI_PORTSC_PSPD_FULL                 0u
#define  EHCI_PORTSC_PSPD_LOW                  1u
#define  EHCI_PORTSC_PSPD_HIGH                 2u
#define  EHCI_PORTSC_PSPD_UNDEF                3u

#define  EHCI_PORTSC_PTC_DIS                   0u
#define  EHCI_PORTSC_PTC_J                     1u
#define  EHCI_PORTSC_PTC_K                     2u
#define  EHCI_PORTSC_PTC_SE0                   3u
#define  EHCI_PORTSC_PTC_PKT                   4u
#define  EHCI_PORTSC_PTC_FORCE_EN              5u

#define  EHCI_PORTSC_PIC_OFF                   0u
#define  EHCI_PORTSC_PIC_AMBER                 1u
#define  EHCI_PORTSC_PIC_GREEN                 2u
#define  EHCI_PORTSC_PIC_UNDEF                 3u

#define  EHCI_PORTSC_LS_SE0                    0u
#define  EHCI_PORTSC_LS_J                      2u
#define  EHCI_PORTSC_LS_K                      1u
#define  EHCI_PORTSC_LS_UNDEF                  3u

//                                                                 ----------------- OTGSC (SYNOPSYS) -----------------
#define  EHCI_OTGSC_DPIE                       DEF_BIT_30
#define  EHCI_OTGSC_MSE                        DEF_BIT_29
#define  EHCI_OTGSC_BSEIE                      DEF_BIT_28
#define  EHCI_OTGSC_BSVIE                      DEF_BIT_27
#define  EHCI_OTGSC_ASVIE                      DEF_BIT_26
#define  EHCI_OTGSC_AVVIE                      DEF_BIT_25
#define  EHCI_OTGSC_IDIE                       DEF_BIT_24
#define  EHCI_OTGSC_DPIS                       DEF_BIT_23
#define  EHCI_OTGSC_MSS                        DEF_BIT_21
#define  EHCI_OTGSC_BSEIS                      DEF_BIT_20
#define  EHCI_OTGSC_BSVIS                      DEF_BIT_19
#define  EHCI_OTGSC_ASVIS                      DEF_BIT_18
#define  EHCI_OTGSC_AVVIS                      DEF_BIT_17
#define  EHCI_OTGSC_IDIS                       DEF_BIT_16
#define  EHCI_OTGSC_DPS                        DEF_BIT_15
#define  EHCI_OTGSC_MST                        DEF_BIT_13
#define  EHCI_OTGSC_BSE                        DEF_BIT_12
#define  EHCI_OTGSC_BSV                        DEF_BIT_11
#define  EHCI_OTGSC_ASV                        DEF_BIT_10
#define  EHCI_OTGSC_AVV                        DEF_BIT_09
#define  EHCI_OTGSC_ID                         DEF_BIT_08
#define  EHCI_OTGSC_HABA                       DEF_BIT_07
#define  EHCI_OTGSC_IDPU                       DEF_BIT_05
#define  EHCI_OTGSC_DP                         DEF_BIT_04
#define  EHCI_OTGSC_OT                         DEF_BIT_03
#define  EHCI_OTGSC_HAAR                       DEF_BIT_02
#define  EHCI_OTGSC_VC                         DEF_BIT_01
#define  EHCI_OTGSC_VD                         DEF_BIT_00

//                                                                 ---------------- USBMODE (SYNOPSYS) ----------------
#define  EHCI_USBMODE_TXHSD_MSK                DEF_BIT_FIELD(3u, 12u)
#define  EHCI_USBMODE_SDIS                     DEF_BIT_04
#define  EHCI_USBMODE_SLOM                     DEF_BIT_03
#define  EHCI_USBMODE_ES                       DEF_BIT_02
#define  EHCI_USBMODE_CM_MSK                   DEF_BIT_FIELD(2u, 0u)

#define  EHCI_USBMODE_TXHSD_10                 0u
#define  EHCI_USBMODE_TXHSD_11                 1u
#define  EHCI_USBMODE_TXHSD_12                 2u
#define  EHCI_USBMODE_TXHSD_13                 3u
#define  EHCI_USBMODE_TXHSD_14                 4u
#define  EHCI_USBMODE_TXHSD_15                 5u
#define  EHCI_USBMODE_TXHSD_16                 6u
#define  EHCI_USBMODE_TXHSD_17                 7u

#define  EHCI_USBMODE_CM_IDLE                  0u
#define  EHCI_USBMODE_CM_DEV_CTRLR             2u
#define  EHCI_USBMODE_CM_HOST_CTRLR            3u

/********************************************************************************************************
 *                                           FRAME LIST ELEMENT
 *******************************************************************************************************/

#define  EHCI_FRM_LIST_T                       DEF_BIT_00
#define  EHCI_FRM_LIST_TYPE_MSK                DEF_BIT_FIELD(2u, 1u)
#define  EHCI_FRM_LIST_LINK_PTR_MSK            DEF_BIT_FIELD(27u, 5u)

#define  EHCI_FRM_LIST_TYPE_ITD                0u
#define  EHCI_FRM_LIST_TYPE_QH                 1u
#define  EHCI_FRM_LIST_TYPE_SITD               2u
#define  EHCI_FRM_LIST_TYPE_FSTN               3u

/********************************************************************************************************
 *                                               DESCRIPTORS
 *******************************************************************************************************/

//                                                                 ------------------------ QH ------------------------
#define  EHCI_QH_LEN                           sizeof(USBH_EHCI_HC_QH)

//                                                                 QH Horizontal Link Pointer.
#define  EHCI_QH_HOR_LINK_PTR_MSK              DEF_BIT_FIELD(27u, 5u)
#define  EHCI_QH_TYPE_SEL_MSK                  DEF_BIT_FIELD(2u, 1u)
#define  EHCI_QH_T                     DEF_BIT_00

#define  EHCI_QH_TYPE_SEL_iTD                  0u
#define  EHCI_QH_TYPE_SEL_QH                   1u
#define  EHCI_QH_TYPE_SEL_siTD                 2u
#define  EHCI_QH_TYPE_SEL_FSTN                 3u

//                                                                 Endpoint characteristics.
#define  EHCI_QH_RL_MSK                        DEF_BIT_FIELD(4u, 28u)
#define  EHCI_QH_C                             DEF_BIT_27
#define  EHCI_QH_MPS_MSK                       DEF_BIT_FIELD(11u, 16u)
#define  EHCI_QH_H                             DEF_BIT_15
#define  EHCI_QH_DTC                           DEF_BIT_14
#define  EHCI_QH_EPS_MSK                       DEF_BIT_FIELD(2u, 12u)
#define  EHCI_QH_ENDPT_MSK                     DEF_BIT_FIELD(4u, 8u)
#define  EHCI_QH_I                             DEF_BIT_07
#define  EHCI_QH_DEV_ADDR_MSK                  DEF_BIT_FIELD(7u, 0u)

#define  EHCI_QH_EPS_FULL                      0u
#define  EHCI_QH_EPS_LOW                       1u
#define  EHCI_QH_EPS_HIGH                      2u

//                                                                 Endpoint capabilities.
#define  EHCI_QH_MULT_MSK                      DEF_BIT_FIELD(2u, 30u)
#define  EHCI_QH_PORT_NBR_MSK                  DEF_BIT_FIELD(7u, 23u)
#define  EHCI_QH_HUB_ADDR_MSK                  DEF_BIT_FIELD(7u, 16u)
#define  EHCI_QH_SPLIT_CMPL_MSK                DEF_BIT_FIELD(8u, 8u)
#define  EHCI_QH_INT_SCHED_MSK                 DEF_BIT_FIELD(8u, 0u)

//                                                                 ----------------------- QTD ------------------------
#define  EHCI_QTD_PAGE_QTY                     5u
#define  EHCI_QTD_LEN                          sizeof(USBH_EHCI_HC_QTD)

//                                                                 Next qTD ptr.
//                                                                 Alternate next qTD ptr.
#define  EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK       DEF_BIT_FIELD(27u, 5u)
#define  EHCI_QTD_T                            DEF_BIT_00

//                                                                 Token.
#define  EHCI_QTD_DATA_TOGGLE                  DEF_BIT_31
#define  EHCI_QTD_TOTAL_BYTES_TO_XFER_MSK      DEF_BIT_FIELD(15u, 16u)
#define  EHCI_QTD_IOC                          DEF_BIT_15
#define  EHCI_QTD_C_PAGE_MSK                   DEF_BIT_FIELD(3u, 12u)
#define  EHCI_QTD_CERR_MSK                     DEF_BIT_FIELD(2u, 10u)
#define  EHCI_QTD_PID_CODE_MSK                 DEF_BIT_FIELD(2u, 8u)
#define  EHCI_QTD_STATUS_MSK                   DEF_BIT_FIELD(8u, 0u)

#define  EHCI_QTD_STATUS_ACTIVE                DEF_BIT_07
#define  EHCI_QTD_STATUS_HALTED                DEF_BIT_06
#define  EHCI_QTD_STATUS_DATA_BUF_ERR          DEF_BIT_05
#define  EHCI_QTD_STATUS_BABBLE                DEF_BIT_04
#define  EHCI_QTD_STATUS_XACT_ERR              DEF_BIT_03
#define  EHCI_QTD_STATUS_MISSED_UFRM           DEF_BIT_02
#define  EHCI_QTD_STATUS_SPLIT_X_STATE         DEF_BIT_01
#define  EHCI_QTD_STATUS_PING_STATE            DEF_BIT_00

#define  EHCI_QTD_PID_OUT                      0u
#define  EHCI_QTD_PID_IN                       1u
#define  EHCI_QTD_PID_SETUP                    2u

//                                                                 Buffer ptr.
#define  EHCI_QTD_BUF_PTR_MSK                  DEF_BIT_FIELD(20u, 12u)
#define  EHCI_QTD_CUR_OFFSET_MSK               DEF_BIT_FIELD(12u, 0u)

#define  EHCI_QTD_PAGE_LEN                     4096u

//                                                                 ----------------------- ITD ------------------------
//                                                                 Next link ptr.
#define  EHCI_ITD_NEXT_LINK_PTR_MSK            DEF_BIT_FIELD(27u, 5u)
#define  EHCI_ITD_TYPE_MSK                     DEF_BIT_FIELD(2u, 1u)
#define  EHCI_ITD_T                            DEF_BIT_00

#define  EHCI_ITD_TYPE_ITD                     0u
#define  EHCI_ITD_TYPE_QH                      1u
#define  EHCI_ITD_TYPE_SITD                    2u
#define  EHCI_ITD_TYPE_FSTN                    3u

//                                                                 Transaction field.
#define  EHCI_ITD_TRANSACTION_OFFSET_MSK       DEF_BIT_FIELD(12u, 0u)
#define  EHCI_ITD_PG_MSK                       DEF_BIT_FIELD(3u, 12u)
#define  EHCI_ITD_IOC                          DEF_BIT_15
#define  EHCI_ITD_TRANSACTION_LEN_MSK          DEF_BIT_FIELD(12u, 16u)
#define  EHCI_ITD_STATUS                       DEF_BIT_FIELD(4u, 28u)

#define  EHCI_ITD_STATUS_XACT_ERR              DEF_BIT_28
#define  EHCI_ITD_STATUS_BABBLE                DEF_BIT_29
#define  EHCI_ITD_STATUS_DATA_BUF_ERR          DEF_BIT_30
#define  EHCI_ITD_STATUS_ACTIVE                DEF_BIT_31

//                                                                 Buffer pointer field.
#define  EHCI_ITD_BUF_PTR_MSK                  DEF_BIT_FIELD(20u, 12u)

//                                                                 Buffer pointer 0.
#define  EHCI_ITD_DEV_ADDR_MSK                 DEF_BIT_FIELD(7u, 0u)
#define  EHCI_ITD_R                            DEF_BIT_07
#define  EHCI_ITD_ENDPT_MSK                    DEF_BIT_FIELD(4u, 8u)

//                                                                 Buffer pointer 1.
#define  EHCI_ITD_ENDPT_MPS_MSK                DEF_BIT_FIELD(11u, 0u)
#define  EHCI_ITD_IO                           DEF_BIT_11

//                                                                 Buffer pointer 2.
#define  EHCI_ITD_MULT_MSK                     DEF_BIT_FIELD(2u, 0u)

#define  EHCI_ITD_PAGE_LEN                     4096u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum usbh_ehci_ctrlr_type {
  USBH_EHCI_CTRLR_TYPE_STD,                                     // Standard EHCI controllers.
  USBH_EHCI_CTRLR_TYPE_SYNOPSYS                                 // Synopsys' EHCI based USB controller implementation.
} USBH_EHCI_CTRLR_TYPE;

/********************************************************************************************************
 *                                           REGISTER STRUCTURES
 *******************************************************************************************************/

//                                                                 ------------ EHCI CAPABILITY REGISTERS -------------
typedef struct usbh_ehci_cap_reg {
  CPU_REG08 CAPLENGTH;
  CPU_REG08 RSVD0;
  CPU_REG16 HCIVERSION;
  CPU_REG32 HCSPARAMS;
  CPU_REG32 HCCPARAMS;
  CPU_REG32 HCS_PORTROUTE[8u];
} USBH_EHCI_CAP_REG;

//                                                                 ------------ EHCI OPERATIONAL REGISTERS ------------
typedef struct usbh_ehci_oper_reg {
  CPU_REG32 USBCMD;
  CPU_REG32 USBSTS;
  CPU_REG32 USBINTR;
  CPU_REG32 FRINDEX;
  CPU_REG32 CTRLDSSEGMENT;
  CPU_REG32 PERIODICLISTBASE;
  CPU_REG32 ASYNCLISTADDR;
  CPU_REG32 TTCTRL;                                             // Synopsys only.
  CPU_REG32 BURSTSIZE;                                          // Synopsys only.
  CPU_REG32 TXFILLTUNING;                                       // Synopsys only.
  CPU_REG32 RSVD0[2u];
  CPU_REG32 ULPI_VIEWPORT;                                      // Synopsys only.
  CPU_REG32 RSVD1[3u];
  CPU_REG32 CONFIGFLAG;
  CPU_REG32 PORTSC[EHCI_RH_PORT_MAX_QTY];
  CPU_REG32 OTGSC;                                              // Synopsys only.
  CPU_REG32 USBMODE;                                            // Synopsys only.
} USBH_EHCI_OPER_REG;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbh_ehci_hcd_qtd USBH_EHCI_HCD_QTD;

typedef struct usbh_ehci_hcd_ep USBH_EHCI_HCD_EP;

/********************************************************************************************************
 *                                           QUEUE HEAD (QH)
 *
 * Note(s) : (1) qH descriptors require an alignment of 32 bytes. Typical length of a qH is 12 words
 *               (48 bytes). This leaves 4 words (16 bytes) before next alignment. In most of the cases,
 *               this space will be lost, that's why the cost of using it is neglectable.
 *******************************************************************************************************/

typedef struct usbh_ehci_hc_qh {
  //                                                               ----------------- HARDWARE FIELDS ------------------
  CPU_REG32 HorLinkPtr;
  CPU_REG32 EP_Characteristics;
  CPU_REG32 EP_Capabilities;
  CPU_REG32 QTD_CurPtr;
  CPU_REG32 QTD_NextPtr;
  CPU_REG32 QTD_AltNextPtr;
  CPU_REG32 QTD_Token;
  CPU_REG32 QTD_BufPagePtrList[EHCI_QTD_PAGE_QTY];

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)
  CPU_REG32 QTD_ExtBufPagePtrList[EHCI_QTD_PAGE_QTY];
#endif

  //                                                               ----------- CONTROL FIELDS. SEE NOTE 1. ------------
  USBH_EHCI_HCD_EP  *HCD_EP_Ptr;                                // Pointer to HCD EP strcut.

  USBH_EHCI_HCD_QTD *HCD_QTD_HeadPtr;                           // Pointer to head of HCD qTD queue.
  USBH_EHCI_HCD_QTD *HCD_QTD_TailPtr;                           // Pointer to tail of HCD qTD queue.
} USBH_EHCI_HC_QH;

/********************************************************************************************************
 *                                   HOST CONTROLLER DRIVER ENDPOINT
 *******************************************************************************************************/

struct usbh_ehci_hcd_ep {
  USBH_DEV_HANDLE           DevHandle;                          // Handle on device.
  USBH_EP_HANDLE            EP_Handle;                          // Handle on endpoint.

  USBH_HCD_EP_PARAMS        EP_Params;                          // Endpoint parameters.

  CPU_BOOLEAN               IsSuspended;                        // Flag that indicates if EP is currently suspended.

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U                FrmListLevel;                       // EP level in frame list binary tree.
  CPU_INT16U                FrmListIx;                          // EP index in frame list binary tree at given level.

  USBH_HCD_BIN_TREE_EP_DESC BinTreeEP_Desc;                     // Binary tree endpoint position descriptor.
#endif

  void                      *HC_EP_Ptr;                         // Ptr to EP's HW structure. (qH for non-isoc EP.)

  USBH_EHCI_HCD_EP          *NextPtr;                           // Pointer to next structure in list.
};

/********************************************************************************************************
 *                                   QUEUE TRANSFER DESCRIPTOR (QTD)
 *******************************************************************************************************/

//                                                                 ---------------------- HC QTD ----------------------
typedef struct usbh_ehci_hc_qtd {
  CPU_REG32 NextPtr;
  CPU_REG32 AltNextPtr;
  CPU_REG32 Token;
  CPU_REG32 BufPagePtrList[EHCI_QTD_PAGE_QTY];

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)
  CPU_REG32 ExtBufPagePtrList[EHCI_QTD_PAGE_QTY];
#endif
} USBH_EHCI_HC_QTD;

//                                                                 --------------------- HCD QTD ----------------------
struct usbh_ehci_hcd_qtd {
  USBH_EHCI_HC_QTD  *HC_qTD_HeadPtr;                            // Pointer to URB's head HC qTD.
  USBH_EHCI_HC_QTD  *HC_qTD_TailPtr;                            // Pointer to URB's tail HC qTD.

  USBH_EHCI_HC_QTD  *HC_qTD_DummyPtr;                           // Pointer to last dummy HC qTD.

  CPU_INT08U        *HW_BufPtr;                                 // Pointer to hardware buffer.

  USBH_EHCI_HCD_QTD *NextPtr;                                   // Pointer to next HCD qTD.
};

/********************************************************************************************************
 *                                               HCD DATA
 *
 * Note(s) : (1) Maximum periodic interval is always expressed in frame.
 *******************************************************************************************************/

typedef struct usbh_hcd_data {
  MEM_SEG              *DataMemSegPtr;                          // Pointer to mem seg to use for control data.
  MEM_SEG              *HC_DescMemSegPtr;                       // Pointer to mem seg to use for HW descriptors.

  USBH_EHCI_CAP_REG    *CapRegPtr;                              // Pointer to EHCI capability  registers.
  USBH_EHCI_OPER_REG   *OperRegPtr;                             // Pointer to EHCI operational registers.

  USBH_EHCI_CTRLR_TYPE CtrlrType;                               // Indicates type of controller.

  CPU_INT08U           RH_PortQty;                              // Qty or port on root hub.

  KAL_SEM_HANDLE       QH_AsyncUnlinkDoneSemHandle;             // Sem that signals end of async qH unlink oper.
  KAL_LOCK_HANDLE      QH_AsyncUnlinkDoneLockHandle;            // Lock that protects access to async qH unlink oper...
                                                                // ... done ressources.
  USBH_EHCI_HCD_EP     *AsyncHCD_EP_ListHeadPtr;                // Asynchronous EP list head

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT32U           *FrameList;                              // Pointer to frame list table.

  USBH_HCD_BIN_TREE_BW BinTreeBW;                               // Binary tree bandwidth strcutre.

  USBH_EHCI_HCD_EP     *PeriodicHCD_EP_ListHeadPtr;             // Periodic EP list head

  USBH_EHCI_HC_QH      *PeriodicDummyQH_Ptr;                    // Pointer to dummy qH always used at end of frame list.

  CPU_INT16U           MaxPeriodicInterval;                     // Maximum periodic interval. See note 1.
#endif

  MEM_DYN_POOL         HCD_EP_Pool;                             // Pool of HCD EP  structures.
  MEM_DYN_POOL         HCD_qTD_Pool;                            // Pool of HCD qTD structures.

  MEM_DYN_POOL         HC_QH_Pool;                              // Pool of HC qH  structures.
  MEM_DYN_POOL         HC_qTD_Pool;                             // Pool of HC qTD structures.

  CPU_ADDR             DedMemAddrStart;                         // Start addr of dedicated memory.
  CPU_ADDR             DedMemAddrEnd;                           // End   addr of dedicated memory.
  MEM_DYN_POOL         DedMemBufPool;                           // Pool of HCD buffers located in dedicated memory  ...
                                                                // ... to where app buffers will be copied if necessary.
  CPU_INT32U           DedicatedDataBufMaxLen;                  // Length, in octets, of dedicated memory data buffers.
} USBH_HCD_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DRIVER API FUNCTIONS
 *******************************************************************************************************/

static void USBH_HCD_InitStd(USBH_HC_DRV     *p_hc_drv,
                             MEM_SEG         *p_mem_seg,
                             MEM_SEG         *p_mem_seg_buf,
                             USBH_HC_CFG_EXT *p_hc_cfg_ext,
                             RTOS_ERR        *p_err);

static void USBH_HCD_InitSynopsys(USBH_HC_DRV     *p_hc_drv,
                                  MEM_SEG         *p_mem_seg,
                                  MEM_SEG         *p_mem_seg_buf,
                                  USBH_HC_CFG_EXT *p_hc_cfg_ext,
                                  RTOS_ERR        *p_err);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_Uninit(USBH_HC_DRV *p_hc_drv,
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

static void USBH_HCD_ISR_Handle(USBH_HC_DRV *p_hc_drv);

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

static void USBH_HCD_InitHandle(USBH_HC_DRV          *p_hc_drv,
                                MEM_SEG              *p_mem_seg,
                                MEM_SEG              *p_mem_seg_buf,
                                USBH_HC_CFG_EXT      *p_hc_cfg_ext,
                                USBH_EHCI_CTRLR_TYPE ctrlr_type,
                                RTOS_ERR             *p_err);

//                                                                 --------- HCD EP LIST MANAGEMENT FUNCTIONS ---------
static void USBH_EHCI_HCD_EP_ListInsert(USBH_EHCI_HCD_EP **pp_hcd_ep_head,
                                        USBH_EHCI_HCD_EP *p_hcd_ep);

static void USBH_EHCI_HCD_EP_ListRemove(USBH_EHCI_HCD_EP **pp_hcd_ep_head,
                                        USBH_EHCI_HCD_EP *p_hcd_ep);

static void USBH_EHCI_HCD_EP_ListXferCmpl(USBH_EHCI_HCD_EP *p_hcd_ep_head);

//                                                                 --------- GENERAL QH MANAGEMENT FUNCTIONS ----------
static USBH_EHCI_HC_QH *USBH_EHCI_QH_Create(USBH_HC_DRV      *p_hc_drv,
                                            CPU_INT08U       dev_addr,
                                            USBH_EHCI_HCD_EP *p_hcd_ep,
                                            RTOS_ERR         *p_err);

static void USBH_EHCI_QH_Free(USBH_HCD_DATA    *p_hcd_data,
                              USBH_EHCI_HCD_EP *p_hcd_ep,
                              RTOS_ERR         *p_err);

static void USBH_EHCI_HC_QH_Link(USBH_HCD_DATA    *p_hcd_data,
                                 USBH_EHCI_HCD_EP *p_hcd_ep,
                                 RTOS_ERR         *p_err);

static void USBH_EHCI_HC_QH_Unlink(USBH_HCD_DATA    *p_hcd_data,
                                   USBH_EHCI_HCD_EP *p_hcd_ep,
                                   RTOS_ERR         *p_err);

//                                                                 --------- PERIODIC QH MANAGEMENT FUNCTIONS ---------
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_EHCI_QH_PeriodicInsert(USBH_HC_DRV      *p_hc_drv,
                                        USBH_EHCI_HCD_EP *p_hcd_ep,
                                        RTOS_ERR         *p_err);

static void USBH_EHCI_QH_BinTreePosToFrmList(USBH_HCD_BIN_TREE_EP_DESC pos_desc,
                                             CPU_INT16U                interval_max,
                                             CPU_INT08U                *p_frm_list_level,
                                             CPU_INT16U                *p_frm_list_ix,
                                             CPU_INT08U                *p_s_mask);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
static CPU_INT16U USBH_EHCI_PeriodicBranchBwGet(CPU_INT16U branch_ix,
                                                void       *p_arg,
                                                RTOS_ERR   *p_err);
#endif
#endif

//                                                                 ------------- QTD MANAGMEENT FUNCTIONS -------------
static USBH_EHCI_HCD_QTD *USBH_EHCI_qTD_Submit(USBH_HCD_DATA       *p_hcd_data,
                                               USBH_EHCI_HCD_EP    *p_hcd_ep,
                                               USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                               CPU_INT08U          *p_hw_buf,
                                               RTOS_ERR            *p_err);

static CPU_INT08U *USBH_EHCI_qTD_Complete(USBH_HCD_DATA     *p_hcd_data,
                                          USBH_EHCI_HCD_EP  *p_hcd_ep,
                                          USBH_EHCI_HCD_QTD *p_hcd_qtd,
                                          CPU_INT32U        *p_rem_len,
                                          RTOS_ERR          *p_err);

static void USBH_EHCI_qTD_Abort(USBH_HCD_DATA     *p_hcd_data,
                                USBH_EHCI_HCD_EP  *p_hcd_ep,
                                USBH_EHCI_HCD_QTD *p_hcd_qtd);

static CPU_INT32U USBH_EHCI_qTD_Free(USBH_HCD_DATA     *p_hcd_data,
                                     USBH_EHCI_HCD_QTD *p_hcd_qtd);

//                                                                 ------------------ MISCELLANEOUS -------------------
static void USBH_EHCI_AsyncQH_IOAAD_IntPend(USBH_HCD_DATA *p_hcd_data,
                                            RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                   INITIALIZED GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               API FOR FULLY EHCI COMPLIANT CONTROLLER
 *
 * Note(s) : (1) This API should be used with USB host controllers that implements the Enhanced Host
 *               Controller Interface (EHCI) specification maintained by Intel Corporation. The host
 *               controller should not support Full-Speed and Low-Speed devices connected directly to the
 *               root hub and should rely on Companion Controller(s) for these.
 *******************************************************************************************************/

USBH_HC_DRV_API USBH_HCD_API_EHCI = {
  USBH_HCD_InitStd,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_HCD_Uninit,
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

/********************************************************************************************************
 *                               API FOR SYNOPSYS' EHCI COMPLIANT CONTROLLER
 *
 * Note(s) : (1) This API should be used with USB host controllers implementing Synopsys inc. Enhanced
 *               Host Controller Interface (EHCI) specification compliant USB host IP but that embeds a
 *               transaction translator (and hence does not require a companion controller to handle Full
 *               and Low speed devices connected to the root hub).
 *******************************************************************************************************/

USBH_HC_DRV_API USBH_HCD_API_EHCI_SYNOPSYS = {
  USBH_HCD_InitSynopsys,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_HCD_Uninit,
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

//                                                                 ----- EHCI HOST CONTROLLER ROOT HUB DRIVER API -----
USBH_HC_RH_API USBH_HCD_API_RH_EHCI = {
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
 *                                           USBH_HCD_InitStd()
 *
 * @brief    Initializes EHCI Host Controller driver. This function should be used with Synopsys EHCI
 *           compliant controller.
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    p_mem_seg       Pointer to memory segment from where driver's internal control data
 *
 * @param    p_mem_seg_buf   Pointer to memory segment from where data buffers are allocated.
 *
 * @param    p_cfg           Pointer to configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               -RETURNED BY USBH_HCD_InitHandle()-
 *                           See USBH_HCD_InitHandle() for additional return error codes.
 *
 *           Core layer.
 *******************************************************************************************************/
static void USBH_HCD_InitStd(USBH_HC_DRV     *p_hc_drv,
                             MEM_SEG         *p_mem_seg,
                             MEM_SEG         *p_mem_seg_buf,
                             USBH_HC_CFG_EXT *p_hc_cfg_ext,
                             RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_SET((p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH), *p_err, RTOS_ERR_INVALID_CFG,; );

  USBH_HCD_InitHandle(p_hc_drv,
                      p_mem_seg,
                      p_mem_seg_buf,
                      p_hc_cfg_ext,
                      USBH_EHCI_CTRLR_TYPE_STD,
                      p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_InitStd()
 *
 * @brief    Initializes EHCI Host Controller driver. This function should be used with Synopsys EHCI
 *           ant controller.
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    p_mem_seg       Pointer to memory segment from where driver's internal control data
 *
 * @param    p_mem_seg_buf   Pointer to memory segment from where data buffers are allocated.
 *
 * @param    p_cfg           Pointer to configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               -RETURNED BY USBH_HCD_InitHandle()-
 *                           See USBH_HCD_InitHandle() for additional return error codes.
 *******************************************************************************************************/
static void USBH_HCD_InitSynopsys(USBH_HC_DRV     *p_hc_drv,
                                  MEM_SEG         *p_mem_seg,
                                  MEM_SEG         *p_mem_seg_buf,
                                  USBH_HC_CFG_EXT *p_hc_cfg_ext,
                                  RTOS_ERR        *p_err)
{
  USBH_HCD_InitHandle(p_hc_drv,
                      p_mem_seg,
                      p_mem_seg_buf,
                      p_hc_cfg_ext,
                      USBH_EHCI_CTRLR_TYPE_SYNOPSYS,
                      p_err);
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Uninit()
 *
 * @brief    Un-initializes Host Controller driver.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE       Operation successful.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_Uninit(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
  USBH_HCD_DATA *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;

  if (p_hcd_data != DEF_NULL) {
    if (KAL_SEM_HANDLE_IS_NULL(p_hcd_data->QH_AsyncUnlinkDoneSemHandle) == DEF_NO) {
      KAL_SemDel(p_hcd_data->QH_AsyncUnlinkDoneSemHandle);
    }

    if (KAL_LOCK_HANDLE_IS_NULL(p_hcd_data->QH_AsyncUnlinkDoneLockHandle) == DEF_NO) {
      KAL_LockDel(p_hcd_data->QH_AsyncUnlinkDoneLockHandle);
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HCD_Start()
 *
 * @brief    Starts Host Controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE               Operation successful.
 *                           - RTOS_ERR_IO                 BSP function call failed.
 *                           - RTOS_ERR_FAIL               Failed to start controller.
 *                           - RTOS_ERR_NOT_SUPPORTED      Periodic interval unsupported.
 *                           - RTOS_ERR_SEG_OVF            Failed to alloc frame list table.
 *                           - RTOS_ERR_DRV_EP_ALLOC   Failed to alloc periodic dummy qh.
 *                           -RETURNED BY USBH_HCD_BinTreeBwInit()-
 *                       See USBH_HCD_BinTreeBwInit() for additional return error codes.
 *
 * @note     (1) This function allocates all the data structures required for periodic transfers.
 *               This cannot be done at init since it is required to read some registers and the
 *               controller may not be ready at that moment.
 *******************************************************************************************************/
static void USBH_HCD_Start(USBH_HC_DRV *p_hc_drv,
                           RTOS_ERR    *p_err)
{
  CPU_INT16U         timeout;
  CPU_INT32U         usbcmd_reg;
  CPU_INT08U         usbcmd_fls_val = EHCI_USBCMD_FLS_1024_64;
  CPU_BOOLEAN        usbcmd_fs2_val = DEF_NO;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_HC_BSP_API    *p_bsp_api = p_hc_drv->BSP_API_Ptr;
  USBH_EHCI_CAP_REG  *p_cap_reg;
  USBH_EHCI_OPER_REG *p_oper_reg;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT16U      interval_ix;
  USBH_EHCI_HC_QH *p_dummy_qh;
#endif

  //                                                               ---------- INIT CLK, IO, INT, PWR IN BSP -----------
  if (p_bsp_api != DEF_NULL) {
    CPU_BOOLEAN ok;

    if (p_bsp_api->IO_Cfg != DEF_NULL) {
      ok = p_bsp_api->IO_Cfg();                                 // Configure board specific IO.
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP IO_Cfg() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }

    if (p_bsp_api->PwrCfg != DEF_NULL) {
      ok = p_bsp_api->PwrCfg();                                 // Configure board specific pwr.
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP PwrCfg() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }

    if (p_bsp_api->ClkCfg != DEF_NULL) {
      ok = p_bsp_api->ClkCfg();                                 // Configure board specific clk.
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP ClkCfg() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }

    if (p_bsp_api->IntCfg != DEF_NULL) {
      ok = p_bsp_api->IntCfg();                                 // Configure board specific interrupts.
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP IntCfg() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }
  }

  //                                                               Retrieve base address of cap and oper registers.
  p_cap_reg = (USBH_EHCI_CAP_REG *) p_hc_drv->HW_InfoPtr->BaseAddr;
  p_oper_reg = (USBH_EHCI_OPER_REG *)(p_hc_drv->HW_InfoPtr->BaseAddr + p_cap_reg->CAPLENGTH);

  //                                                               -------------- START EHCI CONTROLLER ---------------
  timeout = 1000u;
  p_oper_reg->USBCMD = EHCI_USBCMD_HCRESET;                     // Apply hardware reset.
  do {
    timeout--;                                                  // Wait for the reset completion.
  } while ((DEF_BIT_IS_CLR(p_oper_reg->USBCMD, EHCI_USBCMD_HCRESET) == DEF_NO)
           && (timeout > 0u));
  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  if (p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_SYNOPSYS) {
    //                                                             Set ctrlr in host mode.
    DEF_BIT_FIELD_WR(p_oper_reg->USBMODE,
                     EHCI_USBMODE_CM_HOST_CTRLR,
                     EHCI_USBMODE_CM_MSK);
  }

  //                                                               Ensure controller is halted.
  if (DEF_BIT_IS_SET(p_oper_reg->USBSTS, EHCI_USBSTS_HC_HAL) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);

    LOG_ERR(("In EHCI start -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  p_hcd_data->CapRegPtr = p_cap_reg;
  p_hcd_data->OperRegPtr = p_oper_reg;
  p_hcd_data->AsyncHCD_EP_ListHeadPtr = DEF_NULL;
  p_hcd_data->RH_PortQty = DEF_BIT_FIELD_RD(p_cap_reg->HCSPARAMS,
                                            EHCI_HCSPARAMS_N_PORTS_MSK);
  if (p_hcd_data->RH_PortQty > EHCI_RH_PORT_MAX_QTY) {
    p_hcd_data->RH_PortQty = EHCI_RH_PORT_MAX_QTY;
    LOG_DBG(("WARNING - Number of ports usable on EHCI root hub: ", (u)EHCI_RH_PORT_MAX_QTY));
  }

  //                                                               ---------- INITIALIZE PERIODIC TRANSFERS -----------
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if (p_hcd_data->FrameList == DEF_NULL) {
    CPU_INT16U max_periodic_interval;
    CPU_SIZE_T periodic_frm_list_align;
    CPU_SIZE_T bin_tree_bw_branch_qty;

    if (p_hcd_data->MaxPeriodicInterval == USBH_PERIODIC_XFER_MAX_INTERVAL_DFLT) {
      //                                                           Retrieve default value for periodic interval.
      max_periodic_interval = (DEF_BIT_IS_SET(p_cap_reg->HCCPARAMS, EHCI_HCCPARAMS_PFLF) == DEF_YES) ? USBH_PERIODIC_XFER_MAX_INTERVAL_256
                              : USBH_PERIODIC_XFER_MAX_INTERVAL_1024;
    } else {
      max_periodic_interval = p_hcd_data->MaxPeriodicInterval;
    }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    switch (max_periodic_interval) {
      case 1024u:                                               // EHCI always support frame list size of 1024.
        break;

      case 256u:
      case 512u:
        //                                                         256 or 512 supported if frame list is programmable.
        if (DEF_BIT_IS_SET(p_cap_reg->HCCPARAMS, EHCI_HCCPARAMS_PFLF) == DEF_YES) {
          break;
        }
      //                                                           fallthrough

      case 8u:
      case 16u:
      case 32u:
      case 64u:
      case 128u:
        //                                                         Synopsys EHCI compliant ctrlr support other sizes.
        if (p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_SYNOPSYS) {
          break;
        }
      //                                                           fallthrough

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED,; )
    }
#endif

    if (p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr != DEF_NULL) {
      periodic_frm_list_align = DEF_MAX(EHCI_PERIODIC_FRM_LIST_ALIGN, p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr->BufAlignOctets);
    } else {
      periodic_frm_list_align = EHCI_PERIODIC_FRM_LIST_ALIGN;
    }

    //                                                             Get a mem block for Periodic Frame List
    p_hcd_data->FrameList = (CPU_INT32U *)Mem_SegAllocHW("USBH - EHCI periodic list base",
                                                         p_hcd_data->HC_DescMemSegPtr,
                                                         (max_periodic_interval * sizeof(CPU_INT32U)),
                                                         periodic_frm_list_align,
                                                         DEF_NULL,
                                                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Allocate binary tree used for BW usage tracking.
    bin_tree_bw_branch_qty = max_periodic_interval;
    if (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
      bin_tree_bw_branch_qty *= 8u;
    }

    USBH_HCD_BinTreeBwInit(&p_hcd_data->BinTreeBW,
                           p_hcd_data->DataMemSegPtr,
                           bin_tree_bw_branch_qty,
                           p_hc_drv->HW_InfoPtr->RH_Spd,
                           (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) ? DEF_YES : DEF_NO,
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
                           DEF_NULL,
                           DEF_NULL,
#else
                           USBH_EHCI_PeriodicBranchBwGet,
                           p_hcd_data,
#endif
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_hcd_data->MaxPeriodicInterval = max_periodic_interval;
    p_hcd_data->PeriodicHCD_EP_ListHeadPtr = DEF_NULL;
  }

  //                                                               Alloc dummy qH.
  p_dummy_qh = (USBH_EHCI_HC_QH *)Mem_DynPoolBlkGet(&p_hcd_data->HC_QH_Pool,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }

  p_dummy_qh->HorLinkPtr = EHCI_QH_T;                           // Dummy qH always terminates frame list entry.
  p_dummy_qh->EP_Characteristics = DEF_BIT_NONE;
  p_dummy_qh->EP_Capabilities = DEF_BIT_NONE;
  p_dummy_qh->QTD_CurPtr = DEF_NULL;
  p_dummy_qh->QTD_NextPtr = EHCI_QTD_T;
  p_dummy_qh->QTD_AltNextPtr = EHCI_QTD_T;
  p_dummy_qh->QTD_Token = DEF_BIT_NONE;
  p_dummy_qh->QTD_BufPagePtrList[0u] = DEF_NULL;
  p_dummy_qh->QTD_BufPagePtrList[1u] = DEF_NULL;
  p_dummy_qh->QTD_BufPagePtrList[2u] = DEF_NULL;
  p_dummy_qh->QTD_BufPagePtrList[3u] = DEF_NULL;
  p_dummy_qh->QTD_BufPagePtrList[4u] = DEF_NULL;
  p_dummy_qh->HCD_EP_Ptr = DEF_NULL;

  CPU_DCACHE_RANGE_FLUSH(p_dummy_qh, EHCI_QH_LEN);

  for (interval_ix = 0u; interval_ix < p_hcd_data->MaxPeriodicInterval; interval_ix++) {
    p_hcd_data->FrameList[interval_ix] = (CPU_INT32U)p_dummy_qh & EHCI_FRM_LIST_LINK_PTR_MSK;

    DEF_BIT_FIELD_WR(p_hcd_data->FrameList[interval_ix],
                     EHCI_FRM_LIST_TYPE_QH,
                     EHCI_FRM_LIST_TYPE_MSK);
  }

  CPU_DCACHE_RANGE_FLUSH(p_hcd_data->FrameList,
                         max_periodic_interval * sizeof(CPU_INT32U));

  p_hcd_data->PeriodicDummyQH_Ptr = p_dummy_qh;

  switch (p_hcd_data->MaxPeriodicInterval) {
    case USBH_PERIODIC_XFER_MAX_INTERVAL_032:
      usbcmd_fs2_val = DEF_YES; // fallthrough
    case USBH_PERIODIC_XFER_MAX_INTERVAL_512:
      usbcmd_fls_val = EHCI_USBCMD_FLS_512_32;
      break;

    case USBH_PERIODIC_XFER_MAX_INTERVAL_016:
      usbcmd_fs2_val = DEF_YES; // fallthrough
    case USBH_PERIODIC_XFER_MAX_INTERVAL_256:
      usbcmd_fls_val = EHCI_USBCMD_FLS_256_16;
      break;

    case USBH_PERIODIC_XFER_MAX_INTERVAL_008:
      usbcmd_fs2_val = DEF_YES; // fallthrough
    case USBH_PERIODIC_XFER_MAX_INTERVAL_128:
      usbcmd_fls_val = EHCI_USBCMD_FLS_128_8;
      break;

    case USBH_PERIODIC_XFER_MAX_INTERVAL_064:
      usbcmd_fs2_val = DEF_YES; // fallthrough
    case USBH_PERIODIC_XFER_MAX_INTERVAL_1024:
    default:
      usbcmd_fls_val = EHCI_USBCMD_FLS_1024_64;
      break;
  }
#endif

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)                     // Set ctrl data seg if arch is 64 bits.
  if (DEF_BIT_IS_SET(p_cap_reg->HCCPARAMS, EHCI_HCCPARAMS_64BIT) == DEF_YES) {
    MEM_SEG_INFO seg_info;

    (void)Mem_SegRemSizeGet(p_hcd_data->HC_DescMemSegPtr,
                            1u,
                            &seg_info,
                            &err_lib);
    RTOS_ASSERT_CRITICAL((err_lib == RTOS_ERR_NONE), RTOS_ERR_CRITICAL_FAIL,; )

    p_oper_reg->CTRLDSSEGMENT = (CPU_INT32U)DEF_BIT_FIELD_RD(seg_info.AddrBase,
                                                             EHCI_64BIT_SEGMENT_MSK);
  }
#endif

  usbcmd_reg = 0u;

  //                                                               If HC supports async sched park mode, enable it.
  if (DEF_BIT_IS_SET(p_cap_reg->HCCPARAMS, EHCI_HCCPARAMS_ASPC) == DEF_YES) {
    DEF_BIT_SET(usbcmd_reg, EHCI_USBCMD_ASPME);

    DEF_BIT_FIELD_WR(usbcmd_reg, 3u, EHCI_USBCMD_ASPMC_MSK);
  }

  DEF_BIT_FIELD_WR(usbcmd_reg,
                   EHCI_USBCMD_ITC_1,
                   EHCI_USBCMD_ITC_MSK);

  DEF_BIT_FIELD_WR(usbcmd_reg,
                   usbcmd_fls_val,
                   EHCI_USBCMD_FLS_MSK);

  if (usbcmd_fs2_val == DEF_YES) {
    DEF_BIT_SET(usbcmd_reg, EHCI_USBCMD_FLS2);                  // Synopsys only.
  }

  DEF_BIT_SET(usbcmd_reg, EHCI_USBCMD_RS);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  DEF_BIT_SET(usbcmd_reg, EHCI_USBCMD_PSE);

  p_oper_reg->PERIODICLISTBASE = (CPU_INT32U)p_hcd_data->FrameList;
#else
  p_oper_reg->PERIODICLISTBASE = DEF_NULL;
#endif

  p_oper_reg->ASYNCLISTADDR = DEF_NULL;
  p_oper_reg->USBCMD = usbcmd_reg;

  //                                                               Force ports in full-speed if requested.
  if ((p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_SYNOPSYS)
      && (p_hc_drv->HW_InfoPtr->RH_Spd != USBH_DEV_SPD_HIGH)) {
    CPU_INT08U rh_port;

    for (rh_port = 0u; rh_port < p_hcd_data->RH_PortQty; rh_port++) {
      DEF_BIT_SET(p_oper_reg->PORTSC[rh_port],
                  EHCI_PORTSC_PFSC);
    }
  }

  if (p_bsp_api != DEF_NULL) {
    CPU_BOOLEAN ok;

    if (p_bsp_api->Start != DEF_NULL) {
      ok = p_bsp_api->Start();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP Start() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }
  }

  p_oper_reg->USBINTR = EHCI_USBINTR_USBIE                      // USB Interrupt Enable.
                        | EHCI_USBINTR_USBEIE                   // USB Error Interrupt Enable.
                        | EHCI_USBINTR_HSEE                     // Host System Error Enable.
                        | EHCI_USBINTR_IOAAE;                   // Interrupt on Async Advance Enable.

  p_oper_reg->CONFIGFLAG = EHCI_CONFIGFLAG_CF;                  // All port routed to EHCI.

  LOG_VRB(("EHCI driver started"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Stop()
 *
 * @brief    Stops Host Controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *                           - RTOS_ERR_IO     BSP's Stop function failed.
 *                           - RTOS_ERR_FAIL   Failed to stop EHCI controller.
 *******************************************************************************************************/
static void USBH_HCD_Stop(USBH_HC_DRV *p_hc_drv,
                          RTOS_ERR    *p_err)
{
  CPU_INT16U         retry;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_HC_BSP_API    *p_bsp_api = p_hc_drv->BSP_API_Ptr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;
  USBH_EHCI_CAP_REG  *p_cap_reg = p_hcd_data->CapRegPtr;

  p_oper_reg->USBCMD = DEF_BIT_NONE;                            // Stop EHCI ctrlr.

  retry = DEF_INT_16U_MAX_VAL;
  do {
    retry--;
  } while ((DEF_BIT_IS_CLR(p_oper_reg->USBSTS, EHCI_USBSTS_HC_HAL) == DEF_YES)
           && (retry > 0u));
  if (retry == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  //                                                               Give port ownership to Companion Ctrlr if avail.
  if (DEF_BIT_FIELD_RD(p_cap_reg->HCCPARAMS, EHCI_HCSPARAMS_N_CC_MSK) > 0u) {
    p_oper_reg->CONFIGFLAG = DEF_BIT_NONE;
  }

  p_oper_reg->USBINTR = DEF_BIT_NONE;                           // Disable all int.

  if (p_bsp_api != DEF_NULL) {
    CPU_BOOLEAN ok;

    if (p_bsp_api->Stop != DEF_NULL) {
      ok = p_bsp_api->Stop();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

        LOG_ERR(("In EHCI BSP Stop() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }
    }
  }

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  {
    RTOS_ERR err_lib;

    //                                                             Free periodic dummy qH.
    Mem_DynPoolBlkFree(&p_hcd_data->HC_QH_Pool,
                       (void *)p_hcd_data->PeriodicDummyQH_Ptr,
                       &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing periodic dummy qH -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    }
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_Suspend()
 *
 * @brief    Suspends host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *                           - RTOS_ERR_FAIL   Failed to halt host controller.
 *******************************************************************************************************/
static void USBH_HCD_Suspend(USBH_HC_DRV *p_hc_drv,
                             RTOS_ERR    *p_err)
{
  CPU_INT16U         retry;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  DEF_BIT_CLR(p_oper_reg->USBCMD, EHCI_USBCMD_RS);              // Stop host controller.

  //                                                               Ensure HC halted.
  retry = DEF_INT_16U_MAX_VAL;
  do {
    retry--;
  } while ((DEF_BIT_IS_CLR(p_oper_reg->USBSTS, EHCI_USBSTS_HC_HAL) == DEF_YES)
           && (retry > 0u));
  if (retry == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  LOG_VRB(("EHCI driver suspended"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Resume()
 *
 * @brief    Resumes host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE       Operation successful.
 *******************************************************************************************************/
static void USBH_HCD_Resume(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  DEF_BIT_SET(p_oper_reg->USBCMD, EHCI_USBCMD_RS);              // Resume host controller.

  LOG_VRB(("EHCI driver resumed"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_FrameNbrGet()
 *
 * @brief    Retrieves current (micro)frame number.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @return   (micro)Frame number.
 *******************************************************************************************************/
static CPU_INT16U USBH_HCD_FrameNbrGet(USBH_HC_DRV *p_hc_drv)
{
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;
  CPU_INT16U         frame_nbr;
  CPU_INT16U         uframe_nbr;
  CPU_INT16U         frame;

  frame = p_oper_reg->FRINDEX;
  frame_nbr = DEF_BIT_FIELD_RD(frame, EHCI_FRINDEX_FI_FRAME_MSK);
  uframe_nbr = DEF_BIT_FIELD_RD(frame, EHCI_FRINDEX_FI_UFRAME_MSK);

  frame = USBH_FRM_NBR_FMT(frame_nbr, uframe_nbr);

  return (frame);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Open()
 *
 * @brief    Opens an endpoint.
 *
 * @param    p_hc_drv            Pointer to host controller driver structure.
 *
 * @param    dev_handle          Device handle.
 *
 * @param    ep_handle           Endpoint handle.
 *
 * @param    p_hcd_ep_params     Pointer to Host Controller Driver endpoint parameters structure.
 *
 * @param    pp_hcd_ep_data      Pointer to variable that will receive a pointer to HCD's endpoint
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *                                   - RTOS_ERR_NONE               Operation successful.
 *                                   - RTOS_ERR_DRV_EP_ALLOC   Unable to allocated structure for endpoint.
 *                                   - RTOS_ERR_NOT_SUPPORTED      Endpoint type not supported.
 *******************************************************************************************************/
static void USBH_HCD_EP_Open(USBH_HC_DRV        *p_hc_drv,
                             USBH_DEV_HANDLE    dev_handle,
                             USBH_EP_HANDLE     ep_handle,
                             USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                             void               **pp_hcd_ep_data,
                             RTOS_ERR           *p_err)
{
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = DEF_NULL;

  //                                                               Allocate  HCD EP.
  p_hcd_ep = (USBH_EHCI_HCD_EP *)Mem_DynPoolBlkGet(&p_hcd_data->HCD_EP_Pool,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);

    LOG_ERR(("EHCI driver cannot allocate HCD QH -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  //                                                               Initialize HCD EP.
  p_hcd_ep->DevHandle = dev_handle;
  p_hcd_ep->EP_Handle = ep_handle;
  p_hcd_ep->IsSuspended = DEF_NO;
  p_hcd_ep->EP_Params = *p_hcd_ep_params;

  *pp_hcd_ep_data = (void *)p_hcd_ep;

  switch (p_hcd_ep_params->Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      p_hcd_ep->HC_EP_Ptr = (void *)USBH_EHCI_QH_Create(p_hc_drv,
                                                        p_hcd_ep_params->DevAddr,
                                                        p_hcd_ep,
                                                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }

      if (p_hcd_ep_params->Type != USBH_EP_TYPE_INTR) {
        USBH_EHCI_HCD_EP_ListInsert(&p_hcd_data->AsyncHCD_EP_ListHeadPtr,
                                    p_hcd_ep);
      }
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
      else {
        if (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
          p_hcd_ep->EP_Params.Interval = DEF_MIN(p_hcd_ep_params->Interval,
                                                 p_hcd_data->MaxPeriodicInterval * 8u);
        } else {
          p_hcd_ep->EP_Params.Interval = DEF_MIN(p_hcd_ep_params->Interval,
                                                 p_hcd_data->MaxPeriodicInterval);
        }

        USBH_EHCI_HCD_EP_ListInsert(&p_hcd_data->PeriodicHCD_EP_ListHeadPtr,
                                    p_hcd_ep);

        USBH_EHCI_QH_PeriodicInsert(p_hc_drv,
                                    p_hcd_ep,
                                    p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          break;
        }
      }
#endif

      USBH_EHCI_HC_QH_Link(p_hcd_data,
                           p_hcd_ep,
                           p_err);
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_close;

    USBH_HCD_EP_Close(p_hc_drv,
                      (void *)p_hcd_ep,
                      &err_close);
    if (RTOS_ERR_CODE_GET(err_close) != RTOS_ERR_NONE) {
      LOG_ERR(("Closing endpoint on failed open -> ", RTOS_ERR_LOG_ARG_GET(err_close)));
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Close()
 *
 * @brief    Closes endpoint.
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data   Pointer to host controller driver endpoint data.
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               - RTOS_ERR_NONE           Operation successful.
 *                               - RTOS_ERR_NOT_SUPPORTED  Endpoint type not supported.
 *                               - RTOS_ERR_POOL_FULL      Unable to free endpoint.
 *******************************************************************************************************/
static void USBH_HCD_EP_Close(USBH_HC_DRV *p_hc_drv,
                              void        *p_hcd_ep_data,
                              RTOS_ERR    *p_err)
{
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      USBH_EHCI_HC_QH_Unlink(p_hcd_data,
                             p_hcd_ep,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      if (p_hcd_ep->EP_Params.Type != USBH_EP_TYPE_INTR) {
        USBH_EHCI_HCD_EP_ListRemove(&p_hcd_data->AsyncHCD_EP_ListHeadPtr,
                                    p_hcd_ep);
      }
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
      else {
        USBH_HCD_BinTreeBwUpdate(&p_hcd_data->BinTreeBW,
                                 p_hcd_ep->BinTreeEP_Desc,
                                 DEF_NO);

        USBH_EHCI_HCD_EP_ListRemove(&p_hcd_data->PeriodicHCD_EP_ListHeadPtr,
                                    p_hcd_ep);
      }
#endif

      USBH_EHCI_QH_Free(p_hcd_data,
                        p_hcd_ep,
                        p_err);
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }

  Mem_DynPoolBlkFree(&p_hcd_data->HCD_EP_Pool,
                     (void *)p_hcd_ep,
                     p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Suspend()
 *
 * @brief    Suspends/resumes endpoint.
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data   Pointer to host controller driver endpoint data.
 *
 * @param    suspend         Flag that indicates if endpoint should be suspended or resumed.
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_SUPPORTED  Endpoint type not supported.
 *******************************************************************************************************/
static void USBH_HCD_EP_Suspend(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN suspend,
                                RTOS_ERR    *p_err)
{
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      if (suspend == DEF_YES) {
        USBH_EHCI_HC_QH_Unlink(p_hcd_data,
                               p_hcd_ep,
                               p_err);
      } else {
        USBH_EHCI_HC_QH_Link(p_hcd_data,
                             p_hcd_ep,
                             p_err);
      }
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_hcd_ep->IsSuspended = suspend;
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_HaltClr()
 *
 * @brief    Clears halt condition on endpoint.
 *
 * @param    p_hc_drv            Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver endpoint data.
 *
 * @param    data_toggle_clr     Flag that indicates if data toggle on endpoint should be cleared.
 *                                   - DEF_YES     Clear data toggle,
 *                                   - DEF_NO      Do not clear data toggle.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *                                   - RTOS_ERR_NOT_SUPPORTED  Endpoint type not supported.
 *******************************************************************************************************/
static void USBH_HCD_EP_HaltClr(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN data_toggle_clr,
                                RTOS_ERR    *p_err)
{
  USBH_HCD_DATA     *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP  *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;
  USBH_EHCI_HC_QH   *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  USBH_EHCI_HCD_QTD *p_hcd_qtd_head = p_hc_qh->HCD_QTD_HeadPtr;

  CPU_DCACHE_RANGE_INV(p_hc_qh, EHCI_QH_LEN);

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      if ((data_toggle_clr == DEF_YES)
          && (p_hcd_ep->EP_Params.Type != USBH_EP_TYPE_CTRL)) {
        DEF_BIT_CLR(p_hc_qh->QTD_Token, EHCI_QTD_DATA_TOGGLE);
      }

      DEF_BIT_CLR(p_hc_qh->QTD_Token, EHCI_QTD_STATUS_HALTED);

      if (p_hc_qh->HCD_QTD_HeadPtr == DEF_NULL) {
        p_hc_qh->QTD_NextPtr = EHCI_QTD_T;
      } else {
        p_hc_qh->QTD_NextPtr = (CPU_INT32U)p_hcd_qtd_head->HC_qTD_HeadPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;
      }

      CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);

      if (p_hcd_ep->IsSuspended == DEF_NO) {
        USBH_EHCI_HC_QH_Link(p_hcd_data,
                             p_hcd_ep,
                             p_err);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      }
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Submit()
 *
 * @brief    Submits a URB.
 *
 * @param    p_hc_drv            Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver endpoint data.
 *
 * @param    pp_hcd_urb_data     Pointer to variable that will receive a pointer to HCD data related
 *
 * @param    p_hcd_urb_params    Pointer to structure that describes content of the URB.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *                                   - RTOS_ERR_NONE           Operation successful.
 *                                   - RTOS_ERR_SEG_OVF        Failed to allocate URB or buffer.
 *                                   - RTOS_ERR_NOT_SUPPORTED  Unsupported endpoint type.
 *                                   - RTOS_ERR_WOULD_OVF      Dedicated memory buffer too small for data.
 *******************************************************************************************************/
static void USBH_HCD_URB_Submit(USBH_HC_DRV         *p_hc_drv,
                                void                *p_hcd_ep_data,
                                void                **pp_hcd_urb_data,
                                USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                RTOS_ERR            *p_err)
{
  CPU_INT08U       *p_hw_buf;
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;

  //                                                               Check if buf should be copied to ded mem.
  if (USBH_EHCI_DED_MEM_BUF_SHOULD_COPY(p_hc_drv, p_hcd_urb_params->BufPtr, p_hcd_urb_params->BufLen) == DEF_NO) {
    p_hw_buf = p_hcd_urb_params->BufPtr;
  } else {
    //                                                             Ensure ded mem buffer large enough for URB buffer.
    if (p_hcd_urb_params->BufLen > p_hcd_data->DedicatedDataBufMaxLen) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      return;
    }

    p_hw_buf = (CPU_INT08U *)Mem_DynPoolBlkGet(&p_hcd_data->DedMemBufPool,
                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (p_hcd_urb_params->Token != USBH_TOKEN_IN) {
      Mem_Copy(p_hw_buf,
               p_hcd_urb_params->BufPtr,
               p_hcd_urb_params->BufLen);
    }
  }

  if ((p_hcd_urb_params->Token != USBH_TOKEN_IN)
      && (p_hcd_urb_params->BufLen > 0u)) {
    CPU_DCACHE_RANGE_FLUSH(p_hw_buf,
                           p_hcd_urb_params->BufLen);
  }

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      *pp_hcd_urb_data = (void *)USBH_EHCI_qTD_Submit(p_hcd_data,
                                                      p_hcd_ep,
                                                      p_hcd_urb_params,
                                                      p_hw_buf,
                                                      p_err);
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Complete()
 *
 * @brief    Completes URB transfer.
 *
 * @param    p_hc_drv            Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver endpoint data.
 *
 * @param    p_hcd_urb_data      Pointer to host controller driver URB data.
 *
 * @param    p_hcd_urb_params    Pointer to structure that describes content of the URB.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *
 * @return   Transferred length, in bytes.
 *******************************************************************************************************/
static CPU_INT32U USBH_HCD_URB_Complete(USBH_HC_DRV         *p_hc_drv,
                                        void                *p_hcd_ep_data,
                                        void                *p_hcd_urb_data,
                                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                        RTOS_ERR            *p_err)
{
  CPU_INT08U       *p_hw_buf = DEF_NULL;
  CPU_INT32U       xfer_len = 0u;
  CPU_INT32U       rem_len;
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      p_hw_buf = USBH_EHCI_qTD_Complete(p_hcd_data,
                                        p_hcd_ep,
                                        (USBH_EHCI_HCD_QTD *)p_hcd_urb_data,
                                        &rem_len,
                                        p_err);

      xfer_len = p_hcd_urb_params->BufLen - rem_len;
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL, 0u);
  }

  if ((p_hcd_urb_params->Token == USBH_TOKEN_IN)
      && (xfer_len > 0u)) {
    CPU_DCACHE_RANGE_INV((void *)p_hw_buf, xfer_len);
  }

  //                                                               Check if buf from ded mem should be copied.
  if (USBH_EHCI_DED_MEM_BUF_SHOULD_COPY(p_hc_drv, p_hcd_urb_params->BufPtr, p_hcd_urb_params->BufLen) == DEF_YES) {
    RTOS_ERR err_lib;

    if (p_hcd_urb_params->Token == USBH_TOKEN_IN) {
      Mem_Copy(p_hcd_urb_params->BufPtr,
               p_hw_buf,
               p_hcd_urb_params->BufLen);
    }

    Mem_DynPoolBlkFree(&p_hcd_data->DedMemBufPool,
                       (void *)p_hw_buf,
                       &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing dedicated memory buffer -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    }
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Abort()
 *
 * @brief    Aborts pending transfer.
 *
 * @param    p_hc_drv            Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver endpoint data.
 *
 * @param    p_hcd_urb_data      Pointer to host controller driver URB data.
 *
 * @param    p_hcd_urb_params    Pointer to structure that describes content of the URB.
 *******************************************************************************************************/
static void USBH_HCD_URB_Abort(USBH_HC_DRV         *p_hc_drv,
                               void                *p_hcd_ep_data,
                               void                *p_hcd_urb_data,
                               USBH_HCD_URB_PARAMS *p_hcd_urb_params)
{
  USBH_HCD_DATA    *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HCD_EP *p_hcd_ep = (USBH_EHCI_HCD_EP *)p_hcd_ep_data;

  (void)p_hcd_urb_params;

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_INTR:
      USBH_EHCI_qTD_Abort(p_hcd_data,
                          p_hcd_ep,
                          (USBH_EHCI_HCD_QTD *)p_hcd_urb_data);
      break;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
      && (USBH_CFG_ISOC_EN == DEF_ENABLED))
    case USBH_EP_TYPE_ISOC:
      break;
#endif

    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                           USBH_HCD_ISR_Handle()
 *
 * @brief    Host controller driver ISR handler.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *******************************************************************************************************/
static void USBH_HCD_ISR_Handle(USBH_HC_DRV *p_hc_drv)
{
  CPU_INT32U         int_status;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  int_status = p_oper_reg->USBSTS;
  p_oper_reg->USBSTS = int_status;                              // Clear the interrupt status register
  int_status &= p_oper_reg->USBINTR;

  //                                                               -------------- HOST SYSTEM ERROR INT ---------------
  if (DEF_BIT_IS_SET(int_status, EHCI_USBSTS_HSE) == DEF_YES) {
    LOG_ERR(("EHCI ISR HC halted -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
  }

  //                                                               -------------- PORT CHANGE DETECT INT --------------
  if (DEF_BIT_IS_SET(int_status, EHCI_USBSTS_PCD) == DEF_YES) {
    LOG_VRB(("EHCI ISR event on RH"));

    USBH_HUB_RootEvent(p_hc_drv->HostNbr,
                       p_hc_drv->Nbr,
                       USBH_HUB_PORT_ALL);
  }

  //                                                               ------------------ ASYNC ADVANCE -------------------
  if (DEF_BIT_IS_SET(int_status, EHCI_USBSTS_IOAA) == DEF_YES) {
    RTOS_ERR err_kal;

    DEF_BIT_CLR(p_oper_reg->USBCMD, EHCI_USBCMD_IOAAD);

    KAL_SemPost(p_hcd_data->QH_AsyncUnlinkDoneSemHandle,
                KAL_OPT_POST_NONE,
                &err_kal);
    if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
      LOG_ERR(("Posting qH free ready semaphore -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
    }
  }

  //                                                               ------------- USB INT OR USB ERROR INT -------------
  if ((DEF_BIT_IS_SET(int_status, EHCI_USBSTS_USBINT) == DEF_YES)
      || (DEF_BIT_IS_SET(int_status, EHCI_USBSTS_USBERRINT) == DEF_YES)) {
    LOG_VRB(("EHCI ISR xfer cmpl"));

    //                                                             Complete xfers from async list.
    USBH_EHCI_HCD_EP_ListXferCmpl(p_hcd_data->AsyncHCD_EP_ListHeadPtr);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    //                                                             Complete xfers from periodic list.
    USBH_EHCI_HCD_EP_ListXferCmpl(p_hcd_data->PeriodicHCD_EP_ListHeadPtr);
#endif
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
 * @brief    Retrieves information related to root hub that are normally retrieved from a HUB
 *           descriptor.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_rh_info   Pointer to root hub info structure to be filled.
 *
 * @return   DEF_OK      Operation successful,
 *           DEF_FAIL    Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_InfosGet(USBH_HC_DRV        *p_hc_drv,
                                        USBH_HUB_ROOT_INFO *p_rh_info)
{
  USBH_HCD_DATA     *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_CAP_REG *p_cap_reg = p_hcd_data->CapRegPtr;

  p_rh_info->NbrPort = p_hcd_data->RH_PortQty;
  p_rh_info->Characteristics = USBH_HUB_OVER_CUR_INDIVIDUAL;

  if (DEF_BIT_IS_SET(p_cap_reg->HCSPARAMS, EHCI_HCSPARAMS_PPC) == DEF_YES) {
    DEF_BIT_SET(p_rh_info->Characteristics,
                USBH_HUB_PWR_MODE_INDIVIDUAL);
  } else {
    DEF_BIT_SET(p_rh_info->Characteristics,
                USBH_HUB_PWR_MODE_GANGED);
  }

  p_rh_info->PwrOn2PwrGood = 20u;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_HCD_RH_PortStatusGet()
 *
 * @brief    Retrieve port status changes and port status.
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    port_nbr        Port Number
 *
 * @param    port_status     Pointer to port status structure to be filled.
 *
 * @return   DEF_OK      Operation successful,
 *           DEF_FAIL    Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortStatusGet(USBH_HC_DRV     *p_hc_drv,
                                             CPU_INT08U      port_nbr,
                                             USBH_HUB_STATUS *p_port_status)
{
  CPU_INT16U         status;
  CPU_INT16U         chng;
  CPU_INT32U         portsc;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  if (port_nbr != USBH_HUB_PORT_HUB_STATUS) {
    portsc = p_oper_reg->PORTSC[port_nbr - 1u];

    status = (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_CCS) == DEF_YES) ? USBH_HUB_STATUS_PORT_CONN     : DEF_BIT_NONE;
    status |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_PED) == DEF_YES) ? USBH_HUB_STATUS_PORT_EN       : DEF_BIT_NONE;
    status |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_OCA) == DEF_YES) ? USBH_HUB_STATUS_PORT_OVER_CUR : DEF_BIT_NONE;
    status |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_SUSP) == DEF_YES) ? USBH_HUB_STATUS_PORT_SUSPEND  : DEF_BIT_NONE;

    if (p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_STD) {
      switch (DEF_BIT_FIELD_RD(portsc, EHCI_PORTSC_LS_MSK)) {
        case EHCI_PORTSC_LS_K:
          status |= USBH_HUB_STATUS_PORT_SPD_LOW;               // Device either FS or LS. Need to release ownership.
          break;

        case EHCI_PORTSC_LS_SE0:
        case EHCI_PORTSC_LS_J:
        case EHCI_PORTSC_LS_UNDEF:
        default:
          status |= USBH_HUB_STATUS_PORT_SPD_HIGH;
          break;
      }
    } else {
      switch (DEF_BIT_FIELD_RD(portsc, EHCI_PORTSC_PSPD_MSK)) {
        case EHCI_PORTSC_PSPD_HIGH:
          status |= USBH_HUB_STATUS_PORT_SPD_HIGH;
          break;

        case EHCI_PORTSC_PSPD_LOW:
          status |= USBH_HUB_STATUS_PORT_SPD_LOW;
          break;

        case EHCI_PORTSC_PSPD_FULL:
        case EHCI_PORTSC_PSPD_UNDEF:
        default:
          status |= USBH_HUB_STATUS_PORT_SPD_FULL;
          break;
      }
    }

    chng = (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_CSC) == DEF_YES) ? USBH_HUB_STATUS_C_PORT_CONN     : DEF_BIT_NONE;
    chng |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_PEDC) == DEF_YES) ? USBH_HUB_STATUS_C_PORT_EN       : DEF_BIT_NONE;
    chng |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_OCC) == DEF_YES) ? USBH_HUB_STATUS_C_PORT_OVER_CUR : DEF_BIT_NONE;
    chng |= (DEF_BIT_IS_SET(portsc, EHCI_PORTSC_PR) == DEF_YES) ? USBH_HUB_STATUS_C_PORT_RESET    : DEF_BIT_NONE;
  } else {
    chng = DEF_BIT_NONE;
    status = DEF_BIT_NONE;
  }

  p_port_status->Chng = chng;
  p_port_status->Status = status;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_PortReq()
 *
 * @brief    Executes request on given port of root hub.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    port_nbr    Port Number.
 *
 * @param    req         Request.
 *
 * @param    feature     Feature.
 *
 * @return   DEF_OK,     If successful.
 *           DEF_FAIL,   If invalid port number.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortReq(USBH_HC_DRV *p_hc_drv,
                                       CPU_INT08U  port_nbr,
                                       CPU_INT08U  req,
                                       CPU_INT16U  feature)
{
  CPU_BOOLEAN        ok = DEF_OK;
  CPU_INT08U         line_state;
  CPU_INT08U         cnt;
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_CAP_REG  *p_cap_reg = p_hcd_data->CapRegPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;
  CPU_REG32          *p_portsc_reg = &p_oper_reg->PORTSC[port_nbr - 1u];

  line_state = DEF_BIT_FIELD_RD(*p_portsc_reg, EHCI_PORTSC_LS_MSK);

  switch (req) {
    case USBH_DEV_REQ_SET_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_PORT_EN:
        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
          break;

        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          DEF_BIT_SET(*p_portsc_reg, EHCI_PORTSC_SUSP);
          break;

        case USBH_HUB_FEATURE_SEL_PORT_RESET:
          if ((p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_STD)
              && (line_state == EHCI_PORTSC_LS_K)) {
            if (DEF_BIT_FIELD_RD(p_cap_reg->HCSPARAMS, EHCI_HCSPARAMS_N_CC_MSK) > 0u) {
              DEF_BIT_SET(*p_portsc_reg,                        // Release port ownership.
                          EHCI_PORTSC_PO);
            } else {
              LOG_DBG(("Low-speed device connected not supported by host controller."));
              ok = DEF_FAIL;
            }
          } else {
            DEF_BIT_CLR(*p_portsc_reg, EHCI_PORTSC_PED);
            DEF_BIT_SET(*p_portsc_reg, EHCI_PORTSC_PR);
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          if (DEF_BIT_IS_SET(p_cap_reg->HCSPARAMS, EHCI_HCSPARAMS_PPC) == DEF_YES) {
            DEF_BIT_SET(*p_portsc_reg, EHCI_PORTSC_PP);
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
          break;
      }
      break;

    case USBH_DEV_REQ_CLR_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_CONN:
          break;

        case USBH_HUB_FEATURE_SEL_PORT_EN:
          DEF_BIT_CLR(*p_portsc_reg, EHCI_PORTSC_PED);
          break;

        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          DEF_BIT_SET(*p_portsc_reg, EHCI_PORTSC_FPR);
          KAL_Dly(20u);
          DEF_BIT_CLR(*p_portsc_reg, EHCI_PORTSC_FPR);
          break;

        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
          break;

        case USBH_HUB_FEATURE_SEL_PORT_RESET:
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          break;

        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
          DEF_BIT_SET(*p_portsc_reg,                            // Clear Port connection status chng
                      EHCI_PORTSC_CSC);
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
          DEF_BIT_SET(*p_portsc_reg, EHCI_PORTSC_PEDC);
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
          if (DEF_BIT_IS_CLR(*p_portsc_reg, EHCI_PORTSC_CCS) == DEF_YES) {
            return (DEF_OK);
          }

          DEF_BIT_CLR(*p_portsc_reg, EHCI_PORTSC_PR);

          cnt = 0u;
          while (((*p_portsc_reg & EHCI_PORTSC_PR) != 0u)
                 && (cnt < 5u)) {
            KAL_Dly(2u);
            cnt++;
          }
          if (cnt >= 5u) {
            return (DEF_FAIL);
          }

          if ((p_hcd_data->CtrlrType == USBH_EHCI_CTRLR_TYPE_STD)
              && (DEF_BIT_IS_CLR(*p_portsc_reg, EHCI_PORTSC_PED) == DEF_YES)) {
            if (DEF_BIT_FIELD_RD(p_cap_reg->HCSPARAMS, EHCI_HCSPARAMS_N_CC_MSK) > 0u) {
              DEF_BIT_SET(*p_portsc_reg,                        // Release port ownership.
                          EHCI_PORTSC_PO);
            } else {
              LOG_DBG(("Full-speed device connected not supported by host controller."));
              ok = DEF_FAIL;
            }
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
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
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    en          Flag that indicates if root hub interrupt should be enabled or disabled.
 *                       DEF_ENABLED     Enable  root hub interrupt.
 *                       DEF_DISABLED    Disable root hub interrupt.
 *
 * @return   DEF_OK,     If successful.
 *           DEF_FAIL,   If invalid port number.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_IntEn(USBH_HC_DRV *p_hc_drv,
                                     CPU_BOOLEAN en)
{
  USBH_HCD_DATA      *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  if (en == DEF_ENABLED) {
    DEF_BIT_SET(p_oper_reg->USBINTR,
                EHCI_USBINTR_PCIE);
  } else {
    DEF_BIT_CLR(p_oper_reg->USBINTR,
                EHCI_USBINTR_PCIE);
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HCD_InitHandle()
 *
 * @brief    Initializes Host Controller Driver
 *
 * @param    p_hc_drv        Pointer to host controller driver structure.
 *
 * @param    p_mem_seg       Pointer to memory segment from where driver's internal control data
 *
 * @param    p_mem_seg_buf   Pointer to memory segment from where data buffers are allocated.
 *
 * @param    ctrlr_type      Controller type.
 *                           USBH_EHCI_CTRLR_TYPE_STD
 *                           USBH_EHCI_CTRLR_TYPE_SYNOPSYS
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               - RTOS_ERR_NONE       Operation successful.
 *                               - RTOS_ERR_NOT_AVAIL  Missing needed kernel feature.
 *                               - RTOS_ERR_ISR        Failed to create required kernel signals.
 *                                                     Cannot create in ISR.
 *                               - RTOS_ERR_POOL_EMPTY Failed to create required kernel signals.
 *                                                     No more blocks available in pool.
 *                               - RTOS_ERR_SEG_OVF    Failed to create required dynamic memory pools.
 *                               - RTOS_ERR_IO         Call to BSP failed.
 *******************************************************************************************************/
static void USBH_HCD_InitHandle(USBH_HC_DRV          *p_hc_drv,
                                MEM_SEG              *p_mem_seg,
                                MEM_SEG              *p_mem_seg_buf,
                                USBH_HC_CFG_EXT      *p_hc_cfg_ext,
                                USBH_EHCI_CTRLR_TYPE ctrlr_type,
                                RTOS_ERR             *p_err)
{
  CPU_BOOLEAN     ok;
  CPU_SIZE_T      qh_nbr_max;
  CPU_SIZE_T      qtd_nbr_max;
  CPU_SIZE_T      qh_nbr_init;
  CPU_SIZE_T      qtd_nbr_init;
  CPU_SIZE_T      qh_align;
  CPU_SIZE_T      qtd_align;
  USBH_HCD_DATA   *p_hcd_data;
  USBH_HC_BSP_API *p_bsp_api = p_hc_drv->BSP_API_Ptr;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgOptimizeSpdPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgInitAllocPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );
#endif

  //                                                               Validate required kernel feature avail.
  ok = KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_BLOCKING);
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  ok &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_POST_NONE);
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_DEL_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_DEL_NONE);
#endif
  if (ok != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescQty), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescIsocQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  qh_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty  + 1u;
  qh_nbr_init = qh_nbr_max;
  qtd_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty + p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty;
  qtd_nbr_init = qtd_nbr_max;
#else
  qh_nbr_max = LIB_MEM_BLK_QTY_UNLIMITED;
  qh_nbr_init = 0u;
  qtd_nbr_max = LIB_MEM_BLK_QTY_UNLIMITED;
  qtd_nbr_init = 0u;
#endif

  LOG_VRB(("Initializing EHCI driver"));

  p_hcd_data = (USBH_HCD_DATA *)Mem_SegAlloc("EHCI - Driver Data",
                                             p_mem_seg,
                                             sizeof(USBH_HCD_DATA),
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_hc_drv->DataPtr = DEF_NULL;

    return;
  }

  p_hc_drv->DataPtr = (void *)p_hcd_data;

  Mem_Clr(p_hcd_data,
          sizeof(USBH_HCD_DATA));

  p_hcd_data->QH_AsyncUnlinkDoneSemHandle = KAL_SemHandleNull;
  p_hcd_data->QH_AsyncUnlinkDoneLockHandle = KAL_LockHandleNull;
  p_hcd_data->DataMemSegPtr = p_mem_seg;
  p_hcd_data->CtrlrType = ctrlr_type;

  if (p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL) {
    p_hcd_data->DedicatedDataBufMaxLen = p_hc_cfg_ext->DedicatedMemCfgPtr->DataBufLen;
  } else {
    p_hcd_data->DedicatedDataBufMaxLen = 0u;
  }

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_hcd_data->FrameList = DEF_NULL;

  if (p_hc_cfg_ext != DEF_NULL) {
    p_hcd_data->MaxPeriodicInterval = p_hc_cfg_ext->MaxPeriodicInterval;
  } else {
    p_hcd_data->MaxPeriodicInterval = USBH_PERIODIC_XFER_MAX_INTERVAL_DFLT;
  }
#endif

  if ((p_bsp_api != DEF_NULL)
      && (p_bsp_api->Init != DEF_NULL)) {
    ok = p_bsp_api->Init(USBH_HCD_ISR_Handle, p_hc_drv);
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

      LOG_ERR(("In EHCI BSP Init() -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      return;
    }
  }

  if (p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr != DEF_NULL) { // ----------------- DEDICATED MEM SEG ----------------
    USBH_HC_DEDICATED_MEM_INFO *p_dedicated_mem_info = p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr;

    p_hcd_data->HC_DescMemSegPtr = p_dedicated_mem_info->MemSegPtr;
    qh_align = DEF_MAX(EHCI_QH_ALIGN, p_dedicated_mem_info->BufAlignOctets);
    qtd_align = DEF_MAX(EHCI_QTD_ALIGN, p_dedicated_mem_info->BufAlignOctets);
  } else {
    p_hcd_data->HC_DescMemSegPtr = p_mem_seg_buf;

    qh_align = EHCI_QH_ALIGN;
    qtd_align = EHCI_QTD_ALIGN;
  }

  if (p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL) { // ----------------- DEDICATED MEM SEG ----------------
    MEM_SEG_INFO               seg_info;
    USBH_HC_CFG_DEDICATED_MEM  *p_dedicated_mem_cfg = p_hc_cfg_ext->DedicatedMemCfgPtr;
    USBH_HC_DEDICATED_MEM_INFO *p_dedicated_mem_info = p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr;

    Mem_DynPoolCreateHW("USBH - EHCI Dedicated memory buffer pool",
                        &p_hcd_data->DedMemBufPool,
                        p_dedicated_mem_info->MemSegPtr,
                        p_dedicated_mem_cfg->DataBufLen,
                        p_dedicated_mem_info->BufAlignOctets,
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                        p_dedicated_mem_cfg->DataBufQty,
#else
                        0u,
#endif
                        p_dedicated_mem_cfg->DataBufQty,
                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    (void)Mem_SegRemSizeGet(p_dedicated_mem_info->MemSegPtr,
                            1u,
                            &seg_info,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_hcd_data->DedMemAddrStart = seg_info.AddrBase;
    p_hcd_data->DedMemAddrEnd = seg_info.AddrBase + seg_info.TotalSize;
  }

  Mem_DynPoolCreateHW("USBH - EHCI HC QH Pool",                 // Create pool of HC QHs.
                      &p_hcd_data->HC_QH_Pool,
                      p_hcd_data->HC_DescMemSegPtr,
                      sizeof(USBH_EHCI_HC_QH),
                      qh_align,
                      qh_nbr_init,
                      qh_nbr_max,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }

  Mem_DynPoolCreateHW("USBH - EHCI HC qTD Pool",                // Create pool of HC qTDs.
                      &p_hcd_data->HC_qTD_Pool,
                      p_hcd_data->HC_DescMemSegPtr,
                      sizeof(USBH_EHCI_HC_QTD),
                      qtd_align,
                      qtd_nbr_init,
                      qtd_nbr_max,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }

  Mem_DynPoolCreate("USBH - EHCI HCD QH Pool",                  // Create pool of HCD EDs.
                    &p_hcd_data->HCD_EP_Pool,
                    p_mem_seg,                                  // HCD structures allocated from main mem seg.
                    sizeof(USBH_EHCI_HCD_EP),
                    sizeof(CPU_ALIGN),
                    qh_nbr_init,
                    qh_nbr_max,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }

  Mem_DynPoolCreate("USBH - EHCI HCD qTD Pool",                 // Create pool of HCD EDs.
                    &p_hcd_data->HCD_qTD_Pool,
                    p_mem_seg,                                  // HCD structures allocated from main mem seg.
                    sizeof(USBH_EHCI_HCD_QTD),
                    sizeof(CPU_ALIGN),
                    qtd_nbr_init,
                    qtd_nbr_max,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }

  p_hcd_data->QH_AsyncUnlinkDoneSemHandle = KAL_SemCreate("USBH - EHCI qH free ready semaphore",
                                                          DEF_NULL,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hcd_data->QH_AsyncUnlinkDoneLockHandle = KAL_LockCreate("USBH - EHCI qH free ready lock",
                                                            DEF_NULL,
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  LOG_VRB(("EHCI driver initialized"));
}

/****************************************************************************************************//**
 *                                       USBH_EHCI_HCD_EP_ListInsert()
 *
 * @brief    Inserts a HCD endpoint at head of given list.
 *
 * @param    pp_hcd_ep_head  Pointer to endpoint list's head pointer.
 *
 * @param    p_hcd_ep        Pointer to HCD endpoint to insert.
 *******************************************************************************************************/
static void USBH_EHCI_HCD_EP_ListInsert(USBH_EHCI_HCD_EP **pp_hcd_ep_head,
                                        USBH_EHCI_HCD_EP *p_hcd_ep)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_hcd_ep->NextPtr = *pp_hcd_ep_head;
  *pp_hcd_ep_head = p_hcd_ep;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       USBH_EHCI_HCD_EP_ListRemove()
 *
 * @brief    Removes given HCD endpoint from list.
 *
 * @param    pp_hcd_ep_head  Pointer to endpoint list's head pointer.
 *
 * @param    p_hcd_ep        Pointer to HCD endpoint to remove.
 *******************************************************************************************************/
static void USBH_EHCI_HCD_EP_ListRemove(USBH_EHCI_HCD_EP **pp_hcd_ep_head,
                                        USBH_EHCI_HCD_EP *p_hcd_ep)
{
  USBH_EHCI_HCD_EP *p_hcd_qh_prev;
  USBH_EHCI_HCD_EP *p_hcd_qh_cur;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_hcd_qh_cur = *pp_hcd_ep_head;
  p_hcd_qh_prev = DEF_NULL;

  while (p_hcd_qh_cur != p_hcd_ep) {
    p_hcd_qh_prev = p_hcd_qh_cur;
    p_hcd_qh_cur = p_hcd_qh_cur->NextPtr;
  }

  if (p_hcd_qh_prev == DEF_NULL) {
    *pp_hcd_ep_head = p_hcd_ep->NextPtr;
  } else {
    p_hcd_qh_prev->NextPtr = p_hcd_ep->NextPtr;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       USBH_EHCI_HCD_EP_ListXferCmpl()
 *
 * @brief    Complete all transfers done by the controller in given list.
 *
 * @param    p_hcd_ep_head   Pointer to head element of list.
 *******************************************************************************************************/
static void USBH_EHCI_HCD_EP_ListXferCmpl(USBH_EHCI_HCD_EP *p_hcd_ep_head)
{
  USBH_EHCI_HCD_EP *p_hcd_ep_cur = p_hcd_ep_head;

  //                                                               Retrieve the qTD(s) that have completed.
  while (p_hcd_ep_cur != DEF_NULL) {
    CPU_BOOLEAN       done = DEF_NO;
    USBH_EHCI_HC_QH   *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep_cur->HC_EP_Ptr;
    USBH_EHCI_HCD_QTD *p_hcd_qtd_cur;
    USBH_EHCI_HC_QTD  *p_hc_qtd_cur;

    CPU_DCACHE_RANGE_INV(p_hc_qh, EHCI_QH_LEN);

    p_hcd_qtd_cur = p_hc_qh->HCD_QTD_HeadPtr;

    if (p_hcd_qtd_cur == DEF_NULL) {
      p_hcd_ep_cur = p_hcd_ep_cur->NextPtr;

      continue;
    }

    p_hc_qtd_cur = p_hcd_qtd_cur->HC_qTD_HeadPtr;
    while ((done != DEF_YES)
           && (p_hcd_qtd_cur != DEF_NULL)) {
      CPU_DCACHE_RANGE_INV(p_hc_qtd_cur, EHCI_QTD_LEN);

      if (DEF_BIT_IS_CLR(p_hc_qtd_cur->Token, EHCI_QTD_STATUS_ACTIVE) == DEF_YES) {
        if ((p_hc_qtd_cur == p_hcd_qtd_cur->HC_qTD_TailPtr)
            || (DEF_BIT_FIELD_RD(p_hc_qtd_cur->Token, EHCI_QTD_TOTAL_BYTES_TO_XFER_MSK) > 0u)) {
          USBH_URB_Done(p_hcd_ep_cur->DevHandle,
                        p_hcd_ep_cur->EP_Handle);

          //                                                       Remove qTD from qH's list.
          if (p_hc_qh->HCD_QTD_TailPtr == p_hcd_qtd_cur) {
            p_hc_qh->HCD_QTD_TailPtr = DEF_NULL;
          }

          p_hc_qh->HCD_QTD_HeadPtr = p_hcd_qtd_cur->NextPtr;

          p_hcd_qtd_cur = p_hcd_qtd_cur->NextPtr;

          if (p_hcd_qtd_cur != DEF_NULL) {
            p_hc_qtd_cur = p_hcd_qtd_cur->HC_qTD_HeadPtr;
          }

          CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);
        } else {
          p_hc_qtd_cur = (USBH_EHCI_HC_QTD *)(p_hc_qtd_cur->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);
        }
      } else {
        done = DEF_YES;                                         // No other qTDs completed on this qH.
      }
    }

    p_hcd_ep_cur = p_hcd_ep_cur->NextPtr;
  }
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_QH_Create()
 *
 * @brief    Creates queue head (qH) structure for the given endpoint
 *
 * @param    p_hc_drv        Pointer to Host Controller Driver structure.
 *
 * @param    dev_addr        Endpoint's device address.
 *
 * @param    p_hcd_ep_data   Pointer to host controller driver endpoint data.
 *
 * @param    p_err           Pointer to variable that will receive the error code from this function.
 *                               - RTOS_ERR_NONE               Operation successful.
 *                               - RTOS_ERR_DRV_EP_ALLOC   Failed to create required dynamic memory pools.
 *
 * @return   Pointer to created qH structure.
 *******************************************************************************************************/
static USBH_EHCI_HC_QH *USBH_EHCI_QH_Create(USBH_HC_DRV      *p_hc_drv,
                                            CPU_INT08U       dev_addr,
                                            USBH_EHCI_HCD_EP *p_hcd_ep,
                                            RTOS_ERR         *p_err)
{
  CPU_INT08U      buf_page_cnt;
  USBH_EHCI_HC_QH *p_hc_qh = DEF_NULL;
  USBH_HCD_DATA   *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;

  //                                                               Allocate memory for a queue head
  p_hc_qh = (USBH_EHCI_HC_QH *)Mem_DynPoolBlkGet(&p_hcd_data->HC_QH_Pool,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);

    LOG_ERR(("EHCI driver cannot allocate HC QH -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto err_free;
  }

  p_hc_qh->HCD_QTD_HeadPtr = DEF_NULL;
  p_hc_qh->HCD_QTD_TailPtr = DEF_NULL;
  p_hc_qh->HCD_EP_Ptr = p_hcd_ep;

  //                                                               Configure EP_Characteristics.
  p_hc_qh->EP_Characteristics = DEF_BIT_NONE;

  if (p_hcd_ep->EP_Params.Type == USBH_EP_TYPE_CTRL) {
    DEF_BIT_SET(p_hc_qh->EP_Characteristics, EHCI_QH_DTC);

    if (p_hcd_ep->EP_Params.DevSpd != USBH_DEV_SPD_HIGH) {
      DEF_BIT_SET(p_hc_qh->EP_Characteristics, EHCI_QH_C);
    }
  }

  DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                   p_hcd_ep->EP_Params.MaxPktSize,
                   EHCI_QH_MPS_MSK);

  switch (p_hcd_ep->EP_Params.DevSpd) {
    case USBH_DEV_SPD_LOW:
      DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                       EHCI_QH_EPS_LOW,
                       EHCI_QH_EPS_MSK);
      break;

    case USBH_DEV_SPD_FULL:
      DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                       EHCI_QH_EPS_FULL,
                       EHCI_QH_EPS_MSK);
      break;

    case USBH_DEV_SPD_HIGH:
    default:
      DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                       EHCI_QH_EPS_HIGH,
                       EHCI_QH_EPS_MSK);
      break;
  }

  DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                   USBH_EP_ADDR_TO_LOG(p_hcd_ep->EP_Params.Addr),
                   EHCI_QH_ENDPT_MSK);

  DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                   dev_addr,
                   EHCI_QH_DEV_ADDR_MSK);

  //                                                               Configure EP_Capabilities.
  p_hc_qh->EP_Capabilities = DEF_BIT_NONE;

  DEF_BIT_FIELD_WR(p_hc_qh->EP_Capabilities,
                   USBH_EP_NBR_TRANSACTION_GET(p_hcd_ep->EP_Params.MaxPktSize) + 1u,
                   EHCI_QH_MULT_MSK);

  if (p_hcd_ep->EP_Params.DevSpd != USBH_DEV_SPD_HIGH) {
    CPU_INT08U nearest_hs_hub_addr;
    CPU_INT08U nearest_hs_hub_port_nbr;

    if (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
      USBH_DEV_HANDLE nearest_hs_hub_handle;

      nearest_hs_hub_handle = USBH_DevHS_HubNearestHandleGet(p_hcd_ep->DevHandle,
                                                             &nearest_hs_hub_port_nbr,
                                                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto err_free;
      }

      if (USBH_DEV_HANDLE_IS_RH(nearest_hs_hub_handle) == DEF_NO) {
        nearest_hs_hub_addr = USBH_DevAddrGet(nearest_hs_hub_handle,
                                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto err_free;
        }
      } else {
        nearest_hs_hub_addr = 0u;                               // Synopsys only.
      }
    } else {
      nearest_hs_hub_port_nbr = 1u;
      nearest_hs_hub_addr = 0u;                                 // Synopsys only.
    }

    DEF_BIT_FIELD_WR(p_hc_qh->EP_Capabilities,
                     nearest_hs_hub_port_nbr,
                     EHCI_QH_PORT_NBR_MSK);

    DEF_BIT_FIELD_WR(p_hc_qh->EP_Capabilities,
                     nearest_hs_hub_addr,
                     EHCI_QH_HUB_ADDR_MSK);
  }

  p_hc_qh->HorLinkPtr = DEF_NULL;

  //                                                               Clr the qH overlay area.
  p_hc_qh->QTD_CurPtr = DEF_BIT_NONE;
  p_hc_qh->QTD_NextPtr = EHCI_QTD_T;
  p_hc_qh->QTD_AltNextPtr = EHCI_QTD_T;
  p_hc_qh->QTD_Token = DEF_BIT_NONE;

  for (buf_page_cnt = 0u; buf_page_cnt < EHCI_QTD_PAGE_QTY; buf_page_cnt++) {
    p_hc_qh->QTD_BufPagePtrList[buf_page_cnt] = DEF_NULL;

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)
    p_hc_qh->QTD_ExtBufPagePtrList[buf_page_cnt] = DEF_NULL;
#endif
  }

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_BULK:
    case USBH_EP_TYPE_CTRL:
      LOG_VRB(("EHCI async EP open nbr ", (u)USBH_EP_ADDR_TO_PHY(p_hcd_ep->EP_Params.Addr), " on device ", (u)dev_addr));

      //                                                           Init NAK reload cnt.
      DEF_BIT_FIELD_WR(p_hc_qh->EP_Characteristics,
                       15u,
                       EHCI_QH_RL_MSK);
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    case USBH_EP_TYPE_INTR:
      LOG_VRB(("EHCI intr EP open nbr ", (u)USBH_EP_ADDR_TO_PHY(p_hcd_ep->EP_Params.Addr), " on device ", (u)dev_addr));
      break;
#endif

    default:
      break;
  }

  CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);

  return (p_hc_qh);

err_free:
  {
    RTOS_ERR err_lib;

    if (p_hc_qh != DEF_NULL) {
      Mem_DynPoolBlkFree(&p_hcd_data->HC_QH_Pool,
                         (void *)p_hc_qh,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing HC QH -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }
    }
  }

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_QH_Free()
 *
 * @brief    Close the endpoint by unlinking the EHCI queue head
 *
 * @param    p_hcd_data  Pointer to Host Controller Driver data.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE       Operation successful.
 *                           - RTOS_ERR_POOL_FULL  Failed to free qH.
 *******************************************************************************************************/
static void USBH_EHCI_QH_Free(USBH_HCD_DATA    *p_hcd_data,
                              USBH_EHCI_HCD_EP *p_hcd_ep,
                              RTOS_ERR         *p_err)
{
  Mem_DynPoolBlkFree(&p_hcd_data->HC_QH_Pool,
                     p_hcd_ep->HC_EP_Ptr,
                     p_err);
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_HC_QH_Link()
 *
 * @brief    Link qH to proper Host Controller list (async or periodic).
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *                           - RTOS_ERR_FAIL   Failed to start processing of Host Controller list.
 *******************************************************************************************************/
static void USBH_EHCI_HC_QH_Link(USBH_HCD_DATA    *p_hcd_data,
                                 USBH_EHCI_HCD_EP *p_hcd_ep,
                                 RTOS_ERR         *p_err)
{
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;
  USBH_EHCI_HC_QH    *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  CORE_DECLARE_IRQ_STATE;

  if (p_hc_qh->HorLinkPtr != DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // qH is already linked.
    return;
  }

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
      CORE_ENTER_ATOMIC();
      if (DEF_BIT_IS_SET(p_oper_reg->USBSTS, EHCI_USBSTS_ASS) == DEF_YES) {
        USBH_EHCI_HC_QH *p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_oper_reg->ASYNCLISTADDR & EHCI_ASYNCLISTADDR_LPL);

        p_hc_qh->HorLinkPtr = p_hc_qh_cur->HorLinkPtr;
        CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);

        CPU_DCACHE_RANGE_INV(p_hc_qh_cur, EHCI_QH_LEN);
        p_hc_qh_cur->HorLinkPtr = (CPU_INT32U)p_hc_qh & EHCI_QH_HOR_LINK_PTR_MSK;
        CPU_DCACHE_RANGE_FLUSH(p_hc_qh_cur, EHCI_QH_LEN);
      } else {
        CPU_INT16U retry;
        CPU_INT32U hor_link_ptr;

        //                                                         Make the list wrap on itself.
        hor_link_ptr = (CPU_INT32U)p_hc_qh & EHCI_QH_HOR_LINK_PTR_MSK;
        DEF_BIT_FIELD_WR(hor_link_ptr,
                         EHCI_QH_TYPE_SEL_QH,
                         EHCI_QH_TYPE_SEL_MSK);

        p_hc_qh->HorLinkPtr = hor_link_ptr;

        DEF_BIT_SET(p_hc_qh->EP_Characteristics, EHCI_QH_H);

        CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);

        CPU_MB();

        //                                                         Set new HC qH as current async qH.
        p_oper_reg->ASYNCLISTADDR = (CPU_INT32U)p_hc_qh & EHCI_ASYNCLISTADDR_LPL;

        retry = DEF_INT_16U_MAX_VAL;                            // Wait till async list processing is enabled
        while ((DEF_BIT_IS_CLR(p_oper_reg->USBSTS, EHCI_USBSTS_ASS) == DEF_YES)
               && (retry > 0u)) {
          retry--;

          DEF_BIT_SET(p_oper_reg->USBCMD,                       // Enable async list processing
                      EHCI_USBCMD_ASE);
        }
        if (retry == 0u) {
          CORE_EXIT_ATOMIC();

          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
          break;
        }
      }
      CORE_EXIT_ATOMIC();

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    case USBH_EP_TYPE_INTR:
    {
      CPU_INT16U interval_branch_qty = DEF_BIT(p_hcd_ep->FrmListLevel);
      CPU_INT16U interval_branch_start_ix = p_hcd_ep->FrmListIx * interval_branch_qty;
      CPU_INT16U interval_branch_cnt;

      for (interval_branch_cnt = interval_branch_start_ix; interval_branch_cnt < (interval_branch_start_ix + interval_branch_qty); interval_branch_cnt++) {
        CPU_INT16U      btree_ix;
        USBH_EHCI_HC_QH *p_hc_qh_cur;
        USBH_EHCI_HC_QH *p_hc_qh_prev = DEF_NULL;

        btree_ix = USBH_HCD_BinTreeIxToggle(p_hcd_data->MaxPeriodicInterval,
                                            interval_branch_cnt);

        p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hcd_data->FrameList[btree_ix] & EHCI_FRM_LIST_LINK_PTR_MSK);
        while ((p_hc_qh_cur != p_hcd_data->PeriodicDummyQH_Ptr)
               && (p_hc_qh_cur->HCD_EP_Ptr->FrmListLevel < p_hcd_ep->FrmListLevel)) {
          p_hc_qh_prev = p_hc_qh_cur;
          p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK);
        }

        CORE_ENTER_ATOMIC();

        if (p_hc_qh->HorLinkPtr == DEF_NULL) {
          CPU_INT32U hor_link_ptr = (CPU_INT32U)p_hc_qh_cur & EHCI_QH_HOR_LINK_PTR_MSK;

          DEF_BIT_FIELD_WR(hor_link_ptr,
                           EHCI_QH_TYPE_SEL_QH,
                           EHCI_QH_TYPE_SEL_MSK);

          p_hc_qh->HorLinkPtr = hor_link_ptr;

          CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);
        }

        CPU_MB();

        if (p_hc_qh_prev == DEF_NULL) {
          CPU_INT32U frm_list_entry = (CPU_INT32U)p_hc_qh & EHCI_FRM_LIST_LINK_PTR_MSK;

          DEF_BIT_FIELD_WR(frm_list_entry,
                           EHCI_FRM_LIST_TYPE_QH,
                           EHCI_FRM_LIST_TYPE_MSK);

          p_hcd_data->FrameList[btree_ix] = frm_list_entry;

          CPU_DCACHE_RANGE_FLUSH(&p_hcd_data->FrameList[btree_ix], 4u);
        } else {
          CPU_INT32U hor_link_ptr = (CPU_INT32U)p_hc_qh & EHCI_QH_HOR_LINK_PTR_MSK;

          DEF_BIT_FIELD_WR(hor_link_ptr,
                           EHCI_QH_TYPE_SEL_QH,
                           EHCI_QH_TYPE_SEL_MSK);

          p_hc_qh_prev->HorLinkPtr = hor_link_ptr;

          CPU_DCACHE_RANGE_FLUSH(p_hc_qh_prev, EHCI_QH_LEN);
        }
        CORE_EXIT_ATOMIC();
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }
    break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_HC_QH_Unlink()
 *
 * @brief    Unlink qH from Host Controller list (async or periodic).
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *                           - RTOS_ERR_FAIL   Failed to stop processing of Host Controller list.
 *                           -RETURNED BY USBH_EHCI_AsyncQH_IOAAD_IntPend()-
 *                       See USBH_EHCI_AsyncQH_IOAAD_IntPend() for additional return error codes.
 *******************************************************************************************************/
static void USBH_EHCI_HC_QH_Unlink(USBH_HCD_DATA    *p_hcd_data,
                                   USBH_EHCI_HCD_EP *p_hcd_ep,
                                   RTOS_ERR         *p_err)
{
  USBH_EHCI_HC_QH    *p_hc_qh_cur;
  USBH_EHCI_HC_QH    *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;
  CORE_DECLARE_IRQ_STATE;

  switch (p_hcd_ep->EP_Params.Type) {
    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_BULK:
      CORE_ENTER_ATOMIC();
      if (p_hc_qh->HorLinkPtr == DEF_NULL) {
        CORE_EXIT_ATOMIC();                                     // qH is not linked.

        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;
      }

      p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_oper_reg->ASYNCLISTADDR & EHCI_ASYNCLISTADDR_LPL);

      if ((CPU_INT32U)p_hc_qh_cur == (p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK)) {
        CPU_INT16U retry = DEF_INT_16U_MAX_VAL;

        //                                                         Wait till async list processing is disabled
        while ((DEF_BIT_IS_SET(p_oper_reg->USBSTS, EHCI_USBSTS_ASS) == DEF_YES)
               && (retry > 0u)) {
          retry--;

          DEF_BIT_CLR(p_oper_reg->USBCMD,                       // Disable async list processing
                      EHCI_USBCMD_ASE);
        }
        if (retry == 0u) {
          CORE_EXIT_ATOMIC();

          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
          break;
        }

        p_oper_reg->ASYNCLISTADDR = DEF_NULL;
        CORE_EXIT_ATOMIC();
      } else {
        USBH_EHCI_HC_QH *p_hc_qh_prev = p_hc_qh_cur;

        //                                                         Remove HCD qH from list.
        p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK);

        while (p_hc_qh_cur != p_hc_qh) {
          p_hc_qh_prev = p_hc_qh_cur;
          p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK);
        }

        p_hc_qh_prev->HorLinkPtr = p_hc_qh->HorLinkPtr;

        //                                                         If qH to remove was head or reclam list, replace it.
        if (DEF_BIT_IS_SET(p_hc_qh->EP_Characteristics, EHCI_QH_H) == DEF_YES) {
          DEF_BIT_SET(p_hc_qh_prev->EP_Characteristics, EHCI_QH_H);
        }

        CPU_DCACHE_RANGE_FLUSH(p_hc_qh_prev, EHCI_QH_LEN);
        CORE_EXIT_ATOMIC();

        USBH_EHCI_AsyncQH_IOAAD_IntPend(p_hcd_data,             // Wait for async advance doorbell.
                                        p_err);
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    case USBH_EP_TYPE_INTR:
    {
      CPU_BOOLEAN per_dis = DEF_NO;
      CPU_INT16U  interval_branch_qty = DEF_BIT(p_hcd_ep->FrmListLevel);
      CPU_INT16U  interval_branch_start_ix = p_hcd_ep->FrmListIx * interval_branch_qty;
      CPU_INT16U  interval_branch_cnt;

      for (interval_branch_cnt = interval_branch_start_ix; interval_branch_cnt < (interval_branch_start_ix + interval_branch_qty); interval_branch_cnt++) {
        CPU_INT16U      btree_ix;
        USBH_EHCI_HC_QH *p_hc_qh_prev = DEF_NULL;

        btree_ix = USBH_HCD_BinTreeIxToggle(p_hcd_data->MaxPeriodicInterval,
                                            interval_branch_cnt);

        p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hcd_data->FrameList[btree_ix] & EHCI_FRM_LIST_LINK_PTR_MSK);
        while ((p_hc_qh_cur != p_hcd_data->PeriodicDummyQH_Ptr)
               && (p_hc_qh_cur != p_hc_qh)) {
          p_hc_qh_prev = p_hc_qh_cur;
          p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK);
        }

        if (p_hc_qh_cur == p_hc_qh) {
          CORE_ENTER_ATOMIC();
          if (p_hc_qh_prev == DEF_NULL) {
            if (per_dis == DEF_NO) {                            // Dis periodic list when need to write frm list entry.
              CPU_INT16U retry = DEF_INT_16U_MAX_VAL;

              per_dis = DEF_YES;

              DEF_BIT_CLR(p_oper_reg->USBCMD, EHCI_USBCMD_PSE);
              while ((DEF_BIT_IS_SET(p_oper_reg->USBSTS, EHCI_USBSTS_PSS) == DEF_YES)
                     && (retry > 0u)) {
                retry--;
              }
              if (retry == 0u) {
                CORE_EXIT_ATOMIC();

                RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
                return;
              }
            }

            p_hcd_data->FrameList[btree_ix] = p_hc_qh->HorLinkPtr;

            CPU_DCACHE_RANGE_FLUSH(&p_hcd_data->FrameList[btree_ix], 4u);
          } else {
            p_hc_qh_prev->HorLinkPtr = p_hc_qh->HorLinkPtr;

            CPU_DCACHE_RANGE_FLUSH(p_hc_qh_prev, EHCI_QH_LEN);
          }
          CORE_EXIT_ATOMIC();
        }
      }

      if (per_dis == DEF_YES) {                                 // Re-enable periodic list.
        CPU_INT16U retry = DEF_INT_16U_MAX_VAL;

        DEF_BIT_SET(p_oper_reg->USBCMD, EHCI_USBCMD_PSE);

        while ((DEF_BIT_IS_CLR(p_oper_reg->USBSTS, EHCI_USBSTS_PSS) == DEF_YES)
               && (retry > 0u)) {
          retry--;
        }
        if (retry == 0u) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
          break;
        }
      } else {
        KAL_Dly(2u);                                            // Wait at least one frm so EHCI loses all ref to qH.
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }
    break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_hc_qh->HorLinkPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                       USBH_EHCI_QH_PeriodicInsert()
 *
 * @brief    Insert qH in periodic Host Controller Driver's binary tree.
 *
 * @param    p_hc_drv    Pointer to host controller driver structure.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *                           -RETURNED BY USBH_HCD_BinTreeBwHighestPosFind()-
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_EHCI_QH_PeriodicInsert(USBH_HC_DRV      *p_hc_drv,
                                        USBH_EHCI_HCD_EP *p_hcd_ep,
                                        RTOS_ERR         *p_err)
{
  CPU_INT08U                s_mask;
  CPU_INT08U                c_mask;
  CPU_INT32U                ep_cap;
  USBH_HCD_BIN_TREE_EP_DESC bin_tree_ep_desc;
  USBH_HCD_DATA             *p_hcd_data = (USBH_HCD_DATA *)p_hc_drv->DataPtr;
  USBH_EHCI_HC_QH           *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;

  bin_tree_ep_desc = USBH_HCD_BinTreeBwHighestPosFind(&p_hcd_data->BinTreeBW,
                                                      &p_hcd_ep->EP_Params,
                                                      DEF_YES,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_HCD_BinTreeBwUpdate(&p_hcd_data->BinTreeBW,
                           bin_tree_ep_desc,
                           DEF_YES);

  //                                                               Determine smask and frm list position.
  if (p_hc_drv->HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
    USBH_EHCI_QH_BinTreePosToFrmList(bin_tree_ep_desc,
                                     p_hcd_data->MaxPeriodicInterval * 8u,
                                     &p_hcd_ep->FrmListLevel,
                                     &p_hcd_ep->FrmListIx,
                                     &s_mask);

    if (DEF_BIT_IS_SET(bin_tree_ep_desc.MainTimeSlotNeededBW, USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_OTHER_UFRM) == DEF_NO) {
      c_mask = DEF_BIT_NONE;                                    // Not a split transaction.
    } else {
      c_mask = s_mask << USBH_HCD_LIB_SPLIT_PHASE_GAP;          // Split transaction.
    }
  } else {
    s_mask = 0x01u;
    c_mask = 0x04u;
    p_hcd_ep->FrmListLevel = bin_tree_ep_desc.Level;
    p_hcd_ep->FrmListIx = bin_tree_ep_desc.IntervalLinearIx;
  }

  p_hcd_ep->BinTreeEP_Desc = bin_tree_ep_desc;

  ep_cap = p_hc_qh->EP_Capabilities;

  DEF_BIT_FIELD_WR(ep_cap,
                   s_mask,
                   EHCI_QH_INT_SCHED_MSK);

  DEF_BIT_FIELD_WR(ep_cap,
                   c_mask,
                   EHCI_QH_SPLIT_CMPL_MSK);

  p_hc_qh->EP_Capabilities = ep_cap;

  CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);
}
#endif

/****************************************************************************************************//**
 *                                   USBH_EHCI_QH_BinTreePosToFrmList()
 *
 * @brief    Converts a binary tree position returned from hcd_lib into a frame list poisition with
 *           a mask for the uFrame.
 *
 * @param    pos_desc            Endpoint binary tree position descriptor.
 *
 * @param    interval_max        Maximum interval of binary tree (in (u)Frame).
 *
 * @param    p_frm_list_level    Pointer to variable that will receive the frame list level.
 *
 * @param    p_frm_list_ix       Pointer to variable that will receive the frame list index (phase).
 *
 * @param    p_s_mask            Pointer to variable that will receive the value for the s_mask.
 *
 * @note     (1) HCD lib uses the uFrame (if controller in high-speed) as its unit for the branches in
 *               its bandwidth usage table. However, EHCI uses a frame unit for its binary tree and
 *               uses a mask to determine in which uFrame to execute the transfer. Hence, it is
 *               necessary to convert the coordinates of the transfer in a frame basis and compute the
 *               uFrame mask.
 *
 * @note     (2) If the interval is shorter than one frame, the uFrame mask will have to repeat
 *               itself.
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_EHCI_QH_BinTreePosToFrmList(USBH_HCD_BIN_TREE_EP_DESC pos_desc,
                                             CPU_INT16U                interval_max,
                                             CPU_INT08U                *p_frm_list_level,
                                             CPU_INT16U                *p_frm_list_ix,
                                             CPU_INT08U                *p_s_mask)
{
  CPU_INT08U submask_copy_cnt;
  CPU_INT08U level_max;
  CPU_INT16U bin_tree_ix;
  CPU_INT16U submask_len;

  level_max = CPU_CntTrailZeros16(interval_max);

  //                                                               Compute uFrame mask. See note 2.
  submask_len = (pos_desc.Level > (level_max - 3u)) ? DEF_BIT(level_max - pos_desc.Level) : 8u;
  *p_s_mask = DEF_BIT(pos_desc.IntervalLinearIx & (submask_len - 1u));

  //                                                               Copy and translate s_mask as needed.
  for (submask_copy_cnt = 1u; submask_copy_cnt < (8u / submask_len); submask_copy_cnt++) {
    *p_s_mask |= (*p_s_mask & DEF_BIT_FIELD(submask_len, 0u)) << (submask_len * submask_copy_cnt);
  }

  //                                                               Compute frame list level and index (phase).
  bin_tree_ix = USBH_HCD_BinTreeIxToggle(DEF_BIT(level_max - pos_desc.Level),
                                         pos_desc.IntervalLinearIx);

  *p_frm_list_level = (pos_desc.Level > (level_max - 3u)) ? (level_max - 3u) : pos_desc.Level;
  *p_frm_list_ix = bin_tree_ix >> 3u;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_EHCI_PeriodicBranchBwGet()
 *
 * @brief    Retrieves used bandwidth for a given branch of binary tree.
 *
 * @param    branch_ix   Index of the branch.
 *
 * @param    p_arg       Pointer to custom argument.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE   Operation successful.
 *
 * @return   Used bandwidth for given branch in bytes.
 *******************************************************************************************************/
#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED))
static CPU_INT16U USBH_EHCI_PeriodicBranchBwGet(CPU_INT16U branch_ix,
                                                void       *p_arg,
                                                RTOS_ERR   *p_err)
{
  CPU_INT16U      frm_list_ix;
  CPU_INT08U      ufrm_mask;
  CPU_INT16U      used_bw = 0u;
  USBH_HCD_DATA   *p_hcd_data = (USBH_HCD_DATA *)p_arg;
  USBH_EHCI_HC_QH *p_hc_qh_cur;

  if (p_hcd_data->BinTreeBW.BranchOnuFrm == DEF_YES) {
    CPU_INT16U frm_list_msk = (p_hcd_data->MaxPeriodicInterval - 1u);

    ufrm_mask = DEF_BIT(DEF_BIT_FIELD_RD(branch_ix, ~(unsigned)frm_list_msk));
    frm_list_ix = branch_ix & frm_list_msk;
  } else {
    ufrm_mask = 0x01u;
    frm_list_ix = branch_ix;
  }

  p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hcd_data->FrameList[frm_list_ix] & EHCI_FRM_LIST_LINK_PTR_MSK);
  while (p_hc_qh_cur != p_hcd_data->PeriodicDummyQH_Ptr) {
    CPU_INT08U         qh_s_msk;
    CPU_INT08U         qh_c_msk;
    USBH_HCD_EP_PARAMS *p_ep_params = &p_hc_qh_cur->HCD_EP_Ptr->EP_Params;

    qh_s_msk = DEF_BIT_FIELD_RD(p_hc_qh_cur->EP_Capabilities,
                                EHCI_QH_INT_SCHED_MSK);

    qh_c_msk = DEF_BIT_FIELD_RD(p_hc_qh_cur->EP_Capabilities,
                                EHCI_QH_SPLIT_CMPL_MSK);

    if (DEF_BIT_IS_SET(ufrm_mask, qh_s_msk) == DEF_YES) {
      if ((USBH_EP_IS_IN(p_ep_params->Addr) == DEF_NO)
          || (p_ep_params->DevSpd == USBH_DEV_SPD_HIGH)) {
        used_bw += USBH_HCD_EP_NeededBwGet(&p_hcd_data->BinTreeBW,
                                           p_ep_params);
      } else {
        used_bw += USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_SSPLIT;
      }
    } else if (DEF_BIT_IS_SET(ufrm_mask, qh_c_msk) == DEF_YES) {
      if (USBH_EP_IS_IN(p_ep_params->Addr) == DEF_YES) {
        used_bw += USBH_HCD_EP_NeededBwGet(&p_hcd_data->BinTreeBW,
                                           p_ep_params);
      } else {
        used_bw += USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_CSPLIT;
      }
    }

    p_hc_qh_cur = (USBH_EHCI_HC_QH *)(p_hc_qh_cur->HorLinkPtr & EHCI_QH_HOR_LINK_PTR_MSK);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (used_bw);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_EHCI_qTD_Submit()
 *
 * @brief    Prepares and submits a qTD.
 *
 * @param    p_hcd_data          Pointer to host controller driver data.
 *
 * @param    p_hcd_ep            Pointer to host controller driver endpoint.
 *
 * @param    p_hcd_urb_params    Pointer to structure describing URB.
 *
 * @param    p_hw_buf            Pointer to data buffer to use for transfer.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this function.
 *                                   - RTOS_ERR_NONE               Operation successful.
 *                                   - RTOS_ERR_DRV_URB_ALLOC  Failed to allocate required structure(s) for
 *                                     URB.
 *
 * @return   Host controller driver qTD.
 *
 * @note     (1) First HC qTD of a structure is either the dummy HC qTD of the previous URB Q-ed on
 *               the qH or, if nothing Q-ed on the qH, a new one.
 *******************************************************************************************************/
static USBH_EHCI_HCD_QTD *USBH_EHCI_qTD_Submit(USBH_HCD_DATA       *p_hcd_data,
                                               USBH_EHCI_HCD_EP    *p_hcd_ep,
                                               USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                               CPU_INT08U          *p_hw_buf,
                                               RTOS_ERR            *p_err)
{
  CPU_INT08U        qtd_toggle = 0u;
  CPU_INT08U        pid = 0u;
  CPU_INT08U        *p_buf_cur;
  CPU_INT32U        urb_xfer_len_rem = p_hcd_urb_params->BufLen;
  USBH_EHCI_HC_QH   *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  USBH_EHCI_HCD_QTD *p_hcd_qtd;
  USBH_EHCI_HCD_QTD *p_hcd_qtd_qh_tail;
  USBH_EHCI_HC_QTD  *p_hc_qtd_prev;
  USBH_EHCI_HC_QTD  *p_hc_qtd_cur;
  USBH_EHCI_HC_QTD  *p_hc_qtd_dummy;
  USBH_EHCI_HC_QTD  *p_hc_qtd_first;
  CORE_DECLARE_IRQ_STATE;

  if ((p_hcd_ep->EP_Params.Type == USBH_EP_TYPE_CTRL)
      && (p_hcd_urb_params->Token != USBH_TOKEN_SETUP)) {
    qtd_toggle = 1u;
  }

  switch (p_hcd_urb_params->Token) {
    case USBH_TOKEN_SETUP:
      pid = EHCI_QTD_PID_SETUP;
      break;

    case USBH_TOKEN_OUT:
      pid = EHCI_QTD_PID_OUT;
      break;

    case USBH_TOKEN_IN:
      pid = EHCI_QTD_PID_IN;
      break;
  }

  //                                                               Get a HCD qTD structure
  p_hcd_qtd = (USBH_EHCI_HCD_QTD *)Mem_DynPoolBlkGet(&p_hcd_data->HCD_qTD_Pool,
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);

    LOG_ERR(("EHCI cannot alloc HCD QTD -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (DEF_NULL);
  }

  p_hcd_qtd->HC_qTD_HeadPtr = DEF_NULL;
  p_hcd_qtd->HC_qTD_TailPtr = DEF_NULL;
  p_hcd_qtd->HC_qTD_DummyPtr = DEF_NULL;
  p_hcd_qtd->HW_BufPtr = p_hw_buf;

  //                                                               Retrieve first HC qTD. See note 1.
  CORE_ENTER_ATOMIC();
  p_hcd_qtd_qh_tail = p_hc_qh->HCD_QTD_TailPtr;

  if (p_hcd_qtd_qh_tail == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    p_hc_qtd_first = (USBH_EHCI_HC_QTD *)Mem_DynPoolBlkGet(&p_hcd_data->HC_qTD_Pool,
                                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);

      (void)USBH_EHCI_qTD_Free(p_hcd_data, p_hcd_qtd);

      LOG_ERR(("EHCI cannot alloc HC QTD -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      return (DEF_NULL);
    }

    p_hcd_qtd->HC_qTD_HeadPtr = p_hc_qtd_first;
    p_hcd_qtd->HC_qTD_TailPtr = p_hc_qtd_first;
  } else {
    p_hc_qtd_first = p_hcd_qtd_qh_tail->HC_qTD_DummyPtr;
    CORE_EXIT_ATOMIC();
  }

  //                                                               Allocate a dummy qTD to put at end.
  p_hc_qtd_dummy = (USBH_EHCI_HC_QTD *)Mem_DynPoolBlkGet(&p_hcd_data->HC_qTD_Pool,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);

    (void)USBH_EHCI_qTD_Free(p_hcd_data, p_hcd_qtd);

    LOG_ERR(("EHCI cannot alloc HC QTD -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (DEF_NULL);
  }

  p_hcd_qtd->HC_qTD_DummyPtr = p_hc_qtd_dummy;
  p_hc_qtd_dummy->NextPtr = EHCI_QTD_T;
  p_hc_qtd_dummy->AltNextPtr = EHCI_QTD_T;
  p_hc_qtd_dummy->Token = DEF_BIT_NONE;

  CPU_DCACHE_RANGE_FLUSH(p_hc_qtd_dummy, EHCI_QTD_LEN);

  p_buf_cur = p_hw_buf;
  p_hc_qtd_prev = DEF_NULL;
  p_hc_qtd_cur = p_hc_qtd_first;

  do {
    CPU_INT08U page_ix = 0u;
    CPU_INT16U qtd_xfer_len = 0u;

    p_hc_qtd_cur->AltNextPtr = (CPU_INT32U)p_hc_qtd_dummy & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;
    if (p_hc_qtd_prev != DEF_NULL) {
      p_hc_qtd_prev->NextPtr = (CPU_INT32U)p_hc_qtd_cur & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;

      CPU_DCACHE_RANGE_FLUSH(p_hc_qtd_prev, EHCI_QTD_LEN);
    }

    while ((qtd_xfer_len < urb_xfer_len_rem)
           && (page_ix < EHCI_QTD_PAGE_QTY)) {
      CPU_INT16U qtd_page_len = 0u;

      p_hc_qtd_cur->BufPagePtrList[page_ix] = (CPU_INT32U)p_buf_cur;
#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)
      p_hc_qtd_cur->ExtBufPagePtrList[page_ix] = (CPU_INT32U)DEF_BIT_FIELD_RD((CPU_INT64U)p_buf_cur,
                                                                              EHCI_64BIT_SEGMENT_MSK);
#endif

      qtd_page_len = EHCI_QTD_PAGE_LEN - DEF_BIT_FIELD_RD((CPU_ADDR)p_buf_cur, EHCI_QTD_CUR_OFFSET_MSK);
      qtd_page_len = DEF_MIN(qtd_page_len, (urb_xfer_len_rem - qtd_xfer_len));

      qtd_xfer_len += qtd_page_len;

      //                                                           Ensure qTD does not cross a max packet size boundary.
      if ((page_ix == EHCI_QTD_PAGE_QTY - 1u)
          && (urb_xfer_len_rem > qtd_xfer_len)) {
        qtd_page_len -= (qtd_xfer_len % p_hcd_ep->EP_Params.MaxPktSize);
        qtd_xfer_len -= (qtd_xfer_len % p_hcd_ep->EP_Params.MaxPktSize);
      }

      p_buf_cur += qtd_page_len;

      page_ix++;
    }

    //                                                             Init token field.
    p_hc_qtd_cur->Token = DEF_BIT_NONE;

    DEF_BIT_FIELD_WR(p_hc_qtd_cur->Token,
                     qtd_xfer_len,
                     EHCI_QTD_TOTAL_BYTES_TO_XFER_MSK);

    if (qtd_toggle == 1u) {
      DEF_BIT_SET(p_hc_qtd_cur->Token, EHCI_QTD_DATA_TOGGLE);
    }

    DEF_BIT_FIELD_WR(p_hc_qtd_cur->Token,
                     3u,
                     EHCI_QTD_CERR_MSK);

    DEF_BIT_FIELD_WR(p_hc_qtd_cur->Token,
                     pid,
                     EHCI_QTD_PID_CODE_MSK);

    if (p_hc_qtd_cur != p_hc_qtd_first) {                       // First qTD active at end in case it is dummy.
      DEF_BIT_SET(p_hc_qtd_cur->Token, EHCI_QTD_STATUS_ACTIVE);
    }

    urb_xfer_len_rem -= qtd_xfer_len;
    p_hc_qtd_prev = p_hc_qtd_cur;

    if (urb_xfer_len_rem > 0u) {
      p_hc_qtd_cur = (USBH_EHCI_HC_QTD *)Mem_DynPoolBlkGet(&p_hcd_data->HC_qTD_Pool,
                                                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);

        (void)USBH_EHCI_qTD_Free(p_hcd_data, p_hcd_qtd);

        LOG_ERR(("EHCI cannot alloc HC QTD -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return (DEF_NULL);
      }

      if (p_hcd_qtd->HC_qTD_HeadPtr == DEF_NULL) {
        p_hcd_qtd->HC_qTD_HeadPtr = p_hc_qtd_cur;
      }
      p_hcd_qtd->HC_qTD_TailPtr = p_hc_qtd_cur;
    }
  } while (urb_xfer_len_rem > 0u);

  if (p_hcd_qtd->HC_qTD_TailPtr == DEF_NULL) {
    p_hcd_qtd->HC_qTD_TailPtr = p_hc_qtd_cur;
  }

  //                                                               Since URB submit success, give ownership of last ...
  //                                                               ... HC qTD to this HCD qTD.
  if (p_hcd_qtd_qh_tail != DEF_NULL) {
    p_hcd_qtd_qh_tail->HC_qTD_DummyPtr = DEF_NULL;
    p_hcd_qtd->HC_qTD_HeadPtr = p_hc_qtd_first;
  }

  DEF_BIT_SET(p_hc_qtd_cur->Token, EHCI_QTD_IOC);

  p_hc_qtd_cur->NextPtr = ((CPU_INT32U)p_hc_qtd_dummy & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);

  CPU_DCACHE_RANGE_FLUSH(p_hc_qtd_cur, EHCI_QTD_LEN);

  CORE_ENTER_ATOMIC();

  CPU_DCACHE_RANGE_INV(p_hc_qh, EHCI_QH_LEN);

  //                                                               Insert HCD qTD in qH's list.
  p_hcd_qtd->NextPtr = DEF_NULL;

  if (p_hc_qh->HCD_QTD_HeadPtr == DEF_NULL) {
    p_hc_qh->HCD_QTD_HeadPtr = p_hcd_qtd;
  } else {
    p_hc_qh->HCD_QTD_TailPtr->NextPtr = p_hcd_qtd;
  }

  p_hc_qh->HCD_QTD_TailPtr = p_hcd_qtd;

  //                                                               Submit qTD on qH.
  DEF_BIT_SET(p_hc_qtd_first->Token, EHCI_QTD_STATUS_ACTIVE);

  CPU_DCACHE_RANGE_FLUSH(p_hc_qtd_first, EHCI_QTD_LEN);

  CPU_MB();

  if ((p_hc_qh->HCD_QTD_HeadPtr == p_hcd_qtd)
      && (DEF_BIT_IS_CLR(p_hc_qh->QTD_Token, EHCI_QTD_STATUS_ACTIVE) == DEF_YES)) {
    p_hc_qh->QTD_NextPtr = (CPU_INT32U)p_hc_qtd_first & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;
  }

  CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);

  CORE_EXIT_ATOMIC();

  return (p_hcd_qtd);
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_qTD_Complete()
 *
 * @brief    Completes a done transfer that use a qTD.
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_hcd_qtd   Pointer to HCD qTD.
 *
 * @param    p_rem_len   Pointer to variable that will receive the remaining length of transfer
 *                       in bytes.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE         Operation successful.
 *                           - RTOS_ERR_IO_FATAL     Fatal error occured during transfer.
 *                           - RTOS_ERR_IO           Error occured during transfer.
 *                           - RTOS_ERR_EP_STALL     Device stalled endpoint.
 *
 * @return   Pointer to qTD buffer.
 *******************************************************************************************************/
static CPU_INT08U *USBH_EHCI_qTD_Complete(USBH_HCD_DATA     *p_hcd_data,
                                          USBH_EHCI_HCD_EP  *p_hcd_ep,
                                          USBH_EHCI_HCD_QTD *p_hcd_qtd,
                                          CPU_INT32U        *p_rem_len,
                                          RTOS_ERR          *p_err)
{
  CPU_BOOLEAN      unlink = DEF_NO;
  CPU_INT08U       *p_hw_buf;
  CPU_INT08U       err_sts;
  USBH_EHCI_HC_QH  *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  USBH_EHCI_HC_QTD *p_hc_qtd_last = p_hcd_qtd->HC_qTD_HeadPtr;
  USBH_EHCI_HC_QTD *p_hc_qtd_next = (USBH_EHCI_HC_QTD *)(p_hc_qtd_last->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CPU_DCACHE_RANGE_INV(p_hc_qtd_next, EHCI_QTD_LEN);
  CPU_DCACHE_RANGE_INV(p_hc_qh, EHCI_QH_LEN);

  //                                                               Find last processed qTD.
  while ((p_hc_qtd_last != p_hcd_qtd->HC_qTD_TailPtr)
         && (DEF_BIT_IS_CLR(p_hc_qtd_next->Token, EHCI_QTD_STATUS_ACTIVE) == DEF_YES)) {
    p_hc_qtd_last = p_hc_qtd_next;
    p_hc_qtd_next = (USBH_EHCI_HC_QTD *)(p_hc_qtd_next->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);

    CPU_DCACHE_RANGE_INV(p_hc_qtd_next, EHCI_QTD_LEN);
  }

  //                                                               Check transfer status.
  err_sts = DEF_BIT_FIELD_RD(p_hc_qtd_last->Token,
                             EHCI_QTD_STATUS_MSK);

  if (DEF_BIT_IS_SET(err_sts, EHCI_QTD_STATUS_MISSED_UFRM) == DEF_YES) {
    LOG_ERR(("Missed u-frame detected by EHCI -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

  if (DEF_BIT_IS_SET(err_sts, EHCI_QTD_STATUS_HALTED) == DEF_YES) {
    LOG_ERR(("Endpoint Halted, addr: ", (u)p_hcd_ep->EP_Params.Addr, ", dev addr: ", (u)p_hcd_ep->EP_Params.DevAddr));

    if (DEF_BIT_IS_SET(err_sts, EHCI_QTD_STATUS_BABBLE) == DEF_YES) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO_FATAL);
      unlink = DEF_YES;
      LOG_ERR(("Babble detected by EHCI -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    } else if (DEF_BIT_IS_SET(err_sts, EHCI_QTD_STATUS_DATA_BUF_ERR) == DEF_YES) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      unlink = DEF_YES;
      LOG_ERR(("Data buffer error detected by EHCI -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    } else if (DEF_BIT_IS_SET(err_sts, EHCI_QTD_STATUS_XACT_ERR) == DEF_YES) {
      if (DEF_BIT_FIELD_RD(p_hc_qtd_last->Token, EHCI_QTD_CERR_MSK) == 0u) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        unlink = DEF_YES;
        LOG_ERR(("Transaction error detected by EHCI -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      }
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_STALL);

      LOG_DBG(("WARNING  - Endpoint stalled by device"));

      //                                                           Reset halt condition on control EP.
      if (p_hcd_ep->EP_Params.Type == USBH_EP_TYPE_CTRL) {
        DEF_BIT_CLR(p_hc_qh->QTD_Token, EHCI_QTD_STATUS_HALTED);
      } else {
        unlink = DEF_YES;
      }
    }
  }

  //                                                               In case of err, unlink qH. Re-linked in HaltClr fnct.
  if (unlink == DEF_YES) {
    RTOS_ERR err_unlink;

    USBH_EHCI_HC_QH_Unlink(p_hcd_data,
                           p_hcd_ep,
                           &err_unlink);
    if (RTOS_ERR_CODE_GET(err_unlink) != RTOS_ERR_NONE) {
      LOG_ERR(("While unlinking qH on error -> ", RTOS_ERR_LOG_ARG_GET(err_unlink)));
    }
  }

  CORE_ENTER_ATOMIC();                                          // Remove reference to qTD in qH.
  if (p_hc_qh->HCD_QTD_HeadPtr == DEF_NULL) {
    p_hc_qh->QTD_CurPtr = DEF_NULL;
    p_hc_qh->QTD_NextPtr = EHCI_QTD_T;
    p_hc_qh->QTD_AltNextPtr = EHCI_QTD_T;
  }

  CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);
  CORE_EXIT_ATOMIC();

  p_hw_buf = p_hcd_qtd->HW_BufPtr;
  *p_rem_len = USBH_EHCI_qTD_Free(p_hcd_data, p_hcd_qtd);

  return (p_hw_buf);
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_qTD_Abort()
 *
 * @brief    Aborts given qTD.
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_hcd_ep    Pointer to host controller driver endpoint.
 *
 * @param    p_hcd_qtd   Pointer to HCD qTD.
 *******************************************************************************************************/
static void USBH_EHCI_qTD_Abort(USBH_HCD_DATA     *p_hcd_data,
                                USBH_EHCI_HCD_EP  *p_hcd_ep,
                                USBH_EHCI_HCD_QTD *p_hcd_qtd)
{
  USBH_EHCI_HC_QH   *p_hc_qh = (USBH_EHCI_HC_QH *)p_hcd_ep->HC_EP_Ptr;
  USBH_EHCI_HCD_QTD *p_hcd_qtd_cur;
  USBH_EHCI_HC_QTD  *p_hc_qtd_head = p_hcd_qtd->HC_qTD_HeadPtr;
  USBH_EHCI_HC_QTD  *p_hc_qtd_cur = p_hc_qtd_head;
  USBH_EHCI_HCD_QTD *p_hcd_qtd_prev = DEF_NULL;
  USBH_EHCI_HC_QTD  *p_hc_qtd_prev = DEF_NULL;
  RTOS_ERR          err_usbh;
  CORE_DECLARE_IRQ_STATE;

  USBH_EHCI_HC_QH_Unlink(p_hcd_data,
                         p_hcd_ep,
                         &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to unlink HC qH -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  CPU_DCACHE_RANGE_INV(p_hc_qh, EHCI_QH_LEN);

  CORE_ENTER_ATOMIC();                                          // Remove qTD from all lists.
  p_hcd_qtd_cur = p_hc_qh->HCD_QTD_HeadPtr;

  //                                                               Retrieve HCD qTD to remove from qH's list.
  while ((p_hcd_qtd_cur != DEF_NULL)
         && (p_hcd_qtd_cur != p_hcd_qtd)) {
    p_hcd_qtd_prev = p_hcd_qtd_cur;
    p_hcd_qtd_cur = p_hcd_qtd_cur->NextPtr;
  }
  if (p_hcd_qtd_cur == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    if (p_hcd_ep->IsSuspended == DEF_NO) {
      USBH_EHCI_HC_QH_Link(p_hcd_data,
                           p_hcd_ep,
                           &err_usbh);
      if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
        LOG_ERR(("Unable to re-link HC qH -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
      }
    }

    LOG_DBG(("qTD to abort not found in qH list."));
    return;
  }

  if (p_hcd_qtd_prev == DEF_NULL) {
    p_hc_qh->HCD_QTD_HeadPtr = p_hcd_qtd_cur->NextPtr;
  } else {
    p_hcd_qtd_prev->NextPtr = p_hcd_qtd_cur->NextPtr;

    //                                                             Remove HC qTD(s) from HC qH's list.
    p_hcd_qtd_prev->HC_qTD_TailPtr->NextPtr = p_hcd_qtd_cur->HC_qTD_TailPtr->NextPtr;

    if (USBH_EP_IS_IN(p_hcd_ep->EP_Params.Addr) == DEF_YES) {
      //                                                           Adjust AltNextPtr for IN EP.
      p_hc_qtd_cur = p_hcd_qtd_prev->HC_qTD_HeadPtr;
      p_hc_qtd_prev = DEF_NULL;
      while (p_hc_qtd_prev != p_hcd_qtd_prev->HC_qTD_TailPtr) {
        p_hc_qtd_cur->AltNextPtr = (CPU_INT32U)p_hcd_qtd_cur->HC_qTD_TailPtr->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;

        p_hc_qtd_prev = p_hc_qtd_cur;
        p_hc_qtd_cur = (USBH_EHCI_HC_QTD *)(p_hc_qtd_cur->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);
      }
    }
  }

  if (p_hc_qh->HCD_QTD_TailPtr == p_hcd_qtd) {
    p_hc_qh->HCD_QTD_TailPtr = p_hcd_qtd_prev;
  }

  //                                                               qTD to remove is last one. Must restitute dummy qTD.
  if ((p_hcd_qtd_cur->NextPtr == DEF_NULL)
      && (p_hcd_qtd_prev != DEF_NULL)) {
    DEF_BIT_CLR(p_hc_qtd_head->Token, EHCI_QTD_STATUS_ACTIVE);

    p_hcd_qtd_prev->HC_qTD_DummyPtr = p_hc_qtd_head;

    if (p_hcd_qtd->HC_qTD_HeadPtr != p_hcd_qtd->HC_qTD_TailPtr) {
      p_hcd_qtd->HC_qTD_HeadPtr = (USBH_EHCI_HC_QTD *)(p_hcd_qtd->HC_qTD_HeadPtr->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);
    } else {
      p_hcd_qtd->HC_qTD_HeadPtr = DEF_NULL;
      p_hcd_qtd->HC_qTD_TailPtr = DEF_NULL;
    }
  }
  CORE_EXIT_ATOMIC();

  //                                                               Chk if a HC qTD to remove is currently being processed.
  p_hc_qtd_cur = p_hc_qtd_head;
  p_hc_qtd_prev = DEF_NULL;
  while (p_hc_qtd_prev != p_hcd_qtd_cur->HC_qTD_TailPtr) {
    if (((CPU_INT32U)p_hc_qtd_cur == p_hc_qh->QTD_CurPtr)
        || ((CPU_INT32U)p_hc_qtd_cur == (p_hc_qh->QTD_NextPtr    & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK))
        || ((CPU_INT32U)p_hc_qtd_cur == (p_hc_qh->QTD_AltNextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK))) {
      if (p_hcd_qtd->NextPtr == DEF_NULL) {
        DEF_BIT_CLR(p_hc_qh->QTD_Token,
                    EHCI_QTD_STATUS_ACTIVE);

        p_hc_qh->QTD_CurPtr = DEF_NULL;
        p_hc_qh->QTD_NextPtr = EHCI_QTD_T;
        p_hc_qh->QTD_AltNextPtr = EHCI_QTD_T;
      } else {
        p_hc_qh->QTD_NextPtr = (CPU_INT32U)p_hcd_qtd->NextPtr->HC_qTD_HeadPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK;
      }

      CPU_DCACHE_RANGE_FLUSH(p_hc_qh, EHCI_QH_LEN);
    }

    p_hc_qtd_prev = p_hc_qtd_cur;
    p_hc_qtd_cur = (USBH_EHCI_HC_QTD *)(p_hc_qtd_cur->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);
  }

  if (p_hcd_ep->IsSuspended == DEF_NO) {
    USBH_EHCI_HC_QH_Link(p_hcd_data,
                         p_hcd_ep,
                         &err_usbh);
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      LOG_ERR(("Unable to re-link HC qH -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    }
  }

  (void)USBH_EHCI_qTD_Free(p_hcd_data, p_hcd_qtd);
}

/****************************************************************************************************//**
 *                                           USBH_EHCI_qTD_Free()
 *
 * @brief    Frees completed qTD.
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_hcd_qtd   Pointer to HCD qTD.
 *
 * @return   Remaining length in bytes of data to transfer in this qTD.
 *******************************************************************************************************/
static CPU_INT32U USBH_EHCI_qTD_Free(USBH_HCD_DATA     *p_hcd_data,
                                     USBH_EHCI_HCD_QTD *p_hcd_qtd)
{
  CPU_INT32U       rem_len = 0u;
  RTOS_ERR         err_lib;
  USBH_EHCI_HC_QTD *p_hc_qtd = p_hcd_qtd->HC_qTD_HeadPtr;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Free all HC qTD(s) associated to this URB.
  if (p_hcd_qtd->HC_qTD_HeadPtr != DEF_NULL) {
    p_hcd_qtd->HC_qTD_TailPtr->NextPtr = DEF_NULL;
    do {
      USBH_EHCI_HC_QTD *p_hc_qtd_next = (USBH_EHCI_HC_QTD *)(p_hc_qtd->NextPtr & EHCI_QTD_NEXT_XFER_ELEM_PTR_MSK);

      rem_len += DEF_BIT_FIELD_RD(p_hc_qtd->Token, EHCI_QTD_TOTAL_BYTES_TO_XFER_MSK);

      Mem_DynPoolBlkFree(&p_hcd_data->HC_qTD_Pool,
                         (void *)p_hc_qtd,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing HC qTD -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }

      p_hc_qtd = p_hc_qtd_next;
    } while (p_hc_qtd != DEF_NULL);
  }

  //                                                               Free dummy qTD if present.
  CORE_ENTER_ATOMIC();
  if (p_hcd_qtd->HC_qTD_DummyPtr != DEF_NULL) {
    USBH_EHCI_HC_QTD *p_hc_qtd_dummy = p_hcd_qtd->HC_qTD_DummyPtr;

    p_hcd_qtd->HC_qTD_DummyPtr = DEF_NULL;
    CORE_EXIT_ATOMIC();

    Mem_DynPoolBlkFree(&p_hcd_data->HC_qTD_Pool,
                       (void *)p_hc_qtd_dummy,
                       &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing HC qTD -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    }
  } else {
    CORE_EXIT_ATOMIC();
  }

  //                                                               Free HCD qTD.
  Mem_DynPoolBlkFree(&p_hcd_data->HCD_qTD_Pool,
                     (void *)p_hcd_qtd,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing HCD qTD -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  return (rem_len);
}

/****************************************************************************************************//**
 *                                       USBH_EHCI_AsyncQH_IOAAD_IntPend()
 *
 * @brief    Pend until interrupt on async advance doorbell is triggered.
 *
 * @param    p_hcd_data  Pointer to host controller driver data.
 *
 * @param    p_err       Pointer to variable that will receive the error code from this function.
 *                           - RTOS_ERR_NONE       Operation successful.
 *                           - RTOS_ERR_ISR
 *                           - RTOS_ERR_OS
 *******************************************************************************************************/
static void USBH_EHCI_AsyncQH_IOAAD_IntPend(USBH_HCD_DATA *p_hcd_data,
                                            RTOS_ERR      *p_err)
{
  USBH_EHCI_OPER_REG *p_oper_reg = p_hcd_data->OperRegPtr;

  //                                                               Lock async unlink done ressources.
  KAL_LockAcquire(p_hcd_data->QH_AsyncUnlinkDoneLockHandle,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               En Int on Async Advance Doorbell and wait for it.
  DEF_BIT_SET(p_oper_reg->USBCMD, EHCI_USBCMD_IOAAD);

  //                                                               Pend until interrupt is triggered.
  KAL_SemPend(p_hcd_data->QH_AsyncUnlinkDoneSemHandle,
              KAL_OPT_PEND_NONE,
              KAL_TIMEOUT_INFINITE,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_LockRelease(p_hcd_data->QH_AsyncUnlinkDoneLockHandle,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
