/******************************************************************************/
/**
 * @file   sl_btmesh_capi_types.h
 * @brief  Silicon Labs Bluetooth Mesh C API
 *
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

#ifndef SL_BTMESH_CAPI_TYPES_H_
#define SL_BTMESH_CAPI_TYPES_H_

/** forward declaration for SDK use  */
struct bgbuf_t;

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sl_btmesh_compatibility_macros.h"
#include "sl_status.h"
#include "sl_btmesh_memory_config.h"
#include <stdbool.h>

/** Dummy vendor ID for Mesh specification models */
#define MESH_SPEC_VENDOR_ID 0xffff

/** Model ID list */

/**  Scene Server model ID  */
#define MESH_SCENE_SERVER_MODEL_ID 0x1203
/** Scene Setup Server model ID */
#define MESH_SCENE_SETUP_SERVER_MODEL_ID 0x1204
/** Scene Client model ID */
#define MESH_SCENE_CLIENT_MODEL_ID 0x1205
/**  LC Server model ID  */
#define MESH_LC_SERVER_MODEL_ID 0x130F
/** LC Setup Server model ID */
#define MESH_LC_SETUP_SERVER_MODEL_ID 0x1310
/** LC Client model ID */
#define MESH_LC_CLIENT_MODEL_ID 0x1311
/** Scheduler Server model ID */
#define MESH_SCHEDULER_SERVER_MODEL_ID       0x1206
/** Scheduler Server Setup Server model ID **/
#define MESH_SCHEDULER_SETUP_SERVER_MODEL_ID 0x1207
/** Scheduler Client model ID */
#define MESH_SCHEDULER_CLIENT_MODEL_ID       0x1208

#if FEATURE_MESH_GATT

/** Mesh GATT provisioning service UUID */
#define MESH_GATT_SERVICE_PROVISIONING_UUID "\x27\x18"
/** Mesh GATT provisioning service data in characteristic UUID */
#define MESH_GATT_SERVICE_PROVISIONING_DATA_IN_UUID "\xdb\x2a"
/** Mesh GATT provisioning service data out characteristic UUID */
#define MESH_GATT_SERVICE_PROVISIONING_DATA_OUT_UUID "\xdc\x2a"

/** Mesh GATT proxy service UUID */
#define MESH_GATT_SERVICE_PROXY_UUID "\x28\x18"
/** Mesh GATT proxy service data in characteristic UUID */
#define MESH_GATT_SERVICE_PROXY_DATA_IN_UUID "\xdd\x2a"
/** Mesh GATT proxy service data out characteristic UUID */
#define MESH_GATT_SERVICE_PROXY_DATA_OUT_UUID "\xde\x2a"

/** Advertising data does not match requested network ID or node identity */
#define MESH_PROXY_SERVICE_NO_MATCH       0
/** Advertising data matches requested network ID */
#define MESH_PROXY_SERVICE_NETWORK_MATCH  1
/** Advertising data matches requested node identity */
#define MESH_PROXY_SERVICE_IDENTITY_MATCH 2

/** Advertise Mesh GATT proxy service with network ID */
#define MESH_GATT_SERVICE_PROXY_NETWORK_ID_ADV 0x00

/** Advertise Mesh GATT proxy service with node identity */
#define MESH_GATT_SERVICE_PROXY_NODE_IDENTITY_ADV 0x01

#endif /* FEATURE_MESH_GATT */

/* Provisioning algortihm bits */
/** Use FIPS-P256 for provisioning */
#define MESH_PROV_FLAG_ALGORITHM_FIPS_P256  0x0001

/* Public key type bits */
/** Out of band public key is available for provisioning */
#define MESH_PROV_FLAG_PUBLIC_KEY_AVAILABLE 0x01

/* Static OOB type bits */
/** Out of band static authentication data is available for provisioning */
#define MESH_PROV_FLAG_STATIC_OOB_AVAILABLE 0x01

/** Do not use out-of-band data in provisioning */
#define MESH_PROV_FLAG_NO_OOB 0x01
/** Use static out-of-band data in provisioning */
#define MESH_PROV_FLAG_STATIC_OOB 0x02
/** Use output out-of-band data in provisioning */
#define MESH_PROV_FLAG_OUTPUT_OOB 0x04
/** Use input out-of-band data in provisioning */
#define MESH_PROV_FLAG_INPUT_OOB 0x08

/** Flag indicating device support for out-of-band output
    authentication using blinking */
#define MESH_PROV_FLAG_OUTPUT_OOB_BLINK   0x01
/** Flag indicating device support for out-of-band output
    authentication using beeping */
#define MESH_PROV_FLAG_OUTPUT_OOB_BEEP    0x02
/** Flag indicating device support for out-of-band output
    authentication using vibration */
#define MESH_PROV_FLAG_OUTPUT_OOB_VIBRATE 0x04
/** Flag indicating device support for out-of-band output
    authentication using numeric display */
#define MESH_PROV_FLAG_OUTPUT_OOB_NUMERIC 0x08
/** Flag indicating device support for out-of-band output
    authentication using alphanumeric display */
#define MESH_PROV_FLAG_OUTPUT_OOB_ALPHA   0x10
/** Bitmask for valid out-of-band output authentication
    flags */
#define MESH_PROV_MASK_OUTPUT_OOB 0x1f

/** Flag indicating device support for out-of-band input
    authentication using push action */
#define MESH_PROV_FLAG_INPUT_OOB_PUSH   0x01
/** Flag indicating device support for out-of-band input
    authentication using twist action */
#define MESH_PROV_FLAG_INPUT_OOB_TWIST    0x02
/** Flag indicating device support for out-of-band input
    authentication using numeric input */
#define MESH_PROV_FLAG_INPUT_OOB_NUMERIC  0x04
/** Flag indicating device support for out-of-band input
    authentication using alphanumeric input */
#define MESH_PROV_FLAG_INPUT_OOB_ALPHA    0x08
/** Bitmask for valid out-of-band input authentication
    flags */
#define MESH_PROV_MASK_INPUT_OOB 0x0f

