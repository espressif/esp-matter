
/**
 * This implementation CAN NOT be called from the ISR method
 * Meanwhile, this implementation also be effective after the vTaskStartScheduler method
 * If need to call from ISR, the APIs should have another implementation
 */

#include "timer_interface.h"
#include "FreeRTOS.h"
#include "task.h"

bool has_timer_expired(Timer *timer)
{
    TickType_t count = xTaskGetTickCount() - timer->init_tick;
    if (timer->type == TIMER_TYPE_MS) {
        if ((count / portTICK_RATE_MS) < timer->time_out) {
            return false;
        }
        return true;
    } else {
        if (((count / portTICK_RATE_MS) / 1000 ) < timer->time_out) {
            return false;
        }
        return true;
    }
}

void countdown_ms(Timer *timer, uint32_t timeout)
{
    if (timer == NULL || timeout <= 0) {
        return;
    }
    timer->init_tick = xTaskGetTickCount();
    timer->time_out = timeout;
    timer->type = TIMER_TYPE_MS;
}

void countdown_sec(Timer *timer, uint32_t timeout)
{
    if (timer == NULL || timeout <= 0) {
        return;
    }
    timer->init_tick = xTaskGetTickCount();
    timer->time_out = timeout;
    timer->type = TIMER_TYPE_S;
}

uint32_t left_ms(Timer *timer)
{
    TickType_t count = xTaskGetTickCount() - timer->init_tick;
    uint32_t    total = 0;
    uint32_t    result_ms = 0;
    if (timer->type == TIMER_TYPE_MS) {
        total = timer->time_out;
    } else {
        total = timer->time_out * 1000;
    }
    result_ms = total - (count / portTICK_RATE_MS);
    return result_ms;
}

void init_timer(Timer *timer)
{
    timer->time_out = 0;
    timer->init_tick = 0;
    timer->type = TIMER_TYPE_MS;
}



