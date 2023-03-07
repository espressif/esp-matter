/**
 * @file
 * Defines an API for the Z-Wave platform independent radio utils.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_RADIO_UITLS_H_
#define ZPAL_RADIO_UITLS_H_

#include <stdbool.h>
#include <stdint.h>
#include <zpal_radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-radio-utils
 * @brief
 * Defines a platform abstraction layer for the Z-Wave platform independent radio utils.
 *
 * @{
 */

/**
 * @brief Function to get region Protocol mode if supported by Radio.
 *
 * @param[in] region Describe where radio is located. @ref zpal_radio_region_t or @ ref zpal_radio_region_tf_t.
 * @return @ref ZPAL_RADIO_PROTOCOL_MODE_1, @ref ZPAL_RADIO_PROTOCOL_MODE_2, @ref ZPAL_RADIO_PROTOCOL_MODE_3
 *         or @ref ZPAL_RADIO_PROTOCOL_MODE_4 if supported by Radio. @ref ZPAL_RADIO_PROTOCOL_MODE_UNDEFINED if not supported by Radio.
 */
zpal_radio_protocol_mode_t zpal_radio_region_get_protocol_mode(uint32_t region);

/**
 * @brief Function to get if giver Protocol mode supports Z-Wave Long Range.
 *
 * @param[in] mode Protocol mode.
 * @return True if given Protocol mode supports Z-Wave Long Range, false if not.
 */
bool zpal_radio_protocol_mode_supports_long_range(zpal_radio_protocol_mode_t mode);

/**
 * @brief Function to check if specified region has a long range channel.
 *
 * @param[in] region Describe where radio is located.
 * @return True if specified region has long range channel.
 */
bool zpal_radio_region_is_long_range(zpal_radio_region_t region);

/**
 * @brief Function to check if specified region has a backup long range channel.
 *
 * @param[in] region Describe where radio is located.
 * @return True if specified region has backup long range channel.
 */
bool zpal_radio_region_is_long_range_backup(zpal_radio_region_t region);


/**
 * @brief Function to check if specified region has End Device long range channels.
 *
 * @param[in] region Describe where radio is located.
 * @return True if specified region has End Device long range channels.
 */
bool zpal_radio_region_is_long_range_end_device(zpal_radio_region_t region);


/**
 * @brief Function to get if specified region has either long range or backup long range channels or both.
 *
 * @param[in] region Describe where radio is located.
 * @return True if specified region has either long range or backup long range channels or both.
 */
bool zpal_radio_region_has_long_range_support(zpal_radio_region_t region);

/**
 * @} //zpal-radio-utils
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_RADIO_UITLS_H_ */
