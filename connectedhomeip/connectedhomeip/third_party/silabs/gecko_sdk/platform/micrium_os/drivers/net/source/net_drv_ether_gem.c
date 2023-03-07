/***************************************************************************//**
 * @file
 * @brief Network Device Driver - Cadence Gigabit Ethernet Mac (Gem)
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

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error Ethernet Driver requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

// Define module name for Power Manager debug feature
#define CURRENT_MODULE_NAME    "MICRIUM_ETHERNET_DRIVER"

#include  <drivers/net/include/net_drv_ether.h>

#include  <net/include/net.h>
#include  <net/include/net_if_ether.h>
#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/tcpip/net_util_priv.h>

#include  <cpu/include/cpu_cache.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/toolchains.h>

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include  "sl_power_manager.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 *
 * Note(s) : (1) Receive buffers usually MUST be aligned to some octet boundary.  However, adjusting
 *               receive buffer alignment MUST be performed from within 'net_dev_cfg.h'.  Do not adjust
 *               the value below as it is used for configuration checking only.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MII_REG_RD_WR_TO                              10000    // MII read write timeout.
#define  RX_BUF_ALIGN_OCTETS                              32    // See Note #1.

#define  DEV_RX_CRC_DIS                                    0
#define  DEV_RX_CRC_EN                                     1
#define  DEV_RX_CRC_OPT                                    2

//                                                                 PHY time to power-up, which is typicalled < 32 us in 100BASE-TX
#define  SYSWAKE_TIME                                    100    // 100: 32 us in 100BASE-TX

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 *
 * Note(s) : (1) Instance specific data area structures should be defined below.  The data area
 *               structure typically includes error counters and variables used to track the
 *               state of the device.  Variables required for correct operation of the device
 *               MUST NOT be defined globally and should instead be included within the instance
 *               specific data area structure and referenced as p_if->Dev_Data structure members.
 *
 *           (2) DMA based devices may require more than one type of descriptor.  Each descriptor
 *               type should be defined below.  An example descriptor has been provided.
 *
 *           (3) All device drivers MUST track the addresses of ALL buffers that have been
 *               transmitted and not yet acknowledged through transmit complete interrupts.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_NET

//                                                                 ------------- DMA DESCRIPTOR DATA TYPE -------------
typedef  struct  dev_desc {                                     // See Note #2.
  CPU_REG32 Addr;                                               // Start Address Register.
  CPU_REG32 Status;                                             // Packet Status and Control Register.
} DEV_DESC;

//                                                                 --------------- DEVICE INSTANCE DATA ---------------
typedef  struct  net_dev_data {
  DEV_DESC    *RxDescMemBlck;
  DEV_DESC    *TxDescMemBlck;
  DEV_DESC    *RxBufDescPtrStart;
  DEV_DESC    *RxBufDescPtrCur;
  DEV_DESC    *RxBufDescPtrEnd;
  DEV_DESC    *TxBufDescPtrStart;
  DEV_DESC    *TxBufDescPtrCur;
  DEV_DESC    *TxBufDescPtrEnd;
  DEV_DESC    *TxBufDescCompPtr;                                // See Note #3.
  CPU_INT16U  RxNRdyCtr;
  CPU_BOOLEAN EnableLPI;
#ifdef  NET_MCAST_MODULE_EN
  CPU_INT08U  MulticastAddrHashBitCtr[64];
#endif
} NET_DEV_DATA;

/********************************************************************************************************
 *                                           REGISTER DEFINITIONS
 *
 * Note(s) : (1) Device register definitions SHOULD NOT be absolute & SHOULD use the provided base address
 *               within the device configuration structure (see 'net_dev_cfg.c'), as well as each device's
 *               register definition structure in order to properly resolve register addresses at run-time
 *               by mapping the device register definition structure onto an interface's base address.
 *
 *           (2) The device register definition structure MUST take into account appropriate register
 *               offsets & apply reserved space as required.  The registers listed within the register
 *               definition structure MUST reflect the exact ordering and data sizes illustrated in the
 *               device user guide.
 *
 *           (3) Device registers SHOULD be declared as volatile variables so that compilers do NOT cache
 *               register values but MUST perform the steps to read or write each register value for every
 *               register read or write access.
 *******************************************************************************************************/

typedef  struct  net_dev {
  CPU_REG32 NET_CTRL;                                           // Network Control.
  CPU_REG32 NET_CFG;                                            // Network Configuration.
  CPU_REG32 NET_STATUS;                                         // Network Status.
  CPU_REG32 USER_IO;                                            // User Input/Output.
  CPU_REG32 DMA_CFG;                                            // DMA configuration.
  CPU_REG32 TX_STATUS;                                          // Transmit Status.
  CPU_REG32 RX_QBAR;                                            // Receive Buffer Queue Base Address.
  CPU_REG32 TX_QBAR;                                            // Transmit Buffer Queue Base Address.
  CPU_REG32 RX_STATUS;                                          // Receive Status.
  CPU_REG32 INTR_STATUS;                                        // Interrupt Status.
  CPU_REG32 INTR_EN;                                            // Interrupt Enable.
  CPU_REG32 INTR_DIS;                                           // Interrupt Disable.
  CPU_REG32 INTR_MASK;                                          // Interrupt Mask Status.
  CPU_REG32 PHY_MAINT;                                          // PHY Maintenance.
  CPU_REG32 RX_PAUSEQ;                                          // Receive Pause Quantum.
  CPU_REG32 TX_PAUSEQ;                                          // Transmit Pause Quantum.
  CPU_REG32 RESERVED1[8];
  CPU_REG32 SYSWAKE;                                            // System wake time
  CPU_REG32 RESERVED2[7];
  CPU_REG32 HASH_BOT;                                           // Hash Register Bottom.
  CPU_REG32 HASH_TOP;                                           // Hash Register Top.
  CPU_REG32 SPEC_ADDR1_BOT;                                     // Specific Address 1 Bottom.
  CPU_REG32 SPEC_ADDR1_TOP;                                     // Specific Address 1 Top.
  CPU_REG32 SPEC_ADDR2_BOT;                                     // Specific Address 2 Bottom.
  CPU_REG32 SPEC_ADDR2_TOP;                                     // Specific Address 2 Top.
  CPU_REG32 SPEC_ADDR3_BOT;                                     // Specific Address 3 Bottom.
  CPU_REG32 SPEC_ADDR3_TOP;                                     // Specific Address 3 Top.
  CPU_REG32 SPEC_ADDR4_BOT;                                     // Specific Address 4 Bottom.
  CPU_REG32 SPEC_ADDR4_TOP;                                     // Specific Address 4 Top.
  CPU_REG32 TYPE_ID_MATCH1;                                     // Type ID Match 1.
  CPU_REG32 TYPE_ID_MATCH2;                                     // Type ID Match 2.
  CPU_REG32 TYPE_ID_MATCH3;                                     // Type ID Match 3.
  CPU_REG32 TYPE_ID_MATCH4;                                     // Type ID Match 4.
  CPU_REG32 WAKE_ON_LAN;                                        // Wake on LAN.
  CPU_REG32 IPG_STRETCH;                                        // IPG Stretch.
  CPU_REG32 STACKED_VLAN;                                       // Stacked VLAN Register.
  CPU_REG32 TX_PFC_PAUSE;                                       // Transmit PFC Pause Register.
  CPU_REG32 SPEC_ADDR1_MASK_BOT;                                // Specific Address Mask 1 Bottom.
  CPU_REG32 SPEC_ADDR1_MASK_TOP;                                // Specific Address Mask 1 Top.
  CPU_REG32 RESERVED3[12];
  CPU_REG32 MODULE_ID;                                          // Module ID.
  CPU_REG32 OCTETS_TX_BOT;                                      // Octets transmitted bottom.
  CPU_REG32 OCTETS_TX_TOP;                                      // Octets transmitted top.
  CPU_REG32 FRAMES_TX;                                          // Frames Transmitted.
  CPU_REG32 BROADCAST_FRAMES_TX;                                // Broadcast Frames Transmitted.
  CPU_REG32 MULTI_FRAMES_TX;                                    // Multicast Frames Transmitted.
  CPU_REG32 PAUSE_FRAMES_TX;                                    // Pause Frames Transmitted.
  CPU_REG32 FRAMES_64B_TX;                                      // Frames Transmitted. 64 bytes.
  CPU_REG32 FRAMES_65TO127B_TX;                                 // Frames Transmitted. 65 to 127 bytes.
  CPU_REG32 FRAMES_128TO511B_TX;                                // Frames Transmitted. 128 to 511 bytes.
  CPU_REG32 FRAMES_512BTO1023B_TX;                              // Frames Transmitted. 512 to 1023 bytes.
  CPU_REG32 FRAMES_1024TO1518B_TX;                              // Frames Transmitted. 1024 ti 1518 bytes.
  CPU_REG32 RESERVED4;
  CPU_REG32 TX_UNDER_RUNS;                                      // Transmit under runs.
  CPU_REG32 SINGLE_COLLISN_FRAMES;                              // Single Collision Frames.
  CPU_REG32 MULTI_COLLISN_FRAMES;                               // Multi Collision Frames.
  CPU_REG32 EXCESSIVE_COLLISNS;                                 // Excesive Collisions.
  CPU_REG32 LATE_COLLISNS;                                      // Late Collisions.
  CPU_REG32 DEFFERED_TX_FRAMES;                                 // Deffered Transmission Frames.
  CPU_REG32 CARRIER_SENSE_ERRS;                                 // Carrier Sense Errors.
  CPU_REG32 OCTETS_RX_BOT;                                      // Octets Received Bottom.
  CPU_REG32 OCTETS_RX_TOP;                                      // Octets Received Top.
  CPU_REG32 FRAMES_RX;                                          // Frames Received.
} NET_DEV;

/********************************************************************************************************
 *                                       REGISTER BIT DEFINITIONS
 *
 * Note(s) : (1) All necessary register bit definitions should be defined within this section.
 *******************************************************************************************************/

//                                                                 ------------------ RX DESCRIPTORS ------------------
#define  GEM_RXBUF_ADDR_MASK              (0xFFFFFFFC)          // RX buffer address.
#define  GEM_RXBUF_ADDR_WRAP              DEF_BIT_01            // Wrap flag.
#define  GEM_RXBUF_ADDR_OWN               DEF_BIT_00            // Ownership flag.

#define  GEM_RXBUF_STATUS_GBC             DEF_BIT_31            // Global broadcast detected.
#define  GEM_RXBUF_STATUS_MULTI_MATCH     DEF_BIT_30            // Multicast hash match.
#define  GEM_RXBUF_STATUS_UNI_MATCH       DEF_BIT_29            // Unicast hash match.
#define  GEM_RXBUF_STATUS_EXT_MATCH       DEF_BIT_28            // External address match.

#define  GEM_RXBUF_STATUS_EOF             DEF_BIT_15            // End of frame.
#define  GEM_RXBUF_STATUS_SOF             DEF_BIT_14            // Start of frame.
#define  GEM_RXBUF_SIZE_MASK              (0x1FFFu)             // Size of frame.

