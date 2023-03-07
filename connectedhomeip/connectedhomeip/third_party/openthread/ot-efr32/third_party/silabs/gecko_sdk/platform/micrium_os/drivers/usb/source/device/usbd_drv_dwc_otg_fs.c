/***************************************************************************//**
 * @file
 * @brief USB Device Driver - USB On-The-Go Full-Speed (Otg_Fs)
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

#if (defined(RTOS_MODULE_USB_DEV_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    USBD_DRV_MODULE

#include  "../../include/usbd_drv.h"

#include  <usb/include/device/usbd_core.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <em_device.h>
#include  <em_core.h>
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
#include  <em_cmu.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                      (USBD, DRV, DWC_OTG_FS)
#define  RTOS_MODULE_CUR                   RTOS_CFG_MODULE_USBD

#define  REG_VAL_TO                        0x1Fu
#define  REG_FMOD_TO                       0x7FFFFu

/********************************************************************************************************
 *                                   DWC OTG USB DEVICE CONSTRAINTS
 *
 * Note(s) : (1) The USB system features a dedicated data RAM for endpoints. The endpoints RAM size varies
 *               according to the SoC.
 *
 *                   (a) RX-FIFO: The USB device uses a single receive FIFO that receives the data directed
 *                                to all OUT enpoints. The size of the receive FIFO is configured with
 *                                the receive FIFO Size register (GRXFSIZ). The value written to GRXFSIZ is
 *                                32-bit words entries.
 *
 *                   (b) TX-FIFO: The core has a dedicated FIFO for each IN endpoint. These can be configured
 *                                by writing to DIEPTXF0 for IN endpoint0 and DIEPTXFx for IN endpoint x.
 *                                The Tx FIFO depth value is in terms of 32-bit words. The minimum RAM space
 *                                required for each IN endpoint Tx FIFO is 64bytes (Max packet size), which
 *                                is equal to 16 32-bit words entries.
 *
 *                   (c) EFM32GG11: 2 KB endpoint memory = 2048 bytes = 512 32-bit words distributed as
 *                                  follows
 *                                  128 entries for Rx/OUT endpoints
 *                                  64 entries for each Tx/IN endpoints (max of 6 Tx/IN endpoints)
 *
 *                   (d) EFR32FG25: 3 KB endpoint memory = 3072 bytes = 768 32-bit words distributed as
 *                                  follows
 *                                  128 entries for Rx/OUT endpoints
 *                                  64 entries for each Tx/IN endpoints (max of 10 Tx/IN endpoints)
 *******************************************************************************************************/

#define  NBR_EPS_IN_OR_OUT_MAX               16u                // Maximum number of endpoints of type IN or OUT
#define  DFIFO_SIZE                        1024u                // Number of entries
#define  MAX_PKT_SIZE                        64u

#define  RXFIFO_SIZE                        128u                // See note #1a.
#define  TXFIFO_EPx_SIZE                     64u                // See note #1b.
#define  TXFIFO_EP0_SIZE                     64u
#define  TXFIFO_EP1_SIZE                     64u
#define  TXFIFO_EP2_SIZE                     64u
#define  TXFIFO_EP3_SIZE                     64u
#define  TXFIFO_EP4_SIZE                     64u
#define  TXFIFO_EP5_SIZE                     64u

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
#define  TXFIFO_EP6_SIZE                     64u
#define  TXFIFO_EP7_SIZE                     64u
#define  TXFIFO_EP8_SIZE                     64u
#define  TXFIFO_EP9_SIZE                     64u

#define  NBR_EPS_IN          (USBAHB_EP_NUM + 1u)               // +1 for control EP IN.
#define  NBR_EPS_OUT         (USBAHB_EP_NUM + 1u)               // +1 for control EP OUT.
#define  NBR_EPS_PHY_MAX     (NBR_EPS_IN + NBR_EPS_OUT)
#else
#define  NBR_EPS_IN                           7u
#define  NBR_EPS_OUT                          7u
#define  NBR_EPS_PHY_MAX     (NBR_EPS_IN + NBR_EPS_OUT)
#endif

/********************************************************************************************************
 *                                           REGISTER BIT DEFINES
 *******************************************************************************************************/

#define  GOTGCTL_BIT_BVALOEN           DEF_BIT_06
#define  GOTGCTL_BIT_BVALOVAL          DEF_BIT_07

#define  GOTGINT_BIT_SEDET             DEF_BIT_02
#define  GOTGINT_BIT_SRSSCHG           DEF_BIT_08
#define  GOTGINT_BIT_HNSSCHG           DEF_BIT_09
#define  GOTGINT_BIT_HNGDET            DEF_BIT_17
#define  GOTGINT_BIT_ADTOCHG           DEF_BIT_18
#define  GOTGINT_BIT_DBCDNE            DEF_BIT_19

#define  GAHBCFG_BIT_TXFELVL           DEF_BIT_07
#define  GAHBCFG_BIT_GINTMSK           DEF_BIT_00

#define  GUSBCFG_BIT_FDMOD             DEF_BIT_30
#define  GUSBCFG_BIT_FHMOD             DEF_BIT_29
#define  GUSBCFG_BIT_HNPCAP            DEF_BIT_09
#define  GUSBCFG_BIT_SRPCAP            DEF_BIT_08
#define  GUSBCFG_BIT_PHYSEL            DEF_BIT_07
#define  GUSBCFG_TRDT_MASK             DEF_BIT_FIELD(15u, 10u)

#define  GRSTCTL_BIT_AHBIDL            DEF_BIT_31
#define  GRSTCTL_FLUSH_TXFIFO_00       DEF_BIT_NONE
#define  GRSTCTL_FLUSH_TXFIFO_01       DEF_BIT_MASK(1u, 6u)
#define  GRSTCTL_FLUSH_TXFIFO_02       DEF_BIT_MASK(2u, 6u)
#define  GRSTCTL_FLUSH_TXFIFO_03       DEF_BIT_MASK(3u, 6u)
#define  GRSTCTL_FLUSH_TXFIFO_ALL      DEF_BIT_MASK(16u, 6u)
#define  GRSTCTL_BIT_TXFFLSH           DEF_BIT_05
#define  GRSTCTL_BIT_RXFFLSH           DEF_BIT_04
#define  GRSTCTL_BIT_HSRST             DEF_BIT_01
#define  GRSTCTL_BIT_CSRST             DEF_BIT_00

#define  GINTSTS_BIT_WKUPINT           DEF_BIT_31
#define  GINTSTS_BIT_SRQINT            DEF_BIT_30
#define  GINTSTS_BIT_CIDSCHG           DEF_BIT_28
#define  GINTSTS_BIT_RESETDET          DEF_BIT_23
#define  GINTSTS_BIT_INCOMPISOOUT      DEF_BIT_21
#define  GINTSTS_BIT_IISOIXFR          DEF_BIT_20
#define  GINTSTS_BIT_OEPINT            DEF_BIT_19
#define  GINTSTS_BIT_IEPINT            DEF_BIT_18
#define  GINTSTS_BIT_EOPF              DEF_BIT_15
#define  GINTSTS_BIT_ISOODRP           DEF_BIT_14
#define  GINTSTS_BIT_ENUMDNE           DEF_BIT_13
#define  GINTSTS_BIT_USBRST            DEF_BIT_12
#define  GINTSTS_BIT_USBSUSP           DEF_BIT_11
#define  GINTSTS_BIT_ESUSP             DEF_BIT_10
#define  GINTSTS_BIT_GONAKEFF          DEF_BIT_07
#define  GINTSTS_BIT_GINAKEFF          DEF_BIT_06
#define  GINTSTS_BIT_RXFLVL            DEF_BIT_04
#define  GINTSTS_BIT_SOF               DEF_BIT_03
#define  GINTSTS_BIT_OTGINT            DEF_BIT_02
#define  GINTSTS_BIT_MMIS              DEF_BIT_01
#define  GINTSTS_BIT_CMOD              DEF_BIT_00
//                                                                 Interrupts are clear by writing 1 to its bit
#define  GINTSTS_INT_ALL              (GINTSTS_BIT_WKUPINT  | GINTSTS_BIT_SRQINT         \
                                       | GINTSTS_BIT_CIDSCHG  | GINTSTS_BIT_INCOMPISOOUT \
                                       | GINTSTS_BIT_IISOIXFR | GINTSTS_BIT_EOPF         \
                                       | GINTSTS_BIT_ISOODRP  | GINTSTS_BIT_ENUMDNE      \
                                       | GINTSTS_BIT_USBRST   | GINTSTS_BIT_USBSUSP      \
                                       | GINTSTS_BIT_ESUSP    | GINTSTS_BIT_SOF          \
                                       | GINTSTS_BIT_MMIS)

#define  GINTMSK_BIT_WUIM              DEF_BIT_31
#define  GINTMSK_BIT_SRQIM             DEF_BIT_30
#define  GINTMSK_BIT_CIDSCHGM          DEF_BIT_28
#define  GINTMSK_BIT_RESETDET          DEF_BIT_23
#define  GINTMSK_BIT_IISOOXFRM         DEF_BIT_21
#define  GINTMSK_BIT_IISOIXFRM         DEF_BIT_20
#define  GINTMSK_BIT_OEPINT            DEF_BIT_19
#define  GINTMSK_BIT_IEPINT            DEF_BIT_18
#define  GINTMSK_BIT_EPMISM            DEF_BIT_17
#define  GINTMSK_BIT_EOPFM             DEF_BIT_15
#define  GINTMSK_BIT_ISOODRPM          DEF_BIT_14
#define  GINTMSK_BIT_ENUMDNEM          DEF_BIT_13
#define  GINTMSK_BIT_USBRST            DEF_BIT_12
#define  GINTMSK_BIT_USBSUSPM          DEF_BIT_11
#define  GINTMSK_BIT_ESUSPM            DEF_BIT_10
#define  GINTMSK_BIT_GONAKEFFM         DEF_BIT_07
#define  GINTMSK_BIT_GINAKEFFM         DEF_BIT_06
#define  GINTMSK_BIT_RXFLVLM           DEF_BIT_04
#define  GINTMSK_BIT_SOFM              DEF_BIT_03
#define  GINTMSK_BIT_OTGINT            DEF_BIT_02
#define  GINTMSK_BIT_MMISM             DEF_BIT_01

