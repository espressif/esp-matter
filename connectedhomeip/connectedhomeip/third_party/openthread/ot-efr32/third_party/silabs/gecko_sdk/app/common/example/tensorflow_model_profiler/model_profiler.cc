/***************************************************************************//**
 * @file model_profiler.cc
 * @brief TFLM model profiler.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "em_cmu.h"
#include "sl_memory.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_MVP_PRESENT)
#include "sl_mvp.h"
#include "sl_mvp_util.h"
#endif

#include "sl_tflite_micro_model.h"
#include "sl_tflite_micro_init.h"
#include "model_profiler.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"

//#define SL_PRINT_MATRIX_DATA                 // Harvest matrix data while profiling.
//#define SL_PRINT_MATRIX_DATA_HUMAN_READABLE

namespace {

// Subtype of MicroInterpreter to access the TfLiteContext
class SLMicroInterpreter : public tflite::MicroInterpreter {
public:
  using MicroInterpreter::MicroInterpreter; // Inherit constructors

  const TfLiteEvalTensor* GetEvalTensor(int tensor_id) const {
    const TfLiteContext& ctx = context();
    return ctx.GetEvalTensor(&ctx, tensor_id);
  }
};


tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model            = nullptr;
SLMicroInterpreter* interpreter = nullptr;
size_t arena_size;
uint8_t *arena;
uint32_t cpu_freq;

// Print uint32_t with thousand separators.
void sli_ui32_print_ts(uint32_t n)
{
  if (n < 1000) {
    printf ("%ld", n);
    return;
  }
  sli_ui32_print_ts(n / 1000);
  printf (",%03ld", n % 1000);
}

void sli_print_ui32(const char *lead_str, uint32_t n, const char *lag_str)
{
  printf(lead_str);
  sli_ui32_print_ts(n);
  printf(lag_str);
}

void sli_print_time(const char *str, uint32_t time)
{
  float duration = (float)time / (float)cpu_freq;
  if (duration > 1.0) {
    printf("%s%.02f s\n", str, duration);
  } else if (duration > 0.001) {
    printf("%s%.02f ms\n", str, duration * 1000.0);
  } else if (duration > 0.000001) {
    printf("%s%.02f us\n", str, duration * 1000000.0);
  }
}

#if defined(SL_PRINT_MATRIX_DATA)
/***************************************************************************//**
 *
 *  Memory layout of 3x3x3 (HEIGHT x WIDTH x DEPTH) matrix.
 *
 *             Matrix "view"        Corresponding memory addresses
 *              -------
 *              | | | |  X = 0               0 1 2
 *       Y = 0  | | | |  X = 1               3 4 5
 *              | | | |  X = 2               6 7 8
 *              -------
 *              | | | |  X = 0
 *       Y = 1  | | | |  X = 1               ...
 *              | | | |  X = 2
 *              -------
 *              | | | |  X = 0
 *       Y = 2  | | | |  X = 1
 *              | | | |  X = 2              24 25 26
 *              -------
 *
 *               ^ ^ ^
 *               | | |
 *
 *       Ch      0 1 2  (Ch = Channel)
 *
 *       Offset(x,y,ch) = (depth * width * y) + (depth * x) + ch
 *                      = (((width * y) + x) * depth) + ch
 *
 ******************************************************************************/
void print_matrix_3d_u8(int height, int width, int depth, const uint8_t *data)
{
  uint8_t val;

#if defined(SL_PRINT_MATRIX_DATA_HUMAN_READABLE)
  // Print matrix data in human readable format channel by channel.
  for (int ch = 0; ch < depth; ch++) {
    printf("//   Channel = %d,  Y = 0..%d,  X = 0..%d ->\n", ch, height - 1, width - 1);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int index = sli_mvp_util_offset_nhwc(height, width, depth, 0, x, y, ch);
        val = data[index];
        printf("0x%02X, ", val);
      }
      printf("\n");
    }
  }
#else
  // Print matrix data as laid out in memory
  for (int y = 0; y < height; y++) {
    printf("//   Y = %d,  X = 0..%d,  Channel = 0..%d ->\n", y, width - 1, depth - 1);
    for (int x = 0; x < width; x++) {
      for (int ch = 0; ch < depth; ch++) {
        int index = sli_mvp_util_offset_nhwc(height, width, depth, 0, x, y, ch);
        val = data[index];
        printf("0x%02X, ", val);
      }
      printf("\n");
    }
  }
#endif
}

