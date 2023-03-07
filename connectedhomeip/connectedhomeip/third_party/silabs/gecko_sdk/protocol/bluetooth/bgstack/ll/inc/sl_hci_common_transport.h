/**
 * @brief The API between HCI and common HCI transport.
 *
 * The interface configuration shall be done using UC configuration.
 *
 */
#ifndef _HCI_COMMON_TRANSPORT_H_
#define _HCI_COMMON_TRANSPORT_H_

#include <stdbool.h>
#include <stdint.h>
#include "sl_btctrl_hci.h"

/**
 * The HCI receives a message from currently selected transport layer.
 * The HCI transport calls this function implemented in the HCI
 * once it has received a complete HCI message. If the HCI does not
 * manage to process the message, the transport may drop the message.
 * @param[in] data Pointer to the received data.
 * @param[in] len Length of the received data.
 * @param[in] lastFragment Indicates if this is the last fragment of
 *       a complete HCI message.
 * @return  0 - success
 *         -1 - out of memory
 *         -2 - badly formatted message. */
int16_t hci_common_transport_receive(uint8_t *data, int16_t len, bool lastFragment);

/**
 * Transmit HCI message using the currently used transport layer.
 * The HCI calls this function to transmit a full HCI message.
 * @param[in] data Data buffer to be transmitted.
 * @param[in] len Length of the data.
 * @return 0 - on success, or -1 on failure. */
uint32_t hci_common_transport_transmit(uint8_t *data, int16_t len);

/**
 * The HCI transport indicates a message has been transmitted. */
void hci_common_transport_transmit_complete(uint32_t status);

/**
 * Initialize the currently selected transport layer. */
void hci_common_transport_init(void);

#endif // _COMMON_TRANSPORT_H_