#define  GRXSTSx_PKTSTS_OUT_NAK        1u
#define  GRXSTSx_PKTSTS_OUT_RX         2u
#define  GRXSTSx_PKTSTS_OUT_COMPL      3u
#define  GRXSTSx_PKTSTS_SETUP_COMPL    4u
#define  GRXSTSx_PKTSTS_SETUP_RX       6u
#define  GRXSTSx_PKTSTS_MASK           DEF_BIT_FIELD(4u, 17u)
#define  GRXSTSx_EPNUM_MASK            DEF_BIT_FIELD(2u, 0u)
#define  GRXSTSx_BCNT_MASK             DEF_BIT_FIELD(11u, 4u)

#define  DCFG_PFIVL_80                 DEF_BIT_MASK(0u, 11u)
#define  DCFG_PFIVL_85                 DEF_BIT_MASK(1u, 11u)
#define  DCFG_PFIVL_90                 DEF_BIT_MASK(2u, 11u)
#define  DCFG_PFIVL_95                 DEF_BIT_MASK(3u, 11u)
#define  DCFG_BIT_NZLSOHSK             DEF_BIT_02
#define  DCFG_DSPD_FULLSPEED           DEF_BIT_MASK(3u, 0u)
#define  DCFG_DAD_MASK                 DEF_BIT_FIELD(7u, 4u)

#define  DCTL_BIT_POPRGDNE             DEF_BIT_11
#define  DCTL_BIT_CGONAK               DEF_BIT_10
#define  DCTL_BIT_SGONAK               DEF_BIT_09
#define  DCTL_BIT_CGINAK               DEF_BIT_08
#define  DCTL_BIT_SGINAK               DEF_BIT_07
#define  DCTL_BIT_GONSTS               DEF_BIT_03
#define  DCTL_BIT_GINSTS               DEF_BIT_02
#define  DCTL_BIT_SDIS                 DEF_BIT_01
#define  DCTL_BIT_RWUSIG               DEF_BIT_00

#define  DSTS_BIT_EERR                 DEF_BIT_03
#define  DSTS_ENUMSPD_FS_PHY_48MHZ     DEF_BIT_MASK(3u, 1u)
#define  DSTS_BIT_SUSPSTS              DEF_BIT_00
#define  DSTS_FNSOF_MASK               DEF_BIT_FIELD(14u, 8u)

#define  DIEPMSK_BIT_INEPNEM           DEF_BIT_06
#define  DIEPMSK_BIT_INEPNMM           DEF_BIT_05
#define  DIEPMSK_BIT_ITTXFEMSK         DEF_BIT_04
#define  DIEPMSK_BIT_TOM               DEF_BIT_03
#define  DIEPMSK_BIT_EPDM              DEF_BIT_01
#define  DIEPMSK_BIT_XFRCM             DEF_BIT_00

#define  DOEPMSK_BIT_OTEPDM            DEF_BIT_04
#define  DOEPMSK_BIT_STUPM             DEF_BIT_03
#define  DOEPMSK_BIT_EPDM              DEF_BIT_01
#define  DOEPMSK_BIT_XFRCM             DEF_BIT_00

#define  DAINT_BIT_OEPINT_EP0          DEF_BIT_16
#define  DAINT_BIT_OEPINT_EP1          DEF_BIT_17
#define  DAINT_BIT_OEPINT_EP2          DEF_BIT_18
#define  DAINT_BIT_OEPINT_EP3          DEF_BIT_19
#define  DAINT_BIT_IEPINT_EP0          DEF_BIT_00
#define  DAINT_BIT_IEPINT_EP1          DEF_BIT_01
#define  DAINT_BIT_IEPINT_EP2          DEF_BIT_02
#define  DAINT_BIT_IEPINT_EP3          DEF_BIT_03

#define  DAINTMSK_BIT_OEPINT_EP0       DEF_BIT_16
#define  DAINTMSK_BIT_OEPINT_EP1       DEF_BIT_17
#define  DAINMSKT_BIT_OEPINT_EP2       DEF_BIT_18
#define  DAINTMSK_BIT_OEPINT_EP3       DEF_BIT_19
#define  DAINTMSK_OEPINT_ALL          (DAINTMSK_BIT_OEPINT_EP0 | DAINTMSK_BIT_OEPINT_EP1 \
                                       | DAINTMSK_BIT_OEPINT_EP2 | DAINTMSK_BIT_OEPINT_EP3)
#define  DAINTMSK_BIT_IEPINT_EP0       DEF_BIT_00
#define  DAINTMSK_BIT_IEPINT_EP1       DEF_BIT_01
#define  DAINTMSK_BIT_IEPINT_EP2       DEF_BIT_02
#define  DAINTMSK_BIT_IEPINT_EP3       DEF_BIT_03
#define  DAINTMSK_IEPINT_ALL          (DAINTMSK_BIT_IEPINT_EP0 | DAINTMSK_BIT_IEPINT_EP1 \
                                       | DAINTMSK_BIT_IEPINT_EP2 | DAINTMSK_BIT_IEPINT_EP3)

#define  DIEPEMPMSK_BIT_INEPTXFEM_EP0  DEF_BIT_00
#define  DIEPEMPMSK_BIT_INEPTXFEM_EP1  DEF_BIT_01
#define  DIEPEMPMSK_BIT_INEPTXFEM_EP2  DEF_BIT_02
#define  DIEPEMPMSK_BIT_INEPTXFEM_EP3  DEF_BIT_03

#define  DxEPCTLx_BIT_EPENA            DEF_BIT_31
#define  DxEPCTLx_BIT_EPDIS            DEF_BIT_30
#define  DxEPCTLx_BIT_SODDFRM          DEF_BIT_29
#define  DxEPCTLx_BIT_SD0PID           DEF_BIT_28
#define  DxEPCTLx_BIT_SEVNFRM          DEF_BIT_28
#define  DxEPCTLx_BIT_SNAK             DEF_BIT_27
#define  DxEPCTLx_BIT_CNAK             DEF_BIT_26
#define  DxEPCTLx_BIT_STALL            DEF_BIT_21
#define  DxEPCTLx_EPTYPE_CTRL          DEF_BIT_MASK(0u, 18u)
#define  DxEPCTLx_EPTYPE_ISO           DEF_BIT_MASK(1u, 18u)
#define  DxEPCTLx_EPTYPE_BULK          DEF_BIT_MASK(2u, 18u)
#define  DxEPCTLx_EPTYPE_INTR          DEF_BIT_MASK(3u, 18u)
#define  DxEPCTLx_BIT_NAKSTS           DEF_BIT_17
#define  DxEPCTLx_BIT_EONUM            DEF_BIT_16
#define  DxEPCTLx_BIT_DPID             DEF_BIT_16
#define  DxEPCTLx_BIT_USBAEP           DEF_BIT_15
#define  DxEPCTLx_MPSIZ_MASK           DEF_BIT_FIELD(11u, 0u)

#define  DxEPCTL0_MPSIZ_64             DEF_BIT_MASK(0u, 0u)
#define  DxEPCTL0_MPSIZ_64_MSK         DEF_BIT_FIELD(2u, 0u)

#define  DOEPCTLx_BIT_SD1PID           DEF_BIT_29
#define  DOEPCTLx_BIT_SNPM             DEF_BIT_20

#define  DIEPINTx_BIT_TXFE             DEF_BIT_07
#define  DIEPINTx_BIT_INEPNE           DEF_BIT_06
#define  DIEPINTx_BIT_ITTXFE           DEF_BIT_04
#define  DIEPINTx_BIT_TOC              DEF_BIT_03
#define  DIEPINTx_BIT_EPDISD           DEF_BIT_01
#define  DIEPINTx_BIT_XFRC             DEF_BIT_00

#define  DOEPINTx_BIT_B2BSTUP          DEF_BIT_06
#define  DOEPINTx_BIT_OTEPDIS          DEF_BIT_04
#define  DOEPINTx_BIT_STUP             DEF_BIT_03
#define  DOEPINTx_BIT_EPDISD           DEF_BIT_01
#define  DOEPINTx_BIT_XFRC             DEF_BIT_00

#define  DOEPTSIZx_STUPCNT_1_PKT       DEF_BIT_MASK(1u, 29u)
#define  DOEPTSIZx_STUPCNT_2_PKT       DEF_BIT_MASK(2u, 29u)
#define  DOEPTSIZx_STUPCNT_3_PKT       DEF_BIT_MASK(3u, 29u)
#define  DOEPTSIZx_XFRSIZ_MSK          DEF_BIT_FIELD(19u, 0u)
#define  DOEPTSIZx_PKTCNT_MSK          DEF_BIT_FIELD(10u, 19u)

#define  DOEPTSIZ0_BIT_PKTCNT          DEF_BIT_19
#define  DOEPTSIZ0_XFRSIZ_MAX_64       DEF_BIT_MASK(64u, 0u)

#define  DIEPTSIZx_MCNT_1_PKT          DEF_BIT_MASK(1u, 29u)
#define  DIEPTSIZx_MCNT_2_PKT          DEF_BIT_MASK(2u, 29u)
#define  DIEPTSIZx_MCNT_3_PKT          DEF_BIT_MASK(3u, 29u)
#define  DIEPTSIZx_XFRSIZ_MSK          DEF_BIT_FIELD(19u, 0u)
#define  DIEPTSIZx_PKTCNT_MSK          DEF_BIT_FIELD(10u, 19u)
#define  DIEPTSIZx_PKTCNT_01           DEF_BIT_MASK(1u, 19u)

#define  DTXFSTSx_EP_FIFO_FULL         DEF_BIT_NONE
#define  DTXFSTSx_EP_FIFO_WAVAIL_01    DEF_BIT_MASK(1u, 0u)
#define  DTXFSTSx_EP_FIFO_WAVAIL_02    DEF_BIT_MASK(2u, 0u)

#define  DOEPTSIZx_RXDPID_DATA0        DEF_BIT_MASK(0u, 29u)
#define  DOEPTSIZx_RXDPID_DATA1        DEF_BIT_MASK(2u, 29u)
#define  DOEPTSIZx_RXDPID_DATA2        DEF_BIT_MASK(1u, 29u)
#define  DOEPTSIZx_RXDPID_MDATA        DEF_BIT_MASK(3u, 29u)

