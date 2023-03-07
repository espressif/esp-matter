/*
 * t_cose_parameters.h
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#ifndef t_cose_parameters_h
#define t_cose_parameters_h

#include "t_cose_sign1_verify.h"
#include "q_useful_buf.h"
#include "t_cose_common.h"
#include <stdint.h>
#include "qcbor.h"


/**
 * \file t_cose_parameters.h
 *
 * \brief A list of COSE parameter labels, both integer and string.
 *
 * It is fixed size to avoid the complexity of memory management and
 * because the number of parameters is assumed to be small.
 *
 * On a 64-bit machine it is 24 * PARAMETER_LIST_MAX which is 244
 * bytes. That accommodates 10 string parameters and 10 integer parameters
 * and is small enough to go on the stack.
 *
 * On a 32-bit machine: 16 * PARAMETER_LIST_MAX = 176
 *
 * This is a big consumer of stack in this implementation.  Some
 * cleverness with a union could save almost 200 bytes of stack, as
 * this is on the stack twice.
 */
struct t_cose_label_list {
    /* Terminated by value LABEL_LIST_TERMINATOR */
    int64_t int_labels[T_COSE_PARAMETER_LIST_MAX+1];
    /*  Terminated by a NULL_Q_USEFUL_BUF_C */
    struct q_useful_buf_c tstr_labels[T_COSE_PARAMETER_LIST_MAX+1];
};


/*
 * The IANA COSE Header Parameters registry lists label 0 as
 * "reserved". This means it can be used, but only by a revision of
 * the COSE standard if it is deemed necessary for some large and good
 * reason. It cannot just be allocated by IANA as any normal
 * assignment. See [IANA COSE Registry]
 * (https://www.iana.org/assignments/cose/cose.xhtml).  It is thus
 * considered safe to use as the list terminator.
 */
#define LABEL_LIST_TERMINATOR 0


/**
 * \brief Clear a label list to empty.
 *
 * \param[in,out] list The list to clear.
 */
static void
clear_label_list(struct t_cose_label_list *list);


/**
 * \brief Indicate whether label list is clear or not.
 *
 * \param[in,out] list  The list to check.
 *
 * \return true if the list is clear.
 */
static bool
is_label_list_clear(const struct t_cose_label_list *list);


/**
 * \brief Check the unknown parameters against the critical labels list.
 *
 * \param[in] critical_labels  The list of critical labels.
 * \param[in] unknown_labels   The parameter labels that occurred.
 *
 * \retval T_COSE_SUCCESS                         None of the unknown labels
 *                                                are critical.
 * \retval T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER  At least one of the unknown
 *                                                labels is critical.
 *
 * Both lists are of parameter labels (CBOR keys). Check to see none of
 * the parameter labels in the unknown list occur in the critical list.
 */
enum t_cose_err_t
check_critical_labels(const struct t_cose_label_list *critical_labels,
                      const struct t_cose_label_list *unknown_labels);



/**
 * \brief Parse the unprotected COSE header parameters.
 *
 * \param[in] decode_context        Decode context to read the parameters from.
 * \param[out] returned_parameters  The parsed parameters.
 *
 * \returns The same as parse_cose_header_parameters().
 *
 * No parameters are mandatory. Which parameters were present or not is
 * indicated in \c returned_parameters.  It is OK for there to be no
 * parameters at all.
 *
 * The first item to be read from the decode_context must be the map
 * data item that contains the parameters.
 */
enum t_cose_err_t
parse_unprotected_header_parameters(QCBORDecodeContext       *decode_context,
                                    struct t_cose_parameters *returned_parameters,
                                    struct t_cose_label_list *unknown);


/**
 * \brief Parse the protected header parameters.
 *
 * \param[in] protected_parameters  Pointer and length of CBOR-encoded
 *                                  protected parameters to parse.
 * \param[out] returned_parameters  The parsed parameters that are returned.
 *
 * \retval T_COSE_SUCCESS                  Protected parameters were parsed.
 * \retval T_COSE_ERR_CBOR_NOT_WELL_FORMED The CBOR formatting of the protected
 *                                         parameters is unparsable.
 *
 * This parses the contents of the protected header parameters after the bstr
 * wrapping is removed.
 *
 * This will error out if the CBOR is not well-formed, the protected
 * header parameters are not a map, the algorithm ID is not found, or the
 * algorithm ID is larger than \c INT32_MAX or smaller than \c
 * INT32_MIN.
 */
enum t_cose_err_t
parse_protected_header_parameters(const struct q_useful_buf_c protected_parameters,
                                  struct t_cose_parameters   *returned_parameters,
                                  struct t_cose_label_list   *critical,
                                  struct t_cose_label_list   *unknown);


/**
 * \brief Copy and combine protected and unprotected parameters.
 *
 * \param[in] protected             The protected header parameters to copy.
 * \param[in] unprotected           The unprotected header parameters to copy.
 * \param[out] returned_parameters  Destination for copy.
 *
 * \retval T_COSE_ERR_DUPLICATE_PARAMETER  If the same parameter occurs in both
 *                                         protected and unprotected.
 * \retval T_COSE_SUCCESS                  If there were no duplicates and the
 *                                         copy and combine succeeded.
 *
 * This merges the protected and unprotected parameters. The COSE standard
 * does not allow a parameter to be duplicated in protected and unprotected so
 * this checks and returns an error if so.
 */
enum t_cose_err_t
check_and_copy_parameters(const struct t_cose_parameters  *protected,
                          const struct t_cose_parameters  *unprotected,
                          struct t_cose_parameters        *returned_parameters);



/* ------------------------------------------------------------------------
 * Inline implementations of public functions defined above.
 */
inline static void clear_label_list(struct t_cose_label_list *list)
{
    memset(list, 0, sizeof(struct t_cose_label_list));
}


inline static bool
is_label_list_clear(const struct t_cose_label_list *list)
{
    return list->int_labels[0] == 0 &&
           q_useful_buf_c_is_null_or_empty(list->tstr_labels[0]);
}

#endif /* t_cose_parameters_h */
