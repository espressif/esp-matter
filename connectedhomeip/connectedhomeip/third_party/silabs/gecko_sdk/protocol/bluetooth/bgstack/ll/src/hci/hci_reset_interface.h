#ifndef HCI_RESET_INTERFACE_H
#define HCI_RESET_INTERFACE_H

#include "hci_reset.h"

bool sl_btctrl_hci_reset_events_pending(void);
bool sl_btctrl_hci_reset_reason_is_sys_reset(void);
void sl_btctrl_hci_reset(void);

#endif // HCI_RESET_INTERFACE_H
