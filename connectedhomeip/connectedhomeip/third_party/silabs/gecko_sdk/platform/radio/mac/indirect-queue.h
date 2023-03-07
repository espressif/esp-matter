/***************************************************************************//**
 * @file indirect-queue.h
 *
 * @brief Manages the pool of indirect messages. These messages cannot be
 *  transmitted until after we receive a poll message from the destination. There
 *  are 3 types of indirect messages:
 *     * Queued headers addressed to short destination addresses.
 *     * Queued headers addressed to long destination addresses.
 *     * Just-in-time (JIT) messages.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef INDIRECT_QUEUE_H
#define INDIRECT_QUEUE_H

void sl_mac_indirect_queue_init(uint16_t timeoutMs);

void sl_mac_mark_indirect_buffers(void);

// Set function for runtime configuration of indirect transmission timeout
void sl_mac_set_indirect_transmission_timeout (uint16_t newTimeoutMs);

// Submit a message to the indirect transmit queue.  This message will be sent
// to the device when the device polls for the data.
sl_status_t sl_mac_indirect_submit(PacketHeader header);

// Returns true if there is an indirect message in the queue with a matching
// address.
bool sl_mac_long_id_data_pending(EmberEUI64 address);

// Takes the appropriate action for an incoming MAC data poll, returning
// true if the poll was handled at the MAC layer.
bool sl_mac_indirect_process_poll(uint8_t mac_index, PacketHeader header);

// Returns true if there is additional data (other than header) that has the
// same destination as header.
bool sl_mac_check_additional_pending_data(PacketHeader header);

// Returns true if header is in the indirect pool (indicating that no one else
// should be troubled with a transmit complete callback).
void sl_mac_indirect_transmit_complete(uint8_t mac_index, sl_status_t status, PacketHeader packet, uint8_t tag);

void sl_mac_indirect_event_handler(void);

// Called every emberTick() to check if anyone has polled.
void sl_mac_indirect_check_poll_flags(void);

// Drop all messages currently waiting to be transmitted for the network indicated.
void sl_mac_indirect_purge(uint8_t nwk_index);
void sli_mac_transient_table_init(void* p_fn);

#endif // INDIRECT_QUEUE_H
