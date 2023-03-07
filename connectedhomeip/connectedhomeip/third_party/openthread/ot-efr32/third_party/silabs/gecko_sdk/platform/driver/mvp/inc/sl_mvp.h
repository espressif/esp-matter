/***************************************************************************//**
 * @file
 * @brief MVP Core driver.
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
#ifndef SL_MVP_H
#define SL_MVP_H

#include "sl_status.h"
#include "em_device.h"
#include "sl_mvp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

// Defines for each of the 8 MVP registers.
#define SLI_MVP_R0 0
#define SLI_MVP_R1 1
#define SLI_MVP_R2 2
#define SLI_MVP_R3 3
#define SLI_MVP_R4 4
#define SLI_MVP_R5 5
#define SLI_MVP_R6 6
#define SLI_MVP_R7 7

// Defines for Loop/Stream increments.
#define SLI_MVP_NOINCR   0x0
#define SLI_MVP_INCRDIM0 0x1
#define SLI_MVP_INCRDIM1 0x2
#define SLI_MVP_INCRDIM2 0x4
#define SLI_MVP_INCRDIM_VEC SLI_MVP_INCRDIM0
#define SLI_MVP_INCRDIM_ROW SLI_MVP_INCRDIM1
#define SLI_MVP_INCRDIM_COL SLI_MVP_INCRDIM2

// Alternative INCRDIM macros, useful when using "NHWC" tensors.
#define SLI_MVP_INCRDIM_DEPTH   SLI_MVP_INCRDIM_VEC
#define SLI_MVP_INCRDIM_HEIGHT  SLI_MVP_INCRDIM_ROW
#define SLI_MVP_INCRDIM_WIDTH   SLI_MVP_INCRDIM_COL

// Defines for Loop dimension resets.
#define SLI_MVP_NO_DIM_RESET  0x0
#define SLI_MVP_RESETDIM0 0x1
#define SLI_MVP_RESETDIM1 0x2
#define SLI_MVP_RESETDIM2 0x4
#define SLI_MVP_RESETDIM_VEC SLI_MVP_RESETDIM0
#define SLI_MVP_RESETDIM_ROW SLI_MVP_RESETDIM1
#define SLI_MVP_RESETDIM_COL SLI_MVP_RESETDIM2

// Alternative RESETDIM macros, useful when using "NHWC" tensors.
#define SLI_MVP_RESETDIM_DEPTH  SLI_MVP_RESETDIM_VEC
#define SLI_MVP_RESETDIM_HEIGHT SLI_MVP_RESETDIM_ROW
#define SLI_MVP_RESETDIM_WIDTH  SLI_MVP_RESETDIM_COL

// Defines for Instruction RegLoad/RegStore selection (for STREAM macro).
#define SLI_MVP_REGLOAD0 0
#define SLI_MVP_REGLOAD1 1
#define SLI_MVP_REGSTORE 2

// Defines for ALUIN macro.
#define SLI_MVP_ALUIN0  0
#define SLI_MVP_ALUIN1  1
#define SLI_MVP_ALUIN2  2
#define SLI_MVP_ENDPROG 1
#define SLI_MVP_NONE    0

// Defines for ALU register modifiers.
#define SLI_MVP_ALUIN_REALZERO   0x1
#define SLI_MVP_ALUIN_REALNEGATE 0x2
#define SLI_MVP_ALUIN_IMAGZERO   0x4
#define SLI_MVP_ALUIN_IMAGNEGATE 0x8

// MVP hardware limits.
#define SLI_MVP_MAX_DIM            (3U)
#define SLI_MVP_MAX_VECTOR_COUNT   ((_MVP_ARRAYDIM0CFG_SIZE_MASK >> _MVP_ARRAYDIM0CFG_SIZE_SHIFT)+ 1U)
#define SLI_MVP_MAX_ROW_LENGTH     ((_MVP_ARRAYDIM1CFG_SIZE_MASK >> _MVP_ARRAYDIM1CFG_SIZE_SHIFT)+ 1U)
#define SLI_MVP_MAX_COLUMN_LENGTH  ((_MVP_ARRAYDIM2CFG_SIZE_MASK >> _MVP_ARRAYDIM2CFG_SIZE_SHIFT)+ 1U)
#define SLI_MVP_MAX_VECTOR_STRIDE  ((_MVP_ARRAYDIM0CFG_STRIDE_MASK >> (_MVP_ARRAYDIM0CFG_STRIDE_SHIFT+1)))
#define SLI_MVP_MIN_VECTOR_STRIDE  (-MAX_VECTOR_STRIDE - 1)
#define SLI_MVP_MAX_ROW_STRIDE     ((_MVP_ARRAYDIM1CFG_STRIDE_MASK >> (_MVP_ARRAYDIM1CFG_STRIDE_SHIFT+1)))
#define SLI_MVP_MIN_ROW_STRIDE     (-MAX_ROW_STRIDE - 1)
#define SLI_MVP_MAX_COLUMN_STRIDE  ((_MVP_ARRAYDIM2CFG_STRIDE_MASK >> (_MVP_ARRAYDIM2CFG_STRIDE_SHIFT+1)))
#define SLI_MVP_MIN_COLUMN_STRIDE  (-MAX_COLUMN_STRIDE - 1)

#define SLI_MVP_FP16_MIN               (-65504.0f)
#define SLI_MVP_FP16_MAX               65504.0f
#define SLI_MVP_ACCUMULATOR_MULTIPLIER (1 << 15)
#define SLI_MVP_ACCUMULATOR_SCALER     (1.0f / SLI_MVP_ACCUMULATOR_MULTIPLIER)

// Array id macro for readability.
#define SLI_MVP_ARRAY(n) (n)

// Loop id macro for readability.
#define SLI_MVP_LOOP(n) (n)

// Instruction id macro for readability.
#define SLI_MVP_INSTR(n) (n)

// Macro to build up ALUOUT field in CFG0.
#define SLI_MVP_ALUOUT(n) ((n) << _MVP_INSTRCFG0_ALUOUTREGID_SHIFT)

// Macro to build up ALUIN field in CFG0. The SLI_MVP_ALU_* macros should
// be used instead when building up the ALU configuration when no flags are used.
// a - 0=ALUIN0, 1=ALUIN1, 2=ALUIN2
// r - 0-7=R0-R7
// f - flags, combination of any of these:
//   SLI_MVP_ALUIN_REALZERO
//   SLI_MVP_ALUIN_REALNEGATE
//   SLI_MVP_ALUIN_IMAGZERO
//   SLI_MVP_ALUIN_IMAGNEGATE
#define SLI_MVP_ALUIN(a, r, f) ((((f) << 4) | (r)) << (8 * (a)))

// First argument to ALU operation. ALU Input X == ALUIN0.
#define SLI_MVP_ALU_X(reg) SLI_MVP_ALUIN(0, reg, 0)

// Second argument to ALU operation. ALU Input Y == ALUIN1.
#define SLI_MVP_ALU_Y(reg) SLI_MVP_ALUIN(1, reg, 0)

// Third argument to ALU operation. ALU Input A == ALUIN2.
#define SLI_MVP_ALU_A(reg) SLI_MVP_ALUIN(2, reg, 0)

// Used to specify the output register. ALU Output Z == ALUOUT.
#define SLI_MVP_ALU_Z(reg) SLI_MVP_ALUOUT(reg)

// Macro to build REGLOAD/REGSTORE field in CFG1.
// SLI_MVP_LOAD() or SLI_MVP_STORE() should be used when building instructions.
#define SLI_MVP_STREAM(s, r, a, i) ((((i) << 7) | ((a) << 4) | 0x8 | (r)) << ((s) * 10))

// Specify use of Load channel 0 or 1.
// Example: SLI_MVP_LOAD(0, SLI_MVP_R1, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM0)
#define SLI_MVP_LOAD(n, r, a, i)  SLI_MVP_STREAM(n, r, a, i)

// Used when storing a register as part of an instruction.
// Example: SLI_MVP_STORE(SLI_MVP_R3, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM0)
#define SLI_MVP_STORE(r, a, i)    SLI_MVP_STREAM(SLI_MVP_REGSTORE, r, a, i)

// Define for MVP operation used in instructions.
#define SLI_MVP_OP(x) (_MVP_INSTRCFG2_ALUOP_ ## x)

// Loop Increment macro.
#define SLI_MVP_LOOP_INCRDIM(array, dim) ((dim) << (4 * (array)))

/**
 * ALU Registers are always represented as a complex float16_t type.
 * This structure is used to allow a program to assign value to either
 * the real or imaginary part of the ALU register.
 */
