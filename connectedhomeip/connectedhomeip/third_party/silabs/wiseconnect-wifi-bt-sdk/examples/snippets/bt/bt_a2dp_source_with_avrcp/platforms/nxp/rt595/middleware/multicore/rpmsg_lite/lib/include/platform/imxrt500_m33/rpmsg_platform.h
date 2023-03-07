/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _RPMSG_PLATFORM_H
#define _RPMSG_PLATFORM_H

/* RPMSG MU channel index */
#define RPMSG_MU_CHANNEL (1)

/*
* No need to align the VRING as defined in Linux because LPC5588 is not intended
* to run the Linux
*/
#ifndef VRING_ALIGN
#define VRING_ALIGN (0x10U)
#endif

/* contains pool of descriptos and two circular buffers */
#ifndef VRING_SIZE
#define VRING_SIZE (0x400UL)
#endif

/* size of shared memory + 2*VRING size */
#define RL_VRING_OVERHEAD (2UL * VRING_SIZE)

#define RL_GET_VQ_ID(link_id, queue_id) (((queue_id)&0x1U) | (((link_id) << 1U) & 0xFFFFFFFEU))
#define RL_GET_LINK_ID(id) (((id)&0xFFFFFFFEU) >> 1U)
#define RL_GET_Q_ID(id) ((id)&0x1U)

#define RL_PLATFORM_LPC6324_M33_DSP_LINK_ID (0U)
#define RL_PLATFORM_HIGHEST_LINK_ID (0U)

/* platform interrupt related functions */
int platform_init_interrupt(unsigned int vector_id, void *isr_data);
int platform_deinit_interrupt(unsigned int vector_id);
int platform_interrupt_enable(unsigned int vector_id);
int platform_interrupt_disable(unsigned int vector_id);
int platform_in_isr(void);
void platform_notify(unsigned int vector_id);

/* platform low-level time-delay (busy loop) */
void platform_time_delay(unsigned int num_msec);

/* platform memory functions */
void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr, unsigned int size, unsigned int flags);
void platform_cache_all_flush_invalidate(void);
void platform_cache_disable(void);
unsigned long platform_vatopa(void *addr);
void *platform_patova(unsigned long addr);

/* platform init/deinit */
int platform_init(void);
int platform_deinit(void);

#endif /* _RPMSG_PLATFORM_H */
