#include "signature.h"
#include "../wallet.h"
#include <sha2.h>
#include <assert.h>
#include <stdio.h>

#define MAX_BYTES 1000

slice_t btc_sign_partial_transaction(btc_transaction_t *utx, wallet_t *wallet) {
  uint8_t transaction[MAX_BYTES + 4];
  size_t len = serialize_btc_transaction(utx, transaction, MAX_BYTES);
  assert(len <= MAX_BYTES);
  transaction[len] = 0x01;
  transaction[len+1] = 0x00;
  transaction[len+2] = 0x00;
  transaction[len+3] = 0x00;

  uint8_t hash[32];
  sha256_Raw(transaction, len+4, hash);
  sha256_Raw(hash, 32, hash);

  uint8_t sig[64];
  assert(hdnode_sign_digest(wallet, hash, sig, NULL, NULL) == 0);
  
  uint8_t der[100];
  int derlen = ecdsa_sig_to_der(sig, der);

  slice_t script;
  script.len = 1 + derlen + 2 + 33;
  script.data = malloc(script.len);
  script.data[0] = derlen + 1;
  memcpy(script.data + 1, der, derlen);
  script.data[1 + derlen] = 1;
  script.data[1 + derlen + 1] = 33;
  memcpy(script.data + 1 + derlen + 2, wallet->public_key, 33);

  return script;
}

btc_transaction_t btc_sign_transaction(btc_transaction_t *utx, wallet_t *wallet) {
  uint64_t n = utx->num_input;
  slice_t signatures[n];

  for(int i = 0; i < n; ++i) {
    btc_transaction_t t = get_partial_btc_transaction(utx, i);
    signatures[i] = btc_sign_partial_transaction(&t, wallet);
    free_partial_btc_transaction(&t, i);
  }

  btc_transaction_t stx = copy_btc_transaction(utx);
  for(int i = 0; i < n; ++i) {
    free_slice(&stx.inputs[i].script);
    stx.inputs[i].script = signatures[i];
  }

  return stx;
}