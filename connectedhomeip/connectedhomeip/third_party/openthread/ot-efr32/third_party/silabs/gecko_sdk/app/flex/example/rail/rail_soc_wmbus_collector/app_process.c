/***************************************************************************//**
 * @file
 * @brief app_process.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_component_catalog.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "app_log.h"
#include "sl_wmbus_support.h"
#include "em_emu.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define RAIL_FIFO_SIZE (256U)
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void print_blocks(const uint8_t *buffer, uint16_t length);
static void print_rx_packets(RAIL_Handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static volatile state_t state = S_IDLE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Receive FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void set_next_state(state_t next_state)
{
  state = next_state;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;

  switch (state) {
    case S_PACKET_RECEIVED:
      print_rx_packets(rail_handle);
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      app_log_error("Radio RX Error occurred\nEvents: %lld\n", current_rail_err);
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_CALIBRATION_ERROR:
      app_log_warning("Radio Calibration Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                      current_rail_err,
                      calibration_status);
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_IDLE:
      break;
    default:
      break;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Put your application code here!                                       //
  // This is called infinitely.                                            //
  // Do not call blocking functions from here!                             //
  ///////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      RAIL_HoldRxPacket(rail_handle);
      state = S_PACKET_RECEIVED;
    } else {
      // Handle Rx error
      current_rail_err = (events & RAIL_EVENTS_RX_COMPLETION);
      state = S_RX_PACKET_ERROR;
    }
  }

  // Perform all calibrations when needed
  if (events & RAIL_EVENT_CAL_NEEDED) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
      state = S_CALIBRATION_ERROR;
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void print_blocks(const uint8_t *buffer, uint16_t length)
{
  for (uint8_t i = 0; i < length; i++) {
    if (i % 16 == 0) {
      app_log_info("[0x%02X ", buffer[i]);
    } else if (i % 16 == 8) {
      app_log_info("| 0x%02X ", buffer[i]);
    } else if (i % 16 == 15 || i == length - 1) {
      app_log_info("0x%02X]\n", buffer[i]);
    } else {
      app_log_info("0x%02X ", buffer[i]);
    }
  }
}

static void print_rx_packets(RAIL_Handle_t rail_handle)
{
  RAIL_RxPacketInfo_t packet_info;
  RAIL_Status_t rail_status;
  RAIL_RxPacketDetails_t packet_details;
  RAIL_RxPacketHandle_t rx_packet_handle;

  rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);

  while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
    packet_details.timeReceived.totalPacketBytes = packet_info.packetBytes;
    packet_details.timeReceived.timePosition = RAIL_PACKET_TIME_AT_SYNC_END;
    RAIL_GetRxPacketDetails(rail_handle, rx_packet_handle, &packet_details);

    RAIL_CopyRxPacket(rx_fifo, &packet_info);
    rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
    }

    const WMBUS_dll_header_t *dllHeader = (WMBUS_dll_header_t*)rx_fifo;
    const WMBUS_stl_header_t *stlHeader = (WMBUS_stl_header_t*)(rx_fifo + sizeof(WMBUS_dll_header_t));

    char mField[3];
    WMBUSframe_MField2Chars(dllHeader->address.detailed.manufacturer, mField);
    app_log_info("RX:[Time:%lu]\n", packet_details.timeReceived.packetTime);
    app_log_info("Block-1:[L:%d,C:0x%02X,M:%c%c%c,ID:%08X,Version:0x%02X,devType:0x%02X]\n",
                 dllHeader->lField,
                 dllHeader->cField.raw,
                 mField[0], mField[1], mField[2],
                 (unsigned int)dllHeader->address.detailed.id,
                 (unsigned int)dllHeader->address.detailed.version,
                 (unsigned int)dllHeader->address.detailed.deviceType);
    if (stlHeader->ciField == WMBUS_CI_EN13757_3_APPLICATION_SHORT) {
      uint8_t *payload_start = rx_fifo + sizeof(WMBUS_dll_header_t) + sizeof(WMBUS_stl_header_t);
      uint16_t payload_len = dllHeader->lField - sizeof(WMBUS_dll_header_t) - sizeof(WMBUS_stl_header_t) + 1;
      app_log_info("AppHeader:[CI:0x%02X,AccessNr:%d,Status:0x%02X,encMode:%d,Accessibility:%02X,encBlocks:%d,sync:%d]\n",
                   stlHeader->ciField,
                   stlHeader->accessNumber,
                   stlHeader->status,
                   stlHeader->confWord.mode_0_5.mode,
                   stlHeader->confWord.mode_0_5.accessibility,
                   stlHeader->confWord.mode_0_5.numOfEncBlocks,
                   stlHeader->confWord.mode_0_5.synchronized);
      if (stlHeader->confWord.mode_0_5.mode == 5) {
        uint8_t iv[16];
        //with long transport layer header, the address from the header should be used
        memcpy(iv, &(dllHeader->address.raw), 8);
        memset(iv + 8, stlHeader->accessNumber, 8);
        WMBUSframe_crypto5decrypt(payload_start, payload_start, iv, payload_len);
      }
      print_blocks(payload_start, payload_len);
    } else {
      print_blocks(rx_fifo + sizeof(WMBUS_dll_header_t), dllHeader->lField - sizeof(WMBUS_dll_header_t) + 1);
    }

    sl_led_toggle(&sl_led_led0);

    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
  }
}