/** FIPS P-256 provisioning algorithm used in provisioning */
#define MESH_PROV_ALGORITHM_FIPS_P256 0x00

/** Out-of-band EC public key is not used in provisioning */
#define MESH_PROV_OOB_PUBLIC_KEY_NO   0x00
/** Out-of-band EC public key is used in provisioning */
#define MESH_PROV_OOB_PUBLIC_KEY_YES  0x01

/** Out-of-band authentication is not used in provisioning */
#define MESH_PROV_AUTH_OOB_NONE   0x00
/** Out-of-band static authentication is used in provisioning */
#define MESH_PROV_AUTH_OOB_STATIC 0x01
/** Out-of-band output authentication is used in provisioning */
#define MESH_PROV_AUTH_OOB_OUTPUT 0x02
/** Out-of-band input authentication is used in provisioning */
#define MESH_PROV_AUTH_OOB_INPUT  0x03

/** Out-of-band output authentication by blinking is used in provisioning */
#define MESH_PROV_OUTPUT_OOB_BLINK    0x00
/** Out-of-band output authentication by beeping is used in provisioning */
#define MESH_PROV_OUTPUT_OOB_BEEP   0x01
/** Out-of-band output authentication by vibration is used in provisioning */
#define MESH_PROV_OUTPUT_OOB_VIBRATE  0x02
/** Out-of-band output authentication by numeric output is used in provisioning */
#define MESH_PROV_OUTPUT_OOB_NUMERIC  0x03
/** Out-of-band output authentication by alphanumeric output is used in provisioning */
#define MESH_PROV_OUTPUT_OOB_ALPHA    0x04

/** Out-of-band input authentication by push action is used in provisioning */
#define MESH_PROV_INPUT_OOB_PUSH    0x00
/** Out-of-band input authentication by twist action is used in provisioning */
#define MESH_PROV_INPUT_OOB_TWIST   0x01
/** Out-of-band input authentication by numeric input is used in provisioning */
#define MESH_PROV_INPUT_OOB_NUMERIC   0x02
/** Out-of-band input authentication by alphanumeric input is used in provisioning */
#define MESH_PROV_INPUT_OOB_ALPHA   0x03

/** Key refresh is in progress */
#define MESH_PROV_SETUP_KR_IN_PROGRESS  0x01
/** IV index update is in progress */
#define MESH_PROV_SETUP_IVU_IN_PROGRESS 0x02

/** Advertisement provisioning bearer (PB-ADV) */
#define MESH_PROV_BEARER_ID_ADV   0x00
#if FEATURE_MESH_GATT
/** GATT provisioning bearer (PB-GATT) */
#define MESH_PROV_BEARER_ID_GATT  0x01
/** All available provisioning bearers */
#define MESH_PROV_BEARERS_MASK      ((1 << MESH_PROV_BEARER_ID_ADV) | (1 << MESH_PROV_BEARER_ID_GATT))
#else
/** All available provisioning bearers */
#define MESH_PROV_BEARERS_MASK      (1 << MESH_PROV_BEARER_ID_ADV)
#endif /* FEATURE_MESH_GATT */

/** Node is blacklisted and thus does not participate in key refresh */
#define MESH_KR_NODE_IS_BLACKLISTED 1

/** Node is not blacklisted and thus does participate in key refresh */
#define MESH_KR_NODE_IS_NOT_BLACKLISTED 0

#define MESH_PLATFORM_MEMPOOL_ID_DDB        0 /**< Device database pool */
#define MESH_PLATFORM_MEMPOOL_ID_FOUNDATION 1 /**< Foundation command pool */
#define MESH_PLATFORM_MEMPOOL_ID_DDB_APPKEY 2 /**< DDB appkey blacklist pool */
#define MESH_PLATFORM_MEMPOOL_COUNT         3 /**< Memory pool count */

/** Model handle */
typedef struct __mesh_model *mesh_model_t;

/** Element handle */
typedef struct __mesh_element *mesh_element_t;

/** Primary element index */
#define MESH_PRIMARY_ELEMENT_INDEX 0

/** Maximum useful Access message size including an opcode */
#define MESH_MAX_ACCESS_MESSAGE_SIZE 380

/** Mesh advertisement types */
typedef enum {
  mesh_packet_undefined = 0x00, /** Undefined packet type */
  mesh_packet_service_data = 0x016, /** Service data */
  mesh_packet_uri = 0x24, /**< URI advertisement */
  mesh_packet_provisioning = 0x29, /**< Provisioning data */
  mesh_packet_generic = 0x2a, /**< Mesh data */
  mesh_packet_beacon = 0x2b, /**< Mesh beacon */
} mesh_packet_type_t;

/** Mesh bearer types */
typedef enum {
  mesh_bearer_adv = 0, /**< Advertisement bearer */
#if FEATURE_MESH_GATT
  mesh_bearer_gatt = 1 /**< GATT bearer */
#endif /* FEATURE_MESH_GATT */
} mesh_bearer_t;

/** Network layer configuration structure */
typedef struct {
  uint8_t relay_interval_min; /**< Minimum relay delay */
  uint8_t relay_interval_max; /**< Maximum relay delay */
} mesh_net_config_t;

/** SAR Transmitter Configuration */
typedef struct mesh_sar_transmitter_config {
  uint32_t segment_interval_step; /**< Segment Interval Step */
  uint32_t unicast_retrans_count; /**< Unicast Retransmissions Count */
  uint32_t unicast_retrans_wo_progress_count; /**< Unicast Retransmissions Count Without Progress */
  uint32_t unicast_retrans_interval_step; /**< Unicast Retransmission Interval Step */
  uint32_t unicast_retrans_interval_increment; /**< Unicast Retransmission Interval Increment */
  uint32_t multicast_retrans_count; /**< Multicast Retransmissions Count */
  uint32_t multicast_retrans_interval_step; /**< Multicast Retransmission Interval Step */
} mesh_sar_transmitter_config_t;

