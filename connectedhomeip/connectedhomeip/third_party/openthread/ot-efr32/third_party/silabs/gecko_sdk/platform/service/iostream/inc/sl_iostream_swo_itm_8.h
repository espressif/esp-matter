/***************************************************************************//**
 * @file
 * @brief IO Stream SWO Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_IOSTREAM_SWO_ITM_8_H
#define SL_IOSTREAM_SWO_ITM_8_H

#include "sl_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

SL_ENUM_GENERIC(sl_iostream_swo_itm_8_msg_type_t, uint16_t) {
  EM_DEBUG_TIME_SYNC = 0x0000,
  EM_DEBUG_RESET_INFO = 0x0001,
  EM_DEBUG_PRINTF = 0x0002,
  EM_DEBUG_API_TRACE = 0x0003,
  EM_DEBUG_ASSERT = 0x0004,
  EM_DEBUG_CORE_DUMP = 0x0005,
  EM_DEBUG_PHY_RX = 0x0006,
  EM_DEBUG_API_RX = 0x0007,
  EM_DEBUG_PHY_TX = 0x0008,
  EM_DEBUG_API_TX = 0x0009,
  EM_DEBUG_PACKET_TRACE = 0x000A,
  EM_DEBUG_ERROR = 0x000B,
  EM_DEBUG_STATS = 0x000C,
  EM_DEBUG_TIME_SYNC_TEST = 0x000D,
  EM_DEBUG_RADIO_REBOOT_COUNT = 0x000E,
  EM_DEBUG_VIRTUAL_UART_TX = 0x0011,
  EM_DEBUG_VIRTUAL_UART_RX = 0x0012,
  EM_PACKET_TRACE_EM2420_TX = 0x0020,
  EM_PACKET_TRACE_EM2420_RX = 0x0021,
  EM_PACKET_TRACE_EM2XX_TX = 0x0022,
  EM_PACKET_TRACE_EM2XX_RX = 0x0023,
  EM_PACKET_TRACE_EM3XX_TX = 0x0024,
  EM_PACKET_TRACE_EM3XX_RX = 0x0025,
  EM_PACKET_TRACE_PRO2P_TX = 0x0026,
  EM_PACKET_TRACE_PRO2P_RX = 0x0027,
  EM_PACKET_TRACE_PRO2P_OTHER = 0x0028,
  EM_PACKET_TRACE_EFR_TX = 0x0029,
  EM_PACKET_TRACE_EFR_RX = 0x002A,
  EM_PACKET_TRACE_EFR_OTHER = 0x002B,
  EM_FLASH_READ_REQUEST = 0x0030,
  EM_FLASH_READ_RESPONSE = 0x0031,
  EM_EEPROM_READ_REQUEST = 0x0032,
  EM_EEPROM_READ_RESPONSE = 0x0033,
  EM_EEPROM_WRITE_REQUEST = 0x0034,
  EM_EEPROM_WRITE_RESPONSE = 0x0035,
  EM_RAM_READ_REQUEST = 0x0036,
  EM_RAM_READ_RESPONSE = 0x0037,
  EM_RAM_WRITE_REQUEST = 0x0038,
  EM_RAM_WRITE_RESPONSE = 0x0039,
  EM_DEBUG_INFO_REQUEST = 0x003A,
  EM_DEBUG_INFO_RESPONSE = 0x003B,
  EM_DEBUG_EZSP = 0x003C,
  EM_DEBUG_EZSP_UART = 0x003D,
  EM_DEBUG_DAG_TRACE = 0x003E,
  EM_SIMULATED_EZSP_CALLBACK_READY = 0x003F,
  EM_SIMULATED_WAKEUP_NCP = 0x0040,
  EM_SIMULATED_NCP_IS_AWAKE = 0x0041,
  EM_DEBUG_INFO_ZNET_VERSION = 0x0042,
  EM_DEBUG_INFO_ZIP_VERSION = 0x0043,
  EM_DEBUG_TIME = 0x0044,
  EM_DEBUG_HEAP_STACK = 0x0045,
  EM_DEBUG_MUSTANG = 0x0046,
  EM_DEBUG_LATENCY = 0x0047,
  EM_DEBUG_TMSP = 0x0048,
  EM_AEM_SAMPLE = 0x0050,
  EM_AEM_COUNTER = 0x0051,
  EM_AEM_REQUEST = 0x0060,
  EM_AEM_RESPONSE = 0x0061,
  EM_AEM_CURRENT_PACKET = 0x0062,
  EM_CPU_USAGE = 0x0070,
  EM_COS_PACKET = 0x0080,
  EM_DEBUG_USER_COMMAND = 0xFFFE,
  EM_DEBUG_USER_RESPONSE = 0xFFFF,
};

#ifdef __cplusplus
}
#endif

#endif /* SL_IOSTREAM_SWO_ITM_8_H */
