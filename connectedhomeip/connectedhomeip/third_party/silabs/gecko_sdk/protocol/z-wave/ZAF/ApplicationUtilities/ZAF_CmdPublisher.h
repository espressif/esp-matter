/**
 * @file
 *
 * @brief Command Publisher module takes care of publishing incoming packages
 * @details This module is part of Security Discovery feature
 * @copyright 2018 Silicon Laboratories Inc.
 */
#ifndef _CMD_PUBLISHER_H_
#define _CMD_PUBLISHER_H_

#include <stdint.h>
#include <stdbool.h>

// These following defines must be aligned with the structs in the private implementation
#define ZAF_CP_HEADER_SIZE 4
#define ZAF_CP_ELEMENT_SIZE 16

// Calculate size of storage
#define ZAF_CP_STORAGE_SIZE(num_elements) (ZAF_CP_HEADER_SIZE + (num_elements) * ZAF_CP_ELEMENT_SIZE)

// Statically allocate storage at a word boundary (or any other boundary if needed)
#define ZAF_CP_STORAGE(name, num_elements) union { void *align; uint8_t storage[ZAF_CP_STORAGE_SIZE(num_elements)]; } name

/** Handle to the Cmd Publisher "object" */
typedef void * CP_Handle_t;

/**
 * Callback function triggered by ZAF_CP_CommandPublish, when corresponding subscriber gets notified.
 *
 * @param pSubscriberContext Subscribers context to be passed to subscriber
 * @param pRxPackage Received Frame
 */
typedef void (*zaf_cp_subscriberFunction_t)(void *pSubscriberContext, void* pRxPackage);

/**
 * Initializes Command publisher module
 * @note ZAF_CP_STORAGE must be called before this
 *
 * @param pStorage Stores the subscribers list. Should be allocated with ZAF_CP_STORAGE
 * @param numSubscribers Number of subscribers
 * @return handle to the initialized content
 */
CP_Handle_t ZAF_CP_Init(void *pStorage, uint8_t numSubscribers);

/**
 * Subscribe a function with pHandle to all command classes.
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @return true if new subscription went OK, false if not
 */
bool ZAF_CP_SubscribeToAll(CP_Handle_t handle,
                           void* pSubscriberContext,
                           zaf_cp_subscriberFunction_t pFunction);

/**
 * Subscribe a function with pHandle to an entire command class.
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @param CmdClass Command Class to subscribe to
 * @return true if new subscription went OK, false if not
 */
bool ZAF_CP_SubscribeToCC(CP_Handle_t handle,
                          void* pSubscriberContext,
                          zaf_cp_subscriberFunction_t pFunction,
                          uint16_t CmdClass);

/**
 * Subscribe a function with pHandle to a specific command of the given command class.
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @param CmdClass Command Class to subscribe to
 * @param Cmd Specific command to subscribe to
 * @return true if new subscription went OK, false if not
 */
bool ZAF_CP_SubscribeToCmd(CP_Handle_t handle,
                           void* pSubscriberContext,
                           zaf_cp_subscriberFunction_t pFunction,
                           uint16_t CmdClass,
                           uint8_t Cmd);

/**
 * Unsubscribe from previously subscribed handle
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @return true if unsubscribing went OK, false if not
 */
bool ZAF_CP_UnsubscribeToAll(CP_Handle_t handle,
                             void* pSubscriberContext,
                             zaf_cp_subscriberFunction_t pFunction);

/**
 * Unsubscribe from previously subscribed command class
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @param CmdClass Command Class to unsubscribe from
 * @return true if unsubscribing went OK, false if not
 */
bool ZAF_CP_UnsubscribeToCC(CP_Handle_t handle,
                            void* pSubscriberContext,
                            zaf_cp_subscriberFunction_t pFunction,
                            uint16_t CmdClass);
/**
 * Unsubscribe from previously subscribed command
 *
 * @param handle Handle to Cmd Publisher module
 * @param pSubscriberContext Context of the subscriber
 * @param pFunction Subscribers callback function
 * @param CmdClass Command Class to unsubscribe from
 * @param Cmd Specific command to unsubscribe from
 * @return true if unsubscribing went OK, false if not
 */
bool ZAF_CP_UnsubscribeToCmd(CP_Handle_t handle,
                             void* pSubscriberContext,
                             zaf_cp_subscriberFunction_t pFunction,
                             uint16_t CmdClass,
                             uint8_t Cmd);

/**
 * Informs all relevant subscribers about incoming frame.
 * Based on incoming frame, it will decide which type of subscribers should be notified
 * It should be called when the receive queue is emptied
 *
 * @param handle Handle to Cmd Publisher module
 * @param pRxPackage Received frame. It will be passed to subscribers
 */
void ZAF_CP_CommandPublish(CP_Handle_t handle, void* pRxPackage);

#endif /* _CMD_PUBLISHER_H_ */