#define  PCGCCTL_BIT_RSTPDWNMODULE     DEF_BIT_03
#define  PCGCCTL_BIT_PWRCLMP           DEF_BIT_02
#define  PCGCCTL_BIT_GATEHCLK          DEF_BIT_01
#define  PCGCCTL_BIT_STPPCLK           DEF_BIT_00

/********************************************************************************************************
 *                           BIT/REG DEFINES FOR GIANT GECKO SERIES 1 ERRATA
 *******************************************************************************************************/

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
#define  USB_REG_DATTRIM1             *(CPU_REG32 *)(USB_BASE + 0x34u)
#define  DATTRIM1_BIT_ENDLYPULLUP      DEF_BIT_07
#endif

/********************************************************************************************************
 *                                         PHY SUSPEND MACROS
 *******************************************************************************************************/

#define  USBD_DRV_PHY_PCGCCTL_RESUME()               \
  DEF_BIT_CLR(p_reg->PCGCCTL, PCGCCTL_BIT_GATEHCLK); \
  DEF_BIT_CLR(p_reg->PCGCCTL, PCGCCTL_BIT_STPPCLK)

#define  USBD_DRV_PHY_PCGCCTL_SUSPEND()              \
  DEF_BIT_SET(p_reg->PCGCCTL, PCGCCTL_BIT_GATEHCLK); \
  DEF_BIT_SET(p_reg->PCGCCTL, PCGCCTL_BIT_STPPCLK)

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
#define  USBD_DRV_PHY_RESUME()                               \
  do {                                                       \
    USBD_DRV_PHY_PCGCCTL_RESUME();                           \
    DEF_BIT_SET(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP); \
  } while (0)

#define  USBD_DRV_PHY_SUSPEND()                              \
  do {                                                       \
    DEF_BIT_CLR(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP); \
    USBD_DRV_PHY_PCGCCTL_SUSPEND();                          \
  } while (0)

#else
#define  USBD_DRV_PHY_RESUME()     \
  do {                             \
    USBD_DRV_PHY_PCGCCTL_RESUME(); \
  } while (0)

#define  USBD_DRV_PHY_SUSPEND()     \
  do {                              \
    USBD_DRV_PHY_PCGCCTL_SUSPEND(); \
  } while (0)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct usbd_dwc_otg_fs_in_ep_reg {                      // -------------- IN-ENDPOINT REGISTERS ---------------
  CPU_REG32 CTLx;                                               // Device control IN endpoint-x
  CPU_REG32 RSVD0;
  CPU_REG32 INTx;                                               // Device IN endpoint-x interrupt
  CPU_REG32 RSVD1;
  CPU_REG32 TSIZx;                                              // Device IN endpoint-x transfer size
  CPU_REG32 DMAx;                                               // OTG_HS device IN endpoint-x DMA address register
  CPU_REG32 DTXFSTSx;                                           // Device IN endpoint-x transmit FIFO status
  CPU_REG32 RSVD2;
} USBD_DWC_OTG_FS_IN_EP_REG;

typedef struct usbd_dwc_otg_fs_out_ep_reg {                     // ------------- OUT ENDPOINT REGISTERS ---------------
  CPU_REG32 CTLx;                                               // Device control OUT endpoint-x
  CPU_REG32 RSVD0;
  CPU_REG32 INTx;                                               // Device OUT endpoint-x interrupt
  CPU_REG32 RSVD1;
  CPU_REG32 TSIZx;                                              // Device OUT endpoint-x transfer size
  CPU_REG32 DMAx;                                               // OTG_HS device OUT endpoint-x DMA address register
  CPU_REG32 RSVD2[2u];
} USBD_DWC_OTG_FS_OUT_EP_REG;

typedef struct usbd_dwc_otg_fs_dfifo_reg {                      // ---------- DATA FIFO ACCESS REGISTERS --------------
  CPU_REG32 DATA[DFIFO_SIZE];                                   // 4K bytes per endpoint
} USBD_DWC_OTG_FS_DFIFO_REG;

typedef struct usbd_dwc_otg_fs_reg {
  //                                                               ----- CORE GLOBAL CONTROL AND STATUS REGISTERS -----
  CPU_REG32                  GOTGCTL;                           // Core control and status
  CPU_REG32                  GOTGINT;                           // Core interrupt
  CPU_REG32                  GAHBCFG;                           // Core AHB configuration
  CPU_REG32                  GUSBCFG;                           // Core USB configuration
  CPU_REG32                  GRSTCTL;                           // Core reset
  CPU_REG32                  GINTSTS;                           // Core interrupt
  CPU_REG32                  GINTMSK;                           // Core interrupt mask
  CPU_REG32                  GRXSTSR;                           // Core receive status debug read
  CPU_REG32                  GRXSTSP;                           // Core status read and pop
  CPU_REG32                  GRXFSIZ;                           // Core receive FIFO size
  CPU_REG32                  DIEPTXF0;                          // Endpoint 0 transmit FIFO size
  CPU_REG32                  RSVD0[54u];
  //                                                               Device IN endpoint transmit FIFO size
  CPU_REG32                  DIEPTXFx[NBR_EPS_IN_OR_OUT_MAX - 1];
  CPU_REG32                  RSVD1[432u];
  //                                                               ----- DEVICE MODE CONTROL AND STATUS REGISTERS -----
  CPU_REG32                  DCFG;                              // Device configuration
  CPU_REG32                  DCTL;                              // Device control
  CPU_REG32                  DSTS;                              // Device status
  CPU_REG32                  RSVD2;
  CPU_REG32                  DIEPMSK;                           // Device IN endpoint common interrupt mask
  CPU_REG32                  DOEPMSK;                           // Device OUT endpoint common interrupt mask
  CPU_REG32                  DAINT;                             // Device All endpoints interrupt
  CPU_REG32                  DAINTMSK;                          // Device All endpoints interrupt mask
  CPU_REG32                  RSVD3[2u];
  CPU_REG32                  DVBUSDIS;                          // Device VBUS discharge time
  CPU_REG32                  DVBUSPULSE;                        // Device VBUS pulsing time
  CPU_REG32                  RSVD4;
  CPU_REG32                  DIEPEMPMSK;                        // Device IN ep FIFO empty interrupt mask
  CPU_REG32                  RSVD5[50u];
  USBD_DWC_OTG_FS_IN_EP_REG  DIEP[NBR_EPS_IN_OR_OUT_MAX];       // Device IN EP registers
  USBD_DWC_OTG_FS_OUT_EP_REG DOEP[NBR_EPS_IN_OR_OUT_MAX];       // Device OUT EP registers
  CPU_REG32                  RSVD6[64u];
  CPU_REG32                  PCGCCTL;                           // Power anc clock gating control
  CPU_REG32                  RSVD7[127u];
  USBD_DWC_OTG_FS_DFIFO_REG  DFIFO[NBR_EPS_IN_OR_OUT_MAX];      // Data FIFO access registers
} USBD_DWC_OTG_FS_REG;

/********************************************************************************************************
 *                                           DRIVER DATA TYPE
 *******************************************************************************************************/
typedef struct usbd_drv_data_ep {                               // ---------- DEVICE ENDPOINT DATA STRUCTURE ----------
  CPU_INT32U  DataBuf[MAX_PKT_SIZE / 4u];                       // Drv internal aligned buffer.
  CPU_INT16U  EP_MaxPktSize[NBR_EPS_PHY_MAX];                   // Max pkt size of opened EPs.
  CPU_INT16U  EP_PktXferLen[NBR_EPS_PHY_MAX];                   // EPs current xfer len.
  CPU_INT08U  *EP_AppBufPtr[NBR_EPS_PHY_MAX];                   // Ptr to endpoint app buffer.
  CPU_INT16U  EP_AppBufLen[NBR_EPS_PHY_MAX];                    // Lenght of endpoint app buffer.
  CPU_INT32U  EP_SetupBuf[2u];                                  // Buffer that contains setup pkt.
  CPU_BOOLEAN EnumDone;                                         // Indicates if EnumDone ISR occurred.
} USBD_DRV_DATA_EP;

