/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rpmsg_platform.h"
#include "rpmsg_env.h"
#include <xtensa/config/core.h>
#include <xtensa/xos.h>
#include "fsl_device_registers.h"
#include "fsl_mu.h"

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
//@Lei
//#include "mcmgr.h"
#endif

static int isr_counter = 0;
static int disable_counter = 0;
static void *platform_lock;

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
static void mcmgr_event_handler(uint16_t vring_idx, void *context)
{
    env_isr((unsigned int)vring_idx);
}
#else
void MU_B_IRQHandler(void *arg)
{
    uint32_t channel;

    if (((1U << 27U) >> RPMSG_MU_CHANNEL) & MUB->SR)
    {
        channel = MUB->RR[RPMSG_MU_CHANNEL]; /* Read message from RX register. */
        env_isr(RL_GET_VQ_ID(RPMSG_LITE_LINK_ID,RL_GET_Q_ID(channel >> 16)));
    }

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

int platform_init_interrupt(unsigned int vector_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    env_lock_mutex(platform_lock);

    RL_ASSERT(0 <= isr_counter);

    if (isr_counter == 0)
    {
        /* @Yuan Enable MUB receive interrupt. */
    	uint32_t reg = MUB->CR & ~(0xf0008);
        MUB->CR = reg | ((1U << 27U) >> RPMSG_MU_CHANNEL);

#if defined(__CM4_CMSIS_VERSION)
        NVIC_SetPriority(MAILBOX_IRQn, 5);
#elif defined (__XCC__)

/*
 * @Lei Register interrupt based on vq_id. It has to be hashed as
 * 0 is NMI
 * 1 is SW
 * 2 & 3 is Timer, Timer.0 & Timer.1
 * 4 is profiling
 * 5~15 level 1 interrupts
 * 16~23 level 2 interrupts
 * 24~31 level 3 interrupts
 * Note that different priority interrupts are not nesting in one stack
 */
    //_xtos_set_interrupt_handler(6, MU_B_IRQHandler);
    xos_register_interrupt_handler(6, MU_B_IRQHandler, ((void *)0));

#else
        NVIC_SetPriority(MAILBOX_IRQn, 2);
#endif
    }
    isr_counter++;

    env_unlock_mutex(platform_lock);

    return 0;
}

int platform_deinit_interrupt(unsigned int vector_id)
{
    /* Prepare the MU Hardware */
    env_lock_mutex(platform_lock);

    RL_ASSERT(0 < isr_counter);
    isr_counter--;
    if (isr_counter == 0)
    {
    	/* @Yuan Disable MUB receive interrupt. */
    	uint32_t reg = MUB->CR & ~(0xf0008 | ((1U << 27U) >> RPMSG_MU_CHANNEL));
        MUB->CR = reg;
#if defined (__XCC__)
/*
 * @Lei Set handler to 0 to cease the interrupt.
 */
//    	_xtos_set_interrupt_handler(6, ((void *)0));
        xos_register_interrupt_handler(6, ((void *)0), ((void *)0));
#else
        NVIC_DisableIRQ(MAILBOX_IRQn);
#endif
    }

    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    env_unlock_mutex(platform_lock);

    return 0;
}

void platform_notify(unsigned int vector_id)
{
    uint32_t msg = (uint32_t)(vector_id << 16);
//#if defined (__XCC__)
/*
 * @Lei MCMGR not supported at this moment.
 */
#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    env_lock_mutex(platform_lock);
    MCMGR_TriggerEvent(kMCMGR_RemoteRPMsgEvent, (uint16_t)RL_GET_Q_ID(vector_id));
    env_unlock_mutex(platform_lock);
#else
    switch (RL_GET_LINK_ID(vector_id))
    {
        case RPMSG_LITE_LINK_ID:
            env_lock_mutex(platform_lock);
            while (!(MUB->SR & ((1U << 23) >> RPMSG_MU_CHANNEL)))
            {
            }

            MUB->TR[RPMSG_MU_CHANNEL] = msg;
            env_unlock_mutex(platform_lock);
            return;
        default:
            return;
    }
#endif
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(unsigned int num_msec)
{
/*
 * @Lei Cycle accurate time delay.
 */
#if defined (__XCC__)
//8MHz main clock on FPGA
#define SystemCoreClock 8000000

    uint32_t loop, ccount;

    /* Get current cycle count */
    ccount = xthal_get_ccount();
    /* Calculate cycles to be delayed */
    loop = SystemCoreClock / 1000U * num_msec;

	do
	{
        loop -= xthal_get_ccount() - ccount;
    } while ( loop>0 );
#endif
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int platform_in_isr(void)
{
/*
 * @Lei HIFI doesn't have direct API or a register file to indicate the core in IRQ. TBD.
 * Always return false.
 */
#if defined (__XCC__)
	return 0;
#elif
    return (((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0UL) ? 1 : 0);
#endif
}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int platform_interrupt_enable(unsigned int vector_id)
{
    RL_ASSERT(0 < disable_counter);
/*
 * @Lei Enable the interrupt. Don't forget to hash the ID.
 * @Yuan Select MUB interrupt as DSP IRQ6 source.
 *
 */

#if defined (__XCC__)
//    _xtos_ints_on(1<<(6));
    xos_interrupt_enable(6);
    disable_counter--;
    return ((int)vector_id);
#elif
    __asm volatile("cpsid i");
    disable_counter--;

    if (disable_counter == 0)
        NVIC_EnableIRQ(MAILBOX_IRQn);
    __asm volatile("cpsie i");
    return ((int)vector_id);
#endif
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int platform_interrupt_disable(unsigned int vector_id)
{
    RL_ASSERT(0 <= disable_counter);

    /*
     * @Lei Disable the interrupt. Don't forget to hash the ID.
     */
#if defined (__XCC__)
//    _xtos_ints_off(1<<(6));
    xos_interrupt_disable(6);
    disable_counter++;
    return ((int)vector_id);
#elif
    __asm volatile("cpsid i");
    // virtqueues use the same NVIC vector
    // if counter is set - the interrupts are disabled
    if (disable_counter == 0)
        NVIC_DisableIRQ(MAILBOX_IRQn);

    disable_counter++;
    __asm volatile("cpsie i");
    return ((int)vector_id);
#endif
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr, unsigned int size, unsigned int flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate(void)
{
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable(void)
{
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
unsigned long platform_vatopa(void *addr)
{
    return ((unsigned long)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(unsigned long addr)
{
    return ((void *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int platform_init(void)
{
#if defined (__XCC__)
/*
 * @Lei MCMGR not supported at this moment.
 */
	xthal_set_region_attribute((void *)RPMSG_LITE_SHMEM_BASE, RPMSG_LITE_SHMEM_SIZE, 0x2, 0);
#elif defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    MCMGR_RegisterEvent(kMCMGR_RemoteRPMsgEvent, mcmgr_event_handler, ((void *)0));
#else
    MAILBOX_Init(MAILBOX);
#endif

    /* Create lock used in multi-instanced RPMsg */
    if(0 != env_create_mutex(&platform_lock, 1))
    {
        return -1;
    }

    return 0;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int platform_deinit(void)
{
#if defined (__XCC__)
/*
 * @Lei MCMGR not supported at this moment.
 */
/* Import from LPC5411x and keep it here - do not deinit mailbox, if there
   is a pending ISR on the other core! */
#elif defined(__CM4_CMSIS_VERSION)
    while (0 != MAILBOX_GetValue(MAILBOX, kMAILBOX_CM0Plus))
        ;
#else
    while (0 != MAILBOX_GetValue(MAILBOX, kMAILBOX_CM4))
        ;
#endif

//    MAILBOX_Deinit(MAILBOX);

    /* Delete lock used in multi-instanced RPMsg */
    env_delete_mutex(platform_lock);
    platform_lock = ((void *)0);
    return 0;
}
