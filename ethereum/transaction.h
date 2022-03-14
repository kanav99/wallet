#pragma once

#include <stdint.h>
#include "../slice.h"
#include <bip32.h>

typedef struct {
  uint8_t num[32];
  uint8_t len;
} uint256_t;

typedef struct {
  uint256_t nonce;
  uint256_t gas_price;
  uint256_t gas_limit;
  uint8_t   to[20];
  uint256_t value;
  slice_t   data;
  slice_t   v;
  slice_t   r;
  slice_t   s;
} eth_transaction_t;

int eth_sign_transaction(eth_transaction_t *tx, HDNode *node);
