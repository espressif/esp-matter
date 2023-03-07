#ifndef HCI_RESET_H
#define HCI_RESET_H

#include <stdint.h>

bool sl_hci_reset_events_pending(void);
bool sl_hci_reset_reason_is_sys_reset(void);
void sl_hci_reset(void);

#endif // HCI_RESET_H
