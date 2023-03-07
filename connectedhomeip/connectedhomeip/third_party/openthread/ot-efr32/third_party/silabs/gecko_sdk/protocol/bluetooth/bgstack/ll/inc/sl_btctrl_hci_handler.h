#ifndef SL_BTCTRL_VENDOR_H
#define SL_BTCTRL_VENDOR_H

#include <stddef.h>
#include <stdbool.h>
#include <sl_status.h>

/**
 * @addtogroup sl_btctrl_hci_handdler API create custom hci command handlers
 * @brief Provides API to create HCI message handling hooks to create custom hci command handlers
 *
 */

/**
 * @brief Opaque message handle to identify HCI command state, this is passed back to API functions
 *
 */
struct sl_btctrl_hci_message;

/**
 * @brief HCI message callback function
 *
 */
typedef bool (*sl_btctrl_vendor_handler)(struct sl_btctrl_hci_message * packet);
typedef struct sl_btctrl_command_handler {
  struct sl_btctrl_command_handler * next;
  sl_btctrl_vendor_handler handler;
} sl_btctrl_command_handler_t;

/**
 * @brief Register HCI handler to be called when packet is received
 *
 * @param str Pointer to sl_btctrl_command_handler structure, this is fully initialized by the function
 * @param handler function pointer to the handler to be called
 */
void sl_btctrl_hci_register_handler(sl_btctrl_command_handler_t * str, sl_btctrl_vendor_handler handler);

/**
 * @brief Get HCI Command opcode of the HCI message
 *
 * @param handle pointer to the HCI message
 * @param opcode pointer to uint16_t where the command opcode is copied to
 * @return sl_status_t SL_STATUS_OK if success
 */
sl_status_t sl_btctrl_hci_message_get_opcode(struct sl_btctrl_hci_message * handle, uint16_t * opcode);

/**
 * @brief Get Length of the parameter data of the HCI command
 *
 * @param handle pointer to the hci message
 * @param length pointer to size_t data where length is to be copied to
 * @return sl_status_t SL_STATUS_OK if success
 */
sl_status_t sl_btctrl_hci_message_get_length(struct sl_btctrl_hci_message * handle, size_t * length);

/**
 * @brief Get parameter data of the HCI command
 * If buffer is too small to contain all data then it is filled with as much data as possible and SL_STATUS_FULL error is returned
 *
 * @param handle pointer to the HCI command
 * @param buffer pointer to data buffer where data is to be copied to
 * @param buffer_len length of the data buffer
 * @return sl_status_t SL_STATUS_OK if success
 */
sl_status_t sl_btctrl_hci_message_get_parameters(struct sl_btctrl_hci_message * handle, uint8_t * buffer, size_t buffer_len);

/**
 * @brief Set HCI response event to the hci command,
 * after this command pointer is an pointer to HCI eventand no other hci_message-functions can be used on it
 * Maximum response size is currently limited to 50bytes
 *
 * @param handle pointer to the HCI command
 * @param bt_status Bluetooth status code to be used in event
 * @param buffer pointer to data buffer to be
 * @param buffer_len length of data to be added as event parameters
 * @return sl_status_t SL_STATUS_OK if success, SL_STATUS_INVALID_PARAMETER if data is too long
 */
sl_status_t sl_btctrl_hci_message_set_response(struct sl_btctrl_hci_message * handle, uint8_t bt_status, const uint8_t * buffer, size_t buffer_len);

/**
 * @brief Maximum length of HCI response supported
 *
 */
#define SL_BTCTRL_MAX_HCI_RESPONSE_LENGTH 50
#endif
