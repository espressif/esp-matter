/***************************************************************************//**
 * @file
 * @brief Board Controller Communications (BCC) definitions
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

#include <string.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "bsp.h"

#if defined(BSP_BCC_LEUART)
#include "em_leuart.h"
#else
#include "em_usart.h"
#endif

#if defined(BSP_STK)

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Module local variables */
static uint32_t rxByteCount;
static uint32_t txByteCount;

/* Module local prototypes */
static void    TxByte(uint8_t data);
static uint8_t RxByte(void);

/** @endcond */

/***************************************************************************//**
 * @addtogroup BSP
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup BSP_STK API for STKs and WSTKs
 * @{
 ******************************************************************************/

/**************************************************************************//**
 * @brief Deinitialize board controller communication support (BCC)
 *        functionality. Reverse actions performed by @ref BSP_BccInit().
 *
 * @return @ref BSP_STATUS_OK.
 *****************************************************************************/
int BSP_BccDeInit(void)
{
  /* Reset counters */
  rxByteCount = 0xFFFFFFFFUL;
  txByteCount = 0xFFFFFFFFUL;

  BSP_BccPinsEnable(false);

#if defined(BSP_BCC_LEUART)
  /* Reset LEUART */
  LEUART_Reset(BSP_BCC_LEUART);
#else
  /* Reset USART */
  USART_Reset(BSP_BCC_USART);
#endif

  /* Disable clock */
  CMU_ClockEnable(BSP_BCC_CLK, false);

  return BSP_STATUS_OK;
}

/**************************************************************************//**
 * @brief Initialize board controller communication support (BCC)
 *        functionality.
 *
 * @return @ref BSP_STATUS_OK.
 *****************************************************************************/
int BSP_BccInit(void)
{
#if defined(BSP_BCC_LEUART)
  LEUART_Init_TypeDef leuartInit = LEUART_INIT_DEFAULT;
#else
  USART_InitAsync_TypeDef usartInit = USART_INITASYNC_DEFAULT;
#endif

  rxByteCount = 0;
  txByteCount = 0;

  /* Enable High Frequency Peripherals */
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif

  /* Enable clocks to GPIO */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable UART clock */
  CMU_ClockEnable(BSP_BCC_CLK, true);

#if defined(BSP_BCC_LEUART)
  /* Enable CORE LE clock in order to access LE modules */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Select CORE LE clock for LE modules */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);

  /* Initialize LEUART */
  leuartInit.baudrate = 115200;
  LEUART_Init(BSP_BCC_LEUART, &leuartInit);
#else
  /* Initialize USART */
  USART_InitAsync(BSP_BCC_USART, &usartInit);
#endif

  /* Initialize UART pins */
  BSP_BccPinsEnable(true);

  return BSP_STATUS_OK;
}

/**************************************************************************//**
 * @brief Get a packet from the board controller.
 *
 * @param[in] pkt Pointer to a @ref BCP_Packet instance.
 *
 * @return True if packet received without errors, false otherwise.
 *****************************************************************************/
bool BSP_BccPacketReceive(BCP_Packet *pkt)
{
  int i;
  int length;
  uint8_t *bptr;

  /* Setup a byte pointer to start of packet buffer */
  bptr   = (uint8_t *) pkt;

  /* Receive packet magic */
  *bptr++ = RxByte();
  if (pkt->magic != BSP_BCP_MAGIC) {
    return false;
  }

  /* Receive packet type */
  *bptr++ = RxByte();
  if ( (pkt->type < BSP_BCP_FIRST) || (pkt->type > BSP_BCP_LAST) ) {
    return false;
  }

  /* Receive packet length */
  *bptr++ = RxByte();
  if (pkt->payloadLength > BSP_BCP_PACKET_SIZE) {
    return false;
  }

#if (BSP_BCP_VERSION == 2)
  /* Receive reserved byte */
  *bptr++ = RxByte();
#endif

  /* Receive packet data length field and sanity check it */
  length  = pkt->payloadLength;
  if (length > BSP_BCP_PACKET_SIZE) {
    length = BSP_BCP_PACKET_SIZE;
  }

  /* Receive packet payload */
  for ( i = 0; i < length; i++ ) {
    *bptr++ = RxByte();
  }

  return true;
}

/**************************************************************************//**
 * @brief Send a packet to the board controller.
 *
 * @param[in] pkt Pointer to a @ref BCP_Packet instance.
 *
 * @return @ref BSP_STATUS_OK.
 *****************************************************************************/
