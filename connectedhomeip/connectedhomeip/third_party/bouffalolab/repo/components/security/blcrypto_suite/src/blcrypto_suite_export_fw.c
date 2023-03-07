#include <bl_crypto_api.h>
#include <blcrypto_suite/blcrypto_suite_supplicant_api.h>

#define MK_MEM(x) .x = x

static const struct bl_crypto_api fw_api = {
    MK_MEM(crypto_bignum_init),
    MK_MEM(crypto_bignum_init_set),
    MK_MEM(crypto_bignum_deinit),
    MK_MEM(crypto_bignum_to_bin),
    MK_MEM(crypto_bignum_add),
    MK_MEM(crypto_bignum_mod),
    MK_MEM(crypto_bignum_exptmod),
    MK_MEM(crypto_bignum_inverse),
    MK_MEM(crypto_bignum_sub),
    MK_MEM(crypto_bignum_div),
    MK_MEM(crypto_bignum_mulmod),
    MK_MEM(crypto_bignum_cmp),
    MK_MEM(crypto_bignum_bits),
    MK_MEM(crypto_bignum_is_zero),
    MK_MEM(crypto_bignum_is_one),
    MK_MEM(crypto_bignum_legendre),
    MK_MEM(crypto_ec_init),
    MK_MEM(crypto_ec_deinit),
    MK_MEM(crypto_ec_prime_len),
    MK_MEM(crypto_ec_prime_len_bits),
    MK_MEM(crypto_ec_get_prime),
    MK_MEM(crypto_ec_get_order),
    MK_MEM(crypto_ec_point_init),
    MK_MEM(crypto_ec_point_deinit),
    MK_MEM(crypto_ec_point_to_bin),
    MK_MEM(crypto_ec_point_from_bin),
    MK_MEM(crypto_ec_point_add),
    MK_MEM(crypto_ec_point_mul),
    MK_MEM(crypto_ec_point_invert),
    MK_MEM(crypto_ec_point_solve_y_coord),
    MK_MEM(crypto_ec_point_compute_y_sqr),
    MK_MEM(crypto_ec_point_is_at_infinity),
    MK_MEM(crypto_ec_point_is_on_curve),
    MK_MEM(crypto_ec_point_cmp),
};

void *blcrypto_suite_get_fw_api_table()
{
    return (void *)&fw_api;
}
