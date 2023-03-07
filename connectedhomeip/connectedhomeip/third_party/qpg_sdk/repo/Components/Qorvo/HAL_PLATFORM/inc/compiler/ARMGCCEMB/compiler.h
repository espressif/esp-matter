#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#define INLINE inline
#define ALWAYS_INLINE static inline __attribute__((always_inline))
#define PACKED_PRE
#define PACKED_POST __attribute__((packed))
#define LINKER_SECTION(name) __attribute__((section((#name))))
#define LINKER_USED __attribute__((used))
#define ALIGNMENT_NEEDED
#define COMPILER_ALIGNED(a)    __attribute__((__aligned__(a)))


//please note: this hinders debugging!!
#define NORETURN __attribute__((noreturn))

#define INTERRUPT_H __attribute__((__interrupt__))

typedef uintptr_t UIntPtr;
typedef UIntPtr   UIntPtr_P;

#define POPCOUNT(byte) __builtin_popcount(byte)

#if defined(GP_DIVERSITY_MEMCPY_ALIGNMENT_NEEDED)
#define MEMCPY(dst, src, len)   memcpy__((dst), (src), (len))
static inline void memcpy__(void* dst, const void* src, size_t len)
{
    if(((UIntPtr)dst % 8) && (len >= 8))
    {
        //Not 8-byte aligned and > 8 bytes to copy -> byte per byte copy
        while(len != 0)
        {
            len--;
            ((unsigned char*)(dst))[len] = ((unsigned char*)(src))[len];
        }
    }
    else
    {
        //8-byte aligned - ok to use memcpy
        memcpy(dst, src, len);
    }
}
#else
#define MEMCPY(dst, src, len)   memcpy((dst), (src), (len))
#endif// if defined(GP_DIVERSITY_MEMCPY_ALIGNMENT_NEEDED)

#endif //_COMPILER_H_

