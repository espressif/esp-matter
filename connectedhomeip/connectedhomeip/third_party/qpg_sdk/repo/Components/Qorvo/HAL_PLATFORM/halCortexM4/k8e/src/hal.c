/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for ARM-based devices.
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
//#define GP_LOCAL_LOG
#define GP_COMPONENT_ID     GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#if defined(HAL_DIVERSITY_PUF)
#include "hal_PUF.h"
#endif
#include "hal_defs.h"
#include "hal_timer.h"

#include "gpBsp.h"
#include "gpLog.h"
#include "gpHal.h"
#include "gpHal_Calibration.h"
#include "gpHal_kx_Fll.h"
#include "gpHal_arm.h"
#include "dev_handlers.h"

#include "gpUtils.h"
#include "gpAssert.h"
#ifdef GP_DIVERSITY_FREERTOS
#include "gpSched.h"
#endif

#if defined(GP_DIVERSITY_FREERTOS) && defined(GP_COMP_GPHAL_BLE)
#include "hal_BleFreeRTOS.h"
#endif //GP_DIVERSITY_FREERTOS

#include "hal_ROM.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_WDT_TIMEOUT 0xFFFF /*in 16us*/

#ifdef HAL_DIVERSITY_WDT_DISABLE
  #define HAL_WDT_ENABLED()  (0)
#else
  #define HAL_WDT_ENABLED()  (1)
#endif

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


volatile Bool hal_PolledInterruptPending = false;
#ifndef GP_DIVERSITY_FREERTOS
volatile Bool hal_SysTickInterruptPending = false;
#endif //GP_DIVERSITY_FREERTOS


/*****************************************************************************
 *                    static Function Definitions
 *****************************************************************************/



/*****************************************************************************
 *                    Linkerscript Symbols Declarations
 *****************************************************************************/
#if defined(__GNUC__)
extern unsigned long _estack;
extern unsigned long _sstack;
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#pragma language=extended
#pragma segment="CSTACK"
#endif

/*****************************************************************************
 *                    External Function Declarations
 *****************************************************************************/

#if defined(GP_COMP_GPHAL_BLE) || defined(GP_COMP_GPHAL_MAC)
extern void gpHal_ISR_RCIInterrupt(UInt16 highPrioRciPending);
extern void gpHal_ISR_IPCGPMInterrupt(UInt64 highPrioIpcPending);
#endif //defined(GP_COMP_GPHAL_BLE) || defined(GP_COMP_GPHAL_MAC)

/*****************************************************************************
 *                    Init
 *****************************************************************************/

void hal_Init(void)
{
    //Clr for debugging purposes
    GP_WB_STANDBY_CLR_PORD_INTERRUPT();

    //Active interrupt triggers every wakeup - unused for now, disable
    GP_WB_WRITE_INT_CTRL_MASK_STBC_ACTIVE_INTERRUPT(0);

    // Enable interrupt when vddb < 1.8V (clear any pending interrupts first)
    GP_WB_STANDBY_CLR_VLT_STATUS_INTERRUPT();
    GP_WB_WRITE_INT_CTRL_MASK_STBC_VLT_STATUS_INTERRUPT(1);

    HAL_LED_INIT();
    HAL_BTN_INIT();

    //Call macro directly - avoid inclusion of gphal related material in gpBsp.c
    GP_BSP_GENERIC_INIT();

    //Initialize unused pins
    GP_BSP_UNUSED_INIT();


#ifdef GP_HALCORTEXM4_DISABLE_JTAG_IN_DEBUG
    /* Disable JTAG in debug mode to enable GPIO8 and GPIO9 as generale purpose IO. */
    /* Disable TDI signal mapping using BSP call*/
    GP_BSP_DEBUG_SWJDP_TDI_DEINIT();
    /* Disable SWV_TDO signal mapping using BSP call */
    GP_BSP_DEBUG_SWJDP_SWV_TDO_DEINIT();
#endif // GP_HALCORTEXM4_DISABLE_JTAG_IN_DEBUG

    //Get and check HW version
    gpHal_InitVersionInfo();

    halTimer_Init();
    // Init event allocation
    gpHal_InitEs();
    hal_InitADC();

#if defined(HAL_DIVERSITY_PUF)
    hal_InitPUF();
#endif //defined(HAL_DIVERSITY_PUF)

    hal_InitSleep();


#if defined(HAL_DIVERSITY_UART)
    hal_InitUart();
#endif

    hal_EnableWatchdog(HAL_WDT_TIMEOUT);


#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
    hal_gpioInit();
#endif // HAL_DIVERSITY_GPIO_INTERRUPT

#if defined(GP_DIVERSITY_FREERTOS) && defined(GP_COMP_GPHAL_BLE)
    hal_BleTaskCreate();
#endif //GP_DIVERSITY_FREERTOS && GP_COMP_GPHAL_BLE
}

