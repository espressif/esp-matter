/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_UTIL_INIT_BLE_CONFIG_H
#define SL_RAIL_UTIL_INIT_BLE_CONFIG_H

#include "rail_types.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> RAIL Init Configuration
// <e SL_RAIL_UTIL_INIT_BLE_ENABLE> Initialize RAIL
// <i> Default: 1
#define SL_RAIL_UTIL_INIT_BLE_ENABLE 1
// </e>

// <h> Project Configuration
// <q SL_RAIL_UTIL_INIT_BLE_SUPPORT_BLE_ENABLE> Enable Bluetooth Support
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_BLE_SUPPORT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE> Enable Scheduler Support (for DMP)
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_INIT_COMPLETE_CALLBACK_BLE_ENABLE> Enable RAIL Init Complete Callback
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_INIT_COMPLETE_CALLBACK_BLE_ENABLE 1
// </h>

// <e SL_RAIL_UTIL_INIT_PROTOCOLS_BLE_ENABLE> Protocol Configuration
// <i> Default: 1
#define SL_RAIL_UTIL_INIT_PROTOCOLS_BLE_ENABLE 1
// </e>

// <h> Available BLE standards
// <o SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT> Default Radio Configuration
// <SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS=> Bluetooth LE 1Mbps
// <SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS=> Bluetooth LE 2Mbps
// <SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS=> Bluetooth LE Coded 125Kbps
// <SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS=> Bluetooth LE Coded 500Kbps
// <SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS=> Bluetooth Quuppa 1Mbps
// <i> Default: SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS
#define SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT  SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS
// </h>

// <e SL_RAIL_UTIL_INIT_CALIBRATIONS_BLE_ENABLE> Calibration Configuration
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_CALIBRATIONS_BLE_ENABLE 0
// </e>

// <h> Calibration Notifications
// <q SL_RAIL_UTIL_INIT_CALIBRATION_TEMPERATURE_NOTIFY_BLE_ENABLE> Enable Temperature Calibration Notifications (RAIL_EVENT_CAL_NEEDED radio event issued when temperature calibrations needed, for example VCO calibration)
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_CALIBRATION_TEMPERATURE_NOTIFY_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_CALIBRATION_ONETIME_NOTIFY_BLE_ENABLE> Enable One-time Calibration Notifications (RAIL_EVENT_CAL_NEEDED radio event issued when one-time calibrations needed, for example IR calibration)
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_CALIBRATION_ONETIME_NOTIFY_BLE_ENABLE 0
// </h>

// <e SL_RAIL_UTIL_INIT_TRANSITIONS_BLE_ENABLE> Auto Transition Configuration
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_TRANSITIONS_BLE_ENABLE 1
// </e>

// <h> TX Transitions
// <o SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_SUCCESS> Transition on TX Success
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_SUCCESS  RAIL_RF_STATE_RX
// <o SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_ERROR> Transition on TX Error
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_ERROR  RAIL_RF_STATE_RX
// </h>

// <h> RX Transitions
// <o SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_SUCCESS> Transition on RX Success
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_TX=> TX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_SUCCESS  RAIL_RF_STATE_RX
// <o SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_ERROR> Transition on RX Error
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_ERROR  RAIL_RF_STATE_RX
// </h>

// <e SL_RAIL_UTIL_INIT_DATA_FORMATS_BLE_ENABLE> Data Format Configuration
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_DATA_FORMATS_BLE_ENABLE 1
// </e>

// <h> TX Data
// <o SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_SOURCE> Source of TX Data
// <TX_PACKET_DATA=> Use frame hardware to packetize data
// <i> Default: TX_PACKET_DATA
#define SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_SOURCE  TX_PACKET_DATA
// <o SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_MODE> Method of Providing TX Data
// <PACKET_MODE=> Packet Mode
// <FIFO_MODE=> FIFO Mode
// <i> Default: PACKET_MODE
#define SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_MODE  PACKET_MODE
// </h>

// <h> RX Data
// <o SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_SOURCE> Source of RX Data
// <RX_PACKET_DATA=> Use frame hardware to packetize data
// <RX_DEMOD_DATA=> Get 8-bit output from demodulator
// <RX_IQDATA_FILTLSB=> Get lowest 16 bits of I/Q data provided to demodulator
// <RX_IQDATA_FILTMSB=> Get highest 16 bits of I/Q data provided to demodulator
// <i> Default: RX_PACKET_DATA
#define SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_SOURCE  RX_PACKET_DATA
// <o SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_MODE> Method of Retrieving RX Data
// <PACKET_MODE=> Packet Mode
// <FIFO_MODE=> FIFO Mode
// <i> Default: PACKET_MODE
#define SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_MODE  PACKET_MODE
// </h>

// <e SL_RAIL_UTIL_INIT_EVENTS_BLE_ENABLE> Radio Event Configuration
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENTS_BLE_ENABLE 1
// </e>

