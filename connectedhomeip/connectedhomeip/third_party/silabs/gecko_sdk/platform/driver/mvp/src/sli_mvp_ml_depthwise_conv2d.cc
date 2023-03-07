/***************************************************************************//**
 * @file
 * @brief C++ implementation of depthwise 2D convolution.
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
#include "sl_mvp_util.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"
#include <cstdlib>
#include <algorithm>
#include <string.h>

namespace {

#define SLI_MVP_CHECK(x)                \
  do {                                  \
    if (!(x)) {                         \
      status = SL_STATUS_INVALID_RANGE; \
    }                                   \
  } while (0)

static sl_status_t status;

// Dimension info specific to loop structure of this kernel
enum {
  kDimBatch,
  kDimInChannel,        // Also acts as out channel group.
  kDimM,
  kDimOutYOffset,
  kDimOutX,
  kDimOutYGroup,
  kDimFilterY,
  kDimFilterX,
  kDimNone,             // Dummy dimension with zero stride and size.
  kDimCount
};

// Stream ID
enum {
  kIStream0 = _MVP_INSTRCFG1_ISTREAM0REGID_SHIFT,
  kIStream1 = _MVP_INSTRCFG1_ISTREAM1REGID_SHIFT,
  kOStream = _MVP_INSTRCFG1_OSTREAMREGID_SHIFT,
};

struct DwConv2DDims {
  int offsets[kDimCount]  = { 0 };
  int sizes[kDimCount];
  int strides[kDimCount]  = { 0 };
  int indices[kDimCount]  = { 0 };
  int extra_incr_map[kDimCount][kDimCount] = { { 0 } };   // Extra increments after final iteration to help with flattening.
  int mvp_dims[kDimCount] = { 0 };                        // MVP dimension mapping.
  int reverse_mvp_dims[SLI_MVP_MAX_DIM];                  // Reverse mapping from mvp dim to this dim.
  int array_no;

  DwConv2DDims(int array_number)
  {
    array_no = array_number;
    for(auto & size : sizes) {
      size = 1;
    }
  }

  int numActiveDims() const
  {
    int count = 0;
    for(int i = 0; i < kDimCount; ++i) {
      if (isActive(i)) {
        ++count;
      }
    }
    return count;
  }

  bool isActive(int dim) const
  {
    return (sizes[dim] > 1) && (strides[dim] > 0);
  }

  int flat_offset() const
  {
    int offset = 0;
    for(int i = 0; i < kDimCount; ++i) {
      offset += offsets[i] * strides[i];
    }
    return offset;
  }

  /* Adjust sizes and extra_incr_map such that dim1 will not increment
   *  and the effects of dim1 incrementing will instead be handled by
   *  dim2.  This generally only works if dim1 and dim2 loops are adjacent
   *  and strides of dim1 are a multiple of dim2 strides
   */
  void flatten_dim_into_another(int dim1, int dim2)
  {
      auto stride_ratio = std::div(strides[dim1], strides[dim2]);
      SLI_MVP_CHECK(stride_ratio.rem == 0);           // check divides evenly
      int extra_incr = stride_ratio.quot - sizes[dim2];
      SLI_MVP_CHECK(extra_incr >= 0);

      extra_incr_map[dim1][dim2] = extra_incr;
      sizes[dim2] = (sizes[dim2] + extra_incr) * sizes[dim1];
      sizes[dim1] = 1;
  }

  /* Return the new size of dim2 in the case that dims are flattened, but don't actually flatten */
  int dry_run_flatten_dim_into_another(int dim1, int dim2)
  {
    auto stride_ratio = std::div(strides[dim1], strides[dim2]);
    SLI_MVP_CHECK(stride_ratio.rem == 0);           // check divides evenly
    int extra_incr = stride_ratio.quot - sizes[dim2];
    SLI_MVP_CHECK(extra_incr >= 0);

    return (sizes[dim2] + extra_incr) * sizes[dim1];
  }

  /* The opposite of flattening, split one dimension into 2 by
   *  dividing into groups.
   * This is generally useful if you want to handle incrementing via multiple
   *  loop iterators, e.g., if the reduced size makes it easier to flatten dim2
   *  later.
   * Also, note can even split such that loop iterators are not adjacent
   */
  void unflatten_dim_from_another(int dim1, int dim2, int group_size)
  {
    auto num_groups = std::div(sizes[dim2], group_size);
    SLI_MVP_CHECK(num_groups.quot > 0);
    SLI_MVP_CHECK(num_groups.rem == 0);

    SLI_MVP_CHECK(sizes[dim1] == 1);    // Dim1 should have been flattened in order to unflatten
    sizes[dim1] = num_groups.quot;
    sizes[dim2] /= num_groups.quot;
    strides[dim1] = sizes[dim2] * strides[dim2];
  }

  /** Assign MVP dimensions to all the active dims,
   *   or explicitly assign to none
   */
  void assign_mvp_dims()
  {
    int j = 0;
    for(int i = 0; i < kDimCount; ++i) {
      if (isActive(i)) {
        mvp_dims[i] = j;
        reverse_mvp_dims[j] = i;
        j++;
        SLI_MVP_CHECK((unsigned)strides[i] <= SLI_MVP_MAX_VECTOR_STRIDE);
        SLI_MVP_CHECK((unsigned)sizes  [i] <= SLI_MVP_MAX_VECTOR_COUNT);
      }
    }

    SLI_MVP_CHECK((unsigned)j <= SLI_MVP_MAX_DIM);

    // Assign any unused MVP dims to a dummy dimension
    for(; (unsigned)j < SLI_MVP_MAX_DIM; j++) {
      reverse_mvp_dims[j] = kDimNone;
    }
  }

  void mvp_incr_index_stream(sli_mvp_program_context_t *p, int dim, int stream_no, int reg_no)
  {
    if(p->last_instr >= 8) {
      status = SL_STATUS_INVALID_PARAMETER;
      return;
    }

    if(isActive(dim)) {
      uint32_t cfg1 = (reg_no << _MVP_INSTRCFG1_ISTREAM0REGID_SHIFT)
                      | MVP_INSTRCFG1_ISTREAM0LOAD
                      | (array_no << _MVP_INSTRCFG1_ISTREAM0ARRAYID_SHIFT)
                      | ((1 << mvp_dims[dim]) << _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_SHIFT);
      p->p->INSTR[p->last_instr].CFG1 |= cfg1 << stream_no;
    }

    mvp_incr_check_other_dimensions(p, dim, reg_no, -1);
  }

  void mvp_incr_index_loop(sli_mvp_program_context_t *p, int dim, int reg_no)
  {
    if((p->loop_level + 1) >= 8) {
      status = SL_STATUS_INVALID_PARAMETER;
      return;
    }

    if(isActive(dim)) {
      // Set DIMx increment bit.
      p->p->LOOP[p->loop_level + 1].CFG |= (MVP_LOOPCFG_ARRAY0INCRDIM0 << (4 * array_no)) << mvp_dims[dim];
    }

    mvp_incr_check_other_dimensions(p, dim, reg_no, p->loop_level + 1);
  }

