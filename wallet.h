#pragma once
#include <bip32.h>

typedef HDNode wallet_t;

wallet_t btc_wallet_from_mnemonic(char *mnemonic, uint32_t coin, uint32_t address);
wallet_t eth_wallet_from_mnemonic(char *mnemonic, uint32_t address);
