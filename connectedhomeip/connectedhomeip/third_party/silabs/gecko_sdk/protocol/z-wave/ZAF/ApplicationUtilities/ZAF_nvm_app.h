/**
 * @file
 * @brief Defines file system for application.
 *
 * @details The module contains a zpal_nvm_handle_t that defines the file system
 * for the application. A pointer to the handle is passed as output parameter
 * from ApplicationFileSystemInit(). There are also functions for verifying
 * that the file system contains the files that are expected.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_ZAF_NVM_APP_H_
#define ZAF_APPLICATIONUTILITIES_ZAF_NVM_APP_H_

#include <stdbool.h>
#include <zpal_nvm.h>

/**
* @brief Opens file system for application and passes a handle to it as output pointer.
*
* @param[out] pFileSystemApplication Handle to file system passed to application.
* @return bool true if zpal_nvm_init() succeeded else false.
*/
bool ApplicationFileSystemInit(zpal_nvm_handle_t* pFileSystemApplication);

/**
 * Returns the application file system handle.
 * @return The application file system handle.
 */
zpal_nvm_handle_t ZAF_GetFileSystemHandle(void);

#endif /* ZAF_APPLICATIONUTILITIES_ZAF_NVM_APP_H_ */
