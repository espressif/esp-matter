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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stddef.h>
#include <stdbool.h>

#include "sl_bit.h"
#include "sl_status.h"

#include "em_device.h"
#include "em_core.h"
#include "em_cmu.h"

#include "sl_usbd_core.h"

#include "sli_usbd_core.h"
#include "sli_usbd_driver.h"

#if defined(_SILICON_LABS_32B_SERIES_2)
#include "sl_usbd_driver_config.h"
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
// Define module name for Power Manager debug feature
#define CURRENT_MODULE_NAME  "USB_DEVICE"
#include "sl_power_manager.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_2)
// FIXME Specific to board BRD4270A issue
#define USB_DP_PORT           gpioPortD
#define USB_DP_PIN            6
#define USB_DM_PORT           gpioPortD
#define USB_DM_PIN            7
#endif

#define  REG_VAL_TO                        0x1Fu
#define  REG_FMOD_TO                       0x7FFFFu

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define USB_REG        USB

#define DIEP_REG       DIEP
#define DOEP_REG       DOEP
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define USB_REG        USBAHB_AHBS
#define USB_APBS_REG   USB_APBS

#define DIEP_REG       DEVINEP
#define DOEP_REG       DEVOUTEP
#endif

// With Buffer DMA, data buffers must be 32 bits aligned
#define BUFFER_BYTE_ALIGNMENT   4u

