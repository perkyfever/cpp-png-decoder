#include "deflate.h"
#include <zlib.h>
#include <stdexcept>
#include "constants.h"

int DecompressBatch(
    z_stream &stream,
    std::vector<uint8_t> &buffer,
    std::vector<uint8_t> &decompressed
) {
    stream.next_out = buffer.data();
    stream.avail_out = buffer.size();
    int ret = inflate(&stream, Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR) {
        throw std::runtime_error("Failed on decompressing data");
    }

    while (!stream.avail_out) {
        size_t dcmp_bytes = buffer.size() - stream.avail_out;
        decompressed.reserve(decompressed.size() + dcmp_bytes);
        for (size_t i = 0; i < dcmp_bytes; ++i) {
            decompressed.push_back(buffer[i]);
        }
        stream.next_out = buffer.data();
        stream.avail_out = buffer.size();
        ret = inflate(&stream, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            throw std::runtime_error("Failed on decompressing data");
        }
    }

    size_t dcmp_bytes = DEFLATE_BUFF_SIZE - stream.avail_out;
    decompressed.reserve(decompressed.size() + dcmp_bytes);
    for (size_t i = 0; i < dcmp_bytes; ++i) {
        decompressed.push_back(buffer[i]);
    }

    return ret;
}

std::vector<uint8_t> Decompress(std::vector<uint8_t> &compressed) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = Z_NULL;
    stream.next_in = Z_NULL;
    int ret = inflateInit(&stream);
    if (ret != Z_OK) {
        throw std::runtime_error("Initializing zlib");
    }

    std::vector<uint8_t> decompressed;
    std::vector<uint8_t> buffer(DEFLATE_BUFF_SIZE);

    stream.next_in = compressed.data();
    stream.avail_in = std::min(DEFLATE_BUFF_SIZE, compressed.size());
    size_t dcmp = stream.avail_in;
    ret = DecompressBatch(stream, buffer, decompressed);
    while (ret != Z_STREAM_END) {
        stream.next_in = compressed.data() + dcmp;
        stream.avail_in = std::min(DEFLATE_BUFF_SIZE, compressed.size() - dcmp);
        dcmp += stream.avail_in;
        ret = DecompressBatch(stream, buffer, decompressed);
    }

    ret = inflateEnd(&stream);
    if (ret != Z_OK) {
        throw std::runtime_error("Failed on closing zstream");
    }

    return decompressed;
}
