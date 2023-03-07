/** @file cli_mem.h
 *
 *  @brief Command Line Interface meory module
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#define INBUF_SIZE    256
#define BUF_ALLOCATED 1
#define BUF_AVAILABLE 0
/** Malloc some memory from the CLI's byte pool
 */
void *cli_mem_malloc(int size);

/** Free previously allocated memory to the CLI's byte pool
 */
int cli_mem_free(char **buffer);

/** Initialize the CLI memory module
 *
 * \note This function is called in the CLI  initialization process
 * itself. Applications need not call this function.
 *
 * \param void
 * \return WM_SUCCESS on success, error code otherwise.
 */
int cli_mem_init(void);

/** Initialize the CLI memory module
 *
 * \note This function is called in the CLI  stop process
 * itself. Applications need not call this function.
 */
int cli_mem_cleanup(void);
