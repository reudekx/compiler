#include "token.h"
#include "lexer.h"
#include "tokenizer.h"

#include <iostream>
#include <cstdio>

int main() {
    const char *file_name = "../data/code.txt";

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        perror("Cannot open file");
    }

    // Scanner scanner = Scanner(file);

    // while (scanner.peek() != EOF) {
    //     printf("Line: %d, Column: %d, Char: %c\n", scanner.line, scanner.column, scanner.peek());
    //     scanner.consume();
    // }

    Tokenizer tokenizer(file);

    Token token = tokenizer.tokenize();

    while (token.type != Token::Type::ENDOFFILE) {
        token.print();
        token = tokenizer.tokenize();
    }

    return 0;
}   