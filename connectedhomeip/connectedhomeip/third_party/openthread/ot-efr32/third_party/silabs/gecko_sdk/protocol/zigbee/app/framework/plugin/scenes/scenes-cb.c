/***************************************************************************//**
 * @file
 * @brief Weak callbacks for the Scenes plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "af.h"
#include "scenes.h"

/** @brief Scenes Server Custom Recall Scene
 *
 * Customize the implementation of the Scenes cluster RecallScene command
 * for desired behavior when gradually transitioning to the specified scene
 * over the specified transition time interval.
 *
 * Upon entry to this callback, the presented scene table entry has been verified
 * to be valid for the endpoint, group ID, and scene ID referenced in the received
 * ZCL Scenes cluster RecallScene command.
 *
 * NOTE: If this callback is implemented, the expectation is that it will INITIATE
 * the scene transition behavior and return promptly with prospective success or
 * failure status, rather than execute through the full duration of the transition
 * time before returning.
 *
 * NOTE: The scene table entry pointer, and the information in the scene
 * table entry, are available only during the execution of this callback
 * function. The scene table entry information (NOT the pointer) must be copied
 * and saved before returning from this callback function if it will need to be
 * referenced at other times to perform the desired transition behavior
 * (for example, by a timer-based periodic handler launched by this callback).
 *
 * If the value of transitionTimeDs is 0xFFFF, the transition time information
 * configured in the scene table entry should be used.
 *
 * Upon arriving at the target scene configuration, the custom implementation
 * should set the Scenes cluster attributes CurrentScene and CurrentGroup to
 * the scene ID and group ID values contained in the sceneEntry, and set the
 * SceneValid attribute to true.
 *
 * Returns true if a customization of this callback HAS been IMPLEMENTED.
 * This REQUIRES that a valid ::EmberAfStatus value has been written to the status
 * argument. Status will be ::EMBER_ZCL_STATUS_SUCCESS if the scene transition has
 * been initiated successfully, even though the transition time has not fully elapsed
 * and the full effect of the scene transition over the transition time has not
 * yet completed. Otherwise, a suitable error status value should be returned
 * in the status argument.
 *
 * Returns false if a customization of this callback HAS NOT been implemented.
 * In this case, the Scenes plugin will perform default handling (apply target
 * scene settings immediately without regard for the specified transition time).
 *
 * @param sceneEntry pointer to ::EmberAfSceneTableEntry for recalled scene Ver.: always
 * @param transitionTimeDs transition time in deci-seconds (1/10 sec) Ver.: always
 * @param status pointer to return ::EmberAfStatus Ver.: always
 *
 * @return true if custom callback is implemented
 *         false if custom callback is not implemented (apply default handling)
 */
WEAK(bool emberAfPluginScenesServerCustomRecallSceneCallback(
       const EmberAfSceneTableEntry * const sceneEntry,
       uint16_t transitionTimeDs,
       EmberAfStatus * const status))
{
  return false;
}
