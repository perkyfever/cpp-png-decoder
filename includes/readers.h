#pragma once

#include <iostream>
#include <vector>
#include "constants.h"

struct PNGChunk {
    uint32_t crc;
    uint32_t type;
    uint32_t length;
    std::vector<uint8_t> bytes;
};

template <typename T>
inline void ReadBytes(
    std::istream &input,
    T *out,
    const char *msg,
    std::streamsize bytes = sizeof(T)
) {
    if (!input.read(reinterpret_cast<char *>(out), bytes)) {
        throw std::runtime_error(msg);
    }
}

uint32_t SwapEndian(uint32_t value);
PNGChunk ReadChunk(std::istream &input);

class BitReader {
public:
    explicit BitReader(std::vector<uint8_t> &bytes);
    uint8_t GetNBits(size_t byte, size_t bit, size_t n_bits);

private:
    std::vector<uint8_t> &bytes;
};
