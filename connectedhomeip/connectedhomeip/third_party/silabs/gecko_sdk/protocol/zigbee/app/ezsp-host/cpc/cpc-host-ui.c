/***************************************************************************//**
 * @file
 * @brief  EZSP CPC Host user interface functions
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "stack/include/ember-types.h"
#include "app/ezsp-host/cpc/cpc-host.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif  // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT
#include "ota-storage-linux-config.h"
#endif  // SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT

#define ERR_LEN               128   // max length error message

static const char options[] = "c:hv::d:";

extern int optind;
extern char *optarg;
#ifdef SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT
// To set the OTA file storage directory in runtime.
extern char defaultStorageDirectory[OTA_FILE_STORAGE_DIR_LENGTH];
#endif  // SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT

static char cpcdInstanceName[CPCD_INSTANCE_LEN];
extern char *zigbee_cpc_instance_name;

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr)
{
  int c;

  if (!argv || !errStr) {
    snprintf(errStr, ERR_LEN, "Error: argv[] is %s and errStr is %s.\n",
             argv ? "valid" : "NULL",
             errStr ? "valid" : "NULL");
    return false;
  }

  while (true) {
    c = getopt(argc, argv, options);
    if (c == -1) {
      if (optind != argc) {
        snprintf(errStr, ERR_LEN, "Invalid option %s.\n", argv[optind]);
      }
      break;
    }

    switch (c) {
      case 'h':
      case '?':
        snprintf(errStr, ERR_LEN, "\n");
        break;
      case 'c':
        if (!optarg || (sscanf(optarg, "%15s", cpcdInstanceName) <= 0)) {
          snprintf(errStr, ERR_LEN, "Invalid CPC daemon instance name %s.\n",
                   optarg ? optarg : "NULL");
        } else {
          cpcdInstanceName[CPCD_INSTANCE_LEN - 1] = '\0';
          zigbee_cpc_instance_name = cpcdInstanceName;
        }
        break;
      case 'd': {
  #ifdef SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT
        char otaStorageDir[128];
        if (!optarg || (sscanf(optarg, "%127s", otaStorageDir) <= 0)) {
          snprintf(errStr, ERR_LEN, "Invalid OTA files storage directory name %s.\n",
                   optarg ? optarg : "NULL");
        } else if (strlen(otaStorageDir) >= OTA_FILE_STORAGE_DIR_LENGTH) {
          snprintf(errStr, ERR_LEN, "OTA files storage directory %s too long.\n",
                   otaStorageDir);
        } else {
          otaStorageDir[OTA_FILE_STORAGE_DIR_LENGTH - 1] = '\0';
          strncpy(defaultStorageDirectory, otaStorageDir, OTA_FILE_STORAGE_DIR_LENGTH);
        }
  #endif  // SL_CATALOG_ZIGBEE_OTA_STORAGE_POSIX_FILESYSTEM_PRESENT
      }
      break;
      default:
        assert(1);
        break;
    }   // end of switch (c)
  } //end while
  return true;
}
