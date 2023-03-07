/***************************************************************************//**
 * @file
 * @brief app_bluetooth.h
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
#ifndef APP_BLUETOOTH_H
#define APP_BLUETOOTH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This function activates the BLE advertising to be connectable for mobiles
 *
 * @param None
 * @return None
 ******************************************************************************/
void activate_bluetooth(void);

/*******************************************************************************
 * This function deactivates the BLE advertising
 *
 * @param None
 * @return None
 ******************************************************************************/
void deactivate_bluetooth(void);

/*******************************************************************************
 * API advertises the received packets - note that we are not connected here
 *
 * @param rssi: current rssi value of the received packet
 * @param packet_count: the packet count
 * @param received_packets: corretly received packets
 * @return None
 ******************************************************************************/
void advertise_received_data(int8_t rssi, uint16_t packet_count, uint16_t received_packets);

/*******************************************************************************
 * Add a characteristic indication to the queue
 *
 * @param characteristic
 * @return None
 ******************************************************************************/
void add_bluetooth_indication(uint16_t characteristic);

/*******************************************************************************
 * Send the first indication in the queue
 *
 * @param None
 * @return None
 ******************************************************************************/
void send_bluetooth_indications(void);

/*******************************************************************************
 * After RX measurement we have to restart the advertising
 *
 * @param None
 * @return None
 ******************************************************************************/
void manage_bluetooth_restart(void);

/*******************************************************************************
 * Check for active bluetooth connection
 * @return bool: true if there is an active connection
 ******************************************************************************/
bool is_bluetooth_connected(void);

#endif // APP_BLUETOOTH_H
