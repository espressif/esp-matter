// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_CHARPTR_H
#define UMOCKTYPES_CHARPTR_H

#ifdef __cplusplus
extern "C" {
#endif

    /* Codes_SRS_UMOCK_C_LIB_01_067: [char\* and const char\* shall be supported out of the box through a separate header, umockvalue_charptr.h.]*/
    /* Codes_SRS_UMOCK_C_LIB_01_069: [The signature shall be: ...*/
    int umocktypes_charptr_register_types(void);

    char* umocktypes_stringify_charptr(const char** value);
    int umocktypes_are_equal_charptr(const char** left, const char** right);
    int umocktypes_copy_charptr(char** destination, const char** source);
    void umocktypes_free_charptr(char** value);

    char* umocktypes_stringify_const_charptr(const char** value);
    int umocktypes_are_equal_const_charptr(const char** left, const char** right);
    int umocktypes_copy_const_charptr(const char** destination, const char** source);
    void umocktypes_free_const_charptr(const char** value);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_CHARPTR_H */
