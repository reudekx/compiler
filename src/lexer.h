#ifndef LEXER_H
#define LEXER_H

#include "tokenizer.h"
#include "scanner.h"

#include <vector>
#include <cstdio>
#include <memory>

#define BUFFER_SIZE 128
#define LEXEME_SIZE 128

class Lexer {

private:
    FILE* file;
    Tokenizer tokenizer;

    Token tokens[2];
    int cur_index = 1;
    int next_index = 0;
public:
    Lexer(FILE* file);

    Token advance();

    inline const Token& peek() const {
        return tokens[cur_index];
    }

    inline const Token& ahead() const {
        return tokens[next_index];
    }

    inline bool match(Token::Type type) const {
        return peek().type == type;
    }

    inline bool match_ahead(Token::Type type) const {
        return ahead().type == type;
    }

};

#endif