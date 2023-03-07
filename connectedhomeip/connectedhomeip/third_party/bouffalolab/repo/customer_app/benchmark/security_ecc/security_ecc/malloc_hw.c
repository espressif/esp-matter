#include "malloc_hw.h"

#include <FreeRTOS.h>
#include "string.h"
#include "stdio.h"

#define MEM_UNIT_SIZE            1024

#define DTCM_ADDR_START     0X2014000
#define DTCM_ADDR_END       (DTCM_ADDR_START + (48 * 1024))
#define OCRAM_ADDR_START    0X2020000
#define OCRAM_ADDR_END      (OCRAM_ADDR_START + (64 * 1024))
#define WRAM_ADDR_START     0X2030000
#define WRAM_ADDR_END       (WRAM_ADDR_START + (112 * 1024))

static const char *g_ram_type[] = {
    "DTCM",
    "OCRAM",
    "WRAM",
};

void *malloc_hw (size_t size, ram_type_t type)
{
    void *ptr;
    uint32_t counts, piece, ptr_piece_num;
    uint32_t *p_heap_addr;
    uint32_t addr_start, addr_end;

    if (type == DTCM) {
        addr_start = DTCM_ADDR_START;
        addr_end = OCRAM_ADDR_END;
    } else if (type == OCRAM) {
        addr_start = OCRAM_ADDR_START;
        addr_end = OCRAM_ADDR_END;
    } else if (type == WRAM) {
        addr_start = WRAM_ADDR_START;
        addr_end = WRAM_ADDR_END;
    } else {
        return NULL;
    }

    ptr_piece_num = xPortGetFreeHeapSize() / MEM_UNIT_SIZE + 1;
    p_heap_addr = pvPortMalloc(ptr_piece_num * 4);
    memset(p_heap_addr, 0, ptr_piece_num * 4);
    if (p_heap_addr == NULL) {
        return NULL;
    }
    
    ptr = NULL;
    counts = 0;
    while (1) {
        ptr = pvPortMalloc(MEM_UNIT_SIZE);

        if (ptr == NULL) {
            goto __exit;
        }

        p_heap_addr[counts++] = (uint32_t)ptr;

        if ((uint32_t)((uint32_t)ptr & 0x0fffffff) >= addr_start) {
            if ((uint32_t)((uint32_t)ptr & 0x0fffffff) <= addr_end) {
                printf("ram type is %s\r\n", g_ram_type[type]);
                ptr = pvPortMalloc(size);
                break;
            }
        }
    }
    for (piece = 0; piece < counts; piece++) {
        vPortFree((uint32_t *)p_heap_addr[piece]);
    }

__exit:
    vPortFree(p_heap_addr);
    return ptr;
}

void free_hw(void *ptr)
{
    vPortFree(ptr);
}
