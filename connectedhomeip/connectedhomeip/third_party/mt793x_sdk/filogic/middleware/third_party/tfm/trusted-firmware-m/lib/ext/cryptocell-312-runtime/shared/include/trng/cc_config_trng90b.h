/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_CONFIG_TRNG90B_H
#define _CC_CONFIG_TRNG90B_H

/*
This file should be updated according to the characterization process.
*/

/*** For Startup Tests ***/
// amount of bytes for the startup test = 528 (at least 4096 bits (NIST SP 800-90B (2nd Draft) 4.3.12) = 22 EHRs = 4224 bits)
#define CC_CONFIG_TRNG90B_AMOUNT_OF_BYTES_STARTUP              528



#endif  // _CC_CONFIG_TRNG90B_H
