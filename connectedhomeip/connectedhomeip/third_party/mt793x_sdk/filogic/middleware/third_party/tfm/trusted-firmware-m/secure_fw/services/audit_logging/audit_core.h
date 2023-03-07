/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __AUDIT_CORE_H__
#define __AUDIT_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "psa_audit_defs.h"
#include "psa/error.h"

#define UNIFORM_SIGNATURE_API(api_name) \
    psa_status_t api_name(psa_invec[], size_t, psa_outvec[], size_t)

/*!
 * \struct log_entry
 *
 * \brief Structure of a single log entry
 *        in the log
 * \details This can't be represented as a
 *          structure because the payload
 *          is of variable size, i.e.
 * |Offset |Name        |
 * |-------|------------|
 * | 0     |TIMESTAMP   |
 * | 8     |IV_COUNTER  |
 * |12     |PARTITION ID|
 * |16     |SIZE        |
 * |20     |RECORD ID   |
 * |24     |PAYLOAD     |
 * |20+SIZE|MAC         |
 *
 * SIZE: at least LOG_MIN_SIZE bytes, known only at runtime. It's the size of
 *       the (RECORD_ID, PAYLOAD) fields
 *
 * MAC_SIZE: known at build time (currently, 4 bytes)
 *
 * At runtime, when adding a record, the value of SIZE has to be checked and
 * must be less than LOG_SIZE - MAC_SIZE - 12 and equal or greater than
 * LOG_MIN_SIZE
 *
 */

/*!
 * \def LOG_MIN_SIZE
 *
 * \brief Minimum size of the encrypted part
 */
#define LOG_MIN_SIZE (4)

/*!
 * \enum audit_tlv_type
 *
 * \brief Possible types for a TLV entry
 *        in payload
 */
enum audit_tlv_type {
    TLV_TYPE_ID = 0,
    TLV_TYPE_AUTH = 1,

    /* This is used to force the maximum size */
    TLV_TYPE_MAX = INT_MAX
};

/*!
 * \struct audit_tlv_entry
 *
 * \brief TLV entry structure with a flexible array member
 */
struct audit_tlv_entry {
    enum audit_tlv_type type;
    uint32_t length;
    uint8_t value[];
};

/*!
 * \def LOG_MAC_SIZE
 *
 * \brief Size in bytes of the MAC for each entry
 */
#define LOG_MAC_SIZE (4)

/*!
 * \struct log_hdr
 *
 * \brief Fixed size header for a log record
 */
struct log_hdr {
    uint64_t timestamp;
    uint32_t iv_counter;
    int32_t partition_id;
    uint32_t size;
    uint32_t id;
};

/*!
 * \struct log_tlr
 *
 * \brief Fixed size logging entry trailer
 */
struct log_tlr {
    uint8_t mac[LOG_MAC_SIZE];
};

/*!
 * \def LOG_HDR_SIZE
 *
 * \brief Size in bytes of the (fixed) header for each entry
 */
#define LOG_HDR_SIZE (sizeof(struct log_hdr))

/*!
 * \def LOG_TLR_SIZE
 *
 * \brief Size in bytes of the (fixed) trailer for each entry
 */
#define LOG_TLR_SIZE (sizeof(struct log_tlr))

/*!
 * \brief Initializes the Audit logging service
 *        during the TFM boot up process
 *
 * \return Returns PSA_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref psa_status_t
 */
psa_status_t audit_core_init(void);

#define LIST_TFM_AUDIT_UNIFORM_SIGNATURE_API \
    X(audit_core_delete_record)              \
    X(audit_core_get_info)                   \
    X(audit_core_get_record_info)            \
    X(audit_core_add_record)                 \
    X(audit_core_retrieve_record)            \

#define X(api_name) UNIFORM_SIGNATURE_API(api_name);
LIST_TFM_AUDIT_UNIFORM_SIGNATURE_API
#undef X

#ifdef __cplusplus
}
#endif

#endif /* __AUDIT_CORE_H__ */
