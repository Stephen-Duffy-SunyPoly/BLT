#pragma once

#include <string>
#include <vector>

enum TokenType {
    // data type key words
    INT_DATATYPE_T,
    FIXED_DATATYPE_T,
    POINTER_DATATYPE_T,
    VOID_DATATYPE_T,
    // structure key words
    FUNCTION_KEYWORD_T,
    INLINE_KEYWORD_T,
    IF_KEYWORD_T,
    ELSE_KEYWORD_T,
    RETURN_KEYWORD_T,
    WHILE_KEYWORD_T,
    FOR_KEYWORD_T,
    CONTINUE_KEYWORD_T,
    BREAK_KEYWORD_T,
    // operators
    LEFT_SHIFT_OPERATOR_T,
    RIGHT_SHIFT_OPERATOR_T,
    POINTER_VALUE_OPERATOR_T,
    LOGICAL_AND_OPERATOR_T,
    LOGICAL_OR_OPERATOR_T,
    ADD_TO_OPERATOR_T,
    SUBTRACT_FROM_OPERATOR_T,
    MULTIPLY_WITH_OPERATOR_T,
    DIVIDE_BY_OPERATOR_T,
    MODULO_BY_OPERATOR_T,
    ADD_OPERATOR_T,
    SUBTRACT_OPERATOR_T,
    MULTIPLY_OPERATOR_T,
    DIVIDE_OPERATOR_T,
    MODULO_OPERATOR_T,
    NEGATE_OPERATOR_T,
    BITWISE_AND_OPERATOR_T,
    BITWISE_OR_OPERATOR_T,
    BITWISE_XOR_OPERATOR_T,
    GREATER_THAN_OR_EQUAL_OPERATOR_T,
    LESS_THAN_OR_EQUAL_OPERATOR_T,
    EQUAL_OPERATOR_T,
    NOT_EQUAL_OPERATOR_T,
    GREATER_THAN_OPERATOR_T,
    LESS_THAN_OPERATOR_T,
    LOGICAL_NOT_OPERATOR_T,
    ASSIGNMENT_OPERATOR_T,
    // structure
    OPEN_BRACE_T,
    CLOSE_BRACE_T,
    OPEN_PAREN_T,
    CLOSE_PAREN_T,
    OPEN_SQUARE_BRACKET_T,
    CLOSE_SQUARE_BRACKET_T,
    COMMA_T,
    SEMICOLON_T,
    // variable data
    NUMBER_T,
    IDENTIFIER_T,
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