//                                                                 ------------------ TX DESCRIPTORS ------------------
#define  GEM_TXBUF_ADDR_MASK              (0xFFFFFFFC)          // TX Buffer address.

#define  GEM_TXBUF_USED                   DEF_BIT_31            // Used flag.
#define  GEM_TXBUF_WRAP                   DEF_BIT_30            // Wrap flag.
#define  GEM_TXBUF_RETRY_EXCED            DEF_BIT_29            // Retry limit exceeded.
#define  GEM_TXBUF_AHB_ERR                DEF_BIT_27            // Frame corruption due to AHB error.
#define  GEM_TXBUF_LATE_COLL              DEF_BIT_26            // Late collision.

#define  GEM_TXBUF_NO_CRC                 DEF_BIT_16            // No CRC to be appended.
#define  GEM_TXBUF_LAST                   DEF_BIT_15            // Last buffer.
#define  GEM_TXBUF_LENGTH_MASK            (0x3FFF)              // Buffer length.

//                                                                 ----------------- NETWORK CONTROL ------------------
#define  GEM_BIT_CTRL_TXLPIEN                  DEF_BIT_19       // Request low power idle from PHY firmware.
#define  GEM_BIT_CTRL_FLUSH_NEXT_RX_DPRAM_PKT  DEF_BIT_18       // Flush next packet from the external DPRAM.
#define  GEM_BIT_CTRL_TX_PFC_PRI_PAUSE_FRAME   DEF_BIT_17       // Transmit PFC priority baste pause frame.
#define  GEM_BIT_CTRL_EN_PFC_PRI_PAUSE_RX      DEF_BIT_16       // Enable PFS priority based pause reception.
#define  GEM_BIT_CTRL_STR_RX_TIMESTAMP         DEF_BIT_15       // Store timestamps to memory.
#define  GEM_BIT_CTRL_TX_ZEROQ_PAUSE_FRAME     DEF_BIT_12       // Transmit zero quantum pause frame.
#define  GEM_BIT_CTRL_TX_PAUSE_FRAME           DEF_BIT_11       // Transmit pause frame.
#define  GEM_BIT_CTRL_TX_HALT                  DEF_BIT_10       // Transmit halt.
#define  GEM_BIT_CTRL_START_TX                 DEF_BIT_09       // Start transmission.
#define  GEM_BIT_CTRL_BACK_PRESSURE            DEF_BIT_08       // Back pressure.
#define  GEM_BIT_CTRL_WREN_STAT_REGS           DEF_BIT_07       // Write enable for stats registers.
#define  GEM_BIT_CTRL_INCR_STATS_REGS          DEF_BIT_06       // Increment statistics registers.
#define  GEM_BIT_CTRL_CLEAR_STATS_REGS         DEF_BIT_05       // Clear statistics registers.
#define  GEM_BIT_CTRL_MGMT_PORT_EN             DEF_BIT_04       // Management port enable.
#define  GEM_BIT_CTRL_TX_EN                    DEF_BIT_03       // Tramsit enable.
#define  GEM_BIT_CTRL_RX_EN                    DEF_BIT_02       // Receive enable.
#define  GEM_BIT_CTRL_LOOPBACK_LOCAL           DEF_BIT_01       // Loop back local.

//                                                                 -------------- NETWORK CONFIGURATION ---------------
#define  GEM_BIT_CFG_UNIDIR_EN                 DEF_BIT_31       // Uni-drection enable.
#define  GEM_BIT_CFG_IGNORE_IPG_RX_ER          DEF_BIT_30       // Ignore IPG rx_er.
#define  GEM_BIT_CFG_RX_BAD_PREAMBLE           DEF_BIT_29       // Receive bad preamble.
#define  GEM_BIT_CFG_IPG_STRETCH_EN            DEF_BIT_28       // IPG stretch enable.
#define  GEM_BIT_CFG_SGMII_EN                  DEF_BIT_27       // SGMII mode enable.
#define  GEM_BIT_CFG_IGNORE_RX_FCS             DEF_BIT_26       // Ingore RX FCS.
#define  GEM_BIT_CFG_RX_HD_WHILE_TX            DEF_BIT_25       // RX half duplex while TX.
#define  GEM_BIT_CFG_RX_CHKSUM_OFFLD_EN        DEF_BIT_24       // Receive checksum offloading enable.
#define  GEM_BIT_CFG_DIS_CP_PAUSE_FRAME        DEF_BIT_23       // Disable copy of pause frame.
#define  GEM_BIT_CFG_DBUS_WIDTH_MSK (DEF_BIT_FIELD(2, 21))      // Data bus width.
#define  GEM_BIT_CFG_DBUS_WIDTH(cfg) (DEF_BIT_MASK(cfg, 21) & GEM_BIT_CFG_DBUS_WIDTH_MSK)
#define  GEM_BIT_CFG_MDC_CLK_DIV_MSK (DEF_BIT_FIELD(3, 18))     // MDC clock division.
#define  GEM_BIT_CFG_MDC_CLK_DIV(cfg) (DEF_BIT_MASK(cfg, 18) & GEM_BIT_CFG_MDC_CLK_DIV_MSK)
#define  GEM_BIT_CFG_FCS_REMOVE               DEF_BIT_17        // FCS remove.
#define  GEM_BIT_CFG_LEN_ERR_FRAME_DISC       DEF_BIT_16        // Length field error frame discard.
#define  GEM_BIT_CFG_RX_BUF_OFF_MSK (DEF_BIT_FIELD(2, 14))      // Receive buffer offset.
#define  GEM_BIT_CFG_RX_BUF_OFF(cfg) (DEF_BIT_MASK(cfg, 14) & GEM_BIT_CFG_RX_BUF_OFF_MSK)
#define  GEM_BIT_CFG_PAUSE_EN                 DEF_BIT_13        // Pause enable.
#define  GEM_BIT_CFG_RETRY_TEST               DEF_BIT_12        // Retry test.
#define  GEM_BIT_CFG_PCS_SEL                  DEF_BIT_11        // PCS select.
#define  GEM_BIT_CFG_GIGE_EN                  DEF_BIT_10        // Gigabit mode enable.
#define  GEM_BIT_CFG_EXT_ADDR_MATCH_EN        DEF_BIT_09        // External address match enable.
#define  GEM_BIT_CFG_UNI_HASH_EN              DEF_BIT_07        // Unicast hash enable.
#define  GEM_BIT_CFG_MULTI_HASH_EN            DEF_BIT_06        // Multicast hash enable.
#define  GEM_BIT_CFG_NO_BROADCAST             DEF_BIT_05        // No broadcast.
#define  GEM_BIT_CFG_COPY_ALL                 DEF_BIT_04        // Copy all frames.
#define  GEM_BIT_CFG_DISC_NON_VLAN            DEF_BIT_02        // Discard non VLAN frames.
#define  GEM_BIT_CFG_FULL_DUPLEX              DEF_BIT_01        // Full duplex.
#define  GEM_BIT_CFG_SPEED                    DEF_BIT_00        // Speed.

//                                                                 ------------------ NETWORK STATUS ------------------
#define  GEM_BIT_STATUS_PFC_PRI_PAUSE_NEG     DEF_BIT_06        // PFC pause negociated.
#define  GEM_BIT_STATUS_PCS_AUTONEG_TX_RES    DEF_BIT_05        // PCS pause tx resolution.
#define  GEM_BIT_STATUS_PCS_AUTONEG_RX_RES    DEF_BIT_04        // PCS pause rx resultion.
#define  GEM_BIT_STATUS_PCS_AUTONEG_DUP_RES   DEF_BIT_03        // PCS duplex resolution.
#define  GEM_BIT_STATUS_PHY_MGMT_IDLE         DEF_BIT_02        // PHY MGMT logic idle.
#define  GEM_BIT_STATUS_MDIO_IN_PIN_STATUS    DEF_BIT_01        // MDIO in pin status.
#define  GEM_BIT_STATUS_PCS_LINK_STATE        DEF_BIT_00        // PCS link state.

//                                                                 ---------------- DMA CONFIGURATION -----------------
#define  GEM_BIT_DMACFG_DISC_WHEN_NO_AHB      DEF_BIT_24        // Discard packet when no AHB resource
#define  GEM_BIT_DMACFG_AHB_RX_SIZE_MSK (DEF_BIT_FIELD(8, 16))  // Receive buffer offset.
#define  GEM_BIT_DMACFG_AHB_RX_SIZE(cfg) (DEF_BIT_MASK(cfg, 16) & GEM_BIT_DMACFG_AHB_RX_SIZE_MSK)
#define  GEM_BIT_DMACFG_CSUM_GEN_OFFLOAD_EN   DEF_BIT_11        // Checksum generation offload enable.
#define  GEM_BIT_DMACFG_TX_PKTBUF_MEMSZ_SEL   DEF_BIT_10        // Transmit packet buffer memory size.
#define  GEM_BIT_DMACFG_RX_PKTBUF_SZ_MSK (DEF_BIT_FIELD(2, 8))  // Receive packet buffer memory size.
#define  GEM_BIT_DMACFG_RX_PKTBUF_SZ(cfg) (DEF_BIT_MASK(cfg, 8) & GEM_BIT_DMACFG_RX_PKTBUF_SZ_MSK)
#define  GEM_BIT_DMACFG_AHB_PKT_SWAP_EN       DEF_BIT_07        // AHB endian swap enable for packets.
#define  GEM_BIT_DMACFG_AHC_MGMT_SWAP_EN      DEF_BIT_06        // AHB endian swap enable for management descriptors
#define  GEM_BIT_DMACFG_RX_AHB_BURST_MSK (DEF_BIT_FIELD(5, 0))  // AHB fixed burst length.
#define  GEM_BIT_DMACFG_RX_AHB_BURST(cfg) (DEF_BIT_MASK(cfg, 0) & GEM_BIT_DMACFG_RX_AHB_BURST_MSK)

//                                                                 ----------------- TRANSMIT STATUS ------------------
#define  GEM_BIT_TXSTATUS_HRESP_NOT_OK        DEF_BIT_08        // Hresp not OK.
#define  GEM_BIT_TXSTATUS_LATE_COLLISION      DEF_BIT_07        // Late collision occurred.
#define  GEM_BIT_TXSTATUS_TX_UNDER_RUN        DEF_BIT_06        // Transmit under run.
#define  GEM_BIT_TXSTATUS_TX_COMPLETE         DEF_BIT_05        // Transmit complete.
#define  GEM_BIT_TXSTATUS_TX_CORR_AHB_ERR     DEF_BIT_04        // Transmit frame corruption due to AHB error.
#define  GEM_BIT_TXSTATUS_TX_GO               DEF_BIT_03        // Transmit go.
#define  GEM_BIT_TXSTATUS_RETRY_EXCEEDED      DEF_BIT_02        // Retry limit exceeded.
#define  GEM_BIT_TXSTATUS_COLLISION           DEF_BIT_01        // Collision occurred.
#define  GEM_BIT_TXSTATUS_USED_BIT_READ       DEF_BIT_00        // Used bit read.

