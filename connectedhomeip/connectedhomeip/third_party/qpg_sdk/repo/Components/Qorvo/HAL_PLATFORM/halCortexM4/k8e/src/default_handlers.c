/*
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 *   Default handlers for ARM-based devices.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "dev_handlers.h"
#include "gpHal_kx_Rap.h"

#include "hal_defs.h"
#include "gpLog.h"
#include "gpHal.h"
#include "gpAssert.h"
#include "gpBsp.h"
#include "gpHal_Calibration.h"
#include "gpStat.h"
#ifndef GP_DIVERSITY_KEEP_NRT_IN_FLASH
#include "gpJumpTables.h"
#endif //GP_DIVERSITY_KEEP_NRT_IN_FLASH

#ifdef GP_COMP_SHMEM
#include "gpShmem.h"
#endif //ifdef GP_COMP_SHMEM
#ifdef GP_DIVERSITY_FREERTOS
#include "FreeRTOSConfig.h"
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4


#define QTA_NUM_PBMS     7
#define RCI_NUM_REQUESTS (32 - 4) // do not retain the last 4 as it makes the custom retention overflow into the default ram_regmap location

#define RAM_MW_RETAINED_SLEEP 0xAABBC000

// GP Micro defines
#define START_ADDRESS_OF_RT_PROG_IN_GPM_PM_ADDR_SPACE 0x10000
#define GPM_WINDOW_GRANULARITY                        128

// Backup/Restore area defines
extern const UIntPtr sw_retention_begin;
extern const UIntPtr sw_retention_end;

#define HAL_SW_RETENTION_BEGIN (UIntPtr) & sw_retention_begin
#define HAL_SW_RETENTION_END   (UIntPtr) & sw_retention_end

#define BBPLL_DCO_COARSE_BACKUP_RESTORE_ADDR HAL_SW_RETENTION_BEGIN
#define SM_LATE_BACKUP_RESTORE_START_ADDR    ((((HAL_SW_RETENTION_BEGIN + 1 /* +1 used to backup BBPLL_DCO_COARSE*/) + 1) / 2) * 2) /* Align to 16-bits. */

/*****************************************************************************
 *                    External Function Definitions
 *****************************************************************************/

extern int main(void);
extern void hal_init(void);

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// Memory defined as UInt32 for 4 byte alignment
// Memory area for RAM based register map
static UInt32 hal_RamRegmap[GP_MM_RAM_RAM_REGMAP_SIZE / sizeof(UInt32)] LINKER_SECTION(".lower_ram_retain");
// Memory area for Event descriptors used by HW ES
static UInt32 hal_AbsoluteEvents[(GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS * GP_MM_RAM_EVENT_SIZE) / sizeof(UInt32)] LINKER_SECTION(".lower_ram_retain");
// Memory area for PBM option fields.
static UInt32 hal_PbmOptions[((GP_HAL_PBM_TYPE1_AMOUNT + GP_HAL_PBM_TYPE2_AMOUNT + GP_HAL_MAX_NR_OF_RESERVED_PBMS) * GP_MM_RAM_PBM_OPTS_SIZE) / sizeof(UInt32)] LINKER_SECTION(".lower_ram_retain");

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static INLINE void init_data_bss(void);
static INLINE void enable_gpio_input_buffers(void);
static INLINE void setup_gpmicro(void);
static INLINE void hal_enableClockOverrides(Bool enable);
static INLINE void wakeup_restore(void);
static INLINE void tosleep_backup(void);

/*****************************************************************************
 *                    External Function Declarations
 *****************************************************************************/

/*
 * IVT references.
 */
extern WEAK INTERRUPT_H void reset_handler(void);
extern WEAK INTERRUPT_H void backup_handler(void);
extern WEAK INTERRUPT_H void hardfault_handler(void);
extern WEAK INTERRUPT_H void memmanage_handler(void);
extern WEAK INTERRUPT_H void busfault_handler(void);
extern WEAK INTERRUPT_H void usagefault_handler(void);
extern WEAK INTERRUPT_H void svcall_handler(void);
extern WEAK INTERRUPT_H void debugmon_handler(void);
extern WEAK INTERRUPT_H void pendsv_handler(void);
extern WEAK INTERRUPT_H void systick_handler(void);

