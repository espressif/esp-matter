/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BSV_INTEGRATION_OTP_H_
#define _BSV_INTEGRATION_OTP_H_

#include <stdint.h>
#include "run_integration_test.h"
#include "test_proj_otp.h"

#define RUNIT_ENV_OTP_START_OFFSET           0x2000UL
#define RUNIT_TEST_OTP_SIZE_IN_WORDS         0x2C
#define RUNIT_MAX_OTP_SIZE_IN_WORDS          0x7FF

/**
 * @brief               Retrieves the current life cycles based on the OTP values.
 *
 * @param lcs           [output] the returned LCS.
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_getLcs(unsigned int *lcs);

/**
 * @brief               Verifies that the current life cycle is lcs
 *
 * @param lcs           [input] the life cycle to expect
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_checkLcs(unsigned int lcs);

/**
 * @brief               Sets the appropriate fields on the OTP buffer to correspond with a specific Kceicv
 *
 * @param otp           [output] The OTP image to alter
 * @param kceicvBuff    [input] the Kceicv to use
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setKceicvInOtpBuff(unsigned int *otp, unsigned int *kceicvBuff);

/**
 * @brief               Sets the appropriate fields on the OTP buffer to correspond with a specific Kce
 *
 * @param otp           [output] The OTP image to alter
 * @param kceBuff       [input] the Kce to use
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setKceInOtpBuff(unsigned int *otp, unsigned int *kceBuff);

/**
 * @brief               Assign a new Kicv key into opt image. Also adjusts all the needed validation fields.
 *
 * @param otp           [ouput] The image to burn to NVM
 * @param kpicvBuff     [input] New Kicv to use
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setKpicvInOtpBuff(unsigned int *otp, unsigned int *kpicvBuff);

/**
 * @brief               Assign a new Kcp key into opt image. Also adjusts all the needed validation fields.
 *
 * @param otp           [ouput] The image to burn to NVM
 * @param kpicvBuff     [input] New Kcp to use
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setKpcInOtpBuff(unsigned int *otp, unsigned int *kcpBuff);

/**
 # @brief               Sets the appropriate fields on the OTP buffer to correspond with a specific Hbk

* @param otp            [output] The OTP image to alter
 * @param hbkBuff       [input] The Hbk to use. 32 bytes of key.
 *                      in case Hbk0 4 lower words will be used
 *                      in case Hbk1 4 upper words will be used
 *                      in case full Hbk all 8 words will be used
 * @param isFullHbk     [input] Defines in which manner to write the hbkBuff to otp
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setHbkOtpBuff(uint32_t *otp, uint32_t *hbkBuff, uint8_t isFullHbk);

/**
 # @brief               Sets the appropriate fields on the OTP buffer to correspond with a specific SW version

* @param otp            [output] The OTP image to alter
 * @param isFullHbk     [input] Defines in which manner to write the hbkBuff to otp
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_setSwVerInOtpBuff(uint32_t *otp, uint8_t isFullHbk);

/**
 * @brief               read OTP image
 *
 * @param otpBuf        [output] The image read from
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_printOtp(uint32_t otpBuf[TEST_OTP_SIZE_IN_WORDS]);

/**
 * @brief               Burn the byte array in otpBuf to otp.
 *                      nextLcs is provided to perform sanity check after burn and powerOnReset.
 *
 * @param otpBuf        [input] The image to burn to NVM
 * @param nextLcs       [input] The expected life cycle after boot
 *
 * @return              RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_burnOtp(unsigned int *otpBuf, unsigned int nextLcs);


#endif //_BSV_INTEGRATION_OTP_H_