//                                                                 ------------------ RECEIVE STATUS ------------------
#define  GEM_BIT_RXSTATUS_HRESP_NOT_OK        DEF_BIT_03        // Hresp not OK.
#define  GEM_BIT_RXSTATUS_RX_OVERRUN          DEF_BIT_02        // Receive overrun.
#define  GEM_BIT_RXSTATUS_FRAME_RECD          DEF_BIT_01        // Frame received.
#define  GEM_BIT_RXSTATUS_BUFFER_NOT_AVAIL    DEF_BIT_00        // Buffer not available.

//                                                                 ----------------- INTERRUPT STATUS -----------------
#define  GEM_BIT_INT_TSU_SEC_INCR             DEF_BIT_26        // TSE seconds register increment.
#define  GEM_BIT_INT_PDELAY_RESP_TX           DEF_BIT_25        // PTP pdelay_resp frame transmitted.
#define  GEM_BIT_INT_PDELAY_REQ_TX            DEF_BIT_24        // PTP pdelay_req frame transmitted.
#define  GEM_BIT_INT_PDELAY_RESP_RX           DEF_BIT_23        // PTP pdelay_resp frame received.
#define  GEM_BIT_INT_PDELAY_REQ_RX            DEF_BIT_22        // PTP pdelay_req frame received.
#define  GEM_BIT_INT_SYNC_TX                  DEF_BIT_21        // PTP sync frame transmitted.
#define  GEM_BIT_INT_DELAY_REQ_TX             DEF_BIT_20        // PTP delay_req frame transmitted.
#define  GEM_BIT_INT_SYNC_RX                  DEF_BIT_19        // PTP sync frame received.
#define  GEM_BIT_INT_DELAY_REQ_RX             DEF_BIT_18        // PTP delay_req frame received.
#define  GEM_BIT_INT_PARTNER_PG_RX            DEF_BIT_17        // PCS link partner page received.
#define  GEM_BIT_INT_AUTONEG_COMPLETE         DEF_BIT_16        // PCS auto-negotiation complete.
#define  GEM_BIT_INT_EXT_INTR                 DEF_BIT_15        // External interrupt.
#define  GEM_BIT_INT_PAUSE_TX                 DEF_BIT_14        // Pause frame transmitted.
#define  GEM_BIT_INT_PAUSE_ZERO               DEF_BIT_13        // Pause time zero.
#define  GEM_BIT_INT_PAUSE_NONZERO_RX         DEF_BIT_12        // Pause frame with non-zero pause quantum received.
#define  GEM_BIT_INT_HRESP_NOT_OK             DEF_BIT_11        // Hresp not OK.
#define  GEM_BIT_INT_RX_OVERRUN               DEF_BIT_10        // Receive overrun.
#define  GEM_BIT_INT_LINK_CHNG                DEF_BIT_09        // Link state change.
#define  GEM_BIT_INT_TX_COMPLETE              DEF_BIT_07        // Transmit complete.
#define  GEM_BIT_INT_TX_CORRUPT_AHB           DEF_BIT_06        // Transmit frame corruption due to AHB error.
#define  GEM_BIT_INT_RETRY_EX_LATE            DEF_BIT_05        // Retry limit exceeded or late collision.
#define  GEM_BIT_INT_TX_USED_READ             DEF_BIT_03        // TX used bit read.
#define  GEM_BIT_INT_RX_USED_READ             DEF_BIT_02        // RX used bit read.
#define  GEM_BIT_INT_RX_COMPLETE              DEF_BIT_01        // Receive complete.
#define  GEM_BIT_INT_MGMT_SENT                DEF_BIT_00        // Management frame sent.

//                                                                 ----------------- PHY MAINTENANCE ------------------
#define  GEM_BIT_PHYMGMT_CLAUSE_22            DEF_BIT_30        // Clause 22 operation.
#define  GEM_BIT_PHYMGMT_OPERATION_MSK (DEF_BIT_FIELD(2, 28))   // Operation.
#define  GEM_BIT_PHYMGMT_OPERATION(cfg) (DEF_BIT_MASK(cfg, 28) & GEM_BIT_PHYMGMT_OPERATION_MSK)
#define  GEM_BIT_PHYMGMT_PHYADDR_MSK (DEF_BIT_FIELD(5, 23))     // PHY address.
#define  GEM_BIT_PHYMGMT_PHYADDR(cfg) (DEF_BIT_MASK(cfg, 23) & GEM_BIT_PHYMGMT_PHYADDR_MSK)
#define  GEM_BIT_PHYMGMT_REGADDR_MSK (DEF_BIT_FIELD(5, 18))     // Register address.
#define  GEM_BIT_PHYMGMT_REGADDR(cfg) (DEF_BIT_MASK(cfg, 18) & GEM_BIT_PHYMGMT_REGADDR_MSK)
#define  GEM_BIT_PHYMGMT_MUST10_MSK (DEF_BIT_FIELD(2, 16))      // Must be 10.
#define  GEM_BIT_PHYMGMT_MUST10(cfg) (DEF_BIT_MASK(cfg, 16) & GEM_BIT_PHYMGMT_MUST10_MSK)
#define  GEM_BIT_PHYMGMT_DATA_MSK (DEF_BIT_FIELD(16, 0))        // Data.
#define  GEM_BIT_PHYMGMT_DATA(cfg) (DEF_BIT_MASK(cfg, 0) & GEM_BIT_PHYMGMT_DATA_MSK)

#define  INT_STATUS_MASK_ALL                      0xFFFFFFFF
#define  INT_STATUS_MASK_SUPPORTED                GEM_BIT_INT_RX_COMPLETE | GEM_BIT_INT_TX_COMPLETE;
#define  CTRL_TX_EN                               DEF_BIT_00
#define  CTRL_RX_EN                               DEF_BIT_00
#define  CTRL_RX_CRC_EN                           DEF_BIT_00

#define  RX_ISR_EVENT_MSK                         GEM_BIT_INT_RX_COMPLETE
#define  TX_ISR_EVENT_MSK                         GEM_BIT_INT_TX_COMPLETE
#define  UNHANDLED_ISR_EVENT_MASK                 DEF_BIT_00

/********************************************************************************************************
 *                                       DESCRIPTOR BIT DEFINITIONS
 *******************************************************************************************************/

#define  DESC_VALID_MSK                           DEF_BIT_00
#define  DESC_WRAP_BIT_MASK                       DEF_BIT_00

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *
 * Note(s) : (1) Device driver functions may be arbitrarily named.  However, it is recommended that device
 *               driver functions be named using the names provided below.  All driver function prototypes
 *               should be located within the driver C source file ('net_dev_&&&.c') & be declared as
 *               static functions to prevent name clashes with other network protocol suite device drivers.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------- FNCT'S COMMON TO ALL DEV'S --------
static void NetDev_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err);

static void NetDev_Start(NET_IF   *p_if,
                         RTOS_ERR *p_err);

static void NetDev_Stop(NET_IF   *p_if,
                        RTOS_ERR *p_err);

static void NetDev_Rx(NET_IF     *p_if,
                      CPU_INT08U **p_data,
                      CPU_INT16U *p_size,
                      RTOS_ERR   *p_err);

static void NetDev_Tx(NET_IF     *p_if,
                      CPU_INT08U *p_data,
                      CPU_INT16U size,
                      RTOS_ERR   *p_err);

static void NetDev_AddrMulticastAdd(NET_IF     *p_if,
                                    CPU_INT08U *paddr_hw,
                                    CPU_INT08U addr_hw_len,
                                    RTOS_ERR   *p_err);

static void NetDev_AddrMulticastRemove(NET_IF     *p_if,
                                       CPU_INT08U *paddr_hw,
                                       CPU_INT08U addr_hw_len,
                                       RTOS_ERR   *p_err);

static void NetDev_ISR_Handler(NET_IF           *p_if,
                               NET_DEV_ISR_TYPE type);

static void NetDev_IO_Ctrl(NET_IF     *p_if,
                           CPU_INT08U opt,
                           void       *p_data,
                           RTOS_ERR   *p_err);

static void NetDev_MII_Rd(NET_IF     *p_if,
                          CPU_INT08U phy_addr,
                          CPU_INT08U reg_addr,
                          CPU_INT16U *p_data,
                          RTOS_ERR   *p_err);

static void NetDev_MII_Wr(NET_IF     *p_if,
                          CPU_INT08U phy_addr,
                          CPU_INT08U reg_addr,
                          CPU_INT16U data,
                          RTOS_ERR   *p_err);

//                                                                 ----- FNCT'S COMMON TO DMA-BASED DEV'S -----
static void NetDev_RxDescInit(NET_IF   *p_if,
                              RTOS_ERR *p_err);

static void NetDev_TxDescInit(NET_IF   *p_if,
                              RTOS_ERR *p_err);

static void NetDev_RxDescFreeAll(NET_IF   *p_if,
                                 RTOS_ERR *p_err);

static void NetDev_RxDescPtrCurInc(NET_IF *p_if);

//                                                                 ------------- HELPER FUNCTIONS -------------
static void NetDev_AssertLPI_TX(NET_IF *p_if);

