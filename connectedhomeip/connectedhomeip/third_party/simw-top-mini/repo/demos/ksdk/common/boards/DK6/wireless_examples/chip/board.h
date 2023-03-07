/*
* Copyright 2019-2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _BOARD_H_
#define _BOARD_H_
#include "clock_config.h"
#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "psector_api.h"
#include "board_utility.h"
#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
#include "MWS.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#ifndef BOARD_NAME
#define BOARD_NAME                      "DK6"
#endif
/*! @brief The manufacturer name */
#define MANUFACTURER_NAME               "NXP"
#ifndef gUartDebugConsole_d
#define gUartDebugConsole_d          1
#endif
#ifndef gUartAppConsole_d
#define gUartAppConsole_d            1
#endif

/* There are 2 red LEDs on USB Dongle: PIO4 and PIO10 */
#define BOARD_LED_USB_DONGLE_GPIO GPIO
#define BOARD_LED_USB_DONGLE_GPIO_PORT 0U
#define BOARD_LED_USB_DONGLE1_GPIO_PIN 4U
#define BOARD_LED_USB_DONGLE2_GPIO_PIN 10U


/* Connectivity */
#ifndef APP_SERIAL_INTERFACE_TYPE
#define APP_SERIAL_INTERFACE_TYPE       (gSerialMgrUsart_c)
#endif

#ifndef APP_SERIAL_INTERFACE_INSTANCE
#define APP_SERIAL_INTERFACE_INSTANCE   0
#else
#if defined DEBUG_SERIAL_INTERFACE_INSTANCE
#if DEBUG_SERIAL_INTERFACE_INSTANCE == APP_SERIAL_INTERFACE_INSTANCE
#error "Debug and App Uarts cannot be the same"
#endif
#endif
#endif

#ifndef DEBUG_SERIAL_INTERFACE_INSTANCE
#define DEBUG_SERIAL_INTERFACE_INSTANCE 1
#else
#if defined APP_SERIAL_INTERFACE_INSTANCE
#if DEBUG_SERIAL_INTERFACE_INSTANCE == APP_SERIAL_INTERFACE_INSTANCE
#error "Debug and App Uarts cannot be the same"
#endif
#endif
#endif

#ifndef APP_SERIAL_INTERFACE_SPEED
#define APP_SERIAL_INTERFACE_SPEED      (115200U)
#endif


#define IOCON_QSPI_MODE_FUNC        (7U)

#define IOCON_USART0_TX_PIN         (8U)
#define IOCON_USART0_RX_PIN         (9U)

#define IOCON_USART1_TX_PIN         (10U)
#define IOCON_USART1_RX_PIN         (11U)

#define IOCON_SWCLK_PIN             (12U)
#define IOCON_SWDIO_PIN             (13U)
#define IOCON_SWD_MODE_FUNC         (2U)   /* no choice for SWD */

#define IOCON_SPIFI_CS_PIN          (16U)
#define IOCON_SPIFI_CLK_PIN         (18U)
#define IOCON_SPIFI_IO0_PIN         (19U)
#define IOCON_SPIFI_IO1_PIN         (21U)
#define IOCON_SPIFI_IO2_PIN         (20U)
#define IOCON_SPIFI_IO3_PIN         (17U)
#define IOCON_SPIFI_MODE_FUNC       (7U)


#ifdef gSpiFiHiPerfMode_d
#define BOARD_SPIFI_CLK_RATE        (32000000UL)
#else
#define BOARD_SPIFI_CLK_RATE        (16000000UL)
#endif
/* Select flash to use */
#if gOTA_externalFlash_d == 1
#define gEepromType_d               gEepromDevice_MX25R8035F_c
#else
#define gEepromType_d               gEepromDevice_InternalFlash_c
#endif

#define BOARD_USART_IRQ(x)              USART##x_IRQn
#define BOARD_USART_IRQ_HANDLER(x)      USART##x_IRQHandler
#define BOARD_UART_BASEADDR(x)          (uint32_t)USART##x
#define BOARD_UART_RESET_PERIPH(x)      kUSART##x_RST_SHIFT_RSTn
#define BOARD_UART_RX_PIN(x)            IOCON_USART##x_RX_PIN
#define BOARD_UART_TX_PIN(x)            IOCON_USART##x_TX_PIN

