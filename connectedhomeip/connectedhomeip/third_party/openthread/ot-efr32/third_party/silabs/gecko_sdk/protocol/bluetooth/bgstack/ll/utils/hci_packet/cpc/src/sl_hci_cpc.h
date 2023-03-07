#ifndef SL_HCI_CPC_H
#define SL_HCI_CPC_H

#include "em_common.h"

typedef uint32_t Ecode_t;

void sl_hci_cpc_init(void);
int sl_hci_cpc_read(uint8_t **read_buf);
void sl_hci_cpc_free(void *buf);
void sl_hci_cpc_rx_done();
Ecode_t sl_hci_cpc_write(uint8_t *data, uint16_t len);
int sl_hci_cpc_new_data();

#endif // SL_HCI_CPC_H
