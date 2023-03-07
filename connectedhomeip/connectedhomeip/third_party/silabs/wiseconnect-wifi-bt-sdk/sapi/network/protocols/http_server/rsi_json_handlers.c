/*******************************************************************************
* @file  rsi_json_handlers.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_driver.h"
#include "rsi_json_handlers.h"

#include "rsi_utils.h"
#include <stdio.h>
#include "stdlib.h"

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Prepare the JSON object data string
 * @param[in]   json_object_string - Prepared JSON object data string
 * @param[in]   json_object - JSON object data
 * @return 		JSON object data string
 *
 *
 */

/* IMPORTANT: Keys are always enclosed in double quotes.
       Use the same keys used in the webpage.
    */

uint8_t *rsi_json_object_stringify(uint8_t *json_object_string, rsi_json_object_t *json_object)
{

  sprintf((char *)json_object_string,
          "{\"ssid\":\"%s\",\"opmode\":%d,\"conec\":%d,\"channel\":%d,\"secenable\":%s,\"sectype\":%d,\"psk\":\"%s\","
          "\"ssid1\":\"%s\",\"ch1\":%d,\"sec1\":%d,\"ssid2\":\"%s\",\"ch2\":%d,\"sec2\":%d,\"ssid3\":\"%s\",\"ch3\":%d,"
          "\"sec3\":%d,\"ssid4\":\"%s\",\"ch4\":%d,\"sec4\":%d,\"ssid5\":\"%s\",\"ch5\":%d,\"sec5\":%d,\"ssid6\":\"%"
          "s\",\"ch6\":%d,\"sec6\":%d,\"ssid7\":\"%s\",\"ch7\":%d,\"sec7\":%d,\"ssid8\":\"%s\",\"ch8\":%d,\"sec8\":%d,"
          "\"ssid9\":\"%s\",\"ch9\":%d,\"sec9\":%d,\"ssid10\":\"%s\",\"ch10\":%d,\"sec10\":%d,\"ssid11\":\"%s\","
          "\"ch11\":%d,\"sec11\":%d}",

          json_object->ssid,
          json_object->opmode,
          json_object->conec,
          json_object->channel,
          (json_object->sec_enable == 1 ? "true" : "false"),
          json_object->sec_type,
          json_object->psk,

          json_object->json_scan_res_object[0].ssid,
          json_object->json_scan_res_object[0].channel,
          json_object->json_scan_res_object[0].sec_type,
          json_object->json_scan_res_object[1].ssid,
          json_object->json_scan_res_object[1].channel,
          json_object->json_scan_res_object[1].sec_type,
          json_object->json_scan_res_object[2].ssid,
          json_object->json_scan_res_object[2].channel,
          json_object->json_scan_res_object[2].sec_type,
          json_object->json_scan_res_object[3].ssid,
          json_object->json_scan_res_object[3].channel,
          json_object->json_scan_res_object[3].sec_type,
          json_object->json_scan_res_object[4].ssid,
          json_object->json_scan_res_object[4].channel,
          json_object->json_scan_res_object[4].sec_type,
          json_object->json_scan_res_object[5].ssid,
          json_object->json_scan_res_object[5].channel,
          json_object->json_scan_res_object[5].sec_type,
          json_object->json_scan_res_object[6].ssid,
          json_object->json_scan_res_object[6].channel,
          json_object->json_scan_res_object[6].sec_type,
          json_object->json_scan_res_object[7].ssid,
          json_object->json_scan_res_object[7].channel,
          json_object->json_scan_res_object[7].sec_type,
          json_object->json_scan_res_object[8].ssid,
          json_object->json_scan_res_object[8].channel,
          json_object->json_scan_res_object[8].sec_type,
          json_object->json_scan_res_object[9].ssid,
          json_object->json_scan_res_object[9].channel,
          json_object->json_scan_res_object[9].sec_type,
          json_object->json_scan_res_object[10].ssid,
          json_object->json_scan_res_object[10].channel,
          json_object->json_scan_res_object[10].sec_type);

  return json_object_string;
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Initialize the JSON object structure with default values
 * @param[in]   json_object - JSON object data
 * @return      Void
 *
 */

void rsi_json_object_scan_list_update(rsi_json_object_t *json_object, rsi_rsp_scan_t *scan_rsp)
{
  uint8_t i          = 0;
  uint8_t scan_count = rsi_bytes4R_to_uint32((uint8_t *)scan_rsp->scan_count);

  for (i = 0; i < scan_count; i++) {
    rsi_strcpy(json_object->json_scan_res_object[i].ssid, scan_rsp->scan_info[i].ssid);
    json_object->json_scan_res_object[i].channel  = scan_rsp->scan_info[i].rf_channel;
    json_object->json_scan_res_object[i].sec_type = scan_rsp->scan_info[i].security_mode;
  }
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Initialize the JSON object structure with default values
 * @param[in]   json_object - JSON object data
 * @return      Void
 *
 */

void rsi_json_object_init(rsi_json_object_t *json_object)
{
  memset(json_object, 0, sizeof(rsi_json_object_t));
  rsi_strcpy(json_object->ssid, "");
  json_object->channel    = 0;
  json_object->opmode     = 0;
  json_object->conec      = 0;
  json_object->sec_enable = 0;
  json_object->sec_type   = 0;
  rsi_strcpy(json_object->psk, "");
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Update the sensor data structure with the updates received from the browser.
 * @param[in]   json_object - JSON object data
 * @param[in]   uint8       - JSON string
 * @param[in]   uint8       - JSON object filename
 * @return      0    - send_update Success
 *              1    - Failure
 */

uint8_t rsi_json_object_update(rsi_json_object_t *json_object, uint8_t *json, uint8_t *json_file_name)
{
  uint8_t filename[24] = "";
  uint8_t send_update  = 0;

  /* Extract the filename from the structure */
  json = rsi_json_extract_filename(json, filename);

  /* Check if it is our object */
  if (strcmp((const char *)filename, (const char *)json_file_name) == 0) {

    rsi_json_object_data_update_helper(json_object, json);

    /* If both check boxes are enabled, reset the data structure */
    if (json_object->sec_enable && json_object->checkbox_2) {
      rsi_json_object_init(json_object);
      send_update = 1;
    }
  }

  /* This boolean indicates whether or not the host needs to push some updated JSON
       data to the module. */
  return send_update;
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Helper function that performs the JSON object update received from the browser.
 * @param[in]   json_object - JSON object data
 * @param[in]   json        - JSON object data string
 * @return      Void
 */
void rsi_json_object_data_update_helper(rsi_json_object_t *json_object, uint8_t *json)
{
  /* Extract SSID value and update the struct */
  rsi_json_extract_string(json, (uint8_t *)"ssid", (uint8_t *)&json_object->ssid[0]);

  /* Extract CHANNEL NUMBER  value */
  rsi_json_extract_int(json, (uint8_t *)"channel", (uint8_t *)&(json_object->channel));

  /* Extract SECURITY ENABLE value */
  rsi_json_extract_boolean(json, (uint8_t *)"secenable", (int *)&(json_object->sec_enable));

  /* Extract SECURITY TYPE value */
  rsi_json_extract_int(json, (uint8_t *)"sectype", &(json_object->sec_type));

  /* Extract PSK value */
  rsi_json_extract_string(json, (uint8_t *)"psk", (uint8_t *)&json_object->psk[0]);

  /* Extract checkbox_2 value */
  rsi_json_extract_boolean(json, (uint8_t *)"cbox2", (int *)&(json_object->checkbox_2));

  /* Extract accelerometer int_vals */
  rsi_json_extract_int(json, (uint8_t *)"accy", (uint8_t *)&(json_object->accelerometer_y));
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Extract the filename from the JSON object string
 * @param[in]   json   - JSON object data string
 * @param[in]   buffer - Contains filename
 * @return      Filename
 *
 *
 *
 */

/* Extract filename out of the received JSON update data. */
uint8_t *rsi_json_extract_filename(uint8_t *json, uint8_t *buffer)
{
  uint8_t c;
  uint8_t i = 0;

  while (i < 24) {

    /* Get character */
    c = json[i];

    if (c != ',')
      buffer[i] = c;
    else
      break;

    ++i;
  }

  buffer[i] = '\0';

  return &json[i + 1];
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Extract the string from the JSON object string
 * @param[in]   json   - JSON object data string
 * @param[in]   key    - String key name
 * @param[in]   string - Extracted JSON string name
 * @return      Void
 *
 *
 *
 */

void rsi_json_extract_string(uint8_t *json, uint8_t *key, uint8_t *string)
{

  uint8_t *key_pos = NULL;
  uint16_t key_len = 0;
  char buffer[66]  = "";
  uint8_t c        = 0;
  uint8_t i        = 0;

  /* Get position of key */
  key_pos = (uint8_t *)strstr((const char *)json, (const char *)key);

  /* If key matches */
  if (key_pos != NULL) {

    key_len = strlen((const char *)key);
    key_pos += (key_len + 1);

    /* Extract the value */
    if (*key_pos == ':') {

      key_pos++;
      if (*key_pos == '"') {
        key_pos++;
        c = *key_pos;

        while (c != '"') {
          buffer[i] = c;
          i++;
          key_pos++;
          c = *key_pos;
        }

        buffer[i] = '\0';

        rsi_strcpy(string, buffer);
        return;
      }
    }
  }
  return;
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Extract the int values from the JSON object string
 * @param[in]   json - JSON object data string
 * @param[in]   key  - Integer value key name
 * @param[out]  val  - Contains extracted int value
 * @return      Void
 *
 *
 */

void rsi_json_extract_int(uint8_t *json, uint8_t *key, uint8_t *val)
{
  uint8_t *key_pos = NULL;
  uint16_t key_len = 0;
  int target       = 0;
  char buffer[20]  = "";
  uint8_t c        = 0;
  uint8_t i        = 0;

  /* Get position of key */
  key_pos = (uint8_t *)strstr((const char *)json, (const char *)key);

  /* If key matches */
  if (key_pos != NULL) {

    key_len = strlen((const char *)key);
    key_pos += (key_len + 1);

    /* Extract the value */
    if (*key_pos == ':') {

      key_pos++;
      c = *key_pos;

      while (rsi_is_int(c)) {
        buffer[i] = c;
        i++;
        key_pos++;
        c = *key_pos;
      }

      buffer[i] = '\0';

      target = rsi_atoi((const int8_t *)buffer);
      *val   = target;
    }
  }
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Extract the float values from the JSON object string
 * @param[in]   json - JSON object data string
 * @param[in]   key  -  Float key name
 * @param[out]  val  -  Extracted float value
 * @return      Void
 *
 *
 */

void rsi_json_extract_float(uint8_t *json, uint8_t *key, float *val)
{
  uint8_t *key_pos = NULL;
  uint16_t key_len = 0;
  float target     = 0;
  char buffer[20]  = "";
  uint8_t c        = 0;
  uint8_t i        = 0;

  /* Get position of key */
  key_pos = (uint8_t *)strstr((const char *)json, (const char *)key);

  /* If key matches */
  if (key_pos != NULL) {

    key_len = strlen((const char *)key);
    key_pos += (key_len + 1);

    /* Extract the value */
    if (*key_pos == ':') {

      key_pos++;
      c = *key_pos;

      while (rsi_is_float(c)) {
        buffer[i] = c;
        i++;
        key_pos++;
        c = *key_pos;
      }

      buffer[i] = '\0';

      target = atof(buffer);
      *val   = target;
    }
  }
}

/*==============================================*/
/**
 * @note        This API is not supported in current release
 * @brief       Extract the boolean from the JSON object string
 * @param[in]   json - JSON object data string
 * @param[in]   key  - Boolean key name
 * @param[out]  val  - Extracted float value
 * @return      Void
 *
 *
 */

void rsi_json_extract_boolean(uint8_t *json, uint8_t *key, int *val)
{
  uint8_t *key_pos = NULL;
  uint16_t key_len = 0;
  uint8_t target   = 0;
  uint8_t c        = 0;
  uint8_t i        = 0;

  /* Get position of key */
  key_pos = (uint8_t *)strstr((const char *)json, (const char *)key);

  /* If key matches */
  if (key_pos != NULL) {

    key_len = strlen((const char *)key);
    key_pos += (key_len + 1);

    /* Extract the value */
    if (*key_pos == ':') {

      key_pos++;
      c = *key_pos;

      while (c != 't' && c != 'f') {
        i++;
        key_pos++;
        c = *key_pos;
      }

      target = (c == 't');
      *val   = target;
    }
  }
}

/*==============================================*/
/**
 * @brief       Check if given input is integer or not
 * @param[in]   c - Input value
 * @return      0 - Success  \n
 *              1 - Failure
 *
 */
int rsi_is_int(uint8_t c)
{
  return (c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == '5') || (c == '6') || (c == '7')
         || (c == '8') || (c == '9') || (c == '-');
}

/*==============================================*/
/**
 * @brief       Check if given input is float or not
 * @param[in]   c - Input value
 * @return      0 - Success \n
 *              1 - Failure
 *
 *
 *
 */

int rsi_is_float(uint8_t c)
{
  return (c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == '5') || (c == '6') || (c == '7')
         || (c == '8') || (c == '9') || (c == '.') || (c == '-');
}
