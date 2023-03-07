#ifndef SL_BTCTRL_HCI_SLEEP_H
#define SL_BTCTRL_HCI_SLEEP_H

#include <stdbool.h>
#include <stdint.h>
#include <em_common.h>
#include <sl_status.h>

void sl_btctrl_hci_sleep_init(void);
void sl_btctrl_hci_sleep_sleep(void);
bool sl_btctrl_hci_sleep_request();
sl_status_t sl_bthci_sleep_wakeup_callback();
void sl_btctrl_hci_sleep_require_em1(bool req);

#ifndef SL_BTCTRL_HCI_SLEEP_EXCLUDE_WEAKS
SL_WEAK bool sl_btctrl_hci_sleep_request()
{
  return false;
}

SL_WEAK void sl_btctrl_hci_sleep_require_em1(bool req_em1)
{
  (void)req_em1;
}
#endif

#endif // SL_BTCTRL_HCI_SLEEP_H
