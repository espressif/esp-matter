/***************************************************************************//**
 * @file
 * @brief Code for PS board Si7211-EB demo
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
#include "em_adc.h"

#include "wheel.h"
#include "graphics.h"
#include "exp_si72xx.h"

#include "ps_si7211.h"

extern volatile bool demoInitiated;

/**************************************************************************//**
 * @brief ADC Initialization
 *****************************************************************************/
static void adcInit(void)
{
  CMU_ClockEnable(cmuClock_ADC0, true);
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(7000000, 0);
  ADC_Init(ADC0, &init);

  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
  initSingle.prsSel         = adcPRSSELCh0;
  initSingle.acqTime        = adcAcqTime1;
  initSingle.resolution     = adcRes12Bit;
  initSingle.input          = adcSingleInpCh4;
  initSingle.reference      = adcRefVDD;
  initSingle.diff           = false;
  initSingle.prsEnable      = false;
  initSingle.leftAdjust     = false;
  initSingle.rep            = false;
  ADC_InitSingle(ADC0, &initSingle);
}

static void analogSetup(void)
{
  EXP_SI72XX_placeSensorsInSleepMode();
  gpioDisablePushButton1();
  adcInit();
}

/**************************************************************************//**
 * @brief  Measure PS_Board analog output voltage
 * @return ADC conversion result
 *****************************************************************************/
static uint16_t measureAnalogOut(void)
{
  uint8_t j = 0;
  uint32_t analogOut = 0;
  for (j = 0; j < SAMPLES_TO_AVG; j++) {
    ADC_Start(ADC0, adcStartSingle);
    while ( (ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0 ) {
    }
    analogOut = analogOut + ADC_DataSingleGet(ADC0);
  }
  return (analogOut / SAMPLES_TO_AVG);
}

/**************************************************************************//**
 * @brief  Convert Si7211 Analog Output Voltage to Magnetic Field (uT)
 * @return microTeslas
 *****************************************************************************/
static int32_t convertAnalogVoltageToMagneticField(uint16_t outputVoltage,
                                                   uint16_t vddVoltage)
{
  int32_t microTeslas;

  /* Full-scale magnetic field = 20470uT */
  uint16_t uT_FullScale = 20470;

  /* magnetic-field(uT) = 20470uT*(2*Vout/Vdd - 1) */
  microTeslas = ((uint32_t)outputVoltage * 2 * uT_FullScale / vddVoltage) - uT_FullScale;
  return microTeslas;
}

/**************************************************************************//**
 * @brief Run PS_Board Si7211 Analog Demo
 *****************************************************************************/
void PS_SI7211_runAnalogDemo(void)
{
  /* ADC(12bit) w/ V(ref)=VDD */
  uint16_t vddData = 4095;
  uint16_t adcOutputData;
  uint16_t outputRatio;
  int32_t magField;

  if (!demoInitiated) {
    analogSetup();
    demoInitiated = true;
  }

  adcOutputData = measureAnalogOut();
  outputRatio = (adcOutputData * 1000) / vddData;
  magField = convertAnalogVoltageToMagneticField(adcOutputData, vddData);

  GRAPHICS_PS_Analog(outputRatio, magField);
}
