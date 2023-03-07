#ifndef BL_CRYPTO_API_H
#define BL_CRYPTO_API_H

#include <stdio.h>
#include <stdint.h>
#include "blcrypto_suite/blcrypto_suite_aes.h"

/**
 * struct crypto_bignum - bignum
 *
 * Internal data structure for bignum implementation. The contents is specific
 * to the used crypto library.
 */
struct crypto_bignum;

/**
 * struct crypto_ec - Elliptic curve context
 *
 * Internal data structure for EC implementation. The contents is specific
 * to the used crypto library.
 */
struct crypto_ec;

/**
 * struct crypto_ec_point - Elliptic curve point
 *
 * Internal data structure for EC implementation to represent a point. The
 * contents is specific to the used crypto library.
 */
struct crypto_ec_point;

struct crypto_aes;

struct bl_crypto_api {
    /**
     * crypto_bignum_init - Allocate memory for bignum
     * Returns: Pointer to allocated bignum or %NULL on failure
     */
    struct crypto_bignum * (*crypto_bignum_init)(void);

    /**
     * crypto_bignum_init_set - Allocate memory for bignum and set the value
     * @buf: Buffer with unsigned binary value
     * @len: Length of buf in octets
     * Returns: Pointer to allocated bignum or %NULL on failure
     */
    struct crypto_bignum * (*crypto_bignum_init_set)(const uint8_t *buf, size_t len);

    /**
     * crypto_bignum_deinit - Free bignum
     * @n: Bignum from crypto_bignum_init() or crypto_bignum_init_set()
     * @clear: Whether to clear the value from memory
     */
    void (*crypto_bignum_deinit)(struct crypto_bignum *n, int clear);

    /**
     * crypto_bignum_to_bin - Set binary buffer to unsigned bignum
     * @a: Bignum
     * @buf: Buffer for the binary number
     * @len: Length of @buf in octets
     * @padlen: Length in octets to pad the result to or 0 to indicate no padding
     * Returns: Number of octets written on success, -1 on failure
     */
    int (*crypto_bignum_to_bin)(const struct crypto_bignum *a,
            uint8_t *buf, size_t buflen, size_t padlen);

    /**
     * crypto_bignum_add - c = a + b
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result of a + b
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_add)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            struct crypto_bignum *c);

    /**
     * crypto_bignum_mod - c = a % b
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result of a % b
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_mod)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            struct crypto_bignum *c);

    /**
     * crypto_bignum_exptmod - Modular exponentiation: d = a^b (mod c)
     * @a: Bignum; base
     * @b: Bignum; exponent
     * @c: Bignum; modulus
     * @d: Bignum; used to store the result of a^b (mod c)
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_exptmod)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            const struct crypto_bignum *c,
            struct crypto_bignum *d);

    /**
     * crypto_bignum_inverse - Inverse a bignum so that a * c = 1 (mod b)
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_inverse)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            struct crypto_bignum *c);

    /**
     * crypto_bignum_sub - c = a - b
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result of a - b
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_sub)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            struct crypto_bignum *c);

    /**
     * crypto_bignum_div - c = a / b
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result of a / b
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_div)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            struct crypto_bignum *c);

    /**
     * crypto_bignum_mulmod - d = a * b (mod c)
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum
     * @d: Bignum; used to store the result of (a * b) % c
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_bignum_mulmod)(const struct crypto_bignum *a,
            const struct crypto_bignum *b,
            const struct crypto_bignum *c,
            struct crypto_bignum *d);

    /**
     * crypto_bignum_cmp - Compare two bignums
     * @a: Bignum
     * @b: Bignum
     * Returns: -1 if a < b, 0 if a == b, or 1 if a > b
     */
    int (*crypto_bignum_cmp)(const struct crypto_bignum *a,
            const struct crypto_bignum *b);

    /**
     * crypto_bignum_bits - Get size of a bignum in bits
     * @a: Bignum
     * Returns: Number of bits in the bignum
     */
    int (*crypto_bignum_bits)(const struct crypto_bignum *a);

    /**
     * crypto_bignum_is_zero - Is the given bignum zero
     * @a: Bignum
     * Returns: 1 if @a is zero or 0 if not
     */
    int (*crypto_bignum_is_zero)(const struct crypto_bignum *a);

    /**
     * crypto_bignum_is_one - Is the given bignum one
     * @a: Bignum
     * Returns: 1 if @a is one or 0 if not
     */
    int (*crypto_bignum_is_one)(const struct crypto_bignum *a);

    /**
     * crypto_bignum_legendre - Compute the Legendre symbol (a/p)
     * @a: Bignum
     * @p: Bignum
     * Returns: Legendre symbol -1,0,1 on success; -2 on calculation failure
     */
    int (*crypto_bignum_legendre)(const struct crypto_bignum *a,
            const struct crypto_bignum *p);

    /**
     * crypto_ec_init - Initialize elliptic curve context
     * @group: Identifying number for the ECC group (IANA "Group Description"
     *  attribute registrty for RFC 2409)
     * Returns: Pointer to EC context or %NULL on failure
     */
    struct crypto_ec * (*crypto_ec_init)(int group);

    /**
     * crypto_ec_deinit - Deinitialize elliptic curve context
     * @e: EC context from crypto_ec_init()
     */
    void (*crypto_ec_deinit)(struct crypto_ec *e);

    /**
     * crypto_ec_prime_len - Get length of the prime in octets
     * @e: EC context from crypto_ec_init()
     * Returns: Length of the prime defining the group
     */
    size_t (*crypto_ec_prime_len)(struct crypto_ec *e);