/********************************************************************************************************
 *                                   DWC OTG USB DEVICE CONSTRAINTS
 *
 * Note(s) : (1) The USB system features a dedicated data RAM for endpoints. The endpoints RAM size varies
 *               according to the SoC.
 *
 *                   (a) RX-FIFO: The USB device uses a single receive FIFO that receives the data directed
 *                                to all OUT enpoints. The size of the receive FIFO is configured with
 *                                the receive FIFO size register (GRXFSIZ). The value written to GRXFSIZ is
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

#define  DFIFO_SIZE                        1024u
#define  MAX_PKT_SIZE                        64u

#define  RXFIFO_SIZE                        128u                // See note #1a.
#define  TXFIFO_EPx_SIZE                     64u                // See note #1b.
#define  TXFIFO_EP0_SIZE                     64u
#define  TXFIFO_EP1_SIZE                     64u
#define  TXFIFO_EP2_SIZE                     64u
#define  TXFIFO_EP3_SIZE                     64u
#define  TXFIFO_EP4_SIZE                     64u
#define  TXFIFO_EP5_SIZE                     64u

#if defined(_SILICON_LABS_32B_SERIES_2)
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

#define SETUP_PACKET_SIZE  8u

/********************************************************************************************************
 *                                           REGISTER BIT DEFINES
 *******************************************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GOTGINT_BIT_SEDET             USB_GOTGINT_SESENDDET
#define  GOTGINT_BIT_SRSSCHG           USB_GOTGINT_SESREQSUCSTSCHNG
#define  GOTGINT_BIT_HNSSCHG           USB_GOTGINT_HSTNEGSUCSTSCHNG
#define  GOTGINT_BIT_HNGDET            USB_GOTGINT_HSTNEGDET
#define  GOTGINT_BIT_ADTOCHG           USB_GOTGINT_ADEVTOUTCHG
#define  GOTGINT_BIT_DBCDNE            USB_GOTGINT_DBNCEDONE
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GINTSTS_BIT_WKUPINT           USB_GINTSTS_WKUPINT
#define  GINTSTS_BIT_SRQINT            USB_GINTSTS_SESSREQINT
#define  GINTSTS_BIT_RESETDET          USB_GINTSTS_RESETDET
#define  GINTSTS_BIT_OEPINT            USB_GINTSTS_OEPINT
#define  GINTSTS_BIT_IEPINT            USB_GINTSTS_IEPINT
#define  GINTSTS_BIT_ENUMDNE           USB_GINTSTS_ENUMDONE
#define  GINTSTS_BIT_USBRST            USB_GINTSTS_USBRST
#define  GINTSTS_BIT_USBSUSP           USB_GINTSTS_USBSUSP
#define  GINTSTS_BIT_ESUSP             USB_GINTSTS_ERLYSUSP
#define  GINTSTS_BIT_RXFLVL            USB_GINTSTS_RXFLVL
#define  GINTSTS_BIT_OTGINT            USB_GINTSTS_OTGINT
#define  GINTSTS_BIT_MMIS              USB_GINTSTS_MODEMIS
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GINTSTS_BIT_WKUPINT           USBAHB_GINTSTS_WKUPINT
#define  GINTSTS_BIT_SRQINT            USBAHB_GINTSTS_SESSREQINT
#define  GINTSTS_BIT_RESETDET          USBAHB_GINTSTS_RESETDET
#define  GINTSTS_BIT_OEPINT            USBAHB_GINTSTS_OEPINT
#define  GINTSTS_BIT_IEPINT            USBAHB_GINTSTS_IEPINT
#define  GINTSTS_BIT_ENUMDNE           USBAHB_GINTSTS_ENUMDONE
#define  GINTSTS_BIT_USBRST            USBAHB_GINTSTS_USBRST
#define  GINTSTS_BIT_USBSUSP           USBAHB_GINTSTS_USBSUSP
#define  GINTSTS_BIT_ESUSP             USBAHB_GINTSTS_ERLYSUSP
#define  GINTSTS_BIT_RXFLVL            USBAHB_GINTSTS_RXFLVL
#define  GINTSTS_BIT_OTGINT            USBAHB_GINTSTS_OTGINT
#define  GINTSTS_BIT_MMIS              USBAHB_GINTSTS_MODEMIS
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GUSBCFG_TRDT_MASK             _USB_GUSBCFG_USBTRDTIM_MASK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GUSBCFG_TRDT_MASK             _USBAHB_GUSBCFG_USBTRDTIM_MASK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GAHBCFG_BIT_GLBLINTRMSK           USB_GAHBCFG_GLBLINTRMSK
#define  GAHBCFG_BIT_DMAEN                 USB_GAHBCFG_DMAEN
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GAHBCFG_BIT_GLBLINTRMSK           USBAHB_GAHBCFG_GLBLINTRMSK
#define  GAHBCFG_BIT_DMAEN                 USBAHB_GAHBCFG_DMAEN
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GRSTCTL_BIT_TXFFLSH           USB_GRSTCTL_TXFFLSH
#define  GRSTCTL_BIT_RXFFLSH           USB_GRSTCTL_RXFFLSH
#define  GRSTCTL_BIT_CSFTRST           USB_GRSTCTL_CSFTRST
#define  GRSTCTL_BIT_AHBIDLE           USB_GRSTCTL_AHBIDLE
#define  GRSTCTL_TXFNUM_SHIFT          _USB_GRSTCTL_TXFNUM_SHIFT
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GRSTCTL_BIT_TXFFLSH           USBAHB_GRSTCTL_TXFFLSH
#define  GRSTCTL_BIT_RXFFLSH           USBAHB_GRSTCTL_RXFFLSH
#define  GRSTCTL_BIT_CSFTRST           USBAHB_GRSTCTL_CSFTRST
#define  GRSTCTL_BIT_AHBIDLE           USBAHB_GRSTCTL_AHBIDLE
#define  GRSTCTL_TXFNUM_SHIFT          _USBAHB_GRSTCTL_TXFNUM_SHIFT
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DCTL_BIT_CGINAK               USB_DCTL_CGNPINNAK
#define  DCTL_BIT_SDIS                 USB_DCTL_SFTDISCON
#define  DCTL_BIT_RWUSIG               USB_DCTL_RMTWKUPSIG
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DCTL_BIT_CGINAK               USBAHB_DCTL_CGNPINNAK
#define  DCTL_BIT_SDIS                 USBAHB_DCTL_SFTDISCON
#define  DCTL_BIT_RWUSIG               USBAHB_DCTL_RMTWKUPSIG
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DxEPCTLx_BIT_EPENA            USB_DIEP_CTL_EPENA      // Same as USB_DOEP_CTL_EPENA
#define  DxEPCTLx_BIT_EPDIS            USB_DIEP_CTL_EPDIS      // Same as USB_DOEP_CTL_EPDIS
#define  DxEPCTLx_BIT_SD0PID           USB_DIEP_CTL_SETD0PIDEF // Same as USB_DOEP_CTL_SETD0PIDEF
#define  DxEPCTLx_BIT_SNAK             USB_DIEP_CTL_SNAK       // Same as USB_DOEP_CTL_SNAK
#define  DxEPCTLx_BIT_CNAK             USB_DIEP_CTL_CNAK       // Same as USB_DOEP_CTL_CNAK
#define  DxEPCTLx_BIT_STALL            USB_DIEP_CTL_STALL      // Same as USB_DOEP_CTL_STALL
#define  DxEPCTLx_BIT_USBAEP           USB_DIEP_CTL_USBACTEP   // Same as USB_DOEP_CTL_USBACTEP
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DxEPCTLx_BIT_EPENA            USBAHB_DEVINEP_CTL_EPENA      // Same as USBAHB_DEVOUTEP_CTLO_EPENA
#define  DxEPCTLx_BIT_EPDIS            USBAHB_DEVINEP_CTL_EPDIS      // Same as USBAHB_DEVOUTEP_CTLO_EPDIS
#define  DxEPCTLx_BIT_SD0PID           USBAHB_DEVINEP_CTL_SETD0PIDEF // Same as USBAHB_DEVOUTEP_CTLO_SETD0PIDEF
#define  DxEPCTLx_BIT_SNAK             USBAHB_DEVINEP_CTL_SNAK       // Same as USBAHB_DEVOUTEP_CTLO_SNAK
#define  DxEPCTLx_BIT_CNAK             USBAHB_DEVINEP_CTL_CNAK       // Same as USBAHB_DEVOUTEP_CTLO_CNAK
#define  DxEPCTLx_BIT_STALL            USBAHB_DEVINEP_CTL_STALL      // Same as USBAHB_DEVOUTEP_CTLO_STALL
#define  DxEPCTLx_BIT_USBAEP           USBAHB_DEVINEP_CTL_USBACTEP   // Same as USBAHB_DEVOUTEP_CTLO_USBACTEP
#endif

#define  DxEPCTL0_MPSIZ_64_MSK         0x3u

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GINTMSK_BIT_WUIM              USB_GINTMSK_WKUPINTMSK
#define  GINTMSK_BIT_SRQIM             USB_GINTMSK_SESSREQINTMSK
#define  GINTMSK_BIT_RESETDET          USB_GINTMSK_RESETDETMSK
#define  GINTMSK_BIT_OEPINT            USB_GINTMSK_OEPINTMSK
#define  GINTMSK_BIT_IEPINT            USB_GINTMSK_IEPINTMSK
#define  GINTMSK_BIT_ENUMDNEM          USB_GINTMSK_ENUMDONEMSK
#define  GINTMSK_BIT_USBRST            USB_GINTMSK_USBRSTMSK
#define  GINTMSK_BIT_USBSUSPM          USB_GINTMSK_USBSUSPMSK
#define  GINTMSK_BIT_RXFLVLM           USB_GINTMSK_RXFLVLMSK
#define  GINTMSK_BIT_OTGINT            USB_GINTMSK_OTGINTMSK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GINTMSK_BIT_WUIM              USBAHB_GINTMSK_WKUPINTMSK
#define  GINTMSK_BIT_RESETDET          USBAHB_GINTMSK_RESETDETMSK
#define  GINTMSK_BIT_OEPINT            USBAHB_GINTMSK_OEPINTMSK
#define  GINTMSK_BIT_IEPINT            USBAHB_GINTMSK_IEPINTMSK
#define  GINTMSK_BIT_ENUMDNEM          USBAHB_GINTMSK_ENUMDONEMSK
#define  GINTMSK_BIT_USBRST            USBAHB_GINTMSK_USBRSTMSK
#define  GINTMSK_BIT_USBSUSPM          USBAHB_GINTMSK_USBSUSPMSK
#define  GINTMSK_BIT_RXFLVLM           USBAHB_GINTMSK_RXFLVLMSK
#define  GINTMSK_BIT_OTGINT            USBAHB_GINTMSK_OTGINTMSK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  GRXSTSx_PKTSTS_OUT_NAK        _USB_GRXSTSR_PKTSTS_GOUTNAK
#define  GRXSTSx_PKTSTS_OUT_RX         _USB_GRXSTSR_PKTSTS_PKTRCV
#define  GRXSTSx_PKTSTS_OUT_COMPL      _USB_GRXSTSR_PKTSTS_XFERCOMPL
#define  GRXSTSx_PKTSTS_SETUP_COMPL    _USB_GRXSTSR_PKTSTS_SETUPCOMPL
#define  GRXSTSx_PKTSTS_SETUP_RX       _USB_GRXSTSR_PKTSTS_SETUPRCV
#define  GRXSTSx_PKTSTS_MASK           _USB_GRXSTSR_PKTSTS_MASK
#define  GRXSTSx_EPNUM_MASK            _USB_GRXSTSR_CHNUM_MASK // DEF_BIT_FIELD(2u, 0u) TODO not same mask
#define  GRXSTSx_BCNT_MASK             _USB_GRXSTSR_BCNT_MASK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  GRXSTSx_PKTSTS_OUT_NAK        0x00000001UL  // TODO fields not defined on SOL
#define  GRXSTSx_PKTSTS_OUT_RX         0x00000002UL
#define  GRXSTSx_PKTSTS_OUT_COMPL      0x00000003UL
#define  GRXSTSx_PKTSTS_SETUP_COMPL    0x00000004UL
#define  GRXSTSx_PKTSTS_SETUP_RX       0x00000006UL
#define  GRXSTSx_PKTSTS_MASK           _USBAHB_GRXSTSR_PKTSTS_MASK
#define  GRXSTSx_EPNUM_MASK            _USBAHB_GRXSTSR_EPNUM_MASK // DEF_BIT_FIELD(2u, 0u) TODO not same mask
#define  GRXSTSx_BCNT_MASK             _USBAHB_GRXSTSR_BCNT_MASK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  PCGCCTL_BIT_RSTPDWNMODULE     USB_PCGCCTL_RSTPDWNMODULE
#define  PCGCCTL_BIT_PWRCLMP           USB_PCGCCTL_PWRCLMP
#define  PCGCCTL_BIT_GATEHCLK          USB_PCGCCTL_GATEHCLK
#define  PCGCCTL_BIT_STOPPCLK          USB_PCGCCTL_STOPPCLK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  PCGCCTL_BIT_RSTPDWNMODULE     USBAHB_PCGCCTL_RSTPDWNMODULE
#define  PCGCCTL_BIT_PWRCLMP           USBAHB_PCGCCTL_PWRCLMP
#define  PCGCCTL_BIT_GATEHCLK          USBAHB_PCGCCTL_GATEHCLK
#define  PCGCCTL_BIT_STOPPCLK          USBAHB_PCGCCTL_STOPPCLK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DOEPMSK_BIT_STUPM             USB_DOEPMSK_SETUPMSK
#define  DOEPMSK_BIT_XFRCM             USB_DOEPMSK_XFERCOMPLMSK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DOEPMSK_BIT_STUPM             USBAHB_DOEPMSK_SETUPMSK
#define  DOEPMSK_BIT_XFRCM             USBAHB_DOEPMSK_XFERCOMPPLMSK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DIEPINTx_BIT_INEPNE           USB_DIEP_INT_INEPNAKEFF
#define  DIEPINTx_BIT_EPDISD           USB_DIEP_INT_EPDISBLD
#define  DIEPINTx_BIT_XFRC             USB_DIEP_INT_XFERCOMPL
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DIEPINTx_BIT_INEPNE           USBAHB_DIEP0INT_INEPNAKEFF
#define  DIEPINTx_BIT_EPDISD           USBAHB_DIEP0INT_EPDISBLD
#define  DIEPINTx_BIT_XFRC             USBAHB_DIEP0INT_XFERCOMPL
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DIEPTSIZx_XFRSIZ_MSK          _USB_DIEP_TSIZ_XFERSIZE_MASK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DIEPTSIZx_XFRSIZ_MSK          0x7FFFFUL   // TODO Not the same mask on Sol : _USBAHB_DIEP0TSIZ_XFERSIZE_MASK = 0x7FUL
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DOEPTSIZx_STUPCNT_3_PKT       _USB_DOEP_TSIZ_RXDPIDSUPCNT_MASK
#define  DOEPTSIZx_XFRSIZ_MSK          _USB_DOEP_TSIZ_XFERSIZE_MASK
#define  DOEPTSIZx_PKTCNT_MSK          _USB_DOEP_TSIZ_PKTCNT_MASK
#define  DOEPTSIZx_SUPCNT_MSK          _USB_DOEP0TSIZ_SUPCNT_MASK
#define  DOEPTSIZx_SUPCNT_SHIFT        _USB_DOEP0TSIZ_SUPCNT_SHIFT
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DOEPTSIZx_STUPCNT_3_PKT       _USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_MASK
#define  DOEPTSIZx_XFRSIZ_MSK          _USBAHB_DEVOUTEP_TSIZ_XFERSIZE_MASK
#define  DOEPTSIZx_PKTCNT_MSK          _USBAHB_DEVOUTEP_TSIZ_PKTCNT_MASK
#define  DOEPTSIZx_SUPCNT_MSK          _USBAHB_DOEP0TSIZ_SUPCNT_MASK
#define  DOEPTSIZx_SUPCNT_SHIFT        _USBAHB_DOEP0TSIZ_SUPCNT_SHIFT
#endif
#define  DOEPTSIZ0_BIT_PKTCNT          0x00080000u
#define  DOEPTSIZ0_XFRSIZ_MAX_64       0x40u

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DOEPINTx_BIT_STUP             USB_DOEP_INT_SETUP
#define  DOEPINTx_BIT_XFRC             USB_DOEP_INT_XFERCOMPL
#define  DOEPINTx_BIT_STUPPKTRCVD      USB_DOEP_INT_STUPPKTRCVD
#define  DOEPINTx_BIT_BACK2BACKSETUP   USB_DOEP_INT_BACK2BACKSETUP
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DOEPINTx_BIT_STUP             USBAHB_DOEP0INT_SETUP
#define  DOEPINTx_BIT_XFRC             USBAHB_DOEP0INT_XFERCOMPL
#define  DOEPINTx_BIT_STUPPKTRCVD      USBAHB_DOEP0INT_STUPPKTRCVD
#define  DOEPINTx_BIT_BACK2BACKSETUP   USBAHB_DOEP0INT_BACK2BACKSETUP
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DIEPMSK_BIT_XFRCM             USB_DIEPMSK_XFERCOMPLMSK
#define  DIEPMSK_BIT_TIMEOUTMSK        USB_DIEPMSK_TIMEOUTMSK
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DIEPMSK_BIT_XFRCM             USBAHB_DIEPMSK_XFERCOMPLMSK
#define  DIEPMSK_BIT_TIMEOUTMSK        USBAHB_DIEPMSK_TIMEOUTMSK
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DAINTMSK_BIT_IEPINT_EP0       USB_DAINTMSK_INEPMSK0
#define  DAINTMSK_BIT_OEPINT_EP0       USB_DAINTMSK_OUTEPMSK0
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DAINTMSK_BIT_IEPINT_EP0       USBAHB_DAINTMSK_INEPMSK0
#define  DAINTMSK_BIT_OEPINT_EP0       USBAHB_DAINTMSK_OUTEPMSK0
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DSTS_ENUMSPD_FS_PHY_48MHZ     _USB_DSTS_ENUMSPD_MASK
#define  DSTS_BIT_SUSPSTS              USB_DSTS_SUSPSTS
#define  DSTS_FNSOF_MASK               _USB_DSTS_SOFFN_MASK
#define  DSTS_FNSOF_SHIFT              _USB_DSTS_SOFFN_SHIFT
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DSTS_ENUMSPD_FS_PHY_48MHZ     _USBAHB_DSTS_ENUMSPD_MASK
#define  DSTS_BIT_SUSPSTS              USBAHB_DSTS_SUSPSTS
#define  DSTS_FNSOF_MASK               _USBAHB_DSTS_SOFFN_MASK
#define  DSTS_FNSOF_SHIFT              _USBAHB_DSTS_SOFFN_SHIFT
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
#define  DCFG_DEVADDR_MASK             _USB_DCFG_DEVADDR_MASK
#define  DCFG_DEVADDR_SHIFT            _USB_DCFG_DEVADDR_SHIFT
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define  DCFG_DEVADDR_MASK             _USBAHB_DCFG_DEVADDR_MASK
#define  DCFG_DEVADDR_SHIFT            _USBAHB_DCFG_DEVADDR_SHIFT
#endif

/********************************************************************************************************
 *                           BIT/REG DEFINES FOR GIANT GECKO SERIES 1 ERRATA
 *******************************************************************************************************/

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
#define  USB_REG_DATTRIM1              USB->DATTRIM1
#define  DATTRIM1_BIT_ENDLYPULLUP      USB_DATTRIM1_ENDLYPULLUP
#endif

