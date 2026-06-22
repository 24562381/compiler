#ifndef PARSE_H
#define PARSE_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "token.h"

struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual void print(int depth = 0) const = 0;
};
//Expression node
struct ExprNode : public ASTNode{};

struct IdentifierNode : public ExprNode
{
    std::string name;
    explicit IdentifierNode(std::string n) : name(std::move(n)) {}
    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[Identifier: " << name << "]\n";
    }
};

struct IntLiteralNode : public ExprNode
{
    std::string value;
    explicit IntLiteralNode(std::string v) : value(std::move(v)) {}
    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[IntLiteral: " << value << "]\n";
    }
};

struct FunctionCallNode : public ExprNode
{
    std::string function_name;
    std::vector<std::unique_ptr<ExprNode>> arguments;
    explicit FunctionCallNode(std::string name) : function_name(std::move(name)) {}
    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[FunctionCall: " << function_name << "]\n";
        for (const auto &arg : arguments)
            arg->print(depth + 1);
    }
};

// New: AST Node to represent binary math expressions (e.g., a + b)
struct BinaryExprNode : public ExprNode
{
    std::string op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;

    BinaryExprNode(std::string o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}

    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[BinaryExpression: " << op << "]\n";
        if (left)
            left->print(depth + 1);
        if (right)
            right->print(depth + 1);
    }
};

struct ReturnStmtNode : public ASTNode
{
    std::unique_ptr<ExprNode> expression;
    explicit ReturnStmtNode(std::unique_ptr<ExprNode> expr) : expression(std::move(expr)) {}
    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[ReturnStatement]\n";
        if (expression)
            expression->print(depth + 1);
    }
};

struct VarDeclNode : public ASTNode
{
    std::string type;
    std::string name;
    std::unique_ptr<ExprNode> init_expr;

    VarDeclNode(std::string t, std::string n, std::unique_ptr<ExprNode> expr)
        : type(std::move(t)), name(std::move(n)), init_expr(std::move(expr)) {}

    void print(int depth) const override
    {
        std::cout << std::string(depth * 2, ' ') << "[VariableDeclaration: " << type << " " << name << "]\n";
        if (init_expr)
            init_expr->print(depth + 1);
    }
};

struct BadStatementNode : public ASTNode
{
    std::string content;
    explicit BadStatementNode(std::string c) : content(std::move(c)) {}
    void print(int depth) const override
    {
        std::cerr << std::string(depth * 2, ' ') << "[⚠️ Unknown/Invalid Statement: \"" << content << "\"]\n";
    }
};

struct FunctionDefNode : public ASTNode
{
    std::string return_type;
    std::string function_name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::vector<std::unique_ptr<ASTNode>> body_statements;

    FunctionDefNode(std::string rtype, std::string name)
        : return_type(std::move(rtype)), function_name(std::move(name)) {}

    void print(int depth) const override
    {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "[FunctionDefinition: " << return_type << " " << function_name << "()]\n";
        std::cout << indent << "  [Parameters]: ";
        for (const auto &param : parameters)
            std::cout << param.first << " " << param.second << " ";
        std::cout << "\n"
                  << indent << "  [Body]:\n";
        for (const auto &stmt : body_statements)
            stmt->print(depth + 2);
    }
};

struct ProgramNode : public ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    void print(int depth) const override
    {
        for (const auto &stmt : statements)
            stmt->print(depth);
    }
};

class Parser
{
public:
    explicit Parser(std::vector<Token> t) : tokens(std::move(t)), index(0) {}

    std::unique_ptr<ProgramNode> parse_program()
    {
        auto program = std::make_unique<ProgramNode>();
        while (!is_eof())
        {
            program->statements.push_back(parse_statement());
        }
        return program;
    }

private:
    std::vector<Token> tokens;
    size_t index;

    bool is_eof() const { return index >= tokens.size(); }

    Token peek(size_t offset = 0) const
    {
        if (index + offset >= tokens.size())
            return Token{TokenType::KW_SEMICOLON, ""};
        return tokens[index + offset];
    }

    Token advance()
    {
        if (!is_eof())
            return tokens[index++];
        return Token{TokenType::KW_SEMICOLON, ""};
    }

    bool match(TokenType type) { return peek().tokenType == type; }

    Token consume(TokenType type, const std::string &err_msg)
    {
        if (match(type))
            return advance();
        std::cerr << "Parser Syntax Error: " << err_msg << " (Got: '" << peek().value << "')\n";
        exit(1);
    }

    std::unique_ptr<ASTNode> parse_statement()
    {
        if (match(TokenType::KW_INT))
        {
            if (peek(2).tokenType == TokenType::KW_LEFT_PAREN)
            {
                return parse_function_definition();
            }
            return parse_variable_declaration();
        }

        if (match(TokenType::KW_RETURN))
        {
            return parse_return();
        }

        if (match(TokenType::KW_IDENTIFIER) && peek(1).tokenType == TokenType::KW_LEFT_PAREN)
        {
            auto call = parse_function_call();
            if (match(TokenType::KW_SEMICOLON))
                advance();
            return call;
        }

        std::string broken_text = advance().value;
        while (!is_eof() && !match(TokenType::KW_SEMICOLON) && !match(TokenType::KW_RIGHT_BRACE))
        {
            broken_text += " " + advance().value;
        }
        if (match(TokenType::KW_SEMICOLON))
        {
            broken_text += ";";
            advance();
        }
        return std::make_unique<BadStatementNode>(broken_text);
    }

