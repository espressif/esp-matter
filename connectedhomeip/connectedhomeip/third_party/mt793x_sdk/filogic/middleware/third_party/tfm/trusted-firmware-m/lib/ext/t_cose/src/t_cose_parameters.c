/*
 * t_cose_parameters.c
 *
 * Copyright 2019-2020, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#include "t_cose_parameters.h"
#include "t_cose_standard_constants.h"


/**
 * \file t_cose_headers.c
 *
 * \brief Implementation of the header parsing functions.
 *
 */


/**
 * \brief Consume a CBOR item, particularly a map or array.
 *
 * \param[in] decode_context   Context to read data items from.
 * \param[in] item_to_consume  The already-read item that is being consumed.
 * \param[out] next_nest_level Nesting level of the next item that will be read.
 *
 * \returns A CBOR decoding error or QCBOR_SUCCESS.
 *
 * The primary purpose of this is to consume (read) all the members of
 * a map or an array, however deeply nested it is.
 *
 * This doesn't do much work for non-nested data items.
 */
static inline QCBORError
consume_item(QCBORDecodeContext *decode_context,
             const QCBORItem    *item_to_consume,
             uint_fast8_t       *next_nest_level)
{
    /* Stack use: 4 + 56 = 60 */
    QCBORError return_value;
    QCBORItem  item;

    if(item_to_consume->uDataType == QCBOR_TYPE_MAP || item_to_consume->uDataType == QCBOR_TYPE_ARRAY) {
        /* There is only real work to do for maps and arrays */

        /* This works for definite and indefinite length maps and
         * arrays by using the nesting level
         */
        do {
            return_value = QCBORDecode_GetNext(decode_context, &item);
            if(return_value != QCBOR_SUCCESS) {
                goto Done;
            }
        } while(item.uNextNestLevel >= item_to_consume->uNextNestLevel);

        *next_nest_level = item.uNextNestLevel;
        return_value = QCBOR_SUCCESS;

    } else {
        /* item_to_consume is not a map or array.  Just pass the
         * nesting level through */
        *next_nest_level = item_to_consume->uNextNestLevel;
        return_value = QCBOR_SUCCESS;
    }

Done:
    return return_value;
}




/**
 * \brief Add a new label to the end of the label list.
 *
 * \param[in] item             Data item to add to the label list.
 * \param[in,out] label_list   The list to add to.
 *
 * \retval T_COSE_SUCCESS                  If added correctly.
 * \retval T_COSE_ERR_TOO_MANY_PARAMETERS  Label list is full.
 * \retval T_COSE_ERR_PARAMETER_CBOR       The item to add doesn't have a label
 *                                         type that is understood
 *
 * The label / key from \c item is added to \c label_list.
 */
static inline enum t_cose_err_t
add_label_to_list(const QCBORItem          *item,
                  struct t_cose_label_list *label_list)
{
    enum t_cose_err_t return_value;
    uint_fast8_t      n;

    /* Assume success until an error adding is encountered. */
    return_value = T_COSE_SUCCESS;

    if(item->uLabelType == QCBOR_TYPE_INT64) {
        /* Add an integer-labeled parameter to the end of the list */
        for(n = 0; label_list->int_labels[n] != LABEL_LIST_TERMINATOR; n++);
        if(n == T_COSE_PARAMETER_LIST_MAX) {
            /* List is full -- error out */
            return_value = T_COSE_ERR_TOO_MANY_PARAMETERS;
            goto Done;
        }
        label_list->int_labels[n] = item->label.int64;

    } else if(item->uLabelType == QCBOR_TYPE_TEXT_STRING) {
        /* Add a string-labeled parameter to the end of the list */
        for(n = 0; !q_useful_buf_c_is_null(label_list->tstr_labels[n]); n++);
        if(n == T_COSE_PARAMETER_LIST_MAX) {
            /* List is full -- error out */
            return_value = T_COSE_ERR_TOO_MANY_PARAMETERS;
            goto Done;
        }
        label_list->tstr_labels[n] = item->label.string;
    } else {
        /* error because label is neither integer or string */
        /* Should never occur because this is caught earlier, but
         * leave it to be safe and because inlining and optimization
         * should take out any unneeded code
         */
        return_value = T_COSE_ERR_PARAMETER_CBOR;
    }

Done:
    return return_value;
}




