/***************************************************************************//**
 * @file
 * @brief MVP 2D Convolution kernel driver.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_mvp_config.h"
#include "sl_mvp_ml_conv2d.h"
#include "sl_mvp.h"
#include "sl_mvp_util.h"
#include "sl_mvp_math.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"
#include <stdbool.h>

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#define SLI_MVP_CHECK(x)                \
  do {                                  \
    if (!(x)) {                         \
      status = SL_STATUS_INVALID_RANGE; \
    }                                   \
  } while (0)

static sl_status_t conv2d(const sli_mvp_ml_conv2d_s8_params_t *params, bool execute);

__STATIC_FORCEINLINE int sli_div_floor_int(const int dividend, const int divisor)
{
  return dividend / divisor;
}

__STATIC_FORCEINLINE int sli_div_ceil_int(const int dividend, const int divisor)
{
  return (dividend / divisor) + (((dividend % divisor) != 0) ? 1 : 0);
}

/***************************************************************************//**
 *
 * 2D Convolution.
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_conv2d_s8(const sli_mvp_ml_conv2d_s8_params_t *params)
{
  return conv2d(params, true);
}

/***************************************************************************//**
 *
 * Check if MVP supports Conv2D on given matrix.
 *
 ******************************************************************************/
bool sli_mvp_ml_conv2d_s8_is_supported(const sli_mvp_ml_conv2d_s8_params_t *params)
{
  return conv2d(params, false) == SL_STATUS_OK;
}

/***************************************************************************//**
 *
 * Return the required scratch buffer size to perform the Conv2D operation
 *
 ******************************************************************************/
int sli_mvp_ml_conv2d_s8_get_scratch_buffer_size(const sli_mvp_ml_conv2d_s8_params_t *params)
{
  int scratch_buffer_size = 0;

  #if SL_MVP_OPTIMIZE_SPEED == 1
  // Required scratch buffer size is the input tensor as float16_t
  const int input_depth       = params->in_channels;
  const int input_height      = params->input_height;
  const int input_width       = params->input_width;
  scratch_buffer_size = (input_width * input_height * input_depth) * sizeof(float16_t);
  #else
  (void)params;
  #endif

  return scratch_buffer_size;
}

