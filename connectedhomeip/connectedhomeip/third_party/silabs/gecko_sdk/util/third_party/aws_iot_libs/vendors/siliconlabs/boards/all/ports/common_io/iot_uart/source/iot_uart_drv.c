/***************************************************************************//**
 * @file    iot_uart_drv.c
 * @brief   UART driver.
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

/* SDK service layer */
#include "sl_power_manager.h"

/* UART driver layer */
#include "iot_uart_desc.h"
#include "iot_uart_cb.h"
#include "iot_uart_drv.h"

/*******************************************************************************
 *                      iot_uart_drv_driver_init
 ******************************************************************************/

sl_status_t iot_uart_drv_driver_init(void *pvHndl)
{
  IotUARTDescriptor_t     *pvDesc = pvHndl;
  USART_InitAsync_TypeDef  xInit  = USART_INITASYNC_DEFAULT;

  /* set default baudrate */
  xInit.baudrate = pvDesc->ulDefaultBaudrate;

  /* set default data bits */
  xInit.databits = pvDesc->xDefaultDataBits;

  /* set default parity */
  xInit.parity = pvDesc->xDefaultParity;

  /* set default stop bits */
  xInit.stopbits = pvDesc->xDefaultStopBits;

  /* set default flow control type */
#if (_SILICON_LABS_32B_SERIES != 0)
  xInit.hwFlowControl = pvDesc->xDefaultFlowCtrl;
#endif

  /* re-initialize descriptor's init struct */
  pvDesc->xInit = xInit;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_uart_drv_driver_deinit
 ******************************************************************************/

sl_status_t iot_uart_drv_driver_deinit(void *pvHndl)
{
  IotUARTDescriptor_t     *pvDesc = pvHndl;
  USART_InitAsync_TypeDef  xInit  = USART_INITASYNC_DEFAULT;

  /* re-initialize descriptor's init struct */
  pvDesc->xInit = xInit;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_uart_drv_config_set
 ******************************************************************************/

sl_status_t iot_uart_drv_config_set(void *pvHndl,
                                       uint32_t ulBaudrate,
                                       uint8_t ucParity,
                                       uint8_t ucStopbits,
                                       uint8_t ucWordlength,
                                       uint8_t ucFlowControl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* process xUartinit baudrate */
  pvDesc->xInit.baudrate = ulBaudrate;

  /* process xUartinit parity */
  switch (ucParity) {
    case 0:
      pvDesc->xInit.parity = usartNoParity;
      break;
    case 1:
      pvDesc->xInit.parity = usartEvenParity;
      break;
    case 2:
      pvDesc->xInit.parity = usartOddParity;
      break;
    default:
      pvDesc->xInit.parity = usartNoParity;
      break;
  }

  /* process xUartinit stop bits */
  switch (ucStopbits) {
    case 0:
      pvDesc->xInit.stopbits = usartStopbits1;
      break;
    case 1:
      pvDesc->xInit.stopbits = usartStopbits2;
      break;
    default:
      pvDesc->xInit.stopbits = usartStopbits1;
      break;
  }

  /* process xUartinit word length */
  switch (ucWordlength) {
    case 1:
      pvDesc->xInit.databits = usartDatabits4;
      break;
    case 2:
      pvDesc->xInit.databits = usartDatabits4;
      break;
    case 3:
      pvDesc->xInit.databits = usartDatabits4;
      break;
    case 4:
      pvDesc->xInit.databits = usartDatabits4;
      break;
    case 5:
      pvDesc->xInit.databits = usartDatabits5;
      break;
    case 6:
      pvDesc->xInit.databits = usartDatabits6;
      break;
    case 7:
      pvDesc->xInit.databits = usartDatabits7;
      break;
    case 8:
      pvDesc->xInit.databits = usartDatabits8;
      break;
    default:
      pvDesc->xInit.databits = usartDatabits8;
      break;
  }

  /* process xUartinit flow control */
#if (_SILICON_LABS_32B_SERIES != 0)
  switch(ucFlowControl) {
    case 0:
      pvDesc->xInit.hwFlowControl = usartHwFlowControlNone;
      break;
    case 1:
      pvDesc->xInit.hwFlowControl = usartHwFlowControlCtsAndRts;
      break;
    default:
      pvDesc->xInit.hwFlowControl = usartHwFlowControlNone;
      break;
  }
#endif

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_uart_drv_config_get
 ******************************************************************************/

sl_status_t iot_uart_drv_config_get(void *pvHndl,
                                       uint32_t *pulBaudrate,
                                       uint8_t *pucParity,
                                       uint8_t *pucStopbits,
                                       uint8_t *pucWordlength,
                                       uint8_t *pucFlowControl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* process xUartinit baudrate */
  *pulBaudrate = pvDesc->xInit.baudrate;

  /* process xUartinit parity */
  switch (pvDesc->xInit.parity) {
    case usartNoParity:
      *pucParity = 0;
      break;
    case usartEvenParity:
      *pucParity = 1;
      break;
    case usartOddParity:
      *pucParity = 2;
      break;
    default:
      *pucParity = 0;
      break;
  }

  /* process xUartinit stop bits */
  switch (pvDesc->xInit.stopbits) {
    case usartStopbits1:
      *pucStopbits = 0;
      break;
    case usartStopbits2:
      *pucStopbits = 1;
      break;
    default:
      *pucStopbits = 0;
      break;
  }

  /* process xUartinit word length */
  switch (pvDesc->xInit.databits) {
    case usartDatabits4:
      *pucWordlength = 4;
      break;
    case usartDatabits5:
      *pucWordlength = 5;
      break;
    case usartDatabits6:
      *pucWordlength = 6;
      break;
    case usartDatabits7:
      *pucWordlength = 7;
      break;
    case usartDatabits8:
      *pucWordlength = 8;
      break;
    default:
      *pucWordlength = 8;
      break;
  }

  /* process xUartinit flow control */
#if (_SILICON_LABS_32B_SERIES != 0)
  switch(pvDesc->xInit.hwFlowControl) {
    case usartHwFlowControlNone:
      *pucFlowControl = 0;
      break;
    case usartHwFlowControlCtsAndRts:
      *pucFlowControl = 1;
      break;
    default:
      *pucFlowControl = 0;
      break;
  }
#else
  *pucFlowControl = 0;
#endif

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_uart_drv_hw_enable
 ******************************************************************************/

sl_status_t iot_uart_drv_hw_enable(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* make sure GPIO clock is enabled */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* enable oscillator to GPIO and USART modules */
  CMU_ClockEnable(pvDesc->xClock, true);

  /* initialize USART hardware */
  USART_InitAsync(pvDesc->pxPeripheral, &pvDesc->xInit);

  /* enable loopback if requested */
  if (pvDesc->ucLoopbackEn == 1) {
    pvDesc->pxPeripheral->CTRL_SET = USART_CTRL_LOOPBK;
  }

  /* set pin modes for USART pins */
  if (pvDesc->ucTxEn == 1) {
    GPIO_PinModeSet(pvDesc->xTxPort,  pvDesc->ucTxPin,  gpioModePushPull, 1);
  }
  if (pvDesc->ucRxEn == 1) {
    GPIO_PinModeSet(pvDesc->xRxPort,  pvDesc->ucRxPin,  gpioModeInput,    0);
  }
  if (pvDesc->ucClkEn == 1) {
    GPIO_PinModeSet(pvDesc->xClkPort, pvDesc->ucClkPin, gpioModePushPull, 1);
  }
  if (pvDesc->ucCsEn == 1) {
    GPIO_PinModeSet(pvDesc->xCsPort,  pvDesc->ucCsPin,  gpioModePushPull, 1);
  }
  if (pvDesc->ucRtsEn == 1) {
    GPIO_PinModeSet(pvDesc->xRtsPort, pvDesc->ucRtsPin, gpioModePushPull, 1);
  }
  if (pvDesc->ucCtsEn == 1) {
    GPIO_PinModeSet(pvDesc->xCtsPort, pvDesc->ucCtsPin, gpioModeInput,   0);
  }

#if (_SILICON_LABS_32B_SERIES == 0)
  /* setup routing (series 0): locations */
  pvDesc->pxPeripheral->ROUTE =
    (pvDesc->ucTxLoc  << _USART_ROUTE_LOCATION_SHIFT) |
    (pvDesc->ucRxLoc  << _USART_ROUTE_LOCATION_SHIFT) |
    (pvDesc->ucClkLoc << _USART_ROUTE_LOCATION_SHIFT) |
    (pvDesc->ucCsLoc  << _USART_ROUTE_LOCATION_SHIFT) |
    (pvDesc->ucRtsLoc << _USART_ROUTE_LOCATION_SHIFT) |
    (pvDesc->ucCtsLoc << _USART_ROUTE_LOCATION_SHIFT) ;

  /* setup routing (series 0): enable */
  pvDesc->pxPeripheral->ROUTE  |=
    (pvDesc->ucTxEn  ? USART_ROUTE_TXPEN  : 0) |
    (pvDesc->ucRxEn  ? USART_ROUTE_RXPEN  : 0) |
    (pvDesc->ucClkEn ? USART_ROUTE_CLKPEN : 0) |
    (pvDesc->ucCsEn  ? USART_ROUTE_CSPEN  : 0) |
    (pvDesc->ucRtsEn ? USART_ROUTE_RTSPEN : 0) |
    (pvDesc->ucCtsEn ? USART_ROUTE_CTSPEN : 0) ;
#endif

#if (_SILICON_LABS_32B_SERIES == 1)
  /* setup routing (series 1): locations */
  pvDesc->pxPeripheral->ROUTELOC0 =
    (pvDesc->ucTxLoc  << _USART_ROUTELOC0_TXLOC_SHIFT ) |
    (pvDesc->ucRxLoc  << _USART_ROUTELOC0_RXLOC_SHIFT ) |
    (pvDesc->ucClkLoc << _USART_ROUTELOC0_CLKLOC_SHIFT) |
    (pvDesc->ucCsLoc  << _USART_ROUTELOC0_CSLOC_SHIFT ) |
    (pvDesc->ucRtsLoc << _USART_ROUTELOC0_RTSLOC_SHIFT) |
    (pvDesc->ucCtsLoc << _USART_ROUTELOC0_CTSLOC_SHIFT) ;

  /* setup routing (series 1): enable */
  pvDesc->pxPeripheral->ROUTEPEN =
    (pvDesc->ucTxEn  ? USART_ROUTEPEN_TXPEN  : 0) |
    (pvDesc->ucRxEn  ? USART_ROUTEPEN_RXPEN  : 0) |
    (pvDesc->ucClkEn ? USART_ROUTEPEN_CLKPEN : 0) |
    (pvDesc->ucCsEn  ? USART_ROUTEPEN_CSPEN  : 0) |
    (pvDesc->ucRtsEn ? USART_ROUTEPEN_RTSPEN : 0) |
    (pvDesc->ucCtsEn ? USART_ROUTEPEN_CTSPEN : 0) ;
#endif

#if (_SILICON_LABS_32B_SERIES == 2)
  /* setup routing (series 2): TX */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].TXROUTE =
    (pvDesc->xTxPort  << _GPIO_USART_TXROUTE_PORT_SHIFT ) |
    (pvDesc->ucTxPin  << _GPIO_USART_TXROUTE_PIN_SHIFT  ) ;

  /* setup routing (series 2): RX */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].RXROUTE =
    (pvDesc->xRxPort  << _GPIO_USART_RXROUTE_PORT_SHIFT ) |
    (pvDesc->ucRxPin  << _GPIO_USART_RXROUTE_PIN_SHIFT  ) ;


  /* setup routing (series 2): CLK */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CLKROUTE =
    (pvDesc->xClkPort << _GPIO_USART_CLKROUTE_PORT_SHIFT) |
    (pvDesc->ucClkPin << _GPIO_USART_CLKROUTE_PIN_SHIFT ) ;

  /* setup routing (series 2): CS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CSROUTE =
    (pvDesc->xCsPort  << _GPIO_USART_CSROUTE_PORT_SHIFT ) |
    (pvDesc->ucCsPin  << _GPIO_USART_CSROUTE_PIN_SHIFT  ) ;

  /* setup routing (series 2): RTS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].RTSROUTE =
    (pvDesc->xRtsPort << _GPIO_USART_RTSROUTE_PORT_SHIFT) |
    (pvDesc->ucRtsPin << _GPIO_USART_RTSROUTE_PIN_SHIFT ) ;

  /* setup routing (series 2): CTS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CTSROUTE =
    (pvDesc->xCtsPort << _GPIO_USART_CTSROUTE_PORT_SHIFT) |
    (pvDesc->ucCtsPin << _GPIO_USART_CTSROUTE_PIN_SHIFT ) ;

  /* setup routing (series 2): enable */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].ROUTEEN =
    (pvDesc->ucTxEn  ? GPIO_USART_ROUTEEN_TXPEN  : 0) |
    (pvDesc->ucRxEn  ? GPIO_USART_ROUTEEN_RXPEN  : 0) |
    (pvDesc->ucClkEn ? GPIO_USART_ROUTEEN_CLKPEN : 0) |
    (pvDesc->ucCsEn  ? GPIO_USART_ROUTEEN_CSPEN  : 0) |
    (pvDesc->ucRtsEn ? GPIO_USART_ROUTEEN_RTSPEN : 0) ;
#endif

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_uart_drv_hw_disable
 ******************************************************************************/

sl_status_t iot_uart_drv_hw_disable(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* disable loopback if it was enabled */
  if (pvDesc->ucLoopbackEn == 1) {
    pvDesc->pxPeripheral->CTRL_CLR = USART_CTRL_LOOPBK;
  }

  /* reset USART hardware */
  USART_Reset(pvDesc->pxPeripheral);

  /* disable USART clock */
  CMU_ClockEnable(pvDesc->xClock, false);

  /* unset pin modes for USART pins */
  if (pvDesc->ucTxEn == 1) {
    GPIO_PinModeSet(pvDesc->xRxPort,  pvDesc->ucRxPin,  gpioModeDisabled, 0);
  }
  if (pvDesc->ucRxEn == 1) {
    GPIO_PinModeSet(pvDesc->xTxPort,  pvDesc->ucTxPin,  gpioModeDisabled, 0);
  }
  if (pvDesc->ucClkEn == 1) {
    GPIO_PinModeSet(pvDesc->xClkPort, pvDesc->ucClkPin, gpioModeDisabled, 0);
  }
  if (pvDesc->ucCsEn == 1) {
    GPIO_PinModeSet(pvDesc->xCsPort,  pvDesc->ucCsPin,  gpioModeDisabled, 0);
  }
  if (pvDesc->ucRtsEn == 1) {
    GPIO_PinModeSet(pvDesc->xRtsPort, pvDesc->ucRtsPin, gpioModeDisabled, 0);
  }
  if (pvDesc->ucCtsEn == 1) {
    GPIO_PinModeSet(pvDesc->xCtsPort, pvDesc->ucCtsPin, gpioModeDisabled, 0);
  }

#if (_SILICON_LABS_32B_SERIES == 0)
  /* setup routing (series 0): locations & enable */
  pvDesc->pxPeripheral->ROUTE = 0;
#endif

#if (_SILICON_LABS_32B_SERIES == 1)
  /* setup routing (series 1): locations */
  pvDesc->pxPeripheral->ROUTELOC0 = 0;

  /* setup routing (series 1): enable */
  pvDesc->pxPeripheral->ROUTEPEN = 0;
#endif

#if (_SILICON_LABS_32B_SERIES == 2)
  /* setup routing (series 2): TX */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].TXROUTE  = 0;

  /* setup routing (series 2): RX */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].RXROUTE  = 0;

  /* setup routing (series 2): CLK */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CLKROUTE = 0;

  /* setup routing (series 2): CS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CSROUTE  = 0;

  /* setup routing (series 2): RTS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].RTSROUTE = 0;

  /* setup routing (series 2): CTS */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].CTSROUTE = 0;

  /* setup routing (series 2): disable */
  GPIO->USARTROUTE[pvDesc->ucPeripheralNo].ROUTEEN  = 0;
