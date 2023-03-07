/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"

/** @brief StateChange
 *
 * This callback is called when a device's state changes.
 *
 * @param nodeId   Ver.: always
 * @param state   Ver.: always
 */
WEAK(void emberAfPluginDeviceTableStateChangeCallback(EmberNodeId nodeId,
                                                      uint8_t state))
{
}

/** @brief Cleared
 *
 * This callback is called when a device table has been cleared.
 *
 */
WEAK(void emberAfPluginDeviceTableClearedCallback(void))
{
}

/** @brief NewDevice
 *
 * This callback is called when a new device joins the gateway.
 *
 * @param uui64   Ver.: always
 */
WEAK(void emberAfPluginDeviceTableNewDeviceCallback(EmberEUI64 eui64))
{
}

/** @brief Rejoin device
 *
 * Called when a device rejoins.
 *
 *@param newNodeEui64:  EUI64 of the rejoined device.
 */
WEAK(void emberAfPluginDeviceTableRejoinDeviceCallback(EmberEUI64 newNodeEui64))
{
}

/*
 *
 * Called when a device leaves.
 *
 *@param newNodeEui64:  EUI64 of the device that left.
 */
WEAK(void emberAfPluginDeviceTableDeviceLeftCallback(EmberEUI64 newNodeEui64))
{
}

/*
 *
 * Called when the device table has been initialized.
 *
 */
WEAK(void emberAfPluginDeviceTableInitialized(void))
{
}

/*
 *
 * Called when the device has been removed from the table.
 *
 *@param currentIndex:  Index of the removed device.
 *
 */
WEAK(void emberAfPluginDeviceTableIndexRemovedCallback(uint16_t currentIndex))
{
}

/*
 *
 * Called when the device table has been initialized.
 *
 *@param index:  Index of the removed device.
 *
 */
WEAK(void emberAfPluginDeviceTableIndexAddedCallback(uint16_t index))
{
}