void harvest_input_matrix(int operation_index)
{
  const tflite::Operator *op = model->subgraphs()->Get(0)->operators()->Get(operation_index);
  int tensor_index = op->inputs()->Get(0);
  const TfLiteEvalTensor *tensor = interpreter->GetEvalTensor(tensor_index);

  if ((tensor->type == kTfLiteInt8) && (tensor->dims->size == 4)) {
    const int8_t *in_data = tflite::micro::GetTensorData<int8_t>(tensor);
    int height = tensor->dims->data[1];
    int width  = tensor->dims->data[2];
    int depth  = tensor->dims->data[3];
    printf("Input tensor dimensions: %dx%dx%dx%d\n", tensor->dims->data[0],
           height, width, depth);
    print_matrix_3d_u8(height, width, depth, reinterpret_cast<const uint8_t*>(in_data));
  }
}

void harvest_output_matrix(int operation_index)
{
  const tflite::Operator *op = model->subgraphs()->Get(0)->operators()->Get(operation_index);
  int tensor_index = op->outputs()->Get(0);
  const TfLiteEvalTensor *tensor = interpreter->GetEvalTensor(tensor_index);

  if ((tensor->type == kTfLiteInt8) && (tensor->dims->size == 4)) {
    const int8_t *out_data = tflite::micro::GetTensorData<int8_t>(tensor);
    int height = tensor->dims->data[1];
    int width  = tensor->dims->data[2];
    int depth  = tensor->dims->data[3];
    printf("Output tensor dimensions: %dx%dx%dx%d\n", tensor->dims->data[0],
           height, width, depth);
    print_matrix_3d_u8(height, width, depth, reinterpret_cast<const uint8_t*>(out_data));
  }
}
#endif

// Subclass tflite::MicroProfiler to get Invoke() hooks from model layers.
class SlProfiler : public tflite::MicroProfiler {
  public:
    SlProfiler() : total_cpu_cycles_(0), total_mvp_instructions_(0),
                  total_mvp_programs_(0), total_mvp_stall_cycles_(0),
                  operation_index_(0) {}

    ~SlProfiler() override = default;

    uint32_t BeginEvent(const char* tag) override
    {
      printf("--------------------------------------------\n");
      printf("%d: %s\n", operation_index_, tag);
#if defined(SL_PRINT_MATRIX_DATA)
      harvest_input_matrix(operation_index_);
#endif
#if defined(SL_CATALOG_MVP_PRESENT)
      sli_mvp_perfcnt_reset_all();
      sli_mvp_progcnt_reset();
#endif
      cpu_cycles = DWT->CYCCNT;
      return 0;
    }

    void EndEvent(uint32_t event_handle) override
    {
#if defined(SL_CATALOG_MVP_PRESENT)
      uint32_t mvp_instructions;
      uint32_t mvp_stall_cycles;
      uint32_t mvp_programs;
#endif

      cpu_cycles = DWT->CYCCNT - cpu_cycles;
      total_cpu_cycles_ += cpu_cycles;
      sli_print_time("Execution time:    ", cpu_cycles);
      sli_print_ui32("CPU cycle count:   ", cpu_cycles, "\n");

#if defined(SL_CATALOG_MVP_PRESENT)
      mvp_instructions = sli_mvp_perfcnt_get(0);
      if (mvp_instructions > 0) {
        mvp_stall_cycles = sli_mvp_perfcnt_get(1);
        mvp_programs = sli_mvp_progcnt_get();

        total_mvp_instructions_ += mvp_instructions;
        total_mvp_stall_cycles_ += mvp_stall_cycles;
        total_mvp_programs_ += mvp_programs;

        sli_print_ui32("MVP instructions:  ", mvp_instructions, "\n");
        sli_print_ui32("MVP stall cycles:  ", mvp_stall_cycles, "\n");
        sli_print_ui32("MVP program count: ", mvp_programs, "\n");
      }
#endif
#if defined(SL_PRINT_MATRIX_DATA)
      harvest_output_matrix(operation_index_);
#endif
      operation_index_++;
    }

    uint32_t total_cpu_cycles(void) { return total_cpu_cycles_; }
    uint32_t total_mvp_instructions(void) { return total_mvp_instructions_; }
    uint32_t total_mvp_programs(void) { return total_mvp_programs_; }
    uint32_t total_mvp_stall_cycles(void) { return total_mvp_stall_cycles_; }
    uint32_t operation_index(void) { return operation_index_; }

  private:
    uint32_t cpu_cycles;
    uint32_t total_cpu_cycles_;
    uint32_t total_mvp_instructions_;
    uint32_t total_mvp_programs_;
    uint32_t total_mvp_stall_cycles_;
    int operation_index_;
    TF_LITE_REMOVE_VIRTUAL_DELETE
};

