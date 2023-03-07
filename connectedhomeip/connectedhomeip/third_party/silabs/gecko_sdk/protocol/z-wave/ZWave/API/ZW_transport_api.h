/**
 * @file ZW_transport_api.h
 * Z-Wave Transport Application layer interface.
 *
 * The Z Wave transport layer controls transfer of data between Z Wave nodes
 * including retransmission, frame check and acknowledgement. The Z Wave
 * transport interface includes functions for transfer of data to other Z Wave
 * nodes. Application data received from other nodes is handed over to the
 * application via the \ref ApplicationCommandHandler function. The ZW_MAX_NODES
 * define defines the maximum of nodes possible in a Z Wave network.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_TRANSPORT_API_H_
#define _ZW_TRANSPORT_API_H_
#include <ZW_security_api.h>
#include <zpal_radio.h>

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/


#define ZW_MAX_NODES        232

/* Max number of Long Range nodes in a Z-wave system */
/* Must not be higher than 4000 according to specs */
#define ZW_MAX_NODES_LR    1024

#define LOWEST_LONG_RANGE_NODE_ID   (0x0100)

#define HIGHEST_LONG_RANGE_NODE_ID  (LOWEST_LONG_RANGE_NODE_ID + ZW_MAX_NODES_LR - 1)
/************************************************************/
/* Node Information frame*/
/************************************************************/
#define NODEPARM_MAX       35   /* max. number of parameters */
#define HOMEID_LENGTH      4    /* Number of bytes in a homeID */

typedef uint32_t TxOptions_t;  //< The universal TX-Option flag count.

/* Transmit frame option flags */
#define TRANSMIT_OPTION_ACK                     0x01    /* request acknowledge from destination node */
#define TRANSMIT_OPTION_LOW_POWER               0x02    /* transmit at low output power level (1/3 of normal RF range)*/
#define TRANSMIT_OPTION_MULTICAST_AS_BROADCAST  0x02    /* The multicast frame should be send as a broadcast */
#define TRANSMIT_OPTION_AUTO_ROUTE              0x04    /* request retransmission via repeater nodes */
/* do not use response route - Even if available */
#define TRANSMIT_OPTION_NO_ROUTE                0x10
/* Use explore frame if needed */
#define TRANSMIT_OPTION_EXPLORE                 0x20



/* Received frame status flags */
/**
 *  \defgroup RECEIVE_STATUS Status codes for receiving frames.
 * \addtogroup RECEIVE_STATUS
 * @{
 */

/**
 * A response route is locked by the application
 */
#define RECEIVE_STATUS_ROUTED_BUSY    0x01
/**
 * Received at low output power level, this must
 * have the same value as TRANSMIT_OPTION_LOW_POWER
 */
#define RECEIVE_STATUS_LOW_POWER      0x02
/**
 * Mask for masking out the received frametype bits
 */
#define RECEIVE_STATUS_TYPE_MASK      0x0C
/**
 * Received frame is singlecast frame (rxOptions == xxxx00xx)
 */
#define RECEIVE_STATUS_TYPE_SINGLE    0x00
/**
 * Received frame is broadcast frame  (rxOptions == xxxx01xx)
 */
#define RECEIVE_STATUS_TYPE_BROAD     0x04
/**
 * Received frame is multicast frame (rxOptions == xxxx10xx)
 */
#define RECEIVE_STATUS_TYPE_MULTI     0x08
/**
 * Received frame is an explore frame (rxOptions == xxx1xxxx)
 * Only TYPE_BROAD can be active at the same time as TYPE_EXPLORE
 */
#define RECEIVE_STATUS_TYPE_EXPLORE   0x10
/**
 * Received frame is not send to me (rxOptions == x1xxxxxx)
 * - useful only in promiscuous mode
 */
#define RECEIVE_STATUS_FOREIGN_FRAME  0x40

/**
 * @}
 */

/* Predefined Node ID's */
#define NODE_BROADCAST              0xFF    /* broadcast */
#define ZW_TEST_NOT_A_NODEID        0x00    /* */

/* Transmit complete codes */
#define TRANSMIT_COMPLETE_OK      0x00
#define TRANSMIT_COMPLETE_NO_ACK  0x01  /* retransmission error */
#define TRANSMIT_COMPLETE_FAIL    0x02  /* transmit error */
#define TRANSMIT_ROUTING_NOT_IDLE 0x03  /* transmit error */
#ifdef ZW_CONTROLLER
/* Assign route transmit complete but no routes was found */
#define TRANSMIT_COMPLETE_NOROUTE 0x04  /* no route found in assignroute */
                                        /* therefore nothing was transmitted */
#endif
#define TRANSMIT_COMPLETE_VERIFIED 0x05 /*Security S2. Receiving node has decrypted the frame.*/

