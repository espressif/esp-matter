/***************************************************************************//**
 * @file sl_wisun_api.h
 * @brief Wi-SUN API
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_WISUN_API_H
#define SL_WISUN_API_H

#include "sl_wisun_types.h"
#include "sl_wisun_events.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @addtogroup SL_WISUN_API Wi-SUN Stack API
 *
 * Wi-SUN Stack API is based on requests from the application to the stack and
 * events from the stack to the application. Requests are made using function
 * calls, where a function call either performs the required action immediately
 * or initiates an internal operation within the stack, which terminates with an
 * event. All events contain a status code, indicating the result of the
 * requested operation. Events are also used by the stack to notify the
 * application of any important information, such as the state of the
 * connection.
 *
 * The application is expected to override sl_wisun_on_event() to handle events
 * from the stack. Because all events share a common header, the function may be
 * implemented as a switch statement. The event-specific data can be accessed
 * through the #sl_wisun_evt_t::evt union.
 *
 *     void sl_wisun_on_event(sl_wisun_evt_t *evt)
 *     {
 *       switch (evt->header.id) {
 *         case SL_WISUN_MSG_CONNECTED_IND_ID:
 *           handle_connected_event(evt->evt.connected);
 *           break;
 *         default:
 *           break;
 *       }
 *     }
 *
 * The API is thread-safe, which means can be called from multiple RTOS tasks. The
 * stack guarantees that only a single request is executed at a time and that requests
 * are handled in the order they were made. Event callback is executed in a
 * different context than the request, so the API functions may be called from
 * the event callback.
 *
 * @{
 *****************************************************************************/

/**
 * @name Callbacks
 * @{
 */

/**************************************************************************//**
 * Callback handler for a single event.
 *
 * @param evt The event to be handled
 *
 * This function is called when the stack sends an event to the application.
 * The application can declare its own version this function to customize
 * event handling. The default implementation discards all events.
 *
 * @see #SL_WISUN_EVT
 *****************************************************************************/
void sl_wisun_on_event(sl_wisun_evt_t *evt);

/**
 * @}
 */

/**************************************************************************//**
 * Set the size of the Wi-SUN network.
 *
 * @param[in] size Size of the network
 *   - #SL_WISUN_NETWORK_SIZE_SMALL: less than 100 nodes
 *   - #SL_WISUN_NETWORK_SIZE_MEDIUM: 100 to 800 nodes
 *   - #SL_WISUN_NETWORK_SIZE_LARGE: 800 and above.
 *   - #SL_WISUN_NETWORK_SIZE_TEST: a few nodes
 *   - #SL_WISUN_NETWORK_SIZE_CERTIFICATION: Wi-SUN FAN certification mode
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the size of the network. The value adjusts
 * internal behavior, such as timing parameters, to optimize device behavior
 * in regard to the network size. The device will function with any setting but
 * may exhibit non-optimal behavior. Setting the size too large may cause slow
 * connection speeds and increased latency. Conversely, a value too small may
 * cause increased network traffic. When configuring the stack for Wi-SUN FAN
 * certification, ::SL_WISUN_NETWORK_SIZE_CERTIFICATION setting should be used.
 * ::SL_WISUN_NETWORK_SIZE_TEST is intended for development time use, providing
 * faster initial connectivity over network performance.
 *****************************************************************************/
sl_status_t sl_wisun_set_network_size(sl_wisun_network_size_t size);

/**************************************************************************//**
 * Initiate a connection to a Wi-SUN network.
 *
 * @param[in] name Name of the Wi-SUN network as a zero-terminated string
 * @param[in] reg_domain Regulatory domain of the Wi-SUN network
 * @param[in] op_class Operating class of the Wi-SUN network
 * @param[in] op_mode Operating mode of the Wi-SUN network
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function initiates connection to a Wi-SUN network. Completion of the
 * request is indicated with a #SL_WISUN_MSG_CONNECTED_IND_ID event.
 *****************************************************************************/
sl_status_t sl_wisun_connect(const uint8_t *name,
                             sl_wisun_regulatory_domain_t reg_domain,
                             sl_wisun_operating_class_t op_class,
                             sl_wisun_operating_mode_t op_mode);

