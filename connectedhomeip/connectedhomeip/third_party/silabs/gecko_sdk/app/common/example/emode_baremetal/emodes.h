/***************************************************************************//**
 * @file
 * @brief Energy modes header file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EMODES_H_
#define EMODES_H_

#include <stdbool.h>
#include "em_device.h"

/***************************************************************************//**
 * Energy mode enumerations.
 ******************************************************************************/
typedef enum {
  EM0,                        /**< EM0 Active                                 */
  EM1,                        /**< EM1 Sleep                                  */
  EM2,                        /**< EM2 DeepSleep                              */
  EM3,                        /**< EM3 Stop                                   */
#if !defined(_SILICON_LABS_32B_SERIES_1)
  EM4,                        /**< EM4                                        */
#else
  EM4H,                       /**< EM4 Hibernate                              */
  EM4S,                       /**< EM4 Shutoff                                */
#endif
  NUM_EMODES
} energy_mode_enum_t;

typedef enum {
  HFXO_,
#if defined(_SILICON_LABS_32B_SERIES_2)
  FSRCO_20MHZ,
  HFRCO_80MHZ,
  HFRCO_38MHZ,
  HFRCO_26MHZ,
  HFRCO_1MHZ,
#elif defined(_SILICON_LABS_32B_SERIES_0)
#if defined(CMU_HFRCOCTRL_BAND_28MHZ)
  HFRCO_28MHZ,
#endif
  HFRCO_21MHZ,
  HFRCO_14MHZ,
  HFRCO_11MHZ,
  HFRCO_7MHZ,
  HFRCO_1MHZ,
#else
#if defined(_DEVINFO_HFRCOCAL16_MASK)
  HFRCO_72MHZ,
#endif
#if defined(_DEVINFO_HFRCOCAL15_MASK)
  HFRCO_64MHZ,
#endif
#if defined(_DEVINFO_HFRCOCAL14_MASK)
  HFRCO_56MHZ,
#endif
#if defined(_DEVINFO_HFRCOCAL13_MASK)
  HFRCO_48MHZ,
#endif
  HFRCO_38MHZ,
  HFRCO_26MHZ,
  HFRCO_19MHZ,
  HFRCO_4MHZ,
  HFRCO_1MHZ,
#endif
  NUM_EM01_OSCS
} em01_oscillator_enum_t;

typedef enum {
#if defined(RTCC_PRESENT)
  EM2_LFXO_RTCC,                 /**< EM2 with LFXO RTCC                      */
  EM2_LFXO_RTCC_RAM_POWERDOWN,   /**< EM2 with LFXO RTCC RAM POWER DOWN       */
  EM2_LFRCO_RTCC,                /**< EM2 with LFRCO RTCC                     */
  EM2_LFRCO_RTCC_RAM_POWERDOWN,  /**< EM2 with LFRCO RTCC RAM POWER DOWN      */
#endif
#if defined(RTC_PRESENT)
  EM2_LFRCO_RTC,                 /**< EM2 with LFRCO RTC                      */
  EM2_LFRCO_RTC_RAM_POWERDOWN,   /**< EM2 with LFRCO RTC RAM POWER DOWN       */
#elif defined(SYSRTC_PRESENT)
  EM2_LFRCO_SYSRTC,              /**< EM2 with LFRCO SYSRTC                   */
  EM2_LFRCO_SYSRTC_RAM_POWERDOWN,/**< EM2 with LFRCO SYSRTC RAM POWER DOWN    */
#if !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
  EM2_LFXO_SYSRTC,               /**< EM2 with LFXO SYSRTC                    */
  EM2_LFXO_SYSRTC_RAM_POWERDOWN, /**< EM2 with LFXO SYSRTC RAM POWER DOWN     */
#endif //!(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
#endif
  NUM_EM2_OSCS
} em2_oscillator_enum_t;

typedef enum {
  EM3_ULFRCO,
  EM3_ULFRCO_RAM_POWERDOWN,
 #if defined(_SILICON_LABS_32B_SERIES_2)
  EM3_ULFRCO_BURTC,            /**< EM3 with ULFRCO BURTC                     */
  EM3_ULFRCO_BURTC_RAM_POWERDOWN,
 #endif
 #if defined(CRYOTIMER_PRESENT)
  EM3_ULFRCO_CRYO,             /**< EM3  with ULFRCO CRYOTIMER                */
  EM3_ULFRCO_CRYO_RAM_POWERDOWN,
 #endif
  NUM_EM3_OSCS
} em3_oscillator_enum_t;

typedef enum {
#if defined(_SILICON_LABS_32B_SERIES_2)
  EM4_LFRCO_BURTC,            /**< EM4 with LFRCO BURTC                       */
  EM4_ULFRCO_BURTC,           /**< EM4 with ULFRCO BURTC                      */
#else
  EM4H_LFXO_RTCC,             /**< EM4H with RTCC                             */
#if defined(CRYOTIMER_PRESENT)
  EM4H_ULFRCO_CRYO,           /**< EM4H with ULFRCO CRYOTIMER                 */
#endif
#endif
  NONE,
  NUM_EM4H_OSCS
} em4h_oscillator_enum_t;

typedef enum {
  WHILE,                      /**< while loop                                 */
  PRIME,                      /**< prime calculations                         */
  COREMARK,                   /**< Coremark                                   */
  NUM_OPS
} operation_enum_t;

/***************************************************************************//**
 * Structure to store the selected energy mode info
 ******************************************************************************/
typedef struct {
  energy_mode_enum_t em;      /**< Energy Mode EM0-4                          */
  uint16_t osc;               /**< Oscillator                                 */
  operation_enum_t op;        /**< Operation                                  */
  bool dcdc;                  /**< With/Without DCDC                          */
} energy_mode_t;

/***************************************************************************//**
 * Enter energy mode functions
 ******************************************************************************/
void em_init(void);

/***************************************************************************//**
 * Function to switch to the selected Emode.
 ******************************************************************************/
void start_emode_test(energy_mode_t*);

#endif /* EMODES_H_ */
