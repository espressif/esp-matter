
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

#include "bl_platform.h"
#include "bl_irqs.h"
#include "bl_utils.h"
#include "reg_access.h"
#include "reg_ipc_app.h"

/**
 * bl_platform_on - Start the platform
 *
 * @bl_hw Main driver data
 *
 * It starts the platform :
 * - initialize IPC
 * - boot the fw
 * - enable link communication/IRQ
 *
 * Called by 802.11 part
 */
struct ipc_shared_env_tag *ipc_shenv;
int bl_platform_on(struct bl_hw *bl_hw)
{
    int ret;

    ipc_shenv = (struct ipc_shared_env_tag *)(&ipc_shared_env);
    ret = bl_ipc_init(bl_hw, ipc_shenv);
    if (ret) {
        return ret;
    }

    /*clear any pending IRQ*/
    ipc_emb2app_ack_clear(0xFFFFFFFF);
#if 0
    RWNX_REG_WRITE(BOOTROM_ENABLE, BL606_WIFI_PLATFORM_ADDRESS, SYSCTRL_MISC_CNTL_ADDR);
#endif

    return 0;
}

void bl_platform_off(struct bl_hw *bl_hw)
{
    ipc_host_disable_irq(bl_hw->ipc_env, IPC_IRQ_E2A_ALL);
#if 0
    tasklet_kill(&bl_hw->task);
    bl_ipc_deinit(bl_hw);
#endif
}
