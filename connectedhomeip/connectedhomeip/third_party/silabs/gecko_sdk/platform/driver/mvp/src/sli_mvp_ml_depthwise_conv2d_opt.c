/***************************************************************************//**
 * @file
 * @brief C implementation of optimized depthwise 2D convolution.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_mvp_ml_depthwise_conv2d.h"
#include "sl_mvp_math.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"

sl_status_t sli_mvp_ml_depthwise_conv2d_s8_gen_opt(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params,
                                                   bool execute)
{
  const int stride_width               = params->stride_width;
  const int stride_height              = params->stride_height;
  const int pad_width                  = params->pad_width;
  const int pad_height                 = params->pad_height;
  const int32_t input_offset           = params->input_offset;
  const int32_t output_offset          = params->output_offset;
  const int batches                    = params->batches;
  const int input_height               = params->input_height;
  const int input_width                = params->input_width;
  const int input_depth                = params->in_channels;
  const int filter_height              = params->filter_height;
  const int filter_width               = params->filter_width;
  const int filter_depth               = params->out_channels;
  const int output_height              = params->output_height;
  const int output_width               = params->output_width;
  const int output_depth               = params->out_channels;
  const int32_t activation_min         = params->output_activation_min;
  const int32_t activation_max         = params->output_activation_max;
  const int8_t *input                  = params->input;
  const int8_t *filter                 = params->filter;
  const float16_t *bias                = params->bias;
  const float16_t *output_scaler       = params->output_scaler;
  int8_t *output                       = params->output;
  int prog_index                       = 0;
  __ALIGNED(4) const float16_t zero[2] = { .0f, .0f };

  if (execute) {
    if ((((uint32_t)bias & 0x1U) != 0U)
        || (((uint32_t)output_scaler & 0x1U) != 0U)) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  bool use_parallel = false;
  if (((output_depth & 0x1U) == 0U)
      && (((uint32_t)input & 0x1U) == 0U)
      && (((uint32_t)output & 0x1U) == 0U)
      && (((uint32_t)filter & 0x1U) == 0U)
      && (((uint32_t)bias & 0x3U) == 0U)
      && (((uint32_t)output_scaler & 0x3U) == 0U)) {
    use_parallel = true;
  }
  int effective_depth = use_parallel ? output_depth / 2 : output_depth;

  if (filter_width > 1024 || filter_height > 1024) {
    // Too large convolution loop
    return SL_STATUS_INVALID_RANGE;
  }

  if (((uint32_t)input_width * effective_depth) > SLI_MVP_MAX_ROW_STRIDE) {
    // Too far between convolution values
    return SL_STATUS_INVALID_RANGE;
  }

  if (((uint32_t)filter_width * effective_depth) > SLI_MVP_MAX_ROW_STRIDE) {
    // Too far between convolution values
    return SL_STATUS_INVALID_RANGE;
  }

  if (((uint32_t)filter_width > SLI_MVP_MAX_VECTOR_COUNT) || ((uint32_t)filter_height > SLI_MVP_MAX_ROW_LENGTH)) {
    // Too many values per convolution
    return SL_STATUS_INVALID_RANGE;
  }

  if (!execute) {
    // Range validation complete; early exit
    return SL_STATUS_OK;
  }

  sli_mvp_program_t *mvp_prog = sli_mvp_get_program_area_double();

  for (int batch = 0; batch < batches; ++batch) {
    for (int out_y = 0; out_y < output_height; ++out_y) {
      for (int out_x = 0; out_x < output_width; ++out_x) {
        // Compute virtual top left corner of full filter overlay on input
        // Will be negative when padding=same and filter goes outside of input on the top/left
        const int in_x_origin = (out_x * stride_width) - pad_width;
        const int in_y_origin = (out_y * stride_height) - pad_height;
        // Compute effective filter top left coordinate
        const int filter_x_begin = in_x_origin < 0 ? -in_x_origin : 0;
        const int filter_y_begin = in_y_origin < 0 ? -in_y_origin : 0;
        // Compute effective input top left coordinate
        const int in_x_begin = in_x_origin < 0 ? 0 : in_x_origin;
        const int in_y_begin = in_y_origin < 0 ? 0 : in_y_origin;
        // Compute effective size of convolution. Reduced from full size if filter goes outside of input on the bottom/right
        const int x_size = (in_x_origin + filter_width >= input_width ? input_width - in_x_origin : filter_width) - filter_x_begin;
        const int y_size = (in_y_origin + filter_height >= input_height ? input_height - in_y_origin : filter_height) - filter_y_begin;

        // Create MVP program for this output
        sli_mvp_program_t *p = &mvp_prog[prog_index];

        // Registers
        // R0 = ACCUMULATOR_SCALER
        // R1 = input_offset_scaled = input_offset * ACCUMULATOR_SCALER
        // R2 = output_offset
        // R3 = activation_min
        // R4 = activation_max
        sli_mvp_prog_set_reg_f16c(p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER, SLI_MVP_ACCUMULATOR_SCALER);
        sli_mvp_prog_set_reg_f16c(p, SLI_MVP_R1, (float16_t)(input_offset * SLI_MVP_ACCUMULATOR_SCALER), (float16_t)(input_offset * SLI_MVP_ACCUMULATOR_SCALER));
        sli_mvp_prog_set_reg_f16c(p, SLI_MVP_R2, (float16_t)output_offset, (float16_t)output_offset);
        bool do_activation = (activation_min != -128) || (activation_max != 127);
        if (do_activation) {
          sli_mvp_prog_set_reg_f16c(p, SLI_MVP_R3, (float16_t)activation_min, (float16_t)activation_min);
          sli_mvp_prog_set_reg_f16c(p, SLI_MVP_R4, (float16_t)activation_max, (float16_t)activation_max);
        }

        // ARRAY0 = input
        sli_mvp_prog_set_array_full(
          p,
          SLI_MVP_ARRAY(0),
          (void *)&input[batch * input_height * input_width * input_depth + in_y_begin * input_width * input_depth + in_x_begin * input_depth],
          use_parallel ? SLI_MVP_DATATYPE_COMPLEX_INT8 : SLI_MVP_DATATYPE_INT8,
          effective_depth,
          y_size,
          x_size,
          1,
          input_width * effective_depth,
          effective_depth
          );

        // ARRAY1 = filter
        sli_mvp_prog_set_array_full(
          p,
          SLI_MVP_ARRAY(1),
          (void *)&filter[batch * filter_height * filter_width * filter_depth + filter_y_begin * filter_width * filter_depth + filter_x_begin * filter_depth],
          use_parallel ? SLI_MVP_DATATYPE_COMPLEX_INT8 : SLI_MVP_DATATYPE_INT8,
          effective_depth,
          y_size,
          x_size,
          1,
          filter_width * effective_depth,
          effective_depth
          );

        // ARRAY2 = bias
        sli_mvp_prog_set_vector(
          p,
          SLI_MVP_ARRAY(2),
          bias != NULL ? (void*)bias : (void*)zero,
          use_parallel ? SLI_MVP_DATATYPE_COMPLEX_BINARY16 : SLI_MVP_DATATYPE_BINARY16,
          bias != NULL ? effective_depth : 1);  // length

        // ARRAY3 = output_scaler
        sli_mvp_prog_set_vector(
          p,
          SLI_MVP_ARRAY(3),
          (void*)output_scaler,                 // addr
          use_parallel ? SLI_MVP_DATATYPE_COMPLEX_BINARY16 : SLI_MVP_DATATYPE_BINARY16,
          effective_depth);                     // length

        // ARRAY4 = output
        sli_mvp_prog_set_array_full(
          p,
          SLI_MVP_ARRAY(4),
          (void *)&output[batch * output_height * output_width * output_depth + out_y * output_width * output_depth + out_x * output_depth],
          use_parallel ? SLI_MVP_DATATYPE_COMPLEX_INT8 : SLI_MVP_DATATYPE_INT8,
          effective_depth,
          1,
          1,
          1,
          effective_depth,
          effective_depth
          );

        // R5 = LOAD(A2) bias_i = load bias_array
        sli_mvp_prog_set_instr(
          p,
          SLI_MVP_INSTR(0),
          SLI_MVP_OP(NOOP),
          SLI_MVP_NONE,
          SLI_MVP_LOAD(0, SLI_MVP_R5, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM2),
          SLI_MVP_NONE,
          false
          );

        // R6 = LOAD(A0) input_i = load input_array
        // R7 = LOAD(A1) filter_i = load filter_array
        // R6 = MAC(R6, R0, R1) input_i = input_i * ACCUMULATOR_SCALER + input_offset_scaled;
        sli_mvp_prog_set_instr(
          p,
          SLI_MVP_INSTR(1),
          SLI_MVP_OP(MACR2A),
          SLI_MVP_ALU_Z(SLI_MVP_R6)
          | SLI_MVP_ALU_X(SLI_MVP_R6)
          | SLI_MVP_ALU_Y(SLI_MVP_R0)
          | SLI_MVP_ALU_A(SLI_MVP_R1),
          SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM2)
          | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM2),
          SLI_MVP_NONE,
          false
          );

        // R5 = MAC(R6, R7, R5) acc += input_i * filter_i
        sli_mvp_prog_set_instr(
          p,
          SLI_MVP_INSTR(2),
          SLI_MVP_OP(MACR2A),
          SLI_MVP_ALU_Z(SLI_MVP_R5)
          | SLI_MVP_ALU_X(SLI_MVP_R6)
          | SLI_MVP_ALU_Y(SLI_MVP_R7)
          | SLI_MVP_ALU_A(SLI_MVP_R5),
          SLI_MVP_NONE,
          SLI_MVP_NONE,
          false
          );

        // R6 = LOAD(A3) output_scaler_i = load output_scaler_array
        // R7 = MAC(R5, R6, R2) output_i = acc * output_scaler_i + output_offset
        // A4 = STORE(R7) output_array = store output_i
        sli_mvp_prog_set_instr(
          p,
          SLI_MVP_INSTR(3),
          SLI_MVP_OP(MACR2A),
          SLI_MVP_ALU_Z(SLI_MVP_R7)
          | SLI_MVP_ALU_X(SLI_MVP_R5)
          | SLI_MVP_ALU_Y(SLI_MVP_R6)
          | SLI_MVP_ALU_A(SLI_MVP_R2),
          SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM2),
          do_activation
          ? SLI_MVP_NONE
          : SLI_MVP_STORE(SLI_MVP_R7, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM0),
          do_activation
          ? false
          : true
          );

        if (do_activation) {
          // R7 = CLIP2A(R3, R4, R7)
          // output = R7    output_i = MIN(MAX(R7,R3),R4)
          // A4 = STORE(R7) output_array = store output_i
          sli_mvp_prog_set_instr(
            p,
            SLI_MVP_INSTR(4),
            SLI_MVP_OP(CLIP2A),
            SLI_MVP_ALU_Z(SLI_MVP_R7)
            | SLI_MVP_ALU_X(SLI_MVP_R3)
            | SLI_MVP_ALU_Y(SLI_MVP_R4)
            | SLI_MVP_ALU_A(SLI_MVP_R7),
            SLI_MVP_NONE,
            SLI_MVP_STORE(SLI_MVP_R7, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM0),
            true
            );
        }

        // Loop over all depth
        sli_mvp_prog_set_loop(
          p,
          SLI_MVP_LOOP(0),
          effective_depth,
          SLI_MVP_INSTR(0),
          do_activation
          ? SLI_MVP_INSTR(4)
          : SLI_MVP_INSTR(3),
          SLI_MVP_NOINCR
          );

        // LOOP(y): increment depth afterwards
        sli_mvp_prog_set_loop(
          p,
          SLI_MVP_LOOP(1),
          y_size,
          SLI_MVP_INSTR(1),
          SLI_MVP_INSTR(2),
          SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM0) // incr d/2 end of every loop
          | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM0)
          );

        // LOOP(x): increment y afterwards
        sli_mvp_prog_set_loop(
          p,
          SLI_MVP_LOOP(2),
          x_size,
          SLI_MVP_INSTR(1),
          SLI_MVP_INSTR(2),
          SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM1) // incr y/1 end of every loop
          | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM1)
          );

        sli_mvp_execute(p, false);
        prog_index ^= 1;
      }
    }
  }

  sli_mvp_wait_for_completion();
  return SL_STATUS_OK;
}
