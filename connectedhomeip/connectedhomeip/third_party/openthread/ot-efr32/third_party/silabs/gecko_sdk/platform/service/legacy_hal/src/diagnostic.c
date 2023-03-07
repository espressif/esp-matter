/***************************************************************************//**
 * @file
 * @brief stubs for HAL diagnostic functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "hal.h"
#include "cortexm3/diagnostic.h"
#include "micro.h"

#include "em_emu.h"
#include "em_rmu.h"

#if defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
#include "sl_iostream.h"
#endif

// Crash info live in noinit RAM segment that is not modified during startup.
NO_INIT(HalCrashInfoType halCrashInfo);

//------------------------------------------------------------------------------
// Preprocessor definitions

// Reserved instruction executed after a failed assert to cause a usage fault
#define ASSERT_USAGE_OPCODE                 0xDE42

#if !defined(WDOG0)
  #define WDOG0      WDOG
  #define WDOG0_IRQn WDOG_IRQn
#endif

#ifdef RTOS
  #include "rtos/rtos.h"
  #define freeRTOS 1
#else
  #define freeRTOS 0
#endif

// Forward Declarations
extern void emRadioSleep(void);

//------------------------------------------------------------------------------
// Local Variables

#if defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)

static const char * const cfsrBits[] =
{
  // Memory management (MPU) faults
  "IACCVIOL: attempted instruction fetch from a no-execute address",  // B0
  "DACCVIOL: attempted load or store at an illegal address",          // B1
  "",                                                                 // B2
  "MUNSTKERR: unstack from exception return caused access violation", // B3
  "MSTKERR: stacking from exception caused access violation",         // B4
  "",                                                                 // B5
  "",                                                                 // B6
  "MMARVALID: MMAR contains valid fault address",                     // B7

  // Bus faults
  "IBUSERR: instruction prefetch caused bus fault",                   // B8
  "PRECISERR: precise data bus fault",                                // B9
  "IMPRECISERR: imprecise data bus fault",                            // B10
  "UNSTKERR: unstacking on exception return caused data bus fault",   // B11
  "STKERR: stacking on exception entry caused data bus fault",        // B12
  "",                                                                 // B13
  "",                                                                 // B14
  "BFARVALID: BFAR contains valid fault address",                     // B15

  // Usage faults
  "UNDEFINSTR: tried to execute an undefined instruction",            // B16
  "INVSTATE: invalid EPSR - e.g., tried to switch to ARM mode",       // B17
  "INVPC: exception return integrity checks failed",                  // B18
  "NOCP: attempted to execute a coprocessor instruction",             // B19
  "",                                                                 // B20
  "",                                                                 // B21
  "",                                                                 // B22
  "",                                                                 // B23
  "UNALIGNED: attempted an unaligned memory access",                  // B24
  "DIVBYZERO: attempted to execute SDIV or UDIV with divisor of 0"    // B25
};

static const char * const intActiveBits[] =
{
#if defined (_EFR_DEVICE)
  #if defined (_SILICON_LABS_32B_SERIES_1_CONFIG_1)
  "EMU_IRQn",         // B0
  "FRC_PRI_IRQn",     // B1
  "WDOG0_IRQn",       // B2
  "FRC_IRQn",         // B3
  "MODEM_IRQn",       // B4
  "RAC_SEQ_IRQn",     // B5
  "RAC_RSM_IRQn",     // B6
  "BUFC_IRQn",        // B7
  "LDMA_IRQn",        // B8
  "GPIO_EVEN_IRQn",   // B9
  "TIMER0_IRQn",      // B10
  "USART0_RX_IRQn",   // B11
  "USART0_TX_IRQn",   // B12
  "ACMP0_IRQn",       // B13
  "ADC0_IRQn",        // B14
  "IDAC0_IRQn",       // B15
  "I2C0_IRQn",        // B16
  "GPIO_ODD_IRQn",    // B17
  "TIMER1_IRQn",      // B18
  "USART1_RX_IRQn",   // B19
  "USART1_TX_IRQn",   // B20
  "LEUART0_IRQn",     // B21
  "PCNT0_IRQn",       // B22
  "CMU_IRQn",         // B23
  "MSC_IRQn",         // B24
  "CRYPTO_IRQn",      // B25
  "LETIMER0_IRQn",    // B26
  "AGC_IRQn",         // B27
  "PROTIMER_IRQn",    // B28
  "RTCC_IRQn",        // B29
  "SYNTH_IRQn",       // B30
  "CRYOTIMER_IRQn",   // B31
  "RFSENSE_IRQn",     // B32
  "FPUEH_IRQn",       // B33
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_2)
  "EMU_IRQn",         // B0
  "FRC_PRI_IRQn",     // B1
  "WDOG0_IRQn",       // B2
  "WDOG1_IRQn",       // B3
  "FRC_IRQn",         // B4
  "MODEM_IRQn",       // B5
  "RAC_SEQ_IRQn",     // B6
  "RAC_RSM_IRQn",     // B7
  "BUFC_IRQn",        // B8
  "LDMA_IRQn",        // B9
  "GPIO_EVEN_IRQn",   // B10
  "TIMER0_IRQn",      // B11
  "USART0_RX_IRQn",   // B12
  "USART0_TX_IRQn",   // B13
  "ACMP0_IRQn",       // B14
  "ADC0_IRQn",        // B15
  "IDAC0_IRQn",       // B16
  "I2C0_IRQn",        // B17
  "GPIO_ODD_IRQn",    // B18
  "TIMER1_IRQn",      // B19
  "USART1_RX_IRQn",   // B20
  "USART1_TX_IRQn",   // B21
  "LEUART0_IRQn",     // B22
  "PCNT0_IRQn",       // B23
  "CMU_IRQn",         // B24
  "MSC_IRQn",         // B25
  "CRYPTO0_IRQn",     // B26
  "LETIMER0_IRQn",    // B27
  "AGC_IRQn",         // B28
  "PROTIMER_IRQn",    // B29
  "RTCC_IRQn",        // B30
  "SYNTH_IRQn",       // B31
  "CRYOTIMER_IRQn",   // B32
  "RFSENSE_IRQn",     // B33
  "FPUEH_IRQn",       // B34
  "SMU_IRQn",         // B35
  "WTIMER0_IRQn",     // B36
  "WTIMER1_IRQn",     // B37
  "PCNT1_IRQn",       // B38
  "PCNT2_IRQn",       // B39
  "USART2_RX_IRQn",   // B40
  "USART2_TX_IRQn",   // B41
  "I2C1_IRQn",        // B42
  "USART3_RX_IRQn",   // B43
  "USART3_TX_IRQn",   // B44
  "VDAC0_IRQn",       // B45
  "CSEN_IRQn",        // B46
  "LESENSE_IRQn",     // B47
  "CRYPTO1_IRQn",     // B48
  "TRNG0_IRQn",       // B49
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_3)
  "EMU_IRQn",         // B0
  "FRC_PRI_IRQn",     // B1
  "WDOG0_IRQn",       // B2
  "WDOG1_IRQn",       // B3
  "FRC_IRQn",         // B4
  "MODEM_IRQn",       // B5
  "RAC_SEQ_IRQn",     // B6
  "RAC_RSM_IRQn",     // B7
  "BUFC_IRQn",        // B8
  "LDMA_IRQn",        // B9
  "GPIO_EVEN_IRQn",   // B10
  "TIMER0_IRQn",      // B11
  "USART0_RX_IRQn",   // B12
  "USART0_TX_IRQn",   // B13
  "ACMP0_IRQn",       // B14
  "ADC0_IRQn",        // B15
  "IDAC0_IRQn",       // B16
  "I2C0_IRQn",        // B17
  "GPIO_ODD_IRQn",    // B18
  "TIMER1_IRQn",      // B19
  "USART1_RX_IRQn",   // B20
  "USART1_TX_IRQn",   // B21
  "LEUART0_IRQn",     // B22
  "PCNT0_IRQn",       // B23
  "CMU_IRQn",         // B24
  "MSC_IRQn",         // B25
  "CRYPTO0_IRQn",     // B26
  "LETIMER0_IRQn",    // B27
  "AGC_IRQn",         // B28
  "PROTIMER_IRQn",    // B29
  "PRORTC_IRQn",      // B30
  "RTCC_IRQn",        // B31
  "SYNTH_IRQn",       // B32
  "CRYOTIMER_IRQn",   // B33
  "RFSENSE_IRQn",     // B34
  "FPUEH_IRQn",       // B35
  "SMU_IRQn",         // B36
  "WTIMER0_IRQn",     // B37
  "USART2_RX_IRQn",   // B38
  "USART2_TX_IRQn",   // B39
  "I2C1_IRQn",        // B40
  "VDAC0_IRQn",       // B41
  "CSEN_IRQn",        // B42
  "LESENSE_IRQn",     // B43
  "CRYPTO1_IRQn",     // B44
  "TRNG0_IRQn"        // B45
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_4)
  "EMU_IRQn",         // B0
  "FRC_PRI_IRQn",     // B1
  "WDOG0_IRQn",       // B2
  "WDOG1_IRQn",       // B3
  "FRC_IRQn",         // B4
  "MODEM_IRQn",       // B5
  "RAC_SEQ_IRQn",     // B6
  "RAC_RSM_IRQn",     // B7
  "BUFC_IRQn",        // B8
  "LDMA_IRQn",        // B9
  "GPIO_EVEN_IRQn",   // B10
  "TIMER0_IRQn",      // B11
  "USART0_RX_IRQn",   // B12
  "USART0_TX_IRQn",   // B13
  "ACMP0_IRQn",       // B14
  "ADC0_IRQn",        // B15
  "IDAC0_IRQn",       // B16
  "I2C0_IRQn",        // B17
  "GPIO_ODD_IRQn",    // B18
  "TIMER1_IRQn",      // B19
  "USART1_RX_IRQn",   // B20
  "USART1_TX_IRQn",   // B21
  "LEUART0_IRQn",     // B22
  "PCNT0_IRQn",       // B23
  "CMU_IRQn",         // B24
  "MSC_IRQn",         // B25
  "CRYPTO0_IRQn",     // B26
  "LETIMER0_IRQn",    // B27
  "AGC_IRQn",         // B28
  "PROTIMER_IRQn",    // B29
  "PRORTC_IRQn",      // B30
  "RTCC_IRQn",        // B31
  "SYNTH_IRQn",       // B32
  "CRYOTIMER_IRQn",   // B33
  "RFSENSE_IRQn",     // B34
  "FPUEH_IRQn",       // B35
  "SMU_IRQn",         // B36
  "WTIMER0_IRQn",     // B37
  "VDAC0_IRQn",       // B38
  "LESENSE_IRQn",     // B39
  "TRNG0_IRQn",       // B40
  "SYSCFG_IRQn",      // B41
  #elif defined (_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  "SETAMPERHOST_IRQn",     // B0
  "SEMBRX_IRQn",           // B1
  "SEMBTX_IRQn",           // B2
  "SMU_SECURE_IRQn",       // B3
  "SMU_PRIVILEGED_IRQn",   // B4
  "EMU_IRQn",              // B5
  "TIMER0_IRQn",           // B6
  "TIMER1_IRQn",           // B7
  "TIMER2_IRQn",           // B8
  "TIMER3_IRQn",           // B9
  "RTCC_IRQn",             // B10
  "USART0_RX_IRQn",        // B11
  "USART0_TX_IRQn",        // B12
  "USART1_RX_IRQn",        // B13
  "USART1_TX_IRQn",        // B14
  "USART2_RX_IRQn",        // B15
  "USART2_TX_IRQn",        // B16
  "ICACHE0_IRQn",          // B17
  "BURTC_IRQn",            // B18
  "LETIMER0_IRQn",         // B19
  "SYSCFG_IRQn",           // B20
  "LDMA_IRQn",             // B21
  "LFXO_IRQn",             // B22
  "LFRCO_IRQn",            // B23
  "ULFRCO_IRQn",           // B24
  "GPIO_ODD_IRQn",         // B25
  "GPIO_EVEN_IRQn",        // B26
  "I2C0_IRQn",             // B27
  "I2C1_IRQn",             // B28
  "EMUDG_IRQn",            // B29
  "EMUSE_IRQn",            // B30
  "AGC_IRQn",              // B31
  "BUFC_IRQn",             // B32
  "FRC_PRI_IRQn",          // B33
  "FRC_IRQn",              // B34
  "MODEM_IRQn",            // B35
  "PROTIMER_IRQn",         // B36
  "RAC_RSM_IRQn",          // B37
  "RAC_SEQ_IRQn",          // B38
  "PRORTC_IRQn",           // B39
  "SYNTH_IRQn",            // B40
  "ACMP0_IRQn",            // B41
  "ACMP1_IRQn",            // B42
  "WDOG0_IRQn",            // B43
  "WDOG1_IRQn",            // B44
  "HFXO00_IRQn",           // B45
  "HFRCO0_IRQn",           // B46
  "HFRCOEM23_IRQn",        // B47
  "CMU_IRQn",              // B48
  "AES_IRQn",              // B49
  "IADC_IRQn",             // B50
  "MSC_IRQn",              // B51
  "DPLL0_IRQn",            // B52
  "SW0_IRQn",              // B53
  "SW1_IRQn",              // B54
  "SW2_IRQn",              // B55
  "SW3_IRQn",              // B56
  "KERNEL0_IRQn",          // B57
  "KERNEL1_IRQn",          // B58
  "M33CTI0_IRQn",          // B59
  "M33CTI1_IRQn",          // B60
  #elif defined (_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  "CRYPTOACC_IRQn",         // B0
  "TRNG_IRQn",              // B1
  "PKE_IRQn",               // B2
  "SMU_SECURE_IRQn",        // B3
  "SMU_S_PRIVILEGED_IRQn",  // B4
  "SMU_NS_PRIVILEGED_IRQn", // B5
  "EMU_IRQn",               // B6
  "TIMER0_IRQn",            // B7
  "TIMER1_IRQn",            // B8
  "TIMER2_IRQn",            // B9
  "TIMER3_IRQn",            // B10
  "TIMER4_IRQn",            // B11
  "RTCC_IRQn",              // B12
  "USART0_RX_IRQn",         // B13
  "USART0_TX_IRQn",         // B14
  "USART1_RX_IRQn",         // B15
  "USART1_TX_IRQn",         // B16
  "ICACHE0_IRQn",           // B17
  "BURTC_IRQn",             // B18
  "LETIMER0_IRQn",          // B19
  "SYSCFG_IRQn",            // B20
  "LDMA_IRQn",              // B21
  "LFXO_IRQn",              // B22
  "LFRCO_IRQn",             // B23
  "ULFRCO_IRQn",            // B24
  "GPIO_ODD_IRQn",          // B25
  "GPIO_EVEN_IRQn",         // B26
  "I2C0_IRQn",              // B27
  "I2C1_IRQn",              // B28
  "EMUDG_IRQn",             // B29
  "EMUSE_IRQn",             // B30
  "AGC_IRQn",               // B31
  "BUFC_IRQn",              // B32
  "FRC_PRI_IRQn",           // B33
  "FRC_IRQn",               // B34
  "MODEM_IRQn",             // B35
  "PROTIMER_IRQn",          // B36
  "RAC_RSM_IRQn",           // B37
  "RAC_SEQ_IRQn",           // B38
  "RDMAILBOX_IRQn",         // B39
  "RFSENSE_IRQn",           // B40
  "PRORTC_IRQn",            // B41
  "SYNTH_IRQn",             // B42
  "WDOG0_IRQn",             // B43
  "HFXO0_IRQn",             // B44
  "HFRCO0_IRQn",            // B45
  "CMU_IRQn",               // B46
  "AES_IRQn",               // B47
  "IADC_IRQn",              // B48
  "MSC_IRQn",               // B49
  "DPLL0_IRQn",             // B50
  "PDM_IRQn",               // B51
  "SW0_IRQn",               // B52
  "SW1_IRQn",               // B53
  "SW2_IRQn",               // B54
  "SW3_IRQn",               // B55
  "KERNEL0_IRQn",           // B56
  "KERNEL1_IRQn",           // B57
  "M33CTI0_IRQn",           // B58
  "M33CTI1_IRQn",           // B59
  "EMUEFP_IRQn",            // B60
  "DCDC_IRQn",              // B61
  "EUART0_RX_IRQn",         // B62
  "EUART0_TX_IRQn",         // B63
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
  "SMU_SECURE_IRQn",        // B0
  "SMU_S_PRIVILEGED_IRQn",  // B1
  "SMU_NS_PRIVILEGED_IRQn", // B2
  "EMU_IRQn",               // B3
  "TIMER0_IRQn",            // B4
  "TIMER1_IRQn",            // B5
  "TIMER2_IRQn",            // B6
  "TIMER3_IRQn",            // B7
  "TIMER4_IRQn",            // B8
  "USART0_RX_IRQn",         // B9
  "USART0_TX_IRQn",         // B10
  "EUSART0_RX_IRQn",        // B11
  "EUSART0_TX_IRQn",        // B12
  "EUSART1_RX_IRQn",        // B13
  "EUSART1_TX_IRQn",        // B14
  "EUSART2_RX_IRQn",        // B15
  "EUSART2_TX_IRQn",        // B16
  "ICACHE0_IRQn",           // B17
  "BURTC_IRQn",             // B18
  "LETIMER0_IRQn",          // B19
  "SYSCFG_IRQn",            // B20
  "MPAHBRAM_IRQn",          // B21
  "LDMA_IRQn",              // B22
  "LFXO_IRQn",              // B23
  "LFRCO_IRQn",             // B24
  "ULFRCO_IRQn",            // B25
  "GPIO_ODD_IRQn",          // B26
  "GPIO_EVEN_IRQn",         // B27
  "I2C0_IRQn",              // B28
  "I2C1_IRQn",              // B29
  "EMUDG_IRQn",             // B30
  "AGC_IRQn",               // B31
  "BUFC_IRQn",              // B32
  "FRC_PRI_IRQn",           // B33
  "FRC_IRQn",               // B34
  "MODEM_IRQn",             // B35
  "PROTIMER_IRQn",          // B36
  "RAC_RSM_IRQn",           // B37
  "RAC_SEQ_IRQn",           // B38
  "HOSTMAILBOX_IRQn",       // B39
  "SYNTH_IRQn",             // B40
  "ACMP0_IRQn",             // B41
  "ACMP1_IRQn",             // B42
  "WDOG0_IRQn",             // B43
  "WDOG1_IRQn",             // B44
  "HFXO0_IRQn",             // B45
  "HFRCO0_IRQn",            // B46
  "HFRCOEM23_IRQn",         // B47
  "CMU_IRQn",               // B48
  "AES_IRQn",               // B49
  "IADC_IRQn",              // B50
  "MSC_IRQn",               // B51
  "DPLL0_IRQn",             // B52
  "EMUEFP_IRQn",            // B53
  "DCDC_IRQn",              // B54
  "VDAC_IRQn",              // B55
  "PCNT0_IRQn",             // B56
  "SW0_IRQn",               // B57
  "SW1_IRQn",               // B58
  "SW2_IRQn",               // B59
  "SW3_IRQn",               // B60
  "KERNEL0_IRQn",           // B61
  "KERNEL1_IRQn",           // B62
  "M33CTI0_IRQn",           // B63
  "M33CTI1_IRQn",           // B64
  "FPUEXH_IRQn",            // B65
  "SEMBRX_IRQn",            // B67
  "SEMBTX_IRQn",            // B68
  "LESENSE_IRQn",           // B69
  "SYSRTC_APP_IRQn",        // B70
  "SYSRTC_SEQ_IRQn",        // B71
  "LCD_IRQn",               // B72
  "KEYSCAN_IRQn",           // B73
  "RFECA0_IRQn",            // B74
  "RFECA1_IRQn",            // B75
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  "SMU_SECURE_IRQn",        // B0
  "SMU_S_PRIVILEGED_IRQn",  // B1
  "SMU_NS_PRIVILEGED_IRQn", // B2
  "EMU_IRQn",               // B3
  "TIMER0_IRQn",            // B4
  "TIMER1_IRQn",            // B5
  "TIMER2_IRQn",            // B6
  "TIMER3_IRQn",            // B7
  "TIMER4_IRQn",            // B8
  "USART0_RX_IRQn",         // B9
  "USART0_TX_IRQn",         // B10
  "EUSART0_RX_IRQn",        // B11
  "EUSART0_TX_IRQn",        // B12
  "EUSART1_RX_IRQn",        // B13
  "EUSART1_TX_IRQn",        // B14
  "MVP_IRQn",               // B15
  "ICACHE0_IRQn",           // B16
  "BURTC_IRQn",             // B17
  "LETIMER0_IRQn",          // B18
  "SYSCFG_IRQn",            // B19
  "MPAHBRAM_IRQn",          // B20
  "LDMA_IRQn",              // B21
  "LFXO_IRQn",              // B22
  "LFRCO_IRQn",             // B23
  "ULFRCO_IRQn",            // B24
  "GPIO_ODD_IRQn",          // B25
  "GPIO_EVEN_IRQn",         // B26
  "I2C0_IRQn",              // B27
  "I2C1_IRQn",              // B28
  "EMUDG_IRQn",             // B29
  "AGC_IRQn",               // B30
  "BUFC_IRQn",              // B31
  "FRC_PRI_IRQn",           // B32
  "FRC_IRQn",               // B33
  "MODEM_IRQn",             // B34
  "PROTIMER_IRQn",          // B35
  "RAC_RSM_IRQn",           // B36
  "RAC_SEQ_IRQn",           // B37
  "HOSTMAILBOX_IRQn",       // B38
  "SYNTH_IRQn",             // B39
  "ACMP0_IRQn",             // B40
  "ACMP1_IRQn",             // B41
  "WDOG0_IRQn",             // B42
  "WDOG1_IRQn",             // B43
  "HFXO0_IRQn",             // B44
  "HFRCO0_IRQn",            // B45
  "HFRCOEM23_IRQn",         // B46
  "CMU_IRQn",               // B47
  "AES_IRQn",               // B48
  "IADC_IRQn",              // B49
  "MSC_IRQn",               // B50
  "DPLL0_IRQn",             // B51
  "EMUEFP_IRQn",            // B52
  "DCDC_IRQn",              // B53
  "PCNT0_IRQn",             // B54
  "SW0_IRQn",               // B55
  "SW1_IRQn",               // B56
  "SW2_IRQn",               // B57
  "SW3_IRQn",               // B58
  "KERNEL0_IRQn",           // B59
  "KERNEL1_IRQn",           // B60
  "FPUEXH_IRQn",            // B61
  "SETAMPERHOST_IRQn",      // B62
  "SEMBRX_IRQn",            // B63
  "SEMBTX_IRQn",            // B64
  "SYSRTC_APP_IRQn",        // B65
  "SYSRTC_SEQ_IRQn",        // B66
  "KEYSCAN_IRQn",           // B67
  "RFECA0_IRQn",            // B68
  "RFECA1_IRQn",            // B69
  "VDAC0_IRQn",             // B70
  "VDAC1_IRQn",             // B71
  "AHB2AHB0_IRQn",          // B72
  "AHB2AHB1_IRQn"           // B73
#elif defined (_SILICON_LABS_32B_SERIES_2_CONFIG_7)
  "CRYPTOACC_IRQn",         // B0
  "TRNG_IRQn",              // B1
  "PKE_IRQn",               // B2
  "SMU_SECURE_IRQn",        // B3
  "SMU_S_PRIVILEGED_IRQn",  // B4
  "SMU_NS_PRIVILEGED_IRQn", // B5
  "EMU_IRQn",               // B6
  "EMUEFP_IRQn",            // B7
  "DCDC_IRQn",              // B8
  "ETAMPDET_IRQn",          // B9
  "TIMER0_IRQn",            // B10
  "TIMER1_IRQn",            // B11
  "TIMER2_IRQn",            // B12
  "TIMER3_IRQn",            // B13
  "TIMER4_IRQn",            // B14
  "RTCC_IRQn",              // B15
  "USART0_RX_IRQn",         // B16
  "USART0_TX_IRQn",         // B17
  "USART1_RX_IRQn",         // B18
  "USART1_TX_IRQn",         // B19
  "EUSART0_RX_IRQn",        // B20
  "EUSART0_TX_IRQn",        // B21
  "ICACHE0_IRQn",           // B22
  "BURTC_IRQn",             // B23
  "LETIMER0_IRQn",          // B24
  "SYSCFG_IRQn",            // B25
  "LDMA_IRQn",              // B26
  "LFXO_IRQn",              // B27
  "LFRCO_IRQn",             // B28
  "ULFRCO_IRQn",            // B29
  "GPIO_ODD_IRQn",          // B30
  "GPIO_EVEN_IRQn",         // B31
  "I2C0_IRQn",              // B32
  "I2C1_IRQn",              // B33
  "EMUDG_IRQn",             // B34
  "EMUSE_IRQn",             // B35
  "AGC_IRQn",               // B36
  "BUFC_IRQn",              // B37
  "FRC_PRI_IRQn",           // B38
  "FRC_IRQn",               // B39
  "MODEM_IRQn",             // B40
  "PROTIMER_IRQn",          // B41
  "RAC_RSM_IRQn",           // B42
  "RAC_SEQ_IRQn",           // B43
  "RDMAILBOX_IRQn",         // B44
  "RFSENSE_IRQn",           // B45
  "SYNTH_IRQn",             // B46
  "PRORTC_IRQn",            // B47
  "ACMP0_IRQn",             // B48
  "WDOG0_IRQn",             // B49
  "HFXO0_IRQn",             // B50
  "HFRCO0_IRQn",            // B51
  "CMU_IRQn",               // B52
  "AES_IRQn",               // B53
  "IADC_IRQn",              // B54
  "MSC_IRQn",               // B55
  "DPLL0_IRQn",             // B56
  "PDM_IRQn",               // B57
  "SW0_IRQn",               // B58
  "SW1_IRQn",               // B59
  "SW2_IRQn",               // B60
  "SW3_IRQn",               // B61
  "KERNEL0_IRQn",           // B62
  "KERNEL1_IRQn",           // B63
  "M33CTI0_IRQn",           // B64
  "M33CTI1_IRQn",           // B65
  "FPUEXH_IRQn",            // B66
  #endif
#elif defined (CORTEXM3_EFM32_MICRO)
  "DMA",            // B0
  "GPIO_EVEN",      // B1
  "TIMER0",         // B2
  "USART0_RX",      // B3
  "USART0_TX",      // B4
  "USB",            // B5
  "ACMP0",          // B6
  "ADC0",           // B7
  "DAC0",           // B8
  "I2C0",           // B9
  "I2C1",           // B10
  "GPIO_ODD",       // B11
  "TIMER1",         // B12
  "TIMER2",         // B13
  "TIMER3",         // B14
  "USART1_RX",      // B15
  "USART1_TX",      // B16
  "LESENSE",        // B17
  "USART2_RX",      // B18
  "USART2_TX",      // B19
  "UART0_RX",       // B20
  "UART0_TX",       // B21
  "UART1_RX",       // B22
  "UART1_TX",       // B23
  "LEUART0",        // B24
  "LEUART1",        // B25
  "LETIMER0",       // B26
  "PCNT0",          // B27
  "PCNT1",          // B28
  "PCNT2",          // B29
  "RTC",            // B30
  "BURTC",          // B31
  "CMU",            // B32
  "VCMP",           // B33
  "LCD",            // B34
  "MSC",            // B35
  "AES",            // B36
  "EBI",            // B37
  "EMU",            // B38
#else
  "Timer1",       // B0
  "Timer2",       // B1
  "Management",   // B2
  "Baseband",     // B3
  "Sleep_Timer",  // B4
  "SC1",          // B5
  "SC2",          // B6
  "Security",     // B7
  "MAC_Timer",    // B8
  "MAC_TX",       // B9
  "MAC_RX",       // B10
  "ADC",          // B11
  "IRQ_A",        // B12
  "IRQ_B",        // B13
  "IRQ_C",        // B14
  "IRQ_D",        // B15
  "Debug"         // B16
#endif
};

// Names of raw crash data items - each name is null terminated, and the
// end of the array is flagged by two null bytes in a row.
// NOTE: the order of these names must match HalCrashInfoType members.
static const char nameStrings[] = "R0\0R1\0R2\0R3\0"
                                  "R4\0R5\0R6\0R7\0"
                                  "R8\0R9\0R10\0R11\0"
                                  "R12\0R13(LR)\0MSP\0PSP\0"
                                  "PC\0xPSR\0MSP used\0PSP used\0"
                                  "CSTACK bottom\0ICSR\0SHCSR\0INT_ACTIVE0\0"
                                  "INT_ACTIVE1\0"
                                  "CFSR\0HFSR\0DFSR\0MMAR/BFAR\0AFSR\0"
                                  "Ret0\0Ret1\0Ret2\0Ret3\0"
                                  "Ret4\0Ret5\0Dat0\0Dat1\0";

#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

static uint16_t savedResetCause;
static HalAssertInfoType savedAssertInfo;

//------------------------------------------------------------------------------
// Functions

#if defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)

void halPrintCrashData(uint8_t port)
{
  (void)port;
  uint32_t *data = (uint32_t*)&halCrashInfo.R0;
  char const *name = nameStrings;
  char const *separator;
  uint8_t i = 0;

  while (*name != '\0') {
    sl_iostream_printf(SL_IOSTREAM_STDOUT, "%s = %4x", name, *data++);
    // increment pointer to end of name
    while (*name != '\0') {
      name++;
    }
    // increment past null pointer for next name
    name++;

    /*lint -save -e448 */
    separator = ((*name != '\0') && ((i & 3) != 3)) ? ", " : "\r\n";

    /*lint -restore */
    sl_iostream_printf(SL_IOSTREAM_STDOUT, separator);
    i++;
  }
}

