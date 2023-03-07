/** @file
 *  @brief protocol
 */
#include "transfer.h"
#include "encrypt_layer.h"
#include <blog.h>

#define __PRO_FRAG_COUNTS_GET(frag_ctrl) \
    BITS_GET(frag_ctrl, 0, PACK_FRAG_END_BIT)

static int __protocol_ack (pro_handle_t handle,
                             struct general_head *p_gen,
                             uint8_t ack_code,
                             uint8_t *ack_buf,
                             uint8_t len);

static void __free (void *ptr)
{
    if (ptr) {
        blog_info("free %p\r\n", ptr);
    }
    vPortFree(ptr);
}

static void *__malloc (size_t size)
{
    void *ptr = NULL;
    ptr = pvPortMalloc(size);
    if (ptr) {
        blog_info("malloc %p\r\n", ptr);
    }
    return ptr;
}

static void __clear_dev (pro_handle_t handle)
{
    handle->tol_len_now  = 0;
    handle->total_length = 0;
    __free(handle->pyld_buf);
    handle->pyld_buf = NULL;
}

static int __pack_trans_end (struct general_head *p_gen)
{
    return ((!BIT_GET(p_gen->ctrl, PRO_CTRL_FRAG_BIT)) ||
            BIT_GET(p_gen->frag_ctrl, PACK_FRAG_END_BIT));
}

static int __pack_trans (pro_handle_t handle,
                           struct general_head *p_general,
                           const void *buf, size_t bytes)
{
    struct pro_event ev;
    uint8_t *src_buf;
    uint8_t  src_len;
    uint8_t  dst_len;
    uint8_t  ev_id;
    uint8_t  pack_type;
    int      ret;

    src_buf = (uint8_t *)buf;
    pack_type = BITS_GET(p_general->ctrl, PRO_CTRL_TYPE_BIT, 2);

    if (__PRO_FRAG_COUNTS_GET(p_general->frag_ctrl) == 0) {

        handle->total_length = get_cpu_le16(src_buf);
        src_len  = src_buf[2];
        src_buf += 3;

        if (bytes != src_len + 3) {
            ret = -PRO_ERROR;
            goto __end;
        }

        if (handle->pyld_buf != NULL) {
            __clear_dev(handle);
        }

        if (handle->total_length != 0) {
            handle->pyld_buf = __malloc(handle->total_length);
            if (handle->pyld_buf == NULL) {
                ret = -PRO_NO_MEM;
                goto __end;
            }
        }
        encrypt_layer_is_head(handle->enc_handle, 1);
    } else {

        src_len  = src_buf[0];
        src_buf += 1;

        if (bytes != src_len + 1) {
            ret = -PRO_ERROR;
            goto __end;
        }
        encrypt_layer_is_head(handle->enc_handle, 0);
    }
    ret = encrypt_layer_read(handle->enc_handle, pack_type, &ev_id,
                              handle->pyld_buf + handle->tol_len_now,
                              &dst_len, src_buf, src_len);

    if (ret != PRO_OK) {
        __clear_dev(handle);
        goto __end;
    }
    handle->tol_len_now += dst_len;

    /* if pack end*/
    if (__pack_trans_end(p_general)) {

        if (handle->tol_len_now != handle->total_length) {
            __clear_dev(handle);
            ret = -PRO_ERROR;
            goto __end;
        }
        ev.type = pack_type;
        ev.event_id = ev_id;
        ev.p_buf = handle->pyld_buf;
        ev.length = handle->tol_len_now;

        if (handle->p_func->pfn_recv_event) {
            handle->ack_buf = __malloc(PRO_CONFIG_BUF_SIZE);
            if (handle->ack_buf == NULL) {
                ret = -PRO_NO_MEM;
                goto __end;
            }
            ret = handle->p_func->pfn_recv_event(handle->p_drv, &ev);
        }

        __clear_dev(handle);
    }

__end:

    /* if this pack is not ack */
    if (BIT_GET(p_general->ctrl, PRO_CTRL_ACK_BIT) == 0) {
        __protocol_ack(handle, p_general, ret,
                       handle->ack_buf, handle->ack_len);
    }
    handle->ack_len = 0;
    __free(handle->ack_buf);
    handle->ack_buf = NULL;

    return ret;
}

static int __frag_section (pro_handle_t handle,
                             struct general_head *p_general,
                             uint8_t *p_buf,
                             size_t bytes)
{
    p_general->frag_ctrl = get_cpu_le16(p_buf);
    p_buf += 2;
    bytes -= 2;

    return __pack_trans(handle, p_general, p_buf, bytes);
}

static int __seq_timeout (pro_handle_t handle, uint32_t timeout_ms)
{
    if (handle->seq_start_ms == 0) {
        return 0;
    }
    return ((xTaskGetTickCount() - handle->seq_start_ms) > timeout_ms) ? 1 : 0;
}