/**
 * \brief Decode the parameter containing the labels of parameters considered
 *        critical.
 *
 * \param[in,out]  decode_context          Decode context to read critical
 *                                         parameter list from.
 * \param[in]      crit_parameter_item     Data item of array holding critical
 *                                         labels.
 * \param[out]     critical_labels         List of labels of critical
 *                                         parameters.
 * \param[out]     return_next_nest_level  Place to return nesting level of
 *                                         next data item.
 *
 * \retval T_COSE_ERR_CBOR_NOT_WELL_FORMED  Undecodable CBOR.
 * \retval T_COSE_ERR_TOO_MANY_PARAMETERS   More critical labels than this
 *                                          implementation can handle.
 * \retval T_COSE_ERR_PARAMETER_CBOR        Unexpected CBOR data type.
 */
static inline enum t_cose_err_t
decode_critical_parameter(QCBORDecodeContext       *decode_context,
                          const QCBORItem          *crit_parameter_item,
                          struct t_cose_label_list *critical_labels,
                          uint_fast8_t             *return_next_nest_level)
{
    /* Stack use 64-bit: 56 + 40 = 96
     *           32-bit: 52 + 20 = 72
     */
    QCBORItem         item;
    uint_fast8_t      num_int_labels;
    uint_fast8_t      num_tstr_labels;
    enum t_cose_err_t return_value;
    QCBORError        cbor_result;
    uint_fast8_t      next_nest_level;
    uint_fast8_t      array_nest_level;

    num_int_labels  = 0;
    num_tstr_labels = 0;

    array_nest_level = crit_parameter_item->uNestingLevel;
    next_nest_level  = crit_parameter_item->uNextNestLevel;

    if(crit_parameter_item->uDataType != QCBOR_TYPE_ARRAY) {
        return_value = T_COSE_ERR_CRIT_PARAMETER;
        goto Done;
    }

    while(next_nest_level > array_nest_level) {
        cbor_result = QCBORDecode_GetNext(decode_context, &item);
        if(cbor_result != QCBOR_SUCCESS) {
            return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        if(item.uDataType == QCBOR_TYPE_INT64) {
            if(num_int_labels >= T_COSE_PARAMETER_LIST_MAX) {
                return_value = T_COSE_ERR_CRIT_PARAMETER;
                goto Done;
            }
            critical_labels->int_labels[num_int_labels++] = item.val.int64;
        } else if(item.uDataType == QCBOR_TYPE_TEXT_STRING) {
            if(num_tstr_labels >= T_COSE_PARAMETER_LIST_MAX) {
                return_value = T_COSE_ERR_CRIT_PARAMETER;
                goto Done;
            }
            critical_labels->tstr_labels[num_tstr_labels++] = item.val.string;
        } else {
            return_value = T_COSE_ERR_CRIT_PARAMETER;
            goto Done;
        }
        next_nest_level = item.uNextNestLevel;
    }

    if(is_label_list_clear(critical_labels)) {
        /* Per RFC 8152 crit parameter can't be empty */
        return_value = T_COSE_ERR_CRIT_PARAMETER;
        goto Done;
    }

    return_value = T_COSE_SUCCESS;

Done:
    *return_next_nest_level = next_nest_level;
    return return_value;
}


/**
 * Public function. See t_cose_parameters.h
 */
enum t_cose_err_t
check_critical_labels(const struct t_cose_label_list *critical_labels,
                      const struct t_cose_label_list *unknown_labels)
{
    enum t_cose_err_t return_value;
    uint_fast8_t      num_unknown;
    uint_fast8_t      num_critical;

    /* Assume success until an unhandled critical label is found */
    return_value = T_COSE_SUCCESS;

    /* Iterate over unknown integer parameters */
    for(num_unknown = 0; unknown_labels->int_labels[num_unknown]; num_unknown++) {
        /* Iterate over critical int labels looking for the unknown label */
        for(num_critical = 0;
            critical_labels->int_labels[num_critical];
            num_critical++) {
            if(critical_labels->int_labels[num_critical] == unknown_labels->int_labels[num_unknown]) {
                /* Found a critical label that is unknown to us */
                return_value = T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER;
                goto Done;
            }
        }
        /* Exit from loop here means all no unknown label was critical */
    }

    /* Iterate over unknown string labels */
    for(num_unknown = 0; !q_useful_buf_c_is_null(unknown_labels->tstr_labels[num_unknown]); num_unknown++) {
        /* iterate over critical string labels looking for the unknown param */
        for(num_critical = 0; !q_useful_buf_c_is_null(critical_labels->tstr_labels[num_critical]); num_critical++) {
            if(!q_useful_buf_compare(critical_labels->tstr_labels[num_critical],
                                     unknown_labels->tstr_labels[num_unknown])){
                /* Found a critical label that is unknown to us */
                return_value = T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER;
                goto Done;
            }
        }
        /* Exit from loop here means all no unknown label was critical */
    }

Done:
    return return_value;
}




/**
 * \brief Add unknown parameter to unknown labels list and fully consume it
 *
 * \param[in] decode_context       CBOR decode context to read from.
 * \param[in] unknown_parameter    The data item for the unknown parameter.
 * \param[in,out] unknown_labels   The list of unknown labels to which to add
 *                                 this new unknown label to.
 * \param[out] next_nest_level     The nest level of the next item that will be
 *                                 fetched. Helps to know if at end of list.
 *
 * \retval T_COSE_ERR_CBOR_NOT_WELL_FORMED  The CBOR is not well-formed.
 * \retval T_COSE_ERR_TOO_MANY_PARAMETERS   The unknown labels list is full.
 * \retval T_COSE_ERR_CBOR_STRUCTURE        The CBOR structure not as expected.
 */
static enum t_cose_err_t
process_unknown_parameter(QCBORDecodeContext        *decode_context,
                          const QCBORItem           *unknown_parameter,
                          struct t_cose_label_list *unknown_labels,
                          uint_fast8_t              *next_nest_level)
{
    enum t_cose_err_t return_value;

    return_value = add_label_to_list(unknown_parameter, unknown_labels);
    if(return_value) {
        goto Done;
    }

    /* The full unknown parameter must be consumed. It could be
     complex deeply-nested CBOR */
    if(consume_item(decode_context, unknown_parameter, next_nest_level)) {
        return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
    }

Done:
    return return_value;
}




/**
 * \brief Clear a struct t_cose_parameters to empty
 *
 * \param[in,out] parameters   Parameter list to clear.
 */
static inline void clear_cose_parameters(struct t_cose_parameters *parameters)
{
#if COSE_ALGORITHM_RESERVED != 0
#error Invalid algorithm designator not 0. Parameter list initialization fails.
#endif

#if T_COSE_UNSET_ALGORITHM_ID != COSE_ALGORITHM_RESERVED
#error Constant for unset algorithm ID not aligned with COSE_ALGORITHM_RESERVED
#endif

    /* This clears all the useful_bufs to NULL_Q_USEFUL_BUF_C
     * and the cose_algorithm_id to COSE_ALGORITHM_RESERVED
     */
    memset(parameters, 0, sizeof(struct t_cose_parameters));

#ifndef T_COSE_DISABLE_CONTENT_TYPE
    /* The only non-zero clear-state value. (0 is plain text in CoAP
     * content format) */
    parameters->content_type_uint =  T_COSE_EMPTY_UINT_CONTENT_TYPE;
#endif
}


/**
 * \brief Parse some COSE header parameters.
 *
 * \param[in] decode_context        The QCBOR decode context to read from.
 * \param[out] returned_parameters  The parsed parameters being returned.
 *
 * \retval T_COSE_SUCCESS                     The parameters were decoded
 *                                            correctly.
 * \retval T_COSE_ERR_PARAMETER_CBOR          CBOR is parsable, but not the
 *                                            right structure (e.g. array
 *                                            instead of a map)
 * \retval T_COSE_ERR_TOO_MANY_PARAMETERS     More than
 *                                            \ref T_COSE_PARAMETER_LIST_MAX
 *                                            parameters.
 * \retval T_COSE_ERR_CBOR_NOT_WELL_FORMED    The CBOR is not parsable.
 * \retval T_COSE_ERR_NON_INTEGER_ALG_ID      The algorithm ID is not an
 *                                            integer. This implementation
 *                                            doesn't support string algorithm
 *                                            IDs.
 * \retval T_COSE_ERR_BAD_CONTENT_TYPE        Error in content type parameter.
 * \retval T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER   A label marked critical is
 *                                                 present and not understood.
 *
 * No parameters are mandatory. Which parameters were present or not
 * is indicated in \c returned_parameters.  It is OK for there to be
 * no parameters at all.
 *
 * The first item to be read from the decode_context must be the map
 * data item that contains the parameters.
 */
static enum t_cose_err_t
parse_cose_header_parameters(QCBORDecodeContext        *decode_context,
                             struct t_cose_parameters  *returned_parameters,
                             struct t_cose_label_list  *critical_labels,
                             struct t_cose_label_list  *unknown_labels)
{
    /* Local stack use 64-bit: 56 + 24 + 488 = 568
     * Local stack use 32-bit: 52 + 12 + 352 = 414
     * Total stack use 64-bit: 568 + 96 + 50 = 694
     * Total stack use 32-bit: 414 + 72 + 25 = 501
     */
    QCBORItem          item;
    enum t_cose_err_t  return_value;
    uint_fast8_t       map_nest_level;
    uint_fast8_t       next_nest_level;
    QCBORError         qcbor_result;

    clear_cose_parameters(returned_parameters);

    if(critical_labels != NULL) {
        clear_label_list(critical_labels);
    }

    /* Get the data item that is the map that is being searched */
    qcbor_result = QCBORDecode_GetNext(decode_context, &item);
    if(qcbor_result == QCBOR_ERR_NO_MORE_ITEMS) {
        return_value = T_COSE_SUCCESS;
        goto Done;
    }
    if(qcbor_result != QCBOR_SUCCESS) {
        return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
        goto Done;
    }
    if(item.uDataType != QCBOR_TYPE_MAP) {
        return_value = T_COSE_ERR_PARAMETER_CBOR;
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
            /* Got not-well-formed CBOR */
            return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        if(item.uLabelType != QCBOR_TYPE_INT64) {
            /* Non integer label. We don't handle those. */
            return_value = process_unknown_parameter(decode_context,
                                                  &item,
                                                  unknown_labels,
                                                  &next_nest_level);
            if(return_value) {
                goto Done;
            }

        } else {
            next_nest_level = item.uNextNestLevel;
            switch(item.label.int64) {

            case COSE_HEADER_PARAM_ALG:
                if(critical_labels == NULL) {
                    return_value = T_COSE_ERR_PARAMETER_NOT_PROTECTED;
                    goto Done;
                }
                if(item.uDataType != QCBOR_TYPE_INT64) {
                    return_value = T_COSE_ERR_NON_INTEGER_ALG_ID;
                    goto Done;
                }
                if(item.val.int64 == COSE_ALGORITHM_RESERVED || item.val.int64 > INT32_MAX) {
                    return_value = T_COSE_ERR_NON_INTEGER_ALG_ID;
                    goto Done;
                }
                if(returned_parameters->cose_algorithm_id != COSE_ALGORITHM_RESERVED) {
                    return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                    goto Done;
                }
                returned_parameters->cose_algorithm_id = (int32_t)item.val.int64;
                break;

            case COSE_HEADER_PARAM_KID:
                if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return_value = T_COSE_ERR_PARAMETER_CBOR;
                    goto Done;
                }
                if(!q_useful_buf_c_is_null_or_empty(returned_parameters->kid)) {
                    return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                    goto Done;
                }
                returned_parameters->kid = item.val.string;
                break;

            case COSE_HEADER_PARAM_IV:
                if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return_value = T_COSE_ERR_PARAMETER_CBOR;
                    goto Done;
                }
                if(!q_useful_buf_c_is_null_or_empty(returned_parameters->iv)) {
                    return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                    goto Done;
                }
                returned_parameters->iv = item.val.string;
                break;

            case COSE_HEADER_PARAM_PARTIAL_IV:
                if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return_value = T_COSE_ERR_PARAMETER_CBOR;
                    goto Done;
                }
                if(!q_useful_buf_c_is_null_or_empty(returned_parameters->partial_iv)) {
                    return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                    goto Done;
                }
                returned_parameters->partial_iv = item.val.string;
                break;

            case COSE_HEADER_PARAM_CRIT:
                if(critical_labels == NULL) {
                    /* crit parameter occuring in non-protected bucket */
                    return_value = T_COSE_ERR_PARAMETER_NOT_PROTECTED;
                    goto Done;
                }
                if(!is_label_list_clear(critical_labels)) {
                    /* Duplicate detection must be here because it is not
                     * done in check_and_copy_parameters()
                     */
                    return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                    goto Done;
                }
                /* decode_critical_parameter() consumes all the items in the
                 * crit parameter array */
                return_value = decode_critical_parameter(decode_context,
                                                        &item,
                                                         critical_labels,
                                                        &next_nest_level);
                if(return_value) {
                    goto Done;
                }
                break;

#ifndef T_COSE_DISABLE_CONTENT_TYPE
            case COSE_HEADER_PARAM_CONTENT_TYPE:
                if(item.uDataType == QCBOR_TYPE_TEXT_STRING) {
                    if(!q_useful_buf_c_is_null_or_empty(returned_parameters->content_type_tstr)) {
                        return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                        goto Done;
                    }
                    returned_parameters->content_type_tstr = item.val.string;
                } else if(item.uDataType == QCBOR_TYPE_INT64) {
                    if(item.val.int64 < 0 || item.val.int64 > UINT16_MAX) {
                        return_value = T_COSE_ERR_BAD_CONTENT_TYPE;
                        goto Done;
                    }
                    if(returned_parameters->content_type_uint != T_COSE_EMPTY_UINT_CONTENT_TYPE) {
                        return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
                        goto Done;
                    }
                    returned_parameters->content_type_uint = (uint32_t)item.val.int64;
                } else {
                    return_value = T_COSE_ERR_BAD_CONTENT_TYPE;
                    goto Done;
                }
                break;
#endif

            default:
                /* The parameter is not recognized. Its label has to
                 * be added to the the list of unknown labels so it
                 * can be checked against the list of critical labels.
                 */
                return_value = process_unknown_parameter(decode_context,
                                                        &item,
                                                         unknown_labels,
                                                        &next_nest_level);
                if(return_value) {
                    goto Done;
                }
                break;
            }
        }
    }
    return_value = T_COSE_SUCCESS;

