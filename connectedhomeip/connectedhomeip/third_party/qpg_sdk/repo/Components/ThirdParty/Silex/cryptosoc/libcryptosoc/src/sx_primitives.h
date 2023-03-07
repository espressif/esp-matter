/**
 * @file
 * @brief Defines the procedures to make primitive operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_primitives_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptodma.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_primitives.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_primitives.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/** @brief Modular Addition: C = A + B mod P
 *  @param a is a pointer to the first operand of the Addition (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Addition (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Addition (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular addition is finished. *result contains the result of the computation
 */
uint32_t modular_addition(uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Subtraction: C = A - B mod P
 *  @param a is a pointer to the first operand of the Subtraction (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Subtraction (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Subtraction (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular subtraction is finished. *result contains the result of the computation
 */
uint32_t modular_substraction(uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Multiplication: C = A * B mod P
 *  @param a is a pointer to the first operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular multiplication is finished. *result contains the result of the computation
 */
uint32_t modular_multiplication(uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Reduction: C = A mod P
 *  @param a is a pointer to the first operand of the Reduction (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Reduction (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular reduction is finished. *result contains the result of the computation
 */
uint32_t modular_reduction(uint8_t *a, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Division: C = A/B mod P
 *  @param a is a pointer to the first operand of the Division (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Division (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Division (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular division is finished. *result contains the result of the computation
 */
uint32_t modular_division(uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Inversion: C = 1/A mod P
 *  @param a is a pointer to the first operand of the Inversion (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the Inversion (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular inversion is finished. *result contains the result of the computation
 */
uint32_t modular_inversion(uint8_t *a, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Multiplication: C = A * B
 *  @param a is a pointer to the first operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when multiplication is finished. *result contains the result of the computation
 */
uint32_t multiplication(uint8_t *a, uint8_t *b, uint8_t *result, uint32_t  size) CHECK_RESULT;


/** @brief Modular Exponentiation: result =  msg.key mod modulo
 *  @param a is a pointer to the message of the modular exponentiation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the key of the modular exponentiation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the modular exponentiation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @return 0 when modular subtraction is finished. *result contains the result of the computation
 */

 uint32_t modular_exponentiation(uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size) CHECK_RESULT;

/** @brief Modular Exponentiation: C = A ^ B mod P using block_t operands
*  @param a is a pointer to the first operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
*  @param b is a pointer to the second operand of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
*  @param modulo is a pointer to the modulo of the Multiplication (this must point to a location in the System Memory, not in the CryptoRAM).
*  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
*  @param size represents the operand's size in bytes.
*  @return 0 when modular multiplication is finished. *result contains the result of the computation
*/
uint32_t modular_exponentiation_blk(block_t a, block_t b, block_t modulo, block_t result, uint32_t  size) CHECK_RESULT;


/** @brief Generic function to load the Public Key to compute primitive arithmetic operations.
 *  It is reserved for internal use and SHOULD NOT be used via the API as it can corrupt the CryptoRAM.
 *  @param op is the [6:0] bits of the CommandReg register, describing the operation asked to the pub key
 *  @param a is a pointer to the first operand of the operation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the operation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the operation (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the operand's size in bytes.
 *  @param PtrA is a memory location of the CryptoRAM where the first operand has to be copied
 *  @param PtrB is a memory location of the CryptoRAM where the second operand has to be copied
 *  @param PtrC is a memory location of the CryptoRAM from where the result has to be copied
 *  @param PtrN is a memory location of the CryptoRAM from where the modulo has to be copied
 *  @return 0 when operation is finished. *result contains the result of the computation
 */
uint32_t primitive_operation(uint32_t op, uint8_t *a, uint8_t *b, uint8_t *modulo, uint8_t *result, uint32_t  size, uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN) CHECK_RESULT;


/** @brief Montgomery point multiplication. resulst = scalar
 *  @param curve domain of the curve to use
 *  @param curve_flags flags of the curve to use
 *  @param size curve size in bytes
 *  @param scalar input
 *  @param point input. If set to ::NULL_blk, use generator p1
 *  @param result output
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_montgomery_mult(block_t curve, uint32_t curve_flags, uint32_t size, block_t scalar, block_t point, block_t result) CHECK_RESULT;

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
