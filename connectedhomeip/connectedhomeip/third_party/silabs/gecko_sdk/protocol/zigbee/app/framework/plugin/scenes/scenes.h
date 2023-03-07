/***************************************************************************//**
 * @file
 * @brief Definitions for the Scenes plugin.
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

/**
 * @defgroup scenes-server  Scenes Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Scenes Cluster Server Component
 *
 * Silicon Labs implementation of the Scenes server cluster.
 * This component supports commands for setting up and recalling scenes.
 * Scenes are stored in a table and each scene consists of a set of
 * values for attributes in other clusters. Clusters that extend the
 * scene table do so through extension field sets. This component supports
 * extensions for the On/Off, Level Control, Thermostat, Color Control,
 * Door Lock, and Window Covering clusters. If the application includes
 * any of these clusters, the component automatically includes and manages
 * the attributes in those clusters. For example, if the application
 * includes the On/Off server cluster, the component saves and recalls
 * the On/Off attribute as part of saving or recalling scenes.
 * Some ZLL extensions are implemented in this component and will be
 * included automatically for ZLL applications. If the ZLL Scenes
 * server cluster component is also enabled, this component uses it for
 * handling some additional ZLL enhancements. Otherwise, these ZLL
 * extensions are disabled. This component requires extending in order
 * to interact with the actual hardware.
 *
 */

/**
 * @addtogroup scenes-server
 * @{
 */

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "scenes-config.h"
#if (EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT == 1)
#define NAME_SUPPORT
#endif
#else // !UC_BUILD
// In Appbuilder, the EMBER_AF_PLUGIN_SCENES_USE_TOKENS is defined
// (in auto generated AF header) if-and-only-if the token is enabled by user,
// so just redefine it to 1 else to 0.
#ifdef EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#undef EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS 1
#else
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS 0
#endif // EMBER_AF_PLUGIN_SCENES_USE_TOKENS
#ifdef EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT
#define NAME_SUPPORT
#endif
#endif // UC_BUILD

EmberAfStatus emberAfScenesSetSceneCountAttribute(uint8_t endpoint,
                                                  uint8_t newCount);
EmberAfStatus emberAfScenesMakeValid(uint8_t endpoint,
                                     uint8_t sceneId,
                                     uint16_t groupId);

// DEPRECATED.
#define emberAfScenesMakeInvalid emberAfScenesClusterMakeInvalidCallback

extern uint8_t emberAfPluginScenesServerEntriesInUse;
#if (EMBER_AF_PLUGIN_SCENES_USE_TOKENS == 1) && !defined(EZSP_HOST)
// In this case, we use token storage
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  halCommonGetIndexedToken(&(entry), TOKEN_SCENES_TABLE, (i))
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  halCommonSetIndexedToken(TOKEN_SCENES_TABLE, (i), &(entry))
  #define emberAfPluginScenesServerNumSceneEntriesInUse()                               \
  (halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
   emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x),               \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    ++emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    --emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
#else
// Use normal RAM storage
extern EmberAfSceneTableEntry emberAfPluginScenesServerSceneTable[];
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  ((entry) = emberAfPluginScenesServerSceneTable[(i)])
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  (emberAfPluginScenesServerSceneTable[(i)] = (entry))
  #define emberAfPluginScenesServerNumSceneEntriesInUse() \
  (emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse() \
  (++emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse() \
  (--emberAfPluginScenesServerEntriesInUse)
#endif // (EMBER_AF_PLUGIN_SCENES_USE_TOKENS == 1) && !defined(EZSP_HOST)

bool emberAfPluginScenesServerParseAddScene(const EmberAfClusterCommand *cmd,
                                            uint16_t groupId,
                                            uint8_t sceneId,
                                            uint16_t transitionTime,
                                            uint8_t *sceneName,
                                            uint8_t *extensionFieldSets);
bool emberAfPluginScenesServerParseViewScene(const EmberAfClusterCommand *cmd,
                                             uint16_t groupId,
                                             uint8_t sceneId);

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
bool emberAfPluginScenesServerCustomRecallSceneCallback(
  const EmberAfSceneTableEntry * const sceneEntry,
  uint16_t transitionTimeDs,
  EmberAfStatus * const status);

/** @} */ // end of name API
/** @} */ // end of scenes-server

void emAfPluginScenesServerPrintInfo(void);
