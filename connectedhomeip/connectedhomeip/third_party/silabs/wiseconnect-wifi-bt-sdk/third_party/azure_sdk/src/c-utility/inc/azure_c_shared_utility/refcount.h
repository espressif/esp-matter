// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


/*this header contains macros for ref_counting a variable.

There are no upper bound checks related to uint32_t overflow because we expect that bigger issues are in
the system when more than 4 billion references exist to the same variable. In the case when such an overflow
occurs, the object's ref count will reach zero (while still having 0xFFFFFFFF references) and likely the
controlling code will take the decision to free the object's resources. Then, any of the 0xFFFFFFFF references
will interact with deallocated memory / resources resulting in an undefined behavior.
*/

#ifndef REFCOUNT_H
#define REFCOUNT_H

#ifdef __cplusplus
#include <cstdlib>
#include <cstdint>
#else
#include <stdlib.h>
#include <stdint.h>
#endif

#include "azure_c_shared_utility/gballoc.h"
#include "azure_macro_utils/macro_utils.h"

// Include the platform-specific file that defines atomic functionality
#include "refcount_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define REFCOUNT_TYPE(type) \
struct MU_C2(MU_C2(REFCOUNT_, type), _TAG)

#define REFCOUNT_SHORT_TYPE(type) \
MU_C2(REFCOUNT_, type)

#define REFCOUNT_TYPE_DECLARE_CREATE(type) MU_C2(REFCOUNT_SHORT_TYPE(type), _Create)
#define REFCOUNT_TYPE_DECLARE_CREATE_WITH_EXTRA_SIZE(type) MU_C2(REFCOUNT_SHORT_TYPE(type), _Create_With_Extra_Size)
#define REFCOUNT_TYPE_CREATE(type) MU_C2(REFCOUNT_SHORT_TYPE(type), _Create)()
#define REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE(type, size) MU_C2(REFCOUNT_SHORT_TYPE(type), _Create_With_Extra_Size)(size)
#define REFCOUNT_TYPE_DECLARE_DESTROY(type) MU_C2(REFCOUNT_SHORT_TYPE(type), _Destroy)
#define REFCOUNT_TYPE_DESTROY(type, var) MU_C2(REFCOUNT_SHORT_TYPE(type), _Destroy)(var)

/*this introduces a new refcount'd type based on another type */
/*and an initializer for that new type that also sets the ref count to 1. The type must not have a flexible array*/
/*the newly allocated memory shall be free'd by free()*/
/*and the ref counting is handled internally by the type in the _Create/ _Create_With_Extra_Size /_Clone /_Destroy functions */

/* Codes_SRS_REFCOUNT_01_005: [ REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE shall allocate memory for the type that is ref counted (type) plus extra memory enough to hold size bytes. ]*/
/* Codes_SRS_REFCOUNT_01_006: [ On success it shall return a non-NULL handle to the allocated ref counted type type. ]*/
/* Codes_SRS_REFCOUNT_01_007: [ If any error occurs, REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE shall return NULL. ]*/
#define DEFINE_CREATE_WITH_EXTRA_SIZE(type) \
static type* REFCOUNT_TYPE_DECLARE_CREATE_WITH_EXTRA_SIZE(type)(size_t size) \
{ \
    REFCOUNT_TYPE(type)* ref_counted = (REFCOUNT_TYPE(type)*)malloc(sizeof(REFCOUNT_TYPE(type)) + size); \
    type* result; \
    if (ref_counted == NULL) \
    { \
        result = NULL; \
    } \
    else \
    { \
        result = &ref_counted->counted; \
        INIT_REF(type, result); \
    } \
    return result; \
} \

/* Codes_SRS_REFCOUNT_01_002: [ REFCOUNT_TYPE_CREATE shall allocate memory for the type that is ref counted. ]*/
/* Codes_SRS_REFCOUNT_01_003: [ On success it shall return a non-NULL handle to the allocated ref counted type type. ]*/
/* Codes_SRS_REFCOUNT_01_004: [ If any error occurs, REFCOUNT_TYPE_CREATE shall return NULL. ]*/
#define DEFINE_CREATE(type) \
static type* REFCOUNT_TYPE_DECLARE_CREATE(type) (void) \
{ \
    return REFCOUNT_TYPE_DECLARE_CREATE_WITH_EXTRA_SIZE(type)(0); \
} \

/* Codes_SRS_REFCOUNT_01_008: [ REFCOUNT_TYPE_DESTROY shall free the memory allocated by REFCOUNT_TYPE_CREATE or REFCOUNT_TYPE_CREATE_WITH_EXTRA_SIZE. ]*/
/* Codes_SRS_REFCOUNT_01_009: [ If counted_type is NULL, REFCOUNT_TYPE_DESTROY shall return. ]*/
#define DEFINE_DESTROY(type) \
static void REFCOUNT_TYPE_DECLARE_DESTROY(type)(type* counted_type) \
{ \
    void* ref_counted = (void*)((unsigned char*)counted_type - offsetof(REFCOUNT_TYPE(type), counted)); \
    free(ref_counted); \
}

#define DEFINE_REFCOUNT_TYPE(type) \
REFCOUNT_TYPE(type) \
{ \
    COUNT_TYPE count; \
    type counted; \
}; \
DEFINE_CREATE_WITH_EXTRA_SIZE(type) \
DEFINE_CREATE(type) \
DEFINE_DESTROY(type) \

#ifndef DEC_RETURN_ZERO
#error refcount_os.h does not define DEC_RETURN_ZERO
#endif // !DEC_RETURN_ZERO
#ifndef INC_REF_VAR
#error refcount_os.h does not define INC_REF_VAR
#endif // !INC_REF
#ifndef DEC_REF_VAR
#error refcount_os.h does not define DEC_REF_VAR
#endif // !DEC_REF
#ifndef INIT_REF_VAR
#error refcount_os.h does not define INIT_REF_VAR
#endif // !INIT_REF

#define INC_REF(type, var) INC_REF_VAR(((REFCOUNT_TYPE(type)*)((unsigned char*)var - offsetof(REFCOUNT_TYPE(type), counted)))->count)
#define DEC_REF(type, var) DEC_REF_VAR(((REFCOUNT_TYPE(type)*)((unsigned char*)var - offsetof(REFCOUNT_TYPE(type), counted)))->count)
#define INIT_REF(type, var) INIT_REF_VAR(((REFCOUNT_TYPE(type)*)((unsigned char*)var - offsetof(REFCOUNT_TYPE(type), counted)))->count)

#ifdef __cplusplus
}
#endif

#endif /*REFCOUNT_H*/


