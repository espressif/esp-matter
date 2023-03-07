/***************************************************************************//**
 * @file
 * @brief Implements application version checking
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "api/btl_interface.h"
#include "core/btl_bootload.h"

#include <string.h>

bool parser_applicationUpgradeValidCallback(ApplicationData_t *app)
{
  uint32_t appPropertiesLocation = (uint32_t)mainBootloaderTable->startOfAppSpace->signature;

  if ((appPropertiesLocation > (uint32_t)mainBootloaderTable->startOfAppSpace)
      && (appPropertiesLocation < (uint32_t)mainBootloaderTable->endOfAppSpace)) {
    ApplicationProperties_t *appProperties = (ApplicationProperties_t *)(appPropertiesLocation);
    // App properties points into flash
    if (bootload_checkApplicationPropertiesMagic(appProperties)) {
      // App properties struct is present
      // Compatibility check of the application properties struct.
      if (((appProperties->structVersion & APPLICATION_PROPERTIES_VERSION_MAJOR_MASK)
           >> APPLICATION_PROPERTIES_VERSION_MAJOR_SHIFT)
          > (uint32_t)APPLICATION_PROPERTIES_VERSION_MAJOR) {
        return false;
      }
      if (app->version <= appProperties->app.version) {
        // Upgrade version is less than or equal to installed app
        return false;
      } else {
        if (memcmp(app->productId, appProperties->app.productId, 16UL) == 0) {
          // Product ID is correct -- valid app upgrade
          return true;
        } else {
          // Wrong product ID
          return false;
        }
      }
    } else {
      // No app properties struct in flash
    }
  } else {
    // No app properties pointer in flash
  }

  // If there was no app properties struct at the app properties location,
  // the old app has probably already been deleted. We cannot know if this is a
  // newer version or not. Assume that the reason for the deletion was that the
  // previous upgrade was interrupted, and allow the upgrade to continue.
  return true;
}
