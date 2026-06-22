#ifndef CODEGEN_H
#define CODEGEN_H

#include "semantic.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Structure to capture a standardized 3-address instruction format
struct TACInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    void print() const {
        if (op == "=") {
            std::cout << result << " = " << arg1 << "\n";
        } else if (op == "int_to_float") {
            std::cout << result << " = int_to_float(" << arg1 << ")\n";
        } else {
            std::cout << result << " = " << arg1 << " " << op << " " << arg2 << "\n";
        }
    }
};

class IRGenerator {
    std::vector<TACInstruction> instructions;
    int tempCount = 1;

    // Creates unique, temporary register strings
    std::string newTemp() {
        return "t" + std::to_string(tempCount++);
    }

public:
    // Recursively processes nodes to unwind expressions into TAC format
        // Recursively processes nodes to unwind expressions into TAC format
    std::string generate(const std::shared_ptr<SemanticNode>& node) {
        if (!node) return "";

        // Base elements simply pass up their raw structural value (Removed std::)
        if (auto litNode = dynamic_cast<const SemanticLiteralNode*>(node.get())) {
            return litNode->value;
        }

        // Implicit type conversion handling (Removed std::)
        if (auto castNode = dynamic_cast<const TypeCastNode*>(node.get())) {
            std::string childVal = generate(castNode->child);
            std::string temp = newTemp();
            instructions.push_back({"int_to_float", childVal, "", temp});
            return temp;
        }

        // Operational statement handling (Removed std::)
        if (auto opNode = dynamic_cast<const SemanticOpNode*>(node.get())) {
            if (opNode->op == "=") {
                std::string rightVal = generate(opNode->right);
                std::string leftVal = generate(opNode->left);
                instructions.push_back({"=", rightVal, "", leftVal});
                return leftVal;
            } else {
                std::string leftVal = generate(opNode->left);
                std::string rightVal = generate(opNode->right);
                std::string temp = newTemp();
                instructions.push_back({opNode->op, leftVal, rightVal, temp});
                return temp;
            }
        }

        return "";
    }


    void printTAC() const {
        for (const auto& instr : instructions) {
            instr.print();
        }
    }
};

#endif // CODEGEN_H
