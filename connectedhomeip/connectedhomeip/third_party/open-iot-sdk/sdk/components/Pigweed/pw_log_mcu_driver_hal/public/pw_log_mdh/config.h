/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_LOG_BACKEND_CONFIG_H_
#define PW_LOG_BACKEND_CONFIG_H_

// Prints the name of the file that emitted the log message.
#ifndef PW_LOG_MDH_SHOW_FILENAME
#define PW_LOG_MDH_SHOW_FILENAME 0
#endif // PW_LOG_SHOW_FILENAME

// Maximum length of a line
#ifndef PW_LOG_MDH_LINE_LENGTH
#define PW_LOG_MDH_LINE_LENGTH 150
#endif // PW_LOG_MDH_LINE_LENGTH

// Module name length
#ifndef PW_LOG_MDH_MODULE_LENGTH
#define PW_LOG_MDH_MODULE_LENGTH 4
#endif // PW_LOG_MDH_MODULE_LENGTH

// Timestamp
#ifndef PW_LOG_MDH_TIMESTAMP
#define PW_LOG_MDH_TIMESTAMP 0
#endif // PW_LOG_MDH_TIMESTAMP

#endif /* PW_LOG_BACKEND_CONFIG_H_ */
