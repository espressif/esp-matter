/***************************************************************************//**
 * @file
 * @brief Common functions for Throughput Test UI
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "throughput_ui.h"
#include "throughput_ui_types.h"

/**************************************************************************//**
 * Sets all values.
 *****************************************************************************/
void throughput_ui_set_all(throughput_t status)
{
  throughput_ui_set_role(status.role);
  throughput_ui_set_state(status.state);
  throughput_ui_set_tx_power(status.tx_power);
  throughput_ui_set_rssi(status.rssi);
  throughput_ui_set_phy(status.phy);
  throughput_ui_set_connection_interval(status.interval);
  throughput_ui_set_pdu_size(status.pdu_size);
  throughput_ui_set_mtu_size(status.mtu_size);
  throughput_ui_set_data_size(status.data_size);
  throughput_ui_set_notifications(status.notifications);
  throughput_ui_set_indications(status.indications);
  throughput_ui_set_throughput(status.throughput);
  throughput_ui_set_count(status.count);
  throughput_ui_update();
}
