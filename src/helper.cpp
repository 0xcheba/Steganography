#include <fstream>
#include <iostream>
#include <random>

#include "helper.hpp"

namespace helper {
    const std::string RED = "\033[1;31m";
    const std::string RESET = "\033[0m";

    const std::string NO_FILE_AVAILABLE = "\n" + RED + "You have no privilege to read this file or file not found!" + RESET;

    auto generator() -> std::string {
        std::string chunkName;
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> lower{'a', 'z'};
        std::uniform_int_distribution<> upper{'A', 'Z'};

        chunkName = static_cast<char>(lower(generator));

        int counter = 0;

        while (counter < 3) {
            chunkName += static_cast<char>(upper(generator));
            counter++;
        }

        return chunkName;
    }

    auto checker(const std::string protectedNames[], std::vector<std::string> namesExist,
                 const std::string &name) -> bool {
        const bool checkExist = std::ranges::find(namesExist, name) != namesExist.end();
        bool checkProtected = false;

        for (int i = 0; i < 16; i++) {
            if (name == protectedNames[i])
                checkProtected = true;
        }

        if (!checkProtected && !checkExist) {
            return false;
        }
        return true;
    }

    // generates table with bytes to make counting crc faster
    auto crc32::gen_table(uint32_t (&table)[256]) -> void {
        for (int i = 0; i < 256; i++) {

            uint32_t c = i;
            for (int j = 0; j < 8; j++) {
                constexpr uint32_t polynomial = 0xEDB88320;

                if (c & 1)
                    c = polynomial ^ (c >> 1);
                else
                    c >>= 1;
            }

            table[i] = c;
        }
    }

    //counts crc
    auto crc32::update(const uint32_t (&table)[256], const uint32_t initial, const std::vector<uint8_t> &data) -> uint32_t {
        uint32_t c = initial ^ 0xFFFFFFFF;

        for (const auto byte: data)
            c = table[(c ^ byte) & 0xFF] ^ (c >> 8);
        return c ^ 0xFFFFFFFF;
    }

    auto chunk_builder(const int &chunkAmount, std::string protectedNames[],
                       std::vector<std::string> &namesExist, std::string chunkName,
                       std::vector<std::string> &pieces, const std::string &key,
                       const bool is_junk, std::vector<std::vector<uint8_t> > &created_chunks,
                       const uint32_t &content_length) -> void {
        for (int i = 0; i < chunkAmount; i++) {
            std::vector<uint8_t> chunk;
            std::vector<uint8_t> junk;

            if (!is_junk) {
                chunk.push_back((content_length >> 24) & 0xFF);
                chunk.push_back((content_length >> 16) & 0xFF);
                chunk.push_back((content_length >> 8) & 0xFF);
                chunk.push_back((content_length) & 0xFF);
            } else {
                junk.push_back((content_length >> 24) & 0xFF);
                junk.push_back((content_length >> 16) & 0xFF);
                junk.push_back((content_length >> 8) & 0xFF);
                junk.push_back((content_length) & 0xFF);
            }

            // chunk name generator

            std::string nextChunkName;

            bool exists = true;

            while (exists) {
                nextChunkName = helper::generator();
                exists = checker(protectedNames, namesExist, nextChunkName);
            }

            namesExist.push_back(nextChunkName);

            std::vector<uint8_t> temp_chunk;
            std::vector<uint8_t> temp_junk;

            for (const char b: chunkName) {
                if (!is_junk)
                    temp_chunk.push_back(b);
                else
                    temp_junk.push_back(b);
            }

            // chunk xored content generator

            std::string &chunkContent = pieces[i];

            if (!is_junk) {
                for (int b = 0; b < chunkContent.size(); b++)
                    temp_chunk.push_back(static_cast<uint8_t>(chunkContent[b] ^ key[b % key.size()]));

                for (const char b: nextChunkName)
                    temp_chunk.push_back(static_cast<uint8_t>(b ^ 4));
            } else {
                std::random_device random_device;
                std::mt19937 generator(random_device());
                std::uniform_int_distribution<> bytes{0, 127};

                for (int b = 0; b < chunkContent.size(); b++)
                    temp_junk.push_back(static_cast<uint8_t>(bytes(generator)));

                for (const char b: nextChunkName)
                    temp_junk.push_back(static_cast<uint8_t>(b ^ 4));
            }

            chunkName = nextChunkName;

            if (!is_junk)
                chunk.insert(chunk.end(), temp_chunk.begin(), temp_chunk.end());
            else
                junk.insert(junk.end(), temp_junk.begin(), temp_junk.end());

            // CRC generator

            uint32_t table[256];
            crc32::gen_table(table);
            if (!is_junk) {
                const uint32_t crc = crc32::update(table, 0, temp_chunk);

                for (int b = 0; b <= 24; b += 8)
                    chunk.push_back((crc >> b) & 0xff);
            } else {
                const uint32_t crc = crc32::update(table, 0, temp_junk);

                for (int b = 0; b <= 24; b += 8)
                    junk.push_back((crc >> b) & 0xff);
            }

            if (!is_junk)
                created_chunks.push_back(chunk);
            else
                created_chunks.push_back(junk);
        }
    }

    auto chunk_injector(const std::string &filepath, const std::vector<std::vector<uint8_t> > &chunks) -> void {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        std::vector<uint8_t> original((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        size_t starts_from = 8;

        while (starts_from + 8 <= original.size()) {
            const uint32_t len = __builtin_bswap32(*reinterpret_cast<uint32_t *>(&original[starts_from]));
            std::string chunk_name(reinterpret_cast<char *>(&original[starts_from + 4]), 4);

            if (chunk_name == "IDAT")
                break;

            starts_from += 8 + len + 4;
        }

        std::vector<uint8_t> new_pic;
        new_pic.insert(new_pic.end(), original.begin(), original.begin() + static_cast<int>(starts_from));

        for (auto &chunk: chunks) {
            new_pic.insert(new_pic.end(), chunk.begin(), chunk.end());
        }

        new_pic.insert(new_pic.end(), original.begin() + static_cast<int>(starts_from), original.end());

        std::string new_filename = filepath;
        new_filename.resize(new_filename.size() - 4);
        new_filename = new_filename + "_mod_.png";
        std::ofstream file_out(new_filename, std::ios::binary);
        if (!file_out.is_open())
            throw std::runtime_error(NO_FILE_AVAILABLE);

        file_out.write(reinterpret_cast<char *>(new_pic.data()), new_pic.size());
    }
}
