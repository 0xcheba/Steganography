#pragma once
#include <string>

namespace checker {
    auto extensions(const std::string &filepath) -> std::string;

    auto magics(const std::string &filepath) -> std::string;

    auto bmp(const std::string &filepath, const std::string &message) -> bool;

    auto png(const std::string &filepath) -> bool;
}
