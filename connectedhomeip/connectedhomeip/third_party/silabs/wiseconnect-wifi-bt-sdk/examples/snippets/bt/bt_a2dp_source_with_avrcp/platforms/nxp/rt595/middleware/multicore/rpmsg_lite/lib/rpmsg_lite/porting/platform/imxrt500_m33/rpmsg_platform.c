/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>

#include "rpmsg_platform.h"
#include "rpmsg_env.h"

#include "fsl_device_registers.h"
#include "fsl_mu.h"

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
#include "mcmgr.h"
#endif

#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif

#define APP_MU_IRQ_PRIORITY (3U)

/* The MU instance used for CM33 and DSP core communication */
#define APP_MU MUA
#define APP_MU_IRQn MU_A_IRQn

static int isr_counter = 0;
static int disable_counter = 0;
static void *platform_lock;

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
static void mcmgr_event_handler(uint16_t vring_idx, void *context)
{
    env_isr((unsigned int)vring_idx);
}
#else
void MU_A_IRQHandler(void)
{
    uint32_t channel;

    if (((1U << 27U) >> RPMSG_MU_CHANNEL) & MU_GetStatusFlags(APP_MU))
    {
        channel = MU_ReceiveMsgNonBlocking(APP_MU, RPMSG_MU_CHANNEL); /* Read message from RX register. */
        env_isr(RL_GET_VQ_ID(RL_PLATFORM_LPC6324_M33_DSP_LINK_ID,RL_GET_Q_ID(channel >> 16)));
    }

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif


static void platform_global_isr_disable(void)
{
    __asm volatile("cpsid i");
}


static void platform_global_isr_enable(void)
{
    __asm volatile("cpsie i");
}


int platform_init_interrupt(unsigned int vector_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    env_lock_mutex(platform_lock);

    RL_ASSERT(0 <= isr_counter);
    if (isr_counter == 0)
	{
        MU_EnableInterrupts(APP_MU, (1U << 27U) >> RPMSG_MU_CHANNEL);
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
        MU_DisableInterrupts(APP_MU, (1U << 27U) >> RPMSG_MU_CHANNEL);
    }

    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    env_unlock_mutex(platform_lock);

    return 0;
}

void platform_notify(unsigned int vector_id)
{
    uint32_t msg = (uint32_t)(vector_id << 16);

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    env_lock_mutex(platform_lock);
    MCMGR_TriggerEvent(kMCMGR_RemoteRPMsgEvent, RL_GET_Q_ID(vector_id));
    env_unlock_mutex(platform_lock);
#else
    switch (RL_GET_LINK_ID(vector_id))
    {
        case 0:
            env_lock_mutex(platform_lock);
            MU_SendMsg(APP_MU, RPMSG_MU_CHANNEL, msg);
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
    uint32_t loop;

    /* Recalculate the CPU frequency */
    SystemCoreClockUpdate();

    /* Calculate the CPU loops to delay, each loop has 3 cycles */
    loop = SystemCoreClock / 3U / 1000U * num_msec;

    /* There's some difference among toolchains, 3 or 4 cycles each loop */
    while (loop)
    {
        __NOP();
        loop--;
    }
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
    return (((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0UL) ? 1 : 0);
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

    platform_global_isr_disable();
    disable_counter--;

    if (disable_counter == 0)
    {
        NVIC_EnableIRQ(APP_MU_IRQn);
    }
    platform_global_isr_enable();
    return ((int)vector_id);
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

    platform_global_isr_disable();
    /* virtqueues use the same NVIC vector
       if counter is set - the interrupts are disabled */
    if (disable_counter == 0)
    {
        NVIC_DisableIRQ(APP_MU_IRQn);
    }
    disable_counter++;
    platform_global_isr_enable();
    return ((int)vector_id);
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
 * Translate CM33 addresses to DSP addresses
 *
 */
unsigned long platform_vatopa(void *addr)
{
    return (((unsigned long)addr & 0x0FFFFFFF) + 0x800000);
}

/**
 * platform_patova
 *
 * Translate DSP addresses to CM33 addresses
 *
 */
void *platform_patova(unsigned long addr)
{
    return (void*)((addr - 0x00800000) | 0x20000000);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int platform_init(void)
{
#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    mcmgr_status_t retval = kStatus_MCMGR_Error;
    retval = MCMGR_RegisterEvent(kMCMGR_RemoteRPMsgEvent, mcmgr_event_handler, ((void *)0));
    if(kStatus_MCMGR_Success != retval)
    {
        return -1;
    }
#else
    MU_Init(APP_MU);
    NVIC_SetPriority(APP_MU_IRQn, APP_MU_IRQ_PRIORITY);
    NVIC_EnableIRQ(APP_MU_IRQn);   
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

    MU_Deinit(APP_MU);

    /* Delete lock used in multi-instanced RPMsg */
    env_delete_mutex(platform_lock);
    platform_lock = ((void *)0);
    return 0;
}
