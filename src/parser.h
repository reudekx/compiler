#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <variant>

#include "lexer.h"
#include "ast.h"

class Parser {
private:
    Lexer *lexer;
    std::unique_ptr<AST::Node> ast;
    std::string result_message;

    AST::NODE parse_file();
    AST::NODE parse_import_stmt();
    AST::NODE parse_global();

    AST::TOKEN take_token();

    void expect(Token::Type type, const char * const message = "Unexpected Token");

public:
    Parser(Lexer *lexer);

    void parse();



};

#endif