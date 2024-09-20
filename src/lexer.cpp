#include "lexer.h"

Lexer::Lexer(FILE* file) : file(file), tokenizer(file) {
    advance();
    advance();
}

Token Lexer::advance() {
    cur_index = next_index;
    next_index = (next_index + 1) % 2;
    tokens[next_index] = tokenizer.tokenize();
    return tokens[cur_index];
}