/**************************************************************************//**
 * Read an IP address.
 *
 * @param[in] address_type Type of the IP address to read
 *   - #SL_WISUN_IP_ADDRESS_TYPE_LINK_LOCAL: Link-local IPv6 address of the device
 *   - #SL_WISUN_IP_ADDRESS_TYPE_GLOBAL: Global unicast IPv6 address of the device
 *   - #SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER: Global unicast IPv6 address of the border router
 *   - #SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT: Link-local IPv6 address of the primary parent
 *   - #SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT: Link-local IPv6 address of the secondary parent
 * @param[out] address IP address to read
 * @return SL_STATUS_OK if successful, an error code otherwise.
 *****************************************************************************/
sl_status_t sl_wisun_get_ip_address(sl_wisun_ip_address_type_t address_type,
                                    sl_wisun_ip_address_t *address);

/**************************************************************************//**
 * Open a socket.
 *
 * @param[in] protocol Protocol type of the socket
 * @param[out] socket_id ID of the opened socket
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function opens a socket. Up to 10 sockets may be open at any
 * given time, including those opened implicitly via
 * sl_wisun_accept_on_socket().
 *****************************************************************************/
sl_status_t sl_wisun_open_socket(sl_wisun_socket_protocol_t protocol,
                                 sl_wisun_socket_id_t *socket_id);

/**************************************************************************//**
 * Close a socket.
 *
 * @param[in] socket_id ID of the socket
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_close_socket(sl_wisun_socket_id_t socket_id);

/**************************************************************************//**
 * Write data to an unconnected socket.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] remote_address IP address of the remote peer
 * @param[in] remote_port Port number of the remote peer
 * @param[in] data_length Amount of data to write
 * @param[in] data Pointer to the data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function initiates a data transmission to a remote peer and can only
 * be used on an unconnected UDP or ICMP socket. Completion of the transmission
 * is indicated with a #SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID event. The function
 * takes a copy of the data, so the caller may free the resource when the function
 * returns.
 *****************************************************************************/
sl_status_t sl_wisun_sendto_on_socket(sl_wisun_socket_id_t socket_id,
                                      const sl_wisun_ip_address_t *remote_address,
                                      uint16_t remote_port,
                                      uint16_t data_length,
                                      const uint8_t *data);

/**************************************************************************//**
 * Set a TCP socket to listening state.
 *
 * @param[in] socket_id ID of the socket
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets a TCP socket to listening state, allowing it to act as
 * a server socket, i.e., to receive connection requests from clients.
 * Connection requests are indicated with
 * #SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID events and accepted using
 * sl_wisun_accept_on_socket(). This function can only be used on an unconnected
 * TCP socket.
 *****************************************************************************/
sl_status_t sl_wisun_listen_on_socket(sl_wisun_socket_id_t socket_id);

/**************************************************************************//**
 * Accept a pending connection request on a TCP socket.
 *
 * @param[in] socket_id ID of the socket on listening state
 * @param[out] remote_socket_id ID of the new connected socket
 * @param[out] remote_address IP address of the remote peer
 * @param[out] remote_port Port number of the remote peer
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function accepts a pending connection request from a remote peer and
 * creates a new connected TCP socket for the connection. To decline a
 * connection request, the request must be accepted and then closed using
 * sl_wisun_close_socket(). The function can only be used on a TCP socket in
 * listening state.
 *****************************************************************************/
sl_status_t sl_wisun_accept_on_socket(sl_wisun_socket_id_t socket_id,
                                      sl_wisun_socket_id_t *remote_socket_id,
                                      sl_wisun_ip_address_t *remote_address,
                                      uint16_t *remote_port);

/**************************************************************************//**
 * Initiate a connection from a socket to a remote peer socket.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] remote_address IP address of the remote peer
 * @param[in] remote_port Port number of the remote peer
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function initiates a connection from a local socket to to a remote peer
 * socket. The result of the connection is indicated with a
 * #SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID event. Connecting a socket is
 * mandatory for TCP client sockets but may be also used on other types of
 * sockets. A connected socket can only receive and transmit data with the
 * designated peer. This function can only be used on an unconnected TCP or
 * UDP socket.
 *****************************************************************************/
sl_status_t sl_wisun_connect_socket(sl_wisun_socket_id_t socket_id,
                                    const sl_wisun_ip_address_t *remote_address,
                                    uint16_t remote_port);

