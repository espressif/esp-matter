/**
 * @file
 * Handler for Command Class Z-Wave Plus Info.
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef _CC_ZWAVE_PLUS_INFO_PRIVATE_H_
#define _CC_ZWAVE_PLUS_INFO_PRIVATE_H_

#include <ZW_classcmd.h>
#include <ZAF_types.h>

typedef struct
{
  uint16_t installerIconType;
  uint16_t userIconType;
}
SEndpointIcon;

typedef struct
{
  SEndpointIcon const * const pEndpointInfo;
  uint8_t endpointInfoSize;
}
SEndpointIconList;

typedef struct
{
  SEndpointIconList const * const pEndpointIconList;
  uint8_t roleType;
  uint8_t nodeType;
  uint16_t installerIconType;
  uint16_t userIconType;
}
SCCZWavePlusInfo;

extern const SCCZWavePlusInfo pCCZWavePlusInfo;

/**
 * @brief Set the data pointer for the zw plus info
 * @remark Only used in the unit tests that is why we keep it in the private 
 *  section but not as static
 * @param pZWPlusInfo A pointer to a ZWPlusInfo
 */
void CC_ZWavePlusInfo_Set(SCCZWavePlusInfo const * const pZWPlusInfo);

#endif /* _CC_ZWAVE_PLUS_INFO_PRIVATE_H_ */
