/***************************************************************************//**
 * @file
 * @brief TODO description
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
#include "sl_device_init_hfxo_config.h"
#include "sl_rail_ble_config_38M4Hz.h"
#include "sl_rail_ieee802154_config_38M4Hz.h"
#include "sl_rail_ble_config_39MHz.h"
#include "sl_rail_ieee802154_config_39MHz.h"

#if SL_DEVICE_INIT_HFXO_FREQ == 38400000

#if RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1Mbps =
  &sl_rail_ble_phy_1Mbps_38M4Hz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2Mbps =
  &sl_rail_ble_phy_2Mbps_38M4Hz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_1MBPS_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1MbpsViterbi =
  &sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_2MBPS_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsViterbi =
  &sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_channelConfig;
#if RAIL_BLE_SUPPORTS_AOX
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsAox =
  &sl_rail_ble_phy_2Mbps_aox_38M4Hz_channelConfig;
#endif
#endif

#if RAIL_BLE_SUPPORTS_CODED_PHY
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy125kbps =
  &sl_rail_ble_phy_125kbps_38M4Hz_channelConfig;
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy500kbps =
  &sl_rail_ble_phy_500kbps_38M4Hz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_SIMULSCAN_PHY
const RAIL_ChannelConfig_t *const RAIL_BLE_PhySimulscan =
  &sl_rail_ble_phy_simulscan_38M4Hz_channelConfig;
#endif

#if RAIL_SUPPORTS_2P4GHZ_BAND && RAIL_SUPPORTS_PROTOCOL_IEEE802154
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHz =
  &sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channelConfig;
#endif

#if RAIL_SUPPORTS_ANTENNA_DIVERSITY && RAIL_SUPPORTS_2P4GHZ_BAND \
  && RAIL_SUPPORTS_PROTOCOL_IEEE802154
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDiv =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_COEX_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzCoex =
  &sl_rail_ieee802154_phy_2G4Hz_coex_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_COEX_PHY && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivCoex =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_coex_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzFem =
  &sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivFem =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_IEEE802154_SUPPORTS_COEX_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzCoexFem =
  &sl_rail_ieee802154_phy_2G4Hz_coex_fem_38M4Hz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_IEEE802154_SUPPORTS_COEX_PHY \
  && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivCoexFem =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_coex_fem_38M4Hz_channelConfig;
#endif

#elif SL_DEVICE_INIT_HFXO_FREQ == 39000000

#if RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1Mbps =
  &sl_rail_ble_phy_1Mbps_39MHz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2Mbps =
  &sl_rail_ble_phy_2Mbps_39MHz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_1MBPS_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1MbpsViterbi =
  &sl_rail_ble_phy_1Mbps_viterbi_39MHz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_2MBPS_VITERBI
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsViterbi =
  &sl_rail_ble_phy_2Mbps_viterbi_39MHz_channelConfig;
#if RAIL_BLE_SUPPORTS_AOX
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsAox =
  &sl_rail_ble_phy_2Mbps_aox_39MHz_channelConfig;
#endif
#endif

#if RAIL_BLE_SUPPORTS_CODED_PHY
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy125kbps =
  &sl_rail_ble_phy_125kbps_39MHz_channelConfig;
const RAIL_ChannelConfig_t *const RAIL_BLE_Phy500kbps =
  &sl_rail_ble_phy_500kbps_39MHz_channelConfig;
#endif

#if RAIL_BLE_SUPPORTS_SIMULSCAN_PHY
const RAIL_ChannelConfig_t *const RAIL_BLE_PhySimulscan =
  &sl_rail_ble_phy_simulscan_39MHz_channelConfig;
#endif

#if RAIL_SUPPORTS_2P4GHZ_BAND && RAIL_SUPPORTS_PROTOCOL_IEEE802154
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHz =
  &sl_rail_ieee802154_phy_2G4Hz_39MHz_channelConfig;
#endif

#if RAIL_SUPPORTS_ANTENNA_DIVERSITY && RAIL_SUPPORTS_2P4GHZ_BAND \
  && RAIL_SUPPORTS_PROTOCOL_IEEE802154
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDiv =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_COEX_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzCoex =
  &sl_rail_ieee802154_phy_2G4Hz_coex_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_COEX_PHY && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivCoex =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_coex_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzFem =
  &sl_rail_ieee802154_phy_2G4Hz_fem_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivFem =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_IEEE802154_SUPPORTS_COEX_PHY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzCoexFem =
  &sl_rail_ieee802154_phy_2G4Hz_coex_fem_39MHz_channelConfig;
#endif

#if RAIL_IEEE802154_SUPPORTS_FEM_PHY && RAIL_IEEE802154_SUPPORTS_COEX_PHY \
  && RAIL_SUPPORTS_ANTENNA_DIVERSITY
const RAIL_ChannelConfig_t *const RAIL_IEEE802154_Phy2p4GHzAntDivCoexFem =
  &sl_rail_ieee802154_phy_2G4Hz_antdiv_coex_fem_39MHz_channelConfig;
#endif

#endif // SL_DEVICE_INIT_HFXO_FREQ
