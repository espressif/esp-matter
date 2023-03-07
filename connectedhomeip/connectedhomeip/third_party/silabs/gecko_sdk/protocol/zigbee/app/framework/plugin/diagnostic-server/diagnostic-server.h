/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Diagnostic Server plugin.
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
 * @defgroup diagnostic-server Diagnostic Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Diagnostic Cluster Server Component
 *
 * This component handles the retrieval of diagnostic information from the stack
 * through the diagnostic cluster. Implement the component
 * as follows:
 *  - 1. Mark all diagnostic attributes you wish to track as external.
 *  - 2. Add a call to emberAfReadDiagnosticAttribute(attributeMetadata, buffer);
 * from emberAfExternalAttributeReadCallback().
 *
 */

/**
 * @addtogroup diagnostic-server
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Read diagnostic attributes
 *
 * @param attributeMetadata Ver.: always
 * @param buffer Ver.: always
 *
 * @return bool true is success
 */
bool emberAfReadDiagnosticAttribute(
  EmberAfAttributeMetadata *attributeMetadata,
  uint8_t *buffer);

/** @} */ // end of name API
/** @} */ // end of diagnostic-server
