#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace helper {
    auto generator() -> std::string;

    auto checker(const std::string protectedNames[], std::vector<std::string> namesExist,
                 const std::string &name) -> bool;

    struct crc32 {
        static auto gen_table(uint32_t (&table)[256]) -> void;

        static auto update(const uint32_t (&table)[256], uint32_t initial,
                           const std::vector<uint8_t> &data) -> uint32_t;
    };

    auto chunk_builder(const int &chunkAmount, std::string protectedNames[],
                       std::vector<std::string> &namesExist, std::string chunkName,
                       std::vector<std::string> &pieces, const std::string &key,
                       bool is_junk, std::vector<std::vector<uint8_t> > &created_chunks,
                       const uint32_t &content_length) -> void;

    auto chunk_injector(const std::string &filepath, const std::vector<std::vector<uint8_t> > &chunks) -> void;
}
