/***************************************************************************//**
 * @file
 * @brief Collect entropy from the ADC on Silicon Labs devices.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -------------------------------------
// Includes

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_ADC_C)

#include "em_device.h"

#if defined(ADC_PRESENT) && defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY)

#include "sl_entropy_adc.h"
#include <string.h>
#include <stdint.h>

// -------------------------------------
// Defines

#define SL_ENTROPY_ADC_OK         0
#define SL_VIN_ATTENUATION_FACTOR 1

// -------------------------------------
// Local function prototypes

static uint32_t entropy_adc_sample_get(mbedtls_entropy_adc_context *ctx);
static uint32_t jenkins_hash(uint8_t *buf, unsigned int len);

// -------------------------------------
// Local function definitions

/***************************************************************************//**
 * @brief
 *   Get one sample from the ADC.
 *
 * @return
 *   ADC sample value.
 ******************************************************************************/
static uint32_t entropy_adc_sample_get(mbedtls_entropy_adc_context *ctx)
{
  ADC_Start(ctx->adc, adcStartSingle);

  // Wait for ADC conversion to complete.
  while ((ctx->adc->STATUS & ADC_STATUS_SINGLEDV) == 0) ;

  // ADC has finished conversion, read and return sample.
  return ADC_DataSingleGet(ctx->adc);
}

/***************************************************************************//**
 * @brief
 *   Jenkins hash function.
 *
 * @param[in] buf
 *   Pointer to data buffer to hash.
 *
 * @param[in] len
 *   Size of data buffer in bytes.
 *
 * @return
 *   The 32 bits hash value.
 ******************************************************************************/
static uint32_t jenkins_hash(uint8_t *buf, unsigned int len)
{
  uint32_t hash;
  uint32_t i;

  // Jenkins hash algorithm.
  for (hash = i = 0; i < len; ++i) {
    hash += buf[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

// -------------------------------------
// Global function definitions

// Initialize an ADC entropy context
void mbedtls_entropy_adc_init(mbedtls_entropy_adc_context *ctx)
{
  memset(ctx, 0, sizeof(mbedtls_entropy_adc_context));
}

// Set and initialize an ADC hardware instance.
int mbedtls_entropy_adc_set_instance(mbedtls_entropy_adc_context *ctx,
                                     unsigned int adc_inst)
{
  // Base the ADC configuration on the default setup.
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initsingle = ADC_INITSINGLE_DEFAULT;

  if (adc_inst == 0) {
    ctx->adc = ADC0;
    ctx->clk = cmuClock_ADC0;
  } else {
    #if ADC_COUNT == 2
    if (adc_inst == 1) {
      ctx->adc = ADC1;
      ctx->clk = cmuClock_ADC1;
    }
    #else
    // Invalid ADC instance, set ADC pointer to 0 and return error.
    ctx->adc = 0;
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    #endif
  }

  // Enable HFPER clock.
  CMU_ClockEnable(cmuClock_HFPER, true);

  // Enable ADC clock.
  CMU_ClockEnable(ctx->clk, true);

  // Initialize timebases.
  init.timebase = ADC_TimebaseCalc(0);

  // Set ADC prescaler to generate the conversion clock (adc_clk_sar) and the
  // acquisition clock (adc_clk_samp) with cycle time longer than the minimal
  // acquisition time of 187.5ns, i.e. 1/187.5ns = 5333333 Hz clock frequency.
  // Use the desired ADC clock frequency of 5000000 Hz to make sure the cycle
  // time is longer than 187.5ns. Then we can use acquisition time of 1 cycle.
  init.prescale = ADC_PrescaleCalc(5000000, 0);

  // Make sure warmup mode is normal, i.e. warms up before each conversion
  // which results in best entropy.
  init.warmUpMode = adcWarmupNormal;

  ADC_Init(ctx->adc, &init);

  // Set input to the builtin entropy source.
  initsingle.reference   = adcRefVEntropy;
  initsingle.resolution  = adcRes12Bit;
  initsingle.diff        = true;
  initsingle.posSel      = adcPosSelVSS;
  initsingle.negSel      = adcNegSelVSS;
  initsingle.acqTime     = adcAcqTime1;

  ADC_InitSingle(ctx->adc, &initsingle);

  // Set attenuation factor to 1 for optimal entropy.
  ctx->adc->SINGLECTRLX &= ~_ADC_SINGLECTRLX_VINATT_MASK;
  ctx->adc->SINGLECTRLX |= SL_VIN_ATTENUATION_FACTOR << _ADC_SINGLECTRLX_VINATT_SHIFT;

  return SL_ENTROPY_ADC_OK;
}

// Free ADC entropy context
void mbedtls_entropy_adc_free(mbedtls_entropy_adc_context *ctx)
{
  if (ctx->adc != NULL) {
    // Reset ADC.
    ADC_Reset(ctx->adc);
    // Disable ADC clock.
    CMU_ClockEnable(ctx->clk, false);
  }
  // Clear context structure.
  memset(ctx, 0, sizeof(mbedtls_entropy_adc_context));
}

// Poll for entropy data
int mbedtls_entropy_adc_poll(mbedtls_entropy_adc_context *ctx,
                             unsigned char *output,
                             size_t len,
                             size_t *olen)
{
  uint32_t sample;
  uint32_t entropy;
  uint32_t hash;
  uint32_t i;

  *olen = len;

  // Generate 32-bit words and apply Jenkins hash whitening function to
  // each word.
  while (len > 0U) {
    // Generate 10 samples and extract 3 bits from each, then finally
    // extract 2 bits from one last sample to generate one 32-bit word.
    entropy = 0U;
    for (i = 0; i < 10U; i++) {
      sample = entropy_adc_sample_get(ctx);
      entropy |= (sample & 0x7) << (i * 3);
    }
    sample = entropy_adc_sample_get(ctx);
    entropy |= (sample & 0x3) << 30;

    hash = jenkins_hash((uint8_t*)&entropy, sizeof(entropy));

    // Copy to output buffer and update parsms. */
    i = len > sizeof(hash) ? sizeof(hash) : len;
    memcpy(output, &hash, i);

    output += i;
    len    -= i;
  }

  return SL_ENTROPY_ADC_OK;
}

#endif // ADC_PRESENT && _ADC_SINGLECTRLX_VREFSEL_VENTROPY
#endif // MBEDTLS_ENTROPY_C && MBEDTLS_ENTROPY_ADC_C
