/**
 * @file
 * Functions for calculation of CRC.
 * @copyright 2018 Silicon Laboratories Inc.
 */
#ifndef _CRC_H_
#define _CRC_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <stdint.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

#define CRC_INITAL_VALUE                    0x1D0Fu

/**
 * Returns a CRC calculation using the CCITT polynomial (0x1021).
 *
 * @param crc Initial value set to CRC_INITAL_VALUE unless calculating multiple parts of a frame. In that case
 *            the value should be set to the result of the previous calculation.
 * @param pDataAddr Pointer to the array of data.
 * @param bDataLen Length of the data.
 * @return CRC value
 */
uint16_t CRC_CheckCrc16(
  uint16_t crc,
  uint8_t *pDataAddr,
  uint16_t bDataLen
);

#endif /* _CRC_H_ */
