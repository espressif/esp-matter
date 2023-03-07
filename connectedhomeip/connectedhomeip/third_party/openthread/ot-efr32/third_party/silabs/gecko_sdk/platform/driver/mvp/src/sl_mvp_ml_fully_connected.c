/***************************************************************************//**
 * @file
 * @brief MVP fully connected kernel driver.
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

#include "sl_mvp_ml_fully_connected.h"
#include "sl_mvp.h"
#include "sl_mvp_math.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"
#include <stdbool.h>

static const float16_t zero = 0.0f;
static sl_status_t factorize_number(int number, int max_factor, int *n, int *m);
static sl_status_t sli_mvp_ml_fully_connected_s8_small_input(const sli_mvp_ml_fully_connected_s8_params_t *params);
static sl_status_t sli_mvp_ml_fully_connected_s8_large_input(const sli_mvp_ml_fully_connected_s8_params_t *params);

sl_status_t sli_mvp_ml_fully_connected_s8(const sli_mvp_ml_fully_connected_s8_params_t *params)
{
  if (!params) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (params->weight_offset != 0) {
    // The quantization specification requires the zero point value to be 0.
    // This means that the weight offset (negative zero point) argument to this
    // function will also be 0.
    // https://www.tensorflow.org/lite/performance/quantization_spec
    EFM_ASSERT(false);
    return SL_STATUS_INVALID_PARAMETER;
  }

  int accum_depth = params->weight_shape.dim[1];
  int input_threshold = 1024;
  if (accum_depth % 2 == 0) {
    input_threshold = 2048;
  }

  if (accum_depth <= input_threshold) {
    return sli_mvp_ml_fully_connected_s8_small_input(params);
  } else {
    return sli_mvp_ml_fully_connected_s8_large_input(params);
  }
}

static sl_status_t sli_mvp_ml_fully_connected_s8_small_input(const sli_mvp_ml_fully_connected_s8_params_t *params)
{
  sli_mvp_program_t *prog = sli_mvp_get_program_area_single();

  int32_t output_offset = params->output_offset;
  float16_t output_multiplier = params->output_multiplier;
  float acc_scaler = SLI_MVP_ACCUMULATOR_SCALER;
  float input_offset_scaled = params->input_offset * acc_scaler;

  int batches = params->output_shape.dim[0];
  int output_depth = params->output_shape.dim[1];
  int accum_depth = params->weight_shape.dim[1];
  size_t output_len = output_depth * batches;
  bool use_parallel_mac = (accum_depth % 2) == 0;
  int inst_cnt = 0;
  int loop_cnt = accum_depth;
  if (use_parallel_mac) {
    loop_cnt = accum_depth / 2;
  }

/*
   Software Reference:

   This is the reference algorithm. Note that there are some differences between the
   reference and the MVP implementation. The accumulator (acc) is 32 bits in the software
   reference while the MVP accumulator is float16_t. The bias values are originally an
   array of 32 bit values, however this is not supported by the MVP so float16_t is used
   instead. Another difference is that we scale down all the numbers during the inner loop
   by 1/65503 in order get a better range. This also means that we have to scale down the
   bias values before they are added to the accumulator. When the output value is calculated
   we scale the values back up by multiplying by 65503.

   for (int b = 0; b < batches; ++b) {
    for (int out_c = 0; out_c < output_depth; ++out_c) {
      int32_t acc = 0;
      for (int d = 0; d < accum_depth; ++d) {
        int32_t input_val = params->input[b * accum_depth + d];
        int32_t weight_val = params->weight[out_c * accum_depth + d];
        acc += (weight_val + weight_offset) * (input_val + input_offset);
      }
      if (params->bias) {
        acc += params->bias[out_c];
      }
      acc = round(acc * multiplier);
      acc += output_offset;
      acc = SL_MAX(acc, params->activation_min);
      acc = SL_MIN(acc, params->activation_max);
      params->output[out_c + output_depth * b] = (int8_t)acc;
    }
   }

   Register Allocation
    R0 - acc_scaler
    R1 - input_offset_scaled
    R3 - multiplier
    R4 - output_offset
    R5 - accumulator/output_scaled
    R6 - input_i
    R7 - weight_i/weight_scaled

   Array Allocation
    A0 - int8_t/int8_t[2]    input[b][a]
    A1 - int8_t/int8_t[2]    weight[o][a]
    A2 - float16_t bias[o] (optional)
    A3 - int8_t    output[b][o]

   b - batches (number of batches, usually 1)
   a - accum_depth (number of input nodes)
   o - output_depth (number of output nodes)

   Loop0: cnt=batches {
    Loop1: cnt=output_depth {
      I0: R5 = bias[A2.Dim0++]
      Loop2: cnt=accum_depth {
        I1: R6 = input[A0.Dim1][A0.Dim2++]
            R7 = weight[A1.Dim1][A1.Dim2++]
            R6 = R6 * R0 + R1
        I2: R5 = R6 * R7 + R5
      } : {A1.Dim1++}
      I3: R5 = R5 * R3 + R4
          output[A3.Dim1][A3.Dim0++] = R5 // Store output
    } : {A0.Dim1++, A3.Dim1++}
   }

 */

  if (use_parallel_mac) {
    sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(0), (int8_t *)params->input, SLI_MVP_DATATYPE_COMPLEX_INT8, batches, accum_depth / 2);
    sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(1), (int8_t *)params->weight, SLI_MVP_DATATYPE_COMPLEX_INT8, output_depth, accum_depth / 2);
  } else {
    sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(0), (int8_t *)params->input, SLI_MVP_DATATYPE_INT8, batches, accum_depth);
    sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(1), (int8_t *)params->weight, SLI_MVP_DATATYPE_INT8, output_depth, accum_depth);
  }

  if (params->bias) {
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(2), (float16_t *)params->bias, SLI_MVP_DATATYPE_BINARY16, params->bias_length);
  } else {
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(2), (float16_t *)&zero, SLI_MVP_DATATYPE_BINARY16, 1);
  }
  sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(3), params->output, SLI_MVP_DATATYPE_INT8, batches, output_depth);

  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R0, acc_scaler, acc_scaler);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R1, input_offset_scaled, input_offset_scaled);
  sli_mvp_prog_set_reg_f16(prog, SLI_MVP_R3, output_multiplier);
  sli_mvp_prog_set_reg_s32(prog, SLI_MVP_R4, output_offset);

  // Instruction 0: acc = bias[A2.Dim2++]
  // LOAD(Array2,R2)
  // INC(Array2,Dim2)
  // NOOP()
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(NOOP),
                         0,
                         SLI_MVP_LOAD(0, SLI_MVP_R5, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_COL),
                         0,
                         0);
  // Instruction 1:
  //   input  = input[A0.Dim1][A0.Dim2++]
  //   weight = weight[A1.Dim1][A1.Dim2++]
  //   input  = input * acc_scaler + input_offset_scaled
  // LOAD(Array0,R6)
  // LOAD(Array1,R7)
  // R6 = MACR2A(R6,R0,R1)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6)
                         | SLI_MVP_ALU_Y(SLI_MVP_R0)
                         | SLI_MVP_ALU_A(SLI_MVP_R1)
                         | SLI_MVP_ALU_Z(SLI_MVP_R6),
                         SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL)
                         | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         0,
                         0);
  // Instruction 2: acc += input * weight
  // R5 = MACR2A(R6,R7,R5)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6)
                         | SLI_MVP_ALU_Y(SLI_MVP_R7)
                         | SLI_MVP_ALU_A(SLI_MVP_R5)
                         | SLI_MVP_ALU_Z(SLI_MVP_R5),
                         0,
                         0,
                         0);
  if (use_parallel_mac) {
    // Instruction 3: acc = acc.real + acc.imag
    // R5 = ADDR(R5)
    sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                           SLI_MVP_OP(ADDR),
                           SLI_MVP_ALU_A(SLI_MVP_R5)
                           | SLI_MVP_ALU_Z(SLI_MVP_R5),
                           0,
                           0,
                           0);
  }
  // Instruction 3/4: output[A3.Dim1][A3.Dim2++] = acc * multiplier + output_offset
  // R5 = MACR2A(R5,R3,R4)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R5)
                         | SLI_MVP_ALU_Y(SLI_MVP_R3)
                         | SLI_MVP_ALU_A(SLI_MVP_R4)
                         | SLI_MVP_ALU_Z(SLI_MVP_R5),
                         0,
                         SLI_MVP_STORE(SLI_MVP_R5, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_ENDPROG);

  /*
     Loop Allocation:
     L0 - Loop over batches.
     L1 - Loop over output_depth (output node).
      inside: output[Dim1][Dim0] is stored and Dim0 is incremented, moving to next output node.
      on exit: Increment Dim1 on input and output matrix, moving to next batch.
     L2 - Loop over accum_depth (input node).
      inside: input[Dim1][Dim0] and weight[Dim1][Dim0] are loaded and Dim0 are incremented for each iteration.
      on exit: Increment Dim1 on weight matrix moving to next row. All increments handled by loads.
   */
  int last_inst = inst_cnt - 1;
  sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(0),
                        batches,
                        SLI_MVP_INSTR(0),
                        SLI_MVP_INSTR(last_inst),
                        0);
  sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(1),
                        output_depth,
                        SLI_MVP_INSTR(0),
                        SLI_MVP_INSTR(last_inst),
                        SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW)
                        | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_ROW));
  sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(2),
                        loop_cnt,
                        SLI_MVP_INSTR(1),
                        SLI_MVP_INSTR(2),
                        SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_ROW));

  sli_mvp_execute(prog, true);
  sli_mvp_math_clamp_i8(params->output, output_len, params->activation_min, params->activation_max);

  return SL_STATUS_OK;
}

