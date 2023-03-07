#include "rom_api.h"

void *(*rtos_memcpy_ptr)(void *dest, const void *src, size_t n);
void *(*rtos_memset_ptr)(void *s, int c, size_t n);
char *(*rtos_strcpy_ptr)(char *dest, const char *src);
size_t (*rtos_strlen_ptr)(const char *s);
int (*rtos_sprintf)(char *str, const char *format, ...);
void (*vApplicationGetIdleTaskMemory)(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void (*vApplicationGetTimerTaskMemory)( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);
void (*vApplicationIdleHook)( void );
void (*vApplicationMallocFailedHook)( void );
void (*vApplicationStackOverflowHook)(TaskHandle_t xTask, char *pcTaskName);
void (*vAssertCalled)(void);
uint32_t (*rtos_clz)(uint32_t priorities);
void (*exception_entry_ptr)(uint32_t mcause, uint32_t mepc, uint32_t mtval);
void (*interrupt_entry_ptr)(uint32_t mcause);
