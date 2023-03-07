/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_utils.h"

psa_status_t its_utils_check_contained_in(size_t superset_size,
                                          size_t subset_offset,
                                          size_t subset_size)
{
    /* Check that subset_offset is valid */
    if (subset_offset > superset_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that subset_offset + subset_size fits in superset_size.
     * The previous check passed, so we know that subset_offset <= superset_size
     * and so the right hand side of the inequality cannot underflow.
     */
    if (subset_size > (superset_size - subset_offset)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

psa_status_t its_utils_validate_fid(const uint8_t *fid)
{
    uint32_t fid_size = ITS_FILE_ID_SIZE;

    /* A file ID is valid if it is non-zero */
    while (fid_size--) {
        if (*fid++) {
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_DOES_NOT_EXIST;
}