#if gUartAppConsole_d
#if APP_SERIAL_INTERFACE_INSTANCE == 1
#define BOARD_APP_UART_IRQ              BOARD_USART_IRQ(1)
#define BOARD_APP_UART_IRQ_HANDLER      BOARD_USART_IRQ_HANDLER(1)
#define BOARD_APP_UART_BASEADDR         BOARD_UART_BASEADDR(1)
#define BOARD_APP_UART_RESET            kUSART1_RST_SHIFT_RSTn
#define BOARD_APP_UART_RX_PIN           IOCON_USART1_RX_PIN
#define BOARD_APP_UART_TX_PIN           IOCON_USART1_TX_PIN
#else
#define BOARD_APP_UART_IRQ              BOARD_USART_IRQ(0)
#define BOARD_APP_UART_IRQ_HANDLER      BOARD_USART_IRQ_HANDLER(0)
#define BOARD_APP_UART_BASEADDR         BOARD_UART_BASEADDR(0)
#define BOARD_APP_UART_RESET            kUSART0_RST_SHIFT_RSTn
#define BOARD_APP_UART_RX_PIN           IOCON_USART0_RX_PIN
#define BOARD_APP_UART_TX_PIN           IOCON_USART0_TX_PIN
#endif
#define BOARD_APP_UART_CLK_ATTACH       kOSC32M_to_USART_CLK
#define BOARD_APP_UART_CLK_FREQ         CLOCK_GetFreq(kCLOCK_Fro32M)
#endif



#if gUartDebugConsole_d

#if DEBUG_SERIAL_INTERFACE_INSTANCE == 1
#define BOARD_DEBUG_UART_IRQ            BOARD_USART_IRQ(1)
#define BOARD_DEBUG_UART_IRQ_HANDLER    BOARD_USART_IRQ_HANDLER(1)
#define BOARD_DEBUG_UART_BASEADDR       BOARD_UART_BASEADDR(1)
#define BOARD_DEBUG_UART_RESET          kUSART1_RST_SHIFT_RSTn
#define BOARD_DEBUG_UART_RX_PIN         IOCON_USART1_RX_PIN
#define BOARD_DEBUG_UART_TX_PIN         IOCON_USART1_TX_PIN
#else
#define BOARD_DEBUG_UART_IRQ            BOARD_USART_IRQ(0)
#define BOARD_DEBUG_UART_IRQ_HANDLER    BOARD_USART_IRQ_HANDLER(0)
#define BOARD_DEBUG_UART_BASEADDR       BOARD_UART_BASEADDR(0)
#define BOARD_DEBUG_UART_RESET          kUSART0_RST_SHIFT_RSTn
#define BOARD_DEBUG_UART_RX_PIN         IOCON_USART0_RX_PIN
#define BOARD_DEBUG_UART_TX_PIN         IOCON_USART0_TX_PIN
#endif

#define BOARD_DEBUG_UART_CLK_ATTACH     kOSC32M_to_USART_CLK
#define BOARD_DEBUG_UART_CLK_FREQ       CLOCK_GetFreq(kCLOCK_Fro32M)

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE           kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE       115200U
#endif

//#define BOARD_DIAG_PORT_MODE           (0x00840083)
//#define BOARD_DIAG_PORT_MODE           (0x9F8B8783)
//#define BOARD_DIAG_PORT_MODE           (0x00000083)
#ifndef BOARD_DIAG_PORT_MODE
#define BOARD_DIAG_PORT_MODE 0
#endif

#define gDbgIoCfg_c                       0 /* Dbg Io forbidden */
//#define gDbgIoCfg_c                     1 /* For Low Power Dbg Io use */
//#define gDbgIoCfg_c                     2 /* For General Purpose Dbg Io use */

#if gDbgUseSramIssueDetector
#undef gDbgIoCfg_c
#define gDbgIoCfg_c                       3
#endif

/* Battery voltage level pin for ADC0 */
#define gADC0BatLevelInputPin	             (14)

/* when Diag Port is enabled (gDbgUseLLDiagPort set to 1), define the mode to use */
/* Use IOs API for Debugging BOARD_DbgSetIoUp() and BOARD_DbgSetIoUp() in board.h */
#define gDbgUseDbgIos                    (gDbgIoCfg_c != 0)

/* Enable Link Layer Diag Port - enable BOARD_DbgDiagIoConf() and BOARD_DbgDiagEnable() API in board.h */
#ifndef BIT
#define BIT(x) (1<<(x))
#endif
#define gDbgLLDiagPort0Msk      (BIT(7)|BIT(23))
#define gDbgLLDiagPort1Msk      (BIT(15)|BIT(31))
#define gDbgUseLLDiagPort       (BOARD_DIAG_PORT_MODE & (gDbgLLDiagPort0Msk|gDbgLLDiagPort1Msk))


/* Bluetooth MAC adress size */
#define BD_ADDR_SIZE                    6

#define BOARD_MAINCLK_FRO12M            0U
#define BOARD_MAINCLK_XTAL32M           2U
#define BOARD_MAINCLK_FRO32M            3U
#define BOARD_MAINCLK_FRO48M            4U

