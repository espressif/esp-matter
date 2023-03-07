/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef CSLIB_HWCONFIG_H
#define CSLIB_HWCONFIG_H

#include "em_device.h"
#include "em_csen.h"

/// @brief Defines the size of the sensor node array.
/// Also defines volatile arrays that have a one-to-one correspondence
/// to the number of sensors in the project.
/// @note Minimum value is 1, maximum value is the number of capacitive
/// sensing-enabled pins on the device
#define DEF_NUM_SENSORS                           4

/// @brief Cross reference between sensor number ordering and APORT ordinal
/// ordering.  For example, if a slider is using APORT 3, 0, 4, 5, the DMA
/// engine returns data as 0, 3, 4, 5, but it has to be reordered so the numbers
/// are presented as 3, 0, 4, 5.  MUX_VALUE_ARRAY in this case is 1, 0, 2, 3
#define MUX_VALUE_ARRAY 2, 3, 1, 0

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
#define AVERAGE_TOUCH_DELTA_ARRAY 2048 >> 4, 2048 >> 4, 2048 >> 4, 2048 >> 4

/// @brief Cutoff below baseline before a point is treated as bad.
/// When a point is treated as bad, the previous value is preserved instead.
/// Manually use the touch delta here, since it is stored in an array.
#define DELTA_CUTOFF (2048 / 4)
/// @brief How much noise to tolerate before swapping to new TRST setting. This value lets
/// the HAL layer switch to a new TRST setting that might be better for noise once noise passes
/// this level in the system.
#define TRST_NOISE_THRESHOLD 18

/// @brief How many sensor passes noise must be above TRST_NOISE_THRESHOLD before changing TRST.
/// This value adds a delay before changing to a new TRST setting in case the noise is due to a touch
/// event beginning.  If the noise is still above the threshold after this many samples, then TRST
/// is stepped to the next option.  Since it is reset after stepping, this means there is a minimum
/// dwell time at the new setting of TRST_DELAY before stepping again to give the touch algorithm time
/// to digest the new noise results.
#define TRST_DELAY 8

/// @brief Boolean to reset or not reset the noise estimation values on a TRST change
#define TRST_NOISE_EST_RESET true

/// @brief Array of TRST values to try when noise occurs.
/// When a TRST change is ordered by the HAL, this array contains the settings used.
#define TRST_ARRAY csenResetPhaseSel0, csenResetPhaseSel1, csenResetPhaseSel2, csenResetPhaseSel3, csenResetPhaseSel4, csenResetPhaseSel5, csenResetPhaseSel6, csenResetPhaseSel7
#define TRST_ARRAY_SIZE 8

#ifndef DEF_DM_TRST
#define DEF_DM_TRST csenResetPhaseSel0
#endif

#ifndef DEF_DM_IDAC
#define DEF_DM_IDAC csenDriveSelFull
#endif

#define CSEN_ACTIVEMODE_DEFAULT                                        \
  {                                                                    \
    csenSampleModeScan,         /* Sample one input and stop. */       \
    csenTrigSelTimer,           /* Use start bit to trigger. */        \
    true,                       /* Enable DMA. */                      \
    false,                      /* Average the accumulated result. */  \
    csenAccMode1,               /* Accumulate 1 sample. */             \
    csenEMASampleW1,            /* Disable the EMA. */                 \
    csenCmpModeDisabled,        /* Disable the comparator. */          \
    0,                          /* Comparator threshold not used. */   \
    csenSingleSelDefault,       /* Disconnect the single input. */     \
    0x80500080,                 /* Enable inputs 7, 20, 22, 31 */      \
    0,                          /* Disable inputs 32 to 63. */         \
    false,                      /* Do not ground inactive inputs. */   \
    csenConvSelDM,              /* Use the DM mode. */                 \
    csenSARRes16,               /* Set SAR resolution to 10 bits. */   \
    csenDMRes16,                /* Set DM resolution to 10 bits. */    \
    4,                          /* Set DM conv/cycle to default. */    \
    6,                          /* Set DM cycles to default. */        \
    128,                        /* Set DM initial delta to default. */ \
    false,                      /* Use DM auto delta reduction. */     \
    DEF_DM_TRST,                /* Use shortest reset phase time. */   \
    DEF_DM_IDAC,                /* Use full output current. */         \
    csenGainSel8X,              /* Use highest converter gain. */      \
  }

#define CSEN_SLEEPMODE_DEFAULT                                         \
  {                                                                    \
    csenSampleModeBonded,       /* Sample bonded channels. */          \
    csenTrigSelTimer,           /* Use start bit to trigger. */        \
    false,                      /* Disable DMA. */                     \
    false,                      /* Do not Avg the acc result. */       \
    csenAccMode1,               /* Accumulate 1 sample. */             \
    csenEMASampleW8,            /* Set EMA to W8.   */                 \
    csenCmpModeEMAWindow,       /* Enable EMA comparator. */           \
    255,                        /* Set wake value +/-EMA. */           \
    csenSingleSelDefault,       /* Disconnect the single input. */     \
    0x80500080,                 /* Enable inputs 7, 20, 22, 31 */      \
    0,                          /* Disable inputs 32 to 63. */         \
    false,                      /* Do not ground inactive inputs. */   \
    csenConvSelDM,              /* Use the DM converter. */            \
    csenSARRes16,               /* Set SAR resolution to 10 bits. */   \
    csenDMRes16,                /* Set DM resolution to 16 bits. */    \
    2,                          /* Set DM conv/cycle to default. */    \
    6,                          /* Set DM cycles to default. */        \
    128,                        /* Set DM initial delta to default. */ \
    false,                      /* Don't disable DM auto reduction. */ \
    DEF_DM_TRST,                /* Use shortest reset phase time. */   \
    DEF_DM_IDAC,                /* Use full output current. */         \
    csenGainSel2X,              /* Lower gain, sensors ganged. */      \
  }

/// @brief Array of cross-references between sensor number and APORT number
extern const uint8_t CSLIB_muxValues[];

/// @brief Array of TRST settings
extern const CSEN_ResetPhaseSel_TypeDef CSLIB_TRST[];

/// @brief Only send comms after LDMA has returned new values
extern uint8_t sendComms;

/// @brief Current TRST setting number
extern uint8_t indexTRST;

/** @} (end cslib_HWconfig) */

#endif // __CSLIB_HWCONFIG_H__