/** SAR Receiver Configuration */
typedef struct mesh_sar_receiver_config {
  uint32_t segments_threshold; /**< Segments Threshold */
  uint32_t ack_delay_increment; /**< Acknowledgement Delay Increment */
  uint32_t discard_timeout; /**< Discard Timeout */
  uint32_t segment_interval_step; /**< Segment Interval Step */
  uint32_t ack_retrans_count; /**< Acknowledgement Retransmissions Count */
} mesh_sar_receiver_config_t;

/** Segmentation and reassembly configuration structure */
typedef struct {
  mesh_sar_transmitter_config_t transmitter; /**< SAR Transmitter Configuration */
  mesh_sar_receiver_config_t receiver;/**< SAR Receiver Configuration */
} mesh_trans_sar_config_t;

/**
 * Mesh device configuration structure.
 */
typedef struct {
  const mesh_net_config_t *p_net_config; /**< Network layer configuration */
  const mesh_trans_sar_config_t *p_trans_sar_config; /**< Transport layer confoiguration */
  const mesh_memory_config_t *p_memory_config; /**< Memory configuration */
  uint8_t flags; /**< Node flags */
  /* The following fields are only relevant for the Node. */
  const uint8_t *p_uuid; /**< Pointer to device UUID */
  uint16_t oob_location;   /**< OOB location bitmask */
  uint8_t oob_public_key;   /**< OOB public key availability */
  uint8_t oob_auth_methods;   /**< OOB authentication methods supported */
  uint8_t oob_auth_output_actions; /**< Output OOB actions supported bitmask */
  uint8_t oob_auth_output_size; /**< Output OOB size */
  uint8_t oob_auth_input_actions; /**< Input OOB actions supported bitmask */
  uint8_t oob_auth_input_size; /**< Input OOB size */
  uint16_t allowed_algorithms; /**< Allowed provisioning algorithms; set
                                  to bitmask containing #MESH_PROV_FLAG_ALGORITHM_FIPS_P256 */
} mesh_device_config_t;

/** Device UUID length */
#define MESH_UUID_LEN   (16)

/** Mesh size */
typedef uint16_t mesh_size_t;

/** Mesh bearer */
typedef uint8_t mesh_bearer_id;

/** Mesh address */
typedef uint16_t mesh_addr_t;

/** Mesh label UUID (full virtual address data) */
typedef struct {
  uint8_t addr[16]; /**< Address data */
} mesh_full_virtual_addr_t;

/** A special nil label UUID */
extern const mesh_full_virtual_addr_t MESH_VA_NIL;

/** Unassigned address */
#define MESH_ADDR_UNASSIGNED            0x0000
/** All IPT border routers address */
#define MESH_ADDR_ALLIPTBORDERROUTERS   0xfff9
/** All IPT nodes address */
#define MESH_ADDR_ALLIPTNODES           0xfffa
/** All nodes with directed forwarding address */
#define MESH_ADDR_ALLDIRECEDTFORWARDING 0xfffb
/** All-proxies address */
#define MESH_ADDR_ALLPROXIES            0xfffc
/** All-friends address */
#define MESH_ADDR_ALLFRIENDS            0xfffd
/** All-relays address */
#define MESH_ADDR_ALLRELAYS             0xfffe
/** All-nodes address (broadcast) */
#define MESH_ADDR_ALLNODES              0xffff

/** Mesh address types */
typedef enum {
  MESH_ADDR_TYPE_UNASSIGNED, /**< Unassigned */
  MESH_ADDR_TYPE_UNICAST, /**< Unicast address */
  MESH_ADDR_TYPE_VIRTUAL, /**< Virtual address (hash of Label UUID) */
  MESH_ADDR_TYPE_GROUP, /**< Group address */
  MESH_ADDR_TYPE_FIXEDGROUP /**< Fixed group address */
}  mesh_addr_type_t;

/**
 * @brief Get address type
 *
 * Get address type of an address
 * @param addr Mesh address
 * @return Type of the address
 */
inline mesh_addr_type_t mesh_addr_type(mesh_addr_t addr)
{
  switch ((addr >> 14) & 3) {
    case 0:
      return (addr == MESH_ADDR_UNASSIGNED)
             ? MESH_ADDR_TYPE_UNASSIGNED
             : MESH_ADDR_TYPE_UNICAST;
    case 1:
      return MESH_ADDR_TYPE_UNICAST;
    case 2:
      return MESH_ADDR_TYPE_VIRTUAL;
    default:
      return (addr >= 0xff00)
             ? MESH_ADDR_TYPE_FIXEDGROUP
             : MESH_ADDR_TYPE_GROUP;
  }
}

/** Mesh message MIC sizes */
typedef enum {
  mesh_mic_32 = 0,   // Explicitly 32 bit MIC
  mesh_mic_64 = 1,   // Explicitly 64 bit MIC
  mesh_mic_longest = 2,   // Use longest MIC which can fit in the PDU
                          // without causing extra overhead (e.g. segmentation)
} mesh_mic_t;

/** Mesh node handle */
typedef uint8_t mesh_node_handle_t;
/** Mesh application handle
    @deprecated Not used */
typedef uint8_t mesh_app_handle_t;
/** Mesh component handle
    @deprecated Not used */
typedef uint32_t mesh_component_handle_t;
/** Mesh packet handle
    @deprecated Not used */
typedef uint32_t mesh_packet_handle_t;
/** Mesh Tx PDU handle */
typedef void* mesh_tx_handle_t;

/** Invalid Tx handle */
#define MESH_INVALID_TX_HANDLE NULL

/** Transport layer Tx related event types */
enum mesh_trans_tx_event_type {
  mesh_trans_tx_event_obo_ack, /**< OBO acknowledgement (friend) */
  mesh_trans_tx_event_sdu_send_started, /**< Segmented TX started */
  mesh_trans_tx_event_sdu_send_completed, /**< TX completed */
  mesh_trans_tx_event_sdu_send_failed, /**< TX Failed */
};

/** Transport layer Tx related event reporting structure */
typedef struct {
  enum mesh_trans_tx_event_type type; /**< Event type */
  union {
    struct {
      mesh_addr_t obo_src; /**< friend address */
    } obo_ack; /**< On-behalf-of acknowledgement received */
  };
} mesh_trans_tx_event_t;

typedef uint8_t mesh_uuid_t[16]; /**< UUID */