/**************************************************************************//**
 * Bind a socket to a specific local address and/or a port number.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] local_address Local IP address to use on the socket. NULL if not bound.
 * @param[in] local_port Local port number to use on the socket. Zero if not bound.
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function binds the local address and/or the port of a socket. When the
 * local address is bound, the socket will only accept traffic sent to the
 * specified address and the transmitted packets will use the address as the
 * source address. If not bound, the socket will accept data sent to any valid
 * address of the device. The source address is selected by the stack.
 * Binding the local port number sets the port number for received and
 * transmitted packets. If not bound, the stack will select a port number
 * automatically. This function can only be used on an unconnected TCP or UDP
 * socket. Once bound to a specific address and/or port, the value cannot
 * be changed.
 *****************************************************************************/
sl_status_t sl_wisun_bind_socket(sl_wisun_socket_id_t socket_id,
                                 const sl_wisun_ip_address_t *local_address,
                                 uint16_t local_port);

/**************************************************************************//**
 * Write data to a connected socket.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] data_length Amount of data to write
 * @param[in] data Pointer to the data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function initiates transmission of data to a connected remote peer and
 * can only be used on a connected socket. Completion of the transmission is
 * indicated with a #SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID event. The function
 * takes a copy of the data, so the caller may free the resource when the function
 * returns.
 *****************************************************************************/
sl_status_t sl_wisun_send_on_socket(sl_wisun_socket_id_t socket_id,
                                    uint16_t data_length,
                                    const uint8_t *data);

/**************************************************************************//**
 * Read data from a socket.
 *
 * @param[in] socket_id ID of the socket
 * @param[out] remote_address IP address of the remote peer
 * @param[out] remote_port Port number of the remote peer
 * @param[in,out] data_length Amount of data to read on input, amount of data read on output
 * @param[in] data Pointer to where the read data is stored
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function reads buffered data from a socket. When reading data from a
 * UDP or ICMP socket, the entire packet must be read. Any data left unread is
 * discarded after this call. TCP sockets allow reading only a part of the
 * buffered data.
 *****************************************************************************/
sl_status_t sl_wisun_receive_on_socket(sl_wisun_socket_id_t socket_id,
                                       sl_wisun_ip_address_t *remote_address,
                                       uint16_t *remote_port,
                                       uint16_t *data_length,
                                       uint8_t *data);

/**************************************************************************//**
 * Disconnect from the Wi-SUN network.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function disconnects an active connection or cancels an ongoing
 * connection attempt.
 *****************************************************************************/
sl_status_t sl_wisun_disconnect();

/**************************************************************************//**
 * Set a trusted certificate used to verify the authentication server certificate.
 *
 * @param[in] certificate_options Options for the certificate
 *   - #SL_WISUN_CERTIFICATE_OPTION_APPEND: Append the certificate to the list of trusted certificates
 *                                          instead of replacing the previous entries
 *   - #SL_WISUN_CERTIFICATE_OPTION_IS_REF: The application guarantees the certificate data will remain
 *                                          in scope and can therefore be referenced instead of copied
 * @param[in] certificate_length Size of the certificate data
 * @param[in] certificate Pointer to the certificate data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_trusted_certificate(uint16_t certificate_options,
                                             uint16_t certificate_length,
                                             const uint8_t *certificate);

/**************************************************************************//**
 * Set the device certificate used to authenticate to the authentication server.
 *
 * @param[in] certificate_options Options for the certificate.
 *   - #SL_WISUN_CERTIFICATE_OPTION_APPEND: Append the certificate to the list of device certificates
 *                                          instead of replacing the previous entries
 *   - #SL_WISUN_CERTIFICATE_OPTION_IS_REF: The application guarantees the certificate data will remain
 *                                          in scope and can therefore be referenced instead of copied
 *   - #SL_WISUN_CERTIFICATE_OPTION_HAS_KEY: The certificate has a private key
 * @param[in] certificate_length Size of the certificate data
 * @param[in] certificate Pointer to the certificate data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_device_certificate(uint16_t certificate_options,
                                            uint16_t certificate_length,
                                            const uint8_t *certificate);

/**************************************************************************//**
 * Set the private key of the device certificate.
 *
 * @param[in] key_options Options for the private key
 *   - #SL_WISUN_PRIVATE_KEY_OPTION_IS_REF: The application guarantees the private key data will remain
 *                                          in scope and can therefore be referenced instead of copied
 * @param[in] key_length Size of the private key data
 * @param[in] key Pointer to the private key data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_device_private_key(uint16_t key_options,
                                            uint16_t key_length,
                                            const uint8_t *key);

/**************************************************************************//**
 * Read a set of statistics.
 *
 * @param[in] statistics_type Type of statistics to read
 *   - #SL_WISUN_STATISTICS_TYPE_PHY: PHY/RF statistics
 *   - #SL_WISUN_STATISTICS_TYPE_MAC: MAC statistics
 *   - #SL_WISUN_STATISTICS_TYPE_FHSS: Frequency hopping statistics
 *   - #SL_WISUN_STATISTICS_TYPE_WISUN: Wi-SUN statistics
 *   - #SL_WISUN_STATISTICS_TYPE_NETWORK: 6LoWPAN/IP stack statistics
 *   - #SL_WISUN_STATISTICS_TYPE_REGULATION: Regional regulation statistics
 * @param[out] statistics Set of statistics read
 * @return SL_STATUS_OK if successful, an error code otherwise.
 *
 * This function reads a set of statistics from the stack. Statistics are
 * cumulative and reset when a connection is initiated or by calling
 * sl_wisun_reset_statistics().
 *****************************************************************************/
