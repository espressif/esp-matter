/*
 * Copyright (c) 2017, GreenPeak Technologies
 *
 *  User mode bootloader
 *
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
#define GP_COMPONENT_ID 1

#include "global.h"
#include "hal.h"

#include "gpUpgrade.h"

#include "hal_user_license.h"

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER) || defined(GP_DIVERSITY_LOG)
#include "gpBaseComps.h"
#endif

#if defined(GP_DIVERSITY_LOG)
#include "gpLog.h"
#endif

#if defined(GP_DIVERSITY_CUSTOM_NATIVE_USER_LICENSE)
/* Select which native user license to use */
#if   defined(GP_DIVERSITY_QPG6105DK_B01)
#include "native_user_license/P334_QPG6105.c"
#else
#error "define native user license"
#endif
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                   Static functions definitions
 *****************************************************************************/

NORETURN static void Bootloader_ModifySpStartApp(UInt32 r0_vtable_addr);

#if !defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
static Bool Bootloader_SpecialMode(void);
#endif

void Bootloader_JumpToApp(UInt32 startAddress);

/*****************************************************************************
 *                   Const
 *****************************************************************************/
extern UInt32 __app_Start__;
const UInt32 app_StartAddr_Active = (UInt32)&__app_Start__; /* App start addr from linker */


#if defined(GP_COMP_EXTSTORAGE)
/* Initialize copy attempt counter  */
const UInt32 umb_failed_copy_attempts LINKER_SECTION(".umb_failed_copy_attempts") = 0; /* No attempts */
#endif


/*****************************************************************************
 *                   Static functions
 *****************************************************************************/
static void umb_blink(UInt8 count)
{
#if defined(GP_DIVERSITY_DEBUG_LED)
    while(count--)
    {
        HAL_LED_SET(RED);
        HAL_LED_SET(GRN);
        HAL_WAIT_MS(10);
        HAL_LED_CLR(RED);
        HAL_LED_CLR(GRN);
        HAL_WAIT_MS(30);
        HAL_LED_CLR(RED);
    }
    HAL_WAIT_MS(30);
#endif
}
/*****************************************************************************
 *                   Jump functions
 *****************************************************************************/

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
void  gpUpgrade_SecureBoot_cbFatalError(gpUpgrade_SecureBoot_Status_t status)
{

#if defined(GP_DIVERSITY_LOG)
    GP_LOG_SYSTEM_PRINTF("FATAL ERROR: %d",0, status);
    gpLog_Flush();
#endif
    while(1);
}
#endif


#if !defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
static Bool Bootloader_SpecialMode(void)
{
    //SW upgrades can fail for numerous reasons. In the field a fallback mechanism should be devised as well to go back
    //to a golden image. In a Dual copy (rescue) strategy, a copy of a known-good application image is kept as a rescue
    //image. If the new application has any bugs and it cannot operate as expected this rescue image solution can provide
    //a solution as well.
    //This function is here to implement a scenario to fallback to such rescue image. Checks can be added here to
    //know if the rescue image needs to be installed or not. An example check is to check for certain key combo that is
    //pressed during start-up of the application. This can enable the special mode, telling the bootloader it needs to
    //program the golde image to become the active application.
    //Rescue image or Dual-copy is currently not implemented in the user mode bootloader example, therefore an additional
    //flash space (external of internal) needs to be foreseen.
    return false;
}
#endif

#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

/* Set stack pointer and jump to the active application reset handler, this call does not return */
NORETURN static void Bootloader_ModifySpStartApp(UInt32 r0_vtable_addr)
{
    asm("LDR         sp, [r0, #0]");    /* SP */
    asm("LDR         r1, [r0, #4]");    /* Reset handler */
    asm("BX          r1");
    while(1);
}

#if defined(__GNUC__)
#pragma GCC pop_options
#endif


/* Relocate to the active user application vector table
 * - By default assume there is only one application in the program memory.
 *   The start address is specified in the linker, vector table pointer is placed at a fixed offset from start addr.
 * - If multiple applications (OTA image for firmware upgrade) are present in the program memory then
 *   this function must be modified to enable the bootloader to know the start addresses and jump to the active application
 */

void Bootloader_JumpToApp(UInt32 startAddress)
{
    UInt32 vector_table_addr;


#if defined(GP_DIVERSITY_GPHAL_K8E)
#if (defined(GP_DIVERSITY_LOG) && defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    /* BBPLL was enabled for accurate UART TX clock, now stop it */
    /* When using lzma compression, BBPLL was running to activate 64MHz clock on the MCU */
    GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(0);
    rap_bbpll_stop();
#endif
#endif /* GP_DIVERSITY_GPHAL_K8C) || GP_DIVERSITY_GPHAL_K8D || GP_DIVERSITY_GPHAL_K8E */

#if  defined(GP_DIVERSITY_GPHAL_K8E)
    /* to boot, we fake we come from ROM bootloader */
    GP_WB_WRITE_CORTEXM4_VECTOR_TABLE_OFFSET( GP_MM_ROM_START >> 8);

    if(GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT())
    {
#if   defined(GP_DIVERSITY_GPHAL_K8E)
        /* Clear entire SYSRAM memory */
        memset((void*)(GP_MM_RAM_LINEAR_START), 0,  GP_MM_RAM_LINEAR_SIZE);
#endif
    }
#endif
    umb_blink(6);
    MEMCPY((void*)&vector_table_addr, (void*)(startAddress), 4);

    __disable_irq();  //disable interrupts
    __set_BASEPRI(0); // turn of priority-based masking

    SCB->VTOR = vector_table_addr << 8;

    /* VTOR is known now*/
    Bootloader_ModifySpStartApp(SCB->VTOR);
    /* Does not return */
}

