#include "slice.h"
#include <stdlib.h>
#include <string.h>

slice_t copy_slice(slice_t src) {
  slice_t dst;
  dst.len = src.len;
  dst.data = malloc(dst.len);
  memcpy(dst.data, src.data, dst.len);
  return dst;
}

void free_slice(slice_t *s) {
  free(s->data);
  s->len = 0;
}