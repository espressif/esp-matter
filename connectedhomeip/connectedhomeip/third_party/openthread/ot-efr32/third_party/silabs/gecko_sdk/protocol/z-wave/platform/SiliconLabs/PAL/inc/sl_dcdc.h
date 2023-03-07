/**
 * @file
 * Functionality for setting up the DC-DC controller for Z-Wave
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef __SL_DCDC_H__
#define __SL_DCDC_H__

#include <stdbool.h>


typedef enum sl_dcdc_config_t
{
  EDCDCMODE_AUTO,
  EDCDCMODE_BYPASS,
  EDCDCMODE_DCDC_LOW_NOISE
} sl_dcdc_config_t;

/**
* Set Zwave DCDC configuration to AUTO, BYPASS or DCDC_LOW_NOISE.
* @param[in]    mode EDCDCMODE_AUTO, EDCDCMODE_BYPASS, EDCDCMODE_DCDC_LOW_NOISE
*
*/
bool sl_dcdc_config_set(sl_dcdc_config_t mode);

/**
* Get current Zwave DCDC configuration
*
* @return       EDCDCMODE_AUTO, EDCDCMODE_BYPASS, EDCDCMODE_DCDC_LOW_NOISE
*/
sl_dcdc_config_t sl_dcdc_config_get(void);

/******************************************************************************
 * @brief Setup VMON to monitor AVDD, and IOVDD, set the dc2dc bypass mode
 *        if the AVDD is less than VMON_AVDD_FALL_THRESHOLD_mV during booting
 *****************************************************************************/
void sl_dcdc_vmon_init(sl_dcdc_config_t mode);

/******************************************************************************
 * @brief Setup DC-DC converter for optimal power consumption in EM2
 *****************************************************************************/
void sl_dcdc_setup_em2(void);
void sl_dcdc_exit_em2(void);

/******************************************************************************
 * @brief Setup DC-DC converter for optimal power consumption in EM2
 *****************************************************************************/
void sl_dcdc_setup_em4h(void);


#endif /* __SL_DCDC_H__ */
