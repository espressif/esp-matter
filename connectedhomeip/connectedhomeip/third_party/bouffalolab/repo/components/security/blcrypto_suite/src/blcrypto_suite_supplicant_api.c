#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include <FreeRTOS.h>
#include <bl_crypto_api.h>
#include <blcrypto_suite/blcrypto_suite_aes.h>
#include <blcrypto_suite/blcrypto_suite_bignum.h>
#include <blcrypto_suite/blcrypto_suite_ecp.h>
#include <blcrypto_suite/blcrypto_suite_hacc_glue.h>
#include <blcrypto_suite/blcrypto_suite_supplicant_api.h>
#include <string.h>

#define os_mem_alloc pvPortMalloc
#define os_mem_free vPortFree

#define IANA_SECP256R1 19

struct crypto_bignum *crypto_bignum_init(void)
{
    blcrypto_suite_mpi *bn = os_mem_alloc(sizeof(blcrypto_suite_mpi));
    if (bn == NULL) {
        return NULL;
    }
    memset((void *)bn, 0, sizeof(blcrypto_suite_mpi));

    blcrypto_suite_mpi_init(bn);

    return (struct crypto_bignum *)bn;
}

struct crypto_bignum *crypto_bignum_init_set(const uint8_t *buf, size_t len)
{
    int ret = 0;
    blcrypto_suite_mpi *bn = (blcrypto_suite_mpi *)crypto_bignum_init();
    if (bn == NULL) {
        return NULL;
    }
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_read_binary(bn, buf, len));
    return (struct crypto_bignum *)bn;
cleanup:
    crypto_bignum_deinit((struct crypto_bignum *)bn, 0);
    return NULL;
}

void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
    blcrypto_suite_mpi_free((blcrypto_suite_mpi *)n);
    os_mem_free((blcrypto_suite_mpi *)n);
}

int crypto_bignum_read_binary(struct crypto_bignum *X, const uint8_t *buf, size_t buflen)
{
    return blcrypto_suite_mpi_read_binary((blcrypto_suite_mpi *)X, buf, buflen);
}

int crypto_bignum_write_binary(const struct crypto_bignum *X, uint8_t *buf, size_t buflen)
{
    return blcrypto_suite_mpi_write_binary((blcrypto_suite_mpi *)X, buf, buflen);
}

int crypto_bignum_to_bin(const struct crypto_bignum *a,
                         uint8_t *buf, size_t buflen, size_t padlen)
{
    int num_bytes, offset;

    if (padlen > buflen) {
        return -1;
    }

    num_bytes = blcrypto_suite_mpi_size((blcrypto_suite_mpi *)a);

    if ((size_t)num_bytes > buflen) {
        return -1;
    }
    if (padlen > (size_t)num_bytes) {
        offset = padlen - num_bytes;
    } else {
        offset = 0;
    }

    memset(buf, 0, offset);
    blcrypto_suite_mpi_write_binary((blcrypto_suite_mpi *)a, buf + offset, blcrypto_suite_mpi_size((blcrypto_suite_mpi *)a));

    return num_bytes + offset;
}

int crypto_bignum_add(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return blcrypto_suite_mpi_add_mpi((blcrypto_suite_mpi *)c, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b) ? -1 : 0;
}

int crypto_bignum_mod(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return blcrypto_suite_mpi_mod_mpi((blcrypto_suite_mpi *)c, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b) ? -1 : 0;
}

int crypto_bignum_exptmod(const struct crypto_bignum *a,
                          const struct crypto_bignum *b,
                          const struct crypto_bignum *c,
                          struct crypto_bignum *d)
{
    return blcrypto_suite_mpi_exp_mod((blcrypto_suite_mpi *)d, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b, (const blcrypto_suite_mpi *)c, NULL) ? -1 : 0;
}

int crypto_bignum_inverse(const struct crypto_bignum *a,
                          const struct crypto_bignum *b,
                          struct crypto_bignum *c)
{
    return blcrypto_suite_mpi_inv_mod((blcrypto_suite_mpi *)c, (const blcrypto_suite_mpi *)a,
                                      (const blcrypto_suite_mpi *)b)
               ? -1
               : 0;
}

int crypto_bignum_sub(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return blcrypto_suite_mpi_sub_mpi((blcrypto_suite_mpi *)c, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b) ? -1 : 0;
}

int crypto_bignum_div(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return blcrypto_suite_mpi_div_mpi((blcrypto_suite_mpi *)c, NULL, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b) ? -1 : 0;
}

