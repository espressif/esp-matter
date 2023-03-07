/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef _ZW_BUILD_NO_H_
#define _ZW_BUILD_NO_H_

// Application Framework Build number.
// If not otherwise specified, it is the same as Z-Wave Protocol Build number
#ifndef ZAF_BUILD_NO
#define ZAF_BUILD_NO  ZW_GetProtocolBuildNumber()
#endif

#endif /* _ZW_BUILD_NO_H_ */
