/***************************************************************************//**
 * @file
 * @brief OS BSP
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 * Note(s) : (1) The kal_priv.h file is included only to be used by this file. Its content should remain
 *               private to rtos and not be used in an application's context.
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../include/bsp_os.h"

#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/source/kal/kal_priv.h>                        // Private file, use should be limited. See Note #1.
#include  <rtos_description.h>

#ifdef  LEGACY_CONFIG_MODE                                      // Backwards compatibility with legacy configuration files.
#include  <bspconfig.h>
#endif

#ifdef  RTOS_MODULE_KERNEL_AVAIL
#include  <kernel/include/os.h>
#elif  defined(RTOS_MODULE_KERNEL_OS2_AVAIL)
#include  <os.h>
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
#include  <usb/include/usb_ctrlr.h>
#endif
#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
#include  <net/include/net_if_ether.h>
#endif
#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
#include  <can/include/can_bus.h>
#endif
#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)              // Only GG11 has been tested with QSPI.
#include  <fs/include/fs_nor_quad_spi.h>
#elif defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
#include  <drivers/fs/include/fs_nor_spidrv.h>
#endif
#endif
#if defined(RTOS_MODULE_IO_SD_AVAIL)
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#endif
#if defined(RTOS_MODULE_ENTROPY_AVAIL)
#include  <entropy/include/entropy.h>
#endif

//                                                                 Third Party Library Includes
#include  <em_chip.h>
#include  <em_cmu.h>
#include  <em_emu.h>

#include  "../include/bsp_opt_def.h"
#include  <bsp_cfg.h>
#include  <sl_sleeptimer_config.h>
#include  "sl_device_init_dcdc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(CMU_HFXOINIT_STK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_STK_DEFAULT               // Use STK default HFXO configuration
#elif defined(CMU_HFXOINIT_WSTK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_WSTK_DEFAULT              // Use Wireless STK default HFXO configuration
#else
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_DEFAULT                   // Use standard emlib HFXO configuration
#endif

#if defined(RTOS_MODULE_USB_DEV_AVAIL)
BSP_HW_INFO_EXT(const USBD_DEV_CTRLR_HW_INFO, BSP_USBD_EFM32_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL) && defined (RTOS_MODULE_USB_HOST_PBHCI_AVAIL)
BSP_HW_INFO_EXT(const USBH_PBHCI_HC_HW_INFO, BSP_USBH_EFM32_PBHCI_HW_Info);
#endif

#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
BSP_HW_INFO_EXT(const NET_IF_ETHER_HW_INFO, BSP_NetEther_HwInfo);
#endif

#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
BSP_HW_INFO_EXT(const CAN_CTRLR_DRV_INFO, BSP_CAN0_BSP_DrvInfo);
BSP_HW_INFO_EXT(const CAN_CTRLR_DRV_INFO, BSP_CAN1_BSP_DrvInfo);
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
BSP_HW_INFO_EXT(const SD_CARD_CTRLR_DRV_INFO, BSP_SD_SDHC_BSP_DrvInfo);
#endif

#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)              // Only GG11 has been tested with QSPI.
BSP_HW_INFO_EXT(const FS_NOR_QUAD_SPI_CTRLR_INFO, BSP_FS_NOR_MX25R_QuadSPI_HwInfo);
#elif defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
BSP_HW_INFO_EXT(const FS_NOR_SPIDRV_CTRLR_INFO, BSP_FS_NOR_MX25R_SPI_HwInfo);
#endif
#endif

#if defined(RTOS_MODULE_ENTROPY_AVAIL)
BSP_HW_INFO_EXT(const ENTROPY_RNG_DRV_INFO, BSP_RNG0_DrvInfo);
#endif

// Resolve Sleeptimer peripheral in auto mode
#if SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_DEFAULT
#if defined(RTCC_PRESENT) && RTCC_COUNT >= 1
#undef SL_SLEEPTIMER_PERIPHERAL
#define SL_SLEEPTIMER_PERIPHERAL SL_SLEEPTIMER_PERIPHERAL_RTCC
#elif defined(RTC_PRESENT) && RTC_COUNT >= 1
#undef SL_SLEEPTIMER_PERIPHERAL
#define SL_SLEEPTIMER_PERIPHERAL SL_SLEEPTIMER_PERIPHERAL_RTC
#elif defined(SYSRTC_PRESENT) && SYSRTC_COUNT >= 1
#undef SL_SLEEPTIMER_PERIPHERAL
#define SL_SLEEPTIMER_PERIPHERAL SL_SLEEPTIMER_PERIPHERAL_SYSRTC
#elif defined(BURTC_PRESENT) && BURTC_COUNT >= 1
#undef SL_SLEEPTIMER_PERIPHERAL
#define SL_SLEEPTIMER_PERIPHERAL SL_SLEEPTIMER_PERIPHERAL_BURTC
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           BSP_SystemInit()
 *
 * Description : Initialize the Board Support Package (BSP).
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) This function MUST be called before any other BSP function is called.
 *******************************************************************************************************/
