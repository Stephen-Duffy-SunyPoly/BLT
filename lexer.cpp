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
    //note: surround words with \\b to force matches on word boundries
    {"\\bint\\b",INT_DATATYPE_T,false},
    {"\\bfixed\\b",FIXED_DATATYPE_T,false},
    {"\\bptr\\b",POINTER_DATATYPE_T,false},
    {"\\bvoid\\b",VOID_DATATYPE_T,false},

};

static RE2 whitespaceExpression("\\s+");
static RE2 lineCommentExpression("//.*");
static RE2 trimStringExpression("^\\s+|\\s+$");

std::string trim(std::string untrimmed) {
    RE2::GlobalReplace(&untrimmed, trimStringExpression,"");
    return untrimmed;
}

void lexLine(std::string &line, std::vector<LexedToken> &lexedTokens, std::string &fileName, size_t lineNumber) {
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
