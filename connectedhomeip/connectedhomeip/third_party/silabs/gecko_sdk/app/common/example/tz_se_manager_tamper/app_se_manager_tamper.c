/***************************************************************************//**
 * @file app_se_manager_tamper.c
 * @brief SE manager tamper functions.
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_tamper.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Command context
static sl_se_command_context_t cmd_ctx;

/// Tamper interrupt status
static bool tamper_int_status;

/// Reset cause buffer
static uint32_t rst_cause_buf;

/// SE status buffer
static sl_se_status_t se_status_buf;

/// SE OTP configuration buffer
static sl_se_otp_init_t se_otp_conf_buf;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Interrupt handler for tamper.
 ******************************************************************************/
void SETAMPERHOST_IRQHandler(void)
{
  tamper_int_status = true;
}

/***************************************************************************//**
 * Enable tamper interrupt.
 ******************************************************************************/
void enable_tamper_int(void)
{
  NVIC_ClearPendingIRQ(SETAMPERHOST_IRQn);
  NVIC_EnableIRQ(SETAMPERHOST_IRQn);
}

/***************************************************************************//**
 * Get tamper interrupt status.
 ******************************************************************************/
bool get_tamper_int_status(void)
{
  if (tamper_int_status) {
    tamper_int_status = false;
    return true;
  } else {
    return false;
  }
}

/***************************************************************************//**
 * Get reset cause buffer pointer.
 ******************************************************************************/
uint32_t * get_rst_cause_buf_ptr(void)
{
  return(&rst_cause_buf);
}

/***************************************************************************//**
 * Get SE status buffer pointer.
 ******************************************************************************/
sl_se_status_t * get_se_status_buf_ptr(void)
{
  return(&se_status_buf);
}

/***************************************************************************//**
 * Get SE OTP configuration buffer pointer.
 ******************************************************************************/
sl_se_otp_init_t * get_se_otp_conf_buf_ptr(void)
{
  return(&se_otp_conf_buf);
}

/***************************************************************************//**
 * Initialize PRS for tamper.
 ******************************************************************************/
void init_tamper_prs(void)
{
  // Enable GPIO and PRS clock if required
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  // Configure push buttons (PB0 and PB1) on WSTK
  GPIO_PinModeSet(PRS_PB0_PORT, PRS_PB0_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(PRS_PB0_PORT, PRS_PB0_PIN, PRS_PB0_PIN, false, false,
                    false);

  GPIO_PinModeSet(PRS_PB1_PORT, PRS_PB1_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(PRS_PB1_PORT, PRS_PB1_PIN, PRS_PB1_PIN, false, false,
                    false);

  // Configure push buttons (PB0 and PB1) as PRS producers
  // Use GPIO pin number for PRS_ASYNC_CH_CTRL_SIGSEL_GPIOPIN0#
  PRS_SourceAsyncSignalSet(TAMPER_INT_PRS_CH, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           PRS_PB0_PIN);
  PRS_SourceAsyncSignalSet(HW_RST_TAMPER_PRS_CH,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           PRS_PB1_PIN);

  // Deactivate PRS outputs before connecting them to tamper sources
  PRS_ChannelLevelSet(TAMPER_INT_PRS_CH, true);
  PRS_ChannelLevelSet(HW_RST_TAMPER_PRS_CH, true);

  // Configure tamper sources as PRS consumers
  // PB0 for interrupt and filter counter, PB1 and software PRS for tamper reset
  // Note: PRS channel 0 is the default producer for all tamper sources
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  PRS_ConnectConsumer(TAMPER_INT_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SE_TAMPERSRC0));
  PRS_ConnectConsumer(TAMPER_CNT_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SE_TAMPERSRC2));
  PRS_ConnectConsumer(HW_RST_TAMPER_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SE_TAMPERSRC4));
  PRS_ConnectConsumer(SW_RST_TAMPER_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SE_TAMPERSRC5));