    /**
     * crypto_ec_prime_len_bits - Get length of the prime in bits
     * @e: EC context from crypto_ec_init()
     * Returns: Length of the prime defining the group in bits
     */
    size_t (*crypto_ec_prime_len_bits)(struct crypto_ec *e);

    /**
     * crypto_ec_get_prime - Get prime defining an EC group
     * @e: EC context from crypto_ec_init()
     * Returns: Prime (bignum) defining the group
     */
    const struct crypto_bignum * (*crypto_ec_get_prime)(struct crypto_ec *e);

    /**
     * crypto_ec_get_order - Get order of an EC group
     * @e: EC context from crypto_ec_init()
     * Returns: Order (bignum) of the group
     */
    const struct crypto_bignum * (*crypto_ec_get_order)(struct crypto_ec *e);

    /**
     * crypto_ec_point_init - Initialize data for an EC point
     * @e: EC context from crypto_ec_init()
     * Returns: Pointer to EC point data or %NULL on failure
     */
    struct crypto_ec_point * (*crypto_ec_point_init)(struct crypto_ec *e);

    /**
     * crypto_ec_point_deinit - Deinitialize EC point data
     * @p: EC point data from crypto_ec_point_init()
     * @clear: Whether to clear the EC point value from memory
     */
    void (*crypto_ec_point_deinit)(struct crypto_ec_point *p, int clear);

    /**
     * crypto_ec_point_to_bin - Write EC point value as binary data
     * @e: EC context from crypto_ec_init()
     * @p: EC point data from crypto_ec_point_init()
     * @x: Buffer for writing the binary data for x coordinate or %NULL if not used
     * @y: Buffer for writing the binary data for y coordinate or %NULL if not used
     * Returns: 0 on success, -1 on failure
     *
     * This function can be used to write an EC point as binary data in a format
     * that has the x and y coordinates in big endian byte order fields padded to
     * the length of the prime defining the group.
     */
    int (*crypto_ec_point_to_bin)(struct crypto_ec *e,
            const struct crypto_ec_point *point, uint8_t *x, uint8_t *y);

    /**
     * crypto_ec_point_from_bin - Create EC point from binary data
     * @e: EC context from crypto_ec_init()
     * @val: Binary data to read the EC point from
     * Returns: Pointer to EC point data or %NULL on failure
     *
     * This function readers x and y coordinates of the EC point from the provided
     * buffer assuming the values are in big endian byte order with fields padded to
     * the length of the prime defining the group.
     */
    struct crypto_ec_point * (*crypto_ec_point_from_bin)(struct crypto_ec *e,
            const uint8_t *val);

    /**
     * crypto_bignum_add - c = a + b
     * @e: EC context from crypto_ec_init()
     * @a: Bignum
     * @b: Bignum
     * @c: Bignum; used to store the result of a + b
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_ec_point_add)(struct crypto_ec *e, const struct crypto_ec_point *a,
            const struct crypto_ec_point *b,
            struct crypto_ec_point *c);

    /**
     * crypto_bignum_mul - res = b * p
     * @e: EC context from crypto_ec_init()
     * @p: EC point
     * @b: Bignum
     * @res: EC point; used to store the result of b * p
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_ec_point_mul)(struct crypto_ec *e, const struct crypto_ec_point *p,
            const struct crypto_bignum *b,
            struct crypto_ec_point *res);

    /**
     * crypto_ec_point_invert - Compute inverse of an EC point
     * @e: EC context from crypto_ec_init()
     * @p: EC point to invert (and result of the operation)
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_ec_point_invert)(struct crypto_ec *e, struct crypto_ec_point *p);

    /**
     * crypto_ec_point_solve_y_coord - Solve y coordinate for an x coordinate
     * @e: EC context from crypto_ec_init()
     * @p: EC point to use for the returning the result
     * @x: x coordinate
     * @y_bit: y-bit (0 or 1) for selecting the y value to use
     * Returns: 0 on success, -1 on failure
     */
    int (*crypto_ec_point_solve_y_coord)(struct crypto_ec *e,
            struct crypto_ec_point *p,
            const struct crypto_bignum *x, int y_bit);

    /**
     * crypto_ec_point_compute_y_sqr - Compute y^2 = x^3 + ax + b
     * @e: EC context from crypto_ec_init()
     * @x: x coordinate
     * Returns: y^2 on success, %NULL failure
     */
    struct crypto_bignum *
    (*crypto_ec_point_compute_y_sqr)(struct crypto_ec *e,
            const struct crypto_bignum *x);

    /**
     * crypto_ec_point_is_at_infinity - Check whether EC point is neutral element
     * @e: EC context from crypto_ec_init()
     * @p: EC point
     * Returns: 1 if the specified EC point is the neutral element of the group or
     *   0 if not
     */
    int (*crypto_ec_point_is_at_infinity)(struct crypto_ec *e,
            const struct crypto_ec_point *p);

    /**
     * crypto_ec_point_is_on_curve - Check whether EC point is on curve
     * @e: EC context from crypto_ec_init()
     * @p: EC point
     * Returns: 1 if the specified EC point is on the curve or 0 if not
     */
    int (*crypto_ec_point_is_on_curve)(struct crypto_ec *e,
            const struct crypto_ec_point *p);

    /**
     * crypto_ec_point_cmp - Compare two EC points
     * @e: EC context from crypto_ec_init()
     * @a: EC point
     * @b: EC point
     * Returns: 0 on equal, non-zero otherwise
     */
    int (*crypto_ec_point_cmp)(const struct crypto_ec *e,
            const struct crypto_ec_point *a,
            const struct crypto_ec_point *b);
};

#endif /* BL_CRYPTO_API_H */
