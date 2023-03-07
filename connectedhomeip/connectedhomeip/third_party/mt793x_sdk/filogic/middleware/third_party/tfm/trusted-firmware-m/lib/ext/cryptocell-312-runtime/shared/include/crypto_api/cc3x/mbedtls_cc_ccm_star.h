/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_aesccm_star
 @{
 */

/*!
 @file

 @brief This file contains the CryptoCell AES-CCM star APIs, their enums and
 definitions.

 This API supports AES-CCM*, as defined in <em>IEEE 802.15.4: IEEE Standard
 for Local and metropolitan area networks— Part 15.4: Low-Rate Wireless
 Personal Area Networks (LR-WPANs)</em>, with the instantiations defined in
 section B.3.2, and the nonce defined in section 7.3.2.
 */


#ifndef _MBEDTLS_AES_CCM_STAR_H
#define _MBEDTLS_AES_CCM_STAR_H


#include "cc_pal_types.h"
#include "cc_error.h"

#include "mbedtls/ccm.h"
#include "mbedtls_ccm_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/************************ Typedefs  ****************************/

/************************ Enums ********************************/

/************************ Structs  ******************************/

/************************ context Structs  ******************************/

/*!
 @brief     This function receives the MAC source address, the frame counter,
            and the MAC size, and returns the required nonce for AES-CCM*, as
            defined in <em>IEEE 802.15.4: IEEE Standard for Local and
            metropolitan area networks— Part 15.4: Low-Rate Wireless Personal
            Area Networks (LR-WPANs)</em>.

 @note      This API should be called before mbedtls_ccm_star_encrypt_and_tag()
            or mbedtls_ccm_star_auth_decrypt(). The generated nonce should
            be provided to these functions. \par

 @return              \c zero on success.
 @return              A non-zero value on failure, as defined in ccm.h.
 */
int mbedtls_ccm_star_nonce_generate(
        /*! The MAC address in EUI-64 format. */
        unsigned char * src_addr,
        /*! The MAC frame counter. */
        uint32_t frame_counter,
        /*! The size of the AES-CCM* MAC tag in bytes:
            4, 6, 8, 10, 12, 14 or 16. */
        uint8_t size_of_t,
        /*! The required nonce for AES-CCM*. */
        unsigned char * nonce_buf);


 #ifdef __cplusplus
}
#endif

#endif /* _MBEDTLS_AES_CCM_STAR_H */

/*!
@}
 */