/********************************************************************************************************
 *                                         PHY SUSPEND MACROS
 *******************************************************************************************************/

#define  SLI_USBD_DRV_PHY_PCGCCTL_RESUME()              \
  SL_CLEAR_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_GATEHCLK); \
  SL_CLEAR_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_STOPPCLK)

#define  SLI_USBD_DRV_PHY_PCGCCTL_SUSPEND()           \
  SL_SET_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_GATEHCLK); \
  SL_SET_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_STOPPCLK)

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
#define  SLI_USBD_DRV_PHY_RESUME()                          \
  do {                                                      \
    SLI_USBD_DRV_PHY_PCGCCTL_RESUME();                      \
    SL_SET_BIT(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP); \
  } while (0)

#define  SLI_USBD_DRV_PHY_SUSPEND()                           \
  do {                                                        \
    SL_CLEAR_BIT(USB_REG_DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP); \
    SLI_USBD_DRV_PHY_PCGCCTL_SUSPEND();                       \
  } while (0)

#else
#define  SLI_USBD_DRV_PHY_RESUME()     \
  do {                                 \
    SLI_USBD_DRV_PHY_PCGCCTL_RESUME(); \
  } while (0)

#define  SLI_USBD_DRV_PHY_SUSPEND()     \
  do {                                  \
    SLI_USBD_DRV_PHY_PCGCCTL_SUSPEND(); \
  } while (0)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                             DRIVER DATA
 *******************************************************************************************************/

typedef struct {                                         // ------ DEVICE ENDPOINT DATA STRUCTURE ------
  uint16_t  EP_MaxPktSize[NBR_EPS_PHY_MAX];              // Max pkt size of opened EPs.
  uint16_t  EP_PktXferLen[NBR_EPS_PHY_MAX];              // EPs current xfer len.
  uint8_t  *EP_AppBufPtr[NBR_EPS_PHY_MAX];               // Ptr to endpoint app buffer.
  uint16_t  EP_AppBufLen[NBR_EPS_PHY_MAX];               // Lenght of endpoint app buffer.
  uint32_t  EP_SetupBuf[(3 * SETUP_PACKET_SIZE) / 4];    // Buffer that contains setup pkt.
  bool EnumDone;                                         // Indicates if EnumDone ISR occurred.
} sli_usbd_driver_endpoint_data_t;

sli_usbd_driver_endpoint_data_t usbd_driver_data = { 0 };

