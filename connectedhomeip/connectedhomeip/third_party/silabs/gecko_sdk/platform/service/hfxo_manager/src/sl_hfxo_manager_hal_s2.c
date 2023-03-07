/***************************************************************************//**
 * @file
 * @brief HFXO Manager HAL series 2 Devices.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_device.h"
#if defined(_SILICON_LABS_32B_SERIES_2)
#include "sl_assert.h"
#include "sli_hfxo_manager.h"
#include "sl_hfxo_manager.h"
#include "sl_hfxo_manager_config.h"
#include "sl_status.h"

#include <stdbool.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

// Defines for hidden field FORCERAWCLK in HFXO_CTRL register
#define _HFXO_MANAGER_CTRL_FORCERAWCLK_SHIFT                  31
#define _HFXO_MANAGER_CTRL_FORCERAWCLK_MASK                   0x80000000UL
#define HFXO_MANAGER_CTRL_FORCERAWCLK                        (0x1UL << _HFXO_MANAGER_CTRL_FORCERAWCLK_SHIFT)

// Defines for hidden PKDETCTRL register
#ifndef _HFXO_PKDETCTRL_MASK
#if (_SILICON_LABS_32B_SERIES_2_CONFIG <= 2)
#define PKDETCTRL  RESERVED4[2]
#else
#define PKDETCTRL  RESERVED3[0]
#endif
#endif
#define _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT           8
#define _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_MASK            0xF00UL
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V105MV          (0x00000000UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V132MV          (0x00000001UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V157MV          (0x00000002UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V184MV          (0x00000003UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V210MV          (0x00000004UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V236MV          (0x00000005UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V262MV          (0x00000006UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V289MV          (0x00000007UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V315MV          (0x00000008UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V341MV          (0x00000009UL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V367MV          (0x0000000AUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V394MV          (0x0000000BUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V420MV          (0x0000000CUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V446MV          (0x0000000DUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V472MV          (0x0000000EUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)
#define HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V499MV          (0x0000000FUL << _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_SHIFT)

// IRQ Name depending on devices
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
#define HFXO_IRQ_NUMBER  HFXO00_IRQn
#define HFXO_IRQ_HANDLER_FUNCTION  HFXO00_IRQHandler
#else
#define HFXO_IRQ_NUMBER  HFXO0_IRQn
#define HFXO_IRQ_HANDLER_FUNCTION  HFXO0_IRQHandler
#endif

// Default values for the Sleepy Crystal settings
// Should be enough to guaranty HFXO startup
#define SLEEPY_XTAL_SETTING_DEFAULT_PKDETTHSTARTUPI  HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_V157MV
#define SLEEPY_XTAL_SETTING_DEFAULT_CTUNEANA         100u
#define SLEEPY_XTAL_SETTING_DEFAULT_COREBIAS         255u

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Error flag to indicate if we failed the startup process
static bool error_flag = false;
#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
// Error retry counter
static uint8_t error_try_cnt = 0;

// Error State status
static bool in_error_state = false;

// Variables to save normal settings
static uint32_t pkdettusstartupi_saved;
static uint32_t ctunexiana_saved;
static uint32_t ctunexoana_saved;
static uint32_t corebiasana_saved;
static uint32_t corebiasstartup_saved;
static uint32_t corebiasstartupi_saved;

// Variables for Sleepy Crystal settings
static uint32_t sleepy_xtal_settings_pkdettusstartupi = SLEEPY_XTAL_SETTING_DEFAULT_PKDETTHSTARTUPI; // Value already shifted
static uint32_t sleepy_xtal_settings_ctuneana = SLEEPY_XTAL_SETTING_DEFAULT_CTUNEANA;
static uint32_t sleepy_xtal_settings_corebias = SLEEPY_XTAL_SETTING_DEFAULT_COREBIAS;
#endif

/***************************************************************************//**
 * HFXO ready notification callback for internal use with power manager
 ******************************************************************************/
__WEAK void sli_hfxo_manager_notify_ready_for_power_manager(void);

/***************************************************************************//**
 * Hardware specific initialization.
 ******************************************************************************/
void sli_hfxo_manager_init_hardware(void)
{
  // Increase HFXO Interrupt priority so that it won't be masked by BASEPRI
  // and will preempt other interrupts.
  NVIC_SetPriority(HFXO_IRQ_NUMBER, 2);

  // Enable HFXO Interrupt if HFXO is used
#if _SILICON_LABS_32B_SERIES_2_CONFIG >= 2
  CMU->CLKEN0_SET = CMU_CLKEN0_HFXO0;
#endif

  HFXO0->IEN_CLR = HFXO_IEN_RDY | HFXO_IEN_DNSERR | HFXO_IEN_COREBIASOPTERR;
  HFXO0->IF_CLR = HFXO_IF_RDY | HFXO_IF_DNSERR | HFXO_IEN_COREBIASOPTERR;

  NVIC_ClearPendingIRQ(HFXO_IRQ_NUMBER);
  NVIC_EnableIRQ(HFXO_IRQ_NUMBER);

  HFXO0->IEN_SET = HFXO_IEN_RDY | HFXO_IEN_DNSERR | HFXO_IEN_COREBIASOPTERR;
}