sl_status_t sl_wisun_get_statistics(sl_wisun_statistics_type_t statistics_type,
                                    sl_wisun_statistics_t *statistics);

/**************************************************************************//**
 * Set a socket option.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] option Socket option to set
 *   - #SL_WISUN_SOCKET_OPTION_EVENT_MODE: Event mode
 *   - #SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP: Multicast group
 * @param[in] option_data Socket option specific data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_socket_option(sl_wisun_socket_id_t socket_id,
                                       sl_wisun_socket_option_t option,
                                       const sl_wisun_socket_option_data_t *option_data);

/**************************************************************************//**
 * Set the maximum TX power.
 *
 * @param[in] tx_power TX power in dBm
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the maximum TX power. The device may use
 * a lower value based on internal decision making or hardware limitations but
 * will never exceed the given value.
 *****************************************************************************/
sl_status_t sl_wisun_set_tx_power(int8_t tx_power);

/**************************************************************************//**
 * Set a channel plan.
 *
 * @param[in] ch0_frequency Frequency of the first channel in kHz
 * @param[in] number_of_channels Number of channels
 * @param[in] channel_spacing Spacing between the channels
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets an application-specific channel plan for use in the
 * following connections. By default, the channel plan is set indirectly based
 * on the regulatory domain and the operating class given in sl_wisun_connect().
 *****************************************************************************/
sl_status_t sl_wisun_set_channel_plan(uint32_t ch0_frequency,
                                      uint16_t number_of_channels,
                                      sl_wisun_channel_spacing_t channel_spacing);

/**************************************************************************//**
 * Set a mask of operating channels.
 *
 * @param[in] channel_mask Mask of operating channels
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets a mask of channels the device is allowed to operate in
 * for unicast frequency hopping and asynchronous frames. By default, all
 * channels in the channel plan are allowed. The mask can only be used to
 * further restrict the channels. Channels outside the channel plan or channels
 * internally excluded are ignored. This mask will be used in the following
 * connections.
 *
 * @warning By comparison to the Wi-SUN FAN specification, the channel mask
 *          logic is inverted. The specification references a mask of excluded
 *          channels.
 *****************************************************************************/
sl_status_t sl_wisun_set_allowed_channel_mask(const sl_wisun_channel_mask_t *channel_mask);

/**************************************************************************//**
 * Set a mask of operating channels.
 *
 * This macro provides backwards compatibility to an older version of a renamed
 * function.
 *****************************************************************************/
#define sl_wisun_set_channel_mask sl_wisun_set_allowed_channel_mask

/**************************************************************************//**
 * Add a MAC address to the list of allowed addresses.
 *
 * @param[in] address MAC address
 *   - **broadcast address**: allow all MAC addresses
 *   - **unicast address**: allow the given MAC address
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function adds a MAC address to the list of allowed addresses. When the
 * first address is added to the list, the list of denied addresses is cleared
 * and the device will start preventing communication with any device whose MAC
 * address does not match any of addresses on the list. By default, all MAC
 * addresses are allowed. Up to 10 MAC addresses may be added to the list.
 *****************************************************************************/