sl_status_t sli_mvp_ml_fully_connected_bias_convert(const int32_t *bias, float16_t *dst, size_t len)
{
  sl_status_t status = SL_STATUS_OK;

  if (bias) {
    for (size_t i = 0; i < len; i++) {
      float value = (float)bias[i] * SLI_MVP_ACCUMULATOR_SCALER;
      if ((value > SLI_MVP_FP16_MAX) || (value < SLI_MVP_FP16_MIN)) {
        status = SL_STATUS_INVALID_PARAMETER;
      }
      dst[i] = value;
    }
  }
  return status;
}

float16_t sli_mvp_ml_fully_connected_output_multiplier(double multiplier)
{
  double output_multiplier = multiplier * SLI_MVP_ACCUMULATOR_MULTIPLIER;
  if (output_multiplier > SLI_MVP_FP16_MAX) {
    return SLI_MVP_FP16_MAX;
  } else if (output_multiplier < SLI_MVP_FP16_MIN) {
    return SLI_MVP_FP16_MIN;
  } else {
    return output_multiplier;
  }
}

bool sli_mvp_ml_fully_connected_s8_is_supported(const sli_mvp_ml_fully_connected_s8_params_t *params)
{
  if (!params) {
    return false;
  }

  // Each MVP array dimension is limited to 10 bits in size (1024).
  int batches = params->output_shape.dim[0];
  int output_depth = params->output_shape.dim[1];
  int accum_depth = params->weight_shape.dim[1];
  int loop_cnt = accum_depth;
  int max_accum_depth = SLI_MVP_MAX_COLUMN_LENGTH;
  if ((accum_depth % 2) == 0) {
    // When parallell execution is possible we can handle 2048 elements when accumulating
    max_accum_depth = SLI_MVP_MAX_COLUMN_LENGTH * 2;
    loop_cnt = accum_depth / 2;
  }

  if (accum_depth > max_accum_depth) {
    // Algorithm that supports large input will be used so we have less restrictions
    sl_status_t ok;
    int n, m;
    ok = factorize_number(loop_cnt, 1024, &n, &m);
    return (ok == SL_STATUS_OK) && (n <= 1024) && (m <= 1024);
  } else {
    return ((batches >= 1) && (batches <= (int)SLI_MVP_MAX_COLUMN_LENGTH))
           && ((output_depth >= 1) && (output_depth <= (int)SLI_MVP_MAX_COLUMN_LENGTH))
           && ((accum_depth >= 1) && (accum_depth <= max_accum_depth))
           && (params->bias_length <= (int)SLI_MVP_MAX_COLUMN_LENGTH);
  }
}

