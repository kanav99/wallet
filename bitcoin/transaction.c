#include "transaction.h"
#include "io.h"
#include "../utils.h"
#include <string.h>
#include <stdlib.h>

btc_transaction_t copy_btc_transaction(btc_transaction_t *src) {
  btc_transaction_t dst;
  dst.version = src->version;
  dst.num_input = src->num_input;
  dst.inputs = malloc(sizeof(btc_transaction_input_t) * dst.num_input);
  for(uint64_t i = 0; i < dst.num_input; ++i) {
    memcpy(dst.inputs[i].hash, src->inputs[i].hash, 32);
    dst.inputs[i].index = src->inputs[i].index;
    dst.inputs[i].script = copy_slice(src->inputs[i].script);
    dst.inputs[i].sequence = src->inputs[i].sequence;
  }
  dst.num_output = src->num_output;
  dst.outputs = malloc(sizeof(btc_transaction_output_t) * dst.num_output);
  for(uint64_t i = 0; i < dst.num_output; ++i) {
    dst.outputs[i].amount = src->outputs[i].amount;
    dst.outputs[i].script = copy_slice(src->outputs[i].script);
  }
  dst.locktime = src->locktime;
  return dst;
}

void free_btc_transaction(btc_transaction_t *utx) {
  for(uint64_t i = 0; i < utx->num_input; ++i) {
    free_slice(&utx->inputs[i].script);
  }
  free(utx->inputs);
  for(uint64_t i = 0; i < utx->num_output; ++i) {
    free_slice(&utx->outputs[i].script);
  }
  free(utx->outputs);
}

btc_transaction_t deserialize_btc_transaction(uint8_t *transaction, size_t transaction_size) {
  IO_INIT;
  btc_transaction_t utx;

  READ_U32(utx.version);
  READ_VINT(utx.num_input);
  utx.inputs = malloc(sizeof(btc_transaction_input_t) * utx.num_input);

  for(uint64_t i = 0; i < utx.num_input; ++i) {
    READ_HASH(utx.inputs[i].hash);
    READ_U32(utx.inputs[i].index);
    READ_SLICE(utx.inputs[i].script);
    READ_U32(utx.inputs[i].sequence);
  }

  READ_VINT(utx.num_output);
  utx.outputs = malloc(sizeof(btc_transaction_output_t) * utx.num_output);

  for(uint64_t i = 0; i < utx.num_output; ++i) {
    READ_U64(utx.outputs[i].amount);
    READ_SLICE(utx.outputs[i].script);
  }

  READ_U32(utx.locktime);
  assert(offset == transaction_size);
  return utx;
}

size_t serialize_btc_transaction(btc_transaction_t *utx, uint8_t *transaction, size_t transaction_size) {
  IO_INIT;

  WRITE_U32(utx->version);
  WRITE_VINT(utx->num_input);
  for(uint64_t i = 0; i < utx->num_input; ++i) {
    WRITE_HASH(utx->inputs[i].hash);
    WRITE_U32(utx->inputs[i].index);
    WRITE_SLICE(utx->inputs[i].script);
    WRITE_U32(utx->inputs[i].sequence);
  }

  WRITE_VINT(utx->num_output);
  for(uint64_t i = 0; i < utx->num_output; ++i) {
    WRITE_U64(utx->outputs[i].amount);
    WRITE_SLICE(utx->outputs[i].script);
  }

  WRITE_U32(utx->locktime);
  return offset;
}

btc_transaction_t get_partial_btc_transaction(btc_transaction_t *utx, int idx) {
  btc_transaction_t utx_copy = copy_btc_transaction(utx);
  for(int i = 0; i < utx_copy.num_input; ++i) {
    if(i != idx) {
      free_slice(&utx_copy.inputs[i].script);
    }
  }
  return utx_copy;
}

void free_partial_btc_transaction(btc_transaction_t *utx, int idx) {
  free_slice(&utx->inputs[idx].script);
  free(utx->inputs);
  for(uint64_t i = 0; i < utx->num_output; ++i) {
    free_slice(&utx->outputs[i].script);
  }
  free(utx->outputs);
}