/***************************************************************************//**
 * Updates sleepy crystal settings in specific hardware registers.
 ******************************************************************************/
sl_status_t sli_hfxo_manager_update_sleepy_xtal_settings_hardware(sl_hfxo_manager_sleepy_xtal_settings_t *settings)
{
  (void)settings;

#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
  EFM_ASSERT(settings->ana_ctune <= (_HFXO_XTALCTRL_CTUNEXIANA_MASK >> _HFXO_XTALCTRL_CTUNEXIANA_SHIFT));
  EFM_ASSERT(settings->core_bias_current <= (_HFXO_XTALCTRL_COREBIASANA_MASK >> _HFXO_XTALCTRL_COREBIASANA_SHIFT));

  sleepy_xtal_settings_ctuneana = settings->ana_ctune;
  sleepy_xtal_settings_corebias = settings->core_bias_current;

  return SL_STATUS_OK;
#else
  return SL_STATUS_NOT_AVAILABLE;
#endif
}

/***************************************************************************//**
 * Checks if HFXO is ready and, if needed, waits for it to be.
 *
 * @note This will also make sure we are not in the process of restarting HFXO
 *       with different settings.
 ******************************************************************************/
bool sli_hfxo_manager_is_hfxo_ready(bool wait)
{
  bool ready = false;

  do {
    ready = (((HFXO0->STATUS & HFXO_STATUS_RDY) != 0) && !error_flag) ? true : false;
  } while (!ready && wait);

  return ready;
}

#if (SL_HFXO_MANAGER_CUSTOM_HFXO_IRQ_HANDLER == 0)
/*******************************************************************************
 * HFXO interrupt handler.
 *
 * @note  The HFXOx_IRQHandler provided by HFXO Manager will call
 *        @ref sl_hfxo_manager_irq_handler. Configure SL_HFXO_MANAGER_CUSTOM_HFXO_IRQ_HANDLER
 *        if the application wants to implement its own HFXOx_IRQHandler.
 ******************************************************************************/
void HFXO_IRQ_HANDLER_FUNCTION(void)
{
  sl_hfxo_manager_irq_handler();
}
#endif

/*******************************************************************************
 * HFXO Manager HFXO interrupt handler.
 ******************************************************************************/
