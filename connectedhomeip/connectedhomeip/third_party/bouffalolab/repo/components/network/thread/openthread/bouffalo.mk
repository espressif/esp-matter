# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk

# src/core
otlib := src/core
otlib_module_srcdir := $(otlib)

ifeq ($(OPENTHREAD_RADIO), 1)
otlib_module_src := \
    api/diags_api.cpp\
    api/error_api.cpp\
    api/instance_api.cpp\
    api/link_raw_api.cpp\
    api/logging_api.cpp\
    api/random_noncrypto_api.cpp\
    api/tasklet_api.cpp\
    common/binary_search.cpp\
    common/binary_search.hpp\
    common/error.hpp\
    common/instance.cpp\
    common/log.cpp\
    common/random.cpp\
    common/string.cpp\
    common/tasklet.cpp\
    common/timer.cpp\
    common/uptime.cpp\
    crypto/aes_ccm.cpp\
    crypto/aes_ecb.cpp\
    crypto/crypto_platform.cpp\
    crypto/storage.cpp\
    diags/factory_diags.cpp\
    mac/link_raw.cpp\
    mac/mac_frame.cpp\
    mac/mac_types.cpp\
    mac/sub_mac.cpp\
    mac/sub_mac_callbacks.cpp\
    radio/radio.cpp\
    radio/radio_callbacks.cpp\
    radio/radio_platform.cpp\
    thread/link_quality.cpp\
    utils/parse_cmdline.cpp

otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 
otlib_module_inc :=      \
    api                  \
    common               \
    config               \
    crypto               \
    diags                \
    mac                  \
    meshcop              \
    radio                \
    thread               \
    utils

