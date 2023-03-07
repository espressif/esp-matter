/***************************************************************************/ /**
 * @file
 * @brief CPC HDLC
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

#ifndef SLI_CPC_HDLC_H
#define SLI_CPC_HDLC_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_status.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define SLI_CPC_HDLC_HEADER_SIZE      5
#define SLI_CPC_HDLC_HEADER_RAW_SIZE  7

#define SLI_CPC_HDLC_FLAG_VAL  0x14

#define SLI_CPC_HDLC_FLAG_POS     0
#define SLI_CPC_HDLC_ADDRESS_POS  1
#define SLI_CPC_HDLC_LENGTH_POS   2
#define SLI_CPC_HDLC_CONTROL_POS  4
#define SLI_CPC_HDLC_HCS_POS      5

#define SLI_CPC_HDLC_FRAME_TYPE_DATA          0
#define SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY   2
#define SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED    3

#define SLI_CPC_HDLC_CONTROL_FRAME_TYPE_SHIFT  6
#define SLI_CPC_HDLC_CONTROL_P_F_SHIFT  3
#define SLI_CPC_HDLC_CONTROL_SEQ_SHIFT  4
#define SLI_CPC_HDLC_CONTROL_SUPERVISORY_FNCT_ID_SHIFT  4
#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_SHIFT  0

#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_MASK  0x37

#define SLI_CPC_HDLC_ACK_SUPERVISORY_FUNCTION   0

#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_INFORMATION  0x00
#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_POLL_FINAL   0x04
#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_RESET_SEQ    0x31
#define SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_ACKNOWLEDGE  0x0E

#define SLI_CPC_HDLC_REJECT_SUPERVISORY_FUNCTION   1
#define SLI_CPC_HDLC_REJECT_PAYLOAD_SIZE  1

#define SLI_CPC_HDLC_FCS_SIZE  2

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * Gets HDLC header flag value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC header flag value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_flag(const uint8_t *header_buf)
{
  return header_buf[SLI_CPC_HDLC_FLAG_POS];
}

/***************************************************************************//**
 * Gets HDLC header address value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC header address value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_address(const uint8_t *header_buf)
{
  return header_buf[SLI_CPC_HDLC_ADDRESS_POS];
}

/***************************************************************************//**
 * Gets HDLC header payload length value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC header payload length value.
 ******************************************************************************/
inline uint16_t sli_cpc_hdlc_get_length(const uint8_t *header_buf)
{
  uint16_t length = 0;

  length  = (uint16_t)header_buf[SLI_CPC_HDLC_LENGTH_POS];
  length |= (uint16_t)header_buf[SLI_CPC_HDLC_LENGTH_POS + 1] << 8;

  return length;
}

/***************************************************************************//**
 * Gets HDLC header control value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC header control value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_control(const uint8_t *header_buf)
{
  return header_buf[SLI_CPC_HDLC_CONTROL_POS];
}

/***************************************************************************//**
 * Gets HDLC header HCS value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC header HCS value.
 ******************************************************************************/
inline uint16_t sli_cpc_hdlc_get_hcs(const uint8_t *header_buf)
{
  uint16_t hcs = 0;

  hcs  = (uint16_t)header_buf[SLI_CPC_HDLC_HCS_POS];
  hcs |= (uint16_t)header_buf[SLI_CPC_HDLC_HCS_POS + 1] << 8;

  return hcs;
}

/***************************************************************************//**
 * Gets HDLC payload FCS value.
 *
 * @param header_buf Pointer to the buffer that contains the HDLC header.
 *
 * @return HDLC payload FCS value.
 ******************************************************************************/
inline uint16_t sli_cpc_hdlc_get_fcs(const uint8_t *payload_buf, uint16_t payload_length)
{
  uint16_t fcs = 0;

  fcs  = (uint16_t)payload_buf[payload_length];
  fcs |= (uint16_t)payload_buf[payload_length + 1] << 8;

  return fcs;
}