#else
  PRS_ConnectConsumer(TAMPER_INT_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SETAMPER_TAMPERSRC26));
  PRS_ConnectConsumer(TAMPER_CNT_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SETAMPER_TAMPERSRC27));
  PRS_ConnectConsumer(HW_RST_TAMPER_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SETAMPER_TAMPERSRC29));
  PRS_ConnectConsumer(SW_RST_TAMPER_PRS_CH, prsTypeAsync,
                      offsetof(PRS_TypeDef, CONSUMER_SETAMPER_TAMPERSRC30));
#endif
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Get EMU->RSTCAUSE after a tamper reset.
 ******************************************************************************/
sl_status_t get_reset_cause(void)
{
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  print_error_cycle(sl_se_get_reset_cause(&cmd_ctx, &rst_cause_buf), &cmd_ctx);
#else
  rst_cause_buf = EMU->RSTCAUSE;
  EMU->CMD = EMU_CMD_RSTCAUSECLR;
  return SL_STATUS_OK;
#endif
}

/***************************************************************************//**
 * Get SE status.
 ******************************************************************************/
sl_status_t get_se_status(void)
{
  print_error_cycle(sl_se_get_status(&cmd_ctx, &se_status_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Get SE OTP configuration.
 ******************************************************************************/
sl_status_t get_se_otp_conf(void)
{
  print_error_cycle(sl_se_read_otp(&cmd_ctx, &se_otp_conf_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Initialize SE OTP configuration.
 ******************************************************************************/
sl_status_t init_se_otp_conf(void)
{
  sl_se_otp_init_t otp_init = SL_SE_OTP_INIT_DEFAULT;

  // Overwrite secure boot options in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.enable_secure_boot = false;
  otp_init.verify_secure_boot_certificate = false;
  otp_init.enable_anti_rollback = false;
  otp_init.secure_boot_page_lock_narrow = false;
  otp_init.secure_boot_page_lock_full = false;

  // Overwrite tamper signal levels in SL_SE_OTP_INIT_DEFAULT if necessary.
  // It is not possible to degrade the default response level of a tamper
  // signal, so if a response is set to a lower level than the default response
  // level, this won't have any effect.
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER_COUNTER] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_WATCHDOG] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_CRC] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_HARDFAULT] =
    SL_SE_TAMPER_LEVEL_RESET;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SOFTWARE_ASSERTION] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_USER_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_MAILBOX_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DCI_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FLASH_INTEGRITY_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SELFTEST_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TRNG_MONITOR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS0] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS2] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS3] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS4] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS5] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS6] =
    SL_SE_TAMPER_LEVEL_PERMANENTLY_ERASE_OTP;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS7] =
    SL_SE_TAMPER_LEVEL_PERMANENTLY_ERASE_OTP;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DECOUPLE_BOD] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TEMPERATURE_SENSOR] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VOLTAGE_GLITCH_FALLING] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VOLTAGE_GLITCH_RISING] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SECURE_LOCK_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_DEBUG_GRANTED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DIGITAL_GLITCH] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_ICACHE_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
#else
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER_COUNTER] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_WATCHDOG] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_ECC_2] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_HARDFAULT] =
    SL_SE_TAMPER_LEVEL_RESET;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SOFTWARE_ASSERTION] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_USER_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_MAILBOX_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DCI_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FLASH_INTEGRITY_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SELFTEST_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TRNG_MONITOR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SECURE_LOCK_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_ATAMPDET_EMPGD] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_ATAMPDET_SUPGD] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SE_ICACHE_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_ECC_1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_BOD] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TEMPERATURE_SENSOR] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DPLL_LOCK_FAIL_LOW] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DPLL_LOCK_FAIL_HIGH] =
    SL_SE_TAMPER_LEVEL_FILTER;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS0] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS2] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS3] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS4] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS5] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS6] =
    SL_SE_TAMPER_LEVEL_PERMANENTLY_ERASE_OTP;
#endif

  // Overwrite tamper filter options in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_filter_period = SL_SE_TAMPER_FILTER_PERIOD_33S;
  otp_init.tamper_filter_threshold = SL_SE_TAMPER_FILTER_THRESHOLD_4;

  // Overwrite tamper flags in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_flags = 0;

  // Overwrite reset threshold in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_reset_threshold = 5;

  // Commit OTP settings. This command is only available once!
  print_error_cycle(sl_se_init_otp(&cmd_ctx, &otp_init), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
