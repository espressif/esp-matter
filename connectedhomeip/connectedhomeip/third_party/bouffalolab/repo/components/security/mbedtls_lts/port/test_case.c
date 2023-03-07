/*
 *  Description: mbedtls hardware acceleration basic test cases
 *  Copyright (C) Bouffalo Lab 2016-2022
 *  SPDX-License-Identifier: Apache-2.0
 *  File Name:   test_case.c
 *  Author:      Chien Wong(qwang@bouffalolab.com)
 *  Start Date:  Jun 8, 2022
 */

#include <stdbool.h>
#include <string.h>
#include <mbedtls/ecp.h>
#include <mbedtls/aes.h>

#define CHECK(r) \
    do { \
        if ((ret = r)) { \
            printf("%s: LN %d returned %d\r\n", __func__, __LINE__, ret); \
            goto out; \
        } \
    } while (0)

bool mbedtls_tc_ecp_p256_mul()
{
    int ret;
    bool passed = false;
    mbedtls_ecp_group group;
    mbedtls_ecp_group_init(&group);
    mbedtls_ecp_group_load(&group, MBEDTLS_ECP_DP_SECP256R1);

    mbedtls_ecp_point R, R_gold;
    mbedtls_mpi m;
    mbedtls_ecp_point_init(&R);
    mbedtls_ecp_point_init(&R_gold);
    mbedtls_mpi_init(&m);

    const char *tv_m_str = "99A1F7B45827419C35E67F42BC02FE7CFA7629430EBBF32AF55E77E8B5ED88F5";
    const char *tv_R_X_str = "06B8FBCE1A4909B119808FCDBE45DF209F9B2FFE5C17E12FC04F199B38FA687A";
    const char *tv_R_Y_str = "AE9FAFB90A1E91F2C1955C7D83D100C0CB5FC6F253EED2DFFD6C2C60A4F183B8";
    const char *tv_R_Z_str = "01";

    CHECK(mbedtls_mpi_read_string(&m, 16, tv_m_str));
    CHECK(mbedtls_mpi_read_string(&R_gold.X, 16, tv_R_X_str));
    CHECK(mbedtls_mpi_read_string(&R_gold.Y, 16, tv_R_Y_str));
    CHECK(mbedtls_mpi_read_string(&R_gold.Z, 16, tv_R_Z_str));

    CHECK(mbedtls_ecp_mul(&group, &R, &m, &group.G, NULL, NULL));
    if (mbedtls_ecp_point_cmp(&R, &R_gold) == 0) {
        passed = true;
    } else {
        printf("Result mismatch!\r\n");
    }
out:
    mbedtls_ecp_group_free(&group);
    mbedtls_ecp_point_free(&R);
    mbedtls_ecp_point_free(&R_gold);
    mbedtls_mpi_free(&m);
    if (ret) {
        passed = false;
    }
    return passed;
}

