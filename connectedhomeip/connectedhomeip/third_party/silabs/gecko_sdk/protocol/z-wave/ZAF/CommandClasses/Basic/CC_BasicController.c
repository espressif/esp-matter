/***************************************************************************
*
* @copyright 2018 Silicon Laboratories Inc.
* @brief Basic Command Class source file
*
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_basis_api.h>
#include <ZW_TransportLayer.h>
#include <ZW_TransportEndpoint.h>
#include "config_app.h"
#include <CC_Basic.h>
#include <CC_Common.h>
#include <agi.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/


JOB_STATUS
CC_Basic_Set_tx(
  const AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t bValue,
  void (*pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  CMD_CLASS_GRP cmdGrp = {COMMAND_CLASS_BASIC, BASIC_SET};

  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      &bValue,
      1,
      true,
      pCbFunc);
}
