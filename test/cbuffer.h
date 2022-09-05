#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/*
    If the buffer is of a power-of-2 size,
    then a much quicker bitwise-AND instruction can be used instead.
*/

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (32)
#endif

#if (BUFFER_SIZE & (BUFFER_SIZE - 1)) != 0
#error "BUFFER_SIZE must be a power of two"
#endif

#define BUFFER_MASK (BUFFER_SIZE-1)

typedef struct cbuffer_s
{
  uint8_t buffer[BUFFER_SIZE];
  int head;
  int tail;
} cbuffer_t;


void cbuffer_init(cbuffer_t* buf);
void cbuffer_add(cbuffer_t* buf, uint8_t item);
uint8_t cbuffer_get(cbuffer_t* buf);
bool cbuffer_empty(cbuffer_t* buf);
bool cbuffer_full(cbuffer_t* buf);
void cbuffer_clear(cbuffer_t* buf);

#endif // CIRCULAR_BUFFER_H
