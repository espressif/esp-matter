/**
 * @file
 *
 * Invokes a CC handler based on the first byte (CC) in a given frame.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZAF_CC_INVOKER_H_
#define ZAF_CC_INVOKER_H_

#include "ZAF_types.h"
#include "ZW_classcmd.h"

/**
 * Invokes the handler with the correct arguments for a given command class entry.
 *
 * Command class handlers can be implemented in different versions having different parameters.
 * This function will invoke the handler with the correct set of arguments based on the handler
 * version.
 *
 * @param[in] p_cc_entry  Pointer to command class entry.
 * @param[in] rxOpt       Receive options.
 * @param[in] pFrameIn    Received frame.
 * @param[in] cmdLength   Length of received frame.
 * @param[out] pFrameOut  Outgoing frame.
 * @param[out] pLengthOut Length of outgoing frame.
 * @return Returns RECEIVED_FRAME_STATUS_CC_NOT_FOUND if the handler version is not supported.
 *         Otherwise, the return value from the handler is passed through.
 */
received_frame_status_t ZAF_CC_invoke_specific(CC_handler_map_latest_t const * const p_cc_entry,
                             RECEIVE_OPTIONS_TYPE_EX *rxOpt,
                             ZW_APPLICATION_TX_BUFFER *pFrameIn,
                             uint8_t cmdLength,
                             ZW_APPLICATION_TX_BUFFER *pFrameOut,
                             uint8_t *pLengthOut);

/**
 * Invokes a handler based on the command class in the given frame.
 *
 * This function is effectively a wrapper of ZAF_CC_invoke_specific() since it finds the
 * corresponding CC entry and invokes ZAF_CC_invoke_specific().
 *
 * @param rxOpt       Receive options.
 * @param pFrameIn    Received frame.
 * @param cmdLength   Length of received frame.
 * @param pFrameOut   Outgoing frame.
 * @param pLengthOut  Length of outgoing frame.
 * @return Returns RECEIVED_FRAME_STATUS_CC_NOT_FOUND if the CC entry is not found or of the
 *         handler version is not supported.
 *         Otherwise, the return value from the handler is passed through.
 */
received_frame_status_t invoke_cc_handler(RECEIVE_OPTIONS_TYPE_EX *rxOpt,
                                          ZW_APPLICATION_TX_BUFFER *pFrameIn,
                                          uint8_t cmdLength,
                                          ZW_APPLICATION_TX_BUFFER * pFrameOut,
                                          uint8_t * pLengthOut);

typedef void * zaf_cc_context_t;

/**
 * Callback type for ZAF_CC_foreach().
 *
 * The CC handler map must always be the latest available version.
 */
typedef bool (*zaf_cc_invoker_callback_t)(CC_handler_map_latest_t const * const p_cc_entry, zaf_cc_context_t context);

/**
 * Invokes callback for each registered CC.
 *
 * Will stop if the callback returns true.
 *
 * @param callback Callback function to invoke.
 * @param context Context to pass on to the callback function.
 */
void ZAF_CC_foreach(zaf_cc_invoker_callback_t callback, zaf_cc_context_t context);

#endif /* ZAF_CC_INVOKER_H_ */
