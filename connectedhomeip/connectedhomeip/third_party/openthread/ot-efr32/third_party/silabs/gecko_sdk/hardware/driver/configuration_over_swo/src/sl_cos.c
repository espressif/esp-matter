/***************************************************************************//**
 * @file
 * @brief Configuration Over SWO Component
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "sl_cos.h"
#if defined (SLI_COS_UARTDRV_VCOM_PRESENT)
#include "sl_uartdrv_instances.h"
#endif // SLI_COS_UARTDRV_VCOM_PRESENT
#if defined (SL_CATALOG_RAIL_UTIL_PTI_PRESENT)
#include "sl_rail_util_pti_config.h"
#endif // SL_CATALOG_RAIL_UTIL_PTI_PRESENT

#include "sl_status.h"
#include "sl_debug_swo.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#if defined (__ICCARM__)
#define DCH_PACKED_STRUCT __packed struct
#elif defined (__clang__) || defined (__ARM_ARCH)
#define DCH_PACKED_STRUCT struct __attribute__ ((__packed__))
#elif defined (__GNUC__)
#define DCH_PACKED_STRUCT struct __attribute__ ((gcc_struct, __packed__))
#endif

#define LOW_BYTE(n)                     ((uint8_t)((n) & 0xFF))
#define HIGH_BYTE(n)                    ((uint8_t)(LOW_BYTE((n) >> 8)))

#define COS_CONFIG_OPTION_TYPE_NULL     0
#define COS_CONFIG_OPTION_TYPE_UART     1
#define COS_CONFIG_OPTION_TYPE_PTI      2

#define COS_CONFIG_BAUDRATE_POS         0
#define COS_CONFIG_BAUDRATE_MASK        0x00FFFFFFLU

#define COS_CONFIG_UART_STOPBITS_1      0
#define COS_CONFIG_UART_STOPBITS_1_5    1
#define COS_CONFIG_UART_STOPBITS_2      2
#define COS_CONFIG_UART_STOPBITS_LAST   COS_CONFIG_UART_STOPBITS_2
#define COS_CONFIG_UART_STOPBITS_POS    24
#define COS_CONFIG_UART_STOPBITS_MASK   0x03LU

#define COS_CONFIG_UART_PARITY_NONE     0
#define COS_CONFIG_UART_PARITY_EVEN     1
#define COS_CONFIG_UART_PARITY_ODD      2
#define COS_CONFIG_UART_PARITY_LAST     COS_CONFIG_UART_PARITY_ODD
#define COS_CONFIG_UART_PARITY_POS      26
#define COS_CONFIG_UART_PARITY_MASK     0x03LU

#define COS_CONFIG_UART_MODE_UART       0
#define COS_CONFIG_UART_MODE_SPISLAVE   1
#define COS_CONFIG_UART_MODE_SPIMASTER  2
#define COS_CONFIG_UART_MODE_LAST       COS_CONFIG_UART_MODE_ENABLED
#define COS_CONFIG_UART_MODE_POS        28
#define COS_CONFIG_UART_MODE_MASK       0x03LU

#define COS_CONFIG_UART_CTS_DISABLED    0
#define COS_CONFIG_UART_CTS_ENABLED     1
#define COS_CONFIG_UART_CTS_LAST        COS_CONFIG_UART_CTS_ENABLED
#define COS_CONFIG_UART_CTS_POS         30
#define COS_CONFIG_UART_CTS_MASK        0x01LU

#define COS_CONFIG_UART_RTS_DISABLED    0
#define COS_CONFIG_UART_RTS_ENABLED     1
#define COS_CONFIG_UART_RTS_LAST        COS_CONFIG_UART_RTS_ENABLED
#define COS_CONFIG_UART_RTS_POS         31
#define COS_CONFIG_UART_RTS_MASK        0x01LU
#define COS_CONFIG_UART_FC_POS          30
#define COS_CONFIG_UART_FC_MASK         0x03LU

#define COS_CONFIG_PTI_MODE_POS         27
#define COS_CONFIG_PTI_MODE_MASK        0x07LU

#define COS_CONFIG_PTI_INTERFACE_POS    30
#define COS_CONFIG_PTI_INTERFACE_MASK   0x03LU

// SWO message type for COS
#define EM_COS_PACKET                   0X0080LU

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

#if defined (SLI_COS_UARTDRV_VCOM_PRESENT)
static UARTDRV_Handle_t uartdrv_handle = NULL;
#endif // SLI_COS_UARTDRV_VCOM_PRESENT

/***************************************************************************//**
 * Structure to store the COS Config Options
 ******************************************************************************/