#endif

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_uart_drv_transfer_tx
 ******************************************************************************/

sl_status_t iot_uart_drv_transfer_tx(void *pvHndl, uint8_t *pucData)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* send byte over TX */
  USART_Tx(pvDesc->pxPeripheral, *pucData);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_uart_drv_transfer_rx
 ******************************************************************************/

sl_status_t iot_uart_drv_transfer_rx(void *pvHndl, uint8_t *pucData)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* receive byte over RX */
  *pucData = USART_Rx(pvDesc->pxPeripheral);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_uart_drv_start_tx
 ******************************************************************************/

sl_status_t iot_uart_drv_start_tx(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* prevent power manager from going into deep sleep during transfer */
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  /* enable IRQ line on NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xTxIRQn);
  NVIC_EnableIRQ(pvDesc->xTxIRQn);

  /* enable USART hardware to trigger TX interrupts */
  USART_IntEnable(pvDesc->pxPeripheral, USART_IEN_TXBL);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_uart_drv_start_rx
 ******************************************************************************/

sl_status_t iot_uart_drv_start_rx(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* prevent power manager from going into deep sleep during transfer */
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  /* enable IRQ line on NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xRxIRQn);
  NVIC_EnableIRQ(pvDesc->xRxIRQn);

  /* enable USART hardware to trigger RX interrupts */
  USART_IntEnable(pvDesc->pxPeripheral, USART_IEN_RXDATAV);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_uart_drv_stop_tx
 ******************************************************************************/

sl_status_t iot_uart_drv_stop_tx(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* stop USART hardware from triggering TX interrupts */
  USART_IntDisable(pvDesc->pxPeripheral, USART_IEN_TXBL);

  /* disable IRQ line on NVIC */
  NVIC_DisableIRQ(pvDesc->xTxIRQn);
  NVIC_ClearPendingIRQ(pvDesc->xTxIRQn);

  /* re-enable deep sleep in power manager */
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_uart_drv_stop_rx
 ******************************************************************************/

sl_status_t iot_uart_drv_stop_rx(void *pvHndl)
{
  IotUARTDescriptor_t *pvDesc = pvHndl;

  /* stop USART hardware from triggering RX interrupts */
  USART_IntDisable(pvDesc->pxPeripheral, USART_IEN_RXDATAV);

  /* disable IRQ line on NVIC */
  NVIC_DisableIRQ(pvDesc->xRxIRQn);
  NVIC_ClearPendingIRQ(pvDesc->xRxIRQn);

  /* re-enable deep sleep in power manager */
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
                          USART0 TX IRQ HANDLER
 ******************************************************************************/
#ifdef USART0
void USART0_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART0);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART0, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART0 RX IRQ HANDLER
 ******************************************************************************/
#ifdef USART0
void USART0_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART0);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART0, USART_IF_RXDATAV);
}
#endif

