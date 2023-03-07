#include "bflb_platform.h"
static uint8_t mallocBuf[64*1024] __attribute__((section(".noinit_data")));
static uint32_t malloced=0;

void simple_malloc_init(void)
{
    malloced=0;
}
void * simple_malloc(uint32_t size)
{ 
    uint8_t *p;
    MSG_DBG("Simple Malloc %d\r\n",size);
    if(malloced+size<sizeof(mallocBuf)){
        p=mallocBuf+malloced;
        malloced+=size;
        return p; 
    }
    return NULL;
}

void simple_free(void *p)
{
    MSG_DBG("Simple Free %08x\r\n",p);
    return ;
}
