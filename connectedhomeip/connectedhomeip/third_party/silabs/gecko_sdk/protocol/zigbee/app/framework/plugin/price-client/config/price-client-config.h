/***************************************************************************//**
 * @brief Zigbee Price Client component configuration header.
 *\n*******************************************************************************
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Price Client configuration

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE> Price table size <2-255>
// <i> Default: 2
// <i> Maximum number of saved prices.  The spec requires Price clients to save at least two prices.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CONSOLIDATED_BILL_TABLE_SIZE> Consolidated Bill Table Size <2-255>
// <i> Default: 2
// <i> Number of stored consolidated bills
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CONSOLIDATED_BILL_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE> Tier label Table Size <1-255>
// <i> Default: 2
// <i> Number of tier label entries to store.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF> Max number of tiers per tariff <1-255>
// <i> Default: 2
// <i> The maximum number of tiers and tier labels that can be stored per tariff.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE> Credit payment table size <1-255>
// <i> Default: 5
// <i> Maximum number of credit payment events that can be stored.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE   5

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE> Currency conversion table size <1-255>
// <i> Default: 2
// <i> Maximum number of currency conversion events that can be stored.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE> CO2 Table Size <2-255>
// <i> Default: 2
// <i> Size of the client CO2 table
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE> Conversion factor table size <2-255>
// <i> Default: 2
// <i> The number of conversion factors to store.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE> Calorific value table size <2-255>
// <i> Default: 2
// <i> The number of calorific values to store.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE> Billing period table size <0-255>
// <i> Default: 2
// <i> The number of billing periods to store.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE> Block Period Table Size <2-255>
// <i> Default: 2
// <i> Size of the block period table
#define EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_THRESHOLD_TABLE_SIZE> Block Threshold Table Size <2-255>
// <i> Default: 2
// <i> Size of the block threshold table.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_THRESHOLD_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_BLOCK_THRESHOLDS> Maximum Number Block Thresholds <0-15>
// <i> Default: 4
// <i> Maximum number of block thresholds stored on the client, per tier.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_BLOCK_THRESHOLDS   4

// <o EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_TIERS> Maximum Number Tiers <0-15>
// <i> Default: 5
// <i> Maximum number of tiers stored on the client.
#define EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_TIERS   5

// </h>

// <<< end of configuration section >>>
