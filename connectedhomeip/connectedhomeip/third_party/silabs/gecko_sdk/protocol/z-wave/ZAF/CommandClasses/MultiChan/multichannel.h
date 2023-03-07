/**
 * @file
 * Helper module for CC Multi Channel.
 *
 * @details Contains supporting functions required by transport module and supervision.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef ZAF_CC_MULTICHAN_HELPER_H_
#define ZAF_CC_MULTICHAN_HELPER_H_

#include <ZW_TransportEndpoint.h>

/**
 * Given a frame and the associated receive options this function returns whether the command is
 * supported or not.
 * @param[in] pRxOpt Receive options.
 * @param[in] pCmd Frame including command.
 * @return true if the command is supported, false otherwise.
 */
bool ZAF_CC_MultiChannel_IsCCSupported(
  RECEIVE_OPTIONS_TYPE_EX * pRxOpt,
  ZW_APPLICATION_TX_BUFFER * pCmd);

/**
 * Encapsulates a frame with Multi Channel.
 * @param[in,out] ppData Pointer to data.
 * @param[in,out] dataLength Pointer to data length.
 * @param[in] pTxOptionsEx Pointer to transmit options.
 */
void CmdClassMultiChannelEncapsulate(
  uint8_t **ppData,
  size_t  *dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx);


#endif /* ZAF_CC_MULTICHAN_HELPER_H_ */
