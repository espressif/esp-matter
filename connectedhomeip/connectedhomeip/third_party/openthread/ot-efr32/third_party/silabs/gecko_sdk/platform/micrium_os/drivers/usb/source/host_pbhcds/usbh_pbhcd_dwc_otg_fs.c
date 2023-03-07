/***************************************************************************//**
 * @file
 * @brief USB Host Pipe-Based Host Controller Driver (Pbhcd)
 *        Synopsys Designware Core USB 2.0 Otg (FS)
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL))

#error USB HOST DWC OTG FS driver requires USB Host PBHCI Module. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_PBHCI_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_PBHCD_DWCOTGFS_FS_MODULE

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/hcd/usbh_pbhcd_priv.h>
#include  <usb/include/host/usbh_pbhci.h>
#include  <em_device.h>
#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, PBHCD, DWC_OTG_FS)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/***********************************************************************************************************
 *                                       PLATFORM-SPECIFIC DEFINES
 *
 *   Notes:  (1) Allocation of data RAM for Endpoint FIFOs. The OTG-FS controller has a dedicated RAM of
 *               1.25 Kbytes = 1280 bytes = 320 32-bit words available for the endpoints IN and OUT.
 *               Host mode features:
 *               - 8 host channels (pipes)
 *               - 1 shared RX FIFO
 *               - 1 periodic TX FIFO
 *               - 1 non-periodic TX FIFO
 *
 *           (2) Receive data FIFO size = Status information + data IN channel + transfer complete status
 *               information
 *
 *               (a) Status information = 1 space
 *                   (one space for status information written to the FIFO along with each received packet)
 *
 *               (b) data IN channel    = (Largest Packet Size / 4) + 1 spaces
 *                   (MINIMUM to receive packets)
 *
 *               (c) OR data IN channel = at least 2 * (Largest Packet Size / 4) + 1 spaces
 *                   (if high-bandwidth channel is enabled or multiple isochronous channels)
 *
 *               (d) transfer complete status information = 1 space per OUT channel
 *                   (one space for transfer complete status information also pushed to the FIFO
 *                   with each channel's last packet)
 *
 *           (3) Non-periodic Transmit FIFO RAM allocation:
 *               - MIN RAM space = largest max packet size among all supported non-periodic OUT channels.
 *               - RECOMMENDED   = two largest packet sizes
 *
 *           (4) Periodic Transmit FIFO RAM allocation:
 *               - MIN RAM space = largest max packet size among all supported periodic OUT channels.
 *               - if one high bandwidth isochronous OUT channel, 2 * max packet size of that channel
 **********************************************************************************************************/

#define  OTGFS_MAX_NBR_HOST_CH                             12u  // Maximum number of endpoints.
#define  DWCOTGFS_DFIFO_SIZE                             1024u  // Number of entries.

#define  DWCOTGFS_MAX_RETRY                             10000u  // Maximum number of retries.

//                                                                 FIFO depth is specified in 32-bit words (see Note #1)
#define  DWCOTGFS_RXFIFO_START_ADDR                         0u
#define  DWCOTGFS_RXFIFO_DEPTH                             64u  // For all IN channel (see Note #2).
#define  DWCOTGFS_NONPER_CH_TXFIFO_DEPTH                  128u  // For all Non-Periodic channels (see Note #3).
#define  DWCOTGFS_NONPER_CH_TXFIFO_START_ADDR    (DWCOTGFS_RXFIFO_DEPTH)
#define  DWCOTGFS_PER_CH_TXFIFO_DEPTH                     128u  // For all Periodic channels (see Note #4).
#define  DWCOTGFS_PER_CH_TXFIFO_START_ADDR       (DWCOTGFS_NONPER_CH_TXFIFO_DEPTH \
                                                  + DWCOTGFS_NONPER_CH_TXFIFO_START_ADDR)

/********************************************************************************************************
 *                                               STATS DEFINES
 *******************************************************************************************************/

#define  USBH_DWCOTGFS_DBG_STATS_EN  DEF_DISABLED

#if (USBH_DWCOTGFS_DBG_STATS_EN == DEF_ENABLED)
#define  USBH_DWCOTGFS_DBG_STATS_RESET()                    Mem_Clr((void *)&USBH_DWCOTGFS_DbgStats, \
                                                                    (CPU_SIZE_T) sizeof(USBH_DWCOTGFS_DBG_STATS));

#define  USBH_DWCOTGFS_DBG_STATS_SET(stat, val)             USBH_DWCOTGFS_DbgStats.stat = val;
#define  USBH_DWCOTGFS_DBG_STATS_GET(stat)                  USBH_DWCOTGFS_DbgStats.stat;
#define  USBH_DWCOTGFS_DBG_STATS_INC(stat)                  USBH_DWCOTGFS_DbgStats.stat++;
#define  USBH_DWCOTGFS_DBG_STATS_INC_IF_TRUE(stat, bool)    if ((bool) == DEF_TRUE) { \
    USBH_DWCOTGFS_DBG_STATS_INC(stat);                                                \
}
#else
#define  USBH_DWCOTGFS_DBG_STATS_RESET()
#define  USBH_DWCOTGFS_DBG_STATS_SET(stat, val)
#define  USBH_DWCOTGFS_DBG_STATS_GET(stat)
#define  USBH_DWCOTGFS_DBG_STATS_INC(stat)
#define  USBH_DWCOTGFS_DBG_STATS_INC_IF_TRUE(stat, bool)
#endif

/********************************************************************************************************
 *                                           REGISTERS DEFINES
 *******************************************************************************************************/

//                                                                 -------------- AHB CONFIGURATION REG ---------------
#define  DWCOTGFS_GAHBCFG_GINTMSK                   DEF_BIT_00  // Global interrupt mask.
#define  DWCOTGFS_GAHBCFG_TXFELVL                   DEF_BIT_07  // Non-periodic TxFIFO empty level.
#define  DWCOTGFS_GAHBCFG_PTXFELVL                  DEF_BIT_08  // Periodic TxFIFO empty level.

//                                                                 ---------------- CORE INTERRUPT REG ----------------
//                                                                 ---------------- INTERRUPT MASK REG ----------------
#define  DWCOTGFS_GINTx_CMOD                        DEF_BIT_00  // Current mode of operation interrupt.
#define  DWCOTGFS_GINTx_OTGINT                      DEF_BIT_02  // OTG interrupt.
#define  DWCOTGFS_GINTx_SOF                         DEF_BIT_03  // Start of frame interrupt.
#define  DWCOTGFS_GINTx_RXFLVL                      DEF_BIT_04  // RxFIFO non-empty interrupt.
#define  DWCOTGFS_GINTx_NPTXFE                      DEF_BIT_05  // Non-periodic TxFIFO empty interrupt.
#define  DWCOTGFS_GINTx_IPXFR                       DEF_BIT_21  // Incomplete periodic transfer interrupt.
#define  DWCOTGFS_GINTx_HPRTINT                     DEF_BIT_24  // Host port interrupt.
#define  DWCOTGFS_GINTx_HCINT                       DEF_BIT_25  // Host Channel interrupt.
#define  DWCOTGFS_GINTx_PTXFE                       DEF_BIT_26  // Periodic TxFIFO empty interrupt.
#define  DWCOTGFS_GINTx_CIDSCHGM                    DEF_BIT_28  // Connector ID status change mask.
#define  DWCOTGFS_GINTx_DISCINT                     DEF_BIT_29  // Disconnect detected interrupt.
#define  DWCOTGFS_GINTx_SRQINT                      DEF_BIT_30  // Session request/new session detected interrupt mask.
#define  DWCOTGFS_GINTx_WKUPINT                     DEF_BIT_31  // Resume/remote wakeup detected interrupt mask.

//                                                                 -------------------- RESET REG ---------------------
#define  DWCOTGFS_GRSTCTL_CORE_SW_RST               DEF_BIT_00  // Core soft reset.
#define  DWCOTGFS_GRSTCTL_HCLK_SW_RST               DEF_BIT_01  // HCLK soft reset.
#define  DWCOTGFS_GRSTCTL_RXFFLUSH                  DEF_BIT_04  // RxFIFO flush.
#define  DWCOTGFS_GRSTCTL_TXFFLUSH                  DEF_BIT_05  // TxFIFO flush.
#define  DWCOTGFS_GRSTCTL_AHBIDL                    DEF_BIT_31  // AHB master idle.
#define  DWCOTGFS_GRSTCTL_NONPER_TXFIFO             0x00u
#define  DWCOTGFS_GRSTCTL_PER_TXFIFO                0x01u
#define  DWCOTGFS_GRSTCTL_ALL_TXFIFO                0x10u

//                                                                 ---------- RECEIVE STATUS DEBUG READ REG -----------
//                                                                 ----------- OTG STATUS READ AND POP REG ------------
#define  DWCOTGFS_GRXSTS_PKTSTS_IN                  2u          // IN data packet received.
#define  DWCOTGFS_GRXSTS_PKTSTS_IN_XFER_COMP        3u          // IN transfer completed(triggers an interrupt).
#define  DWCOTGFS_GRXSTS_PKTSTS_DATA_TOGGLE_ERR     5u          // Data toggle error(triggers an interrupt).
#define  DWCOTGFS_GRXSTS_PKTSTS_CH_HALTED           7u          // Channel halted(triggers an interrupt).
                                                                // bits [3:0] Channel Number.
#define  DWCOTGFS_GRXSTS_CHNUM_MSK                  DEF_BIT_FIELD(4u, 0u)
//                                                                 bits [14:4] Byte Count.
#define  DWCOTGFS_GRXSTS_BCNT_MSK                   DEF_BIT_FIELD(11u, 4u)
//                                                                 bits [16:15] Data PID.
#define  DWCOTGFS_GRXSTS_DPID_MSK                   DEF_BIT_FIELD(2u, 15u)
//                                                                 bits [20:17] Packet Status.
#define  DWCOTGFS_GRXSTS_PKTSTS_MSK                 DEF_BIT_FIELD(4u, 17u)

//                                                                 ---------- GENERAL CORE CONFIGURATION REG ----------
#define  DWCOTGFS_GCCFG_PWRDWN                      DEF_BIT_16  // Power down.
#define  DWCOTGFS_GCCFG_VBUSASEN                    DEF_BIT_18  // Enable the VBUS sensing A Device.
#define  DWCOTGFS_GCCFG_VBUSBSEN                    DEF_BIT_19  // Enable the VBUS sensing B Device.
#define  DWCOTGFS_GCCFG_SOFOUTEN                    DEF_BIT_20  // SOF output enable.

//                                                                 -------------- USB CONFIGURATION REG ---------------
#define  DWCOTGFS_GUSBCFG_TOCAL_MSK                 DEF_BIT_FIELD(3u, 0u)
#define  DWCOTGFS_GUSBCFG_TOCAL_VAL                 7u
#define  DWCOTGFS_GUSBCFG_PHYSEL                    DEF_BIT_06  // Full speed serial transceiver select.
#define  DWCOTGFS_GUSBCFG_SRPCAP                    DEF_BIT_08  // SRP-capable.
#define  DWCOTGFS_GUSBCFG_HNPCAP                    DEF_BIT_09  // HNP-capable.
#define  DWCOTGFS_GUSBCFG_FORCE_HOST                DEF_BIT_29  // Force host mode.

//                                                                 -------------- HOST CONFIGURATION REG --------------
#define  DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL_48MHz       1u          // Select 48MHz PHY clock frequency.
#define  DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL_6MHz        2u          // Select  6MHz PHY clock frequency.
#define  DWCOTGFS_HCFG_FS_LS_SUPPORT                DEF_BIT_02  // FS- and LS-only support.
#define  DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL             DEF_BIT_FIELD(2u, 0u)

//                                                                 -------------- HOST FRAME NUMBER REG ---------------
#define  DWCOTGFS_HFNUM_FRNUM_MSK                   0x0000FFFFu

//                                                                 --- HOST PERIODIC TRANSMIT FIFO/QUEUE STATUS REG ---
#define  DWCOTGFS_HPTXSTS_REQTYPE                   DEF_BIT_FIELD(2u, 25u)
#define  DWCOTGFS_HPTXSTS_CH_EP_NBR                 DEF_BIT_FIELD(4u, 27u)
#define  DWCOTGFS_HPTXSTS_REQTYPE_IN_OUT            0u
#define  DWCOTGFS_HPTXSTS_REQTYPE_ZLP               1u
#define  DWCOTGFS_HPTXSTS_REQTYPE_DIS_CH_CMD        3u
#define  DWCOTGFS_HPTXSTS_PTQXSAV_MSK               DEF_BIT_FIELD(8u, 16u)
#define  DWCOTGFS_HPTXSTS_PTXFSAVL_MSK              DEF_BIT_FIELD(16u, 0u)

//                                                                 --------- HOST ALL CHANNELS INTERRUPT REG ----------
#define  DWCOTGFS_HAINT_CH_INT_MSK                  DEF_BIT_FIELD(16u, 0u)

//                                                                 --- NON-PERIODIC TRANSMIT FIFO/QUEUE STATUS REG ----
#define  DWCOTGFS_HNPTXSTS_NPTQXSAV_MSK             DEF_BIT_FIELD(8u, 16u)
#define  DWCOTGFS_HNPTXSTS_NPTXFSAV_MSK             DEF_BIT_FIELD(16u, 0u)

//                                                                 -------- HOST CHANNEL-X CHARACTERISTICS REG --------
#define  DWCOTGFS_HCCHARx_EPDIR                     DEF_BIT_15  // Endpoint direction.
#define  DWCOTGFS_HCCHARx_LOW_SPEED_DEV             DEF_BIT_17  // Low speed device.
#define  DWCOTGFS_HCCHARx_MC_EC                     DEF_BIT_FIELD(2u, 20u)
#define  DWCOTGFS_HCCHARx_ODDFRM                    DEF_BIT_29  // Odd frame.
#define  DWCOTGFS_HCCHARx_CHDIS                     DEF_BIT_30  // Channel disable.
#define  DWCOTGFS_HCCHARx_CHENA                     DEF_BIT_31  // Channel enable.
#define  DWCOTGFS_HCCHARx_1_TRANSACTION             1u          // 1 transaction.
#define  DWCOTGFS_HCCHARx_2_TRANSACTION             2u          // 2 transaction per frame to be issued.
#define  DWCOTGFS_HCCHARx_3_TRANSACTION             3u          // 3 transaction per frame to be issued.
                                                                // bits [19:18] Endpoint Type
#define  DWCOTGFS_HCCHARx_EP_TYPE_MSK               DEF_BIT_FIELD(2u, 18u)
#define  DWCOTGFS_HCCHARx_EP_TYPE_CTRL              0u
#define  DWCOTGFS_HCCHARx_EP_TYPE_ISOC              1u
#define  DWCOTGFS_HCCHARx_EP_TYPE_BULK              2u
#define  DWCOTGFS_HCCHARx_EP_TYPE_INTR              3u
//                                                                 bits [14:11} Endpoint Number
#define  DWCOTGFS_HCCHARx_EP_NBR_MSK                DEF_BIT_FIELD(4u, 11u)
#define  DWCOTGFS_HCCHARx_MPSIZ_MSK                 DEF_BIT_FIELD(11u, 0u)
#define  DWCOTGFS_HCCHARx_DAD_MSK                   DEF_BIT_FIELD(7u, 22u)

