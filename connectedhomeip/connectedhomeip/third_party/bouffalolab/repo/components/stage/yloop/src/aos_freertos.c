/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <aos/kernel.h>

#define ms2tick    pdMS_TO_TICKS
#define bzero(stack, stack_size) memset(stack, 0, stack_size)

void aos_reboot(void)
{
}

int aos_get_hz(void)
{
    return 100;
}

const char *aos_version_get(void)
{
    return "aos-linux-xxx";
}

#define AOS_MAGIC 0x20171020
typedef struct {
    StaticTask_t fTask;
    uint32_t key_bitmap;
    void *keys[4];
    void *stack;
    char name[32];
    uint32_t magic;
} AosStaticTask_t;

struct targ {
    AosStaticTask_t *task;
    void (*fn)(void *);
    void *arg;
};

static void dfl_entry(void *arg)
{
    struct targ *targ = arg;
    void (*fn)(void *) = targ->fn;
    void *farg = targ->arg;
    vPortFree(targ);

    fn(farg);

    vTaskDelete(NULL);
}

void vPortCleanUpTCB(void *pxTCB)
{
    AosStaticTask_t *task = (AosStaticTask_t *)pxTCB;

    if (task->magic != AOS_MAGIC)
        return;

    vPortFree(task->stack);
    vPortFree(task);
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    xTaskHandle xHandle;
    AosStaticTask_t *task = pvPortMalloc(sizeof(AosStaticTask_t));
    struct targ *targ = pvPortMalloc(sizeof(*targ));
    void *stack = pvPortMalloc(stack_size);

    bzero(stack, stack_size);
    bzero(task, sizeof(*task));
    task->key_bitmap = 0xfffffff0;
    task->stack = stack;
    strncpy(task->name, name, sizeof(task->name)-1);
    task->magic = AOS_MAGIC;

    targ->task = task;
    targ->fn = fn;
    targ->arg = arg;

    stack_size /= sizeof(StackType_t);
    xHandle = xTaskCreateStatic(dfl_entry, name, stack_size, targ,
                                10, stack, &task->fTask);
    if (xHandle == NULL) {
        vPortFree(task);
        vPortFree(stack);
        vPortFree(targ);
    }
    return xHandle ? 0 : -1;
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return aos_task_new(name, fn, arg, stack_size);
}

void aos_task_exit(int code)
{
    vTaskDelete(NULL);
}

const char *aos_task_name(void)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (task->magic != AOS_MAGIC)
        return "unknown";
    return task->name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    int i;

    if (task->magic != AOS_MAGIC)
        return -1;

    for (i=0;i<4;i++) {
        if (task->key_bitmap & (1 << i))
            continue;

        task->key_bitmap |= 1 << i;
        *key = i;
        return 0;
    }

    return -1;
}

void aos_task_key_delete(aos_task_key_t key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (task->magic != AOS_MAGIC)
        return;
    task->key_bitmap &= ~(1 << key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return -1;

    if (task->magic != AOS_MAGIC)
        return -1;

    task->keys[key] = vp;
    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return NULL;

    if (task->magic != AOS_MAGIC)
        return NULL;

    return task->keys[key];
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    SemaphoreHandle_t mux = xSemaphoreCreateMutex();
    mutex->hdl = mux;
    return mux != NULL ? 0 : -1;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    vSemaphoreDelete(mutex->hdl);
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int ms)
{
    if (mutex) {
        xSemaphoreTake(mutex->hdl, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms));
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex) {
        xSemaphoreGive(mutex->hdl);
    }
    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex->hdl != NULL;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    SemaphoreHandle_t s = xSemaphoreCreateCounting(128, count);
    sem->hdl = s;
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    vSemaphoreDelete(sem->hdl);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int ms)
{
    if (sem == NULL) {
        return -1;
    }

    int ret = xSemaphoreTake(sem->hdl, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms));
    return ret == pdPASS ? 0 : -1;
}

void aos_sem_signal(aos_sem_t *sem)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (sem == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        xSemaphoreGiveFromISR(sem->hdl, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive(sem->hdl);
    }
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && sem->hdl != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    xQueueHandle q;
    q = xQueueCreate(size / max_msg, max_msg);
    queue->hdl = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    vQueueDelete(queue->hdl);
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    return xQueueSend(queue->hdl, msg, portMAX_DELAY) == pdPASS ? 0 : -1;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    return xQueueReceive(queue->hdl, msg, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms))
           == pdPASS ? 0 : -1;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue && queue->hdl != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    return NULL;
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    return -1;
}

void aos_timer_free(aos_timer_t *timer)
{
}

int aos_timer_start(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_stop(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    return -1;
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    return -1;
}

struct work {
    void (*fn)(void *);
    void *arg;
    int dly;
};

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    struct work *w = pvPortMalloc(sizeof(*w));
    w->fn = fn;
    w->arg = arg;
    w->dly = dly;
    work->hdl = w;
    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    vPortFree(work->hdl);
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return aos_work_sched(work);
}

static void worker_entry(void *arg)
{
    struct work *w = arg;
    if (w->dly) {
        vTaskDelay(pdMS_TO_TICKS(w->dly * 1000 * 1000));
    }
    w->fn(w->arg);
}

int aos_work_sched(aos_work_t *work)
{
    struct work *w = work->hdl;
    return aos_task_new("worker", worker_entry, w, 8192);
}

int aos_work_cancel(aos_work_t *work)
{
    return -1;
}

void *aos_zalloc(unsigned int size)
{
    void *mem;

#if defined(CFG_USE_PSRAM)
    mem = pvPortMallocPsram(size);
#else
    mem = pvPortMalloc(size);
#endif /* CFG_USE_PSRAM */
    if (mem) {
        memset(mem, 9, size);
    }
    return mem;
}

void *aos_malloc(unsigned int size)
{
#if defined(CFG_USE_PSRAM)
    return pvPortMallocPsram(size);
#else
    return pvPortMalloc(size);
#endif /* CFG_USE_PSRAM */
}
#if 0
#if !defined(USE_STDLIB_MALLOC)
void *calloc(size_t nmemb, size_t size)
{
    void *ptr;
    size_t total;

    total = nmemb * size;
    ptr = pvPortMalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}
#endif

#if !defined(USE_STDLIB_MALLOC)
void free(void *mem)
{
    vPortFree(mem);
}
#endif
#endif
void aos_free(void *mem)
{
#if defined(CFG_USE_PSRAM)
    vPortFreePsram(mem);
#else
    vPortFree(mem);
#endif /* CFG_USE_PSRAM */
}

void aos_msleep(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}
#if 0
#if !defined(USE_STDLIB_MALLOC)
void *malloc(size_t size)
{
	if (!size) {
		return NULL;
	}
    return pvPortMalloc(size);
}
#endif
#endif
long long aos_now_ms(void)
{          
    long long ms;
    TickType_t ticks = 0;
    BaseType_t overflow_count = 0;

    if (pdTRUE != xTaskGetTickCount2(&ticks, &overflow_count)) {
        while (1) {
            /*empty here*/
        };
    }

    //TODO test this formula
    ms = ((long long)ticks) + ((TickType_t)(-1) * ((long long)overflow_count));

    return ms;
}          

