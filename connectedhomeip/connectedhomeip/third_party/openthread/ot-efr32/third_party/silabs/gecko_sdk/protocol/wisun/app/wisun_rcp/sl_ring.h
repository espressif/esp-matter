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
#ifndef SL_RING_H
#define SL_RING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Simple ring buffer (aka circular buffer) implementation.
 *
 * The idea is to be simple to debug and to maintain. It allow one producer and
 * one consumer. Producer and consumer can be tasks or IRQs.
 * If user need multiple producer or multiple consumer, he may add the necessary
 * locking system depending of his needs.
 *
 * There is neither signaling system when a new data is available in the buffer.
 * User may add the calls to the necessary functions.
 *
 * On initialization, user have to assign a allocated buffer to the data. The
 * size of this buffer *must be a power of 2*. User has to assign the size of
 * this buffer minus 1 (that is used as mask for the pointers) to the size_mask
 * field:
 *
 *     uint8_t buf[512]; // 512 is 2^9, so it is correct
 *     struct ring r = {
 *         .buf = buf;
 *         .size_mask = sizeof(buf) - 1; // == 0x1FF
 *         // .count_rd and .count_wr will be set to 0 as necessary
 *     }
 *
 * User may rely of ring_init() to simplify the job.
 *
 * Internally, count_rd and count_wr will contains the overall number of bytes
 * read/write. This choice allow to improve speed, atomicity of operations,
 * simplify some operations and improve debug.
 *
 * For atomicity, count_rd and count_wr are declared volatile. Thus, this struct
 * can be used between IRQ and tasks. This is a bit overkill, but it allow to
 * keep code simple. In order to keep things fast, avoid to access count_rd and
 * count_wr directly.
 *
 * Note to run on multicore chipset, extra memory barriers would be necessary
 * to access to count_rd and count_wr.
 */

struct ring {
    volatile unsigned int count_rd;
    volatile unsigned int count_wr;
    unsigned int size_mask;
    uint8_t *buf;
};

void ring_init(struct ring *ring, void *buf, size_t buf_size);
int ring_push(struct ring *ring, uint8_t data);
int ring_push_buf(struct ring *ring, uint8_t *data, size_t len);
int ring_pop(struct ring *ring);
int ring_pop_buf(struct ring *ring, uint8_t *data, size_t len);
int ring_get(struct ring *ring, unsigned int index);
bool ring_is_empty(struct ring *ring);
unsigned int ring_data_len(struct ring *ring);
unsigned int ring_buffer_size(struct ring *ring);

#endif
