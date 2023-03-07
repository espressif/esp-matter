/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef CSLIB_HWCONFIG_H
#define CSLIB_HWCONFIG_H

#include "em_lesense.h"
#include "em_gpio.h"

/**************************************************************************//**
 *
 * @addtogroup cslib_HWconfig CSLIB library hardware configuration
 * @{
 *
 * @brief Hardware configuration for CSLIB
 *
 * These definitions configure the capacitive sensing hardware in
 * active and sleep mode scans.  They also configure channel-by-channel
 * thresholds and expected touchd deltas.
 *
 *****************************************************************************/

/// @brief Defines the size of the sensor node array.
/// Also defines volatile arrays that have a one-to-one correspondence
/// to the number of sensors in the project.
/// @note Minimum value is 1, maximum value is the number of capacitive
/// sensing-enabled pins on the device
#define DEF_NUM_SENSORS                           4

#define CAPLESENSE_SENSITIVITY_OFFS    1U

//// Upper voltage threshold
#define CAPLESENSE_ACMP_VDD_SCALE      LESENSE_ACMP_VDD_SCALE

/// Configuration for capacitive sense channels in sense mode.
#define LESENSE_CAPSENSE_CH_CONF_SENSE                                                                   \
  {                                                                                                      \
    true,                     /* Enable scan channel. */                                                 \
    true,                     /* Enable the assigned pin on scan channel. */                             \
    false,                    /* Disable interrupts on channel. */                                       \
    lesenseChPinExDis,        /* GPIO pin is disabled during the excitation period. */                   \
    lesenseChPinIdleDis,      /* GPIO pin is disabled during the idle period. */                         \
    false,                    /* Don't use alternate excitation pins for excitation. */                  \
    false,                    /* Disabled to shift results from this channel to the decoder register. */ \
    false,                    /* Disabled to invert the scan result bit. */                              \
    true,                     /* Enabled to store counter value in the result buffer. */                 \
    lesenseClkLF,             /* Use the LF clock for excitation timing. */                              \
    lesenseClkLF,             /* Use the LF clock for sample timing. */                                  \
    0x00U,                    /* Excitation time is set to 0 excitation clock cycles. */                 \
    0x0FU,                    /* Sample delay is set to 15(+1) sample clock cycles. */                   \
    0x00U,                    /* Measure delay is set to 0 excitation clock cycles.*/                    \
    LESENSE_ACMP_VDD_SCALE,   /* ACMP threshold has been set to LESENSE_ACMP_VDD_SCALE. */               \
    lesenseSampleModeCounter, /* ACMP will be used in comparison. */                                     \
    lesenseSetIntLevel,       /* Interrupt is generated if the sensor triggers. */                       \
    0x00U,                    /* Counter threshold has been set to 0x00. */                              \
    lesenseCompModeLess       /* Compare mode has been set to trigger interrupt on "less". */            \
  }

/// Configuration for capacitive sense channels in sleep mode.
#define LESENSE_CAPSENSE_CH_CONF_SLEEP                                                                   \
  {                                                                                                      \
    true,                     /* Enable scan channel. */                                                 \
    true,                     /* Enable the assigned pin on scan channel. */                             \
    true,                     /* Enable interrupts on channel. */                                        \
    lesenseChPinExDis,        /* GPIO pin is disabled during the excitation period. */                   \
    lesenseChPinIdleDis,      /* GPIO pin is disabled during the idle period. */                         \
    false,                    /* Don't use alternate excitation pins for excitation. */                  \
    false,                    /* Disabled to shift results from this channel to the decoder register. */ \
    false,                    /* Disabled to invert the scan result bit. */                              \
    true,                     /* Enabled to store counter value in the result buffer. */                 \
    lesenseClkLF,             /* Use the LF clock for excitation timing. */                              \
    lesenseClkLF,             /* Use the LF clock for sample timing. */                                  \
    0x00U,                    /* Excitation time is set to 0 excitation clock cycles. */                 \
    0x01U,                    /* Sample delay is set to 1(+1) sample clock cycles. */                    \
    0x00U,                    /* Measure delay is set to 0 excitation clock cycles.*/                    \
    LESENSE_ACMP_VDD_SCALE,   /* ACMP threshold has been set to LESENSE_ACMP_VDD_SCALE. */               \
    lesenseSampleModeCounter, /* Counter will be used in comparison. */                                  \
    lesenseSetIntLevel,       /* Interrupt is generated if the sensor triggers. */                       \
    0x0EU,                    /* Counter threshold has been set to 0x0E. */                              \
    lesenseCompModeLess       /* Compare mode has been set to trigger interrupt on "less". */            \
  }

/// Configuration for disabled channels.
#define LESENSE_DISABLED_CH_CONF                                                                         \
  {                                                                                                      \
    false,                    /* Disable scan channel. */                                                \
    false,                    /* Disable the assigned pin on scan channel. */                            \
    false,                    /* Disable interrupts on channel. */                                       \
    lesenseChPinExDis,        /* GPIO pin is disabled during the excitation period. */                   \
    lesenseChPinIdleDis,      /* GPIO pin is disabled during the idle period. */                         \
    false,                    /* Don't use alternate excitation pins for excitation. */                  \
    false,                    /* Disabled to shift results from this channel to the decoder register. */ \
    false,                    /* Disabled to invert the scan result bit. */                              \
    false,                    /* Disabled to store counter value in the result buffer. */                \
    lesenseClkLF,             /* Use the LF clock for excitation timing. */                              \
    lesenseClkLF,             /* Use the LF clock for sample timing. */                                  \
    0x00U,                    /* Excitation time is set to 5(+1) excitation clock cycles. */             \
    0x00U,                    /* Sample delay is set to 7(+1) sample clock cycles. */                    \
    0x00U,                    /* Measure delay is set to 0 excitation clock cycles.*/                    \
    0x00U,                    /* ACMP threshold has been set to 0. */                                    \
    lesenseSampleModeCounter, /* ACMP output will be used in comparison. */                              \
    lesenseSetIntNone,        /* No interrupt is generated by the channel. */                            \
    0x00U,                    /* Counter threshold has been set to 0x01. */                              \
    lesenseCompModeLess       /* Compare mode has been set to trigger interrupt on "less". */            \
  }

