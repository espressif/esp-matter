/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_DHM_C)
#if defined(MBEDTLS_DHM_ALT)
#include <string.h>

#if defined(MBEDTLS_PEM_PARSE_C)
#include "mbedtls/pem.h"
#endif

#if defined(MBEDTLS_ASN1_PARSE_C)
#include "mbedtls/asn1.h"
#endif

#define mbedtls_calloc    calloc
#define mbedtls_free       free

/* include pki.h for using HW Exponentiation function */
#include "cc_bitops.h"
#include "cc_common_math.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "dhm_alt.h"
#include "pka.h"
#include "pki.h"

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * helper to validate the mbedtls_mpi size and import it
 */
static int dhm_read_bignum( mbedtls_mpi *X,
                            unsigned char **p,
                            const unsigned char *end )
{
    int ret, n;

    if( end - *p < 2 )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    n = ( (*p)[0] << 8 ) | (*p)[1];
    (*p) += 2;

    if( (int)( end - *p ) < n )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    if( ( ret = mbedtls_mpi_read_binary( X, *p, n ) ) != 0 )
        return( MBEDTLS_ERR_DHM_READ_PARAMS_FAILED + ret );

    (*p) += n;

    return( 0 );
}

/*
 * Verify sanity of parameter with regards to P
 *
 * Parameter should be: 2 <= public_param <= P - 2
 *
 * For more information on the attack, see:
 *  http://www.cl.cam.ac.uk/~rja14/Papers/psandqs.pdf
 *  http://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2005-2643
 */
static int dhm_check_range( const mbedtls_mpi *param, const mbedtls_mpi *P )
{
    mbedtls_mpi L, U;
    int ret = MBEDTLS_ERR_DHM_BAD_INPUT_DATA;

    mbedtls_mpi_init( &L ); mbedtls_mpi_init( &U );

    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &L, 2 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &U, P, 2 ) );

    if( mbedtls_mpi_cmp_mpi( param, &L ) >= 0 &&
        mbedtls_mpi_cmp_mpi( param, &U ) <= 0 ) {
        ret = 0;
    }

cleanup:
    mbedtls_mpi_free( &L ); mbedtls_mpi_free( &U );
    return( ret );
}


#define EXP_MOD_HW 1
#ifdef EXP_MOD_HW

/**
 *  Execute modular exponentiation using ARM HW PKA.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         All sizes aligned to 32-bit words, leading zeros are present if exist.
 *         Values of A and E should be less than modulus size.
 *         PKA registers used: r0,r1,r2,r3,r4, r30,r31.
 *
 *
 * @return  no return value
 */

