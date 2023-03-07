/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_LOG_MDH_UTIL_H_
#define PW_LOG_MDH_UTIL_H_

// Colors
#define PW_LOG_MDH_MAGENTA   "\033[35m"
#define PW_LOG_MDH_YELLOW    "\033[33m"
#define PW_LOG_MDH_RED       "\033[31m"
#define PW_LOG_MDH_GREEN     "\033[32m"
#define PW_LOG_MDH_BLUE      "\033[96m"
#define PW_LOG_MDH_BLACK     "\033[30m"
#define PW_LOG_MDH_YELLOW_BG "\033[43m"
#define PW_LOG_MDH_WHITE_BG  "\033[47m"
#define PW_LOG_MDH_RED_BG    "\033[41m"
#define PW_LOG_MDH_BOLD      "\033[1m"
#define PW_LOG_MDH_RESET     "\033[0m"

// locking
void _pw_log_init_lock();
void _pw_log_lock();
void _pw_log_unlock();

// Get the filename w/o its path at compile time
#ifdef __FILE_NAME__
// Use compiler define if available
#define PW_LOG_MDH_FILE_NAME __FILE_NAME__
#else

// Compile time computation is used to find the filename.
// __FILE__ is parsed from the end to find the first occurrence of '/'
// The pointer to the character before '/' is used as the filename string.

// If the index is overflowing, returns __FILE__
// Otherwise if '/' is found at the index, returns a pointer to the character before '/'
// Otherwise, delegate to the next condition (not in this macro).
#define PW_LOG_MDH_FILE_NAME_HELPER(INDEX)                                                  \
    sizeof(__FILE__) <= INDEX                   ? __FILE__                                  \
    : __FILE__[sizeof(__FILE__) - INDEX] == '/' ? (&__FILE__[sizeof(__FILE__) - INDEX + 1]) \
                                                :

// filename (up to 40 characters long)
#define PW_LOG_MDH_FILE_NAME        \
    PW_LOG_MDH_FILE_NAME_HELPER(1)  \
    PW_LOG_MDH_FILE_NAME_HELPER(2)  \
    PW_LOG_MDH_FILE_NAME_HELPER(3)  \
    PW_LOG_MDH_FILE_NAME_HELPER(4)  \
    PW_LOG_MDH_FILE_NAME_HELPER(5)  \
    PW_LOG_MDH_FILE_NAME_HELPER(6)  \
    PW_LOG_MDH_FILE_NAME_HELPER(7)  \
    PW_LOG_MDH_FILE_NAME_HELPER(8)  \
    PW_LOG_MDH_FILE_NAME_HELPER(9)  \
    PW_LOG_MDH_FILE_NAME_HELPER(10) \
    PW_LOG_MDH_FILE_NAME_HELPER(11) \
    PW_LOG_MDH_FILE_NAME_HELPER(12) \
    PW_LOG_MDH_FILE_NAME_HELPER(13) \
    PW_LOG_MDH_FILE_NAME_HELPER(14) \
    PW_LOG_MDH_FILE_NAME_HELPER(15) \
    PW_LOG_MDH_FILE_NAME_HELPER(16) \
    PW_LOG_MDH_FILE_NAME_HELPER(17) \
    PW_LOG_MDH_FILE_NAME_HELPER(18) \
    PW_LOG_MDH_FILE_NAME_HELPER(19) \
    PW_LOG_MDH_FILE_NAME_HELPER(20) \
    PW_LOG_MDH_FILE_NAME_HELPER(21) \
    PW_LOG_MDH_FILE_NAME_HELPER(22) \
    PW_LOG_MDH_FILE_NAME_HELPER(23) \
    PW_LOG_MDH_FILE_NAME_HELPER(24) \
    PW_LOG_MDH_FILE_NAME_HELPER(25) \
    PW_LOG_MDH_FILE_NAME_HELPER(26) \
    PW_LOG_MDH_FILE_NAME_HELPER(27) \
    PW_LOG_MDH_FILE_NAME_HELPER(28) \
    PW_LOG_MDH_FILE_NAME_HELPER(29) \
    PW_LOG_MDH_FILE_NAME_HELPER(30) \
    PW_LOG_MDH_FILE_NAME_HELPER(31) \
    PW_LOG_MDH_FILE_NAME_HELPER(32) \
    PW_LOG_MDH_FILE_NAME_HELPER(33) \
    PW_LOG_MDH_FILE_NAME_HELPER(34) \
    PW_LOG_MDH_FILE_NAME_HELPER(35) \
    PW_LOG_MDH_FILE_NAME_HELPER(36) \
    PW_LOG_MDH_FILE_NAME_HELPER(37) \
    PW_LOG_MDH_FILE_NAME_HELPER(38) \
    PW_LOG_MDH_FILE_NAME_HELPER(39) \
    __FILE__
#endif // defined __FILE_NAME__

#endif /* PW_LOG_MDH_UTIL_H_ */