#if gPWR_CpuClk_48MHz
#define BOARD_TARGET_CPU_FREQ           BOARD_MAINCLK_FRO48M
#else
#define BOARD_TARGET_CPU_FREQ           BOARD_MAINCLK_XTAL32M
//#define BOARD_TARGET_CPU_FREQ           BOARD_MAINCLK_FRO32M
#endif

/* Capacitance values for 32MHz. Actual values for all of
 * these should be defined in board.h for systems where crystal accuracy is
 * vital. Adding default values here for builds that do not need this. Value
 * is pF x 100. For example, 6pF becomes 600, 1.2pF becomes 120 */
#ifndef CLOCK_32MfXtalIecLoadpF_x100
#define CLOCK_32MfXtalIecLoadpF_x100    (600)
#endif
#ifndef CLOCK_32MfXtalPPcbParCappF_x100
#define CLOCK_32MfXtalPPcbParCappF_x100 (10)
#endif
#ifndef CLOCK_32MfXtalNPcbParCappF_x100
#define CLOCK_32MfXtalNPcbParCappF_x100 (5)
#endif

#if gClkUseFro32K
#define CLOCK_32k_source kCLOCK_Fro32k
#else
#define CLOCK_32k_source kCLOCK_Xtal32k
#endif

#if gLoggingActive_d
#define RAM2_BASE 0x04020000
#define RAM2_SIZE (64*1024)
#endif

/* gAdvertisingPowerLeveldBm_c and gConnectPowerLeveldBm_c defualt values if not defiend otherwise
 * Valid values are in the range [-30, 10] nonetheless [-4, 3] is a reasonable range
 * e.g.  DBM(-3) for -3dBm
 * */
#define DBM(x) ((uint8_t)(x))

#ifndef gAdvertisingPowerLeveldBm_c
#define gAdvertisingPowerLeveldBm_c  DBM(0)
#endif
#ifndef gConnectPowerLeveldBm_c
#define gConnectPowerLeveldBm_c      DBM(0)
#endif

/* Capacitance values for 32kHz crystals; board-specific. Value is
   pF x 100. For example, 6pF becomes 600, 1.2pF becomes 120 */
#define CLOCK_32kfXtalIecLoadpF_x100    (600)  /* 6.0pF IEC load capacitance */
#define CLOCK_32kfXtalPPcbParCappF_x100 (300)  /* 3.0pF P PCB Parasitic capacitance*/
#define CLOCK_32kfXtalNPcbParCappF_x100 (250)  /* 2.5pF N PCB Parasitic capacitance*/

#ifndef CLOCK_32kfXtalIecLoadpF_x100
#define CLOCK_32kfXtalIecLoadpF_x100    (600)
#endif
#ifndef CLOCK_32kfXtalPPcbParCappF_x100
#define CLOCK_32kfXtalPPcbParCappF_x100 (300)
#endif
#ifndef CLOCK_32kfXtalNPcbParCappF_x100
#define CLOCK_32kfXtalNPcbParCappF_x100 (250)
#endif

 /* Set gTcxo32M_ModeEn_c if you wish to activate the 32M Xtal trimming vs temperature.
 * Set gTcxo32k_ModeEn_c if you wish to activate the 32k Xtal trimming vs temperature.
 * dk6 boards are equipped with a CMOS040LP 32 MHz ultra low power DCXO (Digital Controlled Xtal Oscillator).
 * A temperature sweep has been performed from -40'C to +125'C in 5'C step and the frequency accuracy
 * has been recorded using a frequency meter for a fixed capbank code corresponding to 6 pF IEC load,
 * and the optimum IEC load giving the lowest frequency error obtained thanks to a dichotomy-based algorithm.
 * The PCB parasitic capacitors have been taken into account and specified in the C header file of the SW API
 * XTAL Reference: NDK NX2016SA 32MHz EXS00A-CS11213-6(IEC)
 *                 NDK NX2012SA 32.768kHz EXS00A-MU01089-6(IEC)
 * As a result CLOCK_ai32MXtalIecLoadFfVsTemp is setup to compensate IEC Load vs temperature for Xtal 32MHz.
 *
 * Similarly CLOCK_ai32kXtalIecLoadFfVsTemp has to be put together for the 32k Xtal.
 * The 32k compensation turns out inefficient outside the -40'C to +80'C range.
 *
 *
 *
 **/
#define gTcxo32M_ModeEn_c                     (1)
#define gXo32M_Trim_c                          (1 || gTcxo32M_ModeEn_c)

/* Xtal 32kHz temperature compensation is disabled because table is *not* correct: values populating the temperature
 * compensation array below are just for example TODO */
#define gTcxo32k_ModeEn_c                     (0)
/* 32k not temperature compensated but ATE trimming used */
#if gTcxo32k_ModeEn_c
#define gXo32k_Trim_c                         (1)
#else
#define gXo32k_Trim_c                         (0)
#endif

