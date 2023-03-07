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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bl_sec.h>
#include "hal_hbnram.h"

#include <cli.h>
#include <blog.h>
/*
 *
  total = 4096 byte
  magic(4) + hash128(16) + version(4) + reserve(40)
  key1(4) + reserve(4) + len1(4) + buf
  key2(4) + reserve(4) + len2(4) + buf
  ...
 *
 */

#define HBNRAM_ADDRESS     0x40010000
#define HBNRAM_SIZE        4096
#define MAGIC_SIZE         4
#define HASH128_SIZE       16
#define VERSION_SIZE       4
#define HEAD_RESERVE_SEZE  40
#define HBNRAM_DATA_ADDR   (HBNRAM_ADDRESS + MAGIC_SIZE + HASH128_SIZE)
#define HBNRAM_DATA_SIZE   HBNRAM_SIZE - MAGIC_SIZE - HASH128_SIZE
#define HBNRAM_HEAD_SIZE   (HASH128_SIZE + VERSION_SIZE + HEAD_RESERVE_SEZE + MAGIC_SIZE)
#define KEY_SEZE           4
#define BUF_RESERVE_SIZE   4
#define LEN_SIZE           4
#define BUF_HEAD_SIZE      (KEY_SEZE + BUF_RESERVE_SIZE + LEN_SIZE)
#define SHA_DATA_SIZE      32

#define MAGIC_NUM          0x12345678

static uint8_t *mem_fourbytes_copy(uint8_t *dst, const uint8_t *src, int len)
{
    uint32_t *pfdst, *pfsrc, flen;

    pfdst = (uint32_t *)dst;
    pfsrc = (uint32_t *)src;
    flen = len / 4;

    while (flen > 0) {
        *pfdst = *pfsrc;
        pfdst++;
        pfsrc++;
        flen--;
    }

    return dst;
}

static uint8_t *memset_fourbytes(uint8_t *src, int n, int len)
{
    uint32_t *fpsrc;
    int flen;
    uint32_t data = 0;
    uint32_t num = 0;

    num = n & 0xff;
    data = num | (num << 8) | (num << 16) | (num << 24);
    flen = len / 4;
    fpsrc = (uint32_t *)src;
    while (flen > 0) {
        *fpsrc = data;
        fpsrc++;
        flen--;
    }

    return src;
}

static int sha_check_withctx(uint8_t *input, uint8_t *output, bl_sha_type_t shaType, uint32_t data_size, uint32_t length)
{
    uint32_t i = 0;
    uint8_t *pallc;
    uint32_t count;
    uint32_t remain;

    count = length / data_size;
    remain = length % data_size;
    bl_sha_ctx_t shaCtx;
    bl_sha_mutex_take();
    bl_sha_init(&shaCtx, shaType);

    pallc = pvPortMalloc(data_size);
    for (i = 0; i < count; i++) {
        mem_fourbytes_copy(pallc, input + i * data_size, data_size);
        bl_sha_update(&shaCtx, pallc, data_size);
    }

    if (remain != 0) {
        memset(pallc, 0, data_size);
        mem_fourbytes_copy(pallc, input + i * data_size, data_size);
        bl_sha_update(&shaCtx, pallc, remain);
    }

    if (bl_sha_finish(&shaCtx, output) != 0) {
    	blog_error("Sec_Eng_SHA256_Finish error \r\n");
    }
    bl_sha_mutex_give();

    vPortFree(pallc);

    return 0;
}

static uint8_t *get_avaible_area(void)
{
    uint8_t *phbn_addr;
    uint32_t key_val;

    phbn_addr = (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_HEAD_SIZE);

    while (phbn_addr < (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE)) {
        key_val = *(uint32_t *)phbn_addr;
        if (key_val == 0) {
            return phbn_addr;
        }

        phbn_addr = phbn_addr + BUF_HEAD_SIZE + *(uint32_t *)(phbn_addr + KEY_SEZE + BUF_RESERVE_SIZE);
    }

    return NULL;
}

static uint8_t *find_addr_by_key(uint8_t *key)
{
    uint8_t *paddr;
    uint32_t val;
    uint32_t len;

    paddr = (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_HEAD_SIZE);
    while (paddr < (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE)) {
        val = *(uint32_t *)paddr;
        if (val == *(uint32_t *)key) {
            return paddr;
        }

        len = *(uint32_t *)(paddr + KEY_SEZE + BUF_RESERVE_SIZE);
        paddr = paddr + BUF_HEAD_SIZE + len;
    }

    return NULL;
}

