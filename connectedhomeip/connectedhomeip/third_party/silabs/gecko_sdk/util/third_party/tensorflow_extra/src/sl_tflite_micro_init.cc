/***************************************************************************//**
 * @file
 * @brief Silicon Labs initialization functions for TensorFlow Light Micro.
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
#if defined __has_include
#if __has_include("sl_tflite_micro_model.h")
  #define HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION
#include "sl_tflite_micro_model.h"
#include "sl_tflite_micro_opcode_resolver.h"
#else
  #define sl_tflite_model_array   nullptr
#endif //__has_include("sl_tflite_micro_model.h")
#if __has_include("sl_tflite_micro_model_parameters.h")
#include "sl_tflite_micro_model_parameters.h"
#endif //__has_include("sl_tflite_micro_model_parameters.h")
#endif //__has_include

#include "sl_tflite_micro_init.h"
#include "sl_tflite_micro_config.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "em_common.h"
#include "em_assert.h"

// Set arena size 
#if (defined(SL_TFLITE_MODEL_RUNTIME_MEMORY_SIZE) && (SL_TFLITE_MICRO_ARENA_SIZE == 0))
  // Use value from model parameters 
  #define ARENA_SIZE SL_TFLITE_MODEL_RUNTIME_MEMORY_SIZE
#else 
  // Use value from configuration 
  #define ARENA_SIZE SL_TFLITE_MICRO_ARENA_SIZE 
#endif // SL_TFLITE_MODEL_RUNTIME_MEMORY_SIZE 

#if SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE && defined(HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION)
#if (ARENA_SIZE == 0)
#warning "You have to configure the arena size in sl_tflite_micro_init_config.h"
#endif // ARENA_SIZE == 0
#endif // SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE && defined(HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION)

#ifndef SL_TFLITE_MICRO_MODEL_ARRAY
#define SL_TFLITE_MICRO_MODEL_ARRAY sl_tflite_model_array
#endif


/***************************************************************************//**
 * @brief
 *  The tensor arena buffer used by TensorFlow Lite Micro.
 ******************************************************************************/
#if SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE && defined(HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION)
static uint8_t tensor_arena[ARENA_SIZE] SL_ATTRIBUTE_ALIGN(16);
#endif

/***************************************************************************//**
 *  @brief The TensorFlow Lite Micro error reporter created by the init function.
 ******************************************************************************/
static tflite::ErrorReporter *sl_tflite_micro_error_reporter = nullptr;

/***************************************************************************//**
 *  @brief The TensorFlow Lite Micro interpreter created by the init function.
 ******************************************************************************/
static tflite::MicroInterpreter *sl_tflite_micro_interpreter = nullptr;

/***************************************************************************//**
 *  @brief A pointer to the input tensor, set by the init function.
 ******************************************************************************/
static TfLiteTensor* sl_tflite_micro_input_tensor = nullptr;

/***************************************************************************//**
 *  @brief A pointer to the output tensor, set by the init function.
 ******************************************************************************/
static TfLiteTensor* sl_tflite_micro_output_tensor = nullptr;

/***************************************************************************//**
 * @brief
 *  Creates the error reporter and opcode resolver and initializes variables
 *  for the flatbuffer given by the configuration.
 ******************************************************************************/
static void init(void)
{
#if SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE && defined(HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION)
  static tflite::MicroErrorReporter micro_error_reporter;
  sl_tflite_micro_error_reporter = &micro_error_reporter;

  // Instantiate model from char array.
  // The array may have been created in autogen/sl_ml_model.h or elsewhere.
  const tflite::Model* model = tflite::GetModel(SL_TFLITE_MICRO_MODEL_ARRAY);

  // Check model schema version
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(sl_tflite_micro_error_reporter, "Error: Invalid model version");
    EFM_ASSERT(false);
  }

  // Create op resolver
  tflite::MicroOpResolver &opcode_resolver = sl_tflite_micro_opcode_resolver(sl_tflite_micro_error_reporter);

  // Instantiate interpreter
  static tflite::MicroInterpreter static_interpreter(
    model, opcode_resolver, tensor_arena, ARENA_SIZE, sl_tflite_micro_error_reporter);
  sl_tflite_micro_interpreter = &static_interpreter;

  // Allocate memory from tensor_arena for the model's tensors.
  if (sl_tflite_micro_interpreter->AllocateTensors() != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(sl_tflite_micro_error_reporter, "Error: Arena size too small, failed to allocate tensors");
    EFM_ASSERT(false);
  }

  // Obtain pointers to input and output tensors
  sl_tflite_micro_input_tensor = sl_tflite_micro_interpreter->input(0);
  sl_tflite_micro_output_tensor = sl_tflite_micro_interpreter->output(0);
#endif // SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE && defined(HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION)
}

tflite::ErrorReporter *sl_tflite_micro_get_error_reporter()
{
  return sl_tflite_micro_error_reporter;
}

tflite::MicroInterpreter *sl_tflite_micro_get_interpreter()
{
  return sl_tflite_micro_interpreter;
}

TfLiteTensor* sl_tflite_micro_get_input_tensor()
{
  return sl_tflite_micro_input_tensor;
}

TfLiteTensor* sl_tflite_micro_get_output_tensor()
{
  return sl_tflite_micro_output_tensor;
}

#ifdef HAS_TFLITE_MICRO_FLATBUFFER_IN_CONFIGURATION
tflite::MicroOpResolver &sl_tflite_micro_opcode_resolver(tflite::ErrorReporter* error_reporter)
{
  SL_TFLITE_MICRO_OPCODE_RESOLVER(opcode_resolver, error_reporter);
  return opcode_resolver;
}
#endif

extern "C" void sl_tflite_micro_init(void)
{
  init();
}
