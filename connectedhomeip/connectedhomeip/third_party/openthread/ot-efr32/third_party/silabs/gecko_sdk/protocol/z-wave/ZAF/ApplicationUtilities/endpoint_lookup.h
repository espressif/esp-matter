/**
 * @file
 * Utility for handling endpoint indexing.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ENDPOINT_LOOKUP_H_
#define _ENDPOINT_LOOKUP_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <stdint.h>

/**
 * Structure Endpoint look-up
 */
typedef struct _ENDPOINT_LOOKUP_
{
  uint8_t bEndPointsCount;
  uint8_t * pEndPointList;
}ENDPOINT_LOOKUP;


/**
 * Returns the endpoint from a given index in the endpoint lookup table.
 * @param pEPLookup Pointer to an endpoint lookup table.
 * @param endpointIndex The index of the endpoint in the endpoint lookup table.
 * @return The endpoint located in the given index if the index exists, otherwise 0xFF.
 */
uint8_t FindEndPointID(ENDPOINT_LOOKUP *pEPLookup, uint8_t endpointIndex);

/**
 * Returns the number of endpoints in the given endpoint lookup table.
 * @param pEPLookup Pointer to an endpoint lookup table.
 * @return Number of endpoints
 */
uint8_t GetEndPointCount(ENDPOINT_LOOKUP *pEPLookup);

/**
 * Returns the index of a given endpoint.
 * @param pEPLookup Pointer to an endpoint lookup table.
 * @param endpoint The endpoint of which the index must be found.
 * @return The index of the given endpoint.
 */
uint8_t FindEndPointIndex(ENDPOINT_LOOKUP *pEPLookup, uint8_t endpoint);

#endif /* _ENDPOINT_LOOKUP_H_ */

