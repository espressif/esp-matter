#ifndef SL_BTCTRL_HCI_CPC_RTOS_H
#define SL_BTCTRL_HCI_CPC_RTOS_H

/**
 * Initialize RTOS task for HCI CPC Reader
 */
sl_status_t sl_btctrl_hci_cpc_rtos_init(void);

/**
 * Deinitialize HCI tasks
 */
void sl_btctrl_hci_cpc_rtos_deinit(void);

/**
 * Callback handler when new packet is received from cpc
 */
void sl_btctrl_hci_cpc_rx(uint8_t endpoint_id, void * arg);
#endif