static void NetDev_DeAssertLPI_TX(NET_IF *p_if);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       NETWORK DEVICE DRIVER API
 *
 * Note(s) : (1) Device driver API structures are used by applications during calls to NetIF_Add().  This
 *               API structure allows higher layers to call specific device driver functions via function
 *               pointer instead of by name.  This enables the network protocol suite to compile & operate
 *               with multiple device drivers.
 *
 *           (2) In most cases, the API structure provided below SHOULD suffice for most device drivers
 *               exactly as is with the exception that the API structure's name which MUST be unique &
 *               SHOULD clearly identify the device being implemented.  For example, the Cirrus Logic
 *               CS8900A Ethernet controller's API structure should be named NetDev_API_CS8900A[].
 *
 *               The API structure MUST also be externally declared in the device driver header file
 *               ('net_dev_&&&.h') with the exact same name & type.
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_DEV_API_ETHER NetDev_API_GEM = {                      // Ether DMA dev API fnct ptrs :
  &NetDev_Init,                                                 // Init/add
  &NetDev_Start,                                                // Start
  &NetDev_Stop,                                                 // Stop
  &NetDev_Rx,                                                   // Rx
  &NetDev_Tx,                                                   // Tx
  &NetDev_AddrMulticastAdd,                                     // Multicast addr add
  &NetDev_AddrMulticastRemove,                                  // Multicast addr remove
  &NetDev_ISR_Handler,                                          // ISR handler
  &NetDev_IO_Ctrl,                                              // I/O ctrl
  &NetDev_MII_Rd,                                               // Phy reg rd
  &NetDev_MII_Wr                                                // Phy reg wr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetDev_Init()
 *
 * @brief    (1) Initialize Network Driver Layer :
 *               - (a) Initialize required clock sources
 *               - (b) Initialize external interrupt controller
 *               - (c) Initialize external GPIO controller
 *               - (d) Initialize driver state variables
 *               - (e) Allocate memory for device DMA descriptors
 *               - (f) Initialize additional device registers
 *                   - (1) (R)MII mode / Phy bus type
 *                   - (2) Disable device interrupts
 *                   - (3) Disable device receiver and transmitter
 *                   - (4) Other necessary device initialization
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) The application developer SHOULD define NetDev_CfgClk() within net_bsp.c
 *               in order to properly enable clocks for specified network interface.  In
 *               some cases, a device may require clocks to be enabled for BOTH the device
 *               and accessory peripheral modules such as GPIO.  A call to this function
 *               MAY need to occur BEFORE any device register accesses are made.  In the
 *               event that a device does NOT require any external clocks to be enabled,
 *               it is recommended that the device driver still call the NetBSP fuction
 *               which may in turn leave the section for the specific interface number
 *               empty.
 *
 * @note     (3) The application developer SHOULD define NetDev_CfgGPIO() within net_bsp.c
 *               in order to properly configure any necessary GPIO necessary for the device
 *               to operate properly.  Micrium recommends defining and calling this NetBSP
 *               function even if no additional GPIO initialization is required.
 *
 * @note     (4) The application developer SHOULD define NetDev_CfgIntCtrl() within net_bsp.c
 *               in order to properly enable interrupts on an external or CPU integrated
 *               interrupt controller.  Interrupt sources that are specific to the DEVICE
 *               hardware MUST NOT be initialized from within NetDev_CfgIntCtrl() and
 *               SHOULD only be modified from within the device driver.
 *               - (a) External interrupt sources are cleared within the NetBSP first level
 *                     ISR handler either before or after the call to the device driver ISR
 *                     handler function.  The device driver ISR handler function SHOULD only
 *                     clear the device specific interrupts and NOT external or CPU interrupt
 *                     controller interrupt sources.
 *
 * @note     (5) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *
 * @note     (6) All device drivers that store instance specific data MUST declare all
 *               instance specific variables within the device data area defined above.
 *
 * @note     (7) Drivers SHOULD validate device configuration values and set *p_err to
 *               NET_DEV_ERR_INVALID_CFG if unacceptible values have been specified. Fields
 *               of interest generally include, but are not limited to :
 *               - (a) p_dev_cfg->RxBufPoolType :
 *                   - (1) NET_IF_MEM_TYPE_MAIN
 *                   - (2) NET_IF_MEM_TYPE_DEDICATED
 *               - (b) p_dev_cfg->TxBufPoolType :
 *                   - (1) NET_IF_MEM_TYPE_MAIN
 *                   - (2) NET_IF_MEM_TYPE_DEDICATED
 *               - (c) p_dev_cfg->RxBufAlignOctets
 *               - (d) p_dev_cfg->TxBufAlignOctets
 *               - (e) p_dev_cfg->RxBufDescNbr
 *               - (f) p_dev_cfg->TxBufDescnbr
 *
 * @note     (8) Descriptors are typically required to be contiguous in memory.  Allocation of
 *               descriptors MUST occur as a single contigous block of memory.  The driver may
 *               use pointer arithmetic to sub-divide and traverse the descriptor list.
 *
 * @note     (9) NetDev_Init() should exit with :
 *               - (a) All device interrupt source disabled. External interrupt controllers
 *                     should however be ready to accept interrupt requests.
 *               - (b) All device interrupt sources cleared.
 *               - (c) Both the receiver and transmitter disabled.
 *
 *           10) Some drivers MAY require knowledge of the Phy configuration in order
 *               to properly configure the MAC with the correct Phy bus mode, speed and
 *               duplex settings.  If a driver requires access to the Phy configuration,
 *               then the driver MUST validate the p_if->Phy_Cfg pointer by checking for
 *               a NULL pointer BEFORE attempting to access members of the Phy
 *               configuration structure.  Phy configuration fields of interest generally
 *               include, but are  not limited to :
 *               - (a) p_phy_cfg->Type :
 *                   - (1) NET_PHY_TYPE_INT            Phy integrated with MAC.
 *                   - (2) NET_PHY_TYPE_EXT            Phy externally attached to MAC.
 *               - (b) p_phy_cfg->BusMode :
 *                   - (1) NET_PHY_BUS_MODE_MII        Phy bus mode configured to MII.
 *                   - (2) NET_PHY_BUS_MODE_RMII       Phy bus mode configured to RMII.
 *                   - (3) NET_PHY_BUS_MODE_SMII       Phy bus mode configured to SMII.
 *               - (c) p_phy_cfg->Spd :
 *                   - (1) NET_PHY_SPD_0               Phy link speed unknown or NOT linked.
 *                   - (2) NET_PHY_SPD_10              Phy link speed configured to  10   mbit/s.
 *                   - (3) NET_PHY_SPD_100             Phy link speed configured to  100  mbit/s.
 *                   - (4) NET_PHY_SPD_1000            Phy link speed configured to  1000 mbit/s.
 *                   - (5) NET_PHY_SPD_AUTO            Phy link speed configured for auto-negotiation.
 *               - (d) p_phy_cfg->Duplex :
 *                   - (1) NET_PHY_DUPLEX_UNKNOWN      Phy link duplex unknown or link not established.
 *                   - (2) NET_PHY_DUPLEX_HALF         Phy link duplex configured to  half duplex.
 *                   - (3) NET_PHY_DUPLEX_FULL         Phy link duplex configured to  full duplex.
 *                   - (4) NET_PHY_DUPLEX_AUTO         Phy link duplex configured for auto-negotiation.
 *******************************************************************************************************/