sl_status_t sl_wisun_allow_mac_address(const sl_wisun_mac_address_t *address);

/**************************************************************************//**
 * Add a MAC address to the list of denied addresses.
 *
 * @param[in] address MAC address
 *   - **broadcast address**: deny all MAC addresses
 *   - **unicast address**: deny the given MAC address
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function adds a MAC address to the list of denied addresses. When the
 * first address is added to the list, the list of allowed addresses is cleared
 * and the device will start preventing communication with any device whose MAC
 * address matches any of the addresses on the list. By default, all MAC
 * addresses are allowed. Up to 10 MAC addresses may be added to the list.
 *****************************************************************************/
sl_status_t sl_wisun_deny_mac_address(const sl_wisun_mac_address_t *address);

/**************************************************************************//**
 * Get a socket option.
 *
 * @param[in] socket_id ID of the socket
 * @param[in] option Socket option to get
 *   - #SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT: Send buffer limit
 * @param[out] option_data Socket option specific data
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function retrieves the value of a socket option.
 *****************************************************************************/
sl_status_t sl_wisun_get_socket_option(sl_wisun_socket_id_t socket_id,
                                       sl_wisun_socket_option_t option,
                                       sl_wisun_socket_option_data_t *option_data);

/**************************************************************************//**
 * Get the current join state.
 *
 * @param[out] join_state Join state
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function retrieves the current state of the connection process. The
 * function can only be used once a connection has been initiated.
 *****************************************************************************/
sl_status_t sl_wisun_get_join_state(sl_wisun_join_state_t *join_state);

/**************************************************************************//**
 * Clear the credential cache.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function clears the cached authentication credentials stored in
 * non-volatile storage. The function is intended for test purposes. Note that
 * clearing the credential cache may prevent the node from reconnecting to the
 * same parent until the corresponding cache entry has expired on the parent.
 *****************************************************************************/
sl_status_t sl_wisun_clear_credential_cache();

/**************************************************************************//**
 * Get the current device MAC address in use.
 *
 * @param[out] address MAC address
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_get_mac_address(sl_wisun_mac_address_t *address);

/**************************************************************************//**
 * Set the device MAC address to be used.
 *
 * @param[in] address MAC address
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the MAC address for use in the following connections.
 * By default, the device will use the built-in unique device MAC address.
 * The address is reset to the built-in value upon power up.
 *****************************************************************************/
sl_status_t sl_wisun_set_mac_address(const sl_wisun_mac_address_t *address);

/**************************************************************************//**
 * Reset a set of statistics in the stack.
 *
 * @param[in] statistics_type Type of statistics to reset
 *   - #SL_WISUN_STATISTICS_TYPE_PHY: PHY/RF statistics
 *   - #SL_WISUN_STATISTICS_TYPE_MAC: MAC statistics
 *   - #SL_WISUN_STATISTICS_TYPE_FHSS: Frequency hopping statistics
 *   - #SL_WISUN_STATISTICS_TYPE_WISUN: Wi-SUN statistics
 *   - #SL_WISUN_STATISTICS_TYPE_NETWORK: 6LoWPAN/IP stack statistics
 * @return SL_STATUS_OK if successful, an error code otherwise.
 *****************************************************************************/
sl_status_t sl_wisun_reset_statistics(sl_wisun_statistics_type_t statistics_type);

