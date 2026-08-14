#include "lexer.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <re2/re2.h>
#include <filesystem>

namespace fs = std::filesystem;

static std::vector<std::string> handledFiles;

struct LexingInfo {
    RE2 *expression;
    TokenType type;
    bool returnsData;
    LexingInfo(const std::string& expression, TokenType type, bool returnsData):
        expression(new RE2(expression)), type(type), returnsData(returnsData) {}
};

static std::vector<LexingInfo> lexTokenInfo{
    //note: surround words with \\b to force matches on word boundaries

    // data type key words
    {"\\bint\\b",INT_DATATYPE_T,false},
    {"\\bfixed\\b",FIXED_DATATYPE_T,false},
    {"\\bptr\\b",POINTER_DATATYPE_T,false},
    {"\\bvoid\\b",VOID_DATATYPE_T,false},
    // structure key words
    {"\\bfunction\\b",FUNCTION_KEYWORD_T,false},
    {"\\binline\\b",INLINE_KEYWORD_T,false},
    {"\\bif\\b",IF_KEYWORD_T,false},
    {"\\belse\\b",ELSE_KEYWORD_T,false},
    {"\\breturn\\b",RETURN_KEYWORD_T,false},
    {"\\bwhile\\b",WHILE_KEYWORD_T,false},
    {"\\bfor\\b",FOR_KEYWORD_T,false},
    {"\\bcontinue\\b",CONTINUE_KEYWORD_T,false},
    {"\\bbreak\\b",BREAK_KEYWORD_T,false},
    // operators
    {"<<",LEFT_SHIFT_OPERATOR_T,false},
    {">>",RIGHT_SHIFT_OPERATOR_T,false},
    {"<\\-",POINTER_VALUE_OPERATOR_T,false},
    {"\\&\\&",LOGICAL_AND_OPERATOR_T,false},
    {"\\|\\|",LOGICAL_OR_OPERATOR_T,false},
    {"\\+=",ADD_TO_OPERATOR_T,false},
    {"\\-=",SUBTRACT_FROM_OPERATOR_T,false},
    {"\\*=",MULTIPLY_WITH_OPERATOR_T,false},
    {"/=",DIVIDE_BY_OPERATOR_T,false},
    {"%=",MODULO_BY_OPERATOR_T,false},
    {"\\+",ADD_OPERATOR_T,false},
    {"\\-", SUBTRACT_OPERATOR_T,false},
    {"\\*",MULTIPLY_OPERATOR_T,false},
    {"/",DIVIDE_OPERATOR_T,false},
    {"%",MODULO_OPERATOR_T,false},
    {"~",NEGATE_OPERATOR_T,false},
    {"\\&",BITWISE_AND_OPERATOR_T,false},
    {"\\|",BITWISE_OR_OPERATOR_T,false},
    {"\\^",BITWISE_XOR_OPERATOR_T,false},
    {">=",GREATER_THAN_OR_EQUAL_OPERATOR_T,false},
    {"<=",LESS_THAN_OR_EQUAL_OPERATOR_T,false},
    {"==",EQUAL_OPERATOR_T,false},
    {"!=",NOT_EQUAL_OPERATOR_T,false},
    {">",GREATER_THAN_OPERATOR_T,false},
    {"<",LESS_THAN_OPERATOR_T,false},
    {"!",LOGICAL_NOT_OPERATOR_T,false},
    {"=",ASSIGNMENT_OPERATOR_T,false},
    // structure
    {"\\{",OPEN_BRACE_T,false},
    {"\\}",CLOSE_BRACE_T,false},
    {"\\(",OPEN_PAREN_T,false},
    {"\\)",CLOSE_PAREN_T,false},
    {"\\[",OPEN_SQUARE_BRACKET_T,false},
    {"\\]",CLOSE_SQUARE_BRACKET_T,false},
    {",",COMMA_T,false},
    {";",SEMICOLON_T,false},
    // variable data
    {"([0-9]+\\.[0-9]*)",FIXED_POINT_NUMBER_T,true},
    {"(0x[0-9a-fA-F]+)",NUMBER_T,true},//hex numbers
    {"(0b[10]+)",NUMBER_T,true}, //binary numbers
    {"([0-9]+)",NUMBER_T,true},//decimal numbers
    {"([a-zA-Z][a-zA-Z0-9_]*)",IDENTIFIER_T,true},
    {"\"((?:\\\\.|[^\"\\\\])*)\"",STRING_LITERAL_T,true} //remember to remove the \ before any quotes leter

};

static RE2 whitespaceExpression("\\s+");
static RE2 lineCommentExpression("//.*");
static RE2 trimStringExpression("^\\s+|\\s+$");

std::string trim(std::string untrimmed) {
    RE2::GlobalReplace(&untrimmed, trimStringExpression,"");
    return untrimmed;
}