typedef struct {
  struct {
    float16_t real;
    float16_t imag;
  } value;
} sli_mvp_alu_bitfield_t;

typedef struct {
  union {
    uint32_t REGSTATE; ///< ALU Rn Register.
    sli_mvp_alu_bitfield_t reg;
  };
} sli_mvp_alu_reg_t;

typedef struct
{
  uint32_t            ADDRCFG;  ///< Array Base Address
  union {
    uint32_t          DIM0CFG;  ///< Dimension 0 Configuration
    struct {
      uint32_t size       : 10;
      uint32_t            : 2;
      uint32_t basetype   : 2;
      uint32_t complex    : 1;
      uint32_t            : 1;
      uint32_t stride     : 12;
      uint32_t            : 4;
    } dim0cfg;
  };
  union {
    uint32_t          DIM1CFG;  ///< Dimension 1 Configuration
    struct {
      uint32_t size       : 10;
      uint32_t            : 6;
      uint32_t stride     : 12;
      uint32_t            : 4;
    } dim1cfg;
  };
  union {
    uint32_t          DIM2CFG;  ///< Dimension 2 Configuration
    struct {
      uint32_t size       : 10;
      uint32_t            : 6;
      uint32_t stride     : 12;
      uint32_t            : 4;
    } dim2cfg;
  };
} sli_mvp_array_reg_t;

