/**
 * Indicator command class version 3
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef CC_INDICATOR_H_
#define CC_INDICATOR_H_

#include <ZAF_types.h>
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <ZW_TransportEndpoint.h>

/**
 * Struct used to pass operational data to TSE module
 */
typedef struct s_CC_indicator_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
  uint8_t indicatorId; /**< Indicator Id */
} s_CC_indicator_data_t;


/**
 * Indicator set callback.
 *
 * Even though on/off time parameters are given in milliseconds, the
 * resolution is 1/10'th of a second.
 *
 * @param on_time_ms  ON duration (in milliseconds) for a single blink cycle.
 *                    If on_time_ms is zero the indicator should be turned off.
 *
 * @param off_time_ms OFF duration (in milliseconds) for a single blink cycle.
 *
 * @param num_cycles  Number of blink cycles. If num_cycles is zero the indicator
 *                    LED should blink "forever" or until the next time this
 *                    function is called.
 */
typedef void (*cc_indicator_callback_t)(uint32_t on_time_ms,
                                        uint32_t off_time_ms,
                                        uint32_t num_cycles);

/**
 * Initializes the Indicator Command Class.
 * @param callback Function that is invoked everytime indicator is set.
 */
void CC_Indicator_Init(cc_indicator_callback_t callback);

/**
 * Send report when change happen via lifeLine by INDICATOR_SET
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
void CC_Indicator_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData);

/**
 * Resets the indicator CC properties to their "off" values if the
 * indicator is no longer active.
 *
 * If the application manually de-activates the Indicator (e.g. learn mode), it should call
 * this function to have the CC Indicator values refreshed before it reports
 * its state to the Lifeline destinations.
*/
void CC_Indicator_RefreshIndicatorProperties(void);


#endif /* CC_INDICATOR_H_ */