Done:
    return return_value;
}


/**
 * Public function. See t_cose_parameters.h
 */
enum t_cose_err_t
parse_protected_header_parameters(const struct q_useful_buf_c encoded_protected_parameters,
                                  struct t_cose_parameters   *returned_params,
                                  struct t_cose_label_list   *critical_labels,
                                  struct t_cose_label_list   *unknown)
{
    /* Local stack use 64-bit: 144 + 8 = 152
     * Local stack use 32-bit: 108 + 4 = 112
     * Total stack use 64-bit: 694 + 144 = 838
     * Total stack use 32-bit: 501 + 112 = 613
     */
    QCBORDecodeContext decode_context;
    enum t_cose_err_t  return_value;

    QCBORDecode_Init(&decode_context, encoded_protected_parameters, QCBOR_DECODE_MODE_NORMAL);

    return_value = parse_cose_header_parameters(&decode_context,
                                                 returned_params,
                                                 critical_labels,
                                                 unknown);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }

    if(QCBORDecode_Finish(&decode_context)) {
        /* A CBOR error here is always not-well-formed */
        return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
    }

Done:
    return return_value;
}


/*
 * Static inline implementation. See documentation above.
 */
enum t_cose_err_t
parse_unprotected_header_parameters(QCBORDecodeContext *decode_context,
                                    struct t_cose_parameters *returned_params,
                                    struct t_cose_label_list *unknown_labels)
{
    return parse_cose_header_parameters(decode_context,
                                        returned_params,
                                        NULL,
                                        unknown_labels);
}