typedef struct
{
  union {
    uint32_t          CFG;      ///< Loop Configuration
    struct {
      uint32_t numiters       : 10;
      uint32_t                : 2;
      uint32_t array0incrdim0 : 1;
      uint32_t array0incrdim1 : 1;
      uint32_t array0incrdim2 : 1;
      uint32_t                : 1;
      uint32_t array1incrdim0 : 1;
      uint32_t array1incrdim1 : 1;
      uint32_t array1incrdim2 : 1;
      uint32_t                : 1;
      uint32_t array2incrdim0 : 1;
      uint32_t array2incrdim1 : 1;
      uint32_t array2incrdim2 : 1;
      uint32_t                : 1;
      uint32_t array3incrdim0 : 1;
      uint32_t array3incrdim1 : 1;
      uint32_t array3incrdim2 : 1;
      uint32_t                : 1;
      uint32_t array4incrdim0 : 1;
      uint32_t array4incrdim1 : 1;
      uint32_t array4incrdim2 : 1;
      uint32_t                : 1;
    } cfg;
  };
  union {
    uint32_t          RST;      ///< Loop Reset
    struct {
      uint32_t                 : 12;
      uint32_t array0resetdim0 : 1;
      uint32_t array0resetdim1 : 1;
      uint32_t array0resetdim2 : 1;
      uint32_t                 : 1;
      uint32_t array1resetdim0 : 1;
      uint32_t array1resetdim1 : 1;
      uint32_t array1resetdim2 : 1;
      uint32_t                 : 1;
      uint32_t array2resetdim0 : 1;
      uint32_t array2resetdim1 : 1;
      uint32_t array2resetdim2 : 1;
      uint32_t                 : 1;
      uint32_t array3resetdim0 : 1;
      uint32_t array3resetdim1 : 1;
      uint32_t array3resetdim2 : 1;
      uint32_t                 : 1;
      uint32_t array4resetdim0 : 1;
      uint32_t array4resetdim1 : 1;
      uint32_t array4resetdim2 : 1;
      uint32_t                 : 1;
    } rst;
  };
} sli_mvp_loop_reg_t;

