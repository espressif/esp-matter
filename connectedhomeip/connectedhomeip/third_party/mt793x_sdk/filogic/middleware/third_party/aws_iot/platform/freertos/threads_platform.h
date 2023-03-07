
#ifndef _THREADS_PLATFORM_FREERTOS_H_
#define _THREADS_PLATFORM_FREERTOS_H_

#include "FreeRTOS.h"
#include "semphr.h"

struct _IoT_Mutex_t {
	SemaphoreHandle_t   lock;
};

#endif