typedef DCH_PACKED_STRUCT __COS_ConfigOption {
  uint8_t  optionType;     ///< option type
  uint8_t  reserved1;      ///< reserved one
  uint16_t reserved2;      ///< reserved two
  uint32_t optionValue;    ///< option value
} COS_ConfigOption_t;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

#if defined (SLI_COS_UARTDRV_VCOM_PRESENT)
static uint32_t sli_cos_vcom_config(uint32_t baudrate, uint8_t flow_control);
static void sli_cos_vcom_write(void);
#endif // SLI_COS_UARTDRV_VCOM_PRESENT

#if defined (SL_CATALOG_RAIL_UTIL_PTI_PRESENT)
static uint32_t sli_cos_pti_config(uint32_t baudrate, RAIL_PtiMode_t mode, uint8_t interface);
static void sli_cos_pti_write(void);
#endif // SL_CATALOG_RAIL_UTIL_PTI_PRESENT

static sl_status_t sli_cos_swo_itm_8_write(const void *buffer,
                                           size_t buffer_length);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the SWO ITM 8 and initiates VCOM/PTI SWO writes.
 ******************************************************************************/
void sl_cos_send_config(void)
{
  // Configure SWO stimulus 8
  sl_debug_swo_enable_itm(8U);

#if defined (SLI_COS_UARTDRV_VCOM_PRESENT)
  sli_cos_vcom_write();
#endif // SLI_COS_UARTDRV_VCOM_PRESENT

#if defined (SL_CATALOG_RAIL_UTIL_PTI_PRESENT)
  // If RAIL PTI MODE is RAIL_PTI_MODE_DISABLED, then don't initiate sli_cos_pti_write().
  if (SL_RAIL_UTIL_PTI_MODE != RAIL_PTI_MODE_DISABLED) {
    sli_cos_pti_write();
  }
#endif // SL_CATALOG_RAIL_UTIL_PTI_PRESENT

  return;
}

/***************************************************************************//**
 * Pack the BaudRate, flow control value of VCOM into a 32 bit integer
 * @param[in] BaudRate       Baud rate that has to be set over bit_0 to bit_23.
 * @param[in] flow_control   Flow control that has to be set over bit_30 and bit_31.
 * return uint32_t result
 ******************************************************************************/
static uint32_t sli_cos_vcom_config(uint32_t baudrate,
                                    uint8_t flow_control)
{
  uint32_t config = 0;

  // Packing baudrate, and flow control info to config.
  config = config | (((baudrate) & (COS_CONFIG_BAUDRATE_MASK)) << COS_CONFIG_BAUDRATE_POS);
  config = config | (((flow_control) & (COS_CONFIG_UART_FC_MASK)) << COS_CONFIG_UART_FC_POS);

  return config;
}

