#ifndef __ROM_API_H__
#define __ROM_API_H__
#include <FreeRTOS.h>
#include <task.h>
#include <stddef.h>
extern void *(*rtos_memcpy_ptr)(void *dest, const void *src, size_t n);
extern void *(*rtos_memset_ptr)(void *s, int c, size_t n);
extern char *(*rtos_strcpy_ptr)(char *dest, const char *src);
extern size_t (*rtos_strlen_ptr)(const char *s);
extern int (*rtos_sprintf)(char *str, const char *format, ...);
extern void (*vApplicationGetIdleTaskMemory)(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
extern void (*vApplicationGetTimerTaskMemory)(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);
extern void (*vApplicationIdleHook)(void);
extern void (*vApplicationMallocFailedHook)(void);
extern void (*vApplicationStackOverflowHook)(TaskHandle_t xTask, char *pcTaskName);
extern void (*vAssertCalled)(void);
extern uint32_t (*rtos_clz)(uint32_t priorities);
extern void (*exception_entry_ptr)(uint32_t mcause, uint32_t mepc, uint32_t mtval);
extern void (*interrupt_entry_ptr)(uint32_t mcause);
#endif
