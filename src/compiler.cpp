#include "token.h"
#include "lexer.h"
#include "tokenizer.h"
#include "parser.h"

#include <iostream>
#include <cstdio>

void test_lexer() {
    const char *file_name = "../data/code.txt";

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        perror("Cannot open file");
    }

    int i = 0;

    Lexer lexer(file);

    while (!lexer.match(Token::Type::ENDOFFILE) && !lexer.match(Token::Type::ERROR)) {
        std::cout << ++i << ") ";
        lexer.peek().print();
        lexer.advance();
    }
    
    lexer.peek().print();

    fclose(file);
}

void test_parser() {
    const char *file_name = "../data/code.txt";

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        perror("Cannot open file");
    }

    Lexer lexer(file);

    Parser parser(&lexer);

    parser.parse();
}

int main() {
    test_parser();

    return 0;
}   