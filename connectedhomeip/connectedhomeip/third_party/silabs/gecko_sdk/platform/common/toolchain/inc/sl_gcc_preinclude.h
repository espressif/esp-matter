/* The startup files contain a stack and heap symbol in addition
 * to the vector table. The size of these internal stack and heap
 * objects depend on the build system providing two macros on the
 * commandline called __STACK_SIZE and __HEAP_SIZE.
 *
 * We provide alternative stack and heap symbols in the sl_memory.c
 * file which can be configured in a separate config file. Go to
 * sl_memory_config.h to configure the stack and heap size. */
#define __STACK_SIZE    0x0
#define __HEAP_SIZE     0x0
