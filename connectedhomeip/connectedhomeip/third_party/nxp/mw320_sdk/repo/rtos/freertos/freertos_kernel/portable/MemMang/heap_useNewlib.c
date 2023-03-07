/**
 * \file heap_useNewlib.c
 * \brief Wrappers required to use newlib malloc-family within FreeRTOS.
 *
 * \par Overview
 * Route FreeRTOS memory management functions to newlib's malloc family.
 * Thus newlib and FreeRTOS share memory-management routines and memory pool,
 * and all newlib's internal memory-management requirements are supported.
 *
 * \author Dave Nadler
 * \date 2-July-2017
 * \version 23-Sep-2019 malloc accounting, comments, newlib version check
 *
 * \see http://www.nadler.com/embedded/newlibAndFreeRTOS.html
 * \see https://sourceware.org/newlib/libc.html#Reentrancy
 * \see https://sourceware.org/newlib/libc.html#malloc
 * \see https://sourceware.org/newlib/libc.html#index-_005f_005fenv_005flock
 * \see https://sourceware.org/newlib/libc.html#index-_005f_005fmalloc_005flock
 * \see https://sourceforge.net/p/freertos/feature-requests/72/
 * \see http://www.billgatliff.com/newlib.html
 * \see http://wiki.osdev.org/Porting_Newlib
 * \see http://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html
 *
 *
 * \copyright
 * (c) Dave Nadler 2017-2019, All Rights Reserved.
 * Web:         http://www.nadler.com
 * email:       drn@nadler.com
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Use or redistributions of source code must retain the above copyright notice,
 *   this list of conditions, ALL ORIGINAL COMMENTS, and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h> // maps to newlib...
#include <malloc.h> // mallinfo...
#include <errno.h>  // ENOMEM
#include <stdbool.h>
#include <stddef.h>

#include "newlib.h"
#if (__NEWLIB__ != 3) || (__NEWLIB_MINOR__ != 1)
  #warning "This wrapper was verified for newlib version 3.1.0; please ensure newlib's external requirements for malloc-family are unchanged!"
#endif

#include "FreeRTOS.h" // defines public interface we're implementing here
#if !defined(configUSE_NEWLIB_REENTRANT) ||  (configUSE_NEWLIB_REENTRANT!=1)
  #warning "#define configUSE_NEWLIB_REENTRANT 1 // Required for thread-safety of newlib sprintf, strtok, etc..."
  // If you're *REALLY* sure you don't need FreeRTOS's newlib reentrancy support, remove this warning...
#endif
#include "task.h"

// ================================================================================================
// External routines required by newlib's malloc (sbrk/_sbrk, __malloc_lock/unlock)
// ================================================================================================

#ifndef NDEBUG
    static int totalBytesProvidedBySBRK = 0;
#endif

// Simplistic sbrk implementations assume stack grows downwards from top of memory,
// and heap grows upwards starting just after BSS.
// FreeRTOS normally allocates task stacks from a pool placed within BSS or DATA.
// Thus within a FreeRTOS task, stack pointer is always below end of BSS.
// When using this module, stacks are allocated from malloc pool, still always prior
// current unused heap area...
#if defined(__MCUXPRESSO)
  #define configLINKER_HEAP_BASE_SYMBOL _pvHeapStart
  #define configLINKER_HEAP_LIMIT_SYMBOL _pvHeapLimit
  #define configLINKER_HEAP_SIZE_SYMBOL _HeapSize
#elif defined(__GNUC__)
  #define configLINKER_HEAP_BASE_SYMBOL end
  #define configLINKER_HEAP_LIMIT_SYMBOL __HeapLimit
  #define configLINKER_HEAP_SIZE_SYMBOL HEAP_SIZE
#endif
extern char configLINKER_HEAP_BASE_SYMBOL;
extern char configLINKER_HEAP_LIMIT_SYMBOL;
extern char configLINKER_HEAP_SIZE_SYMBOL;

static int heapBytesRemaining = (int)&configLINKER_HEAP_SIZE_SYMBOL; // that's (&__HeapLimit)-(&__HeapBase)

//! sbrk/_sbrk version supporting reentrant newlib (depends upon above symbols defined by linker control file).
char * sbrk(int incr) {
    static char *currentHeapEnd = &configLINKER_HEAP_BASE_SYMBOL;
    vTaskSuspendAll(); // Note: safe to use before FreeRTOS scheduler started
    char *previousHeapEnd = currentHeapEnd;
    if (currentHeapEnd + incr > &configLINKER_HEAP_LIMIT_SYMBOL) {
        #if( configUSE_MALLOC_FAILED_HOOK == 1 )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook();
        }
        #elif 1 // safest default when user doesn't explicitly code configUSE_MALLOC_FAILED_HOOK
            // If you want to alert debugger or halt...
            // WARNING: bkpt instruction may prevent watchdog operation...
            while(1) { __asm("bkpt #0"); }; // Stop in GUI as if at a breakpoint (if debugging, otherwise loop forever)
        #else
            // If you prefer to believe your application will gracefully trap out-of-memory...
            _impure_ptr->_errno = ENOMEM; // newlib's thread-specific errno
            xTaskResumeAll();
        #endif
        return (char *)-1; // the malloc-family routine that called sbrk will return 0
    }
    // 'incr' of memory is available: update accounting and return it.
    currentHeapEnd += incr;
    heapBytesRemaining -= incr;
    #ifndef NDEBUG
        totalBytesProvidedBySBRK += incr;
    #endif
    xTaskResumeAll();
    return (char *) previousHeapEnd;
}
//! Synonym for sbrk.
char * _sbrk(int incr) { return sbrk(incr); };

void __malloc_lock(struct _reent *p)   { configASSERT( !xPortIsInsideInterrupt() ); // Make damn sure no mallocs inside ISRs!!
                                               vTaskSuspendAll(); };
void __malloc_unlock(struct _reent *p) { (void)xTaskResumeAll();  };

// newlib also requires implementing locks for the application's environment memory space,
// accessed by newlib's setenv() and getenv() functions.
// As these are trivial functions, momentarily suspend task switching (rather than semaphore).
// ToDo: Move __env_lock/unlock to a separate newlib helper file.
void __env_lock()    {       vTaskSuspendAll(); };
void __env_unlock()  { (void)xTaskResumeAll();  };

#if 1 // Provide malloc debug and accounting wrappers
  /// /brief  Wrap malloc/malloc_r to help debug who requests memory and why.
  /// To use these, add linker options: -Xlinker --wrap=malloc -Xlinker --wrap=_malloc_r
  // Note: These functions are normally unused and stripped by linker.
  int TotalMallocdBytes;
  int MallocCallCnt;
  static bool inside_malloc;
  void *__wrap_malloc(size_t nbytes) {
    extern void * __real_malloc(size_t nbytes);
    MallocCallCnt++;
    TotalMallocdBytes += nbytes;
    inside_malloc = true;
      void *p = __real_malloc(nbytes); // will call malloc_r...
    inside_malloc = false;
    return p;
  };
  void *__wrap__malloc_r(void *reent, size_t nbytes) {
    extern void * __real__malloc_r(size_t nbytes);
    if(!inside_malloc) {
      MallocCallCnt++;
      TotalMallocdBytes += nbytes;
    };
    void *p = __real__malloc_r(nbytes);
    return p;
  };
#endif

// ================================================================================================
// Implement FreeRTOS's memory API using newlib-provided malloc family.
// ================================================================================================

void *pvPortMalloc( size_t xSize ) PRIVILEGED_FUNCTION {
    void *p = malloc(xSize);
    return p;
}
void vPortFree( void *pv ) PRIVILEGED_FUNCTION {
    free(pv);
};

size_t xPortGetFreeHeapSize( void ) PRIVILEGED_FUNCTION {
    struct mallinfo mi = mallinfo(); // available space now managed by newlib
    return mi.fordblks + heapBytesRemaining; // plus space not yet handed to newlib by sbrk
}

// GetMinimumEverFree is not available in newlib's malloc implementation.
// So, no implementation is provided: size_t xPortGetMinimumEverFreeHeapSize( void ) PRIVILEGED_FUNCTION;

//! No implementation needed, but stub provided in case application already calls vPortInitialiseBlocks
void vPortInitialiseBlocks( void ) PRIVILEGED_FUNCTION {};