typedef struct {
  union {
    uint32_t          CFG0;      ///< Instruction Configuration Word 0.
    struct {
      uint32_t aluin0regid      : 3;
      uint32_t                  : 1;
      uint32_t aluin0realzero   : 1;
      uint32_t aluin0realnegate : 1;
      uint32_t aluin0imagzero   : 1;
      uint32_t aluin0imagnegate : 1;
      uint32_t aluin1regid      : 3;
      uint32_t                  : 1;
      uint32_t aluin1realzero   : 1;
      uint32_t aluin1realnegate : 1;
      uint32_t aluin1imagzero   : 1;
      uint32_t aluin1imagnegate : 1;
      uint32_t aluin2regid      : 3;
      uint32_t                  : 1;
      uint32_t aluin2realzero   : 1;
      uint32_t aluin2realnegate : 1;
      uint32_t aluin2imagzero   : 1;
      uint32_t aluin2imagnegate : 1;
      uint32_t                  : 4;
      uint32_t aluoutregid      : 3;
      uint32_t                  : 1;
    } cfg0;
  };
  union {
    uint32_t          CFG1;      ///< Instruction Configuration Word 1.
    struct {
      uint32_t istream0regid         : 3;
      uint32_t istream0load          : 1;
      uint32_t istream0arrayid       : 3;
      uint32_t istream0arrayincrdim0 : 1;
      uint32_t istream0arrayincrdim1 : 1;
      uint32_t istream0arrayincrdim2 : 1;
      uint32_t istream1regid         : 3;
      uint32_t istream1load          : 1;
      uint32_t istream1arrayid       : 3;
      uint32_t istream1arrayincrdim0 : 1;
      uint32_t istream1arrayincrdim1 : 1;
      uint32_t istream1arrayincrdim2 : 1;
      uint32_t ostreamregid          : 3;
      uint32_t ostreamstore          : 1;
      uint32_t ostreamarrayid        : 3;
      uint32_t ostreamarrayincrdim0  : 1;
      uint32_t ostreamarrayincrdim1  : 1;
      uint32_t ostreamarrayincrdim2  : 1;
      uint32_t                       : 2;
    } cfg1;
  };
  union {
    uint32_t          CFG2;      ///< Instruction Configuration Word 2.
    struct {
      uint32_t loop0begin : 1;
      uint32_t loop0end   : 1;
      uint32_t loop1begin : 1;
      uint32_t loop1end   : 1;
      uint32_t loop2begin : 1;
      uint32_t loop2end   : 1;
      uint32_t loop3begin : 1;
      uint32_t loop3end   : 1;
      uint32_t loop4begin : 1;
      uint32_t loop4end   : 1;
      uint32_t loop5begin : 1;
      uint32_t loop5end   : 1;
      uint32_t loop6begin : 1;
      uint32_t loop6end   : 1;
      uint32_t loop7begin : 1;
      uint32_t loop7end   : 1;
      uint32_t            : 4;
      uint32_t aluop      : 9;
      uint32_t            : 2;
      uint32_t endprog    : 1;
    } cfg2;
  };
} sli_mvp_instr_reg_t;

/**
 * Represents a single MVP program that can be loaded onto the hardware. A full
 * algorithm can potentially be represented by a number of different program
 * structures.
 */
typedef struct {
  sli_mvp_alu_reg_t ALU[8U];
  sli_mvp_array_reg_t ARRAY[5U];
  sli_mvp_loop_reg_t LOOP[8U];
  sli_mvp_instr_reg_t INSTR[8U];
  uint32_t CMD;
} sli_mvp_program_t;

/**
 * Type used when configuring performance counters.
 */
typedef enum {
  SLI_MVP_PERFCNT_CYCLES = 0,       ///< Count MVP cycles
  SLI_MVP_PERFCNT_INSTRUCTIONS = 1, ///< Count MVP instructions
  SLI_MVP_PERFCNT_STALL = 2         ///< Count number of stalls
} sli_mvp_perfcnt_t;

