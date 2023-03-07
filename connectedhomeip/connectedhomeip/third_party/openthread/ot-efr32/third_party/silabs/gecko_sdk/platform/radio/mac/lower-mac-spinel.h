/***************************************************************************//**
 * @file
 * @brief
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

#ifndef LOWER_MAC_SPINEL_H
#define LOWER_MAC_SPINEL_H
#include "stack/include/ember.h"
#include "lower-mac-spinel-config.h"

sl_status_t sl_mac_spinel_add_src_match_short_entry(uint16_t short_address);
sl_status_t sl_mac_spinel_add_src_match_ext_entry(uint8_t *ext_address);
sl_status_t sl_mac_spinel_clear_src_match_short_entry(uint16_t short_address);
sl_status_t sl_mac_spinel_clear_src_match_ext_entry(uint8_t *ext_address);

//-------------------------------------------------------------------------------
// uc callbacks

void sli_lower_mac_tick_callback(void);

#endif // LOWER_MAC_SPINEL_H
