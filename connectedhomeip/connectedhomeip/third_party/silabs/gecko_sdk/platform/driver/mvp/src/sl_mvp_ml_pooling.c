/***************************************************************************//**
 * @file
 * @brief MVP pooling kernel driver.
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

#include "sl_mvp_ml_pooling.h"
#include "sl_mvp.h"
#include "sl_mvp_util.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#define SLI_MVP_CHECK(x)                \
  do {                                  \
    if (!(x)) {                         \
      status = SL_STATUS_INVALID_RANGE; \
    }                                   \
  } while (0)

static sl_status_t avg_pool(const sli_mvp_ml_pooling_s8_params_t *params, bool execute);
static sl_status_t avg_pool_opt(const sli_mvp_ml_pooling_s8_params_t *params, bool execute);
static sl_status_t max_pool(const sli_mvp_ml_pooling_s8_params_t *params, bool execute);

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
 * Average pooling.
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_average_pooling_s8(const sli_mvp_ml_pooling_s8_params_t *params)
{
  sl_status_t ret = SL_STATUS_FAIL;

  // Check if the optimized algorithm can be used.
  bool opt_supported = avg_pool_opt(params, false) == SL_STATUS_OK;

  if (((params->output_width * params->output_height)
       > (((9 * params->filter_width * params->filter_height * params->channels) + 5) / 10))
      || !opt_supported) {
    // We prefer the generic algorithm for efficiency, OR we must use it because
    // the optimized algorithm does not support this configuration.
    ret = avg_pool(params, true);
  }

  if ((ret != SL_STATUS_OK) && opt_supported) {
    ret = avg_pool_opt(params, true);
  }

  if (ret != SL_STATUS_OK) {
    EFM_ASSERT(false);
  }

  return ret;
}

/***************************************************************************//**
 *
 * Average pooling with explicit algorithm. (For unit testing only).
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_average_pooling_s8_test(const sli_mvp_ml_pooling_s8_params_t *params,
                                               bool force_optimized)
{
  if (force_optimized) {
    return avg_pool_opt(params, true);
  }
  return avg_pool(params, true);
}

/***************************************************************************//**
 *
 * Max pooling.
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_max_pooling_s8(const sli_mvp_ml_pooling_s8_params_t *params)
{
  return max_pool(params, true);
}

/***************************************************************************//**
 *
 * Check if MVP supports average pooling on given matrix.
 *
 ******************************************************************************/
bool sli_mvp_ml_average_pooling_s8_is_supported(const sli_mvp_ml_pooling_s8_params_t *params)
{
  // Check if optimized algorithm can be used.
  if (avg_pool_opt(params, false) == SL_STATUS_OK) {
    return true;
  }
  // Resort to the generic algorithm.
  return avg_pool(params, false) == SL_STATUS_OK;
}

/***************************************************************************//**
 *
 * Check if MVP supports max pooling on given matrix.
 *
 ******************************************************************************/
bool sli_mvp_ml_max_pooling_s8_is_supported(const sli_mvp_ml_pooling_s8_params_t *params)
{
  return max_pool(params, false) == SL_STATUS_OK;
}