/*****************************************************************************
 *                    Linkerscript Symbols Declarations
 *****************************************************************************/

#include "handler_helpers.c"

#if defined(__GNUC__)
#if !defined(__SEGGER_LINKER)
extern const unsigned long _sidata;
extern const unsigned long _sdata;
extern const unsigned long _ldata;
extern const unsigned long _sbss;
extern const unsigned long _lbss;
extern const unsigned long _estack; /* Application stack */
#endif
extern const unsigned long __stack_end__; /* Application stack */
#if defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
extern const unsigned long _sidata_m;
extern const unsigned long _sdata_m;
extern const unsigned long _ldata_m;
extern const unsigned long _sbss_m;
extern const unsigned long _lbss_m;
#endif //defined(GP_DIVERSITY_ROMBUILD_FOR_MATTER) || defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
#elif defined(__IAR_SYSTEMS_ICC__)
#if defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
#error "Matter ROM build with IAR compiler is not supported yet"
#endif //defined(GP_DIVERSITY_ROMBUILD_FOR_MATTER) || defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
#endif

#ifndef GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
#ifdef GP_COMP_SHMEM
extern const ROM gpShmem_Info_t gpShmemInfo;
#endif
/* Actual IVT */
#if defined(__GNUC__)
__attribute__((section(".isr_vector")))
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".intvec"
#endif
const intvec_elem __vector_table[] = {
    /* Start of stack (grows downwards, so highest address = address where stack starts) */
    {.__ptr = IVT_GET_STACK_POINTER()},
    /* CM4 core handlers */
    {.__fun = reset_handler},      /* Reset */
    {.__fun = backup_handler},     /* NMI */
    {.__fun = hardfault_handler},  /* HardFault */
    {.__fun = memmanage_handler},  /* MemManage */
    {.__fun = busfault_handler},   /* BusFault */
    {.__fun = usagefault_handler}, /* UsageFault */
    {.__ptr = NULL},               /* reserved */
    {.__ptr = NULL},               /* reserved */
    {.__ptr = NULL},               /* reserved */
    {.__ptr = NULL},               /* reserved */
    {.__fun = svcall_handler},     /* SVCall */
    {.__fun = debugmon_handler},   /* DebugMon */
    {.__ptr = NULL},               /* reserved */
    {.__fun = pendsv_handler},     /* PendSV */
    {.__fun = systick_handler},    /* SysTick */
/* chip-specific handlers */
#include "dev_ivt.h"
#ifdef GP_COMP_SHMEM
    ,
    {.__ptr = (void*)&gpShmemInfo} /* reserved */
#endif
};
#endif //ifndef GP_DIVERSITY_CORTEXM4_CUSTOM_IVT

#if defined(__IAR_SYSTEMS_ICC__)
extern __weak void __iar_init_core(void);
extern __weak void __low_level_init(void);
extern __weak void __iar_data_init3(void);
#ifndef GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
#pragma required = __vector_table
#endif //GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
void __iar_cstartup(void)
{
    __iar_init_core();
    __low_level_init();
    __iar_data_init3();
    // startup done -> call main in reset_handler
}
#endif

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

static void set_base_addresses(void)
{
    GP_WB_WRITE_MM_RAM_REGMAP_BASE_ADDRESS((hal_RamRegmap - (UInt32 *) GP_MM_RAM_LINEAR_START));
    GP_WB_WRITE_MM_EVENT_BASE_ADDRESS((hal_AbsoluteEvents - (UInt32 *) GP_MM_RAM_LINEAR_START));
    GP_WB_WRITE_MM_PBM_OPTS_BASE_ADDRESS((hal_PbmOptions - (UInt32 *) GP_MM_RAM_LINEAR_START));
}

#if defined(__GNUC__)
#pragma GCC pop_options
#endif

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/

