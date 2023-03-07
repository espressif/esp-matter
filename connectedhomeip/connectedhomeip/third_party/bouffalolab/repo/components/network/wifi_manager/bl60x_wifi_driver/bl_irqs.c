
/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bl_defs.h"
#include "bl_irqs.h"
#include "ipc_host.h"
#include "bl_os_private.h"

#define REG_SW_SET_PROFILING(env, value)   do{  }while(0)
#define REG_SW_CLEAR_PROFILING(env, value)   do{  }while(0)
#define REG_SW_CLEAR_HOSTBUF_IDX_PROFILING(env)   do{  }while(0)
#define REG_SW_SET_HOSTBUF_IDX_PROFILING(env, val)   do{  }while(0)
#define BIT_0    ( 1 << 0 )
#define BIT_1    ( 1 << 1 )

static uint32_t time_irq_start __attribute__((unused)), time_irq_end __attribute__((unused));

static struct bl_hw *wifi_hw;
#ifdef RWNX_IRQS_DEBUG_ENABLE
#define RWNX_IRQS_DEBUG(...) \
{ \
    bl_os_printf(__VA_ARGS__); \
}
#else
#define RWNX_IRQS_DEBUG(...) do {} while(0)
#endif

int bl_irqs_init(struct bl_hw *bl_hw)
{
    wifi_hw = bl_hw;

    return 0;
}

int bl_irqs_enable(void)
{
    return 0;
}

int bl_irqs_disable(void)
{
    return 0;
}

void bl_irq_bottomhalf(struct bl_hw *bl_hw)
{
    u32 status, statuses = 0;
#ifdef CFG_BL_STATISTIC
    unsigned long now = bl_os_get_time_ms();
#endif

    REG_SW_SET_PROFILING(bl_hw, SW_PROF_RWNX_IPC_IRQ_HDLR);
    status = ipc_host_get_rawstatus(bl_hw->ipc_env);

redo:
    while (status) {
        statuses |= status;
        /* All kinds of IRQs will be handled in one shot (RX, MSG, DBG, ...)
         * this will ack IPC irqs not the cfpga irqs */
        ipc_host_irq(bl_hw->ipc_env, status);
        status = ipc_host_get_rawstatus(bl_hw->ipc_env);
    }
    // bl_os_log_warn("[BH] Handle Event %08X\r\n", statuses);
#ifdef CFG_BL_STATISTIC
    now = bl_os_get_time_ms();
    if (statuses & IPC_IRQ_E2A_RXDESC) {
        bl_hw->stats.last_rx = now;
    }
    if (statuses & IPC_IRQ_E2A_TXCFM) {
        bl_hw->stats.last_tx = now;
    }
#endif

    REG_SW_CLEAR_PROFILING(bl_hw, SW_PROF_RWNX_IPC_IRQ_HDLR);

    ipc_host_enable_irq(bl_hw->ipc_env, IPC_IRQ_E2A_ALL);
    /*we check irq status again, because we think there is corner case here*/
    status = ipc_host_get_rawstatus(bl_hw->ipc_env);
    if (status) {
        goto redo;
    }
}
