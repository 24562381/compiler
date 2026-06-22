#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <stdexcept>

// Explicit internal types for calculations
enum class DataType { FLOAT, INT, UNKNOWN };

// Type-aware Semantic AST Node
struct SemanticNode {
    virtual ~SemanticNode() = default;
    virtual DataType getType() const = 0;
    virtual void print(int depth = 0) const = 0;
};

// Variable/Constant semantic container
struct SemanticLiteralNode : public SemanticNode {
    std::string value;
    DataType type;

    SemanticLiteralNode(std::string val, DataType t) : value(val), type(t) {}

    DataType getType() const override { return type; }
    void print(int depth) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << value << " (" << (type == DataType::FLOAT ? "float" : "int") << ")\n";
    }
};

// Verified operation node
struct SemanticOpNode : public SemanticNode {
    std::string op;
    DataType type;
    std::shared_ptr<SemanticNode> left;
    std::shared_ptr<SemanticNode> right;

    SemanticOpNode(std::string op, DataType t, std::shared_ptr<SemanticNode> l, std::shared_ptr<SemanticNode> r)
        : op(op), type(t), left(std::move(l)), right(std::move(r)) {}

    DataType getType() const override { return type; }
    void print(int depth) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << op << " [" << (type == DataType::FLOAT ? "float" : "int") << "]\n";
        if (left) left->print(depth + 1);
        if (right) right->print(depth + 1);
    }
};

// Injection node representing an internal implicit conversion 
struct TypeCastNode : public SemanticNode {
    std::shared_ptr<SemanticNode> child;
    DataType targetType;

    TypeCastNode(std::shared_ptr<SemanticNode> c, DataType t) : child(std::move(c)), targetType(t) {}

    DataType getType() const override { return targetType; }
    void print(int depth) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "int_to_float\n";
        if (child) child->print(depth + 1);
    }
};

// Engine that traverses the structural parser tree and evaluates semantics
class SemanticAnalyzer {
    std::unordered_map<std::string, DataType> symbolTable;

public:
    SemanticAnalyzer() {
        // Enforce specific simulated global floating-point types 
        symbolTable["x"] = DataType::FLOAT;
        symbolTable["y"] = DataType::FLOAT;
        symbolTable["z"] = DataType::FLOAT;
    }

    std::shared_ptr<SemanticNode> analyze(const ASTNode* node) {
        if (!node) return nullptr;

        // Determine if the node is a Leaf (LiteralNode)
        if (auto litNode = dynamic_cast<const LiteralNode*>(node)) {
            if (litNode->isNumber) {
                return std::make_shared<SemanticLiteralNode>(litNode->value, DataType::INT);
            }
            if (symbolTable.find(litNode->value) == symbolTable.end()) {
                throw std::runtime_error("Semantic Error: Variable '" + litNode->value + "' is undeclared!");
            }
            return std::make_shared<SemanticLiteralNode>(litNode->value, symbolTable[litNode->value]);
        }

        // Determine if the node is an Operation (OpNode)
        if (auto opNode = dynamic_cast<const OpNode*>(node)) {
            auto leftSemantic = analyze(opNode->left.get());
            auto rightSemantic = analyze(opNode->right.get());

            DataType leftType = leftSemantic->getType();
            DataType rightType = rightSemantic->getType();

            if (opNode->op == "=") {
                if (leftType != rightType) {
                    throw std::runtime_error("Semantic Error: Type mismatch in assignment conversion rules!");
                }
                return std::make_shared<SemanticOpNode>(opNode->op, leftType, leftSemantic, rightSemantic);
            }

            // Implicit numeric Promotion Rules (Int promoted to Float)
            if (leftType == DataType::FLOAT || rightType == DataType::FLOAT) {
                if (leftType == DataType::INT) {
                    leftSemantic = std::make_shared<TypeCastNode>(leftSemantic, DataType::FLOAT);
                }
                if (rightType == DataType::INT) {
                    rightSemantic = std::make_shared<TypeCastNode>(rightSemantic, DataType::FLOAT);
                }
                return std::make_shared<SemanticOpNode>(opNode->op, DataType::FLOAT, leftSemantic, rightSemantic);
            }

            return std::make_shared<SemanticOpNode>(opNode->op, DataType::INT, leftSemantic, rightSemantic);
        }

        throw std::runtime_error("Semantic Error: Unknown syntax node structure encountered.");
    }
};

#endif // SEMANTIC_H