/*****************************************************************************
 *                    STACK
 *****************************************************************************/

UInt8* hal_GetStackStartAddress(void)
{
#if defined(__GNUC__)
    return (UInt8*)(&_estack);
#endif
#if defined(__IAR_SYSTEMS_ICC__)
    return (UInt8*) (__sfe( "CSTACK" ));
#endif
}

UInt8* hal_GetStackEndAddress(void)
{
#if defined(__GNUC__)
    return (UInt8*)(&_sstack);
#endif
#if defined(__IAR_SYSTEMS_ICC__)
    return (UInt8*) (__sfb( "CSTACK" ));
#endif
}



/*****************************************************************************
 *                    HEAP
 *****************************************************************************/

#ifdef GP_KX_HEAP_SIZE
#ifdef __ICCARM__
struct internal_malloc_stat_struct {
  size_t maxfp;
  size_t fp;
  size_t used;
};
void __iar_internal_malloc_stats(struct internal_malloc_stat_struct *);

void hal_GetHeapInUse(UInt32* pInUse, UInt32* pReserved, UInt32* pMax)
{
    struct internal_malloc_stat_struct imss;
    __iar_internal_malloc_stats(&imss);

    *pInUse = imss.used;
    *pReserved = imss.fp;
    *pMax   = imss.maxfp;
    GP_ASSERT_DEV_INT(GP_KX_HEAP_SIZE == imss.maxfp);
}
#elif defined(__GNUC__)
#if !defined(__SES_ARM)
#include <malloc.h>
// Linker symbol for size of heap
extern const unsigned long _lheap;

void hal_GetHeapInUse(UInt32* pInUse, UInt32* pReserved, UInt32* pMax)
{
    struct mallinfo mi;

    mi = mallinfo();


    *pInUse = mi.uordblks;
    *pReserved = mi.arena;
    *pMax   = (UInt32)(UIntPtr)&_lheap;
}
#else
void hal_GetHeapInUse(UInt32* pInUse, UInt32* pReserved, UInt32* pMax)
{
    if (pInUse)    *pInUse = 0;
    if (pReserved) *pReserved = 0;
    if (pMax)      *pMax = 0;
}
#endif
#else
#error No known heap implementation for other compiler families then IAR/GCC
#endif
#else
void hal_GetHeapInUse(UInt32* pInUse, UInt32* pReserved, UInt32* pMax)
{
    //No Heap
    *pInUse = 0x0;
    *pReserved = 0x0;
    *pMax   = 0x0;
}
#endif //GP_KX_HEAP_SIZE

/*****************************************************************************
 *                    DEBUG
 *****************************************************************************/


void hal_EnableDebugMode(void)
{
#ifdef GP_BSP_DEBUG_SWJDP_AVAILABLE
    // Init debug pins
#ifdef GP_BSP_DEBUG_SWJDP_SWDIO_TMS_INIT
    GP_BSP_DEBUG_SWJDP_SWDIO_TMS_INIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_SWCLK_TCK_INIT
    GP_BSP_DEBUG_SWJDP_SWCLK_TCK_INIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_SWV_TDO_INIT
    GP_BSP_DEBUG_SWJDP_SWV_TDO_INIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_TDI
    GP_BSP_DEBUG_SWJDP_TDI();
#endif
#endif //GP_BSP_DEBUG_SWJDP_AVAILABLE

}

void hal_DisableDebugMode(void)
{
#ifdef GP_BSP_DEBUG_SWJDP_AVAILABLE
    // deinit debug pins
#ifdef GP_BSP_DEBUG_SWJDP_SWDIO_TMS_DEINIT
    GP_BSP_DEBUG_SWJDP_SWDIO_TMS_DEINIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_SWCLK_TCK_DEINIT
    GP_BSP_DEBUG_SWJDP_SWCLK_TCK_DEINIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_SWV_TDO_DEINIT
    GP_BSP_DEBUG_SWJDP_SWV_TDO_DEINIT();
#endif
#ifdef GP_BSP_DEBUG_SWJDP_TDI_DEINIT
    GP_BSP_DEBUG_SWJDP_TDI_DEINIT();
#endif
#endif //GP_BSP_DEBUG_SWJDP_AVAILABLE
}

