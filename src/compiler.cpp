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

    while (lexer.peek().type != Token::Type::ENDOFFILE && lexer.peek().type != Token::Type::ERROR) {
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

    std::cout << "========== Parse Start ==========" << std::endl;

    parser.parse();

    std::cout << "========== Parse End!! ==========" << std::endl;

    const AST::Node* ast = parser.ast();

    std::cout << parser.result_message() << std::endl;

    ast->data->print();

    
}

int main() {
    test_lexer();
    test_parser();

    return 0;
}   