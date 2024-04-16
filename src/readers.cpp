#include "readers.h"
#include "crc_validation.h"

uint32_t SwapEndian(uint32_t value) {
    return ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) |
           ((value << 24) & 0xFF000000);
}

PNGChunk ReadChunk(std::istream &input) {
    PNGChunk chunk;
    ReadBytes(input, &chunk.length, "Failed on reading chunk length");
    ReadBytes(input, &chunk.type, "Failed on reading chunk type");
    chunk.type = SwapEndian(chunk.type);
    chunk.length = SwapEndian(chunk.length);

    chunk.bytes.resize(chunk.length);
    ReadBytes(input, chunk.bytes.data(), "Failed on reading chunk data", chunk.length);

    ReadBytes(input, &chunk.crc, "Failed on reading chunk crc");
    chunk.crc = SwapEndian(chunk.crc);
    ValidateCRC(SwapEndian(chunk.type), chunk.bytes, chunk.crc);

    return chunk;
}

BitReader::BitReader(std::vector<uint8_t> &bytes) : bytes(bytes) {
}

// rely on cross bytes absence
uint8_t BitReader::GetNBits(size_t byte, size_t bit, size_t n_bits) {
    uint32_t bits = (static_cast<uint32_t>(bytes[byte]) >> bit);
    return static_cast<uint8_t>(bits % (1u << n_bits));
}
