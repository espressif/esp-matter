
/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _REG_IPC_APP_H_
#define _REG_IPC_APP_H_

#include "lmac_types.h"
#include "ipc_compat.h"
#include "reg_access.h"
#include "bl_os_private.h"

#ifdef CFG_CHIP_BL602
#define REG_WIFI_REG_BASE         0x44000000
#endif
#ifdef CFG_CHIP_BL808
#define REG_WIFI_REG_BASE         0x24000000
#endif
#ifdef CFG_CHIP_BL606P
#define REG_WIFI_REG_BASE         0x24000000
#endif

#define REG_IPC_APP_DECODING_MASK 0x0000007F

/**
 * @brief APP2EMB_TRIGGER register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      APP2EMB_TRIGGER   0x0
 * </pre>
 */
#define IPC_APP2EMB_TRIGGER_ADDR   0x12000000
#define IPC_APP2EMB_TRIGGER_OFFSET 0x00000000
#define IPC_APP2EMB_TRIGGER_INDEX  0x00000000
#define IPC_APP2EMB_TRIGGER_RESET  0x00000000

#ifndef __INLINE
#define __INLINE inline
#endif

static __INLINE u32 ipc_app2emb_trigger_get()
{
    return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_APP2EMB_TRIGGER_INDEX);
}

static __INLINE void ipc_app2emb_trigger_set(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_APP2EMB_TRIGGER_INDEX, value);
}

// field definitions
#define IPC_APP2EMB_TRIGGER_MASK   ((u32)0xFFFFFFFF)
#define IPC_APP2EMB_TRIGGER_LSB    0
#define IPC_APP2EMB_TRIGGER_WIDTH  ((u32)0x00000020)

#define IPC_APP2EMB_TRIGGER_RST    0x0

static __INLINE u32 ipc_app2emb_trigger_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_APP2EMB_TRIGGER_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

static __INLINE void ipc_app2emb_trigger_setf(u32 app2embtrigger)
{
    ASSERT_ERR((((u32)app2embtrigger << 0) & ~((u32)0xFFFFFFFF)) == 0);
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_APP2EMB_TRIGGER_INDEX, (u32)app2embtrigger << 0);
}

/**
 * @brief EMB2APP_RAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00    EMB2APP_RAWSTATUS   0x0
 * </pre>
 */
#define IPC_EMB2APP_RAWSTATUS_ADDR   0x12000004
#define IPC_EMB2APP_RAWSTATUS_OFFSET 0x00000004
#define IPC_EMB2APP_RAWSTATUS_INDEX  0x00000001
#define IPC_EMB2APP_RAWSTATUS_RESET  0x00000000

static __INLINE u32 ipc_emb2app_rawstatus_get()
{
    return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_RAWSTATUS_INDEX);
}

static __INLINE void ipc_emb2app_rawstatus_set(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_RAWSTATUS_INDEX, value);
}

// field definitions
#define IPC_EMB2APP_RAWSTATUS_MASK   ((u32)0xFFFFFFFF)
#define IPC_EMB2APP_RAWSTATUS_LSB    0
#define IPC_EMB2APP_RAWSTATUS_WIDTH  ((u32)0x00000020)

#define IPC_EMB2APP_RAWSTATUS_RST    0x0

static __INLINE u32 ipc_emb2app_rawstatus_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_RAWSTATUS_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief EMB2APP_ACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          EMB2APP_ACK   0x0
 * </pre>
 */
#define IPC_EMB2APP_ACK_ADDR   0x12000008
#define IPC_EMB2APP_ACK_OFFSET 0x00000008
#define IPC_EMB2APP_ACK_INDEX  0x00000002
#define IPC_EMB2APP_ACK_RESET  0x00000000

static __INLINE u32 ipc_emb2app_ack_get()
{
    return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_ACK_INDEX);
}

static __INLINE void ipc_emb2app_ack_clear(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_ACK_INDEX, value);
}

// field definitions
#define IPC_EMB2APP_ACK_MASK   ((u32)0xFFFFFFFF)
#define IPC_EMB2APP_ACK_LSB    0
#define IPC_EMB2APP_ACK_WIDTH  ((u32)0x00000020)

#define IPC_EMB2APP_ACK_RST    0x0

static __INLINE u32 ipc_emb2app_ack_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_ACK_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

static __INLINE void ipc_emb2app_ack_clearf(u32 emb2appack)
{
    ASSERT_ERR((((u32)emb2appack << 0) & ~((u32)0xFFFFFFFF)) == 0);
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_ACK_INDEX, (u32)emb2appack << 0);
}