static void NetDev_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err)
{
  NET_DEV_BSP_ETHER *p_dev_bsp;
  NET_PHY_CFG_ETHER *p_phy_cfg;
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  NET_BUF_SIZE buf_size_max;
  NET_BUF_SIZE buf_ix;
#endif
  CPU_SIZE_T reqd_octets;
  CPU_SIZE_T nbytes;

  //                                                               -------- OBTAIN REFERENCE TO CFGs/REGs/BSP ---------
  p_phy_cfg = (NET_PHY_CFG_ETHER *)p_if->Ext_Cfg;
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;
  p_dev_bsp = (NET_DEV_BSP_ETHER *)p_if->Dev_BSP;

  //                                                               --------------- VALIDATE DEVICE CFG ----------------
  //                                                               See Note #7.
  RTOS_ASSERT_DBG_ERR_SET(((p_dev_cfg->RxBufAlignOctets & (RX_BUF_ALIGN_OCTETS - 1u)) == 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET( (p_dev_cfg->RxBufIxOffset == 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET( (p_dev_cfg->TxBufIxOffset == 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET( (p_phy_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               Validate Rx buf size.
  buf_ix = NET_IF_IX_RX;

  buf_size_max = NetBuf_GetMaxSize(p_if->Nbr,
                                   NET_TRANSACTION_RX,
                                   DEF_NULL,
                                   buf_ix);
  if (buf_size_max < NET_IF_ETHER_FRAME_MAX_SIZE) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  //                                                               -------------- ALLOCATE DEV DATA AREA --------------
  p_if->Dev_Data = Mem_SegAllocExt("Net_Dev_data_seg",
                                   DEF_NULL,
                                   sizeof(NET_DEV_DATA),
                                   32u,
                                   &reqd_octets,
                                   p_err);
  if (p_if->Dev_Data == DEF_NULL) {
    return;
  }

  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;

  //                                                               ------------- ENABLE NECESSARY CLOCKS --------------
  //                                                               Enable module clks (see Note #2).
  p_dev_bsp->CfgClk(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------- INITIALIZE EXTERNAL GPIO CONTROLLER --------
  //                                                               Configure Ethernet Controller GPIO (see Note #4).
  p_dev_bsp->CfgGPIO(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----- INITIALIZE EXTERNAL INTERRUPT CONTROLLER ------
  //                                                               Configure ext int ctrl'r (see Note #3).
  p_dev_bsp->CfgIntCtrl(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------- ALLOCATE MEMORY FOR DMA DESCRIPTORS --------
  //                                                               Determine block size.
  nbytes = p_dev_cfg->RxDescNbr * sizeof(DEV_DESC);
  p_dev_data->RxDescMemBlck = Mem_SegAlloc("DMA_rx_pool",              // Name of the memory segment.
                                           (void *)p_dev_cfg->MemAddr, // From the dedicated memory.
                                           nbytes,                     // Block size large enough to hold all Rx descriptors.
                                           p_err);                     // Ptr to variable to return an error code.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               Determine block size.
  nbytes = p_dev_cfg->TxDescNbr * sizeof(DEV_DESC);
  p_dev_data->TxDescMemBlck = Mem_SegAlloc("DMA_tx_pool",              // Name of the memory segment.
                                           (void *)p_dev_cfg->MemAddr, // From the dedicated memory.
                                           nbytes,                     // Block size large enough to hold all Tx descriptors.
                                           p_err);                     // Ptr to variable to return an error code.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                               NetDev_Start()
 *
 * @biref    (1) Start network interface hardware :
 *               - (a) Initialize transmit semaphore count
 *               - (b) Initialize hardware address registers
 *               - (c) Initialize receive and transmit descriptors
 *               - (d) Clear all pending interrupt sources
 *               - (e) Enable supported interrupts
 *               - (f) Enable the transmitter and receiver
 *               - (g) Start / Enable DMA if required
 *
 * @param    p_if    Pointer to a network interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Many DMA devices may generate only one interrupt for several ready receive
 *               descriptors.  In order to accommodate this, it is recommended that all DMA
 *               based drivers count the number of ready receive descriptors during the
 *               receive event and signal the receive task accordingly ONLY for those
 *               NEW descriptors which have not yet been accounted for.  Each time a
 *               descriptor is processed (or discarded) the count for acknowledged and
 *               unprocessed frames should be decremented by 1.  This function initializes the
 *               acknowledged receive descriptor count to 0.
 *
 * @note     (3) Setting the maximum number of frames queued for transmission is optional.  By
 *               default, all network interfaces are configured to block until the previous frame
 *               has completed transmission.  However, DMA based interfaces may have several
 *               frames configured for transmission before blocking is required. The number
 *               of queued transmit frames depends on the number of configured transmit
 *               descriptors.
 *
 * @note     (4) The physical hardware address should not be configured from NetDev_Init(). Instead,
 *               it should be configured from within NetDev_Start() to allow for the proper use
 *               of NetIF_Ether_HW_AddrSet(), hard coded hardware addresses from the device
 *               configuration structure, or auto-loading EEPROM's. Changes to the physical address
 *               only take effect when the device transitions from the DOWN to UP state.
 *
 * @note     (5) The device hardware address is set from one of the data sources below. Each source
 *               is listed in the order of precedence.
 *               - (a) NetIF_Ether_HW_AddrSet()             Call NetIF_Ether_HW_AddrSet() if the HW
 *                                                          address needs to be configured via
 *                                                          run-time from a different data
 *                                                          source. E.g. Non auto-loading
 *                                                          memory such as I2C or SPI EEPROM.
 *                                                          (see Note #3).
 *               - (b) Device Configuration Structure       Configure a valid HW address during
 *                                                          compile time.
 *               - (c) Auto-Loading via EEPROM.             If neither options a) or b) are used,
 *                                                          the IF layer will use the HW address
 *                                                          obtained from the network hardware
 *                                                          address registers.
 *******************************************************************************************************/
static void NetDev_Start(NET_IF   *p_if,
                         RTOS_ERR *p_err)
{
  NET_DEV_BSP_ETHER *p_dev_bsp;
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  CPU_INT08U        hw_addr[NET_IF_ETHER_ADDR_SIZE];
  CPU_INT08U        hw_addr_len;
  CPU_BOOLEAN       hw_addr_cfg;
  RTOS_ERR          local_err;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev_bsp = (NET_DEV_BSP_ETHER *)p_if->Dev_BSP;               // Obtain ptr to dev BSP.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               Set power requirement
  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  #endif

  //                                                               ---------------- CFG TX RDY SIGNAL -----------------
  NetIF_DevCfgTxRdySignal(p_if,                                 // See Note #3.
                          p_dev_cfg->TxDescNbr);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------- CFG HW ADDR --------------------
  hw_addr_cfg = DEF_NO;                                         // See Notes #4 & #5.

  //                                                               Get  app-configured IF layer HW MAC address, ...
  //                                                               ... if any (see Note #4a).
  hw_addr_len = sizeof(hw_addr);
  NetIF_AddrHW_GetHandler(p_if->Nbr, &hw_addr[0u], &hw_addr_len, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    hw_addr_cfg = NetIF_AddrHW_IsValidHandler(p_if->Nbr, &hw_addr[0u], &local_err);
  }

  if (hw_addr_cfg != DEF_YES) {                                 // Else get configured HW MAC address string, if any ...
                                                                // ... (see Note #4b).
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetASCII_Str_to_MAC(p_dev_cfg->HW_AddrStr,                  // Check if configured HW MAC address format is valid.
                        &hw_addr[0u],
                        &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      NetIF_AddrHW_SetHandler(p_if->Nbr,                        // Check if configured HW MAC address is valid.
                              &hw_addr[0u],                     // return error if invalid.
                              sizeof(hw_addr),
                              &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
        return;
      }

      hw_addr_cfg = DEF_YES;                                    // If no errors, configure device    HW MAC address.
    } else {
      //                                                           Else attempt to get device's automatically loaded ...
      //                                                           ... HW MAC address, if any (see Note #4c).
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (hw_addr_cfg == DEF_YES) {                                 // If necessary, set device HW MAC address.
    p_dev->SPEC_ADDR1_BOT = (((CPU_INT32U)hw_addr[0] << (0 * DEF_INT_08_NBR_BITS))
                             | ((CPU_INT32U)hw_addr[1] << (1 * DEF_INT_08_NBR_BITS))
                             | ((CPU_INT32U)hw_addr[2] << (2 * DEF_INT_08_NBR_BITS))
                             | ((CPU_INT32U)hw_addr[3] << (3 * DEF_INT_08_NBR_BITS)));

    p_dev->SPEC_ADDR1_TOP = (((CPU_INT32U)hw_addr[4] << (0 * DEF_INT_08_NBR_BITS))
                             | ((CPU_INT32U)hw_addr[5] << (1 * DEF_INT_08_NBR_BITS)));
  }

  //                                                               --------------- INIT DMA DESCRIPTORS ---------------
  NetDev_RxDescInit(p_if, p_err);                               // Initialize Rx descriptors.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_data->RxNRdyCtr = 0;                                    // No pending frames to process (see Note #3).

  NetDev_TxDescInit(p_if, p_err);                               // Initialize Tx descriptors.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev->SYSWAKE = SYSWAKE_TIME;                                // PHY Wake up time: 32 us in 100BASE-TX
  p_dev_data->EnableLPI = DEF_FALSE;                            // LPI is Disabled by default

  //                                                               -------------------- CFG INT'S ---------------------
  p_dev->INTR_STATUS |= INT_STATUS_MASK_ALL;                    // Clear all pending int. sources.
  p_dev->RX_STATUS = 0xFFFFFFFF;
  p_dev->TX_STATUS = 0xFFFFFFFF;
  p_dev->INTR_EN = INT_STATUS_MASK_SUPPORTED;                   // Enable Rx, Tx and other supported int. sources.

  p_dev->USER_IO = DEF_BIT_00;

  p_dev->NET_CFG |= GEM_BIT_CFG_FCS_REMOVE                      // Remove check seq, enable uni/multi hash.
                    | GEM_BIT_CFG_UNI_HASH_EN
                    | GEM_BIT_CFG_MULTI_HASH_EN;

  p_dev->NET_CFG |= GEM_BIT_CFG_FULL_DUPLEX                     // Full speed by default.
                    | GEM_BIT_CFG_GIGE_EN
                    | GEM_BIT_CFG_SPEED;

  p_dev->DMA_CFG |= GEM_BIT_DMACFG_AHB_RX_SIZE(0x18u)           // Max fifo depth.
                    | GEM_BIT_DMACFG_CSUM_GEN_OFFLOAD_EN        // Checksum offloading.
                    | GEM_BIT_DMACFG_RX_AHB_BURST(0x1F)         // Max AHB burst length.
                    | GEM_BIT_DMACFG_DISC_WHEN_NO_AHB;          // Discard frame on overflow.

  p_dev->DMA_CFG &= ~GEM_BIT_DMACFG_AHB_PKT_SWAP_EN;

  CPU_MB();
  //                                                               ------------------ ENABLE RX & TX ------------------
  p_dev->NET_CTRL |= GEM_BIT_CTRL_TX_EN                         // Enable transmitter & receiver.
                     | GEM_BIT_CTRL_RX_EN
                     | GEM_BIT_CTRL_MGMT_PORT_EN;

  CPU_MB();

  p_dev_bsp->CfgClk(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                               NetDev_Stop()
 *
 * @brief    (1) Shutdown network interface hardware :
 *               - (a) Disable the receiver and transmitter
 *               - (b) Disable receive and transmit interrupts
 *               - (c) Clear pending interrupt requests
 *               - (d) Free ALL receive descriptors (Return ownership to hardware)
 *               - (e) Deallocate ALL transmit buffers
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) It is recommended that a device driver should only post all currently-used,
 *               i.e. not-fully-transmitted, transmit buffers to the network interface transmit
 *               deallocation queue.
 *               - (a) However, a driver MAY attempt to post all queued &/or transmitted buffers.
 *                     The network interface transmit deallocation task will silently ignore any
 *                     unknown or duplicate transmit buffers.  This allows device drivers to
 *                     indiscriminately & easily post all transmit buffers without determining
 *                     which buffers have NOT yet been transmitted.
 * @note     (3) Device drivers should assume that the network interface transmit deallocation
 *               queue is large enough to post all currently-used transmit buffers.
 *               - (a) If the transmit deallocation queue is NOT large enough to post all transmit
 *                     buffers, some transmit buffers may/will be leaked/lost.
 *
 * @note     (4) All functions that require device register access MUST obtain reference to the
 *               device hardware register space PRIOR to attempting to access any registers.
 *               Register definitions SHOULD NOT be absolute & SHOULD use the provided base
 *               address within the device configuration structure, as well as the device
 *               register definition structure in order to properly resolve register addresses
 *               during run-time.
 *******************************************************************************************************/
static void NetDev_Stop(NET_IF   *p_if,
                        RTOS_ERR *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  DEV_DESC          *p_desc;
  CPU_INT08U        i;
  RTOS_ERR          local_err;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               ----------------- DISABLE RX & TX ------------------
  p_dev->NET_CTRL &= ~(GEM_BIT_CTRL_TX_EN                       // Disable transmitter & receiver.
                       | GEM_BIT_CTRL_RX_EN);

  //                                                               -------------- DISABLE & CLEAR INT'S ---------------
  p_dev->INTR_DIS |= INT_STATUS_MASK_SUPPORTED;                 // Disable Rx, Tx and other supported int. sources.
  p_dev->INTR_STATUS |= INT_STATUS_MASK_ALL;                    // Clear all pending int. sources.

  //                                                               --------------- FREE RX DESCRIPTORS ----------------
  NetDev_RxDescFreeAll(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------- FREE USED TX DESCRIPTORS -------------
  p_desc = &p_dev_data->TxBufDescPtrStart[0];
  for (i = 0; i < p_dev_cfg->TxDescNbr; i++) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    if (DEF_BIT_IS_SET(p_desc->Status, GEM_TXBUF_USED)) {       // If NOT yet  tx'd, ...
                                                                // ... dealloc tx buf (see Note #2a1).
      NetIF_TxDeallocQPost((CPU_INT08U *)(p_desc->Addr & GEM_TXBUF_ADDR_MASK), &local_err);
      PP_UNUSED_PARAM(local_err);                               // Ignore possible dealloc err (see Note #2b2).
    }
    p_desc++;
  }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  //                                                               Remove power requirement
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif
}

/****************************************************************************************************//**
 *                                               NetDev_Rx()
 *
 * @brief    (1) This function returns a pointer to the received data to the caller :
 *               - (a) Decrement frame counter
 *               - (b) Determine which receive descriptor caused the interrupt
 *               - (c) Obtain pointer to data area to replace existing data area
 *               - (d) Reconfigure descriptor with pointer to new data area
 *               - (e) Set return values.  Pointer to received data area and size
 *               - (f) Update current receive descriptor pointer
 *               - (g) Increment statistic counters
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_data  Pointer to pointer to received DMA data area. The received data
 *                   area address should be returned to the stack by dereferencing
 *                   p_data as *p_data = (address of receive data area).
 *
 * @param    size    Pointer to size. The number of bytes received should be returned
 *                   to the stack by dereferencing size as *size = (number of bytes).
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *
 * @note     (3) If a receive error occurs and the descriptor is invalid then the function
 *               SHOULD return 0 for the size, a NULL pointer to the data area AND an
 *               error equal to NET_DEV_ERR_RX.
 *               - (a) If the next expected ready / valid descriptor is NOT owned by
 *                     software, then there is descriptor pointer corruption and the
 *                     driver should NOT increment the current receive descriptor
 *                     pointer.
 *               - (b) If the descriptor IS valid, but an error is indicated within
 *                     the descriptor status bits, or length field, then the driver
 *                     MUST increment the current receive descriptor pointer and discard
 *                     the received frame.
 *               - (c) If a new data area is unavailable, the driver MUST increment
 *                     the current receive descriptor pointer and discard the received
 *                     frame.  This will invoke the DMA to re-use the existing configured
 *                     data area.
 *
 * @note     (4) Some devices optionally include each receive packet's CRC in the received
 *               packet data & size.
 *               - (a) CRCs might optionally be included at run-time or at build time. Each
 *                     driver doesn't necessarily need to conditionally include or exclude
 *                     the CRC at build time.  Instead, a device may include/exclude the code
 *                     to subtract the CRC size from the packet size.
 *               - (b) The CRC size should be subtracted from the receive packet size ONLY if
 *                     the CRC was included in the received packet data.
 *******************************************************************************************************/
static void NetDev_Rx(NET_IF     *p_if,
                      CPU_INT08U **p_data,
                      CPU_INT16U *p_size,
                      RTOS_ERR   *p_err)
{
  NET_DEV_DATA *p_dev_data;
  DEV_DESC     *p_desc;
  CPU_INT08U   *pbuf_new;
  CPU_INT32U   rx_len;
  CPU_INT32U   addr;

  //                                                               ------- OBTAIN REFERENCE TO DEVICE CFG/DATA --------
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_desc = (DEV_DESC *)p_dev_data->RxBufDescPtrCur;             // Obtain ptr to next ready descriptor.

  addr = p_desc->Addr;

  //                                                               ------------- CHECK FOR RECEIVE ERRORS -------------
  if ((addr & 1u) == 0) {                                       // See Note #3a.
    *p_size = 0u;
    *p_data = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }
  //                                                               --------------- OBTAIN FRAME LENGTH ----------------
  rx_len = (p_desc->Status & GEM_RXBUF_SIZE_MASK);

  if (rx_len < NET_IF_ETHER_FRAME_MIN_SIZE) {                   // If frame is a runt, ...
    NetDev_RxDescPtrCurInc(p_if);                               // ... discard rx'd frame    (see Note #3b).
    *p_size = 0u;
    *p_data = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }

  //                                                               --------- OBTAIN PTR TO NEW DMA DATA AREA ----------
  //                                                               Request an empty buffer.
  pbuf_new = NetBuf_GetDataPtr(p_if,
                               NET_TRANSACTION_RX,
                               NET_IF_ETHER_FRAME_MAX_SIZE,
                               NET_IF_IX_RX,
                               DEF_NULL,
                               DEF_NULL,
                               DEF_NULL,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // If unable to get a buffer (see Note #3c).
    NetDev_RxDescPtrCurInc(p_if);                               // Free the current descriptor.
    *p_size = 0u;
    *p_data = DEF_NULL;
    return;
  }

  *p_size = rx_len;                                             // Return the size of the received frame.
  *p_data = (CPU_INT08U *)(addr & GEM_RXBUF_ADDR_MASK);         // Return a pointer to the newly received data area.

  CPU_DCACHE_RANGE_INV(*p_data, *p_size);                       // Invalidate received buffer.

  if (p_desc == p_dev_data->RxBufDescPtrEnd) {                  // Update the descriptor to point to a new data area
    p_desc->Addr = ((CPU_INT32U)pbuf_new & GEM_RXBUF_ADDR_MASK) | GEM_RXBUF_ADDR_OWN | GEM_RXBUF_ADDR_WRAP;
  } else {
    p_desc->Addr = ((CPU_INT32U)pbuf_new & GEM_RXBUF_ADDR_MASK) | GEM_RXBUF_ADDR_OWN;
  }

  NetDev_RxDescPtrCurInc(p_if);                                 // Free the current descriptor.
}

/****************************************************************************************************//**
 *                                               NetDev_Tx()
 *
 * @brief    (1) This function transmits the specified data :
 *               - (a) Check if the transmitter is ready.
 *               - (b) Configure the next transmit descriptor for pointer to data and data size.
 *               - (c) Issue the transmit command.
 *               - (d) Increment pointer to next transmit descriptor
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_data  Pointer to data to transmit.
 *
 * @param    size    Size    of data to transmit.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *
 * @note     (3) Care should be taken to avoid skipping transmit descriptors while selecting
 *               the next available descriptor.  Software MUST track the descriptor which
 *               is expected to generate the next transmit complete interrupt.  Skipping
 *               descriptors, unless carefully accounted for, may make it difficult to
 *               know which descriptor will complete transmission next.  Some device
 *               drivers may find it useful to adjust p_dev_data->TxBufDescCompPtr after
 *               having selected the next available transmit descriptor.
 *******************************************************************************************************/
static void NetDev_Tx(NET_IF     *p_if,
                      CPU_INT08U *p_data,
                      CPU_INT16U size,
                      RTOS_ERR   *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  DEV_DESC          *p_desc;
  CPU_INT32U        desc_status;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.
  p_desc = (DEV_DESC *)p_dev_data->TxBufDescPtrCur;             // Obtain ptr to next available Tx descriptor.

  desc_status = p_desc->Status;

  if (p_dev->NET_CTRL & GEM_BIT_CTRL_TXLPIEN) {
    NetDev_DeAssertLPI_TX(p_if);                                // Wake up from LPI
  }

  if (desc_status & GEM_TXBUF_USED) {                           // Find next available Tx descriptor (see Note #3).
    p_desc->Addr = (CPU_INT32U)p_data & GEM_TXBUF_ADDR_MASK;    // Configure descriptor with Tx data area address.

    CPU_DCACHE_RANGE_FLUSH(p_data, size);                       // Flush/Clean buffer to send.

    if (p_dev_data->TxBufDescPtrCur == p_dev_data->TxBufDescPtrEnd) {
      p_desc->Status = GEM_TXBUF_WRAP | (((size) & GEM_TXBUF_LENGTH_MASK) | GEM_TXBUF_LAST);
    } else {
      p_desc->Status = (((size) & GEM_TXBUF_LENGTH_MASK) | GEM_TXBUF_LAST);
    }

    //                                                             Update curr desc ptr to point to next desc.
    if (p_dev_data->TxBufDescPtrCur != p_dev_data->TxBufDescPtrEnd) {
      p_dev_data->TxBufDescPtrCur++;
    } else {
      p_dev_data->TxBufDescPtrCur = p_dev_data->TxBufDescPtrStart;
    }

    CPU_MB();                                                   // Force writes to buf & desc to be visible to the MAC.

    p_dev->NET_CTRL |= GEM_BIT_CTRL_START_TX;

    p_desc = p_dev_data->TxBufDescCompPtr;
    p_dev->INTR_EN = GEM_BIT_INT_TX_COMPLETE;
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                           NetDev_AddrMulticastAdd()
 *
 * @brief    Configure hardware address filtering to accept specified hardware address.
 *
 * @param    p_if        Pointer to an Ethernet network interface.
 *
 * @param    paddr_hw    Pointer to hardware address.
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetDev_AddrMulticastAdd(NET_IF     *p_if,
                                    CPU_INT08U *paddr_hw,
                                    CPU_INT08U addr_hw_len,
                                    RTOS_ERR   *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_DEV           *p_dev;
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  CPU_INT08U        bit_nbr;
  CPU_INT08U        *p_addr_hash_ctrs;
  CORE_DECLARE_IRQ_STATE;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               Calculate the 6-bit hash value.
  bit_nbr = ((paddr_hw[0]       & 0x3F)                              & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[0] >> 6) & 0x3) | ((paddr_hw[1] & 0xF) << 2)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[1] >> 4) & 0xF) | ((paddr_hw[2] & 0x3) << 4)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[2] >> 2) & 0x3F))                             & DEF_BIT_FIELD(6, 0u))
            ^   ((paddr_hw[3]       & 0x3F)                              & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[3] >> 6) & 0x3) | ((paddr_hw[4] & 0xF) << 2)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[4] >> 4) & 0xF) | ((paddr_hw[5] & 0x3) << 4)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[5] >> 2) & 0x3F))                             & DEF_BIT_FIELD(6, 0u));

  CORE_ENTER_ATOMIC();
  p_addr_hash_ctrs = &p_dev_data->MulticastAddrHashBitCtr[bit_nbr];
  (*p_addr_hash_ctrs)++;                                        // Increment hash bit reference ctr.

  if (*p_addr_hash_ctrs == 1u) {
    if (bit_nbr > 31u) {
      DEF_BIT_SET(p_dev->HASH_TOP, DEF_BIT(bit_nbr - 32u));
    } else {
      DEF_BIT_SET(p_dev->HASH_BOT, DEF_BIT(bit_nbr));
    }
  }
  CORE_EXIT_ATOMIC();

#else
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(paddr_hw);
#endif

  PP_UNUSED_PARAM(addr_hw_len);                                 // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                       NetDev_AddrMulticastRemove()
 *
 * @brief    Configure hardware address filtering to reject specified hardware address.
 *
 * @param    p_if        Pointer to an Ethernet network interface.
 *
 * @param    paddr_hw    Pointer to hardware address.
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetDev_AddrMulticastRemove(NET_IF     *p_if,
                                       CPU_INT08U *paddr_hw,
                                       CPU_INT08U addr_hw_len,
                                       RTOS_ERR   *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_DEV           *p_dev;
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  CPU_INT08U        bit_nbr;
  CPU_INT08U        *p_addr_hash_ctrs;
  CORE_DECLARE_IRQ_STATE;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  bit_nbr = ((paddr_hw[0]       & 0x3F)                              & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[0] >> 6) & 0x3) | ((paddr_hw[1] & 0xF) << 2)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[1] >> 4) & 0xF) | ((paddr_hw[2] & 0x3) << 4)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[2] >> 2) & 0x3F))                             & DEF_BIT_FIELD(6, 0u))
            ^   ((paddr_hw[3]       & 0x3F)                              & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[3] >> 6) & 0x3) | ((paddr_hw[4] & 0xF) << 2)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[4] >> 4) & 0xF) | ((paddr_hw[5] & 0x3) << 4)) & DEF_BIT_FIELD(6, 0u))
            ^ ((((paddr_hw[5] >> 2) & 0x3F))                             & DEF_BIT_FIELD(6, 0u));

  CORE_ENTER_ATOMIC();
  p_addr_hash_ctrs = &p_dev_data->MulticastAddrHashBitCtr[bit_nbr];

  if (*p_addr_hash_ctrs > 1u) {
    (*p_addr_hash_ctrs)--;                                      // Decrement hash bit reference ctr.
                                                                // CORE_EXIT_ATOMIC();
                                                                // TODO_NET: validate
  }

  *p_addr_hash_ctrs = 0u;

  if (bit_nbr > 31u) {
    DEF_BIT_CLR(p_dev->HASH_TOP, DEF_BIT(bit_nbr - 32u));
  } else {
    DEF_BIT_CLR(p_dev->HASH_BOT, DEF_BIT(bit_nbr));
  }

  CORE_EXIT_ATOMIC();

