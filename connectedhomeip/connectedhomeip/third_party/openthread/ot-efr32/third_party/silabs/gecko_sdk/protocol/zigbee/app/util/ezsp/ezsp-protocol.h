/***************************************************************************//**
 * @file
 * @brief Definitions useful for creating and decoding EZSP frames. The
 * frame format is defined in the EM260 datasheet and looks like this:
 *   <sequence:1>
 *   <frame control:2>
 *   <frame ID:2>
 *   <parameters:0-120>
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_EZSP_PROTOCOL_H
#define SILABS_EZSP_PROTOCOL_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

//------------------------------------------------------------------------------
// Protocol Definitions

#define EZSP_PROTOCOL_VERSION     0x09

// EZSP max length + Frame Control extra byte +
// Secure EZSP autentication header + Frame ID extra byte + Secure EZSP MIC
#define EZSP_MAX_FRAME_LENGTH     (200 + 1 + 14 + 1 + 4)

// EZSP Sequence Index for both legacy and extended frame format
#define EZSP_SEQUENCE_INDEX       0

// Legacy EZSP Frame Format
#define EZSP_MIN_FRAME_LENGTH     3
#define EZSP_FRAME_CONTROL_INDEX  1
#define EZSP_FRAME_ID_INDEX       2
#define EZSP_PARAMETERS_INDEX     3

// Extended EZSP Frame Format
#define EZSP_EXTENDED_MIN_FRAME_LENGTH          5
#define EZSP_EXTENDED_FRAME_ID_LENGTH           2
#define EZSP_EXTENDED_FRAME_CONTROL_LB_INDEX    1
#define EZSP_EXTENDED_FRAME_CONTROL_HB_INDEX    2
#define EZSP_EXTENDED_FRAME_ID_LB_INDEX         3
#define EZSP_EXTENDED_FRAME_ID_HB_INDEX         4
#define EZSP_EXTENDED_PARAMETERS_INDEX          5

#define EZSP_STACK_TYPE_MESH      0x02

//------------------------------------------------------------------------------
// EZSP Frame Utils
#define emEzspGetFrameControl() (serialGetResponseByte(EZSP_EXTENDED_FRAME_CONTROL_HB_INDEX) << 8 \
                                 | serialGetResponseByte(EZSP_EXTENDED_FRAME_CONTROL_LB_INDEX))

#define emEzspGetFrameId() (serialGetResponseByte(EZSP_EXTENDED_FRAME_ID_HB_INDEX) << 8 \
                            | serialGetResponseByte(EZSP_EXTENDED_FRAME_ID_LB_INDEX))

#include "ezsp-enum.h"
#include "secure-ezsp-protocol.h"

//------------------------------------------------------------------------------
// Frame Control Lower Byte (LB) Definitions

// The high bit of the frame control lower byte indicates the direction of the
// message. Commands are sent from the Host to the EM260. Responses are sent
// from the EM260 to the Host.
#define EZSP_FRAME_CONTROL_DIRECTION_MASK 0x80
#define EZSP_FRAME_CONTROL_COMMAND        0x00
#define EZSP_FRAME_CONTROL_RESPONSE       0x80

// Bits 5 and 6 of the frame control lower byte carry the network index the ezsp
// message is related to. The NCP upon processing an incoming EZSP command,
// temporary switches the current network to the one indicated in the EZSP
// frame control.
#define EZSP_FRAME_CONTROL_NETWORK_INDEX_MASK     0x60
#define EZSP_FRAME_CONTROL_NETWORK_INDEX_OFFSET   5

// Command Frame Control Fields

// The EM260 enters the sleep mode specified by the command frame control once
// it has sent its response.
#define EZSP_FRAME_CONTROL_SLEEP_MODE_MASK 0x03
#define EZSP_FRAME_CONTROL_IDLE            0x00 // Processor idle.
#define EZSP_FRAME_CONTROL_DEEP_SLEEP      0x01 // Wake on interrupt or timer.
#define EZSP_FRAME_CONTROL_POWER_DOWN      0x02 // Wake on interrupt only.
#define EZSP_FRAME_CONTROL_RESERVED_SLEEP  0x03 // Reserved

// Response Frame Control Fields

// The overflow flag in the response frame control indicates to the Host that
// one or more callbacks occurred since the previous response and there was not
// enough memory available to report them to the Host.
#define EZSP_FRAME_CONTROL_OVERFLOW_MASK  0x01
#define EZSP_FRAME_CONTROL_NO_OVERFLOW    0x00
#define EZSP_FRAME_CONTROL_OVERFLOW       0x01

// The truncated flag in the response frame control indicates to the Host that
// the response has been truncated. This will happen if there is not enough
// memory available to complete the response or if the response would have
// exceeded the maximum EZSP frame length.
#define EZSP_FRAME_CONTROL_TRUNCATED_MASK 0x02
#define EZSP_FRAME_CONTROL_NOT_TRUNCATED  0x00
#define EZSP_FRAME_CONTROL_TRUNCATED      0x02

// The pending callbacks flag in the response frame control lower byte indicates
// to the Host that there is at least one callback ready to be read. This flag is
// clear if the response to a callback command read the last pending callback.
#define EZSP_FRAME_CONTROL_PENDING_CB_MASK 0x04
#define EZSP_FRAME_CONTROL_PENDING_CB      0x04
#define EZSP_FRAME_CONTROL_NO_PENDING_CB   0x00

// The synchronous callback flag in the response frame control lower byte indicates
// this ezsp frame is the response to an ezspCallback().
#define EZSP_FRAME_CONTROL_SYNCH_CB_MASK  0x08
#define EZSP_FRAME_CONTROL_SYNCH_CB       0x08
#define EZSP_FRAME_CONTROL_NOT_SYNCH_CB   0x00

// The asynchronous callback flag in the response frame control lower byte indicates
// this ezsp frame is a callback sent asynchronously by the ncp. This flag may
// be set only in the uart version when EZSP_VALUE_UART_SYNCH_CALLBACKS is 0.
#define EZSP_FRAME_CONTROL_ASYNCH_CB_MASK 0x10
#define EZSP_FRAME_CONTROL_ASYNCH_CB      0x10
#define EZSP_FRAME_CONTROL_NOT_ASYNCH_CB  0x00

//------------------------------------------------------------------------------
// Frame Control Higher Byte (HB) Definitions

// Bit 7 of the frame control higher byte indicates whether security is
// enabled or not.
#define EZSP_EXTENDED_FRAME_CONTROL_SECURITY_MASK  0x80
#define EZSP_EXTENDED_FRAME_CONTROL_SECURE         0x80
#define EZSP_EXTENDED_FRAME_CONTROL_UNSECURE       0x00

// Bit 6 of the frame control higher byte indicates whether padding is
// enabled or not.
#define EZSP_EXTENDED_FRAME_CONTROL_PADDING_MASK   0x40
#define EZSP_EXTENDED_FRAME_CONTROL_PADDED         0x40
#define EZSP_EXTENDED_FRAME_CONTROL_UNPADDED       0x00

// Bits 0 and 1 of the frame control higher byte indicates the
// frame format version.
#define EZSP_EXTENDED_FRAME_FORMAT_VERSION_MASK    0x03
#define EZSP_EXTENDED_FRAME_FORMAT_VERSION         0x01

// Reserved bits 2-5
#define EZSP_EXTENDED_FRAME_CONTROL_RESERVED_MASK  0x3C

#endif // __EZSP_PROTOCOL_H__
