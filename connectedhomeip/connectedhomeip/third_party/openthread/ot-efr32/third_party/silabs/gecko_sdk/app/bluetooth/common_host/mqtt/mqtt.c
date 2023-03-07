/***************************************************************************//**
 * @file
 * @brief Synchronous MQTT client for text based message transmission.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#if defined(_WIN32)
#include <windows.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "app_log.h"
#include "mqtt.h"

// Check if libmosquitto version is at least 1.5.7
#if LIBMOSQUITTO_VERSION_NUMBER < 1005007
#warning Untested libmosquitto version!
#endif

#define QOS                    1
#define KEEPALIVE_INTERVAL_SEC 30
#define LOOP_TIMEOUT_MS        1
#ifndef MQTT_LOG_MASK
#define MQTT_LOG_MASK          MOSQ_LOG_NONE
#endif

static void mqtt_on_connect(struct mosquitto *mosq, void *obj, int rc);
static void mqtt_on_disconnect(struct mosquitto *mosq, void *obj, int rc);
static void mqtt_on_message(struct mosquitto *mosq,
                            void *obj,
                            const struct mosquitto_message *message);
static void mqtt_on_log(struct mosquitto *mosq,
                        void *obj,
                        int level,
                        const char *str);
static const char * mqtt_err2str(int rc);

static sl_status_t mqtt_add_topic(mqtt_handle_t *handle, const char *topic);
static sl_status_t mqtt_remove_topic(mqtt_handle_t *handle, const char *topic);
static sl_status_t mqtt_get_topic_by_index(mqtt_handle_t *handle,
                                           uint32_t index,
                                           char **topic);
static void mqtt_destroy_topics(mqtt_handle_t *handle);
/**************************************************************************//**
 * Initialize MQTT client.
 *****************************************************************************/
sl_status_t mqtt_init(mqtt_handle_t *handle)
{
  sl_status_t sc = SL_STATUS_OK;
  int rc = MOSQ_ERR_ERRNO; // return code if mosquitto_new() fails
  struct mosquitto *mosq;

  handle->head_topic = NULL;

  mosquitto_lib_init();

  mosq = mosquitto_new(handle->client_id, true, handle);
  if (mosq != NULL) {
    mosquitto_connect_callback_set(mosq, mqtt_on_connect);
    mosquitto_disconnect_callback_set(mosq, mqtt_on_disconnect);
    mosquitto_message_callback_set(mosq, mqtt_on_message);
    mosquitto_log_callback_set(mosq, mqtt_on_log);

    rc = mosquitto_connect(mosq,
                           handle->host,
                           handle->port,
                           KEEPALIVE_INTERVAL_SEC);
  }

  if (rc != MOSQ_ERR_SUCCESS) {
    app_log_error("MQTT init failed: '%s'" APP_LOG_NL, mqtt_err2str(rc));
    sc = SL_STATUS_INITIALIZATION;
    handle->client = NULL;
    if (mosq != NULL) {
      mosquitto_destroy(mosq);
    }
  } else {
    handle->client = mosq;
  }
  if (sc == SL_STATUS_OK) {
    app_log_info("MQTT init client: %s" APP_LOG_NL, handle->client_id);
  }

  return sc;
}

/**************************************************************************//**
 * Publish an MQTT message on a topic.
 *****************************************************************************/
