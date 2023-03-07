/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "audit_core.h"
#include "psa_audit_defs.h"
#include "tfm_secure_api.h"

/*!
 * \def AUDIT_UART_REDIRECTION
 *
 * \brief If set to 1 by the build system, UART redirection is enabled. Keep it
 *        disabled by default.
 */
#ifndef AUDIT_UART_REDIRECTION
#define AUDIT_UART_REDIRECTION (0U)
#endif

#if (AUDIT_UART_REDIRECTION == 1U)
/* CMSIS Driver for UART */
#include "Driver_USART.h"

#ifndef LOG_UART_NAME
/* Default secure UART name */
#define LOG_UART_NAME Driver_USART1
#endif
extern ARM_DRIVER_USART LOG_UART_NAME;

/*!
 * \def LOG_UART_BAUD_RATE
 *
 * \brief The baud rate used when redirecting the log entry on the secure UART.
 *        The define DEFAULT_UART_BAUDRATE is used.
 */
#ifndef LOG_UART_BAUD_RATE
#define LOG_UART_BAUD_RATE (DEFAULT_UART_BAUDRATE)
#endif

/*!
 * \var log_uart_init_success
 *
 * \brief This variable is 0 in case UART init has failed during service
 *        initialization, 1 otherwise.
 */
static uint8_t log_uart_init_success = 0U;

/*!
 * \var hex_values
 *
 * \brief Array variable used to translate binary to ASCII character
 *        representation for UART redirection
 */
static const char hex_values[] = "0123456789ABCDEF";
#endif

/*!
 * \def MEMBER_SIZE
 *
 * \brief Standard macro to get size of elements in a struct
 */
#define MEMBER_SIZE(type,member) sizeof(((type *)0)->member)

/*!
 * \def LOG_FIXED_FIELD_SIZE
 *
 * \brief Size of the mandatory header fields that are before the info received
 *        from the client partition, i.e.
 *        [TIMESTAMP][IV_COUNTER][PARTITION_ID][SIZE]
 */
#define LOG_FIXED_FIELD_SIZE (MEMBER_SIZE(struct log_hdr, timestamp) + \
                              MEMBER_SIZE(struct log_hdr, iv_counter) + \
                              MEMBER_SIZE(struct log_hdr, partition_id) + \
                              MEMBER_SIZE(struct log_hdr, size))
/*!
 * \def LOG_SIZE
 *
 * \brief Size of the allocated space for the log, in bytes
 *
 * \note Must be a multiple of 8 bytes.
 */
#define LOG_SIZE (1024)

/*!
 * \var log_buffer
 *
 * \brief The private buffer containing the the log in memory
 *
 * \note Aligned to 4 bytes to keep the wrapping on a 4-byte aligned boundary
 */
__attribute__ ((aligned(4)))
static uint8_t log_buffer[LOG_SIZE] = {0};

/*!
 * \var scratch_buffer
 *
 * \brief Scratch buffers needed to hold plain text (and encrypted, if
 *        available) log items to be added
 */
static uint64_t scratch_buffer[(LOG_SIZE)/8] = {0};

/*!
 * \struct log_vars
 *
 * \brief Contains the state variables associated to the current state of the
 *        audit log
 */
struct log_vars {
    uint32_t first_el_idx; /*!< Index in the log of the first element
                                in chronological order */
    uint32_t last_el_idx;  /*!< Index in the log of the last element
                                in chronological order */
    uint32_t num_records;  /*!< Indicates the number of records
                                currently stored in the log. It has to be
                                zero after a reset, i.e. log is empty */
    uint32_t stored_size;  /*!< Indicates the total size of the items
                                currently stored in the log */
};

/*!
 * \var log_state
 *
 * \brief Current state variables for the log
 */
static struct log_vars log_state = {0};

/*!
 * \var global_timestamp
 *
 * \brief Used to have a progressive number attached to each log entry, will be
 *        replaced with a proper timestamping request to TF-M when available
 *
 * \note This is out of the log_state because this is just used to mock up a
 *       timestamping and will be removed later when the final timestamping
 *       mechanism is in place
 */
static uint64_t global_timestamp = 0;

/*!
 * \brief Static inline function to get the log buffer ptr from index
 *
 * \param[in] idx Byte index to be converted to pointer in the log buffer
 *
 * \return Pointer at the beginning of the log item in the log buffer
 */
