#ifndef SL_HCI_UART_H
#define SL_HCI_UART_H

#include "em_common.h"

typedef uint32_t Ecode_t;

void sl_hci_uart_init(void);
int sl_hci_uart_read(uint8_t *data, uint16_t len);
Ecode_t sl_hci_uart_write(uint8_t *data, uint16_t len);
int sl_hci_uart_rx_buffered_length();
void sl_hci_uart_enable_usart(bool enable);
void sl_hci_uart_enable_usart_int(void);
void sl_hci_uart_clear_usart_int(void);
void sl_hci_uart_get_port_pin(uint8_t *port, uint8_t *pin);
void sl_hci_disable_sleep(bool set_sleep_disabled);

#endif // SL_HCI_UART_H