typedef enum {
  SLI_MVP_DATATYPE_UINT8            = 0x0,
  SLI_MVP_DATATYPE_INT8             = 0x1,
  SLI_MVP_DATATYPE_BINARY16         = 0x2,
  SLI_MVP_DATATYPE_COMPLEX_UINT8    = 0x4,
  SLI_MVP_DATATYPE_COMPLEX_INT8     = 0x5,
  SLI_MVP_DATATYPE_COMPLEX_BINARY16 = 0x6
} sli_mvp_datatype_t;

/**
 * MVP configuration parameters.
 */
typedef struct {
  bool use_dma;
  unsigned dma_ch;
} sli_mvp_config_t;

/**
 * Context for using the MVP program builder helper functions.
 */
typedef struct {
  sli_mvp_program_t program[2];   ///< Two MVP program buffers.
  uint32_t loop_begin_end[8];     ///< Storage for instruction loop begin/end bits.
  uint32_t loop_stack[8];         ///< Stack for keeping track of loop nesting level.
  sli_mvp_program_t *p;           ///< Pointer to active MVP program buffer.
  int prog_index;                 ///< Index used to select one of two possible MVP program buffers.
  int last_loop;                  ///< Loop counter.
  int last_instr;                 ///< Instruction counter.
  int loop_level;                 ///< Loop nesting level.
} sli_mvp_program_context_t;

/**
 * @brief
 *   Initialize the mvp hardware.
 *
 * @details
 *   This function must be called once before any of the other functions are
 *   used. This function will configure the MVP hardware based on the content
 *   of the sl_mvp_config.h file.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 */
sl_status_t sli_mvp_init(void);

/**
 * @brief
 *   De-Initialize the mvp hardware.
 *
 * @details
 *   This function can be called when the MVP hardware is no longer being used.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 */
sl_status_t sli_mvp_deinit(void);

/**
 * @brief
 *   Configure the operation of the MVP driver.
 *
 * @details
 *   This function can be used to reconfigure the MVP driver behavior after
 *   sli_mvp_init is called. This function can be used to switch between using
 *   DMA and CPU to load programs at runtime.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 */
sl_status_t sli_mvp_config(sli_mvp_config_t *config);

/**
 * @brief
 *   Execute a single program on the MVP hardware.
 *
 * @details
 *   Used to schedule a program for execution.
 *
 * @param[in] program
 *   The program to run on the MVP hardware.
 *
 * @param[in] wait
 *   If true then the function will wait for the program to complete before
 *   returning to the caller. If false then the function will return to the
 *   caller before the program is completed.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 */
sl_status_t sli_mvp_execute(sli_mvp_program_t *program, bool wait);


/**
 * @brief
 *   Wait for all the mvp program to complete.
 */
void sli_mvp_wait_for_completion(void);

// Functions for setting register value in an MVP program
void sli_mvp_prog_set_reg_s8(sli_mvp_program_t *prog, uint8_t reg, int8_t value);
void sli_mvp_prog_set_reg_s8c(sli_mvp_program_t *prog, uint8_t reg, int8_t real, int8_t imag);
void sli_mvp_prog_set_reg_s16(sli_mvp_program_t *prog, uint8_t reg, int16_t value);
void sli_mvp_prog_set_reg_s16c(sli_mvp_program_t *prog, uint8_t reg, int16_t real, int16_t imag);
void sli_mvp_prog_set_reg_s32(sli_mvp_program_t *prog, uint8_t reg, int32_t value);
void sli_mvp_prog_set_reg_s32c(sli_mvp_program_t *prog, uint8_t reg, int32_t real, int32_t imag);
void sli_mvp_prog_set_reg_f16(sli_mvp_program_t *prog, uint8_t reg, float16_t value);
void sli_mvp_prog_set_reg_f16c(sli_mvp_program_t *prog, uint8_t reg, float16_t real, float16_t imag);
void sli_mvp_prog_set_reg_f32(sli_mvp_program_t *prog, uint8_t reg, float value);
void sli_mvp_prog_set_reg_f32c(sli_mvp_program_t *prog, uint8_t reg, float real, float imag);

