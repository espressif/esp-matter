/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 *
 * Interrupt vector table for Cortex M4
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

#include "hal.h"
#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

 /*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/
#if defined(__GNUC__)
#if !defined(__SEGGER_LINKER)
#define SYMBOL_STACK_END        _estack
#else
#define SYMBOL_STACK_END        __stack_end__
#endif /* __SEGGER_LINKER */
#define IVT_GET_STACK_POINTER() (&(SYMBOL_STACK_END ))
#elif defined(__IAR_SYSTEMS_ICC__)
#define IVT_GET_STACK_POINTER() (__sfe( "CSTACK" ))
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// IVT entries can be stack pointer or function pointer ==> extra mapping needed
typedef void( *intfunc )( void );
#if defined(__GNUC__)
typedef union { intfunc __fun; const void * __ptr; } intvec_elem;
#endif
#if defined(__IAR_SYSTEMS_ICC__)
typedef union { intfunc __fun; void * __ptr; } intvec_elem;
#endif

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
void unexpected_int(void);
void unexpected_fault_validsp(void);


extern WEAK INTERRUPT_H void reset_handler_bl(void);

void hal_IntHandlerPrologue(void)
{
}

void hal_IntHandlerEpilogue(void)
{
}
/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/
// Declaration of ISR handlers
#include "dev_alias.h"
#include "default_handlers.h"
#include "hal_defs.h"

#if defined(__IAR_SYSTEMS_ICC__)
#define CALL_GRAPH_ROOT _Pragma ("call_graph_root=\"interrupt_handlers\"")
#else
#define CALL_GRAPH_ROOT
#endif

