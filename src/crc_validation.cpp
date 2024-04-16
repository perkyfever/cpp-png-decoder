#include "crc_validation.h"
#include <boost/crc.hpp>

void ValidateCRC(uint32_t type, std::vector<uint8_t> &data, uint32_t actual_crc) {
    boost::crc_32_type crc;
    crc.process_bytes(&type, sizeof(type));
    crc.process_bytes(data.data(), data.size());
    if (crc.checksum() != actual_crc) {
        throw std::runtime_error("CRC mismatch");
    }
}