/*****************************************************************************
 *                    SysTick
 *****************************************************************************/
#ifndef GP_DIVERSITY_FREERTOS
Bool sysTickInitialized = false;
void hal_EnableSysTick(UInt32 ticks)
{
    GP_ASSERT_SYSTEM(sysTickInitialized == false);
    /* Disable SysTick */
    SysTick->CTRL = 0;
    /* Set reload value */
    SysTick->LOAD = ticks-1;
    /* Enable SysTick interrupts and set to lower priority */
    NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    NVIC_EnableIRQ(SysTick_IRQn);
    /* Reset the SysTick counter */
    SysTick->VAL = 0;
    /* select processor clock */
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    /* Enable SysTick interrupt */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    /* Enable SysTick */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    sysTickInitialized = true;
}
#endif
/*****************************************************************************
 *                    Watchdog
 *****************************************************************************/

void hal_EnableWatchdog(UInt16 timeout) /*timeout in 16us*/
{
    if (HAL_WDT_ENABLED())
    {
        HAL_DISABLE_GLOBAL_INT();
        // Make sure previous write to WATCHDOG_KEY is processed, this takes max 16us
        hal_Waitus(16);
        GP_WB_WRITE_WATCHDOG_KEY(0x55);
        // Change window is open - changes can be made
        GP_WB_WRITE_WATCHDOG_TIMEOUT(timeout);
        GP_WB_WRITE_WATCHDOG_WATCHDOG_FUNCTION(GP_WB_ENUM_WATCHDOG_FUNCTION_SOFT_POR);
        GP_WB_WRITE_WATCHDOG_FREEZE_WHEN_UC_ASLEEP(true);
        GP_WB_WRITE_WATCHDOG_FREEZE_WHEN_UC_BREAK_POINT(true);
        GP_WB_WRITE_WATCHDOG_ENABLE(1);
        HAL_ENABLE_GLOBAL_INT();

        // Wait for end of access window.
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING(), 1000000UL);

        // Reset the watchdog
        GP_WB_WRITE_WATCHDOG_KEY(0xAA);
        // DISCLAIMER WATCHDOG_KEY requires max 16us to be committed
    }
}

void hal_DisableWatchdog(void)
{
    if (HAL_WDT_ENABLED())
    {
        // Make sure previous write to WATCHDOG_KEY is processed, this takes max 16us
        hal_Waitus(16);
        GP_WB_WRITE_WATCHDOG_KEY(0x55); //Open change window
        GP_WB_WRITE_WATCHDOG_ENABLE(0);
        // Wait for end of access window.
        // Make sure not to close it early by writing to the key
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING(), 1000000UL);
    }
}

void hal_ResetWatchdog(void)
{
    if (HAL_WDT_ENABLED())
    {
        if (GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING() == false)
        {
            // Reset the watchdog
            GP_WB_WRITE_WATCHDOG_KEY(0xAA);
            // DISCLAIMER WATCHDOG_KEY requires max 16us to be committed
        }
    }
}

void hal_TriggerWatchdog(void)
{
    if(GP_WB_READ_WATCHDOG_ENABLE())
    {
        while(1)
        {
            // Let the watchdog time out
        }
    }
}

void hal_EnableWatchdogInterrupt(UInt16 timeout) /*timeout in 16us*/
{
    GP_WB_WRITE_INT_CTRL_MASK_WATCHDOG_TIMER_EXPIRED_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_INT_WATCHDOG_INTERRUPT(1);

    NVIC_ClearPendingIRQ(WATCHDOG_IRQn);
    NVIC_EnableIRQ(WATCHDOG_IRQn);

    HAL_DISABLE_GLOBAL_INT();
    // Make sure previous write to WATCHDOG_KEY is processed, this takes max 16us
    hal_Waitus(16);
    GP_WB_WRITE_WATCHDOG_KEY(0x55);     // enable watchdog access
    GP_WB_WRITE_WATCHDOG_TIMEOUT(timeout);
    GP_WB_WRITE_WATCHDOG_WATCHDOG_FUNCTION(GP_WB_ENUM_WATCHDOG_FUNCTION_GEN_INTERRUPT);
    GP_WB_WRITE_WATCHDOG_ENABLE(1);
    HAL_ENABLE_GLOBAL_INT();

    // Wait for end of access window.
    while (GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING());

    // Reset the watchdog
    GP_WB_WRITE_WATCHDOG_KEY(0xAA);
    // DISCLAIMER WATCHDOG_KEY requires max 16us to be committed
}