/**
 * @brief
 *   Configure a specific array in a program, with possibility to specify strides.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Array index. Values in the range 0-4.
 * @param[in] addr  Base address of the array.
 * @param[in] datatype Datatype of each array element.
 * @param[in] vecs Number of vectors (DIM0).
 * @param[in] rows Number of rows (DIM1).
 * @param[in] cols Number of columns (DIM2).
 * @param[in] vecstride Number of elements to advance when moving to the next vector (can be negative).
 * @param[in] rowstride Number of elements to advance when moving to the next row (can be negative).
 * @param[in] colstride Number of elements to advance when moving to the next column (can be negative).
 */
void sli_mvp_prog_set_array_full(sli_mvp_program_t *prog,
                                 uint8_t index,
                                 void *addr,
                                 sli_mvp_datatype_t type,
                                 unsigned short vecs,
                                 unsigned short rows,
                                 unsigned short cols,
                                 int vecstride,
                                 int rowstride,
                                 int colstride);

/**
 * @brief
 *   Configure a specific array in a program.
 *
 * @note
 *   The resulting memory ordering will be "NCHW" i.e. "vector major".
 *   This aligns with the memory ordering of a C-language 3D array.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Array index. Values in the range 0-4
 * @param[in] addr  Base address of the array.
 * @param[in] datatype Datatype of each array element.
 * @param[in] vecs Number of vectors (DIM0, major dimension).
 * @param[in] rows Number of rows (DIM1).
 * @param[in] cols Number of columns (DIM2, minor dimension).
 */
void sli_mvp_prog_set_array(sli_mvp_program_t *prog,
                            uint8_t index,
                            void *addr,
                            sli_mvp_datatype_t type,
                            unsigned short vecs,
                            unsigned short rows,
                            unsigned short cols);

/**
 * @brief
 *   Configure a specific vector in a program.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index index. Values in the range 0-4
 * @param[in] addr  Base address of the vector.
 * @param[in] datatype Datatype of each vector element.
 * @param[in] len Number of elements in vector (DIM2).
 */
void sli_mvp_prog_set_vector(sli_mvp_program_t *prog,
                             uint8_t index,
                             void *addr,
                             sli_mvp_datatype_t type,
                             unsigned short len);

/**
 * @brief
 *   Configure a specific 2D matrix in a program.
 *
 * @note
 *   The resulting memory ordering will be "row major".
 *
 * @param[in] prog  The program to configure.
 * @param[in] index index. Values in the range 0-4
 * @param[in] addr  Base address of the matrix.
 * @param[in] datatype Datatype of each matrix element.
 * @param[in] rows Number of rows (DIM1, major dimension).
 * @param[in] cols Number of columns (DIM2, minor dimension).
 */
void sli_mvp_prog_set_matrix(sli_mvp_program_t *prog,
                             uint8_t index,
                             void *addr,
                             sli_mvp_datatype_t type,
                             unsigned short rows,
                             unsigned short cols);

/**
 * @brief
 *   Configure a specific instruction in a program.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Instruction index. Values in the range 0-7
 * @param[in] opcode Operation to perform.
 * @param[in] alu_cfg  ALU configuration specifying input/output registers to use.
 * @param[in] load_cfg Load configuration, specifying 0-2 load streams to use.
 * @param[in] store_cfg Store configuration, specifying 0-1 store stream to use.
 * @param[in] end True if this is the last instruction of the program, false otherwise.
 */
void sli_mvp_prog_set_instr(sli_mvp_program_t *prog,
                            uint8_t index,
                            uint32_t opcode,
                            uint32_t alu_cfg,
                            uint32_t load_cfg,
                            uint32_t store_cfg,
                            bool end);

/**
 * @brief
 *   Configure a specific loop in a program.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Loop index. Value in the range 0-7.
 * @param[in] count Number of iterations. Value in the range 1-1024.
 * @param[in] istart Start instruction of this loop.
 * @param[in] iend End instruction of this loop.
 * @param[in] incrarray Optional array increment configuration.
 */