void halPrintCrashDetails(uint8_t port)
{
  (void)port;

  HalCrashInfoType *c = &halCrashInfo;
  uint16_t reason = savedResetCause;
  uint8_t bit;
  const uint8_t numFaults = sizeof(cfsrBits) / sizeof(cfsrBits[0]);

  switch (reason) {
    case RESET_WATCHDOG_EXPIRED:
      sl_iostream_printf(SL_IOSTREAM_STDOUT,
                         "Reset cause: Watchdog expired, no reliable extra information\n");
      break;
    case RESET_WATCHDOG_CAUGHT:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Watchdog caught with enhanced info\n");
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Instruction address: %4x\n", c->PC);
      break;
    case RESET_CRASH_ASSERT:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Assert %s:%d\n",
                         c->data.assertInfo.file, c->data.assertInfo.line);
      break;
    case RESET_FAULT_HARD:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Hard Fault\n");
      if (c->hfsr.bits.VECTTBL) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT,
                           "HFSR.VECTTBL: error reading vector table for an exception\n");
      }
      if (c->hfsr.bits.FORCED) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT,
                           "HFSR.FORCED: configurable fault could not activate\n");
      }
      if (c->hfsr.bits.DEBUGEVT) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT,
                           "HFSR.DEBUGEVT: fault related to debug - e.g., executed BKPT\n");
      }
      break;
    case RESET_FAULT_MEM:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Memory Management Fault\n");
      if (c->cfsr.bits.DACCVIOL || c->cfsr.bits.IACCVIOL) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, "Instruction address: %4x\n", c->PC);
      }
      if (c->cfsr.bits.MMARVALID) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, "Illegal access address: %4x\n", c->faultAddress);
      }
      for (bit = SCB_CFSR_MEMFAULTSR_Pos; bit < (SCB_CFSR_MEMFAULTSR_Pos + 8); bit++) {
        if ((c->cfsr.word & (1 << bit)) && (*cfsrBits[bit] != '\0')) {
          sl_iostream_printf(SL_IOSTREAM_STDOUT, "CFSR.%s\n", cfsrBits[bit]);
        }
      }
      break;
    case RESET_FAULT_BUS:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Bus Fault\n");
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Instruction address: %4x\n", c->PC);
      if (c->cfsr.bits.IMPRECISERR) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT,
                           "Address is of an instruction after bus fault occurred, not the cause.\n");
      }
      if (c->cfsr.bits.BFARVALID) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, "Illegal access address: %4x\n",
                           c->faultAddress);
      }
      for (bit = SCB_CFSR_BUSFAULTSR_Pos; bit < SCB_CFSR_USGFAULTSR_Pos; bit++) {
        if (((c->cfsr.word >> bit) & 1U) && (*cfsrBits[bit] != '\0')) {
          sl_iostream_printf(SL_IOSTREAM_STDOUT, "CFSR.%s\n", cfsrBits[bit]);
        }
      }
      if ((c->cfsr.word & 0xFF) == 0) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, "CFSR.(none) load or store at an illegal address\n");
      }
      break;
    case RESET_FAULT_USAGE:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Usage Fault\n");
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Instruction address: %4x\n", c->PC);
      for (bit = SCB_CFSR_USGFAULTSR_Pos;
           (bit < numFaults) && (bit < (sizeof(c->cfsr.word) * 8));
           bit++) {
        if (((c->cfsr.word >> bit) & 1U) && (*cfsrBits[bit] != '\0')) {
          sl_iostream_printf(SL_IOSTREAM_STDOUT, "CFSR.%s\n", cfsrBits[bit]);
        }
      }
      break;
    case RESET_FAULT_DBGMON:
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Reset cause: Debug Monitor Fault\n");
      sl_iostream_printf(SL_IOSTREAM_STDOUT, "Instruction address: %4x\n", c->PC);
      break;
    default:
      break;
  }
}

