#include "png_decoder.h"
#include <cstring>
#include <fstream>
#include "constants.h"
#include "crc_validation.h"
#include "deflate.h"
#include "readers.h"
#include "reverse_filter.h"

void Decoder::ValidateSignature(std::istream &input) {
    std::vector<uint8_t> signature(Signature::SIZE);
    ReadBytes(input, signature.data(), "Reading signature", Signature::SIZE);
    if (std::memcmp(signature.data(), Signature::SIGNATURE, Signature::SIZE) != 0) {
        throw std::runtime_error("Failed on validating signature");
    }
}

void Decoder::ReadIHDR(std::istream &input) {
    uint32_t length, type;
    ReadBytes(input, &length, "Reading IHDR chunk length");
    ReadBytes(input, &type, "Reading IHDR chunk type");

    type = SwapEndian(type);
    length = SwapEndian(length);
    if (length != IHDR_CHUNK_SIZE) {
        throw std::runtime_error("IHDR chunk data is of wrong length");
    } else if (type != ChunkType::IHDR) {
        throw std::runtime_error("First chunk is not IHDR chunk");
    }

    std::vector<uint8_t> data(length);
    ReadBytes(input, data.data(), "Reading IHDR meta data", length);

    uint32_t crc;
    ReadBytes(input, &crc, "Reading IHDR crc");
    ValidateCRC(SwapEndian(type), data, SwapEndian(crc));

    width = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    height = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    bit_depth = data[8];
    color_type = data[9];
    compression_method = data[10];
    filter_method = data[11];
    interlace_method = data[12];
    // Checking the constraints
    // http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html
    if (filter_method != 0) {
        throw std::runtime_error("Filter method must be 0");
    }
    if (compression_method != 0) {
        throw std::runtime_error("Compression method must be 0");
    }
    if (interlace_method != IS_INTERLACED && interlace_method != NON_INTERLACED) {
        throw std::runtime_error("Wrong interlaced method");
    }
    if (bit_depth > MAX_BIT_DEPTH) {
        throw std::runtime_error("Unsupported bit depth");
    }
}

Decoder::Decoder(std::istream &input) {
    ValidateSignature(input);
    ReadIHDR(input);  // must init meta data fields

    PNGChunk chunk = ReadChunk(input);
    while (chunk.type != ChunkType::IEND) {
        if (chunk.type == ChunkType::IDAT) {
            data_bytes.insert(data_bytes.end(), chunk.bytes.begin(), chunk.bytes.end());
        } else if (chunk.type == ChunkType::PLTE) {
            if (!plte_bytes.empty()) {
                throw std::runtime_error("There must not be more than one PLTE chunk");
            }
            if (chunk.length % 3 != 0) {
                throw std::runtime_error("PLTE chunk length is not divisible by 3");
            }
            plte_bytes.reserve(chunk.length / 3);
            for (size_t i = 0; i < chunk.length; i += 3) {
                plte_bytes.push_back(
                    {chunk.bytes[i], chunk.bytes[i + 1], chunk.bytes[i + 2]}
                );
            }
        }
        chunk = ReadChunk(input);
    }

    if (input.peek() != std::ifstream::traits_type::eof()) {
        throw std::runtime_error("IEND found but stream has more data");
    }
}

Image Decoder::DecodeImage() {
    // Decompress concatenated data bytes using zlib
    std::vector<uint8_t> decompressed = Decompress(data_bytes);
    Image im(static_cast<int>(height), static_cast<int>(width));

    // Decode the original image according to interlace method
    std::vector<std::vector<RGB>> window;
    if (interlace_method == NON_INTERLACED) {
        window = ReverseFilterWindow(
            width, height, bit_depth, color_type, decompressed, plte_bytes
        );
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                im(row, col) = window[row][col];
            }
        }
    } else {
        size_t cur_pass = 0;
        for (size_t pass = 0; pass < Interlace::NUM_PASSES; ++pass) {
            size_t col_inc = Interlace::COL_INCREMENT[pass];
            size_t row_inc = Interlace::ROW_INCREMENT[pass];
            size_t start_col = Interlace::STARTING_COL[pass];
            size_t start_row = Interlace::STARTING_ROW[pass];
            size_t window_width = (width - start_col + col_inc - 1) / col_inc;
            size_t window_height = (height - start_row + row_inc - 1) / row_inc;
            size_t window_size =
                window_height * ScanlineSize(color_type, bit_depth, window_width);
            if (window_size > 0) {
                std::vector<uint8_t> current_pass(window_size);
                std::memcpy(
                    &current_pass[0], &decompressed[cur_pass], current_pass.size()
                );
                window = ReverseFilterWindow(
                    window_width, window_height, bit_depth, color_type, current_pass,
                    plte_bytes
                );
                for (size_t row = 0; row < window_height; ++row) {
                    for (size_t col = 0; col < window_width; ++col) {
                        size_t r = start_row + row * row_inc;
                        size_t c = start_col + col * Interlace::COL_INCREMENT[pass];
                        im(r, c) = window[row][col];
                    }
                }
            }
            cur_pass += window_size;
        }
    }

    return im;
}

Image ReadPng(std::string_view filename) {
    std::ifstream pic(filename.data(), std::ios::binary);
    if (!pic.is_open()) {
        throw std::runtime_error("Could not open the picture");
    }
    Decoder decoder(pic);
    Image image = decoder.DecodeImage();
    std::cout << "Decoded successfully!" << std::endl;
    return image;
}