/**
 * @brief EMB2APP_UNMASK_SET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_UNMASK   0x0
 * </pre>
 */
#define IPC_EMB2APP_UNMASK_SET_ADDR   0x1200000C
#define IPC_EMB2APP_UNMASK_SET_OFFSET 0x0000000C
#define IPC_EMB2APP_UNMASK_SET_INDEX  0x00000003
#define IPC_EMB2APP_UNMASK_SET_RESET  0x00000000

static __INLINE u32 ipc_emb2app_unmask_get()
{
    return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_SET_INDEX);
}

static __INLINE void ipc_emb2app_unmask_set(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_SET_INDEX, value);
}

// field definitions
#define IPC_EMB2APP_UNMASK_MASK   ((u32)0xFFFFFFFF)
#define IPC_EMB2APP_UNMASK_LSB    0
#define IPC_EMB2APP_UNMASK_WIDTH  ((u32)0x00000020)

#define IPC_EMB2APP_UNMASK_RST    0x0

static __INLINE u32 ipc_emb2app_unmask_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_SET_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

static __INLINE void ipc_emb2app_unmask_setf(u32 emb2appunmask)
{
    ASSERT_ERR((((u32)emb2appunmask << 0) & ~((u32)0xFFFFFFFF)) == 0);
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_SET_INDEX, (u32)emb2appunmask << 0);
}

/**
 * @brief EMB2APP_UNMASK_CLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_UNMASK   0x0
 * </pre>
 */
#define IPC_EMB2APP_UNMASK_CLEAR_ADDR   0x12000010
#define IPC_EMB2APP_UNMASK_CLEAR_OFFSET 0x00000010
#define IPC_EMB2APP_UNMASK_CLEAR_INDEX  0x00000004
#define IPC_EMB2APP_UNMASK_CLEAR_RESET  0x00000000

static __INLINE void ipc_emb2app_unmask_clear(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_CLEAR_INDEX, value);
}

// fields defined in symmetrical set/clear register
static __INLINE void ipc_emb2app_unmask_clearf(u32 emb2appunmask)
{
    ASSERT_ERR((((u32)emb2appunmask << 0) & ~((u32)0xFFFFFFFF)) == 0);
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_UNMASK_CLEAR_INDEX, (u32)emb2appunmask << 0);
}

/**
 * @brief EMB2APP_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_STATUS   0x0
 * </pre>
 */
#define IPC_EMB2APP_STATUS_ADDR   0x1200001C
#define IPC_EMB2APP_STATUS_OFFSET 0x0000001C
#define IPC_EMB2APP_STATUS_INDEX  0x00000007
#define IPC_EMB2APP_STATUS_RESET  0x00000000

static __INLINE u32 ipc_emb2app_status_get()
{
    return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_STATUS_INDEX);
}

static __INLINE void ipc_emb2app_status_set(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_EMB2APP_STATUS_INDEX, value);
}

// field definitions
#define IPC_EMB2APP_STATUS_MASK   ((u32)0xFFFFFFFF)
#define IPC_EMB2APP_STATUS_LSB    0
#define IPC_EMB2APP_STATUS_WIDTH  ((u32)0x00000020)

#define IPC_EMB2APP_STATUS_RST    0x0

static __INLINE u32 ipc_emb2app_status_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_EMB2APP_STATUS_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief APP_SIGNATURE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   ----------
 *  31:00        APP_SIGNATURE   0x0
 * </pre>
 */
#define IPC_APP_SIGNATURE_ADDR   0x12000040
#define IPC_APP_SIGNATURE_OFFSET 0x00000040
#define IPC_APP_SIGNATURE_INDEX  0x00000010
#define IPC_APP_SIGNATURE_RESET  0x00000000

static __INLINE u32 ipc_app_signature_get()
{
      return REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_APP_SIGNATURE_INDEX);
}

static __INLINE void ipc_app_signature_set(u32 value)
{
    REG_IPC_APP_WR(REG_WIFI_REG_BASE, IPC_APP_SIGNATURE_INDEX, value);
}

// field definitions
#define IPC_APP_SIGNATURE_MASK   ((u32)0xFFFFFFFF)
#define IPC_APP_SIGNATURE_LSB    0
#define IPC_APP_SIGNATURE_WIDTH  ((u32)0x00000020)

#define IPC_APP_SIGNATURE_RST    0x0

static __INLINE u32 ipc_app_signature_getf()
{
    u32 localVal = REG_IPC_APP_RD(REG_WIFI_REG_BASE, IPC_APP_SIGNATURE_INDEX);
    ASSERT_ERR((localVal & ~((u32)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}


#endif // _REG_IPC_APP_H_

