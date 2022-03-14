#pragma once

#include <stdint.h>
#include "transaction.h"

void print_u256(uint256_t *u256);

uint64_t rlp_read_eth_transaction(uint8_t *rlp, eth_transaction_t *tx, bool signed_tx);
uint64_t rlp_write_eth_transaction(uint8_t *rlp, eth_transaction_t *tx);