/** Callback for events related to transport layer sending */
typedef void (*mesh_trans_tx_event_cb)(mesh_trans_tx_event_t *event,
                                       void *cb_param);

#if FEATURE_MESH_GATT

/**
 * GATT service
 */
typedef enum {
  mesh_gatt_service_proxy, /**< Mesh proxy service */
  mesh_gatt_service_provisioning /**< Mesh provisioning service */
} mesh_gatt_service_t;

/**
 * GATT connection handle
 */
typedef void* mesh_gatt_handle_t;

/**
 * GATT connection event types
 */
typedef enum {
  mesh_gatt_event_connected,
  mesh_gatt_event_disconnected,
  mesh_gatt_event_data
} mesh_gatt_event_type_t;

/**
 * GATT PDU type
 */
typedef enum {
  mesh_gatt_pdu_data = 0,
  mesh_gatt_pdu_beacon = 1,
  mesh_gatt_pdu_proxyconfig = 2,
  mesh_gatt_pdu_provisioning = 3,
} mesh_gatt_pdu_t;

/**
 * GATT event structure
 */
typedef struct {
  mesh_gatt_event_type_t type; /**< Event type */
  union {
    struct {
      uint8_t addr[6]; /**< Remote address */
    } connect; /**< Connection event */
    struct {
      sl_status_t reason; /**< Disconnection reason */
    } disconnect; /**< Disconnection event */
    struct {
      struct bgbuf_t *buf; /**< Data contents */
    } data; /**< Incoming data */
  };
} mesh_gatt_event_t;

/**
 * GATT event handler callback
 */
typedef void (*mesh_gatt_cb)(mesh_gatt_handle_t handle,
                             mesh_gatt_event_t *event,
                             void *cb_param);

#endif /* FEATURE_MESH_GATT */

/** Mesh AES cryptographic key type */
typedef enum {
  MESH_CRYPTO_KEY_NET = 0, /**< 128 bit network key */
  MESH_CRYPTO_KEY_APP, /**< 128 bit application key */
  MESH_CRYPTO_KEY_DEV, /**< 128 bit device key */
  MESH_CRYPTO_KEY_NONE, /**< 128 bit AES key with no mesh context */
  MESH_CRYPTO_KEY_NONE_256, /**< 256 bit AES key with no mesh context */
} mesh_crypto_key_type_t;

/** Key refresh phase */
typedef enum {
  MESH_CRYPTO_KEY_REFRESH_PHASE_NORMAL, /**< No key refresh ongoing */
  MESH_CRYPTO_KEY_REFRESH_PHASE_1, /**< Key refresh phase 1 */
  MESH_CRYPTO_KEY_REFRESH_PHASE_2, /**< Key refresh phase 2 */
  MESH_CRYPTO_KEY_REFRESH_PHASE_3, /**< Key refresh phase 3 */
} mesh_crypto_key_refresh_phase_t;

/** Mesh AES cryptographic key index */
typedef uint16_t mesh_crypto_key_index_t;

/** Invalid AES cryptographic key index */
#define MESH_CRYPTO_KEY_INDEX_INVALID 0xffff

/** Mesh AES cryptographic key length */
#define MESH_CRYPTO_KEY_LEN 16

/** Mesh AES 256 bit cryptographic key length */
#define MESH_CRYPTO_KEY_256_LEN 32

/** Mesh AES cryptographic key opaque structure */
struct mesh_crypto_key;

/** Mesh AES cryptographic key handle */
typedef struct mesh_crypto_key mesh_crypto_key_t;

/** Mesh AES MAC data maximum length */
#define MESH_CRYPTO_MAC_LEN 16

/** Mesh AES cryptographic MAC structure */
struct mesh_crypto_mac {
  uint8_t data[MESH_CRYPTO_MAC_LEN]; /**< MAC data */
};

/** Mesh AES cryptographic MAC type */
typedef struct mesh_crypto_mac mesh_crypto_mac_t;

/** Mesh HMAC-SHA-256 data length */
#define MESH_CRYPTO_HMAC_SHA_256_LEN 32

/** Mesh HMAC-SHA-256 structure */
struct  mesh_crypto_hmac_sha_256 {
  uint8_t data[MESH_CRYPTO_HMAC_SHA_256_LEN]; /**< HMAC-SHA-256 data */
};

/** Mesh HMAC-SHA-256 type */
typedef struct mesh_crypto_hmac_sha_256 mesh_crypto_hmac_sha_256_t;

/** Mesh SHA-1 data length */
#define MESH_CRYPTO_SHA_1_LEN 20

/** Mesh SHA-1 MAC type */
typedef struct {
  uint8_t data[MESH_CRYPTO_SHA_1_LEN]; /**< SHA-1 digest */
} mesh_crypto_sha1_t;

/** Mesh elliptic curve cryptographic public key length */
#define MESH_CRYPTO_EC_PUBLIC_KEY_LEN 64
/** Mesh elliptic curve cryptographic private key length */
#define MESH_CRYPTO_EC_PRIVATE_KEY_LEN  32

/** Mesh elliptic curve cryptographic key pair opaque structure */
struct mesh_crypto_ec_key;

/** Mesh elliptic curve cryptographic key pair handle */
typedef struct mesh_crypto_ec_key mesh_crypto_ec_key_t;

/** Mesh EC Diffie-Hellman shared secret opaque strcture */
struct mesh_crypto_ecdh_secret;

/** Mesh EC Diffie-Hellman shared secret handle */
typedef struct mesh_crypto_ecdh_secret mesh_crypto_ecdh_secret_t;

/** Mesh provisioning authentication data length */
#define MESH_CRYPTO_AUTH_LEN 16

/** Incoming packet */
#define MESH_PACKET_DIR_IN      0
/** Outgoing packet */
#define MESH_PACKET_DIR_OUT     1

/** Advertisement packet */
#define MESH_PACKET_TYPE_ADV    0

#define MESH_NODE_FLAG_DEBUG        1   /**< Debugging enabled */
#define MESH_NODE_FLAG_PROVISIONER  2   /**< Provisioner role */
#define MESH_NODE_FLAG_CFG_CLIENT   4   /**< Node with config client */