void print_mem_map(void)
{
    uint8_t *paddr;
    uint32_t val;
    uint32_t len;
    char keybuf[5];
    
    blog_info("|head-64bytes|\r\n");
    memset(keybuf, 0, 5);
    paddr = (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_HEAD_SIZE);
    while (paddr < (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE)) {
        val = *(uint32_t *)paddr;
        if (val != 0) {
            mem_fourbytes_copy((uint8_t*)keybuf, paddr, 4);
            len = *(uint32_t *)(paddr + KEY_SEZE + BUF_RESERVE_SIZE);
            blog_info("|%s:head-12bytes data-%ldbytes|\r\n", keybuf, len);
        } else {
            blog_info("|unused-%ldbytes|\r\n", (uint32_t)((uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE) - paddr));
            return ;
        }

        paddr = paddr + BUF_HEAD_SIZE + len;
    }

    return ;
}

int hal_hbnram_init(void)
{
    uint8_t output[32];
    uint8_t hash128[16];
    uint8_t calc_hash[16];
    uint32_t magic_val;
    int flag;

    magic_val = *(uint32_t *)HBNRAM_ADDRESS;
    if (MAGIC_NUM != magic_val) {
        blog_error("magic is not right ,recalculate.\r\n");
        memset_fourbytes((uint8_t *)HBNRAM_ADDRESS, 0, HBNRAM_SIZE);
        *(uint32_t *)HBNRAM_ADDRESS = (uint32_t)MAGIC_NUM;
        sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
        mem_fourbytes_copy((uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), output, HASH128_SIZE);
        return -1;
    } else {
        mem_fourbytes_copy(hash128, (uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), HASH128_SIZE);
        sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
        mem_fourbytes_copy(calc_hash, output, HASH128_SIZE);
        flag = memcmp(calc_hash, hash128, HASH128_SIZE);
        if (flag == 0) {
            blog_info("hash check success \r\n");
        } else {
            blog_error("hash check failed, memset mem \r\n");
            memset_fourbytes((uint8_t *)HBNRAM_ADDRESS, 0, HBNRAM_SIZE);
            sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
            mem_fourbytes_copy((uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), output, HASH128_SIZE);
            return -1;
        }
    }

    return 0;
}

int hal_hbnram_deinit(void)
{
    memset_fourbytes((uint8_t *)HBNRAM_ADDRESS, 0, HBNRAM_SIZE);

    return 0;
}

int hal_hbnram_alloc(const char *key, int len)
{
    uint8_t *paddr;
    uint8_t output[32];
    int left_size;
    uint32_t alen;
    uint8_t keybuf[4];

    if ((strlen(key) > KEY_SEZE) || (len <= 0)) {
        blog_error("illegal para. \r\n");
        return -1;
    }

    paddr = get_avaible_area();
    if (paddr == NULL) {
        blog_error("do not have area for alloc. \r\n");
        return -1;
    }

    if (len % 4 != 0) {
        alen = (4 - len % 4) + len;
    } else {
        alen = len;
    }

    if (paddr + alen + BUF_HEAD_SIZE > (uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE)) {
        left_size = (uint32_t)((uint8_t *)(HBNRAM_ADDRESS + HBNRAM_SIZE) - (uint8_t *)(paddr + BUF_HEAD_SIZE));
        if (left_size < 0) {
            blog_error("not have enough area for alloc \r\n");
        } else {
            blog_error("only left %d bytes, not enough for alloc \r\n", left_size);
        }

        return -1;
    }

    memset(keybuf, 0, sizeof(keybuf));
    memcpy(keybuf, (uint8_t *)key, strlen(key));
    mem_fourbytes_copy(paddr, keybuf, KEY_SEZE);
    mem_fourbytes_copy((uint8_t *)(paddr + KEY_SEZE + BUF_RESERVE_SIZE), (uint8_t *)&alen, LEN_SIZE);
    sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
    mem_fourbytes_copy((uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), output, HASH128_SIZE);

    return 0;
}