void BSP_SystemInit(void)
{
  CMU_Clock_TypeDef  sleeptimer_clock;
  CMU_Select_TypeDef sleeptimer_select;
  CMU_Osc_TypeDef    sleeptimer_osc;
#if (BSP_HF_CLK_SEL == BSP_HF_CLK_HFXO) || (BSP_HF_CLK_SEL == BSP_HF_CLK_DPLL_48MHZ)
  CMU_HFXOInit_TypeDef hfxo_init = BSP_HFXO_CONFIG;
#endif
#if (BSP_LF_CLK_SEL == BSP_LF_CLK_LFXO)
  CMU_LFXOInit_TypeDef lfxo_init = CMU_LFXOINIT_DEFAULT;
#endif

#if !defined(_SILICON_LABS_32B_SERIES_2)
  CMU_Clock_TypeDef system_clock = cmuClock_HF;
#else
  CMU_Clock_TypeDef system_clock = cmuClock_SYSCLK;
#endif

  CHIP_Init();                                                  // Chip revision alignment and errata fixes
  sl_device_init_dcdc();

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_DEFAULT)
#error "Missing definition of BSP_HF_CLK_SEL in bsp_cfg.h"

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFRCO)
#if !defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockSelectSet(system_clock, cmuSelect_HFRCO);            // Select reference clock for High Freq. clock
#else
  CMU_ClockSelectSet(system_clock, cmuSelect_HFRCODPLL);        // Select reference clock for High Freq. clock
#endif

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFXO)
  CMU_HFXOInit(&hfxo_init);                                      // Initialize HFXO with board specific parameters
  CMU_ClockSelectSet(system_clock, cmuSelect_HFXO);             // Select reference clock for High Freq. clock

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_DPLL_48MHZ)
  CMU_HFXOInit(&hfxo_init);                                      // Initialize HFXO with board specific parameters
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

#if !defined(_SILICON_LABS_32B_SERIES_2)
#define DPLL_N 479
#define DPLL_M 499

  //                                                               Use HFRCO with DPLL to generate 48 MHz
  //                                                               The CMU_DPLLLock() could probably be used.
  CMU_HFRCOBandSet(cmuHFRCOFreq_48M0Hz);
  CMU->HFRCOCTRL |= CMU_HFRCOCTRL_FINETUNINGEN;
  //                                                               Setup DPLL frequency. The formula is Fdco = Fref*(N+1)/(M+1)
  CMU->DPLLCTRL1 = (DPLL_N << _CMU_DPLLCTRL1_N_SHIFT) | (DPLL_M << _CMU_DPLLCTRL1_M_SHIFT);
  //                                                               Enable DPLL
  CMU->OSCENCMD = CMU_OSCENCMD_DPLLEN;
  //                                                               Set HFClock from HFRCO
  CMU_ClockSelectSet(system_clock, cmuSelect_HFRCO);            // Select reference clock for High Freq. clock