#define HAL_DEFINE_INTERRUPT_WRAPPER(rawfunc, implfunc)             \
CALL_GRAPH_ROOT                                                     \
INTERRUPT_H void rawfunc (void)                                     \
{                                                                   \
    hal_IntHandlerPrologue();                                       \
    implfunc ();                                                    \
    hal_IntHandlerEpilogue();                                       \
}
HAL_DEFINE_INTERRUPT_WRAPPER(pendsv_handler,    pendsv_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(systick_handler,   systick_handler_impl)
HAL_DEFINE_INTERRUPT_WRAPPER(stbc_handler,      stbc_handler_impl)
#if defined(GP_DIVERSITY_LOG) &&  defined(GP_DIVERSITY_GPHAL_K8E)
HAL_DEFINE_INTERRUPT_WRAPPER(dmas_handler,      dmas_handler_impl)
#endif


/*****************************************************************************
 *                    Interrupt Vector Table
 *****************************************************************************/
#if defined(__GNUC__)
extern unsigned long SYMBOL_STACK_END;
#elif defined(__IAR_SYSTEMS_ICC__)
// segment declaration needed in case of IAR
#pragma language=extended
#pragma segment="CSTACK"
#endif

#if defined(__GNUC__)
__attribute__ ((section(".isr_vector")))
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".intvec"
#endif
const intvec_elem __vector_table[] = {
    /* Start of stack (groes downwards, so highest address = address where stack starts) */
    { .__ptr = IVT_GET_STACK_POINTER()},
    /* CM4 core handlers */
    { .__fun = reset_handler_bl},       /* Reset */
    { .__fun = backup_handler},         /* NMI */
    { .__fun = hardfault_handler},      /* HardFault */
    { .__fun = memmanage_handler},      /* MemManage */
    { .__fun = busfault_handler},       /* BusFault */
    { .__fun = usagefault_handler},     /* UsageFault */
    { .__ptr = NULL },                  /* reserved */
    { .__ptr = NULL },                  /* reserved */
    { .__ptr = NULL },                  /* reserved */
    { .__ptr = NULL },                  /* reserved */
    { .__fun = svcall_handler},         /* SVCall */
    { .__fun = debugmon_handler},       /* DebugMon */
    { .__ptr = NULL },                  /* reserved */
    { .__fun = pendsv_handler},         /* PendSV */
    { .__fun = systick_handler},        /* SysTick */
    /* chip-specific handlers */
    { .__ptr = 0 },                     /* adcif_handler */
    { .__ptr = 0 },                     /* asp_handler */
    { .__ptr = 0 },                     /* dma0_handler || bbpll_handler */
#if defined(GP_DIVERSITY_LOG) &&  defined(GP_DIVERSITY_GPHAL_K8E)
    { .__fun = dmas_handler_impl },
#else
    { .__ptr = 0 },                     /* dma1_handler || dmas_handler */
#endif
    { .__ptr = 0 },                     /* es_handler */
    { .__ptr = 0 },                     /* gpio_handler */
    { .__ptr = 0 },                     /* gpmicro_handler */
    { .__ptr = 0 },                     /* i2cm_handler */
    { .__ptr = 0 },                     /* i2csl_handler */
    { .__ptr = 0 },                     /* i2sm_handler */
    { .__ptr = 0 },                     /* ipcext2x_handler */
    { .__ptr = 0 },                     /* ipcgpm2x_handler */
    { .__ptr = 0 },                     /* ipcx2int_handler */
    { .__ptr = 0 },                     /* ir_handler */
    { .__ptr = 0 },                     /* keypad_handler */
    { .__ptr = 0 },                     /* mri_handler */
    { .__ptr = 0 },                     /* parble_handler */
    { .__ptr = 0 },                     /* parfcs_handler */
    { .__ptr = 0 },                     /* phy_handler */
    { .__ptr = 0 },                     /* pwm_handler */
    { .__ptr = 0 },                     /* qta_handler */
    { .__ptr = 0 },                     /* rci_handler */
    { .__ptr = 0 },                     /* rpa_handler */
    { .__ptr = 0 },                     /* secproc_handler */
    { .__ptr = 0 },                     /* spim_handler */
    { .__ptr = 0 },                     /* spisl_handler */
    { .__ptr = 0 },                     /* ssp_handler */
#if  defined(GP_DIVERSITY_GPHAL_K8E)
    { .__fun = stbc_handler},
#else
    { .__ptr = 0 },                     /* stbc_handler */
#endif
    { .__ptr = 0 },                     /* timer_handler */
    { .__ptr = 0 },                     /* trc_handler */
#if defined(GP_DIVERSITY_LOG) &&  defined(GP_DIVERSITY_GPHAL_K8E)
    { .__fun = uart0_handler_impl },
#else
    { .__ptr = 0 },                     /* uart0_handler */
#endif
    { .__ptr = 0 },                     /* uart1_handler */
#if !defined(GP_DIVERSITY_GPHAL_K8E)
    { .__ptr = 0 },                     /* uart2_handler */
    { .__ptr = 0 },                     /* usb_handler */
#endif
    { .__ptr = 0 },                     /* watchdog_handler */
};

/*****************************************************************************
 *                    Additional sections
 *****************************************************************************/

/*
 * Default alias functions for weak and undefined isr functions.
 */
extern void Bootloader_Panic(void);
void unexpected_fault_validsp(void)
{
    Bootloader_Panic();
}

void unexpected_int(void)
{
    Bootloader_Panic();
}

#if defined(__GNUC__)
// Stub functions for C++ related lib _init/_fini functions
void __attribute__((weak)) _init(void)
{

}
void __attribute__((weak)) _fini(void)
{

}
#endif //__GNUC__

#if defined(__IAR_SYSTEMS_ICC__)
extern __weak void __iar_init_core( void );
extern __weak void __low_level_init( void );
extern __weak void __iar_data_init3( void );

#ifndef GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
#pragma required=__vector_table
#endif //GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
void __iar_cstartup( void )
{
  __iar_init_core();
  __low_level_init();
  __iar_data_init3();
  /* startup done -> call main in reset_handler */
}
#endif