    // Upgraded: Parses a primary value, then loops if mathematical binary operators follow it
    std::unique_ptr<ExprNode> parse_expression()
    {
        auto left_node = parse_primary();

        // Check if an operator token (+, -, *, /) comes up next
        while (match(TokenType::KW_PLUS) || match(TokenType::KW_MINUS) ||
               match(TokenType::KW_MULTIPLY) || match(TokenType::KW_DIVISION))
        {

            Token op_token = advance();        // Extends past the operator token
            auto right_node = parse_primary(); // Extracts the right-hand value operand

            // Build the binary expression sub-tree layer
            left_node = std::make_unique<BinaryExprNode>(op_token.value, std::move(left_node), std::move(right_node));
        }

        return left_node;
    }

    // Helper to extract a baseline foundational piece (Variable, Call, or Number literal)
    std::unique_ptr<ExprNode> parse_primary()
    {
        if (match(TokenType::KW_IDENTIFIER) && peek(1).tokenType == TokenType::KW_LEFT_PAREN)
        {
            return parse_function_call();
        }
        if (match(TokenType::KW_IDENTIFIER))
            return std::make_unique<IdentifierNode>(advance().value);
        if (match(TokenType::KW_INTLIT))
            return std::make_unique<IntLiteralNode>(advance().value);

        std::cerr << "Parser Error: Expected expression variable or raw value, got '" << peek().value << "'\n";
        exit(1);
    }

    std::unique_ptr<FunctionCallNode> parse_function_call()
    {
        Token id_token = consume(TokenType::KW_IDENTIFIER, "Expected function call identifier");
        auto call_node = std::make_unique<FunctionCallNode>(id_token.value);

        consume(TokenType::KW_LEFT_PAREN, "Expected '('");
        if (!match(TokenType::KW_RIGHT_PAREN))
        {
            while (true)
            {
                call_node->arguments.push_back(parse_expression());
                if (match(TokenType::KW_COMMA))
                    advance(); // consume ',' and parse next arg
                else
                    break;
            }
        }
        consume(TokenType::KW_RIGHT_PAREN, "Expected ')'");
        return call_node;
    }

    std::unique_ptr<VarDeclNode> parse_variable_declaration()
    {
        Token type_tok = consume(TokenType::KW_INT, "Expected variable type specifier");
        Token name_tok = consume(TokenType::KW_IDENTIFIER, "Expected variable name identifier");

        std::unique_ptr<ExprNode> init_expr = nullptr;
        if (match(TokenType::KW_EQUAL))
        {
            advance();
            init_expr = parse_expression();
        }

        consume(TokenType::KW_SEMICOLON, "Expected ';' ending variable declaration");
        return std::make_unique<VarDeclNode>(type_tok.value, name_tok.value, std::move(init_expr));
    }

    std::unique_ptr<ReturnStmtNode> parse_return()
    {
        consume(TokenType::KW_RETURN, "Expected 'return' keyword");
        std::unique_ptr<ExprNode> expr = nullptr;
        if (!match(TokenType::KW_SEMICOLON))
        {
            expr = parse_expression();
        }
        consume(TokenType::KW_SEMICOLON, "Expected ';' after return statement");
        return std::make_unique<ReturnStmtNode>(std::move(expr));
    }

    std::unique_ptr<FunctionDefNode> parse_function_definition()
    {
        Token type_tok = consume(TokenType::KW_INT, "Expected type specifier");
        Token name_tok = consume(TokenType::KW_IDENTIFIER, "Expected definition identifier");
        auto func_def = std::make_unique<FunctionDefNode>(type_tok.value, name_tok.value);

        consume(TokenType::KW_LEFT_PAREN, "Expected '('");
        if (!match(TokenType::KW_RIGHT_PAREN))
        {
            while (true)
            {
                Token p_type = consume(TokenType::KW_INT, "Expected parameter type");
                Token p_name = consume(TokenType::KW_IDENTIFIER, "Expected parameter name");
                func_def->parameters.push_back({p_type.value, p_name.value});
                if (match(TokenType::KW_COMMA))
                    advance(); // consume ',' and parse next param
                else
                    break;
            }
        }
        consume(TokenType::KW_RIGHT_PAREN, "Expected ')'");
        consume(TokenType::KW_LEFT_BRACE, "Expected opening brace '{'");
        if(match(TokenType::KW_RIGHT_BRACE)){
            std::cerr << "You must return a value from this function!" << " (Got: '" << peek().value << "')\n";
            exit(1);
        }
        while (!is_eof() && !match(TokenType::KW_RIGHT_BRACE))
        {
            func_def->body_statements.push_back(parse_statement());
        }
        consume(TokenType::KW_RIGHT_BRACE, "Expected closing brace '}'");
        return func_def;
    }
};

#endif