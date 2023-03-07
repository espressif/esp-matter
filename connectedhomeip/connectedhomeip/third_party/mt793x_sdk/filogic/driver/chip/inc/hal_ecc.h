/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2020. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */


#ifndef __HAL_ECC_H__
#define __HAL_ECC_H__


#define HAL_ECC_MODULE_ENABLED
#ifdef HAL_ECC_MODULE_ENABLED


#include <stdint.h>

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ECC
 * @{
 * This section introduces the Elliptic curve cryptography(ECC) APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, ECC function groups, enums, structures and functions.
 *
 * @section HAL_ECC_Terms_Chapter Terms and acronyms
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b ECC                  | Elliptic curve cryptography.|
 * |\b ECDSA                | Elliptic curve digital signature algorithm. |
 * |\b NIST                 | National institute of standards and technology. |
 *
 * @section HAL_ECC_Features_Chapter Supported features
 * The ECC has been designed to process signature and verification based on Elliptic curve digital signature algorithm.
 *
 * @section HAL_ECC_Driver_Usage_Chapter  How to use this driver
 * - Trigger ECC to verify the signarure. \n
 *  - Step1: Call hal_ecc_init() to initialize the ECC clock.
 *  - Step2: Call hal_ecc_ecdsa_sign() to generate the ECDSA signarure or hal_ecc_ecdsa_verify() to verify the ECDSA signarure.
 *  - Step3: Call hal_ecc_deinit() to de-initialize the ECC clock.
 *  - Sample code:
 *  @code
 *       // ECC need 32 bytes(32 * 8 bits) length when using NIST P-256 curve. Little endian format.
 *       // You can test sign/verify funtion by using the following golden data.
 *       // e: {0x0377BCC0, 0x26681592, 0x5F3CDF14, 0xC64E5D61, 0xC535C273, 0x637536F7, 0x19F5BF25, 0x1FDA2156}.
 *       // d: {0xA112ED54, 0xFDAF4EE1, 0x4DC4192F, 0x7C7A9947, 0xF013D563, 0x84335DD3, 0x3B51E0FC, 0xACEC122D}.
 *       // k: {0xFD11A53D, 0x0AEBFE6D, 0x3694C98E, 0xA3CE7B21, 0x8566A7E8, 0x2DEA7054, 0x1958A428, 0xC8BDD79F}.
 *       // r: {0xF345B5B5, 0x8926F457, 0xFDAB95A9, 0xBD362686, 0x253EB72A, 0xD33E3511, 0xB21737AE, 0x2F350F06}.
 *       // s: {0x5313B579, 0x814492C3, 0x135D7EF3, 0xA686FD6E, 0xCED6F8A5, 0x0749A6B2, 0x151E00C0, 0x338AE2FA}.
 *       // Qx:{0xC3E79B79, 0x8F335540, 0x684E285C, 0xAAAA74F1, 0x6AE6900E, 0x65455B8E, 0xE75F70CD, 0x5AF2E9D1}.
 *       // Qy:{0x3016AC86, 0x50FDF6D9, 0xB69BA98B, 0xC5EC1D8B, 0x9A296177, 0x32F97CCB, 0xD8565D9D, 0xEC52712F}.
 *
 *       uint32_t   e[8]; // input data
 *       uint32_t   d[8]; // input data
 *       uint32_t   k[8]; // input data
 *       uint32_t   r[8]; // output data for signature, input data for verification
 *       uint32_t   s[8]; // output data for signature, input data for verification
 *       uint32_t   v[8]; // output data
 *       uint32_t  Qx[8]; // input data
 *       uint32_t  Qy[8]; // input data
 *
 *       // Initializes the ECC clock.
 *       if(HAL_ECC_STATUS_OK != hal_ecc_init()) {
 *             //error handle
 *       }
 *       // Generate the ECDSA signarure based on NIST P-256 curve.
 *       if(HAL_ECC_STATUS_OK != hal_ecc_ecdsa_sign(HAL_ECC_CURVE_NIST_P_256, d, k, e, r, s)) {
 *             //error handle
 *       }
 *       // Verify the ECDSA signarure based on NIST P-256 curve.
 *       if(HAL_ECC_STATUS_OK != hal_ecc_ecdsa_verify(HAL_ECC_CURVE_NIST_P_256, r, s, e, Qx, Qy, v)) {
 *             //error handle
 *       }
 *       if(memcmp(v, r, 4 * 8)) {
 *             //error handle
 *       }
 *       // De-initialize the ECC clock.
 *       hal_ecc_deinit();
 *
 *  @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

/***********************************
 * Enum
 ***********************************/


/** @defgroup hal_ecc_enum Enum
  * @{
  */

/**
 *@brief This enum defines the HAL ECC curve ID.
 */
typedef enum {
    HAL_ECC_CURVE_NIST_P_192 = 0,   /**< in/out data length: 192 bits */
    HAL_ECC_CURVE_NIST_P_224 = 1,   /**< in/out data length: 224 bits */
    HAL_ECC_CURVE_NIST_P_256 = 2,   /**< in/out data length: 256 bits */
    HAL_ECC_CURVE_NIST_P_384 = 3,   /**< in/out data length: 384 bits */
    HAL_ECC_CURVE_NIST_P_521 = 4    /**< in/out data length: 521 bits */
} hal_ecc_curve_t;


/**
 *@brief This enum defines the HAL ECC interface return value.
 */
typedef enum {
    HAL_ECC_STATUS_HARDWARE_ERROR = -4,          /**< Hardware error happened */
    HAL_ECC_STATUS_INVALID_PARAMETER = -3,       /**< Invalid parameter */
    HAL_ECC_STATUS_OPERATION_NOT_SUPPORTED = -2, /**< operation not supported */
    HAL_ECC_CURVE_NOT_SUPPORTED = -1,            /**< ECC curve not supported */
    HAL_ECC_STATUS_OK = 0                        /**< ECC status ok */
} hal_ecc_status_t;
/**
  * @}
  */

/***********************************
 * Function
 ***********************************/


/**
 * @brief     This function initializes the ECC hardware clock.
 * @return    Indicates whether this function call is successful or not.
 *            If the return value is true, the operation completed successfully.
 *            If the return value is false, ECC can not be initialized.
 * @par       Example
 * Sample code, please refer to @ref HAL_ECC_Driver_Usage_Chapter.
 */
bool hal_ecc_init(void);


/**
 * @brief     This function de-initializes the ECC hardware clock.
 * @par       Example
 * Sample code, please refer to @ref HAL_ECC_Driver_Usage_Chapter.
 */
void hal_ecc_deinit(void);


/**
 * @brief Elliptic curve digital signature algorithm sign function.
 *
 * Calculate an ECDSA signature using elliptic curve digital signature
 * algorithm with the specified curve, private key, random number, and data.
 *
 * @param curve[in] An elliptic curve of choice. See hal_ecc_curve_t for more
 *                  information.
 * @param d[in]     The private key.
 * @param k[in]     The random data.
 * @param e[in]     The data which has already hashed to create a signature.
 * @param r[out]    The first part of a signature result.
 * @param s[out]    The second part of a signature result.
 *
 * @return          #HAL_ECC_STATUS_OK is returned if signature is generated
 *                  using the specified curve. Otherwise, see descriptions
 *                  in hal_ecc_status_t.
 *
 * @par       Example
 * Sample code, please refer to @ref HAL_ECC_Driver_Usage_Chapter.
 */
hal_ecc_status_t hal_ecc_ecdsa_sign(
    const   hal_ecc_curve_t     curve,
    const   uint32_t            *d,
    const   uint32_t            *k,
    const   uint32_t            *e,
    uint32_t            *r,
    uint32_t            *s);


/**
 * @brief Elliptic curve digital signature algorithm verify function.
 *
 * Verify an ECDSA signature using elliptic curve digital signature
 * algorithm with the specified curve, public key, signature, and data.
 *
 * @param curve     An elliptic curve of choice. See hal_ecc_curve_t for more
 *                  information.
 * @param r[in]     The first part of a signature result.
 * @param s[in]     The second part of a signature result.
 * @param Qx[in]    The first part of a public key.
 * @param Qy[in]    The second part of a public key.
 * @param e[in]     The data which has already hashed to verify a signature.
 * @param v[out]    The output of verification.
 *
 * @return          #HAL_ECC_STATUS_OK is returned if signature is generated
 *                  using the specified curve. Otherwise, see descriptions
 *                  in hal_ecc_status_t.
 *
 * @par       Example
 * Sample code, please refer to @ref HAL_ECC_Driver_Usage_Chapter.
 */
hal_ecc_status_t hal_ecc_ecdsa_verify(
    const   hal_ecc_curve_t     curve,
    const   uint32_t            *r,
    const   uint32_t            *s,
    const   uint32_t            *Qx,
    const   uint32_t            *Qy,
    const   uint32_t            *e,
    uint32_t            *v);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_ECC_MODULE_ENABLED */

#endif  /*!__HAL_ECC_H__ */