static sl_status_t avg_pool_opt(const sli_mvp_ml_pooling_s8_params_t *params, bool execute)
{
  const int batches            = params->batches;
  const int depth              = params->channels;
  const int input_height       = params->input_height;
  const int input_width        = params->input_width;
  const int output_height      = params->output_height;
  const int output_width       = params->output_width;
  const int pad_width          = params->pad_width;
  const int pad_height         = params->pad_height;
  const bool needs_padding     = params->padding;
  const int stride_height      = params->stride_height;
  const int stride_width       = params->stride_width;
  const int filter_width       = params->filter_width;
  const int filter_height      = params->filter_height;
  const int32_t activation_max = params->output_activation_max;
  const int32_t activation_min = params->output_activation_min;
  const int8_t *input          = params->input;
  int8_t *output               = params->output;

  sl_status_t status           = SL_STATUS_OK;
  sli_mvp_program_context_t *p = sli_mvp_get_program_area_context();

  const int parallel = (depth % 2) == 0 ? 2 : 1;
  const int x_stride = depth / parallel;
  const int input_y_stride = (input_width * depth) / parallel;
  const int output_y_stride = (output_width * depth) / parallel;

  if (needs_padding == false) {
    if ((pad_width != 0) || (pad_height != 0)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Check tensor alignment.
  if ((parallel == 2) && execute) {
    // Tensor buffers must be 2-byte aligned when processing two values at a
    // time with the complex int8 datatype.
    if ((((uint32_t)input & 0x1) != 0U) || (((uint32_t)output & 0x1) != 0U)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Early exit.
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_mvp_pb_init_program(p);

  for (int batch = 0; batch < batches; ++batch) {
    for (int out_y = 0; out_y < output_height; ++out_y) {
      for (int out_x = 0; out_x < output_width; ++out_x) {
        const int in_x_origin = (out_x * stride_width) - pad_width;
        const int in_y_origin = (out_y * stride_height) - pad_height;

        // Compute the boundaries of the filter region clamped so as to
        // ensure that the filter window fits in the input array.
        const int filter_x_start = SL_MAX(0, -in_x_origin);
        const int filter_x_end   = SL_MIN(filter_width, input_width - in_x_origin);
        const int filter_y_start = SL_MAX(0, -in_y_origin);
        const int filter_y_end   = SL_MIN(filter_height, input_height - in_y_origin);

        const int input_offset = sli_mvp_util_offset_nhwc(input_height, input_width, depth,
                                                          batch,
                                                          in_y_origin + filter_y_start,
                                                          in_x_origin + filter_x_start,
                                                          0 /* channel */);
        const int output_offset = sli_mvp_util_offset_nhwc(output_height, output_width, depth,
                                                           batch,
                                                           out_y,
                                                           out_x,
                                                           0 /* channel */);

        const int filter_x_count = filter_x_end - filter_x_start;
        const int filter_y_count = filter_y_end - filter_y_start;
        const float16_t final_scale_factor = SLI_MVP_ACCUMULATOR_MULTIPLIER
                                             / (filter_x_count * filter_y_count);

        // Register allocation:
        //   Constants:
        //     c_final_scale_factor    R0       (r_... => real datatype)
        //     c_accumulator_scaler    R1       (c_... => complex datatype)
        //
        //   Registers used temporarily in different parts:
        //     r_acc                   R2
        //     r_input_i               R3
        //     r_activation_min        R4
        //     r_activation_max        R5
        //     r_tmp                   R6
        //
        // Arrays:
        //   Array0  input
        //   Array1  output

        sli_mvp_pb_begin_program(p);

        // Set up input array
        sli_mvp_pb_config_array_full(p->p,
                                     SLI_MVP_ARRAY(0),
                                     (void*)&input[input_offset],    // addr
                                     (parallel == 2)
                                     ? SLI_MVP_DATATYPE_COMPLEX_INT8 // datatype
                                     : SLI_MVP_DATATYPE_INT8,
                                     depth / parallel,               // vecs
                                     input_height,                   // rows
                                     input_width,                    // cols
                                     1,                              // vecstride, distance to next "channel"
                                     input_y_stride,                 // rowstride, distance to next "y"
                                     x_stride,                       // colstride, distance to next "x"
                                     &status);

        // Set up output array
        sli_mvp_pb_config_array_full(p->p,
                                     SLI_MVP_ARRAY(1),
                                     (void*)&output[output_offset],  // addr
                                     (parallel == 2)
                                     ? SLI_MVP_DATATYPE_COMPLEX_INT8 // datatype
                                     : SLI_MVP_DATATYPE_INT8,
                                     depth / parallel,               // vecs
                                     output_height,                  // rows
                                     output_width,                   // cols
                                     1,                              // vecstride, distance to next "channel"
                                     output_y_stride,                // rowstride, distance to next "y"
                                     x_stride,                       // colstride, distance to next "x"
                                     &status);

        sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R0, final_scale_factor, final_scale_factor);
        sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, SLI_MVP_ACCUMULATOR_SCALER, SLI_MVP_ACCUMULATOR_SCALER);
        bool do_activation = (activation_min != -128) || (activation_max != 127);
        if (do_activation) {
          sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R4, (float16_t)activation_min, (float16_t)activation_min);
          sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R5, (float16_t)activation_max, (float16_t)activation_max);
        }

        sli_mvp_pb_begin_loop(p, depth / parallel, &status); {  // Loop over channels.

          // R2 = CLEAR() r_acc = 0
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(CLEAR),
                             SLI_MVP_ALU_Z(SLI_MVP_R2),
                             SLI_MVP_NONE,
                             SLI_MVP_NONE,
                             &status);

          sli_mvp_pb_begin_loop(p, filter_y_count, &status); {    // Loop over filter height
            sli_mvp_pb_begin_loop(p, filter_x_count, &status); {  // Loop over filter width

              // Accumulate input tensor values covered by filter.
              // LOAD(ARRAY0, R3)
              // R2 = MACR2A(R3, R1, R2) r_acc = (r_input_i * c_accumulator_scaler) + r_acc
              sli_mvp_pb_compute(p,
                                 SLI_MVP_OP(MACR2A),
                                 SLI_MVP_ALU_Z(SLI_MVP_R2)
                                 | SLI_MVP_ALU_X(SLI_MVP_R3)
                                 | SLI_MVP_ALU_Y(SLI_MVP_R1)
                                 | SLI_MVP_ALU_A(SLI_MVP_R2),
                                 SLI_MVP_LOAD(0, SLI_MVP_R3, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                                 SLI_MVP_NONE,
                                 &status);
            }
            sli_mvp_pb_end_loop(p);      // Filter width
            sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
            sli_mvp_pb_postloop_reset_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_RESETDIM_COL);
          }
          sli_mvp_pb_end_loop(p);        // Filter height
          sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
          sli_mvp_pb_postloop_reset_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_RESETDIM_ROW);

          // Divide by filter size to get average.
          // R2 = MULR2A(R2, R0) r_acc = r_acc * c_final_scale_factor
          // STORE(ARRAY1, R2)
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(MULR2A),
                             SLI_MVP_ALU_Z(SLI_MVP_R2)
                             | SLI_MVP_ALU_X(SLI_MVP_R2)
                             | SLI_MVP_ALU_Y(SLI_MVP_R0),
                             SLI_MVP_NONE,
                             do_activation
                             ? SLI_MVP_NONE
                             : SLI_MVP_STORE(SLI_MVP_R2, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                             &status);
          if (do_activation) {
            // R6 = CLIP2A(R4, R5, R2) r_tmp = CLIP(r_acc, r_act_min, r_act_max)
            // STORE(ARRAY1, R6)
            sli_mvp_pb_compute(p,
                               SLI_MVP_OP(CLIP2A),
                               SLI_MVP_ALU_Z(SLI_MVP_R6)
                               | SLI_MVP_ALU_X(SLI_MVP_R4)
                               | SLI_MVP_ALU_Y(SLI_MVP_R5)
                               | SLI_MVP_ALU_A(SLI_MVP_R2),
                               SLI_MVP_NONE,
                               SLI_MVP_STORE(SLI_MVP_R6, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                               &status);
          }
        }
        sli_mvp_pb_end_loop(p);  // Channels

        // Check if any errors found during program generation.
        if (status != SL_STATUS_OK) {
          return status;
        }

        // Execute the program
        if (execute) {
          sli_mvp_pb_execute_program(p);
        }
      }
    }
  }
  sli_mvp_wait_for_completion();

  return SL_STATUS_OK;
}