/** @brief Reset handler is invoked at power-on-reset and when waking up from sleep. */
#ifdef GP_HALCORTEXM4_RESET_HANDLER
void GP_HALCORTEXM4_RESET_HANDLER(void)
#else
void reset_handler(void)
#endif
{
    hal_reset_always();

    Bool sysReset;

    HAL_FORCE_DISABLE_GLOBAL_INT();

    // The reset handler runs after power-on-reset as well as after wakeup from sleep.
    // In case of power-on-reset, the PORD interrupt will be active.
    sysReset = GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT();

    if(!sysReset)
    {
        // Reset handler can also be triggered through SYSRESET or LOCKUP or debugger.
        // In these cases PORD will not be active.
        // Check the magic word to determine if this is actual wake-up or some sort of unplanned reset.
        UInt32 ram_mw = *(UInt32*)GP_MM_RAM_MAGIC_WORD_START;
        if(ram_mw == 0)
        {
            // Not wake-up from sleep. Must be treated as reset.
            sysReset = true;
        }
    }

    // Indicate to calibration routine that chip is coming out of sleep/POR
    gpHal_CalibrationSetFirstAfterWakeup(true);

    if(!sysReset) /* wake-up from sleep */
    {
        // Move stack pointer to avoid overwriting application stack.
        __set_MSP((UInt32)HAL_SP_STACK_END_ADDRESS - HAL_SP_BACKUP_SIZE - HAL_SP_BACKUP_MARGIN);


        // Read magic word, then clear it.
        UInt32 ram_mw = *(UInt32*)GP_MM_RAM_MAGIC_WORD_START;
        // Zero-out magic word
        *(UInt32*)GP_MM_RAM_MAGIC_WORD_START = 0;

        if((ram_mw >> 12) != (RAM_MW_RETAINED_SLEEP >> 12))
        {
            // Waking up from deep sleep (32 kHz sleep or RC sleep).

            GP_WB_WRITE_WATCHDOG_KEY(0x55);

            // Do the real restore work
            wakeup_restore();

            // Enable IO-ring
            GP_WB_WRITE_STANDBY_ACTIVATE_IORING(1);
            // Enable main analog LDO
            GP_WB_WRITE_STANDBY_MASTER_LDO_PUP(1);

            // Restore addtional registers according to BSP retention list
            hal_CustomRestore();

            // Enable the GPMicro
            GP_WB_WRITE_STANDBY_CLK_ENA_GPMICRO(1); // was for power saving, we dont need a clock while in reset
            GP_WB_WRITE_STANDBY_RESET_GPMICRO(0);
        }

        // In case this was used
        GP_WB_WRITE_PMUD_IMMEDIATE_WAKEUP(0);

        // do not start event scheduler before the time ref sync is done
        GP_WB_WRITE_STANDBY_CLK_ENA_ES_EVENT(0);

        if(GP_WB_READ_PMUD_CLK_32M_PUP() != 0)
        {
            while (!GP_WB_READ_PMUD_CLK_32M_RDY()) {}
        }

        // restore clock config in the correct order
        // the config is retrieved from the mw that was written before sleep
        if(ram_mw & 0x2)
        {
            // enable XTAL and wait for ready, pup bbpll, close loop
            GP_WB_WRITE_BBPLL_DCO_COARSE_NEW(GP_WB_READ_U8(BBPLL_DCO_COARSE_BACKUP_RESTORE_ADDR)); // restore the current dco_coarse
            rap_bbpll_resume();
        }
        if(ram_mw & 0x4)
        {
            GP_WB_WRITE_STANDBY_PRESCALE_UCCORE(GP_WB_ENUM_CLOCK_SPEED_M64);
        }
        if(ram_mw & 0x8)
        {
            GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(1);
        }

        // Make sure desense fix overrides are enabled on wake up
        hal_enableClockOverrides(true);

        // enable clocks
        GP_WB_WRITE_STANDBY_ENABLE_MAIN_CLOCKS(1);

        // Restore NVIC state.
        hal_RestoreNvic();

        // We need to explicitly wait until the symbol counter is updated before using it.
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_INIT_TIME_REFERENCE_BUSY(), HAL_WAIT_TIME_TIME_REF_UPDATE_US);
        // enable clocks
        gpHal_ResetHandler_SetSymbolStartupTime();

        // Timebase is correct, clock to es can be re-enabled
        GP_WB_WRITE_STANDBY_CLK_ENA_ES_EVENT(1);

        // Enable interrupts
        GP_WB_WRITE_INT_CTRL_GLOBAL_INTERRUPTS_ENABLE(1);


        // Enable interrupts
        HAL_FORCE_ENABLE_GLOBAL_INT();

        // Log when the chip wakes up from sleep
        GP_STAT_SAMPLE_TIME();

        // Restore top of application stack, , just before longjmp
        MEMCPY((void*)((UInt32)HAL_SP_STACK_END_ADDRESS - HAL_SP_BACKUP_SIZE), hal_sp_backup, HAL_SP_BACKUP_SIZE);

        hal_resume();
    }
    else
    {
        // Starting from power-on-reset.


        // if !0 then started by bootloader from flash, else ramprogram
        if(SCB->VTOR != 0)
        {
            /*
             * Harmonize VTOR and CORTEXM4_VECTOR_TABLE_OFFSET, prevents
             * ending up in the bootloader upon SYSRESET or LOCKUP
             */
            GP_WB_WRITE_CORTEXM4_VECTOR_TABLE_OFFSET(SCB->VTOR >> 8);
        }

        // Init data and bss sections - depends on toolchain
        init_data_bss();

        //Init windows
        set_base_addresses();

        // Init MAC filter
#include "cm_ram_init.c"

        // Enable input buffers on special pins
        enable_gpio_input_buffers();

#ifndef GP_DIVERSITY_FREERTOS
        // Default interrupt priorities -> Busfault and STBC have higher priority than other interrupts
        memset((void*)NVIC->IP, 2 << (8 - __NVIC_PRIO_BITS), 36);
        NVIC_SetPriority(STBC_IRQn, 1);
#else
        /**
         * Set the default priority to configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY to ensure that any ISR sending
         * signal to task, must be lower than or equal to configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
         */
        memset((void*)NVIC->IP, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - __NVIC_PRIO_BITS), 36);
        NVIC_SetPriority(STBC_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1);
