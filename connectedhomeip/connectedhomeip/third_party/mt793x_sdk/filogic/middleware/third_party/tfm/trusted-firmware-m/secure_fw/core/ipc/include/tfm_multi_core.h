/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MULTI_CORE_H__
#define __TFM_MULTI_CORE_H__

#include <stdbool.h>

/* Security attributes of target memory region in memory access check. */
struct security_attr_info_t {
    bool is_valid;             /* Whether the target memory region is valid */
    bool is_secure;            /* Secure memory or non-secure memory */
};

/* Memory access attributes of target memory region in memory access check. */
struct mem_attr_info_t {
    bool is_mpu_enabled;       /* Whether memory protection unit(s) enabled */
    bool is_valid;             /* Whether the target memory region is valid */
    bool is_xn;                /* Execute Never or not */
    bool is_priv_rd_allow;     /* Privileged read is allowed or not */
    bool is_priv_wr_allow;     /* Privileged write is allowed or not */
    bool is_unpriv_rd_allow;   /* Unprivileged read is allowed or not */
    bool is_unpriv_wr_allow;   /* Unprivileged write is allowed or not */
};

/**
 * \brief Retrieve general security isolation configuration information of the
 *        target memory region according to the system memory region layout and
 *        fill the \ref security_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref security_attr_info_t to be filled
 *
 * \return void
 *
 * \note This function doesn't access any hardware security isolation unit.
 */
void tfm_get_mem_region_security_attr(const void *p, size_t s,
                                      struct security_attr_info_t *p_attr);

/**
 * \brief Retrieve general secure memory protection configuration information of
 *        the target memory region according to the system memory region layout
 *        and symbol addresses and fill the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 *
 * \note This function doesn't access any hardware memory protection unit.
 *       The \ref is_mpu_enabled field is set to false by default.
 */
void tfm_get_secure_mem_region_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr);

/**
 * \brief Retrieve general non-secure memory protection configuration
 *        information of the target memory region according to the system memory
 *        region layout and fill the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 *
 * \note This function doesn't access any hardware memory protection unit.
 *       The \ref is_mpu_enabled field is set to false by default.
 */
void tfm_get_ns_mem_region_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr);

#endif /* __TFM_MULTI_CORE_H__ */