#else
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(paddr_hw);
#endif

  PP_UNUSED_PARAM(addr_hw_len);                                 // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                           NetDev_ISR_Handler()
 *
 * @brief    This function serves as the device Interrupt Service Routine Handler. This ISR
 *           handler MUST service and clear all necessary and enabled interrupt events for
 *           the device.
 *
 * @param    p_if    Pointer to interface requiring service.
 *
 * @param    type    Network Interface defined argument representing the type of ISR in progress. Codes
 *                   for Rx, Tx, Overrun, Jabber, etc... are defined within net_if.h and are passed
 *                   into this function by the corresponding Net BSP ISR handler function. The Net
 *                   BSP ISR handler function may be called by a specific ISR vector and therefore
 *                   know which ISR type code to pass.  Otherwise, the Net BSP may pass
 *                   NET_DEV_ISR_TYPE_UNKNOWN and the device driver MAY ignore the parameter when
 *                   the ISR type can be deduced by reading an available interrupt status register.
 *                   Type codes that are defined within net_if.c include but are not limited to :
 *                   NET_DEV_ISR_TYPE_RX
 *                   NET_DEV_ISR_TYPE_TX_COMPLETE
 *                   NET_DEV_ISR_TYPE_UNKNOWN
 *
 * @note     (1) This function is called via function pointer from the context of an ISR.
 *
 * @note     (2) In the case of an interrupt occurring prior to Network Protocol Stack initialization,
 *               the device driver should ensure that the interrupt source is cleared in order
 *               to prevent the potential for an infinite interrupt loop during system initialization.
 *
 * @note     (3) Many DMA devices generate only one interrupt event for several ready receive
 *               descriptors.  In order to accommodate this, it is recommended that all DMA based
 *               drivers count the number of ready receive descriptors during the receive event
 *               and signal the receive task for ONLY newly received descriptors which have not
 *               yet been signaled for during the last receive interrupt event.
 *
 * @note     (4) Many DMA devices generate only one interrupt event for several transmit
 *               complete descriptors.  In this case, the driver MUST determine which descriptors
 *               have completed transmission and post each descriptor data area address to
 *               the transmit deallocation task.  The code provided below assumes one
 *               interrupt per transmit event which may not necessarily be the case for all
 *               devices.
 *******************************************************************************************************/
