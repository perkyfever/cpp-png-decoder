#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>
#include "image.h"

class Decoder {
public:
    Decoder(std::istream &input);
    Image DecodeImage();
    ~Decoder() = default;

private:
    void ReadIHDR(std::istream &input);
    static void ValidateSignature(std::istream &input);
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t filter_method;
    uint8_t interlace_method;
    uint8_t compression_method;
    std::vector<uint8_t> data_bytes;
    std::vector<std::array<uint8_t, 3>> plte_bytes;  // <R, G, B>
};

Image ReadPng(std::string_view filename);
