/******************************************************************************
*  Filename:       cc26x4.h
*  Revised:        $Date: 2020-06-30 10:53:08 +0200 (Tue, 30 Jun 2020) $
*  Revision:       $Revision: 57872 $
*
*  Description:    Collection of architecture definitions for CC26x4 devices
*
// ##### LICENSE HEADER #####
*
******************************************************************************/

/* IRQ numbers */
typedef enum IRQn
{
    NonMaskableInt_IRQn             = -14,   /* 2  Non Maskable Interrupt */
    HardFault_IRQn                  = -13,   /* 3  Hard Fault Interrupt */
    MemoryManagement_IRQn           = -12,   /* 4  Memory Management Interrupt */
    BusFault_IRQn                   = -11,   /* 5  Bus Fault Interrupt */
    UsageFault_IRQn                 = -10,   /* 6  Usage Fault Interrupt */
    SecureFault_IRQn                = -9,   /* 7  Secure Fault Interrupt */
    SVCall_IRQn                     = -5,   /* 11 SV Call Interrupt */
    DebugMonitor_IRQn               = -4,   /* 12 Debug Monitor Interrupt */
    PendSV_IRQn                     = -2,   /* 14 Pend SV Interrupt */
    SysTick_IRQn                    = -1,   /* 15 System Tick Interrupt */
    AON_GPIO_EDGE_IRQn              = 0,    /* 16 Edge detect event from IOC */
    I2C0_IRQ_IRQn                   = 1,    /* 17 Interrupt event from I2C0 */
    RFC_CPE_1_IRQn                  = 2,    /* 18 Combined Interrupt for CPE Generated events */
    PKA_IRQ_IRQn                    = 3,    /* 19 PKA Interrupt event */
    AON_RTC_COMB_IRQn               = 4,    /* 20 Event from AON_RTC */
    UART0_COMB_IRQn                 = 5,    /* 21 UART0 combined interrupt */
    AUX_SWEV0_IRQn                  = 6,    /* 22 AUX software event 0 */
    SSI0_COMB_IRQn                  = 7,    /* 23 SSI0 combined interrupt */
    SSI1_COMB_IRQn                  = 8,    /* 24 SSI1 combined interrupt */
    RFC_CPE_0_IRQn                  = 9,    /* 25 Combined Interrupt for CPE Generated events */
    RFC_HW_COMB_IRQn                = 10,   /* 26 Combined RFC hardware interrupt */
    RFC_CMD_ACK_IRQn                = 11,   /* 27 RFC Doorbell Command Acknowledgement Interrupt */
    I2S_IRQ_IRQn                    = 12,   /* 28 Interrupt event from I2S */
    AUX_SWEV1_IRQn                  = 13,   /* 29 AUX software event 1 */
    WDT_IRQ_IRQn                    = 14,   /* 30 Watchdog interrupt event */
    GPT0A_IRQn                      = 15,   /* 31 GPT0A interrupt event */
    GPT0B_IRQn                      = 16,   /* 32 GPT0B interrupt event */
    GPT1A_IRQn                      = 17,   /* 33 GPT1A interrupt event */
    GPT1B_IRQn                      = 18,   /* 34 GPT1B interrupt event */
    GPT2A_IRQn                      = 19,   /* 35 GPT2A interrupt event */
    GPT2B_IRQn                      = 20,   /* 36 GPT2B interrupt event */
    GPT3A_IRQn                      = 21,   /* 37 GPT3A interrupt event */
    GPT3B_IRQn                      = 22,   /* 38 GPT3B interrupt event */
    CRYPTO_RESULT_AVAIL_IRQ_IRQn    = 23,   /* 39 CRYPTO result available interupt event */
    DMA_DONE_COMB_IRQn              = 24,   /* 40 Combined DMA done */
    DMA_ERR_IRQn                    = 25,   /* 41 DMA bus error */
    FLASH_IRQn                      = 26,   /* 42 FLASH controller error event */
    SWEV0_IRQn                      = 27,   /* 43 Software event 0 */
    AUX_COMB_IRQn                   = 28,   /* 44 AUX combined event */
    AON_PROG0_IRQn                  = 29,   /* 45 AON programmable event 0 */
    PROG0_IRQn                      = 30,   /* 46 Programmable Interrupt 0 */
    AUX_COMPA_IRQn                  = 31,   /* 47 AUX Compare A event */
    AUX_ADC_IRQ_IRQn                = 32,   /* 48 AUX ADC interrupt event */
    TRNG_IRQ_IRQn                   = 33,   /* 49 TRNG Interrupt event */
    OSC_COMB_IRQn                   = 34,   /* 50 Combined event from Oscillator control */
    AUX_TIMER2_EV0_IRQn             = 35,   /* 51 AUX Timer2 event 0 */
    UART1_COMB_IRQn                 = 36,   /* 52 UART1 combined interrupt */
    BATMON_COMB_IRQn                = 37,   /* 53 Combined event from battery monitor */
    SSI2_COMB_IRQn                  = 38,   /* 54 SSI2 combined interrupt */
    SSI3_COMB_IRQn                  = 39,   /* 55 SSI3 combined interrupt */
    UART2_COMB_IRQn                 = 40,   /* 56 UART2 combined interrupt */
    UART3_COMB_IRQn                 = 41,   /* 57 UART3 combined interrupt */
    I2C1_IRQ_IRQn                   = 42,   /* 58 Interrupt event from I2C1 */
    SWEV1_IRQn                      = 43,   /* 59 Software event 1. Name not provided by DOC yet and thus preliminary */
    SWEV2_IRQn                      = 44,   /* 60 Software event 2. Name not provided by DOC yet and thus preliminary */
    SWEV3_IRQn                      = 45,   /* 61 Software event 3. Name not provided by DOC yet and thus preliminary */
    SWEV4_IRQn                      = 46,   /* 62 Software event 4. Name not provided by DOC yet and thus preliminary */

} IRQn_Type;

/* Architecture-specific constants */
#define __SAUREGION_PRESENT     0x0001U     /* SAU present */
#define __DSP_PRESENT           0x0001U     /* DSP extension present */
#define __Vendor_SysTickConfig  0x0000U     /* Set to 1 if different SysTick Config is used */
#define __CM33_REV              0x0001U     /* Core revision */
#define __MPU_PRESENT           0x0001U     /* MPU present or not */
#define __FPU_PRESENT           0x0001U     /* FPU present or not */
#define __VTOR_PRESENT          0x0001U     /* VTOR present */
#define __NVIC_PRIO_BITS        0x0003U     /* 3 NVIC priority bits */