void sli_mvp_prog_set_loop(sli_mvp_program_t *prog,
                           uint8_t index,
                           unsigned short count,
                           uint8_t istart,
                           uint8_t iend,
                           uint32_t incrarray);

/**
 * @brief
 *   Configure one of the performance counters.
 *
 * @details
 *   Each of the performance counters can track different events. What event to count
 *   is configurable using the type parameter.
 *
 * @param[in] id
 *   The id of the performance counter to configure.
 *
 * @param[in] type
 *   Type of event to count.
 */
void sli_mvp_perfcnt_conf(unsigned id, sli_mvp_perfcnt_t type);

/**
 * @brief
 *   Reset all performance counters.
 */
void sli_mvp_perfcnt_reset_all(void);

/**
 * @brief
 *   Fetch the current aggregated value of a performance counter.
 *
 * @param[in] id
 *   The id of the performance counter to configure.
 *
 * @return
 *   Performance counter value.
 */
uint32_t sli_mvp_perfcnt_get(unsigned id);

/**
 * @brief
 *   Reset program count counter.
 */
void sli_mvp_progcnt_reset(void);

/**
 * @brief
 *   Fetch the current aggregated value of the program count counter.
 *
 * @return
 *   Program count counter value.
 */
uint32_t sli_mvp_progcnt_get(void);

/**
 * @brief
 *  Start a MVP program loop.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 * @param[in] iterations Loop count.
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER when loop count is exhausted.
 *             status is only updated when this error occur.
 */
void sli_mvp_pb_begin_loop(sli_mvp_program_context_t *p, int iterations, sl_status_t *status);

/**
 * @brief
 *  Begin a MVP program, will initialize part of the program context.
 *  Must be called once for each MVP program, and prior to any loop, compute
 *  or execute functions.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 */
void sli_mvp_pb_begin_program(sli_mvp_program_context_t *p);

/**
 * @brief
 *  Insert a MVP instruction in the program.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 * @param[in] opcode Instruction opcode.
 * @param[in] alu_cfg Instruction ALU configuration.
 * @param[in] load_cfg Instruction load configuration.
 * @param[in] store_cfg Instruction store configuration.
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER when instruction count is exhausted.
 *             status is only updated when this error occur.
 */
void sli_mvp_pb_compute(sli_mvp_program_context_t *p, uint32_t opcode, uint32_t alu_cfg, uint32_t load_cfg, uint32_t store_cfg, sl_status_t *status);

/**
 * @brief
 *   Configure a specific 3D array in a program.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Array index. Values in the range 0-4.
 * @param[in] addr  Base address of the array.
 * @param[in] datatype Datatype of each array element.
 * @param[in] vecs Number of vectors (DIM0).
 * @param[in] rows Number of rows (DIM1).
 * @param[in] cols Number of columns (DIM2).
 * @param[in] vecstride Number of elements to advance when moving to the next vector (can be negative).
 * @param[in] rowstride Number of elements to advance when moving to the next row (can be negative).
 * @param[in] colstride Number of elements to advance when moving to the next column (can be negative).
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER if an invalid array
 *             index is specified. Returns SL_STATUS_INVALID_RANGE if invalid
 *             array size.
 *             status is only updated when these errors occur.
 */
void sli_mvp_pb_config_array_full(sli_mvp_program_t *prog,
                                  uint8_t index,
                                  void *addr,
                                  sli_mvp_datatype_t type,
                                  unsigned short vecs,
                                  unsigned short rows,
                                  unsigned short cols,
                                  int vecstride,
                                  int rowstride,
                                  int colstride,
                                  sl_status_t *status);

