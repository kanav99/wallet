#include "bitcoin/transaction.h"
#include "bitcoin/signature.h"
#include "ethereum/rlp.h"
#include "ethereum/chains.h"
#include "utils.h"
#include "wallet.h"
#include <stdio.h>
#include <assert.h>

char mnemonic[] = "<redacted>";

int bitcoin_example() {
  
  char transactionhex[] = "0200000002e31008fb7c53ddeda348beb0dd66920391a59277361d66336ae8b2eb2b002306010000001976a914fb4d8e0ffb3efb94fae120ea41b8b94bf7f46b0f88acffffffffea8cd55c3cccb94d88d14b1f5d3bc4282c2a1f8d928e6b63e2c0e117159b82b3000000001976a914fb4d8e0ffb3efb94fae120ea41b8b94bf7f46b0f88acffffffff0150c30000000000001976a914ed614881f32c024a80d1b6b58dfed8f493f41c7288ac00000000";

  uint8_t buf[1000];
  fromhex(transactionhex, buf);
  btc_transaction_t utx = deserialize_btc_transaction(buf, strlen(transactionhex) / 2);

  wallet_t node = btc_wallet_from_mnemonic(mnemonic, 0, 0);

  btc_transaction_t stx = btc_sign_transaction(&utx, &node);
  size_t len = serialize_btc_transaction(&stx, buf, 1000);

  char hex[2000];
  tohex(buf, len, hex);
  printf("%s\n", hex);

  free_btc_transaction(&utx);
  free_btc_transaction(&stx);

  return 0;
}

void weenus_faucet(HDNode *node) {
  static uint8_t weenus_addr[] = {0x10, 0x18, 0x48, 0xD5, 0xC5, 0xbB, 0xca, 0x18, 0xE6, 0xb4, 0x43, 0x1e, 0xEd, 0xF6, 0xB9, 0x5E, 0x9A, 0xDF, 0x82, 0xFA};
  eth_transaction_t tx;

  tx.nonce.len = 1;
  tx.nonce.num[0] = 2;

  tx.gas_limit.len = 2;
  tx.gas_limit.num[0] = 0x7f;
  tx.gas_limit.num[1] = 0x7f;

  tx.gas_price.len = 5;
  tx.gas_price.num[0] = 0x7f;
  tx.gas_price.num[1] = 0x7f;
  tx.gas_price.num[2] = 0x7f;
  tx.gas_price.num[3] = 0x7f;
  tx.gas_price.num[4] = 0x7f;

  tx.value.len = 0;
  tx.value.num[0] = 0x00;

  memcpy(tx.to, weenus_addr, 20);
  
  tx.data.len = 4;
  tx.data.data = malloc(4);
  tx.data.data[0] = 0x9f;
  tx.data.data[1] = 0x67;
  tx.data.data[2] = 0x8c;
  tx.data.data[3] = 0xca;
  
  tx.v.len = 1;
  tx.v.data = malloc(1);
  tx.v.data[0] = ETH_ROPSTEN;

  tx.r.len = 0;
  tx.s.len = 0;

  eth_sign_transaction(&tx, node);

  char hex[1000];
  uint8_t rlp[1000];
  uint64_t len = rlp_write_eth_transaction(rlp, &tx);
  tohex(rlp, len, hex);
  printf("%s\n", hex);
}

int ethereum_example() {
  char transactionhex[] = "f869038502540be40082c35094646f7e571a2d2ae709bcb9f7f12c6e47f235fd9c80b844a9059cbb0000000000000000000000005913e2469b1f7d07f93b7cb5cdc68286865cc02f0000000000000000000000000000000000000000000000000000000000000001038080";

  uint8_t buf[1000];
  fromhex(transactionhex, buf);

  wallet_t node = eth_wallet_from_mnemonic(mnemonic, 0);

  eth_transaction_t tx;
  uint64_t txlen = rlp_read_eth_transaction(buf, &tx, false);
  printf("nonce: ");
  for(int i = 0; i < tx.nonce.len; ++i) {
    printf("%02x", tx.nonce.num[i]);
  }
  printf("\n");
  tx.nonce.num[0] = 4;

  printf("amount: ");
  print_u256(&tx.value);
  printf("\n");

  if (tx.data.len != 0) {
    printf("smart contract initialization\n");
    // keccack256("transfer(address,uint256)") = 0xa9059cbb....
    if (tx.data.len > 4 && tx.data.data[0] == 0xa9 && tx.data.data[1] == 0x05 && tx.data.data[2] == 0x9c && tx.data.data[3] == 0xbb) {
      uint256_t address;
      address.len = 32;
      memcpy(address.num, tx.data.data + 4, 32);
      printf("erc20 target address: ");
      print_u256(&address);
      printf("\n");

      uint256_t amount;
      amount.len = 32;
      memcpy(amount.num, tx.data.data + 36, 32);
      printf("erc20 amount: ");
      print_u256(&amount);
      printf("\n");
    }
  }

  eth_sign_transaction(&tx, &node);
  char finalHex[1000];
  uint64_t txlen2 = rlp_write_eth_transaction(buf, &tx);
  tohex(buf, txlen2, finalHex);
  printf("%s\n", finalHex);
  return 0;
}

int main() {
  ethereum_example();
}
