#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <em_device.h>
#include <em_gpio.h>
#include <em_core.h>
#include "sl_component_catalog.h"
#include "sl_uartdrv_instances.h"
#include "sl_uartdrv_usart_vcom_config.h"
#include "sl_btctrl_linklayer.h"
#include "sl_hci_common_transport.h"
#include "sl_hci_fifo.h"
#include "sl_hci_uart.h"

#define RX_BUFFER_LEN 64
#define uart_rx_buffer_size         64 // size needs to be even number

static bool sleep_disabled = false;
static UARTDRV_Handle_t handle = NULL;
static fifo_define(uart_rx_buffer, uart_rx_buffer_size);

static struct {
  struct sl_hci_fifo *rx_buffer;
} uart;

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
// Power manager transition events of interest.
#define POWER_MANAGER_EVENTS_OF_INTEREST            \
  (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1   \
   | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
   | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
   | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2)

static void energy_mode_transition_cb(sl_power_manager_em_t from,
                                      sl_power_manager_em_t to);

static sl_power_manager_em_transition_event_handle_t pm_handle;
static sl_power_manager_em_transition_event_info_t pm_event_info =
{ POWER_MANAGER_EVENTS_OF_INTEREST, energy_mode_transition_cb };
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

static void start_rx(UARTDRV_Handle_t handle);
static void tx_callback(UARTDRV_Handle_t handle, Ecode_t status, uint8_t *buffer, UARTDRV_Count_t count);
static void enable_usart_irq(void);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void energy_mode_transition_cb(sl_power_manager_em_t from,
                                      sl_power_manager_em_t to)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  if (from == SL_POWER_MANAGER_EM0 && to == SL_POWER_MANAGER_EM1) {
    if (sl_hci_uart_rx_buffered_length() > 0 || sleep_disabled) {
      /* Wake the device up immediately by setting the RXFULL interrupt.
         Setting the RXDATAV interrupt does not wake the device up as
         expected, hence why RXFULL is used here. */
      USART_IntEnable(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXFULL);
      USART_IntSet(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXFULL);
    } else {
      USART_IntClear(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXDATAV);
      USART_IntEnable(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXDATAV);
    }
  }
  if (from <= SL_POWER_MANAGER_EM1 && to == SL_POWER_MANAGER_EM2) {
    USART_Enable(SL_UARTDRV_USART_VCOM_PERIPHERAL, usartDisable);
  }
  if (from == SL_POWER_MANAGER_EM1 && to == SL_POWER_MANAGER_EM0) {
    USART_IntDisable(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXFULL | USART_IF_RXDATAV);
  }
  if (from == SL_POWER_MANAGER_EM2) {
    USART_Enable(SL_UARTDRV_USART_VCOM_PERIPHERAL, usartEnable);
  }

  CORE_EXIT_ATOMIC();
}
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

void sl_hci_uart_init(void)
{
  sl_hci_fifo_init(&uart_rx_buffer, 0);
  uart.rx_buffer = &uart_rx_buffer;
  handle = sl_uartdrv_get_default();
  enable_usart_irq();
  start_rx(handle);
  USART_Enable(SL_UARTDRV_USART_VCOM_PERIPHERAL, usartEnable);
  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_subscribe_em_transition_event(&pm_handle, &pm_event_info);
  #endif // SL_CATALOG_POWER_MANAGER_PRESENT
}

static void enable_usart_irq(void)
{
#if SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 0
  NVIC_EnableIRQ(USART0_RX_IRQn);
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 1
  NVIC_EnableIRQ(USART1_RX_IRQn);
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 2
  NVIC_EnableIRQ(USART2_RX_IRQn);
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 3
  NVIC_EnableIRQ(USART3_RX_IRQn);
#else
#error Unsupported VCOM device
#endif
}

#if SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 0
void USART0_RX_IRQHandler()
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 1
void USART1_RX_IRQHandler()
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 2
void USART2_RX_IRQHandler()
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 3
void USART3_RX_IRQHandler()
#else
#error Unsupported VCOM device
#endif
{
  int irq = USART_IntGet(SL_UARTDRV_USART_VCOM_PERIPHERAL);
  if (irq & USART_IF_RXDATAV) {
    USART_IntClear(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXDATAV);
  } else if (irq & USART_IF_RXFULL) {
    USART_IntClear(SL_UARTDRV_USART_VCOM_PERIPHERAL, USART_IF_RXFULL);
  }
}

bool rx_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  (void)channel;
  (void)sequenceNo;
  (void)userParam;
  uint8_t *dma_tail;
  int size = sl_hci_fifo_size(uart.rx_buffer) / 2;
  // reserve next half of fifo for dma
  sl_hci_fifo_dma_reserve(uart.rx_buffer, &dma_tail, size);
  // dma transfer is ready, update the tail of fifo
  sl_hci_fifo_dma_set_tail(uart.rx_buffer, dma_tail);
  return true;
}

static void start_rx(UARTDRV_Handle_t handle)
{
  int size = sl_hci_fifo_size(uart.rx_buffer) / 2;
  // reserve first half of fifo for dma
  sl_hci_fifo_dma_reserve(uart.rx_buffer, NULL, size);
  uint8_t *dst0 = sl_hci_fifo_buffer(uart.rx_buffer);
  uint8_t *dst1 = sl_hci_fifo_buffer(uart.rx_buffer) + size;
  uint8_t *src = (uint8_t *)&handle->peripheral.uart->RXDATA;
  DMADRV_PeripheralMemoryPingPong(handle->rxDmaCh, handle->rxDmaSignal,
                                  dst0, dst1, src, true, size, dmadrvDataSize1, rx_callback, NULL);
}

static void update_buffer_status(UARTDRV_Handle_t handle)
{
  int remaining = 0;
  uint8_t *dma_tail;
  DMADRV_TransferRemainingCount(handle->rxDmaCh, &remaining);
  sl_hci_fifo_dma_reserve(uart.rx_buffer, &dma_tail, 0);
  sl_hci_fifo_dma_set_tail(uart.rx_buffer, dma_tail - remaining);
}

void sl_hci_disable_sleep(bool set_sleep_disabled)
{
  sleep_disabled = set_sleep_disabled;
}

int sl_hci_uart_read(uint8_t *data, uint16_t len)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  update_buffer_status(handle);

  CORE_EXIT_ATOMIC();
  len = sl_hci_fifo_read(uart.rx_buffer, data, len);
  return len;
}

/* Peek how much data buffered in rx that can be immediately read out */
int sl_hci_uart_rx_buffered_length()
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  update_buffer_status(handle);

  CORE_EXIT_ATOMIC();
  return sl_hci_fifo_length(uart.rx_buffer);
}

static void tx_callback(UARTDRV_Handle_t handle, Ecode_t status, uint8_t *buffer, UARTDRV_Count_t count)
{
  (void)handle;
  (void)buffer;
  (void)count;
  hci_common_transport_transmit_complete(status);
}

Ecode_t sl_hci_uart_write(uint8_t *data, uint16_t len)
{
  Ecode_t status;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  status = UARTDRV_Transmit(handle, data, len, tx_callback);

  CORE_EXIT_ATOMIC();
  return status;
}

void sl_hci_uart_get_port_pin(uint8_t *port, uint8_t *pin)
{
  *port = SL_UARTDRV_USART_VCOM_RX_PORT;
  *pin = SL_UARTDRV_USART_VCOM_RX_PIN;
}
