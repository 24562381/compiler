#include <iostream>
#include <string>
#include <vector>
#include <cctype>

#include "token.h"

class Lexer {

public:

    explicit Lexer(std::string source)
        : src(std::move(source)), lcount(1) {}

    void tokenize(std::vector<Token>& tokens) {

        while (!is_eof()) {

            skip_whiteSpace_newLine();

            if (is_eof()) break;

            char c = peek();

            if (std::isalpha(c) || c == '_') {

                std::string word = read_word();

                if (kw.is_kw(word))
                    tokens.emplace_back(kw.kwlist[word], word);
                else
                    tokens.emplace_back(TokenType::KW_IDENTIFIER, word);
            }

            else if (std::isdigit(c)) {

                std::string digit = read_digit();

                if (error) break;

                tokens.emplace_back(TokenType::KW_INTLIT, digit);
            }

            
            else {

                std::string s(1, c);

                if (kw.is_kw(s)) {

                    tokens.emplace_back(kw.kwlist[s], s);
                    advance();
                }
                else {

                    std::cerr
                        << "Unknown character: " << c
                        << "\nIn line " << lcount << '\n';

                    break;
                }
            }
        }
    }

private:

    size_t lcount;
    std::string src;
    size_t cursor = 0;
    bool error = false;

    KW kw;

    bool is_eof() const {
        return cursor >= src.length();
    }

    char peek() const {
        return is_eof() ? '\0' : src[cursor];
    }

    char advance() {

        if (is_eof()) return '\0';

        return src[cursor++];
    }

    void skip_whiteSpace_newLine() {

        while (!is_eof() && std::isspace(peek())) {

            if (peek() == '\n')
                lcount++;

            advance();
        }
    }

    std::string read_word() {

        std::string word;

        while (
            !is_eof() &&
            (std::isalnum(peek()) || peek() == '_')
        ) {
            word.push_back(advance());
        }

        return word;
    }

    std::string read_digit() {

        std::string word;

        while (!is_eof() && std::isdigit(peek())) {
            word.push_back(advance());
        }

        
        
        if (!is_eof() && std::isalpha(peek())) {

            std::cerr
                << "Identifier can't start with a digit!"
                << "\nIn line " << lcount << '\n';

            error = true;
        }

        return word;
    }
};