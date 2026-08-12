#pragma once

#include <string>
#include <vector>

enum TokenType {
    INT_DATATYPE_T,
    FIXED_DATATYPE_T,
    POINTER_DATATYPE_T,
    VOID_DATATYPE_T
};

struct LexedToken {
    TokenType type;
    std::string value;
    std::string fileName;
    size_t line;
    size_t column;
};

const std::string WHITESPACE = " \t\n\r\f\v";

void lexFile(std::string &fileName, std::vector<LexedToken>& lexedTokens);
std::string trim(std::string untrimmed);

