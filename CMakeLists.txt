cmake_minimum_required(VERSION 3.16)
project(png-decoder)

find_package(PNG)

if (NOT DEFINED PNG_FOUND)
    message(FATAL_ERROR "Can't find libpng")
endif()

include(build.cmake)
add_executable(test_png_decoder
        test.cpp
        test_commons.hpp
        libpng_wrappers.h
)

target_link_libraries(test_png_decoder ${PNG_STATIC} ${PNG_LIBRARY})