int hal_hbnram_buffer_set(const char *key, uint8_t *buf, int length)
{
    uint8_t *phead;
    uint32_t area_size = 0;
    uint8_t output[32];
    uint8_t tmpbuf[4];
    uint32_t count;
    uint8_t keybuf[4];

    if (key == NULL || buf == NULL || length <= 0) {
        blog_error("illegal para \r\n");

        return -1;
    }

    if (strlen(key) > 4) {
        blog_error("key oversize \r\n");
        return -1;
    }

    memset(keybuf, 0, sizeof(keybuf));
    memcpy(keybuf, (uint8_t *)key, strlen(key));
    phead = find_addr_by_key(keybuf);
    if (phead == NULL) {
        blog_error("not correct key, return \r\n");

        return -1;
    }

    area_size = *(uint32_t *)(phead + KEY_SEZE + BUF_RESERVE_SIZE);

    if (length > area_size) {
        blog_error("length is too big than alloc size\r\n");
        return -1;
    } 

    if (length % 4 != 0) {
        count = length % 4;
        if (length > 4) {    
            mem_fourbytes_copy(phead + BUF_HEAD_SIZE, buf, length - count);
            memcpy(tmpbuf, buf + length - count, count);
            mem_fourbytes_copy(phead + BUF_HEAD_SIZE + length - count, tmpbuf, 4);
        } else {
            memcpy(tmpbuf, buf, count);
            mem_fourbytes_copy(phead + BUF_HEAD_SIZE, tmpbuf, 4);
       }
    } else {
       mem_fourbytes_copy(phead + BUF_HEAD_SIZE, buf, length);
    }
  
    sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
    mem_fourbytes_copy((uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), output, HASH128_SIZE);

    return length;
}

int hal_hbnram_buffer_get(const char *key, uint8_t *buf, int length)
{
    uint8_t *phead;
    uint32_t area_size = 0;
    uint32_t count;
    uint8_t tmpbuf[4];
    uint8_t keybuf[4];

    if (buf == NULL || key == NULL || length <=0) {
        blog_error("illegal para \r\n");
        return -1;
    }

    if (strlen(key) > 4) {
        blog_error("key oversize \r\n");
    }

    memset(keybuf, 0, sizeof(keybuf));
    memcpy(keybuf, (uint8_t *)key, strlen(key));
    phead = find_addr_by_key(keybuf);
    if (phead == NULL) {
        blog_error("key not correct, return \r\n");
        return -1;
    }

    area_size = *(uint32_t *)(phead + KEY_SEZE + BUF_RESERVE_SIZE);
    if (length > area_size) {
        blog_error("length is big than alloc size \r\n");
        return -1;
    }

    if (length % 4 != 0) {
        count = length % 4;
        if (length > 4) {
            mem_fourbytes_copy(buf, phead + BUF_HEAD_SIZE, length - count);
            mem_fourbytes_copy(tmpbuf, phead + BUF_HEAD_SIZE + length - count, 4);
            memcpy(buf + length - count, tmpbuf, count);
        } else {
            mem_fourbytes_copy(tmpbuf, phead + BUF_HEAD_SIZE, 4);
            memcpy(buf, tmpbuf, count);
        }
    } else {
        mem_fourbytes_copy(buf, phead + BUF_HEAD_SIZE, length);
    }

    return length;
}

int hal_hbnram_handle_get_fromkey(const char *key, hbnram_handle_t *handle)
{
    uint8_t *phead;
    uint8_t keybuf[4];

    if (key == NULL || handle == NULL) {
        blog_error("invalid handle pointer \r\n");

        return -1;
    }

    if (strlen(key) > 4) {
        blog_error("key oversize \r\n");
    }

    memset(keybuf, 0, sizeof(keybuf));
    memcpy(keybuf, (uint8_t *)key, strlen(key));
    phead = find_addr_by_key(keybuf);
    if (phead == NULL) {
        blog_error("invalid key \r\n");
        return -1;
    }
    handle->block_addr = phead;
    handle->size = *(uint32_t *)(phead + KEY_SEZE + BUF_RESERVE_SIZE);
    handle->write_idex = 0;
    handle->read_idex = 0;

    return 0;
}

