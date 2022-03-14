#include "utils.h"
#include <string.h>

void fromhex(const char *str, uint8_t* buf) {
  size_t len = strlen(str) / 2;
  for (size_t i = 0; i < len; i++) {
    uint8_t c = 0;
    if (str[i * 2] >= '0' && str[i * 2] <= '9') c += (str[i * 2] - '0') << 4;
    if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
      c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
    if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
      c += (str[i * 2 + 1] - '0');
    if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
      c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
    buf[i] = c;
  }
}

void tohex(const uint8_t* buf, size_t len, char* str) {
  for (size_t i = 0; i < len; i++) {
    uint8_t c = buf[i];
    str[i * 2] = "0123456789abcdef"[c >> 4];
    str[i * 2 + 1] = "0123456789abcdef"[c & 0xf];
  }
  str[len * 2] = 0;
}