else
otlib_module_src := \
    api/backbone_router_api.cpp\
    api/backbone_router_ftd_api.cpp\
    api/border_agent_api.cpp\
    api/border_router_api.cpp\
    api/channel_manager_api.cpp\
    api/channel_monitor_api.cpp\
    api/child_supervision_api.cpp\
    api/coap_api.cpp\
    api/coap_secure_api.cpp\
    api/commissioner_api.cpp\
    api/crypto_api.cpp\
    api/dataset_api.cpp\
    api/dataset_ftd_api.cpp\
    api/dataset_updater_api.cpp\
    api/diags_api.cpp\
    api/dns_api.cpp\
    api/dns_server_api.cpp\
    api/error_api.cpp\
    api/heap_api.cpp\
    api/history_tracker_api.cpp\
    api/icmp6_api.cpp\
    api/instance_api.cpp\
    api/ip6_api.cpp\
    api/jam_detection_api.cpp\
    api/joiner_api.cpp\
    api/link_api.cpp\
    api/link_metrics_api.cpp\
    api/link_raw_api.cpp\
    api/logging_api.cpp\
    api/message_api.cpp\
    api/multi_radio_api.cpp\
    api/netdata_api.cpp\
    api/netdata_publisher_api.cpp\
    api/netdiag_api.cpp\
    api/network_time_api.cpp\
    api/ping_sender_api.cpp\
    api/random_crypto_api.cpp\
    api/random_noncrypto_api.cpp\
    api/server_api.cpp\
    api/sntp_api.cpp\
    api/srp_client_api.cpp\
    api/srp_client_buffers_api.cpp\
    api/srp_server_api.cpp\
    api/tasklet_api.cpp\
    api/tcp_api.cpp\
    api/thread_api.cpp\
    api/thread_ftd_api.cpp\
    api/trel_api.cpp\
    api/udp_api.cpp\
    backbone_router/backbone_tmf.cpp\
    backbone_router/backbone_tmf.hpp\
    backbone_router/bbr_leader.cpp\
    backbone_router/bbr_leader.hpp\
    backbone_router/bbr_local.cpp\
    backbone_router/bbr_local.hpp\
    backbone_router/bbr_manager.cpp\
    backbone_router/bbr_manager.hpp\
    backbone_router/multicast_listeners_table.cpp\
    backbone_router/multicast_listeners_table.hpp\
    backbone_router/ndproxy_table.cpp\
    backbone_router/ndproxy_table.hpp\
    border_router/infra_if.cpp\
    border_router/infra_if.hpp\
    border_router/routing_manager.cpp\
    border_router/routing_manager.hpp\
    coap/coap.cpp\
    coap/coap.hpp\
    coap/coap_message.cpp\
    coap/coap_message.hpp\
    coap/coap_secure.cpp\
    coap/coap_secure.hpp\
    common/appender.cpp\
    common/appender.hpp\
    common/arg_macros.hpp\
    common/array.hpp\
    common/as_core_type.hpp\
    common/binary_search.cpp\
    common/binary_search.hpp\
    common/bit_vector.hpp\
    common/clearable.hpp\
    common/code_utils.hpp\
    common/const_cast.hpp\
    common/crc16.cpp\
    common/crc16.hpp\
    common/data.cpp\
    common/data.hpp\
    common/debug.hpp\
    common/encoding.hpp\
    common/equatable.hpp\
    common/error.cpp\
    common/error.hpp\
    common/extension.hpp\
    common/heap.cpp\
    common/heap.hpp\
    common/heap_allocatable.hpp\
    common/heap_array.hpp\
    common/heap_data.cpp\
    common/heap_data.hpp\
    common/heap_string.cpp\
    common/heap_string.hpp\
    common/instance.cpp\
    common/instance.hpp\
    common/iterator_utils.hpp\
    common/linked_list.hpp\
    common/locator.hpp\
    common/locator_getters.hpp\
    common/log.cpp\
    common/log.hpp\
    common/logging.hpp\
    common/message.cpp\
    common/message.hpp\
    common/new.hpp\
    common/non_copyable.hpp\
    common/notifier.cpp\
    common/notifier.hpp\
    common/numeric_limits.hpp\
    common/owned_ptr.hpp\
    common/owning_list.hpp\
    common/pool.hpp\
    common/ptr_wrapper.hpp\
    common/random.cpp\
    common/random.hpp\
    common/retain_ptr.hpp\
    common/serial_number.hpp\
    common/settings.cpp\
    common/settings.hpp\
    common/settings_driver.hpp\
    common/string.cpp\
    common/string.hpp\
    common/tasklet.cpp\
    common/tasklet.hpp\
    common/time.hpp\
    common/time_ticker.cpp\
    common/time_ticker.hpp\
    common/timer.cpp\
    common/timer.hpp\
    common/tlvs.cpp\
    common/tlvs.hpp\
    common/trickle_timer.cpp\
    common/trickle_timer.hpp\
    common/type_traits.hpp\
    common/uptime.cpp\
    common/uptime.hpp\
    crypto/aes_ccm.cpp\
    crypto/aes_ccm.hpp\
    crypto/aes_ecb.cpp\
    crypto/aes_ecb.hpp\
    crypto/context_size.hpp\
    crypto/crypto_platform.cpp\
    crypto/ecdsa.cpp\
    crypto/ecdsa.hpp\
    crypto/ecdsa_tinycrypt.cpp\
    crypto/hkdf_sha256.cpp\
    crypto/hkdf_sha256.hpp\
    crypto/hmac_sha256.cpp\
    crypto/hmac_sha256.hpp\
    crypto/mbedtls.cpp\
    crypto/mbedtls.hpp\
    crypto/pbkdf2_cmac.cpp\
    crypto/pbkdf2_cmac.hpp\
    crypto/sha256.cpp\
    crypto/sha256.hpp\
    crypto/storage.cpp\
    crypto/storage.hpp\
    diags/factory_diags.cpp\
    diags/factory_diags.hpp\
    mac/channel_mask.cpp\
    mac/channel_mask.hpp\
    mac/data_poll_handler.cpp\
    mac/data_poll_handler.hpp\
    mac/data_poll_sender.cpp\
    mac/data_poll_sender.hpp\
    mac/link_raw.cpp\
    mac/link_raw.hpp\
    mac/mac.cpp\
    mac/mac.hpp\
    mac/mac_filter.cpp\
    mac/mac_filter.hpp\
    mac/mac_frame.cpp\
    mac/mac_frame.hpp\
    mac/mac_links.cpp\
    mac/mac_links.hpp\
    mac/mac_types.cpp\
    mac/mac_types.hpp\
    mac/sub_mac.cpp\
    mac/sub_mac.hpp\
    mac/sub_mac_callbacks.cpp\
    meshcop/announce_begin_client.cpp\
    meshcop/announce_begin_client.hpp\
    meshcop/border_agent.cpp\
    meshcop/border_agent.hpp\
    meshcop/commissioner.cpp\
    meshcop/commissioner.hpp\
    meshcop/dataset.cpp\
    meshcop/dataset.hpp\
    meshcop/dataset_local.cpp\
    meshcop/dataset_local.hpp\
    meshcop/dataset_manager.cpp\
    meshcop/dataset_manager.hpp\
    meshcop/dataset_manager_ftd.cpp\
    meshcop/dataset_updater.cpp\
    meshcop/dataset_updater.hpp\
    meshcop/dtls.cpp\
    meshcop/dtls.hpp\
    meshcop/energy_scan_client.cpp\
    meshcop/energy_scan_client.hpp\
    meshcop/extended_panid.cpp\
    meshcop/extended_panid.hpp\
    meshcop/joiner.cpp\
    meshcop/joiner.hpp\
    meshcop/joiner_router.cpp\
    meshcop/joiner_router.hpp\
    meshcop/meshcop.cpp\
    meshcop/meshcop.hpp\
    meshcop/meshcop_leader.cpp\
    meshcop/meshcop_leader.hpp\
    meshcop/meshcop_tlvs.cpp\
    meshcop/meshcop_tlvs.hpp\
    meshcop/network_name.cpp\
    meshcop/network_name.hpp\
    meshcop/panid_query_client.cpp\
    meshcop/panid_query_client.hpp\
    meshcop/timestamp.cpp\
    meshcop/timestamp.hpp\
    net/checksum.cpp\
    net/checksum.hpp\
    net/dhcp6.hpp\
    net/dhcp6_client.cpp\
    net/dhcp6_client.hpp\
    net/dhcp6_server.cpp\
    net/dhcp6_server.hpp\
    net/dns_client.cpp\
    net/dns_client.hpp\
    net/dns_dso.cpp\
    net/dns_dso.hpp\
    net/dns_types.cpp\
    net/dns_types.hpp\
    net/dnssd_server.cpp\
    net/dnssd_server.hpp\
    net/icmp6.cpp\
    net/icmp6.hpp\
    net/ip4_address.cpp\
    net/ip4_address.hpp\
    net/ip6.cpp\
    net/ip6.hpp\
    net/ip6_address.cpp\
    net/ip6_address.hpp\
    net/ip6_filter.cpp\
    net/ip6_filter.hpp\
    net/ip6_headers.cpp\
    net/ip6_headers.hpp\
    net/ip6_mpl.cpp\
    net/ip6_mpl.hpp\
    net/ip6_types.hpp\
    net/nd6.cpp\
    net/nd6.hpp\
    net/nd_agent.cpp\
    net/nd_agent.hpp\
    net/netif.cpp\
    net/netif.hpp\
    net/sntp_client.cpp\
    net/sntp_client.hpp\
    net/socket.cpp\
    net/socket.hpp\
    net/srp_client.cpp\
    net/srp_client.hpp\
    net/srp_server.cpp\
    net/srp_server.hpp\
    net/tcp6.cpp\
    net/tcp6.hpp\
    net/udp6.cpp\
    net/udp6.hpp\
    radio/max_power_table.hpp\
    radio/radio.cpp\
    radio/radio.hpp\
    radio/radio_callbacks.cpp\
    radio/radio_platform.cpp\
    radio/trel_interface.cpp\
    radio/trel_interface.hpp\
    radio/trel_link.cpp\
    radio/trel_link.hpp\
    radio/trel_packet.cpp\
    radio/trel_packet.hpp\
    thread/address_resolver.cpp\
    thread/address_resolver.hpp\
    thread/announce_begin_server.cpp\
    thread/announce_begin_server.hpp\
    thread/announce_sender.cpp\
    thread/announce_sender.hpp\
    thread/anycast_locator.cpp\
    thread/anycast_locator.hpp\
    thread/child_mask.hpp\
    thread/child_table.cpp\
    thread/child_table.hpp\
    thread/csl_tx_scheduler.cpp\
    thread/csl_tx_scheduler.hpp\
    thread/discover_scanner.cpp\
    thread/discover_scanner.hpp\
    thread/dua_manager.cpp\
    thread/dua_manager.hpp\
    thread/energy_scan_server.cpp\
    thread/energy_scan_server.hpp\
    thread/indirect_sender.cpp\
    thread/indirect_sender.hpp\
    thread/indirect_sender_frame_context.hpp\
    thread/key_manager.cpp\
    thread/key_manager.hpp\
    thread/link_metrics.cpp\
    thread/link_metrics.hpp\
    thread/link_metrics_tlvs.hpp\
    thread/link_quality.cpp\
    thread/link_quality.hpp\
    thread/lowpan.cpp\
    thread/lowpan.hpp\
    thread/mesh_forwarder.cpp\
    thread/mesh_forwarder.hpp\
    thread/mesh_forwarder_ftd.cpp\
    thread/mesh_forwarder_mtd.cpp\
    thread/mle.cpp\
    thread/mle.hpp\
    thread/mle_router.cpp\
    thread/mle_router.hpp\
    thread/mle_tlvs.hpp\
    thread/mle_types.cpp\
    thread/mle_types.hpp\
    thread/mlr_manager.cpp\
    thread/mlr_manager.hpp\
    thread/mlr_types.hpp\
    thread/neighbor_table.cpp\
    thread/neighbor_table.hpp\
    thread/network_data.cpp\
    thread/network_data.hpp\
    thread/network_data_leader.cpp\
    thread/network_data_leader.hpp\
    thread/network_data_leader_ftd.cpp\
    thread/network_data_leader_ftd.hpp\
    thread/network_data_local.cpp\
    thread/network_data_local.hpp\
    thread/network_data_notifier.cpp\
    thread/network_data_notifier.hpp\
    thread/network_data_publisher.cpp\
    thread/network_data_publisher.hpp\
    thread/network_data_service.cpp\
    thread/network_data_service.hpp\
    thread/network_data_tlvs.cpp\
    thread/network_data_tlvs.hpp\
    thread/network_data_types.cpp\
    thread/network_data_types.hpp\
    thread/network_diagnostic.cpp\
    thread/network_diagnostic.hpp\
    thread/network_diagnostic_tlvs.hpp\
    thread/panid_query_server.cpp\
    thread/panid_query_server.hpp\
    thread/radio_selector.cpp\
    thread/radio_selector.hpp\
    thread/router_table.cpp\
    thread/router_table.hpp\
    thread/src_match_controller.cpp\
    thread/src_match_controller.hpp\
    thread/thread_netif.cpp\
    thread/thread_netif.hpp\
    thread/thread_tlvs.hpp\
    thread/time_sync_service.cpp\
    thread/time_sync_service.hpp\
    thread/tmf.cpp\
    thread/tmf.hpp\
    thread/topology.cpp\
    thread/topology.hpp\
    thread/uri_paths.cpp\
    thread/uri_paths.hpp\
    utils/channel_manager.cpp\
    utils/channel_manager.hpp\
    utils/channel_monitor.cpp\
    utils/channel_monitor.hpp\
    utils/child_supervision.cpp\
    utils/child_supervision.hpp\
    utils/flash.cpp\
    utils/flash.hpp\
    utils/heap.cpp\
    utils/heap.hpp\
    utils/history_tracker.cpp\
    utils/history_tracker.hpp\
    utils/jam_detector.cpp\
    utils/jam_detector.hpp\
    utils/otns.cpp\
    utils/otns.hpp\
    utils/parse_cmdline.cpp\
    utils/parse_cmdline.hpp\
    utils/ping_sender.cpp\
    utils/ping_sender.hpp\
    utils/slaac_address.cpp\
    utils/slaac_address.hpp\
    utils/srp_client_buffers.cpp\
    utils/srp_client_buffers.hpp

otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 
otlib_module_inc :=      \
    api                  \
    backbone_router      \
    border_router        \
    coap                 \
    common               \
    config               \
    crypto               \
    diags                \
    mac                  \
    meshcop              \
    net                  \
    radio                \
    thread               \
    utils

endif

otlib_module_inc := $(addprefix $(otlib_module_srcdir)/,${otlib_module_inc})
#otlib_module_srcdir := $(addprefix $(otlib_module_inc)/,${otlib_module_srcdir})
otlib_module_inc := $(otlib_module_inc) $(otlib)
otlib_module_srcdir := $(otlib_module_inc)

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=
COMPONENT_SRCS := $(otlib_module_src)
COMPONENT_SRCDIRS := $(otlib_module_srcdir) 

# src/ncp 
ifdef CONFIG_NCP
otlib := src
otlib_module_srcdir := ncp
otlib_module_src := \
    changed_props_set.cpp\
    changed_props_set.hpp\
    example_vendor_hook.cpp\
    ncp_base.cpp\
    ncp_base.hpp\
    ncp_base_dispatcher.cpp\
    ncp_base_ftd.cpp\
    ncp_base_mtd.cpp\
    ncp_base_radio.cpp\
    ncp_config.h\
    ncp_hdlc.cpp\
    ncp_hdlc.hpp\
    ncp_spi.cpp\
    ncp_spi.hpp

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=
else
# src/cli 
otlib := src
otlib_module_srcdir := cli
otlib_module_src := \
    cli.cpp\
    cli.hpp\
    cli_coap.cpp\
    cli_coap.hpp\
    cli_coap_secure.cpp\
    cli_coap_secure.hpp\
    cli_commissioner.cpp\
    cli_commissioner.hpp\
    cli_config.h\
    cli_dataset.cpp\
    cli_dataset.hpp\
    cli_history.cpp\
    cli_history.hpp\
    cli_joiner.cpp\
    cli_joiner.hpp\
    cli_network_data.cpp\
    cli_network_data.hpp\
    cli_output.cpp\
    cli_output.hpp\
    cli_srp_client.cpp\
    cli_srp_client.hpp\
    cli_srp_server.cpp\
    cli_srp_server.hpp\
    cli_tcp.cpp\
    cli_tcp.hpp\
    cli_udp.cpp\
    cli_udp.hpp\
    x509_cert_key.hpp

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

