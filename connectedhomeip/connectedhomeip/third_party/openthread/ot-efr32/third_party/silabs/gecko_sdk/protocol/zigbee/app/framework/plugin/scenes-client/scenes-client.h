/***************************************************************************//**
 * @file
 * @brief Definitions for the Scenes Client plugin.
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
 * @defgroup scenes-client Scenes Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Scenes Cluster Client Component
 *
 * Silicon Labs implementation of the Scenes client cluster.
 * The component is only intended to assist with debugging, as the implemented
 * callbacks simply parse responses from the Scenes server and print the results.
 * Before production, this component should be replaced with code that acts on the
 * responses in an appropriate way.
 *
 */

/**
 * @addtogroup scenes-client
 * @{
 */

/**
 * @name API
 * @{
 */
/** @brief Add scenes response
 *
 * @param cmd Cluster command Ver.: always
 * @param status Ver.: always
 * @param groupId Ver.: always
 * @param sceneId Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginScenesClientParseAddSceneResponse(const EmberAfClusterCommand *cmd,
                                                    uint8_t status,
                                                    uint16_t groupId,
                                                    uint8_t sceneId);

/** @brief View scenes response
 *
 * @param cmd cluster command Ver.: always
 * @param status Ver.: always
 * @param groupId Ver.: always
 * @param sceneId Ver.: always
 * @param transitionTime Ver.: always
 * @param sceneName Ver.: always
 * @param extentionFieldSets Ver.: always
 *
 * @param bool true is success
 *
 */
bool emberAfPluginScenesClientParseViewSceneResponse(const EmberAfClusterCommand *cmd,
                                                     uint8_t status,
                                                     uint16_t groupId,
                                                     uint8_t sceneId,
                                                     uint16_t transitionTime,
                                                     const uint8_t *sceneName,
                                                     const uint8_t *extensionFieldSets);

/** @} */ // end of name API
/** @} */ // end of scenes-client
