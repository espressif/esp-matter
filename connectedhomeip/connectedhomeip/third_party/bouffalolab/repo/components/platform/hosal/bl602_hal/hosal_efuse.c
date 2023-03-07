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
#include "blog.h"
#include "bl602_mfg_efuse.h"
#include "bl_irq.h"
#include "hosal_efuse.h"

int hosal_efuse_read(uint32_t addr, uint32_t *data, uint32_t len)
{
    /* we must ensure 4-byte alignment */
    if (addr % 4 || (uint32_t)data % 4) {
        blog_error("parameter error!\r\n");
        return -1;
    }
    
    GLOBAL_IRQ_SAVE();
    mfg_efuse_read(addr, data, len, 1);
    GLOBAL_IRQ_RESTORE();
    
    return 0;
}

int hosal_efuse_write(uint32_t addr, uint32_t *data, uint32_t len)
{
    /* we must ensure 4-byte alignment */
    if (addr % 4 || (uint32_t)data % 4) {
        blog_error("parameter error!\r\n");
        return -1;
    }

    GLOBAL_IRQ_SAVE();
    mfg_efuse_write_pre(addr, data, len);
    mfg_efuse_program();
    GLOBAL_IRQ_RESTORE();

    return 0;
}