// <h> RX Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_RX_TIMING_DETECT_BLE_ENABLE> RX Timing Detect
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_TIMING_DETECT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_TIMING_LOST_BLE_ENABLE> RX Timing Lost
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_TIMING_LOST_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_PREAMBLE_DETECT_BLE_ENABLE> RX Preamble Detect
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_PREAMBLE_DETECT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_PREAMBLE_LOST_BLE_ENABLE> RX Preamble Lost
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_PREAMBLE_LOST_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_SYNC1_DETECT_BLE_ENABLE> RX Sync1 Detect
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_SYNC1_DETECT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_SYNC2_DETECT_BLE_ENABLE> RX Sync2 Detect
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_SYNC2_DETECT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_FILTER_PASSED_BLE_ENABLE> RX Filter Passed
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_FILTER_PASSED_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_ADDRESS_FILTERED_BLE_ENABLE> RX Address Filtered
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_ADDRESS_FILTERED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_PACKET_RECEIVED_BLE_ENABLE> RX Packet Received
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_PACKET_RECEIVED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_FRAME_ERROR_BLE_ENABLE> RX Frame Error
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_FRAME_ERROR_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_PACKET_ABORTED_BLE_ENABLE> RX Packet Aborted
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_PACKET_ABORTED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_TIMEOUT_BLE_ENABLE> RX Timeout
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_TIMEOUT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_CHANNEL_HOPPING_COMPLETE_BLE_ENABLE> RX Channel Hopping Complete
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_CHANNEL_HOPPING_COMPLETE_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_DUTY_CYCLE_RX_END_BLE_ENABLE> RX Duty Cycle RX End
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_DUTY_CYCLE_RX_END_BLE_ENABLE 0
// </h>
// <h> TX Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_TX_PACKET_SENT_BLE_ENABLE> TX Packet Sent
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_PACKET_SENT_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_ABORTED_BLE_ENABLE> TX Aborted
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_ABORTED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_BLOCKED_BLE_ENABLE> TX Blocked
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_BLOCKED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_STARTED_BLE_ENABLE> TX Started
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_STARTED_BLE_ENABLE 0
// </h>
// <h> RSSI Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_RSSI_AVERAGE_DONE_BLE_ENABLE> RSSI Average Done
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RSSI_AVERAGE_DONE_BLE_ENABLE 0
// </h>
// <h> FIFO Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_ALMOST_FULL_BLE_ENABLE> RX FIFO, Almost Full
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_ALMOST_FULL_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_FULL_BLE_ENABLE> RX FIFO, Full
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_FULL_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_OVERFLOW_BLE_ENABLE> RX FIFO, Overflow
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_FIFO_OVERFLOW_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_FIFO_ALMOST_EMPTY_BLE_ENABLE> TX FIFO, Almost Empty
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_FIFO_ALMOST_EMPTY_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TX_UNDERFLOW_BLE_ENABLE> TX FIFO, Underflow
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_UNDERFLOW_BLE_ENABLE 1
// </h>
// <h> CCA Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_TX_CHANNEL_CLEAR_BLE_ENABLE> TX CCA, Channel Clear
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_CHANNEL_CLEAR_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TX_CHANNEL_BUSY_BLE_ENABLE> TX CCA, Channel Busy
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_CHANNEL_BUSY_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_CCA_RETRY_BLE_ENABLE> TX CCA, Retry
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_CCA_RETRY_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TX_START_CCA_BLE_ENABLE> TX CCA, Started
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_START_CCA_BLE_ENABLE 0
// </h>
// <h> Scheduled Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_RX_TX_SCHEDULED_RX_TX_STARTED_BLE_ENABLE> Scheduled RX/TX Started
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_TX_SCHEDULED_RX_TX_STARTED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_RX_SCHEDULED_RX_END_BLE_ENABLE> Scheduled RX End
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_SCHEDULED_RX_END_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_RX_SCHEDULED_RX_MISSED_BLE_ENABLE> Scheduled RX Missed
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_SCHEDULED_RX_MISSED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_TX_SCHEDULED_TX_MISSED_BLE_ENABLE> Scheduled TX Missed
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TX_SCHEDULED_TX_MISSED_BLE_ENABLE 1
// </h>
// <h> ACK Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_RX_ACK_TIMEOUT_BLE_ENABLE> RX ACK, Timeout
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_RX_ACK_TIMEOUT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TXACK_PACKET_SENT_BLE_ENABLE> TX ACK, Packet Sent
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TXACK_PACKET_SENT_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TXACK_ABORTED_BLE_ENABLE> TX ACK, Aborted
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TXACK_ABORTED_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TXACK_BLOCKED_BLE_ENABLE> TX ACK, Blocked
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TXACK_BLOCKED_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_TXACK_UNDERFLOW_BLE_ENABLE> TX ACK, FIFO Underflow
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_TXACK_UNDERFLOW_BLE_ENABLE 0
// </h>
// <h> Protocol Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_IEEE802154_DATA_REQUEST_COMMAND_BLE_ENABLE> IEEE 802.15.4 Data Request Command
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_IEEE802154_DATA_REQUEST_COMMAND_BLE_ENABLE 0
// <q SL_RAIL_UTIL_INIT_EVENT_ZWAVE_BEAM_BLE_ENABLE> Z-Wave Beam
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_ZWAVE_BEAM_BLE_ENABLE 0
// </h>
// <h> Dynamic Multiprotocol (DMP) Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_CONFIG_UNSCHEDULED_BLE_ENABLE> Config Unscheduled
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_CONFIG_UNSCHEDULED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_CONFIG_SCHEDULED_BLE_ENABLE> Config Scheduled
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_CONFIG_SCHEDULED_BLE_ENABLE 1
// <q SL_RAIL_UTIL_INIT_EVENT_SCHEDULER_STATUS_BLE_ENABLE> Scheduler Status
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_SCHEDULER_STATUS_BLE_ENABLE 1
// </h>
// <h> Calibration Radio Events
// <q SL_RAIL_UTIL_INIT_EVENT_CAL_NEEDED_BLE_ENABLE> Calibration Needed
// <i> Default: 0
#define SL_RAIL_UTIL_INIT_EVENT_CAL_NEEDED_BLE_ENABLE 1
// </h>
// </h>
// <<< end of configuration section >>>

#endif // SL_RAIL_UTIL_INIT_BLE_CONFIG_H
