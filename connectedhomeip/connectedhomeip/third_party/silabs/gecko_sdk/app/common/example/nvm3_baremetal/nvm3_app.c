/***************************************************************************//**
 * @file
 * @brief NVM3 examples functions
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
#include <stdio.h>
#include <string.h>

#include "nvm3_app.h"
#include "nvm3_default.h"
#include "nvm3_default_config.h"
#include "sl_cli.h"
#include "sl_cli_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Maximum number of data objects saved
#define MAX_OBJECT_COUNT    10

// Max and min keys for data objects
#define MIN_DATA_KEY  NVM3_KEY_MIN
#define MAX_DATA_KEY  (MIN_DATA_KEY + MAX_OBJECT_COUNT - 1)

// Key of write counter object
#define WRITE_COUNTER_KEY   MAX_OBJECT_COUNT

// Key of delete counter object
#define DELETE_COUNTER_KEY   (WRITE_COUNTER_KEY + 1)

// Use the default nvm3 handle from nvm3_default.h
#define NVM3_DEFAULT_HANDLE nvm3_defaultHandle

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Buffer for reading from NVM3
static char buffer[NVM3_DEFAULT_MAX_OBJECT_SIZE];

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static void initialise_counters(void)
{
  uint32_t type;
  size_t len;
  Ecode_t err;

  // check if the designated keys contain counters, and initialise if needed.
  err = nvm3_getObjectInfo(NVM3_DEFAULT_HANDLE, WRITE_COUNTER_KEY, &type, &len);
  if ((err != ECODE_NVM3_OK) || (type != NVM3_OBJECTTYPE_COUNTER)) {
    nvm3_writeCounter(NVM3_DEFAULT_HANDLE, WRITE_COUNTER_KEY, 0);
  }

  err = nvm3_getObjectInfo(NVM3_DEFAULT_HANDLE, DELETE_COUNTER_KEY, &type, &len);
  if ((err != ECODE_NVM3_OK) || (type != NVM3_OBJECTTYPE_COUNTER)) {
    nvm3_writeCounter(NVM3_DEFAULT_HANDLE, DELETE_COUNTER_KEY, 0);
  }
}
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Write data to NVM3
 *
 * This function implements the CLI command 'write' (see nvm3_app.slcp)
 * It stores string data at the selected NVM3 key.
 ******************************************************************************/
void nvm3_app_write(sl_cli_command_arg_t *arguments)
{
  uint32_t key;
  size_t len;

  // Get the key and data from CLI
  key = sl_cli_get_argument_uint32(arguments, 0);
  char *data = sl_cli_get_argument_string(arguments, 1);

  if (key > MAX_DATA_KEY) {
    printf("Invalid key\r\n");
    return;
  }

  len = strlen(data);

  if (len > NVM3_DEFAULT_MAX_OBJECT_SIZE) {
    printf("Maximum object size exceeded\r\n");
    return;
  }

  if (ECODE_NVM3_OK == nvm3_writeData(NVM3_DEFAULT_HANDLE,
                                      key,
                                      (unsigned char *)data,
                                      len)) {
    printf("Stored data at key %lu\r\n", key);
    // Track number of writes in counter object
    nvm3_incrementCounter(NVM3_DEFAULT_HANDLE, WRITE_COUNTER_KEY, NULL);
  } else {
    printf("Error storing data\r\n");
  }
}

/***************************************************************************//**
 * Delete data in NVM3.
 *
 * This function implements the CLI command 'delete' (see nvm3_app.slcp)
 * It deletes the data object stored at the selected NVM3 key.
 ******************************************************************************/
void nvm3_app_delete(sl_cli_command_arg_t *arguments)
{
  // Get the key from CLI
  uint32_t key = sl_cli_get_argument_uint32(arguments, 0);

  if (key > MAX_DATA_KEY) {
    printf("Invalid key\r\n");
    return;
  }

  if (ECODE_NVM3_OK == nvm3_deleteObject(NVM3_DEFAULT_HANDLE, key)) {
    printf("Deleted data at key %lu\r\n", key);
    // Track number or deletes in counter object
    nvm3_incrementCounter(NVM3_DEFAULT_HANDLE, DELETE_COUNTER_KEY, NULL);
  } else {
    printf("Error deleting key\r\n");
  }
}

/***************************************************************************//**
 * Read data from NVM3.
 *
 * This function implements the CLI command 'read' (see nvm3_app.slcp)
 * It reads the data object stored at the selected NVM3 key.
 ******************************************************************************/
void nvm3_app_read(sl_cli_command_arg_t *arguments)
{
  uint32_t type;
  size_t len;
  Ecode_t err;

  // Get the key from CLI
  uint32_t key = sl_cli_get_argument_uint32(arguments, 0);

  if (key > MAX_DATA_KEY) {
    printf("Invalid key\r\n");
    return;
  }

  err = nvm3_getObjectInfo(NVM3_DEFAULT_HANDLE, key, &type, &len);
  if (err != NVM3_OBJECTTYPE_DATA || type != NVM3_OBJECTTYPE_DATA) {
    printf("Key does not contain data object\r\n");
    return;
  }

  err = nvm3_readData(NVM3_DEFAULT_HANDLE, key, buffer, len);
  if (ECODE_NVM3_OK == err) {
    buffer[len] = '\0';
    printf("Read data from key %lu:\r\n", key);
    printf("%s\r\n", buffer);
  } else {
    printf("Error reading data from key %lu\r\n", key);
  }
}