#endif

        if(GP_WB_READ_PMUD_CLK_32M_PUP() != 0)
        {
            while(!GP_WB_READ_PMUD_CLK_32M_RDY()) {}
        }

        // Enable main analog LDO
        GP_WB_WRITE_STANDBY_MASTER_LDO_PUP(1);

        // enable XTAL and wait for ready, pup bbpll, calibrate bbpll, close loop
        rap_bbpll_start();

        // switch to bbpll clock
        GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(1);

        // Enable interrupt
        GP_WB_WRITE_INT_CTRL_GLOBAL_INTERRUPTS_ENABLE(1);

        // Setup and enable the GPMicro
        setup_gpmicro();
        GP_WB_WRITE_STANDBY_CLK_ENA_GPMICRO(1); // for power saving, clock is not required while in reset
        GP_WB_WRITE_STANDBY_RESET_GPMICRO(0);

#if !defined(GP_DIVERSITY_KEEP_NRT_IN_FLASH) 
        // Setup ROM jumptables
        gpJumpTables_Init();
#endif //!GP_DIVERSITY_KEEP_NRT_IN_FLASH && !GP_DIVERSITY_ROM_BUILD

#ifdef GP_DIVERSITY_ENABLE_DEFAULT_BOD_HANDLING
        // Do not jump to main if voltage is too low
        while (GP_WB_READ_STANDBY_VLT_STATUS()) {}
#endif //GP_DIVERSITY_ENABLE_DEFAULT_BOD_HANDLING

        HAL_FORCE_ENABLE_GLOBAL_INT();

        // We need to explicitly wait until the symbol counter is updated before using it.
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_INIT_TIME_REFERENCE_BUSY(), HAL_WAIT_TIME_TIME_REF_UPDATE_US);

#ifdef GP_HALCORTEXM4_PRE_MAIN_INIT
        GP_HALCORTEXM4_PRE_MAIN_INIT();
#endif
        main();

        // Poor man's halt
        while (1) {}
    }
}

