#pragma once
#include <iostream>
#include <string>

struct TokenLocationInfo {
    const std::string fileName;
    const size_t line;
    const size_t column;

    [[nodiscard]] std::string toString() const {
        return fileName +"("+std::to_string(line)+"):" + std::to_string(column);
    }
};

inline std::ostream& operator <<(std::ostream& os, const TokenLocationInfo& info) {
    return os << info.fileName  << "(" << info.line << "):" << info.column;
}
