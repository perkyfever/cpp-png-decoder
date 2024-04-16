#pragma once

#include <cstddef>
#include <cstdint>

// MISC CONSTANTS
constexpr size_t MAX_BIT_DEPTH = 8;
constexpr size_t IHDR_CHUNK_SIZE = 13;

// SIGNATURE
struct Signature {
    static constexpr size_t SIZE = 8;
    static constexpr char SIGNATURE[] = "\x89PNG\r\n\x1a\n";
};

// CHUNK TYPES
struct ChunkType {
    static constexpr uint32_t IDAT = 0x49444154;
    static constexpr uint32_t IHDR = 0x49484452;
    static constexpr uint32_t PLTE = 0x504C5445;
    static constexpr uint32_t IEND = 0x49454E44;
};

// DEFLATE BUFFER SIZE
constexpr size_t DEFLATE_BUFF_SIZE = 4096;

// COLOR TYPES
struct ColorType {
    static constexpr uint8_t RGB = 2;
    static constexpr uint8_t PLTE = 3;
    static constexpr uint8_t RGB_ALPHA = 6;
    static constexpr uint8_t GRAYSCALE = 0;
    static constexpr uint8_t GRAYSCALE_ALPHA = 4;
};

// FILTER TYPES
struct FilterType {
    static constexpr uint8_t UP = 2;
    static constexpr uint8_t SUB = 1;
    static constexpr uint8_t NONE = 0;
    static constexpr uint8_t PAETH = 4;
    static constexpr uint8_t AVERAGE = 3;
};

// INTERLACE PARAMS
constexpr uint32_t IS_INTERLACED = 1;
constexpr uint32_t NON_INTERLACED = 0;

struct Interlace {
    static constexpr size_t NUM_PASSES = 7;
    static constexpr size_t STARTING_ROW[NUM_PASSES] = {0, 0, 4, 0, 2, 0, 1};
    static constexpr size_t STARTING_COL[NUM_PASSES] = {0, 4, 0, 2, 0, 1, 0};
    static constexpr size_t ROW_INCREMENT[NUM_PASSES] = {8, 8, 8, 4, 4, 2, 2};
    static constexpr size_t COL_INCREMENT[NUM_PASSES] = {8, 8, 4, 4, 2, 2, 1};
    static constexpr size_t BLOCK_HEIGHT[NUM_PASSES] = {8, 8, 4, 4, 2, 2, 1};
    static constexpr size_t BLOCK_WIDTH[NUM_PASSES] = {8, 4, 4, 2, 2, 1, 1};
};
