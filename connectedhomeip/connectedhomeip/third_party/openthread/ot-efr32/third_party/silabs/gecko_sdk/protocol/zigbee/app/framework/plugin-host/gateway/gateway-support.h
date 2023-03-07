/***************************************************************************//**
 * @file
 * @brief Definitions for the Gateway plugin.
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
 * @defgroup gateway Gateway
 * @ingroup component host
 * @brief API and Callbacks for the Gateway Component
 *
 * Software that supports a CLI based application running on a gateway-style
 * device that has a POSIX compatible operating system.   This plugin is
 * NOT compatible with an system-on-a-chip (SOC) platform.
 *
 */

/**
 * @addtogroup gateway
 * @{
 */

typedef uint8_t BackchannelState;
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum BackchannelStateEnum
#else
enum
#endif
{
  NO_CONNECTION = 0,
  CONNECTION_EXISTS = 1,
  NEW_CONNECTION = 2,
  CONNECTION_ERROR = 3,
};

/**
 * @name API
 * @{
 */

extern const bool backchannelSupported;
extern bool backchannelEnable;
extern int backchannelSerialPortOffset;

void gatewayBackchannelStop(void);

EmberStatus backchannelStartServer(uint8_t port);
EmberStatus backchannelStopServer(uint8_t port);
EmberStatus backchannelReceive(uint8_t port, char* data);
EmberStatus backchannelSend(uint8_t port, uint8_t * data, uint8_t length);

EmberStatus backchannelClientConnectionCleanup(uint8_t port);

BackchannelState backchannelCheckConnection(uint8_t port,
                                            bool waitForConnection);

EmberStatus backchannelMapStandardInputOutputToRemoteConnection(int port);
EmberStatus backchannelCloseConnection(uint8_t port);
EmberStatus backchannelServerPrintf(const char* formatString, ...);
EmberStatus backchannelClientPrintf(uint8_t port, const char* formatString, ...);
EmberStatus backchannelClientVprintf(uint8_t port,
                                     const char* formatString,
                                     va_list ap);

/** @} */ // end of name API
/** @} */ // end of gateway