static sl_status_t conv2d(const sli_mvp_ml_conv2d_s8_params_t *params, bool execute)
{
  // Consume all input parameters.
  const float16_t input_offset_scaled = params->input_offset * SLI_MVP_ACCUMULATOR_SCALER;
  const int stride_width              = params->stride_width;
  const int stride_height             = params->stride_height;
  const int dilation_width_factor     = params->dilation_width;
  const int dilation_height_factor    = params->dilation_height;
  const int pad_width                 = params->pad_width;
  const int pad_height                = params->pad_height;
  const bool needs_padding            = params->padding;
  const int8_t *input                 = params->input;
  const int8_t *filter                = params->filter;
  const float16_t *bias               = params->bias;
  const float16_t *output_scaler      = params->output_scaler;
  const float16_t zero                = 0.0f;
  int8_t *output                      = params->output;

  #if SL_MVP_OPTIMIZE_SPEED == 1
  float16_t *scaled_input = (float16_t*)params->scratch_buffer;
  #endif

  sl_status_t status                  = SL_STATUS_OK;
  sli_mvp_program_context_t *p        = sli_mvp_get_program_area_context();

  if (needs_padding == false) {
    if ((pad_width != 0) || (pad_height != 0)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  if ((dilation_width_factor != 1) || (dilation_height_factor != 1)) {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  if (execute) {
    // Some 8-bit tensor buffers must be 2-byte aligned when processing two
    // values at a time with the complex int8 datatype, or if the datatype
    // itself dictates it.
    if ((((uint32_t)input & 0x1) != 0U)
        || (((uint32_t)filter & 0x1) != 0U)
        || (((uint32_t)bias & 0x1) != 0U)
        || (((uint32_t)output_scaler & 0x1) != 0U)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  if (status != SL_STATUS_OK) {
    if (execute) {
      EFM_ASSERT(false);
    }
    return status;
  }

  sli_mvp_pb_init_program(p);

  const int batches           = params->batches;
  const int input_depth       = params->in_channels;
  const int output_depth      = params->out_channels;
  const int input_height      = params->input_height;
  const int input_width       = params->input_width;
  const int filter_height     = params->filter_height;
  const int filter_width      = params->filter_width;
  const int output_height     = params->output_height;
  const int output_width      = params->output_width;
  const int32_t output_offset = params->output_offset;


#if (SL_MVP_OPTIMIZE_SPEED == 1)

  // If SL_MVP_OPTIMIZE_SPEED is enabled, calculate the input accumulator scaler
  // in a separate program. The scaled inputs are stored in a temporary array 
  // and used directly by the conv2D algorithm.
  for (int batch = 0; batch < batches; ++batch) {
    int input_stride_dim2 = 1;
    int input_size_dim2   = input_depth;
    int input_stride_dim1 = input_size_dim2;
    int input_size_dim1   = input_width;
    int input_stride_dim0 = input_size_dim2 * input_size_dim1;
    int input_size_dim0   = input_height;

    int input_index = sli_mvp_util_offset_nhwc(input_height, input_width, input_depth,
                                                batch /* out_channel_start */,
                                                0,
                                                0,
                                                0);

    // Condition to pack two reals to use both FMACs in MVP and double throughput.
    const bool use_parallel_mac_input_scaling =  (input_size_dim2    % 2 == 0)
                                              && (input_stride_dim1  % 2 == 0)
                                              && (input_stride_dim0  % 2 == 0)
                                              && (input_index        % 2 == 0);

    if (use_parallel_mac_input_scaling) {
      input_size_dim2    /= 2;
      input_stride_dim1  /= 2;
      input_stride_dim0  /= 2;
    }

    SLI_MVP_CHECK(input_stride_dim0 <= (int)SLI_MVP_MAX_VECTOR_STRIDE);

    sli_mvp_pb_begin_program(p);

    // Input array
    sli_mvp_pb_config_array_full(p->p,
                                 SLI_MVP_ARRAY(0),
                                 (void*)&input[input_index],
                                 use_parallel_mac_input_scaling == true
                                 ? SLI_MVP_DATATYPE_COMPLEX_INT8
                                 : SLI_MVP_DATATYPE_INT8,
                                 input_size_dim0,
                                 input_size_dim1,
                                 input_size_dim2,
                                 input_stride_dim0,
                                 input_stride_dim1,
                                 input_stride_dim2,
                                 &status);

    // Output array
    sli_mvp_pb_config_array_full(p->p,
                                 SLI_MVP_ARRAY(1),
                                 (void*)&scaled_input[input_index],
                                 use_parallel_mac_input_scaling == true
                                 ? SLI_MVP_DATATYPE_COMPLEX_BINARY16
                                 : SLI_MVP_DATATYPE_BINARY16,
                                 input_size_dim0,
                                 input_size_dim1,
                                 input_size_dim2,
                                 input_stride_dim0,
                                 input_stride_dim1,
                                 input_stride_dim2,
                                 &status);

    sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER);
    if (use_parallel_mac_input_scaling) {
      sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, input_offset_scaled, input_offset_scaled);
    } else {
      sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R1, input_offset_scaled);
    }

    sli_mvp_pb_begin_loop(p, input_size_dim0, &status); // input width
      sli_mvp_pb_begin_loop(p, input_size_dim1, &status); // input height
        sli_mvp_pb_begin_loop(p, input_size_dim2, &status); // input depth
          // LOAD(ARRAY0, R5)      Input
          // LOAD(ARRAY1, R7)      Filter
          // R6 = MACC(R5, R0, R1) Compute(r_input_i, MACC, r_input_i, c_accumulator_scaler, c_input_offset_scaled)
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(MACC),
                             SLI_MVP_ALU_Z(SLI_MVP_R6)
                             | SLI_MVP_ALU_X(SLI_MVP_R5)
                             | SLI_MVP_ALU_Y(SLI_MVP_R0)
                             | SLI_MVP_ALU_A(SLI_MVP_R1),
                             SLI_MVP_LOAD(0, SLI_MVP_R5, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM2),
                             SLI_MVP_STORE(SLI_MVP_R6, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM2),
                             &status);

        sli_mvp_pb_end_loop(p); // input depth
        sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM1);
        sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM1);
      sli_mvp_pb_end_loop(p); // input height
      sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM0);
      sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM0);
    sli_mvp_pb_end_loop(p); // input width


    // Check if any errors found during program generation.
    if (status != SL_STATUS_OK) {
      if (execute) {
        EFM_ASSERT(false);
      }
      return status;
    }

    if (execute) {
      sli_mvp_pb_execute_program(p);
    }
  }
