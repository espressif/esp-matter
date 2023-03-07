/***************************************************************************
*
* @copyright 2021 Silicon Laboratories Inc.
* @brief ZWave+ Info Command Class source file
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <CC_ZWavePlusInfoPrivate.h>
#include <ZW_product_id_enum.h>
#include "SizeOf.h"
#include "config_app.h"

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

#if defined(ENDPOINT_ICONS)

static SEndpointIcon ZWavePlusEndpointIcons[] = {ENDPOINT_ICONS};

const SEndpointIconList EndpointIconList = {
                                      .pEndpointInfo = ZWavePlusEndpointIcons,
                                      .endpointInfoSize = sizeof_array(ZWavePlusEndpointIcons)
};

#define ENDPOINT_ICON_LIST &EndpointIconList

#else /* defined(ENDPOINT_ICONS) */

#define ENDPOINT_ICON_LIST NULL

#endif /* defined(ENDPOINT_ICONS) */

const SCCZWavePlusInfo pCCZWavePlusInfo = {
                               .pEndpointIconList = ENDPOINT_ICON_LIST,
                               .roleType = APP_ROLE_TYPE,
                               .nodeType = ZWAVEPLUS_INFO_REPORT_NODE_TYPE_ZWAVEPLUS_NODE,
                               .installerIconType = APP_ICON_TYPE,
                               .userIconType = APP_USER_ICON_TYPE
};
