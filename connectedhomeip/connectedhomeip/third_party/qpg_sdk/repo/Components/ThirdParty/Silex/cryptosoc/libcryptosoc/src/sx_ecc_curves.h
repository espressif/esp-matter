/**
 * @file
 * @brief This file contains the hard-coded curve parameters
 *          for common ECC
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_ECC_CURVES_H_
#define SX_ECC_CURVES_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_ecc_curves_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "cryptodma.h"

/**
 *  ECC point structure having an x and y coordinate
 */
typedef struct {
    uint8_t x[72];   // X coordinate (256-bit)
    uint8_t y[72];   // Y coordinate (256-bit)
} t_ecc_point;


/**
 *  ECC structure for ECC curve definition
 */
typedef struct sx_ecc_curve_t {
   block_t  params;
   uint32_t command;
   uint32_t bitsize;
} sx_ecc_curve_t;

/**
 * Domain parameters (curve, subgroup and generator) identifiers.
 *
 * Only curves over prime fields are supported.
 *
 * \warning This library does not support validation of arbitrary domain
 * parameters. Therefore, only well-known domain parameters from trusted
 * sources should be used. See mbedtls_ecp_group_load().
 */
typedef enum
{
    SX_ECP_DP_NONE = 0,
    SX_ECP_DP_SECP192R1,      /*!< 192-bits NIST curve  */
    SX_ECP_DP_SECP224R1,      /*!< 224-bits NIST curve  */
    SX_ECP_DP_SECP256R1,      /*!< 256-bits NIST curve  */
    SX_ECP_DP_SECP384R1,      /*!< 384-bits NIST curve  */
    SX_ECP_DP_SECP521R1,      /*!< 521-bits NIST curve  */
    SX_ECP_DP_BP256R1,        /*!< 256-bits Brainpool curve */
    SX_ECP_DP_BP384R1,        /*!< 384-bits Brainpool curve */
    SX_ECP_DP_BP512R1,        /*!< 512-bits Brainpool curve */
    SX_ECP_DP_CURVE25519,     /*!< Curve25519               */
    SX_ECP_DP_SECP192K1,      /*!< 192-bits "Koblitz" curve */
    SX_ECP_DP_SECP224K1,      /*!< 224-bits "Koblitz" curve */
    SX_ECP_DP_SECP256K1,      /*!< 256-bits "Koblitz" curve */
    SX_ECP_DP_CURVE448,       /*!< Curve448                 */
    SX_ECP_DP_SECB233,        /*!< 233-bits binary curve  */
} sx_ecp_group_id;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_ecc_curves.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_ecc_curves.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

const sx_ecc_curve_t * sx_find_ecp_curve(sx_ecp_group_id id);

/**
 * @brief Get size of \p curve in bits
 * @param curve is a pointer to a ::sx_ecc_curve_t structure describing the curve
 */
uint32_t sx_ecc_curve_bitsize(const sx_ecc_curve_t *curve);

/**
 * @brief Get size of \p curve in bytes
 * @param curve is a pointer to a ::sx_ecc_curve_t structure describing the curve
 */
uint32_t sx_ecc_curve_bytesize(const sx_ecc_curve_t *curve);

// extern const sx_ecc_curve_t sx_ecc_curve_p192;
// extern const sx_ecc_curve_t sx_ecc_curve_p224;
// extern const sx_ecc_curve_t sx_ecc_curve_p256;
// extern const sx_ecc_curve_t sx_ecc_curve_p384;
// extern const sx_ecc_curve_t sx_ecc_curve_p521;
// extern const sx_ecc_curve_t sx_ecc_curve_b233;
// extern const sx_ecc_curve_t sx_ecc_curve_curve25519;
// extern const sx_ecc_curve_t sx_ecc_curve_curve448;

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

/**
 * @brief same as sx_find_ecp_curve, but will guarantee that curves are fetched from FLASH
 * @param curve is a pointer to a ::sx_ecc_curve_t structure describing the curve
 */
const sx_ecc_curve_t * sx_find_ecp_curve_flash(sx_ecp_group_id id);

#endif //defined(GP_DIVERSITY_ROM_CODE)



#endif
