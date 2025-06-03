#pragma once
#include <string>

namespace encryptor {
    auto png(const std::string &filepath, const std::string &message, const std::string &key) -> void;

    auto bmp(const std::string &filepath, const std::string &message) -> void;
}
