/*
 * Copyright (c) 2017, GreenPeak Technologies
 *
 *   Default handlers for ARM-based devices.
 *
 *
 *                ,               This software is owned by GreenPeak Technologies
 *                g               and protected under applicable copyright laws.
 *               ]&$              It is delivered under the terms of the license
 *               ;QW              and is intended and supplied for use solely and
 *               G##&             exclusively with products manufactured by
 *               N#&0,            GreenPeak Technologies.
 *              +Q*&##
 *              00#Q&&g
 *             ]M8  *&Q           THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *             #N'   Q0&          CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *            i0F j%  NN          IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *           ,&#  ##, "KA         LIMITED TO, IMPLIED WARRANTIES OF
 *           4N  NQ0N  0A         MERCHANTABILITY AND FITNESS FOR A
 *          2W',^^ `48  k#        PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *         40f       ^6 [N        GREENPEAK TECHNOLOGIES B.V. SHALL NOT, IN ANY
 *        jB9         `, 0A       CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *       ,&?             ]G       INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *      ,NF               48      FOR ANY REASON WHATSOEVER.
 *      EF                 @
 *     0!                         $Header$
 *    M'   GreenPeak              $Change$
 *   0'         Technologies      $DateTime$
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpHal.h"
#if   defined(GP_DIVERSITY_GPHAL_K8E)
#include "hal_ROM.h"
#include "rom_jump_table.h"
#endif
#include "hal_defs.h"
#include "hal_user_license.h"
#include "gpAssert.h"

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
#include "gpUpgrade.h"
#include "gpSecureBoot.h"
#include "sx_generic.h"
#include "sx_rng.h"
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
#define RAM_MW_SPI_ACCESS_SIM_MODE (0xCD376510UL)
#define FLASH_USER_APP_ERASE_MW 0xF299F88BUL
#endif
#define HAL_WAIT_TIME_TIME_REF_UPDATE_US    1000
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Linkerscript Symbols Declarations
 *****************************************************************************/
#if defined(__GNUC__) && !defined(__SEGGER_LINKER)
extern const unsigned long _sidata;
extern const unsigned long _sdata;
extern const unsigned long _ldata;
extern const unsigned long _sbss;
extern const unsigned long _lbss;
extern const unsigned long _estack; /* Application stack */
#endif

#if defined(__GNUC__) && !defined(__SES_ARM)
extern void __libc_init_array(void);
#endif

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
extern UInt32 umb_flash_rma_token_start__;
#endif
/*****************************************************************************
 *                    External Function Declarations
 *****************************************************************************/

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/
extern void Bootloader_JumpToApp(UInt32 startAddress);
extern const UInt32 app_StartAddr_Active;
extern void main(void);
extern void __libc_init_array(void);

void init_data_bss()
{
#if defined(__GNUC__)
#if !defined(__SES_ARM)
    /* copy data section */
    __builtin_memcpy((void*)&_sdata, (void*)&_sidata, (size_t)&_ldata);
    /* zero out bss */
    __builtin_memset((void*)&_sbss, 0, (size_t)&_lbss);
    // Initialize C++ constructor/destructor code
    __libc_init_array();
#else
    extern int __segger_cstartup(void);
    __segger_cstartup();
#endif
#elif defined(__IAR_SYSTEMS_ICC__)
    extern void __iar_cstartup(void);
    __iar_cstartup();
#endif
}