int crypto_bignum_mulmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         const struct crypto_bignum *c,
                         struct crypto_bignum *d)
{
    int res;

    blcrypto_suite_mpi temp;
    blcrypto_suite_mpi_init(&temp);

    res = blcrypto_suite_mpi_mul_mpi(&temp, (const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b);
    if (res) {
        return -1;
    }

    res = blcrypto_suite_mpi_mod_mpi((blcrypto_suite_mpi *)d, &temp, (blcrypto_suite_mpi *)c);

    blcrypto_suite_mpi_free(&temp);

    return res ? -1 : 0;
}

int crypto_bignum_cmp(const struct crypto_bignum *a,
                      const struct crypto_bignum *b)
{
    return blcrypto_suite_mpi_cmp_mpi((const blcrypto_suite_mpi *)a, (const blcrypto_suite_mpi *)b);
}

int crypto_bignum_bits(const struct crypto_bignum *a)
{
    return blcrypto_suite_mpi_bitlen((const blcrypto_suite_mpi *)a);
}

int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
    return (blcrypto_suite_mpi_cmp_int((const blcrypto_suite_mpi *)a, 0) == 0);
}

int crypto_bignum_is_one(const struct crypto_bignum *a)
{
    return (blcrypto_suite_mpi_cmp_int((const blcrypto_suite_mpi *)a, 1) == 0);
}

int crypto_bignum_legendre(const struct crypto_bignum *a,
                           const struct crypto_bignum *p)
{
    blcrypto_suite_mpi exp, tmp;
    int res = -2, ret;

    blcrypto_suite_mpi_init(&exp);
    blcrypto_suite_mpi_init(&tmp);

    /* exp = (p-1) / 2 */
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_sub_int(&exp, (const blcrypto_suite_mpi *)p, 1));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_shift_r(&exp, 1));
#if USE_HWCRYPTO
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_hacc_mpi_exp_mod_secp256r1(&tmp, (const blcrypto_suite_mpi *)a, &exp));
#else
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_exp_mod(&tmp, (const blcrypto_suite_mpi *)a, &exp, (const blcrypto_suite_mpi *)p, NULL));
#endif

    if (blcrypto_suite_mpi_cmp_int(&tmp, 1) == 0) {
        res = 1;
    } else if (blcrypto_suite_mpi_cmp_int(&tmp, 0) == 0
               /* The below check is workaround for the case where HW
             * does not behave properly for X ^ A mod M when X is
             * power of M. Instead of returning value 0, value M is
             * returned.*/
               || blcrypto_suite_mpi_cmp_mpi(&tmp, (const blcrypto_suite_mpi *)p) == 0) {
        res = 0;
    } else {
        res = -1;
    }

cleanup:
    blcrypto_suite_mpi_free(&tmp);
    blcrypto_suite_mpi_free(&exp);
    return res;
}

struct crypto_ec {
    blcrypto_suite_ecp_group group;
};

struct crypto_ec *crypto_ec_init(int group)
{
    struct crypto_ec *e;

    blcrypto_suite_ecp_group_id grp_id;

    /* IANA registry to mbedtls internal mapping*/
    switch (group) {
        case IANA_SECP256R1:
            /* For now just support NIST-P256.
         * This is of type "short Weierstrass".
         */
            grp_id = BLCRYPTO_SUITE_ECP_DP_SECP256R1;
            break;
        default:
            return NULL;
    }
    e = os_mem_alloc(sizeof(*e));
    if (e == NULL) {
        return NULL;
    }
    memset((void *)e, 0, sizeof(*e));

    blcrypto_suite_ecp_group_init(&e->group);

    if (blcrypto_suite_ecp_group_load(&e->group, grp_id)) {
        crypto_ec_deinit(e);
        e = NULL;
    }

    return e;
}

void crypto_ec_deinit(struct crypto_ec *e)
{
    if (e == NULL) {
        return;
    }

    blcrypto_suite_ecp_group_free(&e->group);
    os_mem_free(e);
}

struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
    blcrypto_suite_ecp_point *pt;
    if (e == NULL) {
        return NULL;
    }

    pt = os_mem_alloc(sizeof(blcrypto_suite_ecp_point));

    if (pt == NULL) {
        return NULL;
    }

    memset((void *)pt, 0, sizeof(blcrypto_suite_ecp_point));

    blcrypto_suite_ecp_point_init(pt);

    return (struct crypto_ec_point *)pt;
}

size_t crypto_ec_prime_len(struct crypto_ec *e)
{
    return blcrypto_suite_mpi_size(&e->group.P);
}