//                                                                 ----------- HOST CHANNEL-X INTERRUPT REG -----------
#define  DWCOTGFS_HCINTx_XFRC                       DEF_BIT_00  // transfer complete interrupt.
#define  DWCOTGFS_HCINTx_CH_HALTED                  DEF_BIT_01  // Channel halted interrupt.
#define  DWCOTGFS_HCINTx_STALL                      DEF_BIT_03  // STALL response received interrupt.
#define  DWCOTGFS_HCINTx_NAK                        DEF_BIT_04  // NAK response received interrupt.
#define  DWCOTGFS_HCINTx_ACK                        DEF_BIT_05  // ACK response received/transmitted interrupt.
#define  DWCOTGFS_HCINTx_TXERR                      DEF_BIT_07  // Transaction error interrupt.
#define  DWCOTGFS_HCINTx_BBERR                      DEF_BIT_08  // Babble error interrupt.
#define  DWCOTGFS_HCINTx_FRMOR                      DEF_BIT_09  // Frame overrun interrupt.
#define  DWCOTGFS_HCINTx_DTERR                      DEF_BIT_10  // Data toggle error interrupt.
#define  DWCOTGFS_HCINTx_INT_MSK                    0x000007BBu
#define  DWCOTGFS_HCINTx_HALT_SRC_NONE              0u
#define  DWCOTGFS_HCINTx_HALT_SRC_XFER_CMPL         1u
#define  DWCOTGFS_HCINTx_HALT_SRC_STALL             2u
#define  DWCOTGFS_HCINTx_HALT_SRC_NAK               3u
#define  DWCOTGFS_HCINTx_HALT_SRC_TXERR             4u
#define  DWCOTGFS_HCINTx_HALT_SRC_BBERR             5u
#define  DWCOTGFS_HCINTx_HALT_SRC_FRMOR             6u
#define  DWCOTGFS_HCINTx_HALT_SRC_DTERR             7u
#define  DWCOTGFS_HCINTx_HALT_SRC_ABORT             8u

//                                                                 --------- HOST CHANNEL-X TRANSFER SIZE REG ---------
#define  DWCOTGFS_HCTSIZx_PID_DATA0                 0u
#define  DWCOTGFS_HCTSIZx_PID_DATA1                 2u
#define  DWCOTGFS_HCTSIZx_PID_DATA2                 1u
#define  DWCOTGFS_HCTSIZx_PID_SETUP_MDATA           3u
#define  DWCOTGFS_HCTSIZx_PID_NONE                  0xFFu
//                                                                 bits [30:29] PID.
#define  DWCOTGFS_HCTSIZx_PID_MSK                   DEF_BIT_FIELD(2u, 29u)
//                                                                 bits [28:19] Packet Count.
#define  DWCOTGFS_HCTSIZx_PKTCNT_MSK                DEF_BIT_FIELD(10u, 19u)
//                                                                 bits [18:0] Transfer size.
#define  DWCOTGFS_HCTSIZx_XFRSIZ_MSK                DEF_BIT_FIELD(19u, 0u)

//                                                                 --------- HOST PORT CONTROL AND STATUS REG ---------
#define  DWCOTGFS_HPRT_PORT_CONN_STS                DEF_BIT_00  // Port connect status.
#define  DWCOTGFS_HPRT_PORT_CONN_DET                DEF_BIT_01  // Port connect detected.
#define  DWCOTGFS_HPRT_PORT_EN                      DEF_BIT_02  // Port enable.
#define  DWCOTGFS_HPRT_PORT_EN_CHNG                 DEF_BIT_03  // Port enable/disable change.
#define  DWCOTGFS_HPRT_PORT_OC                      DEF_BIT_04  // Port overcurrent active.
#define  DWCOTGFS_HPRT_PORT_OCCHNG                  DEF_BIT_05  // Port overcurrent change.
#define  DWCOTGFS_HPRT_PORT_RES                     DEF_BIT_06  // Port resume.
#define  DWCOTGFS_HPRT_PORT_SUSP                    DEF_BIT_07  // Port suspend.
#define  DWCOTGFS_HPRT_PORT_RST                     DEF_BIT_08  // Port reset.
#define  DWCOTGFS_HPRT_PORT_PWR                     DEF_BIT_12  // Port power.
#define  DWCOTGFS_HPRT_PORT_SPEED                   DEF_BIT_FIELD(2u, 17u)
#define  DWCOTGFS_HPRT_PORT_SPD_FS                  1u
#define  DWCOTGFS_HPRT_PORT_SPD_LS                  2u
#define  DWCOTGFS_HPRT_PORT_SPD_NO_SPEED            3u
//                                                                 Macro to avoid clearing some bits generating int.
#define  DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg)       {  \
    DEF_BIT_CLR((reg), (DWCOTGFS_HPRT_PORT_EN          \
                        | DWCOTGFS_HPRT_PORT_CONN_DET  \
                        | DWCOTGFS_HPRT_PORT_EN_CHNG   \
                        | DWCOTGFS_HPRT_PORT_OCCHNG)); \
}

#define  DWCOTGFS_PCGCCTL_BIT_RSTPDWNMODULE         DEF_BIT_03
#define  DWCOTGFS_PCGCCTL_BIT_PWRCLMP               DEF_BIT_02
#define  DWCOTGFS_PCGCCTL_BIT_GATEHCLK              DEF_BIT_01
#define  DWCOTGFS_PCGCCTL_BIT_STPPCLK               DEF_BIT_00

/********************************************************************************************************
 *                           BIT/REG DEFINES FOR GIANT GECKO SERIES 1 ERRATA
 *******************************************************************************************************/

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
#define  USB_REG_DATTRIM1             *(CPU_REG32 *)(USB_BASE + 0x34u)
#define  DATTRIM1_BIT_ENDLYPULLUP      DEF_BIT_07
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DRIVER DATA TYPES
 *******************************************************************************************************/

typedef struct usbh_dwcotgfs_host_ch_reg {                      // ------------- HOST CHANNEL-X DATA TYPE -------------
  CPU_REG32 HCCHARx;                                            // Host channel-x characteristics
  CPU_REG32 HCSPLTx;                                            // Host channel-x split control
  CPU_REG32 HCINTx;                                             // Host channel-x interrupt
  CPU_REG32 HCINTMSKx;                                          // Host channel-x interrupt mask
  CPU_REG32 HCTSIZx;                                            // Host channel-x transfer size
  CPU_REG32 HCDMAx;                                             // Host channel-x DMA address
  CPU_REG32 RSVD1[2u];
} USBH_DWCOTGFS_HOST_CH_REG;

typedef struct usbh_dwcotgfs_dfifo_reg {                        // ---------- DATA FIFO ACCESS DATA TYPE --------------
  CPU_REG32 DATA[DWCOTGFS_DFIFO_SIZE];                          // 4K bytes for ALL channels.
} USBH_DWCOTGFS_DFIFO_REG;

typedef struct usbh_dwcotgfs_reg {
  //                                                               ----- CORE GLOBAL CONTROL AND STATUS REGISTERS -----
  CPU_REG32                 GOTGCTL;                            // 0x0000 Core control and status
  CPU_REG32                 GOTGINT;                            // 0x0004 Core interrupt
  CPU_REG32                 GAHBCFG;                            // 0x0008 Core AHB configuration
  CPU_REG32                 GUSBCFG;                            // 0x000C Core USB configuration
  CPU_REG32                 GRSTCTL;                            // 0x0010 Core reset
  CPU_REG32                 GINTSTS;                            // 0x0014 Core interrupt
  CPU_REG32                 GINTMSK;                            // 0x0018 Core interrupt mask
  CPU_REG32                 GRXSTSR;                            // 0x001C Core receive status debug read
  CPU_REG32                 GRXSTSP;                            // 0x0020 Core status read and pop
  CPU_REG32                 GRXFSIZ;                            // 0x0024 Core receive FIFO size
  CPU_REG32                 HNPTXFSIZ;                          // 0x0028 Host non-periodic transmit FIFO size
  CPU_REG32                 HNPTXSTS;                           // 0x002C Core Non Periodic Tx FIFO/Queue status
  CPU_REG32                 RSVD0[2u];
  CPU_REG32                 GCCFG;                              // 0x0038 General core configuration
  CPU_REG32                 CID;                                // 0x003C Core ID register
  CPU_REG32                 RSVD1[48u];
  CPU_REG32                 HPTXFSIZ;                           // 0x0100 Core Host Periodic Tx FIFO size
  CPU_REG32                 RSVD2[191u];
  //                                                               ------ HOST MODE CONTROL AND STATUS REGISTERS ------
  CPU_REG32                 HCFG;                               // 0x0400 Host configuration
  CPU_REG32                 HFIR;                               // 0x0404 Host frame interval
  CPU_REG32                 HFNUM;                              // 0x0408 Host frame number/frame time remaining
  CPU_REG32                 RSVD3;
  CPU_REG32                 HPTXSTS;                            // 0x0410 Host periodic transmit FIFO/queue status
  CPU_REG32                 HAINT;                              // 0x0414 Host all channels interrupt
  CPU_REG32                 HAINTMSK;                           // 0x0418 Host all channels interrupt mask
  CPU_REG32                 RSVD4[9u];
  CPU_REG32                 HPRT;                               // 0x0440 Host port control and status
  CPU_REG32                 RSVD5[47u];
  USBH_DWCOTGFS_HOST_CH_REG HCH[16u];                           // 0x0500 Host Channel-x regiters
  CPU_REG32                 RSVD6[448u];
  //                                                               -- POWER & CLOCK GATING CONTROL & STATUS REGISTER --
  CPU_REG32                 PCGCR;                              // 0x0E00 Power anc clock gating control
  CPU_REG32                 RSVD7[127u];
  //                                                               --- DATA FIFO (DFIFO) HOST-CH X ACCESS REGISTERS ---
  USBH_DWCOTGFS_DFIFO_REG   DFIFO[OTGFS_MAX_NBR_HOST_CH];       // 0x1000 Data FIFO host channel-x access registers
} USBH_DWCOTGFS_REG;

typedef struct usbh_dwcotgfs_ch_info {
  CPU_INT08U  CurXferErrCnt;                                    // For IN and OUT xfer when Transaction Error int.
  CPU_INT08U  HaltSrc;                                          // Indicates source of the halt interrupt.
  CPU_BOOLEAN Halting;                                          // Flag indicating channel halt in progress.
  CPU_BOOLEAN Aborted;                                          // Flag indicating channel has been aborted.
  CPU_BOOLEAN RestartComm;                                      // Flag indicating to restart comm on an unwanted...
                                                                // ...aborted pipe.

  CPU_INT32U  RxXferLen;                                        // Nbr of bytes received in Rx FIFO.
  CPU_INT32U  RxXferCopyLen;                                    // Nbr of bytes really copied from Rx FIFO to app buf.
  CPU_INT32U  RxAppBufLen;                                      // To ensure that the URB EP xfer size not corrupted ..
                                                                // ..for multi-transaction transfer
  CPU_INT08U  *RxAppBufPtr;                                     // Ptr to buf supplied by app.
} USBH_DWCOTGFS_CH_INFO;

typedef struct usbh_pbhcd_data {
  CPU_INT32U            SavedGINTMSK;                           // Saved masked/unmasked int state in case of...
                                                                // ...suspend/resume event.
  USBH_DWCOTGFS_CH_INFO *ChInfoTblPtr;                          // Contains information about host channels.
  CPU_INT08U            ChQty;                                  // Quantity of avail ch.
} USBH_PBHCD_DATA;

/********************************************************************************************************
 *                                           STATS DATA TYPES
 *******************************************************************************************************/

#if (USBH_DWCOTGFS_DBG_STATS_EN == DEF_ENABLED)
typedef CPU_INT32U USBH_DBG_STATS_CNT;                          // Adjust size of the stats cntrs.

typedef struct usbh_strm32fx_int_rx_fifo_non_empty {
  USBH_DBG_STATS_CNT PktstsInCnt;
  USBH_DBG_STATS_CNT PktstsInXferCmplCnt;
  USBH_DBG_STATS_CNT PktstsDataToggleErrCnt;
  USBH_DBG_STATS_CNT PktstsChHaltedCnt;
  USBH_DBG_STATS_CNT PktstsReservedCnt;
  USBH_DBG_STATS_CNT OverrunCnt;
  USBH_DBG_STATS_CNT ChReEnableCnt;
  USBH_DBG_STATS_CNT LastWordFor8BitAccessCnt;
} USBH_DWCOTGFS_INT_RX_FIFO_NON_EMPTY;

typedef struct usbh_strm32fx_ch_int_src {
  CPU_BOOLEAN        XferInProgress;
  USBH_DBG_STATS_CNT IntHostChCnt;
  USBH_DBG_STATS_CNT IntXferCmplCnt;

  USBH_DBG_STATS_CNT IntChhCnt;
  USBH_DBG_STATS_CNT IntChh_ChAborted;
  USBH_DBG_STATS_CNT IntChhSrc_NONE;
  USBH_DBG_STATS_CNT IntChhSrc_XFER_CMPL;
  USBH_DBG_STATS_CNT IntChhSrc_STALL;
  USBH_DBG_STATS_CNT IntChhSrc_NAK;
  USBH_DBG_STATS_CNT IntChhSrc_TXERR;
  USBH_DBG_STATS_CNT IntChhSrc_BBERR;
  USBH_DBG_STATS_CNT IntChhSrc_FRMOR;
  USBH_DBG_STATS_CNT IntChhSrc_DTERR;
  USBH_DBG_STATS_CNT IntChhSrc_ABORT;

  USBH_DBG_STATS_CNT IntStallCnt;
  USBH_DBG_STATS_CNT IntNakCnt;
  USBH_DBG_STATS_CNT IntAckCnt;
  USBH_DBG_STATS_CNT IntTxErrCnt;
  USBH_DBG_STATS_CNT IntBabbleErrCnt;
  USBH_DBG_STATS_CNT IntFrmOverrunErrCnt;
  USBH_DBG_STATS_CNT IntDataToggleErrCnt;
} USBH_DWCOTGFS_CH_INT_SRC;

typedef struct usbh_strm32fx_ch_halt {
  USBH_DBG_STATS_CNT NonPeriodicTxReqQFullCnt;
  USBH_DBG_STATS_CNT PeriodicTxReqQFullCnt;
} USBH_DWCOTGFS_CH_HALT;

typedef struct usbh_dwcotgfs_dbg_stats {                        // ---------------------- STATS -----------------------
  USBH_DBG_STATS_CNT                  HCD_StartCnt;
  USBH_DBG_STATS_CNT                  HCD_StopCnt;
  USBH_DBG_STATS_CNT                  HCD_SuspendCnt;
  USBH_DBG_STATS_CNT                  HCD_ResumeCnt;

  USBH_DBG_STATS_CNT                  PortResetSetCnt;
  USBH_DBG_STATS_CNT                  PortResetClrCnt;
  USBH_DBG_STATS_CNT                  PortSuspendCnt;

  USBH_DBG_STATS_CNT                  DevConnCnt;
  USBH_DBG_STATS_CNT                  DevDisconnCnt;

  USBH_DBG_STATS_CNT                  PipePidCurGetCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeCfgCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeTxCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeTxStartCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeRxStartCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeRxCnt[OTGFS_MAX_NBR_HOST_CH];
  USBH_DBG_STATS_CNT                  PipeAbortCnt[OTGFS_MAX_NBR_HOST_CH];

  USBH_DBG_STATS_CNT                  GINTSTS_IntSofCnt;
  USBH_DBG_STATS_CNT                  GINTSTS_IntHprtCnt;
  USBH_DBG_STATS_CNT                  GINTSTS_IntDiscCnt;
  USBH_DBG_STATS_CNT                  GINTSTS_IntRxflvlCnt;
  USBH_DBG_STATS_CNT                  GINTSTS_IntHcIntCnt;
  USBH_DBG_STATS_CNT                  GINTSTS_IntIpxferCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntPortConnDetectedCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntWrongPortConnDetectedCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntPortEnDisChngCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntPortEnDisChng_NoDevConn_Cnt;
  USBH_DBG_STATS_CNT                  HPRT_IntOvercurrentChngCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntDisconnCnt;
  USBH_DBG_STATS_CNT                  HPRT_IntWrongDisconnCnt;
  USBH_DWCOTGFS_INT_RX_FIFO_NON_EMPTY GRXSTS_IntRxFIFONonEmptyTbl[OTGFS_MAX_NBR_HOST_CH];
  USBH_DWCOTGFS_CH_INT_SRC            HCINTx_IntHostChOutTbl[OTGFS_MAX_NBR_HOST_CH];
  USBH_DWCOTGFS_CH_INT_SRC            HCINTx_IntHostChInTbl[OTGFS_MAX_NBR_HOST_CH];

  USBH_DWCOTGFS_CH_HALT               HostChHaltTbl[OTGFS_MAX_NBR_HOST_CH];
} USBH_DWCOTGFS_DBG_STATS;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_DWCOTGFS_DBG_STATS_EN == DEF_ENABLED)
static USBH_DWCOTGFS_DBG_STATS USBH_DWCOTGFS_DbgStats;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DRIVER API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_PBHCD_Init(USBH_PBHCI_HC_DRV       *p_pbhci_hc_drv,
                            MEM_SEG                 *p_mem_seg,
                            MEM_SEG                 *p_mem_seg_buf,
                            USBH_PBHCI_INIT_CFG_EXT *p_init_cfg_ext,
                            RTOS_ERR                *p_err);

