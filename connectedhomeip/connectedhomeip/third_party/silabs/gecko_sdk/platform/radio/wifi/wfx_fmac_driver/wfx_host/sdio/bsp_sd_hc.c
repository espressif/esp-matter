/***************************************************************************//**
 * @file bsp_sd_hc.c
 * @brief IO-Sd Sdhc BSP - Silicon Labs - Stk3701
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if  defined(RTOS_MODULE_IO_SD_AVAIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#include  <drivers/io/include/sd_drv.h>
#include  <kernel/include/os.h>
#include  <em_cmu.h>
#include  <em_gpio.h>
#include  "em_device.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                RTOS_CFG_MODULE_BSP

/********************************************************************************************************
 *                                             SD ROUTE
 *******************************************************************************************************/

#define  BSP_SD_ROUTE_EXP_HDR           0u                      // Route #0: SD route through 20-pin EXP header.
#define  BSP_SD_ROUTE_SD_CARD_SLOT      1u                      // Route #1: SD route through SD card slot.

//                                                                 TODO change this define to select SD route.
#ifndef  BSP_SD_ROUTE_SEL
#define  BSP_SD_ROUTE_SEL               BSP_SD_ROUTE_SD_CARD_SLOT
#warning "Select one of the connections by defining the symbol BSP_SD_ROUTE_SEL to either 0 (Card Slot) or 1 (EXP Header)."
#endif

#if (BSP_SD_ROUTE_SEL == BSP_SD_ROUTE_EXP_HDR)
#define  BSP_SDIO_CD_PIN_NBR            4u
#define  BSP_SDIO_WP_PIN_NBR            5u
#define  BSP_SDIO_CMD_PIN_NBR           12u
#define  BSP_SDIO_CLK_PIN_NBR           13u
#define  BSP_SDIO_DAT0_PIN_NBR          11u
#define  BSP_SDIO_DAT1_PIN_NBR          10u
#define  BSP_SDIO_DAT2_PIN_NBR          9u
#define  BSP_SDIO_DAT3_PIN_NBR          8u
#else
#define  BSP_SDIO_CD_PIN_NBR            10u
#define  BSP_SDIO_WP_PIN_NBR            9u
#define  BSP_SDIO_CMD_PIN_NBR           15u
#define  BSP_SDIO_CLK_PIN_NBR           14u
#define  BSP_SDIO_DAT0_PIN_NBR          0u
#define  BSP_SDIO_DAT1_PIN_NBR          1u
#define  BSP_SDIO_DAT2_PIN_NBR          2u
#define  BSP_SDIO_DAT3_PIN_NBR          3u
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SD_CARD_DRV                   *BSP_SD_SDHC_DrvPtr;
static SD_CARD_CTRLR_ISR_HANDLE_FNCT BSP_SD_SDHC_ISR_Fnct;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN BSP_SD_SDHC_Init(SD_CARD_CTRLR_ISR_HANDLE_FNCT isr_fnct,
                                    SD_CARD_DRV                   *p_sd_card_drv);

static CPU_BOOLEAN BSP_SD_SDHC_ClkEn(void);

static CPU_BOOLEAN BSP_SD_SDHC_IO_Cfg(void);

static CPU_BOOLEAN BSP_SD_SDHC_IntCfg(void);

static CPU_BOOLEAN BSP_SD_SDHC_PwrCfg(void);

static CPU_BOOLEAN BSP_SD_SDHC_Start(void);

static CPU_BOOLEAN BSP_SD_SDHC_Stop(void);

static CPU_INT32U BSP_SD_SDHC_ClkFreqGet(void);

