/*
 * qcbor_util.c
 *
 * Copyright (c) 2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "qcbor_util.h"


/*
 * Public function. See qcbor_util.h
 */
QCBORError
qcbor_util_consume_item(QCBORDecodeContext *decode_context,
                        const QCBORItem    *item_to_consume,
                        uint_fast8_t       *next_nest_level)
{
    QCBORError return_value;
    QCBORItem  item;

    if(item_to_consume->uDataType == QCBOR_TYPE_MAP ||
       item_to_consume->uDataType == QCBOR_TYPE_ARRAY) {
        /* There is only real work to do for maps and arrays */

        /* This works for definite and indefinite length
         * maps and arrays by using the nesting level
         */
        do {
            return_value = QCBORDecode_GetNext(decode_context, &item);
            if(return_value != QCBOR_SUCCESS) {
                goto Done;
            }
        } while(item.uNextNestLevel >= item_to_consume->uNextNestLevel);

        if(next_nest_level != NULL) {
            *next_nest_level = item.uNextNestLevel;
        }
        return_value = QCBOR_SUCCESS;

    } else {
        /* item_to_consume is not a map or array */
        if(next_nest_level != NULL) {
            /* Just pass the nesting level through */
            *next_nest_level = item_to_consume->uNextNestLevel;
        }
        return_value = QCBOR_SUCCESS;
    }

Done:
    return return_value;
}


/*
 * Public function. qcbor_util.h
 */
enum attest_token_err_t
qcbor_util_get_items_in_map(QCBORDecodeContext *decode_context,
                            struct qcbor_util_items_to_get_t *items_found)
{
    QCBORItem                         item;
    struct qcbor_util_items_to_get_t  *iterator;
    enum attest_token_err_t           return_value;
    uint_fast8_t                      map_nest_level;
    uint_fast8_t                      next_nest_level;

    /* Clear structure holding the items found */
    for(iterator = items_found; iterator->label != 0; iterator++) {
        iterator->item.uDataType = QCBOR_TYPE_NONE;
    }

    /* Get the data item that is the map that is being searched */
    QCBORDecode_GetNext(decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_MAP) {
        return_value = ATTEST_TOKEN_ERR_CBOR_STRUCTURE;
        goto Done;
    }

    /* Loop over all the items in the map. The map may contain further
     * maps and arrays. This also needs to handle definite and
     * indefinite length maps and array.
     *
     * map_nest_level is the nesting level of the data item opening
     * the map that is being scanned. All data items inside this map
     * have a nesting level greater than it. The data item following
     * the map being scanned has a nesting level that is equal to or
     * higher than map_nest_level.
     */
    map_nest_level  = item.uNestingLevel;
    next_nest_level = item.uNextNestLevel;

    while(next_nest_level > map_nest_level) {
        if(QCBORDecode_GetNext(decode_context, &item) != QCBOR_SUCCESS) {
            /* Got non-well-formed CBOR */
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        /* Only look at labels that are integers */
        if(item.uLabelType == QCBOR_TYPE_INT64) {
            /* See if it is one we are looking for */
            for(iterator = items_found; iterator->label != 0; iterator++) {
                if(item.label.int64 == iterator->label) {
                    /* It is one we are looking for. Record it.
                     * This was the point of the whole loop! */
                    iterator->item = item;
                }
            }
        }

        /* Only looking at top-level data items, so just consume any
         * map or array encountered.*/
        if(qcbor_util_consume_item(decode_context, &item, &next_nest_level)) {
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }
    }
    return_value = ATTEST_TOKEN_ERR_SUCCESS;

Done:
    return return_value;
}


/*
 * Public function. See qcbor_util.h
 */
enum attest_token_err_t
qcbor_util_decode_to_labeled_item(QCBORDecodeContext *decode_context,
                                  int32_t label,
                                  QCBORItem *item)
{
    QCBORItem               map_item;
    enum attest_token_err_t return_value;

    return_value = ATTEST_TOKEN_ERR_SUCCESS;

    QCBORDecode_GetNext(decode_context, &map_item);
    if(map_item.uDataType != QCBOR_TYPE_MAP) {
        /* Isn't a map */
        return_value = ATTEST_TOKEN_ERR_CBOR_STRUCTURE;
        goto Done;
    }

    /* Loop over all the items in the map */
    while(1) {
        if(QCBORDecode_GetNext(decode_context, item) != QCBOR_SUCCESS) {
            /* Got non-well-formed CBOR */
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        /* Only look at labels that are integers */
        if(item->uLabelType == QCBOR_TYPE_INT64) {
            /* See if it is one we are looking for */
            if(item->label.int64 == label) {
                /* This is successful exit from the loop */
                return_value = ATTEST_TOKEN_ERR_SUCCESS;
                goto Done;
            }
        }

        /* Only looking at top-level data items, so just consume any
         * map or array encountered */
        if(qcbor_util_consume_item(decode_context, item, NULL)) {
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        if(item->uNextNestLevel < map_item.uNextNestLevel) {
            /* Fetched last item in the map without
             * finding what was requested */
            return_value = ATTEST_TOKEN_ERR_NOT_FOUND;
            goto Done;
        }
    }

Done:
    return return_value;
}


/*
 * Public function. See qcbor_util.h
 */
enum attest_token_err_t
qcbor_util_get_item_in_map(QCBORDecodeContext *decode_context,
                           int32_t label,
                           QCBORItem *item)
{
    struct qcbor_util_items_to_get_t  one_item[2];
    enum attest_token_err_t           return_value;

    one_item[0].label = label;
    one_item[1].label = 0; /* Terminator for search list */

    return_value = qcbor_util_get_items_in_map(decode_context, one_item);
    if(return_value) {
        goto Done;
    }

    if(one_item[0].item.uDataType == QCBOR_TYPE_NONE) {
        return_value = ATTEST_TOKEN_ERR_NOT_FOUND;
        goto Done;
    }

    *item = one_item[0].item;

Done:
    return return_value;
}


/*
 * Public function. See qcbor_util.h
 */
enum attest_token_err_t
qcbor_util_get_top_level_item_in_map(struct q_useful_buf_c payload,
                                     int32_t label,
                                     uint_fast8_t qcbor_type,
                                     QCBORItem *item)
{
    enum attest_token_err_t return_value;
    QCBORItem               found_item;
    QCBORDecodeContext      decode_context;
    QCBORError              cbor_error;

    if(q_useful_buf_c_is_null(payload)) {
        return_value = ATTEST_TOKEN_ERR_COSE_SIGN1_VALIDATION;
        goto Done;
    }

    QCBORDecode_Init(&decode_context, payload, QCBOR_DECODE_MODE_NORMAL);

    return_value = qcbor_util_get_item_in_map(&decode_context,
                                              label,
                                              &found_item);
    if(return_value != ATTEST_TOKEN_ERR_SUCCESS) {
        goto Done;
    }

    cbor_error = QCBORDecode_Finish(&decode_context);
    if(cbor_error != QCBOR_SUCCESS) {
        if(cbor_error == QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
            return_value = ATTEST_TOKEN_ERR_CBOR_STRUCTURE;
        } else {
            /* This is usually due to extra bytes at the end */
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
        }
        goto Done;
    }

    if(found_item.uDataType != qcbor_type) {
        return_value = ATTEST_TOKEN_ERR_CBOR_TYPE;
        goto Done;
    }
    *item = found_item;

Done:
    return return_value;
}

