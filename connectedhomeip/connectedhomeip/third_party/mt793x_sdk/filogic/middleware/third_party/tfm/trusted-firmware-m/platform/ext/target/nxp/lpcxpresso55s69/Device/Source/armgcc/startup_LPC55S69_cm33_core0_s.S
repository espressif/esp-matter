;/*
; * Copyright (c) 2009-2018 ARM Limited
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 startup_ARMCM33.S
; */

    .syntax    unified
    .arch    armv8-m.main

    .section .vectors
    .align 2
    .globl    __Vectors
__Vectors:
    .long   Image$$ARM_LIB_STACK_MSP$$ZI$$Limit             /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   SecureFault_Handler                             /* Secure Fault Handler */
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

    /* External Interrupts */
    .long   WDT_BOD_IRQHandler  /* Windowed watchdog timer, Brownout detect, Flash interrupt */
    .long   DMA0_IRQHandler  /* DMA0 controller */
    .long   GINT0_IRQHandler  /* GPIO group 0 */
    .long   GINT1_IRQHandler  /* GPIO group 1 */
    .long   PIN_INT0_IRQHandler  /* Pin interrupt 0 or pattern match engine slice 0 */
    .long   PIN_INT1_IRQHandler  /* Pin interrupt 1or pattern match engine slice 1 */
    .long   PIN_INT2_IRQHandler  /* Pin interrupt 2 or pattern match engine slice 2 */
    .long   PIN_INT3_IRQHandler  /* Pin interrupt 3 or pattern match engine slice 3 */
    .long   UTICK0_IRQHandler  /* Micro-tick Timer */
    .long   MRT0_IRQHandler  /* Multi-rate timer */
    .long   CTIMER0_IRQHandler  /* Standard counter/timer CTIMER0 */
    .long   CTIMER1_IRQHandler  /* Standard counter/timer CTIMER1 */
    .long   SCT0_IRQHandler  /* SCTimer/PWM */
    .long   CTIMER3_IRQHandler  /* Standard counter/timer CTIMER3 */
    .long   FLEXCOMM0_IRQHandler  /* Flexcomm Interface 0 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM1_IRQHandler  /* Flexcomm Interface 1 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM2_IRQHandler  /* Flexcomm Interface 2 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM3_IRQHandler  /* Flexcomm Interface 3 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM4_IRQHandler  /* Flexcomm Interface 4 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM5_IRQHandler  /* Flexcomm Interface 5 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM6_IRQHandler  /* Flexcomm Interface 6 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   FLEXCOMM7_IRQHandler  /* Flexcomm Interface 7 (USART, SPI, I2C, I2S, FLEXCOMM) */
    .long   ADC0_IRQHandler  /* ADC0  */
    .long   Reserved39_IRQHandler  /* Reserved interrupt */
    .long   ACMP_IRQHandler  /* ACMP  interrupts */
    .long   Reserved41_IRQHandler  /* Reserved interrupt */
    .long   Reserved42_IRQHandler  /* Reserved interrupt */
    .long   USB0_NEEDCLK_IRQHandler  /* USB Activity Wake-up Interrupt */
    .long   USB0_IRQHandler  /* USB device */
    .long   RTC_IRQHandler  /* RTC alarm and wake-up interrupts */
    .long   Reserved46_IRQHandler  /* Reserved interrupt */
    .long   MAILBOX_IRQHandler  /* WAKEUP,Mailbox interrupt (present on selected devices) */
    .long   PIN_INT4_IRQHandler  /* Pin interrupt 4 or pattern match engine slice 4 int */
    .long   PIN_INT5_IRQHandler  /* Pin interrupt 5 or pattern match engine slice 5 int */
    .long   PIN_INT6_IRQHandler  /* Pin interrupt 6 or pattern match engine slice 6 int */
    .long   PIN_INT7_IRQHandler  /* Pin interrupt 7 or pattern match engine slice 7 int */
    .long   CTIMER2_IRQHandler  /* Standard counter/timer CTIMER2 */
    .long   CTIMER4_IRQHandler  /* Standard counter/timer CTIMER4 */
    .long   OS_EVENT_IRQHandler  /* OSEVTIMER0 and OSEVTIMER0_WAKEUP interrupts */
    .long   Reserved55_IRQHandler  /* Reserved interrupt */
    .long   Reserved56_IRQHandler  /* Reserved interrupt */
    .long   Reserved57_IRQHandler  /* Reserved interrupt */
    .long   SDIO_IRQHandler  /* SD/MMC  */
    .long   Reserved59_IRQHandler  /* Reserved interrupt */
    .long   Reserved60_IRQHandler  /* Reserved interrupt */
    .long   Reserved61_IRQHandler  /* Reserved interrupt */
    .long   USB1_UTMI_IRQHandler  /* USB1_UTMI */
    .long   USB1_IRQHandler  /* USB1 interrupt */
    .long   USB1_NEEDCLK_IRQHandler  /* USB1 activity */
    .long   SEC_HYPERVISOR_CALL_IRQHandler  /* SEC_HYPERVISOR_CALL interrupt */
    .long   SEC_GPIO_INT0_IRQ0_IRQHandler  /* SEC_GPIO_INT0_IRQ0 interrupt */
    .long   SEC_GPIO_INT0_IRQ1_IRQHandler  /* SEC_GPIO_INT0_IRQ1 interrupt */
    .long   PLU_IRQHandler  /* PLU interrupt */
    .long   SEC_VIO_IRQHandler  /* SEC_VIO interrupt */
    .long   HASHCRYPT_IRQHandler  /* HASHCRYPT interrupt */
    .long   CASER_IRQHandler  /* CASPER interrupt */
    .long   PUF_IRQHandler  /* PUF interrupt */
    .long   PQ_IRQHandler  /* PQ interrupt */
    .long   DMA1_IRQHandler  /* DMA1 interrupt */
    .long   LSPI_HS_IRQHandler  /* Flexcomm Interface 8 (SPI, , FLEXCOMM) */

    .size    __Vectors, . - __Vectors

    .text
    .thumb
    .thumb_func
    .align    2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
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
    ldr    r4, =__copy_table_start__
    ldr    r5, =__copy_table_end__

