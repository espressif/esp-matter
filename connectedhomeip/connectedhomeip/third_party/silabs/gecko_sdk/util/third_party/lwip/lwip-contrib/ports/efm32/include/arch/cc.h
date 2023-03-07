#ifndef _LWIP_ARCH_CC_H_
#define _LWIP_ARCH_CC_H_

#include <stdio.h>
#include <stdlib.h>

typedef int sys_prot_t;

/** Packed struct support, note that gcc support is already present in lwip */
#if defined(__ICCARM__) || defined(__CC_ARM)
#define PACK_STRUCT_BEGIN  _Pragma("pack(1)")
#define PACK_STRUCT_END    _Pragma("pack()")
#endif

/** Associating the checksum algorithm using Ethernet 802.3 standard
 * lwIP gives 3 algorithms
 * 1 load byte by byte,
 *   construct 16 bits word,
 *   add last byte
 * 2 load first byte if odd address,
 *   loop processing 16 bits words,
 *   add last byte.
 * 3 load first byte and word if not 4 byte aligned,
 *   loop processing 32 bits words,
 *   add last word/byte.
 */
#define LWIP_CHKSUM_ALGORITHM    3

/** Define local assert methods */
#ifndef LWIP_NOASSERT
#define LWIP_PLATFORM_ASSERT(x)    do { printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); } while (0)
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif

/** diagnostic output */
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)      do { printf x; } while (0)
#else
#define LWIP_PLATFORM_DIAG(x)
#endif

/** Define local rand method */
#define LWIP_RAND    rand

#endif /* _LWIP_ARCH_CC_H_ */
