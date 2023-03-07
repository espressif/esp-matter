
#ifndef _TIMER_PLATFORM_FREERTOS_H_
#define _TIMER_PLATFORM_FREERTOS_H_

#include "stdint.h"
#include "FreeRTOS.h"

typedef enum {
    TIMER_TYPE_MS   = 0x00,
    TIMER_TYPE_S    = 0x01,
} timer_type;

struct Timer {
	uint32_t        time_out;
    TickType_t      init_tick;
    timer_type      type;
};

#endif

