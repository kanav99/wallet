#pragma once
#include <stdint.h>
#include <string.h>

void fromhex(const char *str, uint8_t* buf);
void tohex(const uint8_t* buf, size_t len, char* str);