#if defined (SL_CATALOG_RAIL_UTIL_PTI_PRESENT)
/***************************************************************************//**
 * Pack the BaudRate and mode value of PTI into a 32 bit integer
 * @param[in] BaudRate       Baud rate that has to be set over bit_0 to bit_23.
 * @param[in] mode           RAIL_PtiMode_t to be used.
 *                           RAIL_PTI_MODE_DISABLED;      Turn PTI off entirely.
 *                           RAIL_PTI_MODE_SPI;           SPI mode.
 *                           RAIL_PTI_MODE_UART;          UART mode.
 *                           RAIL_PTI_MODE_UART_ONEWIRE;  9-bit UART mode.
 *                           PTI Modes that has to be set over bit_27 to bit_29.
 * @param[in] interface      Interface value that has to be set over bit_30 and bit_31.
 * return uint32_t config
 ******************************************************************************/
static uint32_t sli_cos_pti_config(uint32_t baudrate,
                                   RAIL_PtiMode_t mode, uint8_t interface)
{
  uint32_t config = 0;
  COS_PtiMode_t mode_Val = COS_CONFIG_PTI_MODE_EFRUART;

  // Mapping received Rail PTI Mode to COS PTI Mode
  if (RAIL_PTI_MODE_SPI == mode) {
    mode_Val = COS_CONFIG_PTI_MODE_EFRSPI;
  }

  // Packing baudrate, mode and interface info to config.
  config = config | (((baudrate) & (COS_CONFIG_BAUDRATE_MASK)) << COS_CONFIG_BAUDRATE_POS);
  config = config | (((mode_Val) & (COS_CONFIG_PTI_MODE_MASK)) << COS_CONFIG_PTI_MODE_POS);
  config = config | (((interface) & (COS_CONFIG_PTI_INTERFACE_MASK)) << COS_CONFIG_PTI_INTERFACE_POS);

  return config;
}
#endif // SL_CATALOG_RAIL_UTIL_PTI_PRESENT

/***************************************************************************//**
 * Write the structured VCOM data on SWO ITM channel 8.
 ******************************************************************************/
#if defined (SLI_COS_UARTDRV_VCOM_PRESENT)
static void sli_cos_vcom_write(void)
{
  COS_ConfigOption_t Cos_Uart_Config;
  uartdrv_handle = sl_uartdrv_get_default();
  uint32_t baudrate = 0;

  Cos_Uart_Config.optionType = COS_CONFIG_OPTION_TYPE_UART;

#ifdef SL_CATALOG_UARTDRV_EUSART_PRESENT
  if (uartdrv_handle->type == (UARTDRV_UartType_t)uartdrvUartTypeEuart) {
    baudrate = EUSART_BaudrateGet(uartdrv_handle->peripheral.euart);
  }
#endif // SL_CATALOG_UARTDRV_EUSART_PRESENT

#ifdef SL_CATALOG_UARTDRV_USART_PRESENT
  if (uartdrv_handle->type == (UARTDRV_UartType_t)uartdrvUartTypeUart) {
    baudrate = USART_BaudrateGet(uartdrv_handle->peripheral.uart);
  }
#endif // SL_CATALOG_UARTDRV_USART_PRESENT

  Cos_Uart_Config.optionValue = sli_cos_vcom_config(baudrate, uartdrv_handle->fcType);

  sli_cos_swo_itm_8_write(&Cos_Uart_Config, sizeof(Cos_Uart_Config));
}
#endif // SLI_COS_UARTDRV_VCOM_PRESENT

/***************************************************************************//**
 * Write the structured PTI data on SWO ITM channel 8.
 ******************************************************************************/
#if defined (SL_CATALOG_RAIL_UTIL_PTI_PRESENT)
static void sli_cos_pti_write(void)
{
  COS_ConfigOption_t Cos_Pti_Config;

  Cos_Pti_Config.optionType = COS_CONFIG_OPTION_TYPE_PTI;
  Cos_Pti_Config.optionValue = sli_cos_pti_config(SL_RAIL_UTIL_PTI_BAUD_RATE_HZ, SL_RAIL_UTIL_PTI_MODE, COS_CONFIG_PTI_INTERFACE_0);

  sli_cos_swo_itm_8_write(&Cos_Pti_Config, sizeof(Cos_Pti_Config));
}
#endif // SL_CATALOG_RAIL_UTIL_PTI_PRESENT