#define MESH_MAX_ACCESS_PDU_LEN (384)   /**< Maximum access PDU including MIC */

#define MESH_MAX_ACCESS_MESSAGE_LEN (380)   /**< Maximum access PDU excluding MIC */

/**
 * Mesh node state
 */
typedef enum {
  mesh_device_state_uninitialized = 0,
  mesh_device_state_unprovisioned,
  mesh_device_state_unprovisioned_beaconing,
  mesh_device_state_provisioning,
  mesh_device_state_operational,
  mesh_device_state_provisioner,
  mesh_device_state_last
} mesh_device_state_t;

/** Mesh provisioning process state */
typedef enum {
  mesh_prov_beaconing = 0,
  mesh_prov_link_open = 1,
  mesh_prov_key_exchange = 2,
  mesh_prov_finished = 3,
} mesh_provisioning_step_t;

/* Message flags */

// Replies may get artificially delayed by a random amount to avoid
//
// 1) sending a reply before original sender has managed to switch to
//    listening
//
// 2) clobbering neighbours' transmissions when multiple nodes respond
//    to the same message
//
/** Message is an access layer reply to a unicast request */
#define MESH_MESSAGE_FLAG_REPLY       0x01
/** Message is an access layer reply to a multicast request */
#define MESH_MESSAGE_FLAG_MULTICAST_REPLY 0x02

/** Message is either received with TTL=0 or is a response to such a message
    and should be sent out with TTL=0 */
#define MESH_MESSAGE_FLAG_TTL0  0x04

/** Force the use of transport layer segmentation */
#define MESH_MESSAGE_FLAG_SEGTRANS 0x10

/** Use friendship credentials instead of normal credentials
    when sending data; used when publishing with friendship
    credential flag on in model publish settings */
#define MESH_MESSAGE_FLAG_FRIENDCRED 0x20

/** Use extended packet size instead of standard */
#define MESH_MESSAGE_FLAG_EXTENDED 0x40
/** long delay needed in sending two messages in reply that need to be sent in order */
#define MESH_MESSAGE_FLAG_LONG_DELAY  0x80

/** Bearer layer message sending callback signature
 *  When the bearer packet has been sent, the callback
 *  will be called to indicate that.
 */
typedef void (*mesh_send_cb)(sl_status_t status,
                             mesh_tx_handle_t handle,
                             void *cb_param);

/* Time */

/** CPU tick or some other similar high resolution time unit */
typedef uint64_t mesh_tick_t;

/** Maximum value representable with a 64-bit uint */
#define MESH_MAX_TICK           0xffffffffffffffff

/** Low resolution time (in seconds) that advances during sleep
    modes. Used only for counting relative time differences; Epoch
    value is not important. */
typedef uint32_t mesh_clock_t;

/** Persistent storage handle to data */
typedef uint16_t mesh_pstore_handle_t;

/** Mesh model type */
typedef enum {
  MESH_MODEL_BT_SIG, /**< SIG model */
  MESH_MODEL_VENDOR /**< Vendor model */
} mesh_model_type_t;

/** Model publication state */
typedef struct {
  mesh_addr_t addr;   /**< Publication address */
  mesh_full_virtual_addr_t va;   /**< Full virtual address data; needed if
                                    publication address is a virtual one */
  mesh_crypto_key_index_t key_index;   /**< Publication app key */
  uint8_t ttl;   /**< Publication time to live */
  uint8_t period;   /**< Publication period */
  uint8_t retrans;   /**< Retransmission count and interval */
  uint8_t friendcred : 1; /**< Use friendship credentials flag */
} mesh_model_publish_t;

/** Model transmission options */
typedef struct {
  uint8_t extended_packet_size : 1; /**< Use extended packet size */
} mesh_model_tx_options_t;

/** Cryptographic key event */
typedef enum {
  key_added = 0,
  key_updated = 1,
  key_deleted = 2,
  key_last,
} mesh_key_event_t;

/** Model configuration event */
typedef enum {
  model_appkey_bindings_changed = 0,
  model_publication_changed = 1,
  model_subscriptions_changed = 2,
  model_last,
} mesh_model_config_event_t;

/** Dummy vendor ID for Mesh specification models
    @deprecated Use MESH_SPEC_VENDOR_ID instead */
#define MESH_VENDOR_ID_SIG      0xffff

/** TTL value indicating default TTL should be used */
#define MESH_USE_DEFAULT_TTL (255)

/** Instrumentation event */
typedef enum {
  mesh_instr_hello = 0x00,

  mesh_instr_net_pdu_decryption_failure = 0x01,
  mesh_instr_net_pdu_decrypted = 0x02,
  mesh_instr_net_pdu_dropped = 0x03,
  mesh_instr_net_pdu_handled = 0x04,
  mesh_instr_net_pdu_not_handled = 0x05,
  mesh_instr_net_pdu_relayed = 0x06,
  mesh_instr_net_pdu_not_relayed = 0x07,
  mesh_instr_net_pdu_sent = 0x08,
  mesh_instr_net_pdu_not_sent = 0x09,

  mesh_instr_trans_pdu_handled = 0x0a,
  mesh_instr_trans_pdu_not_handled = 0x0b,
  mesh_instr_trans_receiver_start = 0x0c,
  mesh_instr_trans_receiver_cleanup = 0x0d,
  mesh_instr_trans_receiver_recv = 0x0e,
  mesh_instr_trans_receiver_ack_sent = 0x0f,
  mesh_instr_trans_receiver_ack_not_sent = 0x10,
  mesh_instr_trans_receiver_timer_set = 0x11,
  mesh_instr_trans_receiver_timer_expired = 0x12,

  mesh_instr_trans_sender_start = 0x13,
  mesh_instr_trans_sender_table_full = 0x14,
  mesh_instr_trans_sender_cleanup = 0x15,
  mesh_instr_trans_sender_ack_handled = 0x16,
  mesh_instr_trans_sender_ack_not_handled = 0x17,
  mesh_instr_trans_sender_pdu_sent = 0x18,
  mesh_instr_trans_sender_pdu_not_sent = 0x19,
  mesh_instr_trans_sender_sdu_sent = 0x1a,
  mesh_instr_trans_sender_sdu_not_sent = 0x1b,
  mesh_instr_trans_sender_timer_set = 0x1c,
  mesh_instr_trans_sender_timer_expired = 0x1d,

  mesh_instr_adv_sent = 0x1e,
  mesh_instr_adv_not_sent = 0x1f,
  mesh_instr_adv_queue_push = 0x20,
  mesh_instr_adv_queue_pop = 0x21,

  mesh_instr_tx_complete = 0x22,
  mesh_instr_tx_cancelled = 0x23,
} mesh_instr_event_t;