size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
    return blcrypto_suite_mpi_bitlen(&e->group.P);
}

const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
    return (const struct crypto_bignum *)&e->group.P;
}

const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
    return (const struct crypto_bignum *)&e->group.N;
}

void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
    blcrypto_suite_ecp_point_free((blcrypto_suite_ecp_point *)p);
    os_mem_free(p);
}

int crypto_ec_point_to_bin(struct crypto_ec *e,
                           const struct crypto_ec_point *point, uint8_t *x, uint8_t *y)
{
    int len = blcrypto_suite_mpi_size(&e->group.P);

    if (x) {
        if (crypto_bignum_to_bin((struct crypto_bignum *)&((blcrypto_suite_ecp_point *)point)->X,
                                 x, len, len) < 0) {
            return -1;
        }
    }

    if (y) {
        if (crypto_bignum_to_bin((struct crypto_bignum *)&((blcrypto_suite_ecp_point *)point)->Y,
                                 y, len, len) < 0) {
            return -1;
        }
    }

    return 0;
}

struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
                                                 const uint8_t *val)
{
    blcrypto_suite_ecp_point *pt;
    int len, ret;

    if (e == NULL) {
        return NULL;
    }

    len = blcrypto_suite_mpi_size(&e->group.P);

    pt = os_mem_alloc(sizeof(blcrypto_suite_ecp_point));

    if (pt == NULL) {
        return NULL;
    }
    memset((void *)pt, 0, sizeof(blcrypto_suite_ecp_point));

    blcrypto_suite_ecp_point_init(pt);

    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_read_binary(&pt->X, val, len));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_read_binary(&pt->Y, val + len, len));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset((&pt->Z), 1));

    return (struct crypto_ec_point *)pt;

cleanup:
    blcrypto_suite_ecp_point_free(pt);
    os_mem_free(pt);
    return NULL;
}

int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b,
                        struct crypto_ec_point *c)
{
    int ret;
    blcrypto_suite_mpi one;

    blcrypto_suite_mpi_init(&one);

    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset(&one, 1));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_ecp_muladd(&e->group, (blcrypto_suite_ecp_point *)c, &one, (const blcrypto_suite_ecp_point *)a, &one, (const blcrypto_suite_ecp_point *)b));

cleanup:
    blcrypto_suite_mpi_free(&one);
    return ret ? -1 : 0;
}

int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
#if USE_HWCRYPTO
    int ret;
    ret = blcrypto_suite_hacc_ecp_mul(&e->group,
                                      (blcrypto_suite_ecp_point *)res,
                                      (const blcrypto_suite_mpi *)b,
                                      (const blcrypto_suite_ecp_point *)p,
                                      NULL,
                                      NULL);
    return ret;
#else
    int ret;

    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_ecp_mul(&e->group,
                                                  (blcrypto_suite_ecp_point *)res,
                                                  (const blcrypto_suite_mpi *)b,
                                                  (const blcrypto_suite_ecp_point *)p,
                                                  NULL,
                                                  NULL));
cleanup:
    return ret ? -1 : 0;
#endif
}

/*  Currently mbedtls does not have any function for inverse
 *  This function calculates inverse of a point.
 *  Set R = -P
 */
static int ecp_opp(const blcrypto_suite_ecp_group *grp, blcrypto_suite_ecp_point *R, const blcrypto_suite_ecp_point *P)
{
    int ret = 0;

    /* Copy */
    if (R != P) {
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_ecp_copy(R, P));
    }

    /* In-place opposite */
    if (blcrypto_suite_mpi_cmp_int(&R->Y, 0) != 0) {
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_sub_mpi(&R->Y, &grp->P, &R->Y));
    }

cleanup:
    return (ret);
}

int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
    return ecp_opp(&e->group, (blcrypto_suite_ecp_point *)p, (blcrypto_suite_ecp_point *)p) ? -1 : 0;
}

