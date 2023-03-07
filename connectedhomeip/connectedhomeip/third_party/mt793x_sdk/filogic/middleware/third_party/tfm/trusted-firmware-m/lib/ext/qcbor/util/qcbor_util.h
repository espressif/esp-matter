/*
 * qcbor_util.h
 *
 * Copyright (c) 2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __QCBOR_UTILS_H__
#define __QCBOR_UTILS_H__


#include "qcbor.h"
#include "q_useful_buf.h"
#include "attest_token.h" /* For error codes */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *\file qcbor_util.h
 *
 * \brief Some utility functions for decoding CBOR with QCBOR.
 *
 * All functions search only for integer labeled data items. If data
 * items labeled otherwise are present, they will be skipped over.
 *
 * These functions may eventually expand in to a more general and
 * useful set of decoding utilities.
 *
 * \c uint_fast8_t is used for type and nest levels. They are
 * 8-bit quantities, but making using uint8_t variables
 * and parameters can result in bigger, slower code.
 * \c uint_fast8_t is part of \c <stdint.h>. It is not
 * used in structures where it is more important to keep
 * the size smaller.
 */


/**
 *\brief Consume a data item, particularly a map or array.
 *
 * \param[in] decode_context   The CBOR context from which to
 *                             consume the map or array.
 * \param[in] item_to_consume  The item to consume.
 * \param[out] next_nest_level The nesting level of the item
 *                             that would be consumed next.
 *
 * \return A \c  QCBORError when there is something wrong with the
 *               encoded CBOR.
 *
 * If the \c item_to_consume is not a map or array this does nothing
 * but return the \c next_nest_level (which is just copied from \c
 * item_to_consume). If it is a map or array all subordinate items
 * will be consumed from the \c decode_context.
 */
QCBORError
qcbor_util_consume_item(QCBORDecodeContext *decode_context,
                        const QCBORItem *item_to_consume,
                        uint_fast8_t *next_nest_level);


/**
 * Descriptor for a single labeled item to be retrieved by
 * qcbor_util_get_items_in_map().  An array of these is passed to
 * qcbor_util_get_items_in_map() terminated by one of these with label
 * 0.
 */
struct qcbor_util_items_to_get_t {
    /**
     * The integer label to search for. List terminated by label 0.
     */
    int64_t   label;
    /**
     * Where the retrieved item is returned. Item.uDataType is
     * QCBOR_TYPE_NONE if not found
     */
    QCBORItem item;
};


/**
 * \brief Search a CBOR map for multiple integer-labeled items.
 *
 * \param[in,out] decode_context  The QCBOR decode context to
 *                                consume and look through.
 * \param[in,out] items           The array of labels to search for
 *                                and the places to return what was
 *                                found. See \ref
 *                                qcbor_util_items_to_get_t.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         The next item in the decode context is not a map.
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         The CBOR is not well-formed.
 * \retval ATTEST_TOKEN_ERR_SUCCESS
 *         Success. This just means the map was searched, not that
 *         anything was found. The contents of \c items must be
 *         checked to see what was found.
 *
 * The next item from \c decode_context must be a map. This is the map
 * that will be searched. Only items at the immediate subordinate
 * level in the map will be checked for label matches. This will
 * consume the all the data items in the map.
 *
 * Note that this cannot be used for finding maps and arrays in at map
 * and decoding them since it consumes them and does not return their
 * contents. qcbor_util_decode_to_labeled_item() is more useful for
 * this.
 *
 * This will ignore any data items that do not have integer labels.
 */
enum attest_token_err_t
qcbor_util_get_items_in_map(QCBORDecodeContext *decode_context,
                            struct qcbor_util_items_to_get_t *items);


/**
 * \brief Decode a map up to a particular label and stop.
 *
 * \param[in,out] decode_context The QCBOR decode context to
 *                               consume and look through.
 * \param[in] label              The label of the item being sought.
 * \param[out] item              The item that is filled in with the
 *                               when the match is found.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         The next item in the decode context is not a map.
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         The CBOR is not well-formed.
 * \retval ATTEST_TOKEN_ERR_SUCCESS
 *         The labeled item was found.
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
           The entire map was consumed without finding \c label.
 *
 * The next item from \c decode_context must be a map. This is the map
 * that will be searched.

 * This will decode consuming data items from \c decode_context until
 * the labeled item is found. It will consume all items in any map or
 * array encountered before finding \c label.
 *
 * Typically this is used to decode to the start of a map or array.
 * The next data item from \c decode_context will be the first item in
 * the map or the array.
 *
 * This works for any CBOR data type, not just maps or arrays.
 *
 * This will ignore any data items that do not have integer labels.
 **/
enum attest_token_err_t
qcbor_util_decode_to_labeled_item(QCBORDecodeContext *decode_context,
                                  int32_t label,
                                  QCBORItem *item);


/**
 * \brief Search a map for one particular integer labeled item.
 *
 * \param[in,out] decode_context The QCBOR decode context to
 *                               consume and look through.
 * \param[in] label              The label of the item being sought.
 * \param[out] item              The item that is filled in when
 *                               the match is found.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR was not well-formed
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         Starting item on \c decode context was not a map.
 * \retval ATTEST_TOKEN_ERR_SUCCESS
 *         The labeled item was found and returned.
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         The entire map was consumed without finding \c label.
 *
 * This is qcbor_util_get_items_in_map() for just one item.
 *
 * This will consume the whole map. It is most useful for getting
 * unstructured data items like integers and strings, but not good for
 * arrays and maps. See qcbor_util_decode_to_labeled_item() for arrays
 * and maps.
 */
enum attest_token_err_t
qcbor_util_get_item_in_map(QCBORDecodeContext *decode_context,
                           int32_t label,
                           QCBORItem *item);


/**
 * \brief Search encoded CBOR from beginning to end for labeled item.
 *
 * \param[in] payload     Encoded chunk of CBOR to decode.
 * \param[in] label       Integer label of item to look for.
 * \param[in] qcbor_type  One of \c QCBOR_TYPE_xxx indicating the
 *                        type of the data item expected.
 * \param[out] item       Place to copy the \c QCBORItem to that
 *                        describes what was found.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *        The labeled item was found, but it didn't match \c qcbor_type.
 * \retval ATTEST_TOKEN_ERR_SUCCESS
 *        The labeled item was found and returned.
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR was not well-formed
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         A map was expected.
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         The entire map was consumed without finding \c label.
 *
 * This will decode the \c payload from beginning to end. If there are
 * extra bytes at the end of it or all the maps and arrays in it are
 * not closed this will return an error.
 *
 * Since this decodes the payload from start to finish to find one
 * item, calling this multiple times to get multiple items will cause
 * the payload to be completely decoded multiple times. This is not as
 * efficient as qcbor_util_get_items_in_map(), but not that costly
 * either.
 *
 * This uses qcbor_util_get_item_in_map() to do its work.
 */
enum attest_token_err_t
qcbor_util_get_top_level_item_in_map(struct q_useful_buf_c payload,
                                     int32_t label,
                                     uint_fast8_t qcbor_type,
                                     QCBORItem *item);


#ifdef __cplusplus
}
#endif

#endif /* __QCBOR_UTILS_H__ */