static sl_status_t sli_mvp_ml_fully_connected_s8_large_input(const sli_mvp_ml_fully_connected_s8_params_t *params)
{
  sli_mvp_program_t *prog = sli_mvp_get_program_area_single();

  int32_t output_offset = params->output_offset;
  float16_t output_multiplier = params->output_multiplier;
  float acc_scaler = SLI_MVP_ACCUMULATOR_SCALER;
  float input_offset_scaled = params->input_offset * acc_scaler;

  int batches = params->output_shape.dim[0];
  int output_depth = params->output_shape.dim[1];
  int accum_depth = params->weight_shape.dim[1];
  size_t output_len = output_depth * batches;
  bool use_parallel_mac = (accum_depth % 2) == 0;
  int inst_cnt = 0;

/*
   In this algorithm we iterate over each output node and produce
   one MVP program for each output. In this way we can support larger
   input sizes by using two array dimensions and two loops to iterate
   over use a larger amount of input and weight values.

   Register Allocation
    R0 - acc_scaler
    R1 - input_offset_scaled
    R3 - multiplier
    R4 - output_offset
    R5 - accumulator/output_scaled
    R6 - input_i
    R7 - weight_i/weight_scaled

   Array Allocation
    A0 - int8_t/int8_t[2]    input[n][m]
    A1 - int8_t/int8_t[2]    weight[n][m]
    A3 - int8_t              output[1]

   The accum_depth is divided into n*m to fit into the hardware
   dimension registers. The following program is repeated for
   each output node and for each iteration a new row from the
   weight matrix is used. An outer loop will handle batches if
   there are more than 1.

   R5 = bias[out_n]
   Loop0: cnt=n {}
    Loop1: cnt=m {
      I1: R6 = input[n][m++]
          R7 = weight_row[n][m++]
          R6 = R6 * R0 + R1
      I2: R5 = R6 * R7 + R5
   } : {n++}
   I3: R5 = R5 * R3 + R4
    output[0] = R5 // Store output
 */

  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R0, acc_scaler, acc_scaler);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R1, input_offset_scaled, input_offset_scaled);
  sli_mvp_prog_set_reg_f16(prog, SLI_MVP_R3, output_multiplier);
  sli_mvp_prog_set_reg_s32(prog, SLI_MVP_R4, output_offset);

  int n, m;
  int loop_cnt = accum_depth;
  sli_mvp_datatype_t input_type = SLI_MVP_DATATYPE_INT8;

  if (use_parallel_mac) {
    // accumulation loop will process two number in parallel
    loop_cnt = accum_depth / 2;
    input_type = SLI_MVP_DATATYPE_COMPLEX_INT8;
  }

  if (factorize_number(loop_cnt, 1024, &n, &m) != SL_STATUS_OK) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Instruction 0:
  //   input  = input[A0.Dim1][A0.Dim2++]
  //   weight = weight[A1.Dim1][A1.Dim2++]
  //   input  = input * ACCUMULATOR_SCALER + input_offset_scaled
  // LOAD(Array0,R6)
  // LOAD(Array1,R7)
  // R6 = MACR2A(R6,R0,R1)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6)
                         | SLI_MVP_ALU_Y(SLI_MVP_R0)
                         | SLI_MVP_ALU_A(SLI_MVP_R1)
                         | SLI_MVP_ALU_Z(SLI_MVP_R6),
                         SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL)
                         | SLI_MVP_LOAD(1, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         0,
                         0);
  // Instruction 1: acc += input * weight
  // R5 = MACR2A(R6,R7,R5)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6)
                         | SLI_MVP_ALU_Y(SLI_MVP_R7)
                         | SLI_MVP_ALU_A(SLI_MVP_R5)
                         | SLI_MVP_ALU_Z(SLI_MVP_R5),
                         0,
                         0,
                         0);
  if (use_parallel_mac) {
    // Instruction 2: acc = acc.real + acc.imag
    // R5 = ADDR(R5)
    sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                           SLI_MVP_OP(ADDR),
                           SLI_MVP_ALU_A(SLI_MVP_R5)
                           | SLI_MVP_ALU_Z(SLI_MVP_R5),
                           0,
                           0,
                           0);
  }
  // Instruction 2/3: output[A3.Dim1][A3.Dim2++] = acc * multiplier + output_offset
  // R5 = MACR2A(R5,R3,R4)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(inst_cnt++),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R5)
                         | SLI_MVP_ALU_Y(SLI_MVP_R3)
                         | SLI_MVP_ALU_A(SLI_MVP_R4)
                         | SLI_MVP_ALU_Z(SLI_MVP_R5),
                         0,
                         SLI_MVP_STORE(SLI_MVP_R5, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_ENDPROG);

  // Factorize accum_depth into two factors that are small enough
  // to fit in the MVP dimension registers.
  sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(0),
                        n,
                        SLI_MVP_INSTR(0),
                        SLI_MVP_INSTR(1),
                        0);
  sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(1),
                        m,
                        SLI_MVP_INSTR(0),
                        SLI_MVP_INSTR(1),
                        SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_ROW)
                        | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_ROW));

  for (int b = 0; b < batches; b++) {
    for (int i = 0; i < output_depth; i++) {
      // Iterate over each output
      const int8_t *input = &params->input[b * accum_depth];
      const int8_t *weight_row = &params->weight[i * accum_depth];
      float16_t bias_value = .0f;
      if (params->bias) {
        bias_value = params->bias[i];
      }
      int8_t *output_ptr = &params->output[b * output_depth + i];

      // Represent the input and weight row by using two dimensions n*m=accum_depth
      // this is done in order to make it fit in the MVP hardware as a single program.
      sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(0), (int8_t *)input, input_type, n, m);
      sli_mvp_prog_set_matrix(prog, SLI_MVP_ARRAY(1), (int8_t *)weight_row, input_type, n, m);
      sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(3), output_ptr, SLI_MVP_DATATYPE_INT8, 1);
      sli_mvp_prog_set_reg_f16(prog, SLI_MVP_R5, bias_value);
      sli_mvp_execute(prog, true);
    }
  }

  sli_mvp_math_clamp_i8(params->output, output_len, params->activation_min, params->activation_max);

  return SL_STATUS_OK;
}

static sl_status_t factorize_number(int number, int max_factor, int *n, int *m)
{
  static const uint8_t primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
  // Try to split the number into two factors that are less than or equal to max_factor.
  int a = 1;
  int b = number;
  sl_status_t status = SL_STATUS_OK;

  while (b > max_factor) {
    int b_prev = b;
    for (size_t i = 0; i < (sizeof(primes) / sizeof(primes[0])); i++) {
      int f = primes[i];
      if (b % f == 0) {
        a = a * f;
        b = b / f;
        break;
      }
    }
    if (b == b_prev) {
      // No more factors found
      status = SL_STATUS_FAIL;
      break;
    }
  }

  *n = a;
  *m = b;
  return status;
}
