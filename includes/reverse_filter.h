#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include "image.h"

uint32_t ScanlineSize(uint8_t color_type, uint8_t bit_depth, uint32_t width);
std::vector<std::vector<RGB>> ReverseFilterWindow(
    uint32_t width,
    uint32_t height,
    uint8_t bit_depth,
    uint8_t color_type,
    std::vector<uint8_t> &data_bytes,
    std::vector<std::array<uint8_t, 3>> &plte_bytes
);