__attribute__ ((always_inline)) __STATIC_INLINE
struct log_hdr *GET_LOG_POINTER(const uint32_t idx)
{
    return (struct log_hdr *)( &log_buffer[idx] );
}

/*!
 * \brief Static inline function to get the pointer to the SIZE field
 *
 * \param[in] idx Byte index to which retrieve the corresponding size pointer
 *
 * \return Pointer to the size field in the log item header
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t *GET_SIZE_FIELD_POINTER(const uint32_t idx)
{
    return (uint32_t *) GET_LOG_POINTER( (idx+offsetof(struct log_hdr, size))
                                         % LOG_SIZE );
}

/*!
 * \brief Static inline function to compute the full log entry size starting
 *        from the value of the size field
 *
 * \param[in] size Size of the log line from which derive the size of the whole
 *                 log item
 *
 * \return Full log item size
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t COMPUTE_LOG_ENTRY_SIZE(const uint32_t size)
{
    return (LOG_FIXED_FIELD_SIZE + size + LOG_MAC_SIZE);
}

/*!
 * \brief Static inline function to get the index to the base of the log buffer
 *        for the next item with respect to the current item
 *
 * \param[in] idx Byte index of the current item in the log
 *
 * \return Index of the next item in the log
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t GET_NEXT_LOG_INDEX(const uint32_t idx)
{
    return (uint32_t) ( (idx + COMPUTE_LOG_ENTRY_SIZE(
                                   *GET_SIZE_FIELD_POINTER(idx)) ) % LOG_SIZE );
}

/*!
 * \brief Static function to update the state variables of the log after the
 *        addition of a new log record of a given size
 *
 * \param[in] first_el_idx First element index
 * \param[in] last_el_idx  Last element index
 * \param[in] stored_size  New value of the stored size
 * \param[in] num_records  Number of elements stored
 *
 */
static void audit_update_state(const uint32_t first_el_idx,
                               const uint32_t last_el_idx,
                               const uint32_t stored_size,
                               const uint32_t num_records)
{
    /* Update the indexes */
    log_state.first_el_idx = first_el_idx;
    log_state.last_el_idx = last_el_idx;

    /* Update the number of records stored */
    log_state.num_records = num_records;

    /* Update the size of the stored records */
    log_state.stored_size = stored_size;
}

/*!
 * \brief Static function to identify the begin and end position for a new write
 *        into the log. It will replace items based on "older entries first"
 *        policy in case not enough space is available in the log
 *
 * \param[in]  size  Size of the record we need to fit
 * \param[out] begin Pointer to the index to begin
 * \param[out] end   Pointer to the index to end
 *
 */
static void audit_replace_record(const uint32_t size,
                                 uint32_t *begin,
                                 uint32_t *end)
{
    uint32_t first_el_idx = 0, last_el_idx = 0;
    uint32_t num_items = 0, stored_size = 0;
    uint32_t start_pos = 0, stop_pos = 0;

    /* Retrieve the current state variables of the log */
    first_el_idx = log_state.first_el_idx;
    last_el_idx = log_state.last_el_idx;
    num_items = log_state.num_records;
    stored_size = log_state.stored_size;

    /* If there is not enough size, remove older entries */
    while (size > (LOG_SIZE - stored_size)) {

        /* In case we did a full loop without finding space, reset */
        if (num_items == 0) {
            first_el_idx = 0;
            last_el_idx = 0;
            num_items = 0;
            stored_size = 0;
            break;
        }

        /* Remove the oldest */
        stored_size -= COMPUTE_LOG_ENTRY_SIZE(
                           *GET_SIZE_FIELD_POINTER(first_el_idx) );
        num_items--;
        first_el_idx = GET_NEXT_LOG_INDEX(first_el_idx);
    }

    /* Get the start and stop positions */
    if (num_items == 0) {
        start_pos = first_el_idx;
    } else {
        start_pos = GET_NEXT_LOG_INDEX(last_el_idx);
    }
    stop_pos = ((start_pos + COMPUTE_LOG_ENTRY_SIZE(size)) % LOG_SIZE);

    /* Return begin and end positions */
    *begin = start_pos;
    *end = stop_pos;

    /* Update the state with the new values of variables */
    audit_update_state(first_el_idx, last_el_idx, stored_size, num_items);
}

