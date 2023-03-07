/***************************************************************************//**
 * @file
 * @brief EFP (Energy Friendly PMIC) driver.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_assert.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_se.h"
#include "sl_efp.h"
#include "sl_i2cspm.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup extdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SL_EFP
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#define EFP_EFP01_I2C_ADDR   0x60

static sl_efp_handle_t host_efp = NULL;
static CORE_DECLARE_IRQ_STATE;

static sl_status_t sl_efp_decouple_handoff(sl_efp_handle_t handle, uint8_t bk_iri_con, uint8_t bk_ton_max, uint8_t bk_ipk);

/***************************************************************************//**
 * @brief
 *   Configure I2C GPIO pins.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] enable
 *   If true, configure I2C pins for I2C use. If false, pins are configured as
 *   GPIO pins to enable "direct mode" EM transfer mode.
 ******************************************************************************/
static void enable_i2c_pins(sl_efp_handle_t handle, bool enable)
{
  if (enable) {
    handle->in_direct_mode = false;
    GPIO_PinModeSet(handle->init_data.i2c_sda_port, handle->init_data.i2c_sda_pin,
                    gpioModeWiredAnd, 1);
    GPIO_PinModeSet(handle->init_data.i2c_scl_port, handle->init_data.i2c_scl_pin,
                    gpioModeWiredAnd, 1);

    // Enable I2C module and clock.
    CMU_ClockEnable(handle->i2c_clock, true);
    I2C_Enable(handle->init_data.i2c_peripheral, true);

#if defined(_SILICON_LABS_32B_SERIES_0)
    handle->init_data.i2c_peripheral->ROUTE |= I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN;
#elif defined(_SILICON_LABS_32B_SERIES_1)
    handle->init_data.i2c_peripheral->ROUTEPEN |= I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
#elif defined(_SILICON_LABS_32B_SERIES_2)
    GPIO->I2CROUTE[handle->i2c_peripheral_index].ROUTEEN |= GPIO_I2C_ROUTEEN_SDAPEN
                                                         | GPIO_I2C_ROUTEEN_SCLPEN;
#endif
  } else {
    handle->in_direct_mode = true;
    GPIO_PinModeSet(handle->init_data.i2c_sda_port, handle->init_data.i2c_sda_pin,
                    gpioModePushPull, 1);
    GPIO_PinModeSet(handle->init_data.i2c_scl_port, handle->init_data.i2c_scl_pin,
                    gpioModePushPull, 1);

#if defined(_SILICON_LABS_32B_SERIES_0)
    init_data.i2c_instance->ROUTE &= ~(I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN);
#elif defined(_SILICON_LABS_32B_SERIES_1)
    handle->init_data.i2c_peripheral->ROUTEPEN &= ~(I2C_ROUTEPEN_SDAPEN
                                                  | I2C_ROUTEPEN_SCLPEN);
#elif defined(_SILICON_LABS_32B_SERIES_2)
    GPIO->I2CROUTE[handle->i2c_peripheral_index].ROUTEEN &= ~(GPIO_I2C_ROUTEEN_SDAPEN
                                                            | GPIO_I2C_ROUTEEN_SCLPEN);
#endif

    // Disable I2C module and clock to save energy.
    I2C_Enable(handle->init_data.i2c_peripheral, false);
    CMU_ClockEnable(handle->i2c_clock, false);
  }
}

/***************************************************************************//**
 * @brief
 *   Perform a VDD measurement.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] msn_addr
 *   Register address of the most significant byte of measurement.
 *
 * @param[in] lsby_addr
 *   Register address of the least significant byte of measurement.
 *
 * @param[out] voltage_mv
 *   The result of the VDD measurement.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
static sl_status_t get_vdd(sl_efp_handle_t handle,
                           uint8_t msn_addr,
                           uint8_t lsby_addr,
                           uint16_t *voltage_mv)
{
  sl_status_t ret_val;
  uint16_t read_value = 0;
  uint8_t adc_cc_ctrl, tmp, ss_mode;

  // Get ADC_INTERVAL value.
  ret_val = sl_efp_read_register(handle, EFP01_ADC_CC_CTRL, &adc_cc_ctrl);

  if (ret_val == SL_STATUS_OK) {
    if ((adc_cc_ctrl & _EFP01_ADC_CC_CTRL_ADC_INTERVAL_MASK) == 0) {
      // Issue an ADC start command if automated measurements are not enabled.
      ret_val = sl_efp_write_register(handle,
                                      EFP01_CMD,
                                      _EFP01_CMD_ADC_START_MASK);
    }
  }

  // Wait at least 18.5 usec for the result, read of SS mode provides the delay.
  if (ret_val == SL_STATUS_OK) {
    ret_val = sl_efp_read_register(handle, EFP01_CC_MODE, &ss_mode);
  }

  if (ret_val == SL_STATUS_OK) {                    // Read result MSB.
    ret_val = sl_efp_read_register(handle, msn_addr, &tmp);
  }

  if (ret_val == SL_STATUS_OK) {                    // Read result LSB.
    read_value = tmp << 8;
    ret_val = sl_efp_read_register(handle, lsby_addr, &tmp);
  }

  if (ret_val == SL_STATUS_OK) {                    // Scale result correctly.
    read_value |= tmp;
    if (ss_mode & _EFP01_CC_MODE_SC_MODE_MASK) {
      // Result is a VDDA measurement.
      *voltage_mv = (uint16_t)lroundf(read_value * 1.01f);
    } else {
      // Result is a VDDB measurement.
      *voltage_mv = (uint16_t)lroundf(read_value * 1.49f);
    }
  }

  return ret_val;
}

/// @endcond

#if defined(_SILICON_LABS_32B_SERIES_2) && defined(EMU_VSCALE_PRESENT)
/***************************************************************************//**
 * @brief
 *   Reimplementation of the EMU function with the same name
 *
 * @details
 *   This function is called by EMU_VScaleEM01 to let EFP know that voltage scaling
 *   is requested.
 *
 * @param[in] voltage
 *   Voltage to set in EFP.
 ******************************************************************************/