/***************************************************************************//**
 * Gets HDLC frame type value.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return HDLC frame type value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_frame_type(uint8_t control)
{
  uint8_t type = control >> SLI_CPC_HDLC_CONTROL_FRAME_TYPE_SHIFT;

  if (type == 1 || type == 0) {
    type = SLI_CPC_HDLC_FRAME_TYPE_DATA;
  }

  return type;
}

/***************************************************************************//**
 * Gets HDLC frame SEQ value.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return HDLC frame SEQ value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_seq(uint8_t control)
{
  return (control >> SLI_CPC_HDLC_CONTROL_SEQ_SHIFT) & 0x07;
}

/***************************************************************************//**
 * Gets HDLC frame ACK value.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return HDLC frame ACK value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_ack(uint8_t control)
{
  return control & 0x07;
}

/***************************************************************************//**
 * Gets HDLC frame supervisory function value.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return HDLC frame supervisory function value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_supervisory_function(uint8_t control)
{
  return (control >> SLI_CPC_HDLC_CONTROL_SUPERVISORY_FNCT_ID_SHIFT) & 0x03;
}

/***************************************************************************//**
 * Gets HDLC u-frame type.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return HDLC u-frame type.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_get_unumbered_type(uint8_t control)
{
  return (control >> SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_SHIFT) & SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_MASK;
}

/***************************************************************************//**
 * Creates HDLC header.
 *
 * @param header_buf Pointer to the buffer where to write HDLC header.
 * @param address Address value.
 * @param length Length of payload.
 * @param control Control value.
 * @param compute_crc Set to true if this function shall compute the header
 *                    CRC (HCS). Set to false if DMA generates it automatically.
 ******************************************************************************/
void sli_cpc_hdlc_create_header(uint8_t *header_buf,
                                uint8_t address,
                                uint16_t length,
                                uint8_t control,
                                bool compute_crc);

/***************************************************************************//**
 * Creates header control value data frame type.
 *
 * @param seq Sequence number.
 * @param ack ACK value.
 * @param poll_final activate P/F bit (Poll/Final)
 *
 * @return HDLC header control value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_create_control_data(uint8_t seq, uint8_t ack, bool poll_final)
{
  uint8_t control = SLI_CPC_HDLC_FRAME_TYPE_DATA << SLI_CPC_HDLC_CONTROL_FRAME_TYPE_SHIFT;

  control |= seq << SLI_CPC_HDLC_CONTROL_SEQ_SHIFT;
  control |= ack;
  control |= (uint8_t)((uint8_t)poll_final << SLI_CPC_HDLC_CONTROL_P_F_SHIFT);

  return control;
}

/***************************************************************************//**
 * Creates header control value supervisory frame type.
 *
 * @param ack ACK value.
 * @param supervisory_function Supervisory function ID.
 *
 * @return HDLC header control value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_create_control_supervisory(uint8_t ack, uint8_t supervisory_function)
{
  uint8_t control = SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY << SLI_CPC_HDLC_CONTROL_FRAME_TYPE_SHIFT;

  control |= supervisory_function << SLI_CPC_HDLC_CONTROL_SUPERVISORY_FNCT_ID_SHIFT;
  control |= ack;

  return control;
}

/***************************************************************************//**
 * Creates header control value unumbered frame type.
 *
 * @return HDLC header control value.
 ******************************************************************************/
inline uint8_t sli_cpc_hdlc_create_control_unumbered(uint8_t type)
{
  uint8_t control = SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED << SLI_CPC_HDLC_CONTROL_FRAME_TYPE_SHIFT;

  control |= type << SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_SHIFT;

  return control;
}

/***************************************************************************//**
 * Gets HDLC u-frame poll/final bit.
 *
 * @param control Control value specified in HDLC header.
 *
 * @return true if HDLC frame poll/frame bit is set.
 ******************************************************************************/
inline bool sli_cpc_hdlc_is_poll_final(uint8_t control)
{
  if (control & (1 << SLI_CPC_HDLC_CONTROL_P_F_SHIFT)) {
    return true;
  }
  return false;
}

/***************************************************************************//**
 * Update the ACK number in a frame's header.
 ******************************************************************************/
inline void sli_cpc_hdlc_set_control_ack(uint8_t *control,
                                         uint8_t ack)
{
  *control &= ~0x07;
  *control |= ack;
}

/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // SLI_CPC_HDLC_H
