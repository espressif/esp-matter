/***************************************************************************//**
 * @file
 * @brief Coulomb Counter examples functions
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
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "coulomb_app.h"
#include "nvm3_default.h"
#include "nvm3_default_config.h"
#include "sl_coulomb_counter.h"
#include "sl_cli.h"
#include "sl_cli_instances.h"
#include "sl_efp.h"
#include "sl_sleeptimer.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
// Delay to read and save coulomb counter total charge periodically.
#ifndef COULOMB_DELAY_MS
#define COULOMB_DELAY_MS         1000
#endif

// Maximum number of data objects saved
#define MAX_OBJECT_COUNT    1

// Use the default nvm3 handle from nvm3_default.h
#define NVM3_DEFAULT_HANDLE nvm3_defaultHandle

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/
static volatile bool coulomb_timeout = false;
sl_sleeptimer_timer_handle_t timer;

// Key to store total coulombs in NVM3.
static uint32_t key_total = NVM3_KEY_MIN;

static sl_coulomb_counter_output_mask_t mask;
static volatile float total_charge = 0;
static size_t len = sizeof(total_charge);
static Ecode_t err;

// Track previous and current charge in the coulomb counter.
static volatile float prev_cc_charge = 0;
static volatile float curr_cc_charge = 0;

// Buffer for reading from NVM3
static uint8_t buffer[NVM3_MAX_OBJECT_SIZE];

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle,
                       void *data)
{
  (void)handle;
  (void)data;
  coulomb_timeout = true;
}

static Ecode_t update_total(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  float tmp_curr_cc;
  float tmp_prev_cc;

  prev_cc_charge = curr_cc_charge;

  sl_coulomb_counter_read(&mask);
  curr_cc_charge = sl_coulomb_counter_get_total_charge();

  err = nvm3_readData(NVM3_DEFAULT_HANDLE, key_total, buffer, len);
  if (ECODE_NVM3_OK == err) {
    float *value = (float *) buffer;
    total_charge = value[0];
  }

  if (total_charge > (FLT_MAX - 1)) {
    return ECODE_NVM3_ERR_INT_SIZE_ERROR;
  }

  // Specify order of volatile accesses to avoid compiler warning
  tmp_curr_cc = curr_cc_charge;
  tmp_prev_cc = prev_cc_charge;
  total_charge = total_charge + (tmp_curr_cc - tmp_prev_cc);

  err = nvm3_writeData(NVM3_DEFAULT_HANDLE, key_total, (float *)&total_charge, len);
  CORE_EXIT_CRITICAL();

  return err;
}

static sl_status_t coulomb_calibrate(bool verbose)
{
  sl_coulomb_counter_calibration_status_t calib_status;
  sl_coulomb_counter_output_mask_t outputs_needing_calibration;

  // Calibrate coulomb counter
  outputs_needing_calibration = sl_coulomb_counter_outputs_need_calibration();

  if (outputs_needing_calibration == 0) {
    if (verbose) {
      printf("All outputs are already calibrated.\r\n");
    }

    return SL_STATUS_OK;
  }

  calib_status = sl_coulomb_counter_calibrate_init(outputs_needing_calibration);
  if (calib_status == SL_COULOMB_COUNTER_CALIBRATION_ERROR) {
    return SL_STATUS_FAIL;
  }

  if (verbose) {
    printf("Start calibration.\r\n");
  }

  do {
    calib_status = sl_coulomb_counter_calibrate();
    if (calib_status == SL_COULOMB_COUNTER_CALIBRATION_ERROR) {
      return SL_STATUS_FAIL;
    } else if (calib_status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2) {
      /*
       * When using outputs that have different configurations in EM2, the
       * coulomb counter driver requests the EM2 peak current configuration to
       * be set, in order to calibrate accurately.
       * If using the EFP, that means that BK_IPK must be set like so:
       *  BK_IPK = 4 * BK_IPK_EM2 + 2;
       *
       * Usually, EM2 peak current is lower than EM0 peak current, so make sure
       * that operations that draw a significant amount of current (like radio
       * transmissions) do not run while this lower peak current is set.
       *
       */
    } else if (calib_status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0) {
      /*
       * Restore peak current to its original value. If using the EFP, that
       * means restore BK_IPK.
       */
    } else {
      calib_status = sl_coulomb_counter_calibrate_wait();
    }
  } while (calib_status == SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS
           || calib_status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0
           || calib_status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2);

  if (verbose) {
    printf("Calibration is complete.\r\n");
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Return coulombs consumed since last boot.
 *
 * This function implements the CLI command 'coulomb_get' (see coulomb_app.slcp).
 ******************************************************************************/
void coulomb_app_get(void)
{
  sl_coulomb_counter_output_mask_t mask;
  float total_charge = 0;

  sl_coulomb_counter_read(&mask);
  total_charge = sl_coulomb_counter_get_total_charge();
  printf("Coulombs consumed since last boot: \r\n");
  printf("%f \r\n", total_charge);
}

/***************************************************************************//**
 * Calibrate coulomb counter.
 *
 * This function implements the CLI command 'coulomb_calibrate' (see coulomb_app.slcp)
 ******************************************************************************/
void coulomb_app_calibrate(void)
{
  sl_status_t status;

  status = sl_coulomb_counter_stop();
  if (status != SL_STATUS_OK) {
    printf("Failed to stop coulomb counter.\r\n");
  }

  status = coulomb_calibrate(true);
  if (status != SL_STATUS_OK) {
    printf("Failed to calibrate coulomb counter.\r\n");
  }

  prev_cc_charge = 0;
  curr_cc_charge = 0;

  // start coulomb counter
  status = sl_coulomb_counter_start();
  if (status != SL_STATUS_OK) {
    printf("Failed to start coulomb counter.\r\n");
  }
}

/***************************************************************************//**
 * Initialize coulomb counter example.
 ******************************************************************************/
void coulomb_app_init(void)
{
  sl_status_t status;

  status = coulomb_calibrate(false);
  if (status != SL_STATUS_OK) {
    printf("Failed to calibrate coulomb counter.\r\n");
  }

  // Start coulomb counter
  printf("Starting coulomb counter.\r\n");
  status = sl_coulomb_counter_start();
  if (status != SL_STATUS_OK) {
    printf("Failed to start coulomb counter.\r\n");
  }

  // Create timer for saving coulomb counter total charge periodically.
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        COULOMB_DELAY_MS,
                                        on_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

/***************************************************************************//**
 * Return coulombs consumed over device's lifetime.
 *
 * This function implements the CLI command 'coulomb_get_total'
 * (see coulomb_app.slcp)
 ******************************************************************************/
void coulomb_app_get_total(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  err = nvm3_readData(NVM3_DEFAULT_HANDLE, key_total, buffer, len);
  if (ECODE_NVM3_OK == err) {
    float *value = (float *) buffer;
    printf("Total coulombs consumed over device's lifetime: %f\r\n", value[0]);
  } else {
    printf("Error reading data\r\n");
  }

  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Read coulomb counters and update total value.
 *
 * This function implements the CLI command 'coulomb_update_total'
 * (see coulomb_app.slcp)
 ******************************************************************************/
void coulomb_app_update_total(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  if (ECODE_NVM3_OK == update_total()) {
    printf("Update total value to: %f\r\n", total_charge);
  } else {
    printf("Error updating data\r\n");
  }

  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Reset total counter to zero.
 *
 * This function implements the CLI command 'coulomb_reset_total'
 * (see coulomb_app.slcp)
 ******************************************************************************/
void coulomb_app_reset_total(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  total_charge = 0;
  prev_cc_charge = 0;
  curr_cc_charge = 0;

  if (ECODE_NVM3_OK == nvm3_writeData(NVM3_DEFAULT_HANDLE, key_total, (float *)&total_charge, len)) {
    printf("Reset total counter to zero.\r\n");
  } else {
    printf("Error while reseting.\r\n");
  }
  CORE_EXIT_CRITICAL();
}

void coulomb_app_process_action(void)
{
  if (coulomb_timeout == true) {
    update_total();
    coulomb_timeout = false;
  }
}
