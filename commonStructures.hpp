#pragma once
#include <string>

struct TokenLocationInfo {
    const std::string fileName;
    const size_t line;
    const size_t column;
};