static void lexLine(std::string &line, std::vector<LexedToken> &lexedTokens, std::string &fileName, size_t lineNumber) {
    absl::string_view lineView(line);
    while (!lineView.empty()) {
        auto pos = static_cast<size_t>(reinterpret_cast<long long>(lineView.data()) - reinterpret_cast<long long>(line.data()));
        bool found = false;
        //parse through the any existing tokens
        for (auto &token : lexTokenInfo) {


            if (!token.returnsData) {
                if (RE2::Consume(&lineView, *token.expression)) {
                    lexedTokens.emplace_back(token.type, "", fileName,lineNumber,pos);
                    found = true;
                    break;
                }
            } else {
                std::string valueData;
                if (RE2::Consume(&lineView, *token.expression, &valueData)) {
                    lexedTokens.emplace_back(token.type, valueData, fileName,lineNumber,pos);
                    found = true;
                    break;
                }
            }
        }
        //attempt to remove whitespace
        if (RE2::Consume(&lineView, whitespaceExpression)) {
            continue;
        }
        //if we hit a line comment then eat the rest of the line
        if (RE2::Consume(&lineView, lineCommentExpression)) {
            continue;
        }
        if (found) {
            continue;
        }
        //by putting parenthesis after the line number the jet brains consol make the file name a clickable link
        std::cerr << lineView << std::endl;
        throw std::runtime_error("Lexing Error, Unknown data: "+fileName+" ("+std::to_string(lineNumber)+"):"+std::to_string(pos)+"");
    }
}

void lexFile(std::string &fileName, std::vector<LexedToken> &lexedTokens) {
    //chek if this file has already been parsed
    for (std::string &file : handledFiles) {
        if (file == fileName) {
            //this file has already been processed
            return;
        }
    }
    handledFiles.push_back(fileName);
    std::ifstream fileIn(fileName);
    if (!fileIn.is_open()) {
        std::cerr << "Failed to open file: " << fs::absolute(fileName) << std::endl;
        throw std::runtime_error("Failed to open file: "+fileName);
    }
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(fileIn, line)) {
        lineNumber++;
        //pre process line
        std::string lineTrimmed = trim(line);
        if (lineTrimmed.empty()) {
            continue;
        };
        if (lineTrimmed[0] == '@') {
            //annotation processing (import files)
            std::vector<std::string> split;
            size_t firstSpace = lineTrimmed.find_first_of(WHITESPACE);
            size_t nextSpace = firstSpace;
            //split this line into words
            if (firstSpace != std::string::npos) {
                do {
                    split.push_back(lineTrimmed.substr(0,nextSpace));
                    lineTrimmed = lineTrimmed.substr(nextSpace+1);
                    lineTrimmed = trim(lineTrimmed);
                } while ((nextSpace = lineTrimmed.find_first_of(WHITESPACE)) != std::string::npos);
            }
            if (!lineTrimmed.empty()) {
                split.push_back(lineTrimmed);//add the final token
            }
            if (split[0] == "@import") {
                if (split.size() < 2) {
                    throw std::runtime_error("Invalid import in: "+fileName+" ("+std::to_string(lineNumber)+"). Imports must include a file name!");
                }
                //attempt to parse this file
                std::string newFileName = split[1]+".blt";
                lexFile(newFileName,lexedTokens);
            } else {
                throw std::runtime_error("Invalid annotation in: "+fileName+" ("+std::to_string(lineNumber)+")");
            }
            continue;
        }
        if (lineTrimmed[0] == '/' && lineTrimmed[1] == '/') {
            //line comment, do nothing
            continue;
        }

        lexLine(line, lexedTokens, fileName, lineNumber);
    }
}