void halPrintCrashSummary(uint8_t port)
{
  (void)port;

  HalCrashInfoType *c = &halCrashInfo;
  uint32_t sp, stackBegin, stackEnd, size, used;
  uint16_t pct;
  uint8_t *mode;
  const char *stack;
  uint8_t bit;

  if (c->LR & 4) {
    stack = "process";
    sp = c->processSP;
    used = c->processSPUsed;
    stackBegin = 0;
    stackEnd = 0;
  } else {
    stack = "main";
    sp = c->mainSP;
    used = c->mainSPUsed;
    stackBegin = (uint32_t)c->mainStackBottom;
    stackEnd = (uint32_t)(uint8_t *)_CSTACK_SEGMENT_END;
  }

  mode = (uint8_t *)((c->LR & 8) ? "Thread" : "Handler");
  size = stackEnd - stackBegin;
  pct = size ? (uint16_t)(((100 * used) + (size / 2)) / size) : 0;
  sl_iostream_printf(SL_IOSTREAM_STDOUT, "%s mode using %s stack (%4x to %4x), SP = %4x\n",
                     mode, stack, stackBegin, stackEnd, sp);
  sl_iostream_printf(SL_IOSTREAM_STDOUT, "%u bytes used (%u%%) in %s stack (out of %u bytes total)\n",
                     (uint16_t)used, pct, stack, (uint16_t)size);

  // Valid SP range is [stackBegin, stackEnd] inclusive, but contents
  // of stack only go into [stackBegin, stackend).
  if ((sp > stackEnd) || (sp < stackBegin)) {
    sl_iostream_printf(SL_IOSTREAM_STDOUT, "SP is outside %s stack range!\n", stack);
  }

  if (c->intActive.word[0] || c->intActive.word[1]) {
    sl_iostream_printf(SL_IOSTREAM_STDOUT, "Interrupts active (or pre-empted and stacked):\n");
    for (bit = 0; bit < 32; bit++) {
      if ((c->intActive.word[0] & (1 << bit)) && (*intActiveBits[bit] != '\0')) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, " %s", intActiveBits[bit]);
      }
    }
    for (bit = 0; bit < (sizeof(intActiveBits) / sizeof(intActiveBits[0])) - 32; bit++) {
      if ((c->intActive.word[1] & (1 << bit)) && (*intActiveBits[bit + 32] != '\0')) {
        sl_iostream_printf(SL_IOSTREAM_STDOUT, " %s", intActiveBits[bit + 32]);
      }
    }
    sl_iostream_printf(SL_IOSTREAM_STDOUT, "\n");
  } else {
    sl_iostream_printf(SL_IOSTREAM_STDOUT, "No interrupts active\n");
  }
  sl_iostream_printf(SL_IOSTREAM_STDOUT, "\n");
}

