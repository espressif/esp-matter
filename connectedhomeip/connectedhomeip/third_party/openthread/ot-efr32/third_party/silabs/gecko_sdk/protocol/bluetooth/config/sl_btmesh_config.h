#ifndef SL_BTMESH_CONFIG_H
#define SL_BTMESH_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bluetooth Mesh Stack Configuration

// <o SL_BTMESH_CONFIG_MAX_APP_BINDS> Maximum number of application bindings allowed <0-4>
// <i> Default: 4
// <i> Define the number of application bindings allowed.
#define SL_BTMESH_CONFIG_MAX_APP_BINDS       (4)

// <o SL_BTMESH_CONFIG_MAX_SUBSCRIPTIONS> Maximum number of subscriptions allowed <0-255>
// <i> Default: 4
// <i> Define the number of subscriptions allowed.
#define SL_BTMESH_CONFIG_MAX_SUBSCRIPTIONS       (4)

// <o SL_BTMESH_CONFIG_MAX_NETKEYS> Maximum number of Network Keys allowed <0-4>
// <i> Default: 4
// <i> Define the number of Network Keys the application needs.
#define SL_BTMESH_CONFIG_MAX_NETKEYS       (4)

// <o SL_BTMESH_CONFIG_MAX_APPKEYS> Maximum number of Application Keys allowed <0-4>
// <i> Default: 4
// <i> Define the number of Application Keys the application needs.
#define SL_BTMESH_CONFIG_MAX_APPKEYS       (4)

// <o SL_BTMESH_CONFIG_NET_CACHE_SIZE> Network Cache size
// <i> Default: 16
// <i> Define the Network Cache size.
#define SL_BTMESH_CONFIG_NET_CACHE_SIZE       (16)

// <o SL_BTMESH_CONFIG_RPL_SIZE> Replay Protection List size
// <i> Default: 32
// <i> Define the Replay Protection List size. Must be dividable by 16.
#define SL_BTMESH_CONFIG_RPL_SIZE       (32)

// <o SL_BTMESH_CONFIG_MAX_SEND_SEGS> Maximum number of simultaneous segmented transmissions
// <i> Default: 4
// <i> Define the maximum number simultaneous segmented transmissions allowed. Set to a low number if not much segmentation is used.
#define SL_BTMESH_CONFIG_MAX_SEND_SEGS       (4)

// <o SL_BTMESH_CONFIG_MAX_RECV_SEGS> Maximum number of simultaneous segmented receptions
// <i> Default: 4
// <i> Define the maximum number of simultaneous segmented receptions. Set to a low number if not much segmentation is used.
#define SL_BTMESH_CONFIG_MAX_RECV_SEGS       (4)

// <o SL_BTMESH_CONFIG_MAX_VAS> Maximum number of virtual addresses
// <i> Default: 4
// <i> Define the maximum number of virtual addresses the application needs. Set to 0 if virtual address not used
#define SL_BTMESH_CONFIG_MAX_VAS       (4)

// <o SL_BTMESH_CONFIG_MAX_PROV_SESSIONS> Maximum number of provisioning sessions allowed
// <i> Default: 2
// <i> Define the number of provisioning sessions the application needs.
// <i> For a node the value is 1 or 2  (depending on whether Unprovisioned Device Beaconing is initiated by 1 or 2 Bearers).
// <i> For a provisioner the value may be over 2.
#define SL_BTMESH_CONFIG_MAX_PROV_SESSIONS       (2)

// <o SL_BTMESH_CONFIG_MAX_PROV_BEARERS> Maximum number of provisioning bearers allowed
// <i> Default: 2
// <i> Define the number of provisioning bearers the application needs.
// <i> The value is 1,2 or 3.
#define SL_BTMESH_CONFIG_MAX_PROV_BEARERS (2)

// <o SL_BTMESH_CONFIG_MAX_GATT_CONNECTIONS> Number of connections to reserve for GATT Proxies <1-2>
// <i> Default: 2
// <i> Set to the number of simultaneous GATT Proxy connections the application should support.
// <i> Note that the total number of Bluetooth connections (SL_BT_CONFIG_MAX_CONNECTIONS) must include these!
#define SL_BTMESH_CONFIG_MAX_GATT_CONNECTIONS       (2)

// <o SL_BTMESH_CONFIG_GATT_TXQ_SIZE> GATT TX Queue size
// <i> Default: 4
// <i> Define the GATT TX Queue size. The value decides the number of PDUs that may be pending transmit on the GATT bearer.
#define SL_BTMESH_CONFIG_GATT_TXQ_SIZE       (4)

// <o SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICES> Maximum number of provisioned devices allowed
// <i> Default: 0
// <i> Define the number of provisioned devices the application needs. Only applicable for provisioner. Please note that provisiner reserves one entry for its own data
#define SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICES       (0)

