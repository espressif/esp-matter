#ifndef _BTIF_PLATFORM_H_
#define _BTIF_PLATFORM_H_

#ifdef CHIP_MT7933
#include "hal_nvic.h"
#include "btif_mt7933.h"
#include "memory_attribute.h"
#endif

typedef void (*btif_irq_handler_t)(int, void *);

void btif_platform_request_irq(unsigned int irq, btif_irq_handler_t handler,
							   const char *name, void *dev);
void btif_platform_free_irq(unsigned int irq);
int btif_platform_irq_ctrl(unsigned int irq_id, unsigned char en);
void btif_platform_clk_ctrl(unsigned char en);
void btif_platform_apdma_clk_ctrl(unsigned char en);
void btif_platform_dcache_clean(void);
unsigned char *btif_platform_apdma_get_vfifo(unsigned char dir);

#endif /*_BTIF_PLATFORM_H_*/
