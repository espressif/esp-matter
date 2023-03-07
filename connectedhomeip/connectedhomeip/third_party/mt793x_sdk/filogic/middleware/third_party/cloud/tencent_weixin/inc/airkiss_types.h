/*
 * airkiss_types.h
 *
 *  Created on: Dec 16, 2015
 *      Author: zorrowu
 */

#ifndef AIRKISS_TYPES_H_
#define AIRKISS_TYPES_H_

#ifndef WITHOUT_STDINT_H
#include <stdint.h>
#else

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef int8_t
typedef signed char int8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef int16_t
typedef signed short int16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef int32_t
typedef signed int int32_t;
#endif

#endif


#define ak_socket int


typedef struct ak_mutex_t ak_mutex_t;


#endif /* AIRKISS_TYPES_H_ */