/** @brief Backup handler is invoked as non-maskable-interrupt when chip is going to sleep. */
void backup_handler(void)
{
    UInt8 clk_mode = (GP_WB_READ_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE() << 2) | (GP_WB_READ_STANDBY_PRESCALE_UCCORE() == GP_WB_ENUM_CLOCK_SPEED_M64) << 1 | GP_WB_READ_BBPLL_LOOP_IS_CLOSED();

    // Disable interrupts
    GP_WB_WRITE_INT_CTRL_GLOBAL_INTERRUPTS_ENABLE(0);
    // Make HAL_WAIT valid
    GP_WB_WRITE_STANDBY_PRESCALE_UCCORE(GP_WB_ENUM_CLOCK_SPEED_M32);
    // Clear PORD interrupt as it is not cleared by itself in 16 MHz sleep mode
    GP_WB_STANDBY_CLR_PORD_INTERRUPT();

    // includes DCDC disabling if needed

    // Apply sleep counter calibration factor and realign sleep counter to symbol counter.
    gpHal_RealignTimebase();

    // Log when the chip goes to sleep
    GP_STAT_SAMPLE_TIME();
    GP_WB_STANDBY_HALT_TIME_REFERENCE();

    // Disable BBPLL and dependent items
    GP_WB_WRITE_U8(BBPLL_DCO_COARSE_BACKUP_RESTORE_ADDR, GP_WB_READ_BBPLL_DCO_COARSE()); // backup the current dco_coarse
    GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(0);
    rap_bbpll_stop();

    // Make sure desense fix overrides are disabled before sleep
    hal_enableClockOverrides(false);

    // Turn off clocks
    GP_WB_WRITE_STANDBY_ENABLE_MAIN_CLOCKS(0);

    // Perform the actual backup
    if(GP_WB_READ_PMUD_STBY_MODE() != GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE)
    {
        // Going to (non-retained) deep sleep (32 kHz sleep or RC sleep).

        // Disable ioring early
        GP_WB_WRITE_STANDBY_ACTIVATE_IORING(0);

        // Sample retention
        GP_WB_STANDBY_SAMPLE_RET();

        // Perform the full backup
        tosleep_backup();

        // Backup additional registers according to BSP retention list
        hal_CustomBackup();
    }

    // Always Backup top of application stack.
    MEMCPY(hal_sp_backup, (void*)((UInt32)HAL_SP_STACK_END_ADDRESS - HAL_SP_BACKUP_SIZE), HAL_SP_BACKUP_SIZE);

    // Backup NVIC state.
    hal_BackupNvic();

    // Dummy write - stall until backup is done
    GP_WB_STANDBY_CLR_BACKUP_INTERRUPT();

    // Do the late overrides just before sleep
    rap_sys_backup_late();

    // Call ROM bootloader to write magic words and enter sleep.
    hal_go_to_sleep(clk_mode); // does not return
}

/*
 * Helper functions, will be inlined or can be moved to compiler, environment
 * differentiated files.
 */

void init_data_bss(void)
{
#if defined(__GNUC__)
#if !defined(__SEGGER_LINKER)
extern void __libc_init_array(void);

    // copy data section
    __builtin_memcpy((void*)&_sdata, (void*)&_sidata, (size_t)&_ldata);
    // zero out bss
    __builtin_memset((void*)&_sbss, 0, (size_t)&_lbss);
#if defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
    // copy data section for the matter rom
    __builtin_memcpy((void*)&_sdata_m, (void*)&_sidata_m, (size_t)&_ldata_m);
    // zero out bss for the matter rom
    __builtin_memset((void*)&_sbss_m, 0, (size_t)&_lbss_m);
#endif //defined(GP_DIVERSITY_ROMBUILD_FOR_MATTER) || defined(GP_DIVERSITY_ROMUSAGE_FOR_MATTER)
    // Initialize C++ constructor/destructor code
    __libc_init_array();
#else
    extern int __segger_cstartup(void);
    __segger_cstartup();
#endif
#elif defined(__IAR_SYSTEMS_ICC__)
    __iar_cstartup();
#endif
}

