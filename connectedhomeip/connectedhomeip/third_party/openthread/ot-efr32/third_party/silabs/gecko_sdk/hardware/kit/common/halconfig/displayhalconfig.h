#ifndef DISPLAYHALCONFIG_H
#define DISPLAYHALCONFIG_H

#include "hal-config.h"

// -----------------------------------------------------------------------------
// Peripheral/pin configuration

#if BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART0
// USART0
  #define PAL_SPI_USART_UNIT            USART0
  #define PAL_SPI_USART_INDEX           0
  #define PAL_SPI_USART_CLOCK           cmuClock_USART0
#elif BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART1
// USART1
  #define PAL_SPI_USART_UNIT            USART1
  #define PAL_SPI_USART_INDEX           1
  #define PAL_SPI_USART_CLOCK           cmuClock_USART1
#elif BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART2
// USART2
  #define PAL_SPI_USART_UNIT            USART2
  #define PAL_SPI_USART_INDEX           2
  #define PAL_SPI_USART_CLOCK           cmuClock_USART2
#elif BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART3
// USART3
  #define PAL_SPI_USART_UNIT            USART3
  #define PAL_SPI_USART_INDEX           3
  #define PAL_SPI_USART_CLOCK           cmuClock_USART3
#elif BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART4
// USART4
  #define PAL_SPI_USART_UNIT            USART4
  #define PAL_SPI_USART_INDEX           4
  #define PAL_SPI_USART_CLOCK           cmuClock_USART4
#elif BSP_SPIDISPLAY_USART == HAL_SPI_PORT_USART5
// USART5
  #define PAL_SPI_USART_UNIT            USART5
  #define PAL_SPI_USART_INDEX           5
  #define PAL_SPI_USART_CLOCK           cmuClock_USART5
#else
  #error "Display config: Unknown USART selection"
#endif

#define LCD_PORT_SI                   BSP_SPIDISPLAY_MOSI_PORT
#define LCD_PIN_SI                    BSP_SPIDISPLAY_MOSI_PIN
#define PAL_SPI_USART_LOCATION_TX     BSP_SPIDISPLAY_MOSI_LOC
#define LCD_PORT_SCLK                 BSP_SPIDISPLAY_CLK_PORT
#define LCD_PIN_SCLK                  BSP_SPIDISPLAY_CLK_PIN
#define PAL_SPI_USART_LOCATION_SCLK   BSP_SPIDISPLAY_CLK_LOC
#define LCD_PORT_SCS                  BSP_SPIDISPLAY_CS_PORT
#define LCD_PIN_SCS                   BSP_SPIDISPLAY_CS_PIN

#define PAL_SPI_BAUDRATE              HAL_SPIDISPLAY_FREQUENCY

#if defined(BSP_SPIDISPLAY_ENABLE_PORT)
// Use power/enable pin
  #define LCD_PORT_DISP_PWR           BSP_SPIDISPLAY_ENABLE_PORT
  #define LCD_PIN_DISP_PWR            BSP_SPIDISPLAY_ENABLE_PIN
#endif

#if defined(BSP_SPIDISPLAY_EXTMODE_PORT)
// Use EXTMODE pin to toggle between SPI and EXTCOMIN polarity inversion
  #define LCD_PORT_EXTMODE            BSP_SPIDISPLAY_EXTMODE_PORT
  #define LCD_PIN_EXTMODE             BSP_SPIDISPLAY_EXTMODE_PIN
#endif

#if !defined(RTCC_PRESENT) && !defined(RTC_PRESENT) && defined(BURTC_PRESENT)
  #if HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO
    #define PAL_BURTC_CLOCK_LFXO
  #elif HAL_CLK_LFCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO
    #define PAL_BURTC_CLOCK_ULFRCO
  #else
    #define PAL_BURTC_CLOCK_LFRCO
  #endif
#endif
#if defined(RTCC_PRESENT)
  #if HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO
    #define PAL_RTCC_CLOCK_LFXO
  #elif HAL_CLK_LFCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO
    #define PAL_RTCC_CLOCK_ULFRCO
  #else
    #define PAL_RTCC_CLOCK_LFRCO
  #endif
#endif
#if defined(RTC_PRESENT)
  #if HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO
    #define PAL_RTC_CLOCK_LFXO
  #elif HAL_CLK_LFCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO
    #define PAL_RTC_CLOCK_ULFRCO
  #else
    #define PAL_RTC_CLOCK_LFRCO
  #endif
#endif

#if HAL_SPIDISPLAY_EXTMODE_SPI
// --------------------------------
// EXTMODE is LOW, use SPI command for polarity inversion
#elif HAL_SPIDISPLAY_EXTMODE_EXTCOMIN
// --------------------------------
// EXTMODE is HIGH, use EXTCOMIN pin for polarity inversion

// Configure DISPLAY driver for EXTCOMIN
  #define POLARITY_INVERSION_EXTCOMIN

// Use hardware polarity inversion pin
  #define LCD_PORT_EXTCOMIN               BSP_SPIDISPLAY_EXTCOMIN_PORT
  #define LCD_PIN_EXTCOMIN                BSP_SPIDISPLAY_EXTCOMIN_PIN

  #if HAL_SPIDISPLAY_EXTCOMIN_USE_CALLBACK
