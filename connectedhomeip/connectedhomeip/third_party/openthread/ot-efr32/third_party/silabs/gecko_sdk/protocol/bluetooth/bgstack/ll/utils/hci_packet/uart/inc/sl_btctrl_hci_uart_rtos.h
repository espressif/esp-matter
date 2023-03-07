#ifndef SL_BTCTRL_HCI_UART_RTOS_H
#define SL_BTCTRL_HCI_UART_RTOS_H

/**
 * Initialize RTOS task for HCI UART Reader
 */
sl_status_t sl_btctrl_hci_uart_rtos_init(void);

/**
 * Deinitialize HCI tasks
 */
void sl_btctrl_hci_uart_rtos_deinit(void);

#endif