bool mbedtls_tc_mpi_exp_mod()
{
    int ret;
    bool passed = false;
    mbedtls_mpi A, E, N, X, X_gold;
    const char *A_str = "AA088D0869CAB1CE4A264DC94CC06FEA69D3B1E0D829D3AC1250332C2429CB8C1B06655D23931AA46D9D4C83BAF05483BD6EF4F61355657B470EB3FD2574FDBC0723A5E3606BBE219480F52276C7D6529C905E060D8708C809ED07A545412681FF443D80AA34875F5BE3BDEB00A0A46899BB8A0CFB5A3CA7E582EE3DF5384B400D33CA64658D0FFBE502BE2486F9FA2E9A1CFBD27066370A7AE880F25144F45CE019457B77FF444441391588A014F429D65D8478FE4D36350578987379329B310D77AC060272B03D18FA642D989D4FF6C1A0F798C16CEF49F55909F8FF041A7F0984A5616C2C3D7E981523B015003494F07CF37E229415C0FF853D78033909";
    const char *E_str = "010001";
    const char *N_str = "B1B6DBD53B85084FDF88F85CDEE3AEF2844554AC9BE440E78D7CD83B50C6D25CC7B0B5377CF0D1999ED7B94F7BA27B396164B4CCFCA497EB08F41211B0932195C5FD8ABE09855687B6C51F20C514A110F1B6D169B48B545B99306075E8CCCCBD7ABDF3F59FAB4BCD900F60531EF855A754568A36D70E467B719F22D02B40A5051854CA59B5B9C3FDED203124CDE3BE427C5973435E040A3E6A45615235FD6F827205E1DD1BAA6658DB94BDF4CF3E9DF98D59C96AC2447C101455C618D8CCC59078708EE9DE3E33A6D88A771904948EFFEEB48611DA98F0A6A3D5C5B2E65C7A3F5D4D9D827FB117A0599970C305C39ADC01EEC70A52C7A2C8D84EFB5C52BA1B4B";
    const char *X_gold_str = "22BA4261AACA908A6C5FD14A9038744AE64C30B14F56CF94378FEA98537F9CD14C4BB171AA987F6DFD904865D7148746D635BFD0D76290E499A38FA9068D29972BAB7BA8D042E19B6561DD5B1586FFB7560A6EB6834F059EC6F310C562F6896B4C3432C7FFB8867A86DE1CDB859EAF8C6FC07F5DC2ADD8D0F485F8BD916B10C5628A2ADE42653F750FC8440083317D78724D9BB25C00A0732518E8810F0F9B97BA89AE0F3677B053974ED2D2839423B6EE34A488AF42E5CA039103C4EBAB6CF0105549B647CBB5C13A52C5773C53040E37434620055993749FB37B9EFC37A5BF0CD6E46B0EBD950D8104B901F82C4D80662C6EF65A3F4CAECD5CE64CE1741EF3";

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&E);
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&X);
    mbedtls_mpi_init(&X_gold);
    CHECK(mbedtls_mpi_read_string(&A, 16, A_str));
    CHECK(mbedtls_mpi_read_string(&E, 16, E_str));
    CHECK(mbedtls_mpi_read_string(&N, 16, N_str));
    CHECK(mbedtls_mpi_read_string(&X_gold, 16, X_gold_str));
    CHECK(mbedtls_mpi_exp_mod(&X, &A, &E, &N, NULL));
    if (mbedtls_mpi_cmp_mpi(&X, &X_gold) == 0) {
        passed = true;
    } else {
        printf("Result mismatch!\r\n");
    }
out:
    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&E);
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&X);
    mbedtls_mpi_free(&X_gold);
    if (ret) {
        passed = false;
    }
    return passed;
}

bool mbedtls_tc_aes_ecb()
{
    bool passed = true;
    mbedtls_aes_context aes;
    const size_t rounds = 100 * 1000;
    const void *plaintext = "0123456789ABCDEF";
    const void *key = "FEDCBA9876543210";
    const uint8_t expected_ciphertext[16] = "\x38\x44\x36\x19\xc1\x70\xf5\x96\x81\xc4\xa6\xe0\xf1\x18\xc2\x49";
    uint8_t ciphertext[16];

    for (size_t i = 0; i < rounds; ++i) {
        memset(ciphertext, 0, 16);
        mbedtls_aes_init(&aes);
        mbedtls_aes_setkey_enc(&aes, key, 128);
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plaintext, ciphertext);
        mbedtls_aes_free(&aes);
        if (memcmp(expected_ciphertext, ciphertext, 16)) {
            passed = false;
            break;
        }
    }

    return passed;
}

#define MD(tc) {tc, #tc}
static struct {
    bool (*tc_fun)(void);
    const char *desc;
} tc[] = {
    MD(mbedtls_tc_ecp_p256_mul),
    MD(mbedtls_tc_mpi_exp_mod),
    MD(mbedtls_tc_aes_ecb),
};

void mbedtls_test_case_run()
{
    for (int i = 0; i < sizeof(tc) / sizeof(tc[0]); ++i) {
        printf("Running test case %s\r\n", tc[i].desc);
        if (tc[i].tc_fun()) {
            printf("\t\tPassed\r\n");
        } else {
            printf("\t\tFailed\r\n");
        }
    }
}