private:
  void mvp_incr_check_other_dimensions(sli_mvp_program_context_t *p, int dim, int reg_no, int loop_no)
  {
    for (int dim2 = 0; dim2 < kDimCount; ++dim2) {
      int cnt = extra_incr_map[dim][dim2];
      if (cnt != 0) {
        int dim_id = mvp_dims[dim2];

        if (loop_no >= 0) {
          uint32_t loopcfg_arrayincr_mask = (MVP_LOOPCFG_ARRAY0INCRDIM0
                                             | MVP_LOOPCFG_ARRAY0INCRDIM1
                                             | MVP_LOOPCFG_ARRAY0INCRDIM2)
                                            << (4 * array_no);
          // Check if a free array incrementer is available.
          if ((p->p->LOOP[loop_no].CFG & loopcfg_arrayincr_mask) == 0U) {
            // Set DIMx increment bit.
            loopcfg_arrayincr_mask = (MVP_LOOPCFG_ARRAY0INCRDIM0 << (4 * array_no))
                                     << dim_id;
            p->p->LOOP[loop_no].CFG |= loopcfg_arrayincr_mask;
            cnt--;
          }
        }

        if (cnt > 0) {
          sli_mvp_pb_begin_loop(p, cnt, &status); {
            sli_mvp_pb_compute(p,
                               SLI_MVP_OP(NOOP),
                               SLI_MVP_NONE,
                               SLI_MVP_LOAD(0, reg_no, array_no, 1 << dim_id),
                               SLI_MVP_NONE,
                               &status);
          }
          sli_mvp_pb_end_loop(p);
        }
      }
    }
  }
};

inline void calc_strides(int *strides, int height, int width, int depth)
{
  strides[0] = 1;
  strides[1] = depth;
  strides[2] = depth * width;
  strides[3] = strides[2] * height;
}

inline int div_floor(const int dividend, const int divisor)
{
  return dividend / divisor;
}

inline int div_ceil(const int dividend, const int divisor)
{
  return (dividend / divisor) + (((dividend % divisor) != 0) ? 1 : 0);
}

