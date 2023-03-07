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

#if defined(IADC_COUNT) && (IADC_COUNT > 0)

#include <em_iadc.h>

static void ADC_Input_Select(IADC_PosInput_t input)
{
  IADC_InitSingle_t initSingle   = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;
  singleInput.negInput = iadcNegInputGnd;
  singleInput.posInput = input; // the positive input (avdd) is scaled down by 4
  IADC_initSingle(IADC0, &initSingle, &singleInput);
}

uint32_t ADC_Measure_VSupply(void)
{
  uint32_t sampleAVDD;

  // Measure AVDD (battery supply voltage level)
  ADC_Input_Select(iadcPosInputAvdd);
  IADC_command(IADC0,iadcCmdStartSingle);
  while (IADC0->SINGLEFIFOSTAT == 0) ;
  sampleAVDD = IADC_pullSingleFifoData(IADC0);

  // Convert to mV (1.21V ref and 12 bit resolution).
  // Also we scale the sample up by 4 since the input was scaled down by 4
  sampleAVDD = ((sampleAVDD * 1210 ) / 4095) * 4 ;

  return sampleAVDD;
}

void ADC_Enable(void)
{
  /*800 sereis has only 1.21 reference volatge therefore we need to scale down the input voltage by 4*/
  IADC_Init_t init               = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t allConfigs   = IADC_ALLCONFIGS_DEFAULT;
  CMU_ClockEnable(cmuClock_IADC0, true);
  IADC_init(IADC0, &init, &allConfigs);
}

void ADC_Disable(void)
{
   IADC_reset(IADC0);
   // Disable IADC clock
   CMU_ClockEnable(cmuClock_IADC0, false);
}

#endif /* defined(IADC_COUNT) && (IADC_COUNT > 0) */