#define TEMP_ZERO_K -273
#define TEMP_ZERO_K_128th_DEG (TEMP_ZERO_K*128)

#define gAdcUsed_d           1

#define ABSOLUTE_VALUE(x) (((x) < 0) ? -(x) : (x))

#if gAdcUsed_d

/* Full scale voltage of ADC0 is 3600 mV */
#define gAdc0FullRangeVoltage                    (3600)

/* Full voltage value of battery is 3300 mV */
#define gBatteryFullVoltage                       (3300)

/* Resolution of ADC0 */
#define gAdc0MaxResolution                        (12)

/* Convert ADC0 output to voltage in mV */
#define ADC_TO_MV(x)                            (((x) * gAdc0FullRangeVoltage) >> gAdc0MaxResolution)

/* Compute voltage percentage */
#define ADC_MV_TO_PERCENT(x)                    (((x) * 100) / gBatteryFullVoltage)

/* Temperature sensor channel of ADC */
#define ADC_TEMPERATURE_SENSOR_CHANNEL             7U

/* Battery level input channel of ADC */
#define ADC_BAT_LEVEL_CHANNEL                     0x06

/* Temperature sensor driver code enable */
#define ADC_TEMP_SENSOR_DRIVER_EN               1

/* ADC initiate time */
#define ADC_WAIT_TIME_US                           300
#else
#define ADC_TEMP_SENSOR_DRIVER_EN               0
#endif

#if defined gLoggingActive_d && (gLoggingActive_d > 0)

#ifndef DBG_ADC
#define DBG_ADC 0
#endif

#ifndef DBG_INIT
#define DBG_INIT 0
#endif

#define ADC_DBG_LOG(fmt, ...)   if (DBG_ADC) do { DbgLogAdd(__FUNCTION__ , fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); } while (0);
#define INIT_DBG_LOG(fmt, ...)   if (DBG_INIT) do { DbgLogAdd(__FUNCTION__ , fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); } while (0);

#else

#define ADC_DBG_LOG(...)
#define INIT_DBG_LOG(...)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
//extern flash_config_t gFlashConfig;

#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
void BOARD_GetCoexIoCfg(void **rfDeny, void **rfActive, void **rfStatus);
#endif

status_t BOARD_InitDebugConsole(void);
status_t BOARD_DeinitDebugConsole(void);

/* Function to initialize/deinitialize ADC on board configuration. */
void BOARD_InitAdc(void);
void BOARD_EnableAdc(void);
void BOARD_DeInitAdc(void);

/* Function to read battery level on board configuration. */
uint8_t BOARD_GetBatteryLevel(void);

int32_t BOARD_GetTemperature(void);

/* Function called by the BLE connection manager to generate PER MCU keys */
extern void BOARD_GetMCUUid(uint8_t* aOutUid16B, uint8_t* pOutLen);

/* Function called to get the USART Clock in Hz */
extern uint32_t BOARD_GetUsartClock(int8_t instance);

/* Function called to get the CTIMER clock in Hz */
extern uint32_t BOARD_GetCtimerClock(CTIMER_Type *timer);

extern void BOARD_UnInitButtons(void);

extern void BOARD_InitFlash(void);

extern uint16_t BOARD_GetPotentiometerLevel(void);

extern void BOARD_SetFaultBehaviour(void);

extern uint32_t BOARD_GetSpiClock(uint32_t instance);
extern void BOARD_InitPMod_SPI_I2C( void );
extern void BOARD_InitSPI( void );

extern void BOARD_InitSPIFI(void);
extern void BOARD_SetSpiFi_LowPowerEnter(void);

/* For debug only */
extern void BOARD_DbgDiagIoConf(void);
extern void BOARD_DbgDiagEnable();
extern void BOARD_InitDbgIo(void);
extern void BOARD_DbgIoSet(int pinid, int val);

/* Passivate outputs used for driving LEDs */
extern void BOARD_SetLEDs_LowPower(void);

/* Perform preparation to let inputs acquire wake capability */
extern void BOARD_SetButtons_LowPowerEnter(void);

extern void BOARD_SetDEBUG_UART_LowPower(void);
extern status_t BOARD_DeinitDebugConsole(void);
extern void BOARD_SetSpiFiPinForLp(void);


/* Switch to target frequency */
extern void BOARD_CpuClockUpdate(void);

extern void BOARD_common_hw_init(void);

/* Function called to initial the pins */
extern void BOARD_InitDEBUG_UART(void);
extern void BOARD_InitHostInterface(void);
extern void BOARD_InitPins(void);
extern void BOARD_SetPinsForPowerMode(void);

extern bool BOARD_IsADCEnabled(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