#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

void halStartPCDiagnostics(void)
{
}

void halStopPCDiagnostics(void)
{
}

uint16_t halGetPCDiagnostics(void)
{
  return 0;
}

//------------------------------------------------------------------------------

void halInternalClassifyReset(void)
{
  // Table used to convert from RESET_EVENT register bits to reset types
  static const uint16_t resetEventTable[] = {
  #if defined (_SILICON_LABS_32B_SERIES_2)
    RESET_POWERON_HV,                  // bit  0 : POR
    RESET_EXTERNAL_PIN,                // bit  1 : PIN
    RESET_SOFTWARE_EM4,                // bit  2 : EM4
    RESET_WATCHDOG_EXPIRED,            // bit  3 : WDOG0
    RESET_WATCHDOG_EXPIRED,            // bit  4 : WDOG1
    RESET_FATAL_LOCKUP,                // bit  5 : LOCKUP
    RESET_SOFTWARE,                    // bit  6 : SYSREQ
    RESET_BROWNOUT_DVDD,               // bit  7 : DVDDBOD
    RESET_UNKNOWN_UNKNOWN,             // bit  8 : DVDDLEBOD // TODO: make new reset cause?
    RESET_BROWNOUT_DEC,                // bit  9 : DECBOD
    RESET_BROWNOUT_AVDD,               // bit 10 : AVDDBOD
    RESET_UNKNOWN_UNKNOWN,             // bit 11 : IOVDD0BOD // TODO: make new reset cause?
    RESET_UNKNOWN_UNKNOWN,             // bit 12 : RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 13 : TAMPER // TODO: make new reset cause?
    RESET_UNKNOWN_UNKNOWN,             // bit 14 : M0SYSREQ // TODO: make new reset cause?
    RESET_UNKNOWN_UNKNOWN,             // bit 15 : M0LOCKUP // TODO: make new reset cause?
  #elif defined (_EFR_DEVICE)
    RESET_POWERON_HV,                  // bit  0: PORST
    RESET_UNKNOWN_UNKNOWN,             // bit  1: RESERVED
    RESET_BROWNOUT_AVDD,               // bit  2: AVDDBOD
    RESET_BROWNOUT_DVDD,               // bit  3: DVDDBOD
    RESET_BROWNOUT_DEC,                // bit  4: DECBOD
    RESET_UNKNOWN_UNKNOWN,             // bit  5: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit  6: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit  7: RESERVED
    RESET_EXTERNAL_PIN,                // bit  8: EXTRST
    RESET_FATAL_LOCKUP,                // bit  9: LOCKUPRST
    RESET_SOFTWARE,                    // bit 10: SYSREQRST
    RESET_WATCHDOG_EXPIRED,            // bit 11: WDOGRST
    RESET_UNKNOWN_UNKNOWN,             // bit 12: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 13: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 14: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 15: RESERVED
    RESET_SOFTWARE_EM4,                // bit 16: EM4RST
  #endif
  };

  uint32_t resetEvent = RMU_ResetCauseGet();
  RMU_ResetCauseClear();
  uint16_t cause = RESET_UNKNOWN;
  uint16_t i;

  HalResetCauseType *resetCause = (HalResetCauseType*)(RAM_MEM_BASE);

  for (i = 0; i < sizeof(resetEventTable) / sizeof(resetEventTable[0]); i++) {
    if (resetEvent & (1 << i)) {
      cause = resetEventTable[i];
      break;
    }
  }

  if (cause == RESET_SOFTWARE) {
    if ((resetCause->signature == RESET_VALID_SIGNATURE)
        && (RESET_BASE_TYPE(resetCause->reason) < NUM_RESET_BASE_TYPES)) {
      // The extended reset cause is recovered from RAM
      // This can be trusted because the hardware reset event was software
      //  and additionally because the signature is valid
      savedResetCause = resetCause->reason;
    } else {
      savedResetCause = RESET_SOFTWARE_UNKNOWN;
    }
    // mark the signature as invalid
    resetCause->signature = RESET_INVALID_SIGNATURE;
  } else if ((cause == RESET_BOOTLOADER_DEEPSLEEP)
             && (resetCause->signature == RESET_VALID_SIGNATURE)
             && (resetCause->reason == RESET_BOOTLOADER_DEEPSLEEP)) {
    // Save the crash info for bootloader deep sleep (even though it's not used
    // yet) and invalidate the reset signature.
    resetCause->signature = RESET_INVALID_SIGNATURE;
    savedResetCause = resetCause->reason;
  } else {
    savedResetCause = cause;
  }

  // If the last reset was due to an assert, save the assert info.
  if (savedResetCause == RESET_CRASH_ASSERT) {
    savedAssertInfo = halCrashInfo.data.assertInfo;
  }
}

