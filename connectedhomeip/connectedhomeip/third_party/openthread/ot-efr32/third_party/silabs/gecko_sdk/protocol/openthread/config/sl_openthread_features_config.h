#ifndef _SL_OPENTHREAD_FEATURES_CONFIG_H
#define _SL_OPENTHREAD_FEATURES_CONFIG_H
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h> OpenThread Stack Configurations

// <h>  Thread Stack Protocol Version
// <o   OPENTHREAD_CONFIG_THREAD_VERSION>
//      <OT_THREAD_VERSION_1_1=> Thread 1.1
//      <OT_THREAD_VERSION_1_2=> Thread 1.2
//      <OT_THREAD_VERSION_1_3=> Thread 1.3
// <i>  Thread 1.2 and Thread 1.3 are compatible with Thread 1.1.
// <i>  Current Default: OT_THREAD_VERSION_1_3
#ifndef OPENTHREAD_CONFIG_THREAD_VERSION
#define OPENTHREAD_CONFIG_THREAD_VERSION OT_THREAD_VERSION_1_3
#endif
// </h>

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
// <h>  The following features require at least Thread Stack Protocol Version 1.2
// <q>  Backbone Router
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE    0
#endif
// <q> CSL Auto Synchronization using data polling
#ifndef OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE  1
#endif
// <q>  CSL (Coordinated Sampled Listening) Debug
#ifndef OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE      0
#endif
// <q>  CSL (Coordinated Sampled Listening) Receiver
#ifndef OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE   1
#endif
// <q>  DUA (Domain Unicast Address)
#ifndef OPENTHREAD_CONFIG_DUA_ENABLE
#define OPENTHREAD_CONFIG_DUA_ENABLE                1
#endif
// <q>  Link Metrics Initiator
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE 1
#endif
// <q>  Link Metrics Subject
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE 1
#endif
// <q>  Multicast Listener Registration
#ifndef OPENTHREAD_CONFIG_MLR_ENABLE
#define OPENTHREAD_CONFIG_MLR_ENABLE                1
#endif
// <q>  DNS Client (Thread 1.3)
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE         1
#endif
// <q>  DNS-SD Server (Thread 1.3)
#ifndef OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
#define OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE       0
#endif
// <q>  Service Registration Protocol (SRP) Client (Thread 1.3)
#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE         1
#endif
// <q>  Service Registration Protocol (SRP) Server (Thread 1.3)
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_ENABLE         0
#endif
// <q>  TCPlp (Low power TCP over OpenThread) (Thread 1.3)
#ifndef OPENTHREAD_CONFIG_TCP_ENABLE
#define OPENTHREAD_CONFIG_TCP_ENABLE                0
#endif
// <q> Thread over Infrastructure (Thread 1.3: NCP only)
#ifndef OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#define OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE 0
#endif
// </h>
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

