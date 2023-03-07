/***************************************************************************//**
 * @file
 * @brief Silicon Labs Series-1 PSA Crypto TRNG driver functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sli_crypto_trng_driver.h"

#if defined(TRNG_PRESENT) \
  && (defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT))

#include <string.h>
#include "em_cmu.h"
#include "sl_assert.h"

#define SLI_TRNG_MAX_RETRIES 10

/**
 * \brief TRNG soft reset function
 *
 * This function is used as part of initializing and/or resetting the TRNG
 * peripheral. It assumes the peripheral is clocked, and will trigger a
 * 'soft reset', as well as wait (with a timeout) for the FIFO buffer to
 * fill up again. When the FIFO doesn't fill (due to startup tests failure)
 * the function will retry the reset sequence until passing or too many
 * retries failing.
 *
 * \retval PSA_SUCCESS if the TRNG came up successfully after reset,
 * \retval PSA_ERROR_HARDWARE_FAILURE if too many subsequent startup test
 *         failures are observed.
 */
static psa_status_t sli_crypto_trng_soft_reset(void)
{
  for (size_t i = 0; i < SLI_TRNG_MAX_RETRIES; i++) {
    uint32_t ctrl = TRNG0->CONTROL;

    ctrl |= TRNG_CONTROL_SOFTRESET;
    TRNG0->CONTROL = ctrl;
    ctrl &= ~TRNG_CONTROL_SOFTRESET;
    TRNG0->CONTROL = ctrl;

    // Wait for TRNG to complete startup tests and start filling the FIFO.
    size_t loop_cycles = CMU_ClockFreqGet(cmuClock_CORE) >> 10;
    for (size_t j = 0; (TRNG0->FIFOLEVEL == 0) && (j < loop_cycles); j++) {
      ;
    }

    if (TRNG0->FIFOLEVEL > 0) {
      return PSA_SUCCESS;
    }
  }

  // If softreset is not successful, turn off the TRNG enable bit to
  // signal that the next entropy fetch function needs to re-initialize
  // the TRNG peripheral.
  TRNG0->CONTROL &= ~TRNG_CONTROL_ENABLE;
  return PSA_ERROR_HARDWARE_FAILURE;
}

/**
 * \brief TRNG FIFO clearing function
 *
 * This function is used to clear the TRNG internal FIFO by reading and
 * discarding data as quickly as possible until the FIFO is empty.
 */
static void sli_crypto_trng_clear_fifo(void)
{
  volatile uint32_t val32;

  // Empty FIFO
  while (TRNG0->FIFOLEVEL) {
    val32 = TRNG0->FIFO;
    (void)val32;
  }
}

/**
 * \brief TRNG status check function
 *
 * This function is used as part of getting entropy from the TRNG peripheral.
 * It checks whether any alarms have gone off or failure conditions have
 * occurred, which may impact the entropy quality of the output, and deals
 * with the conditions accordingly.
 *
 * \note Since this function may interact with the TRNG peripheral, the amount
 *       of data available in the FIFO may have changed after calling this
 *       function. Callers should re-evaluate before retrieving more data from
 *       the FIFO.
 *
 * \retval PSA_SUCCESS if the TRNG did not flag any alarm or error conditions,
 *         or a non-critical alarm (not requiring discarding of already
 *         collected entropy) was triggered and dealt with without issue.
 * \retval PSA_ERROR_INSUFFICIENT_ENTROPY if a noise alarm or error condition
 *         was reported and subsequently handled, and requires erasure of the
 *         previously gathered entropy since the last time this function
 *         returned successfully. In this case, the gathered entropy should
 *         be discarded and a new attempt to collect entropy should be made.
 * \retval PSA_ERROR_HARDWARE_FAILURE if an attempt was made to reset the TRNG,
 *         but the attempt failed. This condition also requires erasure of the
 *         collected entropy since the last non-alarmed collection.
 */
