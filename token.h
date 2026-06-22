#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>
#include <unordered_map>

enum class TokenType {

    // Variables / Types
    KW_INT,
    KW_STRING,

    // General
    KW_IDENTIFIER,
    KW_RETURN,
    KW_INTLIT,

    // Punctuators
    KW_SEMICOLON,
    KW_COMMA,
    KW_LEFT_PAREN,
    KW_RIGHT_PAREN,
    KW_LEFT_BRACE,
    KW_RIGHT_BRACE,
    KW_SINGLE_QUOTATION,
    KW_DOUBLE_QUOTATION,

    // Operators
    KW_PLUS,
    KW_MINUS,
    KW_MULTIPLY,
    KW_DIVISION,
    KW_EQUAL
};

struct Token {

    TokenType tokenType;
    std::string value;

    Token(TokenType t, const std::string& v)
        : tokenType(t), value(v) {}

    void print() const {
        std::cout << "<" << value << ", "
                  << static_cast<int>(tokenType)
                  << ">\n";
    }
};

struct KW {

    std::unordered_map<std::string, TokenType> kwlist {

        // Data Types
        {"int", TokenType::KW_INT},
        {"string", TokenType::KW_STRING},

        // General
        {"return", TokenType::KW_RETURN},

        // Punctuators
        {";", TokenType::KW_SEMICOLON},
        {",", TokenType::KW_COMMA},
        {"(", TokenType::KW_LEFT_PAREN},
        {")", TokenType::KW_RIGHT_PAREN},
        {"{", TokenType::KW_LEFT_BRACE},
        {"}", TokenType::KW_RIGHT_BRACE},
        {"'", TokenType::KW_SINGLE_QUOTATION},
        {"\"", TokenType::KW_DOUBLE_QUOTATION},

        // Operators
        {"+", TokenType::KW_PLUS},
        {"-", TokenType::KW_MINUS},
        {"*", TokenType::KW_MULTIPLY},
        {"/", TokenType::KW_DIVISION},
        {"=", TokenType::KW_EQUAL}
    };

    bool is_kw(const std::string& word) const {
        return kwlist.find(word) != kwlist.end();
    }

    TokenType get_kw(const std::string& word) const {
        return kwlist.at(word);
    }
};
#endif