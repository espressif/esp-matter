/**
 * @file serialapi_file.h
 * @copyright 2022 Silicon Laboratories Inc.
 * @brief module that handle SerialAPI application data
 * @details This module implement SerialAPI filesystem functions
 */

#include <stdbool.h>
#include <stdint.h>
#include <zpal_radio.h>
#include <zpal_nvm.h>

#define FILE_ID_APPLICATIONSETTINGS       102
#define FILE_ID_APPLICATIONCMDINFO        103
#define FILE_ID_APPLICATIONCONFIGURATION  104
#define FILE_ID_APPLICATIONDATA           200
#define FILE_ID_PROPRIETARY_1             300

/**
 * @brief Initailize the serialAPI file system
 *
 * return false if application files was corrupted or didn't exist already, otherwise true
 */
uint8_t SerialApiFileInit(void);

/**
 * @brief Reset SerialAPI allication file system
 */

void AppResetNvm(void);
/**
 * @brief Reads application data from file system.
 */
uint8_t SerialApiNvmReadAppData(uint32_t offset, uint8_t* pAppData, uint32_t iLength);

/**
 * @brief Writes application data to file system.
 */
uint8_t SerialApiNvmWriteAppData(uint32_t offset, const uint8_t* pAppData, uint32_t iLength);

uint8_t
SaveApplicationSettings(uint8_t bListening,
                        uint8_t bGeneric,
                        uint8_t bSpecific);

uint8_t
ReadApplicationSettings(uint8_t* pListening,
                        uint8_t* pGeneric,
                        uint8_t* pSpecific);

/**
 * @brief Writes the node cmd class information setting
 */
uint8_t
SaveApplicationCCInfo (uint8_t bUnSecureIncludedCCLen,
                       const uint8_t* pUnSecureIncludedCC,
                       uint8_t bSecureIncludedUnSecureCCLen,
                       const uint8_t* pSecureIncludedUnSecureCC,
                       uint8_t  bSecureIncludedSecureCCLen,
                       const uint8_t* pSecureIncludedSecureCC);

/**
 * @brief read the node cmd class information setting
 */
uint8_t
ReadApplicationCCInfo (uint8_t* pUnSecureIncludedCCLen,
                       uint8_t* pUnSecureIncludedCC,
                       uint8_t* pSecureIncludedUnSecureCCLen,
                       uint8_t* pSecureIncludedUnSecureCC,
                       uint8_t* pSecureIncludedSecureCCLen,
                       uint8_t* pSecureIncludedSecureCC);

/**
 * @brief Writes the application configuration to file system
 */
uint8_t
SaveApplicationRfRegion(zpal_radio_region_t rfRegion);

/**
 * @brief Reads the application configuration from file system
 */
uint8_t
ReadApplicationRfRegion(zpal_radio_region_t* rfRegion);

/**
 * @brief Writes the application Tx power configuration to file system
 */
uint8_t
SaveApplicationTxPowerlevel(zpal_tx_power_t ipower, zpal_tx_power_t power0dbmMeasured);

/**
 * @brief Reads the application Tx power configuration from file system
 */
uint8_t
ReadApplicationTxPowerlevel(zpal_tx_power_t *ipower, zpal_tx_power_t *power0dbmMeasured);


/**
 * Writes the application Max LR tx power value configuration to file system
 * 
 * @param maxTxPwr  the LR max tx power value in deci
 * @return value was saved correctly
 */
uint8_t
SaveApplicationMaxLRTxPwr(zpal_tx_power_t maxTxPwr);

/**
 * Reads the application Max LR tx power value configuration from file system
 * 
 * @param maxTxPwr  pointer to the LR max tx power value in deci
 * @return value was read correctly
 */
uint8_t
ReadApplicationMaxLRTxPwr(zpal_tx_power_t *maxTxPwr);

/**
 * @brief Writes radio_debug_enable to file system
 */
uint8_t
SaveApplicationEnablePTI(uint8_t radio_debug_enable);

/**
 * @brief Reads radio_debug_enable from file system
 */
uint8_t
ReadApplicationEnablePTI(uint8_t *radio_debug_enable);

/**
 * @brief Reads the application version from NVM
 */
uint32_t
ReadApplicationVersion(void);

/**
 * Returns the handle to Serial API's file system.
 * @return Handle to file system.
 */
zpal_nvm_handle_t SerialAPIGetFileSystemHandle(void);