void enable_gpio_input_buffers(void)
{
    UInt8 u8 = 0;

    GP_WB_SET_IOB_GPIO_17_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
    GP_WB_SET_IOB_GPIO_18_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
    if (!GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
    {
        GP_WB_SET_IOB_GPIO_19_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
        GP_WB_SET_IOB_GPIO_20_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
    }
    GP_WB_SET_IOB_GPIO_21_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
    GP_WB_SET_IOB_GPIO_22_INPUT_BUFFER_ENABLE_TO_GPIO_INPUT_BUFFER_ENABLE(u8, 1);
    GP_WB_WRITE_IOB_GPIO_INPUT_BUFFER_ENABLE(u8);
}

void setup_gpmicro(void)
{
}

static void wakeup_restore(void)
{
    UInt16* ret_area16 = (UInt16*)SM_LATE_BACKUP_RESTORE_START_ADDR;
    UInt8* ret_area8 = (UInt8*)SM_LATE_BACKUP_RESTORE_START_ADDR;
    UIntLoop i, loc;

    GP_WB_MM_RESTORE_START();
    /* DO NOT MOVE following statement!
     * ... ensures the RESTORE is finished (blocking read).
     */
    *(volatile UInt32*)GP_MM_RAM_MAGIC_WORD_START = 0;

    GP_WB_STANDBY_PRELOAD_RET();

    GP_WB_WRITE_ES_SET_VALID_EVENTS(ret_area16[0]);
    loc = 2;

#define RET8(WB_REG)                          \
    do                                        \
    {                                         \
        GP_WB_WRITE_##WB_REG(ret_area8[loc]); \
        ++loc;                                \
    } while(0)

    RET8(RADIO_ARB_CLAIM);

    for(i = 0; i < QTA_NUM_PBMS; ++i)
    {
        RET8(QTA_PBEFE_DATA_REQ);
    }
#undef RET8

    // ALIGN ON 16-bits here
    loc /= 2;
    for(i = 0; i < RCI_NUM_REQUESTS; ++i)
    {
        GP_WB_WRITE_U16(GP_WB_RCI_QUEUE_REQUEST_ENTRY_TYPE_ADDRESS, ret_area16[loc++]);
    }

    // Wait until queue updates done
    while (GP_WB_READ_QTA_BUSY() == 1) {}
}

static void tosleep_backup(void)
{
    UInt16* ret_area16 = (UInt16*)SM_LATE_BACKUP_RESTORE_START_ADDR;
    UInt8* ret_area8 = (UInt8*)SM_LATE_BACKUP_RESTORE_START_ADDR;
    UIntLoop i, off, loc;

    // Do HW SM backup
    GP_WB_MM_BACKUP_START();

    // Save queue contents
    ret_area16[0] = GP_WB_READ_ES_VALID_EVENTS();
    loc = 2;

#define RET8(WB_REG)                              \
    do                                            \
    {                                             \
        ret_area8[loc++] = GP_WB_READ_##WB_REG(); \
    } while(0)

    RET8(RADIO_ARB_CLAIM);
#undef RET8

    off = GP_WB_QTA_PBM_PTR_1_ADDRESS - GP_WB_QTA_PBM_PTR_0_ADDRESS;
    for(i = 0; i < QTA_NUM_PBMS; ++i)
    {
        ret_area8[loc++] = GP_WB_READ_U8(GP_WB_QTA_PBM_PTR_0_ADDRESS + i * off) & GP_WB_QTA_PBEFE_DATA_REQ_MASK;
    }

    // ALIGN ON 16-bits here
    loc /= 2;
    for(i = 0; i < RCI_NUM_REQUESTS; ++i)
    {
        ret_area16[loc++] = GP_WB_READ_U16(GP_WB_RCI_QUEUE_DATA_0_ENTRY_TYPE_ADDRESS + i * 2);
    }
    loc *= 2; // Move back to 8-bit alignment
    // Make sure that backed up data stays within the sw_ret region defined in linker script
    GP_ASSERT_SYSTEM(loc <= (HAL_SW_RETENTION_END - SM_LATE_BACKUP_RESTORE_START_ADDR));
}
static void hal_enableClockOverrides(Bool enable)
{
    GP_WB_WRITE_PMUD_XO_LDO_RDY_OVERRIDE(enable);
    GP_WB_WRITE_PMUD_CLK_32M_RDY_OVERRIDE(enable);
}
