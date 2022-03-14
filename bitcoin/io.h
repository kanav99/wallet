#pragma once
#include <monero/serialize.h>
#include <assert.h>

#define IO_INIT \
    size_t offset = 0; \
    uint64_t temp; \
    uint8_t tempVarInt[9]; \
    int vintlen;

#define READ_VINT(X) \
    vintlen = xmr_read_varint(transaction + offset, transaction_size - offset, &temp); \
    assert(vintlen > 0); \
    X = temp; \
    offset += vintlen;

#define WRITE_VINT(X) \
    vintlen = xmr_write_varint(tempVarInt, 9, X); \
    assert(vintlen > 0); \
    assert(offset + vintlen <= transaction_size); \
    memcpy(transaction + offset, tempVarInt, vintlen); \
    offset += vintlen;

#define READ_U32(X) \
    X = *(uint32_t *)(transaction + offset); \
    offset += 4;

#define WRITE_U32(X) \
    assert(offset + 4 <= transaction_size); \
    *(uint32_t *)(transaction + offset) = X; \
    offset += 4;

#define READ_U64(X) \
    X = *(uint64_t *)(transaction + offset); \
    offset += 8;

#define WRITE_U64(X) \
    assert(offset + 8 <= transaction_size); \
    *(uint64_t *)(transaction + offset) = X; \
    offset += 8;

#define READ_SLICE(X) \
    READ_VINT(X.len); \
    X.data = malloc(X.len); \
    memcpy(X.data, transaction + offset, X.len); \
    offset += X.len;

#define WRITE_SLICE(X) \
    WRITE_VINT(X.len); \
    assert(offset + X.len <= transaction_size); \
    memcpy(transaction + offset, X.data, X.len); \
    offset += X.len;

#define READ_HASH(X) \
    memcpy(X, transaction + offset, 32); \
    offset += 32;

#define WRITE_HASH(X) \
    assert(offset + 32 <= transaction_size); \
    memcpy(transaction + offset, X, 32); \
    offset += 32;
