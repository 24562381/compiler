#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
// #include <cctype>

// Enum representing types of valid tokens
enum class TokenType {
    IDENTIFIER,
    NUMBER,
    ASSIGN,
    PLUS,
    MULTIPLY,
    UNKNOWN
};

// Structural blueprint for single token objects
struct Token {
    TokenType type;
    std::string value;

    std::string typeToString() const {
        switch (type) {
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::NUMBER:     return "NUMBER";
            case TokenType::ASSIGN:     return "ASSIGN_OP";
            case TokenType::PLUS:       return "ADD_OP";
            case TokenType::MULTIPLY:   return "MULT_OP";
            default:                    return "UNKNOWN";
        }
    }
};

// Iterates through text to group sequential characters into tokens
inline std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < source.length()) {
        // Skip spacing characters
        if (std::isspace(source[i])) {
            i++;
            continue;
        }

        // Construct multi-character identifiers
        if (std::isalpha(source[i])) {
            std::string id = "";
            while (i < source.length() && std::isalnum(source[i])) {
                id += source[i];
                i++;
            }
            tokens.push_back({TokenType::IDENTIFIER, id});
            continue;
        }

        // Construct multi-character base-10 numerical values
        if (std::isdigit(source[i])) {
            std::string num = "";
            while (i < source.length() && std::isdigit(source[i])) {
                num += source[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER, num});
            continue;
        }

        // Single character punctuation mapping
        switch (source[i]) {
            case '=': tokens.push_back({TokenType::ASSIGN, "="}); break;
            case '+': tokens.push_back({TokenType::PLUS, "+"}); break;
            case '*': tokens.push_back({TokenType::MULTIPLY, "*"}); break;
            default:  tokens.push_back({TokenType::UNKNOWN, std::string(1, source[i])}); break;
        }
        i++;
    }
    return tokens;
}

#endif // LEXER_H
