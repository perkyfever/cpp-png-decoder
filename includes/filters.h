#pragma once

#include <cstdint>
#include <vector>

void SubFilter(std::vector<uint8_t> &cur_scanline, size_t bpp);
void UpFilter(std::vector<uint8_t> &cur_scanline, std::vector<uint8_t> &prev_scanline);
void AverageFilter(
    std::vector<uint8_t> &cur_scanline,
    std::vector<uint8_t> &prev_scanline,
    size_t bpp
);
void PaethFilter(
    std::vector<uint8_t> &cur_scanline,
    std::vector<uint8_t> &prev_scanline,
    size_t bpp
);
