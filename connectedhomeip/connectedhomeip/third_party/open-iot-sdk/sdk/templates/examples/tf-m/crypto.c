/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa/crypto.h"

#include "tfm_ns_interface.h"

#include <stdio.h>

// RTOS-specific initialization that is not declared in any header file
uint32_t tfm_ns_interface_init(void);

// PSA Crypto API example - verify an SHA-256 hash
void psa_thread(void *argument)
{
    printf("In psa_thread()\r\n");

    uint32_t ret = tfm_ns_interface_init();
    if (ret != 0) {
        printf("tfm_ns_interface_init() failed: %u\r\n", ret);
        return;
    }

    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_SHA_256;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    static const unsigned char input[] = {'a', 'b', 'c'};
    static const unsigned char expected_hash[] = {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40,
                                                  0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17,
                                                  0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
    size_t expected_hash_len = PSA_HASH_LENGTH(alg);

    printf("Verify a hash...\t");

    // Verify message hash
    status = psa_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        printf("Failed to begin hash operation. Error %u\r\n", status);
        return;
    }
    status = psa_hash_update(&operation, input, sizeof(input));
    if (status != PSA_SUCCESS) {
        printf("Failed to update hash operation. Error %u\r\n", status);
        return;
    }
    status = psa_hash_verify(&operation, expected_hash, expected_hash_len);
    if (status != PSA_SUCCESS) {
        printf("Failed to verify hash. Error %u\r\n", status);
        return;
    }

    printf("Verified a hash\r\n");
}
