#include <esp_log.h>
#include <esp_heap_caps.h>

static const char *HEAP_PROFILING_TAG = "heap_profiling";

static void memory_profiler_dump_heap_stat(const char *state)
{
#ifdef CONFIG_ENABLE_MEMORY_PROFILING
    ESP_LOGI(HEAP_PROFILING_TAG,"========== HEAP-DUMP-START ==========\n");
    ESP_LOGI(HEAP_PROFILING_TAG,"state: %s\n", state);
    ESP_LOGI(HEAP_PROFILING_TAG,"\tDescription\tInternal\n");
    ESP_LOGI(HEAP_PROFILING_TAG,"Current Free Memory\t%d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    ESP_LOGI(HEAP_PROFILING_TAG,"Largest Free Block\t%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL));
    ESP_LOGI(HEAP_PROFILING_TAG,"Min. Ever Free Size\t%d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL));
    ESP_LOGI(HEAP_PROFILING_TAG,"========== HEAP-DUMP-END ==========\n");
#endif
}

#ifdef CONFIG_ENABLE_MEMORY_PROFILING
#define MEMORY_PROFILER_DUMP_HEAP_STAT(state) memory_profiler_dump_heap_stat(state)
#else
#define MEMORY_PROFILER_DUMP_HEAP_STAT(state)
#endif // CONFIG_ENABLE_MEMORY_PROFILING