void hal_ResetWatchdog(void)
{
}

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
void check_rma(void)
{
    if(gpSecureBoot_CheckRMAMode((UInt32)&umb_flash_rma_token_start__, GP_SECUREBOOT_RMA_ACTION_BULK_ERASE) == true)
    {

        /* Execute Bulk Erase */
        flash_info_t f;
        GP_WB_WRITE_STANDBY_FLASH_GLOBAL_WRITE_LOCK(0);
        __DMB();

        /* Receive flash information from ROM bootloader */
        f = ROM_flash_info();
        f->chip_erase(FLASH_USER_APP_ERASE_MW);
    }
#if defined(GP_APP_DIVERSITY_SBL_SUPPORT_RMA_ACTION_OPEN_MSI)
    else if(gpSecureBoot_CheckRMAMode((UInt32)&umb_flash_rma_token_start__, GP_SECUREBOOT_RMA_ACTION_OPEN_MSI) == true)
    {
       /* Enable interrupt */
        GP_WB_WRITE_INT_CTRL_GLOBAL_INTERRUPTS_ENABLE(1);

        GP_WB_WRITE_IOB_GPIO_0_ALTERNATE_CFG(GP_WB_ENUM_GPIO_0_ALTERNATES_UCSLAVE_INTOUTN << 1 | 1);

        /* Enable pull-up on interrupt pin - gpio 0 */
        GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);

        /* Open SPI port and connect to MSI */
        GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_USE_SPI);

        GP_BSP_SSPI_MISO_INIT();
        GP_BSP_SSPI_MOSI_INIT();
        GP_BSP_SSPI_SCLK_INIT();
        GP_BSP_SSPI_SSN_INIT();

        /* Enable pull-up on SSN - gpio 6 */
        GP_WB_WRITE_IOB_GPIO_6_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);

        /* Indicate simulated Bare Access SPI Mode */
        *(UInt32 *)GP_MM_RAM_MAGIC_WORD_START = RAM_MW_SPI_ACCESS_SIM_MODE;

        GP_WB_WRITE_SPI_SL_ENABLE(1);

        /* Unlock the FLASH for SPI mode */
        GP_WB_WRITE_STANDBY_FLASH_GLOBAL_WRITE_LOCK(0);

        /* Enable IO-ring */
        GP_WB_WRITE_STANDBY_ACTIVATE_IORING(1);
        GP_WB_WRITE_PMUD_GPIO_FUNCTION_ENABLE(1);
        /* Enable main clocks */
        GP_WB_WRITE_STANDBY_ENABLE_MAIN_CLOCKS(1);
        /* Enable main analog LDO */
        GP_WB_WRITE_STANDBY_MASTER_LDO_PUP(1);

        __WFI();
        while(1);
    }
#endif
}
#endif

#if defined(GP_DIVERSITY_SECURE_BOOTLOADER_PRODUCTION)
void lock_debug_port(void)
{
    /* These registers only exist from K8D onwards */
    GP_WB_WRITE_PMUD_DISABLE_UC_DEBUG(1);
    GP_WB_WRITE_PMUD_DISABLE_SWJDP_ITF(1);
}
#endif //defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)

