/***************************************************************************//**
 * @file
 * @brief MVP Math functions.
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
#include "sl_mvp_math.h"
#include "sl_mvp.h"
#include "sl_mvp_program_area.h"
#include "sl_common.h"

static void sli_clamp_i8(int8_t *data, size_t len, int8_t min, int8_t max);

void sli_mvp_math_vector_add_i8(const int8_t *input_a, const int8_t *input_b, int8_t *output, size_t len)
{
  sli_mvp_program_t *program = sli_mvp_get_program_area_single();

  // Configure 3 Arrays: Array0 = input_a Array1 = input_b Array2 = output
  sli_mvp_prog_set_vector(program, SLI_MVP_ARRAY(0), (void *)input_a, SLI_MVP_DATATYPE_INT8, len);
  sli_mvp_prog_set_vector(program, SLI_MVP_ARRAY(1), (void *)input_b, SLI_MVP_DATATYPE_INT8, len);
  sli_mvp_prog_set_vector(program, SLI_MVP_ARRAY(2), output, SLI_MVP_DATATYPE_INT8, len);

  // Instruction 0
  // LOAD(Array0,R1)
  // LOAD(Array1,R2)
  // R3 = ADDC(R1,_,R2)
  // STORE(Array2, R3)
  sli_mvp_prog_set_instr(program,
                         SLI_MVP_INSTR(0),
                         SLI_MVP_OP(ADDC),
                         SLI_MVP_ALU_X(SLI_MVP_R1)
                         | SLI_MVP_ALU_A(SLI_MVP_R2)
                         | SLI_MVP_ALU_Z(SLI_MVP_R3),
                         SLI_MVP_LOAD(0, SLI_MVP_R1, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL)
                         | SLI_MVP_LOAD(1, SLI_MVP_R2, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_STORE(SLI_MVP_R3, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_ENDPROG);

  // Loop 0 is the outer loop, counting down columns
  sli_mvp_prog_set_loop(program,
                        SLI_MVP_LOOP(0),
                        len,          // iterate (once for each row)
                        SLI_MVP_INSTR(0), // starts at Instruction 0
                        SLI_MVP_INSTR(0), // ends at Instruction 0
                        SLI_MVP_NONE); // No increments with this loop

  sli_mvp_execute(program, true);
}

void sli_mvp_math_clamp_i8(int8_t *data, size_t len, int8_t min, int8_t max)
{
  size_t remaining = len;
  const size_t threshold = 160; // non-mvp algorithm is faster bellow this threshold
  sli_mvp_program_t *prog = sli_mvp_get_program_area_single();

  if ((min == -128) && (max == 127)) {
    return; // No need to process
  }

  // Small datasets are faster to handle by cpu due to overhead of hw accelerator
  if (len <= threshold) {
    sli_clamp_i8(data, remaining, min, max);
    return;
  }

  // Arrays:
  // -------
  // A0 - input (data)
  // A1 - output (data)
  //
  // Registers:
  // ----------
  // R0 - min value
  // R1 - max value
  // R2 - data_i0
  // R3 - data_i1
  //
  // All datatypes used is int8_t complex so all loads/stores and operations
  // will operate on 2 int8_t elements at a time. The loop of the algorithm
  // will write 4 elements on each iteration.
  //
  // Algorithm:
  // ----------
  // R0 = min
  // R1 = max
  // R2 = Load(A0), A0.Dim0++
  // Loop0: cnt=n/4 {
  //   R3 = Load(A0), A0.Dim0++
  //   R2 = CLIP2A(R0,R1,R2)
  //   Store(A1,R2), A1.Dim0++
  //   R2 = Load(A0), A0.Dim0++
  //   R3 = CLIP2A(R0,R1,R3)
  //   Store(A1,R3), A1.Dim0++

  sli_mvp_prog_set_reg_s8c(prog, SLI_MVP_R0, min, min);
  sli_mvp_prog_set_reg_s8c(prog, SLI_MVP_R1, max, max);

  // R2 = Load(A0), A0.Dim0++
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(0),
                         SLI_MVP_OP(NOOP),
                         0,
                         SLI_MVP_LOAD(0, SLI_MVP_R2, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                         0,
                         0);

  // R3 = Load(A0), A0.Dim0++
  // R2 = CLIP2A(R0,R1,R2)
  // Store(A1,R2), A1.Dim0++
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(1),
                         SLI_MVP_OP(CLIP2A),
                         SLI_MVP_ALU_X(SLI_MVP_R0) // X (R0) - Min value
                         | SLI_MVP_ALU_Y(SLI_MVP_R1) // Y (R1) - Max value
                         | SLI_MVP_ALU_A(SLI_MVP_R2) // A (R2) - input
                         | SLI_MVP_ALU_Z(SLI_MVP_R2), // Z (R2) - result
                         SLI_MVP_LOAD(0, SLI_MVP_R3, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_STORE(SLI_MVP_R2, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         0);

  // R2 = Load(A0), A0.Dim0++
  // R3 = CLIP2A(R0,R1,R3)
  // Store(A1,R3), A1.Dim0++
  sli_mvp_prog_set_instr(prog, SLI_MVP_INSTR(2),
                         SLI_MVP_OP(CLIP2A),
                         SLI_MVP_ALU_X(SLI_MVP_R0) // X (R0) - Min value
                         | SLI_MVP_ALU_Y(SLI_MVP_R1) // Y (R1) - Max value
                         | SLI_MVP_ALU_A(SLI_MVP_R3) // A (R2) - input
                         | SLI_MVP_ALU_Z(SLI_MVP_R3), // Z (R2) - result
                         SLI_MVP_LOAD(0, SLI_MVP_R2, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_STORE(SLI_MVP_R3, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_COL),
                         SLI_MVP_ENDPROG);

  while (remaining >= threshold) {
    // We can process a maximum of 2048 elements in each mvp program.
    size_t max_elements = SL_MIN(2048u, remaining);
    size_t batch_size = max_elements / 4; // each loop will process 4 elements
    size_t num_elements = batch_size * 4;
    size_t array_size = num_elements / 2;

    // Configure input/output arrays
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(0), data, SLI_MVP_DATATYPE_COMPLEX_INT8, array_size);
    sli_mvp_prog_set_vector(prog, SLI_MVP_ARRAY(1), data, SLI_MVP_DATATYPE_COMPLEX_INT8, array_size);

    // Loop 0 is iterating over the batch size, handling 4 elements at a time
    sli_mvp_prog_set_loop(prog, SLI_MVP_LOOP(0),
                          batch_size,
                          SLI_MVP_INSTR(1),
                          SLI_MVP_INSTR(2),
                          SLI_MVP_NONE);

    sli_mvp_execute(prog, true);

    data += num_elements;
    remaining -= num_elements;
  }

  // Handle the remaining elements in software
  if (remaining > 0) {
    sli_clamp_i8(data, remaining, min, max);
  }

  sli_mvp_wait_for_completion();
}

static void sli_clamp_i8(int8_t *data, size_t len, int8_t min, int8_t max)
{
  for (size_t i = 0; i < len; i++) {
    int8_t v = data[i];
    if (v < min) {
      data[i] = min;
    } else if (v > max) {
      data[i] = max;
    }
  }
}