static void NetDev_ISR_Handler(NET_IF           *p_if,
                               NET_DEV_ISR_TYPE type)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  DEV_DESC          *p_desc;
  CPU_DATA          reg_val;
  CPU_INT32U        int_clr;
  CPU_INT08U        *p_data;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(type);                                        // Prevent 'variable unused' compiler warning.

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               ---------------- DETERMINE ISR TYPE ----------------
  reg_val = p_dev->INTR_STATUS;
  int_clr = 0u;

  //                                                               HANDLE RX ISRs
  if ((reg_val & RX_ISR_EVENT_MSK) > 0) {
    NetIF_RxQPost(p_if->Nbr, &err);                             // Signal Net Task.

    p_dev->INTR_DIS = GEM_BIT_INT_RX_COMPLETE;
    int_clr = RX_ISR_EVENT_MSK;                                 // Clear device Rx interrupt event flag.
  }

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  //                                                               HANDLE TX ISRs
  if ((reg_val & TX_ISR_EVENT_MSK) > 0) {
    p_desc = p_dev_data->TxBufDescCompPtr;

    while (p_desc->Addr != DEF_NULL) {
      if (DEF_BIT_IS_SET(p_desc->Status, GEM_TXBUF_USED)) {
        p_data = (CPU_INT08U *)p_desc->Addr;
        NetIF_TxDeallocQPost(p_data, &err);
        NetIF_DevTxRdySignal(p_if);                             // Signal Net IF that Tx resources are available.
        if (p_dev_data->TxBufDescCompPtr != p_dev_data->TxBufDescPtrEnd) {
          p_dev_data->TxBufDescCompPtr++;
        } else {
          p_dev_data->TxBufDescCompPtr = p_dev_data->TxBufDescPtrStart;
        }

        p_desc->Addr = DEF_NULL;
        p_desc = p_dev_data->TxBufDescCompPtr;
      } else {
        break;
      }
    }
    //                                                             Check if Tx Q is empty
    if ((p_dev_data->TxBufDescPtrCur == p_dev_data->TxBufDescCompPtr) && (p_dev_data->EnableLPI == DEF_TRUE)) {
      NetDev_AssertLPI_TX(p_if);                                // Enter LPI
    }

    int_clr |= TX_ISR_EVENT_MSK;                                // Clear device Tx interrupt event flag.
  }

  //                                                               HANDLE MISC ISRs
  int_clr |= UNHANDLED_ISR_EVENT_MASK;
  p_dev->INTR_STATUS = int_clr;                                 // Clear unhandled interrupt event flag.
}

/****************************************************************************************************//**
 *                                               NetDev_IO_Ctrl()
 *
 * @brief    This function provides a mechanism for the Phy driver to update the MAC link
 *           and duplex settings, as well as a method for the application and link state
 *           timer to obtain the current link status.  Additional user specified driver
 *           functionality MAY be added if necessary.
 *
 * @param    p_if    Pointer to interface requiring service.
 *
 * @param    opt     Option code representing desired function to perform. The Network Protocol Suite
 *                   specifies the option codes below. Additional option codes may be defined by the
 *                   driver developer in the driver's header file.
 *                   NET_IF_IO_CTRL_LINK_STATE_GET
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE
 *                   Driver defined operation codes MUST be defined starting from 20 or higher
 *                   to prevent clashing with the pre-defined operation code types. See the
 *                   device driver header file for more details.
 *
 * @param    p_data  Pointer to optional data for either sending or receiving additional function
 *                   arguments or return data.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (1) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *******************************************************************************************************/