/********************************************************************************************************
 *                                             DEVICE CONFIGS
 *******************************************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_2)
//---------- USB DEVICE ENDPOINTS INFO TBL -----------
static sli_usbd_driver_endpoint_info_t usbd_endpoint_info_table[] = {
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL                                                   | SL_USBD_ENDPOINT_INFO_DIR_OUT, 0u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL                                                   | SL_USBD_ENDPOINT_INFO_DIR_IN, 0u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 1u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 1u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 2u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 2u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 3u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 3u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 4u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 4u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 5u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 5u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 6u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 6u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 7u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 7u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 8u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 8u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 9u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 9u, 64u },
  { 0x00u, 0u, 0u }
};
#else
//---------- USB DEVICE ENDPOINTS INFO TBL -----------
static sli_usbd_driver_endpoint_info_t usbd_endpoint_info_table[] = {
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL                                                                            | SL_USBD_ENDPOINT_INFO_DIR_OUT, 0u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL                                                                            | SL_USBD_ENDPOINT_INFO_DIR_IN, 0u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 1u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 1u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 2u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 2u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 3u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 3u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 4u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 4u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_OUT, 5u, 64u },
  { SL_USBD_ENDPOINT_INFO_TYPE_CTRL | SL_USBD_ENDPOINT_INFO_TYPE_ISOC | SL_USBD_ENDPOINT_INFO_TYPE_BULK | SL_USBD_ENDPOINT_INFO_TYPE_INTR | SL_USBD_ENDPOINT_INFO_DIR_IN, 5u, 64u },
  { 0x00u, 0u, 0u }
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void DWC_EP_OutProcess(void);

static void DWC_EP_InProcess(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
*  Initialize the Full-speed device
********************************************************************************************************/
sl_status_t sli_usbd_driver_init(void)
{
  uint8_t            ep_nbr;
  uint32_t           reg_to;
  volatile uint32_t  ctrl_reg;

#if defined(_SILICON_LABS_32B_SERIES_2)
  // Make sure the HCLK has at least a 30MHz frequency
  uint32_t hclk_freq = SystemHCLKGet();
  EFM_ASSERT(hclk_freq >= 30000000);

  CMU_ClockSelectSet(cmuClock_USB, cmuSelect_PLL0);      // Select PLL0 as clock input of USB IP.
  CMU_ClockEnable(cmuClock_USB, true);                   // Enable USB clock. It will also enable PLL0.

  // IO settings.
  // VBUS_SENSE allows to detect device connect/disconnect events
  GPIO_PinModeSet(SL_USBD_DRIVER_VBUS_SENSE_PORT, SL_USBD_DRIVER_VBUS_SENSE_PIN, gpioModeInput, 0);
  GPIO->USBROUTE.USBVBUSSENSEROUTE = (SL_USBD_DRIVER_VBUS_SENSE_PORT << _GPIO_USB_USBVBUSSENSEROUTE_PORT_SHIFT)
                                     | (SL_USBD_DRIVER_VBUS_SENSE_PIN << _GPIO_USB_USBVBUSSENSEROUTE_PIN_SHIFT);

  // Setup D+ and D- pins as input, with pull-up on D+ to indicate full speed.
  GPIO_PinModeSet(USB_DP_PORT, USB_DP_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(USB_DM_PORT, USB_DM_PIN, gpioModeInputPull, 0);

#else //defined(_SILICON_LABS_32B_SERIES_2)
#if defined(USBC_MEM_BASE)
  // GG
  CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC;
  CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_HFCLK);
#else
  // GG11
  {
    uint16_t timebase;

    // Configure high frequency used by USB
    CMU_USHFRCOBandSet(cmuUSHFRCOFreq_48M0Hz);
    CMU_ClockSelectSet(cmuClock_USBR, cmuSelect_USHFRCO);
    CMU_ClockEnable(cmuClock_USBR, true);

    // Configure low frequency used by the USB LEM feature
#if (BSP_LF_CLK_SEL == BSP_LF_CLK_LFXO)
    CMU_ClockSelectSet(cmuClock_USBLE, cmuSelect_LFXO);
#elif (BSP_LF_CLK_SEL == BSP_LF_CLK_ULFRCO)
    CMU_ClockSelectSet(cmuClock_USBLE, cmuSelect_ULFRCO);
#else
    CMU_ClockSelectSet(cmuClock_USBLE, cmuSelect_LFRCO);
#endif
    CMU_ClockEnable(cmuClock_USBLE, true);

    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_USB;
    // 3 ms period is necessary when enabling USB LEM
    timebase = CMU_ClockFreqGet(cmuClock_USBLE);
    timebase = ((3u * timebase) + 999u) / 1000u;

    // Enable the Low Energy Mode (LEM)
    USB_REG->LEMCTRL &= ~_USB_LEMCTRL_TIMEBASE_MASK;
    USB_REG->LEMCTRL |= timebase << _USB_LEMCTRL_TIMEBASE_SHIFT;

    USB_REG->CTRL = USB_CTRL_LEMOSCCTRL_GATE
                    | USB_CTRL_LEMIDLEEN
                    | USB_CTRL_LEMPHYCTRL;

    // Enable VBUS high detection interrupt.
    SL_SET_BIT(USB_REG->IEN, USB_IEN_VBUSDETH);
  }
#endif // defined(USBC_MEM_BASE)

  USB_REG->ROUTE = USB_ROUTE_VBUSENPEN | USB_ROUTE_PHYPEN;
#endif // defined(_SILICON_LABS_32B_SERIES_2)

  // Disable USB Interrupt
  NVIC_DisableIRQ(USB_IRQn);

#if defined(_SILICON_LABS_32B_SERIES_2)
  // Enable the USB IP prior to setting other registers.
  USB_APBS->EN_SET = USB_EN_EN;
  // Ensure PLL0 is stable.
  CMU_WaitUSBPLLLock();
#endif

  // Disable the global interrupt
  SL_CLEAR_BIT(USB_REG->GAHBCFG, GAHBCFG_BIT_GLBLINTRMSK);

  // -------------------- CORE RESET --------------------
  // Check AHB master state machine is in IDLE condition
  reg_to = REG_VAL_TO;
  while ((SL_IS_BIT_CLEAR(USB_REG->GRSTCTL, GRSTCTL_BIT_AHBIDLE)) && (reg_to > 0u)) {
    reg_to--;
  }

  // Resets the HCLK and PCLK domains
  SL_SET_BIT(USB_REG->GRSTCTL, GRSTCTL_BIT_CSFTRST);

  // Check all necessary logic is reset in the core
  reg_to = REG_VAL_TO;
  while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_CSFTRST)) && (reg_to > 0u)) {
    reg_to--;
  }

#if !defined(_SILICON_LABS_32B_SERIES_2)
  // Enable Sessions Request protocol capabilities.
  USB_REG->GUSBCFG |= USB_GUSBCFG_HNPCAP | USB_GUSBCFG_SRPCAP;
  // Force the core to device mode
  SL_SET_BIT(USB_REG->GUSBCFG, USB_GUSBCFG_FORCEDEVMODE);
  // Wait at least 25ms before the change takes effect
  reg_to = REG_FMOD_TO;
  while (reg_to > 0u) {
    reg_to--;
  }
#endif

  // ------------------- DEVICE INIT --------------------
  // Reset the PHY clock
  USB_REG->PCGCCTL = 0x00u;
  USB_REG->GOTGCTL = 0x00u;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  SL_SET_BIT(USB_REG->DATTRIM1, DATTRIM1_BIT_ENDLYPULLUP);
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  // 80% of the frame interval
  USB_REG->DCFG = USBAHB_DCFG_PERFRINT_DEFAULT  // tbc
                  // Default to full-speed device
                  | USBAHB_DCFG_DEVSPD_DEFAULT
                  // Enable PHY clock switched from 48 MHz to 32 KHz during suspend
                  | USBAHB_DCFG_ENA32KHZSUSP
                  // Set resume period to detect a valid resume event TODO CM see if 10 must be really set or just reset value of 2 is enough
                  | (10 << _USBAHB_DCFG_RESVALID_SHIFT);
#else
  // 80% of the frame interval
  USB_REG->DCFG = USB_DCFG_PERFRINT_80PCNT
                  // Default to full-speed device
                  | USB_DCFG_DEVSPD_FS;
#endif

  // Flush all transmit FIFOs
  USB_REG->GRSTCTL = GRSTCTL_BIT_TXFFLSH
                     | (16 << GRSTCTL_TXFNUM_SHIFT);

// Wait for the flush completion
  reg_to = REG_VAL_TO;
  while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_TXFFLSH)) && (reg_to > 0u)) {
    reg_to--;
  }

// Flush the receive FIFO
  SL_SET_BIT(USB_REG->GRSTCTL, GRSTCTL_BIT_RXFFLSH);

// Wait for the flush completion
  reg_to = REG_VAL_TO;
  while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_RXFFLSH)) && (reg_to > 0u)) {
    reg_to--;
  }

  USB_REG->DIEPMSK = 0x00u;    // Disable interrupts for the Device IN Endpoints
  USB_REG->DOEPMSK = 0x00u;    // Disable interrupts for the Device OUT Endpoints
  USB_REG->DAINTMSK = 0x00u;   // Disable interrupts foo all Device Endpoints

  USB_REG->DIEP0CTL |= DxEPCTLx_BIT_SNAK;
  USB_REG->DOEP0CTL |= DxEPCTLx_BIT_SNAK;
  for (ep_nbr = 0u; ep_nbr < (NBR_EPS_IN - 1); ep_nbr++) {
    // ----------------- IN ENDPOINT RESET ----------------
    ctrl_reg = USB_REG->DIEP_REG[ep_nbr].CTL;
    if (SL_IS_BIT_SET(ctrl_reg, DxEPCTLx_BIT_EPENA)) {
      ctrl_reg = 0x00u;
      ctrl_reg = DxEPCTLx_BIT_EPDIS | DxEPCTLx_BIT_SNAK;
    } else {
      ctrl_reg = 0x00u;
    }

    USB_REG->DIEP_REG[ep_nbr].CTL = ctrl_reg;
    USB_REG->DIEP_REG[ep_nbr].TSIZ = 0x00u;
    // Clear any pending Interrupt
    USB_REG->DIEP_REG[ep_nbr].INT = 0x000000FFu;

    // ---------------- OUT ENDPOINT RESET ----------------
    ctrl_reg = USB_REG->DOEP_REG[ep_nbr].CTL;
    if (SL_IS_BIT_SET(ctrl_reg, DxEPCTLx_BIT_EPENA)) {
      ctrl_reg = 0x00u;
      ctrl_reg = DxEPCTLx_BIT_EPDIS | DxEPCTLx_BIT_SNAK;
    } else {
      ctrl_reg = 0x00u;
    }

    USB_REG->DOEP_REG[ep_nbr].CTL = ctrl_reg;
    USB_REG->DOEP_REG[ep_nbr].TSIZ = 0x00u;
    // Clear any pending Interrupt
    USB_REG->DOEP_REG[ep_nbr].INT = 0x000000FFu;
  }

  // Disable all interrupts
  USB_REG->GINTMSK = 0x00u;

#if defined(_SILICON_LABS_32B_SERIES_2)
  USB_APBS->IEN_CLR = USB_IEN_DWCOTG | USB_IEN_VBUS;
#endif

  // Set Device Address to zero
  USB_REG->DCFG &= ~DCFG_DEVADDR_MASK;

  // Lock access to registers (see Note #2)
  USB_REG->PCGCCTL = PCGCCTL_BIT_PWRCLMP
                     | PCGCCTL_BIT_RSTPDWNMODULE;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Get the USB speed of the device driver
