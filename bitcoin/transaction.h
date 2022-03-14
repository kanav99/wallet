#pragma once
#include "../slice.h"
#include <stdint.h>
#include <string.h>

typedef struct {
  uint8_t  hash[32];
  uint32_t index;
  slice_t  script;
  uint32_t sequence;
} btc_transaction_input_t;

typedef struct {
  uint64_t amount;
  slice_t  script;
} btc_transaction_output_t;

typedef struct {
  uint32_t version;
  uint64_t num_input;
  btc_transaction_input_t *inputs;
  uint64_t num_output;
  btc_transaction_output_t *outputs;
  uint32_t locktime;
} btc_transaction_t;

void free_btc_transaction(btc_transaction_t *utx);
btc_transaction_t copy_btc_transaction(btc_transaction_t *src);

size_t serialize_btc_transaction(btc_transaction_t *utx, uint8_t *transaction, size_t transaction_size);
btc_transaction_t deserialize_btc_transaction(uint8_t *transaction, size_t transaction_size);

btc_transaction_t get_partial_btc_transaction(btc_transaction_t *utx, int idx);
void free_partial_btc_transaction(btc_transaction_t *utx, int idx);