UInt16 hal_GetRomVersion(void)
{
     return ROM_flash_info()->numSectors;
}

/*****************************************************************************
 *                    Reset reason
 *****************************************************************************/

hal_ResetReason_t hal_GetResetReason(void)
{
    UInt8 reason = GP_WB_READ_PMUD_POR_REASON();

    switch (reason)
    {
        case GP_WB_ENUM_POR_REASON_HW_POR:
            return hal_ResetReason_HWPor;
        case GP_WB_ENUM_POR_REASON_SOFT_POR_BY_REGMAP:
            return hal_ResetReason_SWPor;
        case GP_WB_ENUM_POR_REASON_SOFT_POR_BY_WATCHDOG:
            return hal_ResetReason_Watchdog;
        case GP_WB_ENUM_POR_REASON_POR_BY_VDDB_CUTOFF:
            return hal_ResetReason_BrownOut;
        default:
            //Not specified separately:
            // GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ISO_TX
            // GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ES
            // GP_WB_ENUM_POR_REASON_SOFT_POR_BY_BBPLL
            // GP_WB_ENUM_POR_REASON_POR_BY_WATCHDOG_HARTBEAT
            // GP_WB_ENUM_POR_REASON_POR_BY_VDDDIG_NOK
            // GP_WB_ENUM_POR_REASON_POR_BY_GLOBAL_LDO_NOK
            // GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_MW_INVALID
            // GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_CRC_INVALID
            // GP_WB_ENUM_POR_REASON_SOFT_POR_FLASH_BL_CRC_INVALID
            // GP_WB_ENUM_POR_REASON_SOFT_POR_BOOTLOADER
            // GP_WB_ENUM_POR_REASON_SOFT_POR_PRESERVE_DBG_ITF
            // GP_WB_ENUM_POR_REASON_SOFT_POR_BY_BOD
            return hal_ResetReason_Unspecified;
    }
}

hal_WakeupReason_t hal_GetWakeupReason(void)
{
    UInt8 reason = GP_WB_READ_PMUD_WKUP_REASON();

    switch (reason)
    {
        case GP_WB_ENUM_WKUP_REASON_POR:
            return hal_WakeupReason_Reset;
        case GP_WB_ENUM_WKUP_REASON_TIMER_EVENT:
            return hal_WakeupReason_Timer;
        case GP_WB_ENUM_WKUP_REASON_LPCOMP_EVENT:
            return hal_WakeupReason_LpComp;
        case GP_WB_ENUM_WKUP_REASON_GPIO_EVENT:
            return hal_WakeupReason_Gpio;
        default:
            //Not specified separately:
            // GP_WB_ENUM_WKUP_REASON_IMMEDIATE_EVENT:
            // GP_WB_ENUM_WKUP_REASON_WAKEUP_ON_RF_EVENT:
            return hal_WakeupReason_Unspecified;
    }
}


/*****************************************************************************
 *                    Timer
 *****************************************************************************/


/*****************************************************************************
 *                    Atomic
 *****************************************************************************/

volatile UInt8 l_n_atomic = 0;


void hal__AtomicOn (void)
{
    /* global interrupt disable */
    __disable_irq();
    if (!l_n_atomic)
    {
        __set_BASEPRI(1 << (8 - __NVIC_PRIO_BITS)); // Only the BOD has prio 1
    }
    l_n_atomic++;
    /* most interrupts are disabled now, except a few */
    __enable_irq();
}
void hal__AtomicOff (void)
{
    /* re-enable interrupts */
    /* disable interrupts just in case */
    __disable_irq();
    if (l_n_atomic > 1)
    {
        l_n_atomic--;
        __set_BASEPRI(1 << (8 - __NVIC_PRIO_BITS)); // Only the BOD has prio 1
        __enable_irq();
    }
    else
    {
        l_n_atomic = 0;
        __set_BASEPRI(0); // turn of priority-based masking
        __enable_irq();
    }
}