uint16_t halGetExtendedResetInfo(void)
{
  return savedResetCause;
}

const HalAssertInfoType *halGetAssertInfo(void)
{
  return &savedAssertInfo;
}

uint8_t halGetResetInfo(void)
{
  return RESET_BASE_TYPE(savedResetCause);
}

// Translate EM3xx reset codes to the codes previously used by the EM2xx.
// If there is no corresponding code, return the EM3xx base code with bit 7 set.
uint8_t halGetEm2xxResetInfo(void)
{
  uint8_t reset = halGetResetInfo();

  // Any reset with an extended value field of zero is considered an unknown
  // reset, except for FIB resets.
  if ((RESET_EXTENDED_FIELD(halGetExtendedResetInfo()) == 0)
      && (reset != RESET_FIB)) {
    return EM2XX_RESET_UNKNOWN;
  }

  switch (reset) {
    case RESET_UNKNOWN:
      return EM2XX_RESET_UNKNOWN;
    case RESET_BOOTLOADER:
      return EM2XX_RESET_BOOTLOADER;
    case RESET_EXTERNAL:    // map pin resets to poweron for EM2xx compatibility
//    return EM2XX_RESET_EXTERNAL;
    case RESET_POWERON:
      return EM2XX_RESET_POWERON;
    case RESET_WATCHDOG:
      return EM2XX_RESET_WATCHDOG;
    case RESET_SOFTWARE:
      return EM2XX_RESET_SOFTWARE;
    case RESET_CRASH:
      return EM2XX_RESET_ASSERT;
    default:
      return (reset | 0x80);    // set B7 for all other reset codes
  }
}

