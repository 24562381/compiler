#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

// Base structural Node type for our structural tree representation
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int depth = 0) const = 0;
};

// Structural node for Binary operations or variable mutations
struct OpNode : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    OpNode(std::string op, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(op), left(std::move(l)), right(std::move(r)) {}

    void print(int depth) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << op << "\n";
        if (left) left->print(depth + 1);
        if (right) right->print(depth + 1);
    }
};

// Structural node for text values and literal identifiers
struct LiteralNode : public ASTNode {
    std::string value;
    bool isNumber;

    LiteralNode(std::string val, bool num) : value(val), isNumber(num) {}

    void print(int depth) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << value << "\n";
    }
};

// Parser object to cycle sequentially through our validated tokens
class Parser {
    std::vector<Token> tokens;
    size_t index = 0;

    Token peek() const {
        if (index < tokens.size()) return tokens[index];
        return {TokenType::UNKNOWN, ""};
    }

    Token consume(TokenType expected) {
        if (peek().type == expected) {
            return tokens[index++];
        }
        throw std::runtime_error("Parser Error: Unexpected token type encountered.");
    }

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    // Base terms extraction
    std::unique_ptr<ASTNode> parsePrimary() {
        Token t = peek();
        if (t.type == TokenType::IDENTIFIER) {
            index++;
            return std::make_unique<LiteralNode>(t.value, false);
        } else if (t.type == TokenType::NUMBER) {
            index++;
            return std::make_unique<LiteralNode>(t.value, true);
        }
        throw std::runtime_error("Parser Error: Expected an identifier or structural number value.");
    }

    // High precedence operator handler (*)
    std::unique_ptr<ASTNode> parseMultiplicative() {
        auto left = parsePrimary();
        while (peek().type == TokenType::MULTIPLY) {
            Token op = consume(TokenType::MULTIPLY);
            auto right = parsePrimary();
            left = std::make_unique<OpNode>(op.value, std::move(left), std::move(right));
        }
        return left;
    }

    // Low precedence operator handler (+)
    std::unique_ptr<ASTNode> parseAdditive() {
        auto left = parseMultiplicative();
        while (peek().type == TokenType::PLUS) {
            Token op = consume(TokenType::PLUS);
            auto right = parseMultiplicative();
            left = std::make_unique<OpNode>(op.value, std::move(left), std::move(right));
        }
        return left;
    }

    // Full statement layout verification (=)
    std::unique_ptr<ASTNode> parseAssignment() {
        auto left = parsePrimary();
        if (peek().type == TokenType::ASSIGN) {
            Token op = consume(TokenType::ASSIGN);
            auto right = parseAdditive();
            return std::make_unique<OpNode>(op.value, std::move(left), std::move(right));
        }
        return left;
    }
};

#endif // PARSER_H
