#include "test_commons.hpp"


int main() {

    CheckImage("logo.png");
    CheckImage("lenna_grayscale.png");
    CheckImage("lenna_index.png");
    CheckImage("logo_alpha.png");
    CheckImage("1.png");
    CheckImage("inter.png");
    CheckImage("alpha_grayscale.png");
    try {
        CheckImage("crc.png");
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }

    return 0;
}