/** Instrumentation result codes for dropping a network PDU */
typedef enum {
  mesh_instr_net_pdu_dropped_cache_hit = 0x00,
  mesh_instr_net_pdu_dropped_invalid_src = 0x01,
  mesh_instr_net_pdu_dropped_invalid_dst = 0x02,
  mesh_instr_net_pdu_dropped_internal_error = 0x03,
} mesh_instr_net_pdu_dropped_reason_t;
/** Number of instrumentation errors for dropping a network PDU */
#define mesh_instr_net_pdu_dropped_reasons 4

/** Instrumentation result codes for not handling a network PDU */
typedef enum {
  mesh_instr_net_pdu_not_handled_not_for_me = 0x00,
} mesh_instr_net_pdu_not_handled_reason_t;
/** Number of instrumentation errors for not handling a network PDU */
#define mesh_instr_net_pdu_not_handled_reasons 1

/** Instrumentation result codes for not relaying a network PDU */
typedef enum {
  mesh_instr_net_pdu_not_relayed_no_relay = 0x00,
  mesh_instr_net_pdu_not_relayed_hop_limit = 0x01,
  mesh_instr_net_pdu_not_relayed_out_of_memory = 0x02,
  mesh_instr_net_pdu_not_relayed_internal_error = 0x03,
} mesh_instr_net_pdu_not_relayed_reason_t;
/** Number of instrumentation errors for not relaying a network PDU */
#define mesh_instr_net_pdu_not_relayed_reasons 4

/** Instrumentation result codes for not sending a network PDU */
typedef enum {
  mesh_instr_net_pdu_not_sent_out_of_memory = 0x00,
  mesh_instr_net_pdu_not_sent_internal_error = 0x01,
  mesh_instr_net_pdu_not_sent_hop_limit = 0x02,
} mesh_instr_net_pdu_not_sent_reason_t;
/** Number of instrumentation errors for not sending a network PDU */
#define mesh_instr_net_pdu_not_sent_reasons 3

/** Instrumentation result codes for not handling a transport PDU */
typedef enum {
  mesh_instr_trans_pdu_not_handled_invalid_pdu = 0x00,
  mesh_instr_trans_pdu_not_handled_invalid_src = 0x01,
  mesh_instr_trans_pdu_not_handled_invalid_dst = 0x02,
  mesh_instr_trans_pdu_not_handled_invalid_seg = 0x03,
  mesh_instr_trans_pdu_not_handled_old_seqauth = 0x04,
  mesh_instr_trans_pdu_not_handled_old_data = 0x05,
  mesh_instr_trans_pdu_not_handled_out_of_recv = 0x06,
  mesh_instr_trans_pdu_not_handled_out_of_memory = 0x07,
  mesh_instr_trans_pdu_not_handled_header_mismatch = 0x08,
} mesh_instr_trans_pdu_not_handled_reason_t;
/** Number of instrumentation errors for not handling a transport PDU */
#define mesh_instr_trans_pdu_not_handled_reasons 9

/** Instrumentation result codes for not sending a transport layer acknowledgement */
typedef enum {
  mesh_instr_trans_ack_not_sent_out_of_memory = 0x00,
  mesh_instr_trans_ack_not_sent_internal_error = 0x01,
} mesh_instr_trans_ack_not_sent_reason_t;
/** Number of instrumentation errors for not sending transport layer acknowledgement */
#define mesh_instr_trans_receiver_ack_not_sent_reasons 2

/** Instrumentation result codes for not handling a transport layer acknowledgement */
typedef enum {
  mesh_instr_trans_ack_not_handled_not_found = 0x00,
  mesh_instr_trans_ack_not_handled_non_cumulative = 0x01,
} mesh_instr_trans_ack_not_handled_reason_t;
/** Number of instrumentation errors for not handling transport layer acknowledgement */
#define mesh_instr_trans_sender_ack_not_handled_reasons 2

/** Instrumentation result codes for receiving a transport layer message */
typedef enum {
  mesh_instr_trans_receive_success = 0x00,
  mesh_instr_trans_receive_incomplete = 0x01,
  mesh_instr_trans_receive_sender_cancelled = 0x02,
  mesh_instr_trans_receive_receiver_cancelled = 0x03,
} mesh_instr_trans_receive_status_t;
/** Number of instrumentation errors for transport layer message reception */
#define mesh_instr_trans_receiver_receive_reasons 3

/** Instrumentation result codes for not sending a transport layer PDU */
typedef enum {
  mesh_instr_trans_pdu_not_sent_wrong_state = 0x00,
  mesh_instr_trans_pdu_not_sent_out_of_memory = 0x01,
  mesh_instr_trans_pdu_not_sent_internal_error = 0x02,
} mesh_instr_trans_pdu_not_sent_reason_t;
/** Number of instrumentation errors for not sending transport layer PDU */
#define mesh_instr_trans_sender_pdu_not_sent_reasons 3

/** Instrumentation result codes for not sending a transport layer SDU */
typedef enum {
  mesh_instr_trans_sdu_not_sent_invalid_src = 0x00,
  mesh_instr_trans_sdu_not_sent_invalid_dst = 0x01,
  mesh_instr_trans_sdu_not_sent_invalid_key = 0x02,
  mesh_instr_trans_sdu_not_sent_wrong_state = 0x03,
  mesh_instr_trans_sdu_not_sent_out_of_memory = 0x04,
  mesh_instr_trans_sdu_not_sent_internal_error = 0x05,
} mesh_instr_trans_sdu_not_sent_reason_t;
/** Number of instrumentation errors for not sending transport layer SDU */
#define mesh_instr_trans_sender_sdu_not_sent_reasons 6

