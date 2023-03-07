/**
 * @file
 * Handler for Command Class Color Switch.
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef CC_COLOR_SWITCH_H
#define CC_COLOR_SWITCH_H

#include <ZAF_types.h>
#include <Assert.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_Actuator.h>
/**
 * Defined Color Components ID
 */
typedef enum EColorComponents_ {
  ECOLORCOMPONENT_WARM_WHITE,   ///< ECOLORCOMPONENT_WARM_WHITE
  ECOLORCOMPONENT_COLD_WHITE,   ///< ECOLORCOMPONENT_COLD_WHITE
  ECOLORCOMPONENT_RED,          ///< ECOLORCOMPONENT_RED
  ECOLORCOMPONENT_GREEN,        ///< ECOLORCOMPONENT_GREEN
  ECOLORCOMPONENT_BLUE,         ///< ECOLORCOMPONENT_BLUE
  ECOLORCOMPONENT_AMBER,        ///< ECOLORCOMPONENT_AMBER
  ECOLORCOMPONENT_CYAN,         ///< ECOLORCOMPONENT_CYAN (For 6ch color mixing)
  ECOLORCOMPONENT_PURPLE,       ///< ECOLORCOMPONENT_PURPLE (For 6ch color mixing)
  //ECOLORCOMPONENT_INDEXED_COLOR, //(OBSOLETED)
  ECOLORCOMPONENT_COLOR_UNDEFINED
} EColorComponents;


/**
 * Structure that holds Color Switch CC related data.
 */
typedef struct colorComponent_
{
  RECEIVE_OPTIONS_TYPE_EX rxOpt;             ///< saves Rx values, including data about supervision.
                                             ///< (Must be first member - required by TSE)
  EColorComponents colorId;                  ///< Color Component ID
  uint8_t ep;                                ///< Endpoint that supports colorId
  s_Actuator obj;                            ///< Actuator data
  void (*cb)(struct colorComponent_ *color); ///< pointer to app callback function that triggers driver changes
} s_colorComponent;


/**
 * Initializes data needed for Color Switch CC, including setting supported colors in colorSwitch_colorsSupported
 * @param colors Pointer to array of supported color components
 * @param length Number of elements in colors array
 * @param durationDefault Factory default duration. The value must be encoded like described in
 *                        requirement CC:0000.00.00.11.015 in SDS13781.
 * @param CC_ColorSwitch_cb callback function
 */
void CC_ColorSwitch_Init(s_colorComponent *colors,
                         uint8_t length,
                         uint8_t durationDefault,
                         void (*CC_ColorSwitch_cb)(void));

/**
 * Sends Color Switch report when TSE was triggered
 * @param txOptions TxOptions, filled in by TSE
 * @param pData Color Switch related data
 */
void CC_ColorSwitch_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void *pData);

/**
 * Function triggered after some change happened in Actuator module
 * @details
 * Responsible for informing the app about the change and triggering TSE if one of the colors reached its final state.
 * In case of supervision timed change,it also sends Supervision Report with final state
 *
 * @param pObj pointer to object that was processed by Actuator
 */
void CC_ColorSwitch_ColorChanged_cb(s_Actuator *pObj);

#endif // CC_COLOR_SWITCH_H
