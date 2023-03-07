/***************************************************************************//**
 * @file
 * @brief Digital to Analog Converter (VDAC) Peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_vdac.h"
#if defined(VDAC_COUNT) && (VDAC_COUNT > 0)
#include "em_cmu.h"

/***************************************************************************//**
 * @addtogroup vdac
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Validation of the VDAC channel for assert statements. */
#define VDAC_CH_VALID(ch)    ((ch) <= 1)

/** A maximum VDAC clock. */
#define VDAC_MAX_CLOCK            1000000

/** The maximum clock frequency of the internal clock oscillator, 10 MHz + 20%. */
#define VDAC_INTERNAL_CLOCK_FREQ  12000000

/** @endcond */

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined(_VDAC_EN_MASK)
static void VDAC_DisableModule(VDAC_TypeDef* vdac)
{
  while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
  }

  /* Wait for all synchronizations to finish */
  if (vdac->EN & VDAC_EN_EN) {
    vdac->CMD = _VDAC_CMD_CH0DIS_MASK;
    while (vdac->STATUS & (VDAC_STATUS_CH0ENS)) {
    }

    vdac->CMD = _VDAC_CMD_CH1DIS_MASK;
    while (vdac->STATUS & (VDAC_STATUS_CH1ENS)) {
    }

#if defined(_VDAC_CMD_CH0FIFOFLUSH_MASK)
    while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
    }

    vdac->CMD = VDAC_CMD_CH0FIFOFLUSH | VDAC_CMD_CH1FIFOFLUSH;

    while (vdac->STATUS & (VDAC_STATUS_SYNCBUSY | VDAC_STATUS_CH0FIFOFLBUSY | VDAC_STATUS_CH1FIFOFLBUSY)) {
    }
#endif
    vdac->EN_CLR = _VDAC_EN_EN_MASK;
    while (vdac->EN & _VDAC_EN_DISABLING_MASK) {
    }
  }
}
#endif

/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enable/disable the VDAC channel.
 *
 * @param[in] vdac
 *   A pointer to the VDAC peripheral register block.
 *
 * @param[in] ch
 *   A channel to enable/disable.
 *
 * @param[in] enable
 *   True to enable VDAC channel, false to disable.
 ******************************************************************************/
void VDAC_Enable(VDAC_TypeDef *vdac, unsigned int ch, bool enable)
{
  EFM_ASSERT(VDAC_REF_VALID(vdac));
  EFM_ASSERT(VDAC_CH_VALID(ch));

#if defined(_VDAC_STATUS_SYNCBUSY_MASK)
  while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
  }
