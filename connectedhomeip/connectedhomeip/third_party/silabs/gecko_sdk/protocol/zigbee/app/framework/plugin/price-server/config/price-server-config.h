/***************************************************************************//**
 * @brief Zigbee Price Server component configuration header.
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

// <h>Zigbee Price Server configuration

// <o EMBER_AF_PLUGIN_PRICE_SERVER_PRICE_TABLE_SIZE> Price table size <5-255>
// <i> Default: 5
// <i> Maximum amount of saved prices
#define EMBER_AF_PLUGIN_PRICE_SERVER_PRICE_TABLE_SIZE   5

// <o EMBER_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE> Tariff table size <2-255>
// <i> Default: 2
// <i> Maximum number of stored tariffs
#define EMBER_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_CONVERSION_FACTOR_TABLE_SIZE> Conversion Factor table size <2-255>
// <i> Default: 2
// <i> Maximum number of stored conversion factors
#define EMBER_AF_PLUGIN_PRICE_SERVER_CONVERSION_FACTOR_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_CALORIFIC_VALUE_TABLE_SIZE> Calorific Value table size <2-255>
// <i> Default: 2
// <i> Maximum number of stored calorific values
#define EMBER_AF_PLUGIN_PRICE_SERVER_CALORIFIC_VALUE_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_CO2_VALUE_TABLE_SIZE> CO2 Value table size <2-255>
// <i> Default: 2
// <i> Maximum number of Co2  values
#define EMBER_AF_PLUGIN_PRICE_SERVER_CO2_VALUE_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_TIER_LABELS_TABLE_SIZE> Tier Label table size <2-255>
// <i> Default: 2
// <i> Maximum number of tier labels
#define EMBER_AF_PLUGIN_PRICE_SERVER_TIER_LABELS_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_BILLING_PERIOD_TABLE_SIZE> Billing Period table size <2-255>
// <i> Default: 2
// <i> Maximum number of billing period entries
#define EMBER_AF_PLUGIN_PRICE_SERVER_BILLING_PERIOD_TABLE_SIZE   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_CONSOLIDATED_BILL_TABLE_SIZE> Consolidated Bill table size <5-255>
// <i> Default: 5
// <i> Maximum number of consolidated bills
#define EMBER_AF_PLUGIN_PRICE_SERVER_CONSOLIDATED_BILL_TABLE_SIZE   5

// <o EMBER_AF_PLUGIN_PRICE_SERVER_CREDIT_PAYMENT_TABLE_SIZE> Credit Payment table size <5-255>
// <i> Default: 5
// <i> Maximum number of stored credit payments
#define EMBER_AF_PLUGIN_PRICE_SERVER_CREDIT_PAYMENT_TABLE_SIZE   5

// <o EMBER_AF_PLUGIN_PRICE_SERVER_MAX_TIERS_PER_TARIFF> Max tiers per tariff <1-255>
// <i> Default: 2
// <i> Maximum number of tiers and tier labels for a given tariff ID
#define EMBER_AF_PLUGIN_PRICE_SERVER_MAX_TIERS_PER_TARIFF   2

// <o EMBER_AF_PLUGIN_PRICE_SERVER_BLOCK_PERIOD_TABLE_SIZE> Block period table size <1-255>
// <i> Default: 2
// <i> The number of block period table entries
#define EMBER_AF_PLUGIN_PRICE_SERVER_BLOCK_PERIOD_TABLE_SIZE   2

// </h>

// <<< end of configuration section >>>
