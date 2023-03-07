#include <string.h>
#include <FreeRTOS.h>

void *blcrypto_suite_calloc(size_t numitems, size_t size)
{
    size_t sizel = numitems * size;
    void *ptr = pvPortMalloc(sizel);
    if (ptr) {
        memset(ptr, 0, sizel);
    }
    return ptr;
}

void blcrypto_suite_free(void *ptr)
{
    vPortFree(ptr);
}