const char * halGetResetString(void)
{
  // Table used to convert from reset types to reset strings.
  #define RESET_BASE_DEF(basename, value, string)  string,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)     /*nothing*/
  static const char resetStringTable[][4] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF
  uint8_t resetInfo = halGetResetInfo();
  if (resetInfo >= (sizeof(resetStringTable) / sizeof(resetStringTable[0]))) {
    return resetStringTable[0x00];   // return unknown
  } else {
    return resetStringTable[resetInfo];
  }
}

const char * halGetExtendedResetString(void)
{
  // Create a table of reset strings for each extended reset type
  typedef const char ResetStringTableType[][4];
  #define RESET_BASE_DEF(basename, value, string) \
  }; static ResetStringTableType basename##ResetStringTable = {
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  string,
  {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  // Create a table of pointers to each of the above tables
  #define RESET_BASE_DEF(basename, value, string)  (ResetStringTableType *)basename##ResetStringTable,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)     /*nothing*/
  static ResetStringTableType * const extendedResetStringTablePtrs[] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  uint16_t extResetInfo = halGetExtendedResetInfo();
  // access the particular table of extended strings we are interested in
  ResetStringTableType *extendedResetStringTable =
    extendedResetStringTablePtrs[RESET_BASE_TYPE(extResetInfo)];

  // return the string from within the proper table
  return (*extendedResetStringTable)[((extResetInfo) & 0xFF)];
}