/* Bootloader panic handling, spin lock */
void Bootloader_Panic(void)
{
    HAL_WAIT_MS(5000);
    GP_WB_WRITE_PMUD_SOFT_POR_BOOTLOADER(1);
    while(1)
    {
#if defined(GP_DIVERSITY_DEBUG_LED)
        UInt32 delay;
        delay = 0xFFFFFF;
        HAL_LED_SET(RED);
        while(delay--);
        HAL_LED_CLR(RED);
#endif
    }
}

/*****************************************************************************
 *                   main
 *****************************************************************************/

int main(void)
{
    UInt32 magicWord;
    HAL_LED_INIT_LEDS();

    if (GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT())
    {
        /* Upgrade flow only on POR */
        UInt8 resetReason = GP_WB_READ_PMUD_POR_REASON();
#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER) || defined(GP_DIVERSITY_LOG)
        gpBaseComps_StackInit();
#endif

#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("app_StartAddr_Active: 0x%lx resetReason: %x",0, app_StartAddr_Active, resetReason);
        gpLog_Flush();
        HAL_WAIT_MS(100);
#endif

        /* PORD reason that trigger a software upgrade */
        if (resetReason == GP_WB_ENUM_POR_REASON_HW_POR                 /* chip reset */
                || resetReason == GP_WB_ENUM_POR_REASON_SOFT_POR_BOOTLOADER /* reset via ROM bootloader */
#if  defined(GP_DIVERSITY_GPHAL_K8E)
                || resetReason == GP_WB_ENUM_POR_REASON_SOFT_POR_PRESERVE_DBG_ITF /* reset via debugger */
#endif
                || resetReason == GP_WB_ENUM_POR_REASON_SOFT_POR_BY_WATCHDOG /* reset via WDT timeout */)

        {
#ifndef GP_DIVERSITY_APP_LICENSE_BASED_BOOT
            /* (OPTIONAL) Special mode for golden/rescue image install */
            if (Bootloader_SpecialMode())
            {
                /* Rescue image install */
                gpUpgrade_InstallRescueImage(); /* Install */
                gpUpgrade_ClrPendingImage();
                gpUpgrade_SetUpgradeHandled(gpUpgrade_StatusFailedRescueInstalled);
            }

            if(gpUpgrade_IsImagePending())
            {
                /* New image pending to be installed */
                gpUpgrade_Status_t upgStatus = gpUpgrade_InstallPendingImage(); /* Install */

                /* Upgrade status flow */
                if((upgStatus == gpUpgrade_StatusFailedChecksumError) || \
                   (upgStatus == gpUpgrade_StatusPreCheckFailed) || \
                   (upgStatus == gpUpgrade_StatusInvalidAddress))
                {
                    /* Clear pending image and write upgrade status */
                    gpUpgrade_ClrPendingImage();
                    gpUpgrade_SetUpgradeHandled(upgStatus);

                    //start current application
                }
                else if(upgStatus == gpUpgrade_StatusSuccess)
                {
                    /* Clear pending image and write upgrade status */
                    gpUpgrade_ClrPendingImage();
                    gpUpgrade_SetUpgradeHandled(upgStatus);

                    //start new application
                    gpUpgrade_SetPendingToActive();
                }
                else
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("Bootloader_Panic",0);
#endif
                    Bootloader_Panic();
                }
            }
            
#else /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */
            /* select the application that is most recent (highest freshness counter) */
#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
            gpUpgrade_SecureBoot_selectActiveApplication();
#else
            gpUpgrade_selectActiveApplication();
#endif
#endif /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */
        }
    }


    memcpy((void*)&magicWord, (void*)(app_StartAddr_Active + USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET), 4);
    if (magicWord != USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
    {
#ifdef GP_DIVERSITY_LOG
        GP_LOG_SYSTEM_PRINTF("Start 0x%lx",0, app_StartAddr_Active); 
        gpLog_Flush();
        GP_LOG_SYSTEM_PRINTF("Unexpected application. mw: %lx",0, magicWord);
        gpLog_PrintBuffer(4,(unsigned char*)app_StartAddr_Active);
#endif // GP_DIVERSITY_LOG
        //unexpected application
        Bootloader_Panic();
    }

}