.L_loop0:
    cmp    r4, r5
    bge    .L_loop0_done
    ldr    r1, [r4]
    ldr    r2, [r4, #4]
    ldr    r3, [r4, #8]

.L_loop0_0:
    subs    r3, #4
    ittt    ge
    ldrge    r0, [r1, r3]
    strge    r0, [r2, r3]
    bge    .L_loop0_0

    adds    r4, #12
    b    .L_loop0

.L_loop0_done:
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
    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

.L_loop1:
    cmp    r2, r3
    ittt    lt
    ldrlt    r0, [r1], #4
    strlt    r0, [r2], #4
    blt    .L_loop1
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
    ldr    r3, =__zero_table_start__
    ldr    r4, =__zero_table_end__

.L_loop2:
    cmp    r3, r4
    bge    .L_loop2_done
    ldr    r1, [r3]
    ldr    r2, [r3, #4]
    movs    r0, 0

.L_loop2_0:
    subs    r2, #4
    itt    ge
    strge    r0, [r1, r2]
    bge    .L_loop2_0

    adds    r3, #8
    b    .L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    ldr    r1, =__bss_start__
    ldr    r2, =__bss_end__

    movs    r0, 0
.L_loop3:
    cmp    r1, r2
    itt    lt
    strlt    r0, [r1], #4
    blt    .L_loop3
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

    cpsid   i              /* Disable IRQs */
    bl    SystemInit

    mrs     r0, control    /* Get control value */
    orr     r0, r0, #2     /* Select switch to PSP */
    msr     control, r0
    ldr     r0, =Image$$ARM_LIB_STACK$$ZI$$Limit
    msr     psp, r0

#ifndef __START
#define __START _start
#endif
    bl    __START

    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b DefaultISR
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    ldr   r0,=HardFault_Handler
    bx    r0
    .size HardFault_Handler, . - HardFault_Handler

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak WDT_BOD_IRQHandler
    .type WDT_BOD_IRQHandler, %function
WDT_BOD_IRQHandler:
    ldr   r0,=WDT_BOD_DriverIRQHandler
    bx    r0
    .size WDT_BOD_IRQHandler, . - WDT_BOD_IRQHandler

    .align 1
    .thumb_func
    .weak DMA0_IRQHandler
    .type DMA0_IRQHandler, %function
DMA0_IRQHandler:
    ldr   r0,=DMA0_DriverIRQHandler
    bx    r0
    .size DMA0_IRQHandler, . - DMA0_IRQHandler

    .align 1
    .thumb_func
    .weak GINT0_IRQHandler
    .type GINT0_IRQHandler, %function
GINT0_IRQHandler:
    ldr   r0,=GINT0_DriverIRQHandler
    bx    r0
    .size GINT0_IRQHandler, . - GINT0_IRQHandler

    .align 1
    .thumb_func
    .weak GINT1_IRQHandler
    .type GINT1_IRQHandler, %function
GINT1_IRQHandler:
    ldr   r0,=GINT1_DriverIRQHandler
    bx    r0
    .size GINT1_IRQHandler, . - GINT1_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT0_IRQHandler
    .type PIN_INT0_IRQHandler, %function
PIN_INT0_IRQHandler:
    ldr   r0,=PIN_INT0_DriverIRQHandler
    bx    r0
    .size PIN_INT0_IRQHandler, . - PIN_INT0_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT1_IRQHandler
    .type PIN_INT1_IRQHandler, %function
PIN_INT1_IRQHandler:
    ldr   r0,=PIN_INT1_DriverIRQHandler
    bx    r0
    .size PIN_INT1_IRQHandler, . - PIN_INT1_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT2_IRQHandler
    .type PIN_INT2_IRQHandler, %function
PIN_INT2_IRQHandler:
    ldr   r0,=PIN_INT2_DriverIRQHandler
    bx    r0
    .size PIN_INT2_IRQHandler, . - PIN_INT2_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT3_IRQHandler
    .type PIN_INT3_IRQHandler, %function
PIN_INT3_IRQHandler:
    ldr   r0,=PIN_INT3_DriverIRQHandler
    bx    r0
    .size PIN_INT3_IRQHandler, . - PIN_INT3_IRQHandler

    .align 1
    .thumb_func
    .weak UTICK0_IRQHandler
    .type UTICK0_IRQHandler, %function
UTICK0_IRQHandler:
    ldr   r0,=UTICK0_DriverIRQHandler
    bx    r0
    .size UTICK0_IRQHandler, . - UTICK0_IRQHandler

    .align 1
    .thumb_func
    .weak MRT0_IRQHandler
    .type MRT0_IRQHandler, %function
MRT0_IRQHandler:
    ldr   r0,=MRT0_DriverIRQHandler
    bx    r0
    .size MRT0_IRQHandler, . - MRT0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER0_IRQHandler
    .type CTIMER0_IRQHandler, %function
CTIMER0_IRQHandler:
    ldr   r0,=CTIMER0_DriverIRQHandler
    bx    r0
    .size CTIMER0_IRQHandler, . - CTIMER0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER1_IRQHandler
    .type CTIMER1_IRQHandler, %function
CTIMER1_IRQHandler:
    ldr   r0,=CTIMER1_DriverIRQHandler
    bx    r0
    .size CTIMER1_IRQHandler, . - CTIMER1_IRQHandler

    .align 1
    .thumb_func
    .weak SCT0_IRQHandler
    .type SCT0_IRQHandler, %function
SCT0_IRQHandler:
    ldr   r0,=SCT0_DriverIRQHandler
    bx    r0
    .size SCT0_IRQHandler, . - SCT0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER3_IRQHandler
    .type CTIMER3_IRQHandler, %function
CTIMER3_IRQHandler:
    ldr   r0,=CTIMER3_DriverIRQHandler
    bx    r0
    .size CTIMER3_IRQHandler, . - CTIMER3_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM0_IRQHandler
    .type FLEXCOMM0_IRQHandler, %function
FLEXCOMM0_IRQHandler:
    ldr   r0,=FLEXCOMM0_DriverIRQHandler
    bx    r0
    .size FLEXCOMM0_IRQHandler, . - FLEXCOMM0_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM1_IRQHandler
    .type FLEXCOMM1_IRQHandler, %function
FLEXCOMM1_IRQHandler:
    ldr   r0,=FLEXCOMM1_DriverIRQHandler
    bx    r0
    .size FLEXCOMM1_IRQHandler, . - FLEXCOMM1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM2_IRQHandler
    .type FLEXCOMM2_IRQHandler, %function
FLEXCOMM2_IRQHandler:
    ldr   r0,=FLEXCOMM2_DriverIRQHandler
    bx    r0
    .size FLEXCOMM2_IRQHandler, . - FLEXCOMM2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM3_IRQHandler
    .type FLEXCOMM3_IRQHandler, %function
FLEXCOMM3_IRQHandler:
    ldr   r0,=FLEXCOMM3_DriverIRQHandler
    bx    r0
    .size FLEXCOMM3_IRQHandler, . - FLEXCOMM3_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM4_IRQHandler
    .type FLEXCOMM4_IRQHandler, %function
FLEXCOMM4_IRQHandler:
    ldr   r0,=FLEXCOMM4_DriverIRQHandler
    bx    r0
    .size FLEXCOMM4_IRQHandler, . - FLEXCOMM4_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM5_IRQHandler
    .type FLEXCOMM5_IRQHandler, %function
FLEXCOMM5_IRQHandler:
    ldr   r0,=FLEXCOMM5_DriverIRQHandler
    bx    r0
    .size FLEXCOMM5_IRQHandler, . - FLEXCOMM5_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM6_IRQHandler
    .type FLEXCOMM6_IRQHandler, %function
FLEXCOMM6_IRQHandler:
    ldr   r0,=FLEXCOMM6_DriverIRQHandler
    bx    r0
    .size FLEXCOMM6_IRQHandler, . - FLEXCOMM6_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM7_IRQHandler
    .type FLEXCOMM7_IRQHandler, %function
FLEXCOMM7_IRQHandler:
    ldr   r0,=FLEXCOMM7_DriverIRQHandler
    bx    r0
    .size FLEXCOMM7_IRQHandler, . - FLEXCOMM7_IRQHandler

    .align 1
    .thumb_func
    .weak ADC0_IRQHandler
    .type ADC0_IRQHandler, %function
ADC0_IRQHandler:
    ldr   r0,=ADC0_DriverIRQHandler
    bx    r0
    .size ADC0_IRQHandler, . - ADC0_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved39_IRQHandler
    .type Reserved39_IRQHandler, %function
Reserved39_IRQHandler:
    ldr   r0,=Reserved39_DriverIRQHandler
    bx    r0
    .size Reserved39_IRQHandler, . - Reserved39_IRQHandler

    .align 1
    .thumb_func
    .weak ACMP_IRQHandler
    .type ACMP_IRQHandler, %function
ACMP_IRQHandler:
    ldr   r0,=ACMP_DriverIRQHandler
    bx    r0
    .size ACMP_IRQHandler, . - ACMP_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved41_IRQHandler
    .type Reserved41_IRQHandler, %function
Reserved41_IRQHandler:
    ldr   r0,=Reserved41_DriverIRQHandler
    bx    r0
    .size Reserved41_IRQHandler, . - Reserved41_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved42_IRQHandler
    .type Reserved42_IRQHandler, %function
Reserved42_IRQHandler:
    ldr   r0,=Reserved42_DriverIRQHandler
    bx    r0
    .size Reserved42_IRQHandler, . - Reserved42_IRQHandler

    .align 1
    .thumb_func
    .weak USB0_NEEDCLK_IRQHandler
    .type USB0_NEEDCLK_IRQHandler, %function
USB0_NEEDCLK_IRQHandler:
    ldr   r0,=USB0_NEEDCLK_DriverIRQHandler
    bx    r0
    .size USB0_NEEDCLK_IRQHandler, . - USB0_NEEDCLK_IRQHandler

    .align 1
    .thumb_func
    .weak USB0_IRQHandler
    .type USB0_IRQHandler, %function
USB0_IRQHandler:
    ldr   r0,=USB0_DriverIRQHandler
    bx    r0
    .size USB0_IRQHandler, . - USB0_IRQHandler

    .align 1
    .thumb_func
    .weak RTC_IRQHandler
    .type RTC_IRQHandler, %function
RTC_IRQHandler:
    ldr   r0,=RTC_DriverIRQHandler
    bx    r0
    .size RTC_IRQHandler, . - RTC_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved46_IRQHandler
    .type Reserved46_IRQHandler, %function
Reserved46_IRQHandler:
    ldr   r0,=Reserved46_DriverIRQHandler
    bx    r0
    .size Reserved46_IRQHandler, . - Reserved46_IRQHandler

    .align 1
    .thumb_func
    .weak MAILBOX_IRQHandler
    .type MAILBOX_IRQHandler, %function
MAILBOX_IRQHandler:
    ldr   r0,=MAILBOX_DriverIRQHandler
    bx    r0
    .size MAILBOX_IRQHandler, . - MAILBOX_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT4_IRQHandler
    .type PIN_INT4_IRQHandler, %function
PIN_INT4_IRQHandler:
    ldr   r0,=PIN_INT4_DriverIRQHandler
    bx    r0
    .size PIN_INT4_IRQHandler, . - PIN_INT4_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT5_IRQHandler
    .type PIN_INT5_IRQHandler, %function
PIN_INT5_IRQHandler:
    ldr   r0,=PIN_INT5_DriverIRQHandler
    bx    r0
    .size PIN_INT5_IRQHandler, . - PIN_INT5_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT6_IRQHandler
    .type PIN_INT6_IRQHandler, %function
PIN_INT6_IRQHandler:
    ldr   r0,=PIN_INT6_DriverIRQHandler
    bx    r0
    .size PIN_INT6_IRQHandler, . - PIN_INT6_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT7_IRQHandler
    .type PIN_INT7_IRQHandler, %function
PIN_INT7_IRQHandler:
    ldr   r0,=PIN_INT7_DriverIRQHandler
    bx    r0
    .size PIN_INT7_IRQHandler, . - PIN_INT7_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER2_IRQHandler
    .type CTIMER2_IRQHandler, %function
CTIMER2_IRQHandler:
    ldr   r0,=CTIMER2_DriverIRQHandler
    bx    r0
    .size CTIMER2_IRQHandler, . - CTIMER2_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER4_IRQHandler
    .type CTIMER4_IRQHandler, %function
CTIMER4_IRQHandler:
    ldr   r0,=CTIMER4_DriverIRQHandler
    bx    r0
    .size CTIMER4_IRQHandler, . - CTIMER4_IRQHandler

    .align 1
    .thumb_func
    .weak OS_EVENT_IRQHandler
    .type OS_EVENT_IRQHandler, %function
OS_EVENT_IRQHandler:
    ldr   r0,=OS_EVENT_DriverIRQHandler
    bx    r0
    .size OS_EVENT_IRQHandler, . - OS_EVENT_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved55_IRQHandler
    .type Reserved55_IRQHandler, %function
Reserved55_IRQHandler:
    ldr   r0,=Reserved55_DriverIRQHandler
    bx    r0
    .size Reserved55_IRQHandler, . - Reserved55_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved56_IRQHandler
    .type Reserved56_IRQHandler, %function
Reserved56_IRQHandler:
    ldr   r0,=Reserved56_DriverIRQHandler
    bx    r0
    .size Reserved56_IRQHandler, . - Reserved56_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved57_IRQHandler
    .type Reserved57_IRQHandler, %function
Reserved57_IRQHandler:
    ldr   r0,=Reserved57_DriverIRQHandler
    bx    r0
    .size Reserved57_IRQHandler, . - Reserved57_IRQHandler

    .align 1
    .thumb_func
    .weak SDIO_IRQHandler
    .type SDIO_IRQHandler, %function
SDIO_IRQHandler:
    ldr   r0,=SDIO_DriverIRQHandler
    bx    r0
    .size SDIO_IRQHandler, . - SDIO_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved59_IRQHandler
    .type Reserved59_IRQHandler, %function
Reserved59_IRQHandler:
    ldr   r0,=Reserved59_DriverIRQHandler
    bx    r0
    .size Reserved59_IRQHandler, . - Reserved59_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved60_IRQHandler
    .type Reserved60_IRQHandler, %function
Reserved60_IRQHandler:
    ldr   r0,=Reserved60_DriverIRQHandler
    bx    r0
    .size Reserved60_IRQHandler, . - Reserved60_IRQHandler

    .align 1
    .thumb_func
    .weak Reserved61_IRQHandler
    .type Reserved61_IRQHandler, %function
Reserved61_IRQHandler:
    ldr   r0,=Reserved61_DriverIRQHandler
    bx    r0
    .size Reserved61_IRQHandler, . - Reserved61_IRQHandler

    .align 1
    .thumb_func
    .weak USB1_UTMI_IRQHandler
    .type USB1_UTMI_IRQHandler, %function
USB1_UTMI_IRQHandler:
    ldr   r0,=USB1_UTMI_DriverIRQHandler
    bx    r0
    .size USB1_UTMI_IRQHandler, . - USB1_UTMI_IRQHandler

    .align 1
    .thumb_func
    .weak USB1_IRQHandler
    .type USB1_IRQHandler, %function
USB1_IRQHandler:
    ldr   r0,=USB1_DriverIRQHandler
    bx    r0
    .size USB1_IRQHandler, . - USB1_IRQHandler

    .align 1
    .thumb_func
    .weak USB1_NEEDCLK_IRQHandler
    .type USB1_NEEDCLK_IRQHandler, %function
USB1_NEEDCLK_IRQHandler:
    ldr   r0,=USB1_NEEDCLK_DriverIRQHandler
    bx    r0
    .size USB1_NEEDCLK_IRQHandler, . - USB1_NEEDCLK_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_HYPERVISOR_CALL_IRQHandler
    .type SEC_HYPERVISOR_CALL_IRQHandler, %function
SEC_HYPERVISOR_CALL_IRQHandler:
    ldr   r0,=SEC_HYPERVISOR_CALL_DriverIRQHandler
    bx    r0
    .size SEC_HYPERVISOR_CALL_IRQHandler, . - SEC_HYPERVISOR_CALL_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_GPIO_INT0_IRQ0_IRQHandler
    .type SEC_GPIO_INT0_IRQ0_IRQHandler, %function
SEC_GPIO_INT0_IRQ0_IRQHandler:
    ldr   r0,=SEC_GPIO_INT0_IRQ0_DriverIRQHandler
    bx    r0
    .size SEC_GPIO_INT0_IRQ0_IRQHandler, . - SEC_GPIO_INT0_IRQ0_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_GPIO_INT0_IRQ1_IRQHandler
    .type SEC_GPIO_INT0_IRQ1_IRQHandler, %function
SEC_GPIO_INT0_IRQ1_IRQHandler:
    ldr   r0,=SEC_GPIO_INT0_IRQ1_DriverIRQHandler
    bx    r0
    .size SEC_GPIO_INT0_IRQ1_IRQHandler, . - SEC_GPIO_INT0_IRQ1_IRQHandler

    .align 1
    .thumb_func
    .weak PLU_IRQHandler
    .type PLU_IRQHandler, %function
PLU_IRQHandler:
    ldr   r0,=PLU_DriverIRQHandler
    bx    r0
    .size PLU_IRQHandler, . - PLU_IRQHandler

    .align 1
    .thumb_func
    .weak SEC_VIO_IRQHandler
    .type SEC_VIO_IRQHandler, %function
SEC_VIO_IRQHandler:
    ldr   r0,=SEC_VIO_DriverIRQHandler
    bx    r0
    .size SEC_VIO_IRQHandler, . - SEC_VIO_IRQHandler

    .align 1
    .thumb_func
    .weak HASHCRYPT_IRQHandler
    .type HASHCRYPT_IRQHandler, %function
HASHCRYPT_IRQHandler:
    ldr   r0,=HASHCRYPT_DriverIRQHandler
    bx    r0
    .size HASHCRYPT_IRQHandler, . - HASHCRYPT_IRQHandler

    .align 1
    .thumb_func
    .weak CASER_IRQHandler
    .type CASER_IRQHandler, %function
CASER_IRQHandler:
    ldr   r0,=CASER_DriverIRQHandler
    bx    r0
    .size CASER_IRQHandler, . - CASER_IRQHandler

    .align 1
    .thumb_func
    .weak PUF_IRQHandler
    .type PUF_IRQHandler, %function
PUF_IRQHandler:
    ldr   r0,=PUF_DriverIRQHandler
    bx    r0
    .size PUF_IRQHandler, . - PUF_IRQHandler

    .align 1
    .thumb_func
    .weak PQ_IRQHandler
    .type PQ_IRQHandler, %function
PQ_IRQHandler:
    ldr   r0,=PQ_DriverIRQHandler
    bx    r0
    .size PQ_IRQHandler, . - PQ_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_IRQHandler
    .type DMA1_IRQHandler, %function
DMA1_IRQHandler:
    ldr   r0,=DMA1_DriverIRQHandler
    bx    r0
    .size DMA1_IRQHandler, . - DMA1_IRQHandler

    .align 1
    .thumb_func
    .weak LSPI_HS_IRQHandler
    .type LSPI_HS_IRQHandler, %function
LSPI_HS_IRQHandler:
    ldr   r0,=LSPI_HS_DriverIRQHandler
    bx    r0
    .size LSPI_HS_IRQHandler, . - LSPI_HS_IRQHandler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm
/* Exception Handlers */
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    SecureFault_Handler
    def_irq_handler    DebugMon_Handler
                def_irq_handler     WDT_BOD_DriverIRQHandler
                def_irq_handler     DMA0_DriverIRQHandler
                def_irq_handler     GINT0_DriverIRQHandler
                def_irq_handler     GINT1_DriverIRQHandler
                def_irq_handler     PIN_INT0_DriverIRQHandler
                def_irq_handler     PIN_INT1_DriverIRQHandler
                def_irq_handler     PIN_INT2_DriverIRQHandler
                def_irq_handler     PIN_INT3_DriverIRQHandler
                def_irq_handler     UTICK0_DriverIRQHandler
                def_irq_handler     MRT0_DriverIRQHandler
                def_irq_handler     CTIMER0_DriverIRQHandler
                def_irq_handler     CTIMER1_DriverIRQHandler
                def_irq_handler     SCT0_DriverIRQHandler
                def_irq_handler     CTIMER3_DriverIRQHandler
                def_irq_handler     FLEXCOMM0_DriverIRQHandler
                def_irq_handler     FLEXCOMM1_DriverIRQHandler
                def_irq_handler     FLEXCOMM2_DriverIRQHandler
                def_irq_handler     FLEXCOMM3_DriverIRQHandler
                def_irq_handler     FLEXCOMM4_DriverIRQHandler
                def_irq_handler     FLEXCOMM5_DriverIRQHandler
                def_irq_handler     FLEXCOMM6_DriverIRQHandler
                def_irq_handler     FLEXCOMM7_DriverIRQHandler
                def_irq_handler     ADC0_DriverIRQHandler
                def_irq_handler     Reserved39_DriverIRQHandler
                def_irq_handler     ACMP_DriverIRQHandler
                def_irq_handler     Reserved41_DriverIRQHandler
                def_irq_handler     Reserved42_DriverIRQHandler
                def_irq_handler     USB0_NEEDCLK_DriverIRQHandler
                def_irq_handler     USB0_DriverIRQHandler
                def_irq_handler     RTC_DriverIRQHandler
                def_irq_handler     Reserved46_DriverIRQHandler
                def_irq_handler     MAILBOX_DriverIRQHandler
                def_irq_handler     PIN_INT4_DriverIRQHandler
                def_irq_handler     PIN_INT5_DriverIRQHandler
                def_irq_handler     PIN_INT6_DriverIRQHandler
                def_irq_handler     PIN_INT7_DriverIRQHandler
                def_irq_handler     CTIMER2_DriverIRQHandler
                def_irq_handler     CTIMER4_DriverIRQHandler
                def_irq_handler     OS_EVENT_DriverIRQHandler
                def_irq_handler     Reserved55_DriverIRQHandler
                def_irq_handler     Reserved56_DriverIRQHandler
                def_irq_handler     Reserved57_DriverIRQHandler
                def_irq_handler     SDIO_DriverIRQHandler
                def_irq_handler     Reserved59_DriverIRQHandler
                def_irq_handler     Reserved60_DriverIRQHandler
                def_irq_handler     Reserved61_DriverIRQHandler
                def_irq_handler     USB1_UTMI_DriverIRQHandler
                def_irq_handler     USB1_DriverIRQHandler
                def_irq_handler     USB1_NEEDCLK_DriverIRQHandler
                def_irq_handler     SEC_HYPERVISOR_CALL_DriverIRQHandler
                def_irq_handler     SEC_GPIO_INT0_IRQ0_DriverIRQHandler
                def_irq_handler     SEC_GPIO_INT0_IRQ1_DriverIRQHandler
                def_irq_handler     PLU_DriverIRQHandler
                def_irq_handler     SEC_VIO_DriverIRQHandler
                def_irq_handler     HASHCRYPT_DriverIRQHandler
                def_irq_handler     CASER_DriverIRQHandler
                def_irq_handler     PUF_DriverIRQHandler
                def_irq_handler     PQ_DriverIRQHandler
                def_irq_handler     DMA1_DriverIRQHandler
                def_irq_handler     LSPI_HS_DriverIRQHandler

    .end
