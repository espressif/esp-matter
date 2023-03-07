/***************************************************************************//**
 * @file
 * @brief The Simple Storage Module driver interface.  In other words, primitives
 * for reading / writing and storing data about the OTA file that is stored,
 * or is in the process of being downloaded and stored.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @defgroup ota-storage-simple OTA Storage Simple
 * @ingroup component
 * @brief API and Callbacks for the OTA Storage Simple Component
 *
 */

/**
 * @addtogroup ota-storage-simple
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup ota_storage_simple_driver_cb OTA Storage Simple Driver
 * @ingroup af_callback
 * @brief Callbacks for OTA Storage Simple Driver Component
 *
 */

/**
 * @addtogroup ota_storage_simple_driver_cb
 * @{
 */

/** @brief Custom Storage Init callback.
 *
 * @return bool
 *
 */
bool emberAfCustomStorageInitCallback(void);

/** @brief Custom Storage Read callback.
 *
 * @param offset offset
 * @param length length
 * @param returnData data returned
 *
 * @return bool
 *
 */
bool emberAfCustomStorageReadCallback(uint32_t offset,
                                      uint32_t length,
                                      uint8_t* returnData);

/** @brief Custom Storage Write callback.
 *
 * @param dataToWrite data to be written
 * @param offset offset
 * @param length length
 *
 * @return bool
 *
 */
bool emberAfCustomStorageWriteCallback(const uint8_t* dataToWrite,
                                       uint32_t offset,
                                       uint32_t length);

/** @} */ // end of ota_storage_simple_driver_cb
/** @} */ // end of name Callbacks
/** @} */ // end of ota-storage-simple

// TODO: put this gating back in once we have that mechanism in place for
// the generated CLI
//#if defined(EMBER_TEST)
void emAfOtaLoadFileCommand(void);
//#endif
