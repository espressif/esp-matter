/***************************************************************************//**
 * @file
 * @brief USB-Host Pbhcd BSP
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    BSP_USB_MODULE

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>
#include  <kernel/include/os.h>
#include  <rtos_description.h>

#if  defined(RTOS_MODULE_USB_HOST_AVAIL) \
  && defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL)

#include  <usb/include/host/usbh_core.h>
#include  <drivers/usb/include/usbh_pbhcd.h>

#include  "em_cmu.h"
#include  "em_gpio.h"
#include  "em_device.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ USB HOST FNCTS ------------------
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Init(USBH_PBHCD_ISR_HANDLE_FNCT isr_fnct,
                                                  USBH_PBHCI_HC_DRV          *p_pbhci_hc_drv);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_ClkCfg(void);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_IO_Cfg(void);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_IntCfg(void);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_PwrCfg(void);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Start(void);

static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Stop(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_PBHCD_ISR_HANDLE_FNCT BSP_USBH_EFM32GG_ISR_Fnct;    // Function to call when int occurs.
static USBH_PBHCI_HC_DRV          *BSP_USBH_EFM32GG_DrvPtr;     // Pointer to data to pass to ISR.

//                                                                 ------------------ BSP API STRUCT ------------------
static USBH_PBHCI_BSP_API BSP_USBH_EFM32GG_STK3700A_BSP_API_Ptr = {
  BSP_USBH_EFM32GG_STK3700A_Init,
  BSP_USBH_EFM32GG_STK3700A_ClkCfg,
  BSP_USBH_EFM32GG_STK3700A_IO_Cfg,
  BSP_USBH_EFM32GG_STK3700A_IntCfg,
  BSP_USBH_EFM32GG_STK3700A_PwrCfg,
  BSP_USBH_EFM32GG_STK3700A_Start,
  BSP_USBH_EFM32GG_STK3700A_Stop,
  DEF_NULL
};

//                                                                 ----------------- PIPE INFO TABLE ------------------
USBH_PBHCI_PIPE_INFO BSP_USBH_EFM32GG_PipeInfoTbl[] =
{
  { USBH_PIPE_INFO_TYPE_CTRL                                                       | USBH_PIPE_INFO_DIR_BOTH, 0u, 64u, 0u },
  { USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 1u, 512u, 0u },
  { USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 2u, 512u, 0u },
  { USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 3u, 512u, 0u },
  { USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 4u, 512u, 0u },
  { USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 5u, 512u, 0u },
  { DEF_BIT_NONE, 0u, 0u, 0u }
};

//                                                                 ------------------ HW INFO STRUCT ------------------
const USBH_PBHCI_HC_HW_INFO BSP_USBH_EFM32_PBHCI_HW_Info = {
  //                                                               USB Controller register base address.
#if defined(USBC_MEM_BASE)
  .HW_Info.BaseAddr = USBC_MEM_BASE,
#else
  .HW_Info.BaseAddr = USB_MEM_BASE,
#endif
  .HW_Info.RH_Spd = USBH_DEV_SPD_FULL,                          // Maximum speed of USB controller.
  .HW_Info.HC_Type = USBH_HC_TYPE_PIPE,
  .HW_Info.HW_DescDedicatedMemInfoPtr = DEF_NULL,               // Insert info on dedicated memory if any.
  .HW_Info.DataBufDedicatedMemInfoPtr = DEF_NULL,

  .DrvAPI_Ptr = &USBH_PBHCD_API_EFM32_OTG_FS_FIFO,              // Ptr to appropriate driver API structure.
  .BSP_API_Ptr = &BSP_USBH_EFM32GG_STK3700A_BSP_API_Ptr,
  .PipeInfoTblPtr = BSP_USBH_EFM32GG_PipeInfoTbl
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   BSP_USBH_EFM32GG_STK3700A_Init()
 *
 * Description : Initializes USB host BSP.
 *
 * Argument(s) : isr_fnct    Interrupt service rooutine to call when an interruption occurs.
 *
 *               p_hc_drv    Pointer to host controller driver information to pass to isr_fnct.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Init(USBH_PBHCD_ISR_HANDLE_FNCT isr_fnct,
                                                  USBH_PBHCI_HC_DRV          *p_hc_drv)
{
  BSP_USBH_EFM32GG_ISR_Fnct = isr_fnct;
  BSP_USBH_EFM32GG_DrvPtr = p_hc_drv;

  NVIC_DisableIRQ(USB_IRQn);

  return (DEF_OK);
}

/********************************************************************************************************
 *                                  BSP_USBH_EFM32GG_STK3700A_ClkCfg()
 *
 * Description : Initializes USB host controller's clock.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_ClkCfg(void)
{
#if defined(USBC_MEM_BASE)
  //                                                               GG
  if (CMU_ClockSelectGet(cmuClock_HF) != cmuSelect_HFXO) {
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  }
  //                                                               Enable USB clock
  CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC;

  CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_HFCLK);

  USB->ROUTE = USB_ROUTE_VBUSENPEN | USB_ROUTE_PHYPEN;
#else
  //                                                               GG11
  CMU_ClockSelectSet(cmuClock_USBR, cmuSelect_HFRCO);
  CMU_ClockEnable(cmuClock_USBR, true);

  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_USB;

  USB->CTRL |= USB_CTRL_SELFPOWERED_HIGH;

  //                                                               Do not use low power mode - for now
  //                                                               USB->CTRL = USB_CTRL_LEMOSCCTRL_GATE | USB_CTRL_LEMIDLEEN | USB_CTRL_LEMPHYCTRL;

  //                                                               Set selfpowered
  USB->CTRL |= USB_CTRL_SELFPOWERED_HIGH;
  USB->ROUTE = USB_ROUTE_PHYPEN;
  USB->CTRL |= USB_CTRL_VBUSENAP_HIGH;
  USB->ROUTE |= USB_ROUTE_VBUSENPEN;

#endif

  return (DEF_OK);
}

/********************************************************************************************************
 *                                  BSP_USBH_EFM32GG_STK3700A_IO_Cfg()
 *
 * Description : Initializes USB host controller's IOs.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_IO_Cfg(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
#if defined(USBC_MEM_BASE)
  //                                                               GG
  GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0);           // Enable VBUSEN pin
#else
  //                                                               GG11
  GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 1);           // Enable VBUSEN pin
#endif

  return (DEF_OK);
}

/********************************************************************************************************
 *                                  BSP_USBH_EFM32GG_STK3700A_IntCfg()
 *
 * Description : Initializes USB host controller's interrupts.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_IntCfg(void)
{
  NVIC_DisableIRQ(USB_IRQn);

  return (DEF_OK);
}

/********************************************************************************************************
 *                                  BSP_USBH_EFM32GG_STK3700A_PwrCfg()
 *
 * Description : Initializes USB host controller's power.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_PwrCfg(void)
{
  return (DEF_OK);
}

/********************************************************************************************************
 *                                   BSP_USBH_EFM32GG_STK3700A_Start()
 *
 * Description : Starts USB host BSP.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Start(void)
{
  NVIC_EnableIRQ(USB_IRQn);

  return (DEF_OK);
}

/********************************************************************************************************
 *                                   BSP_USBH_EFM32GG_STK3700A_Stop()
 *
 * Description : Stops USB host BSP.
 *
 * Argument(s) : None.
 *
 * Return(s)   : DEF_OK,   if successful.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_USBH_EFM32GG_STK3700A_Stop(void)
{
  NVIC_DisableIRQ(USB_IRQn);

  return (DEF_OK);
}

/********************************************************************************************************
 *                                           USB_IRQHandler()
 *
 * Description : USB host controller interrupt service routine.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void USB_IRQHandler(void)
{
  OSIntEnter();
  BSP_USBH_EFM32GG_ISR_Fnct(BSP_USBH_EFM32GG_DrvPtr);
  OSIntExit();
}

#endif