static void BSP_SD_SDHC_CapabilitiesGet(SD_HOST_CAPABILITIES *p_capabilities);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- BSP API STRUCTURE -----------------
static const SD_CARD_CTRLR_BSP_API BSP_SD_SDHC_BSP_API = {
  .Init = BSP_SD_SDHC_Init,
  .ClkEn = BSP_SD_SDHC_ClkEn,
  .IO_Cfg = BSP_SD_SDHC_IO_Cfg,
  .IntCfg = BSP_SD_SDHC_IntCfg,
  .PwrCfg = BSP_SD_SDHC_PwrCfg,
  .Start = BSP_SD_SDHC_Start,
  .Stop = BSP_SD_SDHC_Stop,
  .ClkFreqGet = BSP_SD_SDHC_ClkFreqGet,
  .SignalVoltSet = DEF_NULL,
  .CapabilitiesGet = BSP_SD_SDHC_CapabilitiesGet,
  .BSP_API_ExtPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- DRIVER INFORMATION ----------------
const SD_CARD_CTRLR_DRV_INFO BSP_SD_SDHC_BSP_DrvInfo = {
  .BSP_API_Ptr = &BSP_SD_SDHC_BSP_API,
  .DrvAPI_Ptr = &SDHC_Drv_API_Arasan,
  .HW_Info.BaseAddr = 0x400F1000u,
  .HW_Info.CardSignalVolt = SD_CARD_BUS_SIGNAL_VOLT_AUTO,
#if (BSP_SD_ROUTE_SEL == BSP_SD_ROUTE_EXP_HDR)                  // For 20-pin EXP Header SD route.
  .HW_Info.CardDetectMode = SD_CARD_DETECT_MODE_WIRED,
#else
  .HW_Info.CardDetectMode = SD_CARD_DETECT_MODE_INTERRUPT,
#endif
  .HW_Info.InfoExtPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION DEFINITIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          BSP_SD_SDHC_Init()
 *
 * @brief    Initializes BSP.
 *
 * @param    isr_fnct        ISR handler function.
 *
 * @param    p_sd_card_drv   Pointer to sd card driver data.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_Init(SD_CARD_CTRLR_ISR_HANDLE_FNCT isr_fnct,
                                    SD_CARD_DRV                   *p_sd_card_drv)
{
  BSP_SD_SDHC_ISR_Fnct = isr_fnct;
  BSP_SD_SDHC_DrvPtr = p_sd_card_drv;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                          BSP_SD_SDHC_ClkEn()
 *
 * @brief    Enables SD controller clock.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_ClkEn(void)
{
#if (BSP_SD_ROUTE_SEL == BSP_SD_ROUTE_EXP_HDR)                  // For 20-pin EXP Header SD route.
  CMU_ClockEnable(cmuClock_SDIOREF, true);

  CMU_AUXHFRCOBandSet(cmuAUXHFRCOFreq_38M0Hz);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_SDIOREF, cmuSelect_AUXHFRCO);

  CMU_ClockEnable(cmuClock_GPIO, true);

  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_SDIO;

  DEF_BIT_FIELD_WR(CMU->SDIOCTRL, CMU_SDIOCTRL_SDIOCLKSEL_AUXHFRCO, _CMU_SDIOCTRL_SDIOCLKSEL_MASK);
#else
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockEnable(cmuClock_SDIOREF, true);

  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
  CMU_OscillatorEnable(cmuOsc_USHFRCO, true, true);

  CMU_ClockEnable(cmuClock_GPIO, true);

  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_SDIO;

  DEF_BIT_FIELD_WR(CMU->SDIOCTRL, CMU_SDIOCTRL_SDIOCLKSEL_HFRCO, _CMU_SDIOCTRL_SDIOCLKSEL_MASK);
#endif

  DEF_BIT_CLR(CMU->SDIOCTRL, CMU_SDIOCTRL_SDIOCLKDIS);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         BSP_SD_SDHC_IO_Cfg()
 *
 * @brief    Configures I/O.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_IO_Cfg(void)
{
#if (BSP_SD_ROUTE_SEL == BSP_SD_ROUTE_EXP_HDR)                  // For 20-pin EXP Header SD route.
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_DAT3_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_DAT2_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_DAT1_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_DAT0_PIN_NBR, gpioModePushPull, 1u);

  GPIO_PinModeSet(gpioPortE, BSP_SDIO_CMD_PIN_NBR, gpioModePushPull, 0u);
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_CLK_PIN_NBR, gpioModePushPull, 1u);

  //                                                               Select proper location for each SDIO pin.
  SDIO->ROUTELOC0 = SDIO_ROUTELOC0_DATLOC_LOC0
                    | SDIO_ROUTELOC0_CDLOC_LOC0
                    | SDIO_ROUTELOC0_WPLOC_LOC0
                    | SDIO_ROUTELOC0_CLKLOC_LOC0;
  SDIO->ROUTELOC1 = SDIO_ROUTELOC1_CMDLOC_LOC0;
#else                                                           // For SD card slot SD route.
  //                                                               Enable SD power.
  GPIO_PinModeSet(gpioPortE, 7u, gpioModePushPull, 1);
  GPIO_PinOutSet(gpioPortE, 7u);

  GPIO_PinModeSet(gpioPortA, BSP_SDIO_DAT3_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortA, BSP_SDIO_DAT2_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortA, BSP_SDIO_DAT1_PIN_NBR, gpioModePushPull, 1u);
  GPIO_PinModeSet(gpioPortA, BSP_SDIO_DAT0_PIN_NBR, gpioModePushPull, 1u);

  GPIO_PinModeSet(gpioPortE, BSP_SDIO_CMD_PIN_NBR, gpioModePushPull, 0u);
  GPIO_PinModeSet(gpioPortE, BSP_SDIO_CLK_PIN_NBR, gpioModePushPull, 1u);

  GPIO_PinModeSet(gpioPortB, BSP_SDIO_CD_PIN_NBR, gpioModeInput, 0);
  //                                                               Select proper location for each SDIO pin.
  SDIO->ROUTELOC0 = SDIO_ROUTELOC0_DATLOC_LOC1
                    | SDIO_ROUTELOC0_CDLOC_LOC3
                    | SDIO_ROUTELOC0_WPLOC_LOC3
                    | SDIO_ROUTELOC0_CLKLOC_LOC1;
  SDIO->ROUTELOC1 = SDIO_ROUTELOC1_CMDLOC_LOC1;
#endif

  //                                                               Enable SD pins.
  SDIO->ROUTEPEN = SDIO_ROUTEPEN_CLKPEN
                   | SDIO_ROUTEPEN_CMDPEN
                   | SDIO_ROUTEPEN_D0PEN
                   | SDIO_ROUTEPEN_D1PEN
                   | SDIO_ROUTEPEN_D2PEN
                   | SDIO_ROUTEPEN_D3PEN;

  SDIO->CTRL = (0 << _SDIO_CTRL_ITAPDLYEN_SHIFT)
               | (0 << _SDIO_CTRL_ITAPDLYSEL_SHIFT)
               | (0 << _SDIO_CTRL_ITAPCHGWIN_SHIFT)
               | (1 << _SDIO_CTRL_OTAPDLYEN_SHIFT)
               | (8 << _SDIO_CTRL_OTAPDLYSEL_SHIFT);

  SDIO->CFG0 = (0x20 << _SDIO_CFG0_TUNINGCNT_SHIFT)
               | (0x30 << _SDIO_CFG0_TOUTCLKFREQ_SHIFT)
               | (1 << _SDIO_CFG0_TOUTCLKUNIT_SHIFT)
               | (0xD0 << _SDIO_CFG0_BASECLKFREQ_SHIFT)
               | (SDIO_CFG0_MAXBLKLEN_1024B)
               | (1 << _SDIO_CFG0_C8BITSUP_SHIFT)
               | (1 << _SDIO_CFG0_CADMA2SUP_SHIFT)
               | (1 << _SDIO_CFG0_CHSSUP_SHIFT)
               | (1 << _SDIO_CFG0_CSDMASUP_SHIFT)
               | (1 << _SDIO_CFG0_CSUSPRESSUP_SHIFT)
               | (1 << _SDIO_CFG0_C3P3VSUP_SHIFT)
               | (1 << _SDIO_CFG0_C3P0VSUP_SHIFT)
               | (1 << _SDIO_CFG0_C1P8VSUP_SHIFT);

  SDIO->CFG1 = (0 << _SDIO_CFG1_ASYNCINTRSUP_SHIFT)
#ifdef eMMC_CARD
               | (SDIO_CFG1_SLOTTYPE_RMSDSLOT)
#else
               | (SDIO_CFG1_SLOTTYPE_EMSDSLOT)
#endif
               | (1 << _SDIO_CFG1_CSDR50SUP_SHIFT)
               | (1 << _SDIO_CFG1_CSDR104SUP_SHIFT)
               | (1 << _SDIO_CFG1_CDDR50SUP_SHIFT)
               | (1 << _SDIO_CFG1_CDRVASUP_SHIFT)
               | (1 << _SDIO_CFG1_CDRVCSUP_SHIFT)
               | (1 << _SDIO_CFG1_CDRVDSUP_SHIFT)
               | (1 << _SDIO_CFG1_RETUNTMRCTL_SHIFT)
               | (1 << _SDIO_CFG1_TUNSDR50_SHIFT)
               | (0 << _SDIO_CFG1_RETUNMODES_SHIFT)
               | (1 << _SDIO_CFG1_SPISUP_SHIFT)
               | (1 << _SDIO_CFG1_ASYNCWKUPEN_SHIFT);

  SDIO->CFGPRESETVAL0 = (0 << _SDIO_CFGPRESETVAL0_INITSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_INITCLKGENEN_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_INITDRVST_SHIFT)
                        | (0x4 << _SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_DSPCLKGENEN_SHIFT)
                        | (0x3 << _SDIO_CFGPRESETVAL0_DSPDRVST_SHIFT);

  SDIO->CFGPRESETVAL1 = (2 << _SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL1_HSPCLKGENEN_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL1_HSPDRVST_SHIFT)
                        | (4 << _SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL1_SDR12CLKGENEN_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL1_SDR12DRVST_SHIFT);

  SDIO->CFGPRESETVAL2 = (2 << _SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR25CLKGENEN_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR25DRVST_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR50CLKGENEN_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL2_SDR50DRVST_SHIFT);

  SDIO->CFGPRESETVAL3 = (0 << _SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL3_SDR104CLKGENEN_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL3_SDR104DRVST_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL3_DDR50CLKGENEN_SHIFT)
                        | (3 << _SDIO_CFGPRESETVAL3_DDR50DRVST_SHIFT);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         BSP_SD_SDHC_IntCfg()
 *
 * @brief    Configures interruption.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_IntCfg(void)
{
  NVIC_ClearPendingIRQ(SDIO_IRQn);
  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         BSP_SD_SDHC_PwrCfg()
 *
 * @brief    Configures controller power.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_PwrCfg(void)
{
  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                          BSP_SD_SDHC_Start()
 *
 * @brief    Starts controller.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_Start(void)
{
  NVIC_EnableIRQ(SDIO_IRQn);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                          BSP_SD_SDHC_Stop()
 *
 * @brief    Stops controller.
 *
 * @return   DEF_OK, if successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_SD_SDHC_Stop(void)
{
  NVIC_DisableIRQ(SDIO_IRQn);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       BSP_SD_SDHC_ClkFreqGet()
 *
 * @brief    Gets SD clock frequency.
 *
 * @return   Clock frequency, in hertz.
 *******************************************************************************************************/
static CPU_INT32U BSP_SD_SDHC_ClkFreqGet(void)
{
#if (BSP_SD_ROUTE_SEL == BSP_SD_ROUTE_EXP_HDR)                  // For 20-pin EXP Header SD route.
  CMU_AUXHFRCOFreq_TypeDef  freq;

  freq = CMU_AUXHFRCOBandGet();
#else
  CMU_HFRCOFreq_TypeDef  freq;

  freq = CMU_HFRCOBandGet();
#endif

  return ((CPU_INT32U)freq);
}

/****************************************************************************************************//**
 *                                    BSP_SD_SDHC_CapabilitiesGet()
 *
 * @brief    Allows to update the Host Controller capabilities.
 *
 * @param    p_capabilities  Pointer to the Host Controller capabilities variable.
 *
 * @note     (1) This Host Controller does not support the voltage switch to 1.8V for signaling.
 *******************************************************************************************************/
static void BSP_SD_SDHC_CapabilitiesGet(SD_HOST_CAPABILITIES *p_capabilities)
{
  //                                                               See Note #1.
  DEF_BIT_CLR(p_capabilities->Capabilities, SD_CAP_BUS_SIGNAL_VOLT_1_8);
}

/****************************************************************************************************//**
 *                                          SDIO_IRQHandler()
 *
 * @brief    Interrupt Service Routine Handler.
 *******************************************************************************************************/
void SDIO_IRQHandler(void)
{
  OSIntEnter();
  BSP_SD_SDHC_ISR_Fnct(BSP_SD_SDHC_DrvPtr);
  OSIntExit();
}

#endif // RTOS_MODULE_IO_SD_AVAIL