static int mbedtls_mpi_exp_mod_hw(
        mbedtls_mpi *X/*out*/, const mbedtls_mpi *A/*in*/, const mbedtls_mpi *E/*in*/,
        const mbedtls_mpi *N/*in*/, mbedtls_mpi *_RR/*in*/ )
{
    int err = 0;
    uint32_t n_size_bits, a_size_bits;
    uint32_t n_len_words, a_len_words;
    mbedtls_mpi T;

    mbedtls_mpi_init( &T );

    /* compare range of input parameters */
    if( mbedtls_mpi_cmp_int( N, 0 ) < 0 || ( N->p[0] & 1 ) == 0 ) {
        return( MBEDTLS_ERR_MPI_BAD_INPUT_DATA );
    }
    if( mbedtls_mpi_cmp_int( E, 0 ) < 0 ) {
        return( MBEDTLS_ERR_MPI_BAD_INPUT_DATA );
    }

    /* get actual length of A and N in bits and words */
    a_size_bits = mbedtls_mpi_bitlen( A );
    n_size_bits = mbedtls_mpi_bitlen( N );
    n_len_words = CALC_FULL_32BIT_WORDS( n_size_bits );
    a_len_words = CALC_FULL_32BIT_WORDS( a_size_bits );

    /* if prime N size is very small or very big (not meets to HW PKA and standards requirements),
     * then use SW mbedtls function. These exceptions are not important for real practic.  */
    if( n_size_bits < PKA_MIN_OPERATION_SIZE_BITS || n_size_bits > PKA_MAX_OPERATION_SIZE_BITS ) {
          err = mbedtls_mpi_exp_mod( X, A, E, N, _RR );
          goto End;
    }

    /* allocate positive X  */
    err = mbedtls_mpi_grow( X, N->n/* + 1*/ );
    X->s = 1;

    if ( err != 0 ) {
        goto End;
    }

    /* if needed reduce A according to its sign and modulus N and then
       perform exponentiation using PKA HW. */
    if( a_size_bits > n_size_bits ) {
        err = mbedtls_mpi_grow( &T, N->n/* + 1*/ );
        if ( err != 0 ) {
            goto End;
        }
        /* reduction by modulus P using PKA. */
        err = PkiLongNumDiv( A->p, a_len_words, N->p, n_len_words, T.p/*ModRes*/, NULL/*DivRes*/ );
        if (err != 0) {
            goto End;
        }

        /* execute exponentiation on PKA HW */
        err = PkiExecModExpLeW( X->p/*out*/, T.p/*in*/, n_len_words, N->p/*modulus*/, n_size_bits, E->p, E->n );
        if ( err != 0 ) {
            goto End;
        }
    }
    /* exponentiation without reduction */
    else {
        /* execute exponentiation on PKA HW */
        err = PkiExecModExpLeW( X->p/*out*/, A->p/*in*/, a_len_words, N->p/*modulus*/, n_size_bits, E->p, E->n );
        if ( err != 0 ) {
            goto End;
        }
    }

    /* modular correction for negative base A */
    if( ( A->s == -1 )  && ( E->n != 0 ) && (( E->p[0] & 1 ) != 0) )
    {
        X->s = -1;
        err = mbedtls_mpi_add_mpi( X, N, X );
        if ( err != 0 ) {
            goto End;
        }
    }


End:

    if(err) {
        mbedtls_zeroize( X->p, X->n );
    }
    mbedtls_mpi_free(&T);

    return err;

}
#else
#define mbedtls_mpi_exp_mod_hw mbedtls_mpi_exp_mod
#endif

void mbedtls_dhm_init( mbedtls_dhm_context *ctx )
{
    if (NULL == ctx)
    {
        CC_PalAbort("ctx cannot be NULL");
    }

    CC_PalMemSetZero(ctx, sizeof(mbedtls_dhm_context));

}

/*
 * Parse the ServerKeyExchange parameters
 */
int mbedtls_dhm_read_params( mbedtls_dhm_context *ctx,
                     unsigned char **p,
                     const unsigned char *end )
{
    int ret;
    if ( (ctx == NULL) ||
         (p == NULL) ||
         (end == NULL) ) {
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA;
    }
    if (*p == NULL) {
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA;
    }

    if( ( ret = dhm_read_bignum( &ctx->P,  p, end ) ) != 0 ||
        ( ret = dhm_read_bignum( &ctx->G,  p, end ) ) != 0 ||
        ( ret = dhm_read_bignum( &ctx->GY, p, end ) ) != 0 ){
        return( ret );
    }

    if( ( ret = dhm_check_range( &ctx->GY, &ctx->P ) ) != 0 ){
        return( ret );
    }

    ctx->len = mbedtls_mpi_size( &ctx->P );

    return( 0 );
}

/*
 * Setup and write the ServerKeyExchange parameters
 */
int mbedtls_dhm_make_params( mbedtls_dhm_context *ctx, int x_size,
                     unsigned char *output, size_t *olen,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret, count = 0;
    size_t n1, n2, n3;
    unsigned char *p;

    if ( (ctx == NULL) || (output == NULL) || (olen == NULL) ||
         (f_rng == NULL) || (p_rng == NULL)) {
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA ;
    }
    /*
     * x size must be less or equal to size of P
     */
    if ((x_size > (int)mbedtls_mpi_size(&ctx->P)) || (x_size <= 0)){
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA ;
    }

    if( mbedtls_mpi_cmp_int( &ctx->P, 0 ) == 0 ){
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );
    }
    /*
     * Generate X as large as possible ( < P )
     */
    do
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &ctx->X, x_size, f_rng, p_rng ) );

        while( mbedtls_mpi_cmp_mpi( &ctx->X, &ctx->P ) >= 0 )
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &ctx->X, 1 ) );

        if( count++ > 10 )
            return( MBEDTLS_ERR_DHM_MAKE_PARAMS_FAILED );
    }
    while( dhm_check_range( &ctx->X, &ctx->P ) != 0 );

    /*
     * Calculate GX = G^X mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod_hw( &ctx->GX, &ctx->G, &ctx->X,
                          &ctx->P , &ctx->RP ) );
    if( ( ret = dhm_check_range( &ctx->GX, &ctx->P ) ) != 0 ){
        return( ret );
    }
    /*
     * export P, G, GX
     */