int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x, int y_bit)
{
    blcrypto_suite_mpi temp;
    blcrypto_suite_mpi *y_sqr, *y;
    blcrypto_suite_mpi_init(&temp);
    int ret = 0;

    y = &((blcrypto_suite_ecp_point *)p)->Y;

    /* Faster way to find sqrt
     * Works only with curves having prime p
     * such that p ≡ 3 (mod 4)
     *  y_ = (y2 ^ ((p+1)/4)) mod p
     *
     *  if LSB of both x and y are same: y = y_
     *   else y = p - y_
     * y_bit is LSB of x
     */
    y_bit = (y_bit != 0);

    y_sqr = (blcrypto_suite_mpi *)crypto_ec_point_compute_y_sqr(e, x);

    if (y_sqr) {
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_add_int(&temp, &e->group.P, 1));
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_div_int(&temp, NULL, &temp, 4));
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_exp_mod(y, y_sqr, &temp, &e->group.P, NULL));

        if (y_bit != blcrypto_suite_mpi_get_bit(y, 0))
            BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_sub_mpi(y, &e->group.P, y));

        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_copy(&((blcrypto_suite_ecp_point *)p)->X, (const blcrypto_suite_mpi *)x));
        BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset(&((blcrypto_suite_ecp_point *)p)->Z, 1));
    } else {
        ret = 1;
    }
cleanup:
    blcrypto_suite_mpi_free(&temp);
    blcrypto_suite_mpi_free(y_sqr);
    os_mem_free(y_sqr);
    return ret ? -1 : 0;
}

struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
                              const struct crypto_bignum *x)
{
    blcrypto_suite_mpi temp, temp2, num;
    int ret = 0;

    blcrypto_suite_mpi *y_sqr = os_mem_alloc(sizeof(blcrypto_suite_mpi));
    if (y_sqr == NULL) {
        return NULL;
    }
    memset((void *)y_sqr, 0, sizeof(blcrypto_suite_mpi));

    blcrypto_suite_mpi_init(&temp);
    blcrypto_suite_mpi_init(&temp2);
    blcrypto_suite_mpi_init(&num);
    blcrypto_suite_mpi_init(y_sqr);

    /* y^2 = x^3 + ax + b  mod  P*/
    /* mbedtls does not have mod-add or mod-mul apis.
     *
     */

    /* Calculate x^3  mod P*/
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset(&num, 3));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_exp_mod(&temp, (const blcrypto_suite_mpi *)x, &num, &e->group.P, NULL));

    /* Calculate ax  mod P*/
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset(&num, -3));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_mul_mpi(&temp2, (const blcrypto_suite_mpi *)x, &num));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate ax + b  mod P. Note that b is already < P*/
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_add_mpi(&temp2, &temp2, &e->group.B));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate x^3 + ax + b  mod P*/
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_add_mpi(&temp2, &temp2, &temp));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_mod_mpi(y_sqr, &temp2, &e->group.P));

cleanup:
    blcrypto_suite_mpi_free(&temp);
    blcrypto_suite_mpi_free(&temp2);
    blcrypto_suite_mpi_free(&num);
    if (ret) {
        blcrypto_suite_mpi_free(y_sqr);
        os_mem_free(y_sqr);
        return NULL;
    } else {
        return (struct crypto_bignum *)y_sqr;
    }
}

int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
                                   const struct crypto_ec_point *p)
{
    return blcrypto_suite_ecp_is_zero((blcrypto_suite_ecp_point *)p);
}

int crypto_ec_point_is_on_curve(struct crypto_ec *e,
                                const struct crypto_ec_point *p)
{
    blcrypto_suite_mpi y_sqr_lhs, *y_sqr_rhs = NULL, two;
    int ret = 0, on_curve = 0;

    blcrypto_suite_mpi_init(&y_sqr_lhs);
    blcrypto_suite_mpi_init(&two);

    /* Calculate y^2  mod P*/
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_lset(&two, 2));
    BLCRYPTO_SUITE_MPI_CHK(blcrypto_suite_mpi_exp_mod(&y_sqr_lhs, &((const blcrypto_suite_ecp_point *)p)->Y, &two, &e->group.P, NULL));

    y_sqr_rhs = (blcrypto_suite_mpi *)crypto_ec_point_compute_y_sqr(e, (const struct crypto_bignum *)&((const blcrypto_suite_ecp_point *)p)->X);

    if (y_sqr_rhs && (blcrypto_suite_mpi_cmp_mpi(y_sqr_rhs, &y_sqr_lhs) == 0)) {
        on_curve = 1;
    }

cleanup:
    blcrypto_suite_mpi_free(&y_sqr_lhs);
    blcrypto_suite_mpi_free(&two);
    blcrypto_suite_mpi_free(y_sqr_rhs);
    os_mem_free(y_sqr_rhs);
    return (ret == 0) && (on_curve == 1);
}

int crypto_ec_point_cmp(const struct crypto_ec *e,
                        const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b)
{
    return blcrypto_suite_ecp_point_cmp((const blcrypto_suite_ecp_point *)a,
                                        (const blcrypto_suite_ecp_point *)b);
}
