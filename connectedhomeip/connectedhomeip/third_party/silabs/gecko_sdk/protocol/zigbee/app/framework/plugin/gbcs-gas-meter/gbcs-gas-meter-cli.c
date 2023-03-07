/***************************************************************************//**
 * @file
 * @brief CLI for the GBCS Gas Meter plugin.
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

#include "app/framework/include/af.h"
#include "gbcs-gas-meter.h"

#ifdef UC_BUILD

// plugin gbcs-gas-meter report-attributes
void emAfPluginGbcsGasMeterCliReportAttributes(sl_cli_command_arg_t *arguments)
{
  emberAfPluginGbcsGasMeterReportAttributes();
}

#else

// plugin gbcs-gas-meter report-attributes
void emAfPluginGbcsGasMeterCliReportAttributes(void)
{
  emberAfPluginGbcsGasMeterReportAttributes();
}

#endif
