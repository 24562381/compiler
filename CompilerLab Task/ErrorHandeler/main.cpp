#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

int main() {
    // 0. Define input string
    std::string sourceCode = "x = y + z * 10";
    std::cout << "=== SOURCE CODE ===\n" << sourceCode << "\n\n";

    try {
        // 1. Run Lexical Analysis (Tokenization)
        std::vector<Token> tokens = tokenize(sourceCode);
        std::cout << "=== 1. TOKENS GENERATED ===\n";
        for (const auto& t : tokens) {
            std::cout << "<" << t.typeToString() << ", \"" << t.value << "\">\n";
        }
        std::cout << "\n";

        // 2. Run Syntax Analysis (Parsing)
        Parser parser(tokens);
        std::unique_ptr<ASTNode> rawAST = parser.parseAssignment();
        std::cout << "=== 2. ABSTRACT SYNTAX TREE ===\n";
        rawAST->print();
        std::cout << "\n";

        // 3. Run Semantic Analysis (Type Checking & Promotion)
        SemanticAnalyzer semanticAnalyzer;
        std::shared_ptr<SemanticNode> semanticAST = semanticAnalyzer.analyze(rawAST.get());
        std::cout << "=== 3. TYPE-DECORATED SEMANTIC TREE ===\n";
        semanticAST->print();
        std::cout << "\n";

        // 4. Run Intermediate Code Generation (Three-Address Code)
        IRGenerator irGenerator;
        irGenerator.generate(semanticAST);
        std::cout << "=== 4. THREE-ADDRESS INTERMEDIATE CODE ===\n";
        irGenerator.printTAC();

    } catch (const std::exception& e) {
        std::cerr << "Compilation Failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
