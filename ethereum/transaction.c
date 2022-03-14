#include "transaction.h"
#include "rlp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int eth_sign_transaction(eth_transaction_t *tx, HDNode *node) {
  uint8_t buf[1000];
  uint64_t len = rlp_write_eth_transaction(buf, tx);

  uint8_t hash[32];
  keccak_256(buf, len, hash);

  uint8_t sig[64];

  uint8_t v;

  assert(hdnode_sign_digest(node, hash, sig, &v, NULL) == 0);

  tx->r.len = 32;
  tx->s.len = 32;
  tx->r.data = malloc(32);
  tx->s.data = malloc(32);

  memcpy(tx->r.data, sig, 32);
  memcpy(tx->s.data, sig + 32, 32);

  tx->v.len = 1;
  tx->v.data[0] = 2 * tx->v.data[0] + (v % 2) + 35;

  return 0;
}