endif

# src/lib/platform 
otlib := src/lib
otlib_module_srcdir := platform
otlib_module_src := exit_code.c exit_code.h
otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

# src/lib/hdlc
otlib := src/lib
otlib_module_srcdir := hdlc
otlib_module_src := hdlc.cpp hdlc.hpp
otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

# src/lib/spinel
otlib := src/lib
otlib_module_srcdir := spinel
otlib_module_src := \
    openthread-spinel-config.h\
    radio_spinel.hpp\
    radio_spinel_impl.hpp\
    spinel.c\
    spinel_buffer.cpp\
    spinel_buffer.hpp\
    spinel_decoder.cpp\
    spinel_decoder.hpp\
    spinel_encoder.cpp\
    spinel_encoder.hpp\
    spinel_platform.h

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=


# src/lib/url
otlib := src/lib
otlib_module_srcdir := url
otlib_module_src := url.cpp url.hpp
otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

# third_party/mbedtls
otlib := third_party/mbedtls
otlib_module_srcdir := $(otlib)
otlib_module_src := \
      repo/include/mbedtls/aes.h\
      repo/include/mbedtls/aesni.h\
      repo/include/mbedtls/arc4.h\
      repo/include/mbedtls/aria.h\
      repo/include/mbedtls/asn1.h\
      repo/include/mbedtls/asn1write.h\
      repo/include/mbedtls/base64.h\
      repo/include/mbedtls/bignum.h\
      repo/include/mbedtls/blowfish.h\
      repo/include/mbedtls/bn_mul.h\
      repo/include/mbedtls/camellia.h\
      repo/include/mbedtls/ccm.h\
      repo/include/mbedtls/certs.h\
      repo/include/mbedtls/chacha20.h\
      repo/include/mbedtls/chachapoly.h\
      repo/include/mbedtls/check_config.h\
      repo/include/mbedtls/cipher.h\
      repo/include/mbedtls/cipher_internal.h\
      repo/include/mbedtls/cmac.h\
      repo/include/mbedtls/compat-1.3.h\
      repo/include/mbedtls/config.h\
      repo/include/mbedtls/config_psa.h\
      repo/include/mbedtls/ctr_drbg.h\
      repo/include/mbedtls/debug.h\
      repo/include/mbedtls/des.h\
      repo/include/mbedtls/dhm.h\
      repo/include/mbedtls/ecdh.h\
      repo/include/mbedtls/ecdsa.h\
      repo/include/mbedtls/ecjpake.h\
      repo/include/mbedtls/ecp.h\
      repo/include/mbedtls/ecp_internal.h\
      repo/include/mbedtls/entropy.h\
      repo/include/mbedtls/entropy_poll.h\
      repo/include/mbedtls/error.h\
      repo/include/mbedtls/gcm.h\
      repo/include/mbedtls/havege.h\
      repo/include/mbedtls/hkdf.h\
      repo/include/mbedtls/hmac_drbg.h\
      repo/include/mbedtls/md.h\
      repo/include/mbedtls/md2.h\
      repo/include/mbedtls/md4.h\
      repo/include/mbedtls/md5.h\
      repo/include/mbedtls/md_internal.h\
      repo/include/mbedtls/memory_buffer_alloc.h\
      repo/include/mbedtls/net.h\
      repo/include/mbedtls/net_sockets.h\
      repo/include/mbedtls/nist_kw.h\
      repo/include/mbedtls/oid.h\
      repo/include/mbedtls/padlock.h\
      repo/include/mbedtls/pem.h\
      repo/include/mbedtls/pk.h\
      repo/include/mbedtls/pk_internal.h\
      repo/include/mbedtls/pkcs11.h\
      repo/include/mbedtls/pkcs12.h\
      repo/include/mbedtls/pkcs5.h\
      repo/include/mbedtls/platform.h\
      repo/include/mbedtls/platform_time.h\
      repo/include/mbedtls/platform_util.h\
      repo/include/mbedtls/poly1305.h\
      repo/include/mbedtls/ripemd160.h\
      repo/include/mbedtls/rsa.h\
      repo/include/mbedtls/rsa_internal.h\
      repo/include/mbedtls/sha1.h\
      repo/include/mbedtls/sha256.h\
      repo/include/mbedtls/sha512.h\
      repo/include/mbedtls/ssl.h\
      repo/include/mbedtls/ssl_cache.h\
      repo/include/mbedtls/ssl_ciphersuites.h\
      repo/include/mbedtls/ssl_cookie.h\
      repo/include/mbedtls/ssl_internal.h\
      repo/include/mbedtls/ssl_ticket.h\
      repo/include/mbedtls/threading.h\
      repo/include/mbedtls/timing.h\
      repo/include/mbedtls/version.h\
      repo/include/mbedtls/x509.h\
      repo/include/mbedtls/x509_crl.h\
      repo/include/mbedtls/x509_crt.h\
      repo/include/mbedtls/x509_csr.h\
      repo/include/mbedtls/xtea.h\
      repo/library/aes.c\
      repo/library/aesni.c\
      repo/library/arc4.c\
      repo/library/aria.c\
      repo/library/asn1parse.c\
      repo/library/asn1write.c\
      repo/library/base64.c\
      repo/library/bignum.c\
      repo/library/blowfish.c\
      repo/library/camellia.c\
      repo/library/ccm.c\
      repo/library/certs.c\
      repo/library/chacha20.c\
      repo/library/chachapoly.c\
      repo/library/cipher.c\
      repo/library/cipher_wrap.c\
      repo/library/cmac.c\
      repo/library/ctr_drbg.c\
      repo/library/debug.c\
      repo/library/des.c\
      repo/library/dhm.c\
      repo/library/ecdh.c\
      repo/library/ecdsa.c\
      repo/library/ecjpake.c\
      repo/library/ecp.c\
      repo/library/ecp_curves.c\
      repo/library/entropy.c\
      repo/library/entropy_poll.c\
      repo/library/error.c\
      repo/library/gcm.c\
      repo/library/havege.c\
      repo/library/hkdf.c\
      repo/library/hmac_drbg.c\
      repo/library/md.c\
      repo/library/md2.c\
      repo/library/md4.c\
      repo/library/md5.c\
      repo/library/memory_buffer_alloc.c\
      repo/library/net_sockets.c\
      repo/library/nist_kw.c\
      repo/library/oid.c\
      repo/library/padlock.c\
      repo/library/pem.c\
      repo/library/pk.c\
      repo/library/pk_wrap.c\
      repo/library/pkcs11.c\
      repo/library/pkcs12.c\
      repo/library/pkcs5.c\
      repo/library/pkparse.c\
      repo/library/pkwrite.c\
      repo/library/platform.c\
      repo/library/platform_util.c\
      repo/library/poly1305.c\
      repo/library/psa_crypto.c\
      repo/library/psa_crypto_driver_wrappers.c\
      repo/library/psa_crypto_se.c\
      repo/library/psa_crypto_slot_management.c\
      repo/library/psa_crypto_storage.c\
      repo/library/psa_its_file.c\
      repo/library/ripemd160.c\
      repo/library/rsa.c\
      repo/library/rsa_internal.c\
      repo/library/sha1.c\
      repo/library/sha256.c\
      repo/library/sha512.c\
      repo/library/ssl_cache.c\
      repo/library/ssl_ciphersuites.c\
      repo/library/ssl_cli.c\
      repo/library/ssl_cookie.c\
      repo/library/ssl_msg.c\
      repo/library/ssl_srv.c\
      repo/library/ssl_ticket.c\
      repo/library/ssl_tls.c\
      repo/library/ssl_tls13_keys.c\
      repo/library/threading.c\
      repo/library/timing.c\
      repo/library/version.c\
      repo/library/version_features.c\
      repo/library/x509.c\
      repo/library/x509_create.c\
      repo/library/x509_crl.c\
      repo/library/x509_crt.c\
      repo/library/x509_csr.c\
      repo/library/x509write_crt.c\
      repo/library/x509write_csr.c\
      repo/library/xtea.c\
      repo/library/constant_time.c


otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 
otlib_module_srcdir := $(addprefix $(otlib)/,repo/library)

otlib_module_inc := $(otlib) $(otlib)/repo/include/mbedtls $(otlib)/repo/include

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) src
COMPONENT_PRIV_INCLUDEDIRS :=


# third_party/tcplp
ifeq ($(OPENTHREAD_RADIO), 0)
otlib := third_party/tcplp
otlib_module_srcdir := $(otlib)
otlib_module_src := \
    bsdtcp/cc/cc_newreno.c                      \
    bsdtcp/tcp_input.c                          \
    bsdtcp/tcp_output.c                         \
    bsdtcp/tcp_reass.c                          \
    bsdtcp/tcp_sack.c                           \
    bsdtcp/tcp_subr.c                           \
    bsdtcp/tcp_timer.c                          \
    bsdtcp/tcp_timewait.c                       \
    bsdtcp/tcp_usrreq.c                         \
    lib/bitmap.c                                \
    lib/cbuf.c                                  \
    lib/lbuf.c


otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 
otlib_module_inc :=      \
    bdstcp/cc           \
    bdstcp/sys          \
    lib

otlib_module_inc := $(addprefix $(otlib_module_srcdir)/,${otlib_module_inc})
otlib_module_inc := $(otlib_module_inc) $(otlib)
otlib_module_srcdir := $(otlib_module_inc)

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=
COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += third_party/tcplp/bsdtcp third_party/tcplp/lib third_party/tcplp/bsdtcp/cc
endif

