/***************************************************************************//**
 * @file
 * @brief Helper functions for using the backup power mode
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rmu.h"
#include "em_rtcc.h"

#include "app_bumode.h"

/**************************************************************************//**
 * @brief  Configure backup power mode and restore values from retention
 *         registers if we are recovering from a backup event
 *****************************************************************************/
void buSetup(uint32_t * rtccResetVal,
             uint32_t * rtccResetTs,
             uint32_t * bodCnt,
             uint32_t * buTime,
             uint32_t * bodTime,
             uint8_t * currentApp)
{
  EMU_EM4Init_TypeDef      em4Init      = EMU_EM4INIT_DEFAULT;
  EMU_VmonHystInit_TypeDef vmonHystInit = EMU_VMONHYSTINIT_DEFAULT;
  ADC_Init_TypeDef         adcInit      = ADC_INIT_DEFAULT;
  EMU_BUInit_TypeDef       buInit       = EMU_BUINIT_DEFAULT;

  // Enable RTCC clock sources so we can read from its registers
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Use LFXO as clock source for the RTCC
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockEnable(cmuClock_RTCC, true);

  /* Check if we are waking from a backup power event,
     and restore RTCC and retention register values if we are */
  if (RMU->RSTCAUSE & RMU_RSTCAUSE_BUMODERST) {
    RTCC->RET[RET_BODCOUNT].REG++;
    *rtccResetVal = RTCC_CounterGet();
    *rtccResetTs = RTCC->CC[2].CCV;
    RMU_ResetCauseClear();
    EMU_UnlatchPinRetention();
    *bodCnt = RTCC->RET[RET_BODCOUNT].REG;
    *currentApp = (uint8_t)RTCC->RET[RET_CURAPP].REG;
    if (RTCC->STATUS & RTCC_STATUS_BUMODETS) {
      *buTime = (*rtccResetVal - *rtccResetTs) / 1024;
      *bodTime = RTCC->RET[RET_BODTIME].REG + *buTime;
    }
    RTCC->RET[RET_BODTIME].REG = *bodTime;
  } else {
    RTCC->RET[RET_BODCOUNT].REG = 0;
    RTCC->RET[RET_BODTIME].REG = 0;
    RTCC->RET[RET_CURAPP].REG = 0;
    RTCC->RET[RET_LEDCOLOR].REG = ((int32_t)330 << 16) | (int32_t)90;
    RTCC->RET[RET_LEDPOWER].REG = ((uint32_t)6 << 16) | (uint32_t)6;
  }

  if ((RMU->RSTCAUSE & RMU_RSTCAUSE_EM4RST) && !(RMU->RSTCAUSE & RMU_RSTCAUSE_BUMODERST)) {
    EMU_UnlatchPinRetention();
  }

  // VMON must be configured to enable backup mode
  vmonHystInit.fallThreshold = 1850;
  vmonHystInit.riseThreshold = 2200;

  EMU_VmonHystInit(&vmonHystInit);
  EMU_VmonEnable(emuVmonChannel_AVDD, true);

  buInit.pwrRes = emuBuPwrRes_Res3;
  buInit.buVinProbeEn = true;
  EMU_BUInit(&buInit);

  em4Init.retainLfxo = true;
  EMU_EM4Init(&em4Init);

  CMU_ClockEnable(cmuClock_ADC0, true);

  // Setup ADC to measure AVDD and BUVDD voltages
  adcInit.timebase = ADC_TimebaseCalc(0);
  // Set ADC clock to 7 MHz, use default HFPERCLK
  adcInit.prescale = ADC_PrescaleCalc(7000000, 0);
  ADC_Init(ADC0, &adcInit);
}

/**************************************************************************//**
 * @brief  Select ADC input channel
 *****************************************************************************/
void buADCInputSel(ADC_PosSel_TypeDef input)
{
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  // Init for single conversion use, use 5V reference
  singleInit.reference  = adcRef5V;
  singleInit.posSel      = input;
  singleInit.resolution = adcRes12Bit;
  singleInit.acqTime = adcAcqTime256;

  ADC_InitSingle(ADC0, &singleInit);
}

/**************************************************************************//**
 * @brief  Measure BUVDD and AVDD voltages
 *****************************************************************************/
void buMeasVdd(uint16_t * buVddBuffer, uint8_t buVddBufferIndex, uint16_t * aVddVoltage)
{
  /* Temp fix. This is currently missing in EMLIB. */
#define adcPosSelBUVDD (ADC_PosSel_TypeDef)225
  uint32_t sample;

  // Measure BUVDD
  buADCInputSel(adcPosSelBUVDD);
  ADC_Start(ADC0, adcStartSingle);
  while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;
  sample = ADC_DataSingleGet(ADC0);

  /* Calculate backup voltage relative to 5V reference
     and convert result to 1/100 volt scale */
  buVddBuffer[buVddBufferIndex] = (uint16_t)((sample * 8 * 5 * 100) / 4096);

  // Measure AVDD
  buADCInputSel(adcPosSelAVDD);
  ADC_Start(ADC0, adcStartSingle);
  while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;
  sample = ADC_DataSingleGet(ADC0);

  /* Calculate supply voltage relative to 5V reference
     and convert result to 1/100 volt scale */
  *aVddVoltage = (uint16_t)((sample * 5 * 100) / 4096);
}

/**************************************************************************//**
 * @brief  Control backup battery charging/discharging
 *****************************************************************************/
void buPwrSwitch(bool chargeEn, bool disCharge)
{
  if (chargeEn) {
    // Enable backup battery charging
    GPIO_PinModeSet(gpioPortD, 8, gpioModeDisabled, 0);
    EMU_BUBuInactPwrConSet(emuBuBuInactPwrCon_MainBu);
  } else if (disCharge) {
    // Ground backup battery to discharge it
    EMU_BUBuInactPwrConSet(emuBuBuInactPwrCon_None);
    GPIO_PinModeSet(gpioPortD, 8, gpioModeWiredAnd, 0);
  } else {
    // Disable charging/discharging
    EMU_BUBuInactPwrConSet(emuBuBuInactPwrCon_None);
    GPIO_PinModeSet(gpioPortD, 8, gpioModeDisabled, 0);
  }
}

/**************************************************************************//**
 * @brief  Store current app selection to retention registers
 *****************************************************************************/
void buSetCurApp(uint8_t app)
{
  RTCC->RET[RET_CURAPP].REG = app;
}

/**************************************************************************//**
 * @brief  Store RGB LED settings to retention registers
 *****************************************************************************/
void buSaveLedSettings(RGBLED_Settings_t * ledSettings)
{
  RTCC->RET[RET_LEDCOLOR].REG = ((int32_t)(ledSettings->led0Color) << 16) | ((int32_t)(ledSettings->led1Color));
  RTCC->RET[RET_LEDPOWER].REG = ((int32_t)(ledSettings->led0Power) << 16) | ((int32_t)(ledSettings->led1Power));
}

/**************************************************************************//**
 * @brief  Load RGB LED settings from retention registers
 *****************************************************************************/
void buLoadLedSettings(RGBLED_Settings_t * ledSettings)
{
  ledSettings->led0Color = (int16_t)(RTCC->RET[RET_LEDCOLOR].REG >> 16);
  ledSettings->led1Color = (int16_t)(RTCC->RET[RET_LEDCOLOR].REG & 0xFFFF);
  ledSettings->led0Power = (int8_t)(RTCC->RET[RET_LEDPOWER].REG >> 16);
  ledSettings->led1Power = (int8_t)(RTCC->RET[RET_LEDPOWER].REG & 0xFF);
}