********************************************************************************************************/
sl_status_t sli_usbd_driver_get_speed(sl_usbd_device_speed_t *p_spd)
{
  if (p_spd == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *p_spd = SL_USBD_DEVICE_SPEED_FULL;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Get the EP info table structure for the device driver
********************************************************************************************************/
sl_status_t sli_usbd_driver_get_endpoint_info(sli_usbd_driver_endpoint_info_t **p_info_ptr)
{
  if (p_info_ptr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *p_info_ptr = usbd_endpoint_info_table;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Start device operation with VBUS detection disabled
********************************************************************************************************/
sl_status_t sli_usbd_driver_start(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // Add Power Manager EM1 requirement
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  usbd_driver_data.EnumDone = false;

  // Unlock registers access to write.
  SL_CLEAR_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_PWRCLMP);
  SL_CLEAR_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_RSTPDWNMODULE);
  SLI_USBD_DRV_PHY_RESUME();

  // Clear any pending interrupt
  USB_REG->GINTSTS = 0xFFFFFFFFu;

  // Enable interrupts
  USB_REG->GINTMSK = GINTMSK_BIT_USBSUSPM
                     | GINTMSK_BIT_USBRST
                     | GINTMSK_BIT_ENUMDNEM
                     | GINTMSK_BIT_WUIM
#if !defined(_SILICON_LABS_32B_SERIES_2)
                     | GINTMSK_BIT_SRQIM
                     | GINTMSK_BIT_OTGINT
#endif
                     | GINTMSK_BIT_RESETDET;

#if defined(_SILICON_LABS_32B_SERIES_2)
  USB_APBS->CTRL_CLR = USB_CTRL_BYPSVREG;   // Enable internal voltage regulator.
  USB_APBS->CTRL_SET = USB_CTRL_VBSSNSHEN;  // Enable VBUS sense high to detect connection.
  USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSLEN;  // Ensure VBUS sense low is disabled.
  USB_APBS->IEN_SET = USB_IEN_DWCOTG        // Enable main USB IP interrupt.
                      | USB_IEN_VBUS;       // Enable VBUS sense interrupt.
#endif

  // Enable DMA Mode
  SL_SET_BIT(USB_REG->GAHBCFG, GAHBCFG_BIT_DMAEN);

  // Enable Global Interrupt
  SL_SET_BIT(USB_REG->GAHBCFG, GAHBCFG_BIT_GLBLINTRMSK);

  NVIC_EnableIRQ(USB_IRQn);

  SLI_USBD_DRV_PHY_SUSPEND();

  // Generate Device connect event to the USB host
  SL_CLEAR_BIT(USB_REG->DCTL, DCTL_BIT_SDIS);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Stop Full-speed device operation
********************************************************************************************************/
sl_status_t sli_usbd_driver_stop(void)
{
  uint16_t          timeout;

  SLI_USBD_DRV_PHY_RESUME();

  NVIC_DisableIRQ(USB_IRQn);

  // Disable all interrupts and clear any pending interrupt.
  USB_REG->GINTMSK = 0x00u;
  USB_REG->GINTSTS = 0xFFFFFFFF;

  // Disable the global interrupt.
  SL_CLEAR_BIT(USB_REG->GAHBCFG, GAHBCFG_BIT_GLBLINTRMSK);

  // Generate Device Disconnect event to the USB host.
  SL_SET_BIT(USB_REG->DCTL, DCTL_BIT_SDIS);
  // Reset the USB core.
  SL_SET_BIT(USB_REG->GRSTCTL, GRSTCTL_BIT_CSFTRST);

  //FIXME: replace KAL_Dly
  // Disconnect signal for at least 2 us.
  //KAL_Dly(1u);

  timeout = 65535u;
  while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_CSFTRST))
         && (timeout > 0u)) {
    timeout--;
  }

  SLI_USBD_DRV_PHY_SUSPEND();
  SL_SET_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_PWRCLMP);
  SL_SET_BIT(USB_REG->PCGCCTL, PCGCCTL_BIT_RSTPDWNMODULE);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // Remove Power Manager EM1 requirement
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Assign an address to device
********************************************************************************************************/
sl_status_t sli_usbd_driver_set_address(uint8_t  dev_addr)
{
  USB_REG->DCFG &= ~DCFG_DEVADDR_MASK;
  USB_REG->DCFG |= (dev_addr << 4u);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Retrieve current frame number
********************************************************************************************************/
sl_status_t sli_usbd_driver_get_frame_number(uint16_t *p_frame_nbr)
{
  if (p_frame_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  *p_frame_nbr = (USB_REG->DSTS & DSTS_FNSOF_MASK) >> DSTS_FNSOF_SHIFT;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Open and configure a device endpoint, given its characteristics
********************************************************************************************************/
sl_status_t sli_usbd_driver_open_endpoint(uint8_t   ep_addr,
                                          uint8_t   ep_type,
                                          uint16_t  max_pkt_size,
                                          uint8_t   transaction_frame)
{
  uint8_t          ep_log_nbr;
  uint8_t          ep_phy_nbr;
  uint32_t          reg_val;

  (void)&transaction_frame;

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);
  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  reg_val = 0u;

  SLI_USBD_LOG_VRB(("USBD driver EP FIFO Open for endpoint addr: 0x", (X)ep_addr));

  switch (ep_type) {
    case SL_USBD_ENDPOINT_TYPE_CTRL:
      if (SL_USBD_ENDPOINT_IS_IN(ep_addr) == true) {
        // Enable Device EP0 IN interrupt
        SL_SET_BIT(USB_REG->DAINTMSK, DAINTMSK_BIT_IEPINT_EP0);
        // Enable common IN EP Transfer complete interrupt
        USB_REG->DIEPMSK |= (DIEPMSK_BIT_XFRCM);
      } else {
        // Enable EP0 OUT interrupt
        SL_SET_BIT(USB_REG->DAINTMSK, DAINTMSK_BIT_OEPINT_EP0);
        // Enable common OUT EP Setup and Xfer complete interrupt
        USB_REG->DOEPMSK |= (DOEPMSK_BIT_STUPM | DOEPMSK_BIT_XFRCM);

        SL_SET_BIT(USB_REG->DOEP0CTL, DxEPCTLx_BIT_SNAK);
      }

      break;

    case SL_USBD_ENDPOINT_TYPE_INTR:
    case SL_USBD_ENDPOINT_TYPE_BULK:
      if (SL_USBD_ENDPOINT_IS_IN(ep_addr)) {
        reg_val = (max_pkt_size                                 // cfg EP max packet size
                   | (ep_type << 18u)                           // cfg EP type
                   | (ep_log_nbr << 22u)                        // Tx FIFO number
                   | DxEPCTLx_BIT_SD0PID                        // EP start data toggle
                   | DxEPCTLx_BIT_USBAEP);                      // USB active EP

        USB_REG->DIEP_REG[ep_log_nbr - 1].CTL = reg_val;

        //Enable Device EPx IN Interrupt
        SL_SET_BIT(USB_REG->DAINTMSK, (1 << ep_log_nbr));
      } else {
        reg_val = (max_pkt_size                                 // cfg EP max packet size
                   | (ep_type << 18u)                           // cfg EP type
                   | DxEPCTLx_BIT_SD0PID                        // EP start data toggle
                   | DxEPCTLx_BIT_USBAEP);                      // USB active EP

        USB_REG->DOEP_REG[ep_log_nbr - 1].CTL = reg_val;

        // Enable Device EPx OUT Interrupt
        reg_val = (1 << ep_log_nbr) << 16u;
        SL_SET_BIT(USB_REG->DAINTMSK, reg_val);
      }
      break;

    case SL_USBD_ENDPOINT_TYPE_ISOC:
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  usbd_driver_data.EP_MaxPktSize[ep_phy_nbr] = max_pkt_size;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Close a device endpoint, and uninitialize/clear endpoint configuration in hardware
********************************************************************************************************/
sl_status_t sli_usbd_driver_close_endpoint(uint8_t ep_addr)
{
  uint8_t ep_log_nbr;
  uint32_t reg_val;
  __IOM uint32_t *diep_ctl_ptr;
  __IOM uint32_t *doep_ctl_ptr;

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);
  reg_val = 0x00u;

  diep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DIEP0CTL;
  doep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DOEP0CTL;

  SLI_USBD_LOG_VRB(("USBD driver EP Closed for endpoint addr: 0x", (X)ep_addr));

  // ------------------- IN ENDPOINTS -------------------
  if (SL_USBD_ENDPOINT_IS_IN(ep_addr)) {
    // Deactive the Endpoint
    SL_CLEAR_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_USBAEP);
    reg_val = 1 << ep_log_nbr;
  } else {
    // ------------------ OUT ENDPOINTS -------------------
    // Deactive the Endpoint
    SL_CLEAR_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_USBAEP);

    reg_val = (1 << ep_log_nbr) << 16u;
  }

  SL_CLEAR_BIT(USB_REG->DAINTMSK, reg_val);                        // Dis. EP interrupt

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Configure endpoint with buffer to receive data
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx_start(uint8_t   ep_addr,
                                              uint8_t   *p_buf,
                                              uint32_t  buf_len,
                                              uint32_t  *p_pkt_len)
{
  uint8_t          ep_log_nbr;
  uint8_t          ep_phy_nbr;
  uint16_t          ep_pkt_len;
  uint16_t          pkt_cnt;
  uint32_t          ctl_reg;
  uint32_t          tsiz_reg;
  __IOM uint32_t *doep_ctl_ptr;
  __IOM uint32_t *doep_tsiz_ptr;
  __IOM uint32_t *doep_dmaaddr_ptr;

  // Validate that the buffer is correctly 4 bytes aligned
  if (p_buf != NULL) {
    if (((uint32_t)p_buf % BUFFER_BYTE_ALIGNMENT) != 0u) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);
  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  ep_pkt_len = (uint16_t)SLI_USBD_GET_MIN(buf_len, usbd_driver_data.EP_MaxPktSize[ep_phy_nbr]);

  SLI_USBD_LOG_VRB(("USBD driver EP FIFO RxStart for endpoint addr: 0x", (X)ep_addr));

  // Get pointers to Control and Transfer EP registers
  doep_ctl_ptr = (ep_log_nbr != 0) ?  &USB_REG->DOEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DOEP0CTL;
  doep_tsiz_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].TSIZ : &USB_REG->DOEP0TSIZ;
  doep_dmaaddr_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].DMAADDR : &USB_REG->DOEP0DMAADDR;

  // Read Control and Transfer EP registers
  ctl_reg = *doep_ctl_ptr;
  tsiz_reg = *doep_tsiz_ptr;

  // Clear EP transfer size and packet count
  tsiz_reg &= ~(DOEPTSIZx_XFRSIZ_MSK | DOEPTSIZx_PKTCNT_MSK);

  if (buf_len == 0u) {
    tsiz_reg |= usbd_driver_data.EP_MaxPktSize[ep_phy_nbr];     // Set transfer size to max pkt size
    tsiz_reg |= (1u << 19u);                                    // Set packet count
  } else {
    pkt_cnt = (ep_pkt_len + (usbd_driver_data.EP_MaxPktSize[ep_phy_nbr] - 1u))
              / usbd_driver_data.EP_MaxPktSize[ep_phy_nbr];
    tsiz_reg |= (pkt_cnt << 19u);                               // Set packet count
                                                                // Set transfer size
    tsiz_reg |= pkt_cnt * usbd_driver_data.EP_MaxPktSize[ep_phy_nbr];
  }

  usbd_driver_data.EP_AppBufPtr[ep_phy_nbr] = p_buf;
  usbd_driver_data.EP_AppBufLen[ep_phy_nbr] = ep_pkt_len;

  // Set buffer address to receive data
  *doep_dmaaddr_ptr = (uint32_t)usbd_driver_data.EP_AppBufPtr[ep_phy_nbr];

  // Clear EP NAK and Enable EP for receiving
  ctl_reg |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;

  usbd_driver_data.EP_PktXferLen[ep_phy_nbr] = 0u;

  *doep_tsiz_ptr = tsiz_reg;
  *doep_ctl_ptr = ctl_reg;

  *p_pkt_len = ep_pkt_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Receive the specified amount of data from device endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx(uint8_t   ep_addr,
                                        uint8_t   *p_buf,
                                        uint32_t  buf_len,
                                        uint32_t  *p_xfer_len)
{
  uint8_t       ep_phy_nbr;
  uint16_t       xfer_len;

  (void)&p_buf;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  xfer_len = usbd_driver_data.EP_PktXferLen[ep_phy_nbr];

  SLI_USBD_LOG_VRB(("USBD driver EP FIFO Rx for endpoint addr: 0x", (X)ep_addr));

  *p_xfer_len = SLI_USBD_GET_MIN(xfer_len, buf_len);

  if (xfer_len > buf_len) {
    return SL_STATUS_RECEIVE;
  } else {
    return SL_STATUS_OK;
  }
}

/****************************************************************************************************//**
*  Receive zero-length packet from endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx_zlp(uint8_t ep_addr)
{
  (void)&ep_addr;

  SLI_USBD_LOG_VRB(("USBD driver EP RxZLP for endpoint addr: 0x", (X)ep_addr));

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Configure endpoint with buffer to transmit data
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx(uint8_t   ep_addr,
                                        uint8_t   *p_buf,
                                        uint32_t  buf_len,
                                        uint32_t  *p_xfer_len)
{
  uint8_t       ep_phy_nbr;
  uint16_t       ep_pkt_len;
  uint8_t          ep_log_nbr;
  __IOM uint32_t *diep_dmaaddr_ptr;

  // Validate that the buffer is correctly 4 bytes aligned
  if (((uint32_t)p_buf % BUFFER_BYTE_ALIGNMENT) != 0u) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);
  ep_pkt_len = (uint16_t)SLI_USBD_GET_MIN(usbd_driver_data.EP_MaxPktSize[ep_phy_nbr], buf_len);

  diep_dmaaddr_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].DMAADDR : &USB_REG->DIEP0DMAADDR;

  // Set buffer address of the data to transmit for DMA
  *diep_dmaaddr_ptr = (uint32_t)p_buf;

  *p_xfer_len = ep_pkt_len;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Transmit the specified amount of data to device endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx_start(uint8_t  ep_addr,
                                              uint8_t  *p_buf,
                                              uint32_t buf_len)
{
  uint8_t ep_log_nbr;
  uint32_t ctl_reg;
  uint32_t tsiz_reg;
  __IOM uint32_t *diep_ctl_ptr;
  __IOM uint32_t *diep_tsiz_ptr;

  (void)p_buf;

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);

  // Get pointers to Control and Transfer EP registers
  diep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DIEP0CTL;
  diep_tsiz_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].TSIZ : &USB_REG->DIEP0TSIZ;

  // Read EP control and Transfer registers.
  ctl_reg = *diep_ctl_ptr;
  tsiz_reg = *diep_tsiz_ptr;

  tsiz_reg &= ~DIEPTSIZx_XFRSIZ_MSK;   // Clear EP transfer size
  tsiz_reg |= buf_len;                 // Transfer size
  tsiz_reg |= (1u << 19u);             // Packet count

  // Clear EP NAK mode & Enable EP transmitting.
  ctl_reg |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;

  *diep_tsiz_ptr = tsiz_reg;
  *diep_ctl_ptr = ctl_reg;

  // Prepare for Setup Transaction after ZLP sent
  if ((buf_len == 0)
      && (ep_log_nbr == 0)) {
    USB_REG->DOEP0TSIZ = (DOEPTSIZx_STUPCNT_3_PKT
                          | DOEPTSIZ0_BIT_PKTCNT
                          | DOEPTSIZ0_XFRSIZ_MAX_64);
    USB_REG->DOEP0DMAADDR = (uint32_t)usbd_driver_data.EP_SetupBuf;
    USB_REG->DOEP0CTL |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;
  }

  SLI_USBD_LOG_VRB(("USBD driver EP addr 0x ", (X)ep_addr, " using FIFO has tx'd ", (u)buf_len, "bytes"));

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Transmit zero-length packet from endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx_zlp(uint8_t ep_addr)
{
  SLI_USBD_LOG_VRB(("USBD driver EP TxZLP on endpoint addr: 0x", (X)ep_addr));

  return sli_usbd_driver_endpoint_tx_start(ep_addr, (uint8_t *)0, 0u);
}

/****************************************************************************************************//**
*  Abort any pending transfer on endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_abort_endpoint(uint8_t ep_addr)
{
  uint8_t          ep_log_nbr;
  uint32_t          reg_to;
  __IOM uint32_t *diep_ctl_ptr;
  __IOM uint32_t *diep_int_ptr;
  __IOM uint32_t *doep_ctl_ptr;

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);

  diep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DIEP0CTL;
  diep_int_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].INT : &USB_REG->DIEP0INT;
  doep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DOEP0CTL;

  // ------------------- IN ENDPOINTS -------------------
  if (SL_USBD_ENDPOINT_IS_IN(ep_addr) == true) {
    // Set Endpoint to NAK mode
    SL_SET_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_SNAK);

    // Wait for IN EP NAK effective
    reg_to = REG_VAL_TO;
    while ((SL_IS_BIT_CLEAR(*diep_int_ptr, DIEPINTx_BIT_INEPNE) == true)
           && (reg_to > 0u)) {
      reg_to--;
    }

    SL_SET_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_EPDIS);

    // Wait for EP disable
    reg_to = REG_VAL_TO;
    while ((SL_IS_BIT_SET(*diep_ctl_ptr, DxEPCTLx_BIT_EPDIS) == true)
           && (reg_to > 0u)) {
      reg_to--;
    }
    // Clear EP Disable interrupt
    SL_SET_BIT(*diep_int_ptr, DIEPINTx_BIT_EPDISD);

    // Flush EPx TX FIFO
    USB_REG->GRSTCTL = GRSTCTL_BIT_TXFFLSH |  (ep_log_nbr << 6u);

    // Wait for the flush completion
    reg_to = REG_VAL_TO;
    while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_TXFFLSH) == true)
           && (reg_to > 0u)) {
      reg_to--;
    }
  } else {
    // ------------------ OUT ENDPOINTS -------------------
    // Set Endpoint to NAK mode
    SL_SET_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_SNAK);

    // Flush EPx RX FIFO
    SL_SET_BIT(USB_REG->GRSTCTL, GRSTCTL_BIT_RXFFLSH);

    // Wait for the flush completion
    reg_to = REG_VAL_TO;
    while ((SL_IS_BIT_SET(USB_REG->GRSTCTL, GRSTCTL_BIT_RXFFLSH) == true)
           && (reg_to > 0u)) {
      reg_to--;
    }
  }

  SLI_USBD_LOG_VRB(("USBD driver EP Abort for endpoint addr: 0x", (X)ep_addr));

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  Set or clear stall condition on endpoint
********************************************************************************************************/
sl_status_t sli_usbd_driver_stall_endpoint(uint8_t  ep_addr,
                                           bool     state)
{
  uint8_t ep_log_nbr;
  uint32_t ep_type;
  __IOM uint32_t *diep_ctl_ptr;
  __IOM uint32_t *doep_ctl_ptr;

  ep_log_nbr = SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr);

  diep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DIEP0CTL;
  doep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DOEP0CTL;

  SLI_USBD_LOG_VRB(("USBD driver EP Stall for endpoint addr:", (X)ep_addr));

  // ------------------- IN ENDPOINTS -------------------
  if (SL_USBD_ENDPOINT_IS_IN(ep_addr) == true) {
    if (state == true) {
      // Disable Endpoint if enable for transmit
      if (SL_IS_BIT_SET(*diep_ctl_ptr, DxEPCTLx_BIT_EPENA) == true) {
        SL_SET_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_EPDIS);
      }

      // Set Stall bit
      SL_SET_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_STALL);
    } else {
      ep_type = (*diep_ctl_ptr >> 18u) & 0x03u;
      // Clear Stall Bit
      SL_CLEAR_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_STALL);

      if ((ep_type == SL_USBD_ENDPOINT_TYPE_INTR)
          || (ep_type == SL_USBD_ENDPOINT_TYPE_BULK)) {
        // Set DATA0 PID
        SL_SET_BIT(*diep_ctl_ptr, DxEPCTLx_BIT_SD0PID);
      }
    }
  } else {
    // ------------------- OUT ENDPOINTS ------------------
    if (state == true) {
      // Set Stall bit
      SL_SET_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_STALL);
    } else {
      ep_type = (*doep_ctl_ptr >> 18u) & 0x03u;
      // Clear Stall Bit
      SL_CLEAR_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_STALL);

      if ((ep_type == SL_USBD_ENDPOINT_TYPE_INTR) || (ep_type == SL_USBD_ENDPOINT_TYPE_BULK)) {
        // Set DATA0 PID
        SL_SET_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_SD0PID);
      }
    }
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
*  USB device Interrupt Service Routine (ISR) handler
********************************************************************************************************/
sl_status_t sli_usbd_driver_irq_handler(void)
{
  uint32_t          int_stat;

  // Read global interrupt status register
  int_stat = USB_REG->GINTSTS;

#if defined(_SILICON_LABS_32B_SERIES_2)
  uint32_t int_status_wrapper = USB_APBS->IF;

  //------------------ VBUS INTERRUPT ------------------
  if (SL_IS_BIT_SET(int_status_wrapper, _USB_IF_VBUS_MASK) == true) {
    // Clear interrupt.
    USB_APBS->IF_CLR = USB_IF_VBUS;

    // Detect VBUS high event.
    if (SL_IS_BIT_SET(USB_APBS->STATUS, _USB_STATUS_VBUSVALID_MASK) == true) {
      SLI_USBD_DRV_PHY_RESUME();

      USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSHEN;  // Disable VBUS sense high.
      USB_APBS->CTRL_SET = USB_CTRL_VBSSNSLEN;  // Enable VBUS sense low to detect disconnection.

      // Notify connect event.
      sli_usbd_core_connect_event();
    } else {
      USB_APBS->CTRL_CLR = USB_CTRL_VBSSNSLEN;  // Disable VBUS sense low.
      USB_APBS->CTRL_SET = USB_CTRL_VBSSNSHEN;  // Enable VBUS sense high to detect disconnection.

      // Notify disconnect event.
      sli_usbd_core_disconnect_event();

      usbd_driver_data.EnumDone = false;
      SLI_USBD_DRV_PHY_SUSPEND();
    }
  }
#else
  //-------------- SESSION REQ DETECTION ---------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_SRQINT) == true) {
    SLI_USBD_DRV_PHY_RESUME();

    // Notify connect event.
    sli_usbd_core_connect_event();

    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_SRQINT);
  }

  //------------------ OTG INTERRUPT -------------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_OTGINT) == true) {
    uint32_t otgint_stat = USB_REG->GOTGINT;

    // Clear all OTG interrupt sources.
    USB_REG->GOTGINT |= (GOTGINT_BIT_SEDET
                         | GOTGINT_BIT_SRSSCHG
                         | GOTGINT_BIT_HNSSCHG
                         | GOTGINT_BIT_HNGDET
                         | GOTGINT_BIT_ADTOCHG
                         | GOTGINT_BIT_DBCDNE);

    if (SL_IS_BIT_SET(otgint_stat, GOTGINT_BIT_SEDET) == true) {
      // Notify disconnect event.
      sli_usbd_core_disconnect_event();

      usbd_driver_data.EnumDone = false;
      SLI_USBD_DRV_PHY_SUSPEND();
    }
  }
