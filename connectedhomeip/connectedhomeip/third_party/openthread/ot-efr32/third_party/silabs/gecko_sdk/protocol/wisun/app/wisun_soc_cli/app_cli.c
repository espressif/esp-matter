/***************************************************************************//**
 * @file app_cli.c
 * @brief Application CLI handler
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <string.h>
#include "sl_malloc.h"
#include <assert.h>
#include <cmsis_os2.h>
#include "sl_cli.h"
#include "sl_wisun_ip6string.h"
#include "sl_sleeptimer.h"
#include "wisun_test_certificates.h"
#include "sl_slist.h"
#include "app_settings.h"
#include "sl_wisun_rf_test.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_api.h"
#include "sl_wisun_trace_api.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_version.h"

#ifdef WISUN_FAN_CERTIFICATION
#include "sl_wisun_alliance_certificates.h"
#endif

#if defined __ICCARM__
#define htonl(x) (__REV(x))
#define htons(x) (__REV16(x))
#define ntohl(x) (__REV(x))
#define ntohs(x) (__REV16(x))
#elif defined __GNUC__
#include <machine/endian.h>
#define	htonl(x) __htonl(x)
#define	htons(x) __htons(x)
#define	ntohl(x) __ntohl(x)
#define	ntohs(x) __ntohs(x)
#else
#error "Toolchain undefined"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#include "os.h"
#endif
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
#include "FreeRTOS.h"
#include "task.h"
#endif

#define APP_TASK_PRIORITY (osPriority_t)35
#define APP_TASK_STACK_SIZE 500 // in units of CPU_INT32U

#define APP_ICMPV6_TYPE_ECHO_REQUEST 128
#define APP_ICMPV6_TYPE_ECHO_RESPONSE 129
#define APP_ICMPV6_CODE_ECHO_REQUEST 0
#define APP_ICMPV6_CODE_ECHO_RESPONSE 0
#define APP_ICMPV6_PORT 0

SL_PACK_START(1)
typedef struct {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t identifier;
  uint16_t sequence_number;
  uint8_t payload[];
} SL_ATTRIBUTE_PACKED app_icmpv6_echo_request_t;
SL_PACK_END()

static const app_enum_t app_socket_event_mode[] =
{
  { "indication", 0 },
  { "polling", 1 },
  { NULL, 0 }
};

static const app_enum_t app_remote_address_enum[] =
{
  { "br", SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER },
  { "border_router", SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER },
  { "p1", SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT },
  { "parent1", SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT },
  { "p2", SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT },
  { "parent2", SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT },
  { NULL, 0 }
};

static const app_enum_t app_mac_enum[] =
{
  { "all", 0 },
  { "any", 0 },
  { NULL, 0 }
};

static const app_enum_t app_regulation_tx_level_enum[] =
{
  { "low", SL_WISUN_REGULATION_TX_LEVEL_LOW },
  { "warning", SL_WISUN_REGULATION_TX_LEVEL_WARNING },
  { "alert", SL_WISUN_REGULATION_TX_LEVEL_ALERT },
  { NULL, 0 }
};

typedef sl_status_t (*app_socket_option_handler)(sl_wisun_socket_option_data_t *option_data,
                                                 const char *option_data_str);

static sl_status_t app_socket_event_mode_handler(sl_wisun_socket_option_data_t *option_data,
                                                 const char *option_data_str);

static sl_status_t app_socket_join_multicast_group_handler(sl_wisun_socket_option_data_t *option_data,
                                                           const char *option_data_str);

static sl_status_t app_socket_leave_multicast_group_handler(sl_wisun_socket_option_data_t *option_data,
                                                            const char *option_data_str);

static sl_status_t app_get_ip_address(sl_wisun_ip_address_t *value,
                                      const char *value_str);

static const char* app_get_ip_address_str(const sl_wisun_ip_address_t *value);

void app_connect(sl_cli_command_arg_t *arguments);

typedef struct
{
  char *option;
  sl_wisun_socket_option_t option_enum;
  app_socket_option_handler handler;
} app_socket_option_t;

static const app_socket_option_t app_socket_options[] =
{
  { "event_mode", SL_WISUN_SOCKET_OPTION_EVENT_MODE, app_socket_event_mode_handler },
  { "join_multicast_group", SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP, app_socket_join_multicast_group_handler },
  { "leave_multicast_group", SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP, app_socket_leave_multicast_group_handler },
  { "send_buffer_limit", SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT, NULL },
  { NULL, (sl_wisun_socket_option_t)0, NULL }
};

typedef app_icmpv6_echo_request_t app_icmpv6_echo_response_t;

static sl_wisun_socket_id_t app_ping_socket_id = SL_WISUN_INVALID_SOCKET_ID;
static uint32_t app_ping_tick_count;

static const sl_wisun_ip_address_t APP_IN6ADDR_ANY = { 0 };

static const sl_wisun_mac_address_t APP_BROADCAST_MAC =
{
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

const char * const APP_SOCKET_TYPE_STR[] =
{
  "UDP client",
  "UDP server",
  "TCP client",
  "TCP server"
};

const char * const APP_SOCKET_STATE_STR[] =
{
  "ACTIVE",
  "LISTENING",
  "CONNECTING",
  "CLOSING"
};

typedef enum
{
  APP_CONNECTION_STATE_NOT_CONNECTED,
  APP_CONNECTION_STATE_CONNECTING,
  APP_CONNECTION_STATE_CONNECTED
} app_connection_state_t;

typedef enum {
  APP_SOCKET_TYPE_UDP_CLIENT,
  APP_SOCKET_TYPE_UDP_SERVER,
  APP_SOCKET_TYPE_TCP_CLIENT,
  APP_SOCKET_TYPE_TCP_SERVER
} app_socket_type_t;

typedef enum {
  APP_SOCKET_STATE_ACTIVE,
  APP_SOCKET_STATE_LISTENING,
  APP_SOCKET_STATE_CONNECTING,
  APP_SOCKET_STATE_CLOSING
} app_socket_state_t;

typedef struct
{
  sl_slist_node_t node;
  sl_wisun_socket_id_t socket_id;
  app_socket_type_t socket_type;
  app_socket_state_t socket_state;
  sl_wisun_ip_address_t remote_address;
  uint16_t remote_port;
} app_socket_entry_t;

#define APP_MAX_SOCKET_ENTRIES 10
static sl_slist_node_t *app_socket_entry_list_free;
static sl_slist_node_t *app_socket_entry_list;
static app_socket_entry_t app_socket_entries[APP_MAX_SOCKET_ENTRIES];
static app_connection_state_t app_connection_state;
static uint32_t app_connection_tick_count;

static app_socket_entry_t *app_socket_alloc_entry()
{
  sl_slist_node_t *item;
  app_socket_entry_t *entry = NULL;

  item = sl_slist_pop(&app_socket_entry_list_free);
  if (item) {
    entry = SL_SLIST_ENTRY(item, app_socket_entry_t, node);
    memset(entry, 0, sizeof(app_socket_entry_t));
    entry->socket_id = SL_WISUN_INVALID_SOCKET_ID;
  }

  return entry;
}

static void app_socket_free_entry(app_socket_entry_t *entry)
{
  if (!entry) {
    return;
  }

  // Remove the entry from the active entries list
  sl_slist_remove(&app_socket_entry_list, &entry->node);

  // Push the entry to the free entries list
  sl_slist_push(&app_socket_entry_list_free, &entry->node);
}

static app_socket_entry_t *app_socket_entry(sl_wisun_socket_id_t socket_id)
{
  app_socket_entry_t *entry = NULL;

  SL_SLIST_FOR_EACH_ENTRY(app_socket_entry_list, entry, app_socket_entry_t, node) {
    if (entry->socket_id == socket_id) {
      return entry;
    }
  }

  return NULL;
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

static void app_cli_task(void *argument)
{
  sl_status_t ret;
  (void)argument;

  ret = sl_wisun_set_regulation_tx_thresholds(app_settings_wisun.regulation_warning_threshold,
                                              app_settings_wisun.regulation_alert_threshold);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set regulation TX thresholds: %lu]\r\n", ret);
  }

  if (app_settings_app.autoconnect) {
    app_connect(NULL);
  }

  osThreadExit();
}

void app_cli_init(void)
{
  int i;
  osThreadId_t app_task_id;

  printf("Wi-SUN CLI Application\r\n");

  // Initialize socket entry lists
  sl_slist_init(&app_socket_entry_list_free);
  sl_slist_init(&app_socket_entry_list);

  for (i = 0; i < APP_MAX_SOCKET_ENTRIES; ++i) {
    sl_slist_push(&app_socket_entry_list_free, &app_socket_entries[i].node);
  }

  const osThreadAttr_t app_task_attribute = {
    "App Task",
    osThreadDetached,
    NULL,
    0,
    NULL,
    (APP_TASK_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8u,
    APP_TASK_PRIORITY,
    0,
    0
  };

  app_task_id = osThreadNew(app_cli_task, NULL, &app_task_attribute);
  assert(app_task_id != 0);
}

void app_about(void)
{
  printf("Wi-SUN CLI Application\r\n");
  printf("Versions:\r\n");
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
  printf("  * Compiler (GCC): %u.%u.%u\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif
#if defined(__ICCARM__) && defined(__VER__)
  printf("  * Compiler (IAR): %lu.%lu.%lu\r\n", __VER__ / 1000000UL, (__VER__ % 1000000UL) / 1000UL, __VER__ % 1000UL);
#endif
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) && defined(OS_VERSION)
  printf("  * Micrium OS kernel: %lu.%lu.%lu\r\n", OS_VERSION / 10000UL, (OS_VERSION % 10000UL) / 100UL, OS_VERSION % 100UL);
#endif
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && defined(tskKERNEL_VERSION_MAJOR) && defined(tskKERNEL_VERSION_MINOR) && defined(tskKERNEL_VERSION_BUILD)
  printf("  * FreeRTOS kernel: %u.%u.%u\r\n", tskKERNEL_VERSION_MAJOR, tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);
#endif
#if defined(SL_WISUN_VERSION_MAJOR) && defined(SL_WISUN_VERSION_MINOR) && defined(SL_WISUN_VERSION_PATCH)
  printf("  * Wi-SUN: %u.%u.%u\r\n", SL_WISUN_VERSION_MAJOR, SL_WISUN_VERSION_MINOR, SL_WISUN_VERSION_PATCH);
#endif
}

static void app_handle_network_update_ind(sl_wisun_evt_t *evt)
{
  sl_status_t ret;
  sl_wisun_ip_address_t address;

  if (evt->evt.network_update.status == SL_STATUS_OK) {
    printf("[Network update]\r\n");

    if (evt->evt.network_update.flags & (1 << SL_WISUN_NETWORK_UPDATE_FLAGS_GLOBAL_IP)) {
      ret = sl_wisun_get_ip_address(SL_WISUN_IP_ADDRESS_TYPE_GLOBAL, &address);
      if (ret == SL_STATUS_OK) {
        printf("[Global IPv6 address updated: %s]\r\n", app_get_ip_address_str(&address));
      } else {
        printf("[Global IPv6 address not available]\r\n");
      }
    }

    if (evt->evt.network_update.flags & (1 << SL_WISUN_NETWORK_UPDATE_FLAGS_PRIMARY_PARENT)) {
      ret = sl_wisun_get_ip_address(SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT, &address);
      if (ret == SL_STATUS_OK) {
        printf("[Primary parent updated: %s]\r\n", app_get_ip_address_str(&address));
      } else {
        printf("[Primary parent removed]\r\n");
      }
    }

    if (evt->evt.network_update.flags & (1 << SL_WISUN_NETWORK_UPDATE_FLAGS_SECONDARY_PARENT)) {
      ret = sl_wisun_get_ip_address(SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT, &address);
      if (ret == SL_STATUS_OK) {
        printf("[Secondary parent updated: %s]\r\n", app_get_ip_address_str(&address));
      } else {
        printf("[Secondary parent removed]\r\n");
      }
    }
  }
}

static void app_handle_connected_ind(sl_wisun_evt_t *evt)
{
  sl_status_t ret;
  uint32_t tick_count, time_ms;
  sl_wisun_ip_address_t address;

  tick_count = sl_sleeptimer_get_tick_count();
  time_ms = sl_sleeptimer_tick_to_ms(tick_count - app_connection_tick_count);

  if (evt->evt.connected.status == SL_STATUS_OK) {
    app_connection_state = APP_CONNECTION_STATE_CONNECTED;
    printf("[Connected: %lu s]\r\n", time_ms / 1000);
    ret = sl_wisun_get_ip_address(SL_WISUN_IP_ADDRESS_TYPE_GLOBAL, &address);
    if (ret == SL_STATUS_OK) {
      printf("[IPv6 address: %s]\r\n", app_get_ip_address_str(&address));
    } else {
      printf("[IPv6 address not available]\r\n");
    }
  } else {
    app_connection_state = APP_CONNECTION_STATE_NOT_CONNECTED;
    printf("[Connection failed: %lu]\r\n", evt->evt.connected.status);
  }
}

static void app_handle_ping_socket_data(sl_wisun_evt_t *evt)
{
  app_icmpv6_echo_response_t *packet;
  uint32_t tick_count, time_ms;

  packet = (app_icmpv6_echo_response_t *)evt->evt.socket_data.data;

  if (packet->type != APP_ICMPV6_TYPE_ECHO_RESPONSE) {
    printf("unexpected ICMP message type: %hu\r\n", packet->type);
    return;
  }

  tick_count = sl_sleeptimer_get_tick_count();
  time_ms = sl_sleeptimer_tick_to_ms(tick_count - app_ping_tick_count);

  printf("%u bytes from %s: icmp_seq=%u time=%lu ms\r\n",
         evt->evt.socket_data.data_length,
         app_get_ip_address_str(&evt->evt.socket_data.remote_address),
         htons(packet->sequence_number),
         time_ms);
}

static void app_handle_socket_data(sl_wisun_evt_t *evt)
{
  app_printable_data_ctx_t printable_data_ctx;
  char *printable_data;

  printf("[Data from %s (%u): %lu,%u",
         app_get_ip_address_str(&evt->evt.socket_data.remote_address),
         evt->evt.socket_data.remote_port,
         evt->evt.socket_data.socket_id, evt->evt.socket_data.data_length);
  if (app_settings_app.printable_data_length) {
    printable_data = app_util_printable_data_init(&printable_data_ctx,
                                                  evt->evt.socket_data.data,
                                                  evt->evt.socket_data.data_length,
                                                  app_settings_app.printable_data_as_hex,
                                                  app_settings_app.printable_data_length);
    while (printable_data) {
      printf("\r\n%s", printable_data);
      printable_data = app_util_printable_data_next(&printable_data_ctx);
    }
  }
  printf("]\r\n");
}

static void app_handle_socket_data_ind(sl_wisun_evt_t *evt)
{
  if (evt->evt.socket_data.socket_id == app_ping_socket_id) {
    app_handle_ping_socket_data(evt);
  } else {
    app_handle_socket_data(evt);
  }
}

static void app_handle_socket_data_available_ind(sl_wisun_evt_t *evt)
{
  printf("[Data available: %lu,%u]\r\n",
         evt->evt.socket_data_available.socket_id,
         evt->evt.socket_data_available.data_length);
}

static void app_handle_socket_connected_ind(sl_wisun_evt_t *evt)
{
  app_socket_entry_t* entry;

  entry = app_socket_entry(evt->evt.socket_connected.socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket: %lu]\r\n",
           evt->evt.socket_connected.socket_id);
    return;
  }

  if (evt->evt.socket_connected.status == SL_STATUS_OK) {
    entry->socket_state = APP_SOCKET_STATE_ACTIVE;
    printf("[Opened: %lu]\r\n", evt->evt.socket_connected.socket_id);
  } else {
    sl_wisun_close_socket(entry->socket_id);
    app_socket_free_entry(entry);
    printf("[Open failed: %lu]\r\n", evt->evt.socket_connected.status);
  }
}

static void app_handle_socket_connection_available_ind(sl_wisun_evt_t *evt)
{
  sl_status_t ret;
  app_socket_entry_t* entry;

  printf("[Socket connection available: %lu]\r\n",
         evt->evt.socket_connection_available.socket_id);

  entry = app_socket_alloc_entry();
  if (!entry) {
    printf("[Failed: unable to allocate a socket entry]\r\n");
    return;
  }

  entry->socket_id = evt->evt.socket_connection_available.socket_id;
  entry->socket_type = APP_SOCKET_TYPE_TCP_CLIENT;
  entry->socket_state = APP_SOCKET_STATE_ACTIVE;

  ret = sl_wisun_accept_on_socket(evt->evt.socket_connection_available.socket_id,
                                  &entry->socket_id,
                                  &entry->remote_address,
                                  &entry->remote_port);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to accept a connection: %lu]\r\n", ret);
    return;
  }

  // Push the entry to the active entries list
  sl_slist_push_back(&app_socket_entry_list, &entry->node);

  printf("[Accepted %s (%u): %lu]\r\n", app_get_ip_address_str(&entry->remote_address),
    entry->remote_port, entry->socket_id);
}

static void app_handle_socket_closing_ind(sl_wisun_evt_t *evt)
{
  app_socket_entry_t* entry;

  entry = app_socket_entry(evt->evt.socket_closing.socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket: %lu]\r\n",
           evt->evt.socket_closing.socket_id);
    return;
  }

  entry->socket_state = APP_SOCKET_STATE_CLOSING;

  printf("[Closing: %lu]\r\n", evt->evt.socket_closing.socket_id);
}

static void app_handle_disconnected_ind(sl_wisun_evt_t *evt)
{
  (void)evt;

  app_connection_state = APP_CONNECTION_STATE_NOT_CONNECTED;

  printf("[Disconnected]\r\n");
}

static void app_handle_connection_lost_ind(sl_wisun_evt_t *evt)
{
  (void)evt;

  app_connection_state = APP_CONNECTION_STATE_CONNECTING;

  printf("[Connection lost, connecting to \"%s\"]\r\n", app_settings_wisun.network_name);
}

static void app_handle_socket_data_sent(sl_wisun_evt_t *evt)
{
  app_socket_entry_t* entry;

  entry = app_socket_entry(evt->evt.socket_data_sent.socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket: %lu]\r\n",
           evt->evt.socket_data_sent.socket_id);
    return;
  }

  if (evt->evt.socket_data_sent.status == SL_STATUS_OK) {
    printf("[Data sent: %lu,%lu]\r\n", evt->evt.socket_data_sent.socket_id, evt->evt.socket_data_sent.socket_space_left);
  } else {
    printf("[Failed: data sent, error %lu (socket %lu)]\r\n", evt->evt.socket_data_sent.status, evt->evt.socket_data_sent.socket_id);
  }
}

static void app_handle_socket_data_sent_ind(sl_wisun_evt_t *evt)
{
  // Handle socket data sent indications only for UDP and TCP sockets.
  if (evt->evt.socket_data_sent.socket_id != app_ping_socket_id) {
    app_handle_socket_data_sent(evt);
  }
}

static void app_handle_join_state_ind(sl_wisun_evt_t *evt)
{
  const app_enum_t *ptr;

  ptr = app_util_get_enum_by_integer(app_settings_wisun_join_state_enum, evt->evt.join_state.join_state);
  if (ptr) {
    printf("[Join state: %s (%lu)]\r\n", ptr->value_str, ptr->value);
  }
}

static void app_handle_regulation_tx_level_ind(sl_wisun_evt_t *evt)
{
  const app_enum_t *ptr;

  ptr = app_util_get_enum_by_integer(app_regulation_tx_level_enum,
                                     evt->evt.regulation_tx_level.tx_level);
  if (ptr) {
    printf("[Regulation TX level: %s (%lu) (%lu ms)]\r\n", ptr->value_str, ptr->value, evt->evt.regulation_tx_level.tx_duration_ms);
  }
}

void sl_wisun_on_event(sl_wisun_evt_t *evt)
{
  app_wisun_cli_mutex_lock();

  switch (evt->header.id) {
    case SL_WISUN_MSG_NETWORK_UPDATE_IND_ID:
     app_handle_network_update_ind(evt);
     break;
    case SL_WISUN_MSG_CONNECTED_IND_ID:
      app_handle_connected_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_DATA_IND_ID:
      app_handle_socket_data_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND_ID:
      app_handle_socket_data_available_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID:
      app_handle_socket_connected_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID:
      app_handle_socket_connection_available_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_CLOSING_IND_ID:
      app_handle_socket_closing_ind(evt);
      break;
    case SL_WISUN_MSG_DISCONNECTED_IND_ID:
      app_handle_disconnected_ind(evt);
      break;
    case SL_WISUN_MSG_CONNECTION_LOST_IND_ID:
      app_handle_connection_lost_ind(evt);
      break;
    case SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID:
      app_handle_socket_data_sent_ind(evt);
      break;
    case SL_WISUN_MSG_JOIN_STATE_IND_ID:
      app_handle_join_state_ind(evt);
      break;
    case SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID:
      app_handle_regulation_tx_level_ind(evt);
      break;
    default:
      printf("[Unknown event: %d]\r\n", evt->header.id);
  }

  app_wisun_cli_mutex_unlock();
}

void app_connect(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  const uint8_t *root_certificate;
  const uint8_t *client_certificate;
  const uint8_t *client_key;
  sl_wisun_channel_mask_t channel_mask;
  (void)arguments;

  app_wisun_cli_mutex_lock();

  if (app_connection_state != APP_CONNECTION_STATE_NOT_CONNECTED) {
    printf("[Failed: already connecting or connected]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_set_network_size((sl_wisun_network_size_t)app_settings_wisun.network_size);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set network size: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_set_tx_power(app_settings_wisun.tx_power);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set TX power: %lu]\r\n", ret);
    goto cleanup;
  }

  if (app_settings_wisun.certificate_chain == APP_CERTIFICATE_CHAIN_SILABS) {
    root_certificate = WISUN_ROOT_CERTIFICATE;
    client_certificate = WISUN_CLIENT_CERTIFICATE;
    client_key = WISUN_CLIENT_KEY;
#ifdef WISUN_FAN_CERTIFICATION
  } else if (app_settings_wisun.certificate_chain == APP_CERTIFICATE_CHAIN_CERTIF) {
    root_certificate = WISUN_ALLIANCE_ROOT_CERTIFICATE;
    client_certificate = WISUN_ALLIANCE_CLIENT_CERTIFICATE;
    client_key = WISUN_ALLIANCE_CLIENT_KEY;
#endif
  } else {
    printf("[Failed: invalid certificate chain: %u]\r\n", app_settings_wisun.certificate_chain);
    goto cleanup;
  }

  ret = sl_wisun_set_trusted_certificate(SL_WISUN_CERTIFICATE_OPTION_IS_REF,
                                         strlen((const char *)root_certificate) + 1,
                                         root_certificate);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set the trusted certificate: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_set_device_certificate(SL_WISUN_CERTIFICATE_OPTION_IS_REF | SL_WISUN_CERTIFICATE_OPTION_HAS_KEY,
                                        strlen((const char *)client_certificate) + 1,
                                        client_certificate);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set the device certificate: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_set_device_private_key(SL_WISUN_PRIVATE_KEY_OPTION_IS_REF,
                                        strlen((const char *)client_key) + 1,
                                        client_key);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set the device private key: %lu]\r\n", ret);
    goto cleanup;
  }

  if (app_settings_wisun.regulatory_domain == SL_WISUN_REGULATORY_DOMAIN_APP) {
    ret = sl_wisun_set_channel_plan(app_settings_wisun.ch0_frequency,
                                    app_settings_wisun.number_of_channels,
                                    (sl_wisun_channel_spacing_t) app_settings_wisun.channel_spacing);
    if (ret != SL_STATUS_OK) {
      printf("[Failed: unable to set the channel plan: %lu]\r\n", ret);
      goto cleanup;
    }
  }

  ret = app_settings_get_channel_mask(app_settings_wisun.allowed_channels, &channel_mask);
  ret = sl_wisun_set_channel_mask(&channel_mask);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set channel mask: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_set_unicast_settings(app_settings_wisun.uc_dwell_interval_ms);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set unicast settings: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_set_regulation(app_settings_wisun.regulation);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set regional regulation: %lu]\r\n", ret);
    goto cleanup;
  }

  ret = sl_wisun_connect((const uint8_t *)app_settings_wisun.network_name,
                         (sl_wisun_regulatory_domain_t)app_settings_wisun.regulatory_domain,
                         (sl_wisun_operating_class_t)app_settings_wisun.operating_class,
                         (sl_wisun_operating_mode_t)app_settings_wisun.operating_mode);
  if (ret == SL_STATUS_OK) {
    app_connection_state = APP_CONNECTION_STATE_CONNECTING;
    app_connection_tick_count = sl_sleeptimer_get_tick_count();
    printf("[Connecting to \"%s\"]\r\n", app_settings_wisun.network_name);
  } else {
    printf("[Connection failed: %lu]\r\n", ret);
  }

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_disconnect(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;

  app_wisun_cli_mutex_lock();

  if (app_connection_state == APP_CONNECTION_STATE_NOT_CONNECTED) {
    printf("[Failed: already disconnected]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_disconnect();
  if (ret == SL_STATUS_OK) {
    app_connection_state = APP_CONNECTION_STATE_NOT_CONNECTED;
    printf("[Disconnecting]\r\n");
  } else {
    printf("[Disconnection failed: %lu]\r\n", ret);
  }

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_ping(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  sl_wisun_ip_address_t remote_address;
  char *arg_remote_address;
  //uint16_t arg_count = 1;
  app_icmpv6_echo_request_t *packet;
  uint16_t packet_data_length;
  uint8_t *packet_data = NULL;
  uint16_t payload_data_length;
  uint8_t *payload_data = NULL;
  (void)arguments;

  app_wisun_cli_mutex_lock();

  arg_remote_address = sl_cli_get_argument_string(arguments, 0);
  ret = app_get_ip_address(&remote_address, arg_remote_address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: invalid remote address parameter]\r\n");
    goto cleanup;
  }

#if 0 // not used yet
  if (sl_cli_get_argument_count(arguments) == 2) {
    arg_count = sl_cli_get_argument_uint16(arguments, 1);
  }
#endif

  if (app_ping_socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(app_ping_socket_id);
    app_ping_socket_id = SL_WISUN_INVALID_SOCKET_ID;
  }

  ret = sl_wisun_open_socket(SL_WISUN_SOCKET_PROTOCOL_ICMP, &app_ping_socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to open a socket: %lu]\r\n", ret);
    goto cleanup;
  }

  packet_data_length = app_settings_ping.packet_length;
  payload_data_length = packet_data_length - sizeof(app_icmpv6_echo_request_t);
  packet_data = sl_malloc(packet_data_length);
  if (!packet_data) {
    printf("[Failed: unable to allocate memory for an ICMP packet]\r\n");
    goto error_handler;
  }

  // ICMPv6 Echo Request header, checksum is calculated by the stack
  packet = (app_icmpv6_echo_request_t *)packet_data;
  packet->type = APP_ICMPV6_TYPE_ECHO_REQUEST;
  packet->code = APP_ICMPV6_CODE_ECHO_REQUEST;
  packet->checksum = 0;
  packet->identifier = htons(app_settings_ping.identifier);
  packet->sequence_number = htons(app_settings_ping.sequence_number);

  // ICMPv6 Echo Request payload
  payload_data = packet->payload;
  while (payload_data_length) {
    if (payload_data_length < app_settings_ping.pattern_length) {
      memcpy(payload_data, app_settings_ping.pattern, payload_data_length);
      payload_data += payload_data_length;
      payload_data_length = 0;
    } else {
      memcpy(payload_data, app_settings_ping.pattern, app_settings_ping.pattern_length);
      payload_data += app_settings_ping.pattern_length;
      payload_data_length -= app_settings_ping.pattern_length;
    }
  }

  ret = sl_wisun_sendto_on_socket(app_ping_socket_id,
                                  &remote_address,
                                  APP_ICMPV6_PORT,
                                  packet_data_length,
                                  packet_data);
  if (ret == SL_STATUS_OK) {
    app_ping_tick_count = sl_sleeptimer_get_tick_count();
    printf("PING %s: %u data bytes\r\n", app_get_ip_address_str(&remote_address), packet_data_length);
    goto cleanup;
  } else {
    printf("[Failed: unable to send an ICMP packet: %lu]\r\n", ret);
    goto error_handler;
  }

error_handler:

  if (app_ping_socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(app_ping_socket_id);
    app_ping_socket_id = SL_WISUN_INVALID_SOCKET_ID;
  }

cleanup:

  sl_free(packet_data);
  packet_data = NULL;

  app_wisun_cli_mutex_unlock();
}

void app_tcp_client(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  char *arg_remote_address;
  sl_wisun_ip_address_t remote_address;
  uint16_t remote_port;

  app_wisun_cli_mutex_lock();

  // Parameters
  arg_remote_address = sl_cli_get_argument_string(arguments, 0);
  ret = app_get_ip_address(&remote_address, arg_remote_address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: invalid remote address parameter]\r\n");
    goto cleanup;
  }

  remote_port = sl_cli_get_argument_uint16(arguments, 1);
  if (!remote_port) {
    printf("[Failed: invalid remote port parameter]\r\n");
    goto cleanup;
  }

  entry = app_socket_alloc_entry();
  if (!entry) {
    printf("[Failed: unable to allocate a socket entry]\r\n");
    goto cleanup;
  }

  entry->socket_type = APP_SOCKET_TYPE_TCP_CLIENT;
  entry->socket_state = APP_SOCKET_STATE_CONNECTING;
  entry->remote_address = remote_address;
  entry->remote_port = remote_port;

  ret = sl_wisun_open_socket(SL_WISUN_SOCKET_PROTOCOL_TCP, &entry->socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to open a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  ret = sl_wisun_connect_socket(entry->socket_id, &remote_address, remote_port);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to connect a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  // Push the entry to the active entries list
  sl_slist_push_back(&app_socket_entry_list, &entry->node);

  printf("[Opening: %s (%u): %lu]\r\n", app_get_ip_address_str(&remote_address), remote_port, entry->socket_id);
  goto cleanup;

error_handler:

  if (entry->socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(entry->socket_id);
  }
  app_socket_free_entry(entry);

cleanup:

  app_wisun_cli_mutex_unlock();

}

void app_tcp_server(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  uint16_t local_port;

  app_wisun_cli_mutex_lock();

  // Parameters
  local_port = sl_cli_get_argument_uint16(arguments, 0);
  if (!local_port) {
    printf("[Failed: invalid local port parameter]\r\n");
    goto cleanup;
  }

  entry = app_socket_alloc_entry();
  if (!entry) {
    printf("[Failed: unable to allocate a socket entry]\r\n");
    goto cleanup;
  }

  entry->socket_type = APP_SOCKET_TYPE_TCP_SERVER;
  entry->socket_state = APP_SOCKET_STATE_LISTENING;
  entry->remote_port = local_port;

  ret = sl_wisun_open_socket(SL_WISUN_SOCKET_PROTOCOL_TCP, &entry->socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to open a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  ret = sl_wisun_bind_socket(entry->socket_id, &APP_IN6ADDR_ANY, local_port);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to bind a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  ret = sl_wisun_listen_on_socket(entry->socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to listen on a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  // Push the entry to the active entries list
  sl_slist_push_back(&app_socket_entry_list, &entry->node);

  printf("[Listening: %lu]\r\n", entry->socket_id);
  goto cleanup;

error_handler:

  if (entry->socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(entry->socket_id);
  }
  app_socket_free_entry(entry);

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_udp_client(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  char *arg_remote_address;
  sl_wisun_ip_address_t remote_address;
  uint16_t remote_port;

  app_wisun_cli_mutex_lock();

  // Parameters
  arg_remote_address = sl_cli_get_argument_string(arguments, 0);
  ret = app_get_ip_address(&remote_address, arg_remote_address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: invalid remote address parameter]\r\n");
    goto cleanup;
  }

  remote_port = sl_cli_get_argument_uint16(arguments, 1);
  if (!remote_port) {
    printf("[Failed: invalid remote port parameter]\r\n");
    goto cleanup;
  }

  entry = app_socket_alloc_entry();
  if (!entry) {
    printf("[Failed: unable to allocate a socket entry]\r\n");
    goto cleanup;
  }

  entry->socket_type = APP_SOCKET_TYPE_UDP_CLIENT;
  entry->socket_state = APP_SOCKET_STATE_ACTIVE;
  entry->remote_address = remote_address;
  entry->remote_port = remote_port;

  ret = sl_wisun_open_socket(SL_WISUN_SOCKET_PROTOCOL_UDP, &entry->socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to open a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  ret = sl_wisun_connect_socket(entry->socket_id, &remote_address, remote_port);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to connect a socket: %lu]\r\n]", ret);
    goto error_handler;
  }

  // Push the entry to the active entries list
  sl_slist_push_back(&app_socket_entry_list, &entry->node);

  printf("[Opened: %lu]\r\n", entry->socket_id);
  goto cleanup;

error_handler:

  if (entry->socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(entry->socket_id);
  }
  app_socket_free_entry(entry);

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_udp_server(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  uint16_t local_port;

  app_wisun_cli_mutex_lock();

  // Parameters
  local_port = sl_cli_get_argument_uint16(arguments, 0);
  if (!local_port) {
    printf("[Failed: invalid local port parameter]\r\n");
    goto cleanup;
  }

  entry = app_socket_alloc_entry();
  if (!entry) {
    printf("[Failed: unable to allocate a socket entry]\r\n");
    goto cleanup;
  }

  entry->socket_type = APP_SOCKET_TYPE_UDP_SERVER;
  entry->socket_state = APP_SOCKET_STATE_ACTIVE;
  entry->remote_port = local_port;

  ret = sl_wisun_open_socket(SL_WISUN_SOCKET_PROTOCOL_UDP, &entry->socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to open a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  ret = sl_wisun_bind_socket(entry->socket_id, &APP_IN6ADDR_ANY, local_port);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to bind a socket: %lu]\r\n", ret);
    goto error_handler;
  }

  // Push the entry to the active entries list
  sl_slist_push_back(&app_socket_entry_list, &entry->node);

  printf("[Listening: %lu]\r\n", entry->socket_id);
  goto cleanup;

error_handler:

  if (entry->socket_id != SL_WISUN_INVALID_SOCKET_ID) {
    sl_wisun_close_socket(entry->socket_id);
  }
  app_socket_free_entry(entry);

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_close(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  sl_wisun_socket_id_t socket_id;

  app_wisun_cli_mutex_lock();

  // Parameters
  socket_id = sl_cli_get_argument_uint32(arguments, 0);

  entry = app_socket_entry(socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_close_socket(socket_id);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to close a socket: %lu]\r\n", ret);
    goto cleanup;
  }

  app_socket_free_entry(entry);

  printf("[Closed: %lu]\r\n", socket_id);

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_read(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  sl_wisun_ip_address_t remote_address;
  uint16_t remote_port;
  uint16_t data_length;
  uint8_t data[40];
  sl_wisun_socket_id_t socket_id;
  app_printable_data_ctx_t printable_data_ctx;
  char *printable_data;

  app_wisun_cli_mutex_lock();

  // Parameters
  socket_id = sl_cli_get_argument_uint32(arguments, 0);

  data_length = sl_cli_get_argument_uint16(arguments, 1);
  if (!data_length) {
    printf("[Failed: invalid amount of bytes to read]\r\n");
    goto cleanup;
  }

  entry = app_socket_entry(socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket]\r\n");
    goto cleanup;
  }

  if (entry->socket_type == APP_SOCKET_TYPE_TCP_SERVER) {
    printf("[Failed: TCP server socket is not readable]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_receive_on_socket(socket_id, &remote_address, &remote_port, &data_length, data);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to read from a socket: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[Data from %s (%u): %lu,%u",
         app_get_ip_address_str(&remote_address), remote_port,
         socket_id, data_length);
  if (app_settings_app.printable_data_length) {
    printable_data = app_util_printable_data_init(&printable_data_ctx,
                                                  data,
                                                  data_length,
                                                  app_settings_app.printable_data_as_hex,
                                                  app_settings_app.printable_data_length);
    while (printable_data) {
      printf("\r\n%s", printable_data);
      printable_data = app_util_printable_data_next(&printable_data_ctx);
    }
  }
  printf("]\r\n");

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_write(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  const char *data;
  sl_wisun_socket_id_t socket_id;

  app_wisun_cli_mutex_lock();

  // Parameters
  socket_id = sl_cli_get_argument_uint32(arguments, 0);

  data = sl_cli_get_argument_string(arguments, 1);
  if (!data || !strlen(data)) {
    printf("[Failed: invalid data to be written]\r\n");
    goto cleanup;
  }

  entry = app_socket_entry(socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket]\r\n");
    goto cleanup;
  }

  if (entry->socket_type == APP_SOCKET_TYPE_TCP_SERVER) {
    printf("[Failed: TCP server socket is not writable]\r\n");
    goto cleanup;
  }

  if (entry->socket_type == APP_SOCKET_TYPE_UDP_SERVER) {
    printf("[Failed: UDP server socket requires socket_writeto]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_send_on_socket(socket_id, strlen(data), (const uint8_t *)data);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to write to a socket: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[Wrote %u bytes]\r\n", strlen(data));

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_writeto(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  app_socket_entry_t* entry;
  char *arg_remote_address;
  sl_wisun_ip_address_t remote_address;
  uint16_t remote_port;
  const char *data;
  sl_wisun_socket_id_t socket_id;

  app_wisun_cli_mutex_lock();

  // Parameters
  socket_id = sl_cli_get_argument_uint32(arguments, 0);

  arg_remote_address = sl_cli_get_argument_string(arguments, 1);
  ret = app_get_ip_address(&remote_address, arg_remote_address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: invalid remote address parameter]\r\n");
    goto cleanup;
  }

  remote_port = sl_cli_get_argument_uint16(arguments, 2);
  if (!remote_port) {
    printf("[Failed: invalid remote port parameter]\r\n");
    goto cleanup;
  }

  data = sl_cli_get_argument_string(arguments, 3);
  if (!data || !strlen(data)) {
    printf("[Failed: invalid data to be written]\r\n");
    goto cleanup;
  }

  entry = app_socket_entry(socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket]\r\n");
    goto cleanup;
  }

  if (entry->socket_type == APP_SOCKET_TYPE_TCP_SERVER) {
    printf("[Failed: TCP server socket is not writable]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_sendto_on_socket(socket_id, &remote_address, remote_port, strlen(data), (const uint8_t *)data);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to write to a socket: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[Wrote %u bytes]\r\n", strlen(data));

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_list(sl_cli_command_arg_t *arguments)
{
  app_socket_entry_t *entry = NULL;
  char info_str[64];
  (void)arguments;

  app_wisun_cli_mutex_lock();

  printf("!  ##  Type        Info\r\n");

  SL_SLIST_FOR_EACH_ENTRY(app_socket_entry_list, entry, app_socket_entry_t, node) {
    sprintf(info_str, "%s (%u) (%s)", app_get_ip_address_str(&entry->remote_address),
      entry->remote_port, APP_SOCKET_STATE_STR[entry->socket_state]);
    printf("#  %2lu  %10s  %s\r\n", entry->socket_id, APP_SOCKET_TYPE_STR[entry->socket_type], info_str);
  }

  printf("!\r\n");

  app_wisun_cli_mutex_unlock();
}

void app_socket_set_option(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret = SL_STATUS_OK;
  app_socket_entry_t* entry;
  sl_wisun_socket_id_t socket_id;
  char *arg_option;
  char *arg_option_data;
  const app_socket_option_t *iter;
  SL_ALIGN(4) sl_wisun_socket_option_data_t option_data SL_ATTRIBUTE_ALIGN(4);

  app_wisun_cli_mutex_lock();

  // Parameters
  socket_id = sl_cli_get_argument_uint32(arguments, 0);

  arg_option = sl_cli_get_argument_string(arguments, 1);
  if (!arg_option || !strlen(arg_option)) {
    printf("[Failed: invalid option parameter]\r\n");
    goto cleanup;
  }

  arg_option_data = sl_cli_get_argument_string(arguments, 2);
  if (!arg_option || !strlen(arg_option)) {
    printf("[Failed: invalid option data parameter]\r\n");
    goto cleanup;
  }

  entry = app_socket_entry(socket_id);
  if (!entry) {
    printf("[Failed: unable to find the specified socket]\r\n");
    goto cleanup;
  }

  iter = app_socket_options;
  while (iter->option) {
    if (!strcmp(iter->option, arg_option)) {
      if (iter->handler) {
        ret = iter->handler(&option_data, arg_option_data);
        break;
      }
    }
    iter++;
  }

  if (!iter->option) {
    printf("[Failed: invalid option parameter]\r\n");
    goto cleanup;
  }

  if (ret != SL_STATUS_OK) {
    printf("[Failed: invalid option data parameter]\r\n");
    goto cleanup;
  }

  ret = sl_wisun_set_socket_option(socket_id,
                                   iter->option_enum,
                                   &option_data);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to set socket option: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[Socket option set]\r\n");

cleanup:

  app_wisun_cli_mutex_unlock();
}

static sl_status_t app_socket_event_mode_handler(sl_wisun_socket_option_data_t *option_data,
                                                 const char *option_data_str)
{
  // The caller guarantees the aligment of the option data,
  // thus the warning can be ignored.
  #if defined __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  #elif defined __ICCARM__
  #pragma diag_suppress=Pa039
  #endif

  return app_util_get_integer(&option_data->event_mode.mode, option_data_str, app_socket_event_mode, false);

  // Restore the defaults
  #ifdef __GNUC__
  #pragma GCC diagnostic pop
  #elif defined __ICCARM__
  #pragma diag_default=Pa039
  #endif
}

static sl_status_t app_socket_join_multicast_group_handler(sl_wisun_socket_option_data_t *option_data,
                                                           const char *option_data_str)
{
  // The caller guarantees the aligment of the option data,
  // thus the warning can be ignored.
  #ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  #elif defined __ICCARM__
  #pragma diag_suppress=Pa039
  #endif

  option_data->multicast_group.action = SL_WISUN_MULTICAST_GROUP_ACTION_JOIN;
  if (!stoip6(option_data_str, strlen(option_data_str), &option_data->multicast_group.address.address[0])) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;

  // Restore the defaults
  #ifdef __GNUC__
  #pragma GCC diagnostic pop
  #elif defined __ICCARM__
  #pragma diag_default=Pa039
  #endif
}

static sl_status_t app_socket_leave_multicast_group_handler(sl_wisun_socket_option_data_t *option_data,
                                                            const char *option_data_str)
{
  // The caller guarantees the aligment of the option data,
  // thus the warning can be ignored.
  #ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  #elif defined __ICCARM__
  #pragma diag_suppress=Pa039
  #endif

  option_data->multicast_group.action = SL_WISUN_MULTICAST_GROUP_ACTION_LEAVE;
  if (!stoip6(option_data_str, strlen(option_data_str), &option_data->multicast_group.address.address[0])) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;

  // Restore the defaults
  #ifdef __GNUC__
  #pragma GCC diagnostic pop
  #elif defined __ICCARM__
  #pragma diag_default=Pa039
  #endif
}

static sl_status_t app_get_ip_address(sl_wisun_ip_address_t *value,
                                      const char *value_str)
{
  const app_enum_t* value_enum;

  if (!value_str) {
    return SL_STATUS_FAIL;
  }

  value_enum = app_util_get_enum_by_string(app_remote_address_enum, value_str);
  if (value_enum) {
    // Resolve the enumerated address
    return sl_wisun_get_ip_address((sl_wisun_ip_address_type_t)value_enum->value, value);
  }

  // Convert address string to binary
  if (!stoip6(value_str, strlen(value_str), &value->address[0])) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

static const char* app_get_ip_address_str(const sl_wisun_ip_address_t *value)
{
  static char remote_address_str[40];

  ip6tos(value->address, remote_address_str);

  return remote_address_str;
}

void app_mac_allow(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  char *address_str = NULL;
  sl_wisun_mac_address_t address;
  const app_enum_t* value_enum;

  app_wisun_cli_mutex_lock();

  address_str = sl_cli_get_argument_string(arguments, 0);

  value_enum = app_util_get_enum_by_string(app_mac_enum, address_str);
  if (value_enum) {
    // Assume enumeration means a broadcast address
    address = APP_BROADCAST_MAC;
  } else {
    // Attempt to convert the MAC address string
    ret = app_util_get_mac_address(&address, address_str);
    if (ret != SL_STATUS_OK) {
      printf("[Failed: unable to parse the MAC address: %lu]\r\n", ret);
      goto cleanup;
    }
  }

  ret = sl_wisun_allow_mac_address(&address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to add the MAC address to the access list: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[MAC address added to the access list]\r\n");

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_mac_deny(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  char *address_str = NULL;
  sl_wisun_mac_address_t address;
  const app_enum_t* value_enum;

  app_wisun_cli_mutex_lock();

  address_str = sl_cli_get_argument_string(arguments, 0);

  value_enum = app_util_get_enum_by_string(app_mac_enum, address_str);
  if (value_enum) {
    // Assume enumeration means a broadcast address
    address = APP_BROADCAST_MAC;
  } else {
    // Attempt to convert the MAC address string
    ret = app_util_get_mac_address(&address, address_str);
    if (ret != SL_STATUS_OK) {
      printf("[Failed: unable to parse the MAC address: %lu]\r\n", ret);
      goto cleanup;
    }
  }

  ret = sl_wisun_deny_mac_address(&address);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: unable to add the MAC address to the access list: %lu]\r\n", ret);
    goto cleanup;
  }

  printf("[MAC address added to the access list]\r\n");

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_socket_get_option(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret = SL_STATUS_OK;
  app_socket_entry_t* entry;
  sl_wisun_socket_id_t socket_id;
  char *arg_option;
  const app_socket_option_t *iter;
  SL_ALIGN(4) sl_wisun_socket_option_data_t option_data SL_ATTRIBUTE_ALIGN(4);

  app_wisun_cli_mutex_lock();

  if (sl_cli_get_argument_count(arguments) == 0) {
    iter = app_socket_options;
    printf("Options list :\r\n");
    while (iter->option) {
      printf("%s\r\n",iter->option);
      iter++;
    }

  } else if (sl_cli_get_argument_count(arguments) != 2) {
    printf("[Failed: invalid number of arguments]\r\n");
    goto cleanup;

  } else {
    // Parameters
    socket_id = sl_cli_get_argument_uint32(arguments, 0);

    arg_option = sl_cli_get_argument_string(arguments, 1);
    if (!arg_option || !strlen(arg_option)) {
      printf("[Failed: invalid option parameter]\r\n");
      goto cleanup;
    }

    entry = app_socket_entry(socket_id);
    if (!entry) {
      printf("[Failed: unable to find the specified socket]\r\n");
      goto cleanup;
    }

    iter = app_socket_options;
    while (iter->option) {
      if (!strcmp(iter->option, arg_option)) {
          break;
      }
      iter++;
    }

    if (!iter->option) {
      printf("[Failed: invalid option parameter]\r\n");
      goto cleanup;
    }

    ret = sl_wisun_get_socket_option(socket_id,
                                    iter->option_enum,
                                    &option_data);
    if (ret != SL_STATUS_OK) {
      printf("[Failed: unable to get socket option: %lu]\r\n", ret);
      goto cleanup;
    }

    switch (iter->option_enum) {
      case SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT:
        printf("[Socket option get: %lu]\r\n", option_data.send_buffer_limit.limit);
        break;
      default:
        printf("[Failed: invalid option parameter]\r\n");
        goto cleanup;
    }
  }

cleanup:

  app_wisun_cli_mutex_unlock();
}

void app_clear_credential_cache(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;

  app_wisun_cli_mutex_lock();

  ret = sl_wisun_clear_credential_cache();
  if (ret == SL_STATUS_OK) {
    printf("[Credential cache cleared]\r\n");
  } else {
    printf("[Credential cache clear failed: %lu]\r\n", ret);
  }

  app_wisun_cli_mutex_unlock();
}

void app_reset_statistics(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;

  app_wisun_cli_mutex_lock();

  ret = sl_wisun_reset_statistics(SL_WISUN_STATISTICS_TYPE_PHY);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

  ret = sl_wisun_reset_statistics(SL_WISUN_STATISTICS_TYPE_MAC);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

  ret = sl_wisun_reset_statistics(SL_WISUN_STATISTICS_TYPE_FHSS);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

  ret = sl_wisun_reset_statistics(SL_WISUN_STATISTICS_TYPE_WISUN);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

  ret = sl_wisun_reset_statistics(SL_WISUN_STATISTICS_TYPE_NETWORK);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

cleanup:

  if (ret == SL_STATUS_OK) {
    printf("[Statistics reset]\r\n");
  } else {
    printf("[Statistics reset failed: %lu]\r\n", ret);
  }

  app_wisun_cli_mutex_unlock();
}


void app_rftest_start_stream(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;
  (void)ret;
  uint16_t channel;

  channel = sl_cli_get_argument_uint32(arguments, 0);

  ret = sl_wisun_start_stream(channel);

  printf("RF Test stream started %lu\n", ret);
}

void app_rftest_stop_stream(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;
  (void)ret;

  ret = sl_wisun_stop_stream();
  printf("RF Test stream stopped %lu\n", ret);
}

void app_rftest_start_tone(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;
  (void)ret;
  uint16_t channel;

  channel = sl_cli_get_argument_uint32(arguments, 0);

  ret = sl_wisun_start_tone(channel);

  printf("RF Test tone started %lu\n", ret);
}

void app_rftest_stop_tone(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)arguments;
  (void)ret;

  ret = sl_wisun_stop_tone();
  printf("RF Test tone stopped %lu\n", ret);
}

void app_rftest_set_tx_power(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  (void)ret;
  int16_t tx_power;

  tx_power = sl_cli_get_argument_int8(arguments, 0);

  ret = sl_wisun_set_test_tx_power(tx_power);
  printf("RF Test tx Power set to  %d\n", tx_power);
}

static const app_enum_t app_trace_level_type_enum[] =
{
  {"all",  0xFF},
  {NULL,      0}
};

static const app_enum_t app_trace_level_enum[] =
{
  {"none",    0},
  {"error",   1},
  {"err",     1},
  {"warning", 2},
  {"warn",    2},
  {"info",    3},
  {"debug",   4},
  {"dbg",     4},
  {NULL,      0}
};

static bool trace_level_add_entry(sl_wisun_trace_group_config_t *trace_config, uint8_t *group_count, uint8_t group_id, uint8_t trace_level)
{
  uint8_t i;

  for (i = 0; i < *group_count; i++) {
    if (trace_config[i].group_id == group_id) {
      // Found existing group. Update it.
      trace_config[i].trace_level = trace_level;
      return true;
    }
  }

  // group not found. Add new one.
  trace_config[i].group_id = group_id;
  trace_config[i].trace_level = trace_level;
  (*group_count)++;
  return false;
}

void app_set_trace_level(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  uint8_t i, group_count = 0;
  char *trace_config_string;
  uint32_t trace_level, group_id;
  char group_id_str[20], trace_level_str[20];
  sl_wisun_trace_group_config_t *trace_config = NULL;
  int res;
  const app_enum_t *value_enum;

  app_wisun_cli_mutex_lock();

  trace_config_string = sl_cli_get_argument_string(arguments, 0);
  trace_config = sl_malloc(SL_WISUN_TRACE_GROUP_COUNT * sizeof(sl_wisun_trace_group_config_t));
  trace_config_string = strtok(trace_config_string, ";");
  while (trace_config_string != NULL) {

    res = sscanf(trace_config_string, "%[^,],%s", group_id_str, trace_level_str);
    if (res != 2) {
      // Error parsing string
      ret = SL_STATUS_INVALID_PARAMETER;
      goto cleanup;
    }

    // Check group_id enum
    value_enum = app_util_get_enum_by_string(app_trace_level_type_enum, group_id_str);
    if (value_enum) {
      // group_id is an enum (only "all" accepted)

      // Check trace_level enum
      value_enum = app_util_get_enum_by_string(app_trace_level_enum, trace_level_str);

      if (value_enum) {
        trace_level = value_enum->value;
      } else {
        ret = app_util_get_integer(&trace_level, trace_level_str, NULL, false);
        if (ret != SL_STATUS_OK) {
          // Not a valid level value
          ret = SL_STATUS_INVALID_TYPE;
          goto cleanup;
        }
      }

      for (i = 0; i < SL_WISUN_TRACE_GROUP_COUNT; i++) {
        trace_level_add_entry(trace_config, &group_count, i, trace_level);
      }
    } else {
      // group_id is not an enum

      ret = app_util_get_integer(&group_id, group_id_str, NULL, false);
      if (ret != SL_STATUS_OK) {
        // Not a valid group value
        ret = SL_STATUS_INVALID_TYPE;
        goto cleanup;
      }

      // Check trace_level enum
      value_enum = app_util_get_enum_by_string(app_trace_level_enum, trace_level_str);

      if (value_enum) {
        trace_level = value_enum->value;
      } else {
        ret = app_util_get_integer(&trace_level, trace_level_str, NULL, false);
        if (ret != SL_STATUS_OK) {
          // Not a valid level value
          ret = SL_STATUS_INVALID_TYPE;
          goto cleanup;
        }
      }

      trace_level_add_entry(trace_config, &group_count, group_id, trace_level);
    }
    trace_config_string = strtok(NULL, ";");
  }

  ret = sl_wisun_set_trace_level(group_count, trace_config);
  if (ret != SL_STATUS_OK) {
    goto cleanup;
  }

cleanup:
  sl_free(trace_config);
  if (ret == SL_STATUS_OK) {
    printf("[Set %d trace groups]\r\n", group_count);
  } else {
    printf("[Error when setting trace level: %lu]\r\n", ret);
  }

  app_wisun_cli_mutex_unlock();
}

void app_set_unicast_tx_mode(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  uint8_t mode;

  app_wisun_cli_mutex_lock();

  mode = sl_cli_get_argument_uint8(arguments, 0);

  ret = sl_wisun_set_unicast_tx_mode(mode);

  if (ret == SL_STATUS_OK) {
    printf("[Unicast Tx mode set to %hu suceeded]\r\n", mode);
  } else {
    printf("[Failed: unable to set unicast Tx mode: %lu]\r\n", ret);
  }

  app_wisun_cli_mutex_unlock();
}
