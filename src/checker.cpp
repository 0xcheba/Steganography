#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include "checker.hpp"

namespace checker {
    const std::string RED = "\033[1;31m";
    const std::string RESET = "\033[0m";

    const std::string NO_FILE_AVAILABLE = "\n" + RED + "You have no privilege to read this file or file not found!" + RESET;
    const std::string FILE_MUST_BE_PNG_OR_BMP = "\n" + RED + "File must be a .png or a .bmp/.dib!" + RESET;

    // checks file extension
    auto extensions(const std::string &filepath) -> std::string {
        const size_t slash = filepath.find_last_of('/');
        const std::string filename = filepath.substr(slash + 1);
        const size_t point = filename.find_last_of('.');

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);


        if (point == std::string::npos)
            throw std::runtime_error("\n" + RED + "There is no file given, or the file has no extension!" + RESET);

        const std::string extension = filename.substr(point + 1);

        if (extension == "png")
            return "png";
        if (extension == "bmp" || extension == "dib")
            return "bmp";
        throw std::runtime_error(FILE_MUST_BE_PNG_OR_BMP);
    }

    // checks file magic number
    auto magics(const std::string &filepath) -> std::string {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        const unsigned char png[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        unsigned char bufPng[8];
        file.read(reinterpret_cast<char *>(bufPng), 8);
        const int chkPng = std::memcmp(png, bufPng, 8);

        file.seekg(0);

        constexpr unsigned char bmp[] = {0x42, 0x4d};
        unsigned char bufBmp[2];
        file.read(reinterpret_cast<char *>(bufBmp), 2);
        const int chkBmp = std::memcmp(bmp, bufBmp, 2);

        file.close();

        if (chkPng == 0)
            return "png";
        if (chkBmp == 0)
            return "bmp";
        throw std::runtime_error(FILE_MUST_BE_PNG_OR_BMP);
    }

    // checks bmp/dib file attributes
    auto bmp(const std::string &filepath, const std::string &message) -> bool {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

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

        if (bit_count != 24) {
            std::cerr << RED + "File bitCount must be 24-bit! " << bit_count << "-bit provided." + RESET <<
                    std::endl;
            return false;
        }

        if (compression != 0) {
            std::cerr << RED + "Pixels in a file must not be compressed!" + RESET << std::endl;
            return false;
        }

        if (width * height < message.size() * 8 + 1) {
            std::cerr << RED + "Image is too small!" + RESET << std::endl;
            return false;
        }

        file.close();

        return true;
    }

    // checks if png file is available
    auto png(const std::string &filepath) -> bool {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        file.close();
        return true;
    }
}