int BSP_BccPacketSend(BCP_Packet *pkt)
{
  int i;

  /* Apply magic */
  pkt->magic = BSP_BCP_MAGIC;

  /* Transmit packet magic */
  TxByte(pkt->magic);

  /* Transmit packet type */
  TxByte(pkt->type);

  /* Transmit packet length */
  TxByte(pkt->payloadLength);

#if (BSP_BCP_VERSION == 2)
  /* Transmit reserved byte */
  TxByte(pkt->reserved);
#endif

  /* Transmit packet payload */
  for ( i = 0; i < pkt->payloadLength; i++ ) {
    TxByte(pkt->data[i]);
  }

  return BSP_STATUS_OK;
}

/**************************************************************************//**
 * @brief Enable GPIO pins for the USART/LEUART used for board communication.
 *
 * @param[in] enable Set to true to enable pins, set to false to disable.
 *****************************************************************************/
void BSP_BccPinsEnable(bool enable)
{
  if (enable) {
    /* Configure GPIO pin for UART TX */
    /* To avoid false start, configure output as high. */
    GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1);

    /* Configure GPIO pin for UART RX */
    GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 1);

#if defined(BSP_BCC_ENABLE_PORT)
    /* Enable the switch that enables UART communication. */
    GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1);
#endif

#if defined(BSP_BCC_LEUART)
    BSP_BCC_LEUART->ROUTE |= LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | BSP_BCC_LOCATION;
#else
    /* Enable pins at correct UART/USART location. */
#if defined(GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE[BSP_BCC_USART_INDEX].ROUTEEN =
      GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN;
    GPIO->USARTROUTE[BSP_BCC_USART_INDEX].TXROUTE =
      (BSP_BCC_TXPORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (BSP_BCC_TXPIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[BSP_BCC_USART_INDEX].RXROUTE =
      (BSP_BCC_RXPORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (BSP_BCC_RXPIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
#elif defined(USART_ROUTEPEN_TXPEN)
    BSP_BCC_USART->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;
    BSP_BCC_USART->ROUTELOC0 =
      (BSP_BCC_USART->ROUTELOC0
       & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK) )
      | (BSP_BCC_TX_LOCATION)
      | (BSP_BCC_RX_LOCATION);
#elif defined(USART_ROUTE_TXPEN)
    BSP_BCC_USART->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | BSP_BCC_LOCATION;
#endif
#endif
  } else {
#if defined(BSP_BCC_ENABLE_PORT)
    GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModeDisabled, 0);
#endif
#if defined(BSP_BCC_LEUART)
    BSP_BCC_LEUART->ROUTE &= ~(LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN);
#else
    /* Disable UART/USART pins. */
#if defined(GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE[BSP_BCC_USART_INDEX].ROUTEEN &=
      ~(GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN);
#elif defined(USART_ROUTEPEN_TXPEN)
    BSP_BCC_USART->ROUTEPEN &= ~(USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN);
#elif defined(USART_ROUTE_TXPEN)
    BSP_BCC_USART->ROUTE &= ~(USART_ROUTE_RXPEN | USART_ROUTE_TXPEN);
#endif
#endif

    GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeDisabled, 0);
  }
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static uint8_t RxByte(void)
{
  uint8_t byte;

  /* Poll RX data available flag and return a character when one is available */

#if defined(BSP_BCC_LEUART)
  while (!(BSP_BCC_LEUART->IF & LEUART_IF_RXDATAV)) ;
  byte = BSP_BCC_LEUART->RXDATA;
#else
  while (!(BSP_BCC_USART->STATUS & USART_STATUS_RXDATAV)) ;
  byte = BSP_BCC_USART->RXDATA;
#endif

  rxByteCount++;
  return byte;
}

static void TxByte(uint8_t data)
{
  /* Check TX buffer and allow for a pending transfer to complete */

#if defined(BSP_BCC_LEUART)
  while (!(BSP_BCC_LEUART->STATUS & LEUART_STATUS_TXBL)) ;
  BSP_BCC_LEUART->TXDATA = (uint32_t) data;
#else
  while (!(BSP_BCC_USART->STATUS & USART_STATUS_TXBL)) ;
  BSP_BCC_USART->TXDATA = (uint32_t) data;
#endif

  txByteCount++;
}

/** @endcond */

/** @} (end group BSP_STK) */
/** @} (end group BSP) */

#endif /* BSP_STK */