static void NetDev_IO_Ctrl(NET_IF     *p_if,
                           CPU_INT08U opt,
                           void       *p_data,
                           RTOS_ERR   *p_err)
{
  NET_DEV_BSP_ETHER  *p_dev_bsp;
  NET_DEV_LINK_ETHER *p_link_state;
  NET_DEV_CFG_ETHER  *p_dev_cfg;
  NET_DEV_DATA       *p_dev_data;
  NET_DEV            *p_dev;
  NET_PHY_API_ETHER  *p_phy_api;
  CPU_INT16U         duplex;
  CPU_INT16U         spd;

  //                                                               ------- OBTAIN REFERENCE TO DEVICE REGISTERS -------
  p_dev_bsp = (NET_DEV_BSP_ETHER *)p_if->Dev_BSP;
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.

  //                                                               ----------- PERFORM SPECIFIED OPERATION ------------
  switch (opt) {
    case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:
      p_link_state = (NET_DEV_LINK_ETHER *)p_data;
      RTOS_ASSERT_DBG_ERR_SET((p_link_state != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      p_phy_api = (NET_PHY_API_ETHER *)p_if->Ext_API;
      RTOS_ASSERT_DBG_ERR_SET((p_phy_api != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      p_phy_api->LinkStateGet(p_if, p_link_state, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      break;

    case NET_IF_IO_CTRL_LINK_STATE_UPDATE:
      p_link_state = (NET_DEV_LINK_ETHER *)p_data;

      duplex = NET_PHY_DUPLEX_UNKNOWN;
      if (p_link_state->Duplex != duplex) {
        switch (p_link_state->Duplex) {
          case NET_PHY_DUPLEX_FULL:
            p_dev->NET_CFG |= GEM_BIT_CFG_FULL_DUPLEX;
            break;

          case NET_PHY_DUPLEX_HALF:
            p_dev->NET_CFG &= ~GEM_BIT_CFG_FULL_DUPLEX;
            break;

          default:
            break;
        }
      }

      spd = NET_PHY_SPD_0;
      if (p_link_state->Spd != spd) {
        switch (p_link_state->Spd) {
          case NET_PHY_SPD_10:
            p_dev->NET_CFG &= ~GEM_BIT_CFG_SPEED;
            p_dev->NET_CFG &= ~GEM_BIT_CFG_GIGE_EN;
            break;

          case NET_PHY_SPD_100:
            p_dev->NET_CFG |= GEM_BIT_CFG_SPEED;
            p_dev->NET_CFG &= ~GEM_BIT_CFG_GIGE_EN;
            break;

          case NET_PHY_SPD_1000:
            p_dev->NET_CFG |= GEM_BIT_CFG_GIGE_EN | GEM_BIT_CFG_SPEED;
            break;

          default:
            break;
        }
      }
      break;

    case NET_IF_IO_CTRL_EEE_GET_INFO:
      *(CPU_BOOLEAN *)p_data = p_dev_data->EnableLPI;
      break;

    case NET_IF_IO_CTRL_EEE:
      p_dev_data->EnableLPI = *(CPU_BOOLEAN *)p_data;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO_FATAL);
      break;
  }

  if (opt == NET_IF_IO_CTRL_LINK_STATE_UPDATE) {
    p_dev_bsp->CfgClk(p_if, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                               NetDev_MII_Rd()
 *
 * @brief    Write data over the (R)MII bus to the specified Phy register.
 *
 * @param    p_if        Pointer to the interface requiring service.
 *
 * @param    phy_addr    (R)MII bus address of the Phy requiring service.
 *
 * @param    reg_addr    Phy register number to write to.
 *
 * @param    p_data      Pointer to variable to store returned register data.
 *
 * @param    p_err       Pointer to return error code.
 *
 * @note     (1) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *******************************************************************************************************/
static void NetDev_MII_Rd(NET_IF     *p_if,
                          CPU_INT08U phy_addr,
                          CPU_INT08U reg_addr,
                          CPU_INT16U *p_data,
                          RTOS_ERR   *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV           *p_dev;
  CPU_INT32U        timeout;
  CPU_INT32U        phy_word;

  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  PP_UNUSED_PARAM(p_err);

  //                                                               ------- OBTAIN REFERENCE TO DEVICE REGISTERS -------
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               ------------ PERFORM MII READ OPERATION ------------

  phy_word = GEM_BIT_PHYMGMT_OPERATION(2u)
             | GEM_BIT_PHYMGMT_CLAUSE_22
             | GEM_BIT_PHYMGMT_MUST10(2u)
             | GEM_BIT_PHYMGMT_PHYADDR(phy_addr)
             | GEM_BIT_PHYMGMT_REGADDR(reg_addr)
             | GEM_BIT_PHYMGMT_DATA(0u);

  p_dev->PHY_MAINT = phy_word;

  for (timeout = 0; timeout < 10000u; timeout++) {
    if (DEF_BIT_IS_SET(p_dev->NET_STATUS, GEM_BIT_STATUS_PHY_MGMT_IDLE)) {
      break;
    }
  }

  *p_data = p_dev->PHY_MAINT & GEM_BIT_PHYMGMT_DATA_MSK;
}

/****************************************************************************************************//**
 *                                               NetDev_MII_Wr()
 *
 * @brief    Write data over the (R)MII bus to the specified Phy register.
 *
 * @param    p_if        Pointer to the interface requiring service.
 *
 * @param    phy_addr    (R)MII bus address of the Phy requiring service.
 *
 * @param    reg_addr    Phy register number to write to.
 *
 * @param    data        Data to write to the specified Phy register.
 *
 * @param    p_err       Pointer to return error code.
 *
 * @note     (1) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *******************************************************************************************************/
static void NetDev_MII_Wr(NET_IF     *p_if,
                          CPU_INT08U phy_addr,
                          CPU_INT08U reg_addr,
                          CPU_INT16U data,
                          RTOS_ERR   *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV           *p_dev;
  CPU_INT32U        timeout;
  CPU_INT32U        phy_word;

  PP_UNUSED_PARAM(p_err);

  //                                                               ------- OBTAIN REFERENCE TO DEVICE REGISTERS -------
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               ------------ PERFORM MII READ OPERATION ------------

  phy_word = GEM_BIT_PHYMGMT_OPERATION(1u)
             | GEM_BIT_PHYMGMT_CLAUSE_22
             | GEM_BIT_PHYMGMT_MUST10(2u)
             | GEM_BIT_PHYMGMT_PHYADDR(phy_addr)
             | GEM_BIT_PHYMGMT_REGADDR(reg_addr)
             | GEM_BIT_PHYMGMT_DATA(data);

  p_dev->PHY_MAINT = phy_word;

  for (timeout = 0; timeout < 10000u; timeout++) {
    if (DEF_BIT_IS_SET(p_dev->NET_STATUS, GEM_BIT_STATUS_PHY_MGMT_IDLE)) {
      break;
    }
  }
}

/****************************************************************************************************//**
 *                                           NetDev_RxDescInit()
 *
 * @brief    (1) This function initializes the Rx descriptor list for the specified interface :
 *               - (a) Obtain reference to the Rx descriptor(s) memory block
 *               - (b) Initialize Rx descriptor pointers
 *               - (c) Obtain Rx descriptor data areas
 *               - (d) Initialize hardware registers
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) Memory allocation for the descriptors MUST be performed BEFORE calling this
 *               function. This ensures that multiple calls to this function do NOT allocate
 *               additional memory to the interface and that the Rx descriptors may be safely
 *               re-initialized by calling this function.
 *
 * @note     (3) All Rx descriptors are allocated as ONE memory block.  This removes the
 *               necessity to ensure that descriptor blocks are returned to the descriptor
 *               pool in the opposite order in which they were allocated; doing so would
 *               ensure that each memory block address was contiguous to the one before
 *               and after it.  If the descriptors are NOT contiguous, then software
 *               MUST NOT assign a pointer to the pool start address and use pointer
 *               arithmetic to navigate the descriptor list.  Since pointer arithmetic
 *               is a convenient way to navigate the descriptor list, ONE block is allocated
 *               and the driver uses pointer arithmetic to slice the block into descriptor
 *               sized units.
 *******************************************************************************************************/
static void NetDev_RxDescInit(NET_IF   *p_if,
                              RTOS_ERR *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  DEV_DESC          *p_desc;
  void              *p_buf;
  CPU_INT16U        i;

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               -------------- ALLOCATE DESCRIPTORS  ---------------
  //                                                               See Note #3.
  p_desc = p_dev_data->RxDescMemBlck;
  //                                                               -------------- INIT DESCRIPTOR PTRS  ---------------
  p_dev_data->RxBufDescPtrStart = (DEV_DESC *)p_desc;
  p_dev_data->RxBufDescPtrCur = (DEV_DESC *)p_desc;
  p_dev_data->RxBufDescPtrEnd = (DEV_DESC *)p_desc + (p_dev_cfg->RxDescNbr - 1);

  //                                                               --------------- INIT RX DESCRIPTORS ----------------
  for (i = 0; i < p_dev_cfg->RxDescNbr; i++) {
    p_desc->Status = 0;
    p_buf = (void *)NetBuf_GetDataPtr(p_if,
                                      NET_TRANSACTION_RX,
                                      NET_IF_ETHER_FRAME_MAX_SIZE,
                                      NET_IF_IX_RX,
                                      DEF_NULL,
                                      DEF_NULL,
                                      DEF_NULL,
                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    CPU_DCACHE_RANGE_FLUSH(p_buf, p_dev_cfg->RxBufLargeSize);

    p_desc->Addr = (CPU_INT32U)p_buf;

    p_desc->Addr &= ~GEM_RXBUF_ADDR_OWN;

    if (p_desc == (p_dev_data->RxBufDescPtrEnd)) {              // Set WRAP bit on last descriptor in list.
      p_desc->Addr |= GEM_RXBUF_ADDR_WRAP;
    }

    p_desc++;                                                   // Point to next descriptor in list.
  }

  //                                                               ------------- INIT HARDWARE REGISTERS --------------
  //                                                               Configure the DMA with the Rx desc start address.
  p_dev->RX_QBAR = (CPU_INT32U)p_dev_data->RxBufDescPtrStart;
}

/****************************************************************************************************//**
 *                                           NetDev_RxDescFreeAll()
 *
 * @brief    (1) This function returns the descriptor memory block and descriptor data area
 *               memory blocks back to their respective memory pools :
 *               - (a) Free Rx descriptor data areas
 *               - (b) Free Rx descriptor memory block
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) No mechanism exists to free a memory pool.  However, ALL receive buffers
 *               and the Rx descriptor blocks MUST be returned to their respective pools.
 *******************************************************************************************************/
static void NetDev_RxDescFreeAll(NET_IF   *p_if,
                                 RTOS_ERR *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  DEV_DESC          *p_desc;
  CPU_INT08U        *p_desc_data;
  CPU_INT16U        i;

  PP_UNUSED_PARAM(p_err);

  //                                                               ------- OBTAIN REFERENCE TO DEVICE CFG/DATA --------
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.

  //                                                               ------------- FREE RX DESC DATA AREAS --------------
  p_desc = p_dev_data->RxBufDescPtrStart;
  for (i = 0; i < p_dev_cfg->RxDescNbr; i++) {                  // Free Rx descriptor ring.
    p_desc_data = (CPU_INT08U *)(p_desc->Addr & GEM_RXBUF_ADDR_MASK);
    NetBuf_FreeBufDataAreaRx(p_if->Nbr, p_desc_data);           // Return data area to Rx data area pool.
    p_desc++;
  }
}

/****************************************************************************************************//**
 *                                           NetDev_RxDescPtrCurInc()
 *
 * @brief    (1) Increment current descriptor pointer to next receive descriptor :
 *               - (a) Return ownership of current descriptor back to DMA.
 *               - (b) Point to the next descriptor.
 *
 * @param    p_if    Pointer to interface requiring service.
 *******************************************************************************************************/
static void NetDev_RxDescPtrCurInc(NET_IF *p_if)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  DEV_DESC          *p_desc;
  NET_DEV           *p_dev;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               --------- OBTAIN REFERENCE TO DEVICE DATA ----------
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_desc = (DEV_DESC *)p_dev_data->RxBufDescPtrCur;             // Obtain ptr to next ready descriptor.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  p_desc = p_dev_data->RxBufDescPtrCur;                         // Obtain pointer to current Rx descriptor.
  p_desc->Addr &= ~GEM_RXBUF_ADDR_OWN;

  if (p_dev_data->RxBufDescPtrCur != p_dev_data->RxBufDescPtrEnd) {
    p_dev_data->RxBufDescPtrCur++;                              // Point to next Buffer Descriptor.
  } else {                                                      // Wrap around end of descriptor list if necessary.
    p_dev_data->RxBufDescPtrCur = p_dev_data->RxBufDescPtrStart;
  }

  p_desc = p_dev_data->RxBufDescPtrCur;
  if (p_desc->Addr & GEM_RXBUF_ADDR_OWN) {
    NetIF_RxQPost(p_if->Nbr, &local_err);
  } else {
    CPU_MB();
    p_dev->INTR_EN = GEM_BIT_INT_RX_COMPLETE;
  }
}

/****************************************************************************************************//**
 *                                           NetDev_TxDescInit()
 *
 * @brief    (1) This function initializes the Tx descriptor list for the specified interface :
 *               - (a) Obtain reference to the Rx descriptor(s) memory block
 *               - (b) Initialize Tx descriptor pointers
 *               - (c) Obtain Rx descriptor data areas
 *               - (d) Initialize hardware registers
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *
 * @note     (2) All functions that require device register access MUST obtain reference
 *               to the device hardware register space PRIOR to attempting to access
 *               any registers.  Register definitions SHOULD NOT be absolute and SHOULD
 *               use the provided base address within the device configuration structure,
 *               as well as the device register definition structure in order to properly
 *               resolve register addresses during run-time.
 *
 * @note     (3) All Tx descriptors are allocated as ONE memory block.  This removes the
 *               necessity to ensure that descriptor blocks are returned to the descriptor
 *               pool in the opposite order in which they were allocated; doing so would
 *               ensure that each memory block address was contiguous to the one before
 *               and after it.  If the descriptors are NOT contiguous, then software
 *               MUST NOT assign a pointer to the pool start address and use pointer
 *               arithmetic to navigate the descriptor list.  Since pointer arithmetic
 *               is a convenient way to navigate the descriptor list, ONE block is allocated
 *               and the driver uses pointer arithmetic to slice the block into descriptor
 *               sized units.
 *******************************************************************************************************/
static void NetDev_TxDescInit(NET_IF   *p_if,
                              RTOS_ERR *p_err)
{
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_DATA      *p_dev_data;
  NET_DEV           *p_dev;
  DEV_DESC          *p_desc;
  CPU_INT16U        i;

  PP_UNUSED_PARAM(p_err);

  //                                                               -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS ---
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev_data = (NET_DEV_DATA *)p_if->Dev_Data;                  // Obtain ptr to dev data area.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  //                                                               -------------- ALLOCATE DESCRIPTORS  ---------------
  //                                                               See Note #3.
  p_desc = p_dev_data->TxDescMemBlck;
  //                                                               -------------- INIT DESCRIPTOR PTRS  ---------------
  p_dev_data->TxBufDescPtrStart = (DEV_DESC *)p_desc;
  p_dev_data->TxBufDescPtrCur = (DEV_DESC *)p_desc;
  p_dev_data->TxBufDescCompPtr = (DEV_DESC *)p_desc;
  p_dev_data->TxBufDescPtrEnd = (DEV_DESC *)p_desc + (p_dev_cfg->TxDescNbr - 1u);

  //                                                               --------------- INIT TX DESCRIPTORS ----------------
  for (i = 0; i < p_dev_cfg->TxDescNbr; i++) {                  // Initialize Tx descriptor ring
    p_desc->Status = GEM_TXBUF_USED;

    if (p_desc == (p_dev_data->TxBufDescPtrEnd)) {              // Set WRAP bit on last descriptor in list.
      p_desc->Status |= GEM_TXBUF_WRAP;
    }
    p_desc++;                                                   // Point to next descriptor in list.
  }

  //                                                               ------------- INIT HARDWARE REGISTERS --------------
  //                                                               Configure the DMA with the Tx desc start address.
  p_dev->TX_QBAR = (CPU_INT32U)p_dev_data->TxBufDescPtrStart;
}

/********************************************************************************************************
 *                                          NetDev_AssertLPI_TX()
 *
 * @brief    Assert the LPI bit in ETH NETWORKCTRL to request low power idle mode from the PHY.
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @note     The LPI bit is ORed into transmit pause, so no frames will be sent.
 *******************************************************************************************************/
static void NetDev_AssertLPI_TX(NET_IF *p_if)
{
  NET_DEV           *p_dev;
  NET_DEV_CFG_ETHER *p_dev_cfg;

  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  p_dev->NET_CTRL |= GEM_BIT_CTRL_TXLPIEN;
}

/********************************************************************************************************
 *                                          NetDev_DeAssertLPI_TX()
 *
 * @brief    De-Assert the LPI bit to exit low power idle.
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @note     LPI will no longer force transmit to pause.
 *******************************************************************************************************/
static void NetDev_DeAssertLPI_TX(NET_IF *p_if)
{
  NET_DEV           *p_dev;
  NET_DEV_CFG_ETHER *p_dev_cfg;

  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;               // Obtain ptr to the dev cfg struct.
  p_dev = (NET_DEV *)p_dev_cfg->BaseAddr;                       // Overlay dev reg struct on top of dev base addr.

  p_dev->NET_CTRL &= ~GEM_BIT_CTRL_TXLPIEN;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IF_ETHER_AVAIL