ifdef CONFIG_NCP
# examples/apps/ncp
otlib := examples/apps
otlib_module_srcdir := ncp
otlib_module_src := \
    ncp.c

ifeq ($(CONFIG_OT_OFFICAL), 1)
otlib_module_src := $(otlib_module_src) main.c
endif

otlib_module_srcdir := $(otlib)/${otlib_module_srcdir}
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)
COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir)
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) $(otlib_module_srcdir)/platforms/utils
COMPONENT_PRIV_INCLUDEDIRS :=
else
# examples/apps/cli
otlib := examples/apps
otlib_module_srcdir := cli
otlib_module_src := \
    cli_uart.cpp

ifeq ($(CONFIG_OT_OFFICAL), 1)
otlib_module_src := $(otlib_module_src) main.c
endif


otlib_module_srcdir := $(otlib)/${otlib_module_srcdir}
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)
COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir)
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) $(otlib_module_srcdir)/platforms/utils
COMPONENT_PRIV_INCLUDEDIRS :=
endif

# examples/platforms/utils
otlib := examples
otlib_module_srcdir := platforms/utils
otlib_module_src := \
    mac_frame.cpp

otlib_module_srcdir := $(otlib)/${otlib_module_srcdir}
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 
COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir)
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib) $(otlib)/platforms


## This component's src 
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))

