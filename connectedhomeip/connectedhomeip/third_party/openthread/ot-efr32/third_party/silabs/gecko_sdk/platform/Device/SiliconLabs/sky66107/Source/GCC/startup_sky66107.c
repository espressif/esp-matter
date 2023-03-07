/**************************************************************************//**
* @file startup_sky66107.c
* @brief Startup file for GCC compilers
*        Should be used with GCC 'GNU Tools ARM Embedded'
* @version 5.8.3
******************************************************************************
* @section License
* <b>(C) Copyright 2018 Silicon Labs, www.silabs.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;
extern uint32_t __zero_table_start__;
extern uint32_t __zero_table_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

/*----------------------------------------------------------------------------
 * Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef union {
  void (*pFunc)(void);
  void *topOfStack;
} tVectorEntry;

/*----------------------------------------------------------------------------
 * External References
 *----------------------------------------------------------------------------*/
#ifndef __START
extern void  _start(void) __attribute__((noreturn));    /* Pre Main (C library entry point) */
#else
extern int  __START(void) __attribute__((noreturn));    /* main entry point */
#endif

#ifndef __NO_SYSTEM_INIT
extern void SystemInit(void);             /* CMSIS System Initialization      */
#endif

/*----------------------------------------------------------------------------
 * Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void);                          /* Default empty handler */
void Reset_Handler(void);                            /* Reset Handler */

/*----------------------------------------------------------------------------
 * User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
#ifndef __STACK_SIZE
#define __STACK_SIZE    0x00000400
#endif
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

#ifndef __HEAP_SIZE
#define __HEAP_SIZE    0x00000C00
#endif
#if __HEAP_SIZE > 0
static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif

/*----------------------------------------------------------------------------
 * Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Cortex-M Processor Exceptions */
void NMI_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__ ((weak, alias("Default_Handler")));

/* Part Specific Interrupts */
void TIM1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MGMT_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void BB_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SLEEPTMR_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SC1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SC2_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void AESCCM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MACTMR_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MACTX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MACRX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IRQA_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IRQB_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IRQC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IRQD_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void DEBUG_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SC3_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SC4_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USB_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
 * Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
#if defined (__ICCARM__)
#pragma data_alignment=256
#endif

extern const tVectorEntry __Vectors[];
const tVectorEntry       __Vectors[] __attribute__ ((section(".vectors"))) = {
  /* Cortex-M Exception Handlers */
  { .topOfStack = &__StackTop },              /*      Initial Stack Pointer */
  { Reset_Handler },                          /*      Reset Handler         */
  { NMI_Handler },                            /*      NMI Handler           */
  { HardFault_Handler },                      /*      Hard Fault Handler    */
  { MemManage_Handler },                      /*      MPU Fault Handler     */
  { BusFault_Handler },                       /*      Bus Fault Handler     */
  { UsageFault_Handler },                     /*      Usage Fault Handler   */
  { Default_Handler },                        /*      Reserved              */
  { Default_Handler },                        /*      Reserved              */
  { Default_Handler },                        /*      Reserved              */
  { Default_Handler },                        /*      Reserved              */
  { SVC_Handler },                            /*      SVCall Handler        */
  { DebugMon_Handler },                       /*      Debug Monitor Handler */
  { Default_Handler },                        /*      Reserved              */
  { PendSV_Handler },                         /*      PendSV Handler        */
  { SysTick_Handler },                        /*      SysTick Handler       */

  /* External interrupts */
  { TIM1_IRQHandler },                       /*0  - TIM1 */
  { TIM2_IRQHandler },                       /*1  - TIM2 */
  { MGMT_IRQHandler },                       /*2  - MGMT */
  { BB_IRQHandler },                         /*3  - BB */
  { SLEEPTMR_IRQHandler },                   /*4  - SLEEPTMR */
  { SC1_IRQHandler },                        /*5  - SC1 */
  { SC2_IRQHandler },                        /*6  - SC2 */
  { AESCCM_IRQHandler },                     /*7  - AESCCM */
  { MACTMR_IRQHandler },                     /*8  - MACTMR */
  { MACTX_IRQHandler },                      /*9  - MACTX */
  { MACRX_IRQHandler },                      /*10 - MACRX */
  { ADC_IRQHandler },                        /*11 - ADC */
  { IRQA_IRQHandler },                       /*12 - IRQA */
  { IRQB_IRQHandler },                       /*13 - IRQB */
  { IRQC_IRQHandler },                       /*14 - IRQC */
  { IRQD_IRQHandler },                       /*15 - IRQD */
  { DEBUG_IRQHandler },                      /*16 - DEBUG */
  { SC3_IRQHandler },                        /*17 - SC3 */
  { SC4_IRQHandler },                        /*18 - SC4 */
  { USB_IRQHandler },                        /*19 - USB */
};

//
// Start Handler called by SystemInit to start the main program running.
// Since IAR and GCC have very different semantics for this, they are
// wrapped in this function that can be called by common code without
// worrying about which compiler is being used.
//

void Start_Handler(void)
{
  uint32_t *pSrc, *pDest;
  uint32_t *pTable __attribute__((unused));

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pTable = &__copy_table_start__;

  for (; pTable < &__copy_table_end__; pTable = pTable + 3) {
    pSrc  = (uint32_t *) *(pTable + 0);
    pDest = (uint32_t *) *(pTable + 1);
    for (; pDest < (uint32_t *) (*(pTable + 1) + *(pTable + 2)); ) {
      *pDest++ = *pSrc++;
    }
  }
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pSrc  = &__etext;
  pDest = &__data_start__;

  for (; pDest < &__data_end__; ) {
    *pDest++ = *pSrc++;
  }
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
  pTable = &__zero_table_start__;

  for (; pTable < &__zero_table_end__; pTable = pTable + 2) {
    pDest = (uint32_t *) *(pTable + 0);
    for (; pDest < (uint32_t *) (*(pTable + 0) + *(pTable + 1)); ) {
      *pDest++ = 0UL;
    }
  }
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
  pDest = &__bss_start__;

  for (; pDest < &__bss_end__; ) {
    *pDest++ = 0UL;
  }
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __START
#define __START    _start
#endif
  __START();
}

/*----------------------------------------------------------------------------
 * Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
#ifndef __NO_SYSTEM_INIT
  SystemInit();
#else
  Start_Handler();
#endif
}

/*----------------------------------------------------------------------------
 * Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  // The Default_Handler is for unimplemented handlers.  Trap execution.
  while (true) {
  }
}

/*----------------------------------------------------------------------------
 * Exit function, in case main ever accidentally returns.
 *
 * A GCC compilation that uses more than "nosys.specs" needs an exit() function.
 * Ideally once START passes control to main, the code should never return.
 *----------------------------------------------------------------------------*/
void exit(int status)
{
  // Trap execution in case main accidentally returns.
  while (true) {
  }
}