std::ostream & operator<<(std::ostream &os, const TokenType &token) {
    switch (token) {
        case INT_DATATYPE_T:
            os << "INT_DATATYPE_T";
            break;
        case FIXED_DATATYPE_T:
            os << "FIXED_DATATYPE_T";
            break;
        case POINTER_DATATYPE_T:
            os << "POINTER_DATATYPE_T";
            break;
        case VOID_DATATYPE_T:
            os << "VOID_DATATYPE_T";
            break;
        case FUNCTION_KEYWORD_T:
            os << "FUNCTION_KEYWORD_T";
            break;
        case INLINE_KEYWORD_T:
            os << "INLINE_KEYWORD_T";
            break;
        case IF_KEYWORD_T:
            os << "IF_KEYWORD_T";
            break;
        case ELSE_KEYWORD_T:
            os << "ELSE_KEYWORD_T";
            break;
        case RETURN_KEYWORD_T:
            os << "RETURN_KEYWORD_T";
            break;
        case WHILE_KEYWORD_T:
            os << "WHILE_KEYWORD_T";
            break;
        case FOR_KEYWORD_T:
            os << "FOR_KEYWORD_T";
            break;
        case CONTINUE_KEYWORD_T:
            os << "CONTINUE_KEYWORD_T";
            break;
        case BREAK_KEYWORD_T:
            os << "BREAK_KEYWORD_T";
            break;
        case LEFT_SHIFT_OPERATOR_T:
            os << "LEFT_SHIFT_OPERATOR_T";
            break;
        case RIGHT_SHIFT_OPERATOR_T:
            os << "RIGHT_SHIFT_OPERATOR_T";
            break;
        case POINTER_VALUE_OPERATOR_T:
            os << "POINTER_VALUE_OPERATOR_T";
            break;
        case LOGICAL_AND_OPERATOR_T:
            os << "LOGICAL_AND_OPERATOR_T";
            break;
        case LOGICAL_OR_OPERATOR_T:
            os << "LOGICAL_OR_OPERATOR_T";
            break;
        case ADD_TO_OPERATOR_T:
            os << "ADD_TO_OPERATOR_T";
            break;
        case SUBTRACT_FROM_OPERATOR_T:
            os << "SUBTRACT_FROM_OPERATOR_T";
            break;
        case MULTIPLY_WITH_OPERATOR_T:
            os << "MULTIPLY_WITH_OPERATOR_T";
            break;
        case DIVIDE_BY_OPERATOR_T:
            os << "DIVIDE_BY_OPERATOR_T";
            break;
        case MODULO_BY_OPERATOR_T:
            os << "MODULO_BY_OPERATOR_T";
            break;
        case ADD_OPERATOR_T:
            os << "ADD_OPERATOR_T";
            break;
        case SUBTRACT_OPERATOR_T:
            os << "SUBTRACT_OPERATOR_T";
            break;
        case MULTIPLY_OPERATOR_T:
            os << "MULTIPLY_OPERATOR_T";
            break;
        case DIVIDE_OPERATOR_T:
            os << "DIVIDE_OPERATOR_T";
            break;
        case MODULO_OPERATOR_T:
            os << "MODULO_OPERATOR_T";
            break;
        case NEGATE_OPERATOR_T:
            os << "NEGATE_OPERATOR_T";
            break;
        case BITWISE_AND_OPERATOR_T:
            os << "BITWISE_AND_OPERATOR_T";
            break;
        case BITWISE_OR_OPERATOR_T:
            os << "BITWISE_OR_OPERATOR_T";
            break;
        case BITWISE_XOR_OPERATOR_T:
            os << "BITWISE_XOR_OPERATOR_T";
            break;
        case GREATER_THAN_OR_EQUAL_OPERATOR_T:
            os << "GREATER_THAN_OR_EQUAL_OPERATOR_T";
            break;
        case LESS_THAN_OR_EQUAL_OPERATOR_T:
            os << "LESS_THAN_OR_EQUAL_OPERATOR_T";
            break;
        case EQUAL_OPERATOR_T:
            os << "EQUAL_OPERATOR_T";
            break;
        case NOT_EQUAL_OPERATOR_T:
            os << "NOT_EQUAL_OPERATOR_T";
            break;
        case GREATER_THAN_OPERATOR_T:
            os << "GREATER_THAN_OPERATOR_T";
            break;
        case LESS_THAN_OPERATOR_T:
            os << "LESS_THAN_OPERATOR_T";
            break;
        case LOGICAL_NOT_OPERATOR_T:
            os << "LOGICAL_NOT_OPERATOR_T";
            break;
        case ASSIGNMENT_OPERATOR_T:
            os << "ASSIGNMENT_OPERATOR_T";
            break;
        case OPEN_BRACE_T:
            os << "OPEN_BRACE_T";
            break;
        case CLOSE_BRACE_T:
            os << "CLOSE_BRACE_T";
            break;
        case OPEN_PAREN_T:
            os << "OPEN_PAREN_T";
            break;
        case CLOSE_PAREN_T:
            os << "CLOSE_PAREN_T";
            break;
        case OPEN_SQUARE_BRACKET_T:
            os << "OPEN_SQUARE_BRACKET_T";
            break;
        case CLOSE_SQUARE_BRACKET_T:
            os << "CLOSE_SQUARE_BRACKET_T";
            break;
        case COMMA_T:
            os << "COMMA_T";
            break;
        case SEMICOLON_T:
            os << "SEMICOLON_T";
            break;
        case NUMBER_T:
            os << "NUMBER_T";
            break;
        case FIXED_POINT_NUMBER_T:
            os << "FIXED_POINT_NUMBER_T";
            break;
        case IDENTIFIER_T:
            os << "IDENTIFIER_T";
            break;
        case STRING_LITERAL_T:
            os << "STRING_LITERAL_T";
            break;
    }
    return os;
}

std::ostream & operator<<(std::ostream &os, const LexedToken &token) {
    os << token.type;
    if (!token.value.empty()) {
        os << "(" << token.value << ")";
    }
    return os;
}