sl_status_t mqtt_publish(mqtt_handle_t *handle,
                         const char *topic,
                         const char *payload,
                         bool retain)
{
  sl_status_t sc = SL_STATUS_OK;

  if (handle->client != NULL) {
    int rc;
    int mid;
    rc = mosquitto_publish(handle->client,
                           &mid,
                           topic,
                           strlen(payload),
                           payload,
                           QOS,
                           retain);
    if (rc != MOSQ_ERR_SUCCESS) {
      app_log_error("MQTT publish attempt failed: '%s'" APP_LOG_NL,
                    mqtt_err2str(rc));
      sc = SL_STATUS_FAIL;
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }
  return sc;
}

/**************************************************************************//**
 * MQTT step processes.
 *****************************************************************************/
sl_status_t mqtt_step(mqtt_handle_t *handle)
{
  sl_status_t sc = SL_STATUS_OK;

  if (handle->client != NULL) {
    int rc;
    rc = mosquitto_loop(handle->client, LOOP_TIMEOUT_MS, 1);
    if (rc != MOSQ_ERR_SUCCESS) {
      app_log_error("MQTT loop failed: '%s'" APP_LOG_NL, mqtt_err2str(rc));
      sc = SL_STATUS_FAIL;
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }
  return sc;
}

/**************************************************************************//**
 * Subscribe to an MQTT topic.
 *****************************************************************************/
sl_status_t mqtt_subscribe(mqtt_handle_t *handle, const char *topic)
{
  sl_status_t sc = SL_STATUS_OK;

  if (handle->client != NULL) {
    // Try to subscribe to topic.
    int rc;
    rc = mosquitto_subscribe(handle->client, NULL, topic, QOS);

    if ((rc != MOSQ_ERR_SUCCESS) && (rc != MOSQ_ERR_NO_CONN)) {
      app_log_error("MQTT subscribe attempt failed to topic '%s': '%s'" APP_LOG_NL,
                    topic,
                    mqtt_err2str(rc));
      sc = SL_STATUS_FAIL;
    }

    // Add topic to topic list.
    sc = mqtt_add_topic(handle, topic);
    if (SL_STATUS_OK != sc) {
      app_log_error("MQTT failed to append topic to topic list." APP_LOG_NL);
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }
  if (sc == SL_STATUS_OK) {
    app_log_info("MQTT subscribe: %s" APP_LOG_NL, topic);
  }

  return sc;
}

/**************************************************************************//**
 * Unsubscribe from an MQTT topic.
 *****************************************************************************/
sl_status_t mqtt_unsubscribe(mqtt_handle_t *handle, const char *topic)
{
  sl_status_t sc = SL_STATUS_OK;

  if (handle->client != NULL) {
    // Try to unsubscribe from topic.
    int rc;
    rc = mosquitto_unsubscribe(handle->client, NULL, topic);

    if ((rc != MOSQ_ERR_SUCCESS) && (rc != MOSQ_ERR_NO_CONN)) {
      app_log_info("MQTT unsubscribe attempt failed from topic '%s': '%s'" APP_LOG_NL,
                   topic,
                   mqtt_err2str(rc));
      sc = SL_STATUS_FAIL;
    }

    // Remove topic from topic list.
    sc = mqtt_remove_topic(handle, topic);
    if (SL_STATUS_OK != sc) {
      app_log_info("MQTT failed to remove topic from topic list." APP_LOG_NL);
      sc = SL_STATUS_FAIL;
    }
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }
  if (sc == SL_STATUS_OK) {
    app_log_info("MQTT unsubscribe: %s" APP_LOG_NL, topic);
  }

  return sc;
}

/**************************************************************************//**
 * Unsubscribe from all MQTT topic.
 *****************************************************************************/
sl_status_t mqtt_unsubscribe_all(mqtt_handle_t *handle)
{
  sl_status_t sc = SL_STATUS_OK;
  char *topic;
  uint8_t i = 0;

  if (handle->client != NULL) {
    // Try to subscribe to topic.
    int rc;
    while (SL_STATUS_OK == mqtt_get_topic_by_index(handle, i, &topic)) {
      rc = mosquitto_unsubscribe(handle->client, NULL, topic);

      if ((rc != MOSQ_ERR_SUCCESS) && (rc != MOSQ_ERR_NO_CONN)) {
        app_log_info("MQTT unsubscribe attempt failed from topic '%s': '%s'" APP_LOG_NL,
                     topic,
                     mqtt_err2str(rc));
        sc = SL_STATUS_FAIL;
      }
      i++;
    }

    // Remove topics from topic list.
    mqtt_destroy_topics(handle);
  } else {
    sc = SL_STATUS_NOT_INITIALIZED;
  }

  return sc;
}

/**************************************************************************//**
 * Deinitialize MQTT client.
 *****************************************************************************/
sl_status_t mqtt_deinit(mqtt_handle_t *handle)
{
  if (handle->client != NULL) {
    int rc;
    rc = mosquitto_disconnect(handle->client);

    if (rc != MOSQ_ERR_SUCCESS) {
      app_log_error("MQTT failed to disconnect: '%s', continue deinit." APP_LOG_NL,
                    mqtt_err2str(rc));
    }

    mosquitto_destroy(handle->client);
    mosquitto_lib_cleanup();
    mqtt_destroy_topics(handle);
  }

  return SL_STATUS_OK;
}

static void mqtt_on_connect(struct mosquitto *mosq, void *obj, int rc)
{
  mqtt_handle_t *handle = (mqtt_handle_t *)obj;
  uint32_t i = 0;
  char *topic;
  int ret = MOSQ_ERR_SUCCESS;

  app_log_info("MQTT connect status '%s'" APP_LOG_NL, mosquitto_connack_string(rc));

  if (rc == 0) {
    if (handle->on_connect != NULL) {
      handle->on_connect(handle);
    }
    while (SL_STATUS_OK == mqtt_get_topic_by_index(handle, i, &topic)) {
      ret = mosquitto_subscribe(mosq, NULL, topic, QOS);
      if (ret != MOSQ_ERR_SUCCESS) {
        app_log_error("MQTT subscribe attempt failed to topic '%s': '%s'" APP_LOG_NL,
                      topic,
                      mqtt_err2str(ret));
      }
      i++;
    }
  }
}

static void mqtt_on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
  app_log_level(rc == 0 ? APP_LOG_LEVEL_DEBUG : APP_LOG_LEVEL_WARNING,
                "MQTT disconnected with reason '%d'" APP_LOG_NL, rc);

  if (rc != 0) {
    int ret;
    ret = mosquitto_reconnect(mosq);
    app_log_level(((ret == MOSQ_ERR_SUCCESS) ? APP_LOG_LEVEL_INFO : APP_LOG_LEVEL_ERROR),
                  "MQTT reconnection attempt with status '%s'" APP_LOG_NL,
                  mqtt_err2str(ret));
  }
}

static void mqtt_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
  mqtt_handle_t *handle = (mqtt_handle_t *)obj;
  char *payload;

  if (handle->on_message != NULL) {
    payload = malloc(message->payloadlen + 1);
    if (NULL == payload) {
      app_log_error("MQTT failed to allocate payload buffer." APP_LOG_NL);
    } else {
      memcpy(payload, message->payload, message->payloadlen);
      // Make sure that payload is NULL terminated.
      payload[message->payloadlen] = 0;

      handle->on_message(handle, message->topic, payload);

      free(payload);
    }
  }
}

static void mqtt_on_log(struct mosquitto *mosq, void *obj, int level, const char *str)
{
  if (level & MQTT_LOG_MASK) {
    app_log("MQTT log (%d): %s" APP_LOG_NL, level, str);
  }
}

static sl_status_t mqtt_add_topic(mqtt_handle_t *handle, const char *topic)
{
  size_t topic_size;

  mqtt_topic_node_t *new = (mqtt_topic_node_t *)malloc(sizeof(mqtt_topic_node_t));
  if (NULL == new) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  new->next = handle->head_topic;
  handle->head_topic = new;

  topic_size = strlen(topic) + 1;
  new->topic = (char *)malloc(sizeof(char) * topic_size);
  memset(new->topic, 0, sizeof(char) * topic_size);
  if (new->topic == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  strcpy(new->topic, topic);
  new->topic_size = topic_size;

  return SL_STATUS_OK;
}

static sl_status_t mqtt_remove_topic(mqtt_handle_t *handle, const char *topic)
{
  mqtt_topic_node_t *current = handle->head_topic;
  mqtt_topic_node_t *previous = NULL;

  if (NULL == handle->head_topic) {
    return SL_STATUS_NOT_FOUND;
  }

  while (current != NULL) {
    if (strncmp(current->topic, topic, strlen(current->topic)) == 0) {
      if (previous != NULL) {
        previous->next = current->next;
      } else {
        handle->head_topic = current->next;
      }
      free(current->topic);
      free(current);
      return SL_STATUS_OK;
    }
    previous = current;
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

static void mqtt_destroy_topics(mqtt_handle_t *handle)
{
  mqtt_topic_node_t *current;
  mqtt_topic_node_t *next;

  for (current = handle->head_topic; current != NULL; current = next) {
    next = current->next;
    free(current->topic);
    free(current);
  }

  handle->head_topic = NULL;
}

static sl_status_t mqtt_get_topic_by_index(mqtt_handle_t *handle, uint32_t index, char **topic)
{
  uint32_t i = 0;
  mqtt_topic_node_t *current = handle->head_topic;

  if (NULL == handle->head_topic) {
    return SL_STATUS_NOT_FOUND;
  }

  while (current != NULL) {
    if (i == index) {
      *topic = current->topic;
      return SL_STATUS_OK;
    }
    current = current->next;
    i++;
  }

  return SL_STATUS_NOT_FOUND;
}

#if defined(_WIN32)
static const char * mqtt_err2str(int rc)
{
  char *ret = NULL;
  static char err_str[256];

  if (MOSQ_ERR_ERRNO == rc) {
    // Make sure to have a default output if FormatMessage fails
    // or if error code is not available in errno.
    strncpy(err_str, "Unknown system error", sizeof(err_str));
    if (errno != 0) {
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, // dwFlags
                    NULL,                // lpSource
                    errno,               // dwMessageId
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                    err_str,             // lpBuffer
                    sizeof(err_str),     // nSize
                    NULL);               // Arguments
    }
    // Make sure that err_str is NULL terminated.
    err_str[sizeof(err_str) - 1] = 0;
    ret = err_str;
  } else {
    ret = (char *)mosquitto_strerror(rc);
  }
  return ret;
}
#else
static const char * mqtt_err2str(int rc)
{
  return (MOSQ_ERR_ERRNO == rc) ? strerror(errno) : mosquitto_strerror(rc);
}
#endif // _WIN32
