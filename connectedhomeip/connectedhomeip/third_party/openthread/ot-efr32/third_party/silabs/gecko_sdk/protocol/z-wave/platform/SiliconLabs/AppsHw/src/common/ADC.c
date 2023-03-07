/**
*
* @file
* @brief ADC utility functions
*
* @copyright 2018 Silicon Laboratories Inc.
*
*/

#include "ADC.h"
#include <em_cmu.h>

#if defined(ADC_COUNT) && (ADC_COUNT > 0)

#include <em_adc.h>

static void ADC_Input_Select(ADC_PosSel_TypeDef input)
{

  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  // Init for single conversion use, use 5V reference
  singleInit.reference  = adcRef5V;
  singleInit.posSel     = input;
  singleInit.resolution = adcRes12Bit;
  singleInit.acqTime    = adcAcqTime256;

  ADC_InitSingle(ADC0, &singleInit);

}

uint32_t ADC_Measure_VSupply(void)
{
  uint32_t sampleAVDD;

  // Measure AVDD (battery supply voltage level)
  ADC_Input_Select(adcPosSelAVDD);
  ADC_Start(ADC0, adcStartSingle);
  while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;
  sampleAVDD = ADC_DataSingleGet(ADC0);

  // Convert to mV (5V ref and 12 bit resolution)
  sampleAVDD = (sampleAVDD * 5000) / 4095;

  return sampleAVDD;
}

void ADC_Enable(void)
{
   ADC_Init_TypeDef adcInit = ADC_INIT_DEFAULT;

   // Enable ADC clock
   CMU_ClockEnable(cmuClock_ADC0, true);

   // Setup ADC
   adcInit.timebase = ADC_TimebaseCalc(0);
   // Set ADC clock to 7 MHz, use default HFPERCLK
   adcInit.prescale = ADC_PrescaleCalc(7000000, 0);
   ADC_Init(ADC0, &adcInit);
}

void ADC_Disable(void)
{
   ADC_Reset(ADC0);
   // Disable ADC clock
   CMU_ClockEnable(cmuClock_ADC0, false);
}

#endif /* defined(ADC_COUNT) && (ADC_COUNT > 0) */