/** Instrumentation result codes for sending a transport layer message */
typedef enum {
  mesh_instr_trans_send_success = 0x00,
  mesh_instr_trans_send_timeout = 0x01,
  mesh_instr_trans_send_receiver_cancelled = 0x02,
  mesh_instr_trans_send_out_of_memory = 0x03,
  mesh_instr_trans_send_internal_error = 0x04,
  mesh_instr_trans_send_sender_cancelled = 0x05,
} mesh_instr_trans_send_status_t;
/** Number of instrumentation errors for transport layer message sending */
#define mesh_instr_trans_sender_send_reasons 5

/** Mesh stack statistics counters */
typedef struct {
  /** Network decryption failure counter */
  uint32_t net_pdu_decryption_failure;
  /** Network decryption success counter */
  uint32_t net_pdu_decrypted;
  /** Network PDU dropped counter */
  uint32_t net_pdu_dropped[mesh_instr_net_pdu_dropped_reasons];
  /** Network PDU handled counter */
  uint32_t net_pdu_handled;
  /** Network PDU not handled counter */
  uint32_t net_pdu_not_handled[mesh_instr_net_pdu_not_handled_reasons];
  /** Network PDU relayed counter */
  uint32_t net_pdu_relayed;
  /** Network PDU not relayed counter */
  uint32_t net_pdu_not_relayed[mesh_instr_net_pdu_not_relayed_reasons];
  /** Network PDU sent counter */
  uint32_t net_pdu_sent;
  /** Network PDU not sent counter */
  uint32_t net_pdu_not_sent[mesh_instr_net_pdu_not_sent_reasons];

  /** Transport PDU processed counter */
  uint32_t trans_pdu_handled;
  /** Transport PDU not handled counter */
  uint32_t trans_pdu_not_handled[mesh_instr_trans_pdu_not_handled_reasons];

  /** Transport SDU reception started counter */
  uint32_t trans_receiver_start;
  /** Transport SDU reception ended counter */
  uint32_t trans_receiver_cleanup[mesh_instr_trans_receiver_receive_reasons];
  /** Transport SDU received counter */
  uint32_t trans_receiver_recv;
  /** Transport acknowledgement sent counter */
  uint32_t trans_receiver_ack_sent;
  /** Transport acknowledgement not sent counter */
  uint32_t trans_receiver_ack_not_sent[mesh_instr_trans_receiver_ack_not_sent_reasons];
  /** Transport receive timer set counter */
  uint32_t trans_receiver_timer_set;
  /** Transport receive timer expired counter */
  uint32_t trans_receiver_timer_expired;

  /** Transport sender started counter */
  uint32_t trans_sender_start;
  /** Transport sender out of resources counter */
  uint32_t trans_sender_table_full;
  /** Transport sender ended counter */
  uint32_t trans_sender_cleanup[mesh_instr_trans_sender_send_reasons];
  /** Transport acknowledgement handled counter */
  uint32_t trans_sender_ack_handled;
  /** Transport acknowledgement not handled counter */
  uint32_t trans_sender_ack_not_handled[mesh_instr_trans_sender_ack_not_handled_reasons];
  /** Transport PDU sent counter */
  uint32_t trans_sender_pdu_sent;
  /** Transport PDU not sent counter */
  uint32_t trans_sender_pdu_not_sent[mesh_instr_trans_sender_pdu_not_sent_reasons];
  /** Transport SDU sent counter */
  uint32_t trans_sender_sdu_sent;
  /** Transport SDU not sent counter */
  uint32_t trans_sender_sdu_not_sent[mesh_instr_trans_sender_sdu_not_sent_reasons];
  /** Transport sender timer set counter */
  uint32_t trans_sender_timer_set;
  /** Transport sender timer expired counter */
  uint32_t trans_sender_timer_expired;
} mesh_statistics_t;

/** Pointer to current mesh statistics */
extern mesh_statistics_t *mesh_stats;

/** Instrumentation result codes for not sending an advertisement */
typedef enum {
  mesh_instr_adv_not_sent_out_of_memory = 0x00,
  mesh_instr_adv_not_sent_internal_error = 0x01,
} mesh_instr_adv_not_sent_reason_t;

/** Instrumentation result codes for removing an advertisement from send queue */
typedef enum {
  mesh_instr_adv_queue_pop_finished = 0x00,
  mesh_instr_adv_queue_pop_cancelled = 0x01,
} mesh_instr_adv_queue_pop_reason_t;

/** Convert integer to pointer */
#define MESH_I2P(i) ((void *)((uintptr_t)(i)))

/** Convert pointer to integer */
#define MESH_P2I(p) ((uintptr_t)(p))

/** Scheduled callback function.
    A scheduled callback will be called by
    the platform once a request made by the mesh stack, by calling
    mesh_platform_callback_schedule() has expired. */
typedef void (*mesh_scheduled_cb_fn)(uint32_t handle, void *ctx);

/** Scheduled callback function. for non-cancellable events
    A scheduled callback will be called by
    the platform once a request made by the mesh stack, by calling
    mesh_platform_callback_schedule_event() has expired. */
typedef void (*mesh_scheduled_event_cb_fn)(uint32_t event, void *ctx);