/********************************************************************************************************
 ********************************************************************************************************
 *                               USB DEVICE CONTROLLER DRIVER API PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ FULL-SPEED MODE DRIVER API ------------
static void USBD_DrvInit(USBD_DRV *p_drv,
                         RTOS_ERR *p_err);

static void USBD_DrvStart(USBD_DRV *p_drv,
                          RTOS_ERR *p_err);

static void USBD_DrvStop(USBD_DRV *p_drv);

static CPU_BOOLEAN USBD_DrvAddrSet(USBD_DRV   *p_drv,
                                   CPU_INT08U dev_addr);

static CPU_INT16U USBD_DrvFrameNbrGet(USBD_DRV *p_drv);

static void USBD_DrvEP_Open(USBD_DRV   *p_drv,
                            CPU_INT08U ep_addr,
                            CPU_INT08U ep_type,
                            CPU_INT16U max_pkt_size,
                            CPU_INT08U transaction_frame,
                            RTOS_ERR   *p_err);

static void USBD_DrvEP_Close(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr);

static CPU_INT32U USBD_DrvEP_RxStart(USBD_DRV   *p_drv,
                                     CPU_INT08U ep_addr,
                                     CPU_INT08U *p_buf,
                                     CPU_INT32U buf_len,
                                     RTOS_ERR   *p_err);

static CPU_INT32U USBD_DrvEP_Rx(USBD_DRV   *p_drv,
                                CPU_INT08U ep_addr,
                                CPU_INT08U *p_buf,
                                CPU_INT32U buf_len,
                                RTOS_ERR   *p_err);

static void USBD_DrvEP_RxZLP(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr,
                             RTOS_ERR   *p_err);

static CPU_INT32U USBD_DrvEP_Tx(USBD_DRV   *p_drv,
                                CPU_INT08U ep_addr,
                                CPU_INT08U *p_buf,
                                CPU_INT32U buf_len,
                                RTOS_ERR   *p_err);

static void USBD_DrvEP_TxStart(USBD_DRV   *p_drv,
                               CPU_INT08U ep_addr,
                               CPU_INT08U *p_buf,
                               CPU_INT32U buf_len,
                               RTOS_ERR   *p_err);

static void USBD_DrvEP_TxZLP(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr,
                             RTOS_ERR   *p_err);

static CPU_BOOLEAN USBD_DrvEP_Abort(USBD_DRV   *p_drv,
                                    CPU_INT08U ep_addr);

static CPU_BOOLEAN USBD_DrvEP_Stall(USBD_DRV    *p_drv,
                                    CPU_INT08U  ep_addr,
                                    CPU_BOOLEAN state);

static void USBD_DrvISR_Handler(USBD_DRV *p_drv);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void DWC_RxFIFO_Rd(USBD_DRV *p_drv);

static void DWC_TxFIFO_Wr(USBD_DRV   *p_drv,
                          CPU_INT08U ep_log_nbr,
                          CPU_INT08U *p_buf,
                          CPU_INT16U ep_pkt_len);

static void DWC_EP_OutProcess(USBD_DRV *p_drv);

static void DWC_EP_InProcess(USBD_DRV *p_drv);

/********************************************************************************************************
 ********************************************************************************************************
 *                                   USB DEVICE CONTROLLER DRIVER API
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- EFM32_OTG_FS DRIVER API -------------
USBD_DRV_API USBD_DrvAPI_EFx32_OTG_FS = { USBD_DrvInit,
                                          USBD_DrvStart,
                                          USBD_DrvStop,
                                          USBD_DrvAddrSet,
                                          DEF_NULL,
                                          DEF_NULL,
                                          DEF_NULL,
                                          USBD_DrvFrameNbrGet,
                                          USBD_DrvEP_Open,
                                          USBD_DrvEP_Close,
                                          USBD_DrvEP_RxStart,
                                          USBD_DrvEP_Rx,
                                          USBD_DrvEP_RxZLP,
                                          USBD_DrvEP_Tx,
                                          USBD_DrvEP_TxStart,
                                          USBD_DrvEP_TxZLP,
                                          USBD_DrvEP_Abort,
                                          USBD_DrvEP_Stall,
                                          USBD_DrvISR_Handler, };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_DrvInit()
 *
 * @brief    Initialize the Full-speed device.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *
 * @note     (1) Since the CPU frequency could be higher than OTG module clock, a timeout is needed
 *               to reset the OTG controller successfully.
 *
 *           (2) If a debugger is used to step into the driver, locking the OTG FS registers may make the
 *               debugger crash if the USB OTG FS register view is opened. Indeed, the various registers
 *               reads done by the debugger to refresh the registers content may not be properly handled
 *               while they are locked.
 *******************************************************************************************************/
static void USBD_DrvInit(USBD_DRV *p_drv,
                         RTOS_ERR *p_err)
{
  CPU_INT08U          ep_nbr;
  CPU_INT32U          reg_to;
  CPU_REG32           ctrl_reg;
  USBD_DWC_OTG_FS_REG *p_reg;
  USBD_DRV_BSP_API    *p_bsp_api;
  USBD_DRV_DATA_EP    *p_drv_data;

  //                                                               Alloc drv internal data.
  p_drv_data = Mem_SegAlloc("USB driver internal data",
                            DEF_NULL,
                            sizeof(USBD_DRV_DATA_EP),
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_Clr(p_drv_data, sizeof(USBD_DRV_DATA_EP));

  p_drv->DataPtr = p_drv_data;                                  // Store drv internal data ptr.

  p_bsp_api = p_drv->BSP_API_Ptr;                               // Get driver BSP API reference.
  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;       // Get USB ctrl reg ref.

  if (p_bsp_api->Init != DEF_NULL) {
    p_bsp_api->Init(p_drv);                                     // Call board/chip specific device controller ...
                                                                // ... initialization function.
  }

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  USB_APBS->EN_SET = USB_EN_EN;                                 // Enable the USB IP prior to setting other registers.
  CMU_WaitUSBPLLLock();                                         // Ensure PLL0 is stable.
#endif

  //                                                               Disable the global interrupt
  DEF_BIT_CLR(p_reg->GAHBCFG, GAHBCFG_BIT_GINTMSK);

  //                                                               -------------------- CORE RESET --------------------
  reg_to = REG_VAL_TO;                                          // Check AHB master state machine is in IDLE condition
  while ((DEF_BIT_IS_CLR(p_reg->GRSTCTL, GRSTCTL_BIT_AHBIDL))
         && (reg_to > 0u)) {
    reg_to--;
  }

  DEF_BIT_SET(p_reg->GRSTCTL, GRSTCTL_BIT_CSRST);               // Resets the HCLK and PCLK domains

  reg_to = REG_VAL_TO;                                          // Check all necessary logic is reset in the core
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_CSRST))
         && (reg_to > 0u)) {
    reg_to--;
  }

#if !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  p_reg->GUSBCFG |= GUSBCFG_BIT_HNPCAP                          // Enable Host Negotiation Protocol capabilities.
                    | GUSBCFG_BIT_SRPCAP;                       // Enable Sessions Request Protocol capabilities.

  DEF_BIT_SET(p_reg->GUSBCFG, GUSBCFG_BIT_FDMOD);               // Force the core to device mode
  reg_to = REG_FMOD_TO;                                         // Wait at least 25ms before the change takes effect
  while (reg_to > 0u) {
    reg_to--;
  }
#endif

  //                                                               ------------------- DEVICE INIT --------------------
  p_reg->PCGCCTL = DEF_BIT_NONE;                                // Reset the PHY clock
  p_reg->GOTGCTL = DEF_BIT_NONE;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  DEF_BIT_SET(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

  p_reg->DCFG = DCFG_PFIVL_80                                   // 80% of the frame interval
                | DCFG_DSPD_FULLSPEED                           // Default to full-speed device
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
                | USBAHB_DCFG_ENA32KHZSUSP                      // Enable PHY clock switched from 48 MHz to 32 KHz during suspend
                | (10 << _USBAHB_DCFG_RESVALID_SHIFT);          // Set resume period to detect a valid resume event TODO CM see if 10 must be really set or just reset value of 2 is enough
#endif
  ;

  p_reg->GRSTCTL = GRSTCTL_BIT_TXFFLSH                          // Flush all transmit FIFOs
                   | GRSTCTL_FLUSH_TXFIFO_ALL;

  reg_to = REG_VAL_TO;                                          // Wait for the flush completion
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_TXFFLSH))
         && (reg_to > 0u)) {
    reg_to--;
  }

  //                                                               Flush the receive FIFO
  DEF_BIT_SET(p_reg->GRSTCTL, GRSTCTL_BIT_RXFFLSH);

  reg_to = REG_VAL_TO;                                          // Wait for the flush completion
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_RXFFLSH))
         && (reg_to > 0u)) {
    reg_to--;
  }
  //                                                               Clear all pending Device interrupts
  p_reg->DIEPMSK = DEF_BIT_NONE;                                // Dis. interrupts for the Device IN Endpoints
  p_reg->DOEPMSK = DEF_BIT_NONE;                                // Dis. interrupts for the Device OUT Endpoints
  p_reg->DAINTMSK = DEF_BIT_NONE;                               // Dis. interrupts foo all Device Endpoints

  for (ep_nbr = 0u; ep_nbr < NBR_EPS_IN_OR_OUT_MAX; ep_nbr++) {
    //                                                             ----------------- IN ENDPOINT RESET ----------------
    ctrl_reg = p_reg->DIEP[ep_nbr].CTLx;
    if (DEF_BIT_IS_SET(ctrl_reg, DxEPCTLx_BIT_EPENA)) {
      ctrl_reg = DEF_BIT_NONE;
      ctrl_reg = DxEPCTLx_BIT_EPDIS
                 | DxEPCTLx_BIT_SNAK;
    } else {
      ctrl_reg = DEF_BIT_NONE;
    }

    p_reg->DIEP[ep_nbr].CTLx = ctrl_reg;
    p_reg->DIEP[ep_nbr].TSIZx = DEF_BIT_NONE;
    p_reg->DIEP[ep_nbr].INTx = 0x000000FFu;                     // Clear any pending Interrupt

    //                                                             ---------------- OUT ENDPOINT RESET ----------------
    ctrl_reg = p_reg->DOEP[ep_nbr].CTLx;
    if (DEF_BIT_IS_SET(ctrl_reg, DxEPCTLx_BIT_EPENA)) {
      ctrl_reg = DEF_BIT_NONE;
      ctrl_reg = DxEPCTLx_BIT_EPDIS
                 | DxEPCTLx_BIT_SNAK;
    } else {
      ctrl_reg = DEF_BIT_NONE;
    }

    p_reg->DOEP[ep_nbr].CTLx = ctrl_reg;
    p_reg->DOEP[ep_nbr].TSIZx = DEF_BIT_NONE;
    p_reg->DOEP[ep_nbr].INTx = 0x000000FFu;                     // Clear any pending Interrupt
  }

  p_reg->GINTMSK = DEF_BIT_NONE;                                // Disable all interrupts
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  USB_APBS->IEN_CLR = USB_IEN_DWCOTG | USB_IEN_VBUS;
#endif

  DEF_BIT_CLR(p_reg->DCFG, DCFG_DAD_MASK);                      // Set Device Address to zero

  p_reg->PCGCCTL = PCGCCTL_BIT_PWRCLMP
                   | PCGCCTL_BIT_RSTPDWNMODULE;                 // Lock access to registers (see Note #2)
}

