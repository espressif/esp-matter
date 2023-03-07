#include <stdio.h>

extern uint8_t _heap_start;
extern char *heap_dump_string[];

void __attribute__((section(".bugkiller_code"))) bugkiller_heap_dump()
{
    size_t xAddress;
    size_t BlockSize = 0;
    BlockLink_t *pxBlock;
    uint32_t nbytes = 0;
  
    /* Ensure the heap region starts on a correctly aligned boundary. */
    xAddress = ( size_t ) &_heap_start;
    if( ( xAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
    {
        xAddress += ( portBYTE_ALIGNMENT - 1 );
        xAddress &= ~portBYTE_ALIGNMENT_MASK;
    }

    pxBlock = (BlockLink_t *)xAddress;

    printf(heap_dump_string[0]);
    printf(heap_dump_string[1]);
    
    BlockSize = pxBlock->xBlockSize;

    while (pxBlock != pxEnd) {
        
        BlockSize = pxBlock->xBlockSize;
        /* we may find used block and calculate block size*/
        if (NULL == pxBlock->pxNextFreeBlock) {
            BlockSize &= ~xBlockAllocatedBit;
        }
        
        if (BlockSize) {
            printf(heap_dump_string[2], pxBlock, pxBlock->pxNextFreeBlock, BlockSize);
            pxBlock = (BlockLink_t *)(((uint8_t *)pxBlock) + BlockSize);
        } else {
            pxBlock = pxBlock->pxNextFreeBlock;
        }
    }

    printf(heap_dump_string[3], pxBlock, pxBlock->pxNextFreeBlock, pxBlock->xBlockSize);
    printf(heap_dump_string[4]);
    printf(heap_dump_string[5]);
    pxBlock = xStart.pxNextFreeBlock;
    
    printf(heap_dump_string[6]);
    /* dump free block HeapStruct addr */
    while (pxBlock->pxNextFreeBlock != NULL) {
        printf(heap_dump_string[7], pxBlock, pxBlock->pxNextFreeBlock, pxBlock->xBlockSize);
        nbytes += pxBlock->xBlockSize;
        
        pxBlock = pxBlock->pxNextFreeBlock;
       
    }
    printf(heap_dump_string[8], pxBlock, pxBlock->pxNextFreeBlock, pxBlock->xBlockSize);
    
    printf(heap_dump_string[9], xPortGetFreeHeapSize(), nbytes);
}