/** Stack diagnostic event types */
typedef enum {
  mesh_stack_diag_event_config_server_beacon_set,
  mesh_stack_diag_event_config_server_default_ttl_set,
  mesh_stack_diag_event_config_server_friend_set,
  mesh_stack_diag_event_config_server_gatt_proxy_set,
  mesh_stack_diag_event_config_server_nettx_set,
  mesh_stack_diag_event_config_server_node_identity_set,
  mesh_stack_diag_event_config_server_relay_set,
  mesh_stack_diag_event_config_server_beacon_get,
  mesh_stack_diag_event_config_server_default_ttl_get,
  mesh_stack_diag_event_config_server_friend_get,
  mesh_stack_diag_event_config_server_gatt_proxy_get,
  mesh_stack_diag_event_config_server_nettx_get,
  mesh_stack_diag_event_config_server_node_identity_get,
  mesh_stack_diag_event_config_server_relay_get,

  mesh_stack_diag_event_replay_protection_list_load,
  mesh_stack_diag_event_replay_protection_list_save,
  mesh_stack_diag_event_replay_protection_list_flush,
  mesh_stack_diag_event_replay_protection_list_set_entry,
  mesh_stack_diag_event_replay_protection_list_clear_entry,
  mesh_stack_diag_event_replay_protection_list_full,

  mesh_stack_diag_event_sar_config_server_transmitter_set,
  mesh_stack_diag_event_sar_config_server_receiver_set,
} mesh_stack_diag_event_type_t;

/** Diagnostic event for configuration server change */
typedef union {
  uint8_t beacon; /**< Value of beacon state after set request */
  uint8_t default_ttl; /**< Value of default TTL state after set request */
  uint8_t friend_; /**< Value of friend state after set request */
  uint8_t gatt_proxy; /**< Value of GATT proxy state after set request */
  struct {
    uint8_t count;
    uint8_t interval;
  } nettx; /**< Value of network transmit state after set request */
  struct {
    mesh_crypto_key_index_t index;
    uint8_t enabled;
  } node_identity; /**< Value of node identity state after set request */
  struct {
    uint8_t enabled;
    uint8_t count;
    uint8_t interval;
  } relay; /**< Value of relay state after set request */
} mesh_stack_diag_event_config_server_t;

/** Diagnostic event for replay protection list */
typedef union {
  struct {
    uint16_t loaded_count;
    uint16_t total_count;
    sl_status_t result;
  } load; /**< Replay protection list loaded from flash */
  struct {
    uint16_t saved_count;
    uint16_t total_count;
    sl_status_t result;
  } save; /**< Replay protection list saved to flash */
  // Nothing for flush
  struct {
    mesh_addr_t src;
    bool cancel;
  } set; /**< Replay protection list entry set */
  struct {
    mesh_addr_t src;
  } clear; /**< Replay protection list entry removed */
  // Nothing for full
} mesh_stack_diag_event_replay_protection_list_t;

/** Diagnostic event for SAR configuration server change */
typedef union {
  mesh_sar_transmitter_config_t transmitter; /**< SAR Transmitter configuration value after set request */
  mesh_sar_receiver_config_t receiver; /**< SAR Receiver configuration value after set request */
} mesh_stack_diag_event_sar_config_server_t;

/** Stack diagnostic event */
typedef struct {
  mesh_stack_diag_event_type_t type; /**< Event type */
  union {
    mesh_stack_diag_event_config_server_t config_server; /**< Config server event */
    mesh_stack_diag_event_replay_protection_list_t replay_protection_list; /**< Replay protection list event */
    mesh_stack_diag_event_sar_config_server_t sar_config_server; /**< SAR Config server event */
  };
} mesh_stack_diag_event_t;

/**
 * Stack diagnostic event handler callback
 */
typedef void (*mesh_stack_diag_event_cb)(const mesh_stack_diag_event_t *event);

/** Secure network beacon callback; called when a secure network
    beacon is successfully received */
typedef void (*mesh_network_beacon_cb)(mesh_crypto_key_index_t net_key_index,
                                       uint8_t key_refresh,
                                       uint8_t iv_update,
                                       uint32_t ivindex);

/** Unprovisioned device beacon callback; called when an unprovisioned
    device beacon is successfully received */
typedef void (*mesh_unprov_beacon_cb)(const uint8_t *uuid,
                                      uint16_t oob,
                                      const uint8_t *uri_hash,
                                      mesh_bearer_t bearer,
                                      const uint8_t *bdaddr,
                                      uint8_t bdaddr_type,
                                      int8_t rssi);

/** Application layer message sending function signature; mainly
    needed for easier writing of unit tests */
typedef sl_status_t (*mesh_app_send_fn)(mesh_crypto_key_type_t app_key_type,
                                        mesh_crypto_key_index_t app_key_index,
                                        mesh_crypto_key_index_t net_key_index,
                                        mesh_addr_t src,
                                        mesh_addr_t dst,
                                        const mesh_full_virtual_addr_t *va,
                                        mesh_mic_t szmict,
                                        mesh_model_t model,
                                        struct bgbuf_t *message,
                                        uint8_t message_flags,
                                        uint8_t ttl,
                                        mesh_send_cb cb,
                                        mesh_trans_tx_event_cb tx_cb,
                                        void *cb_param);

/**
 * event id for application information events
 */
typedef  enum {
  mesh_prov_capabilites_evt = 100,
  mesh_prov_provisioning_suspended_evt,
  mesh_prov_provisioning_start_evt,
  mesh_prov_rfu_1_evt,
  mesh_prov_rfu_2_evt,
  mesh_prov_rfu_3_evt,
  mesh_prov_rfu_4_evt,
  mesh_prov_rfu_5_evt,
} mesh_prov_event_type_t;

/**
 * reason code indicating why provisioning was suspended
 */
typedef enum {
  mesh_prov_reason_capabilities = 0,
  mesh_prov_reason_link_open = 1,
} mesh_prov_suspension_reason_t;

/** Suspend provisioning when Capabilities PDU has been received */
#define MESH_PROV_SUSPENSION_FLAG_CAPABILITIES (1 << 0)

/** Suspend provisioning when provisioning session is open */
#define MESH_PROV_SUSPENSION_FLAG_LINK_OPEN (1 << 1)

/**
 * call back to carry events
 */
typedef void (*provisioner_event_handler_cb)(mesh_prov_event_type_t event_type,
                                             const uint8_t *uuid,
                                             uint8_t len,
                                             const uint8_t *data);

/**
 * GATT Service advertisement type
 */
typedef enum {
  mesh_gatt_adv_type_provisioning_service = 0,
  mesh_gatt_adv_type_proxy_service,
  mesh_gatt_adv_rfu_1,
  mesh_gatt_adv_type_last
} mesh_gatt_adv_type_t;

#endif
