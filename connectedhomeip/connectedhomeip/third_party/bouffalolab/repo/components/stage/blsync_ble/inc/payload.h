#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "endian.h"
#include <string.h>

#define PROTOCOL_TYPE_CMD      (0)
#define PROTOCOL_TYPE_DATA     (1)

#define    PRO_OK               0
#define    PRO_NOT_READY        1
#define    PRO_ERROR            2
#define    PRO_NO_MEM           3
#define    PRO_TIMEOUT          4

struct pro_pyld_func {

    int (*pfn_pyld_read)(void *p_arg, uint8_t *dst_buf,
                         uint8_t *dst_type, uint8_t *dst_len,
                         uint8_t *src_buf, uint8_t  src_len);

    int (*pfn_pyld_write)(void *p_arg, uint8_t *dst_buf,
                          uint8_t *dst_len, uint8_t src_id,
                          uint8_t *src_buf, uint8_t src_len,
                          int16_t remain_len);
};

struct payload {
    const struct pro_pyld_func *pfnc;
    void *p_drv;

#define PYLD_TYPE_CMD      (0)
#define PYLD_TYPE_DATA     (1)
    uint8_t pyld_type;
};

typedef struct payload *pyld_handle_t;

static inline int payload_write (pyld_handle_t handle, uint8_t *dst_buf,
                              uint8_t *dst_len, uint8_t src_id,
                              uint8_t *src_buf, uint8_t src_len,
                              int16_t remain_len)
{
    return handle->pfnc->pfn_pyld_write(handle->p_drv, dst_buf, dst_len,
                                        src_id, src_buf, src_len, remain_len);
}

static inline int payload_read (pyld_handle_t handle, uint8_t *dst_buf,
                                uint8_t *dst_type, uint8_t *dst_len,
                                uint8_t *src_buf, uint8_t src_len)
{
    return handle->pfnc->pfn_pyld_read(handle->p_drv, dst_buf, dst_type,
                                       dst_len, src_buf, src_len);
}

pyld_handle_t pro_payload_init (struct payload *p_pyld,
                                void *p_drv);

#ifdef __cplusplus
}
#endif

#endif