/****************************************************************************************************//**
 *                                           USBD_DrvStart()
 *
 * @brief    Start device operation with VBUS detection disable.
 *
 * @param    p_drv   Pointer to device driver structure.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBD_DrvStart(USBD_DRV *p_drv,
                          RTOS_ERR *p_err)
{
  USBD_DRV_BSP_API    *p_bsp_api;
  USBD_DWC_OTG_FS_REG *p_reg;
  USBD_DRV_DATA_EP    *p_drv_data;

  p_bsp_api = p_drv->BSP_API_Ptr;                               // Get driver BSP API reference.
  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;       // Get USB ctrl reg ref.
  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;

  p_drv_data->EnumDone = DEF_NO;

  DEF_BIT_CLR(p_reg->PCGCCTL, PCGCCTL_BIT_PWRCLMP);
  DEF_BIT_CLR(p_reg->PCGCCTL, PCGCCTL_BIT_RSTPDWNMODULE);       // Unlock registers access to write.
  USBD_DRV_PHY_RESUME();

  p_reg->GINTSTS = 0xFFFFFFFFu;                                 // Clear any pending interrupt
                                                                // Enable interrupts
  p_reg->GINTMSK = GINTMSK_BIT_USBSUSPM
                   | GINTMSK_BIT_USBRST
                   | GINTMSK_BIT_ENUMDNEM
                   | GINTMSK_BIT_WUIM
                   | GINTMSK_BIT_SRQIM
#if !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
                   | GINTMSK_BIT_OTGINT
#endif
                   | GINTMSK_BIT_RESETDET;

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  USB_APBS->CTRL_SET = USB_CTRL_VBSSNSHEN;                    // Enable VBUS sense high to detect connection.
  USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSLEN;                    // Ensure VBUS sense low is disabled.
  USB_APBS->IEN_SET = USB_IEN_DWCOTG                          // Enable main USB IP interrupt.
                      | USB_IEN_VBUS;                         // Enable VBUS sense interrupt.
#endif

  //                                                               Enable Global Interrupt
  DEF_BIT_SET(p_reg->GAHBCFG, GAHBCFG_BIT_GINTMSK);

  if (p_bsp_api->Conn != DEF_NULL) {
    p_bsp_api->Conn();                                          // Call board/chip specific connect function.
  }

  USBD_DRV_PHY_SUSPEND();

  DEF_BIT_CLR(p_reg->DCTL, DCTL_BIT_SDIS);                      // Generate Device connect event to the USB host

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBD_DrvStop()
 *
 * @brief    Stop Full-speed device operation.
 *
 * @param    p_drv   Pointer to device driver structure.
 *
 * @note     (1) Clear and disable USB interrupts.
 *
 * @note     (2) Disconnect from the USB host (e.g, reset the pull-down on the D- pin).
 *******************************************************************************************************/
static void USBD_DrvStop(USBD_DRV *p_drv)
{
  USBD_DRV_BSP_API    *p_bsp_api;
  USBD_DWC_OTG_FS_REG *p_reg;
  CPU_INT16U          timeout;

  p_bsp_api = p_drv->BSP_API_Ptr;                               // Get driver BSP API reference.
  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;       // Get USB ctrl reg ref.

  USBD_DRV_PHY_RESUME();

  if (p_bsp_api->Disconn != DEF_NULL) {
    p_bsp_api->Disconn();
  }

  p_reg->GINTMSK = DEF_BIT_NONE;                                // Disable all interrupts.
  p_reg->GINTSTS = 0xFFFFFFFF;                                  // Clear any pending interrupt.
                                                                // Disable the global interrupt.
  DEF_BIT_CLR(p_reg->GAHBCFG, GAHBCFG_BIT_GINTMSK);

  DEF_BIT_SET(p_reg->DCTL, DCTL_BIT_SDIS);                      // Generate Device Disconnect event to the USB host.
  DEF_BIT_SET(p_reg->GRSTCTL, GRSTCTL_BIT_CSRST);               // Reset the USB core.

  KAL_Dly(1u);                                                  // Disconnect signal for at least 2 us.

  timeout = DEF_INT_16U_MAX_VAL;
  while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_CSRST))
         && (timeout > 0u)) {
    timeout--;
  }

  USBD_DRV_PHY_SUSPEND();
  DEF_BIT_SET(p_reg->PCGCCTL, PCGCCTL_BIT_PWRCLMP);
  DEF_BIT_SET(p_reg->PCGCCTL, PCGCCTL_BIT_RSTPDWNMODULE);
}

/****************************************************************************************************//**
 *                                               USBD_DrvAddrSet()
 *
 * @brief    Assign an address to device.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    dev_addr    Device address assigned by the host.
 *
 * @return   DEF_OK,   if NO error(s).
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) For device controllers that have hardware assistance to enable the device address after
 *               the status stage has completed, the assignment of the device address can also be
 *               combined with enabling the device address mode.
 *
 * @note     (2) For device controllers that change the device address immediately, without waiting the
 *               status phase to complete, see USBD_DrvAddrEn().
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_DrvAddrSet(USBD_DRV   *p_drv,
                                   CPU_INT08U dev_addr)
{
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);

  DEF_BIT_CLR(p_reg->DCFG, DCFG_DAD_MASK);
  p_reg->DCFG |= (dev_addr << 4u);                              // Set Device Address

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBD_DrvFrameNbrGet()
 *
 * @brief    Retrieve current frame number.
 *
 * @param    p_drv   Pointer to device driver structure.
 *
 * @return   Frame number.
 *******************************************************************************************************/
static CPU_INT16U USBD_DrvFrameNbrGet(USBD_DRV *p_drv)
{
  CPU_INT16U          frame_nbr;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);
  frame_nbr = (p_reg->DSTS & DSTS_FNSOF_MASK) >> 8u;

  return (frame_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_DrvEP_Open()
 *
 * @brief    Open and configure a device endpoint, given its characteristics (e.g., endpoint type,
 *           endpoint address, maximum packet size, etc).
 *
 * @param    p_drv               Pointer to device driver structure.
 *
 * @param    ep_addr             Endpoint address.
 *
 * @param    ep_type             Endpoint type :
 *                                   - USBD_EP_TYPE_CTRL,
 *                                   - USBD_EP_TYPE_ISOC,
 *                                   - USBD_EP_TYPE_BULK,
 *                                   - USBD_EP_TYPE_INTR.
 *
 * @param    max_pkt_size        Maximum packet size.
 *
 * @param    transaction_frame   Endpoint transactions per frame.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) Typically, the endpoint open function performs the following operations:
 *               - (a) Validate endpoint address, type and maximum packet size.
 *               - (b) Configure endpoint information in the device controller. This may include not
 *                     only assigning the type and maximum packet size, but also making certain that
 *                     the endpoint is successfully configured (or  realized  or  mapped ). For some
 *                     device controllers, this may not be necessary.
 *
 * @note     (2) If the endpoint address is valid, then the endpoint open function should validate
 *               the attributes allowed by the hardware endpoint :
 *               - (a) The maximum packet size 'max_pkt_size' should be validated to match hardware
 *                     capabilities.
 *******************************************************************************************************/
static void USBD_DrvEP_Open(USBD_DRV   *p_drv,
                            CPU_INT08U ep_addr,
                            CPU_INT08U ep_type,
                            CPU_INT16U max_pkt_size,
                            CPU_INT08U transaction_frame,
                            RTOS_ERR   *p_err)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT08U          ep_phy_nbr;
  CPU_INT32U          reg_val;
  USBD_DRV_DATA_EP    *p_drv_data;
  USBD_DWC_OTG_FS_REG *p_reg;

  (void)&transaction_frame;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);
  p_drv_data = (USBD_DRV_DATA_EP *)(p_drv->DataPtr);
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  reg_val = 0u;

  LOG_VRB(("USBD Drv EP FIFO Open for endpoint addr: 0x", (X)ep_addr));

  switch (ep_type) {
    case USBD_EP_TYPE_CTRL:
      if (USBD_EP_IS_IN(ep_addr) == DEF_YES) {
        //                                                         En. Device EP0 IN interrupt
        DEF_BIT_SET(p_reg->DAINTMSK, DAINTMSK_BIT_IEPINT_EP0);
        //                                                         En. common IN EP Transfer complete interrupt
        DEF_BIT_SET(p_reg->DIEPMSK, DIEPMSK_BIT_XFRCM);
      } else {
        //                                                         En. EP0 OUT interrupt
        DEF_BIT_SET(p_reg->DAINTMSK, DAINTMSK_BIT_OEPINT_EP0);
        //                                                         En. common OUT EP Setup $ Xfer complete interrupt
        DEF_BIT_SET(p_reg->DOEPMSK, (DOEPMSK_BIT_STUPM | DOEPMSK_BIT_XFRCM));
        p_reg->DOEP[ep_log_nbr].TSIZx = (DOEPTSIZx_STUPCNT_3_PKT
                                         | DOEPTSIZ0_BIT_PKTCNT
                                         | DOEPTSIZ0_XFRSIZ_MAX_64);

        DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SNAK);
      }

      break;

    case USBD_EP_TYPE_INTR:
    case USBD_EP_TYPE_BULK:
      if (USBD_EP_IS_IN(ep_addr)) {
        reg_val = (max_pkt_size                                 // cfg EP max packet size
                   | (ep_type << 18u)                           // cfg EP type
                   | (ep_log_nbr << 22u)                        // Tx FIFO number
                   | DxEPCTLx_BIT_SD0PID                        // EP start data toggle
                   | DxEPCTLx_BIT_USBAEP);                      // USB active EP

        p_reg->DIEP[ep_log_nbr].CTLx = reg_val;
        //                                                         En. Device EPx IN Interrupt
        DEF_BIT_SET(p_reg->DAINTMSK, DEF_BIT(ep_log_nbr));
      } else {
        reg_val = (max_pkt_size                                 // cfg EP max packet size
                   | (ep_type << 18u)                           // cfg EP type
                   | DxEPCTLx_BIT_SD0PID                        // EP start data toggle
                   | DxEPCTLx_BIT_USBAEP);                      // USB active EP

        p_reg->DOEP[ep_log_nbr].CTLx = reg_val;

        reg_val = DEF_BIT(ep_log_nbr) << 16u;
        DEF_BIT_SET(p_reg->DAINTMSK, reg_val);                  // En. Device EPx OUT Interrupt
      }
      break;

    case USBD_EP_TYPE_ISOC:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
  }

  p_drv_data->EP_MaxPktSize[ep_phy_nbr] = max_pkt_size;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_Close()
 *
 * @brief    Close a device endpoint, and uninitialize/clear endpoint configuration in hardware.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *******************************************************************************************************/
