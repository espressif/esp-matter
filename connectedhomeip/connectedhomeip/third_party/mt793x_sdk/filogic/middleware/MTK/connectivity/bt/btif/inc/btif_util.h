/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __BTIF_COMMON_H_
#define __BTIF_COMMON_H_

#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "syslog.h"

#define BTIF_OP_ENABLE  (1)
#define BTIF_OP_DISABLE (0)

#define BTIF_LOG_LVL_V 4
#define BTIF_LOG_LVL_D 3
#define BTIF_LOG_LVL_I 2
#define BTIF_LOG_LVL_W 1
#define BTIF_LOG_LVL_E 0
#define BTIF_TAG "[BTIF]"

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)x)
#endif /* #ifndef UNUSED */

void btif_set_log_lvl(unsigned char lvl, int dump_buffer, int times);
unsigned char btif_get_log_lvl(void);

#ifndef MTK_BT_MEM_SHRINK
#define BTIF_LOG_V(fmt, args...)                                   \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_V) {                \
			LOG_I(BTIF, fmt, ##args);                              \
		}                                                          \
	} while (0)

#define BTIF_LOG_D(fmt, args...)                                   \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_D) {                \
			LOG_I(BTIF, fmt, ##args);                              \
		}                                                          \
	} while (0)
#else
#define BTIF_LOG_V(...) do {} while(0)
#define BTIF_LOG_D(...) do {} while(0)
#endif
#define BTIF_LOG_I(fmt, args...)                                   \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_I) {                \
			LOG_I(BTIF, fmt, ##args);                              \
		}                                                          \
	} while (0)

#define BTIF_LOG_W(fmt, args...)                                   \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_W) {                \
			LOG_W(BTIF, fmt, ##args);                              \
		}                                                          \
	} while (0)

#define BTIF_LOG_E(fmt, args...)                                   \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_E) {                \
			LOG_E(BTIF, fmt, ##args);                              \
		}                                                          \
	} while (0)

#define BTIF_LOG_FUNC()                                            \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_D) {                \
			LOG_I(BTIF, "[F]%s() L %d", __func__, __LINE__);       \
		}                                                          \
	} while (0)

#define BTIF_LOG_I_RAW(p, l, fmt, ...)                             \
	do {                                                           \
		if (btif_get_log_lvl() >= BTIF_LOG_LVL_I) {                \
			int raw_count = 0;                                     \
			const unsigned char *ptr = p;                          \
			LOG_I(BTIF, fmt, ##__VA_ARGS__);                       \
			for (raw_count = 0; raw_count < l; ++raw_count) {      \
				LOG_I(BTIF, " %02X", ptr[raw_count]);	           \
			}                                                      \
		}                                                          \
	} while (0)

unsigned int btif_strtoui(const char *str);
void btif_util_init(void);
void btif_util_deinit(void);

xSemaphoreHandle btif_util_binary_sema_create(void);
xSemaphoreHandle btif_util_mutex_create(void);
void btif_util_sema_delete(xSemaphoreHandle sema);
int btif_util_sema_lock(xSemaphoreHandle sema, unsigned char from_isr);
void btif_util_sema_unlock(xSemaphoreHandle sema, unsigned char from_isr);
xTimerHandle btif_util_timer_create(const char *const pcTimerName,
									unsigned int timer_ms, int repeat,
									TimerCallbackFunction_t pxCallbackFunction);

void btif_util_timer_start(xTimerHandle timer);
void btif_util_timer_stop(xTimerHandle timer);
void btif_util_timer_reset(xTimerHandle timer);
void btif_util_task_delay_ms(unsigned int ms);
void btif_util_task_delay_ticks(unsigned int ticks);
void *btif_util_malloc(unsigned int size);
void btif_util_free(void *ptr);
void btif_util_dump_buffer(char *label, const unsigned char *buf,
						   unsigned int length, unsigned char force_print);

#endif /*__BTIF_COMMON_H_*/