#if _SILICON_LABS_GECKO_INTERNAL_SDID == 80

// Workaround for brownouts on Dumbo when DCDC is retimed and radio subsystem is reset
__STATIC_INLINE void disableDcdcRetimingAndRcosync(void)
{
  // Ensure access to EMU registers
  EMU_Unlock();
  EMU_PowerUnlock();

  // Don't need to disable retiming if DCDC is not powering DVDD
  if ((EMU->PWRCFG & _EMU_PWRCFG_PWRCFG_MASK) != EMU_PWRCFG_PWRCFG_DCDCTODVDD) {
    return;
  }

  // Ensure sequencer is halted
  uint32_t clockEnable = *(volatile uint32_t *)(0x400E4000 + 0xC8);
  volatile uint32_t *reg;

  if (clockEnable & 0x4UL) {
    reg = (volatile uint32_t *)(0x40084000UL + 0x40);
    *reg = 0x1UL;
  }

  // If DCDC is in use, ensure retiming and rcosync are disabled
  uint32_t dcdcMode = EMU->DCDCCTRL & _EMU_DCDCCTRL_DCDCMODE_MASK;
  if ((dcdcMode == EMU_DCDCCTRL_DCDCMODE_LOWNOISE)
      || (dcdcMode == EMU_DCDCCTRL_DCDCMODE_LOWPOWER)) {
    BUS_RegBitWrite(&EMU->DCDCTIMING, 28, 0);
    // EMU->DCDCRCOSC is internal, _EMU_DCDCRCOSC_RCOSYNC_SHIFT = 0
    BUS_RegBitWrite((void *)(EMU_BASE + 0x74), 0, 0);
  }
}

#else

// Workaround not needed for dies other than Dumbo
#define disableDcdcRetimingAndRcosync() ((void)0)

#endif

void halInternalSysReset(uint16_t extendedCause)
{
  HalResetCauseType *resetCause = (HalResetCauseType*)(RAM_MEM_BASE);
  INTERRUPTS_OFF();
  // Ensure DCDC settings are compatible with the upcoming radio subsystem reset
  disableDcdcRetimingAndRcosync();

  resetCause->reason = extendedCause;
  resetCause->signature = RESET_VALID_SIGNATURE;
  // force write to complete before reset
  asm ("DMB");
  NVIC_SystemReset();
}

// Cause a usage fault by executing a special UNDEFINED instruction.
// The high byte (0xDE) is reserved to be undefined - the low byte (0x42)
// is arbitrary and distiguishes a failed assert from other usage faults.
// the fault handler with then decode this, grab the filename and linenumber
// parameters from R0 and R1 and save the information for display after a reset
#if defined (__ICCARM__)
#pragma diag_suppress=Og014
static void halInternalAssertFault(const char * filename, int linenumber)
{
  asm ("DC16 0DE42h");
}
#pragma diag_default=Og014
#elif defined (__GNUC__)
__attribute__((noinline))
static void halInternalAssertFault(const char * filename, int linenumber)
{
  asm (".short 0xDE42\n" : : "r" (filename), "r" (linenumber));
}
#endif

void halInternalAssertFailed(const char * filename, int linenumber)
{
#if !defined (_SILICON_LABS_32B_SERIES_2)
  emRadioSleep();
#endif

  halResetWatchdog();              // In case we're close to running out.
  INTERRUPTS_OFF();

#if defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
  sl_iostream_printf(SL_IOSTREAM_STDOUT, "\r\n[ASSERT:%s:%d]\r\n", filename, linenumber);
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

#if defined (__ICCARM__) || defined (__GNUC__)
  // We can use the special fault mechanism to preserve more assert
  // information for display after a crash
  halInternalAssertFault(filename, linenumber);
#else
  // Other toolchains don't handle the inline assembly correctly, so
  // we just call the internal reset
  halCrashInfo.data.assertInfo.file = filename;
  halCrashInfo.data.assertInfo.line = linenumber;
  halInternalSysReset(RESET_CRASH_ASSERT);
#endif
}