#else
  //                                                               Use HFRCO with DPLL to generate 48 MHz
  CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_48M0Hz);
  CMU_ClockSelectSet(system_clock, cmuSelect_HFRCODPLL);        // Select reference clock for High Freq. clock
#endif

#else
#error "The definition of BSP_HF_CLK_SEL in bsp_cfg.h is not handled."
#endif

#if !defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_HFLE, true);
#endif

#if (BSP_LF_CLK_SEL == BSP_LF_CLK_LFXO)
  CMU_LFXOInit(&lfxo_init);

  sleeptimer_osc = cmuOsc_LFXO;
  sleeptimer_select = cmuSelect_LFXO;
#elif (BSP_LF_CLK_SEL == BSP_LF_CLK_LFRCO)
  sleeptimer_osc = cmuOsc_LFRCO;
  sleeptimer_select = cmuSelect_LFRCO;
#else
  #error "The definition of BSP_LF_CLK_SEL in bsp_cfg.h is not handled."
#endif

#if ((SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_RTCC)   \
  || (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_RTC)    \
  || (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_SYSRTC) \
  || (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_BURTC))

#if (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_RTCC)
  #if defined(_SILICON_LABS_32B_SERIES_2)
  sleeptimer_clock = cmuClock_RTCC;
  #else
  sleeptimer_clock = cmuClock_LFE;
  #endif
#elif (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_RTC)
  sleeptimer_clock = cmuClock_LFA;
#elif (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_SYSRTC)
  sleeptimer_clock = cmuClock_SYSRTC;
#elif (SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_BURTC)
  sleeptimer_clock = cmuClock_BURTC;
#endif

  CMU_ClockSelectSet(sleeptimer_clock, sleeptimer_select);
#elif (SL_SLEEPTIMER_PERIPHERAL != SL_SLEEPTIMER_PERIPHERAL_PRORTC)
  #error "Invalid peripheral selected for SL_SLEEPTIMER in sl_sleeptimer_config.h"
#endif

  CMU_OscillatorEnable(sleeptimer_osc, true, false);
}

/********************************************************************************************************
 *                                              BSP_Init()
 *
 * Description : Initializes peripheral such as USB or Ethernet.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) This function must be called after KAL has been initialized.
 *******************************************************************************************************/
void BSP_OS_Init(void)
{
#if defined(RTOS_MODULE_USB_DEV_AVAIL) || (defined(RTOS_MODULE_USB_HOST_AVAIL) && defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL))
  USB_CTRLR_HW_INFO_REG("usb0", &BSP_USBD_EFM32_HwInfo, &BSP_USBH_EFM32_PBHCI_HW_Info);
#endif
  //                                                               ---------- REGISTER ETHERNET CONTROLLERS -----------
#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
  NET_CTRLR_ETHER_REG("eth0", &BSP_NetEther_HwInfo);
#endif
  //                                                               ------------- REGISTER CAN CONTROLLERS -------------
#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
  CAN_CTRLR_REG("can0", &BSP_CAN0_BSP_DrvInfo);
  CAN_CTRLR_REG("can1", &BSP_CAN1_BSP_DrvInfo);
#endif
  //                                                               ----------- REGISTER SD I/O CONTROLLERS ------------
#if defined(RTOS_MODULE_IO_SD_AVAIL)
  IO_SD_CARD_CTRLR_REG("sd0", &BSP_SD_SDHC_BSP_DrvInfo);
#endif
  //                                                               ----------- REGISTER MEMORY CONTROLLERS ------------
#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
  #if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  FS_NOR_QUAD_SPI_HW_INFO_REG("nor0", &BSP_FS_NOR_MX25R_QuadSPI_HwInfo);
  #elif defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
  FS_NOR_SPIDRV_HW_INFO_REG("nor0", &BSP_FS_NOR_MX25R_SPI_HwInfo);
  #endif
#endif
  //                                                               ------------- REGISTER ENTROPY SOURCES -------------
#if defined(RTOS_MODULE_ENTROPY_AVAIL)
  ENTROPY_RNG_REG("rng0", &BSP_RNG0_DrvInfo);
#endif
}
