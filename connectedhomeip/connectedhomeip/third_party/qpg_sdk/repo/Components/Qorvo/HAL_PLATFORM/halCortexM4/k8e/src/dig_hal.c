/*
 * HAL routines
 */

#include "hal.h"
#include "hal_ROM.h"
#include "rom_jump_table.h"
#include <setjmp.h>

#include "hal_defs.h"

static const UInt8 smram_table[] = { 14, 12, 12, 0 };
/* TODO: update with UCRAM size update */
static const UInt8 ucram_table[] = { 15, 15, 15, 15, 15, 14, 14, 0 };

/** Below two variables MUST be in retained RAM
 * _jb_sleep[] array must be aligned to 8-byte boundary since
 * save/restore context function are using LDM/STM instructions
 */
#if defined(__GNUC__)
static __attribute__((aligned(8))) UInt32 _jb_sleep[14];
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 8
static UInt32 _jb_sleep[14]; /* save R4-R11, SP, PC, CONTROL register */
#else
#error "Unknown compiler"
#endif

static UInt8   sleep_config;

void hal_init(void)
{
    rap_sys_hal_init();
    hal_set_crc_mode(0xf, 0xf);
}

extern uint32_t hal_save_context(uint32_t buf[]);
extern uint32_t hal_restore_context(uint32_t buf[], uint32_t jump);

void hal_sleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    if (!hal_save_context(_jb_sleep))
    {
        /* If an interupt is triggered the interrupt is immediately cleared in the interrupt handler. If this is
         * done right before going to sleep we won't wake up anymore. Hence, we verify that no interupt has
         * been triggered before going to sleep.
         * If we disable interrupts and an interupt is received afterwards the chip will still wake up from __WFI
         * but the interupt handler will only be executed after re-enabling interrupts.
         * (see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka16222.html).
         */
        /* don't use the HAL_DISABLE_GLOBAL_INT here because it will disable interrupts at
         * the NVIC level, and therefor it might block interrupts which are needed for waking up the ARM.
         * if you block the interrupts at the top-level, individual interrupts can still wake up the ARM
         * see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka16222.html
         */
        __disable_irq();
        __ISB(); /* Make sure interrupts are disabled */
        if (hal_CanGotoSleep() && !hal_HasUnpolledInterruptBeforeSleep())
        {
            __WFI();
        }
    }
    __enable_irq();
    /* We assume all interrupts will have been polled before the next time this function is entered. */
    /* We clear flag after enabling interrupts so that pending interrupts handlers have been done and we
     * do not skip sleep unnecessarily next time we enter this function.
     * We can be interrupted again right before polling starts in which case the interrupt will be handled but
     * we will not go to sleep afterwards. However, one extra iteration is nothing to worry about. */
    hal_ClrUnpolledInterruptBeforeSleep();
}

void hal_resume(void)
{
    hal_restore_context(_jb_sleep, 1);
}

// crc mode configuration for RAM
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// |   UC RAM cfg  |  SYS RAM cfg  |
// * RAM cfg = 0x0, no CRC
// * RAM cfg = 0xf, CRC over retained part
// * SMRAM RAM cfg = [0x1 - 0xe]: CRC over 2 ^ (cfg + 1) bytes
// * UCRAM RAM cfg = [0x2 - 0xe]: CRC over 2 ^ (cfg + 1) bytes
// * UCRAM cfg = 1, special value to select 96k RAM
void hal_set_crc_mode(UInt8 smram, UInt8 ucram)
{
    UInt8 vddram_sel   = GP_WB_READ_PMUD_PMU_VDDRAM_SEL();
    UInt8 size, config = 0;

    /* SM RAM */
    size    = smram < 0xf ? smram : 0xf;
    config |= (size == 0xf ? smram_table[vddram_sel >> 0 & 3] : size) << 0;
    /* UC RAM */
    size    = ucram < 0xf ? ucram : 0xf;
    config |= (size == 0xf ? ucram_table[vddram_sel >> 2 & 7] : size) << 4;

    sleep_config = config;
}

void hal_go_to_sleep(UInt8 clk_mode)
{
    UInt32 config;

    /* ISH and MW/CRC */
    config  = (*(UInt32*) GP_MM_RAM_MAGIC_WORD_START & 8) | (GP_WB_READ_STANDBY_STBY_MODE_0() & 7);
    /* CLOCK */
    config |= (clk_mode & 7) << 5 | (GP_WB_READ_PMUD_STBY_MODE() == GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE) << 4;
    /* CRC */
    config |= sleep_config << 8;

    ROM_go_to_sleep(config);
}