static sl_status_t avg_pool(const sli_mvp_ml_pooling_s8_params_t *params, bool execute)
{
  // Consume all input parameters.
  const int batches            = params->batches;
  const int depth              = params->channels;
  const int input_height       = params->input_height;
  const int input_width        = params->input_width;
  const int output_height      = params->output_height;
  const int output_width       = params->output_width;
  const int pad_width          = params->pad_width;
  const int pad_height         = params->pad_height;
  const bool needs_padding     = params->padding;
  const int stride_height      = params->stride_height;
  const int stride_width       = params->stride_width;
  const int filter_width       = params->filter_width;
  const int filter_height      = params->filter_height;
  const int32_t activation_max = params->output_activation_max;
  const int32_t activation_min = params->output_activation_min;
  const int8_t *input          = params->input;
  int8_t *output               = params->output;

  sl_status_t status           = SL_STATUS_OK;
  sli_mvp_program_context_t *p = sli_mvp_get_program_area_context();
  bool parallelize_channels    = (depth % 2) == 0;

  if (needs_padding == false) {
    if ((pad_width != 0) || (pad_height != 0)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  if (parallelize_channels && execute) {
    // Tensor buffers must be 2-byte aligned when processing two values at a
    // time with the complex int8 datatype.
    if ((((uint32_t)input & 0x1) != 0U) || (((uint32_t)output & 0x1) != 0U)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Early exit.
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_mvp_pb_init_program(p);

  // Iterate over possible sub-filters that make up the edge boundary cases
  // and record the out_x, out_y ranges and filter ranges that correspond to
  // each case.
  // Doesn't actually loop through each index, but only those that correspond to
  // unique sub-filters.
  const int in_y_origin_center_max = input_height - filter_height;
  const int out_y_center_max = sli_div_floor_int(in_y_origin_center_max + pad_height, stride_height);
  const int in_x_origin_center_max = input_width - filter_width;
  const int out_x_center_max = sli_div_floor_int(in_x_origin_center_max + pad_width, stride_width);

  for (int out_x_min = 0, out_x_max; out_x_min < output_width; out_x_min = out_x_max + 1) {
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

    // When there is only one column, then don't need to use a dimension to increment
    // across it. Instead, can use that dimension for row striding and handle entire
    // column in single program.
    bool single_out_x = output_width_truncated <= 1;
    // However, this mechanism leads to longer strides and so disabling here in those
    // cases where later stride computation would be too large.
    SLI_MVP_CHECK((stride_width * depth) <= (int)SLI_MVP_MAX_VECTOR_STRIDE);
    if ((stride_height * input_width * depth) > (int)SLI_MVP_MAX_VECTOR_STRIDE) {
      single_out_x = false;
    }

    for (int out_y_min = 0, out_y_max; out_y_min < output_height; out_y_min = out_y_max + 1) {
      const int in_y_origin_min = (out_y_min * stride_height) - pad_height;
      const int filter_y_start = SL_MAX(0, -in_y_origin_min);
      const int filter_y_end   = SL_MIN(filter_height, input_height - in_y_origin_min);
      const int filter_height_truncated = filter_y_end - filter_y_start;
      const int filter_count = filter_width_truncated * filter_height_truncated;

      // Check that filter count is in range such that the final scale factor
      // at least keeps full float16_t precision.
      const float16_t final_scale_factor = SLI_MVP_ACCUMULATOR_MULTIPLIER / filter_count;
      SLI_MVP_CHECK(final_scale_factor >= SLI_MVP_FP16_MIN);
      SLI_MVP_CHECK(final_scale_factor <= SLI_MVP_FP16_MAX);

      // We currently don't support large filters.
      // For large filter count, especially with large-magnitude average values,
      // the results will start to become very innaccurate due to accumulator becoming
      // much larger than inputs.
      SLI_MVP_CHECK(filter_count <= 512);

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

      // Special out_y iteration computations
      int in_y_extra_incr;
      int out_y_incr;
      if (output_height_truncated == 1 || single_out_x) {
        // Trivial case.  need no group size calculation
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
                                                             // next stride height multiple
          out_y_incr = div_quot + 1;  // Ceiling
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
        int in_y_extra_incr_adjusted = out_y_size == 1 ? 0 : in_y_extra_incr;

        // Could just set in_y_size to input_height, but it may be possible to
        // enable more programs with a tigher bound on the number of elements.
        int in_y_size;
        if (single_out_x) {
          in_y_size = filter_height_truncated;
          SLI_MVP_CHECK(in_y_extra_incr_adjusted == 0);
        } else {
          // Set in_y_size to exact value needed for rollover, to save MVP code space by using a Reset
          in_y_size = (filter_height_truncated + in_y_extra_incr_adjusted) * out_y_size;
        }

        // Ideally, at this point (or in above loops) would determine:
        //  * if all dimensions fit within MVP.  otherwise split into chunks that do fit
        //  * if problem is too small for MVP, i.e., there is more CPU overhead
        //  * launching MVP than just computing outright, the skip mvp.

        // No more dimensions to include batch loop in program, but it is generally just 1 anyway
        for (int batch = 0; batch < batches; ++batch) {
          // Not enough input dimensions for including channel loop in the program
          // If input stored with channel as first dimension then would help
          int parallelization = 1;
          for (int channel = 0; channel < depth; channel += parallelization) {
            // These array indexing computations should be moved up to highest loop
            // level possible so uncompiled operation is more efficient.
            int output_index_base = sli_mvp_util_offset_nhwc(output_height, output_width, depth,
                                                             batch,
                                                             out_y_min + out_y_offset,
                                                             out_x_min,
                                                             channel);

            int output_stride_col = 0;
            int output_size_col   = 1;                    // Unused dimension
            int output_stride_row = depth;
            int output_size_row   = output_width_truncated;
            int output_stride_vec = out_y_incr * output_width * depth;
            int out_y_extra_incr  = 0;

            if (output_stride_vec > (int)SLI_MVP_MAX_VECTOR_STRIDE) {
              output_stride_vec = output_width * depth;
              out_y_extra_incr = out_y_incr - 1;
            }
            int output_size_vec = out_y_size * (out_y_extra_incr + 1);

            SLI_MVP_CHECK(output_index_base >= 0);
            SLI_MVP_CHECK(output_stride_col >= 0);
            SLI_MVP_CHECK(output_stride_row >= 0);
            SLI_MVP_CHECK(output_stride_vec >= 0);

            int input_index_base = sli_mvp_util_offset_nhwc(input_height, input_width, depth,
                                                            batch,
                                                            (out_y_min + out_y_offset) * stride_height - pad_height + filter_y_start,
                                                            out_x_min * stride_width - pad_width + filter_x_start,
                                                            channel);

            int input_stride_col = depth;
            int input_size_col   = filter_width_truncated;
            int input_stride_row = input_width * depth;
            int input_size_row   = in_y_size;
            int input_stride_vec;
            int input_size_vec;

            if (single_out_x) {
              input_stride_vec = stride_height * input_stride_row;
              input_size_vec   = output_height_truncated;
            } else {
              input_stride_vec = stride_width * depth;
              input_size_vec   = output_width_truncated;
            }

            // If there is at least one more channel left, then group it with this one and do 2 at a time in parallel
            // In general, the paralellization works if depth is even, but the full expression for
            //  testing is included below for completeness
            if (parallelize_channels) {
              SLI_MVP_CHECK((channel + 1) < depth);
              SLI_MVP_CHECK(output_index_base % 2 == 0);
              SLI_MVP_CHECK(input_index_base  % 2 == 0);
              SLI_MVP_CHECK(input_stride_col  % 2 == 0);
              SLI_MVP_CHECK(input_stride_row  % 2 == 0);
              SLI_MVP_CHECK(input_stride_vec  % 2 == 0);
              SLI_MVP_CHECK(output_stride_col % 2 == 0);
              SLI_MVP_CHECK(output_stride_row % 2 == 0);
              SLI_MVP_CHECK(output_stride_vec % 2 == 0);
            }
            parallelization = parallelize_channels ? 2 : 1;
            // Hardware strides are in terms of base unit, so must update strides
            // to reflect base unit scaling by parallelization.
            input_stride_col  /= parallelization;
            input_stride_row  /= parallelization;
            input_stride_vec  /= parallelization;
            output_stride_col /= parallelization;
            output_stride_row /= parallelization;
            output_stride_vec /= parallelization;

            sli_mvp_pb_begin_program(p);

            // Register allocation:
            //   Constants:
            //     c_final_scale_factor    R0       (r_... => real datatype)
            //     c_accumulator_scaler    R1       (c_... => complex datatype)
            //
            //   Registers used temporarily in different parts:
            //     r_acc                   R2
            //     r_input_i               R3
            //     r_activation_min        R4
            //     r_activation_max        R5
            //     r_tmp                   R6
            //
            // Arrays:
            //   Array0  input
            //   Array1  output

            // Set up input array
            sli_mvp_pb_config_array_full(p->p,
                                         SLI_MVP_ARRAY(0),
                                         (void*)&input[input_index_base],   // addr
                                         parallelize_channels
                                         ? SLI_MVP_DATATYPE_COMPLEX_INT8    // datatype
                                         : SLI_MVP_DATATYPE_INT8,
                                         input_size_vec,                    // vecs
                                         input_size_row,                    // rows
                                         input_size_col,                    // cols
                                         input_stride_vec,                  // vecstride
                                         input_stride_row,                  // rowstride
                                         input_stride_col,                  // colstride
                                         &status);

            // Set up output array
            sli_mvp_pb_config_array_full(p->p,
                                         SLI_MVP_ARRAY(1),
                                         (void*)&output[output_index_base], // addr
                                         parallelize_channels
                                         ? SLI_MVP_DATATYPE_COMPLEX_INT8    // datatype
                                         : SLI_MVP_DATATYPE_INT8,
                                         output_size_vec,                   // vecs
                                         output_size_row,                   // rows
                                         output_size_col,                   // cols
                                         output_stride_vec,                 // vecstride
                                         output_stride_row,                 // rowstride
                                         output_stride_col,                 // colstride
                                         &status);

            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R0, final_scale_factor, final_scale_factor);
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, SLI_MVP_ACCUMULATOR_SCALER, SLI_MVP_ACCUMULATOR_SCALER);

            bool do_activation = (activation_min != -128) || (activation_max != 127);
            if (do_activation) {
              sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R4, (float16_t)activation_min, (float16_t)activation_min);
              sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R5, (float16_t)activation_max, (float16_t)activation_max);
            }

            sli_mvp_pb_begin_loop(p, output_width_truncated, &status); {
              sli_mvp_pb_begin_loop(p, out_y_size, &status); {
                // R2 = CLEAR()
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(CLEAR),
                                   SLI_MVP_ALU_Z(SLI_MVP_R2),
                                   SLI_MVP_NONE,
                                   SLI_MVP_NONE,
                                   &status);
                sli_mvp_pb_begin_loop(p, filter_height_truncated, &status); {
                  sli_mvp_pb_begin_loop(p, input_size_col, &status); {
                    // R2 = MAC2RA(R3, R1, R2)
                    // LOAD(ARRAY0, R3)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACR2A),
                                       SLI_MVP_ALU_Z(SLI_MVP_R2)
                                       | SLI_MVP_ALU_X(SLI_MVP_R3)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R1)
                                       | SLI_MVP_ALU_A(SLI_MVP_R2),
                                       SLI_MVP_LOAD(0, SLI_MVP_R3, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p);                    // input_size_col
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }
                sli_mvp_pb_end_loop(p);                    // filter_height_truncated

                if (single_out_x) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
                }

                int input_row_incr_left = in_y_extra_incr_adjusted;
                if (input_row_incr_left-- > 0) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }

                if (input_row_incr_left-- > 0) {     // use one of the available load streams of next op
                  // LOAD(ARRAY0, R3)
                  // R2 = MULR2A(R2, R0)
                  // STORE(ARRAY1, R2)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(MULR2A),
                                     SLI_MVP_ALU_Z(SLI_MVP_R2)
                                     | SLI_MVP_ALU_X(SLI_MVP_R2)
                                     | SLI_MVP_ALU_Y(SLI_MVP_R0),
                                     SLI_MVP_LOAD(0, SLI_MVP_R3, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW),
                                     do_activation
                                     ? SLI_MVP_NONE
                                     : SLI_MVP_STORE(SLI_MVP_R2, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                                     &status);
                } else {
                  // R2 = MULR2A(R2, R0)
                  // STORE(ARRAY1, R2)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(MULR2A),
                                     SLI_MVP_ALU_Z(SLI_MVP_R2)
                                     | SLI_MVP_ALU_X(SLI_MVP_R2)
                                     | SLI_MVP_ALU_Y(SLI_MVP_R0),
                                     SLI_MVP_NONE,
                                     do_activation
                                     ? SLI_MVP_NONE
                                     : SLI_MVP_STORE(SLI_MVP_R2, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                                     &status);
                }

                if (do_activation) {
                  // R6 = CLIP2A(R4, R5, R2)
                  // STORE(ARRAY1, R6)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(CLIP2A),
                                     SLI_MVP_ALU_Z(SLI_MVP_R6)
                                     | SLI_MVP_ALU_X(SLI_MVP_R4)
                                     | SLI_MVP_ALU_Y(SLI_MVP_R5)
                                     | SLI_MVP_ALU_A(SLI_MVP_R2),
                                     SLI_MVP_NONE,
                                     SLI_MVP_STORE(SLI_MVP_R6, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                                     &status);
                }

                // Handle special case where need to increment out_y
                // one piece at a time due to stride limits.
                if (out_y_extra_incr > 0) {
                  sli_mvp_pb_begin_loop(p, out_y_extra_incr, &status); {
                    sli_mvp_pb_begin_loop(p, 1, &status); {
                      sli_mvp_pb_compute(p,
                                         SLI_MVP_OP(NOOP),
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         &status);
                    }
                    sli_mvp_pb_end_loop(p);
                    sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC);
                  }
                  sli_mvp_pb_end_loop(p);
                }

                // Handle any remaining input rows still needing to increment
                if (input_row_incr_left <= 0) {
                } else if (input_row_incr_left == 1) {
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
                } else {
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
              sli_mvp_pb_end_loop(p);                    // out_y_size
              sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_ROW);

              if (!single_out_x) {
                sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
              }
            }
            sli_mvp_pb_end_loop(p);                    // output_width_truncated

            // Check if any errors found during program generation.
            if (status != SL_STATUS_OK) {
              return status;
            }

            // Execute the program
            if (execute) {
              sli_mvp_pb_execute_program(p);
            }
          } // channels
        } // batches
      } // out_y_offset
    } // out_y_range
  } // out_x_range

  if (execute) {
    sli_mvp_wait_for_completion();
  }

  return status;
}

