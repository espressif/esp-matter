/**
 * @file
 * Utility for handling endpoint indexing.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "endpoint_lookup.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/


uint8_t
FindEndPointID(ENDPOINT_LOOKUP *pEPLookup, uint8_t endpointindex)
{
  if (endpointindex < pEPLookup->bEndPointsCount)
  {
    return pEPLookup->pEndPointList[endpointindex];
  }
  return 0xFF;
}

uint8_t GetEndPointCount(ENDPOINT_LOOKUP *pEPLookup)
{
  return  pEPLookup->bEndPointsCount;
}

uint8_t
FindEndPointIndex(ENDPOINT_LOOKUP *pEPLookup, uint8_t endpoint)
{
  uint8_t i;
  if (endpoint) /*0 endpoint is mapped for the first end point*/
  {
    for (i = 0; i < pEPLookup->bEndPointsCount; i++)
    {
      if (pEPLookup->pEndPointList[i] == endpoint)
        return i;
    }
    return 0xFF;
  }
  return 0;
}
