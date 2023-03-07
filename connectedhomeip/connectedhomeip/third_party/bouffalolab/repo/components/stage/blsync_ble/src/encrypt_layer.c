/** @file
 *  @brief protocol
 */
#include "encrypt_layer.h"

static int __pack_encrypt_read (void *p_drv, uint8_t pack_type,
                                uint8_t *dst_id, uint8_t *dst_buf,
                                uint8_t *dst_len, uint8_t *src_buf,
                                size_t src_lenght)
{
    struct encrypt *p_enc;
    uint8_t  enc_ctrl;
    uint32_t ctr, mac;

    p_enc = (struct encrypt *)p_drv;
    enc_ctrl  = src_buf[0];

    if (p_enc->is_head) {
        ctr      = get_cpu_le32(&src_buf[1]);
        mac      = get_cpu_le32(&src_buf[5]);
        src_buf += 9;
        src_lenght -= 9;
    } else {
        src_buf += 1;
        src_lenght -= 1;
    }

    if (pack_type == PROTOCOL_TYPE_CMD) {
        p_enc->pyld.pyld_type = PYLD_TYPE_CMD;
    } else {
        p_enc->pyld.pyld_type = PYLD_TYPE_DATA;
    }

    return payload_read(p_enc->pyld_handle, dst_buf, dst_id,
                        dst_len, src_buf, src_lenght);
}

static int __pack_encrypt_write (void *p_drv, uint8_t pack_type,
                                 uint8_t src_id, uint8_t *dst_buf,
                                 uint8_t *dst_len, uint8_t *src_buf,
                                 uint8_t src_len, int16_t remain_len)
{
    struct encrypt *p_enc;
    uint8_t  enc_ctrl = 0;
    uint32_t ctr = 0, mac = 0;

    p_enc = (struct encrypt *)p_drv;
    dst_buf[0] = enc_ctrl;

    if (p_enc->is_head) {
        put_cpu_le32(&dst_buf[1], ctr);
        put_cpu_le32(&dst_buf[5], mac);
        dst_buf += 9;
        *dst_len += 9;
        remain_len -= 9;
    } else {
        dst_buf += 1;
        *dst_len += 1;
        remain_len -= 1;
    }
    p_enc->pyld.pyld_type = PYLD_TYPE_DATA;

    return payload_write(p_enc->pyld_handle, dst_buf, dst_len,
                         src_id, src_buf, src_len, remain_len);
}

const static struct pro_enc_func __g_enc_func = {
    __pack_encrypt_read,
    __pack_encrypt_write
};

enc_handle_t pro_encrypt_init (struct encrypt *p_enc,
                               void *p_drv)
{
    p_enc->pyld_handle = pro_payload_init(&p_enc->pyld, (void *)&p_enc->pyld);
    p_enc->pfnc = &__g_enc_func;
    p_enc->p_drv = p_drv;
    return p_enc;
}
