/***************************************************************************//**
 * Copyright 2021 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available here[1]. This software is distributed to you in
 * Source Code format and is governed by the sections of the MSLA applicable to
 * Source Code.
 *
 * [1] www.silabs.com/about-us/legal/master-software-license-agreement
 *
 ******************************************************************************/

#include <stdbool.h>
#include <string.h>

#include "sl_ring.h"
#include "sl_wsrcp_log.h"

void ring_init(struct ring *ring, void *buf, size_t buf_size)
{
    memset(ring, 0, sizeof(struct ring));
    ring->buf = buf;
    ring->size_mask = buf_size - 1;
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
}

int ring_push(struct ring *ring, uint8_t data)
{
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    if (ring_data_len(ring) + 1 > ring_buffer_size(ring))
        return -1; // Full
    ring->buf[ring->count_wr++ & ring->size_mask] = data;
    return 0;
}

int ring_push_buf(struct ring *ring, uint8_t *data, size_t len)
{
    size_t remaining;
    unsigned int count_wr;

    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    if (ring_data_len(ring) + len > ring_buffer_size(ring))
        return -1; // Full

    count_wr = ring->count_wr & ring->size_mask;
    remaining = ring_buffer_size(ring) - count_wr;
    if (len > remaining) {
        memcpy(ring->buf + count_wr, data, remaining);
        memcpy(ring->buf, data + remaining, len - remaining);
    } else {
        memcpy(ring->buf + count_wr, data, len);
    }
    ring->count_wr += len;
    return 0;
}

int ring_pop(struct ring *ring)
{
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    if (ring_is_empty(ring))
        return -1; // Empty
    return ring->buf[ring->count_rd++ & ring->size_mask];
}

int ring_pop_buf(struct ring *ring, uint8_t *data, size_t len)
{
    size_t remaining;
    unsigned int count_rd;

    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    if (ring_is_empty(ring))
        return -1; // Empty

    if (len < ring_data_len(ring))
        len = ring_data_len(ring);
    count_rd = ring->count_rd & ring->size_mask;
    remaining = ring_buffer_size(ring) - count_rd;
    if (len > remaining) {
        memcpy(data, ring->buf + count_rd, remaining);
        memcpy(data + remaining, ring->buf, len - remaining);
    } else {
        memcpy(data, ring->buf + count_rd, len);
    }
    ring->count_rd += len;
    return len;
}

int ring_get(struct ring *ring, unsigned int index)
{
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    if (index >= ring_data_len(ring))
        return -1; // Out-of-range
    return ring->buf[(ring->count_rd + index) & ring->size_mask];
}

bool ring_is_empty(struct ring *ring)
{
    unsigned int count_rd, count_wr;
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    count_rd = ring->count_rd;
    count_wr = ring->count_wr;
    return count_wr == count_rd;
}

unsigned int ring_data_len(struct ring *ring)
{
    unsigned int count_rd, count_wr, len;

    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    count_rd = ring->count_rd;
    count_wr = ring->count_wr;
    // Note that when count_wr overlap, count_rd may be greater than count_wr.
    // However, the math are still correct
    len = count_wr - count_rd;
    BUG_ON(len > ring->size_mask + 1);
    return len;
}

unsigned int ring_buffer_size(struct ring *ring)
{
    BUG_ON(!ring->buf);
    BUG_ON((ring->size_mask + 1) & ring->size_mask);
    return ring->size_mask + 1;
}
