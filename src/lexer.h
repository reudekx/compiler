#ifndef LEXER_H
#define LEXER_H

#include "tokenizer.h"
#include "scanner.h"

#include <vector>
#include <cstdio>

#define BUFFER_SIZE 128
#define LEXEME_SIZE 128

class Lexer {

private:
    FILE* file;
    Tokenizer tokenizer;

    Token tokens[2];
    int cur_token_index = 0;
public:
    Lexer(FILE* file) : file(file), tokenizer(file) {

    }

    inline Token get_token() {
        int index = cur_token_index;
        cur_token_index = (cur_token_index + 1) % 2;
        tokens[cur_token_index] = tokenizer.tokenize();
        return tokens[index]
    }

    inline Token peek_token() {
        return cur_token;
    }

};

#endif