/**
 * @file
 * Handler for Command Class Central Scene.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef PRODUCTPLUS_COMMANDCLASSCENTRALSCENE_H_
#define PRODUCTPLUS_COMMANDCLASSCENTRALSCENE_H_


#include <CC_Common.h>
#include <agi.h>

/**
 * Contains all configuration parameters.
 */
typedef struct
{
  uint8_t slowRefresh;
}
central_scene_configuration_t;

typedef struct s_CC_centralScene_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
} s_CC_centralScene_data_t;

/**
 * Function implemented by the application which defines what Central Scene capabilities the
 * application supports.
 * @param pData Pointer to the variable storing data as defined in the
 * specification of the Central Scene command class.
 * @return Number of bytes with key attributes.
 */
extern uint8_t getAppCentralSceneReportData(ZW_CENTRAL_SCENE_SUPPORTED_REPORT_1BYTE_V3_FRAME * pData);

/**
 * Function implemented by the application which defines what Central Scene Configuration Get Command
 * is used to read the use of optional node capabilities for scene notifications.
 * @param pConfiguration Pointer to struct containing Central Scene configuration parameters.
 */
extern void getAppCentralSceneConfiguration(central_scene_configuration_t * pConfiguration);

/**
 * Function implemented by the application which defines what Central Scene Configuration Set Command
 * is used to configure the use of optional node capabilities for scene notifications.
 * @param pConfiguration Pointer to struct containing Central Scene configuration parameters.
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t setAppCentralSceneConfiguration(central_scene_configuration_t * pConfiguration);

/**
 * Initiates the transmission of a "Central Scene Notification" command.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] keyAttribute The key event.
 * @param[in] sceneNumber The scene number.
 * @param[in] pCbFunc Callback function to be called when transmission is done/failed.
 * @return Status of the job.
 */
JOB_STATUS
CommandClassCentralSceneNotificationTransmit(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t keyAttribute,
  uint8_t sceneNumber,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * Send report when change happen via lifeLine.
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
void CC_CentralScene_configuration_report_stx(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
    void *pData);
#endif /* PRODUCTPLUS_COMMANDCLASSCENTRALSCENE_H_ */