void reset_handler_bl(void)
{
#if defined(GP_DIVERSITY_SECURE_BOOTLOADER_PRODUCTION)
// Disable SWD/JTAG debugger access (can not be re-enabled until POR)
// Should be executed as early as possible to decrease the attack window
    lock_debug_port();
#endif //defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)

    hal_reset_always();


#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
    /* Did we came from POR? */
    if(GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT()
        && GP_WB_READ_CORTEXM4_VECTOR_TABLE_OFFSET() == GP_MM_ROM_START >> 8)
    {
        /* Wait random time to make a voltage glitch attack non-deterministic */

        /* Get random number */
        sx_enable_clock();
        UInt16 rand;
        sx_rng_get_rand((uint8_t*)&rand, sizeof(rand));
        sx_disable_clock();

        /* Execute random wait */
        HAL_WAIT_US(rand);

        /* Check for RMA */
        check_rma();
    }

    gpUpgrade_SecureBoot_LockBootloader();

#endif

    /* Did we come from sleep or POR ? */
    if(GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT()
        && GP_WB_READ_CORTEXM4_VECTOR_TABLE_OFFSET() == GP_MM_ROM_START >> 8)
        {

            init_data_bss();

            /* From POR and expected relocation from ROM BL, go to umboot */
            GP_WB_WRITE_PMUD_PMU_VDDRAM_TUNE(-1);
#if defined(GP_DIVERSITY_GPHAL_K8E)
            if (GP_WB_READ_PMUD_CLK_32M_PUP() != 0)
#endif
            {
                while(!GP_WB_READ_PMUD_CLK_32M_RDY()) {}
            }

            /* Enable IO-ring */
            GP_WB_WRITE_STANDBY_ACTIVATE_IORING(1);
            GP_WB_WRITE_PMUD_GPIO_FUNCTION_ENABLE(1);
            /* Enable main analog LDO */
            GP_WB_WRITE_STANDBY_MASTER_LDO_PUP(1);

#if defined(GP_DIVERSITY_GPHAL_K8E)
#if (defined(GP_DIVERSITY_LOG) && defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
            /* Only enable BBPLL to get an accurate UART tx clock, to get debugging logs out of BL */
            /* When using lzma compression, BBPLL needs to run to activate 64MHz clock on the MCU */
            /* enable XTAL and wait for ready, pup bbpll, calibrate bbpll, close loop */
            rap_bbpll_start();
            /* switch to bbpll clock */
            GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(1);
#endif
#endif

            // We need to explicitely wait until the symbol counter is updated before using it.
            GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_INIT_TIME_REFERENCE_BUSY(), HAL_WAIT_TIME_TIME_REF_UPDATE_US);

            main();
        }



    /* Fall through to application*/
    Bootloader_JumpToApp(app_StartAddr_Active + USER_LICENSE_VPP_OFFSET);
    /* Does not return */

}

void backup_handler(void)
{

}

/* not polled, called by isr (halCortexM4) */
void stbc_handler_impl(void)
{
    if(GP_WB_READ_INT_CTRL_MASKED_STBC_VLT_STATUS_INTERRUPT())
    {
        GP_ASSERT_DEV_EXT(GP_WB_READ_STANDBY_VLT_STATUS_COMPARE_VALUE() == 1);

        // Disable the IO ring
        GP_WB_WRITE_PMUD_GPIO_FUNCTION_ENABLE(0);

        /* if the BOD interrupt triggered, then we need to make sure we don't corrupt */
        /* the flash by disabling the flash and programming a reset after a short delay */
        /* After the delay, the power supply may be again at a high level so we can retry starting up */

        // Enable watchdog to trigger a softpor after 50ms
        GP_WB_WRITE_WATCHDOG_KEY(0x55);
        GP_WB_WRITE_WATCHDOG_TIMEOUT(50 << 6);
        GP_WB_WRITE_WATCHDOG_WATCHDOG_FUNCTION(GP_WB_ENUM_WATCHDOG_FUNCTION_SOFT_POR);
        GP_WB_WRITE_WATCHDOG_ENABLE(1);
        GP_WB_WRITE_WATCHDOG_KEY(0xAA);

        // Turn off flash
        GP_WB_WRITE_STANDBY_KEEP_NVM_ON_DURING_ACTIVE(0);
        // Stop all uController activity
        GP_WB_WRITE_STANDBY_RESET_GPMICRO(1); // stop GPMICRO
        GP_WB_WRITE_STANDBY_RESET_INT_UC(1);  // stop ARM
    }

    if(GP_WB_READ_INT_CTRL_MASKED_STBC_ACTIVE_INTERRUPT())
    {
        /* can be handled via a polled mechanism */
        GP_WB_WRITE_INT_CTRL_MASK_STBC_ACTIVE_INTERRUPT(0);
    }

    if(GP_WB_READ_INT_CTRL_MASKED_STBC_PORD_INTERRUPT())
    {
        /* can be handled via a polled mechanism */
        GP_WB_WRITE_INT_CTRL_MASK_STBC_PORD_INTERRUPT(0);
    }
}