int hal_hbnram_copy_from_stream(hbnram_handle_t *handle, uint8_t *buf, int len)
{
    uint8_t headbuf[4] = {0};
    uint8_t tailbuf[4] = {0};
    int head_count;
    int tail_count;
    int count;
    int tmplen;

    if (buf == NULL || handle == NULL) {
        blog_error("invalid pointer \r\n");
        return -1;
    }

    if (len <= 0) {
        blog_error("len == 0 \r\n");
        return -1;
    }

    if (len + handle->read_idex > handle->size) {
        blog_error("len oversize \r\n");
        return -1;
    }

    head_count = handle->read_idex % 4;
    if (head_count == 0) {
        tail_count = len % 4;
        if (tail_count == 0) {
            mem_fourbytes_copy(buf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex, len);
        } else {
            count = len / 4;
            if (count > 0) {
                mem_fourbytes_copy(buf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex, len - tail_count);
                mem_fourbytes_copy(tailbuf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex + len - tail_count, 4);
                memcpy(buf + len - tail_count, tailbuf, tail_count);
            } else {
                mem_fourbytes_copy(tailbuf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex, 4);
                memcpy(buf, tailbuf, len);
            }
        }
    } else {
        mem_fourbytes_copy(headbuf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex - head_count, 4);
        memcpy(buf, headbuf + head_count, (4 - head_count) < len ? (4 - head_count) : len);
        tmplen = len - (4 - head_count);
        if (tmplen > 0) {
            tail_count = tmplen % 4;
            if (tail_count == 0) {
                mem_fourbytes_copy(buf + 4 - head_count, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex - head_count + 4, tmplen);
            } else {
                count = tmplen / 4;
                if (count > 0) {
                    mem_fourbytes_copy(buf + 4 - head_count, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex - head_count + 4, tmplen - tail_count);
                    mem_fourbytes_copy(tailbuf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex - head_count + 4 + tmplen - tail_count, 4);
                    memcpy(buf + 4 - head_count + tmplen - tail_count, tailbuf, tail_count);

                } else {
                    mem_fourbytes_copy(tailbuf, handle->block_addr + BUF_HEAD_SIZE + handle->read_idex - head_count + 4, 4);
                    memcpy(buf + 4 - head_count, tailbuf, tail_count);
                }
            }
        }
    }

    handle->read_idex = handle->read_idex + len;

    return 0;
}

int hal_hbnram_copy_to_stream(hbnram_handle_t *handle, uint8_t *buf, int len)
{
    int head_count;
    int tail_count;
    int count;
    int tmplen;
    uint8_t headbuf[4] = {0};
    uint8_t tailbuf[4] = {0};
    uint8_t output[32];

    if (buf == NULL || handle == NULL) {
        blog_error("invalid  pointer. \r\n");
    }

    if (len + handle->write_idex > handle->size) {
        blog_error("len oversize \r\n");
    }

    if (len <= 0) {
        blog_error("len <= 0 , return \r\n");
        return -1;
    }

    //idex : real position
    if (handle->write_idex % 4 != 0) {
        head_count = handle->write_idex % 4;
        mem_fourbytes_copy(headbuf, handle->block_addr + BUF_HEAD_SIZE + handle->write_idex - head_count, 4);
        if (len >= 4 - head_count) {
            memcpy(headbuf + head_count, buf, 4 - head_count);
        } else {
            memcpy(headbuf + head_count, buf, len);
        }

        mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex - head_count, headbuf, 4);
        tmplen = len - (4 - head_count);
        if (tmplen >= 0) {
            if ((tmplen % 4 == 0)) {
                mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex - head_count + 4, buf + 4 - head_count, tmplen);
            } else {
                count = tmplen / 4;
                tail_count = tmplen % 4;
                if (count == 0) {
                    memcpy(tailbuf, buf  + 4 - head_count, tmplen);
                    mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex + 4 - head_count, tailbuf, 4);
                } else {
                    mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex - head_count + 4, buf + 4 - head_count, tmplen - tail_count);
                    memcpy(tailbuf, buf + 4 - head_count + tmplen - tail_count, tail_count);
                    mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex - head_count + 4 + tmplen - tail_count, tailbuf, 4);
                }
            }
        }
    } else {
        if (len % 4 == 0) {
            mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex, buf, len);
        } else {
            count = len / 4;
            tail_count = len % 4;
            tmplen = len - tail_count;
            if (count == 0) {
                memcpy(tailbuf, buf, tail_count);
                mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex, tailbuf, 4);
            } else {
                mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex, buf, tmplen);
                memcpy(tailbuf, buf + tmplen, tail_count);
                mem_fourbytes_copy(handle->block_addr + BUF_HEAD_SIZE + handle->write_idex + tmplen, tailbuf, 4);
            }
        }
    }

    handle->write_idex = handle->write_idex + len;
    sha_check_withctx((uint8_t*)HBNRAM_DATA_ADDR, output, BL_SHA256, SHA_DATA_SIZE, HBNRAM_DATA_SIZE);
    mem_fourbytes_copy((uint8_t *)(HBNRAM_ADDRESS + MAGIC_SIZE), output, HASH128_SIZE);

    return 0;
}

