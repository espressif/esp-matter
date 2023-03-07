/***************************************************************************//**
 * Copyright 2021 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available here[1]. This software is distributed to you in
 * Source Code format and is governed by the sections of the MSLA applicable to
 * Source Code.
 *
 * [1] www.silabs.com/about-us/legal/master-software-license-agreement
 *
 ******************************************************************************/
#ifndef SL_WSRCP_MAC_H
#define SL_WSRCP_MAC_H

#include <stdbool.h>
#include <stdint.h>

struct sl_wsrcp_mac;

struct sl_wsrcp_mac *wsmac_register(int (*host_tx)(void *host_ctxt, const void *buf, int buf_len),
                                    int (*host_rx)(void *host_ctxt, void *buf, int buf_len),
                                    void *host_ctxt);
bool wsmac_rx_host(struct sl_wsrcp_mac *rcp_mac);
void wsmac_report_rx_crc_error(struct sl_wsrcp_mac *rcp_mac, uint16_t crc, int frame_len, uint8_t header, uint8_t irq_err_counter);

#endif