// Someone else owns the RTC(C), register a callback for EXTCOMIN toggling
    #define PAL_TIMER_REPEAT_FUNCTION  (HAL_SPIDISPLAY_EXTCOMIN_CALLBACK)
extern int HAL_SPIDISPLAY_EXTCOMIN_CALLBACK (void(*pFunction)(void*),
                                             void* argument,
                                             unsigned int frequency);
  #else
// The DISPLAY PAL owns the RTC(C), and should auto-toggle the EXTCOMIN pin
    #define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE

    #if HAL_SPIDISPLAY_EXTCOMIN_USE_PRS
// Perform polarity inversion autonomously using RTC output on PRS
      #define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
      #define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE

      #define LCD_AUTO_TOGGLE_PRS_CH        BSP_SPIDISPLAY_EXTCOMIN_CHANNEL

      #if BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 4
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC0
      #elif BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 8
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC1
      #elif BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 12
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC2
      #elif BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 16
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC3
      #elif BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 20
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC4
      #elif BSP_SPIDISPLAY_EXTCOMIN_CHANNEL < 24
        #define BSP_SPIDISPLAY_ROUTELOC         PRS->ROUTELOC5
      #else
        #error "Invalid SPIDISPLAY PRS channel"
      #endif

// *INDENT-OFF*
      #define LCD_AUTO_TOGGLE_PRS_ROUTELOC() BSP_SPIDISPLAY_ROUTELOC = \
        (BSP_SPIDISPLAY_ROUTELOC & ~0xFFU << ((BSP_SPIDISPLAY_EXTCOMIN_CHANNEL % 4U) * 8U)) \
        | (BSP_SPIDISPLAY_EXTCOMIN_LOC << ((BSP_SPIDISPLAY_EXTCOMIN_CHANNEL % 4U) * 8U))
// *INDENT-ON*

      #define LCD_AUTO_TOGGLE_PRS_ROUTEPEN    (1 << LCD_AUTO_TOGGLE_PRS_CH)
    #endif // HAL_SPIDISPLAY_EXTCOMIN_USE_PRS
  #endif // HAL_SPIDISPLAY_EXTCOMIN_USE_CALLBACK
#else
// --------------------------------
// No EXTMODE setting, assume manual EXTCOMIN control
  #define POLARITY_INVERSION_EXTCOMIN
  #define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE
  #define POLARITY_INVERSION_EXTCOMIN_MANUAL
#endif // HAL_SPIDISPLAY_EXTMODE_SPI

// -----------------------------------------------------------------------------
// Display configuration

/**
 * Maximum number of display devices the display module is configured
 * to support. This number may be increased if the system includes more than
 * one display device. However, the number should be kept low in order to
 * save memory.
 */
#define DISPLAY_DEVICES_MAX   (1)

#if BSP_SPIDISPLAY_DISPLAY == HAL_DISPLAY_SHARP_LS013B7DH03
  #define SHARP_MEMLCD_DEVICE_NAME   "Sharp LS013B7DH03 #1"
  #define LS013B7DH03_POLARITY_INVERSION_FREQUENCY (64)

/**
 * Geometry of display device #0 in the system (i.e. ls013b7dh03 on the WSTK)
 * These defines can be used to declare static framebuffers in order to save
 * extra memory consumed by malloc.
 */
  #include "displayls013b7dh03.h"
  #define DISPLAY0_WIDTH    (LS013B7DH03_WIDTH)
  #define DISPLAY0_HEIGHT   (LS013B7DH03_HEIGHT)
/**
 * Define all display device driver initialization functions here.
 */
  #define DISPLAY_DEVICE_DRIVER_INIT_FUNCTIONS \
  {                                            \
    DISPLAY_Ls013b7dh03Init,                   \
    NULL                                       \
  }
#elif BSP_SPIDISPLAY_DISPLAY == HAL_DISPLAY_SHARP_LS013B7DH06
  #define SHARP_MEMLCD_DEVICE_NAME   "Sharp LS013B7DH06 #1"
  #define LS013B7DH06_POLARITY_INVERSION_FREQUENCY (64)

/* Define time for power supply to ramp after enabled */
  #define SHARP_MEMLCD_POWER_RAMP_US    100

/** Display color mode */
  #define DISPLAY_COLOUR_MODE_IS_RGB_3BIT

/**
 * Geometry of display device #0 in the system (i.e. ls013b7dh06 on the STK)
 * These defines can be used to declare static framebuffers in order to save
 * extra memory consumed by malloc.
 */
  #include "displayls013b7dh06.h"
  #define DISPLAY0_WIDTH          (LS013B7DH06_WIDTH)
  #define DISPLAY0_HEIGHT         (LS013B7DH06_HEIGHT)
  #define DISPLAY0_BITS_PER_PIXEL (LS013B7DH06_BITS_PER_PIXEL)
/**
 * Define all display device driver initialization functions here.
 */
  #define DISPLAY_DEVICE_DRIVER_INIT_FUNCTIONS \
  {                                            \
    DISPLAY_Ls013b7dh06Init,                   \
    NULL                                       \
  }
#endif

#include "displayconfigapp.h"

#endif // DISPLAYHALCONFIG_H
