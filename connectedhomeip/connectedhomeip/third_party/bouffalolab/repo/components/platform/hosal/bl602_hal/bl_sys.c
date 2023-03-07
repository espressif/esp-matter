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
#include <bl602_romdriver.h>
#include <bl602_glb.h>

#include <stdio.h>
#include <stdbool.h>
#include "bl_sys.h"
#include <bl_flash.h>
#define MFG_CONFIG_REG    (0x4000F100)
#define MFG_CONFIG_VAL    ("0mfg")

#define REASON_WDT        (0x77646F67) // watchdog reboot wdog
#define REASON_SOFTWARE   (0x736F6674) // software        soft
#define REASON_POWEROFF   (0x0) // software        soft

#define RST_REASON (*((volatile uint32_t *)0x40010000)) // use 4 Bytes

static BL_RST_REASON_E s_rst_reason = BL_RST_POWER_OFF;

static char *RST_REASON_ARRAY[] = {
    "BL_RST_POWER_OFF",
    "BL_RST_HARDWARE_WATCHDOG",
    "BL_RST_FATAL_EXCEPTION",
    "BL_RST_SOFTWARE_WATCHDOG",
    "BL_RST_SOFTWARE"
};
volatile bool sys_log_all_enable = true;

BL_RST_REASON_E bl_sys_rstinfo_get(void)
{
    BL_RST_REASON_E ret = s_rst_reason;

    s_rst_reason = REASON_POWEROFF;

    return ret;
}

int bl_sys_rstinfo_set(BL_RST_REASON_E val)
{
    if (val == BL_RST_SOFTWARE_WATCHDOG) {
        RST_REASON = REASON_WDT;
    } else if (val == BL_RST_SOFTWARE) {
        RST_REASON = REASON_SOFTWARE;
    }

    return 0;
}

void bl_sys_rstinfo_init(void)
{
    if (RST_REASON == REASON_WDT) {
        s_rst_reason = BL_RST_SOFTWARE_WATCHDOG;
    } else if (RST_REASON == REASON_SOFTWARE) {
        s_rst_reason = BL_RST_SOFTWARE;
    } else {
        s_rst_reason = BL_RST_POWER_OFF;
    }

    bl_sys_rstinfo_set(BL_RST_SOFTWARE_WATCHDOG);
}

int bl_sys_rstinfo_getsting(char *info)
{
    memcpy(info, (char *)RST_REASON_ARRAY[s_rst_reason], strlen(RST_REASON_ARRAY[s_rst_reason]));
    *(info + strlen(RST_REASON_ARRAY[s_rst_reason])) = '\0';
    return 0;
}

int bl_sys_logall_enable(void)
{
    sys_log_all_enable = true;
    return 0;
}

int bl_sys_logall_disable(void)
{
    sys_log_all_enable = false;
    return 0;
}

void bl_sys_mfg_config(void)
{
    union _reg_t {
        uint8_t byte[4];
        uint32_t word;
    } mfg = {
        .byte = MFG_CONFIG_VAL,
    };

    *(volatile uint32_t*)(MFG_CONFIG_REG) = mfg.word;
}

int bl_sys_reset_por(void)
{
    bl_sys_rstinfo_set(BL_RST_SOFTWARE);
    __disable_irq();
    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }

    return 0;
}

void bl_sys_reset_system(void)
{
    __disable_irq();
    GLB_SW_System_Reset();
    while (1) {
        /*empty dead loop*/
    }
}

int bl_sys_isxipaddr(uint32_t addr)
{
    if (((addr & 0xFF000000) == 0x23000000) || ((addr & 0xFF000000) == 0x43000000)) {
        return 1;
    }
    return 0;
}

int bl_sys_em_config(void)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    switch (em_size) {
        case 0 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_0KB);
        }
        break;
        case 8 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_8KB);
        }
        break;
        case 16 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_16KB);
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }

    return 0;
}

int bl_sys_early_init(void)
{
#if 0
    // hclk 40 Mhz
    GLB_Set_System_CLK_Div(3,0);
    SystemCoreClockSet(40*1000*1000);
#else
    // default 80Mhz
#endif
    /* read flash config*/
    bl_flash_init();

    /* we ensure that the vdd core voltage is normal(1.2V) and the chip will work normally */
    uint8_t Ldo11VoutSelValue;
    extern BL_Err_Type EF_Ctrl_Read_Ldo11VoutSel_Opt(uint8_t *Ldo11VoutSelValue);
    
    if(0 == EF_Ctrl_Read_Ldo11VoutSel_Opt(&Ldo11VoutSelValue)){
        HBN_Set_Ldo11_Soc_Vout((HBN_LDO_LEVEL_Type)Ldo11VoutSelValue);
    }

    extern BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg);
    HBN_Aon_Pad_IeSmt_Cfg(1);

    extern void freertos_risc_v_trap_handler(void); //freertos_riscv_ram/portable/GCC/RISC-V/portASM.S
    write_csr(mtvec, &freertos_risc_v_trap_handler);
    
    /* reset here for use wtd first then init hwtimer later*/
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_TMR);
    /*debuger may NOT ready don't print anything*/
    return 0;
}

int bl_sys_init(void)
{
    bl_sys_em_config();
    bl_sys_rstinfo_get();
    bl_sys_rstinfo_init();
    return 0;
}
