#include <iostream>
#include "lexer.hpp"
#include <vector>

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() < 2) {
        std::cerr << "Usage: " << args[0] << " <source file>" << std::endl;
        return EXIT_FAILURE;
    }
    //TODO processing compile flags and things

    std::vector<LexedToken> lexedTokens;
    try {
        lexFile(args[1], lexedTokens);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Generated " << lexedTokens.size() << " tokens" << std::endl;
    return EXIT_SUCCESS;
}
