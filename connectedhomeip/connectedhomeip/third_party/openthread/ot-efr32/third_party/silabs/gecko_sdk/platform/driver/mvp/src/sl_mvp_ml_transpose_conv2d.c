/***************************************************************************//**
 * @file
 * @brief MVP Transposed 2D Convolution kernel driver.
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

#include "sl_mvp_ml_transpose_conv2d.h"
#include "sl_mvp.h"
#include "sl_mvp_util.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

static sl_status_t transpose_conv(const sli_mvp_ml_transpose_conv2d_s8_params_t *params, bool execute);

/***************************************************************************//**
 *
 * Transposed 2D Convolution.
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_transpose_conv2d_s8(const sli_mvp_ml_transpose_conv2d_s8_params_t *params)
{
  return transpose_conv(params, true);
}

/***************************************************************************//**
 *
 * Check if MVP supports transposed convolution on given matrix.
 *
 ******************************************************************************/
bool sli_mvp_ml_transpose_conv2d_s8_is_supported(const sli_mvp_ml_transpose_conv2d_s8_params_t *params)
{
  return transpose_conv(params, false) == SL_STATUS_OK;
}

static sl_status_t transpose_conv(const sli_mvp_ml_transpose_conv2d_s8_params_t *params, bool execute)
{
  const int batches                    = params->batches;
  const int input_depth                = params->in_channels;
  const int input_height               = params->input_height;
  const int input_width                = params->input_width;
  int output_depth                     = params->out_channels;
  const int output_height              = params->output_height;
  const int output_width               = params->output_width;
  const int32_t activation_min         = params->output_activation_min;
  const int32_t activation_max         = params->output_activation_max;
  const int filter_height              = params->filter_height;
  const int filter_width               = params->filter_width;
  const int stride_width               = params->stride_width;
  const int stride_height              = params->stride_height;
  const int pad_width                  = params->pad_width;
  const int pad_height                 = params->pad_height;
  const bool needs_padding             = params->padding;
  const int32_t input_offset           = params->input_offset;
  const int32_t output_offset          = params->output_offset;

  const int8_t *input                  = params->input;
  int8_t *output                       = params->output;
  const float16_t *output_scaler       = params->output_scaler;
  const int8_t *filter                 = params->filter;
  const float16_t *bias                = params->bias;
  float16_t *tmp_buf                   = params->scratch_buffer;
  __ALIGNED(4) const float16_t zero[2] = { .0f, .0f };

  sl_status_t status                   = SL_STATUS_OK;
  sli_mvp_program_context_t *p         = sli_mvp_get_program_area_context();

  if ((needs_padding == false) && ((pad_width != 0) || (pad_height != 0))) {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  if (execute) {
    if ((bias && ((uint32_t)bias & 0x1U) != 0U)
        || (((uint32_t)output_scaler & 0x1U) != 0U)
        || (((uint32_t)tmp_buf & 0x3U) != 0U)
        || (activation_min > activation_max)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Early exit on errors.
  if (status != SL_STATUS_OK) {
    if (execute) {
      EFM_ASSERT(false);
    }
    return status;
  }

  sli_mvp_pb_init_program(p);

  if (execute) {
    // Zero entire temporary buffer using MVP.
    status = sli_mvp_util_memclr_f16(p, tmp_buf, batches, output_depth, output_height, output_width);
  }

  //----------------------------------------------------------------------------
  // Accumulate results in temporary buffer.
  // (Is parallelization possible ?)
  //----------------------------------------------------------------------------

  const int input_array         = SLI_MVP_ARRAY(0);
  const int filter_array        = SLI_MVP_ARRAY(1);
  const int tmp_buf_array       = SLI_MVP_ARRAY(2);
  const int output_array        = SLI_MVP_ARRAY(0);
  const int bias_array          = SLI_MVP_ARRAY(1);
  const int output_scaler_array = SLI_MVP_ARRAY(3);

  // Loop through batches which is usually only one.
  for (int batch = 0; batch < batches; ++batch) {
    // Loop through input elements.
    for (int in_y = 0; in_y < input_height; ++in_y) {
      for (int in_x = 0; in_x < input_width; ++in_x) {
        const int out_y_origin = (in_y * stride_height) - pad_height;
        const int out_x_origin = (in_x * stride_width) - pad_width;

        const int filter_y_start = SL_MAX(0, -out_y_origin);
        const int filter_y_end   = SL_MIN(filter_height, output_height - out_y_origin);
        const int filter_x_start = SL_MAX(0, -out_x_origin);
        const int filter_x_end   = SL_MIN(filter_width, output_width - out_x_origin);

        const int input_base = sli_mvp_util_offset_nhwc(input_height, input_width, input_depth,
                                                        batch, in_y, in_x,
                                                        0 /* input channel */);
        int tmpbuf_base = sli_mvp_util_offset_nhwc(output_height, output_width, output_depth,
                                                   batch,
                                                   out_y_origin + filter_y_start,
                                                   out_x_origin + filter_x_start,
                                                   0 /* output channel */);
        int filter_base = sli_mvp_util_offset_nhwc(filter_height, filter_width, input_depth,
                                                   batch,
                                                   filter_y_start,
                                                   filter_x_start,
                                                   0 /* output channel */);
        const int filter_x_count = filter_x_end - filter_x_start;
        const int filter_y_count = filter_y_end - filter_y_start;

        // Register allocation:
        //   R0  accumulator_scaler   (constant register)
        //   R1  input_offset_scaled  (constant register)
        //   R2  input
        //   R3  filter
        //   R4  tmp_buf
        //   R5  scratch              (intermediate result register)
        //   R7  accumulator          (intermediate result register)
        //
        // Arrays:
        //   Array0  input
        //   Array1  filter
        //   Array2  tmp_buf

        const int output_channel_stride = 1;
        const int filter_pack_stride = filter_height * filter_width * input_depth;

        // Loop through output channels.
        for (int out_channel = 0; out_channel < output_depth; ++out_channel) {
          sli_mvp_pb_begin_program(p);

          sli_mvp_pb_config_array_nhwc(p->p,
                                       input_array,
                                       (void*)&input[input_base],
                                       SLI_MVP_DATATYPE_INT8,
                                       input_height,
                                       input_width,
                                       input_depth,
                                       &status);

          sli_mvp_pb_config_array_nhwc(p->p,
                                       filter_array,
                                       (void*)&filter[filter_base],
                                       SLI_MVP_DATATYPE_INT8,
                                       filter_height,
                                       filter_width,
                                       input_depth,
                                       &status);

          sli_mvp_pb_config_array_nhwc(p->p,
                                       tmp_buf_array,
                                       (void*)&tmp_buf[tmpbuf_base],
                                       SLI_MVP_DATATYPE_BINARY16,
                                       output_height,
                                       output_width,
                                       output_depth,
                                       &status);

          sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER);
          sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R1, input_offset * SLI_MVP_ACCUMULATOR_SCALER);

          sli_mvp_pb_begin_loop(p, input_depth, &status); {         // Loop over input channels.

            // Calculate: tmp_buf += (input + input_offset) * ACCUMULATOR_SCALER * filter
            //      =>    tmp_buf += ((input * ACCUMULATOR_SCALER) + input_offset_scaled) * filter

            // R2 = Input
            // R5 = (R2 * R0) + R1   scratch = (input * ACCUMULATOR_SCALER) + input_offset_scaled
            sli_mvp_pb_compute(p,
                               SLI_MVP_OP(MACR2A),
                               SLI_MVP_ALU_Z(SLI_MVP_R5)
                               | SLI_MVP_ALU_X(SLI_MVP_R2)
                               | SLI_MVP_ALU_Y(SLI_MVP_R0)
                               | SLI_MVP_ALU_A(SLI_MVP_R1),
                               SLI_MVP_LOAD(0, SLI_MVP_R2, input_array, SLI_MVP_NOINCR),
                               SLI_MVP_NONE,
                               &status);

            sli_mvp_pb_begin_loop(p, filter_y_count, &status); {        // Loop over filter height.
              sli_mvp_pb_begin_loop(p, filter_x_count, &status); {      // Loop over filter width.

                // R4 = tmp_buf
                // R3 = Filter
                // R7 = (R5 * R3) + R4   acc += ((input * ACCUMULATOR_SCALER) + input_offset_scaled) * filter
                // output = R7
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(MACR2A),
                                   SLI_MVP_ALU_Z(SLI_MVP_R7)
                                   | SLI_MVP_ALU_X(SLI_MVP_R5)
                                   | SLI_MVP_ALU_Y(SLI_MVP_R3)
                                   | SLI_MVP_ALU_A(SLI_MVP_R4),
                                   SLI_MVP_LOAD(0, SLI_MVP_R4, tmp_buf_array, SLI_MVP_NOINCR)
                                   | SLI_MVP_LOAD(1, SLI_MVP_R3, filter_array, SLI_MVP_INCRDIM_WIDTH),
                                   SLI_MVP_STORE(SLI_MVP_R7, tmp_buf_array, SLI_MVP_INCRDIM_WIDTH),
                                   &status);
              }
              sli_mvp_pb_end_loop(p);      // Filter width.
              sli_mvp_pb_postloop_reset_dim(p, tmp_buf_array, SLI_MVP_RESETDIM_WIDTH);
              sli_mvp_pb_postloop_reset_dim(p, filter_array, SLI_MVP_RESETDIM_WIDTH);
              sli_mvp_pb_postloop_incr_dim(p, tmp_buf_array, SLI_MVP_INCRDIM_HEIGHT);
              sli_mvp_pb_postloop_incr_dim(p, filter_array, SLI_MVP_INCRDIM_HEIGHT);
            }
            sli_mvp_pb_end_loop(p);    // Filter height.
            sli_mvp_pb_postloop_reset_dim(p, tmp_buf_array, SLI_MVP_RESETDIM_HEIGHT);
            sli_mvp_pb_postloop_reset_dim(p, filter_array, SLI_MVP_RESETDIM_HEIGHT);
            sli_mvp_pb_postloop_incr_dim(p, input_array, SLI_MVP_INCRDIM_DEPTH);
            sli_mvp_pb_postloop_incr_dim(p, filter_array, SLI_MVP_INCRDIM_DEPTH);
          }
          sli_mvp_pb_end_loop(p);  // Input channels.

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

          // Next filter "pack".
          filter_base += filter_pack_stride;
          // Next output channel.
          tmpbuf_base += output_channel_stride;
        } // end "out_channel" loop
      }
    }
  }

  //----------------------------------------------------------------------------
  // Build output tensor from final temporary buffer tensor by applying bias,
  // scaler, output offset and activation.
  //----------------------------------------------------------------------------

  // Check if we can use parallel computations.
  bool parallel = false;
  if ((((uint32_t)bias & 0x3U) == 0U)
      && (((uint32_t)tmp_buf & 0x3U) == 0U)
      && (((uint32_t)output_scaler & 0x3U) == 0U)
      && (((uint32_t)output & 0x1U) == 0U)
      && ((output_depth % 2) == 0)) {
    parallel = true;
    output_depth /= 2;
  }

  // Loop through batches which is usually only one.
  for (int batch = 0; batch < batches; ++batch) {
    // Register allocation:
    //   R0  output_offset     (constant register)
    //   R1  activation_min    (constant register)
    //   R2  activation_max    (constant register)
    //   R3  tmp_buf
    //   R4  bias
    //   R5  scaler
    //   R7  accumulator       (intermediate result register)
    //
    // Arrays:
    //   Array0  output
    //   Array1  bias
    //   Array2  tmp_buf
    //   Array3  output_scaler

    sli_mvp_pb_begin_program(p);

    const int output_base = sli_mvp_util_offset_nhwc(output_height, output_width, output_depth,
                                                     batch, 0, 0, 0);
    sli_mvp_pb_config_array_nhwc(p->p,
                                 tmp_buf_array,
                                 (void*)&tmp_buf[output_base],
                                 parallel
                                 ? SLI_MVP_DATATYPE_COMPLEX_BINARY16
                                 : SLI_MVP_DATATYPE_BINARY16,
                                 output_height,
                                 output_width,
                                 output_depth,
                                 &status);

    sli_mvp_pb_config_array_nhwc(p->p,
                                 output_array,
                                 (void*)&output[output_base],
                                 parallel
                                 ? SLI_MVP_DATATYPE_COMPLEX_INT8
                                 : SLI_MVP_DATATYPE_INT8,
                                 output_height,
                                 output_width,
                                 output_depth,
                                 &status);

    sli_mvp_pb_config_vector(p->p,
                             bias_array,
                             bias != NULL ? (void*)bias : (void*)zero,
                             parallel
                             ? SLI_MVP_DATATYPE_COMPLEX_BINARY16
                             : SLI_MVP_DATATYPE_BINARY16,
                             bias != NULL ? output_depth : 1,
                             &status);

    sli_mvp_pb_config_vector(p->p,
                             output_scaler_array,
                             (void*)output_scaler,
                             parallel
                             ? SLI_MVP_DATATYPE_COMPLEX_BINARY16
                             : SLI_MVP_DATATYPE_BINARY16,
                             output_depth,
                             &status);

    sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R0, output_offset, output_offset);
    bool do_activation = (activation_min != -128) || (activation_max != 127);
    if (do_activation) {
      sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, (float16_t)activation_min, (float16_t)activation_min);
      sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R2, (float16_t)activation_max, (float16_t)activation_max);
    }

    // R4 = bias
    sli_mvp_pb_compute(p,
                       SLI_MVP_OP(NOOP),
                       SLI_MVP_NONE,
                       SLI_MVP_LOAD(0, SLI_MVP_R4, bias_array, SLI_MVP_INCRDIM_WIDTH),
                       SLI_MVP_NONE,
                       &status);

    sli_mvp_pb_begin_loop(p, output_height, &status); {
      sli_mvp_pb_begin_loop(p, output_width, &status); {

        // R3 = tmp_buf
        sli_mvp_pb_compute(p,
                           SLI_MVP_OP(NOOP),
                           SLI_MVP_NONE,
                           SLI_MVP_LOAD(0, SLI_MVP_R3, tmp_buf_array, SLI_MVP_NOINCR),
                           SLI_MVP_NONE,
                           &status);

        sli_mvp_pb_begin_loop(p, output_depth, &status); {

          // Calculate: output = ((tmp_buf + bias) * scaler) + output_offset
          //            output = MAX(output, activation_min)
          //            output = MIN(output, activation_max)

          // R5 = scaler
          // R7 = R3 + R4    acc = tmp_buf + bias
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(ADDC),
                             SLI_MVP_ALU_Z(SLI_MVP_R7)
                             | SLI_MVP_ALU_X(SLI_MVP_R3)
                             | SLI_MVP_ALU_A(SLI_MVP_R4),
                             SLI_MVP_LOAD(0, SLI_MVP_R5, output_scaler_array, SLI_MVP_INCRDIM_WIDTH),
                             SLI_MVP_NONE,
                             &status);

          // R3 = tmp_buf          (load for use in next loop iteration)
          // R4 = bias             (load for use in next loop iteration)
          // R7 = (R7 * R5) + R0   acc = ((tmp_buf + bias) * scaler) + output_offset
          // output = R7
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(MACR2A),
                             SLI_MVP_ALU_Z(SLI_MVP_R7)
                             | SLI_MVP_ALU_X(SLI_MVP_R7)
                             | SLI_MVP_ALU_Y(SLI_MVP_R5)
                             | SLI_MVP_ALU_A(SLI_MVP_R0),
                             SLI_MVP_LOAD(0, SLI_MVP_R3, tmp_buf_array, SLI_MVP_INCRDIM_DEPTH)
                             | SLI_MVP_LOAD(1, SLI_MVP_R4, bias_array, SLI_MVP_INCRDIM_WIDTH),
                             do_activation
                             ? SLI_MVP_NONE
                             : SLI_MVP_STORE(SLI_MVP_R7, output_array, SLI_MVP_INCRDIM_DEPTH),
                             &status);

          if (do_activation) {
            // R7 = CLIP2A(R1, R2, R7)
            // output = R7
            sli_mvp_pb_compute(p,
                               SLI_MVP_OP(CLIP2A),
                               SLI_MVP_ALU_Z(SLI_MVP_R7)
                               | SLI_MVP_ALU_X(SLI_MVP_R1)
                               | SLI_MVP_ALU_Y(SLI_MVP_R2)
                               | SLI_MVP_ALU_A(SLI_MVP_R7),
                               SLI_MVP_NONE,
                               SLI_MVP_STORE(SLI_MVP_R7, output_array, SLI_MVP_INCRDIM_DEPTH),
                               &status);
          }
        }
        sli_mvp_pb_end_loop(p);      // Output depth.
        sli_mvp_pb_postloop_incr_dim(p, output_array, SLI_MVP_INCRDIM_WIDTH);
        sli_mvp_pb_postloop_incr_dim(p, tmp_buf_array, SLI_MVP_INCRDIM_WIDTH);
      }
      sli_mvp_pb_end_loop(p);    // Output width.
      sli_mvp_pb_postloop_incr_dim(p, output_array, SLI_MVP_INCRDIM_HEIGHT);
      sli_mvp_pb_postloop_incr_dim(p, tmp_buf_array, SLI_MVP_INCRDIM_HEIGHT);
    }
    sli_mvp_pb_end_loop(p);  // Output height.

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
  } // Batches.
  sli_mvp_wait_for_completion();

  return SL_STATUS_OK;
}
/// @endcond