#endif

  if (ch == 0) {
    if (enable) {
      vdac->CMD = VDAC_CMD_CH0EN;
      while ((vdac->STATUS & VDAC_STATUS_CH0ENS) == 0) {
      }
    } else {
      vdac->CMD = VDAC_CMD_CH0DIS;
      while (vdac->STATUS & VDAC_STATUS_CH0ENS) {
      }
#if defined(_VDAC_CMD_CH0FIFOFLUSH_MASK)
      while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
      }
      vdac->CMD = VDAC_CMD_CH0FIFOFLUSH;
      while (vdac->STATUS & VDAC_STATUS_CH0FIFOFLBUSY) {
      }
#endif
    }
  } else {
    if (enable) {
      vdac->CMD = VDAC_CMD_CH1EN;
      while ((vdac->STATUS & VDAC_STATUS_CH1ENS) == 0) {
      }
    } else {
      vdac->CMD = VDAC_CMD_CH1DIS;
      while (vdac->STATUS & VDAC_STATUS_CH1ENS) {
      }

#if defined(_VDAC_CMD_CH1FIFOFLUSH_MASK)
      while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
      }
      vdac->CMD = VDAC_CMD_CH1FIFOFLUSH;
      while (vdac->STATUS & VDAC_STATUS_CH1FIFOFLBUSY) {
      }
#endif
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize VDAC.
 *
 * @details
 *   Initializes the common parts for both channels. This function will also load
 *   calibration values from the Device Information (DI) page into the VDAC
 *   calibration register.
 *   To complete a VDAC setup, channel control configuration must also be done.
 *   See VDAC_InitChannel().
 *
 * @note
 *   This function will disable both channels prior to configuration.
 *
 * @param[in] vdac
 *   A pointer to the VDAC peripheral register block.
 *
 * @param[in] init
 *   A pointer to the VDAC initialization structure.
 ******************************************************************************/
void VDAC_Init(VDAC_TypeDef *vdac, const VDAC_Init_TypeDef *init)
{
  EFM_ASSERT(VDAC_REF_VALID(vdac));
  uint32_t config = 0;

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  uint32_t cal;
  uint32_t const volatile *calData;

  /* Make sure both channels are disabled. */
  vdac->CMD = VDAC_CMD_CH0DIS | VDAC_CMD_CH1DIS;
  while (vdac->STATUS & (VDAC_STATUS_CH0ENS | VDAC_STATUS_CH1ENS)) {
  }

  /* Get the OFFSETTRIM calibration value. */
  cal = ((DEVINFO->VDAC0CH1CAL & _DEVINFO_VDAC0CH1CAL_OFFSETTRIM_MASK)
         >> _DEVINFO_VDAC0CH1CAL_OFFSETTRIM_SHIFT)
        << _VDAC_CAL_OFFSETTRIM_SHIFT;

  if (init->mainCalibration) {
    calData = &DEVINFO->VDAC0MAINCAL;
  } else {
    calData = &DEVINFO->VDAC0ALTCAL;
  }

  /* Get the correct GAINERRTRIM calibration value. */
  switch (init->reference) {
    case vdacRef1V25Ln:
      config = (*calData & _DEVINFO_VDAC0MAINCAL_GAINERRTRIM1V25LN_MASK)
               >> _DEVINFO_VDAC0MAINCAL_GAINERRTRIM1V25LN_SHIFT;
      break;

    case vdacRef2V5Ln:
      config = (*calData & _DEVINFO_VDAC0MAINCAL_GAINERRTRIM2V5LN_MASK)
               >> _DEVINFO_VDAC0MAINCAL_GAINERRTRIM2V5LN_SHIFT;
      break;

    case vdacRef1V25:
      config = (*calData & _DEVINFO_VDAC0MAINCAL_GAINERRTRIM1V25_MASK)
               >> _DEVINFO_VDAC0MAINCAL_GAINERRTRIM1V25_SHIFT;
      break;

    case vdacRef2V5:
      config = (*calData & _DEVINFO_VDAC0MAINCAL_GAINERRTRIM2V5_MASK)
               >> _DEVINFO_VDAC0MAINCAL_GAINERRTRIM2V5_SHIFT;
      break;

    case vdacRefAvdd:
    case vdacRefExtPin:
      config = (*calData & _DEVINFO_VDAC0MAINCAL_GAINERRTRIMVDDANAEXTPIN_MASK)
               >> _DEVINFO_VDAC0MAINCAL_GAINERRTRIMVDDANAEXTPIN_SHIFT;
      break;
  }

  /* Set the sGAINERRTRIM calibration value. */
  cal |= config << _VDAC_CAL_GAINERRTRIM_SHIFT;

  /* Get the GAINERRTRIMCH1 calibration value. */
  switch (init->reference) {
    case vdacRef1V25Ln:
    case vdacRef1V25:
    case vdacRefAvdd:
    case vdacRefExtPin:
      config = (DEVINFO->VDAC0CH1CAL & _DEVINFO_VDAC0CH1CAL_GAINERRTRIMCH1A_MASK)
               >> _DEVINFO_VDAC0CH1CAL_GAINERRTRIMCH1A_SHIFT;
      break;

    case vdacRef2V5Ln:
    case vdacRef2V5:
      config = (DEVINFO->VDAC0CH1CAL & _DEVINFO_VDAC0CH1CAL_GAINERRTRIMCH1B_MASK)
               >> _DEVINFO_VDAC0CH1CAL_GAINERRTRIMCH1B_SHIFT;
      break;
  }

  /* Set the GAINERRTRIM calibration value. */
  cal |= config << _VDAC_CAL_GAINERRTRIMCH1_SHIFT;

  config = ((uint32_t)init->asyncClockMode << _VDAC_CTRL_DACCLKMODE_SHIFT)
           | ((uint32_t)init->warmupKeepOn << _VDAC_CTRL_WARMUPMODE_SHIFT)
           | ((uint32_t)init->refresh      << _VDAC_CTRL_REFRESHPERIOD_SHIFT)
           | (((uint32_t)init->prescaler   << _VDAC_CTRL_PRESC_SHIFT)
              & _VDAC_CTRL_PRESC_MASK)
           | ((uint32_t)init->reference    << _VDAC_CTRL_REFSEL_SHIFT)
           | ((uint32_t)init->ch0ResetPre  << _VDAC_CTRL_CH0PRESCRST_SHIFT)
           | ((uint32_t)init->outEnablePRS << _VDAC_CTRL_OUTENPRS_SHIFT)
           | ((uint32_t)init->sineEnable   << _VDAC_CTRL_SINEMODE_SHIFT)
           | ((uint32_t)init->diff         << _VDAC_CTRL_DIFF_SHIFT);

  /* Write to VDAC registers. */
  vdac->CAL = cal;
  vdac->CTRL = config;
#elif defined(_SILICON_LABS_32B_SERIES_2)

  VDAC_DisableModule(vdac);

  config = ((((uint32_t)init->warmupTime  << _VDAC_CFG_WARMUPTIME_SHIFT) & _VDAC_CFG_WARMUPTIME_MASK)
            | ((uint32_t)init->dbgHalt        << _VDAC_CFG_DBGHALT_SHIFT)
            | ((uint32_t)init->onDemandClk    << _VDAC_CFG_ONDEMANDCLK_SHIFT)
            | ((uint32_t)init->dmaWakeUp      << _VDAC_CFG_DMAWU_SHIFT)
            | ((uint32_t)init->biasKeepWarm   << _VDAC_CFG_BIASKEEPWARM_SHIFT)
            | ((uint32_t)init->refresh        << _VDAC_CFG_REFRESHPERIOD_SHIFT)
            | ((uint32_t)init->timerOverflow  << _VDAC_CFG_TIMEROVRFLOWPERIOD_SHIFT)
            | (((uint32_t)init->prescaler     << _VDAC_CFG_PRESC_SHIFT) & _VDAC_CFG_PRESC_MASK)
            | ((uint32_t)init->reference      << _VDAC_CFG_REFRSEL_SHIFT)
            | ((uint32_t)init->ch0ResetPre    << _VDAC_CFG_CH0PRESCRST_SHIFT)
            | ((uint32_t)init->sineReset      << _VDAC_CFG_SINERESET_SHIFT)
            | ((uint32_t)init->sineEnable     << _VDAC_CFG_SINEMODE_SHIFT)
            | ((uint32_t)init->diff           << _VDAC_CFG_DIFF_SHIFT));

  vdac->CFG = config;
#endif
}

/***************************************************************************//**
 * @brief
 *   Initialize a VDAC channel.
 *
 * @param[in] vdac
 *   A pointer to the VDAC peripheral register block.
 *
 * @param[in] init
 *   A pointer to the VDAC channel initialization structure.
 *
 * @param[in] ch
 *   A channel number to initialize.
 ******************************************************************************/
void VDAC_InitChannel(VDAC_TypeDef *vdac,
                      const VDAC_InitChannel_TypeDef *init,
                      unsigned int ch)
{
  uint32_t channelConfig, vdacStatus;

  EFM_ASSERT(VDAC_REF_VALID(vdac));
  EFM_ASSERT(VDAC_CH_VALID(ch));

  vdacStatus = vdac->STATUS;

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)

  /* Make sure both channels are disabled. */
  vdac->CMD = VDAC_CMD_CH0DIS | VDAC_CMD_CH1DIS;
  while (vdac->STATUS & (VDAC_STATUS_CH0ENS | VDAC_STATUS_CH1ENS)) {
  }

  channelConfig = ((uint32_t)init->prsSel          << _VDAC_CH0CTRL_PRSSEL_SHIFT)
                  | ((uint32_t)init->prsAsync      << _VDAC_CH0CTRL_PRSASYNC_SHIFT)
                  | ((uint32_t)init->trigMode      << _VDAC_CH0CTRL_TRIGMODE_SHIFT)
                  | ((uint32_t)init->sampleOffMode << _VDAC_CH0CTRL_CONVMODE_SHIFT);

  if (ch == 0) {
    vdac->CH0CTRL = channelConfig;
  } else {
    vdac->CH1CTRL = channelConfig;
  }

#elif defined(_SILICON_LABS_32B_SERIES_2)

  VDAC_DisableModule(vdac);

  channelConfig = ((uint32_t)init->warmupKeepOn             << _VDAC_CH0CFG_KEEPWARM_SHIFT)
                  | ((uint32_t)init->highCapLoadEnable     << _VDAC_CH0CFG_HIGHCAPLOADEN_SHIFT)
                  | (((uint32_t)init->fifoLowDataThreshold << _VDAC_CH0CFG_FIFODVL_SHIFT) & _VDAC_CH0CFG_FIFODVL_MASK)
                  | ((uint32_t)init->chRefreshSource       << _VDAC_CH0CFG_REFRESHSOURCE_SHIFT)
                  | ((uint32_t)init->trigMode              << _VDAC_CH0CFG_TRIGMODE_SHIFT)
                  | ((uint32_t)init->powerMode             << _VDAC_CH0CFG_POWERMODE_SHIFT)
                  | ((uint32_t)init->sampleOffMode         << _VDAC_CH0CFG_CONVMODE_SHIFT);

  if (ch == 0) {
    vdac->CH0CFG = channelConfig;

    vdac->OUTTIMERCFG = ((uint32_t)(vdac->OUTTIMERCFG & ~(_VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_MASK)))
                        | (((uint32_t)init->holdOutTime << _VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_SHIFT) & _VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_MASK);

    vdac->EN_SET = _VDAC_EN_EN_MASK;

    vdac->OUTCTRL = ((uint32_t)(vdac->OUTCTRL & ~(_VDAC_OUTCTRL_ABUSPINSELCH0_MASK | _VDAC_OUTCTRL_ABUSPORTSELCH0_MASK | _VDAC_OUTCTRL_SHORTCH0_MASK | _VDAC_OUTCTRL_AUXOUTENCH0_MASK | _VDAC_OUTCTRL_MAINOUTENCH0_MASK)))
                    | (((uint32_t)init->pin          << _VDAC_OUTCTRL_ABUSPINSELCH0_SHIFT) & _VDAC_OUTCTRL_ABUSPINSELCH0_MASK)
                    | ((uint32_t)init->port          << _VDAC_OUTCTRL_ABUSPORTSELCH0_SHIFT)
                    | ((uint32_t)init->shortOutput   << _VDAC_OUTCTRL_SHORTCH0_SHIFT)
                    | ((uint32_t)init->auxOutEnable  << _VDAC_OUTCTRL_AUXOUTENCH0_SHIFT)
                    | ((uint32_t)init->mainOutEnable << _VDAC_OUTCTRL_MAINOUTENCH0_SHIFT);
  } else if (ch == 1) {
    vdac->CH1CFG = channelConfig;

    vdac->OUTTIMERCFG = (vdac->OUTTIMERCFG & ~(_VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_MASK))
                        | (((uint32_t)init->holdOutTime << _VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_SHIFT) & _VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_MASK);

    vdac->EN_SET = _VDAC_EN_EN_MASK;

    vdac->OUTCTRL = ((uint32_t)(vdac->OUTCTRL & ~(_VDAC_OUTCTRL_ABUSPINSELCH1_MASK | _VDAC_OUTCTRL_ABUSPORTSELCH1_MASK | _VDAC_OUTCTRL_SHORTCH1_MASK | _VDAC_OUTCTRL_AUXOUTENCH1_MASK | _VDAC_OUTCTRL_MAINOUTENCH1_MASK)))
                    | (((uint32_t)init->pin          << _VDAC_OUTCTRL_ABUSPINSELCH1_SHIFT) & _VDAC_OUTCTRL_ABUSPINSELCH1_MASK)
                    | ((uint32_t)init->port          << _VDAC_OUTCTRL_ABUSPORTSELCH1_SHIFT)
                    | ((uint32_t)init->shortOutput   << _VDAC_OUTCTRL_SHORTCH1_SHIFT)
                    | ((uint32_t)init->auxOutEnable  << _VDAC_OUTCTRL_AUXOUTENCH1_SHIFT)
                    | ((uint32_t)init->mainOutEnable << _VDAC_OUTCTRL_MAINOUTENCH1_SHIFT);
  }
#endif

  /* Check if the channel must be enabled. */
  if (init->enable) {
    if (ch == 0) {
      vdac->CMD = VDAC_CMD_CH0EN;
    } else {
      vdac->CMD = VDAC_CMD_CH1EN;
    }
  }

  /* Check if the other channel had to be turned off above
   * and needs to be turned on again. */
  if (ch == 0) {
    if (vdacStatus & VDAC_STATUS_CH1ENS) {
      vdac->CMD = VDAC_CMD_CH1EN;
    }
  } else {
    if (vdacStatus & VDAC_STATUS_CH0ENS) {
      vdac->CMD = VDAC_CMD_CH0EN;
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Set the output signal of a VDAC channel to a given value.
 *
 * @details
 *   This function sets the output signal of a VDAC channel by writing @p value
 *   to the corresponding CHnDATA register.
 *
 * @param[in] vdac
 *   A pointer to the VDAC peripheral register block.
 *
 * @param[in] channel
 *   A channel number to set the output of.
 *
 * @param[in] value
 *   A value to write to the channel output register CHnDATA.
 ******************************************************************************/
void VDAC_ChannelOutputSet(VDAC_TypeDef *vdac,
                           unsigned int channel,
                           uint32_t value)
{
  switch (channel) {
    case 0:
      VDAC_Channel0OutputSet(vdac, value);
      break;
    case 1:
      VDAC_Channel1OutputSet(vdac, value);
      break;
    default:
      EFM_ASSERT(0);
      break;
  }
}

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief
 *   Calculate the prescaler value used to determine VDAC clock.
 *
 * @details
 *   The VDAC clock is given by the input clock divided by the prescaler+1.
 *
 *     VDAC_CLK = IN_CLK / (prescale + 1)
 *
 *   The maximum VDAC clock is 1 MHz. The input clock is HFPERCLK/HFPERCCLK
 *   when VDAC synchronous mode is selected, or an internal oscillator of
 *   10 MHz +/- 20% when asynchronous mode is selected.
 *
 * @note
 *   If the requested VDAC frequency is low and the maximum prescaler value can't
 *   adjust the actual VDAC frequency lower than requested, the maximum prescaler
 *   value is returned resulting in a higher VDAC frequency than requested.
 *
 * @param[in] vdacFreq VDAC frequency target. The frequency will automatically
 *   be adjusted to be below maximum allowed VDAC clock.
 *
 * @param[in] syncMode Set to true if you intend to use VDAC in synchronous
 *   mode.
 *
 * @param[in] hfperFreq Frequency in Hz of HFPERCLK/HFPERCCLK oscillator.
 *   Set to 0 to use the currently defined HFPERCLK/HFPERCCLK clock setting.
 *   This parameter is only used when syncMode is set to true.
 *
 * @return
 *   A prescaler value to use for VDAC to achieve a clock value less than
 *   or equal to @p vdacFreq.
 ******************************************************************************/
uint32_t VDAC_PrescaleCalc(uint32_t vdacFreq, bool syncMode, uint32_t hfperFreq)
{
  uint32_t ret, refFreq;

  /* Make sure that the selected VDAC clock is below the maximum value. */
  if (vdacFreq > VDAC_MAX_CLOCK) {
    vdacFreq = VDAC_MAX_CLOCK;
  }

  if (!syncMode) {
    refFreq = VDAC_INTERNAL_CLOCK_FREQ;
  } else {
    if (hfperFreq) {
      refFreq = hfperFreq;
    } else {
      refFreq = CMU_ClockFreqGet(cmuClock_VDAC0);
    }
  }

  /* Iterate to determine the best prescaler value. Start with the lowest */
  /* prescaler value to get the first equal or less VDAC         */
  /* frequency value. */
  for (ret = 0; ret <= (_VDAC_CTRL_PRESC_MASK >> _VDAC_CTRL_PRESC_SHIFT); ret++) {
    if ((refFreq / (ret + 1)) <= vdacFreq) {
      break;
    }
  }

  /* If ret is higher than the maximum prescaler value, make sure to return
     the maximum value. */
  if (ret > (_VDAC_CTRL_PRESC_MASK >> _VDAC_CTRL_PRESC_SHIFT)) {
    ret = _VDAC_CTRL_PRESC_MASK >> _VDAC_CTRL_PRESC_SHIFT;
  }

  return ret;
}
#else
/***************************************************************************//**
 * @brief
 *   Calculate the prescaler value used to determine VDAC clock.
 *
 * @details
 *   The VDAC clock is given by the input clock divided by the prescaler+1.
 *
 *     VDAC_CLK = IN_CLK / (prescale + 1)
 *
 *   The maximum VDAC clock is 1 MHz.
 *
 * @note
 *   If the requested VDAC frequency is low and the maximum prescaler value can't
 *   adjust the actual VDAC frequency lower than requested, the maximum prescaler
 *   value is returned resulting in a higher VDAC frequency than requested.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] vdacFreq VDAC frequency target. The frequency will automatically
 *   be adjusted to be below maximum allowed VDAC clock.
 *
 * @return
 *   A prescaler value to use for VDAC to achieve a clock value less than
 *   or equal to @p vdacFreq.
 ******************************************************************************/
uint32_t VDAC_PrescaleCalc(VDAC_TypeDef *vdac, uint32_t vdacFreq)
{
  uint32_t ret = 0;
  uint32_t refFreq = 0;

  /* Make sure that the selected VDAC clock is below the maximum value. */
  if (vdacFreq > VDAC_MAX_CLOCK) {
    vdacFreq = VDAC_MAX_CLOCK;
  }

  if (vdac == VDAC0) {
    refFreq = CMU_ClockFreqGet(cmuClock_VDAC0);
  }
#if defined(VDAC1)
  else if (vdac == VDAC1) {
    refFreq = CMU_ClockFreqGet(cmuClock_VDAC1);
  }
#endif
  else {
    EFM_ASSERT(0);
  }

  /* Iterate to determine the best prescaler value. Start with the lowest */
  /* prescaler value to get the first equal or less VDAC         */
  /* frequency value. */
  for (ret = 0; ret <= (_VDAC_CFG_PRESC_MASK >> _VDAC_CFG_PRESC_SHIFT); ret++) {
    if ((refFreq / (ret + 1)) <= vdacFreq) {
      break;
    }
  }

  /* If ret is higher than the maximum prescaler value, make sure to return
     the maximum value. */
  if (ret > (_VDAC_CFG_PRESC_MASK >> _VDAC_CFG_PRESC_SHIFT)) {
    ret = _VDAC_CFG_PRESC_MASK >> _VDAC_CFG_PRESC_SHIFT;
  }

  return ret;
}
#endif

/***************************************************************************//**
 * @brief
 *   Reset VDAC to same state that it was in after a hardwares reset.
 *
 * @param[in] vdac
 *   A pointer to the VDAC peripheral register block.
 ******************************************************************************/
void VDAC_Reset(VDAC_TypeDef *vdac)
{
#if defined(VDAC_SWRST_SWRST)

  while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
  }

  /* Wait for all synchronizations to finish and disable the vdac channels */
  if (vdac->EN & VDAC_EN_EN) {
    vdac->CMD = _VDAC_CMD_CH0DIS_MASK;
    while (vdac->STATUS & VDAC_STATUS_CH0ENS ) {
    }

    vdac->CMD = _VDAC_CMD_CH1DIS_MASK;
    while (vdac->STATUS & VDAC_STATUS_CH1ENS ) {
    }

    while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
    }

    vdac->CMD = _VDAC_CMD_CH0FIFOFLUSH_MASK | _VDAC_CMD_CH1FIFOFLUSH_MASK;
    while (vdac->STATUS & (VDAC_STATUS_CH0FIFOFLBUSY | VDAC_STATUS_CH1FIFOFLBUSY)) {
    }

    while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
    }
  }

  vdac->SWRST_SET = VDAC_SWRST_SWRST;
  while (vdac->SWRST & _VDAC_SWRST_RESETTING_MASK) {
  }

#else
  /* Disable channels before resetting other registers. */
  vdac->CMD     = VDAC_CMD_CH0DIS | VDAC_CMD_CH1DIS;
  while (vdac->STATUS & (VDAC_STATUS_CH0ENS | VDAC_STATUS_CH1ENS)) {
  }
  vdac->CH0CTRL = _VDAC_CH0CTRL_RESETVALUE;
  vdac->CH1CTRL = _VDAC_CH1CTRL_RESETVALUE;
  vdac->CH0DATA = _VDAC_CH0DATA_RESETVALUE;
  vdac->CH1DATA = _VDAC_CH1DATA_RESETVALUE;
  vdac->CTRL    = _VDAC_CTRL_RESETVALUE;
  vdac->IEN     = _VDAC_IEN_RESETVALUE;
  vdac->IFC     = _VDAC_IFC_MASK;
  vdac->CAL     = _VDAC_CAL_RESETVALUE;
#endif
}

/** @} (end addtogroup vdac) */
#endif /* defined(VDAC_COUNT) && (VDAC_COUNT > 0) */
