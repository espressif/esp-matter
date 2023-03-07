// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <stdbool.h>

#include "pw_boot/boot.h"
#include "pw_boot_cortex_m/boot.h"
#include "pw_preprocessor/compiler.h"

// Extern symbols provided by linker script.
// This symbol contains the size of the image.
extern uint8_t _pw_image_size;

// Default handler to insert into the ARMv8-M vector table (below).
// This function exists for convenience. If a device isn't doing what you
// expect, it might have hit a fault and ended up here.
static void DefaultFaultHandler(void) {
  while (true) {
    // Wait for debugger to attach.
  }
}

// Default interrupt handler that entries in the ARMv8-M vector table (below)
// are aliased to, allowing them to be replaced at link time with OS or SDK
// implementations. If a device isn't doing what you expect, it might have
// raised an interrupt and ended up here.
static void DefaultInterruptHandler(void) {
  while (true) {
    // Wait for debugger to attach.
  }
}

// Default handlers to insert into the ARMv8-M vector table (below) that
// are likely to be replaced by OS implementations.
void SVC_Handler(void) PW_ALIAS(DefaultInterruptHandler);
void DebugMon_Handler(void) PW_ALIAS(DefaultInterruptHandler);
void PendSV_Handler(void) PW_ALIAS(DefaultInterruptHandler);
void SysTick_Handler(void) PW_ALIAS(DefaultInterruptHandler);

// Default handlers to insert into the ARMv8-M vector table (below) that
// are call a driver implementation that may be provided by the SDK.
static void WDT0_IRQHandler(void);
static void DMA0_IRQHandler(void);
static void GPIO_INTA_IRQHandler(void);
static void GPIO_INTB_IRQHandler(void);
static void PIN_INT0_IRQHandler(void);
static void PIN_INT1_IRQHandler(void);
static void PIN_INT2_IRQHandler(void);
static void PIN_INT3_IRQHandler(void);
static void UTICK0_IRQHandler(void);
static void MRT0_IRQHandler(void);
static void CTIMER0_IRQHandler(void);
static void CTIMER1_IRQHandler(void);
static void SCT0_IRQHandler(void);
static void CTIMER3_IRQHandler(void);
static void FLEXCOMM0_IRQHandler(void);
static void FLEXCOMM1_IRQHandler(void);
static void FLEXCOMM2_IRQHandler(void);
static void FLEXCOMM3_IRQHandler(void);
static void FLEXCOMM4_IRQHandler(void);
static void FLEXCOMM5_IRQHandler(void);
static void FLEXCOMM14_IRQHandler(void);
static void FLEXCOMM15_IRQHandler(void);
static void ADC0_IRQHandler(void);
static void Reserved39_IRQHandler(void);
static void ACMP_IRQHandler(void);
static void DMIC0_IRQHandler(void);
static void Reserved42_IRQHandler(void);
static void HYPERVISOR_IRQHandler(void);
static void SECURE_VIOLATION_IRQHandler(void);
static void HWVAD0_IRQHandler(void);
static void Reserved46_IRQHandler(void);
static void RNG_IRQHandler(void);
static void RTC_IRQHandler(void);
static void DSP_TIE_EXPSTATE1_IRQHandler(void);
static void MU_A_IRQHandler(void);
static void PIN_INT4_IRQHandler(void);
static void PIN_INT5_IRQHandler(void);
static void PIN_INT6_IRQHandler(void);
static void PIN_INT7_IRQHandler(void);
static void CTIMER2_IRQHandler(void);
static void CTIMER4_IRQHandler(void);
static void OS_EVENT_IRQHandler(void);
static void FLEXSPI0_FLEXSPI1_IRQHandler(void);
static void FLEXCOMM6_IRQHandler(void);
static void FLEXCOMM7_IRQHandler(void);
static void USDHC0_IRQHandler(void);
static void USDHC1_IRQHandler(void);
static void SGPIO_INTA_IRQHandler(void);
static void SGPIO_INTB_IRQHandler(void);
static void I3C0_IRQHandler(void);
static void USB0_IRQHandler(void);
static void USB0_NEEDCLK_IRQHandler(void);
static void WDT1_IRQHandler(void);
static void USB_PHYDCD_IRQHandler(void);
static void DMA1_IRQHandler(void);
static void PUF_IRQHandler(void);
static void POWERQUAD_IRQHandler(void);
static void CASPER_IRQHandler(void);
static void PMU_PMIC_IRQHandler(void);
static void HASHCRYPT_IRQHandler(void);
static void FLEXCOMM8_IRQHandler(void);
static void FLEXCOMM9_IRQHandler(void);
static void FLEXCOMM10_IRQHandler(void);
static void FLEXCOMM11_IRQHandler(void);
static void FLEXCOMM12_IRQHandler(void);
static void FLEXCOMM13_IRQHandler(void);
static void FLEXCOMM16_IRQHandler(void);
static void I3C1_IRQHandler(void);
static void FLEXIO_IRQHandler(void);
static void LCDIF_IRQHandler(void);
static void GPU_IRQHandler(void);
static void MIPI_IRQHandler(void);
static void Reserved88_IRQHandler(void);
static void SDMA_IRQHandler(void);

