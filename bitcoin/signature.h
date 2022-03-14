#pragma once

#include "../slice.h"
#include "../wallet.h"
#include "transaction.h"

slice_t btc_sign_partial_transaction(btc_transaction_t *utx, wallet_t *wallet);
btc_transaction_t btc_sign_transaction(btc_transaction_t *utx, wallet_t *wallet);