static void USBH_PBHCD_Start(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                             RTOS_ERR          *p_err);

static void USBH_PBHCD_Stop(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                            RTOS_ERR          *p_err);

static void USBH_PBHCD_Suspend(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               RTOS_ERR          *p_err);

static void USBH_PBHCD_Resume(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              RTOS_ERR          *p_err);

static CPU_INT16U USBH_PBHCD_FrameNbrGet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv);

static CPU_BOOLEAN USBH_PBHCD_PortResetSet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                           CPU_INT08U        port_nbr);

static CPU_BOOLEAN USBH_PBHCD_PortResetClr(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                           CPU_INT08U        port_nbr,
                                           USBH_DEV_SPD      *p_dev_spd);

static CPU_BOOLEAN USBH_PBHCD_PortSuspend(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                          CPU_INT08U        port_nbr,
                                          CPU_BOOLEAN       set);

static USBH_PBHCI_PID_DATA USBH_PBHCD_PipePidCurGet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                                    CPU_INT08U        pipe_nbr);

static void USBH_PBHCD_PipeCfg(USBH_PBHCI_HC_DRV   *p_pbhci_hc_drv,
                               CPU_INT08U          pipe_nbr,
                               USBH_HCD_EP_PARAMS  *p_hcd_ep_params,
                               USBH_TOKEN          pid_token,
                               USBH_PBHCI_PID_DATA pid_data_cur,
                               RTOS_ERR            *p_err);

static CPU_INT32U USBH_PBHCD_PipeTx(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                    CPU_INT08U        pipe_nbr,
                                    CPU_INT08U        *p_buf,
                                    CPU_INT32U        buf_len,
                                    RTOS_ERR          *p_err);

static void USBH_PBHCD_PipeTxStart(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                   CPU_INT08U        pipe_nbr,
                                   CPU_INT08U        *p_buf,
                                   CPU_INT32U        buf_len,
                                   RTOS_ERR          *p_err);

static void USBH_PBHCD_PipeRxStart(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                   CPU_INT08U        pipe_nbr,
                                   CPU_INT08U        *p_buf,
                                   CPU_INT32U        buf_len,
                                   RTOS_ERR          *p_err);

static CPU_INT32U USBH_PBHCD_PipeRx(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                    CPU_INT08U        pipe_nbr,
                                    CPU_INT08U        *p_buf,
                                    CPU_INT32U        buf_len,
                                    CPU_INT08U        *p_status,
                                    RTOS_ERR          *p_err);

static void USBH_PBHCD_PipeAbort(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                 CPU_INT08U        pipe_nbr,
                                 RTOS_ERR          *p_err);

static CPU_BOOLEAN USBH_PBHCD_SOF_IntEn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                        CPU_BOOLEAN       en);

