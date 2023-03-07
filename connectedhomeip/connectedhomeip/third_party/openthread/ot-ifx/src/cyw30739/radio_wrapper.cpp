/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/**
 * @file
 *   This file implements the wrapper utilities used between openthread stack and radio Platform
 *   Abstraction Layer.
 */

#include <common/as_core_type.hpp>
#include <common/instance.hpp>
#include <openthread/instance.h>

namespace ot {
namespace Mac {

class RadioWrapperMacFrame : public Frame
{
public:
    uint8_t security_header_index_get(void) { return FindSecurityHeaderIndex(); };
    uint8_t payload_index_get(void) { return FindPayloadIndex(); };
};

} // namespace Mac
} // namespace ot

namespace ot {

/*
 * Get start index of frame auxiliary security header.
 */
extern "C" uint8_t radio_wrapper_mac_frame_security_header_idx_get(otRadioFrame *aFrame)
{
    return static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->security_header_index_get();
}

/*
 * Get length of frame auxiliary security header.
 */
extern "C" uint8_t radio_wrapper_mac_frame_security_header_length_get(otRadioFrame *aFrame)
{
    return (static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->GetHeaderLength() -
            static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->security_header_index_get());
}

/*
 * Get start index of frame payload.
 */
extern "C" uint8_t radio_wrapper_mac_frame_payload_index_get(otRadioFrame *aFrame)
{
    return static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->payload_index_get();
}

/*
 * Get lenght of frame payload.
 */
extern "C" uint16_t radio_wrapper_mac_frame_payload_length_get(otRadioFrame *aFrame)
{
    return static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->GetPayloadLength();
}

/*
 * Get key id mode in the frame.
 */
extern "C" uint8_t radio_wrapper_mac_frame_key_id_mode_get(otRadioFrame *aFrame)
{
    uint8_t key_id_mode = 0;

    static_cast<Mac::RadioWrapperMacFrame *>(aFrame)->GetKeyIdMode(key_id_mode);

    return (key_id_mode >> 3);
}

} // namespace ot