// <e>  Border Agent
#ifndef OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE       0
#endif
// </e>
// <e>  Border Router
#ifndef OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE      0
#endif
// </e>
// <e>  Channel Manager
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE    0
#endif
// </e>
// <e>  Channel Monitor
#ifndef OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE    0
#endif
// </e>
// <e>  Child Supervision
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE  0
#endif
// </e>
// <e>  Commissioner
#ifndef OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE       0
#endif
// </e>
// <e>  COAP API
#ifndef OPENTHREAD_CONFIG_COAP_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_API_ENABLE           0
#endif
// </e>
// <e>  COAP Observe (RFC7641) API
#ifndef OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE   0
#endif
// </e>
// <e>  COAP Secure API
#ifndef OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE    0
#endif
// </e>
// <e>  DHCP6 Client
#ifndef OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE       0
#endif
// </e>
// <e>  DHCP6 Server
#ifndef OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
#define OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE       0
#endif
// </e>
// <e>  Diagnostic
#ifndef OPENTHREAD_CONFIG_DIAG_ENABLE
#define OPENTHREAD_CONFIG_DIAG_ENABLE               0
#endif
// </e>
// <e>  ECDSA (Elliptic Curve Digital Signature Algorithm) (Required for Matter support)
#ifndef OPENTHREAD_CONFIG_ECDSA_ENABLE
#define OPENTHREAD_CONFIG_ECDSA_ENABLE              1
#endif
// </e>
// <e>  External Heap
#ifndef OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE      1
#endif
// </e>
// <e>  IPv6 Fragmentation
#ifndef OPENTHREAD_CONFIG_IP6_FRAGMENTATION_ENABLE
#define OPENTHREAD_CONFIG_IP6_FRAGMENTATION_ENABLE  0
#endif
// </e>
// <e>  Maximum number of IPv6 unicast addresses allowed to be externally added
#ifndef OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS
#define OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS   4
#endif
// </e>
// <e>  Maximum number of IPv6 multicast addresses allowed to be externally added
#ifndef OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS
#define OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS   4
#endif
// </e>
// <e>  Jam Detection
#ifndef OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
#define OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE      0
#endif
// </e>
// <e>  Joiner
#ifndef OPENTHREAD_CONFIG_JOINER_ENABLE
#define OPENTHREAD_CONFIG_JOINER_ENABLE             0
#endif
// </e>
// <e>  Legacy Network
#ifndef OPENTHREAD_CONFIG_LEGACY_ENABLE
#define OPENTHREAD_CONFIG_LEGACY_ENABLE             0
#endif
// </e>
// <e>  Link Raw Service
#ifndef OPENTHREAD_CONFIG_LINK_RAW_ENABLE
#define OPENTHREAD_CONFIG_LINK_RAW_ENABLE           0
#endif
// </e>
// <e>  MAC Filter
#ifndef OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE         0
#endif
// </e>
// <e>  MLE Long Routes extension (experimental)
#ifndef OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
#define OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE    0
#endif
// </e>
// <e>  MultiPAN RCP
#ifndef OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
#define OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE       0
#endif
// </e>
// <e>  Multiple OpenThread Instances
#ifndef OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
#define OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE      0
#endif
// </e>
// <e>  OTNS (OpenThread Network Simulator)
#ifndef OPENTHREAD_CONFIG_OTNS_ENABLE
#define OPENTHREAD_CONFIG_OTNS_ENABLE               0
#endif
// </e>
// <e>  Ping Sender Module
#ifndef OPENTHREAD_CONFIG_PING_SENDER_ENABLE
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE        1
#endif
// </e>
// <e>  Platform UDP
#ifndef OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE       0
#endif
// </e>
// <e>  Reference Device for Thread Test Harness
#ifndef OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE   0
#endif
// </e>
// <e>  Service Entries in Thread Network Data
#ifndef OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE    0
#endif
// </e>
// <e>  RAM (volatile-only storage)
#ifndef OPENTHREAD_SETTINGS_RAM
#define OPENTHREAD_SETTINGS_RAM                     0
#endif
// </e>
// <e>  SLAAC Addresses
#ifndef OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE          1
#endif
// </e>
// <e>  SNTP Client
#ifndef OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE        0
#endif
// </e>
// <e>  TMF Network Diagnostics for MTD
#ifndef OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
#define OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE   0
#endif
// </e>
// <e>  Time Synchronization Service
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE          0
// </e>
// <e>  UDP Forward
#ifndef OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
#define OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE        0
#endif
// </e>
// <e>  Enable Mac beacon payload parsing support
#ifndef OPENTHREAD_CONFIG_MAC_BEACON_PAYLOAD_PARSING_ENABLE
#define OPENTHREAD_CONFIG_MAC_BEACON_PAYLOAD_PARSING_ENABLE      1
#endif
// </e>
// </h>
// <h>  Logging
// <o   OPENTHREAD_CONFIG_LOG_OUTPUT> LOG_OUTPUT
//      <OPENTHREAD_CONFIG_LOG_OUTPUT_NONE             => NONE
//      <OPENTHREAD_CONFIG_LOG_OUTPUT_APP              => APP
//      <OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED => PLATFORM_DEFINED
// <i>  Default: OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#ifndef OPENTHREAD_CONFIG_LOG_OUTPUT
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

// <q>  DYNAMIC_LOG_LEVEL
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE  0
#endif

// <e>  Enable Logging
#define OPENTHREAD_FULL_LOGS_ENABLE                 0
#if     OPENTHREAD_FULL_LOGS_ENABLE

// <h>  Note: Enabling higher log levels, which include logging packet details, can cause delays which may result in join failures.
// <o   OPENTHREAD_CONFIG_LOG_LEVEL> LOG_LEVEL
//      <OT_LOG_LEVEL_NONE       => NONE
//      <OT_LOG_LEVEL_CRIT       => CRIT
//      <OT_LOG_LEVEL_WARN       => WARN
//      <OT_LOG_LEVEL_NOTE       => NOTE
//      <OT_LOG_LEVEL_INFO       => INFO
//      <OT_LOG_LEVEL_DEBG       => DEBG
// <i>  Default: OT_LOG_LEVEL_DEBG
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_DEBG
#endif
// <q>  CLI
#ifndef OPENTHREAD_CONFIG_LOG_CLI
#define OPENTHREAD_CONFIG_LOG_CLI                   1
#endif
// <q>  PKT_DUMP
#ifndef OPENTHREAD_CONFIG_LOG_PKT_DUMP
#define OPENTHREAD_CONFIG_LOG_PKT_DUMP              1
#endif
// <q>  PLATFORM
#ifndef OPENTHREAD_CONFIG_LOG_PLATFORM
#define OPENTHREAD_CONFIG_LOG_PLATFORM              1
#endif
// <q>  PREPEND_LEVEL
#ifndef OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
#define OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL         1
#endif

#endif
// </h>
// </e>
// </h>

// <<< end of configuration section >>>
#endif // _SL_OPENTHREAD_FEATURES_CONFIG_H
