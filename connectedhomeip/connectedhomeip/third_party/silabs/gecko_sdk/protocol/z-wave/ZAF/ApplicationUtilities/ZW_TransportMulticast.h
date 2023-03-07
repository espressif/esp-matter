/**
 * @file
 * @brief Handles multicast frames in the Z-Wave Framework.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_ZW_TRANSPORTMULTICAST_H_
#define ZAF_APPLICATIONUTILITIES_ZW_TRANSPORTMULTICAST_H_

#include <stdint.h>
#include <ZW_TransportEndpoint.h>

enum ETRANSPORT_MULTICAST_STATUS
{
  ETRANSPORTMULTICAST_FAILED = 0,
  ETRANSPORTMULTICAST_ADDED_TO_QUEUE = 1
};

void ZW_TransportMulticast_Init(void * pAppHandle);

/**
 * Initiates transmission of a frame and will handle multi channel and multicast transmissions.
 *
 * If the node has associations to multiple endpoints of the same node, this function will transmit
 * one multi channel encapsulated frame that addresses the associated endpoints.
 *
 * If the node is included using S2 and has more than one non-endpoint association, this function
 * will transmit an S2 multicast (broadcast) frame addressing those associations followed by
 * singlecast follow-up frames.
 *
 * If the node is included using S0 there will be no multicast since S0 does not support multicast.
 *
 * If the node is non-securely included and has more than one non-endpoint association, this
 * function will transmit a non-secure multicast followed by singlecast follow-up frames.
 *
 * @param[in] p_data Pointer to data.
 * @param[in] data_length Length of data in bytes.
 * @param[in] fSupervisionEnable Enable Supervision encapsulation if set to true.
 * @param[in] p_nodelist Pointer to a list of nodes.
 * @param[in] p_callback Pointer to a callback function which is called for each transmission to a node.
 * @return Returns status of transmission.
 */
enum ETRANSPORT_MULTICAST_STATUS
ZW_TransportMulticast_SendRequest(const uint8_t * const p_data,
                                  uint8_t data_length,
                                  uint8_t fSupervisionEnable,
                                  TRANSMIT_OPTIONS_TYPE_EX * p_nodelist,
                                  ZAF_TX_Callback_t p_callback);

/**
 * @brief
 */
void ZW_TransportMulticast_clearTimeout(void);

#endif /* ZAF_APPLICATIONUTILITIES_ZW_TRANSPORTMULTICAST_H_ */
