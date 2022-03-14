#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../slice.h"
#include "transaction.h"

void print_u256(uint256_t *u256) {
  printf("0x");
  if (u256->len == 0) {
    printf("00");
    return;
  }
  
  for(int i = 0; i < u256->len; ++i) {
    printf("%02x", u256->num[i]);
  }
}

uint8_t rlp_write_num_without_length(uint8_t *rlp, uint64_t num) {
  if (num == 0) {
    return 0;
  }
  else {
    uint8_t l = rlp_write_num_without_length(rlp, num >> 8);
    rlp[l] = num & 0xff;
    return l + 1;
  }
}

uint64_t rlp_write_length(uint8_t *rlp, uint64_t len, uint8_t offset) {
  if (len < 56) {
    rlp[0] = offset + len;
    return 1;
  }
  uint8_t lenlen = rlp_write_num_without_length(rlp + 1, len);
  rlp[0] = offset + lenlen + 55;
  return 1 + (uint64_t)lenlen;
}

uint64_t rlp_write_string(uint8_t *rlp, uint8_t *item, uint64_t item_size) {
  if (item_size == 1 && item[0] < 0x80) {
    rlp[0] = item[0];
    return 1;
  }

  if (item_size < 56) {
    rlp[0] = 0x80 + item_size;
    memcpy(rlp + 1, item, item_size);
    return 1 + item_size;
  } else {
    uint64_t len = rlp_write_length(rlp, item_size, 0x80);
    memcpy(rlp + len, item, item_size);
    return len + item_size;
  }
}

uint64_t rlp_read_slice(uint8_t *rlp, slice_t *item) {
  uint8_t b0 = rlp[0];
  if (b0 < 0x80) {
    item->data = malloc(1);
    memcpy(item->data, rlp, 1);
    item->len = 1;
    return 1;
  } else if (b0 < 0xb8) {
    uint64_t len = b0 - 0x80;
    item->data = malloc(len);
    memcpy(item->data, rlp + 1, len);
    item->len = len;
    return 1 + len;
  } else {
    uint64_t lenlen = b0 - 0xb7;
    uint64_t len = 0;
    for (uint64_t i = 0; i < lenlen; i++) {
      len = (len << 8) + rlp[i + 1];
    }
    item->data = malloc(len);
    memcpy(item->data, rlp + 1 + lenlen, len);
    item->len = len;
    return 1 + lenlen + len;
  }
}

uint64_t rlp_write_slice(uint8_t *rlp, slice_t *item) {
  return rlp_write_string(rlp, item->data, item->len);
}

uint64_t rlp_read_u256(uint8_t *rlp, uint256_t *item) {
  slice_t slice;
  uint64_t len = rlp_read_slice(rlp, &slice);
  assert(slice.len <= 32);
  memcpy(item->num, slice.data, slice.len);
  item->len = slice.len;
  return len;
}

uint64_t rlp_write_u256(uint8_t *rlp, uint256_t *item) {
  assert(item->len <= 32);
  return rlp_write_string(rlp, item->num, item->len);
}

uint64_t rlp_read_eth_transaction(uint8_t *rlp, eth_transaction_t *tx, bool signed_tx) {
  uint8_t b0 = rlp[0];
  uint64_t payloadlen;
  uint64_t offset;
  
  if (b0 < 0xc0) {
    assert(false);
  }
  
  if (b0 < 0xf8) {
    payloadlen = b0 - 0xc0;
    offset = 1;
  } else {
    uint8_t lenlen = b0 - 0xf7;
    offset = 1 + lenlen;
    payloadlen = 0;
    for (uint64_t i = 0; i < lenlen; i++) {
      payloadlen = (payloadlen << 8) + rlp[i + 1];
    }
  }

  offset += rlp_read_u256(rlp + offset, &tx->nonce);
  offset += rlp_read_u256(rlp + offset, &tx->gas_price);
  offset += rlp_read_u256(rlp + offset, &tx->gas_limit);
  
  slice_t to;
  offset += rlp_read_slice(rlp + offset, &to);
  assert(to.len == 20);
  memcpy(tx->to, to.data, 20);
  
  offset += rlp_read_u256(rlp + offset, &tx->value);
  offset += rlp_read_slice(rlp + offset, &tx->data);
  
  offset += rlp_read_slice(rlp + offset, &tx->v);
  offset += rlp_read_slice(rlp + offset, &tx->r);
  offset += rlp_read_slice(rlp + offset, &tx->s);
  
  if (signed_tx) {
    assert(tx->v.len == 1);
    assert(tx->r.len == 32);
    assert(tx->s.len == 32);
  }

  return offset;
}

uint8_t tmpBuf[1000];

uint64_t rlp_write_eth_transaction(uint8_t *rlp, eth_transaction_t *tx) {
  uint64_t offset = 0;
  offset += rlp_write_u256(tmpBuf + offset, &tx->nonce);
  offset += rlp_write_u256(tmpBuf + offset, &tx->gas_price);
  offset += rlp_write_u256(tmpBuf + offset, &tx->gas_limit);
  
  slice_t to;
  to.data = tx->to;
  to.len = 20;
  offset += rlp_write_slice(tmpBuf + offset, &to);
  
  offset += rlp_write_u256(tmpBuf + offset, &tx->value);
  offset += rlp_write_slice(tmpBuf + offset, &tx->data);
  
  offset += rlp_write_slice(tmpBuf + offset, &tx->v);

  offset += rlp_write_slice(tmpBuf + offset, &tx->r);
  offset += rlp_write_slice(tmpBuf + offset, &tx->s);

  uint64_t t = rlp_write_length(rlp, offset, 0xc0);
  memcpy(rlp + t, tmpBuf, offset);
  
  return t + offset;
}