static int __protocol_send (pro_handle_t handle, uint8_t seq,
                              uint8_t ack, uint8_t type,
                              const void *p_data, uint16_t length)
{
    struct general_head head;
    uint8_t  head_len;
    uint8_t  frag_end;
    uint8_t *p_buf;
    uint8_t *dst_buf;
    uint8_t  dst_len;
    uint8_t  payload_size;
    uint16_t tol_len_now;
    int16_t  mtu_remain;
    int      ret;

    head.ctrl = 0;
    head.seq = seq;
    head.frag_ctrl = 0;
    BIT_MODIFY(head.ctrl, PRO_CTRL_RETRY_BIT, PRO_NOT_RETRY);
    BIT_MODIFY(head.ctrl, PRO_CTRL_PORTE_BIT, PRO_NOT_PORTECTE);
    BIT_MODIFY(head.ctrl, PRO_CTRL_ACK_BIT, ack);
    BITS_SET(head.ctrl, PRO_CTRL_TYPE_BIT, 2, PRO_TYPE_DATA);
    BITS_SET(head.ctrl, PRO_CTRL_VERSION_BIT, 2, PRO_VERSION00);

    frag_end = 0;
    tol_len_now = 0;

    p_buf = __malloc(handle->mtu);
    if (p_buf == NULL) {
        return -PRO_NO_MEM;
    }

    while (!frag_end) {

        mtu_remain = handle->mtu;
        dst_buf = p_buf;
        dst_len = 0;
        head_len = 0;

        /* 2 bytes frag_ctrl */

        dst_buf += 4;
        dst_len += 4;
        mtu_remain -= 4;
        head_len += 4;

        /* 2 bytes total length */
        if (tol_len_now == 0) {
            put_cpu_le16(dst_buf, length);
            dst_buf += 2;
            dst_len += 2;
            mtu_remain -= 2;
            encrypt_layer_is_head(handle->enc_handle, 1);
            head_len += 2;
        }

        /* 1 bytes length */
        dst_buf += 1;
        dst_len += 1;
        mtu_remain -= 1;
        head_len += 1;

        ret = encrypt_layer_write(handle->enc_handle, PRO_TYPE_DATA,
                                  type, dst_buf, &dst_len,
                                  (uint8_t *)p_data + tol_len_now,
                                  length - tol_len_now, mtu_remain);
        encrypt_layer_is_head(handle->enc_handle, 0);

        if (ret < PRO_OK) {
            goto __end;
        }
        payload_size = ret;
        tol_len_now += payload_size;

        if (tol_len_now < length) {

        } else if (tol_len_now == length) {
            BIT_SET(head.frag_ctrl, PACK_FRAG_END_BIT);
            frag_end = 1;
        } else {
            ret = -PRO_ERROR;
            goto __end;
        }

        if (payload_size < length) {
            BIT_SET(head.ctrl, PRO_CTRL_FRAG_BIT);
        }

        memcpy(p_buf, &head, 2);
        put_cpu_le16(&p_buf[2], head.frag_ctrl);
        *(dst_buf - 1) = dst_len - head_len;

        handle->p_func->pfu_bytes_send(NULL, p_buf, dst_len);

        head.seq++;
        head.frag_ctrl++;
        if (xSemaphoreTake(handle->xSemaphore, PRO_CONFIG_TIMEOUT) != pdTRUE) {
            ret = -PRO_TIMEOUT;
            goto __end;
        }
    }

__end:
    __free(p_buf);
    return ret;
}

static void __seq_update (pro_handle_t handle, uint8_t new_seq)
{
    handle->seq_start_ms = xTaskGetTickCount();
    handle->old_seq = new_seq;
}

static int __protocol_ack (pro_handle_t handle,
                             struct general_head *p_gen,
                             uint8_t ack_code,
                             uint8_t *ack_data,
                             uint8_t len)
{
    return __protocol_send(handle,
                           p_gen->seq,
                           PRO_ACK_PACK,
                           ack_code,
                           ack_data,
                           len);
}

int pro_trans_write (pro_handle_t handle,
                     const void *p_data,
                     uint16_t length)
{
    return __protocol_send(handle, 0, PRO_NOT_ACK_PACK, 0,
                           p_data, length);
}

int pro_trans_read (pro_handle_t handle, const void *buf,
                     size_t bytes, uint16_t mtu)
{
    struct general_head head;
    uint8_t *p_buf;
    int    ret;

    if (NULL == handle || NULL == buf) {
        return -PRO_ERROR;
    }

    handle->mtu = mtu;

    p_buf = (uint8_t *)buf;
    memcpy(&head, buf, 2);

    if (BIT_GET(head.ctrl, PRO_CTRL_RETRY_BIT)) {
        /* data update */
        if (head.seq != handle->old_seq) {
            __seq_update(handle, head.seq);
        }
        if (__seq_timeout(handle, PRO_CONFIG_TIMEOUT)) {
            __clear_dev(handle);
            blog_info("-----------timeout-----------\r\n");
            return -PRO_TIMEOUT;
        }
    }

    p_buf += 2;
    bytes -= 2;

    ret = __frag_section(handle, &head, p_buf, bytes);

    return ret;
}

int pro_trans_ack (pro_handle_t handle)
{
    if (xSemaphoreGive(handle->xSemaphore) != pdTRUE) {
        return PRO_ERROR;
    }
    return PRO_OK;
}

int pro_trans_layer_ack_read (pro_handle_t handle,
                                const void *ack_buf,
                                size_t bytes)
{
    if ((!handle) || (!ack_buf)) {
        return -PRO_ERROR;
    }
    if (bytes > PRO_CONFIG_BUF_SIZE) {
        return -PRO_ERROR;
    }
    if (handle->ack_buf == NULL) {
        return -PRO_NO_MEM;
    }
    memcpy(handle->ack_buf, ack_buf, bytes);
    handle->ack_len = bytes;
    return PRO_OK;
}

pro_handle_t pro_trans_init (struct pro_dev  *p_dev,
                             const struct pro_func *p_func,
                             void *p_drv)
{
    memset(p_dev, 0, sizeof(struct pro_dev));
    p_dev->enc_handle = pro_encrypt_init(&p_dev->enc, (void *)&p_dev->enc);
    p_dev->p_func = p_func;
    p_dev->p_drv  = p_drv;
    p_dev->xSemaphore = xSemaphoreCreateBinaryStatic(&p_dev->xSemaphoreBuffer);
    return p_dev;
}

void pro_trans_reset (pro_handle_t handle)
{
    __clear_dev(handle);
}
