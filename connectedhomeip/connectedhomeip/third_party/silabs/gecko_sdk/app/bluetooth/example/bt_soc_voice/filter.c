/***************************************************************************//**
 * @file
 * @brief Biquadratic filters implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "filter.h"

// -----------------------------------------------------------------------------
// Private macros

#ifndef M_LN2
#define M_LN2    0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

// -----------------------------------------------------------------------------
// Private type definitions

/** Filter coefficients */
typedef struct {
  sample_t A, omega, sn, cs, alpha, beta; // Input parameters
  sample_t a0, a1, a2, b0, b1, b2;        // Calculated parameters
} filter_coefficient_t;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Calculates Low Pass Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_lpf_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates High Pass Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_hpf_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates Band Pass Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_bpf_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates Notch Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_notch_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates Peaking Band EQ Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_peq_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates Low Shelf Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_lsh_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Calculates High Shelf Filter parameters.
 *
 * @param[in,out] c Filter coefficients.
 ******************************************************************************/
static void calc_hsh_parameters(filter_coefficient_t *c);

/***************************************************************************//**
 * Applies filter on a sample.
 *
 * @param[in] sample Data sample to be filtered.
 * @param[in] b Biquadratic filter data.
 *
 * @return Filtered sample.
 ******************************************************************************/
static sample_t compute(sample_t sample, biquad_t * b);

// -----------------------------------------------------------------------------
// Public function definitions

/***************************************************************************//**
 * Filter initialization.
 ******************************************************************************/
sl_status_t fil_init(filter_context_t *ctx, filter_parameters_t *fp)
{
  filter_coefficient_t coef;

  if ((ctx == NULL) || (fp == NULL) || (ctx->biquad_list == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (ctx->ch_count == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Setup input parameters.
  coef.A = pow(10, fp->db_gain / 40);
  coef.omega = 2 * M_PI * fp->freq / fp->srate;
  coef.sn = sin(coef.omega);
  coef.cs = cos(coef.omega);
  coef.alpha = coef.sn * sinh(M_LN2 / 2 * fp->bandwidth * coef.omega / coef.sn);
  coef.beta = sqrt(coef.A + coef.A);

  switch (fp->type) {
    case LPF:
      calc_lpf_parameters(&coef);
      break;
    case HPF:
      calc_hpf_parameters(&coef);
      break;
    case BPF:
      calc_bpf_parameters(&coef);
      break;
    case NOTCH:
      calc_notch_parameters(&coef);
      break;
    case PEQ:
      calc_peq_parameters(&coef);
      break;
    case LSH:
      calc_lsh_parameters(&coef);
      break;
    case HSH:
      calc_hsh_parameters(&coef);
      break;
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  for (uint8_t ch = 0; ch < ctx->ch_count; ch++) {
    biquad_t *b = &ctx->biquad_list[ch];

    // Pre-computed coefficients.
    b->a0 = coef.b0 / coef.a0;
    b->a1 = coef.b1 / coef.a0;
    b->a2 = coef.b2 / coef.a0;
    b->a3 = coef.a1 / coef.a0;
    b->a4 = coef.a2 / coef.a0;

    // Clean samples.
    b->x1 = b->x2 = 0;
    b->y1 = b->y2 = 0;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Filter audio data.
 ******************************************************************************/
sl_status_t fil_filter(filter_context_t *ctx, int16_t *in, int16_t *out, uint32_t n_frames)
{
  sample_t sample;
  uint32_t idx;

  if ((ctx == NULL) || (in == NULL) || (out == NULL) || (ctx->biquad_list == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  for (uint8_t ch = 0; ch < ctx->ch_count; ch++) {
    for (uint32_t i = 0; i < n_frames; i++) {
      idx = (i * ctx->ch_count) + ch;
      // Convert sample to unsigned data
      sample = (sample_t)((uint16_t)(((int32_t)in[idx]) - SHRT_MIN));
      out[idx] = (int16_t)compute(sample, &ctx->biquad_list[ch]);
    }
  }

  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
// Private function definitions

static void calc_lpf_parameters(filter_coefficient_t *c)
{
  c->b0 = (1 - c->cs) / 2;
  c->b1 = 1 - c->cs;
  c->b2 = (1 - c->cs) / 2;
  c->a0 = 1 + c->alpha;
  c->a1 = -2 * c->cs;
  c->a2 = 1 - c->alpha;
}

static void calc_hpf_parameters(filter_coefficient_t *c)
{
  c->b0 = (1 + c->cs) / 2;
  c->b1 = -(1 + c->cs);
  c->b2 = (1 + c->cs) / 2;
  c->a0 = 1 + c->alpha;
  c->a1 = -2 * c->cs;
  c->a2 = 1 - c->alpha;
}

static void calc_bpf_parameters(filter_coefficient_t *c)
{
  c->b0 = c->alpha;
  c->b1 = 0;
  c->b2 = -c->alpha;
  c->a0 = 1 + c->alpha;
  c->a1 = -2 * c->cs;
  c->a2 = 1 - c->alpha;
}

static void calc_notch_parameters(filter_coefficient_t *c)
{
  c->b0 = 1;
  c->b1 = -2 * c->cs;
  c->b2 = 1;
  c->a0 = 1 + c->alpha;
  c->a1 = -2 * c->cs;
  c->a2 = 1 - c->alpha;
}

static void calc_peq_parameters(filter_coefficient_t *c)
{
  c->b0 = 1 + (c->alpha * c->A);
  c->b1 = -2 * c->cs;
  c->b2 = 1 - (c->alpha * c->A);
  c->a0 = 1 + (c->alpha / c->A);
  c->a1 = -2 * c->cs;
  c->a2 = 1 - (c->alpha / c->A);
}

static void calc_lsh_parameters(filter_coefficient_t *c)
{
  c->b0 = c->A * ((c->A + 1) - (c->A - 1) * c->cs + c->beta * c->sn);
  c->b1 = 2 * c->A * ((c->A - 1) - (c->A + 1) * c->cs);
  c->b2 = c->A * ((c->A + 1) - (c->A - 1) * c->cs - c->beta * c->sn);
  c->a0 = (c->A + 1) + (c->A - 1) * c->cs + c->beta * c->sn;
  c->a1 = -2 * ((c->A - 1) + (c->A + 1) * c->cs);
  c->a2 = (c->A + 1) + (c->A - 1) * c->cs - c->beta * c->sn;
}

static void calc_hsh_parameters(filter_coefficient_t *c)
{
  c->b0 = c->A * ((c->A + 1) + (c->A - 1) * c->cs + c->beta * c->sn);
  c->b1 = -2 * c->A * ((c->A - 1) + (c->A + 1) * c->cs);
  c->b2 = c->A * ((c->A + 1) + (c->A - 1) * c->cs - c->beta * c->sn);
  c->a0 = (c->A + 1) - (c->A - 1) * c->cs + c->beta * c->sn;
  c->a1 = 2 * ((c->A - 1) - (c->A + 1) * c->cs);
  c->a2 = (c->A + 1) - (c->A - 1) * c->cs - c->beta * c->sn;
}

static sample_t compute(sample_t sample, biquad_t *b)
{
  sample_t result;

  // Compute result.
  result = b->a0 * sample + b->a1 * b->x1 + b->a2 * b->x2 - b->a3 * b->y1 - b->a4 * b->y2;

  // Shift x1 to x2, sample to x1.
  b->x2 = b->x1;
  b->x1 = sample;

  // Shift y1 to y2, result to y1.
  b->y2 = b->y1;
  b->y1 = result;

  return result;
}