#ifdef GP_DIVERSITY_JUMPTABLES
Bool hal_GlobalIntEnabled(void)
{
    return (l_n_atomic == 0);
}

void hal_InitializeGlobalInt(void)
{
    HAL_FORCE_DISABLE_GLOBAL_INT();
    l_n_atomic = 1;
}

void * local_memcpy ( void * destination, const void * source, size_t num )
{
    uint32_t i;
    for(i=0;i<num;i++)
    {
        ((char*)destination)[i] = ((char*)source)[i];
    }
    return destination;
}

void * local_memset ( void * ptr, int value, size_t num )
{
    uint32_t i;
    for(i=0;i<num;i++)
    {
        ((char*)ptr)[i] = value;
    }
    return ptr;
}

void* hal_memset(void *str, int c, size_t n)
{
    return local_memset(str, c, n);
}

void* hal_memcpy(void *str1, const void *str2, size_t n)
{
    return local_memcpy(str1, str2, n);
}

int local_memcmp(const void *str1, const void *str2, size_t n)
{
    const unsigned char *p1 = str1, *p2 = str2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}

int  hal_memcmp(const void *str1, const void *str2, size_t n)
{
    return local_memcmp(str1, str2, n);
}


#endif //def GP_DIVERSITY_JUMPTABLES

/*****************************************************************************
 *                    RESET
 *****************************************************************************/

void hal_Reset(void)
{
    GP_WB_WRITE_PMUD_SOFT_POR(1);
    while (1);
}

/*****************************************************************************
 *                    Bootloader
 *****************************************************************************/

void hal_GoToBootloader(void)
{
    UInt32* addr = (UInt32*) GP_MM_RAM_CRC_START;

    *addr = 0x424f4f54;
    GP_WB_WRITE_PMUD_SOFT_POR_BOOTLOADER(1);
}

/*****************************************************************************
 *                    ISRs
 *****************************************************************************/

static void hal_NotifyRTOS(void)
{
#ifdef GP_DIVERSITY_FREERTOS
    gpSched_NotifySchedTask();
#endif
}

/*
 * hal_IntHandlerPrologue() is the first code executed after wake up, as we always wake up on a interrupt
 */
void hal_IntHandlerPrologue(void)
{
    hal_SetUnpolledInterruptBeforeSleep();

    GP_UTILS_CPUMON_IRQPROLOGUE();
}

/*
 * hal_IntHandlerEpilogue() should be called at the end of each interrupt handler
 */
void hal_IntHandlerEpilogue(void)
{
    GP_UTILS_CPUMON_IRQEPILOGUE();
}

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
void rci_handler_impl(void)
{
    UInt16 unmaskedHighPrioRciInts;

    unmaskedHighPrioRciInts = GP_WB_READ_RCI_UNMASKED_INTERRUPTS() & GPHAL_ISR_RCI_HP_ISR_UNMASKED;
    if(unmaskedHighPrioRciInts != 0)
    {
        // We have a high priority interrupt pending
        gpHal_ISR_RCIInterrupt(unmaskedHighPrioRciInts);
    }
    else
    {
        // Disable LP mask to avoid re-entrance (as this was a low priority interrupt)
        GP_WB_WRITE_INT_CTRL_MASK_RCI_INTERRUPTS(GPHAL_ISR_RCI_HP_ISR_MASK);
    }
    hal_NotifyRTOS();
}

void phy_handler_impl(void)
{
    /* Disable PHY interrupt - since it is a single store, it is atomic */
    GP_WB_WRITE_INT_CTRL_MASK_INT_PHY_INTERRUPT(0);
    hal_NotifyRTOS();
}
#endif //defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
void es_handler_impl(void)
{
    /* Disable ES interrupt - since it is a single store, it is atomic */
    GP_WB_WRITE_INT_CTRL_MASK_INT_ES_INTERRUPT(0);
    hal_NotifyRTOS();
}
#endif