static sl_status_t max_pool(const sli_mvp_ml_pooling_s8_params_t *params, bool execute)
{
  // Consume all input parameters.
  const int batches            = params->batches;
  const int depth              = params->channels;
  const int input_height       = params->input_height;
  const int input_width        = params->input_width;
  const int output_height      = params->output_height;
  const int output_width       = params->output_width;
  const int pad_width          = params->pad_width;
  const int pad_height         = params->pad_height;
  const bool needs_padding     = params->padding;
  const int stride_height      = params->stride_height;
  const int stride_width       = params->stride_width;
  const int filter_width       = params->filter_width;
  const int filter_height      = params->filter_height;
  const int32_t activation_max = params->output_activation_max;
  const int32_t activation_min = params->output_activation_min;
  const int8_t *input          = params->input;
  int8_t *output               = params->output;

  sl_status_t status           = SL_STATUS_OK;
  sli_mvp_program_context_t *p = sli_mvp_get_program_area_context();

  if (needs_padding == false) {
    if ((pad_width != 0) || (pad_height != 0)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  if (execute) {
    // Tensor buffers must be 2-byte aligned when processing two values at a
    // time with the complex int8 datatype.
    if ((((uint32_t)input & 0x1) != 0U) || (((uint32_t)output & 0x1) != 0U)) {
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

  // Iterate over possible sub-filters that make up the edge boundary cases
  // and record the out_x, out_y ranges and filter ranges that correspond to
  // each case.
  // Doesn't actually loop through each index, but only those that correspond to
  // unique sub-filters.
  const int in_y_origin_center_max = input_height - filter_height;
  const int out_y_center_max = sli_div_floor_int(in_y_origin_center_max + pad_height, stride_height);
  const int in_x_origin_center_max = input_width - filter_width;
  const int out_x_center_max = sli_div_floor_int(in_x_origin_center_max + pad_width, stride_width);

  for (int out_x_min = 0, out_x_max; out_x_min < output_width; out_x_min = out_x_max + 1) {
    const int in_x_origin_min = (out_x_min * stride_width) - pad_width;
    const int filter_x_start = SL_MAX(0, -in_x_origin_min);
    const int filter_x_end   = SL_MIN(filter_width, input_width - in_x_origin_min);
    const int filter_width_truncated = filter_x_end - filter_x_start;

    if (in_x_origin_min < 0) { // Case 1.  Each out_x leads to 1 filter_x_start
      out_x_max = out_x_min;   // (and 1 or more filter_x_end)
    } else {                   // Case 2.  true for the rest of the out_x
      if (input_width - in_x_origin_min >= filter_width) { // Case 2a. true for multiple out_x
        out_x_max = out_x_center_max;
      } else {                 // Case 2b. each out_x leads to 1 filter_x_end
        out_x_max = out_x_min;
      }
    }
    const int output_width_truncated = out_x_max - out_x_min + 1;

    // When there is only one column, we don't need to use a dimension to
    // increment across it. Instead, use that dimension for row striding
    // and handle entire column in single program.
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

      // Special out_y iteration computations
      int in_y_extra_incr, out_y_incr;
      if (output_height_truncated == 1 || single_out_x) {
        // Trivial case. Need no group size calculation
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
          out_y_incr = div_quot + 1;  // Ceiling                next stride height multiple
          in_y_extra_incr = stride_height - div_rem;
        }
      }

      int in_y_group_size = filter_height_truncated + in_y_extra_incr;
      if ((output_height_truncated > 1) && !single_out_x) {
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
        // Don't need to worry about the increments if will never go to next output
        int in_y_extra_incr_adjusted = out_y_size == 1 ? 0 : in_y_extra_incr;

        // Could just set in_y_size to input_height, but it may be possible to
        // enable more programs with a tighter bound on the number of elements.
        int in_y_size;
        if (single_out_x) {
          in_y_size = filter_height_truncated;
          SLI_MVP_CHECK(in_y_extra_incr_adjusted == 0);
        } else {
          // Set in_y_size to exact value needed for rollover, to save MVP code space by using a Reset
          in_y_size = (filter_height_truncated + in_y_extra_incr_adjusted) * out_y_size;
        }

        // Ideally, at this point (or in above loops) would determine:
        //  * If all dimensions fit within MVP. Otherwise split into chunks that do fit
        //  * If problem is too small for MVP, i.e., there is more CPU overhead
        //  *   launching MVP than just computing outright, then skip mvp.

        // No more dimensions to include batch loop in program, but it is generally just 1 anyway
        for (int batch = 0; batch < batches; ++batch) {
          // Not enough input dimensions for including channel loop in the program
          // If input stored with channel as first dimension then would help
          int parallelization = 1;
          for (int channel = 0; channel < depth; channel += parallelization) {
            // These array indexing computations should be moved up to highest loop
            // level possible so operation is more efficient.
            int output_index_base = sli_mvp_util_offset_nhwc(output_height, output_width, depth,
                                                             batch, out_y_min + out_y_offset, out_x_min, channel);
            int output_stride_col = 0;
            int output_size_col   = 1;    // unused dimension
            int output_stride_row = depth;
            int output_size_row   = output_width_truncated;
            int output_stride_vec = out_y_incr * output_width * depth;
            int out_y_extra_incr  = 0;

            if (output_stride_vec > (int)SLI_MVP_MAX_VECTOR_STRIDE) {
              output_stride_vec = output_width * depth;
              out_y_extra_incr = out_y_incr - 1;
            }
            int output_size_vec = out_y_size * (out_y_extra_incr + 1);

            SLI_MVP_CHECK(output_index_base >= 0);
            SLI_MVP_CHECK(output_stride_col >= 0);
            SLI_MVP_CHECK(output_stride_row >= 0);
            SLI_MVP_CHECK(output_stride_vec >= 0);

            int input_index_base = sli_mvp_util_offset_nhwc(input_height, input_width, depth,
                                                            batch,
                                                            (out_y_min + out_y_offset) * stride_height - pad_height + filter_y_start,
                                                            out_x_min * stride_width - pad_width + filter_x_start,
                                                            channel);

            int input_stride_col = depth;
            int input_size_col   = filter_width_truncated;
            int input_stride_row = input_width * depth;
            int input_size_row   = in_y_size;
            int input_stride_vec;
            int input_size_vec;

            if (single_out_x) {
              input_stride_vec = stride_height * input_stride_row;
              input_size_vec   = output_height_truncated;
            } else {
              input_stride_vec = stride_width * depth;
              input_size_vec   = output_width_truncated;
            }

            // If there is at least one more channel left, then group it with this one and do 2 at a time in parallel
            // In general, the paralellization works if depth is even, but the full expression for
            // testing is included below for completeness.
            // Could just check for depth being even at top of function and replace
            // all the complex checks with assertions.
            const bool parallelize_channels = (channel + 1 < depth)
                                              && (output_index_base % 2 == 0)
                                              && (input_index_base  % 2 == 0)
                                              && (input_stride_col  % 2 == 0)
                                              && (input_stride_row  % 2 == 0)
                                              && (input_stride_vec  % 2 == 0)
                                              && (output_stride_col % 2 == 0)
                                              && (output_stride_row % 2 == 0)
                                              && (output_stride_vec % 2 == 0);
            parallelization = parallelize_channels ? 2 : 1;

            // Hardware strides are in terms of base units. Update strides to
            // reflect base unit scaling by parallelization.
            input_stride_col  /= parallelization;
            input_stride_row  /= parallelization;
            input_stride_vec  /= parallelization;
            output_stride_col /= parallelization;
            output_stride_row /= parallelization;
            output_stride_vec /= parallelization;

            sli_mvp_pb_begin_program(p);

            // Register allocation:
            //   Constants:
            //     c_lowest                R0       (r_... => real datatype)
            //                                      (c_... => complex datatype)
            //
            //   Registers used temporarily in different parts:
            //     r_acc                   R1
            //     r_input_i               R2
            //     c_activation_min        R3
            //     c_activation_max        R4
            //     r_output_i              R5
            //
            // Arrays:
            //   Array0  input
            //   Array1  output

            // Set up input array
            sli_mvp_pb_config_array_full(p->p,
                                         SLI_MVP_ARRAY(0),
                                         (void*)&input[input_index_base],  // addr
                                         parallelize_channels
                                         ? SLI_MVP_DATATYPE_COMPLEX_INT8   // datatype
                                         : SLI_MVP_DATATYPE_INT8,
                                         input_size_vec,                   // vecs
                                         input_size_row,                   // rows
                                         input_size_col,                   // cols
                                         input_stride_vec,                 // vecstride
                                         input_stride_row,                 // rowstride
                                         input_stride_col,                 // colstride
                                         &status);

            // Set up output array
            sli_mvp_pb_config_array_full(p->p,
                                         SLI_MVP_ARRAY(1),
                                         &output[output_index_base],       // addr
                                         parallelize_channels
                                         ? SLI_MVP_DATATYPE_COMPLEX_INT8   // datatype
                                         : SLI_MVP_DATATYPE_INT8,
                                         output_size_vec,                  // vecs
                                         output_size_row,                  // rows
                                         output_size_col,                  // cols
                                         output_stride_vec,                // vecstride
                                         output_stride_row,                // rowstride
                                         output_stride_col,                // colstride
                                         &status);

            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R0, (float16_t)INT8_MIN, (float16_t)INT8_MIN);
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R3, (float16_t)activation_min, (float16_t)activation_min);
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R4, (float16_t)activation_max, (float16_t)activation_max);

            sli_mvp_pb_begin_loop(p, output_width_truncated, &status); {
              sli_mvp_pb_begin_loop(p, out_y_size, &status); {
                // R1 = COPY(R0)
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(COPY),
                                   SLI_MVP_ALU_Z(SLI_MVP_R1)
                                   | SLI_MVP_ALU_A(SLI_MVP_R0),
                                   SLI_MVP_NONE,
                                   SLI_MVP_NONE,
                                   &status);
                sli_mvp_pb_begin_loop(p, filter_height_truncated, &status); {
                  sli_mvp_pb_begin_loop(p, input_size_col, &status); {
                    // LOAD(ARRAY0, R2)
                    // R1 = MAX2A(R2, R1)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MAX2A),
                                       SLI_MVP_ALU_Z(SLI_MVP_R1)
                                       | SLI_MVP_ALU_X(SLI_MVP_R2)
                                       | SLI_MVP_ALU_A(SLI_MVP_R1),
                                       SLI_MVP_LOAD(0, SLI_MVP_R2, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p);                    // input_size_col
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }
                sli_mvp_pb_end_loop(p);                    // filter_height_truncated

                if (single_out_x) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
                }

                int input_row_incr_left = in_y_extra_incr_adjusted;
                if (input_row_incr_left-- > 0) {
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW);
                }

                if (input_row_incr_left-- > 0) {     // use one of the available load streams of next op
                  // LOAD(ARRAY0, R2)
                  // R5 = CLIP2A(R3, R4, R1)
                  // STORE(ARRAY1, R5)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(CLIP2A),
                                     SLI_MVP_ALU_Z(SLI_MVP_R5)
                                     | SLI_MVP_ALU_X(SLI_MVP_R3)
                                     | SLI_MVP_ALU_Y(SLI_MVP_R4)
                                     | SLI_MVP_ALU_A(SLI_MVP_R1),
                                     SLI_MVP_LOAD(0, SLI_MVP_R2, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW),
                                     SLI_MVP_STORE(SLI_MVP_R5, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                                     &status);
                } else {
                  // R5 = CLIP2A(R3, R4, R1)
                  // STORE(ARRAY1, R5)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(CLIP2A),
                                     SLI_MVP_ALU_Z(SLI_MVP_R5)
                                     | SLI_MVP_ALU_X(SLI_MVP_R3)
                                     | SLI_MVP_ALU_Y(SLI_MVP_R4)
                                     | SLI_MVP_ALU_A(SLI_MVP_R1),
                                     SLI_MVP_NONE,
                                     SLI_MVP_STORE(SLI_MVP_R5, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC),
                                     &status);
                }

                // Handle special case where need to increment out_y
                // one piece at a time due to stride limits.
                if (out_y_extra_incr > 0) {
                  sli_mvp_pb_begin_loop(p, out_y_extra_incr, &status); {
                    sli_mvp_pb_begin_loop(p, 1, &status); {
                      sli_mvp_pb_compute(p,
                                         SLI_MVP_OP(NOOP),
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         SLI_MVP_NONE,
                                         &status);
                    }
                    sli_mvp_pb_end_loop(p);
                    sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_VEC);
                  }
                  sli_mvp_pb_end_loop(p);
                }

                // Handle any remaining input rows still needing to increment
                if (input_row_incr_left <= 0) {
                } else if (input_row_incr_left == 1) {
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
                } else {
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
              sli_mvp_pb_end_loop(p);                    // out_y_size
              sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_ROW);

              if (!single_out_x) {
                sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_VEC);
              }
            }
            sli_mvp_pb_end_loop(p);                    // output_width_truncated

            // Check if any errors found during program generation.
            if (status != SL_STATUS_OK) {
              if (execute) {
                EFM_ASSERT(false);
              }
              return status;
            }

            // Execute the program
            if (execute) {
              sli_mvp_pb_execute_program(p);
            }
          } // channels
        } // batches
      } // out_y_offset
    } // out_y_range
  } // out_x_range

  if (execute) {
    sli_mvp_wait_for_completion();
  }

  return SL_STATUS_OK;
}

/// @endcond
