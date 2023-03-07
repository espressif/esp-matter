/***************************************************************************//**
 * @file
 * @brief HAL functions to control antenna mode
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
#include "sl_status.h"
#include "sl_rail_util_ant_div.h"
#include "rail.h"
#include "em_gpio.h"

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
#include "sl_rail_util_rf_path_config.h"
#endif //defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)

// The existence of Antenna GPIO location information on EFR32XG1 series
// parts enables use of the more flexible RAIL scheme va. legacy GPIO scheme
// for Tx-only diversity. However, the EFR32XG2 series doesn't use locations
// so the HAL configurator doesn't provide any. But EFR32XG2 does have RfPath
// selection, and SL_RAIL_UTIL_ANT_DIV_ANT0_LOC is used for that.
// On EFR32XG1 series, default location(s) to -1 to select legacy GPIO scheme.
// On EFR32XG2 series, default location(s) to 1 to select RAIL scheme RfPath 1;
// to force use of legacy GPIO scheme (because their GPIO choice for Tx-only
// diversity isn't supported by the radio), user must define each respective
// SL_RAIL_UTIL_ANT_DIV_[N]SEL_LOC as -1 in their HAL config include.
#ifdef  _SILICON_LABS_32B_SERIES_2
  #define ANTENNA_UNSPECIFIED_LOC  1 // Location to use RAIL scheme on RfPath 1
#else//!_SILICON_LABS_32B_SERIES_2
  #define ANTENNA_UNSPECIFIED_LOC -1 // Dummy location to select legacy GPIO scheme
#endif//_SILICON_LABS_32B_SERIES_2

#ifndef SL_RAIL_UTIL_ANT_DIV_ANT0_LOC
  #define SL_RAIL_UTIL_ANT_DIV_ANT0_LOC ANTENNA_UNSPECIFIED_LOC
#endif
#ifndef SL_RAIL_UTIL_ANT_DIV_ANT1_LOC
  #define SL_RAIL_UTIL_ANT_DIV_ANT1_LOC ANTENNA_UNSPECIFIED_LOC
#endif

// Determine scheme to use based on platform, PHY, debug, and GPIO location(s):
#if (!defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)                                          \
  && (((defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT)) \
  && (!defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || (SL_RAIL_UTIL_ANT_DIV_ANT0_LOC >= 0))     \
  && (!defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT) || (SL_RAIL_UTIL_ANT_DIV_ANT1_LOC >= 0)))))
  #define ANTENNA_USE_RAIL_SCHEME 1
#else
  #define ANTENNA_USE_RAIL_SCHEME 0
#endif

#define RX_ANTENNA_SUPPORT (SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT \
                            || (SL_RAIL_UTIL_ANTENNA_RX_DEFAULT_MODE != SL_RAIL_UTIL_ANTENNA_MODE_DISABLED))

#if defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT)
#define SL_RAIL_UTIL_ANT_DIV_ENABLE 1
#else //!(defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT))
#define SL_RAIL_UTIL_ANT_DIV_ENABLE 0
#endif //defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT)

// Flag any mis-configuration
#if     ANTENNA_USE_RAIL_SCHEME
  #if     (defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) && !defined(SL_RAIL_UTIL_ANT_DIV_ANT0_LOC))
    #error "Antenna Diversity needs SL_RAIL_UTIL_ANT_DIV_ANT0_LOC specified"
  #endif//(defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) && !defined(SL_RAIL_UTIL_ANT_DIV_ANT0_LOC))
  #if     (defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT) && !defined(SL_RAIL_UTIL_ANT_DIV_ANT1_LOC))
    #error "Antenna Diversity needs SL_RAIL_UTIL_ANT_DIV_ANT1_LOC specified"
  #endif//(defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT) && !defined(SL_RAIL_UTIL_ANT_DIV_ANT1_LOC))
#else//!ANTENNA_USE_RAIL_SCHEME
  #if RX_ANTENNA_SUPPORT
    #if !defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
      #error "Rx Antenna Diversity requires SL_RAIL_UTIL_ANT_DIV_ANT[0|1]SEL_PORT/PIN/LOC to be defined."
    #else
      #error "Rx Antenna Diversity is not supported on this platform or PHY."
    #endif
  #endif//RX_ANTENNA_SUPPORT
#endif//ANTENNA_USE_RAIL_SCHEME

// RAIL Antenna Configuration

sl_status_t sl_rail_util_ant_div_init(void)
{
 #if ANTENNA_USE_RAIL_SCHEME
  RAIL_AntennaConfig_t antennaConfig = { false, }; // Zero out structure
  sl_rail_util_ant_div_get_antenna_config(&antennaConfig);
  if (RAIL_ConfigAntenna(RAIL_EFR32_HANDLE, &antennaConfig)
      != RAIL_STATUS_NO_ERROR) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  sl_status_t status = sl_rail_util_ant_div_set_rx_antenna_mode(SL_RAIL_UTIL_ANTENNA_RX_DEFAULT_MODE);
  if (status != SL_STATUS_OK) {
    return status;
  }
  return sl_rail_util_ant_div_set_tx_antenna_mode(SL_RAIL_UTIL_ANTENNA_TX_DEFAULT_MODE);
 #else//!ANTENNA_USE_RAIL_SCHEME
  return SL_STATUS_OK;
 #endif//ANTENNA_USE_RAIL_SCHEME
}

#if     (defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT))

static sl_rail_util_antenna_mode_t txAntennaMode = SL_RAIL_UTIL_ANTENNA_TX_DEFAULT_MODE;
// Default to first antenna
static sl_rail_util_antenna_selection_t txAntennaSelection = SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1;
static sl_rail_util_antenna_mode_t rxAntennaMode = SL_RAIL_UTIL_ANTENNA_RX_DEFAULT_MODE;

// Rx Antenna Diversity
sl_rail_util_antenna_mode_t sl_rail_util_ant_div_get_rx_antenna_mode(void)
{
  return rxAntennaMode;
}

void sl_rail_util_ant_div_get_antenna_config(RAIL_AntennaConfig_t *antennaConfig)
{
  if (NULL == antennaConfig) {
    return;
  }
#if (defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) \
  && defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PIN))
  antennaConfig->ant0PinEn = true;
  antennaConfig->ant0Port = (uint8_t)SL_RAIL_UTIL_ANT_DIV_ANT0_PORT;
  antennaConfig->ant0Pin  = SL_RAIL_UTIL_ANT_DIV_ANT0_PIN;
#if defined(SL_RAIL_UTIL_ANT_DIV_ANT0_LOC) // efr32xg1x only
  antennaConfig->ant0Loc  = SL_RAIL_UTIL_ANT_DIV_ANT0_LOC;
#endif // ant0 loc
#endif // ant0 port & pin

#if (defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT) \
  && defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PIN))
  antennaConfig->ant1PinEn = true;
  antennaConfig->ant1Port = (uint8_t)SL_RAIL_UTIL_ANT_DIV_ANT1_PORT;
  antennaConfig->ant1Pin  = SL_RAIL_UTIL_ANT_DIV_ANT1_PIN;
#if defined(SL_RAIL_UTIL_ANT_DIV_ANT1_LOC) // efr32xg1x only
  antennaConfig->ant1Loc  = SL_RAIL_UTIL_ANT_DIV_ANT1_LOC;
#endif // ant1 loc
#endif // ant1 port & pin

#if defined(SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE) // efr32xg2x chip-specific
  antennaConfig->defaultPath = SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE;
#endif // internal rf path
}

#ifndef RAIL_RX_OPTIONS_ANTENNA
#define RAIL_RX_OPTIONS_ANTENNA \
  (RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1)
#endif//RAIL_RX_OPTIONS_ANTENNA

static RAIL_Events_t getRxAntOptions(sl_rail_util_antenna_mode_t mode)
{
  switch (mode) {
    default:
    case SL_RAIL_UTIL_ANTENNA_MODE_DISABLED: // Leave antenna untouched (e.g. from Tx)
      return RAIL_RX_OPTIONS_NONE;
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_ENABLE1:
      return RAIL_RX_OPTION_ANTENNA0;
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_ENABLE2:
      return RAIL_RX_OPTION_ANTENNA1;
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_DIVERSITY: // Use radio config's diversity scheme
      return RAIL_RX_OPTIONS_ANTENNA;
      break;
  }
}

#ifdef SL_CATALOG_RAIL_UTIL_INIT_PRESENT
#include "sl_rail_util_init.h"
#else
extern RAIL_Handle_t emPhyRailHandle;
#endif

static sl_status_t radioConfigRxAntenna(sl_rail_util_antenna_mode_t mode)
{
#ifdef SL_CATALOG_RAIL_UTIL_INIT_PRESENT
#if SL_RAIL_UTIL_INIT_INST0_ENABLE
  RAIL_Handle_t emPhyRailHandle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
#else
#error "RAIL instance not initialized"
#endif
#endif
  if (emPhyRailHandle == NULL) {
    // This call is premature, before radio is initialized.
    // Defer to when we're re-called as part of halPluginConfig2p4GHzRadio().
    return SL_STATUS_OK;
  }
  // Tell RAIL what Rx antenna mode to use
  return (RAIL_ConfigRxOptions(emPhyRailHandle, RAIL_RX_OPTIONS_ANTENNA,
                               getRxAntOptions(mode))
          == RAIL_STATUS_NO_ERROR) ? SL_STATUS_OK : SL_STATUS_FAIL;
}

sl_status_t sl_rail_util_ant_div_update_antenna_config(void)
{
  return radioConfigRxAntenna(sl_rail_util_ant_div_get_rx_antenna_mode());
}

sl_status_t sl_rail_util_ant_div_set_rx_antenna_mode(sl_rail_util_antenna_mode_t mode)
{
 #if RX_ANTENNA_SUPPORT
  sl_status_t status  = SL_STATUS_NOT_SUPPORTED;
  if (mode <= SL_RAIL_UTIL_ANTENNA_MODE_DIVERSITY) {
    // Tell RAIL what Rx antenna mode to use, if we can
    status = radioConfigRxAntenna(mode);
  }
  if (status == SL_STATUS_OK) {
    rxAntennaMode = mode;
  }
  return status;
 #else//!RX_ANTENNA_SUPPORT
  return ((mode == SL_RAIL_UTIL_ANTENNA_MODE_DISABLED) ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED);
 #endif//RX_ANTENNA_SUPPORT
}

// Tx Antenna Diversity

static void selectTxAntenna(sl_rail_util_antenna_selection_t txAntenna)
{
  txAntennaSelection = txAntenna;
 #if     ANTENNA_USE_RAIL_SCHEME
  // PHY does selection when a transmit is begun
 #else//!ANTENNA_USE_RAIL_SCHEME
  if (txAntenna == SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1) {
   #ifdef  SL_RAIL_UTIL_ANT_DIV_ANT1_PORT
    GPIO_PinOutClear(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT,
                     SL_RAIL_UTIL_ANT_DIV_ANT1_PIN);
   #endif//SL_RAIL_UTIL_ANT_DIV_ANT1_PORT
   #ifdef  SL_RAIL_UTIL_ANT_DIV_ANT0_PORT
    GPIO_PinOutSet(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT,
                   SL_RAIL_UTIL_ANT_DIV_ANT0_PIN);
   #endif//SL_RAIL_UTIL_ANT_DIV_ANT0_PORT
  } else { // (txAntenna == SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA2)
   #ifdef  SL_RAIL_UTIL_ANT_DIV_ANT0_PORT
    GPIO_PinOutClear(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT,
                     SL_RAIL_UTIL_ANT_DIV_ANT0_PIN);
   #endif//SL_RAIL_UTIL_ANT_DIV_ANT0_PORT
   #ifdef  SL_RAIL_UTIL_ANT_DIV_ANT1_PORT
    GPIO_PinOutSet(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT,
                   SL_RAIL_UTIL_ANT_DIV_ANT1_PIN);
   #endif//SL_RAIL_UTIL_ANT_DIV_ANT1_PORT
  }
 #endif//ANTENNA_USE_RAIL_SCHEME
}

sl_rail_util_antenna_selection_t sl_rail_util_ant_div_get_tx_antenna_selected(void)
{
  return txAntennaSelection;
}

sl_rail_util_antenna_mode_t sl_rail_util_ant_div_get_tx_antenna_mode(void)
{
  return txAntennaMode;
}

sl_status_t sl_rail_util_ant_div_set_tx_antenna_mode(sl_rail_util_antenna_mode_t mode)
{
  sl_status_t status = SL_STATUS_OK;

  switch (mode) {
    case SL_RAIL_UTIL_ANTENNA_MODE_DISABLED: // Leave antenna untouched (e.g. from Rx)
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_ENABLE1:
      selectTxAntenna(SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1);
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_ENABLE2:
      selectTxAntenna(SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA2);
      break;
    case SL_RAIL_UTIL_ANTENNA_MODE_DIVERSITY: // Ensure current selection is effected
      selectTxAntenna(txAntennaSelection);
      break;
    default:
      status = SL_STATUS_NOT_SUPPORTED;
      break;
  }

  if (status == SL_STATUS_OK) {
    txAntennaMode = mode;
  }

  return status;
}

sl_status_t sl_rail_util_ant_div_toggle_tx_antenna(void)
{
  if (txAntennaMode == SL_RAIL_UTIL_ANTENNA_MODE_DIVERSITY) {
    selectTxAntenna(txAntennaSelection ^ SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1 ^ SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA2);
    return SL_STATUS_OK;
  }
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_rail_util_ant_div_set_tx_antenna(sl_rail_util_antenna_selection_t txAntenna)
{
  if (txAntenna == txAntennaSelection) { // Already selected!
    return SL_STATUS_OK;
  }
  return sl_rail_util_ant_div_toggle_tx_antenna();
}

void sl_rail_util_ant_div_init_rx_options(RAIL_RxOptions_t *rxOptions)
{
  if (NULL == rxOptions) {
    return;
  }
#if (SL_RAIL_UTIL_ANT_DIV_RX_MODE \
     != SL_RAIL_UTIL_ANT_DIV_DISABLED)
  *rxOptions = (*rxOptions
                & ~(RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1))
               | ((((RAIL_RxOptions_t)SL_RAIL_UTIL_ANT_DIV_RX_MODE)
                   << RAIL_RX_OPTION_ANTENNA0_SHIFT)
                  & (RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1));
#endif
}

void sl_rail_util_ant_div_init_tx_options(RAIL_TxOptions_t *txOptions)
{
  if (NULL == txOptions) {
    return;
  }
#if (SL_RAIL_UTIL_ANT_DIV_TX_MODE \
     != SL_RAIL_UTIL_ANT_DIV_DISABLED)
  *txOptions = (*txOptions
                & ~(RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1))
               | ((((RAIL_TxOptions_t)SL_RAIL_UTIL_ANT_DIV_TX_MODE)
                   << RAIL_TX_OPTION_ANTENNA0_SHIFT)
                  & (RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1));
#endif
}

#define ANTDIV_RX_PHY_DEFAULT_ENABLED (SL_RAIL_UTIL_ANTENNA_RX_DEFAULT_MODE != SL_RAIL_UTIL_ANTENNA_MODE_DISABLED)

#if SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT
#define antDivRxPhySelected (rxAntennaMode == SL_RAIL_UTIL_ANTENNA_MODE_DIVERSITY)
#elif ANTDIV_RX_PHY_DEFAULT_ENABLED
#define antDivRxPhySelected (true)
#else //!ANTDIV_RX_PHY_DEFAULT_ENABLED
#define antDivRxPhySelected (false)
#endif //SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT

bool sl_rail_util_ant_div_get_phy_select(void)
{
  return antDivRxPhySelected;
}
#else//!(defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT))

sl_rail_util_antenna_mode_t sl_rail_util_ant_div_get_rx_antenna_mode(void)
{
  return SL_RAIL_UTIL_ANTENNA_MODE_DISABLED;
}

sl_status_t sl_rail_util_ant_div_set_rx_antenna_mode(sl_rail_util_antenna_mode_t mode)
{
  return ((mode == SL_RAIL_UTIL_ANTENNA_MODE_DISABLED) ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED);
}

sl_rail_util_antenna_selection_t sl_rail_util_ant_div_get_tx_antenna_selected(void)
{
  return SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1;
}

sl_rail_util_antenna_mode_t sl_rail_util_ant_div_get_tx_antenna_mode(void)
{
  return SL_RAIL_UTIL_ANTENNA_MODE_ENABLE1;
}

sl_status_t sl_rail_util_ant_div_set_tx_antenna_mode(sl_rail_util_antenna_mode_t mode)
{
  return ((mode == SL_RAIL_UTIL_ANTENNA_MODE_ENABLE1) ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED);
}

sl_status_t sl_rail_util_ant_div_toggle_tx_antenna(void)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_rail_util_ant_div_set_tx_antenna(sl_rail_util_antenna_selection_t txAntenna)
{
  return ((txAntenna == SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1)
          ? SL_STATUS_OK : SL_STATUS_NOT_SUPPORTED);
}

void sl_rail_util_ant_div_get_antenna_config(RAIL_AntennaConfig_t *antennaConfig)
{
  (void)antennaConfig;
}

sl_status_t sl_rail_util_ant_div_update_antenna_config(void)
{
  return SL_STATUS_NOT_SUPPORTED;
}

void sl_rail_util_ant_div_init_rx_options(RAIL_RxOptions_t *rxOptions)
{
  (void)rxOptions;
}

void sl_rail_util_ant_div_init_tx_options(RAIL_RxOptions_t *txOptions)
{
  (void)txOptions;
}

bool sl_rail_util_ant_div_get_phy_select(void)
{
  return false;
}
#endif//(defined(SL_RAIL_UTIL_ANT_DIV_ANT0_PORT) || defined(SL_RAIL_UTIL_ANT_DIV_ANT1_PORT))