SlProfiler profiler;

}  // namespace

// Initialize a TFLM model.
bool model_profiler_init(void)
{
  // Setup logging for TFLM.
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  printf("\n--------------------------------------------\n");
  printf("Model profiler.\n");

  // The app runs on HFXO.
  cpu_freq = CMU_ClockFreqGet(cmuClock_CORE);
  printf("CPU core frequency: %.1f MHz\n", (float)cpu_freq/1000000.0);

  // Load the model.
  printf("--------------------------------------------\n");
  printf("Loading model.\n");
  model = tflite::GetModel(sl_tflite_model_array);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    printf("Model provided is schema version %ld, only version "
           "%d is supported.",
           model->version(), TFLITE_SCHEMA_VERSION);
    return false;
  }
  flatbuffers::String *s = const_cast<flatbuffers::String *>(model->description());
  printf("%s\n", s->c_str());

  tflite::MicroOpResolver &resolver = sl_tflite_micro_opcode_resolver(error_reporter);

  // Start a new block, interpreter will be destroyed on block exit.
  {
    // Figure out how much memory we need for the TensorArena.
    sl_memory_region_t heap = sl_memory_get_heap_region();

    arena = reinterpret_cast<uint8_t*>(malloc(heap.size - 2000));
    if (arena == nullptr) {
      printf("Memory allocation failed !");
      return false;
    }
    // Align to 16 byte boundary.
    uint32_t tmp = reinterpret_cast<uint32_t>(arena);
    tmp = (tmp + 16) & 0xFFFFFFF0UL;

    SLMicroInterpreter interpreter(model, resolver,
                                   reinterpret_cast<uint8_t*>(tmp),
                                   heap.size - 2000,
                                   error_reporter);

    if (interpreter.AllocateTensors() != kTfLiteOk) {
      printf("Model tensor allocation failed !");
      return false;
    }
    arena_size = interpreter.arena_used_bytes();
    sli_print_ui32("\nTFLM model arena size: ", arena_size, "\n");
  }

  free(arena);
  // Add bytes because the arena size reported by interpreter is too small.
  arena_size += 1000;
  uint32_t tmp = reinterpret_cast<uint32_t>(malloc(arena_size));
  if (tmp == 0U) {
    printf("Memory allocation failed !");
    return false;
  }
  // Align to 16 byte boundary.
  tmp = (tmp + 16) & 0xFFFFFFF0UL;
  arena = reinterpret_cast<uint8_t*>(tmp);

  // Build an interpreter to run the model with.
  static SLMicroInterpreter static_interpreter(
                                model, resolver, arena,
                                arena_size - 16, error_reporter,
                                nullptr, &profiler);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    printf("Model tensor allocation failed !");
    return false;
  }

  printf("\n--------------------------------------------\n");
  printf("Starting model profiler.\n");

#if defined(SL_CATALOG_MVP_PRESENT)
  sli_mvp_perfcnt_conf(0, SLI_MVP_PERFCNT_INSTRUCTIONS);
#endif

  // Prepare DWT core cycle counting.
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  return true;
}

// Run one inference on the TFLM model.
void model_profiler_process_action(void)
{
  // Set input tensor to zero.
  TfLiteTensor* input_tensor = interpreter->input(0);
  memset(input_tensor->data.int8, 0, input_tensor->bytes);

  // Run the model and make sure it succeeds.
  if (interpreter->Invoke() != kTfLiteOk) {
    printf("Model invokation failed !");
    return;
  }

  printf("--------------------------------------------\n");
  printf("--------------------------------------------\n");
  printf("Profiling summary:\n");
  sli_print_time("Total execution time:   ", profiler.total_cpu_cycles());
  sli_print_ui32("Total CPU cycle count:  ", profiler.total_cpu_cycles(), "\n");
#if defined(SL_CATALOG_MVP_PRESENT)
  auto total_mvp_instructions = profiler.total_mvp_instructions();
  if (total_mvp_instructions > 0) {
    sli_print_ui32("Total MVP instructions: ", total_mvp_instructions, "\n");
    sli_print_ui32("Total MVP stall cycles: ", profiler.total_mvp_stall_cycles(), "\n");
    sli_print_ui32("Total MVP programs:     ", profiler.total_mvp_programs(), "\n");
  }
#endif
  printf("--------------------------------------------\n");
  printf("--------------------------------------------\n");

  printf("Model profiling done.\n");
#if defined(SL_CATALOG_MVP_PRESENT)
  sli_mvp_deinit();
#endif
}
