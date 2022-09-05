#include "cbuffer.h"

void cbuffer_init(cbuffer_t* buf) {
    buf->head = buf->tail = 0;
}

bool cbuffer_empty(cbuffer_t* buf) {
    return buf->head == buf->tail;
}

void cbuffer_add(cbuffer_t* buf, uint8_t item) {
  if(cbuffer_full(buf)) {
    buf->tail = ((buf->tail + 1) & BUFFER_MASK);
  }

  buf->buffer[buf->head] = item;
  buf->head = ((buf->head + 1) & BUFFER_MASK);
}


uint8_t cbuffer_get(cbuffer_t* buf) {
    if (cbuffer_empty(buf))
        return 0;
    return buf->buffer[buf->tail++];
}



bool cbuffer_full(cbuffer_t* buf) {
    return ((buf->head - buf->tail) & BUFFER_MASK) == BUFFER_MASK;
}


void cbuffer_clear(cbuffer_t* buf) {
    buf->head = buf->tail;
}

