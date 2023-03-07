/** @file
 *  @brief protocol
 */
#include "payload.h"

static int __payload_read (void *p_arg, uint8_t *dst_buf, uint8_t *dst_type,
                             uint8_t *dst_len, uint8_t *src_buf, uint8_t src_len)
{
    struct payload *p_pyld = (struct payload *)p_arg;

    if (p_pyld->pyld_type == PYLD_TYPE_CMD) {

        if (src_len != 5) {
            return -PRO_ERROR;
        }

        *dst_len = 4;
        *dst_type = src_buf[0];
        memcpy(dst_buf, &src_buf[1], *dst_len);

    } else {
        if ((src_len - 2) != src_buf[0]) {
            return -PRO_ERROR;
        }

        *dst_len  = src_buf[0];
        *dst_type = src_buf[1];
        memcpy(dst_buf, &src_buf[2], *dst_len);
    }

    return PRO_OK;
}

static int __payload_write (void *p_arg, uint8_t *dst_buf,
                              uint8_t *dst_len, uint8_t src_id,
                              uint8_t *src_buf, uint8_t src_len,
                              int16_t remain_len)
{
    struct payload *p_pyld = (struct payload *)p_arg;
    uint8_t payload_size;

    if (remain_len < 0) {
        return -PRO_ERROR;
    }

    if (p_pyld->pyld_type == PYLD_TYPE_CMD) {

        remain_len -= 1;
        payload_size = (remain_len > src_len) ? src_len : (remain_len);
        if (payload_size != 4) {
            return -PRO_ERROR;
        }

        dst_buf[0] = src_id;
        memcpy(&dst_buf[1], src_buf, 4);
        *dst_len += 5;

    } else {
        remain_len -= 2;
        payload_size = (remain_len > src_len) ? src_len : (remain_len);

        dst_buf[0] = payload_size;
        dst_buf[1] = src_id;
        memcpy(&dst_buf[2], src_buf, payload_size);
        *dst_len += (2 + payload_size);
        remain_len -= (2 + payload_size);
    }
    return payload_size;
}

const static struct pro_pyld_func __g_pyld_func = {
    __payload_read,
    __payload_write,
};

pyld_handle_t pro_payload_init (struct payload *p_pyld,
                                void *p_drv)
{
    p_pyld->pfnc = &__g_pyld_func;
    p_pyld->p_drv = p_drv;
    return p_pyld;
}
