#ifndef  _COMMON_SERVER_H_
#define  _COMMON_SERVER_H_

#include <stdint.h>
#include <types.h>
#include <stdbool.h> 

#define MESH_TRANS_MIC_SIZE_4    4
#define MESH_TRANS_MIC_SIZE_8    8
#define MESH_GEN_OPCODE_SIZE_1   1
#define MESH_GEN_OPCODE_SIZE_2   2

enum{
    MESH_STATE_OFF = 0x00,
    MESH_STATE_ON
};

struct bt_mesh_state_transition{
    u8_t trans_time;
    u8_t delay;
};

struct bt_mesh_last_rcvd_msg{
    u8_t tid;
    u16_t src_addr;
    u16_t dst_addr;
    int64_t rcvd_time;
};

void bt_mesh_srv_update_last_rcvd_msg(struct bt_mesh_last_rcvd_msg *last_msg, u8_t tid, u16_t src_addr, u16_t dst_addr,
                                int64_t rcvd_time);
bool bt_mesh_srv_check_rcvd_msg_with_last(u8_t tid, u16_t src_addr, u16_t dst_addr,
                                int64_t *rcvd_time, struct bt_mesh_last_rcvd_msg *last_msg);

#endif
