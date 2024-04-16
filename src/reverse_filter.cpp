#include "reverse_filter.h"
#include <cstring>
#include "constants.h"
#include "filters.h"
#include "readers.h"

constexpr uint32_t NumSamples(uint8_t color_type) {
    if (color_type == ColorType::RGB) {
        return 3;
    } else if (color_type == ColorType::RGB_ALPHA) {
        return 4;
    } else if (color_type == ColorType::GRAYSCALE_ALPHA) {
        return 2;
    } else if (color_type == ColorType::GRAYSCALE || color_type == ColorType::PLTE) {
        return 1;
    }
    throw std::runtime_error("Wrong color type");
}

// filter_type byte + data itself
uint32_t ScanlineSize(uint8_t color_type, uint8_t bit_depth, uint32_t width) {
    return 1 +
           (width * NumSamples(color_type) * static_cast<uint32_t>(bit_depth) + 7) / 8;
}

void ReverseFilterScanline(
    uint8_t filter_type,
    std::vector<uint8_t> &cur_scanline,
    std::vector<uint8_t> &prev_scanline,
    size_t bpp
) {
    if (filter_type == FilterType::SUB) {
        SubFilter(cur_scanline, bpp);
    } else if (filter_type == FilterType::UP) {
        UpFilter(cur_scanline, prev_scanline);
    } else if (filter_type == FilterType::AVERAGE) {
        AverageFilter(cur_scanline, prev_scanline, bpp);
    } else if (filter_type == FilterType::PAETH) {
        PaethFilter(cur_scanline, prev_scanline, bpp);
    } else if (filter_type != FilterType::NONE) {
        throw std::runtime_error("Wrong filter type");
    }
}

// rely on bit depth <= byte
RGB RecoverPixel(
    uint8_t bit_depth,
    uint8_t color_type,
    std::vector<uint8_t> &scanline,
    std::vector<std::array<uint8_t, 3>> &plte,
    uint32_t col
) {
    BitReader bit_reader(scanline);
    if (color_type == ColorType::GRAYSCALE) {
        size_t sam_per_byte = 8u / bit_depth;
        size_t byte = col / sam_per_byte, bit = col % sam_per_byte;
        // big endian
        uint8_t sample =
            bit_reader.GetNBits(byte, bit_depth * (sam_per_byte - 1 - bit), bit_depth);
        return RGB{sample, sample, sample, 255};
    } else if (color_type == ColorType::RGB) {
        size_t offset = NumSamples(color_type) * col;
        return RGB{scanline[offset], scanline[offset + 1], scanline[offset + 2], 255};
    } else if (color_type == ColorType::PLTE) {
        size_t sam_per_byte = 8u / bit_depth;
        size_t byte = col / sam_per_byte, bit = col % sam_per_byte;
        // big endian
        uint8_t sample =
            bit_reader.GetNBits(byte, bit_depth * (sam_per_byte - 1 - bit), bit_depth);
        return RGB{plte[sample][0], plte[sample][1], plte[sample][2], 255};
    } else if (color_type == ColorType::GRAYSCALE_ALPHA) {
        size_t offset = NumSamples(color_type) * col;
        return RGB{
            scanline[offset], scanline[offset], scanline[offset], scanline[offset + 1]};
    } else if (color_type == ColorType::RGB_ALPHA) {
        size_t offset = NumSamples(color_type) * col;
        return RGB{
            scanline[offset], scanline[offset + 1], scanline[offset + 2],
            scanline[offset + 3]};
    } else {
        throw std::runtime_error("Wrong color type");
    }
}

std::vector<std::vector<RGB>> ReverseFilterWindow(
    uint32_t width,
    uint32_t height,
    uint8_t bit_depth,
    uint8_t color_type,
    std::vector<uint8_t> &data_bytes,
    std::vector<std::array<uint8_t, 3>> &plte_bytes
) {
    uint32_t scanline_size = ScanlineSize(color_type, bit_depth, width);
    // bpp is defined as the number of bytes per complete pixel, rounding up to one
    size_t bpp =
        std::max(NumSamples(color_type) * static_cast<uint32_t>(bit_depth) / 8u, 1u);

    size_t cur = 1;  // swapping current and previous scanlines
    std::vector<std::vector<RGB>> reverse_filtered;
    std::vector<uint8_t> scanlines[2];
    scanlines[0].resize(scanline_size - 1);
    scanlines[1].resize(scanline_size - 1);

    for (size_t row = 0; row < height; ++row) {
        reverse_filtered.emplace_back();
        uint8_t filter_type = data_bytes[row * scanline_size];
        std::memcpy(
            scanlines[cur].data(), &data_bytes[row * scanline_size + 1], scanline_size - 1
        );
        ReverseFilterScanline(filter_type, scanlines[cur], scanlines[1 - cur], bpp);
        for (size_t col = 0; col < width; ++col) {
            RGB rec_pix =
                RecoverPixel(bit_depth, color_type, scanlines[cur], plte_bytes, col);
            reverse_filtered.back().emplace_back(rec_pix);
        }
        cur = 1 - cur;
    }

    return reverse_filtered;
}
