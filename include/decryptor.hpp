#pragma once
#include <string>

namespace decryptor {
    auto png(const std::string &filepath, const std::string &key) -> std::string;

    auto bmp(const std::string &filepath, const int &length) -> std::string;
}
