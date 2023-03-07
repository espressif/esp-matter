/***************************************************************************//**
 * @file
 * @brief MVP add kernel driver.
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

#include "sl_mvp_ml_add.h"
#include "sl_mvp.h"
#include "sl_mvp_math.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"
#include <stdbool.h>
#include <math.h>

sl_status_t sli_mvp_ml_add_s8(const sli_mvp_ml_add_s8_params_t *params)
{
  if (!params) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_mvp_program_t *prog = sli_mvp_get_program_area_single();

  int remaining = params->length;
  const int8_t *input1_data = params->input1;
  const int8_t *input2_data = params->input2;
  int8_t *output_data = params->output;
  float input1_offset_scaled = params->input1_offset * params->input1_multiplier;
  float input2_offset_scaled = params->input2_offset * params->input2_multiplier;

  // *INDENT-OFF*
  /*
     Software Reference:

     This is the reference algorithm for the add operation. Note that in the 8 bit
     version of add there are 3 different quantization parameters. This is why the
     add operation is more complex than a simple addition. We first have to convert
     each of the input by de-quantization before we can perform the add operation.
     After the add operation we have to quantize the output using the output specific
     quantization parameters.

     for (size_t i = 0; i < params->length; ++i) {
       float input1_val = (input1[i] + input1_offset) * input1_scale;
       float input2_val = (input2[i] + input2_offset) * input2_scale;
       float output_val = (input1_val + input2_val) / output_scale + output_zero_point;
       output_val = round(output_val);
       output[i] = clamp(output_val, activation_min, activation_max);
     }

     We can get the core loop without the clamp operation down to 4 instructions on
     the MVP hardware by implementing the following algorithm.

     float16_t input1_offset_scaled = input1_offset * input1_scale;
     float16_t input2_offset_scaled = input2_offset * input2_scale;
     float16_t output_multiplier = 1 / output_scale;

     for (size_t i = 0; i < params->length; ++i) {
       float16_t input1_val = input1[i] * input1_scale + input1_offset_scaled;
       float16_t input2_val = input2[i] * input2_scale + input2_offset_scaled;
       float16_t output_val = input1_val + input2_val;
       output[i] = output_val * output_multiplier + output_offset;
     }

     Here is a representation of the MVP program. We divide the
     input into batches that can fit within one MVP program.

     Arrays: {
       A0 = input1[batch_size]
       A1 = input2[batch_size]
       A2 = output[batch_size]
     }
     Regs: {
       R0 = input1_offset_scaled
       R1 = input1_scale
       R2 = input2_offset_scaled
       R3 = input2_scale
       R4 = output_offset
       R5 = output_multiplier
     }
     Loop0: cnt=batch_size {
       R6 = input1[A0.Dim0++]
       R6 = R6 * R1 + R0
       R7 = input2[A1.Dim0++]
       R7 = R7 * R3 + R2
       R6 = R6 + R7
       R7 = R6 * R5 + R3
       output[A2.Dim0++] = R7 // Store output
     }

   */
  // *INDENT-ON*

  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R0, input1_offset_scaled, input1_offset_scaled);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R1, params->input1_multiplier, params->input1_multiplier);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R2, input2_offset_scaled, input2_offset_scaled);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R3, params->input2_multiplier, params->input2_multiplier);
  sli_mvp_prog_set_reg_s32c(prog, SLI_MVP_R4, params->output_offset, params->output_offset);
  sli_mvp_prog_set_reg_f32c(prog, SLI_MVP_R5, params->output_multiplier, params->output_multiplier);

  // Instruction 0: Load and adjust input1 value
  // Load(Array0, R6)
  // INC(Array0, Dim0)
  // R6 = MACR2A(R6,R1,R0)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(0),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6)
                         | SLI_MVP_ALU_Y(SLI_MVP_R1)
                         | SLI_MVP_ALU_A(SLI_MVP_R0)
                         | SLI_MVP_ALU_Z(SLI_MVP_R6),
                         SLI_MVP_LOAD(0, SLI_MVP_R6, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                         0, 0);

  // Instruction 1: Load and adjust input2 value
  // LOAD(Array1,R7)
  // INC(Array0,Dim0)
  // R7 = MACR2A(R7,R3,R2)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(1),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R7)
                         | SLI_MVP_ALU_Y(SLI_MVP_R3)
                         | SLI_MVP_ALU_A(SLI_MVP_R2)
                         | SLI_MVP_ALU_Z(SLI_MVP_R7),
                         SLI_MVP_LOAD(0, SLI_MVP_R7, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         0, 0);

  // Instruction 2: Add input1 and input2
  // R6 = ADDC(R6,R7)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(2),
                         SLI_MVP_OP(ADDC),
                         SLI_MVP_ALU_X(SLI_MVP_R6) // X (R6) - input1 (de-quantized)
                         | SLI_MVP_ALU_A(SLI_MVP_R7) // A (R7) - input2 (de-quantized)
                         | SLI_MVP_ALU_Z(SLI_MVP_R6), // Z (R6) - result (R6 + R7)
                         0, 0, 0);

  // Instruction 3: Adjust and store output value
  // R7 = MACR2A(R6,R5,R4)
  // STORE(Array2, R7)
  // INC(Array2, Dim0)
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(3),
                         SLI_MVP_OP(MACR2A),
                         SLI_MVP_ALU_X(SLI_MVP_R6) // X (R6) - result of previous ADDC operation
                         | SLI_MVP_ALU_Y(SLI_MVP_R5) // Y (R5) - output_scale
                         | SLI_MVP_ALU_A(SLI_MVP_R4) // A (R4) - output_offset
                         | SLI_MVP_ALU_Z(SLI_MVP_R7), // Z (R7) - output (R6 * R5 + R3)
                         0,
                         SLI_MVP_STORE(SLI_MVP_R7, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_ENDPROG);

  while (remaining >= 2) {
    // Process batch sizes of max 1024, this means 2048 actual values
    // since we operate on 2 elements on each loop iteration.
    int num_elements = SL_MIN(2048, remaining);
    int batch_size = num_elements / 2;
    num_elements = batch_size * 2;

    // Configure the input and output arrays
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(0), (int8_t *)input1_data, SLI_MVP_DATATYPE_COMPLEX_INT8, batch_size);
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(1), (int8_t *)input2_data, SLI_MVP_DATATYPE_COMPLEX_INT8, batch_size);
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(2), output_data, SLI_MVP_DATATYPE_COMPLEX_INT8, batch_size);

    // Loop 0 is iterating over elements
    sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(0),
                          batch_size,
                          SLI_MVP_INSTR(0),
                          SLI_MVP_INSTR(3),
                          0);

    sli_mvp_execute(prog, true);

    input1_data += num_elements;
    input2_data += num_elements;
    output_data += num_elements;
    remaining -= num_elements;
  }

  // When length is an odd number we will get 1 element left
  if (remaining == 1) {
    int input1 = *input1_data + params->input1_offset;
    int input2 = *input2_data + params->input2_offset;
    float sum = input1 * params->input1_multiplier + input2 * params->input2_multiplier;
    sum = sum * params->output_multiplier + params->output_offset;
    sum = round(sum);
    if (sum > 127.0) {
      *output_data = 127;
    } else if (sum < -128.0) {
      *output_data = -128;
    } else {
      *output_data = (int8_t) sum;
    }
  }

  sli_mvp_math_clamp_i8(params->output, params->length, params->activation_min, params->activation_max);

  return SL_STATUS_OK;
}

bool sli_mvp_ml_add_s8_is_supported(const sli_mvp_ml_add_s8_params_t *params)
{
  if (!params) {
    return false;
  } else {
    return true; // everything is supported
  }
}