// Returns the bytes used in the main stack area.
static uint32_t halInternalGetMainStackBytesUsed(uint32_t *p)
{
  for (; p < (uint32_t *)_CSTACK_SEGMENT_END; p++) {
    if (*p != STACK_FILL_VALUE) {
      break;
    }
  }
  return (uint32_t)((uint8_t *)_CSTACK_SEGMENT_END - (uint8_t *)p);
}

// After the low-level fault handler (in faults.s79) has saved the processor
// registers (R0-R12, LR and both MSP an PSP), it calls halInternalCrashHandler
// to finish saving additional crash data. This function returns the reason for
// the crash to the low-level fault handler that then calls
// halInternalSystsemReset() to reset the processor.
uint16_t halInternalCrashHandler(void)
{
  uint32_t activeException;
  uint16_t reason = (uint16_t)RESET_FAULT_UNKNOWN;
  HalCrashInfoType *c = &halCrashInfo;
  uint8_t i, j;
  uint32_t *sp, *s, *sEnd, *stackBottom, *stackTop;
  uint32_t data;

  c->icsr.word = SCB->ICSR;
  c->shcsr.word = SCB->SHCSR;

  c->intActive.word[0] = NVIC->IABR[0];
  c->intActive.word[1] = NVIC->IABR[1];

  c->cfsr.word = SCB->CFSR;
  c->hfsr.word = SCB->HFSR;
  c->dfsr.word = SCB->DFSR;
  c->faultAddress = SCB->MMFAR;
  c->afsr.word = SCB->AFSR;

  // Examine B2 of the saved LR to know the stack in use when the fault occurred
  sp = (uint32_t *)(((c->LR & 4U) != 0U) ? c->processSP : c->mainSP);
  sEnd = sp; // Keep a copy around for walking the stack later

  // Get the bottom of the stack since we allow stack resizing
  c->mainStackBottom = (uint32_t)_CSTACK_SEGMENT_BEGIN;

  // If we're running FreeRTOS and this is a process stack then add
  // extra diagnostic information
  if ((freeRTOS != 0) && ((c->LR & 4U) != 0U)) {
    stackBottom = sp;
    stackTop = sp + 8;
    c->processSPUsed = stackTop - sp;
  } else {
    stackBottom = (uint32_t*)c->mainStackBottom;
    stackTop = (uint32_t*)_CSTACK_SEGMENT_END;
    c->processSPUsed = 0;   // process stack not in use
  }

  // If the stack pointer is valid, read and save the stacked PC and xPSR
  if ((sp >= stackBottom)
      && ((sp + 8) <= stackTop)) {
    sp += 6; // Skip over R0,R1,R2,R3,R12,LR
    c->PC = *sp++;
    c->xPSR.word = *sp++;

    // See if fault was due to a failed assert. This is indicated by
    // a usage fault caused by executing a reserved instruction.
    if ( c->icsr.bits.VECTACTIVE == USAGE_FAULT_VECTOR_INDEX
         && ((uint16_t *)c->PC >= (uint16_t *)_TEXT_SEGMENT_BEGIN)
         && ((uint16_t *)c->PC < (uint16_t *)_TEXT_SEGMENT_END)
         && *(uint16_t *)(c->PC) == ASSERT_USAGE_OPCODE ) {
      // Copy halInternalAssertFailed() arguments into data member specific
      // to asserts.
      c->data.assertInfo.file = (const char *)c->R0;
      c->data.assertInfo.line = c->R1;
#ifdef PUSH_REGS_BEFORE_ASSERT
      // Just before calling halInternalAssertFailed(), R0, R1, R2 and LR were
      // pushed onto the stack - copy these values into the crash data struct.
      c->R0 = *sp++;
      c->R1 = *sp++;
      c->R2 = *sp++;
      c->LR = *sp++;
#endif
      reason = (uint16_t)RESET_CRASH_ASSERT;
    }
    // If a bad stack pointer, PC and xPSR to 0 to indicate they are not known.
  } else {
    c->PC = 0;
    c->xPSR.word = 0;
    sEnd = stackBottom;
  }

  c->mainSPUsed = halInternalGetMainStackBytesUsed((uint32_t*)c->mainStackBottom);

  for (i = 0; i < NUM_RETURNS; i++) {
    c->returns[i] = 0;
  }

  // Search the stack downward for probable return addresses. A probable
  // return address is a value in the CODE segment that also has bit 0 set
  // (since we're in Thumb mode).
  i = 0U;
  s = stackTop;
  while (s > sEnd) {
    data = *(--s);
    if (((uint16_t *)data >= (uint16_t *)_TEXT_SEGMENT_BEGIN)
        && ((uint16_t *)data < (uint16_t *)_TEXT_SEGMENT_END)
        && ((data & 1U) != 0U)) {
      // Only record the first occurrence of a return - other copies could
      // have been in registers that then were pushed.
      for (j = 0; j < NUM_RETURNS; j++) {
        if (c->returns[j] == data) {
          break;
        }
      }
      if (j != NUM_RETURNS) {
        continue;
      }
      // Save the return in the returns array managed as a circular buffer.
      // This keeps only the last NUM_RETURNS in the event that there are more.
      i = (i != 0U) ? i - 1U : NUM_RETURNS - 1U;
      c->returns[i] = data;
    }
  }
  // Shuffle the returns array so returns[0] has last probable return found.
  // If there were fewer than NUM_RETURNS, unused entries will contain zero.
  while ((i--) != 0U) {
    data = c->returns[0];
    for (j = 0; j < NUM_RETURNS - 1U; j++ ) {
      c->returns[j] = c->returns[j + 1U];
    }
    c->returns[NUM_RETURNS - 1U] = data;
  }

  // Read the highest priority active exception to get reason for fault
  activeException = c->icsr.bits.VECTACTIVE;
  switch (activeException) {
    #if defined(WDOG_IF_WARN) && !defined(BOOTLOADER)
    case IRQ_TO_VECTOR_NUMBER(WDOG0_IRQn):
      if (WDOG0->IF & WDOG_IF_WARN) {
        reason = RESET_WATCHDOG_CAUGHT;
      }
      break;
    #endif
    case HARD_FAULT_VECTOR_INDEX:
      reason = (uint16_t)RESET_FAULT_HARD;
      break;
    case MEMORY_FAULT_VECTOR_INDEX:
      reason = (uint16_t)RESET_FAULT_MEM;
      break;
    case BUS_FAULT_VECTOR_INDEX:
      reason = (uint16_t)RESET_FAULT_BUS;
      break;
    case USAGE_FAULT_VECTOR_INDEX:
      // make sure we didn't already identify the usage fault as an assert
      if (reason == (uint16_t)RESET_FAULT_UNKNOWN) {
        reason = (uint16_t)RESET_FAULT_USAGE;
      }
      break;
    case DEBUG_MONITOR_VECTOR_INDEX:
      reason = (uint16_t)RESET_FAULT_DBGMON;
      break;
    default:
      if ((activeException != 0U) && (activeException < VECTOR_TABLE_LENGTH)) {
        reason = (uint16_t)RESET_FAULT_BADVECTOR;
      }
      break;
  }
  return reason;
}