#endif

  //--------------- IN ENDPOINT INTERRUPT --------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_IEPINT) == true) {
    DWC_EP_InProcess();
  }

  //-------------- OUT ENDPOINT INTERRUPT --------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_OEPINT) == true) {
    DWC_EP_OutProcess();
  }

  //---------------- USB RESET DETECTION ---------------
  if ((SL_IS_BIT_SET(int_stat, GINTSTS_BIT_USBRST) == true)
      || (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_RESETDET) == true)) {
    SLI_USBD_DRV_PHY_RESUME();

    // Notify bus reset event.
    sli_usbd_core_reset_event();

    // Clear Remote wakeup signaling
    SL_CLEAR_BIT(USB_REG->DCTL, DCTL_BIT_RWUSIG);

    // Enable Global OUT/IN EP interrupt and RX FIFO non-empty interrupt.
    USB_REG->GINTMSK |= (GINTMSK_BIT_OEPINT
                         | GINTMSK_BIT_IEPINT);

    // Clear USB bus reset interrupt
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_USBRST);
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_RESETDET);

    // Set Device Address to 0
    USB_REG->DCFG &= ~DCFG_DEVADDR_MASK;

    // Set Rx FIFO depth
    USB_REG->GRXFSIZ = RXFIFO_SIZE;

    // Set EP0 to EPx Tx FIFO depth
    // Control endpoint.
    USB_REG->GNPTXFSIZ = (TXFIFO_EP0_SIZE << 16u) |  RXFIFO_SIZE;
    // Non-control endpoints.
    USB_REG->DIEPTXF1 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (1u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF2 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (2u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF3 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (3u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF4 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (4u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF5 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (5u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF6 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (6u * TXFIFO_EPx_SIZE));
#if defined(_SILICON_LABS_32B_SERIES_2)
    USB_REG->DIEPTXF7 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (7u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF8 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (8u * TXFIFO_EPx_SIZE));
    USB_REG->DIEPTXF9 = (TXFIFO_EPx_SIZE << 16u) | (RXFIFO_SIZE + (9u * TXFIFO_EPx_SIZE));
#endif

    usbd_driver_data.EnumDone = false;

    // Prepare to receive setup packets
    USB_REG->DOEP0TSIZ = (DOEPTSIZx_STUPCNT_3_PKT
                          | DOEPTSIZ0_BIT_PKTCNT
                          | DOEPTSIZ0_XFRSIZ_MAX_64);

    USB_REG->DOEP0DMAADDR = (uint32_t)usbd_driver_data.EP_SetupBuf;
    USB_REG->DOEP0CTL |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;
  }

  //------------ ENUMERATION DONE DETECTION ------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_ENUMDNE) == true) {
    if (SL_IS_BIT_SET(USB_REG->DSTS, DSTS_ENUMSPD_FS_PHY_48MHZ) == true) {
      USB_REG->DIEP0CTL &= ~DxEPCTL0_MPSIZ_64_MSK;
      USB_REG->GUSBCFG &= ~GUSBCFG_TRDT_MASK;
      USB_REG->GUSBCFG |= (5 << 10);               // turn around time
    }

    SL_SET_BIT(USB_REG->DCTL, DCTL_BIT_CGINAK);

    // Clear Enumeration done interrupt
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_ENUMDNE);

    usbd_driver_data.EnumDone = true;

    // Prepare to receive setup packets
    USB_REG->DOEP0TSIZ = (DOEPTSIZx_STUPCNT_3_PKT
                          | DOEPTSIZ0_BIT_PKTCNT
                          | DOEPTSIZ0_XFRSIZ_MAX_64);
    USB_REG->DOEP0DMAADDR = (uint32_t)usbd_driver_data.EP_SetupBuf;

    // Enable EP0 OUT
    USB_REG->DOEP0CTL |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;
  }

  //------------------ MODE MISMATCH -------------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_MMIS) == true) {
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_MMIS);
  }

  //------------- EARLY SUSPEND DETECTION --------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_ESUSP) == true) {
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_ESUSP);
  }

  //--------------- USB SUSPEND DETECTION --------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_USBSUSP) == true) {
    if (SL_IS_BIT_SET(USB_REG->DSTS, DSTS_BIT_SUSPSTS) == true) {
      // Notify suspend Event
      sli_usbd_core_suspend_event();
    }

    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_USBSUSP);

    if (usbd_driver_data.EnumDone) {
      SLI_USBD_DRV_PHY_SUSPEND();
    }
  }

  //----------------- WAKE-UP DETECTION ----------------
  if (SL_IS_BIT_SET(int_stat, GINTSTS_BIT_WKUPINT) == true) {
    SLI_USBD_DRV_PHY_RESUME();

    SL_CLEAR_BIT(USB_REG->DCTL, DCTL_BIT_RWUSIG);       // Clear Remote wakeup signaling
    SL_SET_BIT(USB_REG->GINTSTS, GINTSTS_BIT_WKUPINT);  // Clear Remote wakeup interrupt

    // Notify resume Event
    sli_usbd_core_resume_event();
  }