/**************************************************************************//**
 * Get the number of RPL neighbors (parents and children).
 *
 * @param[out] neighbor_count Number of neighbors
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_get_neighbor_count(uint8_t *neighbor_count);

/**************************************************************************//**
 * Get a list of RPL neighbor (parents and children) MAC addresses.
 *
 * @param[in,out] neighbor_count Maximum number of neighbors to read on input,
 *                               number of neighbors read on output
 * @param[out] neighbor_mac_addresses Pointer to memory where to store neighbor
 *                                    MAC addresses
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_get_neighbors(uint8_t *neighbor_count,
                                   sl_wisun_mac_address_t *neighbor_mac_addresses);

/**************************************************************************//**
 * Get information about a RPL neighbor (parent or child).
 *
 * @param[in] neighbor_mac_address Pointer to neighbor MAC address
 * @param[out] neighbor_info Pointer to where the read information is stored
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_get_neighbor_info(const sl_wisun_mac_address_t *neighbor_mac_address,
                                       sl_wisun_neighbor_info_t *neighbor_info);

/**************************************************************************//**
 * Set unicast settings.
 *
 * @param[in] dwell_interval_ms Unicast Dwell Interval (15-255 ms)
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the parameters for unicast channel hopping to be used in
 * the following connections. The Unicast Dwell Interval specifies the duration
 * which the node will listen to a channel within its listening schedule.
 * The default value is 255 ms.
 *****************************************************************************/
sl_status_t sl_wisun_set_unicast_settings(uint8_t dwell_interval_ms);

/**************************************************************************//**
 * Set the private key of the device certificate.
 *
 * @param[in] key_id Key ID of the private key
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the device private key using a key ID reference.
 *****************************************************************************/
sl_status_t sl_wisun_set_device_private_key_id(uint32_t key_id);

/**************************************************************************//**
 * Set the regional regulation.
 *
 * @param[in] regulation Regional regulation
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the regional regulation for use in the following
 * connections. The selected regional regulation will impact both the Wi-SUN
 * stack performance and its behavior. See regulation standards for details.
 * No regulation is set by default.
 *****************************************************************************/
sl_status_t sl_wisun_set_regulation(sl_wisun_regulation_t regulation);

/**************************************************************************//**
 * Set the thresholds for transmission duration level event.
 *
 * @param[in] warning_threshold Warning threshold in percent or -1 to disable
 * @param[in] alert_threshold Alert threshold in percent or -1 to disable
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * This function sets the thresholds for transmission duration level event.
 * When set and when a regional regulation is enabled using
 * sl_wisun_set_regulation(), a #SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID event
 * is sent when one of the configured thresholds is exceeded. This can be used
 * by the application to prevent exceeding the total transmission duration
 * allowed in the regional regulation. Thresholds are defined as a percentage
 * of the maximum transmission duration permitted by the regional regulation.
 *****************************************************************************/
sl_status_t sl_wisun_set_regulation_tx_thresholds(int8_t warning_threshold,
                                                  int8_t alert_threshold);

/**************************************************************************//**
 * Set the async transmission fragmentation parameters.
 *
 * @param[in] fragment_duration_ms Max duration of a fragment in ms (min 500 ms)
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * Async transmissions, such as Wi-SUN PAN advertisement packets, are sent
 * to every allowed operating channel and may therefore block broadcast and
 * unicast traffic. This impact can be reduced by splitting the channel list
 * into fragments based on the maximum transmission duration and by forcing a
 * delay between the fragments, allowing other traffic to occur. This function
 * sets the maximum duration of a PA, PAS, PC, and PCS advertisement period
 * fragments. A small value trades off longer connection times for shorter
 * latencies. Setting the duration to SL_WISUN_ADVERT_FRAGMENT_DISABLE disables
 * advertisement fragmentation.
 *
 * By default, the fragmentation is disabled.
 *****************************************************************************/
sl_status_t sl_wisun_set_advert_fragment_duration(uint32_t fragment_duration_ms);

/**************************************************************************//**
 * Enable an algorithm that trades off unicast communication reliability
 * for latency.
 *
 * @param[in] mode Transmission mode to use
 *   <br/><b>SL_WISUN_UNICAST_TX_MODE_DEFAULT</b>: Default transmission mode.
 *   <br/><b>SL_WISUN_UNICAST_TX_MODE_HIGH_RELIABILITY</b>: High reliability, high
 * latency.
 * @return SL_STATUS_OK if successful, an error code otherwise
 *
 * Enable an algorithm that trades off unicast communication reliability
 * for latency. The mechanism is only effective when all the neighbors are enabled.
 * Enabling this option is detrimental when used with unaware Wi-SUN devices.
 *****************************************************************************/
sl_status_t sl_wisun_set_unicast_tx_mode(uint8_t mode);

/** @} (end SL_WISUN_API) */

#ifdef __cplusplus
}
#endif

#endif  // SL_WISUN_API_H
