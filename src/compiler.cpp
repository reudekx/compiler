#include "token.h"
#include "lexer.h"
#include "tokenizer.h"

#include <iostream>
#include <cstdio>

void compile() {
    const char *file_name = "../data/code.txt";

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        perror("Cannot open file");
    }

    int i = 0;

    Lexer lexer(file);

    while (lexer.peek().type != Token::Type::ENDOFFILE) {
        std::cout << ++i << ") ";
        lexer.peek().print();
        lexer.advance();
    }

    fclose(file);
}

int main() {
    compile();

    return 0;
}   