/**
 * @file
 * @brief Defines the procedures to make primes with the BA414EP
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_PRIME_ALG_H
#define SX_PRIME_ALG_H

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
/**
 * @brief Converge a given number to a prime number
 * @details This algorithm tries to converge to a prime number from a given random number.
 * First try to find a small divider (seiving), then uses the rabin miller if no small divider is found.
 * If nothing is found, the number is assumed prime. If not prime, then adds 2 to the number and re-check it.
 * @param number a pointer to the given random number
 * @param nbr_of_bytes number of bytes of the number
 * @param mr_rounds number of round for the Rabin-Miller algorithm
 * @return 0 if successful, 1 if malloc error, 2 if unable to converge to prime (request for new random)
 */
uint32_t converge_to_prime(uint8_t *number, uint32_t nbr_of_bytes, uint32_t mr_rounds) CHECK_RESULT;

/**
 * @brief Generate a prime from a random number
 * @param len the size of the number in bits
 * @param mr_rounds number of round for the Rabin-Miller algorithm
 * @param prime pointer to the resulting prime buffer
 * @return 0 if succesfull
 */
uint32_t generate_prime(uint32_t len, uint32_t mr_rounds, uint8_t *prime) CHECK_RESULT;


/**
 * Test primality of a given number of length len, using mr_rounds for rabin miller.
 * @param  number    Buffer to the number to check
 * @param  len       Length in bytes of the buffer containing the number
 * @param  mr_rounds Amount of rabin miller rounds to use
 * @return           Returns 1 if prime, 0 otherwize, > 1 if error
 */
uint32_t isPrime(uint8_t *number, uint32_t len, uint32_t mr_rounds);


#if (RSA_ENABLED)

/**
 * @brief Generate an asymmetric key for RSA
 * @param nbr_of_bytes number of required bytes for the key
 * @param exponent exponent needed for RSA priv key generation
 * @param key buffer containing the generated key
 * @param pub key needed ?
 * @param crt key generation ?
 * @return 0 if succesfull
 */
uint32_t get_rsa_key(uint32_t nbr_of_bytes, block_t exponent, block_t key, uint32_t pub, uint32_t crt) CHECK_RESULT;

#endif


#endif