// This is the device's interrupt vector table. It's not referenced in any
// code because the platform (EVKMIMXRT595) expects this table to be present
// at the beginning of flash. The exact address is specified in the pw_boot
// configuration as part of the target config.

// This typedef is for convenience when building the vector table. With the
// exception of SP_main (0th entry in the vector table), image length (8th),
// type (9th), reserved 10th entry, and image load address (13th entry), all
// the entries of the vector table are function pointers.
typedef void (*InterruptHandler)(void);

PW_KEEP_IN_SECTION(".vector_table")
const InterruptHandler vector_table[] = {
    // Core Level - CM33

    // The starting location of the stack pointer.
    // This address is NOT an interrupt handler/function pointer, it is simply
    // the address that the main stack pointer should be initialized to. The
    // value is reinterpret casted because it needs to be in the vector table.
    [0] = (InterruptHandler)(&pw_boot_stack_high_addr),

    // Reset handler, dictates how to handle reset interrupt. This is the
    // address that the Program Counter (PC) is initialized to at boot.
    [1] = pw_boot_Entry,

    // NMI handler.
    [2] = DefaultFaultHandler,
    // HardFault handler.
    [3] = DefaultFaultHandler,
    // MemManage (MPU Fault) handler.
    [4] = DefaultFaultHandler,
    // BusFault handler.
    [5] = DefaultFaultHandler,
    // UsageFault handler.
    [6] = DefaultFaultHandler,
    // SecureFault handler.
    [7] = DefaultFaultHandler,
    // Image Length.
    [8] = (InterruptHandler)(&_pw_image_size),
    // Image Type.
    [9] = 0,
    // Reserved.
    [10] = 0,
    // SVCall handler.
    [11] = SVC_Handler,
    // DebugMon handler.
    [12] = DebugMon_Handler,
    // Image Load Address.
    [13] = (InterruptHandler)(&pw_boot_vector_table_addr),
    // PendSV handler.
    [14] = PendSV_Handler,
    // SysTick handler.
    [15] = SysTick_Handler,

    // Chip Level - MIMXRT595S_cm33

    // Watchdog timer interrupt.
    [16] = WDT0_IRQHandler,
    // DMA interrupt.
    [17] = DMA0_IRQHandler,
    // GPIO Interrupt A.
    [18] = GPIO_INTA_IRQHandler,
    // GPIO Interrupt B.
    [19] = GPIO_INTB_IRQHandler,
    // General Purpose Input/Output interrupt 0.
    [20] = PIN_INT0_IRQHandler,
    // General Purpose Input/Output interrupt 1.
    [21] = PIN_INT1_IRQHandler,
    // General Purpose Input/Output interrupt 2.
    [22] = PIN_INT2_IRQHandler,
    // General Purpose Input/Output interrupt 3.
    [23] = PIN_INT3_IRQHandler,
    // Micro-tick Timer.
    [24] = UTICK0_IRQHandler,
    // Multi-Rate Timer.
    [25] = MRT0_IRQHandler,
    // Standard counter/timer CTIMER0.
    [26] = CTIMER0_IRQHandler,
    // Standard counter/timer CTIMER1.
    [27] = CTIMER1_IRQHandler,
    // SCTimer/PWM.
    [28] = SCT0_IRQHandler,
    // Standard counter/timer CTIMER3.
    [29] = CTIMER3_IRQHandler,
    // FlexComm interrupt.
    [30] = FLEXCOMM0_IRQHandler,
    // FlexComm interrupt.
    [31] = FLEXCOMM1_IRQHandler,
    // FlexComm interrupt.
    [32] = FLEXCOMM2_IRQHandler,
    // FlexComm interrupt.
    [33] = FLEXCOMM3_IRQHandler,
    // FlexComm interrupt.
    [34] = FLEXCOMM4_IRQHandler,
    // FlexComm interrupt.
    [35] = FLEXCOMM5_IRQHandler,
    // FlexComm interrupt. Standalone SPI.
    [36] = FLEXCOMM14_IRQHandler,
    // FlexComm interrupt. Standalone I2C.
    [37] = FLEXCOMM15_IRQHandler,
    // Analog-to-Digital Converter interrupt.
    [38] = ADC0_IRQHandler,
    // Reserved interrupt.
    [39] = Reserved39_IRQHandler,
    // Analog comparator Interrupts.
    [40] = ACMP_IRQHandler,
    // Digital Microphone Interface interrupt.
    [41] = DMIC0_IRQHandler,
    // Reserved interrupt.
    [42] = Reserved42_IRQHandler,
    // Hypervisor interrupt.
    [43] = HYPERVISOR_IRQHandler,
    // Secure violation interrupt.
    [44] = SECURE_VIOLATION_IRQHandler,
    // Hardware Voice Activity Detector interrupt.
    [45] = HWVAD0_IRQHandler,
    // Reserved interrupt.
    [46] = Reserved46_IRQHandler,
    // Random Number Generator interrupt.
    [47] = RNG_IRQHandler,
    // Real Time Clock Alarm interrupt OR Wakeup timer interrupt.
    [48] = RTC_IRQHandler,
    // DSP interrupt.
    [49] = DSP_TIE_EXPSTATE1_IRQHandler,
    // Messaging Unit - Side A.
    [50] = MU_A_IRQHandler,
    // General Purpose Input/Output interrupt 4.
    [51] = PIN_INT4_IRQHandler,
    // General Purpose Input/Output interrupt 5.
    [52] = PIN_INT5_IRQHandler,
    // General Purpose Input/Output interrupt 6.
    [53] = PIN_INT6_IRQHandler,
    // General Purpose Input/Output interrupt 7.
    [54] = PIN_INT7_IRQHandler,
    // Standard counter/timer CTIMER2.
    [55] = CTIMER2_IRQHandler,
    // Standard counter/timer CTIMER4.
    [56] = CTIMER4_IRQHandler,
    // Event timer M33 Wakeup/interrupt.
    [57] = OS_EVENT_IRQHandler,
    // FlexSPI0_IRQ OR FlexSPI1_IRQ.
    [58] = FLEXSPI0_FLEXSPI1_IRQHandler,
    // FlexComm interrupt.
    [59] = FLEXCOMM6_IRQHandler,
    // FlexComm interrupt.
    [60] = FLEXCOMM7_IRQHandler,
    // USDHC interrupt.
    [61] = USDHC0_IRQHandler,
    // USDHC interrupt.
    [62] = USDHC1_IRQHandler,
    // Secure GPIO HS interrupt 0.
    [63] = SGPIO_INTA_IRQHandler,
    // Secure GPIO HS interrupt 1.
    [64] = SGPIO_INTB_IRQHandler,
    // Improved Inter Integrated Circuit 0 interrupt.
    [65] = I3C0_IRQHandler,
    // USB device.
    [66] = USB0_IRQHandler,
    // USB Activity Wake-up Interrupt.
    [67] = USB0_NEEDCLK_IRQHandler,
    // Watchdog timer 1 interrupt.
    [68] = WDT1_IRQHandler,
    // USBPHY DCD interrupt.
    [69] = USB_PHYDCD_IRQHandler,
    // DMA interrupt.
    [70] = DMA1_IRQHandler,
    // QuidKey interrupt.
    [71] = PUF_IRQHandler,
    // Powerquad interrupt.
    [72] = POWERQUAD_IRQHandler,
    // Caspar interrupt.
    [73] = CASPER_IRQHandler,
    // Power Management Control interrupt.
    [74] = PMU_PMIC_IRQHandler,
    // SHA interrupt.
    [75] = HASHCRYPT_IRQHandler,
    // FlexComm interrupt.
    [76] = FLEXCOMM8_IRQHandler,
    // FlexComm interrupt.
    [77] = FLEXCOMM9_IRQHandler,
    // FlexComm interrupt.
    [78] = FLEXCOMM10_IRQHandler,
    // FlexComm interrupt.
    [79] = FLEXCOMM11_IRQHandler,
    // FlexComm interrupt.
    [80] = FLEXCOMM12_IRQHandler,
    // FlexComm interrupt.
    [81] = FLEXCOMM13_IRQHandler,
    // FlexComm interrupt.
    [82] = FLEXCOMM16_IRQHandler,
    // Improved Inter Integrated Circuit 1 interrupt.
    [83] = I3C1_IRQHandler,
    // Flexible I/O interrupt.
    [84] = FLEXIO_IRQHandler,
    // Liquid Crystal Display interface interrupt.
    [85] = LCDIF_IRQHandler,
    // Graphics Processor Unit interrupt.
    [86] = GPU_IRQHandler,
    // MIPI interrupt.
    [87] = MIPI_IRQHandler,
    // Reserved interrupt.
    [88] = Reserved88_IRQHandler,
    // Smart DMA Engine Controller interrupt.
    [89] = SDMA_IRQHandler,
};

