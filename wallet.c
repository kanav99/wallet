#include "wallet.h"
#include <assert.h>
#include <bip39.h>

HDNode hdnode_from_mnemonic(char *mnemonic, uint32_t purpose, uint32_t coin, uint32_t account, uint32_t change, uint32_t address) {
  uint8_t seed[64];
  // set passpharse to empty string
  char passpharse[1] = {0};
  // convert mnemonic to seed
  mnemonic_to_seed(mnemonic, passpharse, seed, NULL);
  
  HDNode node;
  // derive master node from seed
  assert(hdnode_from_seed(seed, 64, "secp256k1", &node));
  // derive purpose node from master node
  assert(hdnode_private_ckd_prime(&node, purpose));
  // derive coin node from purpose node
  // 0 -> mainnet, 1 -> testnet
  assert(hdnode_private_ckd_prime(&node, coin));
  // derive account node from coin node
  assert(hdnode_private_ckd_prime(&node, account));
  // derive change node from account node
  assert(hdnode_private_ckd(&node, change));
  // derive address node from change node
  assert(hdnode_private_ckd(&node, address));

  hdnode_fill_public_key(&node);
  return node;
}

wallet_t btc_wallet_from_mnemonic(char *mnemonic, uint32_t coin, uint32_t address) {
  return hdnode_from_mnemonic(mnemonic, 44, 1, 0, coin, address);
}

wallet_t eth_wallet_from_mnemonic(char *mnemonic, uint32_t address) {
  return hdnode_from_mnemonic(mnemonic, 44, 60, 0, 0, address);
}