/***************************************************************************//**
 * Display NVM3 data.
 *
 * This function implements the CLI command 'display' (see nvm3_app.slcp)
 * It displays:
 *     - keys of deleted objects
 *     - keys and contents of saved objects
 *     - number of objects deleted since last display
 *     - number of objects written since last display
 ******************************************************************************/
void nvm3_app_display(sl_cli_command_arg_t *arguments)
{
  nvm3_ObjectKey_t keys[MAX_OBJECT_COUNT];
  size_t len, objects_count;
  uint32_t type;
  Ecode_t err;
  uint32_t counter = 0;
  size_t i;

  (void)&arguments;

  objects_count = nvm3_enumDeletedObjects(NVM3_DEFAULT_HANDLE,
                                          (uint32_t *)keys,
                                          sizeof(keys) / sizeof(keys[0]),
                                          MIN_DATA_KEY,
                                          MAX_DATA_KEY);
  if (objects_count == 0) {
    printf("No deleted objects found\r\n");
  } else {
    printf("Keys of objects deleted from NVM3:\r\n");
    for (i = 0; i < objects_count; i++) {
      printf("> %lu\r\n", keys[i]);
    }
  }

  // Retrieve the keys of stored data
  objects_count = nvm3_enumObjects(NVM3_DEFAULT_HANDLE,
                                   (uint32_t *)keys,
                                   sizeof(keys) / sizeof(keys[0]),
                                   MIN_DATA_KEY,
                                   MAX_DATA_KEY);

  if (objects_count == 0) {
    printf("No stored objects found\r\n");
  } else {
    printf("Keys and contents of objects stored in NVM3:\r\n");
    for (i = 0; i < objects_count; i++) {
      nvm3_getObjectInfo(NVM3_DEFAULT_HANDLE, keys[i], &type, &len);
      if (type == NVM3_OBJECTTYPE_DATA) {
        err = nvm3_readData(NVM3_DEFAULT_HANDLE, keys[i], buffer, len);
        EFM_ASSERT(ECODE_NVM3_OK == err);
        buffer[len] = '\0';
        printf("> %lu: %s\r\n", keys[i], buffer);
      }
    }
  }

  // Display and reset counters
  err = nvm3_readCounter(NVM3_DEFAULT_HANDLE, DELETE_COUNTER_KEY, &counter);
  if (ECODE_NVM3_OK == err) {
    printf("%lu objects have been deleted since last display\r\n", counter);
  }
  nvm3_writeCounter(NVM3_DEFAULT_HANDLE, DELETE_COUNTER_KEY, 0);
  err = nvm3_readCounter(NVM3_DEFAULT_HANDLE, WRITE_COUNTER_KEY, &counter);
  if (ECODE_NVM3_OK == err) {
    printf("%lu objects have been written since last display\r\n", counter);
  }
  nvm3_writeCounter(NVM3_DEFAULT_HANDLE, WRITE_COUNTER_KEY, 0);
}

/***************************************************************************//**
 * Delete all data in NVM3.
 *
 * This function implements the CLI command 'reset' (see nvm3_app.slcp)
 * It deletes all data stored in NVM3.s
 ******************************************************************************/
void nvm3_app_reset(sl_cli_command_arg_t *arguments)
{
  (void)&arguments;
  printf("Deleting all data stored in NVM3\r\n");
  nvm3_eraseAll(NVM3_DEFAULT_HANDLE);
  // This deletes the counters, too, so they must be re-initialised
  initialise_counters();
}

/***************************************************************************//**
 * Initialize NVM3 example.
 ******************************************************************************/
void nvm3_app_init(void)
{
  Ecode_t err;

  // This will call nvm3_open() with default parameters for
  // memory base address and size, cache size, etc.
  err = nvm3_initDefault();
  EFM_ASSERT(err == ECODE_NVM3_OK);

  // Initialise the counter objects to track writes and deletes.
  initialise_counters();

  // printf is configured to output over IOStream
  printf("\r\nWelcome to the nvm3 sample application\r\n");
  printf("Type 'help' to see available commands\r\n");
}

/***************************************************************************//**
 * NVM3 ticking function.
 ******************************************************************************/
void nvm3_app_process_action(void)
{
  // Check if NVM3 controller can release any out-of-date objects
  // to free up memory.
  // This may take more than one call to nvm3_repack()
  while (nvm3_repackNeeded(NVM3_DEFAULT_HANDLE)) {
    printf("Repacking NVM...\r\n");
    nvm3_repack(NVM3_DEFAULT_HANDLE);
  }
}