static void USBD_DrvEP_Close(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT32U          reg_val;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);
  reg_val = DEF_BIT_NONE;

  LOG_VRB(("USBD Drv EP Closed for endpoint addr: 0x", (X)ep_addr));

  if (USBD_EP_IS_IN(ep_addr)) {                                 // ------------------- IN ENDPOINTS -------------------
                                                                // Deactive the Endpoint
    DEF_BIT_CLR(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_USBAEP);
    reg_val = DEF_BIT(ep_log_nbr);
  } else {                                                      // ------------------ OUT ENDPOINTS -------------------
                                                                // Deactive the Endpoint
    DEF_BIT_CLR(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_USBAEP);
    reg_val = (DEF_BIT(ep_log_nbr) << 16u);
  }

  DEF_BIT_CLR(p_reg->DAINTMSK, reg_val);                        // Dis. EP interrupt
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_RxStart()
 *
 * @brief    Configure endpoint with buffer to receive data.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to data buffer.
 *
 * @param    buf_len     Length of the buffer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Number of bytes that can be handled in one transfer.
 *******************************************************************************************************/
static CPU_INT32U USBD_DrvEP_RxStart(USBD_DRV   *p_drv,
                                     CPU_INT08U ep_addr,
                                     CPU_INT08U *p_buf,
                                     CPU_INT32U buf_len,
                                     RTOS_ERR   *p_err)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT08U          ep_phy_nbr;
  CPU_INT16U          ep_pkt_len;
  CPU_INT16U          pkt_cnt;
  CPU_INT32U          ctl_reg;
  CPU_INT32U          tsiz_reg;
  USBD_DRV_DATA_EP    *p_drv_data;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);
  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  ep_pkt_len = (CPU_INT16U)DEF_MIN(buf_len, p_drv_data->EP_MaxPktSize[ep_phy_nbr]);

  LOG_VRB(("USBD Drv EP FIFO RxStart for endpoint addr: 0x", (X)ep_addr));

  ctl_reg = p_reg->DOEP[ep_log_nbr].CTLx;                       // Read Control EP reg
  tsiz_reg = p_reg->DOEP[ep_log_nbr].TSIZx;                     // Read Transer EP reg

  //                                                               Clear EP transfer size and packet count
  DEF_BIT_CLR(tsiz_reg, (DOEPTSIZx_XFRSIZ_MSK | DOEPTSIZx_PKTCNT_MSK));

  if (buf_len == 0u) {
    tsiz_reg |= p_drv_data->EP_MaxPktSize[ep_phy_nbr];          // Set transfer size to max pkt size
    tsiz_reg |= (1u << 19u);                                    // Set packet count
  } else {
    pkt_cnt = (ep_pkt_len + (p_drv_data->EP_MaxPktSize[ep_phy_nbr] - 1u))
              / p_drv_data->EP_MaxPktSize[ep_phy_nbr];
    tsiz_reg |= (pkt_cnt << 19u);                               // Set packet count
                                                                // Set transfer size
    tsiz_reg |= pkt_cnt * p_drv_data->EP_MaxPktSize[ep_phy_nbr];
  }

  p_drv_data->EP_AppBufPtr[ep_phy_nbr] = p_buf;
  p_drv_data->EP_AppBufLen[ep_phy_nbr] = ep_pkt_len;

  //                                                               Clear EP NAK and Enable EP for receiving
  ctl_reg |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;

  p_drv_data->EP_PktXferLen[ep_phy_nbr] = 0u;
  p_reg->DOEP[ep_log_nbr].TSIZx = tsiz_reg;
  p_reg->DOEP[ep_log_nbr].CTLx = ctl_reg;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (ep_pkt_len);
}

/****************************************************************************************************//**
 *                                               USBD_DrvEP_Rx()
 *
 * @brief    Receive the specified amount of data from device endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to data buffer.
 *
 * @param    buf_len     Length of the buffer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_RX
 *
 * @return   Number of octets received, if NO error(s).
 *           0,                         otherwise.
 *******************************************************************************************************/
static CPU_INT32U USBD_DrvEP_Rx(USBD_DRV   *p_drv,
                                CPU_INT08U ep_addr,
                                CPU_INT08U *p_buf,
                                CPU_INT32U buf_len,
                                RTOS_ERR   *p_err)
{
  CPU_INT08U       ep_phy_nbr;
  CPU_INT16U       xfer_len;
  USBD_DRV_DATA_EP *p_drv_data;

  (void)&p_buf;

  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  xfer_len = p_drv_data->EP_PktXferLen[ep_phy_nbr];

  LOG_VRB(("USBD Drv EP FIFO Rx for endpoint addr: 0x", (X)ep_addr));

  if (xfer_len > buf_len) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  return (DEF_MIN(xfer_len, buf_len));
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_RxZLP()
 *
 * @brief    Receive zero-length packet from endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBD_DrvEP_RxZLP(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr,
                             RTOS_ERR   *p_err)
{
  (void)&p_drv;
  (void)&ep_addr;

  LOG_VRB(("USBD Drv EP RxZLP for endpoint addr: 0x", (X)ep_addr));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBD_DrvEP_Tx()
 *
 * @brief    Configure endpoint with buffer to transmit data.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted.
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Number of octets transmitted, if NO error(s).
 *           0,                            otherwise.
 *******************************************************************************************************/
static CPU_INT32U USBD_DrvEP_Tx(USBD_DRV   *p_drv,
                                CPU_INT08U ep_addr,
                                CPU_INT08U *p_buf,
                                CPU_INT32U buf_len,
                                RTOS_ERR   *p_err)
{
  CPU_INT08U       ep_phy_nbr;
  CPU_INT16U       ep_pkt_len;
  USBD_DRV_DATA_EP *p_drv_data;

  (void)&p_buf;

  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  ep_pkt_len = (CPU_INT16U)DEF_MIN(p_drv_data->EP_MaxPktSize[ep_phy_nbr], buf_len);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (ep_pkt_len);
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_TxStart()
 *
 * @brief    Transmit the specified amount of data to device endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted.
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void USBD_DrvEP_TxStart(USBD_DRV   *p_drv,
                               CPU_INT08U ep_addr,
                               CPU_INT08U *p_buf,
                               CPU_INT32U buf_len,
                               RTOS_ERR   *p_err)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT32U          ctl_reg;
  CPU_INT32U          tsiz_reg;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)(p_drv->CfgPtr->BaseAddr);
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);

  ctl_reg = p_reg->DIEP[ep_log_nbr].CTLx;                       // Read EP control reg.
  tsiz_reg = p_reg->DIEP[ep_log_nbr].TSIZx;                     // Read EP transfer reg

  DEF_BIT_CLR(tsiz_reg, DIEPTSIZx_XFRSIZ_MSK);                  // Clear EP transfer size
  tsiz_reg |= buf_len;                                          // Transfer size
  tsiz_reg |= (1u << 19u);                                      // Packet count

  //                                                               Clear EP NAK mode & Enable EP transmitting.
  ctl_reg |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;

  p_reg->DIEP[ep_log_nbr].TSIZx = tsiz_reg;
  p_reg->DIEP[ep_log_nbr].CTLx = ctl_reg;

  DWC_TxFIFO_Wr(p_drv, ep_log_nbr, p_buf, buf_len);             // Write to Tx FIFO of associated EP

  LOG_VRB(("USBD Drv EP addr 0x ", (X)ep_addr, " using FIFO has tx'd ", (u)buf_len, "bytes"));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_TxZLP()
 *
 * @brief    Transmit zero-length packet from endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_FAIL
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *******************************************************************************************************/
static void USBD_DrvEP_TxZLP(USBD_DRV   *p_drv,
                             CPU_INT08U ep_addr,
                             RTOS_ERR   *p_err)
{
  LOG_VRB(("USBD Drv EP TxZLP on endpoint addr: 0x", (X)ep_addr));

  USBD_DrvEP_TxStart(p_drv,
                     ep_addr,
                     (CPU_INT08U *)0,
                     0u,
                     p_err);
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_Abort()
 *
 * @brief    Abort any pending transfer on endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint Address.
 *
 * @return   DEF_OK,   if NO error(s).
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_DrvEP_Abort(USBD_DRV   *p_drv,
                                    CPU_INT08U ep_addr)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT32U          reg_to;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);

  if (USBD_EP_IS_IN(ep_addr) == DEF_YES) {                      // ------------------- IN ENDPOINTS -------------------
                                                                // Set Endpoint to NAK mode
    DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SNAK);

    reg_to = REG_VAL_TO;                                        // Wait for IN EP NAK effective
    while ((DEF_BIT_IS_CLR(p_reg->DIEP[ep_log_nbr].INTx, DIEPINTx_BIT_INEPNE) == DEF_YES)
           && (reg_to > 0u)) {
      reg_to--;
    }

    DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_EPDIS);

    reg_to = REG_VAL_TO;                                        // Wait for EP disable
    while ((DEF_BIT_IS_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_EPDIS) == DEF_YES)
           && (reg_to > 0u)) {
      reg_to--;
    }
    //                                                             Clear EP Disable interrupt
    DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].INTx, DIEPINTx_BIT_EPDISD);

    //                                                             Flush EPx TX FIFO
    p_reg->GRSTCTL = GRSTCTL_BIT_TXFFLSH |  (ep_log_nbr << 6u);

    reg_to = REG_VAL_TO;                                        // Wait for the flush completion
    while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_TXFFLSH) == DEF_YES)
           && (reg_to > 0u)) {
      reg_to--;
    }
  } else {                                                      // ------------------ OUT ENDPOINTS -------------------
    //                                                             Set Endpoint to NAK mode
    DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SNAK);

    //                                                             Flush EPx RX FIFO
    DEF_BIT_SET(p_reg->GRSTCTL, GRSTCTL_BIT_RXFFLSH);

    reg_to = REG_VAL_TO;                                        // Wait for the flush completion
    while ((DEF_BIT_IS_SET(p_reg->GRSTCTL, GRSTCTL_BIT_RXFFLSH) == DEF_YES)
           && (reg_to > 0u)) {
      reg_to--;
    }
  }

  LOG_VRB(("USBD Drv EP Abort for endpoint addr: 0x", (X)ep_addr));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBD_DrvEP_Stall()
 *
 * @brief    Set or clear stall condition on endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    state       Endpoint stall state.
 *
 * @return   DEF_OK,   if NO error(s).
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_DrvEP_Stall(USBD_DRV    *p_drv,
                                    CPU_INT08U  ep_addr,
                                    CPU_BOOLEAN state)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT32U          ep_type;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  ep_log_nbr = USBD_EP_ADDR_TO_LOG(ep_addr);

  LOG_VRB(("USBD Drv EP Stall for endpoint addr:", (X)ep_addr));

  if (USBD_EP_IS_IN(ep_addr) == DEF_YES) {                      // ------------------- IN ENDPOINTS -------------------
    if (state == DEF_SET) {
      //                                                           Disable Endpoint if enable for transmit
      if (DEF_BIT_IS_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_EPENA) == DEF_YES) {
        DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_EPDIS);
      }
      //                                                           Set Stall bit
      DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_STALL);
    } else {
      ep_type = (p_reg->DIEP[ep_log_nbr].CTLx >> 18u) & DEF_BIT_FIELD(2u, 0u);
      //                                                           Clear Stall Bit
      DEF_BIT_CLR(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_STALL);

      if ((ep_type == USBD_EP_TYPE_INTR)
          || (ep_type == USBD_EP_TYPE_BULK)) {
        //                                                         Set DATA0 PID
        DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SD0PID);
      }
    }
  } else {                                                      // ------------------- OUT ENDPOINTS ------------------
    if (state == DEF_SET) {
      //                                                           Set Stall bit
      DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_STALL);
    } else {
      ep_type = (p_reg->DOEP[ep_log_nbr].CTLx >> 18u) & DEF_BIT_FIELD(2u, 0u);
      //                                                           Clear Stall Bit
      DEF_BIT_CLR(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_STALL);

      if ((ep_type == USBD_EP_TYPE_INTR) || (ep_type == USBD_EP_TYPE_BULK)) {
        //                                                         Set DATA0 PID
        DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SD0PID);
      }
    }
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBD_DrvISR_Handler()
 *
 * @brief    USB device Interrupt Service Routine (ISR) handler.
 *
 * @param    p_drv   Pointer to device driver structure.
 *******************************************************************************************************/
