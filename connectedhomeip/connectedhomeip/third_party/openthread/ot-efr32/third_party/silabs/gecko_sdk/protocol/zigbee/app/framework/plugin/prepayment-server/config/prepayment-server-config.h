/***************************************************************************//**
 * @brief Zigbee Prepayment Server component configuration header.
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

// <h>Zigbee Prepayment Server configuration

// <o EMBER_AF_PLUGIN_PREPAYMENT_SERVER_MAX_PENDING_PAYMENT_MODES> Maximum Number of Pending Payment Modes <1-255>
// <i> Default: 5
// <i> The maximum number of future payment modes that can be stored.
#define EMBER_AF_PLUGIN_PREPAYMENT_SERVER_MAX_PENDING_PAYMENT_MODES   5

// <o EMBER_AF_PLUGIN_PREPAYMENT_SERVER_DEBT_LOG_CAPACITY> Maximum Number of Debt Log Entries <1-255>
// <i> Default: 10
// <i> The maximum number of entries that can be stored in the debt log.
#define EMBER_AF_PLUGIN_PREPAYMENT_SERVER_DEBT_LOG_CAPACITY   10

// </h>

// <<< end of configuration section >>>
