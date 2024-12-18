#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

#include "scanner.h"
#include "token.h"

#define LEXEME_SIZE 128

class Tokenizer {
private:
    struct Lexeme {
        char data[LEXEME_SIZE];
        int length = 0;
    };

    Lexeme lexeme;
    Scanner scanner;
    Token cur_token = Token();

    void init_lexeme();
    void build_lexeme();
    std::string get_literal();

    void consume_space();
    void consume_comment();

    void tokenize_number();
    void tokenize_string();
    void tokenize_keyword();
    void tokenize_eof();
    void tokenize_symbol();

public:
    Tokenizer(FILE* file) : scanner(file) {}

    const Token& tokenize();

};

#endif