#define DHM_MPI_EXPORT(X,n)                     \
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( X, p + 2, n ) ); \
    *p++ = (unsigned char)( n >> 8 );           \
    *p++ = (unsigned char)( n      ); p += n;

    n1 = mbedtls_mpi_size( &ctx->P  );
    n2 = mbedtls_mpi_size( &ctx->G  );
    n3 = mbedtls_mpi_size( &ctx->GX );

    p = output;
    DHM_MPI_EXPORT( &ctx->P , n1 );
    DHM_MPI_EXPORT( &ctx->G , n2 );
    DHM_MPI_EXPORT( &ctx->GX, n3 );

    *olen  = p - output;

    ctx->len = n1;

cleanup:

    if( ret != 0 ){
        return( MBEDTLS_ERR_DHM_MAKE_PARAMS_FAILED + ret );
    }
    return( 0 );
}

/*
 * Set prime modulus and generator
 */
int mbedtls_dhm_set_group( mbedtls_dhm_context *ctx,
                           const mbedtls_mpi *P,
                           const mbedtls_mpi *G )
{
    int ret;

    if( ctx == NULL || P == NULL || G == NULL )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    if( ( ret = mbedtls_mpi_copy( &ctx->P, P ) ) != 0 ||
        ( ret = mbedtls_mpi_copy( &ctx->G, G ) ) != 0 )
    {
        return( MBEDTLS_ERR_DHM_SET_GROUP_FAILED + ret );
    }

    ctx->len = mbedtls_mpi_size( &ctx->P );
    return( 0 );
}

/*
 * Import the peer's public value G^Y
 */
int mbedtls_dhm_read_public( mbedtls_dhm_context *ctx,
                     const unsigned char *input, size_t ilen )
{
    int ret;

    if( ctx == NULL || input == NULL || ilen < 1 || ilen > ctx->len )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    if( ( ret = mbedtls_mpi_read_binary( &ctx->GY, input, ilen ) ) != 0 )
        return( MBEDTLS_ERR_DHM_READ_PUBLIC_FAILED + ret );

    return( 0 );
}

/*
 * Create own private value X and export G^X
 */
int mbedtls_dhm_make_public( mbedtls_dhm_context *ctx, int x_size,
                     unsigned char *output, size_t olen,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret, count = 0;

    if( ctx == NULL || output == NULL || f_rng == NULL || p_rng ==NULL || olen < ctx->len )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );
    if( (x_size > (int)ctx->len ) || (x_size <= 0))
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    if( mbedtls_mpi_cmp_int( &ctx->P, 0 ) == 0 )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    /*
     * generate X and calculate GX = G^X mod P
     */
    do
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &ctx->X, x_size, f_rng, p_rng ) );

        while( mbedtls_mpi_cmp_mpi( &ctx->X, &ctx->P ) >= 0 )
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &ctx->X, 1 ) );

        if( count++ > 10 )
            return( MBEDTLS_ERR_DHM_MAKE_PUBLIC_FAILED );
    }
    while( dhm_check_range( &ctx->X, &ctx->P ) != 0 );

    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod_hw( &ctx->GX, &ctx->G, &ctx->X,
                          &ctx->P , &ctx->RP ) );

    if( ( ret = dhm_check_range( &ctx->GX, &ctx->P ) ) != 0 )
        return( ret );

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->GX, output, olen ) );

cleanup:

    if( ret != 0 )
        return( MBEDTLS_ERR_DHM_MAKE_PUBLIC_FAILED + ret );

    return( 0 );
}

