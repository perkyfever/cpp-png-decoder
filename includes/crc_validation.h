#pragma once

#include <cstdint>
#include <vector>

void ValidateCRC(uint32_t type, std::vector<uint8_t> &data, uint32_t actual_crc);
