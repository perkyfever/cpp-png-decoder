# PNG Decoder

## Problem

PNG is one of the most popular formats for lossless image compression and one of the simplest. It supports grayscale images, regular rgb, indexed images (with a fixed palette) and also
can contain alpha-channel (transparency).

Specification: http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html.


Decoding pipeline:

1. Read signature bytes and validate it.
2. Read chunks until you get IEND chunk. Validate CRC of each chunk.
3. Save the information provided by IHDR and PLTE chunks, since you'll need it later.
4. Concatenate the content of all IDAT chunks into a single byte vector (lets call it `B`). You don't need to implement decoding on the fly in this task.
5. Once you've reached IEND, you can start decoding `B`. Use deflate to decompress `B` into another byte vector `R`.
6. Process `R` as described in the specification. Namely, you should process `R` by scanlines, applying specified filters. For non-interlaced images you need to do only 1 pass. In interlaced images, all 7 passes are concatenated in `R`.


The entry point is `Image ReadPng(std::string_view filename)` function in `png_decoder.h`.

External libraries used for deflating and crc calculation:
1. zlib1g-dev (zlib)
2. boost (for CRC)
