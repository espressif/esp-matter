#ifndef __SPP_H_
#define __SPP_H_

#include <stdio.h>
#include "uuid.h"


struct attr_val {
    uint16_t len;
    uint8_t *data;
};

#define set_attr_val(val,size) \
    { \
        .len = size, \
        .data = val, \
    }


#define val_convert(val,size){ \
    (& (struct attr_val) \
        set_attr_val(val,size)  \
    ) \
}

#define SPP_UUID_0   BT_UUID_DECLARE_16(0xc300)
#define SPP_UUID_1   BT_UUID_DECLARE_16(0xc301)
#define SPP_UUID_2   BT_UUID_DECLARE_16(0xc302)
#define SPP_UUID_3   BT_UUID_DECLARE_16(0xc303)
#define SPP_UUID_4   BT_UUID_DECLARE_16(0xc304)
#define SPP_UUID_5   BT_UUID_DECLARE_16(0xc305)
#define SPP_UUID_6   BT_UUID_DECLARE_16(0xc306)
#define SPP_UUID_7   BT_UUID_DECLARE_16(0xc307)
#define SPP_UUID_8   BT_UUID_DECLARE_16(0xc400)
#define SPP_UUID_9   BT_UUID_DECLARE_16(0xc401)

static const struct bt_uuid *spp_svc_1_uuid = BT_UUID_DECLARE_16(0xa002);
static const struct bt_uuid *spp_svc_2_uuid = BT_UUID_DECLARE_16(0xa003);


void bt_dyn_register_spp_srv(void);
void bt_dyn_unregister_spp_srv(void);

#endif