#define ZW_MAX_CACHED_RETURN_ROUTE_DESTINATIONS  5

/* Max hops in route */
#define MAX_REPEATERS                            4

/* TX_STATUS_TYPE Last Used Route array size definitions */
#define LAST_USED_ROUTE_CONF_SIZE                1
#define LAST_USED_ROUTE_SIZE                     (MAX_REPEATERS + LAST_USED_ROUTE_CONF_SIZE)

/* RSSI value array used in TX_STATUS_TYPE.
 * Each value is an RSSI feedback constant defined above. */
struct s_rssi_val {
  signed char incoming[MAX_REPEATERS + 1];
};


typedef struct _S_ROUTE_LINK_T_
{
  uint8_t from;
  uint8_t to;
} S_ROUTE_LINK_T;


typedef struct _TX_STATUS_TYPE_
{
  uint32_t TransmitTicks;  /* Passed 1ms ticks */
  uint8_t bRepeaters;         /* Repeaters in route, zero for direct range */
  /* rssi_values per hop for direct and routed frames.
   * Contains repeaters + 1 values. */
  struct s_rssi_val rssi_values;
  uint8_t bACKChannelNo;
  uint8_t bLastTxChannelNo;
  uint8_t bRouteSchemeState;
  uint8_t pLastUsedRoute[LAST_USED_ROUTE_SIZE];
  uint8_t bRouteTries;
  S_ROUTE_LINK_T bLastFailedLink;
  int8_t bUsedTxpower;
  int8_t bMeasuredNoiseFloor;
  int8_t bDestinationAckUsedTxPower;
  int8_t bDestinationAckMeasuredRSSI;
  int8_t bDestinationAckMeasuredNoiseFloor;
} TX_STATUS_TYPE;


typedef struct _RECEIVE_OPTIONS_TYPE
{
  /* Frame header info */
  uint8_t  rxStatus;
  /* Command sender Home ID */
  union{
      uint8_t  array[HOMEID_LENGTH];
      uint32_t word;
  }homeId;
  /* Command sender Node ID */
  node_id_t  sourceNode;
  /* Frame destination ID, only valid when frame is not Multicast*/
  node_id_t  destNode;
  /* Average RSSI val in dBm as defined in RSSI feedback constants above */
  signed char rxRSSIVal;
  /* Security key frame was received with. */
  enum SECURITY_KEY securityKey;
  /* Channel on which frame was received */
  uint8_t rxChannelNo;
  /* Channel on which frame was received is a Z-Wave Long Range channel */
  bool isLongRangeChannel;
  /* Transmit power used by the source to transmit the frame */
  int8_t bSourceTxPower;
  /* Noise floor measured by the source node before transmitting */
  int8_t bSourceNoiseFloor;
} RECEIVE_OPTIONS_TYPE;



/**
* This flag will activate frame delivery.
*
* In this transmission mode the S2_send_data will try
* to verify that the receiver understood the sent message.
* This is done by waiting a little to see if the node will
* respond nonce report to the encrypted message. If the node
* does respond with a nonce report then the S2_send_data
* call will automatically cause the system to re-sync the node,
* and deliver the message
*
*/
#define S2_TXOPTION_VERIFY_DELIVERY 1

/**
* This flag must be present on all single cast followup messages.
*/
#define S2_TXOPTION_SINGLECAST_FOLLOWUP 2

/**
* This flag must be present on the first, and only the first single
* cast followup message in a S2 multicast transmission.
*/
#define S2_TXOPTION_FIRST_SINGLECAST_FOLLOWUP 4


/* Transmit options for ZW_SendDataEx */
typedef struct _TRANSMIT_OPTIONS_TYPE
{
  /* Destination node ID - 0xFF == all nodes */
  node_id_t destNode;
  /* Reserved */
  node_id_t bSrcNode;
  /* Transmit options*/
  TxOptions_t txOptions;
  /* Options for enabling specific Security scheme functionality */
  uint8_t txSecOptions;
  /* Security key to use for sending. */
  enum SECURITY_KEY securityKey;
  /* More transmit options */
  uint8_t txOptions2;
} TRANSMIT_OPTIONS_TYPE;


/* Transmit options for ZW_SendDataMultiEx */
typedef struct _TRANSMIT_MULTI_OPTIONS_TYPE
{
  /* Destination group ID */
  uint8_t groupID;
  /* Reserved */
  node_id_t bSrcNode;
  /* Transmit options*/
  TxOptions_t txOptions;
  /* Security key to use for sending - only S2 keys are valid. */
  enum SECURITY_KEY securityKey;
} TRANSMIT_MULTI_OPTIONS_TYPE;


#endif /* _ZW_TRANSPORT_API_H_ */

