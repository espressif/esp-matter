/***************************************************************************//**
 * @file
 * @brief Host routines for the Network Steering plugin.
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
#include "network-steering.h"
#include "network-steering-internal.h"

//============================================================================
// Globals

#define MAX_NETWORKS 16

static uint16_t storedNetworks[MAX_NETWORKS];
static bool memoryCleared = false;

//============================================================================
// Forward Declarations

//============================================================================

uint8_t emAfPluginNetworkSteeringGetMaxPossiblePanIds(void)
{
  return MAX_NETWORKS;
}

void emAfPluginNetworkSteeringClearStoredPanIds(void)
{
  memoryCleared = false;
}

uint16_t* emAfPluginNetworkSteeringGetStoredPanIdPointer(uint8_t index)
{
  if (index >= MAX_NETWORKS) {
    return NULL;
  }
  if (!memoryCleared) {
    MEMSET(storedNetworks, 0xFF, sizeof(uint16_t) * MAX_NETWORKS);
    memoryCleared = true;
  }
  return &(storedNetworks[index]);
}

void emberAfPluginNetworkSteeringMarker(void)
{
  // This is a stub because no marker is necessary on host
}
