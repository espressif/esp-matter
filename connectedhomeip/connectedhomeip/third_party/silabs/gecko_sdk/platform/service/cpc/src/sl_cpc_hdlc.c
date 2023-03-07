/***************************************************************************/ /**
 * @file
 * @brief CPC API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sli_cpc_hdlc.h"
#include "sli_cpc_crc.h"

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

extern inline uint8_t sli_cpc_hdlc_get_flag(const uint8_t *header_buf);
extern inline uint8_t sli_cpc_hdlc_get_address(const uint8_t *header_buf);
extern inline uint16_t sli_cpc_hdlc_get_length(const uint8_t *header_buf);
extern inline uint8_t sli_cpc_hdlc_get_control(const uint8_t *header_buf);
extern inline uint16_t sli_cpc_hdlc_get_hcs(const uint8_t *header_buf);
extern inline uint16_t sli_cpc_hdlc_get_fcs(const uint8_t *payload_buf, uint16_t payload_length);
extern inline uint8_t sli_cpc_hdlc_get_frame_type(uint8_t control);
extern inline uint8_t sli_cpc_hdlc_get_seq(uint8_t control);
extern inline uint8_t sli_cpc_hdlc_get_ack(uint8_t control);
extern inline uint8_t sli_cpc_hdlc_get_supervisory_function(uint8_t control);
extern inline uint8_t sli_cpc_hdlc_get_unumbered_type(uint8_t control);
extern inline bool sli_cpc_hdlc_is_poll_final(uint8_t control);
extern inline uint8_t sli_cpc_hdlc_create_control_data(uint8_t seq, uint8_t ack, bool poll_final);
extern inline uint8_t sli_cpc_hdlc_create_control_supervisory(uint8_t ack, uint8_t supervisory_function);
extern inline uint8_t sli_cpc_hdlc_create_control_unumbered(uint8_t type);
extern inline void sli_cpc_hdlc_set_control_ack(uint8_t *header_buf, uint8_t ack);

/***************************************************************************/ /**
 * Initialize Power Manager module.
 ******************************************************************************/
void sli_cpc_hdlc_create_header(uint8_t *header_buf,
                                uint8_t address,
                                uint16_t length,
                                uint8_t control,
                                bool compute_crc)
{
  header_buf[0] = SLI_CPC_HDLC_FLAG_VAL;
  header_buf[1] = address;
  header_buf[2] = (uint8_t)length;
  header_buf[3] = (uint8_t)(length >> 8);
  header_buf[4] = control;

  if (compute_crc) {
    uint16_t hcs;

    hcs = sli_cpc_get_crc_sw(header_buf, SLI_CPC_HDLC_HEADER_SIZE);

    header_buf[5] = (uint8_t)hcs;
    header_buf[6] = (uint8_t)(hcs >> 8);
  }
}