// <o SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICE_APPKEYS> Maximum number of Application Keys allowed for each Provisioned Device
// <i> Default: 0
// <i> Define the maximum number of Application Keys allowed for each Provisioned Device. Only applicable for provisioner
#define SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICE_APPKEYS       (0)

// <o SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICE_NETKEYS> Maximum number of Network Keys allowed for each Provisioned Device
// <i> Default: 0
// <i> Define the maximum number of Network Keys allowed for each Provisioned Device. Only applicable for provisioner
#define SL_BTMESH_CONFIG_MAX_PROVISIONED_DEVICE_NETKEYS       (0)

// <o SL_BTMESH_CONFIG_MAX_FOUNDATION_CLIENT_CMDS> Maximum number of Client Commands for the Foundation Model
// <i> Default: 0
// <i> Define the maximum number of Client Commands for the Foundation Model. Only applicable for Provisioner.
#define SL_BTMESH_CONFIG_MAX_FOUNDATION_CLIENT_CMDS       (0)

// <o SL_BTMESH_CONFIG_MAX_FRIENDSHIPS> Maximum number of Friendships allowed
// <i> Default: 1
// <i> Define the number of Friendships the application needs. Only applicable for friend node.
#define SL_BTMESH_CONFIG_MAX_FRIENDSHIPS       (1)

// <o SL_BTMESH_CONFIG_FRIEND_MAX_SUBS_LIST> Maximum size of Friendship Subscription List.
// <i> Default: 5
// <i> Define the maximum size of Friendship Subscription List the application needs. Only applicable for friend node.
#define SL_BTMESH_CONFIG_FRIEND_MAX_SUBS_LIST       (5)

// <o SL_BTMESH_CONFIG_FRIEND_MAX_TOTAL_CACHE> Maximum size of Total Friend Cache
// <i> Default: 4
// <i> Define the Maximum size of Total Friend Cache. Only applicable for friend node.
#define SL_BTMESH_CONFIG_FRIEND_MAX_TOTAL_CACHE       (4)

// <o SL_BTMESH_CONFIG_FRIEND_MAX_SINGLE_CACHE> Maximum size of Cache for a single Friendship
// <i> Default: 4
// <i> Define the Maximum size of Cache for a single Friendship. Only applicable for friend node.
#define SL_BTMESH_CONFIG_FRIEND_MAX_SINGLE_CACHE       (4)

// <o SL_BTMESH_CONFIG_APP_TXQ_SIZE> Access Layer TX Queue Size
// <i> Default: 5
// <i> Define the Access Layer TX Queue size.
#define SL_BTMESH_CONFIG_APP_TXQ_SIZE       (5)

// <o SL_BTMESH_CONFIG_SEQNUM_WRITE_INTERVAL_EXP> Element sequence number write interval exponent <0-23>
// <i> Default: 16
// <i> Each network PDU originating from a device must be sent with an increasing sequence number.
// <i> To achieve this also when the device is reset or powered off, the latest sequence numbers are stored
// <i> on flash from time to time as defined by this setting. The setting defines the sequence number writing
// <i> interval as a power of two exponent. E.g., a value of 10 would mean 1024 (2 to the 10th power).
// <i> To avoid excessive flash wear, the interval should be relatively high on a device that generates
// <i> a lot of traffic, and it can be set relatively low on a device that generates little traffic.
#define SL_BTMESH_CONFIG_SEQNUM_WRITE_INTERVAL_EXP       (16)

// <o SL_BTMESH_CONFIG_ITS_KEY_CACHE_SIZE> Size of RAM cache for persistent keys stored within PSA ITS <0-544>
// <i> Default: 16
// <i> When PSA ITS (internal trusted storage) is used to store the
// Mesh encryption keys a RAM cache should be set up to increase
// runtime performance. The size of the cache should be set according
// to the expected use of application and device keys. For a node, it
// can be set to the number of application keys times two (to
// accommodate both key variants during a key refresh) ; for a
// Provisioner, it should be set to the number of application keys
// times two (to accommodate both key variants during a key refresh)
// plus a fraction of the expected number of device keys that will be
// stored. For devices that do not use PSA ITS the setting is ignored.

#define SL_BTMESH_CONFIG_ITS_KEY_CACHE_SIZE       (4)

// <o SL_BTMESH_CONFIG_MAX_PROXY_ACCESS_CONTROL_LIST_ENTRIES> Maximum number of proxy access control list entries
// <i> Default: 8
// <i> Define the number of proxy access control list entries.
#define SL_BTMESH_CONFIG_MAX_PROXY_ACCESS_CONTROL_LIST_ENTRIES  (8)

// </h> End Mesh Bluetooth Stack Configuration

#endif // SL_BTMESH_CONFIG_H