extern "C" {

sl_status_t sli_mvp_ml_depthwise_conv2d_s8_gen(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params,
                                               bool execute)
{
  const int stride_width           = params->stride_width;
  const int stride_height          = params->stride_height;
  const int dilation_width_factor  = params->dilation_width;
  const int dilation_height_factor = params->dilation_height;
  const int pad_width              = params->pad_width;
  const int pad_height             = params->pad_height;
  const bool needs_padding         = params->padding;
  const int32_t input_offset       = params->input_offset;      // r = s(q - Z)
  const int32_t output_offset      = params->output_offset;
  const int depth_multiplier       = params->out_channels / params->in_channels;
  const int batches                = params->batches;
  const int input_height           = params->input_height;
  const int input_width            = params->input_width;
  const int input_depth            = params->in_channels;
  const int filter_height          = params->filter_height;
  const int filter_width           = params->filter_width;
  const int filter_depth           = params->out_channels;
  const int output_height          = params->output_height;
  const int output_width           = params->output_width;
  const int output_depth           = params->out_channels;
  const int8_t *input              = params->input;
  const int8_t *filter             = params->filter;
  const float16_t *bias            = params->bias;
  const float16_t *output_scaler   = params->output_scaler;
  int8_t *output                   = params->output;

  status                           = SL_STATUS_OK;
  int mvp_prog_cnt                 = 0;
  sli_mvp_program_context_t *p     = sli_mvp_get_program_area_context();

  // Error exit if more than one batch.
  if (batches != 1) {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  if (needs_padding == false) {
    if ((pad_width != 0) || (pad_height != 0)) {
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  // In some cases, pack two reals to use both FMACs in MVP and double throughput
  // NOTE: The TFLM tensor allocator should ensure tensor buffers are aligned to 8-bytes
  const float16_t zero_data(0);
  const float16_t parallel_zero_data[2] {zero_data, zero_data};
  // Parallelizing over output channels when depth multiplier is even
  const bool parallelize_channels = depth_multiplier % 2 == 0;
  const int parallelization = parallelize_channels ? 2 : 1;

  if (execute) {
    // Some tensor buffers require specific alignment when processing two
    // values at a time with the complex datatypes, or if the datatype
    // itself dictates it.
    if (parallelize_channels) {
      if ((((uint32_t)output & 0x1) != 0U)
          || (((uint32_t)filter & 0x1) != 0U)
          || (((uint32_t)bias & 0x3) != 0U)
          || (((uint32_t)output_scaler & 0x3) != 0U)) {
        status = SL_STATUS_INVALID_PARAMETER;
      }
    } else {
      // Some tensor buffers require specific alignment when processing two
      // values at a time with the complex datatypes.
      if ((((uint32_t)bias & 0x3) != 0U)
          || (((uint32_t)output_scaler & 0x3) != 0U)) {
        status = SL_STATUS_INVALID_PARAMETER;
      }
    }
  }

  sli_mvp_pb_init_program(p);

  // Gather original dim info.
  int original_input_strides[4];
  int original_output_strides[4];
  int original_filter_strides[4];
  calc_strides(original_input_strides, input_height, input_width, input_depth);
  calc_strides(original_output_strides, output_height, output_width, output_depth);
  calc_strides(original_filter_strides, filter_height, filter_width, filter_depth);

  // Rearrange dims to match the loop structure of the kernel.
  DwConv2DDims input_dims(0), filter_dims(2), output_dims(1);

  input_dims.sizes  [kDimBatch     ] = batches;
  input_dims.strides[kDimBatch     ] = original_input_strides[3];
  input_dims.sizes  [kDimInChannel ] = input_depth;
  input_dims.strides[kDimInChannel ] = original_input_strides[0];
  input_dims.sizes  [kDimM         ] = depth_multiplier;
  input_dims.strides[kDimM         ] = 0;
  input_dims.sizes  [kDimOutYOffset] = output_height;
  input_dims.strides[kDimOutYOffset] = original_input_strides[2] * stride_height;
  input_dims.sizes  [kDimOutX      ] = output_width;
  input_dims.strides[kDimOutX      ] = original_input_strides[1] * stride_width;
  input_dims.sizes  [kDimOutYGroup ] = 1;
  input_dims.strides[kDimOutYGroup ] = input_dims.strides[kDimOutYOffset] * input_dims.sizes[kDimOutYOffset];
  input_dims.sizes  [kDimFilterY   ] = filter_height;
  input_dims.strides[kDimFilterY   ] = original_input_strides[2] * dilation_height_factor;
  input_dims.sizes  [kDimFilterX   ] = filter_width;
  input_dims.strides[kDimFilterX   ] = original_input_strides[1] * dilation_width_factor;
  input_dims.offsets[kDimFilterY   ] = -pad_height;
  input_dims.offsets[kDimFilterX   ] = -pad_width;

  output_dims.sizes  [kDimBatch     ] = batches;
  output_dims.strides[kDimBatch     ] = original_output_strides[3];
  output_dims.sizes  [kDimInChannel ] = input_depth;
  output_dims.strides[kDimInChannel ] = original_output_strides[0] * depth_multiplier;
  output_dims.sizes  [kDimM         ] = depth_multiplier;
  output_dims.strides[kDimM         ] = original_output_strides[0];
  output_dims.sizes  [kDimOutYOffset] = output_height;
  output_dims.strides[kDimOutYOffset] = original_output_strides[2];
  output_dims.sizes  [kDimOutX      ] = output_width;
  output_dims.strides[kDimOutX      ] = original_output_strides[1];
  output_dims.sizes  [kDimOutYGroup ] = 1;
  output_dims.strides[kDimOutYGroup ] = 0;
  output_dims.sizes  [kDimFilterY   ] = filter_height;
  output_dims.strides[kDimFilterY   ] = 0;
  output_dims.sizes  [kDimFilterX   ] = filter_width;
  output_dims.strides[kDimFilterX   ] = 0;

  int effective_depth_multiplier = depth_multiplier;

  if (parallelize_channels) {
    input_dims.sizes[kDimM]    /= parallelization;
    output_dims.sizes[kDimM]   /= parallelization;
    effective_depth_multiplier /= parallelization;

    // Adjust strides due to size change.
    for(int dim = 0; dim < kDimCount; ++dim) {
      if (dim == kDimM) {
        continue;         // Don't update this one.
      }
      output_dims.strides[dim] /= parallelization;
    }
  }

  // Check strides and sizes upfront.
  for(int dim = 0; dim < kDimCount; ++dim) {
    if (input_dims.isActive(dim)) {
      if (((unsigned)input_dims.sizes[dim] > SLI_MVP_MAX_VECTOR_COUNT)
          ||((unsigned)input_dims.strides[dim] > SLI_MVP_MAX_VECTOR_STRIDE)) {
        status = SL_STATUS_INVALID_PARAMETER;
      }
    }
    if (output_dims.isActive(dim)) {
      if (((unsigned)output_dims.sizes[dim] > SLI_MVP_MAX_VECTOR_COUNT)
          || ((unsigned)output_dims.strides[dim] > SLI_MVP_MAX_VECTOR_STRIDE)) {
        status = SL_STATUS_INVALID_PARAMETER;
      }
    }
  }

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Flatten out channel groups immediately to reduce dim count of output.
  output_dims.flatten_dim_into_another(kDimInChannel, kDimM);

  // Iterate over possible sub-filters that make up the edge boundary cases
  // and record the out_x, out_y ranges and filter ranges that correspond to each case.
  // Doesn't actually loop through each index, but only those that correspond to unique sub-filters.
  const int in_y_origin_center_max = input_height - filter_height;
  const int out_y_center_max = div_floor(in_y_origin_center_max + pad_height, stride_height);
  const int in_x_origin_center_max = input_width - filter_width;
  const int out_x_center_max = div_floor(in_x_origin_center_max + pad_width, stride_width);

  for (int out_x_min = 0, out_x_max; out_x_min < output_width; out_x_min = out_x_max + 1) {
    const int in_x_origin_min = (out_x_min * stride_width) - pad_width;
    const int filter_x_start = std::max(0, -in_x_origin_min);
    const int filter_x_end   = std::min(filter_width, input_width - in_x_origin_min);
    const int filter_width_truncated = filter_x_end - filter_x_start;

    if (in_x_origin_min < 0) {  // Case 1.  each out_x leads to 1 filter_x_start
        out_x_max = out_x_min;  // (and 1 or more filter_x_end)
    } else {                    // Case 2.  true for the rest of the out_x
        if (input_width - in_x_origin_min >= filter_width) {
                                // Case 2a. true for multiple out_x
            out_x_max = out_x_center_max;
        } else {                // Case 2b. each out_x leads to 1 filter_x_end
            out_x_max = out_x_min;
        }
    }
    const int output_width_truncated = out_x_max - out_x_min + 1;
    SLI_MVP_CHECK(output_width_truncated > 0);

    for (int out_y_min = 0, out_y_max; out_y_min < output_height; out_y_min = out_y_max + 1) {
      const int in_y_origin_min = (out_y_min * stride_height) - pad_height;
      const int filter_y_start = std::max(0, -in_y_origin_min);
      const int filter_y_end   = std::min(filter_height, input_height - in_y_origin_min);
      const int filter_height_truncated = filter_y_end - filter_y_start;

      if (in_y_origin_min < 0) {  // Case 1.  each out_y leads to 1 filter_y_start
          out_y_max = out_y_min;  // (and 1 or more filter_y_end)
      } else {                    // Case 2.  true for the rest of the out_y
          if (input_height - in_y_origin_min >= filter_height) {
                                  // Case 2a. true for multiple out_y
              out_y_max = out_y_center_max;
          } else {                // Case 2b. each out_y leads to 1 filter_y_end
              out_y_max = out_y_min;
          }
      }
      int output_height_truncated = out_y_max - out_y_min + 1;
      SLI_MVP_CHECK(output_height_truncated > 0);

      // Modified dims based on truncated filters.
      DwConv2DDims in_dims = input_dims, out_dims = output_dims;
      in_dims .sizes  [kDimOutX]       = output_width_truncated;
      out_dims.sizes  [kDimOutX]       = output_width_truncated;
      in_dims .sizes  [kDimFilterX]    = filter_width_truncated;
      out_dims.sizes  [kDimFilterX]    = filter_width_truncated;
      in_dims .sizes  [kDimOutYOffset] = output_height_truncated;
      out_dims.sizes  [kDimOutYOffset] = output_height_truncated;
      in_dims .sizes  [kDimFilterY]    = filter_height_truncated;
      out_dims.sizes  [kDimFilterY]    = filter_height_truncated;

      int out_y_groups = 1;
      int out_y_offsets = output_height_truncated;

      // Work on flattening input dimensions until can fit into accelerator.

      // First flatten out y offset into out x.
      if(((unsigned)in_dims.numActiveDims() > SLI_MVP_MAX_DIM)
         || ((unsigned)in_dims.strides[kDimOutYOffset] > SLI_MVP_MAX_VECTOR_STRIDE)) {
        if (in_dims.isActive(kDimOutYOffset) && in_dims.isActive(kDimOutX)) {
          int new_size = in_dims.dry_run_flatten_dim_into_another(kDimOutYOffset, kDimOutX);

          // Pre-emptively split into groups if will be required after flattening.
          if ((unsigned)new_size > SLI_MVP_MAX_VECTOR_COUNT) {
            // Ideal group size is one that is amenable to flattening later.
            auto group_size = div_ceil(filter_height_truncated, stride_height);
            auto num_groups = std::div(output_height_truncated, group_size);

            // Chop off remainder so divides evenly.
            // Have to redo some previous computations.
            out_y_max -= num_groups.rem;
            output_height_truncated = out_y_max - out_y_min + 1;
            out_dims.sizes[kDimOutYOffset] = output_height_truncated;
            in_dims.sizes[kDimOutYOffset] = output_height_truncated;

            // Now split into the groups as planned.
            // out_dims also has to be split in the same way since loops are not adjacent and the iteration order won't
            // match the original flattened order.
            in_dims.unflatten_dim_from_another(kDimOutYGroup, kDimOutYOffset, group_size);
            out_dims.unflatten_dim_from_another(kDimOutYGroup, kDimOutYOffset, group_size);
            out_y_offsets = group_size;
            out_y_groups = num_groups.quot;
          }

          in_dims.flatten_dim_into_another(kDimOutYOffset, kDimOutX);
          SLI_MVP_CHECK((unsigned)in_dims.sizes[kDimOutX] <= SLI_MVP_MAX_VECTOR_COUNT);
        }
      }

      // Next flatten out y group into filter y
      if(((unsigned)in_dims.numActiveDims() > SLI_MVP_MAX_DIM)
         || ((unsigned)in_dims.strides[kDimOutYGroup] > SLI_MVP_MAX_VECTOR_STRIDE)) {
        if (in_dims.isActive(kDimOutYGroup) && in_dims.isActive(kDimFilterY)) {
          in_dims.flatten_dim_into_another(kDimOutYGroup, kDimFilterY);
        }
      }

      // It is possible that still have one extra dim, but shouldn't have more
      SLI_MVP_CHECK(in_dims.numActiveDims() <= 4);

      // Work on flattening output dimensions until can fit into accelerator

      // flatten out y offset into out x
      if(((unsigned)out_dims.numActiveDims() > SLI_MVP_MAX_DIM)
         || ((unsigned)out_dims.strides[kDimOutYOffset] > SLI_MVP_MAX_VECTOR_STRIDE)) {
        if (out_dims.isActive(kDimOutYOffset) && out_dims.isActive(kDimOutX)) {
          out_dims.flatten_dim_into_another(kDimOutYOffset, kDimOutX);
        }
      }

      SLI_MVP_CHECK((unsigned)out_dims.numActiveDims() <= SLI_MVP_MAX_DIM);

      // split output out_y groups into smaller strides as necessary
      int out_dim_out_y_group_extra_incr = 0;
      if (out_dims.isActive(kDimOutYGroup) && (unsigned)out_dims.strides[kDimOutYGroup] > SLI_MVP_MAX_VECTOR_STRIDE) {
        // first divide out a known factor
        SLI_MVP_CHECK((out_dims.strides[kDimOutYGroup] % out_y_offsets) == 0);
        out_dims.strides[kDimOutYGroup] /= out_y_offsets;
        out_dims.sizes[kDimOutYGroup] *= out_y_offsets;
        int factor = out_y_offsets;

        while ((unsigned)out_dims.strides[kDimOutYGroup] > SLI_MVP_MAX_VECTOR_STRIDE) {
          SLI_MVP_CHECK((out_dims.strides[kDimOutYGroup] % 2) == 0);
          out_dims.strides[kDimOutYGroup] /= 2;
          out_dims.sizes[kDimOutYGroup] *= 2;
          factor *= 2;
        }
        // now, turn the factor into extra incrs
        // Not using the extra_incr_map because not set up for doing extra incr's on same dim yet
        out_dim_out_y_group_extra_incr = factor - 1;
      }

      bool in_channel_outside_program = false;
      if ((unsigned)in_dims.numActiveDims() > SLI_MVP_MAX_DIM) {
        SLI_MVP_CHECK(in_dims.isActive(kDimInChannel));
        in_channel_outside_program = true;
        in_dims.sizes[kDimInChannel] = 1;   // deactivate
        out_dims.sizes[kDimInChannel] = 1;  // deactivate (should be already)
      }

      if ((output_width_truncated == 1)
          && ((output_height_truncated == 1)
              || (filter_height_truncated == 1)
              || (filter_width_truncated == 1))) {
          SLI_MVP_CHECK(!in_channel_outside_program);
      }

      SLI_MVP_CHECK((unsigned)in_dims.numActiveDims() <= SLI_MVP_MAX_DIM);

      // Set up offsets
      in_dims .offsets[kDimOutX      ] += out_x_min;
      in_dims .offsets[kDimOutYOffset] += out_y_min;
      in_dims .offsets[kDimFilterX   ] += filter_x_start;
      in_dims .offsets[kDimFilterY   ] += filter_y_start;
      out_dims.offsets[kDimOutX      ] += out_x_min;
      out_dims.offsets[kDimOutYOffset] += out_y_min;
      out_dims.offsets[kDimFilterX   ] += filter_x_start;
      out_dims.offsets[kDimFilterY   ] += filter_y_start;

      // Assign MVP dimensions
      in_dims.assign_mvp_dims();
      out_dims.assign_mvp_dims();

      // Could work to include batch loop in program, but it is generally just 1 anyway
      for (int batch = 0; batch < batches; ++batch) {
        in_dims.offsets[kDimBatch] = batch;
        out_dims.offsets[kDimBatch] = batch;

        int in_channel_groups = in_channel_outside_program ? input_depth : 1;
        int num_in_channels_per_program = in_channel_outside_program ? 1 : input_depth;
        for(int in_channel_group = 0; in_channel_group < in_channel_groups; ++in_channel_group) {
          in_dims.offsets[kDimInChannel] = in_channel_group;  /* * num_in_channels_per_program; */
          out_dims.offsets[kDimInChannel] = in_channel_group; /* * num_in_channels_per_program; */
          SLI_MVP_CHECK((in_channel_group == 0) || (num_in_channels_per_program == 1));  // check assumption above

          sli_mvp_pb_begin_program(p);

          void *v_ptr;
          const int in_channel_start = in_channel_group * num_in_channels_per_program;
          const int out_channel_start = in_channel_start * depth_multiplier;

          // Register allocation:
          //   Constants:
          //     c_accumulator_scaler    R0       (r_... => real datatype)
          //     c_input_offset_scaled   R1       (c_... => complex datatype)
          //     c_output_offset         R2
          //
          //   Registers used temporarily in different parts:
          //     r_output_scaler_i       R3
          //     r_bias_i                R4
          //     r_acc                   R5
          //     r_input_i               R6
          //     r_filter_i              R7
          //     r_output_i              R7       (reuse)
          //
          // Arrays:
          //   Array0  input
          //   Array1  output
          //   Array2  filter
          //   Array3  bias
          //   Array4  output_scaler

          // Setup initial register content.
          if (parallelize_channels) {
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER, SLI_MVP_ACCUMULATOR_SCALER);
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R1, (float16_t)(input_offset * SLI_MVP_ACCUMULATOR_SCALER), (float16_t)(input_offset * SLI_MVP_ACCUMULATOR_SCALER));
            sli_mvp_prog_set_reg_f16c(p->p, SLI_MVP_R2, output_offset, output_offset);
          } else {
            sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R0, SLI_MVP_ACCUMULATOR_SCALER);
            sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R1, (float16_t)(input_offset * SLI_MVP_ACCUMULATOR_SCALER));
            sli_mvp_prog_set_reg_f16(p->p, SLI_MVP_R2, output_offset);
          }

          // Set up input array
          v_ptr = (void*)&input[in_dims.flat_offset()];
          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(0),
                                       v_ptr,                                          // addr
                                       SLI_MVP_DATATYPE_INT8,                          // datatype
                                       in_dims.sizes[in_dims.reverse_mvp_dims[0]],     // page
                                       in_dims.sizes[in_dims.reverse_mvp_dims[1]],     // rows
                                       in_dims.sizes[in_dims.reverse_mvp_dims[2]],     // cols
                                       in_dims.strides[in_dims.reverse_mvp_dims[0]],   // pagestride
                                       in_dims.strides[in_dims.reverse_mvp_dims[1]],   // rowstride
                                       in_dims.strides[in_dims.reverse_mvp_dims[2]],   // colstride
                                       &status);

          // Set up output array
          const int nonparallel_output_offset = out_dims.flat_offset() * parallelization;
          v_ptr = &output[nonparallel_output_offset];
          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(1),
                                       v_ptr,                                            // addr
                                       parallelize_channels == true
                                       ? SLI_MVP_DATATYPE_COMPLEX_INT8                   // datatype
                                       : SLI_MVP_DATATYPE_INT8,
                                       out_dims.sizes[out_dims.reverse_mvp_dims[0]],     // page
                                       out_dims.sizes[out_dims.reverse_mvp_dims[1]],     // rows
                                       out_dims.sizes[out_dims.reverse_mvp_dims[2]],     // cols
                                       out_dims.strides[out_dims.reverse_mvp_dims[0]],   // pagestride
                                       out_dims.strides[out_dims.reverse_mvp_dims[1]],   // rowstride
                                       out_dims.strides[out_dims.reverse_mvp_dims[2]],   // colstride
                                       &status);

          // Set up filter array
          const int nonparallel_filter_offset = sli_mvp_util_offset_nhwc(filter_height,
                                                                         filter_width,
                                                                         filter_depth,
                                                                         0,
                                                                         filter_y_start,
                                                                         filter_x_start,
                                                                         out_channel_start);
          v_ptr = const_cast<int8_t*>(&filter[nonparallel_filter_offset]);
          sli_mvp_pb_config_array_full(p->p,
                                       SLI_MVP_ARRAY(2),
                                       v_ptr,                                        // addr
                                       parallelize_channels == true
                                       ? SLI_MVP_DATATYPE_COMPLEX_INT8               // datatype
                                       : SLI_MVP_DATATYPE_INT8,
                                       filter_depth/parallelization,                 // page
                                       filter_width_truncated,                       // rows
                                       filter_height_truncated,                      // cols
                                       original_filter_strides[0],                   // pagestride
                                       original_filter_strides[1]/parallelization,   // rowstride
                                       original_filter_strides[2]/parallelization,   // colstride
                                       &status);

          // Set up bias array
          const int nonparallel_bias_offset = out_channel_start;
          void * bias_data_ptr;
          if (bias != nullptr) {
            bias_data_ptr = const_cast<float16_t*>(&bias[nonparallel_bias_offset]);
          } else {
            bias_data_ptr = const_cast<float16_t*>(parallel_zero_data);
          }
          sli_mvp_pb_config_vector(p->p,
                                   SLI_MVP_ARRAY(3),
                                   bias_data_ptr,                        // addr
                                   parallelize_channels == true
                                   ? SLI_MVP_DATATYPE_COMPLEX_BINARY16   // datatype
                                   : SLI_MVP_DATATYPE_BINARY16,
                                   bias != nullptr ? output_depth/parallelization : 1,
                                   &status);

          // Set up output_scaler array
          const int nonparallel_output_scaler_offset = out_channel_start;
          void * output_scaler_data_ptr;
          output_scaler_data_ptr = const_cast<float16_t*>(&output_scaler[nonparallel_output_scaler_offset]);
          sli_mvp_pb_config_vector(p->p,
                                   SLI_MVP_ARRAY(4),
                                   output_scaler_data_ptr,               // addr
                                   parallelize_channels == true
                                   ? SLI_MVP_DATATYPE_COMPLEX_BINARY16   // datatype
                                   : SLI_MVP_DATATYPE_BINARY16,
                                   output_depth/parallelization,
                                   &status);

          // Instructions and loops.

          if (num_in_channels_per_program > 1) {
            sli_mvp_pb_begin_loop(p, num_in_channels_per_program, &status);
          }

          if (effective_depth_multiplier > 1) {
            sli_mvp_pb_begin_loop(p, effective_depth_multiplier, &status);
          }

          // LOAD(ARRAY3, R4)    bias
          // LOAD(ARRAY4, R3)    output_scaler
          // INCR(ARRAY3, DIM2)
          // INCR(ARRAY4, DIM2)
          // R5 = COPY(R4)       Compute(r_acc, COPY, r_bias_i)
          sli_mvp_pb_compute(p,
                             SLI_MVP_OP(COPY),
                             SLI_MVP_ALU_Z(SLI_MVP_R5)
                             | SLI_MVP_ALU_A(SLI_MVP_R4),
                             SLI_MVP_LOAD(0, SLI_MVP_R4, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_COL)
                             | SLI_MVP_LOAD(1, SLI_MVP_R3, SLI_MVP_ARRAY(4), SLI_MVP_INCRDIM_COL),
                             SLI_MVP_NONE,
                             &status);

          sli_mvp_pb_begin_loop(p, out_y_offsets, &status); {
            sli_mvp_pb_begin_loop(p, output_width_truncated, &status); {
              sli_mvp_pb_begin_loop(p, out_y_groups, &status); {
                sli_mvp_pb_begin_loop(p, filter_height_truncated, &status); {
                  sli_mvp_pb_begin_loop(p, filter_width_truncated, &status); {

                    // LOAD(ARRAY0, R6)    input
                    // LOAD(ARRAY2, R7)    filter
                    // INCR(ARRAY2, DIM1)
                    // R6 = MAC(R6, R0, R1) Compute(r_input_i, MACC, r_input_i, c_accumulator_scaler, c_input_offset_scaled);
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACC),
                                       SLI_MVP_ALU_Z(SLI_MVP_R6)
                                       | SLI_MVP_ALU_X(SLI_MVP_R6)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R0)
                                       | SLI_MVP_ALU_A(SLI_MVP_R1),
                                       SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_NOINCR)
                                       | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_ROW),
                                       SLI_MVP_NONE,
                                       &status);
                    in_dims.mvp_incr_index_stream(p, kDimFilterX, kIStream0, SLI_MVP_R6);

                    // R5 = MAC(R6, R7, R5) Compute(r_acc, MACR2A, r_input_i, r_filter_i, r_acc);
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(MACR2A),
                                       SLI_MVP_ALU_Z(SLI_MVP_R5)
                                       | SLI_MVP_ALU_X(SLI_MVP_R6)
                                       | SLI_MVP_ALU_Y(SLI_MVP_R7)
                                       | SLI_MVP_ALU_A(SLI_MVP_R5),
                                       SLI_MVP_NONE,
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p); // filter_width_truncated
                  sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_COL);

                  in_dims.mvp_incr_index_loop(p, kDimFilterY, SLI_MVP_R6);
                }
                sli_mvp_pb_end_loop(p);  // filter_height_truncated

                in_dims.mvp_incr_index_loop(p, kDimOutYGroup, SLI_MVP_R6);

                // R7 = MAC2RA(R5, R3, R2) Compute(r_output_i, MACR2A, r_acc, r_output_scaler_i, c_output_offset)
                // STORE(ARRAY1, R7)
                sli_mvp_pb_compute(p,
                                   SLI_MVP_OP(MACR2A),
                                   SLI_MVP_ALU_Z(SLI_MVP_R7)
                                   | SLI_MVP_ALU_X(SLI_MVP_R5)
                                   | SLI_MVP_ALU_Y(SLI_MVP_R3)
                                   | SLI_MVP_ALU_A(SLI_MVP_R2),
                                   SLI_MVP_NONE,
                                   SLI_MVP_STORE(SLI_MVP_R7,SLI_MVP_ARRAY(1),SLI_MVP_NOINCR),
                                   &status);

                out_dims.mvp_incr_index_stream(p, kDimOutYGroup, kOStream, SLI_MVP_R6);

                if (out_dim_out_y_group_extra_incr == 0) {
                  // R5 = COPY(R4)       Compute(r_acc, COPY, r_bias_i)
                  sli_mvp_pb_compute(p,
                                     SLI_MVP_OP(COPY),
                                     SLI_MVP_ALU_Z(SLI_MVP_R5)
                                     | SLI_MVP_ALU_A(SLI_MVP_R4),
                                     SLI_MVP_NONE,
                                     SLI_MVP_NONE,
                                     &status);
                } else {
                  sli_mvp_pb_begin_loop(p, out_dim_out_y_group_extra_incr, &status); {
                    // R5 = COPY(R4)       Compute(r_acc, COPY, r_bias_i)
                    sli_mvp_pb_compute(p,
                                       SLI_MVP_OP(COPY),
                                       SLI_MVP_ALU_Z(SLI_MVP_R5)
                                       | SLI_MVP_ALU_A(SLI_MVP_R4),
                                       SLI_MVP_LOAD(0, SLI_MVP_R7, SLI_MVP_ARRAY(1), 1 << out_dims.mvp_dims[kDimOutYGroup]),
                                       SLI_MVP_NONE,
                                       &status);
                  }
                  sli_mvp_pb_end_loop(p);
                }
              }
              sli_mvp_pb_end_loop(p);  // out_y_groups

              in_dims.mvp_incr_index_loop(p, kDimOutX, SLI_MVP_R7);
              out_dims.mvp_incr_index_loop(p, kDimOutX, SLI_MVP_R6);
            }
            sli_mvp_pb_end_loop(p);  // output_width_truncated

            out_dims.mvp_incr_index_loop(p, kDimOutYOffset, SLI_MVP_R7);
            in_dims.mvp_incr_index_loop(p, kDimOutYOffset, SLI_MVP_R6);
          }
          sli_mvp_pb_end_loop(p);  // out_y_offsets
          sli_mvp_pb_postloop_incr_dim(p, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_VEC);

          out_dims.mvp_incr_index_loop(p, kDimM, SLI_MVP_R7);

          if (effective_depth_multiplier > 1) {
            sli_mvp_pb_end_loop(p);
          }

          in_dims.mvp_incr_index_loop(p, kDimInChannel, SLI_MVP_R6);

          if (num_in_channels_per_program > 1) {
            sli_mvp_pb_end_loop(p);
          }

          // Check if any errors found during program generation.
          if (status != SL_STATUS_OK) {
            return status;
          }

          // Execute the program
          if (execute) {
            sli_mvp_pb_execute_program(p);
          } else {
            mvp_prog_cnt++;
          }
        } // in_channel_groups
      } // batches
    } // out_y_range
  } // out_x_range

  if (execute) {
    sli_mvp_wait_for_completion();
    sli_mvp_math_clamp_i8(params->output,
                          batches * output_height * output_width * output_depth,
                          params->output_activation_min,
                          params->output_activation_max);
  } else {
    int computations = filter_height * filter_width
                       * output_height * output_width * output_depth;
    if ((computations / mvp_prog_cnt) < 120 ) {
      // Force fallback to other algorithms by reporting invalid status.
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  return status;
}

} // extern "C"

} // namespace