void ipcgpm2x_handler_impl(void)
{
#ifdef GP_COMP_GPHAL_BLE
    {
        UInt64 unmaskedHighPrioIpcGpm2xInts;

        unmaskedHighPrioIpcGpm2xInts = GPHAL_ISR_IPC_GPM_HP_ISR_UNMASKED & GP_WB_READ_IPC_UNMASKED_GPM2X_INTERRUPTS();
        if(unmaskedHighPrioIpcGpm2xInts)
        {
            // We have a high priority interrupt pending
            gpHal_ISR_IPCGPMInterrupt(unmaskedHighPrioIpcGpm2xInts);
        }
        else
        {
            // Only keep HP IPC interrupts enabled
            GP_WB_WRITE_INT_CTRL_MASK_IPCGPM2X_INTERRUPTS(GPHAL_ISR_IPC_GPM_HP_ISR_MASK);
        }
    }
#else
    GP_WB_WRITE_INT_CTRL_MASK_INT_IPCGPM2X_INTERRUPT(0x0);
#endif //GP_COMP_GPHAL_BLE
    hal_NotifyRTOS();
}

/* not polled, called by isr (halCortexM4) */
void stbc_handler_impl(void)
{
    if(GP_WB_READ_INT_CTRL_MASKED_STBC_VLT_STATUS_INTERRUPT())
    {
        /* We're assuming that the VLT_STATUS interrupt is masked */
        /* If it isn't, code needs to be added to handle the interrupt */
        GP_ASSERT_DEV_EXT(false);
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

#ifndef GP_DIVERSITY_FREERTOS
void systick_handler_impl(void)
{
    hal_SysTickInterruptPending = true;
}
#endif //GP_DIVERSITY_FREERTOS

Bool hal_HandleRadioInterrupt(Bool execute)
{
    Bool pending;

    HAL_DISABLE_GLOBAL_INT();

    /* check if a (radio related) interrupt occured */
    pending = (
#if defined(GP_COMP_GPHAL_BLE)
               (GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_READ_IPC_UNMASKED_GPM2X_INTERRUPTS()) ||
#endif //GP_COMP_GPHAL_BLE
#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
               (GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_READ_RCI_UNMASKED_INTERRUPTS()) ||
#endif //GP_COMP_GPHAL_MAC || GP_COMP_GPHAL_BLE
               GP_WB_READ_INT_CTRL_UNMASKED_ES_INTERRUPT()  ||
               GP_WB_READ_INT_CTRL_UNMASKED_STBC_INTERRUPT() ||
               GP_WB_READ_INT_CTRL_UNMASKED_PHY_INTERRUPT()
#ifndef GP_DIVERSITY_FREERTOS
               || hal_SysTickInterruptPending
#endif
    );

    HAL_ENABLE_GLOBAL_INT();

    if (pending && execute)
    {
        gpHal_Interrupt();

        HAL_DISABLE_GLOBAL_INT();

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
        // Re-enable LP RCI interrupts
        GP_WB_WRITE_INT_CTRL_MASK_RCI_INTERRUPTS(GPHAL_ISR_RCI_HP_ISR_MASK | GPHAL_ISR_RCI_LP_ISR_MASK);
#else
        GP_WB_WRITE_INT_CTRL_MASK_INT_RCI_INTERRUPT(1);
#endif // #if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
#ifdef GP_COMP_GPHAL_MAC
        // enable PHY interrupt
        GP_WB_WRITE_INT_CTRL_MASK_INT_PHY_INTERRUPT(1);
#endif // GP_COMP_GPHAL_MAC

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
        GP_WB_WRITE_INT_CTRL_MASK_INT_ES_INTERRUPT(1);
#endif
#ifdef GP_COMP_GPHAL_BLE
        // Re-enable LP IPC interrupts
        GP_WB_WRITE_INT_CTRL_MASK_IPCGPM2X_INTERRUPTS(GPHAL_ISR_IPC_GPM_HP_ISR_MASK | GPHAL_ISR_IPC_GPM_LP_ISR_MASK);
#else
        GP_WB_WRITE_INT_CTRL_MASK_INT_IPCGPM2X_INTERRUPT(1);
#endif // GP_COMP_GPHAL_BLE

        GP_WB_WRITE_INT_CTRL_MASK_INT_STBC_INTERRUPT(1);

        // All polled interrupts are handled, clear flag to avoid extra wakeup interation
        hal_ClrUnpolledInterruptBeforeSleep();

        HAL_ENABLE_GLOBAL_INT();
    }
    // Handle periodic calibrations
#ifndef GP_DIVERSITY_FREERTOS
    if (execute && (hal_SysTickInterruptPending || gpHal_CalibrationGetFirstAfterWakeup()))
    {
        gpHal_CalibrationHandleTasks();
        hal_SysTickInterruptPending = false;
    }
#endif
    return pending;
}
