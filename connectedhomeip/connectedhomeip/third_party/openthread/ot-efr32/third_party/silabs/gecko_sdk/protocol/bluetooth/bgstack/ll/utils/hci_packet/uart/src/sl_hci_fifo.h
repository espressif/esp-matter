#ifndef SL_HCI_FIFO_H
#define SL_HCI_FIFO_H
#include <stdint.h>

enum sl_hci_fifo_flag{
  sl_hci_fifo_flag_full      = 0x01,
  sl_hci_fifo_flag_dma_full  = 0x02,
};

typedef uint16_t fifo_size_t;

struct sl_hci_fifo {
  uint8_t *buffer;
  uint8_t *head;
  uint8_t *tail;
  uint8_t *dma_tail;
  uint8_t flags;
  fifo_size_t buffer_size;
};

#define fifo_define(name, size)    \
  uint8_t name##__buffer[size];    \
  struct sl_hci_fifo name =        \
  {                                \
    .buffer      = name##__buffer, \
    .buffer_size = size,           \
  }

void sl_hci_fifo_init(struct sl_hci_fifo *fifo, uint8_t config_flags);
static inline uint8_t *sl_hci_fifo_buffer(struct sl_hci_fifo *fifo)
{
  return fifo->buffer;
}
static inline fifo_size_t sl_hci_fifo_size(struct sl_hci_fifo *fifo)
{
  return fifo->buffer_size;
}
fifo_size_t sl_hci_fifo_length(struct sl_hci_fifo *fifo);
fifo_size_t sl_hci_fifo_space(struct sl_hci_fifo *fifo);
fifo_size_t sl_hci_fifo_read(struct sl_hci_fifo *fifo, uint8_t *buffer, fifo_size_t count);
fifo_size_t sl_hci_fifo_write(struct sl_hci_fifo *fifo, const uint8_t *buffer, fifo_size_t count);
fifo_size_t sl_hci_fifo_dma_space(struct sl_hci_fifo *fifo);
fifo_size_t sl_hci_fifo_dma_reserve(struct sl_hci_fifo *fifo, uint8_t **buffer, fifo_size_t count);
void sl_hci_fifo_dma_set_tail(struct sl_hci_fifo *fifo, uint8_t *tail);
#endif // SL_HCI_FIFO_H
