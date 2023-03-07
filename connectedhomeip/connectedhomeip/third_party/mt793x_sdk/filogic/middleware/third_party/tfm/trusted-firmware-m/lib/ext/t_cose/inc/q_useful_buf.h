/*
 * q_useful_buf.h
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#ifndef __Q_USEFUL_BUF_H__
#define __Q_USEFUL_BUF_H__

#include "UsefulBuf.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \file q_useful_buf.h
 *
 * \brief This is a TF-M coding style version of UsefulBuf.
 *        See UsefulBuf for documentation of these functions.
 */


#define NULL_Q_USEFUL_BUF_C  NULLUsefulBufC

#define NULL_Q_USEFUL_BUF    NULLUsefulBuf


static inline int q_useful_buf_c_is_null(struct q_useful_buf_c in)
{
    return UsefulBuf_IsNULLC(in);
}


static inline int q_useful_buf_is_null(struct q_useful_buf in)
{
    return UsefulBuf_IsNULL(in);
}


static inline int q_useful_buf_c_is_empty(struct q_useful_buf_c in)
{
    return UsefulBuf_IsEmptyC(in);
}

static inline int q_useful_buf_is_empty(struct q_useful_buf in)
{
    return UsefulBuf_IsEmpty(in);
}


static inline int q_useful_buf_is_null_or_empty(struct q_useful_buf in)
{
    return UsefulBuf_IsNULLOrEmpty(in);
}


static inline int q_useful_buf_c_is_null_or_empty(struct q_useful_buf_c in)
{
    return UsefulBuf_IsNULLOrEmptyC(in);
}


static inline struct q_useful_buf q_useful_buf_unconst(struct q_useful_buf_c in)
{
    return UsefulBuf_Unconst(in);
}

#define Q_USEFUL_BUF_FROM_SZ_LITERAL UsefulBuf_FROM_SZ_LITERAL

#define Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL  UsefulBuf_FROM_BYTE_ARRAY_LITERAL

#define Q_USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define Q_USEFUL_BUF_FROM_BYTE_ARRAY UsefulBuf_FROM_BYTE_ARRAY


static inline struct q_useful_buf_c q_useful_buf_from_sz(const char *string)
{
    return UsefulBuf_FromSZ(string);
}

static inline struct q_useful_buf_c
useful_buf_copy_offset(struct q_useful_buf dest,
                       size_t offset,
                       struct q_useful_buf_c src)
{
    return UsefulBuf_CopyOffset(dest, offset, src);
}



static inline struct q_useful_buf_c q_useful_buf_copy(struct q_useful_buf dest,
                                                     struct q_useful_buf_c src)
{
    return UsefulBuf_Copy(dest, src);
}


static inline struct q_useful_buf_c q_useful_buf_set(struct q_useful_buf dest,
                                                     uint8_t value)
{
    return UsefulBuf_Set(dest, value);
}


static inline struct q_useful_buf_c q_useful_buf_copy_ptr(struct q_useful_buf d,
                                                          const void *ptr,
                                                          size_t len)
{
    return UsefulBuf_CopyPtr(d, ptr, len);
}


static inline struct q_useful_buf_c q_useful_buf_head(struct q_useful_buf_c buf,
                                                      size_t amount)
{
    return UsefulBuf_Head(buf, amount);
}

static inline struct q_useful_buf_c q_useful_buf_tail(struct q_useful_buf_c buf,
                                                      size_t amount)
{
    return UsefulBuf_Tail(buf, amount);
}

static inline int q_useful_buf_compare(const struct q_useful_buf_c buf1,
                                       const struct q_useful_buf_c buf2)
{
    return UsefulBuf_Compare(buf1, buf2);
}

static inline size_t q_useful_buf_is_value(const struct q_useful_buf_c buf,
                                           uint8_t uValue)
{
    return UsefulBuf_IsValue(buf, uValue);
}

static inline size_t
q_useful_buf_find_bytes(const struct q_useful_buf_c bytes_to_search,
                      const struct q_useful_buf_c bytes_to_find)
{
    return UsefulBuf_FindBytes(bytes_to_search, bytes_to_find);
}


#ifdef __cplusplus
}
#endif

#endif /* __Q_USEFUL_BUF_H__ */