void EMU_EFPEM01VScale(EMU_VScaleEM01_TypeDef voltage)
{
  /*
   * Apply voltage scaling only if EFP is powering DECOUPLE
   */
  if (host_efp != NULL && host_efp->init_data.handoff_size > 0) {
    if (voltage == emuVScaleEM01_HighPerformance) {
      sl_efp_set_vob_em01_voltage(host_efp, 1100);
    } else if (voltage == emuVScaleEM01_LowPower) {
      sl_efp_set_vob_em01_voltage(host_efp, 1000);
    }
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Reimplementation of the EMU function with the same name.
 *
 * @details
 *   This function will be called upon wakeup from the _WFI() instruction in
 *   EMU_EnterEM2/3() functions and will set the EFP in EM0 mode.
 *   A critical section is used to get predictable EFP energy mode sequence
 *   timing.
 ******************************************************************************/
void EMU_EFPEM23PostsleepHook(void)
{
  if (host_efp != NULL) {
    sl_efp_enter_em0(host_efp);
    CORE_EXIT_CRITICAL();
  }
}

/***************************************************************************//**
 * @brief
 *   Reimplementation of the EMU function with the same name.
 *
 * @details
 *   This function will be called prior to the _WFI() instruction in
 *   EMU_EnterEM2/3() functions and will set the EFP in EM2 mode.
 *   A critical section is used to get predictable EFP energy mode sequence
 *   timing.
 ******************************************************************************/
void EMU_EFPEM23PresleepHook(void)
{
  if (host_efp != NULL) {

    CORE_ENTER_CRITICAL();

    if ((host_efp->init_data.em_transition_mode == efp_em_transition_mode_gpio_bitbang)
     || (host_efp->init_data.em_transition_mode == efp_em_transition_mode_i2c)) {
      sl_efp_enter_em2(host_efp);
    } else if(host_efp->init_data.em_transition_mode == efp_em_transition_mode_emu) {
      if (!host_efp->in_direct_mode) {
        sl_efp_enable_direct_mode(host_efp);
      }
    }
  }
}

#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
/***************************************************************************//**
 * @brief
 *   Reimplementation of the EMU function with the same name.
 *
 * @details
 *   It will be called in EMU_EnterEM4(). It makes sure that direct mode is enabled
 *   so when MCU enters EM4, EMU will send a command to EFP to pass to EM4 as well.
 *
 * @warning
 *   EFP in EM4 mode is supported only on devices that have Direct Mode hardware
 *   support integrated in EMU (EFRxG22 and later devices). So this requires
 *   init_data.em_transition_mode == efp_em_transition_mode_emu.
 *
 * @note
 *   This function enables direct mode if necessary.
 ******************************************************************************/
void EMU_EFPEM4PresleepHook(void)
{
  if (host_efp != NULL) {
    if (host_efp->init_data.em_transition_mode == efp_em_transition_mode_emu) {

      if (!host_efp->in_direct_mode) {
        sl_efp_enable_direct_mode(host_efp);
      }

      EMU_EFPDriveDecoupleSet(false); // Clear EFPDRVDECOUPLE to re-enable internal LDO (if disabled)
    }
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Enable direct mode in EFP and EMU. And configure I2C pins for bitbang EM
 *   transfer mode.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @note
 *   This must be the last EFP driver function called prior to entering EM2/3
 *   via EMU_EnterEMx() calls. Any EFP API call that involves an I2C transfer
 *   will leave the I2C GPIO pins in I2C mode.
 *
 * @return
 *   SL_STATUS_OK on success.
 *   SL_STATUS_INVALID_MODE if current EM transfer mode is I2C.
 *   SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_enable_direct_mode(sl_efp_handle_t handle)
{
  sl_status_t ret_val = SL_STATUS_OK;

  if (handle->init_data.em_transition_mode == efp_em_transition_mode_i2c) {
    return SL_STATUS_INVALID_MODE;
  }
  if (handle->in_direct_mode) {
    return SL_STATUS_OK;
  }

  ret_val = sl_efp_write_register_field(handle,
                                      EFP01_EM_CRSREG_CTRL,
                                      1 << _EFP01_EM_CRSREG_CTRL_DIRECT_MODE_EN_SHIFT,
                                      _EFP01_EM_CRSREG_CTRL_DIRECT_MODE_EN_MASK,
                                      _EFP01_EM_CRSREG_CTRL_DIRECT_MODE_EN_SHIFT);

  if (handle->init_data.em_transition_mode == efp_em_transition_mode_gpio_bitbang) {
    enable_i2c_pins(handle, false);
  }
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
  else {
    EMU_EFPDirectModeEnable(true);
    handle->in_direct_mode = true;
  }
#endif

  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Set EFP in EM0 energy mode.
 *
 * @note
 *   This function enables direct mode if necessary.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_enter_em0(sl_efp_handle_t handle)
{
  sl_status_t ret_val = SL_STATUS_OK;

  if (handle->init_data.em_transition_mode == efp_em_transition_mode_gpio_bitbang) {
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
    EMU_EFPDirectModeEnable(false);
#endif
    if (!handle->in_direct_mode) {
      ret_val = sl_efp_enable_direct_mode(handle);
    }
    GPIO_PinOutSet(handle->init_data.i2c_scl_port, handle->init_data.i2c_scl_pin);
  } else if (handle->init_data.em_transition_mode == efp_em_transition_mode_i2c) {
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
    EMU_EFPDirectModeEnable(false);
#endif
    ret_val = sl_efp_write_register(handle, EFP01_EM_CRSREG_CTRL, handle->em_ctrl);
  }
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
  else if (handle->init_data.em_transition_mode == efp_em_transition_mode_emu) {
    // In this mode, EMU handles EM transition in EFP.
    return SL_STATUS_FAIL;
  }
#endif

  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Set EFP in EM2 energy mode.
 *
 * @note
 *   This function enables direct mode if necessary.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_enter_em2(sl_efp_handle_t handle)
{
  uint8_t tmp;
  sl_status_t ret_val = SL_STATUS_OK;

  if (handle->init_data.em_transition_mode == efp_em_transition_mode_gpio_bitbang) {
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
    EMU_EFPDirectModeEnable(false);
#endif
    if (!handle->in_direct_mode) {
      ret_val = sl_efp_enable_direct_mode(handle);
    }
    GPIO_PinOutSet(handle->init_data.i2c_scl_port, handle->init_data.i2c_scl_pin);
  } else if (handle->init_data.em_transition_mode == efp_em_transition_mode_i2c) {
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
    EMU_EFPDirectModeEnable(false);
#endif
    ret_val = sl_efp_read_register(handle, EFP01_EM_CRSREG_CTRL, &handle->em_ctrl);

    if (ret_val == SL_STATUS_OK) {
      tmp = (handle->em_ctrl & ~_EFP01_EM_CRSREG_CTRL_EM_SEL_MASK)
            | (_EFP01_EM_CRSREG_CTRL_EM_SEL_EM2
               << _EFP01_EM_CRSREG_CTRL_EM_SEL_SHIFT);
      ret_val = sl_efp_write_register(handle, EFP01_EM_CRSREG_CTRL, tmp);
    }

    if (!handle->in_direct_mode) {
      // Disable I2C module, pins and clock to save energy.
      enable_i2c_pins(handle, false);
    }
  }
#if defined(_EMU_CTRL_EFPDIRECTMODEEN_SHIFT)
  else if (handle->init_data.em_transition_mode == efp_em_transition_mode_emu) {
    // In this mode, EMU handles EM transition of EFP.
    return SL_STATUS_FAIL;
  }
#endif

  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Measure average VDD.
 *
 * @note
 *   This function measures VDDA or VDDB depending of the voltage on the
 *   VDDA pin after power on. If VDDA is powered this function returns a VDDA
 *   reading, otherwise a VDDB reading is returned.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[out] voltage_mv
 *   Average VDD voltage expressed in millivolts.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_get_vdd_avg(sl_efp_handle_t handle, uint16_t *voltage_mv)
{
  return get_vdd(handle, EFP01_VDD_AVG_MSN, EFP01_VDD_AVG_LSBY, voltage_mv);
}

/***************************************************************************//**
 * @brief
 *   Measure maximum VDD.
 *
 * @note
 *   This function measures VDDA or VDDB depending of the voltage on the
 *   VDDA pin after power on. If VDDA is powered this function returns a VDDA
 *   reading, otherwise a VDDB reading is returned.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[out] voltage_mv
 *   Maximum VDD voltage expressed in millivolts.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_get_vdd_max(sl_efp_handle_t handle, uint16_t *voltage_mv)
{
  return get_vdd(handle, EFP01_VDD_MAX_MSN, EFP01_VDD_MAX_LSBY, voltage_mv);
}

/***************************************************************************//**
 * @brief
 *   Measure minimum VDD.
 *
 * @note
 *   This function measures VDDA or VDDB depending of the voltage on the
 *   VDDA pin after power on. If VDDA is powered this function returns a VDDA
 *   reading, otherwise a VDDB reading is returned.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[out] voltage_mv
 *   Minimum VDD voltage expressed in miilivolts.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_get_vdd_min(sl_efp_handle_t handle, uint16_t *voltage_mv)
{
  return get_vdd(handle, EFP01_VDD_MIN_MSN, EFP01_VDD_MIN_LSBY, voltage_mv);
}

/***************************************************************************//**
 * @brief
 *   Deinitialize EFP driver and underlying EFP hardware.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @return
 *   SL_STATUS_OK if successful or an error code otherwise.
 ******************************************************************************/
sl_status_t sl_efp_deinit(sl_efp_handle_t handle)
{
  if (host_efp == handle) {
    host_efp = NULL;
  }
  enable_i2c_pins(handle, false);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Turn the internal EMU LDO regulator that supplies DECOUPLE pin on or off.
 *
 * @note
 *   Make sure that an external source supply the DECOUPLE pin before turning the
 *   LDO off.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] enable
 *   True to turn LDO on, false to turn off.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_NOT_SUPPORTED, or SL_STATUS_FAIL if not a host
 *   EFP instance.
 ******************************************************************************/
sl_status_t sl_efp_emu_ldo_enable(sl_efp_handle_t handle, bool enable)
{
  if (handle->init_data.is_host_efp) {
#if defined(_SILICON_LABS_32B_SERIES_1)
    if (enable) {
      *(volatile uint32_t*)0x400E303C &= ~0x00000040UL;
    } else {
      *(volatile uint32_t*)0x400E303C |= 0x00000040UL;
    }
    return SL_STATUS_OK;
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_PROTECTED_REGISTER |
                                              SE_COMMAND_OPTION_WRITE);
    SE_addParameter(&command, 0x4000408c);    // Address
    SE_addParameter(&command, 1 << 14);       // Mask
    if (enable) {
      SE_addParameter(&command, 0);           // Value
    } else {
      SE_addParameter(&command, 1 << 14);     // Value
    }

    SE_executeCommand(&command);
    if (SE_readCommandResponse() != SE_RESPONSE_OK) {
      return SL_STATUS_FAIL;
    }
    return SL_STATUS_OK;
#elif defined(_EMU_CTRL_EFPDRVDECOUPLE_SHIFT)

    EMU_EFPDriveDecoupleSet(!enable);

    return SL_STATUS_OK;
#else
#warning "Internal LDO control not implemented for this device family."
    EFM_ASSERT(false);
    return SL_STATUS_NOT_SUPPORTED;
#endif

  } else {
    return SL_STATUS_FAIL; // This is not a host EFP instance.
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize EFP driver and configure underlying EFP hardware.
 *
 * @note This function must not be called from interrupt context.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] init
 *   Initialization data structure.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors, or SL_STATUS_FAIL
 *   on illegal initialization.
 ******************************************************************************/
sl_status_t sl_efp_init(sl_efp_handle_t handle, const sl_efp_init_data_t *init)
{
  int i;
  uint8_t *p;
  uint8_t tmp = 0;
  sl_status_t ret_val;

  if (init->is_host_efp) {
    if (host_efp == NULL) {
      host_efp = handle;
    } else {
      return SL_STATUS_FAIL;  // Only allow one host EFP instance.
    }
  }

  // Keep a copy of init data in the handle.
  handle->init_data = *init;

  // Assemble an I2C SPM initialization structure.
  I2CSPM_Init_TypeDef i2c_init = {
    .port              = init->i2c_peripheral,
    .sclPort           = init->i2c_scl_port,
    .sclPin            = init->i2c_scl_pin,
    .sdaPort           = init->i2c_sda_port,
    .sdaPin            = init->i2c_sda_pin,
#if defined(_SILICON_LABS_32B_SERIES_0)
    .portLocation;   = init->i2c_port_location,
#elif defined(_SILICON_LABS_32B_SERIES_1)
    .portLocationScl = init->i2c_scl_port_location,
    .portLocationSda = init->i2c_sda_port_location,
#endif
    .i2cRefFreq        = 0,
    .i2cMaxFreq        = I2C_FREQ_FASTPLUS_MAX,
    .i2cClhr           = i2cClockHLRStandard
  };

  if ( false ) {
#if defined(I2C0)
  } else if (init->i2c_peripheral == I2C0) {
    handle->i2c_clock = cmuClock_I2C0;
#if defined(_SILICON_LABS_32B_SERIES_2)
    handle->i2c_peripheral_index = 0;
#endif
#endif
#if defined(I2C1)
  } else if (init->i2c_peripheral == I2C1) {
    handle->i2c_clock = cmuClock_I2C1;
#if defined(_SILICON_LABS_32B_SERIES_2)
    handle->i2c_peripheral_index = 1;
#endif
#endif
#if defined(I2C2)
  } else if (init->i2c_peripheral == I2C2) {
    handle->i2c_clock = cmuClock_I2C2;
#if defined(_SILICON_LABS_32B_SERIES_2)
    handle->i2c_peripheral_index = 2;
#endif
#endif
  }

  // Initialize I2C interface.
  I2CSPM_Init(&i2c_init);
  handle->in_direct_mode = false;

  /* Reset EFP01 to its default, out-of-reset register configuration.
     Note that EFP01's registers will only be reset to defaults by one of the below
     events:
     1) Cycling power to EFP01.  Forces a EFP POR Reset.
     2) Writing a 1 to EFP01's CMD.OTPREREAD bit. Causes the OTP defaults to be reloaded
     3) Writing a 1 to EFP01's CMD.RESET bit. Causes the EFP01 to reset its registers to 0,
      then immediately reload OTP defaults. Not recommended for use (see notes on sl_efp_reset() function)

     EFP01's registers will *not* be reset during EFR32 debug
     (e.g, after downloading an image or issuing a reset) or when the host processor is reset.
     When debugging, it may be desirable to force EFP01 to out-of-reset defaults
     during initialization - otherwise EFP01 will retain all register configurations
     written since the last system power cycle or EFP01 reset.

     Warning! When powering DECOUPLE, the internal LDO needs to be re-enabled *before* resetting EFP to default
     EFP will default to DECOUPLE DCDC off, so a brown-out will occur if the internal LDO is not enabled. */
  if (init->reset_to_default) {
    sl_efp_emu_ldo_enable(handle, true);
    ret_val = sl_efp_reset_to_default(handle);
  }

  // Enable I2C bus internal pullups and slow mode.
  // Note: Can be removed when all old EFP boards are obsoleted.
  ret_val = sl_efp_write_register(handle, EFP01_I2C_CTRL, 0x09);

  enable_i2c_pins(handle, true);

  // Do configuration register writes if a configuration is defined.
  if (init->config_size > 0) {
    i = init->config_size;
    p = init->config_data;
    while (i && (ret_val == SL_STATUS_OK)) {
      ret_val = sl_efp_write_register(handle, *p, *(p + 1));
      i--;
      p += 2;
    }
  }

#if defined(EMU_CTRL_EFPDRVDVDD)
    EMU_EFPDriveDvddSet(true);
#endif

  // Configure GPIO pin as EFP IRQ input.
  if ((init->irq_pin_mode != efp_irq_pin_disabled) && (ret_val == SL_STATUS_OK)) {
    // Clear interrupt flags.
    ret_val  = sl_efp_read_register(handle, EFP01_STATUS_G, &tmp);
    ret_val |= sl_efp_write_register(handle, EFP01_STATUS_G, tmp);
    ret_val |= sl_efp_read_register(handle, EFP01_STATUS_V, &tmp);
    ret_val |= sl_efp_write_register(handle, EFP01_STATUS_V, tmp);
#if defined(EMU_CTRL_EFPDRVDVDD)
    if (init->irq_pin_mode == efp_irq_pin_emu) {
      if (ret_val == SL_STATUS_OK) {
          GPIO_PinModeSet(init->irq_port, init->irq_pin, gpioModeInput, 1);
      }

      /* Get and clear all pending EMU EFP interrupts */
      uint32_t interruptMask = EMU_EFPIntGet();
      EMU_EFPIntClear(interruptMask);
      EMU_EFPIntEnable(1U << _EMU_EFPIEN_EFPIEN_SHIFT); // Set EFPIEN to enable EMU IRQ
    }
# else
    if (ret_val == SL_STATUS_OK) {
      GPIO_PinModeSet(init->irq_port, init->irq_pin, gpioModeInputPull, 1);
    }
#endif
  }

  // Prepare prelim workaround to speed-up EM2 wakeup timing.
  ret_val = sl_efp_read_register(handle, EFP01_EM_CRSREG_CTRL, &handle->em_ctrl);
  handle->em_ctrl &= ~_EFP01_EM_CRSREG_CTRL_EM_SEL_MASK;

  // Do decouple handoff routine if handoff parameters are defined.
  if (init->handoff_size > 0) {
    p = init->handoff_parameters;
    uint8_t iri_con = *(p);
    p++;
    uint8_t ton_max = *(p);
    p++;
    uint8_t bk_ipk = *(p);
    ret_val = sl_efp_decouple_handoff(handle, iri_con, ton_max, bk_ipk);
  }

  // Prepare for "direct mode" EM transitions.
  if ((init->em_transition_mode == efp_em_transition_mode_gpio_bitbang
    || init->em_transition_mode == efp_em_transition_mode_emu)
    && (ret_val == SL_STATUS_OK)) {
    ret_val = sl_efp_enable_direct_mode(handle);
  }

  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Perform DECOUPLE LDO->DCDC Handoff sequence
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] bk_iri_con
 *   Desired current limit
 *
 * @param[in] bk_ton_max
 *   Desired TON MAX.
 *
 * @param[in] bk_ipk
 *   Desired peak current.
 ******************************************************************************/
static sl_status_t sl_efp_decouple_handoff(sl_efp_handle_t handle, uint8_t bk_iri_con, uint8_t bk_ton_max, uint8_t bk_ipk)
{
  sl_status_t status;
  sl_status_t ret_val;
  uint8_t tmp;

  // Set VOB target to higher level to guarantee it will overdrive the EFR32 DECOUPLE LDO output
#if defined(EMU_VSCALE_PRESENT)
  // Read EM01 VSCALE setting
  EMU_VScaleEM01_TypeDef em01VScale = EMU_VScaleGet();
  if (em01VScale == emuVScaleEM01_LowPower) {
#if defined(_SILICON_LABS_32B_SERIES_2)
    sl_efp_set_vob_em01_voltage(handle, 1050);
#else
    sl_efp_set_vob_em01_voltage(handle, 950);
#endif
  } else {
    sl_efp_set_vob_em01_voltage(handle, 1150);
  }
#elif defined(_SILICON_LABS_32B_SERIES_1)
  sl_efp_set_vob_em01_voltage(handle, 1130);
#else // For Series 1.  Note Series 1 Voltage scaling not supported in here at the moment
  sl_efp_set_vob_em01_voltage(handle, 1130);
#endif

  // Set peak current to minimum
  sl_efp_write_register_field(handle, EFP01_BK_IPK, 0,
                              _EFP01_BK_IPK_BK_IPK_MASK,
                              _EFP01_BK_IPK_BK_IPK_SHIFT);

  // Set Ton time to minimum
  sl_efp_write_register_field(handle, EFP01_BK_CTRL1,
                              1,
                              _EFP01_BK_CTRL1_BK_TON_MAX_MASK,
                              _EFP01_BK_CTRL1_BK_TON_MAX_SHIFT);

  // Set current limit to maximum
  sl_efp_write_register_field(handle, EFP01_BK_CTRL2,
                              15,
                              _EFP01_BK_CTRL2_BK_IRI_CON_MASK,
                              _EFP01_BK_CTRL2_BK_IRI_CON_SHIFT);

  // Enable VOB DCDC in buck only mode
  sl_efp_set_vob_mode(handle, efp_vob_mode_buck);

  // Make sure VOB output is ready before turning off internal LDO regulator.
  do {
    status = sl_efp_read_register(handle, EFP01_STATUS_LIVE, &tmp);
  } while (((tmp & _EFP01_STATUS_LIVE_VOB_INREG_LIVE_MASK) == 0)
           || (status != SL_STATUS_OK));

  // Set desired peak current
  sl_efp_write_register_field(handle, EFP01_BK_IPK, bk_ipk,
                              _EFP01_BK_IPK_BK_IPK_MASK,
                              _EFP01_BK_IPK_BK_IPK_SHIFT);

  // Set desired TON MAX
  sl_efp_write_register_field(handle, EFP01_BK_CTRL1,
                              bk_ton_max,
                              _EFP01_BK_CTRL1_BK_TON_MAX_MASK,
                              _EFP01_BK_CTRL1_BK_TON_MAX_SHIFT);

  // Set desired current limit
  sl_efp_write_register_field(handle, EFP01_BK_CTRL2,
                              bk_iri_con,
                              _EFP01_BK_CTRL2_BK_IRI_CON_MASK,
                              _EFP01_BK_CTRL2_BK_IRI_CON_SHIFT);

  // Turn off internal EFR32 LDO regulator.
  ret_val = sl_efp_emu_ldo_enable(handle, false);

  if (ret_val != SL_STATUS_OK){
    return ret_val;
  }

  // If voltage scaling is supported, set desired EM23 VOB voltage based of the
  // EMU configuration
#if defined(EMU_VSCALE_PRESENT)
  // Set desired EM01 VOB voltage
  if (em01VScale == emuVScaleEM01_LowPower) {
#if defined(_SILICON_LABS_32B_SERIES_2)
    ret_val = sl_efp_set_vob_em01_voltage(handle, 1000);
#else
    ret_val = sl_efp_set_vob_em01_voltage(handle, 900);
#endif
  } else {
    ret_val = sl_efp_set_vob_em01_voltage(handle, 1100);
  }
  // Read EM23VSCALE setting
  EMU_VScaleEM23_TypeDef em23VScale = (EMU_VScaleEM23_TypeDef)((EMU->CTRL & _EMU_CTRL_EM23VSCALE_MASK)
                                                               >> _EMU_CTRL_EM23VSCALE_SHIFT);
  if (em23VScale == emuVScaleEM23_LowPower) {
    ret_val = sl_efp_set_vob_em23_voltage(handle, 900);
  } else {
    ret_val = sl_efp_set_vob_em23_voltage(handle, 1100);
  }
#else
  ret_val = sl_efp_set_vob_em01_voltage(handle, 1100);
  ret_val = sl_efp_set_vob_em23_voltage(handle, 1100);
#endif
  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Read EFP OTP register.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] addr
 *   Register address.
 *
 * @param[out] data
 *   The value read from the OTP register.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_read_otp(sl_efp_handle_t handle, uint8_t addr, uint8_t *data)
{
  uint8_t read_value;
  sl_status_t ret_val;

  ret_val = sl_efp_write_register(handle, 0x0F, 0x06);  // Enable OTP access.

  if (ret_val == SL_STATUS_OK) { // Need a few us delay, use reg. read as delay.
    ret_val = sl_efp_read_register(handle, EFP01_OTP_STATCTRL, &read_value);
  }

  if (ret_val == SL_STATUS_OK) {                        // Setup OTP address.
    ret_val = sl_efp_write_register(handle, EFP01_OTP_ADDR, addr & 0x7F);
  }

  if (ret_val == SL_STATUS_OK) {                        // Issue OTP read command.
    ret_val = sl_efp_write_register_field(handle,
                                          EFP01_OTP_STATCTRL,
                                          1,
                                          _EFP01_OTP_STATCTRL_CMD_READOTP_MASK,
                                          _EFP01_OTP_STATCTRL_CMD_READOTP_SHIFT);
  }

  if (ret_val == SL_STATUS_OK) { // Need a few us delay, use reg. read as delay.
    ret_val = sl_efp_read_register(handle, EFP01_OTP_STATCTRL, &read_value);
  }

  if (ret_val == SL_STATUS_OK) {                        // Readout OTP data.
    ret_val = sl_efp_read_register(handle, EFP01_OTP_RDATA, &read_value);
  }

  if (ret_val == SL_STATUS_OK) {                        // Disable OTP access.
    *data = read_value;                                 // OTP value.
    ret_val = sl_efp_write_register(handle, 0x0F, 0x00);
  }

  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Read EFP control register.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] addr
 *   Register address.
 *
 * @param[out] data
 *   The value read from the register.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_read_register(sl_efp_handle_t handle, uint8_t addr, uint8_t *data)
{
  uint8_t read_value;

  I2C_TransferSeq_TypeDef seq = {
    .addr   = EFP_EFP01_I2C_ADDR,
    .flags  = I2C_FLAG_WRITE_READ,
    .buf[0] = { .data = &addr, .len = 1 },
    .buf[1] = { .data = &read_value, .len = 1 }
  };

  if (handle->in_direct_mode) {
    enable_i2c_pins(handle, true);
  }

  if (I2CSPM_Transfer(handle->init_data.i2c_peripheral, &seq) != i2cTransferDone) {
    EFM_ASSERT(false);
    return SL_STATUS_IO;
  }
  *data = read_value;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Read EFP control register field.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] addr
 *   Register address.
 *
 * @param[out] data
 *   The value read from the register field.
 *
 * @param[in] mask
 *   Bit mask corresponding to target bit field.
 *
 * @param[in] pos
 *   Bit number of least significant bit of target bit field.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_read_register_field(sl_efp_handle_t handle,
                                       uint8_t addr,
                                       uint8_t *data,
                                       uint8_t mask,
                                       uint8_t pos)
{
  uint8_t tmp;
  sl_status_t ret_val;

  ret_val = sl_efp_read_register(handle, addr, &tmp);
  if (ret_val == SL_STATUS_OK) {
    *data = (tmp & mask) >> pos;
  }
  return ret_val;
}

/***************************************************************************//**
 * @brief
 *   Reset EFP. Perform a full reset of the EFP, this is equivalent to a power
 *   on reset.
 *
 * @warning
 *   When using the CMD.RESET bit, all the EFP's internal registers will
 *   be momentarily reset to 0's before being overwritten with the OTP defaults.
 *   This means that any enabled DCDCs or LDOs will be momentarily disabled, then
 *   re-enabled.
 *   If the load current on that output is large enough during this reset, the
 *   voltage may drop and cause a brownout.
 *   Recommend use of the sl_efp_reset_to_default() function instead, which
 *   simply reloads the OTP defaults without clearning all the registers to 0 first.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_reset(sl_efp_handle_t handle)
{
  return sl_efp_write_register(handle,EFP01_CMD, _EFP01_CMD_RESET_MASK);
}

/***************************************************************************//**
 * @brief
 *   Reset EFP to default settings. This function is recommended over
 *   sl_efp_reset() to reset EFP.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_reset_to_default(sl_efp_handle_t handle)
{
  return sl_efp_write_register(handle,EFP01_CMD, _EFP01_CMD_OTP_REREAD_MASK);
}

/***************************************************************************//**
 * @brief
 *   Set EM transition mode.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] mode
 *   New EFP Energy Mode (EM) transition mode. See @ref sl_efp_em_transition_mode_t
 *   for valid modes.
 *
 * @return
 *   SL_STATUS_OK.
 ******************************************************************************/
sl_status_t sl_efp_set_em_transition_mode(sl_efp_handle_t handle,
                                          sl_efp_em_transition_mode_t mode)
{
  if (handle->init_data.em_transition_mode == mode) {
    return SL_STATUS_OK;
  }
  handle->init_data.em_transition_mode = mode;
  if (mode == efp_em_transition_mode_gpio_bitbang ||
      mode == efp_em_transition_mode_emu) {
    // We need to reconfigure direct mode when passing from EMU mode to gpio mode
    // and vice versa.
    handle->in_direct_mode = false;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA EM01 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] ipk
 *   Target peak current setting.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voa_em01_ipk(sl_efp_handle_t handle, uint8_t ipk)
{
  EFM_ASSERT(ipk < (uint8_t)(_EFP01_BB_IPK_BB_IPK_MASK >> _EFP01_BB_IPK_BB_IPK_SHIFT));

  return sl_efp_write_register_field(handle, EFP01_BB_IPK, ipk,
                                     _EFP01_BB_IPK_BB_IPK_MASK,
                                     _EFP01_BB_IPK_BB_IPK_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA EM23 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] ipk
 *   Target peak current setting.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voa_em23_ipk(sl_efp_handle_t handle, uint8_t ipk)
{
  EFM_ASSERT(ipk < (uint8_t)(_EFP01_BB_IPK_BB_IPK_EM2_MASK >> _EFP01_BB_IPK_BB_IPK_EM2_SHIFT));

  return sl_efp_write_register_field(handle, EFP01_BB_IPK, ipk,
                                     _EFP01_BB_IPK_BB_IPK_EM2_MASK,
                                     _EFP01_BB_IPK_BB_IPK_EM2_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA EM01 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] current_ma
 *   Target peak current expressed in milliamperes.
 *
 * @param[in] vddb_mv
 *   VDDB expressed in millivolt.
 *
 * @param[in] voa_mv
 *   VOA expressed in millivolt.
 *
 * @param[in] inductor_nh
 *   Inductor size expressed in nanohenry.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 *
 * @deprecated
 ******************************************************************************/
sl_status_t sl_efp_set_voa_em01_peak_current(sl_efp_handle_t handle,
                                             unsigned int current_ma,
                                             unsigned int vddb_mv,
                                             unsigned int voa_mv,
                                             unsigned int inductor_nh)
{
  long l;
  float f;

  f = (current_ma - 90.0f - (25.0f * (vddb_mv - voa_mv) / inductor_nh)) / 9.0f;
  l = lroundf(f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_BB_IPK_BB_IPK_MASK >> _EFP01_BB_IPK_BB_IPK_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_BB_IPK, (uint8_t)l,
                                     _EFP01_BB_IPK_BB_IPK_MASK,
                                     _EFP01_BB_IPK_BB_IPK_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA EM23 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] current_ma
 *   Target peak current expressed in milliamperes.
 *
 * @param[in] vddb_mv
 *   VDDB expressed in millivolt.
 *
 * @param[in] voa_mv
 *   VOA expressed in millivolt.
 *
 * @param[in] inductor_nh
 *   Inductor size expressed in nanohenry.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 *
 * @deprecated
 ******************************************************************************/
sl_status_t sl_efp_set_voa_em23_peak_current(sl_efp_handle_t handle,
                                             unsigned int current_ma,
                                             unsigned int vddb_mv,
                                             unsigned int voa_mv,
                                             unsigned int inductor_nh)
{
  long l;
  float f;

  f = (current_ma - 108.0f - (25.0f * (vddb_mv - voa_mv) / inductor_nh)) / 36.0f;
  l = lroundf(f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_BB_IPK_BB_IPK_EM2_MASK >> _EFP01_BB_IPK_BB_IPK_EM2_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_BB_IPK, (uint8_t)l,
                                     _EFP01_BB_IPK_BB_IPK_EM2_MASK,
                                     _EFP01_BB_IPK_BB_IPK_EM2_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA mode.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] mode
 *   Regulator mode.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voa_mode(sl_efp_handle_t handle, sl_efp_voa_mode_t mode)
{
  return sl_efp_write_register_field(handle, EFP01_BB_CTRL3, mode,
                                     _EFP01_BB_CTRL3_BB_MODE_MASK,
                                     _EFP01_BB_CTRL3_BB_MODE_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set VOA SW (switched output) mode.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] mode
 *   VOA SW mode.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voa_sw_mode(sl_efp_handle_t handle, sl_efp_voa_sw_mode_t mode)
{
  return sl_efp_write_register_field(handle, EFP01_BIAS_SW, mode,
                                     _EFP01_BIAS_SW_VOA_SW_CON_MASK,
                                     _EFP01_BIAS_SW_VOA_SW_CON_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOA output voltage.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] voltage_mv
 *   Voltage in millivolt.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voa_voltage(sl_efp_handle_t handle, unsigned int voltage_mv)
{
  long l;

  l = lroundf((voltage_mv - 1737.4f) / 30.6f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_VOA_V_VOA_V_MASK >> _EFP01_VOA_V_VOA_V_SHIFT);

  return sl_efp_write_register(handle, EFP01_VOA_V, (uint8_t)l);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM01 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] ipk
 *   Target peak current setting.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em01_ipk(sl_efp_handle_t handle, uint8_t ipk)
{
  EFM_ASSERT(ipk < (uint8_t)(_EFP01_BK_IPK_BK_IPK_MASK >> _EFP01_BK_IPK_BK_IPK_SHIFT));

  return sl_efp_write_register_field(handle, EFP01_BK_IPK, ipk,
                                     _EFP01_BK_IPK_BK_IPK_MASK,
                                     _EFP01_BK_IPK_BK_IPK_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM01 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] current_ma
 *   Target peak current expressed in milliamperes.
 *
 * @param[in] vddb_mv
 *   VDDB expressed in millivolt.
 *
 * @param[in] vob_mv
 *   VOB expressed in millivolt.
 *
 * @param[in] inductor_nh
 *   Inductor size expressed in nanohenry.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 *
 * @deprecated
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em01_peak_current(sl_efp_handle_t handle,
                                             unsigned int current_ma,
                                             unsigned int vddb_mv,
                                             unsigned int vob_mv,
                                             unsigned int inductor_nh)
{
  long l;
  float f;

  f = (current_ma - 90.0f - (25.0f * (vddb_mv - vob_mv) / inductor_nh)) / 9.0f;
  l = lroundf(f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_BK_IPK_BK_IPK_MASK >> _EFP01_BK_IPK_BK_IPK_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_BK_IPK, (uint8_t)l,
                                     _EFP01_BK_IPK_BK_IPK_MASK,
                                     _EFP01_BK_IPK_BK_IPK_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM01 output voltage.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] voltage_mv
 *   Voltage in millivolt.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em01_voltage(sl_efp_handle_t handle, unsigned int voltage_mv)
{
  long l;

  l = lroundf((voltage_mv - 809.5f) / 22.3f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_VOB_EM0_V_VOB_EM0_V_MASK >> _EFP01_VOB_EM0_V_VOB_EM0_V_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_VOB_EM0_V, (uint8_t)l,
                                     _EFP01_VOB_EM0_V_VOB_EM0_V_MASK,
                                     _EFP01_VOB_EM0_V_VOB_EM0_V_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM23 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] ipk
 *   Target peak current setting.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em23_ipk(sl_efp_handle_t handle, uint8_t ipk)
{
  EFM_ASSERT(ipk < (uint8_t)(_EFP01_BK_IPK_BK_IPK_EM2_MASK >> _EFP01_BK_IPK_BK_IPK_EM2_SHIFT));

  return sl_efp_write_register_field(handle, EFP01_BK_IPK, ipk,
                                     _EFP01_BK_IPK_BK_IPK_EM2_MASK,
                                     _EFP01_BK_IPK_BK_IPK_EM2_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM23 peak output current.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] current_ma
 *   Target peak current expressed in milliamperes.
 *
 * @param[in] vddb_mv
 *   VDDB expressed in millivolt.
 *
 * @param[in] vob_mv
 *   VOB expressed in millivolt.
 *
 * @param[in] inductor_nh
 *   Inductor size expressed in nanohenry.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 *
 * @deprecated
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em23_peak_current(sl_efp_handle_t handle,
                                             unsigned int current_ma,
                                             unsigned int vddb_mv,
                                             unsigned int vob_mv,
                                             unsigned int inductor_nh)
{
  long l;
  float f;

  f = (current_ma - 108.0f - (25.0f * (vddb_mv - vob_mv) / inductor_nh)) / 36.0f;
  l = lroundf(f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_BK_IPK_BK_IPK_EM2_MASK >> _EFP01_BK_IPK_BK_IPK_EM2_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_BK_IPK, (uint8_t)l,
                                     _EFP01_BK_IPK_BK_IPK_EM2_MASK,
                                     _EFP01_BK_IPK_BK_IPK_EM2_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB EM23 output voltage.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] voltage_mv
 *   Voltage in millivolt.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_vob_em23_voltage(sl_efp_handle_t handle, unsigned int voltage_mv)
{
  long l;

  l = lroundf((voltage_mv - 809.5f) / 22.3f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_VOB_EM2_V_VOB_EM2_V_MASK >> _EFP01_VOB_EM2_V_VOB_EM2_V_SHIFT);

  return sl_efp_write_register(handle, EFP01_VOB_EM2_V, (uint8_t)l);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOB mode.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] mode
 *   Regulator mode.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_vob_mode(sl_efp_handle_t handle, sl_efp_vob_mode_t mode)
{
  return sl_efp_write_register_field(handle, EFP01_BK_CTRL1, mode,
                                     _EFP01_BK_CTRL1_BK_MODE_MASK,
                                     _EFP01_BK_CTRL1_BK_MODE_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Set regulator VOC output voltage.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] voltage_mv
 *   Voltage in millivolt.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_set_voc_voltage(sl_efp_handle_t handle, unsigned int voltage_mv)
{
  long l;

  l = lroundf((voltage_mv - 1717.8f) / 30.5f);
  l = SL_MAX(l, 0);
  l = SL_MIN(l, _EFP01_VOC_V_VOC_V_MASK >> _EFP01_VOC_V_VOC_V_SHIFT);

  return sl_efp_write_register_field(handle, EFP01_VOC_V, (uint8_t)l,
                                     _EFP01_VOC_V_VOC_V_MASK,
                                     _EFP01_VOC_V_VOC_V_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Write EFP control register.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] addr
 *   Register address.
 *
 * @param[in] data
 *   The value to write to the register.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_write_register(sl_efp_handle_t handle, uint8_t addr, uint8_t data)
{
  I2C_TransferSeq_TypeDef seq = {
    .addr   = EFP_EFP01_I2C_ADDR,
    .flags  = I2C_FLAG_WRITE_WRITE,
    .buf[0] = { .data = &addr, .len = 1 },
    .buf[1] = { .data = &data, .len = 1 }
  };

  if (handle->in_direct_mode) {
    enable_i2c_pins(handle, true);
  }

  if (I2CSPM_Transfer(handle->init_data.i2c_peripheral, &seq) != i2cTransferDone) {
    EFM_ASSERT(false);
    return SL_STATUS_IO;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Write EFP control register field.
 *
 * @note
 *   This function performs a read-modify-write of a EFP register.
 *
 * @param[in] handle
 *   EFP instance handle.
 *
 * @param[in] addr
 *   Register address.
 *
 * @param[in] data
 *   The value to write to the register field.
 *
 * @param[in] mask
 *   Bit mask corresponding to target bit field.
 *
 * @param[in] pos
 *   Bit number of least significant bit of target bit field.
 *
 * @return
 *   SL_STATUS_OK or SL_STATUS_IO on I2C transfer errors.
 ******************************************************************************/
sl_status_t sl_efp_write_register_field(sl_efp_handle_t handle,
                                        uint8_t addr,
                                        uint8_t data,
                                        uint8_t mask,
                                        uint8_t pos)
{
  uint8_t tmp;
  sl_status_t ret_val;

  ret_val = sl_efp_read_register(handle, addr, &tmp);
  if (ret_val == SL_STATUS_OK) {
    tmp = (tmp & ~mask) | ((data << pos) & mask);
    ret_val = sl_efp_write_register(handle, addr, tmp);
  }
  return ret_val;
}
/** @} (end addtogroup SL_EFP) */
/** @} (end addtogroup extdrv) */