void sl_hfxo_manager_irq_handler(void)
{
  uint32_t irq_flag = HFXO0->IF;
#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
  bool disondemand =  (HFXO0->CTRL & _HFXO_CTRL_DISONDEMAND_MASK) ? true : false;
  bool forceen = (HFXO0->CTRL & _HFXO_CTRL_FORCEEN_MASK) ? true : false;
#endif

  // RDY Interrupt Flag Handling
  if (irq_flag & HFXO_IF_RDY) {
    // Clear Ready flag
    HFXO0->IF_CLR = irq_flag & HFXO_IF_RDY;

#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
    if (error_flag) {
      // Clear error flag, i.e. we successfully stated HFXO with the modified settings
      error_flag = false;

      // If it's the first time we succeed after an error, try back the normal settings
      if (error_try_cnt <= 1) {
        // Disable HFXO.
        HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
        HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;

        while ((HFXO0->STATUS & HFXO_STATUS_ENS) != 0) {
        }

        // Put back normal settings
        HFXO0->PKDETCTRL = (HFXO0->PKDETCTRL & ~_HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_MASK) | pkdettusstartupi_saved;
        HFXO0->XTALCTRL = (HFXO0->XTALCTRL & ~(_HFXO_XTALCTRL_CTUNEXIANA_MASK | _HFXO_XTALCTRL_CTUNEXOANA_MASK))
                          | ctunexiana_saved
                          | ctunexoana_saved;
        HFXO0->XTALCFG = (HFXO0->XTALCFG & ~(_HFXO_XTALCFG_COREBIASSTARTUPI_MASK | _HFXO_XTALCFG_COREBIASSTARTUP_MASK))
                         | corebiasstartup_saved
                         | corebiasstartupi_saved;
        HFXO0->XTALCTRL = (HFXO0->XTALCTRL & ~_HFXO_XTALCTRL_COREBIASANA_MASK) | corebiasana_saved;

        // Put back FORCEEN and DISONDEMAND state
        if (!disondemand) {
          HFXO0->CTRL_CLR = HFXO_CTRL_DISONDEMAND;
        } else {
          HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;
        }
        if (forceen) {
          HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;
        } else {
          HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
        }
      } else {
        // Call notification function to tell users that sleepy crystal settings are kept
        // This should only happen if you are in test condition or if you have a bad crystal.
        sl_hfxo_manager_notify_consecutive_failed_startups();
        in_error_state = true;
      }
    } else {
      sli_hfxo_manager_end_startup_measurement();

      sli_hfxo_manager_notify_ready_for_power_manager();

      // Clear counter since we successfully started HFXO with normal settings
      // or we are just keeping sleepy crystal settings indefinitely.
      error_try_cnt = 0;
    }
#else
    sli_hfxo_manager_end_startup_measurement();

    sli_hfxo_manager_notify_ready_for_power_manager();
#endif
  }

  // DNSERR Interrupt Flag Handling
  if (irq_flag & HFXO_IF_DNSERR) {
    // Clear error flag
    HFXO0->IF_CLR = irq_flag & HFXO_IF_DNSERR;

#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
    // We should not fail twice in a row
    EFM_ASSERT(error_flag == false);

    // Update global variables related to error.
    error_flag = true;
    error_try_cnt++;

    // Save current settings
    pkdettusstartupi_saved = (HFXO0->PKDETCTRL & _HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_MASK);
    ctunexiana_saved = (HFXO0->XTALCTRL & _HFXO_XTALCTRL_CTUNEXIANA_MASK);
    ctunexoana_saved = (HFXO0->XTALCTRL & _HFXO_XTALCTRL_CTUNEXOANA_MASK);
    corebiasana_saved = (HFXO0->XTALCTRL & _HFXO_XTALCTRL_COREBIASANA_MASK);
    corebiasstartup_saved = (HFXO0->XTALCFG & _HFXO_XTALCFG_COREBIASSTARTUP_MASK);
    corebiasstartupi_saved = (HFXO0->XTALCFG & _HFXO_XTALCFG_COREBIASSTARTUPI_MASK);

    // Disable HFXO.
    HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
    HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;

    // Use FORCERAWCLK bit to exit error state when disabling
    HFXO0->CTRL_SET = HFXO_MANAGER_CTRL_FORCERAWCLK;
    while ((HFXO0->STATUS & _HFXO_STATUS_ENS_MASK) != 0U) {
    }
    HFXO0->CTRL_CLR = HFXO_MANAGER_CTRL_FORCERAWCLK;

    // Change settings:
    //Reduce Peak Detection Threshold for Startup Intermediate stage to 2 (V157MV)
    HFXO0->PKDETCTRL = (HFXO0->PKDETCTRL & ~_HFXO_MANAGER_PKDETCTRL_PKDETTHSTARTUPI_MASK) | sleepy_xtal_settings_pkdettusstartupi;
    // Reduce CTUNE values for steady stage
    if (((ctunexiana_saved >> _HFXO_XTALCTRL_CTUNEXIANA_SHIFT) > 100)
        || ((ctunexoana_saved >> _HFXO_XTALCTRL_CTUNEXOANA_SHIFT) > 100)) {
      HFXO0->XTALCTRL = (HFXO0->XTALCTRL & ~(_HFXO_XTALCTRL_CTUNEXIANA_MASK | _HFXO_XTALCTRL_CTUNEXOANA_MASK))
                        | (sleepy_xtal_settings_ctuneana << _HFXO_XTALCTRL_CTUNEXIANA_SHIFT)
                        | (sleepy_xtal_settings_ctuneana << _HFXO_XTALCTRL_CTUNEXOANA_SHIFT);
    }
    // Increase core bias current at all stages
    HFXO0->XTALCFG = (HFXO0->XTALCFG & ~(_HFXO_XTALCFG_COREBIASSTARTUPI_MASK | _HFXO_XTALCFG_COREBIASSTARTUP_MASK))
                     | ((sleepy_xtal_settings_corebias >> 2) << _HFXO_XTALCFG_COREBIASSTARTUPI_SHIFT)
                     | ((sleepy_xtal_settings_corebias >> 2) << _HFXO_XTALCFG_COREBIASSTARTUP_SHIFT);
    HFXO0->XTALCTRL = (HFXO0->XTALCTRL & ~_HFXO_XTALCTRL_COREBIASANA_MASK)
                      | (sleepy_xtal_settings_corebias << _HFXO_XTALCTRL_COREBIASANA_SHIFT);

    // Put back FORCEEN and DISONDEMAND state
    if (!disondemand) {
      HFXO0->CTRL_CLR = HFXO_CTRL_DISONDEMAND;
    } else {
      HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;
    }
    if (forceen) {
      HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;
    } else {
      HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
    }
#endif
  }

  if (irq_flag & HFXO_IF_COREBIASOPTERR) {
    // Clear Core Bias Optimization error flag
    HFXO0->IF_CLR = irq_flag & HFXO_IF_COREBIASOPTERR;

#if (SL_HFXO_MANAGER_SLEEPY_CRYSTAL_SUPPORT == 1)
    // In case the Core Bias Optimization fails during error handling,
    // we disable it
    if (in_error_state == true) {
      // Disable HFXO.
      HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
      HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;

      while ((HFXO0->STATUS & HFXO_STATUS_ENS) != 0) {
      }

      // Skip Core Bias Optimization in case of error
      HFXO0->XTALCTRL_SET = HFXO_XTALCTRL_SKIPCOREBIASOPT;

      // Put back FORCEEN and DISONDEMAND state
      if (!disondemand) {
        HFXO0->CTRL_CLR = HFXO_CTRL_DISONDEMAND;
      } else {
        HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;
      }
      if (forceen) {
        HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;
      } else {
        HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;
      }
    }
#endif
  }
}
#endif // _SILICON_LABS_32B_SERIES_2