#endif

  // Implemented as single parameterizable MVP program.
  // Note that there is some flexibility lost by having to compute full output
  // values at once vs. being able to have a partial sum stored in the output
  // and keep updating. However, with int8 output, don't want to lose precision
  // as we go.

  // Iterate over possible sub-filters that make up the edge boundary cases
  // and record the out_x, out_y ranges and filter ranges that correspond to
  // each case. Doesn't actually loop through each index, but only those that
  // correspond to unique sub-filters.
  const int in_y_origin_center_max = input_height - filter_height;
  const int out_y_center_max = sli_div_floor_int(in_y_origin_center_max + pad_height, stride_height);
  const int in_x_origin_center_max = input_width - filter_width;
  const int out_x_center_max = sli_div_floor_int(in_x_origin_center_max + pad_width, stride_width);

  for (int out_x_min = 0, out_x_max; out_x_min < output_width; out_x_min = out_x_max + 1) {
    /* Truncate filter width to actual filter width when filter starts outside of
    valid input area, i.e. padded area */
    const int in_x_origin_min = (out_x_min * stride_width) - pad_width;
    const int filter_x_start = SL_MAX(0, -in_x_origin_min);
    const int filter_x_end   = SL_MIN(filter_width, input_width - in_x_origin_min);
    const int filter_width_truncated = filter_x_end - filter_x_start;

    if (in_x_origin_min < 0) { // Case 1.  each out_x leads to 1 filter_x_start
      out_x_max = out_x_min;   // (and 1 or more filter_x_end)
    } else {                   // Case 2.  true for the rest of the out_x
      if (input_width - in_x_origin_min >= filter_width) {
        // Case 2a. true for multiple out_x
        out_x_max = out_x_center_max;
      } else {                 // Case 2b. each out_x leads to 1 filter_x_end
        out_x_max = out_x_min;
      }
    }
    const int output_width_truncated = out_x_max - out_x_min + 1;

    // When there is only one column, then don't need to use a dimension to
    // increment across it. Instead, use that dimension for row striding and
    // handle entire column in single program.
    bool single_out_x = output_width_truncated <= 1;

    for (int out_y_min = 0, out_y_max; out_y_min < output_height; out_y_min = out_y_max + 1) {
      const int in_y_origin_min = (out_y_min * stride_height) - pad_height;
      const int filter_y_start = SL_MAX(0, -in_y_origin_min);
      const int filter_y_end   = SL_MIN(filter_height, input_height - in_y_origin_min);
      const int filter_height_truncated = filter_y_end - filter_y_start;

      if (in_y_origin_min < 0) { // Case 1.  each out_y leads to 1 filter_y_start
        out_y_max = out_y_min;   // (and 1 or more filter_y_end)
      } else {                   // Case 2.  true for the rest of the out_y
        if (input_height - in_y_origin_min >= filter_height) {
          // Case 2a. true for multiple out_y
          out_y_max = out_y_center_max;
        } else {                 // Case 2b. each out_y leads to 1 filter_y_end
          out_y_max = out_y_min;
        }
      }
      int output_height_truncated = out_y_max - out_y_min + 1;

      // Special out_y iteration computations.
      int in_y_extra_incr;
      int out_y_incr;
      if (output_height_truncated == 1 || single_out_x) {
        // Trivial case. Need no group size calculation.
        in_y_extra_incr = 0;
        out_y_incr = 1;
      } else if (stride_height == filter_height_truncated) { // Case 1: no extra work required
        in_y_extra_incr = 0;
        out_y_incr = 1;
      } else if (stride_height > filter_height_truncated) {  // Case 2: requires an extra counting
                                                             // loop, but still one pass
        in_y_extra_incr = stride_height - filter_height_truncated;
        out_y_incr = 1;
      } else {                                               // Case 3: requires an extra counting
                                                             // loop and multiple passes
        int div_rem  = filter_height_truncated % stride_height;
        int div_quot = filter_height_truncated / stride_height;
        if (div_rem == 0) {                                  // Case 3a: filter_height_truncated
                                                             // exactly divides stride_height
          out_y_incr = div_quot;
          in_y_extra_incr = 0;
        } else {                                             // Case 3b: need to increment up to
          out_y_incr = div_quot + 1;  // Ceiling             // next stride height multiple
          in_y_extra_incr = stride_height - div_rem;
        }
      }

      int in_y_group_size = filter_height_truncated + in_y_extra_incr;
      if (output_height_truncated > 1 && !single_out_x) {
        SLI_MVP_CHECK(in_y_group_size == (out_y_incr * stride_height));
      }

      // Since do a subset of outputs per program that are spaced out, iterate
      // over the starting offset positions but don't spend time looping over
      // offsets that have no valid out_y in inner loop.
      int out_y_num_offsets = SL_MIN(out_y_incr, output_height_truncated);

      for (int out_y_offset = 0; out_y_offset < out_y_num_offsets; ++out_y_offset) {
        // Could compute out_y_size for offset 0 and use for all but it is possible
        // that some more programs may be possible with a tighter bound on array size.
        SLI_MVP_CHECK(output_height_truncated >= out_y_offset);
        int out_y_size = sli_div_ceil_int(output_height_truncated - out_y_offset, out_y_incr);
        // Don't need to worry about the increments if will never go to next output.
        int in_y_extra_incr_adjusted = out_y_size == 1 ? 0 : in_y_extra_incr;

        // Could just set in_y_size to input_height, but it may be possible to
        // enable more programs with a tigher bound on the number of elements.
        int in_y_size;
        if (single_out_x) {
          in_y_size = filter_height_truncated;
          SLI_MVP_CHECK(in_y_extra_incr_adjusted == 0);
        } else {
          in_y_size = (filter_height_truncated + in_y_extra_incr_adjusted) * out_y_size;
        }

        // Ideally, at this point (or in above loops) would determine:
        //  * if all dimensions fit within MVP.  otherwise split into chunks that do fit
        //  * if problem is too small for MVP, i.e., there is more CPU overhead
        //  *   launching MVP than just computing outright, then skip mvp.

        // No more dimensions to include batch loop in program, but it is generally just 1 anyway.
        for (int batch = 0; batch < batches; ++batch) {
          // These array indexing computations should be moved up to highest loop
          // level possible so uncompiled operation is more efficient.
          int output_index_base = sli_mvp_util_offset_nhwc(output_height, output_width, output_depth,
                                                           batch,
                                                           out_y_min + out_y_offset,
                                                           out_x_min, 0 /* out_channel_start */);
          int output_stride_col = 1;
          int output_size_col   = output_depth;
          int output_stride_row = output_depth;
          int output_size_row   = output_width_truncated;
          int output_stride_vec = out_y_incr * output_width * output_depth;
          int out_y_extra_incr  = 0;

          if (output_stride_vec > (int)SLI_MVP_MAX_VECTOR_STRIDE) {
            output_stride_vec = output_width * output_depth;
            out_y_extra_incr = out_y_incr - 1;
          }
          SLI_MVP_CHECK(output_stride_vec <= (int)SLI_MVP_MAX_VECTOR_STRIDE);
          int output_size_vec = out_y_size * (out_y_extra_incr + 1);

          SLI_MVP_CHECK(output_index_base >= 0);
          SLI_MVP_CHECK(output_stride_col >= 0);
          SLI_MVP_CHECK(output_stride_row >= 0);
          SLI_MVP_CHECK(output_stride_vec >= 0);

          int filter_index_base = sli_mvp_util_offset_nhwc(filter_height, filter_width, input_depth,
                                                           0 /* out_channel_start */,
                                                           filter_y_start,
                                                           filter_x_start,
                                                           0 /* in_channel_start */);
          int filter_stride_col = 1;
          int filter_size_col   = filter_width_truncated * input_depth;
          int filter_stride_row = filter_width * input_depth;
          int filter_size_row   = filter_height_truncated;
          int filter_stride_vec = filter_height * filter_stride_row;
          int filter_size_vec   = output_depth;

          int input_index_base = sli_mvp_util_offset_nhwc(input_height, input_width, input_depth,
                                                          batch,
                                                          (out_y_min + out_y_offset) * stride_height
                                                          - pad_height
                                                          + dilation_height_factor * filter_y_start,
                                                          out_x_min * stride_width
                                                          - pad_width
                                                          + dilation_width_factor * filter_x_start,
                                                          0);
          int input_stride_col = 1;
          int input_size_col   = filter_width_truncated * input_depth;
          int input_stride_row = input_width * input_depth;
          int input_size_row   = in_y_size;
          int input_stride_vec;
          int input_size_vec;

          if (single_out_x) {
            input_stride_vec = stride_height * input_stride_row;
            input_size_vec   = output_height_truncated;
          } else {
            input_stride_vec = stride_width * input_depth;
            input_size_vec   = output_width_truncated;
          }

          // Condition to pack two reals to use both FMACs in MVP and double throughput.
          const bool use_parallel_mac = (input_index_base     % 2 == 0)
                                        && (input_size_col    % 2 == 0)
                                        && (input_stride_row  % 2 == 0)
                                        && (input_stride_vec  % 2 == 0)
                                        && (filter_index_base % 2 == 0)
                                        && (filter_size_col   % 2 == 0)
                                        && (filter_stride_row % 2 == 0)
                                        && (filter_stride_vec % 2 == 0);
          if (use_parallel_mac) {
            input_size_col    /= 2;
            input_stride_vec  /= 2;
            input_stride_row  /= 2;
            filter_size_col   /= 2;
            filter_stride_vec /= 2;
            filter_stride_row /= 2;
          }

          sli_mvp_pb_begin_program(p);

          // Register allocation:
          //   Constants:
          //     c_accumulator_scaler    R0       (c_... => complex datatype)
          //     c_input_offset_scaled   R1
          //     c_output_offset         R2
          //
          //   Registers used temporarily in different parts:
          //     r_output_scaler_i       R3       (r_... => real datatype)
          //     r_bias_i                R4
          //     r_acc                   R5
          //     r_input_i               R6
          //     r_filter_i              R7
          //     r_output_i              R7       (reuse)
          //
          // Arrays:
          //   Array0  input
          //   Array1  filter
          //   Array2  bias
          //   Array3  scaler
          //   Array4  output
          #if (SL_MVP_OPTIMIZE_SPEED == 1)
          // Use the prescaled input values
          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(0),
                                       (void*)&scaled_input[input_index_base],
                                       use_parallel_mac == true
                                       ? SLI_MVP_DATATYPE_COMPLEX_BINARY16
                                       : SLI_MVP_DATATYPE_BINARY16,
                                       input_size_vec,
                                       input_size_row,
                                       input_size_col,
                                       input_stride_vec,
                                       input_stride_row,
                                       input_stride_col,
                                       &status);
          #else
          // Use unscaled input values
          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(0),
                                       (void*)&input[input_index_base],
                                       use_parallel_mac == true
                                       ? SLI_MVP_DATATYPE_COMPLEX_INT8
                                       : SLI_MVP_DATATYPE_INT8,
                                       input_size_vec,
                                       input_size_row,
                                       input_size_col,
                                       input_stride_vec,
                                       input_stride_row,
                                       input_stride_col,
                                       &status);
          #endif

          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(1),
                                       (void*)&filter[filter_index_base],
                                       use_parallel_mac == true
                                       ? SLI_MVP_DATATYPE_COMPLEX_INT8
                                       : SLI_MVP_DATATYPE_INT8,
                                       filter_size_vec,
                                       filter_size_row,
                                       filter_size_col,
                                       filter_stride_vec,
                                       filter_stride_row,
                                       filter_stride_col,
                                       &status);

          sli_mvp_pb_config_vector(p->p,
                                   SLI_MVP_ARRAY(2),
                                   bias != NULL ? (void*)bias : (void*)&zero,
                                   SLI_MVP_DATATYPE_BINARY16,
                                   bias != NULL ? output_depth : 1,
                                   &status);

          sli_mvp_pb_config_vector(p->p,
                                   SLI_MVP_ARRAY(3),
                                   (void*)output_scaler,
                                   SLI_MVP_DATATYPE_BINARY16,
                                   output_depth,
                                   &status);

          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(4),
                                       &output[output_index_base],
                                       SLI_MVP_DATATYPE_INT8,
                                       output_size_vec,
                                       output_size_row,
                                       output_size_col,
                                       output_stride_vec,
                                       output_stride_row,
                                       output_stride_col,
                                       &status);

          sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER);
          if (use_parallel_mac) {
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, input_offset_scaled, input_offset_scaled);
          } else {
            sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R1, input_offset_scaled);
          }
          sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R2, (float16_t)output_offset);

          sli_mvp_pb_begin_loop(p, output_depth, &status); {

            // LOAD(ARRAY2, R4)    Bias
            // LOAD(ARRAY3, R3)    Scaler
            // R5 = COPY(R4)       Compute(r_acc, COPY, r_bias_i)
            sli_mvp_pb_compute(p,
                               SLI_MVP_OP(COPY),
                               SLI_MVP_ALU_Z(SLI_MVP_R5)
                               | SLI_MVP_ALU_A(SLI_MVP_R4),
                               SLI_MVP_LOAD(0, SLI_MVP_R4, SLI_MVP_ARRAY(2), SLI_MVP_NOINCR)
                               | SLI_MVP_LOAD(1, SLI_MVP_R3, SLI_MVP_ARRAY(3), SLI_MVP_NOINCR),
                               SLI_MVP_NONE,
                               &status);

            sli_mvp_pb_begin_loop(p, output_width_truncated, &status); {
              sli_mvp_pb_begin_loop(p, out_y_size, &status); {
                sli_mvp_pb_begin_loop(p, filter_height_truncated, &status); {
                  sli_mvp_pb_begin_loop(p, input_size_col, &status); {

                    #if (SL_MVP_OPTIMIZE_SPEED == 1)
                    // Accumulate input * filter

                    // R5 = MAC(R6, R7, R5) Compute(r_acc, MACR2A, r_input_i, r_filter_i, r_acc)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACR2A),
                                       SLI_MVP_ALU_Z(SLI_MVP_R5)
                                       | SLI_MVP_ALU_X(SLI_MVP_R6)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R7)
                                       | SLI_MVP_ALU_A(SLI_MVP_R5),
                                       SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL)
                                       | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                                       SLI_MVP_NONE,
                                       &status);

                    #else // SL_MVP_OPTIMIZE_SPEED == 0

                    // 1. Scale the inputs by accumulator scaler and offset
                    // 2. Accumulate input * filter

                    // LOAD(ARRAY0, R6)      Input
                    // LOAD(ARRAY1, R7)      Filter
                    // R6 = MACC(R6, R0, R1) Compute(r_input_i, MACC, r_input_i, c_accumulator_scaler, c_input_offset_scaled)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACC),
                                       SLI_MVP_ALU_Z(SLI_MVP_R6)
                                       | SLI_MVP_ALU_X(SLI_MVP_R6)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R0)
                                       | SLI_MVP_ALU_A(SLI_MVP_R1),
                                       SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL)
                                       | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                                       SLI_MVP_NONE,
                                       &status);

                    // R5 = MAC(R6, R7, R5) Compute(r_acc, MACR2A, r_input_i, r_filter_i, r_acc)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACR2A),
                                       SLI_MVP_ALU_Z(SLI_MVP_R5)
                                       | SLI_MVP_ALU_X(SLI_MVP_R6)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R7)
                                       | SLI_MVP_ALU_A(SLI_MVP_R5),
                                       SLI_MVP_NONE,
                                       SLI_MVP_NONE,
                                       &status);
                    #endif
                  }
                  sli_mvp_pb_end_loop(p);          // input_size_col
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_ROW);
                }
                sli_mvp_pb_end_loop(p);        // filter_height_truncated
                if (single_out_x) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
                }

                int input_row_incr_left = in_y_extra_incr_adjusted;
                if (input_row_incr_left-- > 0) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }

                if (use_parallel_mac) {
                  // r_acc = r_acc.imag + r_acc.real
                  // R5 = ADDR(R5)       Compute(r_acc, ADDR, r_acc)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(ADDR),
                                     SLI_MVP_ALU_Z(SLI_MVP_R5)
                                     | SLI_MVP_ALU_A(SLI_MVP_R5),
                                     input_row_incr_left-- > 0
                                     ? SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW)
                                     : SLI_MVP_NONE,
                                     SLI_MVP_NONE,
                                     &status);
                }

                // R7 = MAC(R5, R3, R2) Compute(r_output_i, MACC, r_acc, r_output_scaler_i, c_output_offset)
                // STORE(ARRAY4, R7)  Output
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(MACC),
                                   SLI_MVP_ALU_Z(SLI_MVP_R7)
                                   | SLI_MVP_ALU_X(SLI_MVP_R5)
                                   | SLI_MVP_ALU_Y(SLI_MVP_R3)
                                   | SLI_MVP_ALU_A(SLI_MVP_R2),
                                   input_row_incr_left-- > 0
                                   ? SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW)
                                   : SLI_MVP_NONE,
                                   SLI_MVP_STORE(SLI_MVP_R7, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM_VEC),
                                   &status);

                // Handle special case where need to increment out_y
                // one piece at a time due to stride limits.
                if (out_y_extra_incr > 0) {
                  sli_mvp_pb_begin_loop(p, out_y_extra_incr, &status); {
                    // NOOP used for loop incrementing output VEC array index by dummy load of R7.
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(NOOP),
                                       SLI_MVP_NONE,
                                       SLI_MVP_LOAD(0, SLI_MVP_R7, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM_VEC),
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p);
                }

                // R5 = COPY(R4)       Compute(r_acc, COPY, r_bias_i)
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(COPY),
                                   SLI_MVP_ALU_Z(SLI_MVP_R5)
                                   | SLI_MVP_ALU_A(SLI_MVP_R4),
                                   SLI_MVP_NONE,
                                   SLI_MVP_NONE,
                                   &status);

                // Handle any remaining input rows still needing to increment
                if (input_row_incr_left == 1) {
                  sli_mvp_pb_begin_loop(p, 1, &status); {
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(NOOP),
                                       SLI_MVP_NONE,
                                       SLI_MVP_NONE,
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p);
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                } else if (input_row_incr_left > 1) {
                  sli_mvp_pb_begin_loop(p, input_row_incr_left - 1, &status); {
                    sli_mvp_pb_begin_loop(p, 1, &status); {
                      sli_mvp_pb_compute(p,
                                         SLI_MVP_OP(NOOP),
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         &status);
                    }
                    sli_mvp_pb_end_loop(p);
                    sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                  }
                  sli_mvp_pb_end_loop(p);
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }
              }
              sli_mvp_pb_end_loop(p);      // out_y_size
              sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM_ROW);
              if (!single_out_x) {
                sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
              }
            }
            sli_mvp_pb_end_loop(p);    // output_width_truncated
            sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM_COL);
            sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC);
            sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_COL);
            sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_COL);

          }
          sli_mvp_pb_end_loop(p);  // output_depth

          // Check if any errors found during program generation.
          if (status != SL_STATUS_OK) {
            if (execute) {
              EFM_ASSERT(false);
            }
            return status;
          }

          if (execute) {
            sli_mvp_pb_execute_program(p);
          }
        } // batches
      } // out_y_offset
    } // out_y_range
  } // out_x_range

  if (execute) {
    sli_mvp_wait_for_completion();
    sli_mvp_math_clamp_i8(params->output,
                          batches * output_height * output_width * output_depth,
                          params->output_activation_min,
                          params->output_activation_max);
  }

  return SL_STATUS_OK;
}

/// @endcond
