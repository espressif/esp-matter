/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <inttypes.h>
#include <stdio.h>

static SemaphoreHandle_t xSemaphore = NULL;

static void thread_A(void *argument)
{
    const TickType_t xDelay = 500;

    for (int i = 0; i < 5; ++i) {
        if (xSemaphore != NULL) {
            if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
                printf("Message from Thread A\r\n");
            }
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(xDelay);
    }
}

static void thread_B(void *argument)
{
    const TickType_t xDelay = 500;

    for (int i = 0; i < 5; ++i) {
        if (xSemaphore != NULL) {
            if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
                printf("Message from Thread B\r\n");
            }
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(xDelay);
    }
}

int main()
{
    const uint16_t task_stack_size = 0x100;

    printf("Inside main() \r\n");

    xSemaphore = xSemaphoreCreateMutex();
    if (xSemaphore == NULL) {
        printf("Error: Failed to create semaphore\r\n");
        return -1;
    }

    BaseType_t ret;
    TaskHandle_t task_id_a;
    TaskHandle_t task_id_b;
    printf("Creating threads\r\n");

    ret = xTaskCreate(thread_A, "TaskA", task_stack_size, NULL, configMAX_PRIORITIES - 2, &task_id_a);
    if (ret != pdPASS) {
        printf("Error: Failed to create Thread A - xTaskCreate() returned %d\r\n", (int)ret);
        return -1;
    }
    printf("Thread A ID 0x%" PRIxPTR "\r\n", (uintptr_t)task_id_a);

    ret = xTaskCreate(thread_B, "TaskB", task_stack_size, NULL, configMAX_PRIORITIES - 2, &task_id_b);
    if (ret != pdPASS) {
        printf("Error: Failed to create Thread B - xTaskCreate() returned %d\r\n", (int)ret);
        return -1;
    }
    printf("Thread B ID 0x%" PRIxPTR "\r\n", (uintptr_t)task_id_b);

    vTaskStartScheduler();

    return 0;
};