static void USBH_PBHCD_ISR_Handle(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_DWCOTGFS_ISR_PortConn(USBH_DWCOTGFS_REG *p_reg,
                                       USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       USBH_PBHCD_DATA   *p_data);

static void USBH_DWCOTGFS_ISR_PortDisconn(USBH_DWCOTGFS_REG *p_reg,
                                          USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                          USBH_PBHCD_DATA   *p_data);

static void USBH_DWCOTGFS_ISR_RxFIFONonEmpty(USBH_DWCOTGFS_REG *p_reg,
                                             USBH_PBHCD_DATA   *p_data);

static void USBH_DWCOTGFS_ISR_HostChOUT(USBH_DWCOTGFS_REG *p_reg,
                                        USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                        USBH_PBHCD_DATA   *p_data,
                                        CPU_INT08U        ch_nbr);

static void USBH_DWCOTGFS_ISR_HostChIN(USBH_DWCOTGFS_REG *p_reg,
                                       USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       USBH_PBHCD_DATA   *p_data,
                                       CPU_INT08U        ch_nbr);

static void USBH_DWCOTGFS_SW_Reset(USBH_PBHCD_DATA *p_data);

static void USBH_DWCOTGFS_WrPkt(USBH_DWCOTGFS_REG *p_reg,
                                CPU_INT32U        *p_src,
                                CPU_INT08U        ch_nbr,
                                CPU_INT16U        bytes);

static void USBH_DWCOTGFS_ChHalt(USBH_DWCOTGFS_REG *p_reg,
                                 CPU_INT08U        ch_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                       INITIALIZED GLOBAL VARIABLES ACCESSED BY OTHER MODULES/OBJECTS
 ********************************************************************************************************
 *******************************************************************************************************/

USBH_PBHCI_HCD_CFG USBH_PBHCD_Cfg_DWCOTGFS_FS = {
  USBH_PBHCI_PERIODIC_XFER_MAX_PRECISION_INIFINITE,             // No restrictions for FS periodic interval.
  0u,                                                           // No support      for HS periodic interval.
  1u,                                                           // 1 port on root hub.

  DEF_NO                                                        // Remote wakeup supported by platform, but not by drv.
};

USBH_PBHCI_HC_API USBH_PBHCD_API_EFM32_OTG_FS_FIFO = {
  USBH_PBHCD_Init,
  DEF_NULL,
  USBH_PBHCD_Start,
  USBH_PBHCD_Stop,
  USBH_PBHCD_Suspend,
  USBH_PBHCD_Resume,
  USBH_PBHCD_FrameNbrGet,

  USBH_PBHCD_PortResetSet,
  USBH_PBHCD_PortResetClr,
  USBH_PBHCD_PortSuspend,

  DEF_NULL,
  DEF_NULL,

  USBH_PBHCD_PipePidCurGet,
  USBH_PBHCD_PipeCfg,
  USBH_PBHCD_PipeTx,
  USBH_PBHCD_PipeTxStart,
  USBH_PBHCD_PipeRxStart,
  USBH_PBHCD_PipeRx,
  USBH_PBHCD_PipeAbort,

  USBH_PBHCD_SOF_IntEn,

  &USBH_PBHCD_Cfg_DWCOTGFS_FS
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DRIVER API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/
/****************************************************************************************************//**
 *                                               USBH_PBHCD_Init()
 *
 * @brief    Initialize driver's internal structures and variables.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_mem_seg       Pointer to memory segment from which to allocate internal data.
 *
 * @param    p_mem_seg_buf   Pointer to memory segment from which to allocate buffers.
 *
 * @param    p_init_cfg_ext  Pointer to extended configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE       Operation was successful.
 *                               - RTOS_ERR_SEG_OVF    Data allocation failed.
 *                               - RTOS_ERR_INIT       Call to BSP's Init() failed.
 *******************************************************************************************************/
static void USBH_PBHCD_Init(USBH_PBHCI_HC_DRV       *p_pbhci_hc_drv,
                            MEM_SEG                 *p_mem_seg,
                            MEM_SEG                 *p_mem_seg_buf,
                            USBH_PBHCI_INIT_CFG_EXT *p_init_cfg_ext,
                            RTOS_ERR                *p_err)
{
  USBH_PBHCI_BSP_API *p_pbhci_bsp_api;
  USBH_PBHCD_DATA    *p_data;

  PP_UNUSED_PARAM(p_mem_seg_buf);

  p_data = (USBH_PBHCD_DATA *)Mem_SegAlloc("Synopsys DWC OTG FS driver data",
                                           p_mem_seg,
                                           sizeof(USBH_PBHCD_DATA),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_Clr(p_data, sizeof(USBH_PBHCD_DATA));

  //                                                               Alloc pipe tbl.
  p_data->ChQty = p_init_cfg_ext->PipeMaxNbr + 1u;
  p_data->ChInfoTblPtr = (USBH_DWCOTGFS_CH_INFO *)Mem_SegAlloc("Synopsys DWC OTG FS driver channel info table",
                                                               p_mem_seg,
                                                               sizeof(USBH_DWCOTGFS_CH_INFO) * (p_data->ChQty),
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_pbhci_hc_drv->DataPtr = (void *)p_data;
  p_pbhci_bsp_api = p_pbhci_hc_drv->BSP_API_Ptr;

  if ((p_pbhci_bsp_api != DEF_NULL)
      && (p_pbhci_bsp_api->Init != DEF_NULL)) {
    CPU_BOOLEAN ok;

    //                                                             Call BSP Init() function.
    ok = p_pbhci_bsp_api->Init(USBH_PBHCD_ISR_Handle, p_pbhci_hc_drv);
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
      return;
    }
  }

  USBH_DWCOTGFS_DBG_STATS_RESET();

  LOG_VRB(("Drv Init() done."));
}

/****************************************************************************************************//**
 *                                         USBH_PBHCD_Start()
 *
 * @brief    Starts Pipe-Based Host Controller. After this call, the USB pipe-based host controller
 *           is ready to detect device connections.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE   If operation is successful.
 *                               - RTOS_ERR_IO     Call to BSP's Start() failed.
 *                               - RTOS_ERR_IO     Waiting some operation termination or
 *                                                 flushing Tx/Rx FIFO fails.
 *
 * @note     (1) This reset flushes the control logic in the AHB Clock domain. Only AHB Clock Domain
 *               pipelines are reset.
 *                   - FIFOs are not flushed with this bit.
 *                   - All state machines in the AHB clock domain are reset to the Idle state after terminating the
 *                     transactions on the AHB, following the protocol.
 *                   - CSR control bits used by the AHB clock domain state machines are cleared.
 *
 * @note     (2) The application can write to this bit any time it wants to reset the core. It clears
 *               all the interrupts and all the CSR register bits except for the following bits:
 *                   - RSTPDMODL bit in PCGCCTL
 *                   - GAYEHCLK  bit in PCGCCTL
 *                   - PWRCLMP   bit in PCGCCTL
 *                   - STPPCLK   bit in PCGCCTL
 *                   - FSLSPCS   bit in HCFG
 *                   - DSPD      bit in DCFG
 *               @n
 *               All the transmit FIFOs and the receive FIFO are flushed
 *               Any transactions on the AHB Master are terminated as soon as possible.
 *               Any transactions on the USB are terminated immediately.
 *******************************************************************************************************/
static void USBH_PBHCD_Start(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                             RTOS_ERR          *p_err)
{
  USBH_PBHCI_BSP_API *p_pbhci_bsp_api = p_pbhci_hc_drv->BSP_API_Ptr;
  USBH_PBHCD_DATA    *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_REG  *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT16U         retry_cnt;
  CPU_INT32U         reg_val;
  CPU_INT08U         i;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(HCD_StartCnt);
  //                                                               ---------------- CALL BSP CFG FNCTS ----------------
  if (p_pbhci_bsp_api != DEF_NULL) {
    CPU_BOOLEAN ok;

    if (p_pbhci_bsp_api->ClkCfg != DEF_NULL) {
      ok = p_pbhci_bsp_api->ClkCfg();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
    }

    if (p_pbhci_bsp_api->IO_Cfg != DEF_NULL) {
      ok = p_pbhci_bsp_api->IO_Cfg();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
    }

    if (p_pbhci_bsp_api->IntCfg != DEF_NULL) {
      ok = p_pbhci_bsp_api->IntCfg();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
    }

    if (p_pbhci_bsp_api->PwrCfg != DEF_NULL) {
      ok = p_pbhci_bsp_api->PwrCfg();
      if (ok != DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
    }
  }

  USBH_DWCOTGFS_SW_Reset(p_data);                               // Init some drv's internal sw resources.

  DEF_BIT_CLR(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_PWRCLMP);
  DEF_BIT_CLR(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_RSTPDWNMODULE);
  DEF_BIT_CLR(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_STPPCLK);

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  DEF_BIT_SET(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

  //                                                               ------------- INIT CORE OF HOST CTRLR --------------
  DEF_BIT_CLR(p_reg->GAHBCFG, DWCOTGFS_GAHBCFG_GINTMSK);        // Disable the global interrupt in AHB cfg reg.

  retry_cnt = 0u;                                               // Check AHB master IDLE state before resetting core.
  reg_val = p_reg->GRSTCTL;
  while ((DEF_BIT_IS_CLR(reg_val, DWCOTGFS_GRSTCTL_AHBIDL) == DEF_YES)
         && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
    retry_cnt++;
    reg_val = p_reg->GRSTCTL;
  }
  if (retry_cnt == DWCOTGFS_MAX_RETRY) {
    LOG_ERR(("Waiting for AHB master IDLE state in PBHCD Start().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  DEF_BIT_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_CORE_SW_RST);    // Clr all other ctrl logic (see Note #2).
  retry_cnt = 0u;                                               // Wait all necessary logic is reset in the core.
  reg_val = p_reg->GRSTCTL;
  while ((DEF_BIT_IS_SET(reg_val, DWCOTGFS_GRSTCTL_CORE_SW_RST) == DEF_YES)
         && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
    retry_cnt++;
    reg_val = p_reg->GRSTCTL;
  }
  if (retry_cnt == DWCOTGFS_MAX_RETRY) {
    LOG_ERR(("Waiting for HCLK and PCLK domains reset in PBHCD Start().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               -------------- INIT HOST MODE OF CORE --------------
  DEF_BIT_SET(p_reg->GUSBCFG, DWCOTGFS_GUSBCFG_FORCE_HOST);     // Force the core in Host mode.
  KAL_Dly(50u);                                                 // Wait at least 25ms before core forces the Host mode.

  DEF_BIT_SET(p_reg->GUSBCFG, DWCOTGFS_GUSBCFG_TOCAL_VAL);      // Add a few nbr of PHY clk to FS interpkt dly.

  p_reg->PCGCR = 0u;                                            // Reset the Power and Clock Gating Register.
  p_reg->HCFG = (DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL_48MHz
                 | DWCOTGFS_HCFG_FS_LS_SUPPORT);                // FS- and LS-Only Support.
  p_reg->HFIR = 48000u;

  for (i = 0u; i < OTGFS_MAX_NBR_HOST_CH; i++) {                // Clear the interrupts for each channel.
    p_reg->HCH[i].HCINTMSKx = 0u;
    p_reg->HCH[i].HCINTx = DWCOTGFS_HCINTx_INT_MSK;
  }

  reg_val = p_reg->HPRT;

  if (DEF_BIT_IS_CLR(reg_val, DWCOTGFS_HPRT_PORT_PWR) == DEF_YES) {
    DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg_val);                   // To avoid clearing some important bits.
    DEF_BIT_SET(reg_val, DWCOTGFS_HPRT_PORT_PWR);               // Turn on the Host port power.
    p_reg->HPRT = reg_val;
  }

  KAL_Dly(50u);                                                 // Dly necessary for proper cfg of FIFO size.

  //                                                               Enables the Host mode interrupts.
  CORE_ENTER_ATOMIC();
  p_reg->GINTMSK = 0u;                                          // Disable all interrupts.
  p_reg->GINTSTS = 0xFFFFFFFFu;                                 // Clear any pending interrupts.
  p_reg->GOTGINT = 0xFFFFFFFFu;                                 // Clear any OTG pending interrupts.
  p_reg->GINTMSK = (DWCOTGFS_GINTx_RXFLVL                       // Unmask Receive FIFO Non-Empty interrupt.
                    | DWCOTGFS_GINTx_WKUPINT                    // Unmask Resume/Remote Wakeup Detected intr.
                    | DWCOTGFS_GINTx_HPRTINT);                  // Unmask Host Port intr.
  CORE_EXIT_ATOMIC();

  p_reg->GRXFSIZ = DWCOTGFS_RXFIFO_DEPTH;                       // Set Rx FIFO depth.
                                                                // Set Non-Periodic and Periodic Tx FIFO depths.
  p_reg->HNPTXFSIZ = (DWCOTGFS_NONPER_CH_TXFIFO_DEPTH << 16u)
                     | DWCOTGFS_NONPER_CH_TXFIFO_START_ADDR;
  p_reg->HPTXFSIZ = (DWCOTGFS_PER_CH_TXFIFO_DEPTH << 16u)
                    | DWCOTGFS_PER_CH_TXFIFO_START_ADDR;

  p_reg->GRSTCTL = (DWCOTGFS_GRSTCTL_TXFFLUSH
                    | (DWCOTGFS_GRSTCTL_ALL_TXFIFO << 6u));     // Flush All Tx FIFOs.
  retry_cnt = 0u;                                               // Wait for the complete FIFO flushing.
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_TXFFLUSH) == DEF_YES)
         && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
    retry_cnt++;
  }
  if (retry_cnt == DWCOTGFS_MAX_RETRY) {
    LOG_ERR(("Waiting for complete Tx FIFO flushing in PBHCD Start().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  p_reg->GRSTCTL = DWCOTGFS_GRSTCTL_RXFFLUSH;                   // Flush the entire RxFIFO.
  retry_cnt = 0u;                                               // Wait for the complete FIFO flushing.
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_RXFFLUSH) == DEF_YES)
         && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
    retry_cnt++;
  }
  if (retry_cnt == DWCOTGFS_MAX_RETRY) {
    LOG_ERR(("Waiting for complete Rx FIFO flushing in PBHCD Start().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               -------------- CALL BSP START() FNCT ---------------
  if ((p_pbhci_bsp_api != DEF_NULL)
      && (p_pbhci_bsp_api->Start != DEF_NULL)) {
    CPU_BOOLEAN ok;

    ok = p_pbhci_bsp_api->Start();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  DEF_BIT_SET(p_reg->GAHBCFG, DWCOTGFS_GAHBCFG_GINTMSK);        // Enable the global interrupt in AHB cfg reg.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBH_PBHCD_Stop()
 *
 * @brief    Stops Pipe-Based Host Controller.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE   Operation was successful.
 *                               - RTOS_ERR_IO     Call to BSP's Stop() failed.
 *******************************************************************************************************/
static void USBH_PBHCD_Stop(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                            RTOS_ERR          *p_err)
{
  USBH_PBHCI_BSP_API *p_pbhci_bsp_api = p_pbhci_hc_drv->BSP_API_Ptr;
  USBH_DWCOTGFS_REG  *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U         reg_val;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(HCD_StopCnt);

  DEF_BIT_CLR(p_reg->GAHBCFG, DWCOTGFS_GAHBCFG_GINTMSK);        // Disable the global interrupt.
  CORE_ENTER_ATOMIC();
  p_reg->GINTMSK = 0u;                                          // Disable all interrupts.
  p_reg->GINTSTS = 0xFFFFFFFFu;                                 // Clear any pending interrupts.
  CORE_EXIT_ATOMIC();
  p_reg->GOTGINT = 0xFFFFFFFFu;                                 // Clear any OTG pending interrupts.

  if ((p_pbhci_bsp_api != DEF_NULL)
      && (p_pbhci_bsp_api->Stop != DEF_NULL)) {
    CPU_BOOLEAN ok;

    ok = p_pbhci_bsp_api->Stop();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  reg_val = p_reg->HPRT;
  if (DEF_BIT_IS_SET(reg_val, DWCOTGFS_HPRT_PORT_PWR) == DEF_YES) {
    DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg_val);                   // To avoid clearing some important bits.
    DEF_BIT_CLR(reg_val, DWCOTGFS_HPRT_PORT_PWR);               // Turn off the Host port power.
    p_reg->HPRT = reg_val;
  }

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  DEF_BIT_CLR(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

  DEF_BIT_SET(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_PWRCLMP);
  DEF_BIT_SET(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_RSTPDWNMODULE);
  DEF_BIT_SET(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_STPPCLK);

  LOG_VRB(("Stop()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_Suspend()
 *
 * @brief    Suspends Pipe-Based Host Controller.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *******************************************************************************************************/
static void USBH_PBHCD_Suspend(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                               RTOS_ERR          *p_err)
{
  USBH_PBHCD_DATA   *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(HCD_SuspendCnt);

  CORE_ENTER_ATOMIC();
  p_data->SavedGINTMSK = p_reg->GINTMSK;                        // Save unmasked current intr.
  p_reg->GINTMSK = DEF_BIT_NONE;                                // Mask all possible intr.
  CORE_EXIT_ATOMIC();

  LOG_VRB(("Suspend()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_Resume()
 *
 * @brief    Resumes Pipe-Based Host Controller.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *******************************************************************************************************/
static void USBH_PBHCD_Resume(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                              RTOS_ERR          *p_err)
{
  USBH_PBHCD_DATA   *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(HCD_ResumeCnt);

  CORE_ENTER_ATOMIC();
  p_reg->GINTMSK = p_data->SavedGINTMSK;                        // Restored unmasked current intr.
  p_data->SavedGINTMSK = DEF_BIT_NONE;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("Resume()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_FrameNbrGet()
 *
 * @brief    Retrieves current frame number.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @return   Current frame number.
 *******************************************************************************************************/
static CPU_INT16U USBH_PBHCD_FrameNbrGet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT16U        frm_nbr = p_reg->HFNUM & USBH_FRM_NBR_MASK;

  LOG_VRB(("FrameNbrGet()"));

  return (frm_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PortResetSet()
 *
 * @brief    Starts reset signaling on given port.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    port_nbr        Port number.
 *
 * @return   DEF_OK,     If port reset successful.
 *           DEF_FAIL,   Otherwise.
 *
 *           Pipe-Based Host Controller Interface (PBHCI).
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_PBHCD_PortResetSet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                           CPU_INT08U        port_nbr)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        reg_val;

  (void)&port_nbr;

  USBH_DWCOTGFS_DBG_STATS_INC(PortResetSetCnt);

  reg_val = p_reg->HPRT;
  DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg_val);                     // To avoid clearing some important bits.
  DEF_BIT_SET(reg_val, DWCOTGFS_HPRT_PORT_RST);                 // Start reset signaling to dev.
  p_reg->HPRT = reg_val;

  LOG_VRB(("PortResetSet()"));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PortResetClr()
 *
 * @brief    Clear reset signaling on given port.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    port_nbr        Port number.
 *
 * @param    p_dev_spd       Pointer to variable that will contain the speed of the device
 *                           connected:
 *                           USBH_DEV_SPD_NONE           No device connected.
 *                           USBH_DEV_SPD_FULL           Full-speed device connected.
 *
 * @return   DEF_OK,     If port reset successful.
 *           DEF_FAIL,   Otherwise.
 *
 * @note     (1) The OTG-FS controller requires to properly configure the frame interval based on the
 *               device speed. The frame interval can only be set in the register HFIR after the end
 *               of the port reset. At the end of the port reset, the port is enabled and the
 *               interrupt PENCHNG will be generated. The correct frame interval is written in the
 *               register HFIR. The delay at this moment ensures that the frame interval configuration
 *               has been fully taken into account by the host controller before the initial control
 *               transfer occurs.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_PBHCD_PortResetClr(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                           CPU_INT08U        port_nbr,
                                           USBH_DEV_SPD      *p_dev_spd)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        hprt_reg;
  CPU_INT32U        port_speed;

  (void)&port_nbr;

  USBH_DWCOTGFS_DBG_STATS_INC(PortResetClrCnt);

  hprt_reg = p_reg->HPRT;
  DWCOTGFS_HPRT_BITS_CLR_PRESERVE(hprt_reg);                    // To avoid clearing some important bits.
  DEF_BIT_CLR(hprt_reg, DWCOTGFS_HPRT_PORT_RST);                // Clr USB reset.
  p_reg->HPRT = hprt_reg;

  hprt_reg = p_reg->HPRT;
  //                                                               A device is conn'd, check its speed.
  if (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_CONN_STS) == DEF_YES) {
    port_speed = DEF_BIT_FIELD_RD(hprt_reg, DWCOTGFS_HPRT_PORT_SPEED);

    switch (port_speed) {
      case DWCOTGFS_HPRT_PORT_SPD_LS:
        *p_dev_spd = USBH_DEV_SPD_LOW;
        break;

      case DWCOTGFS_HPRT_PORT_SPD_FS:
        *p_dev_spd = USBH_DEV_SPD_FULL;
        break;

      default:
        *p_dev_spd = USBH_DEV_SPD_NONE;
        break;
    }
    ;
  } else {                                                      // Report to PBHCI that No dev is conn'd.
    *p_dev_spd = USBH_DEV_SPD_NONE;
  }

  KAL_Dly(10u);                                                 // Dly necessary for proper cfg of HCFG & HFIR reg...
                                                                // ...(see Note #1).

  LOG_VRB(("PortResetClr()"));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PortSuspend()
 *
 * @brief    Suspends given port.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    port_nbr        Port number.
 *
 * @param    set             Flag indicating if port needs to be suspended or resumed:
 *                           DEF_SET         Suspend port.
 *                           DEF_CLR         Resume  port.
 *
 * @return   DEF_OK,     If port suspend successful.
 *           DEF_FAIL,   Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_PBHCD_PortSuspend(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                          CPU_INT08U        port_nbr,
                                          CPU_BOOLEAN       set)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        reg_val;

  (void)&port_nbr;

  USBH_DWCOTGFS_DBG_STATS_INC(PortSuspendCnt);

  reg_val = p_reg->HPRT;
  DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg_val);                     // To avoid clearing some important bits.

  if (set == DEF_SET) {                                         // Suspend port.
    DEF_BIT_SET(reg_val, DWCOTGFS_HPRT_PORT_SUSP);              // Stop generation of SOF.
    p_reg->HPRT = reg_val;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
    DEF_BIT_CLR(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

    DEF_BIT_SET(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_STPPCLK);
  } else {
    DEF_BIT_CLR(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_GATEHCLK);
    DEF_BIT_CLR(p_reg->PCGCR, DWCOTGFS_PCGCCTL_BIT_STPPCLK);

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
    DEF_BIT_SET(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

    DEF_BIT_SET(reg_val, DWCOTGFS_HPRT_PORT_RES);               // Issue resume signal to dev.
    p_reg->HPRT = reg_val;

    KAL_Dly(21u);                                               // Resume signal driven for 20 ms as per USB 2.0 spec.

    reg_val = p_reg->HPRT;
    DWCOTGFS_HPRT_BITS_CLR_PRESERVE(reg_val);                   // To avoid clearing some important bits.
    DEF_BIT_CLR(reg_val, DWCOTGFS_HPRT_PORT_RES);               // Clr USB resume signal.
    p_reg->HPRT = reg_val;
  }

  LOG_VRB(("PortSuspend()"));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_PBHCD_PipePidCurGet()
 *
 * @brief    Gets current PID data toggle value of given pipe.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @return   USBH_PBHCI_PID_DATA0, if PID data 0.
 *           USBH_PBHCI_PID_DATA1, if PID data 1.
 *           USBH_PBHCI_PID_DATA2, if PID data 2.
 *           USBH_PBHCI_PID_DATAM, if PID is M data.
 *
 * @note     (1) This function is never called on pipes configured as isochronous or control.
 *
 * @note     (2) The PID returned to PBHCI corresponds to the PID of the next transaction that will
 *               occur for this pipe. After transfer completion, the OTG-FS controller updates
 *               accordingly the field Data PID of register HCTSIZx for the next transaction. Thus no
 *               need of tracking the data toggle internally in the driver, tracking relies on the
 *               host controller.
 *               In case of NAKed transaction, the Data PID field has not been advanced to the next
 *               PID by the host controller.
 *******************************************************************************************************/
static USBH_PBHCI_PID_DATA USBH_PBHCD_PipePidCurGet(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                                    CPU_INT08U        pipe_nbr)
{
  USBH_DWCOTGFS_REG   *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U          hctsizx_reg_dpid;
  USBH_PBHCI_PID_DATA nxt_pid;

  USBH_DWCOTGFS_DBG_STATS_INC(PipePidCurGetCnt[pipe_nbr]);

  hctsizx_reg_dpid = DEF_BIT_FIELD_RD(p_reg->HCH[pipe_nbr].HCTSIZx, DWCOTGFS_HCTSIZx_PID_MSK);

  switch (hctsizx_reg_dpid) {                                   // See Note #2.
    case DWCOTGFS_HCTSIZx_PID_DATA0:
      nxt_pid = USBH_PBHCI_PID_DATA0;
      break;

    case DWCOTGFS_HCTSIZx_PID_DATA1:
      nxt_pid = USBH_PBHCI_PID_DATA1;
      break;

    case DWCOTGFS_HCTSIZx_PID_DATA2:
    case DWCOTGFS_HCTSIZx_PID_SETUP_MDATA:
    default:
      nxt_pid = USBH_PBHCI_PID_DATA0;
      break;
  }
  ;

  LOG_VRB(("PipePidCurGet()"));

  return (nxt_pid);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeCfg()
 *
 * @brief    Configures given pipe with endpoint parameters.
 *
 * @param    p_pbhci_hc_drv      Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr            Pipe number.
 *
 * @param    p_hcd_ep_params     Pointer to endpoint parameters structure.
 *
 * @param    cur_pid             Current PID data toggle for this endpoint.
 *                                   - USBH_PBHCI_PID_SETUP, if PID is SETUP.
 *                                   - USBH_PBHCI_PID_DATA0, if PID is data 0.
 *                                   - USBH_PBHCI_PID_DATA1, if PID is data 1.
 *                                   - USBH_PBHCI_PID_DATA2, if PID is data 2.
 *                                   - USBH_PBHCI_PID_DATAM, if PID is M data.
 *
 * @param    p_err               Pointer to variable that will receive the return error code :
 *                                   - RTOS_ERR_NONE           Operation was successful.
 *
 * @note     (1) The driver must program the Data PID field of register HCTSIZx with the initial
 *               PID:
 *               - (a) Control transaction:
 *                     SETUP  stage: initial PID = SETUP
 *                     DATA   stage: initial PID = DATA1 (IN or OUT transaction). Then alternation
 *                                   between DATA0 and DATA1.
 *                     STATUS stage: initial PID = DATA1 (IN or OUT transaction). Then alternation
 *                                   between DATA0 and DATA1.
 *               - (b) Bulk or Interrupt transaction: initial PID = DATA0 (IN or OUT transaction).
 *                     Then alternation between DATA0 and DATA1.
 *               The PBHCI layer always configures a control pipe before performing a transfer.
 *
 * @note     (2) All these interrupt sources are unmasked regardless the pipe type and direction.
 *               In some cases, some unnecessary interrupt are unmasked but it won't overload the CPU
 *               because they will never be fired. See 'DWCOTGFS_ISR_HostChOUT() and
 *               DWCOTGFS_ISR_HostChIN() Note #1' for more details about interrupts sources that must
 *               be managed according to pipe type and direction.
 *******************************************************************************************************/
static void USBH_PBHCD_PipeCfg(USBH_PBHCI_HC_DRV   *p_pbhci_hc_drv,
                               CPU_INT08U          pipe_nbr,
                               USBH_HCD_EP_PARAMS  *p_hcd_ep_params,
                               USBH_TOKEN          pid_token,
                               USBH_PBHCI_PID_DATA pid_data_cur,
                               RTOS_ERR            *p_err)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        reg_val = 0u;
  CPU_INT08U        ep_log_nbr = USBH_EP_ADDR_TO_LOG(p_hcd_ep_params->Addr);
  CPU_INT16U        ep_max_pkt_size = USBH_EP_MAX_PKT_SIZE_GET(p_hcd_ep_params->MaxPktSize);
  CPU_INT32U        pid_to_set;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(PipeCfgCnt[pipe_nbr]);

  //                                                               ----------- CFG HOST CH CHARACTERISTICS ------------
  reg_val = (CPU_INT32U)ep_max_pkt_size;
  DEF_BIT_FIELD_WR(reg_val,
                   ep_log_nbr,
                   DWCOTGFS_HCCHARx_EP_NBR_MSK);

  if (pid_token == USBH_TOKEN_IN) {
    DEF_BIT_SET(reg_val, DWCOTGFS_HCCHARx_EPDIR);
  } else {
    DEF_BIT_CLR(reg_val, DWCOTGFS_HCCHARx_EPDIR);
  }

  if (p_hcd_ep_params->DevSpd == USBH_DEV_SPD_LOW) {            // If LS dev, indicate that host ch comm to LS dev.
    DEF_BIT_SET(reg_val, DWCOTGFS_HCCHARx_LOW_SPEED_DEV);
  }
  //                                                               Set Multi Count field to 1 transaction. HC FS only.
  if ((p_hcd_ep_params->Type == USBH_EP_TYPE_INTR)
      || (p_hcd_ep_params->Type == USBH_EP_TYPE_ISOC)) {
    DEF_BIT_FIELD_WR(reg_val,
                     DWCOTGFS_HCCHARx_1_TRANSACTION,
                     DWCOTGFS_HCCHARx_MC_EC);
  }
  //                                                               Set dev addr to which the ch belongs.
  DEF_BIT_FIELD_WR(reg_val,
                   p_hcd_ep_params->DevAddr,
                   DWCOTGFS_HCCHARx_DAD_MSK);

  switch (p_hcd_ep_params->Type) {
    case USBH_EP_TYPE_CTRL:
      DEF_BIT_FIELD_WR(reg_val,
                       DWCOTGFS_HCCHARx_EP_TYPE_CTRL,
                       DWCOTGFS_HCCHARx_EP_TYPE_MSK);
      break;

    case USBH_EP_TYPE_INTR:
      DEF_BIT_FIELD_WR(reg_val,
                       DWCOTGFS_HCCHARx_EP_TYPE_INTR,
                       DWCOTGFS_HCCHARx_EP_TYPE_MSK);
      break;

    case USBH_EP_TYPE_BULK:
      DEF_BIT_FIELD_WR(reg_val,
                       DWCOTGFS_HCCHARx_EP_TYPE_BULK,
                       DWCOTGFS_HCCHARx_EP_TYPE_MSK);
      break;

    case USBH_EP_TYPE_ISOC:
    default:
      break;
  }

  p_reg->HCH[pipe_nbr].HCCHARx = reg_val;

  //                                                               ----------------- SET INITIAL PID ------------------
  //                                                               See Note #1.
  if (pid_token == USBH_TOKEN_SETUP) {
    pid_to_set = DWCOTGFS_HCTSIZx_PID_SETUP_MDATA;
  } else {
    pid_to_set = (pid_data_cur == USBH_PBHCI_PID_DATA0) ? DWCOTGFS_HCTSIZx_PID_DATA0 : DWCOTGFS_HCTSIZx_PID_DATA1;
  }

  p_reg->HCH[pipe_nbr].HCTSIZx = pid_to_set << 29u;

  //                                                               ------------------- INT SETTINGS -------------------
  DEF_BIT_CLR(p_reg->HAINTMSK, DEF_BIT(pipe_nbr));              // Dis top level int for this channel. Will be en/dis...
                                                                // at proper time during pipe tx/rx.
  CORE_ENTER_ATOMIC();
  //                                                               Clr any pending int for this channel.
  p_reg->HCH[pipe_nbr].HCINTx = DWCOTGFS_HCINTx_INT_MSK;
  DEF_BIT_CLR(p_reg->HAINT, DEF_BIT(pipe_nbr));

  //                                                               Unmask the necessary xfer int (see Note #2).
  p_reg->HCH[pipe_nbr].HCINTMSKx = (DWCOTGFS_HCINTx_XFRC
                                    | DWCOTGFS_HCINTx_STALL
                                    | DWCOTGFS_HCINTx_NAK
                                    | DWCOTGFS_HCINTx_TXERR
                                    | DWCOTGFS_HCINTx_DTERR
                                    | DWCOTGFS_HCINTx_FRMOR
                                    | DWCOTGFS_HCINTx_BBERR);

  DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_HCINT);            // Make sure Global Host Channel int is enabled.
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeTx()
 *
 * @brief    Prepare transmission for given pipe.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    p_buf           Pointer to start of buffer to send.
 *
 * @param    buf_len         Length of transfer, in bytes.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *
 * @return   Length that can be transmitted in a single transfer, in bytes.
 *
 * @note     (1) The DWC OTG-FS host controller supports transfer-based communication model. But some
 *               development tests showed that the host controller is able to transmit up to 64 bytes
 *               on the USB bus. If the transfer's size is greater than 64 bytes, the host controller
 *               does NOT send the bytes beyond 64 bytes. According to the datasheet, the channel must
 *               be re-enabled to continue the next 64-byte transmission. It implies to manage the
 *               interrupts "Periodic TxFIFO empty" and "Non-periodic TxFIFO empty". To avoid a
 *               certain interrupt handling complexity, the maximum transfer size will be kept to the
 *               maximum packet size (that will never exceed 64 bytes). This applies to transfer type
 *               other than isochronous.
 *******************************************************************************************************/
static CPU_INT32U USBH_PBHCD_PipeTx(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                    CPU_INT08U        pipe_nbr,
                                    CPU_INT08U        *p_buf,
                                    CPU_INT32U        buf_len,
                                    RTOS_ERR          *p_err)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        max_pkt_size = p_reg->HCH[pipe_nbr].HCCHARx & DWCOTGFS_HCCHARx_MPSIZ_MSK;
  CPU_INT32U        xfer_len;

  RTOS_ASSERT_DBG_ERR_SET((DEF_ADDR_IS_ALIGNED((CPU_INT32U)p_buf, CPU_WORD_SIZE_32) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  USBH_DWCOTGFS_DBG_STATS_INC(PipeTxCnt[pipe_nbr]);

  xfer_len = DEF_MIN(buf_len, max_pkt_size);                    // See Note #1.

  LOG_VRB(("PipeTx()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeTxStart()
 *
 * @brief    Start transmission on given pipe.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    p_buf           Pointer to start of buffer to send.
 *
 * @param    buf_len         Length of transfer, in bytes.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *
 * @note     (1) Endianness needs to be re-switched at driver level for the 4 registers related to a
 *               SETUP packet (USBREQ, USBVAL, USBINDX and USBLENG), since normally the core switches
 *               the endianness, but for these fields, the hardware also switches it.
 *
 * @note     (2) When reading the register HCTSIZx, the Data PID field is preserved. Only the number
 *               of packets composing the transfer and its size are configured. The data PID has
 *               already been correctly set for the next transaction by the host controller at the end
 *               of the previous transfer.
 *******************************************************************************************************/
static void USBH_PBHCD_PipeTxStart(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                   CPU_INT08U        pipe_nbr,
                                   CPU_INT08U        *p_buf,
                                   CPU_INT32U        buf_len,
                                   RTOS_ERR          *p_err)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U        ch_type;
  CPU_INT32U        hccharx_reg;
  CPU_INT32U        hctsizx_reg;
  CPU_INT32U        nbr_pkts;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(PipeTxStartCnt[pipe_nbr]);

  //                                                               -------- CFG ADDITIONAL CH CHARACTERISTICS ---------
  //                                                               Set frame parity for periodic xfers.
  ch_type = DEF_BIT_FIELD_RD(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_EP_TYPE_MSK);
  if ((ch_type == DWCOTGFS_HCCHARx_EP_TYPE_INTR)
      || (ch_type == DWCOTGFS_HCCHARx_EP_TYPE_ISOC)) {
    //                                                             Is cur frame nbr even?
    if (DEF_BIT_IS_CLR(p_reg->HFNUM, DEF_BIT_00) == DEF_YES) {
      //                                                           YES: perform xfer in an odd frame.
      DEF_BIT_SET(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_ODDFRM);
    } else {                                                    // NO: perform xfer in an even frame.
      DEF_BIT_CLR(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_ODDFRM);
    }
  }

  //                                                               ---------------- CFG XFER SIZE REG -----------------
  nbr_pkts = 1u;                                                // 1 pkt because transaction-based comm.
                                                                // See Note #2.
  hctsizx_reg = p_reg->HCH[pipe_nbr].HCTSIZx & DWCOTGFS_HCTSIZx_PID_MSK;
  DEF_BIT_SET(hctsizx_reg, ((nbr_pkts << 19u) | buf_len));
  p_reg->HCH[pipe_nbr].HCTSIZx = hctsizx_reg;

  //                                                               --------------------- CFG INT ----------------------
  DEF_BIT_SET(p_reg->HAINTMSK, DEF_BIT(pipe_nbr));              // Enable the top level host channel interrupt.

  //                                                               ------------------ ENABLE CHANNEL ------------------
  hccharx_reg = p_reg->HCH[pipe_nbr].HCCHARx;
  DEF_BIT_SET(hccharx_reg, DWCOTGFS_HCCHARx_CHENA);
  DEF_BIT_CLR(hccharx_reg, DWCOTGFS_HCCHARx_CHDIS);
  p_reg->HCH[pipe_nbr].HCCHARx = hccharx_reg;

  //                                                               ---------------- WRITE DATA TO FIFO ----------------
  if (buf_len > 0u) {
    CORE_ENTER_ATOMIC();
    USBH_DWCOTGFS_WrPkt(p_reg,
                        (CPU_INT32U *)p_buf,
                        pipe_nbr,
                        buf_len);
    CORE_EXIT_ATOMIC();
  }

  USBH_DWCOTGFS_DBG_STATS_SET(HCINTx_IntHostChOutTbl[pipe_nbr].XferInProgress, DEF_TRUE);
  LOG_VRB(("PipeTxStart()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeRxStart()
 *
 * @brief    Prepare for reception on give pipe.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    p_buf           Pointer to start of buffer to fill.
 *
 * @param    buf_len         Maximum length of transfer, in bytes.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *
 * @note     (1) For IN transfers, the field XFRSIZ of register HCTSIZx contains the buffer size that
 *               the application has reserved for the transfer. The application is expected to program
 *               this field as an integer multiple of the maximum size for IN transactions
 *               (periodic and non-periodic).
 *
 * @note     (2) When reading the register HCTSIZx, the Data PID field is preserved. Only the number
 *               of packets composing the transfer and its size are configured. The data PID has
 *               already been correctly set for the next transaction by the host controller at the end
 *               of the previous transfer.
 *
 * @note     (3) The OTG-FS controller has a shared Rx FIFO for ALL OUT channels. This FIFO cannot
 *               retain the data for a long period of time. Reading data must be done in the ISR
 *               context. Thus information about the receive buffer is saved internally to be
 *               available from the ISR.
 *******************************************************************************************************/
static void USBH_PBHCD_PipeRxStart(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                   CPU_INT08U        pipe_nbr,
                                   CPU_INT08U        *p_buf,
                                   CPU_INT32U        buf_len,
                                   RTOS_ERR          *p_err)
{
  USBH_DWCOTGFS_REG     *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_PBHCD_DATA       *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_CH_INFO *p_ch_info = &p_data->ChInfoTblPtr[pipe_nbr];
  CPU_INT32U            ch_type;
  CPU_INT32U            hccharx_reg;
  CPU_INT32U            hctsizx_reg;
  CPU_INT32U            nbr_pkts;
  CPU_INT32U            max_pkt_size;
  CPU_INT32U            max_xfer_chunk_len;
  CPU_INT32U            xfer_len;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((DEF_ADDR_IS_ALIGNED((CPU_INT32U)p_buf, CPU_WORD_SIZE_32) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBH_DWCOTGFS_DBG_STATS_INC(PipeRxStartCnt[pipe_nbr]);
  //                                                               -------- CFG ADDITIONAL CH CHARACTERISTICS ---------
  //                                                               Set frame parity for periodic xfers.
  ch_type = DEF_BIT_FIELD_RD(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_EP_TYPE_MSK);
  if ((ch_type == DWCOTGFS_HCCHARx_EP_TYPE_INTR)
      || (ch_type == DWCOTGFS_HCCHARx_EP_TYPE_ISOC)) {
    //                                                             Is cur frame nbr even?
    if (DEF_BIT_IS_CLR(p_reg->HFNUM, DEF_BIT_00) == DEF_YES) {
      //                                                           YES: perform xfer in an odd (micro)frame.
      DEF_BIT_SET(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_ODDFRM);
    } else {                                                    // NO: perform xfer in an even (micro)frame.
      DEF_BIT_CLR(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_ODDFRM);
    }
  }
  //                                                               ---------------- CFG XFER SIZE REG -----------------
  max_pkt_size = p_reg->HCH[pipe_nbr].HCCHARx & DWCOTGFS_HCCHARx_MPSIZ_MSK;

  max_xfer_chunk_len = DEF_MIN(buf_len, max_pkt_size);
  nbr_pkts = 1u;                                                // 1 pkt because transaction-based comm.
  xfer_len = nbr_pkts * max_pkt_size;                           // See Note #1.
                                                                // See Note #2.
  hctsizx_reg = p_reg->HCH[pipe_nbr].HCTSIZx & DWCOTGFS_HCTSIZx_PID_MSK;
  DEF_BIT_SET(hctsizx_reg, ((nbr_pkts << 19u) | xfer_len));
  p_reg->HCH[pipe_nbr].HCTSIZx = hctsizx_reg;

  //                                                               -------- SAVE SOME INFO FOR ISR PROCESSING ---------
  //                                                               See Note #3.
  p_ch_info->RxAppBufPtr = p_buf;
  p_ch_info->RxAppBufLen = max_xfer_chunk_len;
  //                                                               --------------------- CFG INT ----------------------
  DEF_BIT_SET(p_reg->HAINTMSK, DEF_BIT(pipe_nbr));              // Enable the top level host channel interrupt.

  if (ch_type == DWCOTGFS_HCCHARx_EP_TYPE_INTR) {
    CORE_ENTER_ATOMIC();
    DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_IPXFR);          // Enable incomplete periodic transfer interrupt.
    CORE_EXIT_ATOMIC();
  }
  //                                                               ------------------ ENABLE CHANNEL ------------------
  hccharx_reg = p_reg->HCH[pipe_nbr].HCCHARx;
  DEF_BIT_SET(hccharx_reg, DWCOTGFS_HCCHARx_CHENA);
  DEF_BIT_CLR(hccharx_reg, DWCOTGFS_HCCHARx_CHDIS);
  p_reg->HCH[pipe_nbr].HCCHARx = hccharx_reg;

  USBH_DWCOTGFS_DBG_STATS_SET(HCINTx_IntHostChInTbl[pipe_nbr].XferInProgress, DEF_TRUE);
  LOG_VRB(("PipeRxStart()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeRx()
 *
 * @brief    Execute reception/copy of data received.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr        Pipe number.
 *
 * @param    p_buf           Pointer to start of buffer to fill.
 *
 * @param    buf_len         Maximum length of transfer, in bytes.
 *
 * @param    p_status        Pointer to variable to hold any flag associated with transfer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code :
 *                               - RTOS_ERR_NONE       Operation was successful.
 *                               - RTOS_ERR_RX_OVERRUN Received too much data for buffer length.
 *
 * @return   Actual number of bytes received.
 *
 * @note     (1) The OTG-FS controller has a shared Rx FIFO for ALL OUT channels. This FIFO cannot
 *               retain the data for a long period of time. An interrupt is generated each time
 *               a IN data packet is received. Reading data must be done in the ISR context. When
 *               the PBHCI task calls HCD PipeRx() function, the data is already stored in the
 *               user buffer.
 *
 * @note     (2) See 'DWCOTGFS_ISR_RxFIFONonEmpty() Note #1 and #2'.
 *******************************************************************************************************/
static CPU_INT32U USBH_PBHCD_PipeRx(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                    CPU_INT08U        pipe_nbr,
                                    CPU_INT08U        *p_buf,
                                    CPU_INT32U        buf_len,
                                    CPU_INT08U        *p_status,
                                    RTOS_ERR          *p_err)
{
  USBH_PBHCD_DATA       *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_CH_INFO *p_ch_info = &p_data->ChInfoTblPtr[pipe_nbr];
  CPU_INT32U            rx_len = 0u;
  CPU_INT32U            data_len;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);

  USBH_DWCOTGFS_DBG_STATS_INC(PipeRxCnt[pipe_nbr]);

  CORE_ENTER_ATOMIC();
  rx_len = p_ch_info->RxXferLen;                                // Rx data processed in ISR context (see Note #1).
  data_len = p_ch_info->RxXferCopyLen;                          // See Note #2.
                                                                // Reset for next xfer.
  p_ch_info->RxXferLen = 0u;
  p_ch_info->RxXferCopyLen = 0u;
  CORE_EXIT_ATOMIC();

  if (rx_len > data_len) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX_OVERRUN);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  *p_status = USBH_PBHCI_XFER_STATUS_NONE;

  LOG_VRB(("PipeRx()"));

  return (data_len);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_PipeAbort()
 *
 * @brief       Abort any transfer on given pipe.
 *
 * @param       p_pbhci_hc_drv      Pointer to PBHCI driver structure.
 *
 * @param    pipe_nbr            Pipe number.
 *
 * @param    p_err               Pointer to variable that will receive the return error code :
 *                                   - RTOS_ERR_NONE   Operation was successful.
 *                                   - RTOS_ERR_IO     Failed to wait for operation end.
 *
 * @note        (1) The OTG-FS controller provides 2 shared Tx FIFOs, one for non-periodic pipes and one
 *               for periodic pipes. When aborting a given OUT pipe, the non-periodic or periodic Tx
 *               FIFO will be flushed entirely. There is no way to flush selectively. Thus other
 *               scheduled OUT transfers may be flushed. Hence, the PipeAbort() function must halt
 *               also these other OUT pipes. The Channel Halted interrupt will be fired for each
 *               additional OUT pipe halted. The Channel Halted interrupt ISR will restart the
 *               additional aborted transfers by notifying the PBHCI task about NAKed transfers.
 *******************************************************************************************************/
static void USBH_PBHCD_PipeAbort(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                 CPU_INT08U        pipe_nbr,
                                 RTOS_ERR          *p_err)
{
  USBH_DWCOTGFS_REG     *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_PBHCD_DATA       *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  USBH_DWCOTGFS_CH_INFO *p_ch_info = &p_data->ChInfoTblPtr[pipe_nbr];
  CPU_INT32U            ch_type = DEF_BIT_FIELD_RD(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_EP_TYPE_MSK);
  CPU_BOOLEAN           ch_dir = (CPU_BOOLEAN)(DEF_BIT_FIELD_RD(p_reg->HCH[pipe_nbr].HCCHARx, DWCOTGFS_HCCHARx_EPDIR));
  CPU_INT08U            tx_fifo_nbr;
  CPU_INT08U            tx_fifo_type;
  CPU_INT16U            retry_cnt;
  CPU_BOOLEAN           ch_halt_status;
  CPU_INT08U            i;
  CPU_INT32U            hcchar_val;
  CORE_DECLARE_IRQ_STATE;

  USBH_DWCOTGFS_DBG_STATS_INC(PipeAbortCnt[pipe_nbr]);

  //                                                               --------------- WAIT FOR CH HALT END ---------------
  retry_cnt = 0u;
  CORE_ENTER_ATOMIC();
  ch_halt_status = p_ch_info->Halting;
  CORE_EXIT_ATOMIC();
  while ((ch_halt_status == DEF_TRUE)
         && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
    retry_cnt++;
    CORE_ENTER_ATOMIC();
    ch_halt_status = p_ch_info->Halting;
    CORE_EXIT_ATOMIC();
  }
  if (retry_cnt == DWCOTGFS_MAX_RETRY) {
    LOG_ERR(("Waiting for channel halt end in PBHCD_PipeAbort().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  if (ch_dir == DEF_FALSE) {                                    // ------------- FLUSH OUT FIFO & HALT CH -------------
    if ((ch_type == DWCOTGFS_HCCHARx_EP_TYPE_CTRL)
        || (ch_type == DWCOTGFS_HCCHARx_EP_TYPE_BULK)) {
      tx_fifo_nbr = DWCOTGFS_GRSTCTL_NONPER_TXFIFO;             // Flush non-periodic Tx FIFO.
    } else {
      tx_fifo_nbr = DWCOTGFS_GRSTCTL_PER_TXFIFO;                // Flush periodic Tx FIFO.
    }

    p_reg->GRSTCTL = (DWCOTGFS_GRSTCTL_TXFFLUSH
                      | (tx_fifo_nbr << 6u));
    retry_cnt = 0u;                                             // Wait for the complete FIFO flushing.
    while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_TXFFLUSH) == DEF_YES)
           && (retry_cnt < DWCOTGFS_MAX_RETRY)) {
      retry_cnt++;
    }
    if (retry_cnt == DWCOTGFS_MAX_RETRY) {
      LOG_ERR(("Waiting for complete Tx FIFO flushing in PBHCD_PipeAbort().", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    CORE_ENTER_ATOMIC();
    p_ch_info->Aborted = DEF_TRUE;                              // Indicate ch aborted.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_ABORT;

    DEF_BIT_SET(p_reg->HAINTMSK, DEF_BIT(pipe_nbr));            // En top level int for this ch to process halt int.
    USBH_DWCOTGFS_ChHalt(p_reg, pipe_nbr);                      // first halt required ch to be aborted.
    CORE_EXIT_ATOMIC();

    for (i = 0u; i < p_data->ChQty; i++) {                      // See Note #1.
      CORE_ENTER_ATOMIC();
      hcchar_val = p_reg->HCH[i].HCCHARx;
      tx_fifo_type = (CPU_INT08U)((hcchar_val & DEF_BIT_18) >> 18u);
      //                                                           Determine which pipes OUT to halt besides initial...
      //                                                           ...pipe_nbr.
      if ((DEF_BIT_IS_SET(hcchar_val, DWCOTGFS_HCCHARx_CHENA) == DEF_YES)
          && (DEF_BIT_IS_CLR(hcchar_val, DWCOTGFS_HCCHARx_CHDIS) == DEF_YES)
          && (DEF_BIT_IS_CLR(hcchar_val, DWCOTGFS_HCCHARx_EPDIR) == DEF_YES)
          && (tx_fifo_type == tx_fifo_nbr)
          && (i != pipe_nbr)) {
        USBH_DWCOTGFS_DBG_STATS_INC(PipeAbortCnt[i]);

        p_ch_info = &p_data->ChInfoTblPtr[i];
        p_ch_info->Aborted = DEF_TRUE;
        p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_ABORT;
        p_ch_info->RestartComm = DEF_TRUE;

        DEF_BIT_SET(p_reg->HAINTMSK, DEF_BIT(i));
        USBH_DWCOTGFS_ChHalt(p_reg, i);
      }
      CORE_EXIT_ATOMIC();
    }
  } else {                                                      // -------------------- HALT IN CH --------------------
    CORE_ENTER_ATOMIC();
    p_ch_info->Aborted = DEF_TRUE;
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_ABORT;

    DEF_BIT_SET(p_reg->HAINTMSK, DEF_BIT(pipe_nbr));
    USBH_DWCOTGFS_ChHalt(p_reg, pipe_nbr);
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("PipeAbort()"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_SOF_IntEn()
 *
 * @brief    Enable or disable start-of-frame (SOF) interrupt.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    en              Flag used to indicate if interrupt needs to be enabled or disabled:
 *                           DEF_ENABLED         SOF interrupt needs to be  enabled.
 *                           DEF_DISABLED        SOF interrupt needs to be disabled.
 *
 * @return   DEF_OK,     If SOF interrupt was set successfully.
 *           DEF_FAIL,   Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_PBHCD_SOF_IntEn(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                        CPU_BOOLEAN       en)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (en == DEF_ENABLED) {
    DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_SOF);
  } else {
    DEF_BIT_CLR(p_reg->GINTMSK, DWCOTGFS_GINTx_SOF);
  }
  CORE_EXIT_ATOMIC();

  LOG_VRB(("SOF_IntEn()"));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_PBHCD_ISR_Handle()
 *
 * @brief    USB interrupt handler.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *******************************************************************************************************/
static void USBH_PBHCD_ISR_Handle(USBH_PBHCI_HC_DRV *p_pbhci_hc_drv)
{
  USBH_DWCOTGFS_REG *p_reg = (USBH_DWCOTGFS_REG *)p_pbhci_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_PBHCD_DATA   *p_data = (USBH_PBHCD_DATA *)p_pbhci_hc_drv->DataPtr;
  CPU_INT16U        ch_nbr;
  CPU_INT32U        ch_int;
  CPU_INT32U        gintsts_reg;
  CPU_INT32U        reg_val;

  gintsts_reg = p_reg->GINTSTS;
  gintsts_reg &= p_reg->GINTMSK;                                // Keep only the unmasked interrupts.

  //                                                               ------------------ START OF FRAME ------------------
  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_SOF) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntSofCnt);

    p_reg->GINTSTS = DWCOTGFS_GINTx_SOF;                        // Acknowledge interrupt by a write clear.

    reg_val = p_reg->HFNUM & USBH_FRM_NBR_MASK;
    USBH_PBHCI_EventSOF(p_pbhci_hc_drv, reg_val);
  }
  //                                                               ------------------- DISCONN INT --------------------
  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_DISCINT) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntDiscCnt);

    USBH_DWCOTGFS_ISR_PortDisconn(p_reg,
                                  p_pbhci_hc_drv,
                                  p_data);
  }
  //                                                               -------------- PORT STATUS CHANGE INT --------------
  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_HPRTINT) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntHprtCnt);

    USBH_DWCOTGFS_ISR_PortConn(p_reg,
                               p_pbhci_hc_drv,
                               p_data);
  }
  //                                                               ----------------- EP TRANSFER INT ------------------
  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_RXFLVL) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntRxflvlCnt);
    USBH_DWCOTGFS_ISR_RxFIFONonEmpty(p_reg, p_data);            // Handle the RxFIFO Non-Empty interrupt.
  }

  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_HCINT) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntHcIntCnt);

    ch_int = (p_reg->HAINT & DWCOTGFS_HAINT_CH_INT_MSK);        // Read HAINT reg to serve all the active ch int.
    ch_nbr = CPU_CntTrailZeros(ch_int);
    reg_val = p_reg->HCH[ch_nbr].HCCHARx;

    while (ch_int != 0u) {                                      // Handle Host channel interrupt.
      if ((reg_val & DWCOTGFS_HCCHARx_EPDIR) == 0u) {           // OUT Channel.
        USBH_DWCOTGFS_ISR_HostChOUT(p_reg,
                                    p_pbhci_hc_drv,
                                    p_data,
                                    ch_nbr);
        //                                                         IN Channel.
      } else if ((reg_val & DWCOTGFS_HCCHARx_EPDIR) != 0u) {
        USBH_DWCOTGFS_ISR_HostChIN(p_reg,
                                   p_pbhci_hc_drv,
                                   p_data,
                                   ch_nbr);
      }

      DEF_BIT_CLR(ch_int, DEF_BIT(ch_nbr));
      //                                                           Read HAINT reg to serve all the active ch int.
      ch_int = (p_reg->HAINT & DWCOTGFS_HAINT_CH_INT_MSK);
      ch_nbr = CPU_CntTrailZeros(ch_int);
      reg_val = p_reg->HCH[ch_nbr].HCCHARx;
    }
  }

  if (DEF_BIT_IS_SET(gintsts_reg, DWCOTGFS_GINTx_IPXFR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(GINTSTS_IntIpxferCnt);

    p_reg->GINTSTS = DWCOTGFS_GINTx_IPXFR;                      // Acknowledge interrupt by a write clear.
    DEF_BIT_CLR(p_reg->GINTMSK, DWCOTGFS_GINTx_IPXFR);          // Mask Incomplete Periodic Transfer interrupt.

    ch_nbr = DEF_BIT_FIELD_RD(p_reg->HPTXSTS, DWCOTGFS_HPTXSTS_CH_EP_NBR);
    DEF_BIT_SET(p_reg->HCH[0].HCCHARx, DWCOTGFS_HCCHARx_ODDFRM);
  }
}

/****************************************************************************************************//**
 *                                    USBH_DWCOTGFS_ISR_PortConn()
 *
 * @brief    Handle Host Port interrupt.
 *
 * @param    p_reg           Pointer to registers structure.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_data          Pointer to driver private data structure.
 *
 * @note     (1) This case is a special case when using an USB traffic analyzer. The core generates
 *               an erroneous connection detected interrupt while there is no device connected to the
 *               Host port.
 *
 * @note     (2) When the device is attached to the port. FIRST, the device connection interrupt is
 *               detected and launches a complete reset sequence on the port. THEN, the port enabled
 *               change bit in HPRT register triggers another device connection interrupt and the
 *               PHY clock selection is adjusted to limit the power according to the speed device.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ISR_PortConn(USBH_DWCOTGFS_REG *p_reg,
                                       USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       USBH_PBHCD_DATA   *p_data)
{
  CPU_INT32U hprt_reg;
  CPU_INT32U hprt_reg_dup;
  CPU_INT32U dev_speed;

  PP_UNUSED_PARAM(p_data);

  hprt_reg = p_reg->HPRT;
  hprt_reg_dup = p_reg->HPRT;
  //                                                               To avoid clearing some important interrupts.
  DWCOTGFS_HPRT_BITS_CLR_PRESERVE(hprt_reg_dup);                // To avoid clearing some important bits.

  //                                                               -------------- PORT CONNECT DETECTED ---------------
  if ((DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_CONN_DET) == DEF_YES)
      && (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_CONN_STS) == DEF_YES)) {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntPortConnDetectedCnt);

    DEF_BIT_SET(hprt_reg_dup, DWCOTGFS_HPRT_PORT_CONN_DET);     // Prepare int ack.

    USBH_PBHCI_EventPortConn(p_pbhci_hc_drv, 1u);               // Signal connection on port #1.

    DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_DISCINT);        // Enable the Disconnect interrupt.

    //                                                             See Note #1.
  } else if (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_CONN_DET) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntWrongPortConnDetectedCnt);

    DEF_BIT_SET(hprt_reg_dup, DWCOTGFS_HPRT_PORT_CONN_DET);     // Prepare int ack.
  }
  //                                                               ------------ PORT ENABLE/DISABLE CHANGE ------------
  //                                                               See Note #2.
  if ((DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_EN_CHNG) == DEF_YES)
      && (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_CONN_STS) == DEF_YES)) {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntPortEnDisChngCnt);

    DEF_BIT_SET(hprt_reg_dup, DWCOTGFS_HPRT_PORT_EN_CHNG);      // Prepare int ack.

    //                                                             Is port en after reset sequence due to dev conn?
    if (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_EN) == DEF_YES) {
      DEF_BIT_CLR(p_reg->HCFG, DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL);

      dev_speed = DEF_BIT_FIELD_RD(p_reg->HPRT, DWCOTGFS_HPRT_PORT_SPEED);
      if (dev_speed == DWCOTGFS_HPRT_PORT_SPD_FS) {
        //                                                         Ensure PHY clock = 48 MHz for FS.
        DEF_BIT_SET(p_reg->HCFG, DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL_48MHz);
        p_reg->HFIR = 48000u;
      } else if (dev_speed == DWCOTGFS_HPRT_PORT_SPD_LS) {
        //                                                         Ensure PHY clock = 6 MHz for LS.
        DEF_BIT_SET(p_reg->HCFG, DWCOTGFS_HCFG_FS_LS_PHYCLK_SEL_6MHz);
        p_reg->HFIR = 6000u;
      }
    }
  } else if (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_EN_CHNG) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntPortEnDisChng_NoDevConn_Cnt);

    DEF_BIT_SET(hprt_reg_dup, DWCOTGFS_HPRT_PORT_EN_CHNG);      // Prepare int ack.
  }

  //                                                               ---------------- OVERCURRENT CHANGE ----------------
  if (DEF_BIT_IS_SET(hprt_reg, DWCOTGFS_HPRT_PORT_OCCHNG) == DEF_YES) {
    CPU_BOOLEAN set;

    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntOvercurrentChngCnt);

    DEF_BIT_SET(hprt_reg_dup, DWCOTGFS_HPRT_PORT_OCCHNG);       // Prepare int ack.

    set = (DEF_BIT_IS_SET(p_reg->HPRT, DWCOTGFS_HPRT_PORT_OC) == DEF_YES) ? DEF_SET : DEF_CLR;
    //                                                             Signal an overcurrent on root hub.
    USBH_PBHCI_EventPortOvercurrent(p_pbhci_hc_drv,
                                    0u,
                                    set);
  }

  p_reg->HPRT = hprt_reg_dup;                                   // Ack serviced int.
}

/****************************************************************************************************//**
 *                                   USBH_DWCOTGFS_ISR_PortDisconn()
 *
 * @brief    Handle Disconnect Detected Interrupt.
 *
 * @param    p_reg           Pointer to registers structure.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_data          Pointer to driver private data structure.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ISR_PortDisconn(USBH_DWCOTGFS_REG *p_reg,
                                          USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                          USBH_PBHCD_DATA   *p_data)
{
  CPU_INT08U i;
  CPU_INT32U reg_val;

  DEF_BIT_SET(p_reg->GINTSTS, DWCOTGFS_GINTx_DISCINT);          // Ack Device Disconnected interrupt by a write clr.

  reg_val = p_reg->HPRT;
  //                                                               No device is attached to the port.
  if (DEF_BIT_IS_CLR(reg_val, DWCOTGFS_HPRT_PORT_CONN_STS) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntDisconnCnt);

    DEF_BIT_CLR(p_reg->GINTMSK, DWCOTGFS_GINTx_DISCINT);        // Dis the Disconnect interrupt.
    DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_HPRTINT);        // En Root hub port int to detect a new device conn.

    //                                                             ---------------- RESET HOST CH REG -----------------
    for (i = 0u; i < p_data->ChQty; i++) {
      p_reg->HCH[i].HCINTMSKx = 0u;                             // Mask all int.
      p_reg->HCH[i].HCINTx = 0xFFFFFFFFu;                       // Clr any pending int.
      p_reg->HCH[i].HCTSIZx = 0u;

      reg_val = p_reg->HCH[i].HCCHARx;
      if (DEF_BIT_IS_SET(reg_val, DWCOTGFS_HCCHARx_CHENA) == DEF_YES) {
        USBH_DWCOTGFS_ChHalt(p_reg, i);                         // Halt the channel if enabled.
      } else {
        p_reg->HCH[i].HCCHARx = 0u;                             // Reset the HCCHARx reg.
      }
    }
    //                                                             --------------- FLUSH TX AND RX FIFO ---------------
    p_reg->GRSTCTL = (DWCOTGFS_GRSTCTL_TXFFLUSH
                      | (DWCOTGFS_GRSTCTL_ALL_TXFIFO << 6u));   // Flush All Tx FIFOs.
    reg_val = DWCOTGFS_MAX_RETRY;                               // Wait for the complete FIFO flushing.
    while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_TXFFLUSH))
           && (reg_val > 0u)) {
      reg_val--;
    }
    if (reg_val == 0u) {
      return;
    }

    p_reg->GRSTCTL = DWCOTGFS_GRSTCTL_RXFFLUSH;                 // Flush the entire RxFIFO.
    reg_val = DWCOTGFS_MAX_RETRY;                               // Wait for the complete FIFO flushing.
    while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, DWCOTGFS_GRSTCTL_RXFFLUSH))
           && (reg_val > 0u)) {
      reg_val--;
    }
    if (reg_val == 0u) {
      return;
    }
    //                                                             --------------------- SW RESET ---------------------
    USBH_DWCOTGFS_SW_Reset(p_data);                             // Init some global var for the new dev conn.
                                                                // ----------------- SIGNAL HUB TASK ------------------
    USBH_PBHCI_EventPortDisconn(p_pbhci_hc_drv, 1u);            // Signal device disconn from root hub.
  } else {
    USBH_DWCOTGFS_DBG_STATS_INC(HPRT_IntWrongDisconnCnt);
  }
}

/****************************************************************************************************//**
 *                                 USBH_DWCOTGFS_ISR_RxFIFONonEmpty()
 *
 * @brief    Handle Receive FIFO Non-Empty interrupt generated when a data IN packet has been received.
 *
 * @param    p_reg   Pointer to registers structure.
 *
 * @param    p_data  Pointer to driver private data structure.
 *
 * @note     (1) The size for an IN transfer in the register HCTSIZx must be configured as an integer
 *               multiple of the maximum packet size. If the application buffer size is less than
 *               the maximum packet size and an overrun situation occurs, the host controller accepts
 *               the additional data in the shared Rx FIFO. It does not detect the overrun situation.
 *               In that case, the host controller will report via the register GRXSTS a received
 *               transfer size equal to the overrun transfer size. The macro DEF_MIN() here ensures
 *               that received data will be copied in the application buffer up to the maximum buffer
 *               length.
 *
 * @note     (2) In the situation described in Note #1, the additional data must be read from the Rx
 *               FIFO to properly generate the Transfer Complete interrupt. The overrun situation
 *               will be detected in the PipeRx() function.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ISR_RxFIFONonEmpty(USBH_DWCOTGFS_REG *p_reg,
                                             USBH_PBHCD_DATA   *p_data)
{
  USBH_DWCOTGFS_CH_INFO *p_ch_info;
  CPU_INT32U            *p_buf32;
  CPU_INT08U            *p_buf8;
  CPU_INT32U            last_word;
  CPU_INT08U            ch_nbr;
  CPU_INT32U            reg_val;
  CPU_INT32U            total_byte_cnt;
  CPU_INT32U            byte_to_copy_cnt;
  CPU_INT32U            word_cnt;
  CPU_INT32U            byte_cnt;
  CPU_INT32U            pkt_stat;
  CPU_INT32U            i;

  DEF_BIT_CLR(p_reg->GINTMSK, DWCOTGFS_GINTx_RXFLVL);           // (1)-- Mask RxFLvl interrupt

  //                                                               (2)-- Handle Packet Status bits in GRXSTSP.
  reg_val = p_reg->GRXSTSP;
  ch_nbr = reg_val & DWCOTGFS_GRXSTS_CHNUM_MSK;
  total_byte_cnt = DEF_BIT_FIELD_RD(reg_val, DWCOTGFS_GRXSTS_BCNT_MSK);
  pkt_stat = DEF_BIT_FIELD_RD(reg_val, DWCOTGFS_GRXSTS_PKTSTS_MSK);

  p_ch_info = &p_data->ChInfoTblPtr[ch_nbr];
  p_buf32 = (CPU_INT32U *)p_ch_info->RxAppBufPtr;

  switch (pkt_stat) {
    case DWCOTGFS_GRXSTS_PKTSTS_IN:                             // An IN data packet has been received.
      USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].PktstsInCnt);
      byte_to_copy_cnt = DEF_MIN(total_byte_cnt,                // See Note #1.
                                 p_ch_info->RxAppBufLen);
      word_cnt = byte_to_copy_cnt / CPU_WORD_SIZE_32;
      byte_cnt = byte_to_copy_cnt % CPU_WORD_SIZE_32;

      if (byte_to_copy_cnt > 0u) {
        for (i = 0u; i < word_cnt; i++) {                       // Read rx'ed IN data pkt using 32-bit access.
          p_buf32[i] = *p_reg->DFIFO[0u].DATA;
        }

        if (byte_cnt > 0u) {                                    // Read the remaining word using 8-bit access.
          p_buf8 = (CPU_INT08U *)&p_buf32[i];
          last_word = *p_reg->DFIFO[0u].DATA;

          USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].LastWordFor8BitAccessCnt);

          for (i = 0u; i < byte_cnt; i++) {
            p_buf8[i] = (last_word >> (8u * i)) & 0x000000FFu;
          }
        }

        if (byte_to_copy_cnt < total_byte_cnt) {                // Overrun situation.
          USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].OverrunCnt);
          //                                                       See Note #1.
          word_cnt = ((total_byte_cnt - byte_to_copy_cnt) + 3u) / CPU_WORD_SIZE_32;

          for (i = 0u; i < word_cnt; i++) {
            CPU_INT32U dummy_word;

            dummy_word = *p_reg->DFIFO[0u].DATA;
            (void)&dummy_word;
          }
        }

        p_ch_info->RxXferLen += total_byte_cnt;
        p_ch_info->RxXferCopyLen += byte_to_copy_cnt;
      }
      break;

    case DWCOTGFS_GRXSTS_PKTSTS_IN_XFER_COMP:
      USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].PktstsInXferCmplCnt);
      break;

    case DWCOTGFS_GRXSTS_PKTSTS_DATA_TOGGLE_ERR:
      USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].PktstsDataToggleErrCnt);
      break;

    case DWCOTGFS_GRXSTS_PKTSTS_CH_HALTED:
      USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].PktstsChHaltedCnt);
      break;

    default:
      USBH_DWCOTGFS_DBG_STATS_INC(GRXSTS_IntRxFIFONonEmptyTbl[ch_nbr].PktstsReservedCnt);
      break;                                                    // Handled in interrupt, just ignore data.
  }

  DEF_BIT_SET(p_reg->GINTMSK, DWCOTGFS_GINTx_RXFLVL);           // (3)-- Unmask RxFLvl interrupt
}

/****************************************************************************************************//**
 *                                           USBH_DWCOTGFS_SW_Reset()
 *
 * @brief    Reset some global variables for the new connection of the device.
 *
 * @param    p_data  Pointer to driver private data structure.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_SW_Reset(USBH_PBHCD_DATA *p_data)
{
  Mem_Clr((void *)p_data->ChInfoTblPtr,
          (p_data->ChQty * sizeof(USBH_DWCOTGFS_CH_INFO)));
}

/****************************************************************************************************//**
 *                                        USBH_DWCOTGFS_WrPkt()
 *
 * @brief    Writes a packet into the Tx FIFO associated with the channel.
 *
 * @param    p_reg   Pointer to registers structure.
 *
 * @param    p_src   Pointer to data that will be transmitted.
 *
 * @param    ch_nbr  Host Channel OUT number.
 *
 * @param    bytes   Number of bytes to write.
 *
 * @note     (1) Find the DWORD length, padded by extra bytes as necessary if maximum packet size
 *               is not a multiple of 32-bit word.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_WrPkt(USBH_DWCOTGFS_REG *p_reg,
                                CPU_INT32U        *p_src,
                                CPU_INT08U        ch_nbr,
                                CPU_INT16U        bytes)
{
  CPU_REG32  *p_fifo;
  CPU_INT32U *p_data_buff;
  CPU_INT32U i;
  CPU_INT32U dword_count;

  dword_count = (bytes + 3u) / CPU_WORD_SIZE_32;                // See Note #1.

  p_fifo = p_reg->DFIFO[ch_nbr].DATA;
  p_data_buff = (CPU_INT32U *)p_src;

  for (i = 0u; i < dword_count; i++, p_data_buff++) {
    *p_fifo = *p_data_buff;
  }
}

/****************************************************************************************************//**
 *                                       USBH_DWCOTGFS_ISR_HostChOUT()
 *
 * @brief    Handle all the interrupts related to an OUT transaction (success and errors)
 *
 * @param    p_reg           Pointer to registers structure.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_data          Pointer to driver private data structure.
 *
 * @param    ch_nbr          Host Channel OUT number.
 *
 * @note     (1) The OTG-FS reference manual indicates different non-ACK handling according to the
 *               transfer type. The Host Channel OUT ISR gathers all non-ACK responses in a single
 *               ISR to ease the processing. The table below shows the non-ACK responses that should
 *               be handled according to the transfer type as indicated in the reference manual.
 *               @verbatim
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               | Transfer Type | STALL | NAK | TXERR | BBERR | FRMOR | DTERR |
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               | Control/Bulk  | yes   | yes | yes   | -     | -     | -     |
 *               | Interrupt     | yes   | yes | yes   | -     | yes   | -     |
 *               | Isochronous   | -     | -   | -     | -     | yes   | -     |
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               @endverbatim
 *               The OTG-FS needs most of the time to halt the channel to complete a OUT transfer.
 *               Below is presented the different interrupts generation paths according to the transfer
 *               completing with and without error.
 *               @n
 *               Transfer without error:
 *               @verbatim
 *               XFRC -> USBH_PBHCI_EventPipeTxCmpl()
 *               @endverbatim
 *               Transfer with error:
 *               @verbatim
 *               STALL -> CHH -> USBH_PBHCI_EventPipeTxCmpl()
 *               NAK   -> CHH -> USBH_PBHCI_EventPipeTxCmpl()
 *               TXERR -> CHH -> USBH_PBHCI_EventPipeTxCmpl()
 *               FRMOR -> CHH -> USBH_PBHCI_EventPipeTxCmpl()
 *               @endverbatim
 *
 * @note     (2) The Transaction Error interrupt indicates that one of the following errors occurred on
 *               the USB:
 *                   - CRC check failure
 *                   - Timeout
 *                   - Bit stuff error
 *                   - False EOP
 *
 * @note     (3) In case of transfer aborted, this code prevents the ISR from notifying the PBHCI task
 *               of a transfer completion with error after the pipe abortion.
 *
 * @note     (4) The OTG-FS controller does not support hardware retransmission if the current
 *               transaction for a control, bulk or interrupt has been NAKed or a Transaction Error
 *               has been detected. Hence, retransmission is managed by the PBHCI task.
 *
 * @note     (5) See 'USBH_PBHCD_PipeAbort() Note #1'.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ISR_HostChOUT(USBH_DWCOTGFS_REG *p_reg,
                                        USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                        USBH_PBHCD_DATA   *p_data,
                                        CPU_INT08U        ch_nbr)
{
  USBH_DWCOTGFS_CH_INFO *p_ch_info = &p_data->ChInfoTblPtr[ch_nbr];
  CPU_BOOLEAN           halt_ch = DEF_FALSE;
  CPU_INT32U            hcint_reg;
  RTOS_ERR              err;

  hcint_reg = p_reg->HCH[ch_nbr].HCINTx;
  hcint_reg &= (p_reg->HCH[ch_nbr].HCINTMSKx);

  USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntHostChCnt);
  //                                                               ---------- SUCCESSFUL TRANSFER COMPLETION ----------
  if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_XFRC) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntXferCmplCnt);
    USBH_DWCOTGFS_DBG_STATS_SET(HCINTx_IntHostChOutTbl[ch_nbr].XferInProgress, DEF_FALSE);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_XFRC;           // Ack int.
    p_ch_info->CurXferErrCnt = 0u;                              // Reset error count.
    RTOS_ERR_SET(err, RTOS_ERR_NONE);

    USBH_PBHCI_EventPipeTxCmpl(p_pbhci_hc_drv,
                               ch_nbr,
                               USBH_PBHCI_XFER_LEN_ALL,
                               USBH_PBHCI_XFER_STATUS_NONE,
                               err);

    //                                                             ---------------------- ERROR -----------------------
    //                                                             STALL Response Received int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_STALL) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntStallCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_STALL;          // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_STALL;
    halt_ch = DEF_TRUE;
    //                                                             NAK Response Received int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_NAK) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntNakCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_NAK;            // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NAK;
    p_ch_info->CurXferErrCnt = 0u;                              // Reset err cnt.
    halt_ch = DEF_TRUE;

    //                                                             Transaction Error int (see Note #2).
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_TXERR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntTxErrCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_TXERR;          // Ack int.
    halt_ch = DEF_TRUE;

    p_ch_info->CurXferErrCnt++;                                 // Increment err cnt in case of transaction err.
    if (p_ch_info->CurXferErrCnt < 3u) {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NAK;        // Notify core to retry xfer.
    } else {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_TXERR;      // Notify core to abort xfer.
    }
    //                                                             Unmask ACK int.
    DEF_BIT_SET(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_ACK);

    //                                                             ------------------ CHANNEL HALTED ------------------
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_CH_HALTED) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhCnt);
    USBH_DWCOTGFS_DBG_STATS_SET(HCINTx_IntHostChOutTbl[ch_nbr].XferInProgress, DEF_FALSE);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_CH_HALTED;      // Ack int.
                                                                // Mask ch halted int.
    DEF_BIT_CLR(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_CH_HALTED);

    if ((p_ch_info->Aborted == DEF_TRUE)                        // See Note #3.
        && (p_ch_info->HaltSrc != DWCOTGFS_HCINTx_HALT_SRC_ABORT)) {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NONE;
      USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChh_ChAborted);
    }

    switch (p_ch_info->HaltSrc) {
      case DWCOTGFS_HCINTx_HALT_SRC_STALL:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhSrc_STALL);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_EP_STALL);

        USBH_PBHCI_EventPipeTxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   0u,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_TXERR:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhSrc_TXERR);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_IO);

        USBH_PBHCI_EventPipeTxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   0u,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_NAK:                        // Retransmit xfer on channel (see Note #4).
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhSrc_NAK);
        RTOS_ERR_SET(err, RTOS_ERR_NONE);

        USBH_PBHCI_EventPipeTxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   0u,
                                   USBH_PBHCI_XFER_STATUS_NACK,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_ABORT:
        if (p_ch_info->RestartComm == DEF_TRUE) {               // See Note #5.
          RTOS_ERR_SET(err, RTOS_ERR_NONE);

          USBH_PBHCI_EventPipeTxCmpl(p_pbhci_hc_drv,
                                     ch_nbr,
                                     0u,
                                     USBH_PBHCI_XFER_STATUS_NACK,
                                     err);

          p_ch_info->RestartComm = DEF_FALSE;
        }
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhSrc_ABORT);
        break;

      default:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntChhSrc_NONE);
        break;
    }

    p_ch_info->Halting = DEF_FALSE;
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NONE;
    p_ch_info->Aborted = DEF_FALSE;
    //                                                             ------ ACK RESPONSE RECEIVED/TRANSMITTED INT -------
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_ACK) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChOutTbl[ch_nbr].IntAckCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_ACK;            // Ack int.
    p_ch_info->CurXferErrCnt = 0u;                              // Reset err cnt.
                                                                // Mask ACK int.
    DEF_BIT_CLR(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_ACK);
  }

  if (halt_ch == DEF_TRUE) {
    p_ch_info->Halting = DEF_TRUE;
    USBH_DWCOTGFS_ChHalt(p_reg, ch_nbr);                        // Halt (i.e. disable) the channel.
  }
}

/****************************************************************************************************//**
 *                                       USBH_DWCOTGFS_ISR_HostChIN()
 *
 * @brief    Handle all the interrupts related to an IN transaction (success and errors)
 *
 * @param    p_reg           Pointer to registers structure.
 *
 * @param    p_pbhci_hc_drv  Pointer to PBHCI driver structure.
 *
 * @param    p_data          Pointer to driver private data structure.
 *
 * @param    ch_nbr          Host Channel IN number.
 *
 * @note     (1) The OTG-FS reference manual indicates different non-ACK handling according to the
 *               transfer type. The Host Channel OUT ISR gathers all non-ACK responses in a single
 *               ISR to ease the processing. The table below shows the non-ACK responses that should
 *               be handled according to the transfer type as indicated in the reference manual.
 *               @verbatim
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               | Transfer Type | STALL | NAK | TXERR | BBERR | FRMOR | DTERR |
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               | Control/Bulk  | yes   | yes | yes   | yes   | -     | yes   |
 *               | Interrupt     | yes   | yes | yes   | yes   | yes   | yes   |
 *               | Isochronous   | -     | -   | yes   | yes   | yes   | -     |
 *               +---------------+-------+-----+-------+-------+-------+-------+
 *               @endverbatim
 *               The OTG-FS must always halt the channel to complete a IN transfer. Below is presented
 *               the different interrupts generation paths according to the transfer completing with
 *               and without error.
 *               @n
 *               Transfer without error: a successful IN transfer can take up to 5 interrupts before
 *               calling USBH_PBHCI_EventPipeRxCmpl() because there is the RxFIFO non-empty (RXFLVL)
 *               interrupt to handle before the host controller fires the Transfer Completed (XFRC)
 *               interrupt. A IN transfer implies to handle a few RxFIFO non-empty interrupts per
 *               data IN packet received in the Rx FIFO.
 *               @verbatim
 *               RXFLVL (IN data packet received in Rx FIFO)          ->
 *               RXFLVL (transfer completion status entry in Rx FIFO) ->
 *               XFRC   (halt channel)                                ->
 *               RXFLVL (halt status entry in Rx FIFO)                ->
 *               CHH    (channel halted)                              -> USBH_PBHCI_EventPipeRxCmpl()
 *               @endverbatim
 *               Transfer with error:
 *               @verbatim
 *               STALL -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               NAK   -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               TXERR -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               BBERR -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               FRMOR -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               DTERR -> CHH -> USBH_PBHCI_EventPipeRxCmpl()
 *               @endverbatim
 * @note     (2) The Transaction Error interrupt indicates that one of the following errors occurred on
 *               the USB:
 *                   - CRC check failure
 *                   - Timeout
 *                   - Bit stuff error
 *                   - False EOP
 *
 * @note     (3) In case of transfer aborted, this code prevents the ISR from notifying the PBHCI task
 *               of a transfer completion with error after the pipe abortion.
 *
 * @note     (4) The OTG-FS controller does not support hardware retransmission if the current
 *               transaction for a control, bulk or interrupt has been NAKed or a Transaction Error
 *               has been detected. Hence, retransmission is managed by the PBHCI task.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ISR_HostChIN(USBH_DWCOTGFS_REG *p_reg,
                                       USBH_PBHCI_HC_DRV *p_pbhci_hc_drv,
                                       USBH_PBHCD_DATA   *p_data,
                                       CPU_INT08U        ch_nbr)
{
  USBH_DWCOTGFS_CH_INFO *p_ch_info = &p_data->ChInfoTblPtr[ch_nbr];
  CPU_BOOLEAN           halt_ch = DEF_FALSE;
  CPU_INT32U            hcint_reg;
  RTOS_ERR              err;

  hcint_reg = p_reg->HCH[ch_nbr].HCINTx;
  hcint_reg &= (p_reg->HCH[ch_nbr].HCINTMSKx);

  USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntHostChCnt);

  //                                                               ---------- SUCCESSFUL TRANSFER COMPLETION ----------
  if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_XFRC) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntXferCmplCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_XFRC;           // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_XFER_CMPL;
    halt_ch = DEF_TRUE;
    //                                                             Mask ACK int.
    DEF_BIT_CLR(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_ACK);

    //                                                             ---------------------- ERROR -----------------------
    //                                                             STALL Response Received int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_STALL) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntStallCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_STALL;          // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_STALL;
    halt_ch = DEF_TRUE;

    //                                                             NAK Response Received int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_NAK) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntNakCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_NAK;            // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NAK;
    p_ch_info->CurXferErrCnt = 0u;                              // Reset err cnt.
    halt_ch = DEF_TRUE;

    //                                                             Transaction Error int (see Note #2).
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_TXERR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntTxErrCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_TXERR;          // Ack int.
    halt_ch = DEF_TRUE;

    p_ch_info->CurXferErrCnt++;                                 // Increment err cnt in case of transaction err.
    if (p_ch_info->CurXferErrCnt < 3u) {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NAK;        // Notify core to retry xfer.
    } else {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_TXERR;      // Notify core to abort xfer.
    }
    //                                                             Unmask ACK int.
    DEF_BIT_SET(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_ACK);

    //                                                             Frame Overrun int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_FRMOR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntFrmOverrunErrCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_FRMOR;          // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_FRMOR;
    halt_ch = DEF_TRUE;

    //                                                             Babble Error int.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_BBERR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntBabbleErrCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_BBERR;          // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_BBERR;
    halt_ch = DEF_TRUE;

    //                                                             Data Toggle Error.
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_DTERR) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntDataToggleErrCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_DTERR;          // Ack int.
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_DTERR;
    halt_ch = DEF_TRUE;
    //                                                             ------------------ CHANNEL HALTED ------------------
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_CH_HALTED) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhCnt);
    USBH_DWCOTGFS_DBG_STATS_SET(HCINTx_IntHostChInTbl[ch_nbr].XferInProgress, DEF_FALSE);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_CH_HALTED;      // Ack int.
                                                                // Mask ch halted int.
    DEF_BIT_CLR(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_CH_HALTED);

    if ((p_ch_info->Aborted == DEF_TRUE)                        // See Note #3.
        && (p_ch_info->HaltSrc != DWCOTGFS_HCINTx_HALT_SRC_ABORT)) {
      p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NONE;
      USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChh_ChAborted);
    }

    switch (p_ch_info->HaltSrc) {
      case DWCOTGFS_HCINTx_HALT_SRC_XFER_CMPL:                  // Successful xfer completion.
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_XFER_CMPL);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_NONE);

        USBH_PBHCI_EventPipeRxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_STALL:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_STALL);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_EP_STALL);

        USBH_PBHCI_EventPipeRxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_TXERR:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_TXERR);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_IO);

        USBH_PBHCI_EventPipeRxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_BBERR:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_BBERR);
        p_ch_info->CurXferErrCnt = 0u;                          // Reset err cnt.
        RTOS_ERR_SET(err, RTOS_ERR_IO_FATAL);

        USBH_PBHCI_EventPipeRxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   USBH_PBHCI_XFER_STATUS_NONE,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_DTERR:
      case DWCOTGFS_HCINTx_HALT_SRC_FRMOR:
      case DWCOTGFS_HCINTx_HALT_SRC_NAK:                        // Retransmit xfer on channel (see Note #4).
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_NAK);
        RTOS_ERR_SET(err, RTOS_ERR_NONE);

        USBH_PBHCI_EventPipeRxCmpl(p_pbhci_hc_drv,
                                   ch_nbr,
                                   USBH_PBHCI_XFER_STATUS_NACK,
                                   err);
        break;

      case DWCOTGFS_HCINTx_HALT_SRC_ABORT:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_ABORT);
        break;

      default:
        USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntChhSrc_NONE);
        break;
    }

    p_ch_info->Halting = DEF_FALSE;
    p_ch_info->HaltSrc = DWCOTGFS_HCINTx_HALT_SRC_NONE;
    p_ch_info->Aborted = DEF_FALSE;
    //                                                             ------ ACK RESPONSE RECEIVED/TRANSMITTED INT -------
  } else if (DEF_BIT_IS_SET(hcint_reg, DWCOTGFS_HCINTx_ACK) == DEF_YES) {
    USBH_DWCOTGFS_DBG_STATS_INC(HCINTx_IntHostChInTbl[ch_nbr].IntAckCnt);

    p_reg->HCH[ch_nbr].HCINTx = DWCOTGFS_HCINTx_ACK;            // Ack int.
    p_ch_info->CurXferErrCnt = 0u;                              // Reset err cnt.
                                                                // Mask ACK int.
    DEF_BIT_CLR(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_ACK);
  }

  if (halt_ch == DEF_TRUE) {
    p_ch_info->Halting = DEF_TRUE;
    USBH_DWCOTGFS_ChHalt(p_reg, ch_nbr);                        // Halt (i.e. disable) the channel.
  }
}

/****************************************************************************************************//**
 *                                       USBH_DWCOTGFS_ChHalt()
 *
 * @brief    Halt a given channel.
 *
 * @param    p_reg   Pointer to registers structure.
 *
 * @param    ch_nbr  Host channel number to halt.
 *
 * @note     (1) Setting the bits CHDIS and CHEN allows the host controller core to flush the posted
 *               requests (if any) and generates a channel halted interrupt. The OTG_FS host does not
 *               interrupt the transaction that has already been started on the USB.
 *******************************************************************************************************/
static void USBH_DWCOTGFS_ChHalt(USBH_DWCOTGFS_REG *p_reg,
                                 CPU_INT08U        ch_nbr)
{
  CPU_INT32U hcchar_reg;
  CPU_INT32U ch_type;
  CPU_INT32U tx_q_stat;
  CPU_INT32U tx_q_space_avail;

  hcchar_reg = p_reg->HCH[ch_nbr].HCCHARx;
  //                                                               Flush the posted xfer req (see Note #1).
  DEF_BIT_SET(hcchar_reg, (DWCOTGFS_HCCHARx_CHENA | DWCOTGFS_HCCHARx_CHDIS));

  ch_type = DEF_BIT_FIELD_RD(hcchar_reg, DWCOTGFS_HCCHARx_EP_TYPE_MSK);
  //                                                               Check for space in req queue to issue the halt.
  if ((ch_type == DWCOTGFS_HCCHARx_EP_TYPE_CTRL)
      || (ch_type == DWCOTGFS_HCCHARx_EP_TYPE_BULK)) {
    tx_q_stat = p_reg->HNPTXSTS;                                // Read non-periodic tx req queue status.
    tx_q_space_avail = DEF_BIT_FIELD_RD(tx_q_stat, DWCOTGFS_HNPTXSTS_NPTQXSAV_MSK);
    if (tx_q_space_avail == 0u) {
      USBH_DWCOTGFS_DBG_STATS_INC(HostChHaltTbl[ch_nbr].NonPeriodicTxReqQFullCnt);

      DEF_BIT_CLR(hcchar_reg, DWCOTGFS_HCCHARx_CHENA);
    }
  } else {
    tx_q_stat = p_reg->HPTXSTS;                                 // Read periodic tx req queue status.
    tx_q_space_avail = DEF_BIT_FIELD_RD(tx_q_stat, DWCOTGFS_HPTXSTS_PTQXSAV_MSK);
    if (tx_q_space_avail == 0u) {
      USBH_DWCOTGFS_DBG_STATS_INC(HostChHaltTbl[ch_nbr].PeriodicTxReqQFullCnt);

      DEF_BIT_CLR(hcchar_reg, DWCOTGFS_HCCHARx_CHENA);
    }
  }
  //                                                               Unmask the Channel Halted interrupt.
  DEF_BIT_SET(p_reg->HCH[ch_nbr].HCINTMSKx, DWCOTGFS_HCINTx_CH_HALTED);
  p_reg->HCH[ch_nbr].HCCHARx = hcchar_reg;                      // Update reg.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