// Define handlers that call out to a driver handler provided by the SDK.
#define DRIVER_HANDLER(_IRQHandler, _DriverIRQHandler)            \
  void _DriverIRQHandler(void) PW_ALIAS(DefaultInterruptHandler); \
  static void _IRQHandler(void) { _DriverIRQHandler(); }

DRIVER_HANDLER(WDT0_IRQHandler, WDT0_DriverIRQHandler);
DRIVER_HANDLER(DMA0_IRQHandler, DMA0_DriverIRQHandler);
DRIVER_HANDLER(GPIO_INTA_IRQHandler, GPIO_INTA_DriverIRQHandler);
DRIVER_HANDLER(GPIO_INTB_IRQHandler, GPIO_INTB_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT0_IRQHandler, PIN_INT0_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT1_IRQHandler, PIN_INT1_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT2_IRQHandler, PIN_INT2_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT3_IRQHandler, PIN_INT3_DriverIRQHandler);
DRIVER_HANDLER(UTICK0_IRQHandler, UTICK0_DriverIRQHandler);
DRIVER_HANDLER(MRT0_IRQHandler, MRT0_DriverIRQHandler);
DRIVER_HANDLER(CTIMER0_IRQHandler, CTIMER0_DriverIRQHandler);
DRIVER_HANDLER(CTIMER1_IRQHandler, CTIMER1_DriverIRQHandler);
DRIVER_HANDLER(SCT0_IRQHandler, SCT0_DriverIRQHandler);
DRIVER_HANDLER(CTIMER3_IRQHandler, CTIMER3_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM0_IRQHandler, FLEXCOMM0_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM1_IRQHandler, FLEXCOMM1_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM2_IRQHandler, FLEXCOMM2_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM3_IRQHandler, FLEXCOMM3_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM4_IRQHandler, FLEXCOMM4_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM5_IRQHandler, FLEXCOMM5_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM14_IRQHandler, FLEXCOMM14_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM15_IRQHandler, FLEXCOMM15_DriverIRQHandler);
DRIVER_HANDLER(ADC0_IRQHandler, ADC0_DriverIRQHandler);
DRIVER_HANDLER(Reserved39_IRQHandler, Reserved39_DriverIRQHandler);
DRIVER_HANDLER(ACMP_IRQHandler, ACMP_DriverIRQHandler);
DRIVER_HANDLER(DMIC0_IRQHandler, DMIC0_DriverIRQHandler);
DRIVER_HANDLER(Reserved42_IRQHandler, Reserved42_DriverIRQHandler);
DRIVER_HANDLER(HYPERVISOR_IRQHandler, HYPERVISOR_DriverIRQHandler);
DRIVER_HANDLER(SECURE_VIOLATION_IRQHandler, SECURE_VIOLATION_DriverIRQHandler);
DRIVER_HANDLER(HWVAD0_IRQHandler, HWVAD0_DriverIRQHandler);
DRIVER_HANDLER(Reserved46_IRQHandler, Reserved46_DriverIRQHandler);
DRIVER_HANDLER(RNG_IRQHandler, RNG_DriverIRQHandler);
DRIVER_HANDLER(RTC_IRQHandler, RTC_DriverIRQHandler);
DRIVER_HANDLER(DSP_TIE_EXPSTATE1_IRQHandler,
               DSP_TIE_EXPSTATE1_DriverIRQHandler);
