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

public:
    Lexer(FILE* file) : file(file), tokenizer(file) {

    }

};

#endif