/***************************************************************************//**
 * Custom API which can be used by other software component, to write the
 * structured PTI data on SWO ITM channel 8.
 ******************************************************************************/
void sl_cos_config_pti(uint32_t baudrate,
                       COS_PtiMode_t mode,
                       COS_PtiInterface_t interface)
{
  COS_ConfigOption_t Cos_Pti_Config;
  uint32_t config = 0;

  Cos_Pti_Config.optionType = COS_CONFIG_OPTION_TYPE_PTI;

  // Packing baudrate, mode and interface info to config
  config = config | (((baudrate) & (COS_CONFIG_BAUDRATE_MASK)) << COS_CONFIG_BAUDRATE_POS);
  config = config | (((mode) & (COS_CONFIG_PTI_MODE_MASK)) << COS_CONFIG_PTI_MODE_POS);
  config = config | (((interface) & (COS_CONFIG_PTI_INTERFACE_MASK)) << COS_CONFIG_PTI_INTERFACE_POS);

  Cos_Pti_Config.optionValue = config;

  sli_cos_swo_itm_8_write(&Cos_Pti_Config, sizeof(Cos_Pti_Config));
}

/***************************************************************************//**
 * Custom API which can be used by other software component, to write the
 * structured VCOM data on SWO ITM channel 8.
 ******************************************************************************/
void sl_cos_config_vcom(uint32_t baudrate,
                        uint8_t flow_control)
{
  COS_ConfigOption_t Cos_Uart_Config;

  Cos_Uart_Config.optionType = COS_CONFIG_OPTION_TYPE_UART;
  Cos_Uart_Config.optionValue = sli_cos_vcom_config(baudrate, flow_control);

  sli_cos_swo_itm_8_write(&Cos_Uart_Config, sizeof(Cos_Uart_Config));
}

/***************************************************************************//**
 * Write data on SWO interface (WSTK-port 4900)
 ******************************************************************************/
static sl_status_t sli_cos_swo_itm_8_write(const void *buffer,
                                           size_t buffer_length)
{
  uint8_t *buf = (uint8_t *)buffer;
  uint32_t packet_length;
  uint32_t i;
  uint8_t  output_byte;
  uint8_t  seq_nbr = 0;

  // Full length is 2 square braces, 1 byte length and 2 byte CRC
  packet_length = ( (uint32_t)buffer_length) + 9;

  // The write feature is built upon the existing Ember Debug Message (EDM) protocol, which today is transmitted over SWO UART on ITM Channel 8.
  // The Protocol have the Start-byte, Length, Version, Message type, Sequence number, Message, CRC CCITT-16, and End-byte for correct communication.
  for ( i = 0; i < packet_length; ++i ) {
    if ( i == 0 ) {
      // Frame start
      output_byte = '[';
    } else if ( i == 1 ) {
      // Including special byte, type and sequence number
      output_byte = buffer_length + 4;
    } else if ( i == 2 ) {
      // Special EDM byte
      output_byte = 0xD1;
    } else if ( i == 3 ) {
      // COS Type byte 1
      output_byte = LOW_BYTE((uint16_t)(EM_COS_PACKET));
    } else if ( i == 4 ) {
      // COS Type byte 2
      output_byte = HIGH_BYTE((uint16_t)(EM_COS_PACKET));
    } else if ( i == 5 ) {
      // Sequence number
      output_byte = seq_nbr++;
    } else if ( i == (packet_length - 3) ) {
      // CRC first byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if ( i == (packet_length - 2) ) {
      // CRC second byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if ( i == (packet_length - 1) ) {
      // Frame end
      output_byte = ']';
    } else {
      // Data
      output_byte = buf[i - 6];
    }

    sl_debug_swo_write(8U, output_byte);
  }

  return SL_STATUS_OK;
}
