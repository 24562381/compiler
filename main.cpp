#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "lexer.h"
#include "parse.h"

int main() {
    
    std::ifstream file("test.txt", std::ios::in | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open source file 'test.txt'\n";
        return 1;
    }

    std::streamsize size = file.tellg();

    if (size < 0) {
        std::cerr << "Error: Failed to calculate file size\n";
        return 1;
    }

    
    file.seekg(0, std::ios::beg);

    
    std::string text(static_cast<size_t>(size), '\0');

    if (!file.read(&text[0], size)) {
        std::cerr << "Error: Failed to read file data buffer contents\n";
        return 1;
    }
    file.close(); 

    
    std::vector<Token> tokens;
    Lexer lexer(text);
    lexer.tokenize(tokens);

    std::cout << "=========================================\n";
    std::cout << "           GENERATED TOKENS              \n";
    std::cout << "=========================================\n";
    for (const Token& token : tokens) {
        token.print();
    }

    
    std::cout << "\n=========================================\n";
    std::cout << "         CONSTRUCTED AST LAYOUT          \n";
    std::cout << "=========================================\n";
    
    if (!tokens.empty()) {
        Parser parser(tokens);
        
        
        std::unique_ptr<ProgramNode> root_program = parser.parse_program();
        
        if (root_program) {
            
            root_program->print(0);
        }
    } else {
        std::cout << "[Warning]: Token collection is completely empty. No data to parse.\n";
    }

    return 0;
}
