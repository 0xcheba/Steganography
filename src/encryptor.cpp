#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include "encryptor.hpp"
#include "helper.hpp"

namespace encryptor {
    const std::string RED = "\033[1;31m";
    const std::string YELLOW = "\033[1;33m";
    const std::string RESET = "\033[0m";

    void png(const std::string &filepath, const std::string &message, const std::string &key) {
        // chunk amount and chunk length generator
        const auto messageLength = static_cast<int>(message.length());
        int chunkConLen = 0;
        int chunkAmount = 0;
        std::vector<int> dividers;

        for (int i = 1; i <= messageLength; i++) {
            if (messageLength % i == 0)
                dividers.push_back(i);
        }

        if (dividers.size() == 2) {
            std::string res;
            std::cout << YELLOW + "The length of the message is a prime number. "
                    "The efficiency of hiding this message will be very low. Do you still want to proceed? (y/n) " + RESET;
            do
                std::cin >> res;
            while (res != "y" && res != "n");
            if (res == "y") {
                chunkConLen = messageLength;
                chunkAmount = 1;
            } else {
                std::cout << RED + "Encryption process aborted!" + RESET << std::endl;
                std::exit(0);
            }
        } else {
            chunkAmount = dividers[static_cast<int>(dividers.size()) / 2];
            chunkConLen = messageLength / chunkAmount;
        }

        std::vector<std::string> pieces;

        for (int i = 0; i <= chunkAmount; i++) {
            const int start = i * chunkConLen;
            pieces.push_back(message.substr(start, chunkConLen));
        }

        pieces.pop_back();

        std::string chunkName = "bEGN";
        std::string protectedNames[] = {
            "bEGN", "bKGD", "cHRM", "cICP", "dSIG", "eXIF", "gAMA", "hIST",
            "hIST", "iCCP", "sBIT", "sPLT", "sRGB", "sTER", "tIME", "tRNS", "sTRT"
        };

        std::vector<std::string> namesExist;
        std::vector<std::vector<uint8_t> > created_chunks;
        bool is_junk = false;

        helper::chunk_builder(chunkAmount, protectedNames, namesExist, chunkName, pieces, key, is_junk, created_chunks,
                              static_cast<uint32_t>(chunkConLen + 4));
        is_junk = true;
        chunkName = "sTRT";
        helper::chunk_builder(chunkAmount, protectedNames, namesExist, chunkName, pieces, key, is_junk, created_chunks,
                              static_cast<uint32_t>(chunkConLen + 4));

        std::random_device random;
        std::mt19937 generator(random());
        std::ranges::shuffle(created_chunks.begin(), created_chunks.end(), generator);

        helper::chunk_injector(filepath, created_chunks);
    }

    auto bmp(const std::string &filepath, const std::string &message) -> void {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("\n" + RED + "You have no privilege to read this file or file not found!" + RESET);

        uint32_t off_bits;

        int32_t width, height;
        uint16_t bit_count;
        uint32_t compression;

        file.seekg(10, std::ios::beg);
        file.read(reinterpret_cast<char *>(&off_bits), 4);
        file.seekg(18, std::ios::beg);
        file.read(reinterpret_cast<char *>(&width), 4);
        file.seekg(22, std::ios::beg);
        file.read(reinterpret_cast<char *>(&height), 4);
        file.seekg(28, std::ios::beg);
        file.read(reinterpret_cast<char *>(&bit_count), 2);
        file.seekg(30, std::ios::beg);
        file.read(reinterpret_cast<char *>(&compression), 4);

        const size_t row_size = ((width * 3 + 3) / 4) * 4;
        std::vector<uint8_t> rgb;
        const size_t vec_size = row_size * std::abs(height);
        rgb.resize(vec_size);

        file.seekg(off_bits, std::ios::beg);
        file.read(reinterpret_cast<char *>(rgb.data()), static_cast<long long>(vec_size));

        size_t bit_num = 0;

        // changes every 3-rd byte in pixels
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                size_t i = h * width + w * 3;
                if (bit_num >= message.size() * 8)
                    break;

                const uint8_t bit = (message[bit_num / 8] >> (7 - bit_num % 8)) & 1;
                rgb[i] &= 0xFE;
                rgb[i] |= bit;
                bit_num++;
            }
        }

        std::string new_filename = filepath;
        new_filename.resize(new_filename.size() - 4);
        new_filename = new_filename + "_mod_.bmp";
        std::ofstream file_out(new_filename, std::ios::binary);
        std::vector<char> begin(off_bits);
        file.seekg(0, std::ios::beg);
        file.read(begin.data(), off_bits);
        file.close();

        file_out.write(begin.data(), off_bits);
        file_out.write(reinterpret_cast<char *>(rgb.data()), static_cast<long long>(rgb.size()));
        file_out.close();
    }
}
