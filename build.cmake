find_package(ZLIB REQUIRED)
find_package(Boost REQUIRED)

include_directories(includes)

add_library(deflate OBJECT
        src/deflate.cpp
        includes/deflate.h
        includes/constants.h
)
target_link_libraries(deflate ${ZLIB_LIBRARIES})

add_library(crc_validation OBJECT
        src/crc_validation.cpp
        includes/crc_validation.h
)
target_link_libraries(crc_validation ${Boost_LIBRARIES})

add_library(png-decoder
        src/png_decoder.cpp
        includes/png_decoder.h
        src/readers.cpp
        includes/readers.h
        src/reverse_filter.cpp
        includes/reverse_filter.h
        src/filters.cpp
        includes/filters.h
)
target_link_libraries(png-decoder PRIVATE deflate crc_validation)
set(PNG_STATIC png-decoder)