static void USBD_DrvISR_Handler(USBD_DRV *p_drv)
{
  CPU_INT32U          int_stat;
  USBD_DWC_OTG_FS_REG *p_reg;
  USBD_DRV_DATA_EP    *p_drv_data;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  int_stat = p_reg->GINTSTS;                                    // Read global interrupt status register

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  CPU_INT32U int_status_wrapper = USB_APBS->IF;

  //                                                               ------------------ VBUS INTERRUPT ------------------
  if (DEF_BIT_IS_SET(int_status_wrapper, _USB_IF_VBUS_MASK) == DEF_YES) {
    USB_APBS->IF_CLR = USB_IF_VBUS;                             // Clear interrupt.
    //                                                             Detect VBUS high event.
    if (DEF_BIT_IS_SET(USB_APBS->STATUS, _USB_STATUS_VBUSVALID_MASK) == DEF_YES) {
      USBD_DRV_PHY_RESUME();

      USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSHEN;                  // Disable VBUS sense high.
      USB_APBS->CTRL_SET = USB_CTRL_VBSSNSLEN;                  // Enable VBUS sense low to detect disconnection.

      USBD_EventConn(p_drv);                                    // Notify connect event.
    } else {
      USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSLEN;                  // Disable VBUS sense low.
      USB_APBS->CTRL_SET = USB_CTRL_VBSSNSHEN;                  // Enable VBUS sense high to detect disconnection.

      USBD_EventDisconn(p_drv);                                 // Notify disconnect event.
      p_drv_data->EnumDone = DEF_NO;
      USBD_DRV_PHY_SUSPEND();
    }
  }
#else
  //                                                               -------------- SESSION REQ DETECTION ---------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_SRQINT) == DEF_YES) {
    USBD_DRV_PHY_RESUME();

    USBD_EventConn(p_drv);                                      // Notify connect event.

    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_SRQINT);
  }

  //                                                               ------------------ OTG INTERRUPT -------------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_OTGINT) == DEF_YES) {
    CPU_INT32U otgint_stat = p_reg->GOTGINT;

    //                                                             Clear all OTG interrupt sources.
    DEF_BIT_SET(p_reg->GOTGINT, (GOTGINT_BIT_SEDET
                                 | GOTGINT_BIT_SRSSCHG
                                 | GOTGINT_BIT_HNSSCHG
                                 | GOTGINT_BIT_HNGDET
                                 | GOTGINT_BIT_ADTOCHG
                                 | GOTGINT_BIT_DBCDNE));

    if (DEF_BIT_IS_SET(otgint_stat, GOTGINT_BIT_SEDET) == DEF_YES) {
      USBD_EventDisconn(p_drv);                                 // Notify disconnect event.

      p_drv_data->EnumDone = DEF_NO;
      USBD_DRV_PHY_SUSPEND();
    }
  }
#endif

  //                                                               ------------ RX FIFO NON-EMPTY DETECTION -----------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_RXFLVL) == DEF_YES) {
    DWC_RxFIFO_Rd(p_drv);
  }

  //                                                               --------------- IN ENDPOINT INTERRUPT --------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_IEPINT) == DEF_YES) {
    DWC_EP_InProcess(p_drv);
  }

  //                                                               -------------- OUT ENDPOINT INTERRUPT --------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_OEPINT) == DEF_YES) {
    DWC_EP_OutProcess(p_drv);
  }

  //                                                               ---------------- USB RESET DETECTION ---------------
  if ((DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_USBRST) == DEF_YES)
      || (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_RESETDET) == DEF_YES)) {
    uint8_t ep_ix;
    uint32_t ep_in_ram_start_address = 0u;

    USBD_DRV_PHY_RESUME();

    USBD_EventReset(p_drv);                                     // Notify bus reset event.
                                                                // Enable Global OUT/IN EP interrupt...
                                                                // ...and RX FIFO non-empty interrupt.
    DEF_BIT_CLR(p_reg->DCTL, DCTL_BIT_RWUSIG);                  // Clear Remote wakeup signaling

    DEF_BIT_SET(p_reg->GINTMSK, (GINTMSK_BIT_OEPINT
                                 | GINTMSK_BIT_IEPINT
                                 | GINTMSK_BIT_RXFLVLM));

    //                                                             Clear USB bus reset interrupt
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_USBRST);
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_RESETDET);

    USBD_DrvAddrSet(p_drv, 0u);

    p_reg->GRXFSIZ = RXFIFO_SIZE;                               // Set Rx FIFO depth
    //                                                             Set EP0 to EPx Tx FIFO depth
    p_reg->DIEPTXF0 = (TXFIFO_EP0_SIZE << 16u) |  RXFIFO_SIZE;  // Control endpoint.
    for (ep_ix = 0; ep_ix < (NBR_EPS_IN - 1u); ep_ix++) {       // Non-control endpoints.
      ep_in_ram_start_address = RXFIFO_SIZE + ((ep_ix + 1u) * TXFIFO_EPx_SIZE);
      p_reg->DIEPTXFx[ep_ix] = (TXFIFO_EPx_SIZE << 16u) | ep_in_ram_start_address;
    }

    p_drv_data->EnumDone = DEF_NO;
  }

  //                                                               ------------ ENUMERATION DONE DETECTION ------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_ENUMDNE) == DEF_YES) {
    if (DEF_BIT_IS_SET(p_reg->DSTS, DSTS_ENUMSPD_FS_PHY_48MHZ) == DEF_YES) {
      DEF_BIT_CLR(p_reg->DIEP[0].CTLx, DxEPCTL0_MPSIZ_64_MSK);
      DEF_BIT_CLR(p_reg->GUSBCFG, GUSBCFG_TRDT_MASK);
      p_reg->GUSBCFG |= (5 << 10);                              // turn around time
    }

    DEF_BIT_SET(p_reg->DCTL, DCTL_BIT_CGINAK);
    //                                                             Clear Enumeration done interrupt
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_ENUMDNE);

    p_drv_data->EnumDone = DEF_YES;
  }

  //                                                               ------------------ MODE MISMATCH -------------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_MMIS) == DEF_YES) {
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_MMIS);
  }

  //                                                               ------------- EARLY SUSPEND DETECTION --------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_ESUSP) == DEF_YES) {
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_ESUSP);
  }

  //                                                               --------------- USB SUSPEND DETECTION --------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_USBSUSP) == DEF_YES) {
    if (DEF_BIT_IS_SET(p_reg->DSTS, DSTS_BIT_SUSPSTS) == DEF_YES) {
      USBD_EventSuspend(p_drv);                                 // Notify Suspend Event
    }

    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_USBSUSP);

    if (p_drv_data->EnumDone) {
      USBD_DRV_PHY_SUSPEND();
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
      // TODO CM the clock switch will be handled outside the interrupt by using the power manager PLATFORM_MTL-4674
//      CMU_ClockSelectSet(cmuClock_USB, cmuSelect_LFXO);         // Select 32KHz LFXO for accurate timing reference.
#endif
    }
  }

  //                                                               ----------------- WAKE-UP DETECTION ----------------
  if (DEF_BIT_IS_SET(int_stat, GINTSTS_BIT_WKUPINT) == DEF_YES) {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
    // TODO CM the clock switch will be handled outside the interrupt by using the power manager PLATFORM_MTL-4674
//    CMU_ClockSelectSet(cmuClock_USB, cmuSelect_PLL0);           // Select 48MHz clock source.
#endif
    USBD_DRV_PHY_RESUME();

    DEF_BIT_CLR(p_reg->DCTL, DCTL_BIT_RWUSIG);                  // Clear Remote wakeup signaling
    DEF_BIT_SET(p_reg->GINTSTS, GINTSTS_BIT_WKUPINT);           // Clear Remote wakeup interrupt
    USBD_EventResume(p_drv);                                    // Notify Resume Event
  }

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  if (DEF_BIT_IS_SET(int_status_wrapper, _USB_IF_DWCOTG_MASK) == DEF_YES) {
    USB_APBS->IF_CLR = USB_IF_DWCOTG;                         // Clear main interrupt.
  }
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           DWC_RxFIFO_Rd()
 *
 * @brief       Handle Rx FIFO non-empty interrupt generated when a data OUT packet has been received.
 *
 * @param       p_drv   Pointer to device driver structure.
 *******************************************************************************************************/
