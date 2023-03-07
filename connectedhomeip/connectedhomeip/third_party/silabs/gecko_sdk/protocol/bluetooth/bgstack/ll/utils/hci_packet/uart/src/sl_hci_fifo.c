#include <string.h>
#include "sl_hci_fifo.h"

static inline int MIN(fifo_size_t a, fifo_size_t b)
{
  return (a < b) ? a : b;
}

void sl_hci_fifo_init(struct sl_hci_fifo *fifo, uint8_t config_flags)
{
  fifo->head = fifo->buffer;
  fifo->tail = fifo->buffer;
  fifo->dma_tail = fifo->buffer;
  fifo->flags = config_flags;
}

static fifo_size_t length(struct sl_hci_fifo *fifo, uint8_t *head, uint8_t *tail)
{
  int length = tail - head;
  if (length < 0) {
    length += fifo->buffer_size;
  }
  return (fifo_size_t)length;
}

fifo_size_t sl_hci_fifo_length(struct sl_hci_fifo *fifo)
{
  if (fifo->flags & sl_hci_fifo_flag_full) {
    return fifo->buffer_size;
  }
  return length(fifo, fifo->head, fifo->tail);
}

fifo_size_t sl_hci_fifo_space(struct sl_hci_fifo *fifo)
{
  return fifo->buffer_size - sl_hci_fifo_length(fifo);
}

fifo_size_t sl_hci_fifo_read(struct sl_hci_fifo *fifo, uint8_t *buffer, fifo_size_t count)
{
  count = MIN(count, sl_hci_fifo_length(fifo));
  fifo_size_t bytes_left = count;

  while (bytes_left) {
    fifo_size_t bytes = MIN(bytes_left, &fifo->buffer[fifo->buffer_size] - fifo->head);

    if (buffer) {
      memcpy(buffer, fifo->head, bytes);
      buffer += bytes;
    }
    fifo->head += bytes;
    bytes_left -= bytes;

    if (fifo->head >= &fifo->buffer[fifo->buffer_size]) {
      fifo->head = fifo->buffer;
    }
  }

  if (count > 0) {
    if (fifo->flags & sl_hci_fifo_flag_full) {
      fifo->flags &= ~sl_hci_fifo_flag_full;
    }
    if (fifo->flags & sl_hci_fifo_flag_dma_full) {
      fifo->flags &= ~sl_hci_fifo_flag_dma_full;
    }
  }

  return count;
}

fifo_size_t sl_hci_fifo_write(struct sl_hci_fifo *fifo, const uint8_t *buffer, fifo_size_t count)
{
  count = MIN(count, sl_hci_fifo_space(fifo));
  fifo_size_t bytes_left = count;

  while (bytes_left) {
    fifo_size_t bytes = MIN(bytes_left, &fifo->buffer[fifo->buffer_size] - fifo->tail);

    if (buffer) {
      memcpy(fifo->tail, buffer, bytes);
      buffer += bytes;
    }
    fifo->tail += bytes;
    bytes_left -= bytes;

    if (fifo->tail >= &fifo->buffer[fifo->buffer_size]) {
      fifo->tail = fifo->buffer;
    }
  }

  if (fifo->tail == fifo->head) {
    fifo->flags |= sl_hci_fifo_flag_full;
  }

  return count;
}

fifo_size_t sl_hci_fifo_dma_space(struct sl_hci_fifo *fifo)
{
  if (fifo->flags & sl_hci_fifo_flag_dma_full) {
    return 0;
  }
  return fifo->buffer_size - length(fifo, fifo->head, fifo->dma_tail);
}

fifo_size_t sl_hci_fifo_dma_reserve(struct sl_hci_fifo *fifo, uint8_t **buffer, fifo_size_t count)
{
  count = MIN(count, &fifo->buffer[fifo->buffer_size] - fifo->dma_tail);

  if (buffer) {
    *buffer = fifo->dma_tail;
  }

  fifo->dma_tail += count;
  if (fifo->dma_tail >= &fifo->buffer[fifo->buffer_size]) {
    fifo->dma_tail -= fifo->buffer_size;
  }

  if (fifo->dma_tail == fifo->head) {
    fifo->flags |= sl_hci_fifo_flag_dma_full;
  }

  return count;
}

void sl_hci_fifo_dma_set_tail(struct sl_hci_fifo *fifo, uint8_t *tail)
{
  if (fifo->tail == tail) {
    return;
  }

  fifo->tail = tail;

  if (fifo->tail >= &fifo->buffer[fifo->buffer_size]) {
    fifo->tail -= fifo->buffer_size;
  }

  if (fifo->tail == fifo->head) {
    fifo->flags |= sl_hci_fifo_flag_full;
  }
}

#ifdef UTEST_FIFO

#include <assert.h>
#include <stdio.h>

#define TEST_DATA_SIZE 256
uint8_t *test_data()
{
  static uint8_t data[TEST_DATA_SIZE];
  int i;
  for (i = 0; i < TEST_DATA_SIZE; i++) {
    data[i] = i % TEST_DATA_SIZE;
  }
  return data;
}

void dump(const uint8_t *ptr, fifo_size_t count)
{
  while (count--)
    printf(" %02x", *ptr++);
  printf("\n");
}

void dump_fifo(struct sl_hci_fifo *fifo)
{
  printf("%d %d ", sl_hci_fifo_length(fifo), sl_hci_fifo_space(fifo));
  dump(fifo->buffer, 5);
}

fifo_define(fifo, 5);

int main()
{
  fifo_size_t bytes;
  sl_hci_fifo_init(&fifo, 0);

  uint8_t *write_data = test_data();
  uint8_t read_data[TEST_DATA_SIZE];

  uint8_t *write_ptr = write_data;
  uint8_t *read_ptr = read_data;

  assert(sl_hci_fifo_space(&fifo) == 5);

  bytes = sl_hci_fifo_write(&fifo, write_ptr, 255);
  assert(bytes == 5);
  assert(sl_hci_fifo_length(&fifo) == 5);
  assert(sl_hci_fifo_space(&fifo) == 0);
  write_ptr += bytes;

  bytes = sl_hci_fifo_read(&fifo, read_ptr, 255);
  assert(bytes == 5);
  assert(sl_hci_fifo_length(&fifo) == 0);
  assert(sl_hci_fifo_space(&fifo) == 5);
  read_ptr += bytes;

  write_ptr += sl_hci_fifo_write(&fifo, write_ptr, 3);
  read_ptr += sl_hci_fifo_read(&fifo, read_ptr, 255);

  write_ptr += sl_hci_fifo_write(&fifo, write_ptr, 255);
  read_ptr += sl_hci_fifo_read(&fifo, read_ptr, 255);

  bytes = write_ptr - write_data;
  assert(bytes == 13);
  dump(write_data, bytes);
  dump(read_data, bytes);
  assert(!memcmp(write_data, read_data, bytes));

  // dma tail
  sl_hci_fifo_init(&fifo, 0);
  assert(sl_hci_fifo_space(&fifo) == 5);
  assert(sl_hci_fifo_dma_space(&fifo) == 5);

  bytes = sl_hci_fifo_dma_reserve(&fifo, &write_ptr, 5);
  assert(bytes == 5);
  assert(sl_hci_fifo_dma_space(&fifo) == 0);

  sl_hci_fifo_dma_set_tail(&fifo, write_ptr + 5);
  bytes = sl_hci_fifo_read(&fifo, NULL, 5);
  assert(bytes == 5);
  assert(sl_hci_fifo_dma_space(&fifo) == 5);
}
#endif
