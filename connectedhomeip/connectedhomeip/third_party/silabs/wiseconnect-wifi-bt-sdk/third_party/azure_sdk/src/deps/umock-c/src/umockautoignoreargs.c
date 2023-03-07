// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umockalloc.h"
#include "umock_c/umockautoignoreargs.h"
#include "umock_c/umock_log.h"

static const char ignore_ptr_arg_string[] = "IGNORED_PTR_ARG";
static const char ignore_num_arg_string[] = "IGNORED_NUM_ARG";

#define PARSER_STACK_DEPTH 256

/* Codes_SRS_UMOCKAUTOIGNOREARGS_01_001: [ umockautoignoreargs_is_call_argument_ignored shall determine whether argument argument_index shall be ignored or not. ]*/
int umockautoignoreargs_is_call_argument_ignored(const char* call, size_t argument_index, int* is_argument_ignored)
{
    int result;

    if (
        (call == NULL) ||
        (is_argument_ignored == NULL)
        )
    {
        /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_002: [ If call or is_argument_ignored is NULL, umockautoignoreargs_is_call_argument_ignored shall fail and return a non-zero value. ]*/
        result = MU_FAILURE;
    }
    else
    {
        // find where the args start
        size_t call_string_len = strlen(call);
        size_t i = call_string_len;
        size_t paren_scope_count = 0;
        bool is_error = false;

        /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_010: [ umockautoignoreargs_is_call_argument_ignored shall look for the arguments as being the string contained in the scope of the rightmost parenthesis set in call. ]*/
        do
        {
            i--;
            
            if (call[i] == ')')
            {
                paren_scope_count++;
            }
            else if (call[i] == '(')
            {
                if (paren_scope_count == 0)
                {
                    // error
                    is_error = true;
                    break;
                }
                else
                {
                    paren_scope_count--;
                    if (paren_scope_count == 0)
                    {
                        // found the start of the args
                        break;
                    }
                }
            }
        } while (i > 0);

        if (
            (is_error) ||
            /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_011: [ If a valid scope of the rightmost parenthesis set cannot be formed (imbalanced parenthesis for example), `umockautoignoreargs_is_call_argument_ignored` shall fail and return a non-zero value. ]*/
            (paren_scope_count > 0)
            )
        {
            UMOCK_LOG("Invalid call string: %s", call);
            result = MU_FAILURE;
        }
        else
        {
            const char* cur_pos = &call[i];

            if (cur_pos == NULL)
            {
                /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_004: [ If umockautoignoreargs_is_call_argument_ignored fails parsing the call argument it shall fail and return a non-zero value. ]*/
                result = MU_FAILURE;
            }
            else
            {
                size_t argument_count = 1;
                unsigned char argument_found = 0;

                /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_009: [ If the number of arguments parsed from call is less than argument_index, umockautoignoreargs_is_call_argument_ignored shall fail and return a non-zero value. ]*/
                cur_pos++;

                while (*cur_pos != '\0')
                {
                    /* clear out all spaces */
                    while ((*cur_pos != '\0') && isspace(*cur_pos))
                    {
                        cur_pos++;
                    }

                    if (*cur_pos != '\0')
                    {
                        if (*cur_pos == ')')
                        {
                            /* no more args and we did not get to the arg we wanted */
                            /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_009: [ If the number of arguments parsed from call is less than argument_index, umockautoignoreargs_is_call_argument_ignored shall fail and return a non-zero value. ]*/
                            result = MU_FAILURE;
                            break;
                        }
                        else
                        {
                            if (argument_count == argument_index)
                            {
                                if (
                                    /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_006: [ If the argument value is IGNORED_PTR_ARG then is_argument_ignored shall be set to 1. ]*/
                                    (strncmp(cur_pos, ignore_ptr_arg_string, sizeof(ignore_ptr_arg_string) - 1) == 0) ||
                                    /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_007: [ If the argument value is IGNORED_NUM_ARG then is_argument_ignored shall be set to 1. ]*/
                                    (strncmp(cur_pos, ignore_num_arg_string, sizeof(ignore_num_arg_string) - 1) == 0)
                                    )
                                {
                                    /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_005: [ If umockautoignoreargs_is_call_argument_ignored was able to parse the argument_indexth argument it shall succeed and return 0, while writing whether the argument is ignored in the is_argument_ignored output argument. ]*/
                                    *is_argument_ignored = 1;
                                }
                                else
                                {
                                    /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_008: [ If the argument value is any other value then is_argument_ignored shall be set to 0. ]*/
                                    /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_005: [ If umockautoignoreargs_is_call_argument_ignored was able to parse the argument_indexth argument it shall succeed and return 0, while writing whether the argument is ignored in the is_argument_ignored output argument. ]*/
                                    *is_argument_ignored = 0;
                                }

                                argument_found = 1;
                                break;
                            }
                            else
                            {
                                unsigned char parser_stack[PARSER_STACK_DEPTH];
                                size_t parser_stack_index = 0;
                                int parse_error = 0;

                                /* Codes_SRS_UMOCKAUTOIGNOREARGS_01_003: [ umockautoignoreargs_is_call_argument_ignored shall parse the call string as a function call: function_name(arg1, arg2, ...). ]*/
                                while (
                                    (*cur_pos != '\0') &&
                                    ((*cur_pos != ',') || (parser_stack_index > 0))
                                    )
                                {
                                    if (*cur_pos == '(')
                                    {
                                        if (parser_stack_index == PARSER_STACK_DEPTH)
                                        {
                                            /* error*/
                                            parse_error = 1;
                                            break;
                                        }
                                        else
                                        {
                                            parser_stack[parser_stack_index] = 1;
                                            parser_stack_index++;
                                        }
                                    }
                                    if (*cur_pos == ')')
                                    {
                                        if (parser_stack_index == 0)
                                        {
                                            /* error*/
                                            parse_error = 1;
                                            break;
                                        }
                                        else
                                        {
                                            if (parser_stack[parser_stack_index - 1] == 1)
                                            {
                                                parser_stack_index--;
                                            }
                                        }
                                    }
                                    if (*cur_pos == '{')
                                    {
                                        if (parser_stack_index == PARSER_STACK_DEPTH)
                                        {
                                            /* error*/
                                            parse_error = 1;
                                            break;
                                        }
                                        else
                                        {
                                            parser_stack[parser_stack_index] = 2;
                                            parser_stack_index++;
                                        }
                                    }
                                    if (*cur_pos == '}')
                                    {
                                        if (parser_stack_index == 0)
                                        {
                                            /* error*/
                                            parse_error = 1;
                                            break;
                                        }
                                        else
                                        {
                                            if (parser_stack[parser_stack_index - 1] == 2)
                                            {
                                                parser_stack_index--;
                                            }
                                        }
                                    }

                                    cur_pos++;
                                }

                                if (
                                    (*cur_pos == '\0') ||
                                    parse_error
                                    )
                                {
                                    break;
                                }
                                else
                                {
                                    argument_count++;
                                    cur_pos++;
                                }
                            }
                        }
                    }
                }

                if (argument_found)
                {
                    result = 0;
                }
                else
                {
                    result = MU_FAILURE;
                }
            }
        }
    }

    return result;
}
