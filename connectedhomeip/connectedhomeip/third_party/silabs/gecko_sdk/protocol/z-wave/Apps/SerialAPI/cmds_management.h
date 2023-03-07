/**
 * @file cmds_management.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef APPS_SERIALAPI_CMD_MANAGEMENT_H_
#define APPS_SERIALAPI_CMD_MANAGEMENT_H_

#include <stdint.h>
#include <ZAF_types.h>
/* FUNC_ID_SERIAL_API_SETUP command definitions */
typedef enum
{
  /**
   * The first 8 commands are given as bit-flags, and when all bits were consumed, a byte-array was created to give
   * more room.
   * The first 8 flags are the only ones that shall be used to fill the first byte when generating the response in
   * pOutputBuffer for the command, SERIAL_API_SETUP_CMD_SUPPORTED.
   * This is kept for backwards compatibility.
   */
  SERIAL_API_SETUP_CMD_UNSUPPORTED,
  SERIAL_API_SETUP_CMD_SUPPORTED                  = 1,   //1<<0
  SERIAL_API_SETUP_CMD_TX_STATUS_REPORT           = 2,   //1<<1
  SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET          = 4,   //1<<2 @Deprecated
  SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET          = 8,   //1<<3 @Deprecated
  SERIAL_API_SETUP_CMD_TX_GET_MAX_PAYLOAD_SIZE    = 16,  //1<<4
  SERIAL_API_SETUP_CMD_RF_REGION_GET              = 32,  //1<<5
  SERIAL_API_SETUP_CMD_RF_REGION_SET              = 64,  //1<<6
  SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET        = 128, //1<<7
  /**
   * The below values are not flags and shall only be used with BITMASK_ADD_CMD() when generating
   * the response for the command, SERIAL_API_SETUP_CMD_SUPPORTED.
   */
  SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET          = 3,
  SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_GET          = 5,
                        // The values 6 and 7 are unused, but not reserved.
  SERIAL_API_SETUP_CMD_TX_GET_MAX_LR_PAYLOAD_SIZE = 17,
  SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_16_BIT   = 18,
  SERIAL_API_SETUP_CMD_TX_POWERLEVEL_GET_16_BIT   = 19,
} eSerialAPISetupCmd;

/* SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET definitions */
typedef enum
{
  SERIAL_API_SETUP_NODEID_BASE_TYPE_8_BIT         = 1,
  SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT,
  SERIAL_API_SETUP_NODEID_BASE_TYPE_LAST,
  SERIAL_API_SETUP_NODEID_BASE_TYPE_DEFAULT = SERIAL_API_SETUP_NODEID_BASE_TYPE_8_BIT
} eSerialAPISetupNodeIdBaseType;

/* Global variable for specifying if the SerialAPI command Node ID fields are 8 or 16 bits */
extern eSerialAPISetupNodeIdBaseType nodeIdBaseType;

/* Macro for retrieving a SerialAPI command nodeID value (8 or 16 bit) at address "pData" */
/* Increments the input argument "idx" if nodeID is two bytes wide (i.e. 16 bit) */
#define GET_NODEID(pData, idx) \
    ( (nodeIdBaseType == SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT) ? \
      ((((uint8_t*)pData)[0] << 8) | ((uint8_t*)pData)[1]) : /* 16 bit, MSB | LSB */ \
      ((uint8_t*)pData)[0] );                                /* 8 bit */ \
    do { \
      if (nodeIdBaseType == SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT) \
      { \
        idx++; \
      } \
    } while (0)

#define GET_16BIT_VALUE(pData) \
    ( ( (uint16_t)((uint8_t*)pData)[0] << 8) | (uint16_t)((uint8_t*)pData)[1] ) /* 16 bit, MSB | LSB */

/* Commands minimum length (bytes) */
#define SERIAL_API_SETUP_CMD_TX_STATUS_REPORT_CMD_LENGTH_MIN    2
#define SERIAL_API_SETUP_CMD_RF_REGION_SET_CMD_LENGTH_MIN       2
#define SERIAL_API_SETUP_CMD_TX_POWERLEVEL_SET_CMD_LENGTH_MIN   3
#define SERIAL_API_SETUP_CMD_NODEID_BASETYPE_SET_CMD_LENGTH_MIN 2
#define SERIAL_API_SETUP_CMD_MAX_LR_TX_PWR_SET_CMD_LENGTH_MIN   3

/**
 * Must be called upon receiving a "Node List Command".
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */
void func_id_serial_api_get_init_data(uint8_t inputLength,
                                      const uint8_t *pInputBuffer,
                                      uint8_t *pOutputBuffer,
                                      uint8_t *pOutputLength);

/**
 * Returns bitmask of LR node IDs
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer. First byte should be requested range of nodes.
 *                     Supported values: 0, 1, 2, 3 that corresponds to  offset 0, 128, 256, 384.
 * @param pOutputBuffer Output buffer, contained of: MORE_NODES | BITMASK_OFFSET | BITMASK_LEN | BITMASK_ARRAY
 * @param pOutputLength Length of data in output buffer.
 */
void func_id_serial_api_get_LR_nodes(uint8_t inputLength,
                                     const uint8_t *pInputBuffer,
                                     uint8_t *pOutputBuffer,
                                     uint8_t *pOutputLength);

/**
 * Must be called upon receiving a "Serial API Setup Command".
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */
void func_id_serial_api_setup(uint8_t inputLength,
                              const uint8_t *pInputBuffer,
                              uint8_t *pOutputBuffer,
                              uint8_t *pOutputLength);

/**
 * Must be called upon receiving a "Serial API NVR get".
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */

void
func_id_serial_api_get_nvr(uint8_t inputLength,
                           const uint8_t *pInputBuffer,
                           uint8_t *pOutputBuffer,
                           uint8_t *pOutputLength);

/**
 * Must be called upon receiving a "Serial API ZW Get protocol version".
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */

void
func_id_zw_get_protocol_version(uint8_t inputLength,
                                const uint8_t *pInputBuffer,
                                uint8_t *pOutputBuffer,
                                uint8_t *pOutputLength);

/**
 * Remove a specific node from the network
 * The node is identified by node ID
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */
void func_id_ZW_RemoveNodeIDFromNetwork (uint8_t inputLength,
                                         uint8_t *pInputBuffer,
                                         const uint8_t *pOutputBuffer,
                                         const uint8_t *pOutputLength);

/**
 * Initiate device graceful shutdown
 *
 * @param pCallback   pointer to notifcation callback function. Function is called just before deice go into deepsleep
 * @return true if Shutdown process start, else false
 */
bool InitiateShutdown( ZW_Void_Callback_t pCallback);
#endif /* APPS_SERIALAPI_CMD_MANAGEMENT_H_ */