DRIVER_HANDLER(MU_A_IRQHandler, MU_A_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT4_IRQHandler, PIN_INT4_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT5_IRQHandler, PIN_INT5_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT6_IRQHandler, PIN_INT6_DriverIRQHandler);
DRIVER_HANDLER(PIN_INT7_IRQHandler, PIN_INT7_DriverIRQHandler);
DRIVER_HANDLER(CTIMER2_IRQHandler, CTIMER2_DriverIRQHandler);
DRIVER_HANDLER(CTIMER4_IRQHandler, CTIMER4_DriverIRQHandler);
DRIVER_HANDLER(OS_EVENT_IRQHandler, OS_EVENT_DriverIRQHandler);
DRIVER_HANDLER(FLEXSPI0_FLEXSPI1_IRQHandler,
               FLEXSPI0_FLEXSPI1_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM6_IRQHandler, FLEXCOMM6_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM7_IRQHandler, FLEXCOMM7_DriverIRQHandler);
DRIVER_HANDLER(USDHC0_IRQHandler, USDHC0_DriverIRQHandler);
DRIVER_HANDLER(USDHC1_IRQHandler, USDHC1_DriverIRQHandler);
DRIVER_HANDLER(SGPIO_INTA_IRQHandler, SGPIO_INTA_DriverIRQHandler);
DRIVER_HANDLER(SGPIO_INTB_IRQHandler, SGPIO_INTB_DriverIRQHandler);
DRIVER_HANDLER(I3C0_IRQHandler, I3C0_DriverIRQHandler);
DRIVER_HANDLER(USB0_IRQHandler, USB0_DriverIRQHandler);
DRIVER_HANDLER(USB0_NEEDCLK_IRQHandler, USB0_NEEDCLK_DriverIRQHandler);
DRIVER_HANDLER(WDT1_IRQHandler, WDT1_DriverIRQHandler);
DRIVER_HANDLER(USB_PHYDCD_IRQHandler, USB_PHYDCD_DriverIRQHandler);
DRIVER_HANDLER(DMA1_IRQHandler, DMA1_DriverIRQHandler);
DRIVER_HANDLER(PUF_IRQHandler, PUF_DriverIRQHandler);
DRIVER_HANDLER(POWERQUAD_IRQHandler, POWERQUAD_DriverIRQHandler);
DRIVER_HANDLER(CASPER_IRQHandler, CASPER_DriverIRQHandler);
DRIVER_HANDLER(PMU_PMIC_IRQHandler, PMU_PMIC_DriverIRQHandler);
DRIVER_HANDLER(HASHCRYPT_IRQHandler, HASHCRYPT_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM8_IRQHandler, FLEXCOMM8_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM9_IRQHandler, FLEXCOMM9_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM10_IRQHandler, FLEXCOMM10_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM11_IRQHandler, FLEXCOMM11_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM12_IRQHandler, FLEXCOMM12_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM13_IRQHandler, FLEXCOMM13_DriverIRQHandler);
DRIVER_HANDLER(FLEXCOMM16_IRQHandler, FLEXCOMM16_DriverIRQHandler);
DRIVER_HANDLER(I3C1_IRQHandler, I3C1_DriverIRQHandler);
DRIVER_HANDLER(FLEXIO_IRQHandler, FLEXIO_DriverIRQHandler);
DRIVER_HANDLER(LCDIF_IRQHandler, LCDIF_DriverIRQHandler);
DRIVER_HANDLER(GPU_IRQHandler, GPU_DriverIRQHandler);
DRIVER_HANDLER(MIPI_IRQHandler, MIPI_DriverIRQHandler);
DRIVER_HANDLER(Reserved88_IRQHandler, Reserved88_DriverIRQHandler);
DRIVER_HANDLER(SDMA_IRQHandler, SDMA_DriverIRQHandler);
