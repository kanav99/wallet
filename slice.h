#pragma once

#include <stdint.h>

typedef struct {
  uint64_t len;
  uint8_t* data;
} slice_t;

slice_t copy_slice(slice_t);
void free_slice(slice_t *);