static psa_status_t sli_crypto_trng_check_status(void)
{
  uint32_t status = TRNG0->STATUS;

  if ((status & (TRNG_STATUS_PREIF
                 | TRNG_STATUS_REPCOUNTIF
                 | TRNG_STATUS_APT64IF
                 | TRNG_STATUS_APT4096IF
                 | TRNG_STATUS_ALMIF)) == 0) {
    return PSA_SUCCESS;
  }

  if (status & TRNG_STATUS_PREIF) {
    // On a preliminary noise alarm we clear the FIFO and clear
    // the alarm. The preliminary noise alarm is not critical.
    status &= ~TRNG_STATUS_PREIF;
    TRNG0->STATUS = status;
    sli_crypto_trng_clear_fifo();
    return PSA_ERROR_INSUFFICIENT_ENTROPY;
  } else {
    // Clear alarm conditions by doing a TRNG soft reset.
    psa_status_t psa_status = sli_crypto_trng_soft_reset();
    if (psa_status != PSA_SUCCESS) {
      return psa_status;
    }

    // Report failure
    if ((status & TRNG_STATUS_REPCOUNTIF)
        || (status & TRNG_STATUS_APT64IF)
        || (status & TRNG_STATUS_APT4096IF)
        || (status & TRNG_STATUS_ALMIF)) {
      return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
  }

  return PSA_SUCCESS;
}

/**
 * \brief TRNG init function
 *
 * This function initialises the TRNG peripheral in order for it to generate
 * entropy.
 *
 * \note Since this function may interact with the TRNG peripheral, the amount
 *       of data available in the FIFO may have changed after calling this
 *       function. Callers should re-evaluate before retrieving more data from
 *       the FIFO.
 *
 * \retval PSA_SUCCESS if the peripheral was successfully initialised
 * \retval PSA_ERROR_HARDWARE_FAILURE if the peripheral failed to start
 */
static psa_status_t sli_crypto_trng_init(void)
{
  TRNG0->CONTROL = TRNG_CONTROL_ENABLE
                   | TRNG_CONTROL_REPCOUNTIEN
                   | TRNG_CONTROL_APT64IEN
                   | TRNG_CONTROL_APT4096IEN
                   | TRNG_CONTROL_PREIEN
                   | TRNG_CONTROL_ALMIEN;

  psa_status_t psa_status = sli_crypto_trng_soft_reset();
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Wait for FIFO to be filled before seeding the conditioning function
  while (TRNG0->FIFOLEVEL < 4u) ;

  TRNG0->KEY0 = TRNG0->FIFO;
  TRNG0->KEY1 = TRNG0->FIFO;
  TRNG0->KEY2 = TRNG0->FIFO;
  TRNG0->KEY3 = TRNG0->FIFO;

  return sli_crypto_trng_soft_reset();
}

/**
 * \brief TRNG FIFO reading function (byte-oriented)
 *
 * This function reads the requested amount of bytes from the TRNG output FIFO.
 *
 * \note This function expects the caller to assure that enough data is present
 *       in the TRNG FIFO before calling this function.
 *
 * \param[out] output   output buffer to contain the entropy
 * \param      len      Amount of bytes to read from the FIFO into the output
 */
static void sli_crypto_trng_read_chunk(unsigned char *output, size_t len)
{
  uint32_t tmp;
  EFM_ASSERT(len <= TRNG0->FIFOLEVEL * 4);

  // Read full words, optimise based on buffer alignment
  if (((uint32_t) output & 0x3UL) == 0) {
    // Word-aligned output buffer means we can read directly into the buffer
    while (len >= sizeof(tmp)) {
      *((uint32_t*)output) = TRNG0->FIFO;
      output += sizeof(tmp);
      len -= 4;
    }
  } else {
    // Byte-aligned buffer means we need to copy byte for byte
    while (len >= sizeof(tmp)) {
      tmp = TRNG0->FIFO;
      output[0] = (tmp >>  0) & 0xFF;
      output[1] = (tmp >>  8) & 0xFF;
      output[2] = (tmp >> 16) & 0xFF;
      output[3] = (tmp >> 24) & 0xFF;
      len -= 4;
      output += sizeof(tmp);
    }
  }

  // Handle the case where len is not a word multiple by reading the final bytes
  // from a temporary word buffer
  if (len > 0) {
    tmp = TRNG0->FIFO;
    while (len > 0) {
      output[0] = tmp & 0xFF;
      tmp >>= 8;
      output += 1;
      len -= 1;
    }
  }
}

/**
 * \brief TRNG entropy retrieval
 *
 * This function attempts to read the requested amount of conditioned entropy
 * from the TRNG peripheral.
 *
 * \param[out] output   output buffer to contain the entropy
 * \param      len      Amount of bytes to read from the FIFO into the output
 * \param[out] out_len  Amount of bytes actually written into the output buffer
 *
 * \retval PSA_SUCCESS All of the requested entropy was output into the buffer
 * \retval PSA_ERROR_INSUFFICIENT_ENTROPY A temporary error condition caused
 *           only a partial output (potentially 0), but retrying later should
 *           succeed.
 * \retval PSA_ERROR_HARDWARE_FAILURE An error condition with the TRNG entropy
 *           generation caused us to be unable to output any entropy.
 */
psa_status_t sli_crypto_trng_get_random(unsigned char *output,
                                        size_t len,
                                        size_t *out_len)
{
  unsigned char *outptr = output;

  size_t retries = SLI_TRNG_MAX_RETRIES;
  size_t requested_len = len;
  size_t output_len = 0;
  size_t count = 0;
  size_t available_entropy;
  psa_status_t ret = PSA_SUCCESS;

  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TRNG0;
  if ((TRNG0->CONTROL & TRNG_CONTROL_ENABLE) == 0u) {
    ret = sli_crypto_trng_init();
    if (ret != PSA_SUCCESS) {
      return ret;
    }
  }

  while (len > 0) {
    available_entropy = TRNG0->FIFOLEVEL * 4;
    if (available_entropy == 0) {
      // Wait a bit for the FIFO to fill back up if it went empty
      size_t loop_cycles = CMU_ClockFreqGet(cmuClock_CORE) >> 11;
      for (size_t i = 0; i < loop_cycles; i++) {
        if (TRNG0->FIFOLEVEL > 0) {
          break;
        }
      }

      // If it's still not come back up, return what we have
      if (TRNG0->FIFOLEVEL == 0) {
        ret = PSA_ERROR_INSUFFICIENT_ENTROPY;
        break;
      }
    }

    // Check status for current data in FIFO
    // and handle any error conditions.
    ret = sli_crypto_trng_check_status();
    if (ret == PSA_ERROR_HARDWARE_FAILURE) {
      // A hardware error cannot be recovered from in this loop
      break;
    }

    // The TRNG output data did not pass the internal TRNG random tests.
    // Retry up to SLI_TRNG_MAX_RETRIES times, by resetting the output
    // length counter and buffer pointer to their initial state.
    if (ret == PSA_ERROR_INSUFFICIENT_ENTROPY) {
      output_len = 0;
      outptr = output;
      len = requested_len;
      if (--retries == 0) {
        break;
      } else {
        continue;
      }
    }

    // Re-evaluate available bytes
    available_entropy = TRNG0->FIFOLEVEL * 4;

    // Read up to the amount of available bytes into the output buffer
    count = (len > available_entropy) ? available_entropy : len;
    sli_crypto_trng_read_chunk(outptr, count);
    outptr += count;
    output_len += count;
    len -= count;
  }

  // Discard all of the data in case of an error
  if (ret != PSA_SUCCESS && ret != PSA_ERROR_INSUFFICIENT_ENTROPY) {
    memset(output, 0, len);
    output_len = 0;
  }

  *out_len = output_len;
  return ret;
}

#endif // TRNG_PRESENT && (MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG || MBEDTLS_ENTROPY_HARDWARE_ALT)