#if defined(_SILICON_LABS_32B_SERIES_2)
  if (SL_IS_BIT_SET(int_status_wrapper, _USB_IF_DWCOTG_MASK) == true) {
    // Clear main interrupt.
    USB_APBS->IF_CLR = USB_IF_DWCOTG;
  }
#endif

  return SL_STATUS_OK;
}

/********************************************************************************************************
 *                                           USB_IRQHandler()
 *
 * Description : Wrapper for the driver ISR Handler.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void USB_IRQHandler(void)
{
#if !defined(_SILICON_LABS_32B_SERIES_2) && !defined(USBC_MEM_BASE)
  if (SL_IS_BIT_SET(USB->IF, USB_IF_VBUSDETH)) {
    SL_SET_BIT(USB->IFC, USB_IFC_VBUSDETH);

    // Set VREGO to 3.3v.
    EMU->R5VOUTLEVEL = (10u << _EMU_R5VOUTLEVEL_OUTLEVEL_SHIFT);

    // Wait until VREGO is properly set.
    SL_SET_BIT(EMU->IEN, EMU_IEN_R5VVSINT);
    while (SL_IS_BIT_CLEAR(EMU->IF, EMU_IF_R5VVSINT)) ;
    SL_SET_BIT(EMU->IFC, EMU_IFC_R5VVSINT);
    SL_CLEAR_BIT(EMU->IEN, EMU_IEN_R5VVSINT);
  }
#endif

  // Call the USB Device driver ISR.
  sli_usbd_driver_irq_handler();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           DWC_EP_OutProcess()
 *
 * @brief       Process OUT interrupts on associated EP.
 *******************************************************************************************************/