/**
 * Public function. See t_cose_parameters.h
 */
enum t_cose_err_t
check_and_copy_parameters(const struct t_cose_parameters  *protected,
                          const struct t_cose_parameters  *unprotected,
                          struct t_cose_parameters        *returned_params)
{
    enum t_cose_err_t return_value;

    /* -- Copy all the unprotected parameters -- */
    if(returned_params) {
        *returned_params = *unprotected;
    }

    /* Go one at at time and check the protected parameters. If the
     * parameter is not NULL and there is the same un protected
     * parameter error out. If it is not NULL and there is no
     * unprotected parameter, copy it */
    if(protected->cose_algorithm_id != COSE_ALGORITHM_RESERVED) {
        if(unprotected->cose_algorithm_id != COSE_ALGORITHM_RESERVED) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->cose_algorithm_id = protected->cose_algorithm_id;
        }
    }

    if(!q_useful_buf_c_is_null_or_empty(protected->kid)) {
        if(!q_useful_buf_c_is_null_or_empty(unprotected->kid)) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->kid = protected->kid;
        }
    }

    if(!q_useful_buf_c_is_null_or_empty(protected->iv)) {
        if( !q_useful_buf_c_is_null_or_empty(unprotected->iv)) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->iv = protected->iv;
        }
    }

    if(!q_useful_buf_c_is_null_or_empty(protected->partial_iv)) {
        if( !q_useful_buf_c_is_null_or_empty(unprotected->partial_iv)) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->partial_iv = protected->partial_iv;
        }
    }

#ifndef T_COSE_DISABLE_CONTENT_TYPE
    if(!q_useful_buf_c_is_null_or_empty(protected->content_type_tstr)) {
        if( !q_useful_buf_c_is_null_or_empty(unprotected->content_type_tstr)) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->content_type_tstr = protected->content_type_tstr;
        }
    }

    if(protected->content_type_uint != T_COSE_EMPTY_UINT_CONTENT_TYPE) {
        if(unprotected->content_type_uint != T_COSE_EMPTY_UINT_CONTENT_TYPE) {
            return_value = T_COSE_ERR_DUPLICATE_PARAMETER;
            goto Done;
        }
        if(returned_params) {
            returned_params->content_type_uint = protected->content_type_uint;
        }
    }
#endif

    return_value = T_COSE_SUCCESS;

Done:
    return return_value;
}

