/**
 * @brief Bluetooth controller HCI API.
 *
 * This interface is for external use.
 */
#ifndef _SL_BTCTRL_HCI_H_
#define _SL_BTCTRL_HCI_H_

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

//Initialize Vendor Specific Extensions
void sl_bthci_init_vs(void);

void sl_bthci_init_upper(void);

void hci_enableVendorSpecificDebugging(void);

void hci_debugEnable(void);

sl_status_t hci_send_sleep_command_complete(void);

/**
 * The Bluetooth controller receives a HCI message fragment from the host.
 * The HCI transport can give the HCI message in fragments.
 * @param[in] data Pointer to the received data.
 * @param[in] len Length of the received data.
 * @param[in] lastFragment Indicate whether this is the last
 *       fragment of an HCI message (true / false).
 * @return  0 - success
 *         -1 - out of memory
 *         -2 - badly formatted message. */
int16_t sl_btctrl_hci_receive(uint8_t *data, int16_t len, bool lastFragment);

static inline int16_t hci_common_transport_receive(uint8_t *data, int16_t len, bool lastFragment)
{
  return sl_btctrl_hci_receive(data, len, lastFragment);
}

/**
 * The HCI transport has transmitted a message.
 * The HCI can transmit the next message after the transport
 * has given this indication. */
void sl_btctrl_hci_transmit_complete(uint32_t status);

static inline void hci_common_transport_transmit_complete(uint32_t status)
{
  sl_btctrl_hci_transmit_complete(status);
}

/**
 * Host has sent HCI reset command callback handler.
 * This is implemented as weak function and can be overridden by application to perform own activities before doing reset.
 * It is also possible to call sl_btctrl_hard_reset to start hard reset procedure
 *
 * @return - true to progress with normal soft reset
 *         - false if normal reset procedure should be bypassed
 */
bool sl_btctrl_reset_handler(void);

/**
 * Hard reset device
 * This is called from reset callback handler to implement hard reset
 * If HCI packet is in middle of transmit to host, reset will be done after transmit is complete
 */
void sl_btctrl_request_hard_reset(void);

void sl_btctrl_hci_parser_init_conn(void);

void sl_btctrl_hci_parser_init_adv(void);

void sl_btctrl_hci_parser_init_phy(void);

void sl_btctrl_hci_parser_init_past(void);

void sl_btctrl_hci_parser_init_default(void);

/**
 * Create hardware error event and try to send it to the host.
 * The created event shall be sent as high priority event.
 * @param[in] errorCode Code describing the error. */
void sl_btctrl_hci_send_hardware_error_event(uint8_t errorCode);

/**
 * @brief Sets PAST initiator feature bit,
 * and links in PAST sender and ll_adv_sync symbols to the project.
 */
void sl_btctrl_init_past_local_sync_transfer(void);

/**
 * @brief Sets PAST initiator feature bit,
 * and links in PAST sender, ll_scan_sync and ll_scan_sync_registry symbols to the project.
 */
void sl_btctrl_init_past_remote_sync_transfer(void);

/**
 * @brief Sets PAST receiver feature bit,
 * and links in PAST receiver, ll_scan_sync and ll_scan_sync_registry symbols to the project.
 */
void sl_btctrl_init_past_receiver(void);

#endif // _SL_BTCTRL_HCI_H_