/*
 * Use the blinding method and optimisation suggested in section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology-CRYPTO'96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int dhm_update_blinding( mbedtls_dhm_context *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret, count;

    /*
     * Don't use any blinding the first time a particular X is used,
     * but remember it to use blinding next time.
     */
    if( mbedtls_mpi_cmp_mpi( &ctx->X, &ctx->pX ) != 0 )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &ctx->pX, &ctx->X ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &ctx->Vi, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &ctx->Vf, 1 ) );

        return( 0 );
    }

    /*
     * Ok, we need blinding. Can we re-use existing values?
     * If yes, just update them by squaring them.
     */
    if( mbedtls_mpi_cmp_int( &ctx->Vi, 1 ) != 0 )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vi, &ctx->Vi, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vi, &ctx->Vi, &ctx->P ) );

        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vf, &ctx->Vf, &ctx->Vf ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vf, &ctx->Vf, &ctx->P ) );

        return( 0 );
    }

    /*
     * We need to generate blinding values from scratch
     */

    /* Vi = random( 2, P-1 ) */
    count = 0;
    do
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &ctx->Vi, mbedtls_mpi_size( &ctx->P ), f_rng, p_rng ) );

        while( mbedtls_mpi_cmp_mpi( &ctx->Vi, &ctx->P ) >= 0 )
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &ctx->Vi, 1 ) );

        if( count++ > 10 )
            return( MBEDTLS_ERR_MPI_NOT_ACCEPTABLE );
    }
    while( mbedtls_mpi_cmp_int( &ctx->Vi, 1 ) <= 0 );

    /* Vf = Vi^-X mod P */
    MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &ctx->Vf, &ctx->Vi, &ctx->P ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod_hw( &ctx->Vf, &ctx->Vf, &ctx->X, &ctx->P, &ctx->RP ) );

cleanup:
    return( ret );
}

/*
 * Derive and export the shared secret (G^Y)^X mod P
 */
int mbedtls_dhm_calc_secret( mbedtls_dhm_context *ctx,
                     unsigned char *output, size_t output_size, size_t *olen,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;
    mbedtls_mpi GYb;

    if( ctx == NULL || output == NULL || olen == NULL || output_size < ctx->len )
        return( MBEDTLS_ERR_DHM_BAD_INPUT_DATA );

    if( ( ret = dhm_check_range( &ctx->GY, &ctx->P ) ) != 0 ) {
        return( ret );
    }

    mbedtls_mpi_init( &GYb );

    /* Blind peer's value */
    if( f_rng != NULL )
    {
        MBEDTLS_MPI_CHK( dhm_update_blinding( ctx, f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &GYb, &ctx->GY, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &GYb, &GYb, &ctx->P ) );
    }
    else
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &GYb, &ctx->GY ) );

    /* Do modular exponentiation */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod_hw( &ctx->K, &GYb, &ctx->X,
                          &ctx->P, &ctx->RP ) );

    /* Unblind secret value */
    if( f_rng != NULL )
    {
       MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->K, &ctx->K, &ctx->Vf ) );
       MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->K, &ctx->K, &ctx->P ) );
    }

    *olen = mbedtls_mpi_size( &ctx->K );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->K, output, *olen ) );

cleanup:
    mbedtls_mpi_free( &GYb );
    if( ret != 0 )
        return( MBEDTLS_ERR_DHM_CALC_SECRET_FAILED + ret );

    return( 0 );
}

/*
 * Free the components of a DHM key
 */
void mbedtls_dhm_free( mbedtls_dhm_context *ctx )
{
    if ( ctx != NULL) {
        mbedtls_mpi_free( &ctx->pX); mbedtls_mpi_free( &ctx->Vf ); mbedtls_mpi_free( &ctx->Vi );
        mbedtls_mpi_free( &ctx->RP ); mbedtls_mpi_free( &ctx->K ); mbedtls_mpi_free( &ctx->GY );
        mbedtls_mpi_free( &ctx->GX ); mbedtls_mpi_free( &ctx->X ); mbedtls_mpi_free( &ctx->G );
        mbedtls_mpi_free( &ctx->P );

        mbedtls_zeroize( ctx, sizeof( mbedtls_dhm_context ) );
    }
}

#if defined(MBEDTLS_ASN1_PARSE_C)
/*
 * Parse DHM parameters
 */
