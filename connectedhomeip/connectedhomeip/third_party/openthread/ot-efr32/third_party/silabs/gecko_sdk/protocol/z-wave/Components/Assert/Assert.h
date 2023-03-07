/**
 * @file
 * @brief Defines ASSERT and STATIC_ASSERT
 * @details ASSERT Usage: ASSERT(expression that must be true)
 *          If expression is false, IRQs will be disabled, filename and
 *          line number will be debug printed, and a callback is called.
 *          Assert_SetCb can be used to set the callback.
 *
 *          Define NDEBUG to disable asserts. When NDEBUG is defined all
 *          asserts will be converted to code that will be optimized away
 *          by the compiler.
 *
 *          __FILENAME__ should be defined for each file, and will be
 *          printed as filename on asserts.
 *          Suggested implementation is by having CMAKE/make add it as
 *          compiler commandline define on compile of the individual file.
 *
 *          __FILE__ will be used if __FILENAME__ is not available, but
 *          its highly recommended to use __FILENAME__. Support for
 *          __FILE__ is only to ensure compatebility with other compilers
 *          build setups etc. that users of the SDK may switch to.
 *
 *
 *          STATIC_ASSERT usage:
 *          STATIC_ASSERT(
 *          bMustBeTrue,
 *          STATIC_ASSERT_FAILED_my_scope_unique_static_assert_error_message
 *          );
 *
 *          Use for compile time discovery of a modules expectations not
 *          being fulfilled, e.g. sizes that mismatch or order or struct
 *          members that module depends on.
 *
 *          STATIC_ASSERT Can be used both inside and outside methods.
 *          Error msg must be unique within scope.
 *
 *          STATIC_ASSERT does not compile into any real code or variables.
 *
 *          STATIC_ASSERT will generate compile error if bMustBeTrue cannot
 *          be evaluated compiletime (bMustBeTrue must be a
 *          'constant integral expression' or be derived from one.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdbool.h>

// Allow linking with C++ implementation (for unit tests)
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void (*AssertCb_t)();

void Assert(const char* pFileName, int iLineNumber);
/**
 * @brief Sets the callback function which is executed at the end of an assertion
 * 
 * @param[in] cb The callback function
 */
void Assert_SetCb(AssertCb_t cb);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef assert
#undef assert
#endif // assert
#define assert ASSERT

// __FILENAME__ is a custom definition added on compiler command line, this is to avoid 
// the full paths on the file name. __FILENAME__ is filename with no path
// The defines will ensure that the std full path __FILE__ is used if __FILENAME__ is not available
#ifndef NDEBUG
  #ifdef __FILENAME__
  #define ASSERT(BMUSTBETRUE)    ((BMUSTBETRUE) ? ((void)0) : Assert(__FILENAME__, __LINE__))  
  #else // __FILENAME__
  #define ASSERT(BMUSTBETRUE)    ((BMUSTBETRUE) ? ((void)0) : Assert(__FILE__, __LINE__))  
  #endif // __FILENAME__
#endif // NDEBUG

#ifdef NDEBUG
// Do nothing, without making any debug variables passed to ASSERT unused and
// thus avoid generating compiler warnings when NDEBUG is defined.
// Compiler will optimize this code and debug variables away.
// Do-while wrapping ensures macro requires a ';' at the end, and prevents issues with the if in
// cases where the macro is used within an if/else statement, in which case the following
// else can become else to the if(0).
#define ASSERT(BMUSTBETRUE)   do { (void) (BMUSTBETRUE); } while (0)
#endif // NDEBUG

// Usage: STATIC_ASSERT(bMustBeTrue, STATIC_ASSERT_FAILED_my_scope_unique_static_assert_error_message);
// Can be used both inside and outside methods. Error msg must be unique within scope.
// BMustBeTrue must be a 'constant integral expression' or be derived from only
// 'constant integral expressions'
// Does not generate any real code or variables.
// Will generate compile error if bMustBeTrue cannot be evaluated compiletime.
#define STATIC_ASSERT(bMustBeTrue, UniqueErrorMessage) \
  enum { UniqueErrorMessage = 1 / (bMustBeTrue) }

#endif	// _ASSERT_H_