static void DWC_EP_OutProcess(void)
{
  uint32_t  dev_ep_int;
  uint32_t  ep_log_nbr;
  uint8_t   ep_phy_nbr;
  uint32_t  ep_int_stat;
  __IOM uint32_t *doep_ctl_ptr;
  __IOM uint32_t *doep_int_ptr;
  __IOM uint32_t *doep_tsiz_ptr;

  // Read all Device OUT Endpoint interrupt
  dev_ep_int = USB_REG->DAINT >> 16u;

  while (dev_ep_int != 0x00u) {
    ep_log_nbr = (uint8_t)(31u - __CLZ(dev_ep_int & 0x0000FFFFu));
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SL_USBD_ENDPOINT_LOG_TO_ADDR_OUT(ep_log_nbr));

    doep_ctl_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].CTL : &USB_REG->DOEP0CTL;
    doep_int_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].INT : &USB_REG->DOEP0INT;
    doep_tsiz_ptr = (ep_log_nbr != 0) ? &USB_REG->DOEP_REG[ep_log_nbr - 1].TSIZ : &USB_REG->DOEP0TSIZ;

    ep_int_stat = *doep_int_ptr;

    // Handle Setup transaction
    if (SL_IS_BIT_SET(ep_int_stat, DOEPINTx_BIT_STUP)) {
      sli_usbd_core_setup_event((void *)&usbd_driver_data.EP_SetupBuf[0]);

      // Prepare for next setup transaction
      USB_REG->DOEP0TSIZ = (DOEPTSIZx_STUPCNT_3_PKT
                            | DOEPTSIZ0_BIT_PKTCNT
                            | DOEPTSIZ0_XFRSIZ_MAX_64);
      USB_REG->DOEP0DMAADDR = (uint32_t)usbd_driver_data.EP_SetupBuf;
      USB_REG->DOEP0CTL |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;
    }

    // Handle OUT transaction complete
    if (SL_IS_BIT_SET(ep_int_stat, DOEPINTx_BIT_XFRC)) {
      sl_usbd_core_endpoint_read_complete(ep_log_nbr);

      // Save size of data received
      uint32_t size_rem = *doep_tsiz_ptr & DOEPTSIZx_XFRSIZ_MSK;
      uint16_t byte_cnt = usbd_driver_data.EP_MaxPktSize[ep_phy_nbr] - size_rem;
      usbd_driver_data.EP_PktXferLen[ep_phy_nbr] += byte_cnt;

      if (ep_log_nbr == 0) {
        // Prepare for next setup transaction
        USB_REG->DOEP0TSIZ = (DOEPTSIZx_STUPCNT_3_PKT
                              | DOEPTSIZ0_BIT_PKTCNT
                              | DOEPTSIZ0_XFRSIZ_MAX_64);
        USB_REG->DOEP0DMAADDR = (uint32_t)usbd_driver_data.EP_SetupBuf;
        USB_REG->DOEP0CTL |= DxEPCTLx_BIT_CNAK | DxEPCTLx_BIT_EPENA;
      }
    }

    SL_SET_BIT(*doep_ctl_ptr, DxEPCTLx_BIT_SNAK);

    // Clear interrupt flags
    *doep_int_ptr = ep_int_stat;

    // Read all Device OUT Endpoint interrupt
    dev_ep_int = USB_REG->DAINT >> 16u;
  }
}

/********************************************************************************************************
 *                                           DWC_EP_InProcess()
 *
 * @brief       Process IN interrupts on associated EP.
 *******************************************************************************************************/
static void DWC_EP_InProcess(void)
{
  uint8_t          ep_log_nbr;
  uint32_t          ep_int_stat;
  uint32_t          dev_ep_int;
  __IOM uint32_t *diep_int_ptr;

  // Read all Device IN Endpoint interrupt
  dev_ep_int = (USB_REG->DAINT & 0x0000FFFFu);

  while (dev_ep_int != 0x00u) {
    ep_log_nbr = (uint8_t)(31u - __CLZ(dev_ep_int & 0x0000FFFFu));

    diep_int_ptr = (ep_log_nbr != 0) ? &USB_REG->DIEP_REG[ep_log_nbr - 1].INT : &USB_REG->DIEP0INT;

    // Read IN EP interrupt status
    ep_int_stat = *diep_int_ptr;

    // Check if EP transfer completed interrupt occurred
    if (SL_IS_BIT_SET(ep_int_stat, DIEPINTx_BIT_XFRC)) {
      sl_usbd_core_endpoint_write_complete(ep_log_nbr);
    }

    // Clear interrupt status for IN EP
    *diep_int_ptr = ep_int_stat;

    // Read all Device IN Endpoint interrupt
    dev_ep_int = (USB_REG->DAINT & 0x0000FFFFu);
  }
}
