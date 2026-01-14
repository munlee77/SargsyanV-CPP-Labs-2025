#include "caesar_cipher.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        return CaesarCipher::runApplication(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}