/*******************************************************************************
                          USART1 TX IRQ HANDLER
 ******************************************************************************/

#ifdef USART1
void USART1_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART1);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART1, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART1 RX IRQ HANDLER
 ******************************************************************************/

#ifdef USART1
void USART1_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART1);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART1, USART_IF_RXDATAV);
}
#endif

/*******************************************************************************
                          USART2 TX IRQ HANDLER
 ******************************************************************************/

#ifdef USART2
void USART2_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART2);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART2, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART2 RX IRQ HANDLER
 ******************************************************************************/

#ifdef USART2
void USART2_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART2);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART2, USART_IF_RXDATAV);
}
#endif

/*******************************************************************************
                          USART3 TX IRQ HANDLER
 ******************************************************************************/

#ifdef USART3
void USART3_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART3);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART3, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART3 RX IRQ HANDLER
 ******************************************************************************/

#ifdef USART3
void USART3_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART3);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART3, USART_IF_RXDATAV);
}
#endif

/*******************************************************************************
                          USART4 TX IRQ HANDLER
 ******************************************************************************/

#ifdef USART4
void USART4_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART4);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART4, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART4 RX IRQ HANDLER
 ******************************************************************************/

#ifdef USART4
void USART4_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART4);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART4, USART_IF_RXDATAV);
}
#endif

/*******************************************************************************
                          USART5 TX IRQ HANDLER
 ******************************************************************************/

#ifdef USART5
void USART5_TX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART5);

  /* call consolidated IRQ handler */
  IOT_UART_CB_TX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART5, USART_IF_TXBL);
}
#endif

/*******************************************************************************
                          USART5 RX IRQ HANDLER
 ******************************************************************************/

#ifdef USART5
void USART5_RX_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_uart_desc_get(++lInstNum)->pxPeripheral != USART5);

  /* call consolidated IRQ handler */
  IOT_UART_CB_RX(lInstNum);

  /* clear the requesting interrupt */
  USART_IntClear(USART5, USART_IF_RXDATAV);
}
#endif