int mbedtls_dhm_parse_dhm( mbedtls_dhm_context *dhm, const unsigned char *dhmin,
                   size_t dhminlen )
{
    int ret;
    size_t len;
    unsigned char *p, *end;
#if defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_pem_context pem;

    mbedtls_pem_init( &pem );

    if ((dhm == NULL) || (dhmin == NULL)){
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA;
    }
    /* Avoid calling mbedtls_pem_read_buffer() on non-null-terminated string */
    if( dhminlen == 0 || dhmin[dhminlen - 1] != '\0' )
        ret = MBEDTLS_ERR_PEM_NO_HEADER_FOOTER_PRESENT;
    else
        ret = mbedtls_pem_read_buffer( &pem,
                               "-----BEGIN DH PARAMETERS-----",
                               "-----END DH PARAMETERS-----",
                               dhmin, NULL, 0, &dhminlen );

    if( ret == 0 )
    {
        /*
         * Was PEM encoded
         */
        dhminlen = pem.buflen;
    }
    else if( ret != MBEDTLS_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
        goto exit;

    p = ( ret == 0 ) ? pem.buf : (unsigned char *) dhmin;
#else
    p = (unsigned char *) dhmin;
#endif /* MBEDTLS_PEM_PARSE_C */
    end = p + dhminlen;

    /*
     *  DHParams ::= SEQUENCE {
     *      prime              INTEGER,  -- P
     *      generator          INTEGER,  -- g
     *      privateValueLength INTEGER OPTIONAL
     *  }
     */
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        ret = MBEDTLS_ERR_DHM_INVALID_FORMAT + ret;
        goto exit;
    }

    end = p + len;

    if( ( ret = mbedtls_asn1_get_mpi( &p, end, &dhm->P  ) ) != 0 ||
        ( ret = mbedtls_asn1_get_mpi( &p, end, &dhm->G ) ) != 0 )
    {
        ret = MBEDTLS_ERR_DHM_INVALID_FORMAT + ret;
        goto exit;
    }

    if( p != end )
    {
        /* This might be the optional privateValueLength.
         * If so, we can cleanly discard it */
        mbedtls_mpi rec;
        mbedtls_mpi_init( &rec );
        ret = mbedtls_asn1_get_mpi( &p, end, &rec );
        mbedtls_mpi_free( &rec );
        if ( ret != 0 )
        {
            ret = MBEDTLS_ERR_DHM_INVALID_FORMAT + ret;
            goto exit;
        }
        if ( p != end )
        {
            ret = MBEDTLS_ERR_DHM_INVALID_FORMAT +
                MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            goto exit;
        }
    }

    ret = 0;

    dhm->len = mbedtls_mpi_size( &dhm->P );

exit:
#if defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_pem_free( &pem );
#endif
    if( ret != 0 )
        mbedtls_dhm_free( dhm );

    return( ret );
}

#if defined(MBEDTLS_FS_IO)
/*
 * Load all data from a file into a given buffer.
 *
 * The file is expected to contain either PEM or DER encoded data.
 * A terminating null byte is always appended. It is included in the announced
 * length only if the data looks like it is PEM encoded.
 */
static int load_file( const char *path, unsigned char **buf, size_t *n )
{
    FILE *f;
    long size;

    if( ( f = fopen( path, "rb" ) ) == NULL )
        return( MBEDTLS_ERR_DHM_FILE_IO_ERROR );

    fseek( f, 0, SEEK_END );
    if( ( size = ftell( f ) ) == -1 )
    {
        fclose( f );
        return( MBEDTLS_ERR_DHM_FILE_IO_ERROR );
    }
    fseek( f, 0, SEEK_SET );

    *n = (size_t) size;

    if( *n + 1 == 0 ||
        ( *buf = mbedtls_calloc( 1, *n + 1 ) ) == NULL )
    {
        fclose( f );
        return( MBEDTLS_ERR_DHM_ALLOC_FAILED );
    }

    if( fread( *buf, 1, *n, f ) != *n )
    {
        fclose( f );
        mbedtls_free( *buf );
        return( MBEDTLS_ERR_DHM_FILE_IO_ERROR );
    }

    fclose( f );

    (*buf)[*n] = '\0';

    if( strstr( (const char *) *buf, "-----BEGIN " ) != NULL )
        ++*n;

    return( 0 );
}

/*
 * Load and parse DHM parameters
 */
int mbedtls_dhm_parse_dhmfile( mbedtls_dhm_context *dhm, const char *path )
{
    int ret;
    size_t n;
    unsigned char *buf;

    if ( (dhm == NULL) || (path == NULL) ){
        return MBEDTLS_ERR_DHM_BAD_INPUT_DATA;
    }

    if( ( ret = load_file( path, &buf, &n ) ) != 0 ){
        return( ret );
    }

    ret = mbedtls_dhm_parse_dhm( dhm, buf, n );

    mbedtls_zeroize( buf, n );
    mbedtls_free( buf );

    return( ret );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ASN1_PARSE_C */

#endif /* MBEDTLS_DHM_ALT */
#endif /* MBEDTLS_DHM_C */
