/** @file
 *  @brief protocol
 */
#ifndef __ENCRYPT_STOREY_H__
#define __ENCRYPT_STOREY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "payload.h"

struct pro_enc_func {

    int (*pfn_enc_read)(void *p_drv, uint8_t pack_type, uint8_t *dst_id,
                        uint8_t *dst_buf, uint8_t *dst_len, uint8_t *src_buf,
                        size_t src_lenght);

    int (*pfn_enc_write)(void *p_drv, uint8_t pack_type, uint8_t type_id,
                        uint8_t *dst_buf, uint8_t *dst_len, uint8_t *src_buf,
                        uint8_t src_len, int16_t remain_len);
};

struct encrypt {
    pyld_handle_t pyld_handle;
    struct payload pyld;
    uint8_t is_head;
    const struct pro_enc_func *pfnc;
    void *p_drv;
};

typedef struct encrypt *enc_handle_t;

static inline int encrypt_layer_write (enc_handle_t enc, uint8_t pack_type,
                                       uint8_t type_id, uint8_t *dst_buf,
                                       uint8_t *dst_len, uint8_t *src_buf,
                                       uint8_t src_len, int16_t remain_len)
{
    return enc->pfnc->pfn_enc_write(enc->p_drv, pack_type, type_id,
                                    dst_buf, dst_len, src_buf,
                                    src_len, remain_len);
}

static inline int encrypt_layer_read (enc_handle_t enc, uint8_t pack_type,
                                       uint8_t *dst_id, uint8_t *dst_buf,
                                       uint8_t *dst_len, uint8_t *src_buf,
                                       size_t src_lenght)
{
    return enc->pfnc->pfn_enc_read(enc->p_drv, pack_type, dst_id,
                                   dst_buf, dst_len, src_buf,
                                   src_lenght);
}

static inline void encrypt_layer_is_head (enc_handle_t enc, uint8_t is_head)
{
    enc->is_head = is_head;
}

enc_handle_t pro_encrypt_init (struct encrypt *p_enc, void *p_drv);

#ifdef __cplusplus
}
#endif

#endif