/**
 * @brief
 *   Configure a 3D MVP array in a program in NHWC or "row major" style.
 *
 * @note
 *   NHWC: N = batch, H = height, W = width, C = channel.
 *   NHWC memory ordering is "row major" within each batch, and is the default
 *   memory ordering used in TensorFlow.
 *   Since MVP handles only three dimensions, we silently treat n (batches)
 *   of nhwc as 1.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index Array index. Values in the range 0-4
 * @param[in] addr  Base address of the array.
 * @param[in] datatype Datatype of each array element.
 * @param[in] h  Matrix height (rows, DIM1, major dimension).
 * @param[in] w  Matrix width (cols, DIM2).
 * @param[in] c  Matrix depth (vecs or channels, DIM0, minor dimension).
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER if an invalid array
 *             index is specified. Returns SL_STATUS_INVALID_RANGE if invalid
 *             array size.
 *             status is only updated when these errors occur.
 */
void sli_mvp_pb_config_array_nhwc(sli_mvp_program_t *prog,
                                  uint8_t index,
                                  void *addr,
                                  sli_mvp_datatype_t type,
                                  unsigned short h,
                                  unsigned short w,
                                  unsigned short c,
                                  sl_status_t *status);

/**
 * @brief
 *   Configure a specific 2D matrix in a program in "row major" style.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index index. Values in the range 0-4
 * @param[in] addr  Base address of the matrix.
 * @param[in] datatype Datatype of each matrix element.
 * @param[in] rows Number of rows (DIM1, major dimension).
 * @param[in] cols Number of columns (DIM2, minor dimension).
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER if an invalid array
 *             index is specified. Returns SL_STATUS_INVALID_RANGE if invalid
 *             matrix size.
 *             status is only updated when these errors occur.
 */
void sli_mvp_pb_config_matrix(sli_mvp_program_t *prog,
                              uint8_t index,
                              void *addr,
                              sli_mvp_datatype_t type,
                              unsigned short rows,
                              unsigned short cols,
                              sl_status_t *status);

/**
 * @brief
 *   Configure a specific vector in a program.
 *
 * @param[in] prog  The program to configure.
 * @param[in] index index. Values in the range 0-4
 * @param[in] addr  Base address of the vector.
 * @param[in] datatype Datatype of each vector element.
 * @param[in] len Number of elements in vector (DIM2).
 * @param[out] status Returns SL_STATUS_INVALID_PARAMETER if an invalid array
 *             index is specified. Returns SL_STATUS_INVALID_RANGE if invalid
 *             vector size.
 *             status is only updated when these errors occur.
 */
void sli_mvp_pb_config_vector(sli_mvp_program_t *prog,
                              uint8_t index,
                              void *addr,
                              sli_mvp_datatype_t type,
                              unsigned short len,
                              sl_status_t *status);

/**
 * @brief
 *  End a MVP program loop.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 */
void sli_mvp_pb_end_loop(sli_mvp_program_context_t *p);

/**
 * @brief
 *  Execute a MVP program.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 */
void sli_mvp_pb_execute_program(sli_mvp_program_context_t *p);

/**
 * @brief
 *  Initialize a MVP program context. Must be called once before a group of
 *  MVP programs are executed.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 */
void sli_mvp_pb_init_program(sli_mvp_program_context_t *p);

/**
 * @brief
 *  Set array dimension reset in previous loop iterator.
 *  When used after a call to @ref sli_mvp_end_loop(), the index will be reset
 *  on loop exit.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 * @param[in] array_index Index of array.
 * @param[in] dimension Dimension to reset.
 */
void sli_mvp_pb_postloop_incr_dim(sli_mvp_program_context_t *p, uint8_t array_index, uint8_t dimension);

/**
 * @brief
 *  Set array dimension index incrementer in previous loop iterator.
 *  When used after a call to @ref sli_mvp_end_loop(), the index will increment
 *  on loop exit.
 *
 * @note
 *   This function is part of the MVP program builder (pb) convenience API.
 *
 * @param[in] p Pointer to MVP program context.
 * @param[in] array_index Index of array.
 * @param[in] dimension Dimension to increment.
 */
void sli_mvp_pb_postloop_reset_dim(sli_mvp_program_context_t *p, uint8_t array_index, uint8_t dimension);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_H
