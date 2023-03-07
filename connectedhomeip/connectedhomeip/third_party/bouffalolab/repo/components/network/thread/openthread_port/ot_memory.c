#include <assert.h>
#include <stdlib.h>

#include <openthread/platform/memory.h>

#if defined(CFG_USE_PSRAM)
#include "FreeRTOS.h"
#include "task.h"

void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return pvPortCallocPsram(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    vPortFreePsram(aPtr);
}
#else

void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}
#endif