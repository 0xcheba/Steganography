#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "decryptor.hpp"

namespace decryptor {
    const std::string RED = "\033[1;31m";
    const std::string RESET = "\033[0m";
    const std::string NO_FILE_AVAILABLE = "\n" + RED + "You have no privilege to read this file or file not found!" + RESET;

    auto png(const std::string &filepath, const std::string &key) -> std::string {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        char name[5];
        std::memcpy(name, "bEGN", 4);
        name[4] = '\0';
        char name_check[5];
        std::vector<uint8_t> xored_data;

        // collects data grom custom chunks
        file.seekg(8, std::ios::beg);
        while (file) {
            uint32_t length;
            file.read(reinterpret_cast<char *>(&length), 4);
            length = __builtin_bswap32(length);
            file.read(reinterpret_cast<char *>(&name_check), 4);
            name_check[4] = '\0';

            if (std::strcmp(name_check, name) == 0) {
                for (size_t i = 0; i < length - 4; i++)
                    xored_data.push_back(file.get());
                file.read(reinterpret_cast<char *>(&name), 4);
                for (size_t i = 0; i < 4; i++)
                    name[i] ^= 4;
                file.seekg(8, std::ios::beg);
            } else
                file.seekg(length + 4, std::ios::cur);
            if (std::strcmp(name_check, "IDAT") == 0)
                break;
        }

        file.close();

        std::vector<uint8_t> data;
        std::string message;

        // decrypts data
        for (size_t i = 0; i < xored_data.size(); i++)
            data.push_back(static_cast<uint8_t>(xored_data[i] ^ key[i % key.size()]));

        for (const unsigned char i: data)
            message.push_back(static_cast<char>(i));

        return message;
    }

    auto bmp(const std::string &filepath, const int &length) -> std::string {
        uint32_t off_bits;
        std::vector<uint8_t> bytes;

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        // collects bytes from the file
        file.seekg(10, std::ios::beg);
        file.read(reinterpret_cast<char *>(&off_bits), 4);
        file.seekg(off_bits, std::ios::beg);
        for (size_t i = 0; i < length * 8; i++) {
            bytes.push_back(static_cast<uint8_t>(file.get()));
            file.seekg(2, std::ios::cur);
        }

        std::vector<uint8_t> bits;

        for (const uint8_t byte : bytes)
            bits.push_back(byte & 1);

        uint8_t byte = 0;
        std::string message;

        // build bytes from bits
        for (size_t i = 0; i < bits.size(); i++) {
            byte = (byte << 1) | bits[i];
            if ((i + 1) % 8 == 0) {
                message.push_back(static_cast<char>(byte));
                bytes.clear();
            }
        }

        return message;
    }
}