/*!
 * \brief Static function to perform memory copying into the log buffer. It
 *        takes into account circular wrapping on the log buffer size.
 *
 * \param[in]  src  Pointer to the source buffer
 * \param[in]  size Size in bytes to be copied
 * \param[out] dest Pointer to the destination buffer
 *
 */
static psa_status_t audit_buffer_copy(const uint8_t *src,
                                      const uint32_t size,
                                      uint8_t *dest)
{
    uint32_t idx = 0;
    uint32_t dest_idx = (uint32_t)dest - (uint32_t)&log_buffer[0];

    if ((dest_idx >= LOG_SIZE) || (size > LOG_SIZE)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* TODO: This can be an optimized copy using uint32_t
     *       and enforcing the condition that wrapping
     *       happens only on 4-byte boundaries
     */

    for (idx = 0; idx < size; idx++) {
        log_buffer[(dest_idx + idx) % LOG_SIZE] = src[idx];
    }

    return PSA_SUCCESS;
}

/*!
 * \brief Static function to emulate memcpy
 *
 * \param[in]  src  Pointer to the source buffer
 * \param[in]  size Size in bytes to be copied
 * \param[out] dest Pointer to the destination buffer
 *
 */
static psa_status_t audit_memcpy(const uint8_t *src,
                                 const uint32_t size,
                                 uint8_t *dest)
{
    uint32_t idx = 0;

    for (idx = 0; idx < size; idx++) {
        dest[idx] = src[idx];
    }

    return PSA_SUCCESS;
}

/*!
 * \brief Static function to format a log entry before the addition to the log
 *
 * \param[in]  record       Pointer to the record to be added
 * \param[in]  partition_id Value of the partition ID for the partition which
 *                          originated the audit logging request
 * \param[out] buffer       Pointer to the buffer to format
 *
 */
static psa_status_t audit_format_buffer(const struct psa_audit_record *record,
                                        const int32_t partition_id,
                                        uint64_t *buffer)
{
    struct log_hdr *hdr = NULL;
    struct log_tlr *tlr = NULL;
    uint32_t size;
    uint8_t idx;
    psa_status_t status;

    /* Get the size from the record */
    size = record->size;

    /* Format the scratch buffer with the complete log item */
    hdr = (struct log_hdr *) buffer;

    /* FIXME: Timestamping needs to be obtained through Secure Time service, not
     *        yet available. Use a global timestamp for the time being, without
     *        the need to increase the value of iv_counter. In the final
     *        implementation, iv_counter is concatenated to timestamp to get a
     *        12 byte unique IV to be used by the encryption module, and needs
     *        to be increased every time the timestamp didn't change between
     *        consecutive invocations.
     */
    hdr->timestamp = global_timestamp++;
    hdr->iv_counter = 0;
    hdr->partition_id = partition_id;

    /* Copy the record into the scratch buffer */
    status = audit_memcpy((const uint8_t *) record,
                          size+4,
                          (uint8_t *) &(hdr->size));
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* FIXME: The MAC here is just a dummy value for prototyping. It will be
     *        filled by a call to the crypto interface directly when available.
     */
    tlr = (struct log_tlr *) ((uint8_t *)hdr + LOG_FIXED_FIELD_SIZE + size);
    for (idx=0; idx<LOG_MAC_SIZE; idx++) {
        tlr->mac[idx] = idx;
    }

    return PSA_SUCCESS;
}

/*!
 * \brief Static function to stream an entry of the log to a (secure) UART
 *
 * \details The entry of the log is streamed as a stream of hex values,
 *          not parsed nor interpreted.
 *
 * \param[in] start_idx Byte index in the log from where to start streaming
 *            to UART
 *
 */
static void audit_uart_redirection(const uint32_t start_idx)
{
#if (AUDIT_UART_REDIRECTION == 1U)
    uint32_t size = *GET_SIZE_FIELD_POINTER(start_idx);
    uint8_t end_of_line[] = {'\r', '\n'};
    uint32_t idx = 0;
    uint8_t read_byte;

    if (log_uart_init_success == 1U) {
        for (idx=0; idx<COMPUTE_LOG_ENTRY_SIZE(size); idx++) {
            read_byte = log_buffer[(start_idx+idx) % LOG_SIZE];
            (void)LOG_UART_NAME.Send(&hex_values[(read_byte >> 4) & 0xF],1);
            (void)LOG_UART_NAME.Send(&hex_values[read_byte & 0xF], 1);
            (void)LOG_UART_NAME.Send(" ", 1);
        }
        (void)LOG_UART_NAME.Send(&end_of_line, 2);
    }
#endif
}

static psa_status_t _audit_core_get_info(uint32_t *num_records, uint32_t *size)
{
    /* Return the number of records that are currently stored */
    *num_records = log_state.num_records;

    /* Return the size of the records currently stored */
    *size = log_state.stored_size;

    return PSA_SUCCESS;
}

static psa_status_t _audit_core_get_record_info(const uint32_t record_index,
                                                uint32_t *size)
{
    uint32_t start_idx, idx;

    if (record_index >= log_state.num_records) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* First element to read from the log */
    start_idx = log_state.first_el_idx;

    /* Move the start_idx index to the desired element */
    for (idx = 0; idx < record_index; idx++) {
        start_idx = GET_NEXT_LOG_INDEX(start_idx);
    }

    /* Get the size of the requested record */
    *size = COMPUTE_LOG_ENTRY_SIZE(*GET_SIZE_FIELD_POINTER(start_idx));

    return PSA_SUCCESS;
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t audit_core_init(void)
{
#if (AUDIT_UART_REDIRECTION == 1U)
    int32_t ret = ARM_DRIVER_OK;

    ret = LOG_UART_NAME.Initialize(NULL);
    if (ret != ARM_DRIVER_OK) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    ret = LOG_UART_NAME.Control(ARM_USART_MODE_ASYNCHRONOUS,
                                LOG_UART_BAUD_RATE);
    if (ret != ARM_DRIVER_OK) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* If we get to this point, UART init is successful */
    log_uart_init_success = 1U;
#endif

    /* Clear the log state variables */
    audit_update_state(0,0,0,0);

    return PSA_SUCCESS;
}

psa_status_t audit_core_delete_record(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
    uint32_t first_el_idx, size_removed;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(uint32_t)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    const uint32_t record_index = *((uint32_t *)in_vec[0].base);
    const uint8_t *token = in_vec[1].base;
    const uint32_t token_size = in_vec[1].len;

    /* FixMe: Currently only the removal of the oldest entry, i.e.
     *        record_index 0, is supported. This has to be extended
     *        to support removal of random records
     */
    if (record_index > 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* FixMe: Currently token and token_size parameters are not evaluated
     *        to check if the removal of the desired record_index is
     *        authorised
     */
    if ((token != NULL) || (token_size != 0)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Check that the record index to be removed is contained in the log */
    if (record_index >= log_state.num_records) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* If the log contains just one element, reset the state and return */
    if (log_state.num_records == 1) {

        /* Clear the log state variables */
        audit_update_state(0,0,0,0);

        return PSA_SUCCESS;
    }

    /* Get the index to the element to be removed */
    first_el_idx = log_state.first_el_idx;

    /* Get the size of the element that is being removed */
    size_removed = COMPUTE_LOG_ENTRY_SIZE(
                                         *GET_SIZE_FIELD_POINTER(first_el_idx));

    /* Remove the oldest entry, it means moving the first element to the
     * next log index */
    first_el_idx = GET_NEXT_LOG_INDEX(first_el_idx);

    /* Update the state with the new head and decrease the number of records
     * currently stored and the new size of the stored records */
    log_state.first_el_idx = first_el_idx;
    log_state.num_records--;
    log_state.stored_size -= size_removed;

    return PSA_SUCCESS;
}

psa_status_t audit_core_get_info(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
    if ((in_len != 0) || (out_len != 2)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(uint32_t)) ||
	(out_vec[1].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    uint32_t *num_records = out_vec[0].base;
    uint32_t *size = out_vec[1].base;

    /* Return the number of records that are currently stored */
    *num_records = log_state.num_records;

    /* Return the size of the records currently stored */
    *size = log_state.stored_size;

    return PSA_SUCCESS;
}

psa_status_t audit_core_get_record_info(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
    uint32_t start_idx, idx;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(uint32_t)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    const uint32_t record_index = *((uint32_t *)in_vec[0].base);
    uint32_t *size = out_vec[0].base;

    if (record_index >= log_state.num_records) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* First element to read from the log */
    start_idx = log_state.first_el_idx;

    /* Move the start_idx index to the desired element */
    for (idx = 0; idx < record_index; idx++) {
        start_idx = GET_NEXT_LOG_INDEX(start_idx);
    }

    /* Get the size of the requested record */
    *size = COMPUTE_LOG_ENTRY_SIZE(*GET_SIZE_FIELD_POINTER(start_idx));

    return PSA_SUCCESS;
}

psa_status_t audit_core_add_record(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    uint32_t start_pos = 0, stop_pos = 0;
    uint32_t first_el_idx = 0, last_el_idx = 0, size = 0;
    uint32_t num_items = 0, stored_size = 0;
    int32_t partition_id;
    psa_status_t status;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct psa_audit_record)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    const struct psa_audit_record *record = in_vec[0].base;

    /* Get the value of the partition ID of the caller through TFM secure API */
    if (tfm_core_get_caller_client_id(&partition_id) != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    /* Check if the partition ID of the caller is from NS world */
    if (TFM_CLIENT_ID_IS_NS(partition_id)) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    /* Read the size from the input record */
    size = record->size;

    /* Check that size is a 4-byte multiple as expected */
    if (size % 4) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Check that the entry to be added is not greater than the
     * maximum space available
     */
    if (size > (LOG_SIZE - (LOG_FIXED_FIELD_SIZE+LOG_MAC_SIZE))) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Get the size in bytes and num of elements present in the log */
    status = _audit_core_get_info(&num_items, &stored_size);
    if (status !=  PSA_SUCCESS) {
        return status;
    }

    if (num_items == 0) {

        start_pos = 0;

    } else {

        /* The log is not empty, need to decide the candidate position
         * and invalidate older entries in case there is not enough space
         */
        audit_replace_record(COMPUTE_LOG_ENTRY_SIZE(size),
                             &start_pos,
                             &stop_pos);
    }

    /* Format the scratch buffer with the complete log item */
    status = audit_format_buffer(record, partition_id, &scratch_buffer[0]);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* TODO: At this point, encryption should be called if supported */

    /* Do the copy of the log item to be added in the log */
    status = audit_buffer_copy((const uint8_t *) &scratch_buffer[0],
                               COMPUTE_LOG_ENTRY_SIZE(size),
                               (uint8_t *) &log_buffer[start_pos]);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Retrieve current log state */
    first_el_idx = log_state.first_el_idx;
    num_items = log_state.num_records;
    stored_size = log_state.stored_size;

    /* The last element is the one we just added */
    last_el_idx = start_pos;

    /* Update the number of items and stored size */
    num_items++;
    stored_size += COMPUTE_LOG_ENTRY_SIZE(size);

    /* Update the log state */
    audit_update_state(first_el_idx, last_el_idx, stored_size, num_items);

    /* TODO: At this point, we would need to update the stored copy in
     *       persistent storage. Need to define a strategy for this
     */

    /* Stream to a secure UART if available for the platform and built */
    audit_uart_redirection(last_el_idx);

    return PSA_SUCCESS;
}

psa_status_t audit_core_retrieve_record(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
    uint32_t idx, start_idx, record_size_tmp;
    psa_status_t status;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(uint32_t)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const uint32_t record_index = *((uint32_t *)in_vec[0].base);
    const uint8_t *token = in_vec[1].base;
    const uint32_t token_size = in_vec[1].len;
    uint8_t *buffer = out_vec[0].base;
    uint32_t buffer_size = out_vec[0].len;

    /* FixMe: Currently token and token_size parameters are not evaluated
     *        to be used as a challenge for encryption as encryption support
     *        is still not yet available
     */
    if ((token != NULL) || (token_size != 0)) {
        out_vec[0].len = 0;
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Get the size of the record we want to retrieve */
    status = _audit_core_get_record_info(record_index, &record_size_tmp);

    /* Propagate the error to the caller in case of failure */
    if (status != PSA_SUCCESS) {
        out_vec[0].len = 0;
        return status;
    }

    /* buffer_size must be enough to hold the requested record */
    if (buffer_size < record_size_tmp) {
        out_vec[0].len = 0;
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* First element to read from the log */
    start_idx = log_state.first_el_idx;

    /* Move the start_idx index to the desired element */
    for (idx=0; idx<record_index; idx++) {
        start_idx = GET_NEXT_LOG_INDEX(start_idx);
    }

    /* Do the copy */
    for (idx=0; idx<record_size_tmp; idx++) {
        buffer[idx] = log_buffer[(start_idx + idx) % LOG_SIZE];
    }

    /* Update the retrieved size */
    out_vec[0].len = record_size_tmp;

    return PSA_SUCCESS;
}
/*!@}*/
