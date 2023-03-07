

#include "threads_platform.h"
#include "threads_interface.h"
#include <aws_iot_error.h>


IoT_Error_t aws_iot_thread_mutex_init(IoT_Mutex_t *pMutex)
{
    if (pMutex == NULL) {
        return NULL_VALUE_ERROR;
    }
    pMutex->lock = xSemaphoreCreateMutex();
    if (pMutex->lock == NULL) {
        return MUTEX_INIT_ERROR;
    }
    return SUCCESS;
}

IoT_Error_t aws_iot_thread_mutex_lock(IoT_Mutex_t *pMutex)
{
    if (pMutex == NULL || pMutex->lock == NULL) {
        return NULL_VALUE_ERROR;
    }
    if (xSemaphoreTake(pMutex->lock, portMAX_DELAY) != pdTRUE ) {
        return MUTEX_LOCK_ERROR;
    }
    return SUCCESS;
}

IoT_Error_t aws_iot_thread_mutex_trylock(IoT_Mutex_t *pMutex)
{
    if (pMutex == NULL || pMutex->lock == NULL) {
        return NULL_VALUE_ERROR;
    }
    if (xSemaphoreTake(pMutex->lock, portMAX_DELAY) != pdTRUE ) {
        return MUTEX_LOCK_ERROR;
    }
    return SUCCESS;
}

IoT_Error_t aws_iot_thread_mutex_unlock(IoT_Mutex_t *pMutex)
{
    if (pMutex == NULL || pMutex->lock == NULL) {
        return NULL_VALUE_ERROR;
    }
    if (xSemaphoreGive(pMutex->lock) != pdTRUE ) {
        return MUTEX_UNLOCK_ERROR;
    }
    return SUCCESS;
}

IoT_Error_t aws_iot_thread_mutex_destroy(IoT_Mutex_t *pMutex)
{
    if (pMutex == NULL || pMutex->lock == NULL) {
        return NULL_VALUE_ERROR;
    }
    vSemaphoreDelete(pMutex->lock);
    return SUCCESS;
}