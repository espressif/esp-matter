/**
 * @file Min2Max2.h
 * @brief Min2Max2 header file
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef _MIN2MAX2_H_
#define _MIN2MAX2_H_

#include <stdint.h>

/*===============================   Minimum2   ===============================
**  Returns the smallest of two arguments
**--------------------------------------------------------------------------*/
static inline uint32_t Minimum2(uint32_t a, uint32_t b)
{
	if (a < b)
	{
		return a;
	}

	return b;
}


/*===============================   Maximum2   ===============================
**  Returns the largest of two arguments
**--------------------------------------------------------------------------*/
static inline uint32_t Maximum2(uint32_t a, uint32_t b)
{
	if (a > b)
	{
		return a;
	}

	return b;
}


/*============================   Minimum2Signed   ============================
**  Returns the smallest of two signed arguments
**--------------------------------------------------------------------------*/
static inline int32_t Minimum2Signed(int32_t a, int32_t b)
{
  if (a < b)
  {
    return a;
  }

  return b;
}


/*============================   Maximum2Signed   ============================
**  Returns the largest of two signed arguments
**--------------------------------------------------------------------------*/
static inline int32_t Maximum2Signed(int32_t a, int32_t b)
{
  if (a > b)
  {
    return a;
  }

  return b;
}

#endif	// _MIN2MAX2_H_