/// @brief Cross reference between sensor number ordering and pin
/// ordering.  This allows for using pins that are not in order when
/// doing layout.
#define MUX_VALUE_ARRAY 3, 2, 1, 0

/// @brief Per-channel active threshold settings.  When consecutive conversions
/// for a channel rise above this threshold, the sensor will be qualified as active.
/// @note Minimum threshold used is @ref INACTIVE_THRESHOLD_ARRAY value,
/// maximum value is 100
#define ACTIVE_THRESHOLD_ARRAY 70, 70, 70, 70

/// @brief Per-channel inactive threshold settings.  When consecutive conversions
/// for a channel fall below this threshold, the sensor will be qualified as inactive.
/// @note Minimum threshold used is 1, maximum value is @ref ACTIVE_THRESHOLD_ARRAY
#define INACTIVE_THRESHOLD_ARRAY 30, 30, 30, 30

/// @brief Per-channel expected touch delta.  This value describes the difference
/// in capacitive sensing output codes between the inactive/baseline of the sensor,
/// and the output of the sensor when active(touched).
/// @note These values should be defined in terms of X/16, or X>>4, as they are stored
/// in a packed byte array
#define AVERAGE_TOUCH_DELTA_ARRAY 64 >> 4, 64 >> 4, 64 >> 4, 64 >> 4

/// @brief Per channel port setting for each enabled sensor
#define CSLIB_CHANNEL_PORT  gpioPortC, gpioPortC, gpioPortC, gpioPortC,

/// @brief Per channel pin setting for each enabled sensor
#define CSLIB_CHANNEL_PIN   8UL, 9UL, 10UL, 11UL

/// @brief Bit array sets inputs to be used in active and sleep mode scanning
#define CAPLESENSE_CHANNEL_INT        (LESENSE_IF_CH8 | LESENSE_IF_CH9 | LESENSE_IF_CH10 | LESENSE_IF_CH11)

/// @brief Equal to the number of capacitive sensing input-enabled pins on device
#define LESENSE_CHANNELS        16  /**< Number of channels for the Low Energy Sensor Interface. */

/// @brief Upper voltage threshold.
#define LESENSE_ACMP_VDD_SCALE    0x37U

/// @brief enables/disables each capacitive sensing input pin.
/// This controls the sensors that get converted during active AND sleep scanning.
#define LESENSE_CAPSENSE_CH_IN_USE {                           \
/*  Ch0,   Ch1,   Ch2,   Ch3,   Ch4,   Ch5,   Ch6,   Ch7    */ \
    false, false, false, false, false, false, false, false,    \
/*  Ch8,   Ch9,   Ch10,  Ch11,  Ch12,  Ch13,  Ch14,  Ch15   */ \
    true, true, true, true, false, false, false, false         \
}

/// @brief Array defines sensors to be used in active mode scanning
#define LESENSE_CAPSENSE_SCAN_CONF_SENSE                 \
  {                                                      \
    {                                                    \
      LESENSE_DISABLED_CH_CONF,        /* Channel 0. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 1. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 2. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 3. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 4. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 5. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 6. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 7. */  \
      LESENSE_CAPSENSE_CH_CONF_SENSE,  /* Channel 8. */  \
      LESENSE_CAPSENSE_CH_CONF_SENSE,  /* Channel 9. */  \
      LESENSE_CAPSENSE_CH_CONF_SENSE,  /* Channel 10. */ \
      LESENSE_CAPSENSE_CH_CONF_SENSE,  /* Channel 11. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 12. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 13. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 14. */ \
      LESENSE_DISABLED_CH_CONF         /* Channel 15. */ \
    }                                                    \
  }

/// @brief Array defines sensors to be used in sleep mode scanning
#define LESENSE_CAPSENSE_SCAN_CONF_SLEEP                 \
  {                                                      \
    {                                                    \
      LESENSE_DISABLED_CH_CONF,        /* Channel 0. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 1. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 2. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 3. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 4. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 5. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 6. */  \
      LESENSE_DISABLED_CH_CONF,        /* Channel 7. */  \
      LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 8. */  \
      LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 9. */  \
      LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 10. */ \
      LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 11. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 12. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 13. */ \
      LESENSE_DISABLED_CH_CONF,        /* Channel 14. */ \
      LESENSE_DISABLED_CH_CONF         /* Channel 15. */ \
    }                                                    \
  }

/// Array describing port settings for each enabled channel.  This is
/// populated using @ref CSLIB_CHANNEL_PORT
extern const GPIO_Port_TypeDef CSLIB_ports[];

/// Array describing pin settings for each enabled channel.   This is
/// populated using @ref CSLIB_CHANNEL_PIN
extern const unsigned long CSLIB_pins[];

/// @brief Array of cross-references between sensor number and actual pin number
extern const uint8_t CSLIB_muxValues[];

/// @brief Only send comms after new values
extern uint8_t sendComms;

/** @} (end cslib_HWconfig) */

#endif // CSLIB_HWCONFIG_H