static void DWC_RxFIFO_Rd(USBD_DRV *p_drv)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT08U          ep_phy_nbr;
  CPU_INT08U          pkt_stat;
  CPU_INT08U          byte_rem;
  CPU_INT08U          word_cnt;
  CPU_INT16U          byte_cnt;
  CPU_INT32U          *p_data_buf;
  CPU_INT32U          reg_val;
  CPU_INT32U          i;
  CPU_INT32U          data;
  USBD_DRV_DATA_EP    *p_drv_data;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  reg_val = p_reg->GRXSTSP;
  ep_log_nbr = (reg_val & GRXSTSx_EPNUM_MASK)  >>  0u;
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_LOG_TO_ADDR_OUT(ep_log_nbr));
  byte_cnt = (reg_val & GRXSTSx_BCNT_MASK)   >>  4u;
  pkt_stat = (reg_val & GRXSTSx_PKTSTS_MASK) >> 17u;

  p_drv_data->EP_PktXferLen[ep_phy_nbr] += byte_cnt;

  //                                                               Disable Rx FIFO non-empty
  DEF_BIT_CLR(p_reg->GINTMSK, GINTMSK_BIT_RXFLVLM);

  switch (pkt_stat) {
    case GRXSTSx_PKTSTS_SETUP_RX:
      p_data_buf = &p_drv_data->EP_SetupBuf[0u];
      if (byte_cnt == 8u) {                                     // Read Setup packet from Rx FIFO
        p_data_buf[0u] = *p_reg->DFIFO[ep_log_nbr].DATA;
        p_data_buf[1u] = *p_reg->DFIFO[ep_log_nbr].DATA;
      }

      DEF_BIT_SET(p_reg->DOEP[0u].CTLx, DxEPCTLx_BIT_SNAK);
      break;

    case GRXSTSx_PKTSTS_OUT_RX:
      DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx,                 // Put endpoint in NACK mode.
                  DxEPCTLx_BIT_SNAK);

      if ((byte_cnt > 0u)
          && (p_drv_data->EP_AppBufPtr[ep_phy_nbr] != (CPU_INT08U *)0)) {
        byte_cnt = DEF_MIN(byte_cnt, p_drv_data->EP_AppBufLen[ep_phy_nbr]);
        word_cnt = (byte_cnt / 4u);

        //                                                         Check app buffer alignment.
        if ((((CPU_INT32U)p_drv_data->EP_AppBufPtr[ep_phy_nbr]) % 4u) == 0u) {
          p_data_buf = (CPU_INT32U *)p_drv_data->EP_AppBufPtr[ep_phy_nbr];
        } else {
          p_data_buf = &p_drv_data->DataBuf[0u];                // Use drv internal aligned buf if app buf not aligned.
        }

        //                                                         Read OUT packet from Rx FIFO
        for (i = 0u; i < word_cnt; i++) {
          *p_data_buf = *p_reg->DFIFO[ep_log_nbr].DATA;
          p_data_buf++;
        }

        byte_rem = (byte_cnt - (word_cnt * 4u));
        if (byte_rem > 0u) {                                    // Rd remaining data if byte_cnt not multiple of 4.
          data = *p_reg->DFIFO[ep_log_nbr].DATA;

          for (i = 0u; i < byte_rem; i++) {
            ((CPU_INT08U *)p_data_buf)[i] = (CPU_INT08U)((data >> (8u * i)) & 0xFFu);
          }
        }

        //                                                         Copy data to app buf if not aligned.
        if ((((CPU_INT32U)p_drv_data->EP_AppBufPtr[ep_phy_nbr]) % 4u) != 0u) {
          p_data_buf = &p_drv_data->DataBuf[0u];
          for (i = 0u; i < byte_cnt; i++) {
            p_drv_data->EP_AppBufPtr[ep_phy_nbr][i] = ((CPU_INT08U *)p_data_buf)[i];
          }
        }

        p_drv_data->EP_AppBufPtr[ep_phy_nbr] = (CPU_INT08U *)0;
      }
      break;

    case GRXSTSx_PKTSTS_OUT_COMPL:
    case GRXSTSx_PKTSTS_SETUP_COMPL:
    case GRXSTSx_PKTSTS_OUT_NAK:
    default:
      break;
  }
  //                                                               Enable Rx FIFO non-empty interrupt
  DEF_BIT_SET(p_reg->GINTMSK, GINTMSK_BIT_RXFLVLM);
}

/****************************************************************************************************//**
 *                                           DWC_TxFIFO_Wr()
 *
 * @brief       Writes a packet into the Tx FIFO associated with the Endpoint.
 *
 * @param       p_drv       Pointer to device driver structure.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted.
 *
 * @param    ep_pkt_len  Number of octets to transmit.
 *******************************************************************************************************/
static void DWC_TxFIFO_Wr(USBD_DRV   *p_drv,
                          CPU_INT08U ep_log_nbr,
                          CPU_INT08U *p_buf,
                          CPU_INT16U ep_pkt_len)
{
  CPU_INT32U          nbr_words;
  CPU_INT32U          i;
  CPU_INT32U          words_avail;
  CPU_INT32U          *p_data_buf;
  USBD_DWC_OTG_FS_REG *p_reg;
  USBD_DRV_DATA_EP    *p_drv_data;
  CORE_DECLARE_IRQ_STATE;

  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  nbr_words = (ep_pkt_len + 3u) / 4u;

  do {
    //                                                             Read available words in the EP Tx FIFO
    words_avail = p_reg->DIEP[ep_log_nbr].DTXFSTSx & 0x0000FFFFu;
  } while (words_avail < nbr_words);                            // Check if there are enough words to write into FIFO

  CORE_ENTER_ATOMIC();
  if (((CPU_INT32U)p_buf % 4u) == 0u) {                         // Check app buffer alignment.
    p_data_buf = (CPU_INT32U *)p_buf;
  } else {
    p_data_buf = &p_drv_data->DataBuf[0u];                      // Use drv internal aligned buf if app buf not aligned.

    for (i = 0u; i < ep_pkt_len; i++) {
      ((CPU_INT08U *)p_data_buf)[i] = p_buf[i];
    }
  }

  for (i = 0u; i < nbr_words; i++) {
    *p_reg->DFIFO[ep_log_nbr].DATA = *p_data_buf;
    p_data_buf++;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           DWC_EP_OutProcess()
 *
 * @brief       Process OUT interrupts on associated EP.
 *
 * @param       p_drv       Pointer to device driver structure.
 *******************************************************************************************************/
static void DWC_EP_OutProcess(USBD_DRV *p_drv)
{
  CPU_INT32U          ep_int_stat;
  CPU_INT32U          dev_ep_int;
  CPU_INT32U          ep_log_nbr;
  USBD_DRV_DATA_EP    *p_drv_data;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  p_drv_data = (USBD_DRV_DATA_EP *)p_drv->DataPtr;
  dev_ep_int = p_reg->DAINT >> 16u;                             // Read all Device OUT Endpoint interrupt

  while (dev_ep_int != DEF_BIT_NONE) {
    ep_log_nbr = (CPU_INT08U)(31u - CPU_CntLeadZeros32(dev_ep_int & 0x0000FFFFu));
    ep_int_stat = p_reg->DOEP[ep_log_nbr].INTx;                 // Read OUT EP interrupt status

    //                                                             Check if EP transfer completed occurred
    if (DEF_BIT_IS_SET(ep_int_stat, DOEPINTx_BIT_XFRC)) {
      USBD_EP_RxCmpl(p_drv, ep_log_nbr);
      //                                                           Clear EP transfer complete interrupt
      DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].INTx, DOEPINTx_BIT_XFRC);
    }
    //                                                             Check if EP Setup phase done interrupt occurred
    if (DEF_BIT_IS_SET(ep_int_stat, DOEPINTx_BIT_STUP)) {
      USBD_EventSetup(p_drv, (void *)&p_drv_data->EP_SetupBuf[0u]);
      //                                                           Clear EP0 SETUP complete interrupt
      DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].INTx, DOEPINTx_BIT_STUP);

      //                                                           Re-enable EP for next setup pkt.
      DEF_BIT_SET(p_reg->DOEP[0u].CTLx, DxEPCTLx_BIT_EPENA);
    }
    //                                                             Set Endpoint to NAK mode
    DEF_BIT_SET(p_reg->DOEP[ep_log_nbr].CTLx, DxEPCTLx_BIT_SNAK);

    dev_ep_int = p_reg->DAINT >> 16u;                           // Read all Device OUT Endpoint interrupt
  }
}

/********************************************************************************************************
 *                                           DWC_EP_InProcess()
 *
 * @brief       Process IN interrupts on associated EP.
 *
 * @param       p_drv       Pointer to device driver structure.
 *******************************************************************************************************/
static void DWC_EP_InProcess(USBD_DRV *p_drv)
{
  CPU_INT08U          ep_log_nbr;
  CPU_INT32U          ep_int_stat;
  CPU_INT32U          dev_ep_int;
  USBD_DWC_OTG_FS_REG *p_reg;

  p_reg = (USBD_DWC_OTG_FS_REG *)p_drv->CfgPtr->BaseAddr;
  dev_ep_int = (p_reg->DAINT & 0x0000FFFFu);                    // Read all Device IN Endpoint interrupt

  while (dev_ep_int != DEF_BIT_NONE) {
    ep_log_nbr = (CPU_INT08U)(31u - CPU_CntLeadZeros32(dev_ep_int & 0x0000FFFFu));
    ep_int_stat = p_reg->DIEP[ep_log_nbr].INTx;                 // Read IN EP interrupt status

    //                                                             Check if EP transfer completed interrupt occurred
    if (DEF_BIT_IS_SET(ep_int_stat, DIEPINTx_BIT_XFRC)) {
      USBD_EP_TxCmpl(p_drv, ep_log_nbr);
      //                                                           Clear EP transfer complete interrupt
      DEF_BIT_SET(p_reg->DIEP[ep_log_nbr].INTx, DIEPINTx_BIT_XFRC);
    }

    dev_ep_int = (p_reg->DAINT & 0x0000FFFFu);                  // Read all Device IN Endpoint interrupt
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_AVAIL))
