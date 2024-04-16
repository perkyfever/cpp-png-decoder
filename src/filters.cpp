#include "filters.h"

static const uint16_t MASK = 0xFF;

void SubFilter(std::vector<uint8_t> &cur_scanline, size_t bpp) {
    uint16_t prev = 0;
    for (size_t i = 0; i < cur_scanline.size(); ++i) {
        if (bpp <= i)
            prev = cur_scanline[i - bpp];
        cur_scanline[i] = (static_cast<uint16_t>(cur_scanline[i]) + prev) & MASK;
    }
}

void UpFilter(std::vector<uint8_t> &cur_scanline, std::vector<uint8_t> &prev_scanline) {
    for (size_t i = 0; i < cur_scanline.size(); ++i) {
        cur_scanline[i] = (static_cast<uint16_t>(cur_scanline[i]) +
                           static_cast<uint16_t>(prev_scanline[i])) &
                          MASK;
    }
}

void AverageFilter(
    std::vector<uint8_t> &cur_scanline,
    std::vector<uint8_t> &prev_scanline,
    size_t bpp
) {
    uint16_t prev = 0;
    for (size_t i = 0; i < cur_scanline.size(); ++i) {
        if (bpp <= i)
            prev = cur_scanline[i - bpp];
        uint16_t average = (static_cast<uint16_t>(prev_scanline[i]) + prev) / 2;
        cur_scanline[i] = (static_cast<uint16_t>(cur_scanline[i]) + average) & MASK;
    }
}

uint16_t PaethPredictor(int a, int b, int c) {
    int p = a + b - c;
    int pa = std::abs(p - a);
    int pb = std::abs(p - b);
    int pc = std::abs(p - c);
    if (pa <= pb && pa <= pc) {
        return static_cast<uint16_t>(a);
    } else if (pb <= pc) {
        return static_cast<uint16_t>(b);
    }
    return static_cast<uint16_t>(c);
}

void PaethFilter(
    std::vector<uint8_t> &cur_scanline,
    std::vector<uint8_t> &prev_scanline,
    size_t bpp
) {
    int16_t prev = 0;
    int16_t up_prev = 0;
    for (size_t i = 0; i < cur_scanline.size(); ++i) {
        if (bpp <= i) {
            prev = cur_scanline[i - bpp];
            up_prev = prev_scanline[i - bpp];
        }
        uint16_t predicted =
            PaethPredictor(prev, static_cast<uint16_t>(prev_scanline[i]), up_prev);
        cur_scanline[i] = (static_cast<uint16_t>(cur_scanline[i]) + predicted) & MASK;
    }
}
