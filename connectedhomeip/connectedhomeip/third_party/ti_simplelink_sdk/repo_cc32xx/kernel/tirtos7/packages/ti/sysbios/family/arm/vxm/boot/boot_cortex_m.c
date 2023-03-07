/*****************************************************************************/
/* BOOT_CORTEX_M.C   v##### - Initialize the ARM C runtime environment       */
/* Copyright (c) 2017@%%%%  Texas Instruments Incorporated                   */
/*****************************************************************************/
#include <stdint.h>

#ifdef __TI_RTS_BUILD
/*---------------------------------------------------------------------------*/
/* __TI_default_c_int00 indicates that the default TI entry routine is being  */
/* used.  The linker makes assumptions about what exit does when this symbol */
/* is seen. This symbols should NOT be defined if a customized exit routine  */
/* is used.                                                                  */
/*---------------------------------------------------------------------------*/
__asm("__TI_default_c_int00 .set 1");
#endif

/*----------------------------------------------------------------------------*/
/* Define the user mode stack. The size will be determined by the linker.     */
/*----------------------------------------------------------------------------*/
__attribute__((section(".stack")))
int __stack;

/*----------------------------------------------------------------------------*/
/* Linker defined symbol that will point to the end of the user mode stack.   */
/* The linker will enforce 8-byte alignment.                                  */
/*----------------------------------------------------------------------------*/
extern int __STACK_END;

/*----------------------------------------------------------------------------*/
/* Function declarations.                                                     */
/*----------------------------------------------------------------------------*/
__attribute__((weak)) extern void __mpu_init(void);
extern int _system_pre_init(void);
extern void __TI_auto_init(void);
extern void _args_main(void);
extern int main();

__attribute__((noreturn))
extern void ti_sysbios_runtime_System_exit(int);

/*----------------------------------------------------------------------------*/
/* Default boot routine for Cortex-M                                          */
/*----------------------------------------------------------------------------*/
__attribute__((section(".text:_c_int00"), used, noreturn))
void _c_int00(void)
{
   // Initialize the stack pointer
   register char* stack_ptr = (char*)&__STACK_END - 8;
   __asm volatile ("MSR msp, %0" : : "r" (stack_ptr) : );

   // Initialize the FPU if building for floating point
   #ifdef __ARM_FP
   volatile uint32_t* cpacr = (volatile uint32_t*)0xE000ED88;
   *cpacr |= (0xf0 << 16);
   #endif

   __mpu_init();
   if (_system_pre_init())
   {
       __TI_auto_init();
   }

   _args_main();
   ti_sysbios_runtime_System_exit(0);
}
